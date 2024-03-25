#include <string.h>
#include <sys/types.h>
#if defined INTEGERTIME || defined CLOCKTIME
# include <time.h>
#else
# include <sys/timeb.h>
#endif
#include "lpkit.h"
#include "debug.h"

#define antismallpivot
/* #undef antismallpivot */

/* #define NEWBB */
#undef NEWBB

/* #define peterprintiter */

#define antidegeneracy

static short milpsolve(lprec *lp, REAL *upbo, REAL *lowbo, MYBOOL *sbasis, MYBOOL *slower, int *sbas, int recursive);

/* time() is not an ANSI function, so some compilers don't provide it.
   If that is the case, first try to define the constant CLOCKTIME.
   The compiler then uses the clock() function, which is ANSI complient.
   If that doesn't work, define INTEGERTIME which results in using the
   time() function, but this is only precise on the second, so use it as
   last option */

static double timenow()
{
#ifdef INTEGERTIME
  return((double)time(NULL));
#elif defined CLOCKTIME
  return((double)clock()/CLOCKS_PER_SEC /* CLK_TCK */);
#else
  struct timeb buf;

  ftime(&buf);
  return((double)buf.time+((double) buf.millitm)/1000.0);
#endif
}

static int yieldformessages(lprec *lp)
{
  if(lp->sectimeout>0) {
    double currenttime = timenow();

    if((currenttime-lp->timestart)-(REAL)lp->sectimeout>0)
      lp->spx_status = TIMEOUT;
  }

  if (lp->abort != NULL)
    return(lp->abort(lp, lp->aborthandle));
  else
    return(0);
}

#if USED
/* Get data column stored in a particular eta column */
static int eta_column(lprec *lp, int column)
{
  if(column > lp->eta_size) {
    column = lp->eta_col_end[column] - 1;
    return(lp->eta_row_nr[column]);
  }
  else
    return(0);
}
#endif

static int ftran(lprec *lp, REAL *pcol, REAL roundzero)
{
  int  i, j, k, r, *rowp, ok = TRUE;
  LREAL theta, *vcol;
  REAL  *valuep;

  if (MALLOC(vcol, lp->rows + 1, LREAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    ok = FALSE;
  }
  else {
    for(i = 0; i <= lp->rows; i++)
      vcol[i] = pcol[i];

    for(i = 1; i <= lp->eta_size; i++) {
      k = lp->eta_col_end[i] - 1;
      r = lp->eta_row_nr[k];
      theta = vcol[r];
      if(theta != 0) {
	j = lp->eta_col_end[i - 1];

	/* CPU intensive loop, let's do pointer arithmetic */
	for(rowp = lp->eta_row_nr + j, valuep = lp->eta_value + j;
	    j < k;
	    j++, rowp++, valuep++)
	  vcol[*rowp] += theta * *valuep;

	vcol[r] *= lp->eta_value[k];
      }
    }

    for(i = 0; i <= lp->rows; i++)
      pcol[i] = (REAL) vcol[i];

    free(vcol);

    /* round small values to zero */
    if(roundzero != 0)
      for(i = 0; i <= lp->rows; i++)
	my_round(pcol[i], roundzero);
  }
  return(ok);
} /* ftran */


void btran(lprec *lp, REAL *row, REAL roundzero)
{
  int   i, j, k, *rowp;
  LREAL f;
  REAL  *valuep;

  for(i = lp->eta_size; i >= 1; i--) {
    f = 0;
    k = lp->eta_col_end[i] - 1;
    j = lp->eta_col_end[i - 1];

    for(rowp = lp->eta_row_nr + j, valuep = lp->eta_value + j;
	j <= k;
	j++, rowp++, valuep++)
      f += row[*rowp] * *valuep;

    if(roundzero != 0)
      my_round(f, roundzero);
    row[lp->eta_row_nr[k]] = (REAL) f;
  }
} /* btran */

int build_row_end(lprec *lp)
{
  int i, j, *rownum = NULL;
  int *num = NULL, row_nr;

  if(!lp->row_end_valid) {
    if ((CALLOC(num, lp->rows + 1, int) == NULL) ||
        (CALLOC(rownum, lp->rows + 1, int) == NULL)
       ) {
      FREE(num);
      FREE(rownum);
      return(FALSE);
    }
    for(i = 0; i < lp->non_zeros; i++)
      rownum[lp->mat[i].row_nr]++;

    lp->row_end[0] = 0;

    for(i = 1; i <= lp->rows; i++)
      lp->row_end[i] = lp->row_end[i - 1] + rownum[i];

    for(i = 1; i <= lp->columns_used; i++)
      for(j = lp->col_end[i - 1]; j < lp->col_end[i]; j++) {
	row_nr = lp->mat[j].row_nr;
	if(row_nr != 0) {
	  num[row_nr]++;
	  lp->col_no[lp->row_end[row_nr - 1] + num[row_nr]] = i;
	}
      }

    FREE(num);
    FREE(rownum);
    lp->row_end_valid = TRUE;
  }
  return(TRUE);
}

MYBOOL isvalid(lprec *lp)
{
  int i, j, *rownum = NULL, *colnum;

  /* Check consistency of bounds */
  for(i = 1; i <= lp->columns; i++)
    if(lp->orig_upbo[lp->rows + i] < lp->orig_lowbo[lp->rows + i]){
      report(lp, IMPORTANT, "Error: Column %d (%s) has inconsistent bounds (lowbo > upbo).", i, get_col_name(lp, i));
      return(FALSE);
    }

  /* Check consistency of ranges */
  for(i = 1; i <= lp->rows; i++)
    if(lp->orig_upbo[i] < lp->orig_lowbo[i]){
      report(lp, IMPORTANT, "Error: Row %d (%s) has inconsistent range (lowbo > upbo).", i, get_row_name(lp, i));
      return(FALSE);
    }

  if (!build_row_end(lp))
    return(FALSE);

  if(lp->valid)
    return(TRUE);

  rownum = colnum = NULL;
  if ((CALLOC(rownum, lp->rows + 1, int) == NULL) ||
      (CALLOC(colnum, lp->columns + 1, int) == NULL)
      ) {
    FREE(rownum);
    FREE(colnum);
    return(FALSE);
  }

  for(i = 1 ; i <= lp->columns_used; i++)
    for(j = lp->col_end[i - 1]; j < lp->col_end[i]; j++) {
      colnum[i]++;
      rownum[lp->mat[j].row_nr]++;
    }

  for(i = 1; i <= lp->columns; i++)
    if(colnum[i] == 0) {
      report(lp, NORMAL, "Warning: Variable %s not used in any constraints",
                get_col_name(lp, i));
    }
  free(rownum);
  free(colnum);
  lp->valid = TRUE;
  return(TRUE);
}

static int resize_eta(lprec *lp, int min_size)
{
  while(lp->eta_alloc <= min_size)
    lp->eta_alloc += lp->eta_alloc / RESIZEFACTOR;
  /* report(lp, FULL, "resizing eta to size %d", lp->eta_alloc); */
  if ((REALLOC(lp->eta_value, lp->eta_alloc + 1, REAL) == NULL) ||
      (REALLOC(lp->eta_row_nr, lp->eta_alloc + 1, int) == NULL)) {
    lp->spx_status = OUT_OF_MEMORY;
    return(FALSE);
  }
  else
    return(TRUE);
} /* resize_eta */

static int condensecol(lprec *lp,
			int row_nr,
			REAL *pcol)
{
  int i, elnr, min_size;

  elnr = lp->eta_col_end[lp->eta_size];

  min_size = elnr + lp->rows + 2;
  if(min_size >= lp->eta_alloc) /* maximum local growth of Eta */
    if (!resize_eta(lp, min_size))
      return(FALSE);

  for(i = 0; i <= lp->rows; i++)
    if(i != row_nr && pcol[i] != 0) {
      lp->eta_row_nr[elnr] = i;
      lp->eta_value[elnr] = pcol[i];
      elnr++;
    }

  lp->eta_row_nr[elnr] = row_nr;
  lp->eta_value[elnr] = pcol[row_nr];
  elnr++;
  lp->eta_col_end[lp->eta_size + 1] = elnr;
  return(TRUE);
} /* condensecol */


static void addetacol(lprec *lp /* , int colnr */)
{
  int  i, j, k;
  LREAL theta;

  j = lp->eta_col_end[lp->eta_size];
  lp->eta_size++;

  k = lp->eta_col_end[lp->eta_size] - 1;
  theta = 1 / lp->eta_value[k];
  lp->eta_value[k] = (REAL) theta;
  for(i = j; i < k; i++)
    lp->eta_value[i] = (REAL) (-theta * lp->eta_value[i]);
  lp->justInverted = FALSE;
} /* addetacol */

static int setpivcol(lprec *lp, int varin, REAL *pcol)
{
  int  i, j, colnr, ok = TRUE;

  for(i = 0; i <= lp->rows; i++)
    pcol[i] = 0;

  if(lp->lower[varin]) {
    if(varin > lp->rows) {
      colnr = varin - lp->rows;
      for(i = lp->col_end[colnr - 1]; i < lp->col_end[colnr]; i++)
	pcol[lp->mat[i].row_nr] = lp->mat[i].value;
      pcol[0] -= lp->Extrad;
    }
    else
      pcol[varin] = 1;
  }
  else { /* !lower */
    if(varin > lp->rows) {
      colnr = varin - lp->rows;
      for(i = lp->col_end[colnr - 1]; i < lp->col_end[colnr]; i++)
	pcol[lp->mat[i].row_nr] = -lp->mat[i].value;
      pcol[0] += lp->Extrad;
    }
    else
      pcol[varin] = -1;
  }

 /* Test if we should do the error-correction version */
  if((lp->improve & IMPROVE_FTRAN) && lp->num_inv) {
    REAL   *errors = NULL, sdp;
    matrec *matentry;
    int    k, ie;

    if ((MALLOCCPY(errors, pcol, lp->rows + 1, REAL) == NULL) || (!ftran(lp, pcol, lp->epsel))) {
      FREE(errors);
      lp->spx_status = OUT_OF_MEMORY;
      ok = FALSE;
    }
    else {
      for(j = 1; j <=lp->rows; j++) {
	colnr = lp->bas[j];
	sdp = pcol[j];
	if(colnr <= lp->rows)        /* A slack variable is in the basis */
	  errors[colnr] -= sdp;
	else {                       /* A normal variable is in the basis */
	  colnr -= lp->rows;
	  ie = lp->col_end[colnr];
	  i = lp->col_end[colnr - 1];
	  for(matentry = lp->mat + i; i < ie; i++, matentry++) {
	    k = (*matentry).row_nr;
	    errors[k] -= (*matentry).value*sdp;
	  }
	}
      }
      if (!ftran(lp, errors, lp->epsel))
        ok = FALSE;
      else {
	sdp = 0;
	for(j = 1; j <=lp->rows; j++)
	  if(fabs(errors[j])>sdp)
	    sdp = (REAL) fabs(errors[j]);
    /*      sdp += pow(errors[j],2); */
    /*    sdp = sqrt(sdp/lp->rows); */
	if(sdp > lp->epsel) {
	  if(lp->debug)
	    report(lp, DETAILED, "Iterative FTRAN correction metric %g", sdp);
	  for(j = 1; j <=lp->rows; j++)
	    pcol[j] += errors[j];
	}
      }
      free(errors);
    }
  }
  else
    if (!ftran(lp, pcol, lp->epsel))
      ok = FALSE;

#if FALSE
  if(lp->basis[varin]) {
    for(i = 1; (i <= lp->rows) && (lp->bas[i] != varin); i++);
    if(i > lp->rows)
      printf("oops1\n");
    else {
      for(j = 0; j <= lp->rows; j++)
        if(j != i) {
	  if(pcol[j])
	    printf("oops2\n");
	}
	else
	  if(pcol[j] != 1.0)
	    printf("oops3\n");
    }
  }
#endif

  return(ok);
} /* setpivcol */

#if defined NEWBB
int set_lowbo2(lprec *lp, int varnr, REAL newvalue)
{
  REAL *pcol, delta;
  int ok = TRUE, i, rows = lp->rows;

  varnr += rows;
  delta = newvalue - lp->lowbo[varnr];
  if (delta) {
    if (!lp->lower[varnr])
      delta = -delta;
    if(lp->basis[varnr]) {
      for(i = 1; (i <= rows) && (lp->bas[i] != varnr); i++);
      lp->rhs[i] -= delta;
    }
    else if((CALLOC(pcol, rows + 1) == NULL) || (!setpivcol(lp, varnr, pcol))) { /* construct one column of the tableau */
      lp->spx_status = OUT_OF_MEMORY;
      ok = FALSE;
    }
    else {
      for(i = 0; i <= rows; i++)
        lp->rhs[i] -= pcol[i] * delta;
    }
    if(ok)
      lp->lowbo[varnr] = newvalue;
  }
  return(ok);
}

int set_upbo2(lprec *lp, int varnr, REAL newvalue)
{
  REAL *pcol, delta;
  int ok = TRUE, i, rows = lp->rows;

  varnr += rows;
  if (!lp->lower[varnr]) {
    delta = newvalue - (lp->lowbo[varnr] + lp->upbo[varnr]);
    if (delta) {
      delta = -delta;
      if(lp->basis[varnr]) {
        for(i = 1; (i <= rows) && (lp->bas[i] != varnr); i++);
        lp->rhs[i] -= delta;
      }
      else if((CALLOC(pcol, rows + 1) == NULL) || (!setpivcol(lp, varnr, pcol))) { /* construct one column of the tableau */
        lp->spx_status = OUT_OF_MEMORY;
        ok = FALSE;
      }
      else {
        for(i = 0; i <= rows; i++)
          lp->rhs[i] -= pcol[i] * delta;
      }
    }
  }
  if(ok) {
    lp->upbo[varnr] = newvalue;
    if(newvalue < lp->infinite)
      lp->upbo[varnr] -= lp->lowbo[varnr];
  }
  return(ok);
}
#endif

static int minoriteration(lprec *lp, int colnr, int row_nr)
{
  int  i, j, k, wk, varin, varout, elnr;
  LREAL piv = 0, theta;

  varin = colnr + lp->rows;
  elnr = lp->eta_col_end[lp->eta_size];
  wk = elnr;
  lp->eta_size++;

  if(lp->Extrad != 0) {
    lp->eta_row_nr[elnr] = 0;
    lp->eta_value[elnr] = -lp->Extrad;
    elnr++;
    if(elnr >= lp->eta_alloc)
      if (!resize_eta(lp, elnr))
        return(FALSE);
  }

  /* Move pivot column data to Eta (but not the pivot row item yet) */
  for(j = lp->col_end[colnr - 1] ; j < lp->col_end[colnr]; j++) {
    k = lp->mat[j].row_nr;

    if(k == 0 && lp->Extrad != 0)
      lp->eta_value[lp->eta_col_end[lp->eta_size - 1]] += lp->mat[j].value;
    else if(k != row_nr) {
      lp->eta_row_nr[elnr] = k;
      lp->eta_value[elnr] = lp->mat[j].value;
      elnr++;
      if(elnr >= lp->eta_alloc)
	if (!resize_eta(lp, elnr))
	  return(FALSE);
    }
    else
      piv = lp->mat[j].value;
  }

  /* Now add the pivot row item to Eta */
  lp->eta_row_nr[elnr] = row_nr;
  lp->eta_value[elnr] = (REAL) (1 / piv);
  theta = lp->rhs[row_nr] / piv;
  lp->rhs[row_nr] = (REAL) theta;

  /* Update RHS for new pivot column */
  for(i = wk; i < elnr; i++)
    lp->rhs[lp->eta_row_nr[i]] = (REAL) (lp->rhs[lp->eta_row_nr[i]] - theta * lp->eta_value[i]);

  varout = lp->bas[row_nr];
  lp->bas[row_nr] = varin;
  lp->basis[varout] = FALSE;
  lp->basis[varin] = TRUE;

  /* Scale the pivoted column in Eta by the pivot value */
  for(i = wk; i < elnr; i++)
    lp->eta_value[i] = (REAL) (-lp->eta_value[i] / piv);

  lp->eta_col_end[lp->eta_size] = elnr + 1;
  return(TRUE);
} /* minoriteration */


static void rhsmincol(lprec *lp,
		      LREAL theta,
		      int row_nr,
		      int varin)
{
  int   i, j, k, varout;
  LREAL f;

  if(row_nr > lp->rows + 1) {
    if (lp->trace) {
      report(lp, IMPORTANT, "Error: rhsmincol called with row_nr: %d, rows: %d",
             row_nr, lp->rows);
      report(lp, IMPORTANT, "This indicates numerical instability");
    }
    lp->spx_status = FAILURE;
    return;
  }

  j = lp->eta_col_end[lp->eta_size];
  k = lp->eta_col_end[lp->eta_size + 1];
  for(i = j; i < k; i++) {
    f = lp->rhs[lp->eta_row_nr[i]] - theta * lp->eta_value[i];
    my_round(f, lp->epsb);
    lp->rhs[lp->eta_row_nr[i]] = (REAL) f;
  }

  lp->rhs[row_nr] = theta;
  varout = lp->bas[row_nr];
  lp->bas[row_nr] = varin;
  lp->basis[varout] = FALSE;
  lp->basis[varin] = TRUE;
} /* rhsmincol */

#if USED
static void get_markowitz(lprec *lp, int *rownz, int *colnz, MYBOOL *vbasic, int *rownr, int *colnr)
{
  int elmnr;
  int i, j, holdval, minval;
  unsigned int _MAXINT;

  _MAXINT =(unsigned int)  ~0;
  _MAXINT = _MAXINT / 2 - 1;
  minval = _MAXINT;
  (*rownr) = 0;
  (*colnr) = 0;

  for(j = 1; j <= lp->columns; j++) {
    if((vbasic[lp->rows+j] != TRUE) || (colnz[j] == 0)) continue;

   /* Make sure we have a non-zero value to pivot on */
    for(elmnr = lp->col_end[j-1]; elmnr < lp->col_end[j]; elmnr++) {
      i = lp->mat[elmnr].row_nr;
      if(i == 0) continue;
      if((vbasic[i] != FALSE) || (rownz[i] == 0)) continue;
      if(fabs(lp->mat[elmnr].value) < lp->epspivot) continue;

     /* Now compute the statistic */
      holdval = (rownz[i]-1)*(colnz[lp->rows+j]-1);
      if(holdval < minval) {
        minval = holdval;
        (*rownr) = i;
        (*colnr) = j;
      }
    }

   /* Don't do more work than necessary */
    if((*rownr) && (minval == 0)) break;
  }

  i = (*rownr);
  if(i > 0) {

   /* Update column entry counts */
    for(elmnr = lp->row_end[i - 1] + 1; elmnr <= lp->row_end[i]; elmnr++) {
      j = lp->rows+lp->col_no[elmnr];
      if((vbasic[j] == TRUE) && (colnz[j] > 0))
        colnz[j]--;
    }
   /* Update row entry counts */
    j = (*colnr);
    for(elmnr = lp->col_end[j - 1]; elmnr < lp->col_end[j]; elmnr++) {
      i = lp->mat[elmnr].row_nr;
      if((i > 0) && (vbasic[i] == FALSE) && (rownz[i] > 0))
        rownz[i]--;
    }
    vbasic[lp->rows+(*colnr)] = 2*TRUE;
    vbasic[(*rownr)] = 2*TRUE;
  }
}
#endif

static MYBOOL invert(lprec *lp)
{
  LREAL  theta, hold;
  matrec *matentry;
  REAL   *pcol = NULL;
  MYBOOL   *fcol = NULL, *frow = NULL;
  int    *colnum = NULL, *rownum = NULL, *col = NULL, *row = NULL;
  int    k, kk, kkk, i, j, v, numit, varnr, rownr, colnr, varin;
  int	 singularities;
  short  spx_save;
  MYBOOL   Restart;

 /* Must save spx_status since it is used to carry information about
    the presence and handling of singular columns in the matrix */
  spx_save = lp->spx_status;
  lp->spx_status = RUNNING;
  if(yieldformessages(lp)!=0)
    lp->spx_status = USERABORT;

  if((lp->usermessage != NULL) && (lp->msgmask & MSG_INVERT))
    lp->usermessage(lp, lp->msghandle, MSG_INVERT);

  if(lp->spx_status != RUNNING)
    return(FALSE);
  lp->spx_status = spx_save;
  singularities = 0;

  if(lp->print_at_invert)
    report(lp, DETAILED, "Start Invert iter %d eta_size %d rhs[0] %g ",
	    lp->iter, lp->eta_size, (double) - lp->rhs[0]);

  if ((CALLOC(col, lp->rows + 1, int) == NULL) ||
      (CALLOC(row, lp->rows + 1, int) == NULL) ||
      (CALLOC(pcol, lp->rows + 1, REAL) == NULL) ||
      (CALLOC(frow, lp->rows + 1, MYBOOL) == NULL) ||
      (CALLOC(fcol, lp->columns + 1, MYBOOL) == NULL) ||
      (CALLOC(rownum, lp->rows + 1, int) == NULL) ||
      (CALLOC(colnum, lp->columns + 1, int) == NULL)
     ) {
    lp->spx_status = OUT_OF_MEMORY;
    FREE(colnum);
    FREE(rownum);
    FREE(fcol);
    FREE(frow);
    FREE(pcol);
    FREE(row);
    FREE(col);
    return(FALSE);
  }

  lp->time_refactstart = timenow();    /* Time of start of current cyle */

  /* Initialize working basis indicators to all slacks ... */
  for(i = 0; i <= lp->rows; i++)
    frow[i] = TRUE;                   /* Row slack is in the basis */
  for(i = 0; i < lp->columns; i++)
    fcol[i] = FALSE;                  /* Column has not been pivoted in */

  /* ... then store state of pre-existing basis */
  for(i = 0; i <= lp->rows; i++)
    if(lp->bas[i] > lp->rows)
      fcol[lp->bas[i] - lp->rows] = TRUE;
    else
      frow[lp->bas[i]] = FALSE;

  /* Get row and column number entry counts for basic slacks (includes OF row) */
  for(i = 1; i <= lp->rows; i++) {
    if(frow[i])
      for(j = lp->row_end[i - 1] + 1; j <= lp->row_end[i]; j++) {
	v = lp->col_no[j];
	if(fcol[v]) {
	  colnum[v]++;
	  rownum[i]++;
	}
      }
  }

  /* Reset basis indicators to all slacks */
  for(i = 1; i <= lp->rows; i++) {
    lp->bas[i] = i;
    lp->basis[i] = TRUE;
  }
  for(i = 1; i <= lp->columns; i++)
    lp->basis[i + lp->rows] = FALSE;

  /* Save lower bound-adjusted RHS */
  for(i = 0 /* 1 */; i <= lp->rows; i++) /* changed by peno from 1 to 0 11.07.03 */
    lp->rhs[i] = lp->rh[i];

  /* Adjust active RHS for state of variable */
  for(i = 1; i <= lp->columns; i++) {
    varnr = lp->rows + i;
    if(!lp->lower[varnr]) {
      theta = lp->upbo[varnr];
      k = lp->col_end[i];
      j = lp->col_end[i - 1];
      for(matentry = lp->mat + j; j < k; j++, matentry++) {
        v = (*matentry).row_nr;
        lp->rhs[v] -= theta * (*matentry).value;
      }
    }
  }

  /* Finally, adjust for row state if it is at its upper bound */
  for(i = 1; i <= lp->rows; i++)
    if(!lp->lower[i])
      lp->rhs[i] -= lp->upbo[i];

  /* Check timeout and user abort again */
  spx_save = lp->spx_status;
  lp->spx_status = RUNNING;
  if(yieldformessages(lp) != 0)
    lp->spx_status = USERABORT;
  else
    lp->spx_status = spx_save;

  k = 0;    /* Total number of rows pivoted */
  numit = 0;
  singularities = 0;
  kkk = 0;  /* Number of singleton rows pivoted in current iteration */
  if(!((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY))) {
    /* Progress to the inversion process proper */
    lp->num_inv = 0;
    lp->num_refact++;
    lp->eta_size = 0;

    /* Loop reentry point */

    kk = 0;   /* Singleton pivot iteration counter */
    do {
      kk++;
      kkk = 0;  /* Number of singleton rows pivoted in current iteration */

      /* Loop over rows, hunting for row singletons */
      rownr = 0;
      v = 0;
      while(v < lp->rows) {
        rownr++;
        if(rownr > lp->rows)
          rownr = 1;

        v++;
        if(rownum[rownr] == 1)
          if(frow[rownr]) {
    	    v = 0;
            k++;
            kkk++;

            /* Find first column available to be pivoted */
            j = lp->row_end[rownr - 1] + 1;
    	    while(!(fcol[lp->col_no[j]]))
    	      j++;
    	    colnr = lp->col_no[j];

            /* Reduce item counts for the selected pivot column/row */
            colnum[colnr] = 0;
    	    fcol[colnr] = FALSE;
    	    for(j = lp->col_end[colnr - 1]; j < lp->col_end[colnr]; j++)
    	      if(frow[lp->mat[j].row_nr])
    		rownum[lp->mat[j].row_nr]--;
    	    frow[rownr] = FALSE;
            /* if(rownum[rownr]) */
            /*   rownum[rownr] = 0; */

            /* Perform the pivot */
            if (!minoriteration(lp, colnr, rownr))
	      break;
          }
      }

      if(!((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY))) {
	/* Loop over columns, hunting for column singletons */
	colnr = 0;
	v = 0;
	if((k < lp->rows) && ((kk <= 0) || (kkk > 0)))
	  while(v < lp->columns) {
	    colnr++;
	    if(colnr > lp->columns)
	      colnr = 1;

	    v++;
	    if(colnum[colnr] == 1)
	      if(fcol[colnr]) {
      		v = 0;
		k++;
		kkk++;

		/* Find first available row to be pivoted */
		j = lp->col_end[colnr - 1];
      		while(!(frow[lp->mat[j].row_nr]))
      		  j++;
      		rownr = lp->mat[j].row_nr;

		/* Reduce item counts for the selected pivot column/row */
		rownum[rownr] = 0;
      		frow[rownr] = FALSE;
      		for(j = lp->row_end[rownr - 1] + 1; j <= lp->row_end[rownr]; j++)
      		  if(fcol[lp->col_no[j]])
      		    colnum[lp->col_no[j]]--;
      		fcol[colnr] = FALSE;
		/* if(colnum[colnr]) */
		/*   colnum[colnr] = 0; */

		/* Store pivot information */
		numit++;
      		col[numit - 1] = colnr;
      		row[numit - 1] = rownr;
	      }
	  }

	/* Check timeout and user abort again */
	spx_save = lp->spx_status;
	lp->spx_status = RUNNING;
	if(yieldformessages(lp)!=0)
	  lp->spx_status = USERABORT;
	else
	  lp->spx_status = spx_save;
      }

      Restart = FALSE;
      if(!((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY))) {
        /* Check for more singletons, exhaust the supply - Added by KE */
        if((kkk > 0) && (k < lp->rows))
          Restart = TRUE;
        else if (k < lp->rows) {
#if 0
          for(j = 1; j <= lp->columns; j++) {
            get_markowitz(lp, rownum, colnum, frow, fcol, &rownr, &colnr);
            if(colnr) {
              fcol[colnr] = FALSE;
              /*    colnum[colnr] = 0; */
              if (!setpivcol(lp, lp->rows + colnr, pcol)) {
  		Restart = FALSE;
                break;
              }
  	      else {
                frow[rownr] = FALSE;
            /*    rownum[rownr]--; */
                condensecol(lp, rownr, pcol);
                theta = lp->rhs[rownr] / (LREAL)pcol[rownr];
                rhsmincol(lp, theta, rownr, lp->rows + colnr);
                addetacol(lp /* , colnr */);
                k++;
                kkk++;
  		if(k >= lp->rows)
                  break;
  	      }
            }
          }
#endif
        }
      }
    } while (Restart);
  }
  if(!((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY))) {
    /* Find pivots for remaining non-singleton cases where a column is in the basis */
    if(k < lp->rows) {
      for(j = 1; j <= lp->columns; j++) {
        colnr = j;
        if(fcol[colnr]) {
          fcol[colnr] = FALSE;
          if (!setpivcol(lp, lp->rows + colnr, pcol))
	     break;

          /* Find first coefficient available row to be pivoted; **** KE deleted!
             (original lp_solve version that brings a lot of numerical instability) */
         /*
          rownr = 1;
          while((rownr <= lp->rows) && (!(frow[rownr] && pcol[rownr])))
    	    rownr++;
         */

          /* Find largest coefficient available row to be pivoted; **** KE added!
             much better numerical stability, but requires more CPU processing */
          rownr = lp->rows + 1;
          hold = 0;
          for(i = 1; i <= lp->rows; i++) {
            if(frow[i] && fabs(pcol[i])>hold) {
              hold = fabs(pcol[i]);
              rownr = i;
            }
          }

          if(rownr > lp->rows) {
            /* This column is singular!  Just skip it, leaving one of the
               slack variables basic in its place... (Source: Geosteiner changes!) */
            report(lp, DETAILED, "--> Column %d is singular! Skipped.", colnr);
            singularities++;
          }
          else {
            frow[rownr] = FALSE;
            if (!condensecol(lp, rownr, pcol))
	      break;
            theta = lp->rhs[rownr] / (LREAL)pcol[rownr];
            rhsmincol(lp, (REAL) theta, rownr, lp->rows + colnr);
            addetacol(lp /* , colnr */);
          }
          k++;
          kkk++;
          if(k >= lp->rows)
            break;
        }
      }

      if(!((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY))) {
	/* Check timeout and user abort again */
	spx_save = lp->spx_status;
	lp->spx_status = RUNNING;
	if(yieldformessages(lp)!=0)
	  lp->spx_status = USERABORT;
	else
	  lp->spx_status = spx_save;
      }
    }
  }
  if(!((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY))) {
    /* Perform pivoting of the row-column combinations stored above */
    for(i = numit - 1; i >= 0; i--) {
      colnr = col[i];
      rownr = row[i];
      varin = lp->rows + colnr;

      /* Move the constraint column to the dense pcol vector */
      for(j = 0; j <= lp->rows; j++)
        pcol[j] = 0;
      for(j = lp->col_end[colnr - 1]; j < lp->col_end[colnr]; j++)
        pcol[lp->mat[j].row_nr] = lp->mat[j].value;
      pcol[0] -= lp->Extrad;

      if (!condensecol(lp, rownr, pcol))
        break;
      theta = lp->rhs[rownr] / (LREAL)pcol[rownr];
      rhsmincol(lp, (REAL) theta, rownr, varin);
      addetacol(lp /* , colnr */);
    }

    if(!((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY))) {
      /* Round net RHS values */
      for(i = 1; i <= lp->rows; i++)
	my_round(lp->rhs[i], lp->epsel);

      /* Do user reporting */
      if(yieldformessages(lp)!=0)
	lp->spx_status = USERABORT;

      if((lp->usermessage != NULL) && (lp->msgmask & MSG_INVERT))
	lp->usermessage(lp, lp->msghandle, MSG_INVERT);

      if(lp->print_at_invert)
	report(lp, DETAILED,
  		"End Invert                eta_size %d rhs[0] %g",
  		lp->eta_size, (double) - lp->rhs[0]);

      /* Set inversion completion status */
      lp->justInverted = TRUE;
      lp->doInvert = FALSE;
    }
  }
  free(rownum);
  free(col);
  free(row);
  free(pcol);
  free(frow);
  free(fcol);
  free(colnum);

  return((MYBOOL) (singularities <= 0));
} /* invert */

static int colprim(lprec *lp,
		     MYBOOL minit,
		     REAL   *drow)
{
  int  varnr, i, j, k, ie, ok = TRUE;
  int  colnr;
  LREAL f, dpiv, opiv;
  matrec *matentry;

  if(!minit) {
    for(i = 1; i <= lp->sum; i++)
      drow[i] = 0;
    drow[0] = 1;

    /* Test if we should do the error-correction version */
#if FALSE
    if(FALSE && (lp->improve & IMPROVE_BTRAN) && lp->num_inv) {
      REAL  *errors;

      if (MALLOCCPY(errors, drow, lp->rows + 1) == NULL) {
        lp->spx_status = OUT_OF_MEMORY;
        ok = FALSE;
      }
      else {
	btran(lp, drow, lp->epsel);

	for(j = 1; j <=lp->rows; j++) {
	  colnr = lp->bas[j];
	  if(colnr <= lp->rows)        /* A slack variable is in the basis */
            f = drow[j];
	  else {                       /* A normal variable is in the basis */
	    colnr -= lp->rows;
	    f = 0;
	    ie = lp->col_end[colnr];
	    i = lp->col_end[colnr - 1];
	    for(matentry = lp->mat + i; i < ie; i++, matentry++) {
	      k = (*matentry).row_nr;
	      f += drow[k] * (*matentry).value;
	    }
          }
          errors[j] -= (REAL) f;
	}
	btran(lp, errors, lp->epsel);

	f = 0;
	for(j = 1; j <=lp->rows; j++)
  /*        f += pow(errors[j],2); */
	  if(fabs(errors[j])>f)
	    f = fabs(errors[j]);
  /*      f = sqrt(f/lp->rows); */
	if(f > lp->epsel) {
	  if(lp->debug)
	    report(lp, DETAILED, "Iterative BTRAN correction metric %g", f);
	  for(j = 1; j <=lp->rows; j++)
	    drow[j] += errors[j];
	}
	free(errors);
      }
    }
    else
#endif
      btran(lp, drow, lp->epsel);

    if (ok) {
      /* Continue */
      for(i = 1; i <= lp->columns; i++) {
	varnr = lp->rows + i;
	if(!lp->basis[varnr])
	  if(lp->upbo[varnr] > 0) {
	    f = 0;
	    ie = lp->col_end[i];
	    j = lp->col_end[i - 1];
	    for(matentry = lp->mat + j; j < ie; j++, matentry++) {
	      k = (*matentry).row_nr;
	      f += drow[k] * (*matentry).value;
	    }
	    drow[varnr] = (REAL) f;
	  }
      }
    }
    if (ok)
      for(i = 1; i <= lp->sum; i++)
        my_round(drow[i], lp->epsd);
  }

  if (ok) {
    /* Identify pivot column (fall-through is 'largest coefficient') */
    dpiv = lp->epsd;
    colnr = 0;
    opiv = 0;
    varnr = 0;

    for(i = lp->sum; i > 0; i--) {
      if(!lp->basis[i])
	if(lp->upbo[i] > 0) {

       /* Retrieve the reduced cost, skip if non-positive */
          if(lp->lower[i])
	    f = -drow[i];
	  else
	    f = drow[i];
          if(f < lp->epsd) continue;

          /* Save largest reduced cost */
	  if(f > dpiv) {
            dpiv = f;
            colnr = i;
          }

          /* Compute objective contribution */
	  if(lp->piv_rule == GREEDY_SELECT && i > lp->rows) {
            f = get_mat_raw(lp, 0, i - lp->rows);
   	    if(f < opiv) {
              opiv = f;
	      varnr = i;
 	      if(i <= lp->rows+1) {
	        colnr = varnr;
	        break;
	      }
	    }
	  }
          if(colnr > 0 && lp->piv_rule == FIRST_SELECT) break;
	}
    }

    if(lp->trace) {
      if(colnr>0)
	report(lp, DETAILED, "colprim:%d, reduced cost: %g",
		colnr, (double)dpiv);
      else
	report(lp, DETAILED,
		"colprim: no positive reduced costs found, optimality!\n");
    }
    if(colnr == 0) {
      lp->doIterate = FALSE;
      lp->doInvert = FALSE;
      lp->spx_status = OPTIMAL;
    }
  }
  else {
    colnr = -1;
    lp->doIterate = FALSE;
    lp->doInvert = FALSE;
    lp->spx_status = OUT_OF_MEMORY;
  }
  return(colnr);
} /* colprim */

static int rowprim(lprec *lp,
		     int colnr,
		     RREAL *theta,
		     REAL *pcol)
{
  int   i, row_nr;
  LREAL f, quot, savef;

  row_nr = 0;
  (*theta) = lp->infinite;
  savef = 0;
  quot = 0;

  for(i = 1; i <= lp->rows; i++) {
    f = pcol[i];
    if(f != 0) {
      if(my_abs(f) < lp->epspivot) {
        if(lp->trace)
  	  report(lp, FULL, "Pivot %g rejected, too small (limit %g)",
		 (double)f, (double)lp->epspivot);
      }
      else { /* pivot alright */
	if(f > 0)
	  quot = lp->rhs[i] / f;
	else if(lp->upbo[lp->bas[i]] < lp->infinite)
	  quot = (lp->rhs[i] - lp->upbo[lp->bas[i]]) / f;
        else {
	  savef = f;
          quot = 2 * lp->infinite;
	}
        my_round(quot, lp->epsel);
        if(quot < (*theta))
          if(quot >= 0)  /* Added by KE 19052002 */
        {
          (*theta) = quot;
          row_nr = i;
          /* if(lp->piv_rule == FIRST_SELECT) break; */ /* this can not be right <peno> */
        }
      }
    }
  }

  /* No pivot greater than epspivot was found; accept a smaller one */
  if(row_nr == 0)
    for(quot = 0, i = 1; i <= lp->rows; i++) {
      f = pcol[i];
      if(f != 0) {
	if(f > 0)
	  quot = lp->rhs[i] / f;
	else if(lp->upbo[lp->bas[i]] < lp->infinite)
	  quot = (lp->rhs[i] - lp->upbo[lp->bas[i]]) / f;
        else {
	  savef = f;
          quot = 2 * lp->infinite;
	}
        my_round(quot, lp->epsel);
        if(quot < (*theta))
          if(quot >= 0)    /* Added by KE 19052002 */
        {
          (*theta) = quot;
          row_nr = i;
          /* if(lp->piv_rule == FIRST_SELECT) break; */ /* this can not be right <peno> */
        }
      }
    }

  if(row_nr == 0) {
    if(lp->upbo[colnr] == lp->infinite) {
      lp->doIterate = FALSE;
      lp->doInvert = FALSE;
      lp->spx_status = UNBOUNDED;
    }
    else {
      i = 1;
      while(pcol[i] >= 0 && i <= lp->rows)
	i++;
      if(i > lp->rows) { /* empty column with upperbound! */
	lp->lower[colnr] = FALSE;
	lp->rhs[0] += lp->upbo[colnr]*pcol[0];
        lp->doIterate = FALSE;
        lp->doInvert = FALSE;
      }
      else /* if(pcol[i]<0) */ {
	row_nr = i;
      }
    }
  }
  else
/*  if((*theta) < 0) { */
    if((*theta) >= lp->infinite) {   /* Added by KE 19052002 */
      (*theta) = -1;                 /* Added by KE 19052002 */
      report(lp, SEVERE, "Warning: Numerical instability, quot = %g",
	      (double)(*theta));
      report(lp, IMPORTANT, "pcol[%d] = %18g, rhs[%d] = %18g , upbo = %g",
	      row_nr, (double)savef, row_nr, (double)lp->rhs[row_nr],
	      (double)lp->upbo[lp->bas[row_nr]]);
    }

  if(row_nr > 0)
    lp->doIterate = TRUE;
  if(lp->trace)
    report(lp, DETAILED, "rowprim:%d, pivot element:%18g", row_nr,
	    (double)pcol[row_nr]);

  return(row_nr);
} /* rowprim */

#if defined antismallpivot

static int rowdual(lprec *lp, int skiprow)
{
  int   i;
  int   row_nr;
  RREAL f, g, minrhs, maxrhs;
  /* MYBOOL  artifs; */

  row_nr = 0;
  if(skiprow) {
    f = lp->upbo[lp->bas[skiprow]];
    if(lp->rhs[skiprow] < f - lp->rhs[skiprow])
      g = lp->rhs[skiprow];
    else
      g = f - lp->rhs[skiprow];
    maxrhs = g;
  }
  else
    maxrhs = -lp->infinite;
  minrhs = -lp->epsb - lp->epsd;
  i = 0;
  /* artifs = FALSE; */
  while(i < lp->rows /* && !artifs */) {
    i++;
    f = lp->upbo[lp->bas[i]];
    /* if(f == 0 && (lp->rhs[i] != 0)) { */
    /* if(f == 0 && (my_abs(lp->rhs[i]) > lp->epsb)) { */

#if FALSE
    g = lp->epsd;
    if((my_abs(f) <= g) && (my_abs(lp->rhs[i]) >= g)) {    /* KE+PN changed from 0, respectively */
      artifs = TRUE;
      row_nr = i;
      if(lp->piv_rule == FIRST_SELECT) break;
    }
    else
#endif
    {
      if(lp->rhs[i] < f - lp->rhs[i])
	g = lp->rhs[i];
      else
	g = f - lp->rhs[i];
      if((g < minrhs) && (g > maxrhs)) {
	row_nr = i;
	if(lp->piv_rule == FIRST_SELECT) break;
	minrhs = g - lp->epsd; /* KE added lp->epsel on 20030621 to avoid NUMERICAL ERROR warning */
      }
    }
  }

  if(lp->trace) {
    if(row_nr > 0) {
      report(lp, DETAILED,
	      "rowdual: %d, rhs of selected row:        %g",
	      row_nr, (double)lp->rhs[row_nr]);
      if(lp->upbo[lp->bas[row_nr]] < lp->infinite)
	report(lp, DETAILED,
		"\t\tupper bound of basis variable:    %18g",
		(double)lp->upbo[lp->bas[row_nr]]);
    }
    else
      report(lp, FULL, "rowdual: no infeasibilities found");
  }

  return(row_nr);
} /* rowdual */

#if defined antidegeneracypeter
REAL peter;
#endif

#else

static int rowdual(lprec *lp)
{
  int   i;
  int   row_nr;
  RREAL f, g, minrhs;
  MYBOOL  artifs;

  row_nr = 0;
  minrhs = -lp->epsb - lp->epsd;
  i = 0;
  artifs = FALSE;
  while(i < lp->rows && !artifs) {
    i++;
    f = lp->upbo[lp->bas[i]];
    /* if(f == 0 && (lp->rhs[i] != 0)) { */
    /* if(f == 0 && (my_abs(lp->rhs[i]) > lp->epsb)) { */
    g = lp->epsd;
    if((my_abs(f) <= g) && (my_abs(lp->rhs[i]) >= g)) {
      artifs = TRUE;
      row_nr = i;
      if(lp->piv_rule == FIRST_SELECT) break;
    }
    else {
      if(lp->rhs[i] < f - lp->rhs[i])
	g = lp->rhs[i];
      else
	g = f - lp->rhs[i];
      if(g < minrhs) {
	row_nr = i;
	if(lp->piv_rule == FIRST_SELECT) break;
	minrhs = g - lp->epsd; /* KE added lp->epsel on 20030621 to avoid NUMERICAL ERROR warning */
      }
    }
  }

  if(lp->trace) {
    if(row_nr > 0) {
      report(lp, DETAILED,
	      "rowdual: %d, rhs of selected row:        %g",
	      row_nr, (double)lp->rhs[row_nr]);
      if(lp->upbo[lp->bas[row_nr]] < lp->infinite)
	report(lp, DETAILED,
		"\t\tupper bound of basis variable:    %18g",
		(double)lp->upbo[lp->bas[row_nr]]);
    }
    else
      report(lp, FULL, "rowdual: no infeasibilities found");
  }

  return(row_nr);
} /* rowdual */

#endif

static int coldual(lprec *lp,
		     int row_nr,
		     MYBOOL minit,
		     REAL *prow,
		     REAL *drow)
{
  int   i, j, k, r, varnr, *rowp, row;
  int   colnr;
  LREAL d, f, g, pivot, theta, quot;
  REAL  *valuep;

  lp->doIterate = FALSE;
  if(!minit) {
    for(i = 0; i <= lp->rows; i++) {
      prow[i] = 0;
      drow[i] = 0;
    }

    drow[0] = 1;
    prow[row_nr] = 1;

    /* A double BTRAN equation solver process is implemented "in-line" below in
       order to save time and to implement different rounding for the two */
/*     btran(lp, drow, lp->epsd);
     btran(lp, prow, lp->epsel); */

    for(i = lp->eta_size; i >= 1; i--) {
      d = 0;
      f = 0;
      k = lp->eta_col_end[i] - 1;
      r = lp->eta_row_nr[k];
      j = lp->eta_col_end[i - 1];

      /* this is one of the loops where the program consumes a lot of CPU time
         let's help the compiler by doing some pointer arithmetic instead of array indexing */
      for(rowp = lp->eta_row_nr + j, valuep = lp->eta_value + j;
	  j <= k;
	  j++, rowp++, valuep++) {
	f += prow[*rowp] * *valuep;
	d += drow[*rowp] * *valuep;
      }

      my_round(f, lp->epsel * 10.0); /* in test <peno> */
      prow[r] = (REAL) f;
      my_round(d, lp->epsd);
      drow[r] = (REAL) d;
    }

    /* Multiply solution vectors with matrix values */
    for(i = 1; i <= lp->columns; i++) {
      varnr = lp->rows + i;
      if(!lp->basis[varnr]) {
	matrec *matentry;

	d = - lp->Extrad * drow[0];
	f = 0;
	k = lp->col_end[i];
	j = lp->col_end[i - 1];

	/* this is one of the loops where the program consumes a lot of cpu time
	   let's help the compiler with pointer arithmetic instead of array indexing */
	for(matentry = lp->mat + j;
	    j < k;
	    j++, matentry++) {
	  row = (*matentry).row_nr;
          d += drow[row] * (*matentry).value;
	  f += prow[row] * (*matentry).value;
	}

	my_round(f, lp->epsel);
	prow[varnr] = (REAL) f;
	my_round(d, lp->epsd);
	drow[varnr] = (REAL) d;
      }
    }
  }

  if(lp->rhs[row_nr] > lp->upbo[lp->bas[row_nr]])
    g = -1;
  else
    g = 1;

  pivot = 0;
  colnr = 0;
  theta = lp->infinite;

  for(i = 1; i <= lp->sum; i++) {
    if(lp->lower[i])
      d = prow[i] * g;
    else
      d = -prow[i] * g;

    if((d < 0) && (!lp->basis[i]) && (lp->upbo[i] > 0)) {
      if(lp->lower[i])
	quot = -drow[i] / d;
      else
	quot = drow[i] / d;
      if(quot < theta) {
	theta = quot;
	pivot = d;
	colnr = i;
#if defined antidegeneracypeter
peter=prow[i]*(lp->lower[i] ? 1.0 : -1.0);
#endif
      }
      else if((quot == theta) && (my_abs(d) > my_abs(pivot))) {
	pivot = d;
	colnr = i;
#if defined antidegeneracypeter
peter=prow[i]*g;
#endif
      }
    }
  }

  if(lp->trace)
    report(lp, NORMAL, "coldual: %d, pivot element:  %g", colnr,
	    (double)prow[colnr]);

  if(colnr > 0)
    lp->doIterate = TRUE;

  return(colnr);
} /* coldual */

static void iteration(lprec *lp,
		      int row_nr,
		      int varin,
		      RREAL *theta,
		      REAL up,
		      MYBOOL *minit,
		      MYBOOL *low,
		      MYBOOL primal)
{
  int  i, k, varout;
  LREAL f;
  REAL  pivot;

  if(yieldformessages(lp)!=0)
    lp->spx_status = USERABORT;

  lp->iter++;
  if((lp->usermessage != NULL) && (lp->msgmask & MSG_ITERATION))
    lp->usermessage(lp, lp->msghandle, MSG_ITERATION);

  if(lp->spx_status != RUNNING)
    return;

  if(((*minit) = (MYBOOL) ((*theta) > (up + lp->epsb)))) {
    (*theta) = up;
    (*low) = (MYBOOL) !(*low);
  }

  k = lp->eta_col_end[lp->eta_size + 1];
  pivot = lp->eta_value[k - 1];

  for(i = lp->eta_col_end[lp->eta_size]; i < k; i++) {
    varout = lp->eta_row_nr[i];
    f = lp->rhs[varout] - (*theta) * lp->eta_value[i];
/*    my_round(f, lp->epsb); */ /* **** Could a large value here actually introduce errors? */
    my_round(f, lp->epsel);
    lp->rhs[varout] = (REAL) f;
  }

  if(!(*minit)) {
    lp->rhs[row_nr] = (REAL) (*theta);
    varout = lp->bas[row_nr];
    lp->bas[row_nr] = varin;
    lp->basis[varout] = FALSE;
    lp->basis[varin] = TRUE;

    if(primal && pivot < 0)
      lp->lower[varout] = FALSE;

    if(!(*low) && up < lp->infinite) {
      (*low) = TRUE;
      lp->rhs[row_nr] = up - lp->rhs[row_nr];
      for(i = lp->eta_col_end[lp->eta_size]; i < k; i++)
	lp->eta_value[i] = -lp->eta_value[i];
    }

    addetacol(lp /* , 0 */);
    lp->num_inv++;
  }

  if(lp->trace) {
    report(lp, DETAILED, "Theta = %g", (double)(*theta));
    if((*minit)) {
      if(!lp->lower[varin])
	report(lp, DETAILED,
		"Iteration: %d, variable %d changed from 0 to its upper bound of %g",
		lp->iter, varin, (double)lp->upbo[varin]);
      else
	report(lp, DETAILED,
		"Iteration: %d, variable %d changed its upper bound of %g to 0",
		lp->iter, varin, (double)lp->upbo[varin]);
    }
    else
      report(lp, DETAILED,
	      "Iteration: %d, variable %d entered basis at: %g",
	      lp->iter, varin, (double)lp->rhs[row_nr]);
    if(!primal) {
      f = 0;
      for(i = 1; i <= lp->rows; i++)
	if(lp->rhs[i] < 0)
	  f -= lp->rhs[i];
	else
	  if(lp->rhs[i] > lp->upbo[lp->bas[i]])
	    f += lp->rhs[i] - lp->upbo[lp->bas[i]];
      report(lp, DETAILED, "Feasibility gap of this basis: %g",
	      (double)f);
    }
    else
      report(lp, DETAILED,
	      "Objective function value of this feasible basis: %g",
	      (double)lp->rhs[0]);
  }
} /* iteration */

static MYBOOL IsDualFeasible(lprec *lp, REAL tol)
{
  int    i;
  MYBOOL feasible = TRUE;
  RREAL  x;

  for(i = 1; i <= lp->rows; i++) {
    x = lp->rhs[i];
    if((x < -tol) || (x > lp->upbo[lp->bas[i]] + tol)) {
      feasible = FALSE;
      break;
    }
  }
  return(feasible);
}

static int primloop(lprec *lp, REAL *refacttime)
{
  int	 i, ok = TRUE, Ncycle;
  RREAL  theta, f;
  REAL   *drow = NULL, *prow = NULL, *pcol = NULL, prevobj, epsel;
  MYBOOL   primal;
  MYBOOL   minit;
  int    colnr, row_nr;

  if(lp->trace)
    report(lp, DETAILED, "Entering primal algorithm");

  if ((CALLOC(drow, lp->sum + 1, REAL) == NULL) ||
      (CALLOC(prow, lp->sum + 1, REAL) == NULL) ||
      (CALLOC(pcol, lp->rows + 1, REAL) == NULL)
     ) {
    lp->spx_status = OUT_OF_MEMORY;
    ok = FALSE;
  }
  else {
    primal = TRUE;
    lp->spx_status = RUNNING;
    lp->doInvert = FALSE;
    lp->doIterate = FALSE;
    lp->Extrad = 0;

    row_nr = 0;
    colnr = 0;

    minit = FALSE;

    prevobj = 0.0;
    epsel = lp->epsel;
    Ncycle = 0;

    while(lp->spx_status == RUNNING) {
#if defined peterprintiter
printf("primal: %f\n",lp->rhs[0]*(lp->maximise ? 1 : -1) / (lp->scaling_used ? lp->scale[0] : 1.0));
#endif

#if defined antidegeneracy
if(lp->piv_rule != FIRST_SELECT) {
  if(fabs((double) lp->rhs[0] - prevobj) < epsel) {
    if(++Ncycle > 50)
      lp->piv_rule = FIRST_SELECT;
  }
  else {
    prevobj = (double) lp->rhs[0];
    Ncycle = 0;
  }
 }
#endif

      lp->doIterate = FALSE;
      lp->doInvert = FALSE;

      if ((colnr = colprim(lp, minit, drow)) == -1) {  /* Solve BTRAN here */
        ok = FALSE;
	break;
      }
      if(colnr > 0) {
	if (!setpivcol(lp, colnr, pcol)) {        /* Solve FTRAN here */
	  ok = FALSE;
	  break;
	}

	row_nr = rowprim(lp, colnr, &theta, pcol);
	if(row_nr > 0)
	  if (!condensecol(lp, row_nr, pcol)) {
	    ok = FALSE;
	    break;
	  }
      }

      if(lp->doIterate) {
	iteration(lp, row_nr, colnr, &theta, lp->upbo[colnr], &minit,
		  &lp->lower[colnr], primal);
	if((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT))
	  break;
      }

      if(lp->num_inv > 0)
	f = (timenow()-lp->time_refactstart) / lp->num_inv;
      else
	f = 0;
      if((lp->num_inv >= lp->max_num_inv) ||
	 ((lp->num_inv > 1) && (f >= MINTIMEPIVOT) && (f >= (*refacttime))))
	lp->doInvert = TRUE;

      if(lp->doInvert) {
	if(lp->print_at_invert)
	  report(lp, DETAILED, "Inverting: Primal = %d", primal);
	i = invert(lp);
	if((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY)) {
	  ok = FALSE;
	  break;
	}
	else if(!i) {
	  lp->spx_status = SINGULAR_BASIS;
	  break;
	}
	/* Check whether we are still feasible or not... */
	if(!IsDualFeasible(lp, lp->epsd))
          lp->spx_status = LOST_PRIMAL_FEASIBILITY;
      }
      else
	(*refacttime) = (REAL) f;

      if(yieldformessages(lp)!=0)
	lp->spx_status = USERABORT;

    }
  }

  FREE(drow);
  FREE(prow);
  FREE(pcol);
  return(ok);
} /* primloop */

static int dualloop(lprec *lp, REAL *refacttime)
{
  int    i, j, ok = TRUE, Ncycle;
#if defined antismallpivot
  int skiprow = 0, row_nr0 = 0;
#endif
  RREAL  f, theta;
  MYBOOL   primal;
  REAL   *drow = NULL, *prow = NULL, *pcol = NULL, prevobj, epsel;
#if defined antismallpivot
  REAL skip0 = 0;
#endif
  MYBOOL   minit;
  int    colnr, row_nr;

  if(lp->trace)
    report(lp, DETAILED, "Entering dual algorithm");

  if ((CALLOC(drow, lp->sum + 1, REAL) == NULL) ||
      (CALLOC(prow, lp->sum + 1, REAL) == NULL) ||
      (CALLOC(pcol, lp->rows + 1, REAL) == NULL)
     ) {
    lp->spx_status = OUT_OF_MEMORY;
    FREE(pcol);
    ok = FALSE;
  }
  else {
    primal = FALSE;
    lp->spx_status = RUNNING;
    lp->doInvert = FALSE;
    lp->doIterate = FALSE;

    /* Set Extrad to be the most negative of the objective coefficients.  */
    /* We effectively subtract Extrad from every element of the objective */
    /* row, thereby making the entire objective row non-negative.  Note   */
    /* that this forces dual feasibility!  Although it also alters the	  */
    /* objective function, we don't really care about that too much	  */
    /* because we only use the dual algorithm to obtain a primal feasible */
    /* solution that we can start the primal algorithm with.  Virtually	  */
    /* any non-zero objective function will work for this!	          */
    lp->Extrad = 0;
    for(i = 1; i <= lp->columns; i++) {
      f = 0;
      for(j = lp->col_end[i - 1]; j < lp->col_end[i]; j++)
	if(lp->mat[j].row_nr == 0)
	  f += lp->mat[j].value;
	else       /* Since the A-matrix is sorted with the objective function */
	  break;   /* first we do not need to scan the entire matrix  - *** KE added */

      if(f < lp->Extrad)
	lp->Extrad = (REAL) f;
    }

    if(lp->trace)
      report(lp, DETAILED, "Extrad = %g", (double)lp->Extrad);

    row_nr = 0;
    colnr = 0;

    minit = FALSE;

    prevobj = 0.0;
    Ncycle = 0;
    epsel = lp->epsel;

    while(lp->spx_status == RUNNING) {
#if defined peterprintiter
printf("dual: %f\n",lp->rhs[0]*(lp->maximise ? 1 : -1) / (lp->scaling_used ? lp->scale[0] : 1.0));
#endif

#if defined antidegeneracy
if(lp->piv_rule != FIRST_SELECT) {
  if(fabs((double) lp->rhs[0] - prevobj) < epsel) {
    if(++Ncycle > 50)
      lp->piv_rule = FIRST_SELECT;
  }
  else {
    prevobj = (double) lp->rhs[0];
    Ncycle = 0;
  }
}
#endif

      lp->doIterate = FALSE;
      lp->doInvert = FALSE;

      if(!minit) {
	row_nr = rowdual(lp
#if defined antismallpivot
                         , skiprow
#endif
                        );
#if defined antismallpivot
	if((row_nr == 0) && (row_nr0 != 0)) {
	  row_nr = row_nr0;
	  skiprow = -1;
	}
#endif
      }

      if(row_nr > 0 ) {
	colnr = coldual(lp, row_nr, minit, prow, drow); /* What about BTRAN in here? */
	/* report(lp, NORMAL, "Dual-phase pivots (minit=%d) col=%d, row=%d", minit, colnr, row_nr); */

	if(colnr > 0) {
	  if (!setpivcol(lp, colnr, pcol)) {                   /* Solve FTRAN here */
	    ok = FALSE;
	    break;
	  }

#if defined antismallpivot
if ((!minit) && (skiprow != -1) && (fabs(pcol[row_nr])<1e-5)) {
 skiprow = row_nr;
 if((row_nr0 == 0) || (fabs(pcol[row_nr]) > skip0)) {
   skip0 = fabs(pcol[row_nr]);
   row_nr0 = skiprow;
 }
 /* continue; */
 lp->doIterate = FALSE;
}
else {
skiprow = row_nr0 = 0;
/*
if ((fabs(pcol[row_nr] - peter)>1e-9) && (fabs(pcol[row_nr] + peter)>1e-9))
 ok = ok;
*/
#endif
	 /* getting div by zero here. Catch it and try to recover */
	  if(pcol[row_nr] == 0) {
	    report(lp, IMPORTANT, "An attempt was made to divide by zero (pcol[%d])", row_nr);
	    lp->doIterate = FALSE;
	    if(!lp->justInverted) {
	      report(lp, IMPORTANT, "Trying to recover. Reinverting Eta");
	      lp->doInvert = TRUE;
	    }
	    else {
	      report(lp, IMPORTANT, "Failed to recover. Can't reinvert");
	      lp->spx_status = FAILURE;
	    }
	  }
	  else {
	    int bnx;

	    if (!condensecol(lp, row_nr, pcol)) {
	      ok = FALSE;
	      break;
	    }
	    bnx = lp->bas[row_nr];
	    f = lp->rhs[row_nr] - lp->upbo[bnx];

	    if(f > lp->epsd) {   /* *** Test of improved stability; was 0, changed by KE  20030619 */
	      theta = f / (RREAL) pcol[row_nr];
	      if(theta <= lp->upbo[colnr] + lp->epsb)
		lp->lower[bnx] = (MYBOOL) !lp->lower[bnx];
	    }
	    else /* f <= 0 */
	      theta = lp->rhs[row_nr] / (RREAL) pcol[row_nr];
	  }
#if defined antismallpivot
}
#endif
	}
	else
	  lp->spx_status = INFEASIBLE;
      }
      else {
	lp->spx_status = SWITCH_TO_PRIMAL;
	lp->doIterate = FALSE;
	lp->Extrad = 0;
	lp->doInvert = TRUE;
      }

      if(lp->doIterate) {
	iteration(lp, row_nr, colnr, &theta, lp->upbo[colnr], &minit,
		  &lp->lower[colnr], primal);
	if((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT))
	  break;
      }

      if(lp->num_inv > 0)
	f = (timenow()-lp->time_refactstart) / (REAL)lp->num_inv;
      else
	f = 0;
      if((lp->num_inv >= lp->max_num_inv) ||
	((lp->num_inv > 1) && (f >= MINTIMEPIVOT) && (f >= (*refacttime))))
	lp->doInvert = TRUE;

      if(lp->doInvert) {
	if(lp->print_at_invert)
	  report(lp, DETAILED, "Inverting: Primal = %d", primal);
	i = invert(lp);
	if((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY)) {
	  ok = FALSE;
	  break;
	}
	else if(!i) {
	  lp->spx_status = SINGULAR_BASIS;
	  break;
	}
      }
      else
	(*refacttime) = (REAL) f;

      if(yieldformessages(lp)!=0)
	lp->spx_status = USERABORT;

    }
  }

  FREE(drow);
  FREE(prow);
  FREE(pcol);

  return(ok);
}

static short solvelp(lprec *lp)
{
  int    singular_count, lost_feas_count;
  MYBOOL feasible;
  REAL	 refacttime;
  int    iter;

  lp->iter = 0;
  iter = 0;      /* Set to -1 to use once-through loop */

  refacttime = 0;
  singular_count = 0;
  lost_feas_count = 0;
  lp->spx_status = RUNNING;

  while(lp->spx_status == RUNNING) {

    if(yieldformessages(lp)!=0) {
      lp->spx_status = USERABORT;
      break;
    }

    /* Check whether we are feasible or infeasible. */
    if(iter >= 0)
      iter = lp->iter;
    feasible = IsDualFeasible(lp, lp->epsd);

   /* Now do the simplex magic */
    if(feasible) {
      if(lp->trace)
        report(lp, NORMAL, "Start at feasible basis");
      if (!primloop(lp, &refacttime))
        break;

      if((lp->spx_status == OPTIMAL) && (!IsDualFeasible(lp, lp->epsd)))
        lp->spx_status = RUNNING;

    }
    else {
      if(lp->trace) {
        if(lost_feas_count > 0)
          report(lp, NORMAL, "Continuing at infeasible basis");
        else
          report(lp, NORMAL, "Start at infeasible basis");
      }
      if (!dualloop(lp, &refacttime))
        break;
      if(lp->spx_status == SWITCH_TO_PRIMAL) {
        if (!primloop(lp, &refacttime))
	  break;

	if((lp->spx_status == OPTIMAL) && (!IsDualFeasible(lp, lp->epsd)))
          lp->spx_status = RUNNING;

      }
    }

    if(lp->spx_status == SINGULAR_BASIS) {
      singular_count++;
      if(singular_count >= DEF_MAXSINGULARITIES) {
        report(lp, IMPORTANT, "SINGULAR BASIS!  Too many singularities - aborting.");
        lp->spx_status = FAILURE;
        break;
      }
      report(lp, DETAILED, "SINGULAR BASIS!  Will attempt to recover.");
      lp->spx_status = RUNNING;
      /* Singular pivots are simply skipped by the inversion, leaving a row
         a row's slack var in the basis instead of the singular problem var
         This basis could be feasible or infeasible.  Check how to restart. */
    }
    else if((lp->spx_status == LOST_PRIMAL_FEASIBILITY) ||
            (((lp->spx_status == INFEASIBLE) || (lp->spx_status == FAILURE)) &&
             (iter > 0) && (iter == lp->iter))) {
      lost_feas_count++;
      if(lost_feas_count >= DEF_MAXSINGULARITIES) {
        report(lp, IMPORTANT, "LOST PRIMAL FEASIBILITY too many times, aborting.");
        lp->spx_status = FAILURE;
        break;
      }
      report(lp, DETAILED, "LOST PRIMAL FEASIBILITY!  Recovering.");
      lp->spx_status = RUNNING;
    }

  }

  lp->total_iter += lp->iter;

  return(lp->spx_status);
} /* solvelp */

static MYBOOL variable_is_fixed(lprec *lp, int i)
{
  return((MYBOOL) (lp->upbo[i]-lp->lowbo[i] < lp->epsel));
} /* variable_is_fixed */

static MYBOOL solution_is_int(lprec *lp, int i, MYBOOL checkfixed)
{
  REAL value;

  value = lp->solution[i];
  value = value - (REAL)floor((double)value);

  if((value < lp->epsilon) || (value > (1 - lp->epsilon))) {
    if(checkfixed)
      return(variable_is_fixed(lp, i));
    else
      return(TRUE);
  }

  return(FALSE);
} /* solution_is_int */

/* static */ void construct_solution(lprec *lp)
{
  int    i, j, basi;
  REAL   f;

  /* zero all results of rows */
  for (i = 1; i <= lp->rows; i++)
    lp->solution[i] = 0.0;
  lp->solution[0] = -lp->orig_rh[0];

  if(lp->scaling_used) {
    lp->solution[0] /= lp->scale[0];

    for(i = lp->rows + 1; i <= lp->sum; i++)
      lp->solution[i] = lp->lowbo[i] * lp->scale[i];

    for(i = 1; i <= lp->rows; i++) {
      basi = lp->bas[i];
      if(basi > lp->rows)
	lp->solution[basi] += (REAL) (lp->rhs[i] * lp->scale[basi]);
    }
    for(i = lp->rows + 1; i <= lp->sum; i++)
      if(!lp->basis[i] && !lp->lower[i])
	lp->solution[i] += lp->upbo[i] * lp->scale[i];

    for(j = 1; j <= lp->columns_used; j++) {
      f = lp->solution[lp->rows + j];
      if(f != 0)
	for(i = lp->col_end[j - 1]; i < lp->col_end[j]; i++) {
          basi = lp->mat[i].row_nr;
          lp->solution[basi] += (f / lp->scale[lp->rows+j])
	    * (lp->mat[i].value / lp->scale[basi]);
        }
    }
  }
  else { /* no scaling */
    for(i = lp->rows + 1; i <= lp->sum; i++)
      lp->solution[i] = lp->lowbo[i];

    for(i = 1; i <= lp->rows; i++) {
      basi = lp->bas[i];
      if(basi > lp->rows)
	lp->solution[basi] += (REAL) lp->rhs[i];
    }

    for(i = lp->rows + 1; i <= lp->sum; i++)
      if(!lp->basis[i] && !lp->lower[i])
	lp->solution[i] += lp->upbo[i];

    for(j = 1; j <= lp->columns_used; j++) {
      f = lp->solution[lp->rows + j];
      if(f != 0)
	for(i = lp->col_end[j - 1]; i < lp->col_end[j]; i++)
	  lp->solution[lp->mat[i].row_nr] += f * lp->mat[i].value;
    }

  }

  /* clean out near-zero slack values */
  for(i = 0; i <= lp->rows; i++) {
    if(my_abs(lp->solution[i]) < lp->epsb)
      lp->solution[i] = 0;
    else if(lp->ch_sign[i])
      lp->solution[i] = -lp->solution[i];
  }

 /* Record the best real-valued solution and compute a simple MIP solution limit */
  if(fabs(lp->real_solution) >= lp->infinite) {
    lp->real_solution = lp->solution[0];
    lp->limMIP_solution = lp->real_solution;
    if(lp->int_count>0)
      for(i = 1; i <= lp->columns; i++) {
        j = lp->rows+i;
        if(!is_int(lp, i) || solution_is_int(lp, j, FALSE)) continue;
        f = get_mat(lp, 0, i);
        if(f < 0)
          lp->limMIP_solution -= f*(ceil(lp->solution[j])-lp->solution[j]);
      }
  }

} /* construct_solution */

static void transfer_solution(lprec *lp)
{
  int i;

  MEMCPY(lp->best_solution, lp->solution, lp->sum + 1);

  /* round integer solution values to actual integers */
  if(lp->scalemode & INTEGERSCALE)
    for(i = 1; i <= lp->columns; i++)
      if(is_int(lp, i))
        lp->best_solution[lp->rows + i] = (REAL) floor(lp->best_solution[lp->rows + i] + 0.5);

}

static void calculate_duals(lprec *lp)
{
  int varnr, i, j;
  REAL scale0;
  LREAL f;

  if(!lp->eta_valid)
    return;

  /* initialize */
  lp->duals[0] = 1;
  for(i = 1; i <= lp->sum; i++)
    lp->duals[i] = 0;

  btran(lp, lp->duals, lp->epsel);

  for(i = 1; i <= lp->columns; i++) {
    varnr = lp->rows + i;
    if(!lp->basis[varnr])
#if FALSE
      if(/* (lp->upbo[varnr] > 0) && */ /* (my_abs(lp->solution[varnr]) > lp->epsel) */ TRUE )
#endif
      {
	f = 0;
	for(j = lp->col_end[i - 1]; j < lp->col_end[i]; j++)
	  f += (LREAL)lp->duals[lp->mat[j].row_nr] * (LREAL)lp->mat[j].value;
	if(lp->maximise)
	  f = -f;
	lp->duals[varnr] = (REAL)f;
      }
  }

  /* the dual values are the reduced costs of the slacks */
  /* When the slack is at its upper bound, change the sign. */
  for(i = 1; i <= lp->rows; i++) {
    if(lp->basis[i])
      lp->duals[i] = 0;
    /* added a test if variable is different from 0 because sometime you get
       -0 and this is different from 0 on for example INTEL processors (ie 0
       != -0 on INTEL !) peno */
    else if((lp->ch_sign[0] == lp->ch_sign[i]) && lp->duals[i])
      lp->duals[i] = - lp->duals[i];
  }

  if (lp->scaling_used)
    scale0 = lp->scale[0];
  else
    scale0 = 1;
  for(i = 1; i <= lp->sum; i++) {
    if(lp->scaling_used) {
      lp->duals[i] /= scale0;
      if(i <= lp->rows)
        lp->duals[i] *= lp->scale[i];
      else
        lp->duals[i] /= lp->scale[i];
    }
    my_round(lp->duals[i], lp->epsd);
  }
} /* calculate_duals */

/* calculate sensitivity duals */
static int calculate_sensitivity_duals(lprec *lp)
 {
  int k,varnr, ok = TRUE;
  REAL *pcol,a,infinite,epsel,from,till,objfromvalue;

  /* one column of the matrix */
  if (CALLOC(pcol, lp->rows + 1, REAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    ok = FALSE;
  }
  else {
    infinite=lp->infinite;
    epsel=lp->epsel;
    for(varnr=1;varnr<=lp->sum;varnr++) {
     from=infinite;
     till=infinite;
     objfromvalue=infinite;
     if ((!lp->basis[varnr]) /* && ((varnr<=lp->rows) || (my_abs(lp->solution[varnr])>epsel)) */) {
      if (!setpivcol(lp,varnr,pcol)) { /* construct one column of the tableau */
       ok = FALSE;
       break;
      }
      for (k=1;k<=lp->rows;k++)   /* search for the rows(s) which first results in further iterations */
       if (my_abs(pcol[k])>epsel) {
	a=(REAL) (lp->rhs[k]/pcol[k]);
	if ((a<=0.0) && (pcol[k]<0.0) && (-a<from)) from=-a;
	if ((a>=0.0) && (pcol[k]>0.0) && ( a<till)) till= a;
	if (lp->upbo[lp->bas[k]] < infinite) {
	 a=(REAL) ((lp->rhs[k]-lp->upbo[lp->bas[k]])/pcol[k]);
	 if ((a<=0.0) && (pcol[k]>0.0) && (-a<from)) from=-a;
	 if ((a>=0.0) && (pcol[k]<0.0) && ( a<till)) till= a;
	}
       }
      if (!lp->lower[varnr]) {
       a=from;
       from=till;
       till=a;
      }
      if ((varnr<=lp->rows) && (!lp->ch_sign[varnr])) {
       a=from;
       from=till;
       till=a;
      }

      if ((varnr > lp->rows) && (my_abs(lp->solution[varnr]) <= epsel)) {
       for (k=1;k<=lp->rows;k++)   /* search for the rows(s) which first results in further iterations */
        if (my_abs(pcol[k])>epsel) {
	 a=(REAL) (lp->rhs[k]/pcol[k]);
         if ((a>=0.0) && (a<objfromvalue)) {
          objfromvalue=a;
	 }
	 if (lp->upbo[lp->bas[k]] < infinite) {
	  a=(REAL) ((lp->rhs[k]-lp->upbo[lp->bas[k]])/pcol[k]);
          if ((a>=0.0) && (a<objfromvalue)) {
           objfromvalue=a;
	  }
	 }
	}
      }
     }

     if (from!=infinite) {
      if (lp->scaling_used) {
       if (varnr<=lp->rows)
        from/=lp->scale[varnr];
       else
        from*=lp->scale[varnr];
      }
      lp->dualsfrom[varnr]=lp->solution[varnr]-from;
     }
     else
      lp->dualsfrom[varnr]=-infinite;

     if (till!=infinite) {
      if (lp->scaling_used) {
       if (varnr<=lp->rows)
        till/=lp->scale[varnr];
       else
        till*=lp->scale[varnr];
      }
      lp->dualstill[varnr]=lp->solution[varnr]+till;
     }
     else
      lp->dualstill[varnr]=infinite;

     if (varnr > lp->rows) {
      if (objfromvalue!=infinite) {
       if (!lp->lower[varnr]) objfromvalue=lp->upbo[varnr]-objfromvalue;
       if ((lp->upbo[varnr] < infinite) && (objfromvalue>lp->upbo[varnr])) objfromvalue=lp->upbo[varnr];
       objfromvalue+=lp->lowbo[varnr];
       if (lp->scaling_used)
        objfromvalue*=lp->scale[varnr];
      }
      else
       objfromvalue=-infinite;
      lp->objfromvalue[varnr-lp->rows]=objfromvalue;
     }
    }

    free(pcol);
  }
  return(ok);
 } /* calculate_sensitivity_duals */

void setpivrow(lprec *lp,
 	       int   row_nr,
	       REAL *prow)
 {
  int i,j,k,r,*rowp,row,varnr;
  REAL f,*valuep,value;

  for(i = 0; i <= lp->rows; i++)
    prow[i] = 0;

  prow[row_nr] = 1;

  for(i = lp->eta_size; i >= 1; i--) {
    f = 0;
    k = lp->eta_col_end[i] - 1;
    r = lp->eta_row_nr[k];
    j = lp->eta_col_end[i - 1];

    /* this is one of the loops where the program consumes a lot of CPU
       time */
    /* let's help the compiler by doing some pointer arithmetic instead
       of array indexing */
    for(rowp = lp->eta_row_nr + j, valuep = lp->eta_value + j;
	j <= k;
	j++, rowp++, valuep++) {
      f += prow[*rowp] * *valuep;
    }

    my_round(f, lp->epsel);
    prow[r] = f;
  }

  for(i = 1; i <= lp->columns; i++) {
    varnr = lp->rows + i;
    if(!lp->basis[varnr]) {
      matrec *matentry;

      f = 0;
      k = lp->col_end[i];
      j = lp->col_end[i - 1];

      /* this is one of the loops where the program consumes a lot
	 of cpu time */
      /* let's help the compiler with pointer arithmetic instead
	 of array indexing */
      for(matentry = lp->mat + j;
	  j < k;
	  j++, matentry++) {
	row = (*matentry).row_nr;
	value = (*matentry).value;
	f += prow[row] * value;
      }

      my_round(f, lp->epsel);
      prow[varnr] = f;
    }
  }
} /* setpivrow */

/* calculate sensitivity objective function */
static int calculate_sensitivity_obj(lprec *lp)
 {
  int i,j,l,varnr,row_nr,ok = TRUE;
  REAL *OrigObj = NULL,*drow = NULL,*prow = NULL,f,a,min1,min2,infinite,epsel,from,till;

  /* objective function */
  if ((CALLOC(drow, lp->sum + 1, REAL) == NULL) ||
      (MALLOC(OrigObj, lp->columns + 1, REAL) == NULL) ||
      (CALLOC(prow,lp->sum + 1, REAL) == NULL)
     ) {
    lp->spx_status = OUT_OF_MEMORY;
    FREE(prow);
    FREE(OrigObj);
    FREE(drow);
    ok = FALSE;
  }
  else {
    for(i = 1; i <= lp->sum; i++)
      drow[i] = 0;
    drow[0] = 1;
    btran(lp, drow, lp->epsel);
    for(i = 1; i <= lp->columns; i++) {
      varnr = lp->rows + i;
      if(!lp->basis[varnr]) {
	f = 0;
	for(j = lp->col_end[i - 1]; j < lp->col_end[i]; j++)
	  f += drow[lp->mat[j].row_nr] * lp->mat[j].value;
	drow[varnr] = f;
      }
    }
    for(i = 1; i <= lp->sum; i++)
      my_round(drow[i], lp->epsd);

    /* original (unscaled) objective function */
    get_row(lp, 0, OrigObj);

    infinite=lp->infinite;
    epsel=lp->epsel;
    for(i = 1; i <= lp->columns; i++) {
     from=-infinite;
     till= infinite;
     varnr = lp->rows + i;
     if (!lp->basis[varnr]) {
      /* only the coeff of the objective function of column i changes. */
      a=drow[varnr];
      if (lp->scaling_used)
       a/=(lp->scale[varnr]*lp->scale[0]);
      if (lp->upbo[varnr]==0.0) /* ignore, because this case doesn't results in further iterations */ ;
      else if (lp->lower[varnr]) from=OrigObj[i]-a; /* less than this value gives further iterations */
      else till=OrigObj[i]-a; /* bigger than this value gives further iterations */
     }
     else {
      /* all the coeff of the objective function change. Search the minimal change needed for further iterations */
      for (row_nr=1;(row_nr<=lp->rows) && (lp->bas[row_nr]!=varnr);row_nr++); /* search on which row the variable exists in the basis */
      if (row_nr<=lp->rows) {     /* safety test; should always be found ... */
       setpivrow(lp,row_nr,prow); /* construct one row of the tableau */
       min1=infinite;
       min2=infinite;
       for (l=1;l<=lp->sum;l++)   /* search for the column(s) which first results in further iterations */
	if ((!lp->basis[l]) && (lp->upbo[l]>0.0) && (my_abs(prow[l])>epsel) && (drow[l]*(lp->lower[l] ? -1 : 1)<epsel)) {
	 a=my_abs(drow[l]/prow[l]);
	 if (lp->scaling_used)
	  a/=(lp->scale[varnr]*lp->scale[0]);
	 if (prow[l]*(lp->lower[l] ? 1 : -1)<0.0) {
	  if (a<min1) min1=a;
	 }
	 else {
	  if (a<min2) min2=a;
	 }
	}
       if (!lp->lower[varnr]) {
	a=min1;
	min1=min2;
	min2=a;
       }
       if (min1<infinite) from=OrigObj[i]-min1;
       if (min2<infinite) till=OrigObj[i]+min2;
       /* if (lp->solution[varnr]==0.0) till=0.0; */ /* if value is 0 then there can't be an upper range */
       if (lp->maximise) {
        if (lp->solution[varnr]-lp->lowbo[varnr]<lp->epsel)
	 from=infinite;
	else if (lp->lowbo[varnr]+lp->upbo[varnr]-lp->solution[varnr]<lp->epsel)
	 till=infinite;
       }
       else {
        if (lp->solution[varnr]-lp->lowbo[varnr]<lp->epsel)
	 till=infinite;
	else if (lp->lowbo[varnr]+lp->upbo[varnr]-lp->solution[varnr]<lp->epsel)
	 from=infinite;
       }
      }
     }
     lp->objfrom[i]=from;
     lp->objtill[i]=till;
    }

    free(prow);
    free(OrigObj);
    free(drow);
  }
  return(ok);
 } /* calculate_sensitivity_obj */

static MYBOOL check_if_less(lprec *lp,
			  REAL x,
			  REAL y,
			  REAL value)
{
  if(x >= y) {
    report(lp, DETAILED, "Error: new upper or lower bound is not more restrictive");
    report(lp, DETAILED, "bound 1: %g, bound 2: %g, value: %g",
  	    (double)x, (double)y, (double)value);
    return(FALSE);
  }
  return(TRUE);
}

#if defined CHECK_SOLUTION
static short check_solution(lprec *lp,
                          int  lastcolumn,
		          REAL *solution,
			  REAL *upbo,
			  REAL *lowbo)
{
  REAL  test, value;
  int i, n;

  report(lp, NORMAL, " ");
  report(lp, NORMAL, "lp_solve solution %g final at iteration %6d, %6d nodes explored\n",
         solution[0], lp->total_iter, lp->total_nodes);

/* Check if solution values are within the bounds; allowing a margin for numerical errors */
  n = 0;
  for(i = lp->rows + 1; i <= lp->rows+lastcolumn; i++) {
    test = lowbo[i];
    if(lp->columns_scaled)
      test *= lp->scale[i];

    if((solution[i] < test-SOLUTIONEPS*(1+fabs(test))) &&
	   !(lp->var_is_sc[i - lp->rows] > 0))  {
      report(lp, NORMAL,
              "Error: variable %s has a solution (%g) smaller than its lower bound (%g)",
              get_col_name(lp, i-lp->rows), (double)solution[i], (double)test);
      n++;
    }

    test = upbo[i];
    if(lp->columns_scaled)
      test *= lp->scale[i];

    if(solution[i] > test+SOLUTIONEPS*(1+fabs(test))) {
      report(lp, NORMAL,
              "Error: variable %s has a solution (%g) larger than its upper bound (%g)",
              get_col_name(lp, i-lp->rows), (double)solution[i], (double)test);
      n++;
    }
	if(n >= 20)
	  break;
  }

/* Check if constraint values are within the bounds; allowing a margin for numerical errors */
  for(i = 1; i <= lp->rows; i++) {

    value = solution[i];

    test = lp->orig_rh[i];
    if(lp->ch_sign[i]) {
      test = -test;
      test += fabs(upbo[i]);
    }
    if(lp->scaling_used)
      test /= lp->scale[i];

    if(value > test+SOLUTIONEPS*(1+fabs(test))) {
      report(lp, NORMAL,
              "Error: constraint %s has a value (%g) larger than its upper bound (%g)",
              get_row_name(lp, i), (double)value, (double)test);
      n++;
    }

    if(lp->ch_sign[i]) {
      test = lp->orig_rh[i];
	  test = -test;
    }
    else
      test = lp->orig_rh[i]-fabs(upbo[i]);
    if(lp->scaling_used)
      test /= lp->scale[i];

    if(value < test-SOLUTIONEPS*(1+fabs(test))) {
      report(lp, NORMAL,
              "Error: constraint %s has a value (%g) smaller than its lower bound (%g)",
              get_row_name(lp, i), (double)value, (double)test);
      n++;
    }

    if(n >= 20)
      break;
  }

  if(n > 0)
    return(FAILURE);
  else
    return(OPTIMAL);

} /* check_solution */
#endif /* CHECK_SOLUTION */

/* set working lower bounds to zero and transform rh correspondingly */
static void basetozero(lprec *lp)
{
  int i, j;
  LREAL theta;

  for(i = 1; i <= lp->columns; i++) {
    j = lp->rows + i;
    theta = lp->lowbo[j];
    if(theta != 0) {
      if(lp->upbo[j] < lp->infinite)
        lp->upbo[j] = (REAL) (lp->upbo[j] - theta);
      for(j = lp->col_end[i - 1]; j < lp->col_end[i]; j++)
        lp->rh[lp->mat[j].row_nr] = (REAL) (lp->rh[lp->mat[j].row_nr] - theta * lp->mat[j].value);
    }
  }
  /* Added by KE */
/*  for(i = 1; i <= lp->rows; i++)
    my_round(lp->rh[i], lp->epsel); */
}

static REAL int_floor(lprec *lp, int column, REAL value, REAL epsscale)
{
  value = (REAL) floor(value);
  if(lp->columns_scaled && (lp->scalemode & INTEGERSCALE)) {
    value /= lp->scale[column];
    if(epsscale)
      value += epsscale*lp->epsel;
    /* value = my_precision(value, lp->epsb); */
  }
  return(value);
}

static REAL int_ceil(lprec *lp, int column, REAL value, REAL epsscale)
{
  value = (REAL) ceil(value);
  if(lp->columns_scaled && (lp->scalemode & INTEGERSCALE)) {
    value /= lp->scale[column];
    if(epsscale)
      value -= epsscale*lp->epsel;
    /* value = my_precision(value, lp->epsb); */
  }
  return(value);
}

static REAL scaled_value(lprec *lp, REAL value, int index)
{
  if(lp->scaling_used)
      value /= lp->scale[index];
  return(value);
}

static REAL unscaled_value(lprec *lp, REAL value, int index)
{
  if(lp->scaling_used)
      value *= lp->scale[index];
  return(value);
}

static MYBOOL SOS_can_activate(lprec *lp, int sosindex, int column)
{
  int i, n, nn;
  int *list;

  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_can_activate: Invalid SOS index %d", sosindex);
    return(FALSE);
  }

  if(!(lp->must_be_int[column] & ISSOS))
    return(FALSE);

  if(sosindex == 0 && lp->sos_count == 1)
    sosindex = 1;

  if(sosindex == 0) {
    for(i = 1; i<=lp->sos_count; i++) {
      nn = SOS_can_activate(lp, i, column);
      if(nn == FALSE)
        return(FALSE);
    }
  }
  else if(SOS_is_member(lp, sosindex, column)) {

    list = lp->sos_list[sosindex-1]->members;
    n = list[0]+1;
    nn = list[n];

   /* Accept if the SOS is empty */
    if(list[n+1] == 0)
      return(TRUE);

   /* Cannot activate a variable if the SOS is full */
    if(list[n+nn] != 0)
      return(FALSE);

   /* Check if we can set variable active in SOS2..SOSn
     (must check left and right neighbours if one variable is already active) */
    if(nn > 1) {

     /* Find the variable that was last activated;
        Also check that the candidate variable is not already active */
      for(i = 1; i <= nn; i++) {
        if(list[n+i] == 0)
          break;
        if(list[n+i] == column)
          return(FALSE);
      }
      i--;
      nn = list[n+i];

     /* SOS accepts an additional variable; confirm neighbourness of candidate;
        Search for the SOS set index of the last activated variable */
      n = list[0];
      for(i = 1; i <= n; i++)
        if(abs(list[i]) == nn) break;
      if(i > n) {
        report(lp, CRITICAL, "Internal index error at SOS %d in SOS_can_activate", sosindex);
        return(FALSE);
      }

     /* SOS accepts an additional variable; confirm neighbourness of candidate */

     /* Check left neighbour */
      if((i > 1) && (list[i-1] == column))
        return(TRUE);
     /* Check right neighbour */
      if((i < n) && (list[i+1] == column))
        return(TRUE);

     /* It is not the right neighbour; return false */
      return(FALSE);
    }
  }
  return(TRUE);
}

static short branch_and_bound(lprec *lp, REAL *upbo, REAL *lowbo, int notint, MYBOOL varcode, MYBOOL prunemode)
{
  REAL   *new_upbo, *new_lowbo;
  REAL    lastsolution, new_bound, sc_bound;
  MYBOOL   *new_lower, *new_basis, IsSOS, floor_first;
  int    *new_bas;
  int     ii, k, MILPCount;
  short   spx_saved, failure = RUNNING, resone = RUNNING, restwo = RUNNING;

  spx_saved = lp->spx_status;
  lp->spx_status = RUNNING;
  if(yieldformessages(lp)!=0)
    lp->spx_status = USERABORT;

  if((lp->usermessage != NULL) && (lp->msgmask & MSG_MILPSTRATEGY))
    lp->usermessage(lp, lp->msghandle, MSG_MILPSTRATEGY);

  if(lp->spx_status != RUNNING)
    return(lp->spx_status);
  lp->spx_status = spx_saved;

 /* allocate room for them */
  new_upbo = new_lowbo = NULL;
  new_lower = new_basis = NULL;
  new_bas = NULL;
  if ((MALLOCCPY(new_upbo, upbo, lp->sum + 1, REAL) == NULL) ||
      (MALLOCCPY(new_lowbo, lowbo, lp->sum + 1, REAL) == NULL) ||
      (MALLOCCPY(new_lower, lp->lower, lp->sum + 1, MYBOOL) == NULL) ||
      (MALLOCCPY(new_basis, lp->basis, lp->sum + 1, MYBOOL) == NULL) ||
      (MALLOCCPY(new_bas, lp->bas, lp->rows + 1, int) == NULL)) {
    FREE(new_bas);
    FREE(new_basis);
    FREE(new_lower);
    FREE(new_lowbo);
    FREE(new_upbo);
    return(OUT_OF_MEMORY);
  }

 /* Initialize the MILP status code pair and set reference values */
  resone = INFEASIBLE;
  restwo = INFEASIBLE;
  k = notint - lp->rows;
  lastsolution = lp->solution[notint];
  IsSOS = (MYBOOL)((lp->sos_count > 0) && (SOS_is_member(lp, 0, k)));

 /* Set default number of runs through the MILP tree */
  if(prunemode == BRANCH_DEFAULT)
    MILPCount = 2;
  else
    MILPCount = 1;

 /* set local pruning info, automatic, or user-defined strategy */
  if((prunemode == BRANCH_FLOOR) || (prunemode == BRANCH_CEILING)) {
    floor_first = (MYBOOL)(prunemode == BRANCH_FLOOR);
  }
  else if(varcode == BB_SOS) {
    if(!SOS_can_activate(lp, 0, k)) {
      MILPCount--;
      floor_first = TRUE;
    }
    else
      floor_first = (MYBOOL)(lastsolution == 0);
  }
  else if(get_var_branch(lp, k) == BRANCH_AUTOMATIC) {
    new_bound = lastsolution - (REAL) floor((double)lastsolution);
    floor_first = (MYBOOL)(new_bound <= 0.5);
    if((lp->bb_rule != BEST_SELECT) && (lp->bb_rule != GREEDY_SELECT))
      floor_first = (MYBOOL) !floor_first;
  }
  else
    floor_first = (MYBOOL)(get_var_branch(lp, k) == BRANCH_FLOOR);

 /* SC logic: If the current SC variable value is in the [0..NZLOBOUND> range, then

	      UP: Set lower bound to NZLOBOUND, upper bound is the original
		  LO: Fix the variable by setting upper/lower bound to zero

    ... indicate that the variable is B&B-active by reversing sign of var_is_sc[]. */
  sc_bound = unscaled_value(lp, lp->var_is_sc[k], notint);

 /* Must make sure that we handle fractional lower bounds properly;
    also to ensure that we do a full binary tree search */
  if(is_int(lp,k) && ((sc_bound > 0) &&
                      (lastsolution > floor(sc_bound)))) {
    if(lastsolution < ceil(sc_bound))
      lastsolution += 1;
    lowbo[notint] = int_floor(lp, notint, lastsolution, 1);
  }

 /* This is the B&B dispatching control */
  while ((MILPCount > 0) && (!lp->Break_bb)) {
    if(floor_first) {
      /* BRANCH_FLOOR: Lock the variable at its local lower bound */

      /* First establish the lower bound to be applied, according to variable type */

      if((sc_bound > 0) && (lastsolution < sc_bound)) {     /* Handle SC-variables for the [0-LoBound> range */
        new_bound = 0;
      }
      else if(varcode == BB_INT) {                          /* Handle pure integers (non-SOS, non-SC) */
        if((floor(lastsolution) == lastsolution) ||
           (scaled_value(lp, (REAL) floor(lastsolution), notint) < /* Skip cases where the lower bound becomes violated */
            lp->orig_lowbo[notint]-lp->epsel)) {
          floor_first = FALSE;
	  new_bound = 0.0;
	}
        else
          new_bound = int_floor(lp, notint, lastsolution, 1);
      }
      else if(IsSOS) {                                      /* Handle all SOS variants */
        new_bound = lp->orig_lowbo[notint];
        if(is_int(lp, k))
	  new_bound = int_ceil(lp, notint, scaled_value(lp, new_bound, notint), -1);
      }
      else                                                  /* Handle all other variable incarnations */
        new_bound = scaled_value(lp, sc_bound, notint);

      if(floor_first) {
        /* Then check if the new bound might conflict */
        if(new_bound < lowbo[notint]) {
          debug_print(lp,
              "New upper bound value %g conflicts with old lower bound %g",
              (double)new_bound, (double)lowbo[notint]);
          resone = MILP_FAIL;
        }
        else { /* bound feasible */
          REAL *new_upbo2, *new_lowbo2;
#         if defined NEWBB
	    REAL old_bound;
#         endif

          if(lp->debug)  /* Added by KE */
            check_if_less(lp, new_bound, upbo[notint], lp->solution[notint]);
/* printf("var %d (%s), new upper bound %f\n",notint,get_col_name(lp,notint-lp->rows),new_bound); */
#         if defined NEWBB
            old_bound = upbo[notint];
	    if(old_bound < lp->infinite)
	      old_bound += lowbo[notint];
            set_upbo2(lp, notint - lp->rows, new_bound);
	    new_upbo2 = NULL;
	    new_lowbo2 = NULL;
#         else
            new_upbo[notint] = new_bound;
  	    lp->eta_valid = FALSE;
	    new_upbo2 = new_upbo;
	    new_lowbo2 = lowbo;
#         endif
          debug_print(lp, "starting floor subproblem with bounds:");
          debug_print_bounds(lp, new_upbo2, lowbo);

          lp->var_is_sc[k] *= -1;

          if(IsSOS && (varcode != BB_INT)) {
            SOS_set_marked(lp, 0, k, (MYBOOL) (new_bound!=0));
              resone = milpsolve(lp, new_upbo2, new_lowbo2, new_basis, new_lower, new_bas,
#if defined NEWBB
		                 FALSE
#else
		                 TRUE
#endif
				);
            SOS_unmark(lp, 0, k);
          }
          else
            resone = milpsolve(lp, new_upbo2, new_lowbo2, new_basis, new_lower, new_bas,
#if defined NEWBB
		                 FALSE
#else
		                 TRUE
#endif
			      );
#         if defined NEWBB
            set_upbo2(lp, notint - lp->rows, old_bound);
#         else
            lp->eta_valid = FALSE;
#         endif
          lp->var_is_sc[k] *= -1;
        }
        if((resone == USERABORT) || (resone == TIMEOUT) || (resone == OUT_OF_MEMORY)) {
          failure = resone;
          break;
        }
      }
      MILPCount--;
      floor_first = FALSE;
    }
    else {
      /* BRANCH_CEILING: Lock the variable at its local upper bound */

      /* First establish the upper bound to be applied, according to variable type */
      floor_first = TRUE;

      if((sc_bound > 0) && (lastsolution < sc_bound)) {     /* Handle SC-variables for the [0-LoBound> range */
        new_bound = scaled_value(lp, sc_bound, notint);
        if(is_int(lp, k))
          new_bound = int_ceil(lp, notint, new_bound, 1);
      }
      else if(varcode == BB_INT) {                          /* Handle pure integers (non-SOS, non-SC) */
        if(((ceil(lastsolution) == lastsolution)) ||        /* Skip branch 0 if the current solution is integer */
            (scaled_value(lp, (REAL) ceil(lastsolution), notint) > /* Skip cases where the upper bound becomes violated */
             lp->orig_upbo[notint]+lp->epsel) ||
            (IsSOS && (lastsolution == 0))) {               /* Don't branch 0 since this is handled in SOS logic */
          floor_first = FALSE;
	  new_bound = 0.0;
	}
        else
          new_bound = int_ceil(lp, notint, lastsolution, 1);
      }
      else if(IsSOS) {                                      /* Handle all SOS variants */
        if(SOS_is_member_of_type(lp, k, SOS3))
          new_bound = 1;
        else {
          new_bound = lp->orig_lowbo[notint];
          if(is_int(lp, k))
	    new_bound = int_floor(lp, notint, scaled_value(lp, new_bound, notint), 1);
        }
      }
      else                                                  /* Handle all other variable incarnations */
        new_bound = scaled_value(lp, sc_bound, notint);

      if (floor_first) {
        /* Then check if the new bound might conflict */
        if(new_bound > upbo[notint]) {
          debug_print(lp,
              "New lower bound value %g conflicts with old upper bound %g",
              (double)new_bound, (double)upbo[notint]);
          restwo = MILP_FAIL;
        }
        else { /* bound feasible */
	  REAL *new_lowbo2, *new_upbo2, *new_upbo3;
#         if defined NEWBB
	    REAL old_bound;
#         endif

          if(lp->debug)  /* Added by KE */
            check_if_less(lp, lowbo[notint], new_bound, lp->solution[notint]);
/* printf("var %d (%s), new lower bound %f\n",notint,get_col_name(lp,notint-lp->rows),new_bound); */
#         if defined NEWBB
            old_bound = lowbo[notint];
            set_lowbo2(lp, notint - lp->rows, new_bound);
	    new_lowbo2 = NULL;
	    new_upbo2 = NULL;
            new_upbo3 = NULL;
#         else
            new_lowbo[notint] = new_bound;
	    lp->eta_valid = FALSE;
	    new_lowbo2 = new_lowbo;
	    new_upbo2 = new_upbo;
            new_upbo3 = upbo;
#         endif
          debug_print(lp, "starting ceiling subproblem with bounds:");
          debug_print_bounds(lp, upbo, new_lowbo2);

  	  lp->var_is_sc[k] *= -1;

          if(IsSOS && (varcode != BB_INT)) {
	    SOS_set_marked(lp, 0, k, TRUE);
	    ii = 0;  /* Difference count in relation to last optimal solution */
            if(SOS_fix_unmarked(lp, k, 0, new_upbo2, 0, TRUE, &ii) >= 0) {
              restwo = milpsolve(lp, new_upbo2, new_lowbo2, new_basis, new_lower, new_bas,
#if defined NEWBB
		                 FALSE
#else
		                 TRUE
#endif
				);
              if((MILPCount == 2) && IsSOS && (lp->orig_lowbo[notint] != 0))
                MILPCount--;
            }
#           if !defined NEWBB
              MEMCPY(new_upbo, upbo, lp->sum + 1);
#           endif
            SOS_unmark(lp, 0, k);
          }
          else
            restwo = milpsolve(lp, new_upbo3, new_lowbo2, new_basis, new_lower, new_bas,
#if defined NEWBB
		                 FALSE
#else
		                 TRUE
#endif
			      );
#         if defined NEWBB
            set_lowbo2(lp, notint - lp->rows, old_bound);
#         else
            lp->eta_valid = FALSE;
#         endif
          lp->var_is_sc[k] *= -1;
        }
      }
      if((restwo == USERABORT) || (restwo == TIMEOUT) || (restwo == OUT_OF_MEMORY)) {
        failure = restwo;
        break;
      }
      MILPCount--;
      floor_first = TRUE;
    }
  }

  if ((MILPCount <= 0) || (lp->Break_bb)) {
    /* Check for user abort or timout (resone was tested earlier) */
    if((resone != OPTIMAL) && (restwo != OPTIMAL)) /* both failed and must have been infeasible */
      failure = INFEASIBLE;
    else
      failure = OPTIMAL;
  }

  free(new_upbo);
  free(new_lowbo);
  free(new_basis);
  free(new_lower);
  free(new_bas);
  return(failure);
}

static MYBOOL SOS_is_marked(lprec *lp, int sosindex, int column)
{
  int i, n;
  int *list;

  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_is_marked: Invalid SOS index %d", sosindex);
    return(FALSE);
  }
  if(!(lp->must_be_int[column] & ISSOS))
    return(FALSE);

  if((sosindex == 0) && (lp->sos_count == 1))
    sosindex = 1;

  if(sosindex == 0) {
    for(i = 1; i<=lp->sos_count; i++) {
      n = SOS_is_marked(lp, i, column);
      if(n) return(TRUE);
    }
  }
  else
  {
    list = lp->sos_list[sosindex-1]->members;
    n = list[0];

   /* Search for the variable (normally always faster to do linear search here) */
    column = -column;
    for(i = 1; i <= n; i++)
      if(list[i] == column)
        return(TRUE);
  }
  return(FALSE);
}

static int find_sc_bbvar(lprec *lp, int *count)
{
  int i, j, var;
  REAL tmpreal;

  var = 0;
  if((lp->sc_count == 0) || (*count > 0))
    return(var);

  for(i = 1; i <= lp->columns; i++) {
    j = lp->rows+i;
    tmpreal = lp->var_is_sc[i];
    if(lp->scaling_used)
      tmpreal *= lp->scale[j];

    if((tmpreal > 0) &&              /* it is an (inactive) SC variable...    */
       (lp->solution[j] < tmpreal)   /* ...and the NZ lower bound is violated */
      )
      {
      if(lp->sos_count == 0 || !SOS_is_marked(lp, 0, i)) {
        if((*count) == 0)  /* Pick up the index of the first non-SC */
          var = j;
        (*count)++;
      }
    }
  }
  return(var);
}

static int find_sos_bbvar(lprec *lp, int *count, MYBOOL intsos)
{
  int k, i, j, var;

  var = 0;
  if((lp->sos_count == 0) || (*count > 0))
    return(var);

  /* Check if SOS'es happen to already be satisified */
  i = SOS_is_satisfied(lp, 0, lp->solution);
  if(i == 0 || i == -1)
    return(-1);

  /* Otherwise identify a SOS variable to enter B&B */
  for(k = 0; k < lp->sos_vars; k++) {
    i = lp->sos_priority[k];
    j = lp->rows + i;
    if(!SOS_is_marked(lp, 0, i) && !SOS_is_full(lp, 0, i, FALSE)) {
      if(!intsos || is_int(lp, i)) {
        (*count)++;
        if(var == 0) {
          var = j;
          break;
        }
      }
    }
  }
  return(var);
}

int get_varpriority(lprec *lp, int column)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "get_varpriority: Column %d out of range", column);
    return(FALSE);
  }
  if(lp->var_priority == NULL)
    return(column);
  else
    return(lp->var_priority[column - 1]);
}

static int find_int_bbvar(lprec *lp, int *count, REAL *lowbo, REAL*upbo, int *firstint, int *lastint)
{
  int i, ii, n, var;

  var = 0;
  if((lp->int_count == 0) || (*count > 0))
    return(var);

  for(n = 1; n <= lp->columns; n++) {
    ii = get_varpriority(lp, n);
    i = ii + lp->rows;
    if(is_int(lp,ii) && !solution_is_int(lp, i, FALSE)) {
      if(lowbo[i] == upbo[i]) { /* this var is already fixed */
        report(lp, IMPORTANT,
               "Warning: INT var %d is already fixed at %d, but has non-INT value %g",
                i - lp->rows, (int)lowbo[i], (double)lp->solution[i]);
        continue;
      }

      if((*count) == 0) { /* Pick up the index of the first non-integer INT */
        var = i;
        (*firstint) = n;
      }
      (*count)++;
      (*lastint) = n;     /* Pick up index of the last non-integer INT */
    }
  }
  return(var);
}

static short milpsolve(lprec *lp,
             REAL   *upbo,
             REAL   *lowbo,
             MYBOOL   *sbasis,
             MYBOOL   *slower,
             int    *sbas,
             int     recursive)
{
  int i, j, tilted;
  MYBOOL varcode, is_better, is_equal;
  int  firstint, lastint;
  int milpitems, notint, nr_not_int;
  short failure;
  REAL tmpreal;
  MYBOOL redo;

  if(lp->Break_bb)
    return(BREAK_BB);

  lp->Level++;
  lp->total_nodes++;

  if(lp->Level > lp->max_level)
    lp->max_level = lp->Level;

  debug_print(lp, "starting milpsolve");

  /* make fresh copies of upbo, lowbo, rh as solving changes them */
  /* Converted to macro by KE */
  if(upbo != NULL)
    MEMCPY(lp->rh,    lp->orig_rh, lp->rows + 1);

  if(upbo != NULL) {
    MEMCPY(lp->upbo,  upbo,        lp->sum + 1);
  }
  else
    upbo = lp->upbo;
  if(lowbo != NULL) {
    MEMCPY(lp->lowbo, lowbo,       lp->sum + 1);
  }
  else
    lowbo = lp->lowbo;

  /* make sure we do not do memcpy(lp->basis, lp->basis ...) ! */
  /* Converted to macro by KE */
  if(recursive) {
    MEMCPY(lp->basis, sbasis, lp->sum + 1);
    MEMCPY(lp->lower, slower, lp->sum + 1);
    MEMCPY(lp->bas,   sbas,   lp->rows + 1);
  }

  failure = RUNNING;
  tilted = 0;

  do {
    if(lp->anti_degen>=2 ||
      (lp->anti_degen==1 && failure==INFEASIBLE))  /* randomly disturb (relax) bounds */
    {
      for(i = 1; i <= lp->columns; i++) {
        j = (rand() % RANDSCALE) + 1;   /* Added 1 by KE */
        tmpreal = ((REAL) j * lp->epsperturb);
        if(tmpreal > lp->epsb)
          lp->lowbo[i + lp->rows] -= tmpreal;
        if(lp->upbo[i + lp->rows] < lp->infinite) {
          j = (rand() % RANDSCALE) + 1;   /* Added 1 by KE */
          tmpreal = ((REAL) j * lp->epsperturb);
          if(tmpreal > lp->epsb)
            lp->upbo[i + lp->rows] += tmpreal;
        }
      }
      lp->eta_valid = FALSE;
      tilted++;
    }

    if(!lp->eta_valid) {
/*
static int n0=0;
static double t0=0.0;
double t;
*/
     /* set lower bounds to zero and transform rh correspondingly */
      basetozero(lp);  /* Moved to procedure by KE */
/* t=timenow(); */
      i = invert(lp);
/*
t=timenow() - t;
t0+=t;
n0++;
printf("invert: %d, time: %f (%f, %f),total_nodes: %d\n", n0,t,t0,t0/n0,lp->total_nodes);
*/
      if((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY))
        failure = lp->spx_status;
      else
        lp->eta_valid = TRUE;
    }

    if(!((failure == USERABORT) || (failure == TIMEOUT) || (failure == OUT_OF_MEMORY))) {
      failure = solvelp(lp);

      if(tilted && failure == OPTIMAL) {
        /* restore to original problem, solve again starting
           from the basis found for the perturbed problem */

        /* restore original problem */  /* Converted to macro by KE */
        MEMCPY(lp->upbo,  upbo,        lp->sum + 1);
        MEMCPY(lp->lowbo, lowbo,       lp->sum + 1);
        MEMCPY(lp->rh,    lp->orig_rh, lp->rows + 1);

        /* transform all to lower bound zero */
        basetozero(lp);  /* Moved to procedure by KE */
        i = invert(lp);
        if((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY))
          failure = lp->spx_status;
        else {
	  lp->eta_valid = TRUE;
          failure = solvelp(lp); /* and solve again */
	}
      }
    }
    redo = FALSE;
    if(failure == INFEASIBLE) {
      report(lp, DETAILED, "Level %d INFEASIBILITY", lp->Level);

      /* Allow up to .. consecutive relaxations for non-B&B phases */
      if(lp->anti_degen && (tilted<=DEF_MAXRELAX) && !recursive)
        redo = TRUE;
      else if(lp->anti_degen)
        report(lp, DETAILED, "Maximum number of relaxations exceeded in resolving infeasibility.");
    }
  } while (redo);

  if(!((failure == USERABORT) || (failure == TIMEOUT) || (failure == OUT_OF_MEMORY))) {

    if(failure != OPTIMAL) {

      if(failure == USERABORT)
        report(lp, NORMAL, "lp_solve stopped by user");
      else if(failure == TIMEOUT)
        report(lp, NORMAL, "lp_solve timed out");
      else if(!recursive)
        report(lp, NORMAL, "The problem is %s",
          (failure == UNBOUNDED) ? "unbounded" : "infeasible");
    }
    else { /* there is a good solution */
      construct_solution(lp);

#if defined CHECK_SOLUTION
      /* because of reports of solution > upbo */
      /* check_solution(lp, lp->columns, lp->solution, upbo, lowbo); */ /* get too many hits ?? */
#endif

      debug_print(lp, "A solution was found");
      debug_print_solution(lp);

      milpitems = lp->int_count+lp->sos_count+lp->sc_count;
      if((lp->total_nodes <= 1) && (milpitems > 0)) {
	report(lp, NORMAL, "Feasible solution %g found as real-valued MILP basis", (double)lp->solution[0]);
	report(lp, NORMAL, " ");
	if((lp->usermessage != NULL) && (lp->msgmask & MSG_LPFEASIBLE))
	    lp->usermessage(lp, lp->msghandle, MSG_LPFEASIBLE);
      }

      /* if this solution is worse than the best so far, this branch must die */

      /* if we can only have integer OF values, we might consider requiring
         next OF be at least 1 better than the best so far, MB */

      if(lp->maximise)
        is_better = (MYBOOL)(lp->solution[0] > lp->best_solution[0] + lp->mip_gap);
      else /* minimising! */
        is_better = (MYBOOL)(lp->solution[0] < lp->best_solution[0] - lp->mip_gap);
      is_equal = (MYBOOL)(!is_better && (fabs(lp->solution[0] - lp->best_solution[0]) <= lp->epsel));

      if((milpitems > 0) && !is_better && !is_equal) {
        report(lp, DETAILED, "Level %d OPT NOB value %g bound %g",
                lp->Level, (double)lp->solution[0], (double)lp->best_solution[0]);
        report(lp, DETAILED, "... but it was worse than the best so far; discarded!");
        lp->Level--;
        return(MILP_FAIL);
      }

      /* check if solution contains enough INT and SC variables */
      varcode = BB_INT;
      notint = 0;
      nr_not_int = 0;
      firstint = 1;
      lastint = lp->columns;

      if(milpitems) {
        /* Collect violated SC variables (since they can also be real-valued);
	   the approach is to get them out of the way, since a 0-value is "cheap" */

	if(lp->sc_count > 0) {
          notint = find_sc_bbvar(lp, &nr_not_int);
          if(notint > 0)
            varcode = BB_SC;
        }

        j = 0;

        if(notint == 0) {
          /* Look among SOS variables if no SC candidate was found */
          if((lp->sos_count > 0) && (nr_not_int == 0)) {
            notint = find_sos_bbvar(lp, &nr_not_int, FALSE);
            if(notint < 0)
              notint = 0;
            if(notint > 0)
              varcode = BB_SOS;
          }

          if (notint == 0) {
            /* Then collect INTS that are not integer valued, and verify bounds */
            if((lp->int_count > 0) && (nr_not_int == 0)) {
              notint = find_int_bbvar(lp, &nr_not_int, lowbo, upbo, &firstint, &lastint);
            }

            /* Check if we need bother with extended strategies */
            if(nr_not_int > 1) {
              /* Then apply strategy (FIRST_SELECT is implicitly the default from above) */
              if(lp->bb_rule == RAND_SELECT) {

                i = rand() % nr_not_int;
                while(i > 0) {
                  notint++;
                  if(is_int(lp,notint - lp->rows) && !solution_is_int(lp, notint, FALSE))
                    i--;
                }
              }
              else if(lp->bb_rule == GREEDY_SELECT) {
                double saveInt, testInt;

                saveInt = lp->infinite;
                for(j = firstint; j <= lastint; j++) {
                  i = get_varpriority(lp, j);
                  if(is_int(lp,i) && !solution_is_int(lp, i+lp->rows, FALSE)) {
                    testInt = get_mat(lp, 0, i);
                    if(lp->maximise)
                      testInt = -testInt;
                    if(testInt<saveInt) {
                      notint = i+lp->rows;
                      saveInt = testInt;
                    }
                  }
                }
              }
              else if((lp->bb_rule == WORST_SELECT) ||
                      (lp->bb_rule == BEST_SELECT) ||
                      (lp->bb_rule == MEDIAN_SELECT)) { /* Rules added by KE */
                double saveInt = 0, testInt, testTmp;

                if(lp->bb_rule == WORST_SELECT)
                  saveInt = 1;
                else if(lp->bb_rule == BEST_SELECT)
                  saveInt = 0;
                else /* if(lp->bb_rule == MEDIAN_SELECT) */
                  nr_not_int = nr_not_int / 2;

                for(j = firstint; j <= lastint; j++) {
                  i = get_varpriority(lp, j);
                  if(is_int(lp,i) && !solution_is_int(lp, i+lp->rows, FALSE)) {
                    testInt = modf(lp->solution[i+lp->rows], &testTmp);
                    if(testInt>0.5) testInt = testInt-0.5;
                    if(lp->bb_rule == BEST_SELECT) {
                      if(testInt>saveInt) {
                      notint = i+lp->rows;
                      saveInt = testInt;
                      }
                    }
                    else if(testInt<saveInt) {
                      notint = i+lp->rows;
                      saveInt = testInt;
                      if(lp->bb_rule == MEDIAN_SELECT) {
                        nr_not_int--;
                        if(nr_not_int <= 0)
                          break;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }

      report(lp, DETAILED, "Level %d OPT %s value %g", lp->Level,
                           (notint) ? "   " : "INT",(double)lp->solution[0]);

     /* If there is at least one value not yet int/sc/sos, then ... */
      if(nr_not_int) {

        debug_print(lp, "Unsatisfied truncated variables; Selecting var %s, val: %g",
                    get_col_name(lp, notint - lp->rows),
                    (double)lp->solution[notint]);
        debug_print(lp, "Current bounds:");
        debug_print_bounds(lp, upbo, lowbo);

        /* Now do the MIP branching with SOS pruning, if called for */
        failure = branch_and_bound(lp, upbo, lowbo, notint, varcode, BRANCH_DEFAULT);

      }
      else { /* all required values are int/sc/SOS */
        debug_print(lp, "--> valid solution found");

        if(is_equal) {
          if((lp->solutionlimit <= 0) || (lp->solutioncount < lp->solutionlimit)) {
            lp->solutioncount++;
            transfer_solution(lp);
            calculate_duals(lp);
	    if ((!calculate_sensitivity_duals(lp)) || (!calculate_sensitivity_obj(lp)))
	      failure = OUT_OF_MEMORY;
	    else
	      if((lp->trace) && (lp->print_sol)) {
	        print_objective(lp);
		print_solution(lp);
	      }
            if((lp->usermessage != NULL) && (lp->msgmask & MSG_MILPEQUAL)) {
              lp->usermessage(lp, lp->msghandle, MSG_MILPEQUAL);
            }
          }
        }
        else if(is_better) { /* Current solution better */
          lp->solutioncount = 1;
          if(lp->debug || ((lp->verbose>=NORMAL) && !lp->print_sol)) {
            if(lp->best_solution[0]*(1-2*lp->maximise) >= lp->infinite)
              report(lp, IMPORTANT,
                     "Feasible solution %g found at iteration %6d, %6d nodes explored",
                     (double)lp->solution[0], lp->total_iter, lp->total_nodes);
            else
              report(lp, IMPORTANT,
                     "Improved solution %g found at iteration %6d, %6d nodes explored",
                     (double)lp->solution[0], lp->total_iter, lp->total_nodes);
          }
          if((lp->usermessage != NULL) && (milpitems > 0)) {
            if((lp->msgmask & MSG_MILPFEASIBLE) && (lp->best_solution[0]*(1-2*lp->maximise) >= lp->infinite))
              lp->usermessage(lp, lp->msghandle, MSG_MILPFEASIBLE);
            else if((lp->msgmask & MSG_MILPBETTER) && (lp->msgmask & MSG_MILPBETTER))
              lp->usermessage(lp, lp->msghandle, MSG_MILPBETTER);
          }

          transfer_solution(lp);
          calculate_duals(lp);
	  if ((!calculate_sensitivity_duals(lp)) || (!calculate_sensitivity_obj(lp)))
	    failure = OUT_OF_MEMORY;
	  else {
	    if((lp->trace) && (lp->print_sol)) {
	      print_objective(lp);
	      print_solution(lp);
	    }

	    if(lp->break_at_first)
	      lp->Break_bb = TRUE;
	    else if(!(fabs(lp->break_at_value) == lp->infinite)) {
	      if(lp->maximise && (lp->best_solution[0] > lp->break_at_value))
		lp->Break_bb = TRUE;
	      if(!lp->maximise && (lp->best_solution[0] < lp->break_at_value))
		 lp->Break_bb = TRUE;
	    }

            /* Add MIP cut (***development placeholder***) */
     		     /*
     		     set_rh(lp, lp->rows, lp->best_solution[0]);
             */
          }
          if((lp->solutioncount == 1) && (milpitems > 0) && (lp->solutionlimit == 1) &&
             (fabs(lp->limMIP_solution-lp->best_solution[0])<lp->epsb))
            lp->Break_bb = TRUE;
        }
      }
    }
  }

  lp->Level--;

  /* failure can have the values:
     OPTIMAL, TIMEOUT, USERABORT, OUT_OF_MEMORY, MILP_FAIL, UNBOUNDED and INFEASIBLE. */

  return(failure);
} /* milpsolve */

int solve(lprec *lp)
{
  int i;
  MYBOOL piv_rule;
  MYBOOL iprocessed;

  lp->timestart = timenow();

  lp->total_iter  = 0;
  lp->max_level   = 1;
  lp->total_nodes = 0;
  piv_rule = lp->piv_rule;

  lp->spx_status = (short) presolve(lp);
  if(lp->spx_status == RUNNING) {
    iprocessed = (MYBOOL) !lp->wasprocessed;
    if (preprocess(lp)) {
      if(yieldformessages(lp)!=0)
	lp->spx_status = USERABORT;
    }
    else if(lp->spx_status == RUNNING)
      lp->spx_status = NOTRUN;

    if(lp->spx_status == RUNNING) {
      if(isvalid(lp)) {
        if(yieldformessages(lp)!=0)
          lp->spx_status = USERABORT;
        if(lp->spx_status == RUNNING) {
	  lp->real_solution = lp->infinite;
          if(lp->maximise && lp->obj_bound == lp->infinite)
            lp->best_solution[0] = -lp->infinite;
          else if(!lp->maximise && lp->obj_bound == -lp->infinite)
            lp->best_solution[0] = lp->infinite;
          else
            lp->best_solution[0] = lp->obj_bound;

          lp->Level = 0;

          if(!lp->basis_valid) {
           /* Initialize row slacks as basic (in-basis) variables */
            for(i = 0; i <= lp->rows; i++) {
              lp->basis[i] = TRUE;
              lp->bas[i]   = i;
            }
           /* Initialize column variables as non-basic (out-of-basis) */
            for(i = lp->rows + 1; i <= lp->sum; i++)
                 lp->basis[i] = FALSE;

           /* Assume all variables are at their lower bounds */
            for(i = 0; i <= lp->sum; i++)
              lp->lower[i] = TRUE;

            lp->basis_valid = TRUE;
          }

          lp->eta_valid = FALSE;
          lp->Break_bb  = FALSE;
          i = milpsolve(lp, lp->orig_upbo, lp->orig_lowbo, lp->basis,
                            lp->lower, lp->bas, FALSE);

          if(iprocessed) {
            postprocess(lp);
	    iprocessed = FALSE;
	  }
          if(lp->lag_status != RUNNING) {
            if(i == OPTIMAL)
#if defined CHECK_SOLUTION
              i = check_solution(lp, lp->columns,
    			         lp->best_solution, lp->orig_upbo, lp->orig_lowbo)
#endif
                         ;
            else {
              report(lp, NORMAL, "lp_solve unsuccessful after %d iterations and a last best value of (%g)",
                 lp->total_iter, lp->best_solution[0]);
              if(lp->total_nodes > 1)
                report(lp, NORMAL, "lp_solve explored %d nodes before termination",
                       lp->total_nodes);
            }
          }
          lp->spx_status = (short) i;
        }
      }
      else {
        /* if we get here, isvalid(lp) failed. I suggest we return FAILURE */
        if((lp->debug) || (lp->trace))
          report(lp, CRITICAL, "Error, the current LP seems to be invalid");
        lp->spx_status = FAILURE;
      }
    }
    if(iprocessed) {
      postprocess(lp);
      iprocessed = FALSE;
    }
  }
  lp->timeend = timenow();
  lp->piv_rule = piv_rule; /* because piv_rule can be changed because of degeneracy */
  return(lp->spx_status);

} /* solve */

int lag_solve(lprec *lp, REAL start_bound, int num_iter, short verbose)
{
  int   i, j, citer, nochange, ok = TRUE;
  MYBOOL  OrigFeas, AnyFeas, same_basis, oldpresolve;
  REAL  *OrigObj = NULL, *ModObj = NULL, *SubGrad = NULL, *BestFeasSol = NULL;
  REAL  Zub, Zlb, Znow, Zold, pie;
  REAL  rhsmod, Step, Delta, SqrsumSubGrad;
  int   *old_bas = NULL;
  MYBOOL  *old_lower = NULL;

  /* note: verbose is ignored, but parameter still included for backward
     compatibility
  */

  i = verbose; /* suppresses compiler warning about unused variable verbose */

  lp->spx_status = RUNNING;

  /* do standard preprocessing */
  if (!preprocess(lp)) {
    lp->lag_status = lp-> spx_status;
    return(lp->lag_status);
  }

  oldpresolve = lp->do_presolve;

  /* allocate mem */
  if ((MALLOC(OrigObj, lp->columns + 1, REAL) == NULL) ||
      (CALLOC(ModObj, lp->columns + 1, REAL) == NULL) ||
      (CALLOC(SubGrad, lp->nr_lagrange, REAL) == NULL) ||
      (CALLOC(BestFeasSol, lp->sum + 1, REAL) == NULL) ||
      (MALLOCCPY(old_bas, lp->bas, lp->rows + 1, int) == NULL) ||
      (MALLOCCPY(old_lower, lp->lower, lp->sum + 1, MYBOOL) == NULL)
     ) {
    lp->lag_status = OUT_OF_MEMORY;
    FREE(old_lower);
    FREE(old_bas);
    FREE(BestFeasSol);
    FREE(SubGrad);
    FREE(ModObj);
    FREE(OrigObj);
    return(lp->lag_status);
  }

  get_row(lp, 0, OrigObj);

  pie = 2;

  if(lp->maximise) {
    Zub  =  (REAL) lp->infinite;
    Zlb  =  start_bound;
    Znow = (REAL) -lp->infinite;
  }
  else {
    Zlb  = (REAL) -lp->infinite;
    Zub  =  start_bound;
    Znow = (REAL) lp->infinite;
  }
  lp->lag_status = RUNNING;
  Step     = 1;
  OrigFeas = FALSE;
  AnyFeas  = FALSE;
  citer    = 0;
  nochange = 0;

  for(i = 0 ; i < lp->nr_lagrange; i++)
    lp->lambda[i] = 0;

  while(lp->lag_status == RUNNING) {
    citer++;

    for(i = 1; i <= lp->columns; i++) {
      rhsmod = OrigObj[i];
      for(j = 0; j < lp->nr_lagrange; j++) {
        Delta = lp->lambda[j] * lp->lag_row[j][i];
        if(lp->maximise)
          rhsmod -= Delta;
        else
          rhsmod += Delta;
      }
      ModObj[i] = rhsmod;
      if (!set_mat(lp, 0, i, rhsmod)) {
        lp->lag_status = lp->spx_status;
	ok = FALSE;
        break;
      }
    }
    if (!ok)
      break;

    rhsmod = 0;
    for(i = 0; i < lp->nr_lagrange; i++) {
/*      if(lp->maximise) */
      rhsmod += lp->lambda[i] * lp->lag_rhs[i];     /* *** My correct version */
/*      else */
/*        rhsmod -= lp->lambda[i] * lp->lag_rhs[i];   // This causes error! */
    }

    if(lp->lag_trace) {
      report(lp, IMPORTANT, "Zub: %10g Zlb: %10g Step: %10g pie: %10g Feas %d",
          (double)Zub, (double)Zlb, (double)Step, (double)pie, OrigFeas);
      for(i = 0; i < lp->nr_lagrange; i++)
        report(lp, IMPORTANT, "%3d SubGrad %10g lambda %10g", i,
           (double)SubGrad[i], (double)lp->lambda[i]);
    }

    if(lp->lag_trace && lp->sum < 20)
      print_lp(lp);

    i = solve(lp);
    lp->do_presolve = RUNNING;
    if((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == OUT_OF_MEMORY)) {
      break;
    }

    if(lp->lag_trace && lp->sum < 20) {
      print_objective(lp);
      print_solution(lp);
    }

    same_basis = TRUE;
    i = 1;
    while(same_basis && i < lp->rows) {
      same_basis = (MYBOOL) (old_bas[i] == lp->bas[i]);
      i++;
    }
    i = 1;
    while(same_basis && i < lp->sum) {
      same_basis = (MYBOOL) (old_lower[i] == lp->lower[i]);
      i++;
    }
    if(!same_basis) {
      MEMCPY(old_lower, lp->lower, lp->sum + 1);
      MEMCPY(old_bas, lp->bas, lp->rows + 1);
      pie *= (REAL) 0.95;
    }

    if(lp->lag_trace)
      report(lp, DETAILED, "Result: %d  same basis: %d", lp->spx_status, same_basis);

    if(lp->spx_status == UNBOUNDED) {
      if(lp->lag_trace)
        for(i = 1; i <= lp->columns; i++)
          report(lp, NORMAL, "%g ", (double)ModObj[i]);
      break;
    }

    if((lp->spx_status == FAILURE) ||
       (lp->spx_status == INFEASIBLE) ||
       (lp->spx_status == MILP_FAIL))
      lp->lag_status = lp->spx_status;

    SqrsumSubGrad = 0;
    for(i = 0; i < lp->nr_lagrange; i++) {
      SubGrad[i]= -lp->lag_rhs[i];
      for(j = 1; j <= lp->columns; j++)
        SubGrad[i] += lp->best_solution[lp->rows + j] * lp->lag_row[i][j];
      SqrsumSubGrad += SubGrad[i] * SubGrad[i];
    }

    OrigFeas = TRUE;
    for(i = 0; (i < lp->nr_lagrange) && (OrigFeas == TRUE); i++) {
      if(lp->lag_con_type[i] == EQ) {
        if(my_abs(SubGrad[i]) > lp->epsb)
        OrigFeas = FALSE;
      }
      else if(SubGrad[i] > lp->epsb)
        OrigFeas = FALSE;
    }

    if(OrigFeas) {
      AnyFeas = TRUE;
      Zold = Znow;
      Znow = 0;
      for(i = 1; i <= lp->columns; i++)
        Znow += lp->best_solution[lp->rows + i] * OrigObj[i];
      if((lp->maximise) && (Znow > Zlb)) {
        Zlb = Znow;
        for(i = 1; i <= lp->sum; i++)
          BestFeasSol[i] = lp->best_solution[i];
        BestFeasSol[0] = Zlb;
        if(lp->lag_trace)
          report(lp, NORMAL, "Best feasible solution: %g", (double)Zlb);
        nochange = 0;
      }
      else if(Znow < Zub) {
        Zub = Znow;
        for(i = 1; i <= lp->sum; i++)
          BestFeasSol[i] = lp->best_solution[i];
        BestFeasSol[0] = Zub;
        if(lp->lag_trace)
          report(lp, NORMAL, "Best feasible solution: %g", (double)Zub);
        nochange = 0;
      }
      else if(Znow == Zold) {
        nochange++;
	i = num_iter / LAG_SINGULARLIMIT;
	if(nochange > i) {
          num_iter = citer;
        }
      }
    }

    if(lp->maximise)
      Zub = my_min(Zub, lp->best_solution[0] + rhsmod);
    else
      Zlb = my_max(Zlb, lp->best_solution[0] - rhsmod);

    if(my_abs(Zub-Zlb)<lp->lag_accept){
      lp->lag_status = OPTIMAL;
    }
    Step = pie * (((REAL) 1.05*Zub) - Zlb) / SqrsumSubGrad;

    for(i = 0; i < lp->nr_lagrange; i++) {
      lp->lambda[i] += Step * SubGrad[i];
      if(lp->lag_con_type[i] != EQ && lp->lambda[i] < 0)
        lp->lambda[i] = 0;
    }

    if(citer == num_iter && lp->lag_status == RUNNING) {
      if(AnyFeas)
        lp->lag_status = FEAS_FOUND;
      else
        lp->lag_status = NO_FEAS_FOUND;
    }
  }

  if(!((lp->spx_status == USERABORT) || (lp->spx_status == TIMEOUT) || (lp->spx_status == UNBOUNDED) || (lp->spx_status == OUT_OF_MEMORY))) {
    for(i = 0; i <= lp->sum; i++)
      lp->best_solution[i] = BestFeasSol[i];

    for(i = 0; i <lp->nr_lagrange; i++) {
      if(!lp->maximise)                     /* *** My addition */
        lp->lambda[i] = -lp->lambda[i];
    }

    for(i = 1; i <= lp->columns; i++)
     if (!set_matrix(lp, 0, i, OrigObj[i], FALSE)) {
       lp->lag_status = lp-> spx_status;
       ok = FALSE;
       break;
     }

    if (ok) {
      if(lp->maximise)
	lp->lag_bound = Zub;
      else
	lp->lag_bound = Zlb;
    }
  }
  else
    ok = FALSE;

  if (ok) {
    /* do standard postprocessing */
    lp->do_presolve = oldpresolve;
    if(lp->lag_status == OPTIMAL) {
      report(lp, NORMAL, "Lagrangean convergence achieved in %d iterations",  citer);
#if defined CHECK_SOLUTION
      i = check_solution(lp, lp->orig_columns,
                         lp->best_solution, lp->orig_upbo, lp->orig_lowbo);
#endif
    }
    else
      report(lp, NORMAL, "lp_solve unsuccessful after %d Lagrangean iterations and a last best value of (%g)",
             citer, lp->best_solution[0]);
    postprocess(lp);
  }

  /* and then free memory */
  free(BestFeasSol);
  free(SubGrad);
  free(OrigObj);
  free(ModObj);
  free(old_bas);
  free(old_lower);

  return(lp->lag_status);
}
