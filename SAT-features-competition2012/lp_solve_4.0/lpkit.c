#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "lpkit.h"
#include "patchlevel.h"

/* Globals */

int     Warn_count; /* used in CHECK version of rounding macro */

int     max_allowed_columns = -1;

void set_magic(int code, int param)
{
  if(code != HASHSIZE) return;
  max_allowed_columns = param;
}

/* Return lp_solve version information */
void lp_solve_version(int *majorversion, int *minorversion,
                      int *release, int *build)
{
  int a;

  a=sizeof(lprec);

  (*majorversion) = MAJORVERSION;
  (*minorversion) = MINORVERSION;
  (*release) = RELEASE;
  (*build) = BUILD;
}

static void set_infinite_ext(lprec *lp, REAL infinite, MYBOOL init)
{
  /* lp->infinite = infinite; */
  int i;

  infinite = fabs(infinite);
  if((init) || (lp->obj_bound == lp->infinite))
    lp->obj_bound = infinite;
  if((init) || (lp->break_at_value == -lp->infinite))
    lp->break_at_value = -infinite;
  for(i = 0; i <= lp->sum; i++) {
    if((!init) && (lp->orig_lowbo[i] == lp->infinite))
      lp->orig_lowbo[i] = infinite;
    if((init) || (lp->orig_upbo[i] == lp->infinite))
      lp->orig_upbo[i] = infinite;
  }
  lp->infinite = infinite;
}

lprec *make_lpext(int rows, int columns, int non_zeros, int mat_alloc, char *lp_name)
{
  lprec *lp;
  int i, sum;

  if(rows < 0 || columns < 0 ||
     ((max_allowed_columns > 0) && (columns > max_allowed_columns))) {
    report(NULL, CRITICAL, "make_lpext: rows < 0 or columns < 0 or columns > %d", max_allowed_columns);
    lp = NULL;
  }
  else if (CALLOC(lp, 1, lprec) != NULL) {
    if (!set_lp_name(lp, lp_name)) {
      FREE(lp);
    }
    else {
      lp->verbose = CRITICAL;
      lp->print_sol = FALSE;
      lp->improve = IMPROVE_NONE;
      lp->scalemode = MMSCALING;
      lp->trace = FALSE;
      lp->lag_trace = FALSE;
      lp->debug = FALSE;
      lp->print_at_invert = FALSE;

      sum = rows + columns;
      lp->rows = rows;
      lp->columns = columns;
      lp->orig_rows = rows;
      lp->orig_columns = columns;
      lp->sum = sum;
      lp->rows_alloc = rows;
      lp->columns_alloc = columns;
      lp->columns_used = 0;
      lp->sum_alloc = sum;
      lp->names_used = FALSE;

      lp->spx_status = NOTRUN;
      lp->lag_status = NOTRUN;

      lp->negrange = DEF_NEGRANGE;
      lp->splitnegvars = DEF_SPLITNEGVARS;
      lp->epsilon = (REAL) DEF_EPSILON;
      lp->epsb = (REAL) DEF_EPSB;
      lp->epsd = (REAL) DEF_EPSD;
      lp->epsel = (REAL) DEF_EPSEL;
      lp->epsperturb = (REAL) DEF_PERTURB;
      lp->epspivot = (REAL) DEF_EPSPIVOT;
      lp->mip_gap = (REAL) DEF_MIP_GAP;
      lp->lag_accept = (REAL) DEF_LAGACCEPT;
      lp->wasprocessed = FALSE;

      lp->non_zeros = non_zeros;
      lp->mat_alloc = mat_alloc;
      lp->eta_alloc = INITIAL_MAT_SIZE;
      lp->max_num_inv = DEFNUMINV;
      lp->var_is_free = NULL;

      if ((CALLOC(lp->mat, lp->mat_alloc, matrec) == NULL) ||
	  (CALLOC(lp->col_no, lp->mat_alloc + 1, int) == NULL) ||
	  (CALLOC(lp->col_end, columns + 1, int) == NULL) ||
	  (CALLOC(lp->row_end, rows + 1, int) == NULL) ||
	  (CALLOC(lp->orig_rh, rows + 1, REAL) == NULL) ||
	  (CALLOC(lp->rh, rows + 1, REAL) == NULL) ||
	  (CALLOC(lp->rhs, rows + 1, LREAL) == NULL) ||
	  (CALLOC(lp->must_be_int, columns + 1, MYBOOL) == NULL) ||
	  (CALLOC(lp->var_is_sc, columns + 1, REAL) == NULL) ||
	  /* (CALLOC(lp->var_is_free, columns + 1, MYBOOL) == NULL) || */
          (CALLOC(lp->var_to_orig, sum + 1, int) == NULL) ||
          (CALLOC(lp->orig_to_var, sum + 1, int) == NULL) ||
          (CALLOC(lp->orig_upbo, sum + 1, REAL) == NULL) ||
	  (CALLOC(lp->upbo, sum + 1, REAL) == NULL) ||
	  (CALLOC(lp->orig_lowbo, sum + 1, REAL) == NULL) ||
	  (CALLOC(lp->lowbo, sum + 1, REAL) == NULL) ||
	  (CALLOC(lp->bas, rows + 1, int) == NULL) ||
	  (CALLOC(lp->basis, sum + 1, MYBOOL) == NULL) ||
	  (CALLOC(lp->lower, sum + 1, MYBOOL) == NULL) ||
	  (CALLOC(lp->eta_value, lp->eta_alloc + 1, REAL) == NULL) ||
	  (CALLOC(lp->eta_row_nr, lp->eta_alloc + 1, int) == NULL) ||
	  (CALLOC(lp->eta_col_end, rows + lp->max_num_inv + 1, int) == NULL) ||
	  (CALLOC(lp->solution, sum + 1, REAL) == NULL) ||
	  (CALLOC(lp->best_solution, sum + 1, REAL) == NULL) ||
	  (CALLOC(lp->duals, sum + 1, REAL) == NULL) ||
	  (CALLOC(lp->dualsfrom, sum + 1, REAL) == NULL) ||
	  (CALLOC(lp->dualstill, sum + 1, REAL) == NULL) ||
	  (CALLOC(lp->objfrom, columns + 1, REAL) == NULL) ||
	  (CALLOC(lp->objtill, columns + 1, REAL) == NULL) ||
          (CALLOC(lp->objfromvalue, columns + 1, REAL) == NULL) ||
	  (CALLOC(lp->ch_sign, rows + 1, MYBOOL) == NULL)
	 ) {
	delete_lp(lp);
	lp = NULL;
      }
      else {
	lp->row_end_valid = FALSE;
	lp->int_count = 0;
        lp->var_priority = NULL;
	lp->sc_count = 0;

        lp->sos_list = NULL;
        lp->sos_alloc = 0;
        lp->sos_count = 0;
        lp->sos_ints = 0;
        lp->sos_vars = 0;
        lp->sos_priority = NULL;

        lp->tighten_on_set = FALSE;

        for(i = 0; i <= columns; i++)
          lp->must_be_int[i] = ISREAL;

	for(i = 0; i <= sum; i++)
	  lp->orig_lowbo[i] = 0;

	lp->basis_valid = FALSE;
#if 0
	lp->basis_valid = TRUE;

	for(i = 0; i <= rows; i++) {
	  lp->bas[i] = i;
	  lp->basis[i] = TRUE;
	}

	for(i = rows + 1; i <= sum; i++)
	  lp->basis[i] = FALSE;

	for(i = 0 ; i <= sum; i++)
	  lp->lower[i] = TRUE;
#endif

        for(i = 0; i <= lp->rows; i++) {
          lp->var_to_orig[i] = i;
          lp->orig_to_var[i] = i;
        }

        for(i = 1; i <= lp->columns; i++) {
          lp->var_to_orig[lp->rows + i] = i;
          lp->orig_to_var[lp->rows + i] = i;
        }

	lp->eta_valid = TRUE;
	lp->eta_size = 0;

	lp->nr_lagrange = 0;

	lp->maximise = FALSE;
	set_infinite_ext(lp, DEF_INFINITE, TRUE);

        lp->piv_rule = BEST_SELECT;
	/* lp->piv_rule = WORST_SELECT; */
	/* lp->piv_rule = FIRST_SELECT; */

        lp->floor_first = BRANCH_FLOOR;
        lp->bb_varbranch = NULL;
	lp->bb_rule = FIRST_SELECT;

	lp->iter = 0;
	lp->total_iter = 0;
	lp->do_presolve = FALSE;
	lp->anti_degen = FALSE;

	lp->scaling_used = FALSE;
	lp->columns_scaled = FALSE;

	lp->valid = FALSE;

	lp->sectimeout = 0;
	lp->solutioncount = 0;
	lp->solutionlimit = 1;

	lp->abort = NULL;
	lp->aborthandle = NULL;
	lp->writelog = NULL;
	lp->loghandle = NULL;
	lp->debuginfo = NULL;
	lp->usermessage = NULL;
	lp->msgmask = MSG_NONE;
	lp->msghandle = NULL;
      }
    }
  }
  return(lp);
}

lprec *make_lp(int rows, int columns)
{
  return(make_lpext(rows, columns, 0, 1, "Unnamed"));
}

static void free_SOSrec(SOSrec *SOS)
{
  FREE(SOS->name);
  if(SOS->size > 0) {
    FREE(SOS->members);
    FREE(SOS->weights);
    FREE(SOS->membersSorted);
    FREE(SOS->membersMapped);
  }
  FREE(SOS);
}

void delete_lp(lprec *lp)
{
  int i;

  if(lp != NULL) {
    FREE(lp->lp_name);
    if(lp->names_used) {
      FREE(lp->row_name);
      FREE(lp->col_name);
      if (lp->rowname_hashtab != NULL)
	free_hash_table(lp->rowname_hashtab);
      if (lp->colname_hashtab != NULL)
	free_hash_table(lp->colname_hashtab);
    }

    FREE(lp->mat);
    FREE(lp->col_no);
    FREE(lp->col_end);
    FREE(lp->row_end);
    FREE(lp->orig_rh);
    FREE(lp->rh);
    FREE(lp->rhs);
    FREE(lp->must_be_int);
    set_varweights(lp, NULL);
    FREE(lp->bb_varbranch);
    FREE(lp->var_is_sc);
    FREE(lp->var_is_free);
    FREE(lp->orig_upbo);
    FREE(lp->orig_lowbo);
    FREE(lp->upbo);
    FREE(lp->lowbo);
    FREE(lp->bas);
    FREE(lp->basis);
    FREE(lp->lower);
    FREE(lp->eta_value);
    FREE(lp->eta_row_nr);
    FREE(lp->eta_col_end);
    FREE(lp->solution);
    FREE(lp->best_solution);
    FREE(lp->var_to_orig);
    FREE(lp->orig_to_var);
    FREE(lp->duals);
    FREE(lp->dualsfrom);
    FREE(lp->dualstill);
    FREE(lp->objfrom);
    FREE(lp->objtill);
    FREE(lp->objfromvalue);
    FREE(lp->ch_sign);
    if(lp->sos_alloc > 0) {
      for(i = 0; i < lp->sos_count; i++)
        free_SOSrec(lp->sos_list[i]);
      FREE(lp->sos_list);
    }
    if(lp->sos_vars > 0) {
      FREE(lp->sos_priority);
    }
    if(lp->scaling_used) {
      FREE(lp->scale);
    }
    if(lp->nr_lagrange > 0) {
      FREE(lp->lag_rhs);
      FREE(lp->lambda);
      FREE(lp->lag_con_type);
      if (lp->lag_row != NULL)
        for(i = 0; i < lp->nr_lagrange; i++) {
	  FREE(lp->lag_row[i]);
        }
      FREE(lp->lag_row);
    }

    FREE(lp);
  }
}

/* Sorting and searching functions */
static int SortByREAL(int *item, REAL *weight, int size, int offset, MYBOOL unique)
{
  int i, ii, saveI;
  REAL saveW;

  for(i = 1; i < size; i++) {
    ii = i+offset-1;
    while (ii > 0 && weight[ii] >= weight[ii+1]) {
      if(unique && (weight[ii] == weight[ii+1]))
        return(item[ii]);
      saveI = item[ii];
      saveW = weight[ii];
      item[ii] = item[ii+1];
      weight[ii] = weight[ii+1];
      item[ii+1] = saveI;
      weight[ii+1] = saveW;
      ii--;
    }
  }
  return(0);
}

static int make_SOSchain(lprec *lp)
{
  int i, j, k, n;
  REAL *order, sum, weight;

  /* Tally SOS variables and create master SOS variable list */
  n = 0;
  for(i = 0; i < lp->sos_count; i++)
    n += lp->sos_list[i]->size;
  lp->sos_vars = n;
  if ((MALLOC(lp->sos_priority, n, int) == NULL) || (MALLOC(order, n, REAL) == NULL)) {
    lp->spx_status = OUT_OF_MEMORY;
    return(-1);
  }

  /* Move variable data to the master SOS list and sort */
  n = 0;
  sum = 0;
  for(i = 0; i < lp->sos_count; i++) {
    for(j = 1; j <= lp->sos_list[i]->size; j++) {
      lp->sos_priority[n] = lp->sos_list[i]->members[j];
      weight = lp->sos_list[i]->weights[j];
      sum += weight;
      order[n] = sum;
/*    order[n] = lp->sos_list[i]->priority * 1000000 + weight; */
      n++;
    }
  }
  i = SortByREAL(lp->sos_priority, order, n, 0, FALSE);

  /* Remove duplicate SOS variables */
  for(i = 0; i < n; i++) {
    /* Scan forward to look for duplicate variables */
    for(j = i+1; j < n; j++) {
      if(lp->sos_priority[i] == lp->sos_priority[j]) {
      /* Duplicate found, shrink the tail end of the list */
	for(k = j+1; k < n; k++)
          lp->sos_priority[k-1] = lp->sos_priority[k];
	n--;
      }
    }
  }

  /* Adjust the size of the master variable list, if necessary*/
  if(n < lp->sos_vars) {
    if (REALLOC(lp->sos_priority, n, int) == NULL) {
      n = -1;
      lp->spx_status = OUT_OF_MEMORY;
    }
    else
      lp->sos_vars = n;
  }

  free(order);
  return(n);
}

static int copy_lagrange(lprec *lp, lprec *newlp)
{
  int i, colsplus = lp->columns_alloc + 1;

  for(i = 0; (i < newlp->nr_lagrange) && (MALLOCCPY(newlp->lag_row[i], lp->lag_row[i], colsplus, REAL) != NULL); i++);
  return(!(i < newlp->nr_lagrange));
}

/* this code has not been reviewed yet and is not complete <peno> */
lprec *copy_lp(lprec *lp)
{
  lprec *newlp;
  int rowsplus, colsplus, sumplus;

  rowsplus = lp->rows_alloc + 1;
  colsplus = lp->columns_alloc + 1;
  sumplus  = lp->sum_alloc + 1;

  if (MALLOCCPY(newlp, lp, 1, lprec) != NULL) { /* copy all non pointers */
    newlp->col_name = newlp->row_name = NULL;
    newlp->lp_name = NULL;
    newlp->rowname_hashtab = newlp->colname_hashtab = NULL;
    newlp->mat = NULL;
    newlp->col_no = NULL;
    newlp->col_end = NULL;
    newlp->row_end = NULL;
    newlp->orig_rh = NULL;
    newlp->rh = NULL;
    newlp->rhs = NULL;
    newlp->must_be_int = NULL;
    newlp->var_is_sc = NULL;
    newlp->var_is_free = NULL;
    newlp->orig_upbo = NULL;
    newlp->upbo = NULL;
    newlp->orig_lowbo = NULL;
    newlp->lowbo = NULL;
    newlp->bas = NULL;
    newlp->basis = NULL;
    newlp->lower = NULL;
    newlp->eta_value = NULL;
    newlp->eta_row_nr = NULL;
    newlp->eta_col_end = NULL;
    newlp->solution = NULL;
    newlp->best_solution = NULL;
    newlp->orig_to_var = NULL;
    newlp->var_to_orig = NULL;
    newlp->var_is_free = NULL;
    newlp->duals = NULL;
    newlp->dualsfrom = NULL;
    newlp->dualstill = NULL;
    newlp->objfrom = NULL;
    newlp->objtill = NULL;
    newlp->objfromvalue = NULL;
    newlp->ch_sign = NULL;
    newlp->sos_list = NULL;
    newlp->sos_priority = NULL;
    newlp->scale = NULL;
    newlp->lag_rhs = NULL;
    newlp->lambda = NULL;
    newlp->lag_con_type = NULL;
    newlp->lag_row = NULL;
    newlp->var_priority = NULL;
    newlp->bb_varbranch = NULL;
    newlp->var_is_free = NULL;
    if ((set_lp_name(newlp, lp->lp_name) == FALSE) ||
        ((newlp->names_used) &&
	 ((MALLOCCPY(newlp->col_name, lp->col_name, colsplus, hashelem *) == NULL) ||
	  (MALLOCCPY(newlp->row_name, lp->row_name, rowsplus, hashelem *) == NULL)
	 )
	) ||
	((lp->rowname_hashtab != NULL) && ((newlp->rowname_hashtab = copy_hash_table(lp->rowname_hashtab)) == NULL)) ||
	((lp->colname_hashtab != NULL) && ((newlp->colname_hashtab = copy_hash_table(lp->colname_hashtab)) == NULL)) ||
	(MALLOCCPY(newlp->mat, lp->mat, newlp->mat_alloc, matrec) == NULL) ||
        (MALLOCCPY(newlp->col_no, lp->col_no, newlp->mat_alloc + 1, int) == NULL) ||
	(MALLOCCPY(newlp->col_end, lp->col_end, colsplus, int) == NULL) ||
	(MALLOCCPY(newlp->row_end, lp->row_end, rowsplus, int) == NULL) ||
	(MALLOCCPY(newlp->orig_rh, lp->orig_rh, rowsplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->rh, lp->rh, rowsplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->rhs, lp->rhs, rowsplus, LREAL) == NULL) ||
	(MALLOCCPY(newlp->must_be_int, lp->must_be_int, colsplus, MYBOOL) == NULL) ||
	(MALLOCCPY(newlp->var_is_sc, lp->var_is_sc, colsplus, REAL) == NULL) ||
        ((lp->var_is_free != NULL) && (MALLOCCPY(newlp->var_is_free, lp->var_is_free, colsplus, int) == NULL)) ||
	(MALLOCCPY(newlp->orig_upbo, lp->orig_upbo, sumplus, REAL) == NULL) ||
        (MALLOCCPY(newlp->upbo, lp->upbo, sumplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->orig_lowbo, lp->orig_lowbo, sumplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->lowbo, lp->lowbo, sumplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->bas, lp->bas, rowsplus, int) == NULL) ||
	(MALLOCCPY(newlp->basis, lp->basis, sumplus, MYBOOL) == NULL) ||
	(MALLOCCPY(newlp->lower, lp->lower, sumplus, MYBOOL) == NULL) ||
	(MALLOCCPY(newlp->eta_value, lp->eta_value, lp->eta_alloc + 1, REAL) == NULL) ||
	(MALLOCCPY(newlp->eta_row_nr, lp->eta_row_nr, lp->eta_alloc + 1, int) == NULL) ||
	(MALLOCCPY(newlp->eta_col_end, lp->eta_col_end, lp->rows_alloc + lp->max_num_inv + 1, int) == NULL) ||
	(MALLOCCPY(newlp->solution, lp->solution, sumplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->best_solution, lp->best_solution, sumplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->var_to_orig, lp->var_to_orig, sumplus, int) == NULL) ||
	(MALLOCCPY(newlp->orig_to_var, lp->orig_to_var, 1 + lp->orig_rows + lp->orig_columns, int) == NULL) ||
	(MALLOCCPY(newlp->duals, lp->duals, sumplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->dualsfrom, lp->dualsfrom, sumplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->dualstill, lp->dualstill, sumplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->objfrom, lp->objfrom, colsplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->objtill, lp->objtill, colsplus, REAL) == NULL) ||
        (MALLOCCPY(newlp->objfromvalue, lp->objfromvalue, colsplus, REAL) == NULL) ||
	(MALLOCCPY(newlp->ch_sign, lp->ch_sign, rowsplus, MYBOOL) == NULL) ||
        ((lp->var_priority != NULL) && (MALLOCCPY(newlp->var_priority, lp->var_priority, colsplus, int) == NULL)) ||
        ((lp->bb_varbranch != NULL) && (MALLOCCPY(newlp->bb_varbranch, lp->bb_varbranch, lp->columns_alloc, MYBOOL) == NULL)) ||
        ((lp->sos_count > 0) && (make_SOSchain(lp) == -1)) ||
	((newlp->scaling_used) && (MALLOCCPY(newlp->scale, lp->scale, sumplus, REAL) == NULL)) ||
	((newlp->nr_lagrange > 0) &&
	 ((MALLOCCPY(newlp->lag_rhs, lp->lag_rhs, newlp->nr_lagrange, REAL) == NULL) ||
	  (MALLOCCPY(newlp->lambda, lp->lambda, newlp->nr_lagrange, REAL) == NULL) ||
	  (MALLOCCPY(newlp->lag_con_type, lp->lag_con_type, newlp->nr_lagrange, MYBOOL) == NULL) ||
	  (MALLOCCPY(newlp->lag_row, lp->lag_row, newlp->nr_lagrange, REAL *) == NULL) ||
	  (copy_lagrange(lp, newlp) == FALSE)
	 )
	)
       ) {
      delete_lp(newlp);
      FREE(newlp);
    }
  }
  return(newlp);
}

static int inc_mat_space(lprec *lp, int maxextra)
{
   int ok = TRUE;

   if(lp->non_zeros + maxextra >= lp->mat_alloc) {
     /* let's allocate at least INITIAL_MAT_SIZE  entries */
     if(lp->mat_alloc < INITIAL_MAT_SIZE)
       lp->mat_alloc = INITIAL_MAT_SIZE;

     /* increase the size by RESIZEFACTOR each time it becomes too small */
     while(lp->non_zeros + maxextra >= lp->mat_alloc)
       lp->mat_alloc += lp->mat_alloc / RESIZEFACTOR;

     if ((REALLOC(lp->mat, lp->mat_alloc, matrec) == NULL) ||
         (REALLOC(lp->col_no, lp->mat_alloc + 1, int) == NULL)) {
       lp->spx_status = OUT_OF_MEMORY;
       ok = FALSE;
     }
   }
   return(ok);
}

static int inc_row_space(lprec *lp)
{
  int i, rowsum, oldrowsalloc, rowcolsum, ok = TRUE;

  if(lp->rows > lp->rows_alloc) {
    oldrowsalloc = lp->rows_alloc;
    lp->rows_alloc = lp->rows + my_max(10, (lp->rows / 10));
    lp->sum_alloc  = lp->rows_alloc + lp->columns_alloc;
    rowsum    = lp->rows_alloc + 1;
    rowcolsum = lp->sum_alloc + 1;

    if ((REALLOC(lp->orig_rh, rowsum, REAL) == NULL) ||
        (REALLOC(lp->rh, rowsum, REAL) == NULL) ||
        (REALLOC(lp->rhs, rowsum, LREAL) == NULL) ||
        (REALLOC(lp->upbo, rowcolsum, REAL) == NULL) ||
        (REALLOC(lp->orig_upbo, rowcolsum, REAL) == NULL) ||
        (REALLOC(lp->lowbo, rowcolsum, REAL) == NULL) ||
        (REALLOC(lp->orig_lowbo, rowcolsum, REAL) == NULL) ||
        (REALLOC(lp->solution, rowcolsum, REAL) == NULL) ||
        (REALLOC(lp->best_solution, rowcolsum, REAL) == NULL) ||
        (REALLOC(lp->var_to_orig, rowcolsum, int) == NULL) ||
        (REALLOC(lp->orig_to_var, rowcolsum, int) == NULL) ||
        (REALLOC(lp->basis, rowcolsum, MYBOOL) == NULL) ||
        (REALLOC(lp->lower, rowcolsum, MYBOOL) == NULL) ||
        ((lp->scale != NULL) && (REALLOC(lp->scale, rowcolsum + lp->nr_lagrange, REAL) == NULL)) ||
        (REALLOC(lp->row_end, rowsum, int) == NULL) ||
        ((lp->names_used) && (REALLOC(lp->row_name, rowsum, hashelem *) == NULL)) ||
        (REALLOC(lp->bas, rowsum, int) == NULL) ||
        (REALLOC(lp->duals, rowcolsum, REAL) == NULL) ||
        (REALLOC(lp->dualsfrom, rowcolsum, REAL) == NULL) ||
        (REALLOC(lp->dualstill, rowcolsum, REAL) == NULL) ||
        (REALLOC(lp->ch_sign, rowsum, MYBOOL) == NULL) ||
        (REALLOC(lp->eta_col_end, rowsum + lp->max_num_inv, int) == NULL)
       ) {
       lp->spx_status = OUT_OF_MEMORY;
       ok = FALSE;
    }
    else if ((lp->names_used) && (lp->row_name != NULL)) {
      for(i = oldrowsalloc + 1; i < rowsum; i++)
        lp->row_name[i] = NULL;
    }
  }
  return(ok);
}

static int inc_col_space(lprec *lp)
{
  int i, colsum, oldcolsalloc, rowcolsum, ok = TRUE;

  if(lp->columns >= lp->columns_alloc) {
    oldcolsalloc = lp->columns_alloc;
    if((max_allowed_columns > 0) && (oldcolsalloc >= max_allowed_columns))
      ok = FALSE;
    else {
      lp->columns_alloc = lp->columns + my_max(DELTACOLALLOC, (lp->columns / 10));
      if((max_allowed_columns > 0) && (lp->columns_alloc > max_allowed_columns))
	lp->columns_alloc = max_allowed_columns;
      lp->sum_alloc = lp->rows_alloc + lp->columns_alloc;
      colsum    = lp->columns_alloc + 1;
      rowcolsum = lp->sum_alloc + 1;
      if ((REALLOC(lp->must_be_int, colsum, MYBOOL) == NULL) ||
          (REALLOC(lp->var_is_sc, colsum, REAL) == NULL) ||
          ((lp->var_is_free != NULL) && (REALLOC(lp->var_is_free, colsum, int) == NULL)) ||
          (REALLOC(lp->upbo, rowcolsum, REAL) == NULL) ||
          (REALLOC(lp->orig_upbo, rowcolsum, REAL) == NULL) ||
          (REALLOC(lp->lowbo, rowcolsum, REAL) == NULL) ||
          (REALLOC(lp->orig_lowbo, rowcolsum, REAL) == NULL) ||
          (REALLOC(lp->solution, rowcolsum, REAL) == NULL) ||
          (REALLOC(lp->best_solution, rowcolsum, REAL) == NULL) ||
          (REALLOC(lp->var_to_orig, rowcolsum, int) == NULL) ||
          (REALLOC(lp->orig_to_var, rowcolsum, int) == NULL) ||
          ((lp->bb_varbranch != NULL) && (REALLOC(lp->bb_varbranch, colsum - 1, MYBOOL) == NULL)) ||
          (REALLOC(lp->basis, rowcolsum, MYBOOL) == NULL) ||
          (REALLOC(lp->lower, rowcolsum, MYBOOL) == NULL) ||
          ((lp->scale != NULL) && (REALLOC(lp->scale, rowcolsum + lp->nr_lagrange, REAL) == NULL)) ||
          (REALLOC(lp->col_end, colsum, int) == NULL) ||
          ((lp->names_used) && (REALLOC(lp->col_name, colsum, hashelem *) == NULL)) ||
          ((lp->var_priority != NULL) && (REALLOC(lp->var_priority, colsum, int) == NULL)) ||
          (REALLOC(lp->duals, rowcolsum, REAL) == NULL) ||
          (REALLOC(lp->dualsfrom, rowcolsum, REAL) == NULL) ||
          (REALLOC(lp->dualstill, rowcolsum, REAL) == NULL) ||
          (REALLOC(lp->objfrom, colsum, REAL) == NULL) ||
          (REALLOC(lp->objtill, colsum, REAL) == NULL) ||
          (REALLOC(lp->objfromvalue, colsum, REAL) == NULL)
	 ) {
         lp->spx_status = OUT_OF_MEMORY;
         ok = FALSE;
      }
      else {
        if ((lp->names_used) && (lp->col_name != NULL)) {
	  for(i = oldcolsalloc+1; i < colsum; i++)
	    lp->col_name[i] = NULL;
        }
	if(lp->bb_varbranch) {
          for(i = oldcolsalloc; i < colsum - 1; i++)
            lp->bb_varbranch[i] = BRANCH_DEFAULT;
	}
      }
    }
  }
  return(ok);
}

/* Implement pure binary search for matrix look-up */
/* This routine only works if the sparse matrix is sorted correctly. This can not be guaranteed at this time */
/* It is however faster ... */
static int find_mat_elm1(lprec *lp, int row, int column, int *insertpos)
{
  int low, high, mid, item;

  (*insertpos) = -1;

  if(row < 0 || row > lp->rows) {
    report(lp, IMPORTANT, "find_mat_elm: Row %d out of range", row);
    return(-1);
  }
  if(column < 1 || column > lp->columns) {
    report(lp, IMPORTANT, "find_mat_elm: Column %d out of range", column);
    return(-1);
  }

  low = lp->col_end[my_min(column - 1, lp->columns_used)];
  (*insertpos) = low;
  high = lp->col_end[my_min(column, lp->columns_used)] - 1;
  if(low > high)
    return(-2);

 /* Do binary search logic */
  mid = (low+high) / 2;
  item = lp->mat[mid].row_nr;
  while(high - low > LINEARSEARCH) {
    if(item < row) {
      low = mid + 1;
      mid = (low+high) / 2;
      item = lp->mat[mid].row_nr;
    }
    else if(item > row) {
      high = mid - 1;
      mid = (low+high) / 2;
      item = lp->mat[mid].row_nr;
    }
    else {
      low = mid;
      high = mid;
    }
  }

 /* Do linear scan search logic */
  if((high > low) && (high - low <= LINEARSEARCH)) {
    item = lp->mat[low].row_nr;
    while((low < high) && (item < row)) {
      low++;
      item = lp->mat[low].row_nr;
    }
    if(item == row)
      high = low;
  }

  (*insertpos) = low;
  if((low == high) && (row == item))
    return(low);
  else {
    if((low < lp->col_end[my_min(column, lp->columns_used)]) && (lp->mat[low].row_nr < row))
      (*insertpos)++;
    return(-2);
  }
}

#define find_mat_elm find_mat_elm1

#if defined CHK_find_mat_elm
#if defined _DEBUG || defined DEBUG
# undef find_mat_elm

/* derrived from the original code because find_mat_elm from KJELL not always returns correct results ... */
static int find_mat_elm2(lprec *lp, int row, int column, int *insertpos)
{
  int elmnr, col_end;
  matrec *matel;

  if(row < 0 || row > lp->rows) {
    report(lp, SEVERE, "find_mat_elm: Row %d out of range", row);
    return(-1);
  }
  if(column < 1 || column > lp->columns) {
    report(lp, SEVERE, "find_mat_elm: Column %d out of range", column);
    return(-1);
  }

  elmnr = lp->col_end[my_min(column - 1, lp->columns_used)];
  matel = lp->mat + elmnr;
  col_end = lp->col_end[my_min(column, lp->columns_used)];
  while(elmnr < col_end && matel->row_nr < /* != */ row) {
    matel++;
    elmnr++;
  }

  *insertpos = elmnr;

  if((elmnr == col_end) || (matel->row_nr > row))
    elmnr = -2;

  return(elmnr);
}

static int find_mat_elm(lprec *lp, int row, int column, int *insertpos)
{
  int elmnr1, elmnr2, insertpos1, insertpos2;

  /* check spare matrix order */

  {
    int column,elmnr,col_end,row;
    matrec *matel;

    for (column=1;column<=lp->columns_used;column++) {
	elmnr = lp->col_end[column - 1];
	matel = lp->mat + elmnr;
	col_end = lp->col_end[column];
	row=-1;
	while(elmnr < col_end) {
	    if ((row!=-1) && (matel->row_nr<=row))
	      fprintf(stderr,"Error\n");
	    row=matel->row_nr;
	    matel++;
	    elmnr++;
	}
    }
  }


  elmnr1 = find_mat_elm1(lp, row, column, &insertpos1);
  elmnr2 = find_mat_elm2(lp, row, column, &insertpos2);
  if ((elmnr1 != elmnr2) || (insertpos1 != insertpos2)) {
    fprintf(stderr,"Error: find_mat_elm1 returns different result from find_mat_elm2\n");
  }

  *insertpos = insertpos1;
  return(elmnr1);
}
#endif
#endif

int set_matrix(lprec *lp, int Row, int Column, REAL Value, MYBOOL doscale)
{
  int elmnr, lastelm, i, columns, *col_end;

  /* This function is inefficient if used to add new matrix entries in
     other places than at the end of the matrix. OK for replacing existing
     non-zero values */

  /* find out if we already have such an entry, or return insertion point */
  i = find_mat_elm(lp, Row, Column, &elmnr);
  if(i == -1)
    return(FALSE);

  /* if ((Row > 0) && (lp->basis[Column] == TRUE)) */
  if ((lp->basis_valid) && (Row > 0) && (lp->basis[lp->rows + Column]))
    lp->basis_valid = FALSE;
  lp->eta_valid = FALSE;

  if((elmnr != lp->col_end[my_min(Column, lp->columns_used)]) && (lp->mat[elmnr].row_nr == Row)) {
    /* there is an existing entry */
    if(my_abs(Value) > lp->epsel) { /* we replace it by something non-zero */
      if(doscale && lp->scaling_used) {
        lp->mat[elmnr].value = my_chsign(lp->ch_sign[Row],
                                         Value * lp->scale[Row] * lp->scale[lp->rows + Column]);
      }
      else { /* no scaling */
        lp->mat[elmnr].value = my_chsign(lp->ch_sign[Row], Value);
      }
    }
    else { /* setting existing non-zero entry to zero. Remove the entry */
      /* This might remove an entire column, or leave just a bound. No
	 nice solution for that yet */

      /* Shift up tail end of the matrix */
      lp->non_zeros--;           /* Don't cross array border - Moved by KE */
      lastelm = lp->non_zeros;
      for(i = elmnr; i < lastelm ; i++)
	lp->mat[i] = lp->mat[i + 1];
      for(i = Column; i <= lp->columns_used; i++)
	lp->col_end[i]--;
    }
  }
  else if(my_abs(Value) > lp->epsel) {
    /* no existing entry. make new one only if not nearly zero */
    /* check if more space is needed for matrix */
    if (!inc_mat_space(lp, 1))
      return(FALSE);

    /* Shift down tail end of the matrix by one */
    lastelm = lp->non_zeros;
    for(i = lastelm; i > elmnr ; i--)
      lp->mat[i] = lp->mat[i - 1];

    if(Column > lp->columns_used) {
      for(i = lp->columns_used + 1; i <= Column; i++)
        lp->col_end[i] = lp->col_end[i - 1];
      lp->columns_used = Column;
    }
    for(i = Column, columns = lp->columns_used, col_end = lp->col_end + Column; i <= columns; i++, col_end++)
      (*col_end)++;

    /* Set new element */
    lp->mat[elmnr].row_nr = Row;

    if(doscale && lp->scaling_used) {
      lp->mat[elmnr].value = my_chsign(lp->ch_sign[Row],
                                       Value * lp->scale[Row] * lp->scale[lp->rows + Column]);
    }
    else {/* no scaling */
      lp->mat[elmnr].value = my_chsign(lp->ch_sign[Row], Value);
    }

    lp->row_end_valid = FALSE;
    lp->non_zeros++;
  }
  if((lp->var_is_free != NULL) && (lp->var_is_free[Column] > 0))
    return(set_matrix(lp, Row, lp->var_is_free[Column], -Value, doscale));
  return(TRUE);
}

int set_mat(lprec *lp, int Row, int Column, REAL Value)
{
  return(set_matrix(lp, Row, Column, Value, lp->scaling_used));
}

int set_obj_fn(lprec *lp, REAL *row)
{
  int i;

  for(i = 1; i <= lp->columns; i++)
    if (!set_mat(lp, 0, i, row[i]))
      return(FALSE);
  return(TRUE);
}

int str_set_obj_fn(lprec *lp, char *row)
{
  int  i, ok = TRUE;
  REAL *arow;
  char *p, *newp;

  if (CALLOC(arow, lp->columns + 1, REAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    ok = FALSE;
  }
  else {
    p = row;
    for(i = 1; i <= lp->columns; i++) {
      arow[i] = (REAL) strtod(p, &newp);
      if(p == newp) {
	report(lp, IMPORTANT, "str_set_obj_fn: Bad string");
	ok = FALSE;
	lp->spx_status = IGNORED;
	break;
      }
      else
	p = newp;
    }
    if (ok)
      if (!set_obj_fn(lp, arow))
        ok = FALSE;
    free(arow);
  }
  return(ok);
}

int add_constraint(lprec *lp, REAL *row, short constr_type, REAL rh)
{
  int    i, j, stcol, ok = TRUE;
  int    elmnr, orignr, newnr, maxcol;
  MYBOOL   *addto;

  if(!(constr_type == LE || constr_type == GE || constr_type == EQ)) {
    report(lp, SEVERE, "add_constraint: Invalid %d constraint type", constr_type);
    return(FALSE);
  }

  if (MALLOC(addto, lp->columns + 1, MYBOOL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    return(FALSE);
  }

  newnr = 0;
  maxcol = 0;
  for(i = 1; i <= lp->columns; i++)
    if(row[i] != 0) {
      addto[i] = TRUE;
      maxcol = i;
      newnr++;
    }
    else
      addto[i] = FALSE;

  orignr = lp->non_zeros;
  lp->non_zeros += newnr;

  if (!inc_mat_space(lp, 0)) {
    free(addto);
    return(FALSE);
  }
  lp->rows++;
  lp->orig_rows++;
  lp->sum++;
  if (!inc_row_space(lp)) {
    free(addto);
    return(FALSE);
  }

  if(lp->scale != NULL) {
    /* shift column scale */
    for(i = lp->sum; i > lp->rows; i--)
      lp->scale[i] = lp->scale[i - 1];
    /* insert default new row scalar */
    lp->scale[lp->rows] = 1;
  }

  if(lp->scaling_used && lp->columns_scaled)
    for(i = 1; i <= maxcol; i++)
      row[i] *= lp->scale[lp->rows + lp->nr_lagrange + i];

  if(constr_type == GE)
    lp->ch_sign[lp->rows] = TRUE;
  else
    lp->ch_sign[lp->rows] = FALSE;

  elmnr = lp->non_zeros - 1;
  orignr--;
  if(lp->columns_used < maxcol) {
    for(j = lp->columns_used + 1; j <= maxcol; j++)
      lp->col_end[j] = lp->col_end[j - 1];
    lp->columns_used = maxcol;
  }

  for(j = lp->columns_used /* maxcol */; j > 0; j--) {
    stcol = lp->col_end[j] - 1;
    lp->col_end[j] = elmnr + 1;
   /* Add a new non-zero entry */
    if(addto[j]) {
      lp->mat[elmnr].row_nr = lp->rows;
      lp->mat[elmnr].value = my_chsign(lp->ch_sign[lp->rows], row[j]);
      elmnr--;
    }

   /* Check if we are finished */
    if(elmnr <= orignr)
      break;

   /* Shift previous column entries down */
    for(i = stcol; i >= lp->col_end[j-1]; i--) {
      lp->mat[elmnr] = lp->mat[orignr];
      orignr--;
      elmnr--;
    }
  }

  free(addto);

  for(i = lp->sum; i > lp->rows; i--) {
    lp->orig_upbo[i]   = lp->orig_upbo[i - 1];
    lp->orig_lowbo[i]  = lp->orig_lowbo[i - 1];
    lp->basis[i]       = lp->basis[i - 1];
    lp->lower[i]       = lp->lower[i - 1];
    lp->var_to_orig[i] = lp->var_to_orig[i - 1];
  }
  for(i = lp->orig_rows + lp->orig_columns; i > lp->orig_rows; i--)
    lp->orig_to_var[i] = lp->orig_to_var[i - 1];

  /* changed from i <= lp->rows to i < lp->rows, MB */
  if(lp->basis_valid) /* makes it much faster when building the initial model <peno> */
    for(i = 1 ; i < lp->rows; i++)
      if(lp->bas[i] >= lp->rows)
        lp->bas[i]++;

  if(constr_type == LE || constr_type == GE) {
    lp->orig_upbo[lp->rows] = lp->infinite;
  }
  else if(constr_type == EQ) {
    lp->orig_upbo[lp->rows] = 0;
  }
  else {
    report(lp, SEVERE, "add_constraint: Wrong constraint type");
    return(FALSE);
  }

  lp->orig_lowbo[lp->rows] = 0;

  lp->var_to_orig[lp->rows] = lp->orig_rows;
  lp->orig_to_var[lp->orig_rows] = lp->rows;

  if(constr_type == GE && rh != 0)
    lp->orig_rh[lp->rows] = -rh;
  else
    lp->orig_rh[lp->rows] = rh;

  lp->row_end_valid = FALSE;

  lp->bas[lp->rows] = lp->rows;
  lp->basis[lp->rows] = TRUE;
  lp->lower[lp->rows] = TRUE;
  if(lp->names_used)
    lp->row_name[lp->rows] = NULL;
  lp->eta_valid = FALSE;
  return(ok);
}

int str_add_constraint(lprec *lp,
			char *row_string,
			short constr_type,
			REAL rh)
{
  int  i, ok = TRUE;
  REAL *aRow;
  char *p, *newp;

  if (CALLOC(aRow, lp->columns + 1, REAL) == NULL) {
    ok = FALSE;
    lp->spx_status = OUT_OF_MEMORY;
  }
  else {
    p = row_string;

    for(i = 1; i <= lp->columns; i++) {
      aRow[i] = (REAL) strtod(p, &newp);
      if(p == newp) {
	report(lp, IMPORTANT, "str_add_constraint: Bad string");
	ok = FALSE;
	lp->spx_status = IGNORED;
	break;
      }
      else
	p = newp;
    }
    if(ok)
      if (!add_constraint(lp, aRow, constr_type, rh))
        ok = FALSE;
    free(aRow);
  }
  return(ok);
}

int del_constraintex(lprec *lp, int del_row, MYBOOL forpresolve)
{
  int i, j;
  int k;
  int elmnr;
  int startcol;

  if(del_row<1 || del_row>lp->rows) {
    report(lp, IMPORTANT, "del_constraint: Attempt to delete constraint %d that does not exist", del_row);
    return(FALSE);
  }

  elmnr = 0;
  startcol = lp->col_end[0];
  for(i = 1; i <= lp->columns_used; i++) {
    for(j = startcol; j < lp->col_end[i]; j++) {
      if(lp->mat[j].row_nr != del_row) {
	lp->mat[elmnr] = lp->mat[j];
	if(lp->mat[elmnr].row_nr > del_row)
	  lp->mat[elmnr].row_nr--;
	elmnr++;
      }
      else
	lp->non_zeros--;
    }
    startcol = lp->col_end[i];
    lp->col_end[i] = elmnr;
  }
  lp->columns_used = lp->columns;
  for(i = del_row; i < lp->rows; i++) {
    lp->orig_rh[i] = lp->orig_rh[i + 1];
    lp->ch_sign[i] = lp->ch_sign[i + 1];
    lp->bas[i] = lp->bas[i + 1];
    if(lp->names_used)
      lp->row_name[i] = lp->row_name[i + 1];
  }
  for(i = 1; i < lp->rows; i++)
    if(lp->bas[i] > del_row)
      lp->bas[i]--;

  if(forpresolve) {
    k = lp->var_to_orig[del_row];
    lp->orig_to_var[k] = -del_row;
  }

  for(i = del_row; i < lp->sum; i++) {
    lp->lower[i] = lp->lower[i + 1];
    lp->basis[i] = lp->basis[i + 1];
    lp->orig_upbo[i] = lp->orig_upbo[i + 1];
    lp->orig_lowbo[i] = lp->orig_lowbo[i + 1];
    if(lp->scaling_used)
      lp->scale[i] = lp->scale[i + 1];
    lp->var_to_orig[i] = lp->var_to_orig[i + 1];
  }

  lp->rows--;
  lp->sum--;

  if(!forpresolve) {
    for(j = 1, i = 1; i <= lp->orig_rows + lp->orig_columns; i++) {
      lp->orig_to_var[j] = lp->orig_to_var[i];
      if((i > lp->orig_rows) || (lp->orig_to_var[j] != del_row))
        j++;
    }
    lp->orig_rows--;

    for(i = del_row; i <= lp->rows; i++)
      lp->var_to_orig[i]--;
  }

  for(i = 1; i <= lp->orig_rows; i++)
    if(lp->orig_to_var[i] > del_row)
      lp->orig_to_var[i]--;

  lp->row_end_valid = FALSE;
  lp->eta_valid     = FALSE;
  lp->basis_valid   = FALSE;
  return(TRUE);
}

int del_constraint(lprec *lp, int del_row)
{
  return(del_constraintex(lp, del_row, FALSE));
}

int add_lag_con(lprec *lp, REAL *row, short con_type, REAL rhs)
{
  int i, ok = TRUE;
  REAL sign;

  if(con_type == LE || con_type == EQ)
    sign = 1;
  else if(con_type == GE)
    sign = -1;
  else {
    report(lp, IMPORTANT, "add_lag_con: con_type %d not implemented", con_type);
    return(FALSE);
  }

  lp->nr_lagrange++;
  if(lp->nr_lagrange == 1) {
    lp->lag_row = NULL;
    lp->lag_rhs = NULL;
    lp->lambda = NULL;
    lp->lag_con_type = NULL;
    if ((CALLOC(lp->lag_row, lp->nr_lagrange, REAL *) == NULL) ||
        (CALLOC(lp->lag_rhs, lp->nr_lagrange, REAL) == NULL) ||
        (CALLOC(lp->lambda, lp->nr_lagrange, REAL) == NULL) ||
        (CALLOC(lp->lag_con_type, lp->nr_lagrange, MYBOOL) == NULL)
       ) {
      FREE(lp->lag_con_type);
      FREE(lp->lambda);
      FREE(lp->lag_rhs);
      FREE(lp->lag_row);
      lp->spx_status = OUT_OF_MEMORY;
      ok = FALSE;
    }
  }
  else {
    if ((REALLOC(lp->lag_row, lp->nr_lagrange, REAL *) == NULL) ||
        (REALLOC(lp->lag_rhs, lp->nr_lagrange, REAL) == NULL) ||
        (REALLOC(lp->lambda, lp->nr_lagrange, REAL) == NULL) ||
        (REALLOC(lp->lag_con_type, lp->nr_lagrange, MYBOOL) == NULL)
       ) {
      lp->spx_status = OUT_OF_MEMORY;
      ok = FALSE;
    }
  }
  if (ok) {
    if (CALLOC(lp->lag_row[lp->nr_lagrange - 1], lp->columns + 1, REAL) == NULL) {
      lp->spx_status = OUT_OF_MEMORY;
      ok = FALSE;
    }
    else {
      lp->lag_rhs[lp->nr_lagrange - 1] = rhs * sign;
      for(i = 1; i <= lp->columns; i++)
	lp->lag_row[lp->nr_lagrange - 1][i] = row[i] * sign;
      lp->lambda[lp->nr_lagrange - 1] = 0;
      lp->lag_con_type[lp->nr_lagrange - 1] = (MYBOOL) con_type;
    }
  }
  return(ok);
}

int str_add_lag_con(lprec *lp, char *row, short con_type, REAL rhs)
{
  int  i, ok = TRUE;
  REAL *a_row;
  char *p, *new_p;

  if (CALLOC(a_row, lp->columns + 1, REAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    ok = FALSE;
  }
  else {
    p = row;

    for(i = 1; i <= lp->columns; i++) {
      a_row[i] = (REAL) strtod(p, &new_p);
      if(p == new_p) {
	report(lp, IMPORTANT, "str_add_lag_con: Bad string");
	ok = FALSE;
	lp->spx_status = IGNORED;
	break;
      }
      else
	p = new_p;
    }
    if(ok)
      if (!add_lag_con(lp, a_row, con_type, rhs))
        ok = FALSE;
    free(a_row);
  }
  return(ok);
}

int is_splitvar(lprec *lp, int column)
{
   return((lp->var_is_free != NULL) && (lp->var_is_free[column] < 0) && (-lp->var_is_free[column] != column));
}

static void del_splitvars(lprec *lp)
{
  int j;

  if(lp->var_is_free != NULL) {
    for (j = lp->columns; j >= 1; j--)
      if(is_splitvar(lp, j))
        del_column(lp, j);
    FREE(lp->var_is_free);
  }
}

int set_var_branch(lprec *lp, int column, short branch_mode)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "set_var_branch: Column %d out of range", column);
    return(FALSE);
  }
  if(lp->bb_varbranch == NULL) {
    int i;

    if(branch_mode == BRANCH_DEFAULT)
      return(TRUE);

    if (MALLOC(lp->bb_varbranch, lp->columns_alloc, MYBOOL) == NULL) {
      lp->spx_status = OUT_OF_MEMORY;
      return(FALSE);
    }
    for(i = 0; i < lp->columns; i++)
      lp->bb_varbranch[i] = BRANCH_DEFAULT;
  }
  lp->bb_varbranch[column - 1] = (MYBOOL) branch_mode;
  return(TRUE);
}

short get_var_branch(lprec *lp, int column)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "get_var_branch: Column %d out of range", column);
    return(lp->floor_first);
  }
  if(lp->bb_varbranch == NULL)
    return(lp->floor_first);
  if(lp->bb_varbranch[column - 1] == BRANCH_DEFAULT)
    return(lp->floor_first);
  else
    return(lp->bb_varbranch[column-1]);
}

int add_columnex(lprec *lp, REAL *column, int del_splitv, matrec *matreccolum, int Nrows)
{
  int i, elmnr, n;

  /* if the column has only one entry, this should be handled as
     a bound, but this currently is not the case */


  if(del_splitv)
    del_splitvars(lp);

  lp->columns++;
  lp->orig_columns++;
  lp->sum++;
  if ((!inc_col_space(lp)) || (!inc_mat_space(lp, lp->rows + 1)))
    return(FALSE);

 /* if scaling is enabled, we can only scale rows, since
    the column scalar has not been computed */
  if(lp->scaling_used) {
    if(matreccolum != NULL) {
      for(i = 0; i < Nrows; i++)
        if(matreccolum[i].row_nr <= lp->rows)
	  matreccolum[i].value *= lp->scale[matreccolum[i].row_nr];
	else
	  matreccolum[i].value *= lp->scale[lp->sum + 1 + matreccolum[i].row_nr];
    }
    else {
      for(i = 0; i <= lp->rows; i++)
        column[i] *= lp->scale[i];
      for(i = 0; i < lp->nr_lagrange; i++)
        column[lp->rows + 1 + i] *= lp->scale[lp->sum + 1 + i];
    }
  }
  if(lp->scale != NULL) {
    for(i = lp->sum + lp->nr_lagrange; i > lp->sum; i--)
      lp->scale[i] = lp->scale[i - 1];
    lp->scale[lp->sum] = 1;
  }

  for(i = lp->columns_used + 1; i <= lp->columns; i++)
    lp->col_end[i] = lp->col_end[i - 1];
  lp->columns_used = lp->columns;
  elmnr = lp->col_end[lp->columns - 1];
  if(matreccolum != NULL) {
    for(i = 0 ; i < Nrows ; i++)
      if(matreccolum[i].row_nr <= lp->rows) {
        lp->mat[elmnr].row_nr = matreccolum[i].row_nr;
        lp->mat[elmnr].value = my_chsign(lp->ch_sign[matreccolum[i].row_nr], matreccolum[i].value);
        lp->non_zeros++;
        elmnr++;
      }
      else {
        lp->lag_row[matreccolum[i].row_nr][lp->columns] = matreccolum[i].value;
      }
  }
  else {
    for(i = 0 ; i <= lp->rows ; i++)
      if(column[i] != 0.0) {
        lp->mat[elmnr].row_nr = i;
        lp->mat[elmnr].value = my_chsign(lp->ch_sign[i], column[i]);
        lp->non_zeros++;
        elmnr++;
      }
    for(i = 0; i < lp->nr_lagrange; i++)
      lp->lag_row[i][lp->columns] = column[lp->rows + 1 + i];
  }

  lp->var_is_sc[lp->columns] = 0;
  if(lp->var_is_free != NULL)
    lp->var_is_free[lp->columns] = 0;
  lp->col_end[lp->columns] = elmnr;
  lp->orig_lowbo[lp->sum] = 0;
  lp->orig_upbo[lp->sum] = lp->infinite;
  lp->lower[lp->sum] = TRUE;
  lp->basis[lp->sum] = FALSE;
  lp->must_be_int[lp->columns] = ISREAL;
  set_var_branch(lp, lp->columns, BRANCH_DEFAULT);
  if(lp->names_used)
    lp->col_name[lp->columns] = NULL;

  lp->var_to_orig[lp->sum] = lp->columns;
  lp->orig_to_var[lp->orig_rows + lp->orig_columns] = lp->orig_columns;

  lp->row_end_valid = FALSE;

  if(lp->var_priority != NULL) {
    for(n = 0, i = 0; i < lp->columns; i++)
      if(lp->var_priority[i] > n)
        n = lp->var_priority[i];
    lp->var_priority[lp->columns - 1] = n + 1;
  }

  return(TRUE);
}

int add_column(lprec *lp, REAL *column)
{
  return(add_columnex(lp, column, TRUE, NULL, 0));
}

int str_add_column(lprec *lp, char *col_string)
{
  int  i, ok = TRUE;
  REAL *aCol;
  char *p, *newp;

  if (CALLOC(aCol, lp->rows + 1, REAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    ok = FALSE;
  }
  else {
    p = col_string;

    for(i = 0; i <= lp->rows; i++) {
      aCol[i] = (REAL) strtod(p, &newp);
      if(p == newp) {
	report(lp, IMPORTANT, "str_add_column: Bad string");
	ok = FALSE;
	lp->spx_status = IGNORED;
	break;
      }
      else
	p = newp;
    }
    if(ok)
      if (!add_column(lp, aCol))
        ok = FALSE;
    free(aCol);
  }
  return(ok);
}

int del_columnex(lprec *lp, int column, MYBOOL forpresolve)
{
  int i, j, from_elm, to_elm, elm_in_col;
  int k;

  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "del_column: Column %d out of range", column);
    return(FALSE);
  }
  if((lp->var_is_free != NULL) && (lp->var_is_free[column] > 0))
    del_column(lp, lp->var_is_free[column]); /* delete corresponding split column (is always after this column) */

  if(lp->var_is_free != NULL)
    for(i = 1; i < lp->columns; i++)
      if(lp->var_is_free[i] == column)
        lp->var_is_free[i] = 0; /* split column is deleted, so delete reference to it */
  for(i = 1; i <= lp->rows; i++) {
    if(lp->bas[i] == lp->rows + column)
      lp->basis_valid = FALSE;
    else if(lp->bas[i] > lp->rows + column)
      lp->bas[i]--;
  }

  if(forpresolve) {
    k = lp->var_to_orig[lp->rows + column];
    lp->orig_to_var[lp->orig_rows + k] = -column;
  }

  for(i = lp->rows + column; i < lp->sum; i++) {
    if(lp->names_used) {
      j = i - lp->rows;
      lp->col_name[j] = lp->col_name[j + 1];
    }
    lp->orig_upbo[i] = lp->orig_upbo[i + 1];
    lp->orig_lowbo[i] = lp->orig_lowbo[i + 1];
    lp->upbo[i] = lp->upbo[i + 1];
    lp->lowbo[i] = lp->lowbo[i + 1];
    lp->basis[i] = lp->basis[i + 1];
    lp->lower[i] = lp->lower[i + 1];
    lp->var_to_orig[i] = lp->var_to_orig[i + 1];
  }
  if(lp->scaling_used) {
    for(i = lp->rows + column; i < lp->sum + lp->nr_lagrange; i++)
      lp->scale[i] = lp->scale[i + 1];
  }

  for(i = 0; i < lp->nr_lagrange; i++)
    for(j = column; j <= lp->columns; j++)
      lp->lag_row[i][j] = lp->lag_row[i][j+1];

  for(i = lp->columns_used + 1; i <= lp->columns; i++)
    lp->col_end[i] = lp->col_end[i - 1];
  lp->columns_used = lp->columns;

  to_elm = lp->col_end[column-1];
  from_elm = lp->col_end[column];
  elm_in_col = from_elm-to_elm;
  for(i = from_elm; i < lp->non_zeros; i++) {
    lp->mat[to_elm] = lp->mat[i];
    to_elm++;
  }
  for(i = column; i < lp->columns; i++) {
    lp->col_end[i] = lp->col_end[i + 1] - elm_in_col;
    lp->must_be_int[i] = lp->must_be_int[i + 1];
    lp->var_is_sc[i] = lp->var_is_sc[i + 1];
    if(lp->var_is_free != NULL)
      lp->var_is_free[i] = lp->var_is_free[i + 1];
  }
  lp->non_zeros -= elm_in_col;
  lp->row_end_valid = FALSE;
  lp->eta_valid = FALSE;

  lp->sum--;
  lp->columns--;
  lp->columns_used--;

  if(!forpresolve) {
    for(j = 1, i = 1; i <= lp->orig_columns; i++) {
      lp->orig_to_var[lp->orig_rows + j] = lp->orig_to_var[lp->orig_rows + i];
      if(lp->orig_to_var[lp->orig_rows + j] != column)
        j++;
    }
    lp->orig_columns--;

    for(i = column; i <= lp->columns; i++)
      lp->var_to_orig[lp->rows + i]--;
  }

  for(i = 1; i <= lp->orig_columns; i++)
    if(lp->orig_to_var[lp->orig_rows + i] > column)
      lp->orig_to_var[lp->orig_rows + i]--;

  return(TRUE);
}

int del_column(lprec *lp, int column)
{
  return(del_columnex(lp, column, FALSE));
}

void set_bounds_tighter(lprec *lp, MYBOOL tighten)
{
  lp->tighten_on_set = tighten;
}

MYBOOL get_bounds_tighter(lprec *lp)
{
  return(lp->tighten_on_set);
}

int set_upbo(lprec *lp, int column, REAL value)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "set_upbo: Column %d out of range", column);
    return(FALSE);
  }
  if(value < lp->infinite && lp->scaling_used)
    value /= lp->scale[lp->rows + column];

  if(lp->tighten_on_set) {
    if(value < lp->orig_lowbo[lp->rows + column]) {
      report(lp, IMPORTANT, "set_upbo: Upperbound must be >= lowerbound\n");
      return(FALSE);
    }
    if(value < lp->orig_upbo[lp->rows + column]) {
      lp->eta_valid = FALSE;
      lp->orig_upbo[lp->rows + column] = value;
    }
  }
  else
  {
    /* allow to set a value larger than the previous set bound. It is up to the user to check if this may happen */
    /* he can use get_upbo() to get the previous bound and determine of he may set it or not */
    /* if(value < lp->orig_upbo[lp->rows + column]) */
    lp->eta_valid = FALSE;
    lp->orig_upbo[lp->rows + column] = value;
  }
  return(TRUE);
}

REAL get_upbo(lprec *lp, int column)
{
  REAL value;
  value = lp->orig_upbo[lp->rows + column];
  if((value<lp->infinite) && lp->columns_scaled) {
    value *= lp->scale[lp->rows + column];
  }
  return(value);
}

int set_lowbo(lprec *lp, int column, REAL value)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "set_lowbo: Column %d out of range", column);
    return(FALSE);
  }
  if(value > -lp->infinite && lp->scaling_used)
    value /= lp->scale[lp->rows + column];

  if(lp->tighten_on_set) {
    if(value > lp->orig_upbo[lp->rows + column]) {
      report(lp, IMPORTANT, "set_lowbo: Upper bound must be >= lower bound\n");
      return(FALSE);
    }
    if((value < 0) || (value > lp->orig_lowbo[lp->rows + column])) {
      lp->eta_valid = FALSE;
      lp->orig_lowbo[lp->rows + column] = value;
    }
  }
  else
  {
    /* allow to set a value smaller than the previous set bound. It is up to the user to check if this may happen */
    /* he can use get_lowbo() to get the previous bound and determine of he may set it or not */
    /* if((value < 0) || (value > lp->orig_lowbo[lp->rows + column])) */
    lp->eta_valid = FALSE;
    lp->orig_lowbo[lp->rows + column] = value;
  }
  return(TRUE);
}

REAL get_lowbo(lprec *lp, int column)
{
  REAL value;
  value = lp->orig_lowbo[lp->rows + column];
  if((value<lp->infinite) && lp->columns_scaled) {
    value *= lp->scale[lp->rows + column];
  }
  return(value);
}

int set_bounds(lprec *lp, int column, REAL lower, REAL upper)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "set_bounds: Column %d out of range", column);
    return(FALSE);
  }
  if(lp->scaling_used) {
    if(lower > -lp->infinite)
      lower /= lp->scale[lp->rows + column];
    if(upper < lp->infinite)
      upper /= lp->scale[lp->rows + column];
  }
  /* following test is moved to isvalid() routine */
  /*
  if(lower > upper)
    report(lp, IMPORTANT, "set_bounds: Column %d Upper bound must be >= lower bound", column);
  */
  lp->eta_valid = FALSE;
  lp->orig_lowbo[lp->rows+column] = lower;
  lp->orig_upbo[lp->rows+column] = upper;
  return(TRUE);
}

int get_bounds(lprec *lp, int column, REAL *lower, REAL *upper)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "get_bounds: Column %d out of range", column);
    return(FALSE);
  }

  if(lower != NULL)
    *lower = get_lowbo(lp, column);
  if(upper != NULL)
    *upper = get_upbo(lp, column);

  return(TRUE);
}

int set_uprange(lprec *lp, int row, REAL value)
{
  if(row > lp->rows || row < 1) {
    report(lp, CRITICAL, "set_uprange: row %d out of range", row);
    return(FALSE);
  }
  if(lp->scaling_used)
    value *= lp->scale[row];
  /* following test is moved to isvalid() routine */
  /*
  if(value < lp->orig_lowbo[row]) {
    report(lp, CRITICAL, "set_uprange: Upperbound must be >= lowerbound");
    return(FALSE);
  }
  */
  /* if(value<lp->orig_upbo[row]) */
  {
    lp->eta_valid = FALSE;
    lp->orig_upbo[row] = value;
  }
  return(TRUE);
}

int set_lowrange(lprec *lp, int row, REAL value)
{
  if(row > lp->rows || row < 1) {
    report(lp, CRITICAL, "set_lowrange: row %d out of range", row);
    return(FALSE);
  }
  if(lp->scaling_used)
    value *= lp->scale[row];
  /* following test is moved to isvalid() routine */
  /*
  if(value > lp->orig_upbo[row]) {
    report(lp, CRITICAL, "set_lowrange: Upperbound must be >= lowerbound");
    return(FALSE);
  }
  */
  /*
    if(value < 0) {
     report(lp, CRITICAL, "set_lowrange: Lower bound cannot be < 0");
     return(FALSE);
    }
    */
  /* if((value<0) || (value>lp->orig_lowbo[row])) */
  {
    lp->eta_valid = FALSE;
    lp->orig_lowbo[row] = value;
  }
  return(TRUE);
}

int set_int(lprec *lp, int column, short must_be_int)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "set_int: Column %d out of range", column);
    return(FALSE);
  }
  if(lp->must_be_int[column] & ISINTEGER) {
    lp->int_count--;
    lp->must_be_int[column] &= !ISINTEGER;
  }
  if(must_be_int) {
    lp->must_be_int[column] |= ISINTEGER;
    lp->int_count++;
    if(lp->scaling_used && lp->columns_scaled && ((lp->scalemode & INTEGERSCALE) == 0))
      unscale_columns(lp);
  }
  return(TRUE);
}

void set_verbose(lprec *lp, short verbose)
{
  lp->verbose = verbose;
}

short get_verbose(lprec *lp)
{
  return(lp->verbose);
}

void set_timeout(lprec *lp, long sectimeout)
{
  lp->sectimeout = sectimeout;
}

long get_timeout(lprec *lp)
{
  return(lp->sectimeout);
}

void put_abortfunc(lprec *lp, abortfunc newabort, void *aborthandle)
{
  lp->abort = newabort;
  lp->aborthandle = aborthandle;
}

void put_logfunc(lprec *lp, logfunc newlog, void *loghandle)
{
  lp->writelog = newlog;
  lp->loghandle = loghandle;
}

void put_msgfunc(lprec *lp, msgfunc newmsg, void *msghandle, int mask)
{
  lp->usermessage = newmsg;
  lp->msgmask = mask;
  lp->msghandle = msghandle;
}

void set_print_sol(lprec *lp, short print_sol)
{
  lp->print_sol = (MYBOOL) print_sol;
}

short is_print_sol(lprec *lp)
{
  return(lp->print_sol);
}

void set_debug(lprec *lp, short debug)
{
  lp->debug = (MYBOOL) debug;
}

short is_debug(lprec *lp)
{
  return(lp->debug);
}

void set_print_at_invert(lprec *lp, short print_at_invert)
{
  lp->print_at_invert = print_at_invert;
}

short is_print_at_invert(lprec *lp)
{
  return(lp->print_at_invert);
}

void set_trace(lprec *lp, short trace)
{
  lp->trace = (MYBOOL) trace;
}

short is_trace(lprec *lp)
{
  return(lp->trace);
}

void set_anti_degen(lprec *lp, short anti_degen)
{
  lp->anti_degen = (MYBOOL) anti_degen;
}

short is_anti_degen(lprec *lp)
{
  return(lp->anti_degen);
}

void set_do_presolve(lprec *lp, short do_presolve)
{
  lp->do_presolve = (MYBOOL) do_presolve;
}

short is_do_presolve(lprec *lp)
{
  return(lp->do_presolve);
}

void set_max_num_inv(lprec *lp, int max_num_inv)
{
  lp->max_num_inv = max_num_inv;
}

int get_max_num_inv(lprec *lp)
{
  return(lp->max_num_inv);
}

void set_bb_rule(lprec *lp, short bb_rule)
{
  lp->bb_rule = (MYBOOL) bb_rule;
}

short get_bb_rule(lprec *lp)
{
  return(lp->bb_rule);
}

void set_obj_bound(lprec *lp, REAL obj_bound)
{
  lp->obj_bound = obj_bound;
}

REAL get_obj_bound(lprec *lp)
{
  return(lp->obj_bound);
}

void set_mip_gap(lprec *lp, REAL mip_gap)
{
  lp->mip_gap = mip_gap;
}

REAL get_mip_gap(lprec *lp)
{
  return(lp->mip_gap);
}

void set_floor_first(lprec *lp, short floor_first)
{
  lp->floor_first = (MYBOOL) floor_first;
}

short get_floor_first(lprec *lp)
{
  return(lp->floor_first);
}

void set_infinite(lprec *lp, REAL infinite)
{
  set_infinite_ext(lp, infinite, FALSE);
}

REAL get_infinite(lprec *lp)
{
  return(lp->infinite);
}

void set_epsilon(lprec *lp, REAL epsilon)
{
  lp->epsilon = epsilon;
}

REAL get_epsilon(lprec *lp)
{
  return(lp->epsilon);
}

void set_epsb(lprec *lp, REAL epsb)
{
  lp->epsb = epsb;
}

REAL get_epsb(lprec *lp)
{
  return(lp->epsb);
}

void set_epsd(lprec *lp, REAL epsd)
{
  lp->epsd = epsd;
}

REAL get_epsd(lprec *lp)
{
  return(lp->epsd);
}

void set_epsel(lprec *lp, REAL epsel)
{
  lp->epsel = epsel;
}

REAL get_epsel(lprec *lp)
{
  return(lp->epsel);
}

void set_scalemode(lprec *lp, short scalemode)
{
  lp->scalemode = (MYBOOL) scalemode;
}

short get_scalemode(lprec *lp)
{
  return(lp->scalemode);
}

void set_improve(lprec *lp, short improve)
{
  lp->improve = (MYBOOL) improve;
}

short get_improve(lprec *lp)
{
  return(lp->improve);
}

void set_lag_trace(lprec *lp, short lag_trace)
{
  lp->lag_trace = (MYBOOL) lag_trace;
}

short is_lag_trace(lprec *lp)
{
  return(lp->lag_trace);
}

void set_piv_rule(lprec *lp, short piv_rule)
{
  lp->piv_rule = (MYBOOL) piv_rule;
}

short get_piv_rule(lprec *lp)
{
  return(lp->piv_rule);
}

void set_break_at_first(lprec *lp, short break_at_first)
{
  lp->break_at_first = (MYBOOL) break_at_first;
}

short is_break_at_first(lprec *lp)
{
  return(lp->break_at_first);
}

void set_break_at_value(lprec *lp, REAL break_at_value)
{
  lp->break_at_value = break_at_value;
}

REAL get_break_at_value(lprec *lp)
{
  return(lp->break_at_value);
}

void set_negrange(lprec *lp, REAL negrange)
{
  if(negrange <= 0)
    lp->negrange = negrange;
  else
    lp->negrange = 0.0;
}

REAL get_negrange(lprec *lp)
{
  return(lp->negrange);
}

void set_splitnegvars(lprec *lp, short splitnegvars)
{
  lp->splitnegvars = splitnegvars;
}

short get_splitnegvars(lprec *lp)
{
  return(lp->splitnegvars);
}

void set_epsperturb(lprec *lp, REAL epsperturb)
{
  lp->epsperturb = epsperturb;
}

REAL get_epsperturb(lprec *lp)
{
  return(lp->epsperturb);
}

void set_epspivot(lprec *lp, REAL epspivot)
{
  lp->epspivot = epspivot;
}

REAL get_epspivot(lprec *lp)
{
  return(lp->epspivot);
}

int get_max_level(lprec *lp)
{
  return(lp->max_level);
}

int get_total_nodes(lprec *lp)
{
  return(lp->total_nodes);
}

int get_total_iter(lprec *lp)
{
  return(lp->total_iter);
}

REAL get_objective(lprec *lp)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_objective: Not a valid basis");
    return(0.0);
  }

  return(*(lp->best_solution));
}

int get_variables(lprec *lp, REAL *var)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_variables: Not a valid basis");
    return(FALSE);
  }

  memcpy(var, lp->best_solution + (1 + lp->rows), lp->columns * sizeof(*(lp->best_solution)));
  return(TRUE);
}

int get_ptr_variables(lprec *lp, REAL **var)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_ptr_variables: Not a valid basis");
    return(FALSE);
  }

  if(var != NULL)
   *var = lp->best_solution + (1 + lp->rows);
  return(TRUE);
}

int get_constraints(lprec *lp, REAL *constr)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_constraints: Not a valid basis");
    return(FALSE);
  }

  memcpy(constr, lp->best_solution + 1, lp->rows * sizeof(*(lp->best_solution)));
  return(TRUE);
}

int get_ptr_constraints(lprec *lp, REAL **constr)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_ptr_constraints: Not a valid basis");
    return(FALSE);
  }

  if(constr != NULL)
   *constr = lp->best_solution + 1;
  return(TRUE);
}

int get_lambda(lprec *lp, REAL *lambda)
{
  if(!lp->basis_valid || lp->nr_lagrange == 0) {
    report(lp, CRITICAL, "get_lambda: Not a valid basis");
    return(FALSE);
  }

  memcpy(lambda, lp->lambda, (lp->nr_lagrange) * sizeof(*lambda));
  return(TRUE);
}

int get_ptr_lambda(lprec *lp, REAL **lambda)
{
  if(!lp->basis_valid || lp->nr_lagrange == 0) {
    report(lp, CRITICAL, "get_ptr_lambda: Not a valid basis");
    return(FALSE);
  }
  if(lambda != NULL)
   *lambda = lp->lambda;
  return(TRUE);
}

int get_orig_index(lprec *lp, int lp_index)
{
  if((lp_index >= 1) && (lp_index <= lp->rows + lp->columns))
    return(lp->var_to_orig[lp_index]);
  else
    return(0);
}

int get_lp_index(lprec *lp, int orig_index)
{
  if((orig_index >=1) && (orig_index <= lp->orig_rows + lp->orig_columns))
    return(lp->orig_to_var[orig_index]);
  else
    return(0);
}

int get_sensitivity_rhs(lprec *lp, REAL *duals, REAL *dualsfrom, REAL *dualstill)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_sensitivity_rhs: Not a valid basis");
    return(FALSE);
  }

  if(duals != NULL)
   memcpy(duals, lp->duals + 1, lp->sum * sizeof(*(lp->duals)));
  if(dualsfrom != NULL)
   memcpy(dualsfrom, lp->dualsfrom + 1, lp->sum * sizeof(*(lp->dualsfrom)));
  if(dualstill != NULL)
   memcpy(dualstill, lp->dualstill + 1, lp->sum * sizeof(*(lp->dualstill)));
  return(TRUE);
}

int get_ptr_sensitivity_rhs(lprec *lp, REAL **duals, REAL **dualsfrom, REAL **dualstill)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_ptr_sensitivity_rhs: Not a valid basis");
    return(FALSE);
  }

  if(duals != NULL)
   *duals = lp->duals + 1;
  if(dualsfrom != NULL)
   *dualsfrom = lp->dualsfrom + 1;
  if(dualstill != NULL)
   *dualstill = lp->dualstill + 1;
  return(TRUE);
}

int get_sensitivity_obj(lprec *lp, REAL *objfrom, REAL *objtill)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_sensitivity_obj: Not a valid basis");
    return(FALSE);
  }

  if(objfrom != NULL)
   memcpy(objfrom, lp->objfrom + 1, lp->columns * sizeof(*(lp->objfrom)));
  if(objtill != NULL)
   memcpy(objtill, lp->objtill + 1, lp->columns * sizeof(*(lp->objtill)));
  return(TRUE);
}

int get_ptr_sensitivity_obj(lprec *lp, REAL **objfrom, REAL **objtill)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_ptr_sensitivity_obj: Not a valid basis");
    return(FALSE);
  }

  if(objfrom != NULL)
   *objfrom = lp->objfrom + 1;
  if(objtill != NULL)
   *objtill = lp->objtill + 1;
  return(TRUE);
}

int get_sensitivity_objex(lprec *lp, REAL *objfrom, REAL *objtill, REAL *objfromvalue, REAL *objtillvalue)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_sensitivity_objex: Not a valid basis");
    return(FALSE);
  }

  if(objfrom != NULL)
   memcpy(objfrom, lp->objfrom + 1, lp->columns * sizeof(*(lp->objfrom)));
  if(objtill != NULL)
   memcpy(objtill, lp->objtill + 1, lp->columns * sizeof(*(lp->objtill)));

  if(objfromvalue != NULL)
   memcpy(objfromvalue, lp->objfromvalue + 1, lp->columns * sizeof(*(lp->objfromvalue)));
/*
  if(objtillvalue != NULL)
   memcpy(objtillvalue, lp->objtillvalue + 1, lp->columns * sizeof(*(lp->objtillvalue)));
*/
  return(TRUE);
}

int get_ptr_sensitivity_objex(lprec *lp, REAL **objfrom, REAL **objtill, REAL **objfromvalue, REAL **objtillvalue)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_ptr_sensitivity_objex: Not a valid basis");
    return(FALSE);
  }

  if(objfrom != NULL)
   *objfrom = lp->objfrom + 1;
  if(objtill != NULL)
   *objtill = lp->objtill + 1;

  if(objfromvalue != NULL)
   *objfromvalue = lp->objfromvalue + 1;
  if(objtillvalue != NULL)
   *objtillvalue = /* lp->objtillvalue + 1 */ NULL;

  return(TRUE);
}

int get_primal_solution(lprec *lp, REAL *pv)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_primal_solution: Not a valid basis");
    return(FALSE);
  }

  memcpy(pv, lp->best_solution, (lp->sum + 1) * sizeof(*pv));
  return(TRUE);
}

int get_ptr_primal_solution(lprec *lp, REAL **pv)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_primal_solution: Not a valid basis");
    return(FALSE);
  }

  if(pv != NULL)
   *pv = lp->best_solution;

  return(TRUE);
}

int get_work_solution(lprec *lp, REAL *wv)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_work_solution: Not a valid basis");
    return(FALSE);
  }

  memcpy(wv, lp->solution, (lp->sum + 1) * sizeof(*wv));
  return(TRUE);
}

int get_ptr_work_solution(lprec *lp, REAL **wv)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_ptr_work_solution: Not a valid basis");
    return(FALSE);
  }

  if(wv != NULL)
   *wv = lp->solution;

  return(TRUE);
}

void set_solutionlimit(lprec *lp, int limit)
{
  lp->solutionlimit = limit;
}

int get_solutionlimit(lprec *lp)
{
  return(lp->solutionlimit);
}

int get_solutioncount(lprec *lp)
{
  return(lp->solutioncount);
}

int get_Nrows(lprec *lp)
{
  return(lp->rows);
}

int get_Norig_rows(lprec *lp)
{
  return(lp->orig_rows);
}

int get_Lrows(lprec *lp)
{
  return(lp->nr_lagrange);
}

int get_Ncolumns(lprec *lp)
{
  return(lp->columns);
}

int get_Norig_columns(lprec *lp)
{
  return(lp->orig_columns);
}

static int SortByINT(int *item, int *weight, int size, int offset, MYBOOL unique)
{
  int i, ii, saveI;
  int saveW;

  for(i = 1; i < size; i++) {
    ii = i+offset-1;
    while (ii > 0 && weight[ii] >= weight[ii+1]) {
      if(unique && (weight[ii] == weight[ii+1]))
        return(item[ii]);
      saveI = item[ii];
      saveW = weight[ii];
      item[ii] = item[ii+1];
      weight[ii] = weight[ii+1];
      item[ii+1] = saveI;
      weight[ii+1] = saveW;
      ii--;
    }
  }
  return(0);
}

static int SearchFor(int target, int *attributes, int size, int offset, MYBOOL absolute)
{
  int beginPos, endPos;
  int newPos, match;

 /* Set starting and ending index offsets */
  beginPos = offset;
  endPos = beginPos + size - 1;

 /* Do binary search logic based on a sorted attribute vector */
  newPos = (beginPos + endPos) / 2;
  match = attributes[newPos];
  if(absolute)
    match = abs(match);
  while(endPos - beginPos > LINEARSEARCH) {
    if(match < target) {
      beginPos = newPos + 1;
      newPos = (beginPos + endPos) / 2;
      match = attributes[newPos];
      if(absolute)
	match = abs(match);
    }
    else if(match > target) {
      endPos = newPos - 1;
      newPos = (beginPos + endPos) / 2;
      match = attributes[newPos];
      if(absolute)
	match = abs(match);
    }
    else {
      beginPos = newPos;
      endPos = newPos;
    }
  }

 /* Do linear (unsorted) search logic */
  if(endPos - beginPos <= LINEARSEARCH) {
    match = attributes[beginPos];
    if(absolute)
      match = abs(match);
    while((beginPos < endPos) && (match != target)) {
      beginPos++;
      match = attributes[beginPos];
      if(absolute)
	match = abs(match);
    }
    if(match == target)
      endPos = beginPos;
  }

 /* Return the index if a match was found, or signal failure with a -1 */
  if((beginPos == endPos) && (match == target))
    return(beginPos);
  else
    return(-1);
}

/* SOS record functions */
int append_SOSrec(lprec *lp, SOSrec *SOS, int size, int *variables, REAL *weights)
{
  int i, oldsize, newsize, nn;

  oldsize = SOS->size;
  newsize = oldsize + size;
  nn = abs(SOS->type);

 /* Shift existing active data right (normally zero) */
  if(SOS->members == NULL) {
    if (CALLOC(SOS->members, 1 + newsize + 1 + nn, int) == NULL)
      return(-1);
  }
  else {
    if (REALLOC(SOS->members, 1 + newsize + 1 + nn, int) == NULL)
      return(-1);
    for(i = newsize+1+nn; i > newsize+1; i--)
      SOS->members[i] = SOS->members[i-size];
  }
  SOS->members[0] = newsize;
  SOS->members[newsize+1] = nn;

 /* Copy the new data into the arrays */
  if(SOS->weights == NULL) {
    if (CALLOC(SOS->weights, 1 + newsize, REAL) == NULL)
      return(-1);
  }
  else {
    if (REALLOC(SOS->weights, 1 + newsize, REAL) == NULL)
      return(-1);
  }
  for(i = oldsize+1; i <= newsize; i++) {
    SOS->members[i] = variables[i-oldsize-1];
    if((SOS->members[i] < 1) || (SOS->members[i] > lp->columns))
      report(lp, IMPORTANT, "Invalid SOS variable definition index %d", SOS->members[i]);
    else
      lp->must_be_int[SOS->members[i]] |= ISSOS;
    if(weights == NULL)
      SOS->weights[i] = (REAL) i;  /* Follow standard, which is sorted ascending */
    else
      SOS->weights[i] = weights[i-oldsize-1];
    SOS->weights[0] += SOS->weights[i] /* weights[i-oldsize-1] */;
  }

 /* Sort the new paired lists ascending by weight (simple bubble sort) */
  i = SortByREAL(SOS->members, SOS->weights, newsize, 1, TRUE);
  if(i > 0)
    report(lp, CRITICAL, "Invalid SOS variable weight at index %d", i);

 /* Define mapping arrays to search large SOS's faster */
  if ((REALLOC(SOS->membersSorted, newsize, int) == NULL) || (REALLOC(SOS->membersMapped, newsize, int) == NULL))
    return(-1);
  for(i = oldsize+1; i <= newsize; i++) {
    SOS->membersSorted[i - 1] = SOS->members[i];
    SOS->membersMapped[i - 1] = i;
  }
  SortByINT(SOS->membersMapped, SOS->membersSorted, newsize, 0, TRUE);

 /* Confirm the new size */
  SOS->size = newsize;

  return(newsize);
}

static SOSrec *create_SOSrec(lprec *lp, char *name, short type, int priority, int size, int *variables, REAL *weights)
{
  SOSrec *SOS = NULL;

  if (CALLOC(SOS, 1, SOSrec) != NULL) {
    SOS->type = type;
    if(name == NULL)
      SOS->name = NULL;
    else
    {
      SOS->name = (char *) malloc(strlen(name)+1);
      strcpy(SOS->name, name);
    }
    if(type < 0)
      type = (short) abs(type);
    SOS->tagorder = 0;
    SOS->size = 0;
    SOS->priority = priority;
    SOS->members = NULL;
    SOS->weights = NULL;
    SOS->membersSorted = NULL;
    SOS->membersMapped = NULL;

    if(size > 0)
      if ((size = append_SOSrec(lp, SOS, size, variables, weights)) == -1) {
        FREE(SOS);
      }
  }
  return(SOS);
}

int is_int(lprec *lp, int column)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "is_int: Column %d out of range", column);
    return(FALSE);
  }
  return((lp->must_be_int[column] & ISINTEGER) != 0);
}

int set_varweights(lprec *lp, REAL *weights)
{
  FREE(lp->var_priority);
  if(weights != NULL) {
    int n;

    if (MALLOC(lp->var_priority, lp->columns, int) == NULL) {
      lp->spx_status = OUT_OF_MEMORY;
      return(FALSE);
    }

    for(n = 0; n < lp->columns; n++)
      lp->var_priority[n] = n + 1;
    n = SortByREAL(lp->var_priority, weights + 1, lp->columns, 0, FALSE);
  }
  return(TRUE);
}

int set_semicont(lprec *lp, int column, short must_be_sc)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "set_semicont: Column %d out of range", column);
    return(FALSE);
  }
  if(lp->var_is_sc[column] != 0) {
    lp->sc_count--;
    lp->must_be_int[column] &= !ISSEMI;
  }
  lp->var_is_sc[column] = must_be_sc;
  if(must_be_sc) {
    lp->must_be_int[column] |= ISSEMI;
    lp->sc_count++;
  }
  if(get_lowbo(lp,column) == 0.0)
    set_lowbo(lp, column, 1.0);
  return(TRUE);
}

int is_semicont(lprec *lp, int column)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "is_semicont: Column %d out of range", column);
    return(FALSE);
  }
  return((lp->must_be_int[column] & ISSEMI) != 0);
}

int is_SOS_var(lprec *lp, int column)
{
  if(column > lp->columns || column < 1) {
    report(lp, IMPORTANT, "is_SOS_var: Column %d out of range", column);
    return(FALSE);
  }
  return((lp->must_be_int[column] & ISSOS) != 0);
}

int add_SOS(lprec *lp, char *name, short sostype, int priority, int count, int *sosvars, REAL *weights)
{
  SOSrec *SOS, *SOSHold;
  int i,k;

  if((sostype<1) /* || (sostype>9) */ || (count<0)) {
    report(lp, IMPORTANT, "add_SOS: Invalid SOS type definition %d", sostype);
    return(FALSE);
  }
  /* Make size in the list to handle another SOS record */
  if(lp->sos_alloc == 0) {
    lp->sos_alloc = SOS_START_SIZE;
    if (MALLOC(lp->sos_list, lp->sos_alloc, SOSrec *) == NULL) {
      lp->spx_status = OUT_OF_MEMORY;
      return(FALSE);
    }
  }
  else if(lp->sos_count == lp->sos_alloc) {
    lp->sos_alloc += lp->sos_alloc / RESIZEFACTOR;
    if (REALLOC(lp->sos_list, lp->sos_alloc, SOSrec *) == NULL) {
      lp->spx_status = OUT_OF_MEMORY;
      return(FALSE);
    }
  }

  /* Create and append SOS to list */
  if ((SOS = create_SOSrec(lp, name, sostype, priority, count, sosvars, weights)) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    return(FALSE);
  }
  lp->sos_list[lp->sos_count] = SOS;
  lp->sos_count++;
  k = lp->sos_count;
  SOS->tagorder = k;

  /* Sort the SOS list by given priority */

  for(i = lp->sos_count-1; i > 0; i--)
    if(lp->sos_list[i]->priority < lp->sos_list[i-1]->priority) {
      SOSHold = lp->sos_list[i];
      lp->sos_list[i] = lp->sos_list[i-1];
      lp->sos_list[i-1] = SOSHold;
      if(SOSHold == SOS)
	k = i-1;
    }
    else
      break;

  return(k);
}

int SOS_get_type(lprec *lp, int sosindex)
{
  if((sosindex<1) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_get_type: Invalid SOS index %d", sosindex);
    return(FALSE);
  }
  return(lp->sos_list[sosindex-1]->type);
}

int SOS_infeasible(lprec *lp, int sosindex)
{
  int i, n, nn, failindex;
  int *list;

  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_infeasible: Invalid SOS index %d", sosindex);
    return(FALSE);
  }

  if(sosindex == 0 && lp->sos_count == 1)
    sosindex = 1;

  failindex = 0;
  if(sosindex == 0) {
    for(i = 1; i <= lp->sos_count; i++) {
      failindex = SOS_infeasible(lp, i);
      if(failindex > 0) break;
    }
  }
  else {
    list = lp->sos_list[sosindex-1]->members;
    n = list[0];
    nn = list[n+1];
   /* Find index of next lower-bounded variable */
    for(i = 1; i <= n; i++) {
      if(lp->orig_lowbo[lp->rows + abs(list[i])] > 0)
	break;
    }

   /* Find if there is another lower-bounded variable beyond the type window */
    i = i + nn;
    while(i <= n) {
      if(lp->orig_lowbo[lp->rows + abs(list[i])] > 0)
	break;
      i++;
    }
    if(i <= n)
      failindex = abs(list[i]);
  }
  return(failindex);
}

int SOS_member_index(lprec *lp, int sosindex, int member)
{
  int n;
  SOSrec *SOS;

  SOS = lp->sos_list[sosindex-1];
  n = SOS->members[0];

  n = SearchFor(member, SOS->membersSorted, n, 0, FALSE);
  if(n >= 0)
    n = SOS->membersMapped[n];

  return(n);
}

int SOS_is_member(lprec *lp, int sosindex, int column)
{
  int i;
  short n;
  int *list;

  n = FALSE;
  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_is_member: Invalid SOS index %d", sosindex);
    return(n);
  }

  if(sosindex == 0 && lp->sos_count == 1)
    sosindex = 1;

  if(sosindex == 0) {
    if(lp->must_be_int[column] & ISSOS) {
      for(i = 1; i <= lp->sos_count; i++) {
        n = (short) SOS_is_member(lp, i, column);
	if(n) break;
      }
    }
  }
  else if(lp->must_be_int[column] & ISSOS) {

   /* Search for the variable */
    i = SOS_member_index(lp, sosindex, column);

   /* Signal active status if found, otherwise return FALSE */
    if(i > 0) {
      list = lp->sos_list[sosindex-1]->members;
      if(list[i] < 0)
        n = -TRUE;
      else
	n = TRUE;
    }
  }
  return(n);
}

MYBOOL SOS_is_member_of_type(lprec *lp, int column, short sostype)
{
  int i;

  for(i = 1; i <= lp->sos_count; i++) {
    if((SOS_get_type(lp, i) == sostype) && SOS_is_member(lp, i, column))
      return(TRUE);
  }
  return(FALSE);
}

static MYBOOL SOS_is_active(lprec *lp, int sosindex, int column)
{
  int i, n, nn;
  int *list;

  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_is_active: Invalid SOS index %d", sosindex);
    return(FALSE);
  }
  if(!(lp->must_be_int[column] & ISSOS))
    return(FALSE);

  if((sosindex == 0) && (lp->sos_count == 1))
    sosindex = 1;

  if(sosindex == 0) {
    for(i = 1; i <= lp->sos_count; i++) {
      n = SOS_is_active(lp, i, column);
      if(n) return(TRUE);
    }
  }
  else {

    list = lp->sos_list[sosindex - 1]->members;
    n = list[0] + 1;
    nn = list[n];

   /* Scan the active (non-zero) SOS index list */
    for(i = 1; (i <= nn) && (list[n + i] != 0); i++)
      if(list[n + i] == column) return(TRUE);
  }
  return(FALSE);
}

MYBOOL SOS_is_full(lprec *lp, int sosindex, int column, MYBOOL activeonly)
{
  int i, nn, n;
  int *list;

  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_is_full: Invalid SOS index %d\n", sosindex);
    return(FALSE);
  }

  if(!(lp->must_be_int[column] & ISSOS))
    return(FALSE);

  if(sosindex == 0 && lp->sos_count == 1)
    sosindex = 1;

  if(sosindex == 0) {
    for(i = 1; i<=lp->sos_count; i++) {
      if(SOS_is_full(lp, i, column, activeonly))
        return(TRUE);
    }
  }
  else if(SOS_is_member(lp, sosindex, column)) {

    list = lp->sos_list[sosindex-1]->members;
    n = list[0]+1;
    nn = list[n];

   /* Info: Last item in the active list is non-zero if the current SOS is full */
    if(list[n+nn] != 0)
      return(TRUE);

    if(!activeonly) {
     /* Spool to last active item */
      for(i = nn-1; (i>0) && (list[n+i]==0); i--);
      if(i > 0) {
        nn = nn - i;
        i = SOS_member_index(lp, sosindex, list[n+i]);
        for(; (nn>0) && (list[i]<0); i++, nn--);
        if(nn == 0)
          return(TRUE);
      }
    }
  }

  return(FALSE);
}

MYBOOL SOS_set_marked(lprec *lp, int sosindex, int column, MYBOOL asactive)
{
  int i, n, nn;
  int *list;

  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_set_marked: Invalid SOS index %d", sosindex);
    return(FALSE);
  }

  if(!(lp->must_be_int[column] & ISSOS))
    return(FALSE);

  if(sosindex == 0 && lp->sos_count == 1)
    sosindex = 1;

 /* Define an IBM-"SOS3" member variable temporarily as integer, if it is
    not already a permanent integer; is reset in SOS_unmark */
  if(asactive && !is_int(lp, column) && SOS_is_member_of_type(lp, column, SOS3)) {
    lp->must_be_int[column] |= ISSOSTEMPINT;
    set_int(lp, column, TRUE);
  }

  if(sosindex == 0) {
    nn = TRUE;
    for(i = 1; i<=lp->sos_count; i++) {
      nn = nn && SOS_set_marked(lp, i, column, asactive);
    }
    return((MYBOOL) nn);
  }
  else
  {
    list = lp->sos_list[sosindex-1]->members;
    n = list[0]+1;
    nn = list[n];

   /* Search for the variable */
    i = SOS_member_index(lp, sosindex, column);

   /* First mark active in the set member list as used */
    if((i > 0) && (list[i] > 0))
      list[i] *= -1;
    else
      return(TRUE);

   /* Then move the variable to the live list */
    if(asactive) {
      for(i = 1; i <= nn; i++) {
        if(list[n+i] == column)
          return(FALSE);
	else if(list[n+i] == 0) {
          list[n+i] = column;
          return(FALSE);
	}
      }
    }
    return(TRUE);
  }
}

MYBOOL SOS_unmark(lprec *lp, int sosindex, int column)
{
  int i, n, nn;
  int *list;
  MYBOOL isactive;

  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_unmark: Invalid SOS index %d", sosindex);
    return(FALSE);
  }

  if(!(lp->must_be_int[column] & ISSOS))
    return(FALSE);

  if(sosindex == 0 && lp->sos_count == 1)
    sosindex = 1;

 /* Undefine a SOS3 member variable that has temporarily been set as integer */
  if(lp->must_be_int[column] & ISSOSTEMPINT) {
    lp->must_be_int[column] &= !ISSOSTEMPINT;
    set_int(lp, column, FALSE);
  }

  if(sosindex == 0) {
    nn = TRUE;
    for(i = 1; i<=lp->sos_count; i++)
      nn = nn && SOS_unmark(lp, i, column);
    return((MYBOOL) nn);
  }
  else
  {
    list = lp->sos_list[sosindex-1]->members;
    n = list[0]+1;
    nn = list[n];

   /* Search for the variable */
    i = SOS_member_index(lp, sosindex, column);

   /* Restore sign in main list */
    if((i > 0) && (list[i] < 0))
      list[i] *= -1;
    else
      return(TRUE);

   /* Find the variable in the active list... */
    isactive = SOS_is_active(lp, sosindex, column);
    if(isactive) {
      for(i = 1; i <= nn; i++)
        if(list[n+i] == column)
          break;
     /* ...shrink the list if found, otherwise return error */
      if(i <= nn) {
        for(; i<nn; i++)
	  list[n+i] = list[n+i+1];
        list[n+nn] = 0;
        return(TRUE);
      }
      return(FALSE);
    }
    else
      return(TRUE);
  }
}

int SOS_fix_unmarked(lprec *lp, int variable, int sosindex, REAL *bound, REAL value, MYBOOL isupper, int *diffcount)
{
  int i, ii, count;
  int n, nn, nLeft, nRight;
  int *list;

  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_fix_unmarked: Invalid SOS index %d", sosindex);
    return(FALSE);
  }

  if((sosindex == 0) && (lp->sos_count == 1))
    sosindex = 1;

  count = 0;
  if(sosindex == 0) {
    for(i = 1; i<=lp->sos_count; i++) {
      if(SOS_is_member(lp, i, variable))
      count += SOS_fix_unmarked(lp, variable, i, bound, value, isupper, diffcount);
    }
  }
  else
  {
    list = lp->sos_list[sosindex-1]->members;
    n = list[0]+1;

   /* Count the number of active and free SOS variables */
    nn = list[n];
    for(i = 1; i <= nn; i++) {
      if(list[n+i] == 0)
        break;
    }
    i--;
    i = nn - i;  /* Establish the number of unused slots */

   /* Determine the free SOS variable window */
    if(i == nn) {
      nLeft = 0;
      nRight = SOS_member_index(lp, sosindex, variable);
    }
    else {
      nLeft  = SOS_member_index(lp, sosindex, list[n+1]);
      if(variable == list[n+1])
        nRight = nLeft;
      else
        nRight = SOS_member_index(lp, sosindex, variable);
    }

 /* nLeft = nLeft - i; */  /* Loop 1..(nLeft-1)  *** KE 9.6.2003 commented out since we always step from left-to-right in the SOS list */
    nRight = nRight + i;  /* Loop (nRight+1)..n */

   /* Fix variables outside of the free SOS variable window */
    for(i = 1; i < n; i++)  {
     /* Skip the SOS variable window */
      if((i >= nLeft) && (i<=nRight))
        continue;
     /* Otherwise proceed to set bound */
      ii = list[i];
      if(ii > 0) {
        ii += lp->rows;
        if(bound[ii] != value) {
         /* Verify that we don't violate original bounds */
          if(isupper && value < lp->orig_lowbo[ii])
            return(-ii);
          else if(!isupper && (value > lp->orig_upbo[ii]))
            return(-ii);
         /* OK, set the new bound */
          count++;
          bound[ii] = value;
        }
        if(lp->solution[ii] != value)
          (*diffcount)++;
      }
    }
  }
  return(count);
}

int SOS_is_satisfied(lprec *lp, int sosindex, REAL *solution)
/* The return code is [-2..+2], depending on the type of satisfaction */
{
  int i, n, nn, count;
  int *list;
  short type, status = 0;

  if((sosindex<0) || (sosindex>lp->sos_count)) {
    report(lp, IMPORTANT, "SOS_is_satisfied: Invalid SOS index %d", sosindex);
    return(FALSE);
  }

  if(sosindex == 0 && lp->sos_count == 1)
    sosindex = 1;

  if(sosindex == 0) {
    for(i = 1; i<=lp->sos_count; i++) {
      status = (short) SOS_is_satisfied(lp, i, solution);
      if(status != 0 && status != -1) break;
    }
  }
  else
  {
    type = (short) SOS_get_type(lp, sosindex);
    list = lp->sos_list[sosindex-1]->members;
    n = list[0]+1;
    nn = list[n];

   /* Count the number of active SOS variables */
    for(i = 1; i <= nn; i++) {
      if(list[n+i] == 0)
	break;
    }
    count = i-1;
    if(count == nn)
      status = 0;    /* Set is full    */
    else
      status = -1;   /* Set is partial */

   /* Find index of the first active variable; fail if some are non-zero */
    if(count > 0) {
      nn = list[n+1];
      for(i = 1; i < n; i++) {
        if(abs(list[i]) == nn || (solution[lp->rows + abs(list[i])] != 0))
	  break;
      }
      if(abs(list[i]) != nn)
	status = 2;  /* Set inconsistency error (leading set variables are non-zero) */
      else
      {
       /* Scan the active SOS variables; fail if some are zero */
	while(count > 0) {
	  if(solution[lp->rows + abs(list[i])] == 0)
            break;
          i++;
          count--;
	}
        if(count > 0)
          status = 2; /* Set inconsistency error (active set variables are zero) */
      }
    }
    else
      i = 1;

   /* Scan the trailing set of SOS variables; fail if some are non-zero */
    if(status != 2) {
      n--;
      while(i <= n) {
        if(solution[lp->rows + abs(list[i])] != 0)
	  break;
	i++;
      }
      if(i <= n)
        status = 2;

     /* Code member deficiency for SOS3 separately */
      else if(status == -1 && type <= SOS3)
        status = -2;
    }
  }
  return(status);
}

int set_rh(lprec *lp, int row, REAL value)
{
  if(row > lp->rows || row < 0) {
    report(lp, IMPORTANT, "set_rh: Row %d out of Range", row);
    return(FALSE);
  }

  if (((row == 0) && (!lp->maximise)) ||
      ((row > 0) && lp->ch_sign[row])) /* setting of RHS of OF IS meaningful */
    value = -value;
  if(lp->scaling_used)
    lp->orig_rh[row] = value * lp->scale[row];
  else
    lp->orig_rh[row] = value;
  lp->eta_valid = FALSE;
  return(TRUE);
}

REAL get_rh(lprec *lp, int row)
{
  REAL value;

  if(row > lp->rows || row < 0) {
    report(lp, IMPORTANT, "get_rh: Row %d out of Range", row);
    return(0.0);
  }

  value = lp->orig_rh[row];
  if (((row == 0) && (!lp->maximise)) ||
      ((row > 0) && lp->ch_sign[row]))    /* setting of RHS of OF IS meaningful */
    value = -value;
  if(lp->scaling_used)
    value /= lp->scale[row];
  return(value);
}

REAL get_rh_upper(lprec *lp, int row)
{
  REAL value, valueR;

  value = lp->orig_rh[row];
  if(lp->ch_sign[row]) {
    valueR = get_rh_range(lp, row);
    if(valueR >= lp->infinite)
      return(lp->infinite);
    value = -value;
    value += valueR;
  }
  if(lp->scaling_used)
    value /= lp->scale[row];
  return(value);
}

REAL get_rh_lower(lprec *lp, int row)
{
  REAL value, valueR;

  value = lp->orig_rh[row];
  if(lp->ch_sign[row])
    value = -value;
  else {
    valueR = get_rh_range(lp, row);
    if(valueR >= lp->infinite)
      return(-lp->infinite);
    value -= valueR;
  }
  if(lp->scaling_used)
    value /= lp->scale[row];
  return(value);
}

int set_rh_range(lprec *lp, int row, REAL deltavalue)
{
  if(row > lp->rows || row < 1) {
    report(lp, IMPORTANT, "set_rh_range: Row %d out of range", row);
    return(FALSE);
  }

  if(lp->scaling_used && (my_abs(deltavalue) < lp->infinite))
    deltavalue *= lp->scale[row];
  if(deltavalue > lp->infinite)
    deltavalue = lp->infinite;
  else if(deltavalue < -lp->infinite)
    deltavalue = -lp->infinite;

  if(lp->ch_sign[row]) {
    /* GE */
    lp->orig_upbo[row] = my_abs(deltavalue);
  }
  else if(lp->orig_upbo[row] == 0 && deltavalue >= 0) {
    /*  EQ with positive sign of r value */
    set_constr_type(lp, row, GE);
    lp->orig_upbo[row] = deltavalue;
  }
  else if(lp->orig_upbo[row] == lp->infinite) {
    /* LE */
    lp->orig_upbo[row] = my_abs(deltavalue);
  }
  else if(lp->orig_upbo[row] == 0 && deltavalue < 0) {
    /* EQ with negative sign of r value */
    set_constr_type(lp, row, LE);
    lp->orig_upbo[row] = -deltavalue;
  }
  return(TRUE);
}

REAL get_rh_range(lprec *lp, int row)
{
  if(lp->orig_upbo[row] >= lp->infinite)
    return(lp->orig_upbo[row]);
  else
   return(lp->orig_upbo[row] / (lp->scaling_used ? lp->scale[row] : (REAL)  1.0));
}

void set_rh_vec(lprec *lp, REAL *rh)
{
  int i;
  if(lp->scaling_used) {
    for(i = 1; i <= lp->rows; i++)
      lp->orig_rh[i] = my_chsign(lp->ch_sign[i], rh[i]*lp->scale[i]);
  }
  else
    for(i = 1; i <= lp->rows; i++)
      lp->orig_rh[i] = my_chsign(lp->ch_sign[i], rh[i]);
  lp->eta_valid = FALSE;
}

int str_set_rh_vec(lprec *lp, char *rh_string)
{
  int  i, ok = TRUE;
  REAL *newrh;
  char *p, *newp;

  if (CALLOC(newrh, lp->rows + 1, REAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    ok = FALSE;
  }
  else {
    p = rh_string;

    for(i = 1; i <= lp->rows; i++) {
      newrh[i] = (REAL) strtod(p, &newp);
      if(p == newp) {
	report(lp, IMPORTANT, "str_set_rh_vec: Bad string");
	ok = FALSE;
	lp->spx_status = IGNORED;
	break;
      }
      else
	p = newp;
    }
    if(ok)
      set_rh_vec(lp, newrh);
    free(newrh);
  }
  return(ok);
}

void set_maxim(lprec *lp)
{
  int i;
  if(lp->maximise == FALSE) {
    for(i = 0; i < lp->non_zeros; i++)
      if(lp->mat[i].row_nr == 0)
	lp->mat[i].value *= -1;
    lp->eta_valid = FALSE;
    lp->orig_rh[0] *= -1;
  }
  lp->maximise = TRUE;
  lp->ch_sign[0] = TRUE;
}

void set_minim(lprec *lp)
{
  int i;
  if(lp->maximise == TRUE) {
    for(i = 0; i < lp->non_zeros; i++)
      if(lp->mat[i].row_nr == 0)
	lp->mat[i].value = -lp->mat[i].value;
    lp->eta_valid = FALSE;
    lp->orig_rh[0] *= -1;
  }
  lp->maximise = FALSE;
  lp->ch_sign[0] = FALSE;
}

int set_constr_type_ex(lprec *lp, int fromrow, int torow, short *con_type)
{
  int i, row, ch_sign = FALSE;
  short con_type1;
  REAL *f, *f1;

  if(fromrow > lp->rows || fromrow < 1) {
    report(lp, IMPORTANT, "set_constr_type_ex: Row %d out of range", fromrow);
    return(FALSE);
  }
  else if(torow > lp->rows || torow < 1) {
    report(lp, IMPORTANT, "set_constr_type_ex: Row %d out of range", torow);
    return(FALSE);
  }
  if(!MALLOC(f, torow - fromrow + 1, REAL))
    return(FALSE);

  f1 = f;
  for(row = fromrow; row <= torow; row++) {
    con_type1 = *(con_type++);
    *f1 = 1;
    if(con_type1 == EQ) {
      lp->orig_upbo[row] = 0;
      if(lp->ch_sign[row]) {
        *f1 = -1;
        ch_sign = TRUE;
        lp->ch_sign[row] = FALSE;
        if(lp->orig_rh[row] != 0)
          lp->orig_rh[row] *= -1;
      }
    }
    else if(con_type1 == LE) {
      lp->orig_upbo[row] = lp->infinite;
      if(lp->ch_sign[row]) {
        *f1 = -1;
        ch_sign = TRUE;
        lp->ch_sign[row] = FALSE;
        if(lp->orig_rh[row] != 0)
          lp->orig_rh[row] *= -1;
      }
    }
    else if(con_type1 == GE) {
      lp->orig_upbo[row] = lp->infinite;
      if(!lp->ch_sign[row]) {
        *f1 = -1;
        ch_sign = TRUE;
        lp->ch_sign[row] = TRUE;
        if(lp->orig_rh[row] != 0)
	  lp->orig_rh[row] *= -1;
      }
    }
    else {
      report(lp, IMPORTANT, "set_constr_type: Constraint type %d not (yet) implemented", con_type1);
      return(FALSE);
    }
    f1++;
  }
  if(ch_sign) {
    for(i = 0; i < lp->non_zeros; i++)
      if((lp->mat[i].row_nr >= fromrow) && (lp->mat[i].row_nr <= torow))
	lp->mat[i].value *= f[lp->mat[i].row_nr - fromrow];
    lp->eta_valid = FALSE;
  }
  lp->basis_valid = FALSE;

  FREE(f);

  return(TRUE);
}

int set_constr_type(lprec *lp, int row, short con_type)
{
  return(set_constr_type_ex(lp, row, row, &con_type));
}

int get_constr_type(lprec *lp, int row)
{
  if(row < 0 || row > lp->rows) {
    report(lp, IMPORTANT, "get_constr_type: Row %d out of range", row);
    return(-1);
  }
  if(lp->ch_sign[row])
    return(GE);
  else if(lp->orig_upbo[row] == 0)
    return(EQ);
  else
    return(LE);
}

REAL get_mat_raw(lprec *lp, int row, int column)
{
  int elmnr, inspos;

  elmnr = find_mat_elm(lp, row, column, &inspos);
  if(elmnr >= 0)
    return(lp->mat[elmnr].value);
  else
    return(0);
}

REAL get_mat(lprec *lp, int row, int column)
{
  REAL value;
  int elmnr, inspos;

  elmnr = find_mat_elm(lp, row, column, &inspos);
  if(elmnr >= 0) {
    value = my_chsign(lp->ch_sign[row], lp->mat[elmnr].value);
    if(lp->scaling_used)
      value /= lp->scale[row] * lp->scale[lp->rows + column];
  }
  else
    value = 0;
  return(value);
}

REAL mat_elm(lprec *lp, int row, int column)
{
  return(get_mat(lp, row, column));
}

int get_row(lprec *lp, int row_nr, REAL *row)
{
  int i;

  if(row_nr <0 || row_nr > lp->rows) {
    report(lp, IMPORTANT, "get_row: Row %d out of range", row_nr);
    return(FALSE);
  }
  if(!build_row_end(lp))
    return(FALSE);
if(row_nr==0) {
  for(i = 1; i <= lp->columns; i++)
    row[i] = get_mat(lp,row_nr,i);      /* Simplified by KE */
}
else {
  memset(row, 0, (1 + lp->columns) * sizeof(*row));
  for(i = lp->row_end[row_nr - 1] + 1; i <= lp->row_end[row_nr]; i++)
    row[lp->col_no[i]] = get_mat(lp,row_nr, lp->col_no[i]);
}
  return(TRUE);
}

int get_matrecrow(lprec *lp, int row_nr, matrec *matrecrow)
{
  int i, j;
  REAL a;

  if(row_nr <0 || row_nr > lp->rows) {
    report(lp, IMPORTANT, "get_matrecrow: Row %d out of range", row_nr);
    return(-1);
  }
  if(!build_row_end(lp))
    return(FALSE);
if(row_nr == 0) {
  for(j = 0, i = 1; i <= lp->columns; i++) {
    a = get_mat(lp,row_nr,i);
    if(a) {
      matrecrow[j].row_nr = i;
      matrecrow[j++].value = a;
    }
  }
}
else {
  for(j = 0, i = lp->row_end[row_nr - 1] + 1; i <= lp->row_end[row_nr]; i++) {
    matrecrow[j].row_nr = lp->col_no[i];
    matrecrow[j++].value = get_mat(lp,row_nr, lp->col_no[i]);
  }
}
  return(j);
}

int get_column(lprec *lp, int col_nr, REAL *column)
{
  int i, ii;

  if(col_nr < 1 || col_nr > lp->columns) {
    report(lp, IMPORTANT, "get_column: Column %d out of range", col_nr);
    return(FALSE);
  }
  for(i = 0; i <= lp->rows; i++)
    column[i] = 0;
  for(i = lp->col_end[col_nr - 1]; i < lp->col_end[col_nr]; i++) {
    ii = lp->mat[i].row_nr;
    column[ii] = my_chsign(lp->ch_sign[ii], lp->mat[i].value);
    if(lp->scaling_used)
      column[ii] /= (lp->scale[ii] * lp->scale[lp->rows + col_nr]);
  }
  return(TRUE);
}

static int mult_column(lprec *lp, int col_nr, REAL mult)
{
  int i;

  if(col_nr < 1 || col_nr > lp->columns) {
    report(lp, IMPORTANT, "mult_column: Column %d out of range", col_nr);
    return(FALSE);
  }
  for(i = lp->col_end[col_nr - 1]; i < lp->col_end[col_nr]; i++)
    lp->mat[i].value *= mult;
  for(i = 0; i < lp->nr_lagrange; i++)
    lp->lag_row[i][col_nr] *= mult;
  return(TRUE);
}

int get_reduced_costs(lprec *lp, REAL *rc)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_reduced_costs: Not a valid basis");
    return(FALSE);
  }

  memcpy(rc, lp->duals, (lp->sum + 1) * sizeof(*rc));
  return(TRUE);
}

int get_ptr_reduced_costs(lprec *lp, REAL **rc)
{
  if(!lp->basis_valid) {
    report(lp, CRITICAL, "get_ptr_reduced_costs: Not a valid basis");
    return(FALSE);
  }

  if(rc != NULL)
    *rc = lp->duals;
  return(TRUE);
}

int is_feasible(lprec *lp, REAL *values)
{
  int i, elmnr;
  REAL *this_rhs;
  REAL dist;

  if(lp->scaling_used) {
    for(i = lp->rows + 1; i <= lp->sum; i++)
      if(values[i - lp->rows] < lp->orig_lowbo[i] * lp->scale[i]
	 || values[i - lp->rows] > lp->orig_upbo[i]  * lp->scale[i]) {
        if(!((lp->var_is_sc[i - lp->rows]>0) && (values[i - lp->rows]==0)))
          return(FALSE);
      }
  }
  else {
    for(i = lp->rows + 1; i <= lp->sum; i++)
      if(values[i - lp->rows] < lp->orig_lowbo[i]
	 || values[i - lp->rows] > lp->orig_upbo[i]) {
        if(!((lp->var_is_sc[i - lp->rows]>0) && (values[i - lp->rows]==0)))
	  return(FALSE);
      }
  }
  if (CALLOC(this_rhs, lp->rows + 1, REAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    return(FALSE);
  }

  if(lp->scaling_used && lp->columns_scaled) {
    for(i = 1; i <= lp->columns_used; i++)
      for(elmnr = lp->col_end[i - 1]; elmnr < lp->col_end[i]; elmnr++)
	this_rhs[lp->mat[elmnr].row_nr] += lp->mat[elmnr].value * values[i] /
	  lp->scale[lp->rows + i];
  }
  else {
    for(i = 1; i <= lp->columns_used; i++)
      for(elmnr = lp->col_end[i - 1]; elmnr < lp->col_end[i]; elmnr++)
	this_rhs[lp->mat[elmnr].row_nr] += lp->mat[elmnr].value * values[i];
  }
  for(i = 1; i <= lp->rows; i++) {
    dist = lp->orig_rh[i] - this_rhs[i];
    my_round(dist, 0.001); /* ugly constant, MB */
    if((lp->orig_upbo[i] == 0 && dist != 0) || dist < 0) {
      free(this_rhs);
      return(FALSE);
    }
  }
  free(this_rhs);
  return(TRUE);
}

/* fixed by Enrico Faggiolo */
int column_in_lp(lprec *lp, REAL *testcolumn)
{
  int i, j;
  int nz, ident = 1;
  REAL value;

  for(nz = 0, i = 0; i <= lp->rows; i++)
    if(my_abs(testcolumn[i]) > lp->epsel) nz++;

  for(i = 1; (i <= lp->columns) && (ident); i++) {
    ident = nz;
    for(j = lp->col_end[i - 1]; (j < lp->col_end[i]) && (ident >= 0); j++, ident--) {
      value = lp->mat[j].value;
      if(lp->ch_sign[lp->mat[j].row_nr])
	value = -value;
      if(lp->scaling_used) {
	value /= lp->scale[lp->rows + i];
	value /= lp->scale[lp->mat[j].row_nr];
      }
      value -= testcolumn[lp->mat[j].row_nr];
      if(my_abs(value) > lp->epsel)
        ident = -1;
    }
  }

  return(ident == 0);
}

int set_lp_name(lprec *lp, char *name)
{
  if (name == NULL) {
    FREE(lp->lp_name);
  }
  else {
    if (REALLOC(lp->lp_name, strlen(name) + 1, char) == NULL)
      return(FALSE);
    strcpy(lp->lp_name, name);
  }
  return(TRUE);
}

char *get_lp_name(lprec *lp)
{
  return(lp->lp_name);
}

static int init_rowcol_names(lprec *lp)
{
  int ok = TRUE;

  if(!lp->names_used) {
    if (CALLOC(lp->row_name, lp->rows_alloc + 1, hashelem *) == NULL)
      ok = FALSE;
    else {
      if (CALLOC(lp->col_name, lp->columns_alloc + 1, hashelem *) == NULL) {
        FREE(lp->row_name);
	ok = FALSE;
      }
      else {
	if ((lp->rowname_hashtab = create_hash_table(HASHSIZE, 0)) == NULL) {
	  FREE(lp->col_name);
	  FREE(lp->row_name);
	  ok = FALSE;
	}
	else {
	  if ((lp->colname_hashtab = create_hash_table(HASHSIZE, 1)) == NULL) {
	    free_hash_table(lp->rowname_hashtab);
	    FREE(lp->col_name);
	    FREE(lp->row_name);
	    ok = FALSE;
	  }
	  else
	    lp->names_used = TRUE;
	}
      }
    }
  }
  if (!ok)
    lp->spx_status = OUT_OF_MEMORY;
  return(ok);
}

int set_row_name(lprec *lp, int row, char *new_name)
{
  int ok = TRUE;
  hashelem *hp;

  if(row < 0 || row > lp->rows) {
    report(lp, IMPORTANT, "set_row_name: Row %d out of range", row);
    return(FALSE);
  }

  if(!lp->names_used) {
    if (!init_rowcol_names(lp))
      ok = FALSE;
  }
  if (ok) {
    if ((hp = puthash(new_name, row, lp->row_name, lp->rowname_hashtab)) == NULL) {
      lp->spx_status = OUT_OF_MEMORY;
      ok = FALSE;
    }
  }
  return(ok);
}

static char name[50];

char *get_row_name(lprec *lp, int row)
{
  char *ptr;

  if(row < 0 || row > lp->rows) {
    report(lp, IMPORTANT, "get_row_name: Row %d out of range", row);
    return(NULL);
  }

  if(lp->names_used && (lp->row_name[row] != NULL) && (lp->row_name[row]->name != NULL) && (*(lp->row_name[row]->name))) {
    ptr = lp->row_name[row]->name;
  }
  else
  {
    /* static char name[50]; */

    sprintf(name, ROWNAMEMASK, row);
    ptr = name;
  }
  return(ptr);
}

int set_col_name(lprec *lp, int column, char *new_name)
{
  int ok = TRUE;
  hashelem *hp;

  if(column < 1 || column > lp->columns) {
    report(lp, IMPORTANT, "set_col_name: Column %d out of range", column);
    return(FALSE);
  }

  if(!lp->names_used) {
    if (!init_rowcol_names(lp))
      ok = FALSE;
  }
  if (ok) {
    if ((hp = puthash(new_name, column, lp->col_name, lp->colname_hashtab)) == NULL) {
      lp->spx_status = OUT_OF_MEMORY;
      ok = FALSE;
    }
  }
  return(ok);
}

char *get_col_name(lprec *lp, int column)
{
  char *ptr;

  if(column < 1 || column > lp->columns) {
    report(lp, IMPORTANT, "set_col_name: Column %d out of range", column);
    return(NULL);
  }

  if(lp->names_used && (lp->col_name[column] != NULL) && (lp->col_name[column]->name != NULL) && (*(lp->col_name[column]->name))) {
    ptr = lp->col_name[column]->name;
  }
  else
  {
    /* static char name[50]; */

    sprintf((char *) name, COLNAMEMASK, column);
    ptr = name;
  }
  return(ptr);
}

/* Compute the scale factor by the formulae:
      FALSE: SUM (log |Aij|) ^ 2
      TRUE:  SUM (log |Aij| - RowScale[i] - ColScale[j]) ^ 2 */
REAL CurtisReidMeasure(lprec *lp, MYBOOL _Advanced, REAL *FRowScale, REAL *FColScale)
{
  int  row, col, ent;
  REAL value, logvalue, Result;

  Result = 0;
  for(col = 1; col <= lp->columns_used; col++) {
    for(ent = lp->col_end[col-1]; ent < lp->col_end[col]; ent++) {
      row = lp->mat[ent].row_nr;
      value = (REAL) fabs(lp->mat[ent].value);
      if(value > 0) {
        logvalue = (REAL) log(value);
        if(_Advanced)
          logvalue -= FRowScale[row] + FColScale[col];
        Result += logvalue * logvalue;
      }
    }
  }
  return(Result);
}

/* Implementation of the Curtis-Reid scaling based on the paper
   "On the Automatic Scaling of Matrices for Gaussian
    Elimination," Journal of the Institute of Mathematics and
    Its Applications (1972) 10, 118-124.

    Solve the system | M   E | (r)   (sigma)
                     |       | ( ) = (     )
                     | E^T N | (c)   ( tau )

    by the conjugate gradient method (clever recurrences).

    E is the matrix A with all elements = 1

    M is diagonal matrix of row    counts (RowCount)
    N is diagonal matrix of column counts (ColCount)

    sigma is the vector of row    logarithm sums (RowSum)
    tau   is the vector of column logarithm sums (ColSum)

    r, c are resulting row and column scalings (RowScale, ColScale) */

int CurtisReidScales(lprec *lp, MYBOOL _Advanced, REAL *FRowScale, REAL *FColScale)
{
  int    rowbase, row, col, ent;
  REAL   *RowScalem2, *ColScalem2,
         *RowSum, *ColSum,
         *residual_even, *residual_odd;
  REAL   sk,   qk,   ek,
         skm1, qkm1, ekm1,
         qkm2, qkqkm1, ekm2, ekekm1,
         value, logvalue,
         StopTolerance;
  int    *RowCount, *ColCount;
  int    Result;

  if(CurtisReidMeasure(lp, _Advanced, FRowScale, FColScale) < 0.1 * lp->non_zeros)
    return(0);

  /* Allocate temporary memory and find RowSum and ColSum measures */
  ColScalem2 = NULL;
  RowScalem2 = NULL;
  residual_even = NULL;
  ColCount = NULL;
  ColSum = NULL;
  residual_odd = NULL;
  RowCount = NULL;
  RowSum = NULL;

  if ((CALLOC(RowSum, lp->rows + 1, REAL) == NULL) ||
      (CALLOC(RowCount, lp->rows + 1, int) == NULL) ||
      (CALLOC(residual_odd, lp->rows + 1, REAL) == NULL) ||
      (CALLOC(ColSum, lp->columns + 1, REAL) == NULL) ||
      (CALLOC(ColCount, lp->columns + 1, int) == NULL) ||
      (CALLOC(residual_even, lp->columns + 1, REAL) == NULL) ||
      (MALLOC(RowScalem2, lp->rows + 1, REAL) == NULL) ||
      (MALLOC(ColScalem2, lp->columns + 1, REAL) == NULL)) {
    FREE(ColScalem2);
    FREE(RowScalem2);
    FREE(residual_even);
    FREE(ColCount);
    FREE(ColSum);
    FREE(residual_odd);
    FREE(RowCount);
    FREE(RowSum);
    lp->spx_status = OUT_OF_MEMORY;
    return(0);
  }

  /* Set origin for row scaling (1=constraints only, 0=include OF) */
  rowbase = 0;
  for(row=0; row < rowbase; row++)
    FRowScale[row] = 1;

  for(col=1; col<=lp->columns_used; col++) {
    for(ent=lp->col_end[col-1]; ent<lp->col_end[col]; ent++) {
      row=lp->mat[ent].row_nr;
      if(row>=rowbase) {
        value=(REAL) fabs(lp->mat[ent].value);
        if(value>0) {
          logvalue=(REAL) log(value);
          ColSum[col]+=logvalue;
          RowSum[row]+=logvalue;
          ColCount[col]++;
          RowCount[row]++;
        }
      }
    }
  }

  /* Make sure we dont't have division by zero errors */
  for(row=rowbase; row<=lp->rows; row++)
    if(RowCount[row]==0)
      RowCount[row]=1;
  for(col=1; col<=lp->columns; col++)
    if(ColCount[col]==0)
      ColCount[col]=1;

  /* Initialize to RowScale = RowCount-1 RowSum
                   ColScale = 0.0
                   residual = ColSum - ET RowCount-1 RowSum */
  StopTolerance= (REAL) SCALINGEPS * (REAL) lp->non_zeros;
  for(row=rowbase; row<=lp->rows; row++) {
    FRowScale[row]=RowSum[row] / (REAL) RowCount[row];
    RowScalem2[row]=FRowScale[row];
  }

  /* Compute initial residual */
  for(col=1; col<=lp->columns; col++) {
    FColScale[col]=0;
    ColScalem2[col]=0;
    residual_even[col]=ColSum[col];
    for(ent=lp->col_end[col-1]; ent<lp->col_end[col]; ent++) {
      row=lp->mat[ent].row_nr;
      if(row>=rowbase)
        residual_even[col]-=RowSum[row] / (REAL) RowCount[row];
    }
  }

  /* Compute sk */
  sk=0;
  skm1=0;
  for(col=1; col<=lp->columns; col++)
    sk+=(residual_even[col]*residual_even[col]) / (REAL) ColCount[col];

  Result=0;
  qk=1; qkm1=0; qkm2=0;
  ek=0; ekm1=0; ekm2=0;

  while(sk>StopTolerance) {
  /* Given the values of residual and sk, construct
     ColScale (when pass is even)
     RowScale (when pass is odd)  */

    qkqkm1 = qk * qkm1;
    ekekm1 = ek * ekm1;
    if((Result % 2) == 0) { /* pass is even; construct RowScale[pass+1] */
      if(Result != 0) {
  	for(row=rowbase; row<=lp->rows; row++)
          RowScalem2[row]=FRowScale[row];
        if(qkqkm1 != 0) {
	  for(row=rowbase; row<=lp->rows; row++)
	    FRowScale[row]*=(1 + ekekm1 / qkqkm1);
	  for(row=rowbase; row<=lp->rows; row++)
	    FRowScale[row]+=(residual_odd[row] / (qkqkm1 * (REAL) RowCount[row]) -
                            RowScalem2[row] * ekekm1 / qkqkm1);
	}
      }
    }
    else { /* pass is odd; construct ColScale[pass+1] */
      for(col=1; col<=lp->columns; col++)
 	ColScalem2[col]=FColScale[col];
      if(qkqkm1 != 0) {
        for(col=1; col<=lp->columns; col++)
          FColScale[col]*=(1 + ekekm1 / qkqkm1);
        for(col=1; col<=lp->columns; col++)
          FColScale[col]+=(residual_even[col] / ((REAL) ColCount[col] * qkqkm1) -
	                  ColScalem2[col] * ekekm1 / qkqkm1);
      }
    }

    /* update residual and sk (pass + 1) */
    if((Result % 2) == 0) { /* even */
       /* residual */
      for(row=rowbase; row<=lp->rows; row++)
        residual_odd[row]*=ek;
      for(col=1; col<=lp->columns_used; col++)
	for(ent=lp->col_end[col-1]; ent<lp->col_end[col]; ent++) {
	  row=lp->mat[ent].row_nr;
    	  if(row>=rowbase)
            residual_odd[row]+=(residual_even[col] / (REAL) ColCount[col]);
        }
      for(row=rowbase; row<=lp->rows; row++)
        residual_odd[row]*=(-1 / qk);

      /* sk */
      skm1=sk;
      sk=0;
      for(row=rowbase; row<=lp->rows; row++)
	sk+=(residual_odd[row]*residual_odd[row]) / (REAL) RowCount[row];
    }
    else { /* odd */
      /* residual */
      for(col=1; col<=lp->columns; col++)
        residual_even[col]*=ek;
      for(col=1; col<=lp->columns_used; col++)
	for(ent=lp->col_end[col-1]; ent<lp->col_end[col]; ent++) {
	  row=lp->mat[ent].row_nr;
 	  if(row>=rowbase)
            residual_even[col]+=(residual_odd[row] / (REAL) RowCount[row]);
        }
      for(col=1; col<=lp->columns; col++)
        residual_even[col]*=(-1 / qk);

      /* sk */
      skm1=sk;
      sk=0;
      for(col=1; col<=lp->columns; col++)
	sk+=(residual_even[col]*residual_even[col]) / (REAL) ColCount[col];
    }

    /* Compute ek and qk */
    ekm2=ekm1;
    ekm1=ek;
    ek=qk * sk / skm1;

    qkm2=qkm1;
    qkm1=qk;
    qk=1-ek;

    Result++;
  }

  /* Synchronize the RowScale and ColScale vectors */
  ekekm1 = ek * ekm1;
  if(qkm1 != 0) {
    if((Result % 2) == 0) { /* pass is even, compute RowScale */
      for(row=rowbase; row<=lp->rows; row++)
        FRowScale[row]*=((REAL) 1.0 + ekekm1 / qkm1);
      for(row=rowbase; row<=lp->rows; row++)
        FRowScale[row]+=(residual_odd[row] / (qkm1 * (REAL) RowCount[row]) -
                        RowScalem2[row] * ekekm1 / qkm1);
    }
    else { /* pass is odd, compute ColScale */
      for(col=1; col<=lp->columns; col++)
        FColScale[col]*=(1 + ekekm1 / qkm1);
      for(col=1; col<=lp->columns; col++)
        FColScale[col]+=(residual_even[col] / ((REAL) ColCount[col] * qkm1) -
                         ColScalem2[col] * ekekm1 / qkm1);
    }
  }

  /* CHECK: M RowScale + E ColScale = RowSum */
/* double check, error = 0;
  for(row=rowbase; lp->rows; row++) {
    check=(REAL) RowCount[row] * FRowScale[row];
    for (ent = AT->pBeginRow[row]-1; ent <= AT->pEndRow[row]-1; ent++) {
      col = AT->Row[ent] - 1;
      check += ColScale[col];
    }
    check-=RowSum[row];
    error+=check*check;
  }
  */

  /* CHECK: E^T RowScale + N ColScale = ColSum */
/*  error=0;
  for(col=1; col<=lp->columns; col++) {
    check=(REAL) ColCount[col] * FColScale[col];
    for(ent= lp->col_end[col-1]; ent<lp->col_end[col]; ent++) {
      row=lp->mat[ent].row_nr;
      if(row>=rowbase)
        check+=FRowScale[row];
    }
    check-=ColSum[col];
    error+=check*check;
  } */

  /* Convert to scaling factors */
  for(col=1; col<=lp->columns; col++) {
    value=(REAL) exp(-FColScale[col]);
    if(value<MINSCALAR) value=(REAL) MINSCALAR;
    if(value>MAXSCALAR) value=(REAL) MAXSCALAR;
    if(!is_int(lp,col) || (lp->scalemode & INTEGERSCALE))
      FColScale[col]=value;
    else
      FColScale[col]=1;
  }
  for(row=rowbase; row<=lp->rows; row++) {
    value=(REAL) exp(-FRowScale[row]);
    if(value<MINSCALAR) value=(REAL) MINSCALAR;
    if(value>MAXSCALAR) value=(REAL) MAXSCALAR;
    FRowScale[row]=value;
  }

 /* free temporary memory */
  free(RowSum);
  free(ColSum);
  free(RowCount);
  free(ColCount);
  free(residual_even);
  free(residual_odd);
  free(RowScalem2);
  free(ColScalem2);

  return(Result);
}

static void scalecolumns(lprec *lp, REAL *scalechange)
{
  int i,j;

  /* scale matrix entries (including any Lagrangean constraints) */
  for(j = 1; j <= lp->columns; j++) {
    for(i = lp->col_end[j - 1]; i < lp->col_end[j]; i++)
      lp->mat[i].value *= scalechange[j];
    for(i = 0; i < lp->nr_lagrange; i++)
      lp->lag_row[i][j] *= scalechange[j];
  }

  /* scale variable bounds as well */
  for(i = 1, j = lp->rows + 1; j <= lp->sum; i++, j++) { /* was <; changed by PN */
    if(lp->orig_lowbo[j] > -lp->infinite)
      lp->orig_lowbo[j] /= scalechange[i];
    if(lp->orig_upbo[j] < lp->infinite)
      lp->orig_upbo[j] /= scalechange[i];

   /* update the pre-existing column scalar */
    lp->scale[j] *= scalechange[i];
  }

  lp->columns_scaled = TRUE;

}

static void scalerows(lprec *lp, REAL *scalechange, REAL *lagscale)
{
  int i, j;

  /* first row-scale the matrix (including the objective function) */
  for(j = 1; j <= lp->columns_used; j++)
    for(i = lp->col_end[j - 1]; i < lp->col_end[j]; i++)
      lp->mat[i].value *= scalechange[lp->mat[i].row_nr];

  /* and scale the rhs and the row bounds (RANGES in MPS!!) */
  for(i = 0; i <= lp->rows; i++) {
    lp->orig_rh[i] *= scalechange[i];

    if(lp->orig_upbo[i] < lp->infinite)     /* This is the range */
      lp->orig_upbo[i] *= scalechange[i];

    if((lp->orig_lowbo[i] != 0) && (my_abs(lp->orig_lowbo[i]) < lp->infinite))
      lp->orig_lowbo[i] *= scalechange[i];

    /* update the pre-existing row scalar */
    lp->scale[i] *= scalechange[i];
  }

  /* scale Lagrangean constraints, if specified */
  if((lp->scalemode & LAGRANGESCALE) && (lagscale != NULL))
    for(i = 0; i < lp->nr_lagrange; i++) {
      lp->lag_rhs[i] *= lagscale[i + 1];

      /* update the pre-existing row scalar */
      lp->scale[lp->sum + 1 + i] *= lagscale[i + 1];
    }
}

int scaleCR(lprec *lp)
{
  REAL *scalechange;
  int  Result;

  if (MALLOC(scalechange, lp->sum + lp->nr_lagrange + 1, REAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    return(-1);
  }

  if(!lp->scaling_used) {
    if (MALLOC(lp->scale, lp->sum_alloc + lp->nr_lagrange + 1, REAL) == NULL) {
      FREE(scalechange);
      lp->spx_status = OUT_OF_MEMORY;
      return(-1);
    }
    for(Result = 0; Result <= lp->sum + lp->nr_lagrange; Result++)
      lp->scale[Result] = 1;
    lp->scaling_used = TRUE;
  }

  Result=CurtisReidScales(lp, FALSE, scalechange, &scalechange[lp->rows]);
  if(Result>0) {
    /* Cannot handle Lagrangean constraint scaling in current version;
       must therefore set scale to default of unit */
    for(Result = 1; Result <= lp->nr_lagrange; Result++)
      scalechange[lp->sum + Result] = 1;

    /* Do the scaling*/
    scalerows(lp, scalechange, NULL);
    scalecolumns(lp, &scalechange[lp->rows]);
    lp->scalemode |= CURTISREIDSCALE;

    lp->eta_valid = FALSE;
  }

  free(scalechange);

  return(Result);
}

static REAL minmax_to_scale(lprec *lp, REAL min, REAL max)
{
  REAL scale;

  /* Compute scalar according to chosen type */
  if(lp->scalemode & GEOSCALING) { /* Do geometric mean scaling */
    if((min == 0) || (max == 0))
      scale = 1;
    else
      scale = (REAL) exp(-max/min);
  }
  else {                            /* Do numerical range-based scaling */
    /* should do something sensible when min or max is 0, MB */
    if(((min == 0) && (max == 0)) || ((min >= lp->infinite) && (max == 0)))
      scale = 1;
    else if((min == 0) || (max == 0))
    /* Changed from scale=1 by KE; seems to improve numerical stability */
      scale = (REAL) (1 / sqrt(max*max + min*min));
    else
    /* scale = 1 / pow(10, (log10(min) + log10(max)) / 2); */
    /* Jon van Reet noticed: can be simplified to: */
      scale = (REAL) (1 / sqrt(min*max));
  }
  scale = (REAL) my_max(scale, MINSCALAR);
  scale = (REAL) my_min(scale, MAXSCALAR);

  /* create a scalar of power 2, if specified; may improve computer numerics */
  if((scale != 1) && (lp->scalemode & POWERSCALE)) {
    int pow,sgn;
    scale = (REAL) (log(scale)/log(2.0));
    if(scale > 0) {
      sgn = 1;
/*      pow = (int) ceil(scale); */   /* Scale more */
      pow = (int) floor(scale);     /* Scale less */
/*      pow = (int) floor(scale+0.5); */ /* Scale average (round) */
    }
    else {
/*      pow = (int) floor(scale); */  /* Scale more */
      pow = (int) ceil(scale);      /* Scale less */
/*      pow = (int) ceil(scale-0.5); */ /* Scale average (round) */
      sgn = -1;
    }

    scale = 1;
    pow = abs(pow);
    while(pow != 0) {
      scale *= 2;
      pow--;
    }
    if(sgn < 0)
      scale = 1 / scale;
  }

  return(scale);
}

void unscale_columns(lprec *lp)
{
  int i, j;

  if(!(lp->scaling_used && lp->columns_scaled)) return;

  /* unscale mat */
  for(j = 1; j <= lp->columns; j++) {
    for(i = lp->col_end[j - 1]; i < lp->col_end[j]; i++)
      lp->mat[i].value /= lp->scale[lp->rows + j];
    for(i = 0; i < lp->nr_lagrange; i++)
      lp->lag_row[i][j] /= lp->scale[lp->rows + j];
  }

  /* unscale bounds as well */
  for(i = lp->rows + 1, j = 1; i <= lp->sum; i++, j++) { /* was < */ /* changed by PN */
    if(lp->orig_lowbo[i] > -lp->infinite)
      lp->orig_lowbo[i] *= lp->scale[i];
    if(lp->orig_upbo[i] < lp->infinite)
      lp->orig_upbo[i] *= lp->scale[i];
    if(lp->var_is_sc[j] != 0)
      lp->var_is_sc[j] *= lp->scale[i];
  }

  for(i = lp->rows + 1; i<= lp->sum; i++)
    lp->scale[i] = 1;
  lp->columns_scaled = FALSE;
  lp->eta_valid = FALSE;
}

void unscale(lprec *lp)
{
  int i, j;

  if(lp->scaling_used) {
    /* unscale matrix columns */
    for(j = 1; j <= lp->columns; j++) {
      for(i = lp->col_end[j - 1]; i < lp->col_end[j]; i++)
	lp->mat[i].value /= lp->scale[lp->rows + j];
      for(i = 0; i < lp->nr_lagrange; i++)
        lp->lag_row[i][j] /= lp->scale[lp->rows + j];
    }

    /* unscale bounds */
    for(i = lp->rows + 1; i <= lp->sum; i++) { /* was <; changed by PN */
      if(lp->orig_lowbo[i] > -lp->infinite)
	lp->orig_lowbo[i] *= lp->scale[i];
      if(lp->orig_upbo[i] < lp->infinite)
	lp->orig_upbo[i] *= lp->scale[i];
    }

    /* unscale matrix rows */
    for(j = 1; j <= lp->columns; j++) {
      for(i = lp->col_end[j-1]; i < lp->col_end[j]; i++)
	lp->mat[i].value /= lp->scale[lp->mat[i].row_nr];
      for(i = 0; i < lp->nr_lagrange; i++)
        lp->lag_row[i][j] /= lp->scale[lp->sum + 1 + i];
    }

    /* unscale the rhs! */
    for(i = 0; i <= lp->rows; i++)
      lp->orig_rh[i] /= lp->scale[i];
    for(i = 0; i < lp->nr_lagrange; i++)
      lp->lag_rhs[i] /= lp->scale[lp->sum + 1 + i];

    /* and don't forget to unscale the upper and lower bounds ... */
    for(i = 0; i <= lp->rows; i++) {
      if(lp->orig_lowbo[i] != 0)
	lp->orig_lowbo[i] /= lp->scale[i];
      if(lp->orig_upbo[i] != lp->infinite)
	lp->orig_upbo[i] /= lp->scale[i];
    }

    FREE(lp->scale);
    lp->scaling_used = FALSE;
    lp->columns_scaled = FALSE;
    lp->eta_valid = FALSE;
  }
}

REAL scale(lprec *lp, REAL *myrowscale, REAL *mycolscale)
{
  int i, j, row_nr, rcl_count, row_count;
  REAL *row_max = NULL, *row_min = NULL, *scalechange = NULL, absval;
  REAL col_max, col_min;

  if(!lp->scaling_used) {
    if (MALLOC(lp->scale, lp->sum_alloc + lp->nr_lagrange + 1, REAL) == NULL)
      return(0.0);
    for(i = 0; i <= lp->sum + lp->nr_lagrange; i++)
      lp->scale[i] = 1;
  }

  rcl_count = lp->sum + lp->nr_lagrange;
  row_count = lp->rows + lp->nr_lagrange;
  if ((MALLOC(scalechange, rcl_count + 1, REAL) == NULL) ||
      (MALLOC(row_max, row_count + 1, REAL) == NULL) ||
      (MALLOC(row_min, row_count + 1, REAL) == NULL)
     ) {
    FREE(row_min);
    FREE(row_max);
    FREE(scalechange);
    if(!lp->scaling_used) {
      FREE(lp->scale);
    }
    return(0.0);
  }

 /* must initialize due to computation of scaling statistic - KE */
  for(i = 0; i <= rcl_count; i++)
    scalechange[i] = 1;

  row_count = lp->rows + lp->nr_lagrange;

  /* initialise min and max values of rows */
  for(i = 0; i <= row_count; i++) {
    row_max[i] = 0;
    if(lp->scalemode & GEOSCALING)
      row_min[i] = 0;
    else
      row_min[i] = lp->infinite;
  }

  /* calculate row scaling data */
  for(j = 1; j <= lp->columns; j++) {
    for(i = lp->col_end[j - 1]; i < lp->col_end[j]; i++) {
      row_nr = lp->mat[i].row_nr;
      absval = my_abs(lp->mat[i].value);
      if(absval != 0) {
        if(lp->scalemode & GEOSCALING) {
          row_max[row_nr] += (REAL) log(absval);
          row_min[row_nr]++;
        }
        else {
          row_max[row_nr] = my_max(row_max[row_nr], absval);
          row_min[row_nr] = my_min(row_min[row_nr], absval);
        }
      }
    }
    if(lp->scalemode & LAGRANGESCALE)
      for(i = 0; i < lp->nr_lagrange; i++) {
        row_nr = lp->rows + 1 + i;
        absval = my_abs(lp->lag_row[i][j]);
        if(absval != 0) {
          if(lp->scalemode & GEOSCALING) {
            row_max[row_nr] += (REAL) log(absval);
            row_min[row_nr]++;
          }
          else {
            row_max[row_nr] = my_max(row_max[row_nr], absval);
            row_min[row_nr] = my_min(row_min[row_nr], absval);
          }
        }
      }
  }

  /* calculate scale factors for rows */
  for(i = 0; i <= lp->rows; i++) {
    scalechange[i] = minmax_to_scale(lp, row_min[i], row_max[i]);

    /* override values with user entries, if specified */
    if((myrowscale != NULL) && (i > 0))
      scalechange[i] = myrowscale[i];

  }
  if(lp->scalemode & LAGRANGESCALE)
    for(i = 0; i < lp->nr_lagrange; i++) {
      row_nr = lp->rows + 1 + i;
      scalechange[lp->sum + 1 + i] = minmax_to_scale(lp, row_min[row_nr], row_max[row_nr]);
    }

  free(row_max);
  free(row_min);

  /* Row-scale the matrix (including the objective function and Lagrangean constraints) */
  scalerows(lp, scalechange, &scalechange[lp->sum]);

  /* calculate column scales */
  i = 1;
  for(j = 1; j <= lp->columns; j++) {
    if(is_int(lp,j) && ((lp->scalemode & INTEGERSCALE) == 0)) { /* do not scale integer columns */
      scalechange[lp->rows + j] = 1;
    }
    else {
      col_max = 0;
      if(lp->scalemode & GEOSCALING)
        col_min = 0;
      else
        col_min = lp->infinite;

      for(i = lp->col_end[j - 1]; i < lp->col_end[j]; i++) {
        absval = my_abs(lp->mat[i].value);
        if(absval != 0) {
          if(lp->scalemode & GEOSCALING) {
            col_max += (REAL) log(absval);
            col_min++;
          }
          else {
            col_max = my_max(col_max, absval);
            col_min = my_min(col_min, absval);
          }
        }
      }
      for(i = 0; i < lp->nr_lagrange; i++) {
        absval = my_abs(lp->lag_row[i][j]);
        if(absval != 0) {
          if(lp->scalemode & GEOSCALING) {
            col_max += (REAL) log(absval);
            col_min++;
          }
          else {
            col_max = my_max(col_max, absval);
            col_min = my_min(col_min, absval);
          }
        }
      }

      scalechange[lp->rows + j] = minmax_to_scale(lp, col_min, col_max);

      if(mycolscale != NULL)
        scalechange[lp->rows + j] = mycolscale[j];
    }
  }

  /* ... and then column-scale the already row-scaled matrix */
  scalecolumns(lp, &scalechange[lp->rows]);

  /* create a geometric mean-type measure of the extent of scaling performed; */
  /* ideally, upon successive calls to scale() the value should converge to 0 */
  col_max = 0;
  for(j = 1; j <= lp->columns; j++)
    col_max += (REAL) log(scalechange[lp->rows + j]);
  if(lp->columns)
    col_max = (REAL) exp(col_max/lp->columns);

  col_min = 0;
  for(i = 1; i <= lp->rows; i++)
    col_min += (REAL) log(scalechange[i]);
  for(i = 0; i < lp->nr_lagrange; i++)
    col_min += (REAL) log(scalechange[lp->sum + 1 + i]);
  if(row_count)
    col_min = (REAL) exp(col_min/row_count);

  free(scalechange);
  lp->scaling_used = TRUE;
  lp->eta_valid = FALSE;

  return((REAL) sqrt(col_max*col_min)-1);
}

REAL auto_scale(lprec *lp)
{
  if(lp->scalemode & CURTISREIDSCALE)
    return((REAL) scaleCR(lp));
  else
    return(scale(lp, NULL, NULL));
}

int preprocess(lprec *lp)
{
  int j, i, ok = TRUE;
  REAL *new_column;
  REAL hold;
  char *fieldn;
  MYBOOL scaled;

 /* do not process if already preprocessed */
  if(lp->wasprocessed)
    return(ok);

  lp->solutioncount = 0;

  /* Create master SOS variable list */
  if(lp->sos_count > 0)
    if (make_SOSchain(lp) == -1)
      return(FALSE);

  /* Write model statistics */
  report(lp, NORMAL, "Model name:  %s", lp->lp_name);
  j = 0;
  for(i = 1; i <= lp->rows; i++)
    if(lp->orig_lowbo[i] == lp->orig_upbo[i])
      j++;

  fieldn = get_row_name(lp, 0);
  report(lp, NORMAL, "Objective:   %simize(%s)",
         my_if(lp->maximise, "Max", "Min"), fieldn);
  report(lp, NORMAL, "Model size:  %5d variables, %5d constraints, %8d non-zeros.",
         lp->columns, lp->rows+lp->nr_lagrange, lp->non_zeros);
  report(lp, NORMAL, "Variables:   %5d integer,   %5d semi-cont.,     %5d SOS.",
         lp->int_count, lp->sc_count, lp->sos_vars);
  report(lp, NORMAL, "Constraints: %5d equality,  %5d Lagrangean,     %5d SOS.",
         j, lp->nr_lagrange, lp->sos_count);
  report(lp, NORMAL, " ");

  new_column = NULL;
 /* First create extra columns for quasi-free variables */
  for (j = 1; j <= lp->columns; j++) {
   /* First handle strictly negative variables by changing signs (multiply row by -1) */
    i = lp->rows + j;
    hold = lp->orig_upbo[i];
    if((hold < 0) || ((lp->splitnegvars == AUTOMATIC) && (lp->orig_lowbo[i] == -lp->infinite) && (hold != lp->infinite))) { /* was <= 0 */
      if((lp->var_is_free != NULL) && (lp->var_is_free[j] > 0))
        del_column(lp, lp->var_is_free[j]); /* This variable previously has a split var. No longer needed now; delete it */
      mult_column(lp, j, -1);
      if(lp->var_is_free == NULL)
        if (CALLOC(lp->var_is_free, my_max(lp->columns, lp->columns_alloc) + 1, int) == NULL)
          return(FALSE);
      lp->var_is_free[j] = -j; /* indication that upper and lower bounds are switched */
      lp->orig_upbo[i] = -lp->orig_lowbo[i];
      lp->orig_lowbo[i] = -hold;
      /* Check for presence of negative ranged SC variable */
      if(lp->var_is_sc[j] > 0) {
        lp->var_is_sc[j] = lp->orig_lowbo[i];
	lp->orig_lowbo[i] = 0;
      }
    }
   /* Then deal with -+, full-range variables */
    else if(((lp->splitnegvars != AUTOMATIC) || (lp->orig_lowbo[i] == -lp->infinite)) && (lp->orig_lowbo[i] < lp->negrange)) {
      if(lp->var_is_free == NULL)
        if (CALLOC(lp->var_is_free, my_max(lp->columns, lp->columns_alloc) + 1, int) == NULL)
          return(FALSE);
      if(lp->var_is_free[j] <= 0) { /* If this variable wasn't split yet ... */
	if(SOS_is_member(lp, 0, i - lp->rows)) {   /* Added */
	  report(lp, IMPORTANT, "Converted negative bound for SOS variable %d to zero",
		     i - lp->rows);
	  lp->orig_lowbo[i] = 0;
	  continue;
	}
	if(new_column == NULL)
	  if (MALLOC(new_column, lp->rows + 1, REAL) == NULL) {
	    lp->spx_status = OUT_OF_MEMORY;
	    ok = FALSE;
	    break;
	  }
       /* Avoid precision loss by turning off unscaling and rescaling */
       /* in get_column and add_column operations; also make sure that */
       /* full scaling information is preserved */
	scaled = lp->scaling_used;
	lp->scaling_used = FALSE;
	get_column(lp, j, new_column);
	if (!add_columnex(lp, new_column, FALSE, NULL, 0)) {
	  ok = FALSE;
	  break;
	}
	mult_column(lp, lp->columns, -1);
	if(scaled)
	  lp->scale[lp->rows+lp->columns] = lp->scale[i];
	lp->scaling_used = (MYBOOL) scaled;
	if(lp->names_used) {
	  char fieldn[50];

	 /* get_col_name(lp, j, fieldn); */
	 /* strcat(fieldn,"_"); */
	  sprintf(fieldn, "__AntiBodyOf(%d)__", j);
	  if (!set_col_name(lp, lp->columns, fieldn)) {
	    ok = FALSE;
  	    break;
	  }
	}
        lp->var_is_free[j] = lp->columns; /* positive value means var j has a split var in column var_is_free[j] */
      }
      lp->orig_upbo[lp->rows + lp->var_is_free[j]] = -lp->orig_lowbo[i];
      lp->orig_lowbo[i] = 0;

      lp->var_is_free[lp->var_is_free[j]] = -j; /* negative value, but -var_is_free[x] != x indicates x is split var and var_is_free[x] is orig var */
      lp->must_be_int[lp->var_is_free[j]] = lp->must_be_int[j];
    }
   /* Check for positive ranged SC variables */
    else if(lp->var_is_sc[j] > 0) {
      lp->var_is_sc[j] = lp->orig_lowbo[i];
      lp->orig_lowbo[i] = 0;
    }

   /* Tally integer variables in SOS'es */
    if(lp->sos_count > 0 && SOS_is_member(lp, 0, j) && is_int(lp, j))
      lp->sos_ints++;
  }
  FREE(new_column);
  lp->wasprocessed = TRUE;
  return(ok);
}

void postprocess(lprec *lp)
{
  int i,ii,j;
  REAL hold;

 /* Check if the problem actually was preprocessed */
  if(!lp->wasprocessed) return;

 /* Loop over all columns */
  for (j = 1; j <= lp->columns; j++) {
    i = lp->rows + j;
   /* Reconstruct strictly negative values */
    if((lp->var_is_free != NULL) && (lp->var_is_free[j] < 0)) {
     if(-lp->var_is_free[j] == j) { /* only then it is a var where up and low bound are switched, else a split var */
	mult_column(lp, j, -1);
	hold = lp->orig_upbo[i];
	lp->orig_upbo[i] = -lp->orig_lowbo[i];
	lp->orig_lowbo[i] = -hold;
	lp->best_solution[i] = -lp->best_solution[i];

	hold = lp->objfrom[j];
        lp->objfrom[j] = -lp->objtill[j];
        lp->objtill[j] = -hold;

	lp->duals[i] = -lp->duals[i];
	hold = lp->dualsfrom[i];
	lp->dualsfrom[i] = -lp->dualstill[i];
	lp->dualstill[i] = -hold;

	lp->var_is_free[j] = 0; /* switch undone, so clear the status */
	/* Adjust negative ranged SC */
	if(lp->var_is_sc[j] > 0)
	  lp->orig_lowbo[lp->rows + j] = -lp->var_is_sc[j];
      }
    }
   /* Condense values of extra columns of quasi-free variables */
    else if((lp->var_is_free != NULL) && (lp->var_is_free[j] > 0)) { /* a variable that was split in two */
      ii = lp->var_is_free[j]; /* the split var */
      if(lp->objfrom[j] == -lp->infinite)
        lp->objfrom[j] = -lp->objtill[ii];
      lp->objtill[ii] = lp->infinite;
      if(lp->objtill[j] == lp->infinite)
        lp->objtill[j] = -lp->objfrom[ii];
      lp->objfrom[ii] = -lp->infinite;

      ii += lp->rows;
      lp->best_solution[i] -= lp->best_solution[ii]; /* join the solution again */
      lp->best_solution[ii] = 0;

      if(lp->duals[i] == 0)
        lp->duals[i] = -lp->duals[ii];
      lp->duals[ii] = 0;
      if(lp->dualsfrom[i] == -lp->infinite)
        lp->dualsfrom[i] = -lp->dualstill[ii];
      lp->dualstill[ii] = lp->infinite;
      if(lp->dualstill[i] == lp->infinite)
        lp->dualstill[i] = -lp->dualsfrom[ii];
      lp->dualsfrom[ii] = -lp->infinite;

      lp->orig_lowbo[i] = -lp->orig_upbo[ii] /* -lp->infinite*/; /* set original bound */
    }
   /* adjust for semi-continuous variables */
    else if(lp->var_is_sc[j] > 0) {
      lp->orig_lowbo[i] = lp->var_is_sc[j];
    }
  }
  lp->wasprocessed = FALSE;
}

void set_basis(lprec *lp, int *bascolumn)   /* Added by KE */
{
  int i,s,k;

 /* Initialize */
  for(i = 1; i <= lp->sum; i++) {
    lp->basis[i] = FALSE;
    lp->lower[i] = TRUE;
  }

 /* Set basis variables; */
 /* negative index means at lower bound, positive at upper bound */
  for(i = 1; i <= lp->rows; i++) {
    s = bascolumn[i];
    k = abs(s);
    lp->bas[i] = k;
    if(s > 0)
      lp->lower[i] = FALSE;
    lp->basis[k] = TRUE;
  }

  lp->eta_valid = FALSE;   /* Force reinversion */
  lp->basis_valid = TRUE;  /* Do not re-initialize basis on entering Solve */

}

void reset_basis(lprec *lp)
{
  lp->basis_valid = FALSE;   /* Causes reinversion at next opportunity */
}

void get_basis(lprec *lp, int *bascolumn)   /* Added by KE */
{
  int k, i;

  *bascolumn = 0;
  for(i = 1; i <= lp->rows; i++) {
    k = lp->bas[i];
    if(lp->lower[i])
      bascolumn[i] = -k;
    else
      bascolumn[i] = k;
  }
}

int write_LP(lprec *lp, FILE *output)
{
  int i, j, k, ok = FALSE, ok2, Ncol, b;
  REAL *row, a;
  matrec *matrecrow;

  if (MALLOC(row, lp->columns + 1, REAL) != NULL) {
    if(lp->maximise)
      fprintf(output, "max:");
    else
      fprintf(output, "min:");

    get_row(lp, 0, row);
    for(i = 1; i <= lp->columns; i++)
      if((row[i] != 0) && (!is_splitvar(lp, i))) {
	if(row[i] == -1)
	  fprintf(output, " -");
	else if(row[i] == 1)
	  fprintf(output, " +");
	else
	  fprintf(output, " %+.12g ", (double)row[i]);
	fprintf(output, "%s", get_col_name(lp, i));
      }
    fprintf(output, ";\n");

    free(row);

    /* Write constraints */
    if (MALLOC(matrecrow, lp->columns + 1, matrec) != NULL) {
      for(j = 1; j <= lp->rows; j++) {
        Ncol = get_matrecrow(lp, j, matrecrow);
        ok2 = FALSE;
        for(k = 0; k < Ncol; k++) {
          i = matrecrow[k].row_nr;
  	  a = matrecrow[k].value;
  	  if((a != 0) && (!is_splitvar(lp, i))) {
  	    if(!ok2) {
  	      fprintf(output, "%s:", get_row_name(lp, j));
  	      ok2 = TRUE;
#if 1
	      if ((lp->orig_upbo[j]) && (lp->orig_upbo[j] < lp->infinite)) {
#if 0
  	        fprintf(output, " %+.12g %s",
		        (lp->orig_upbo[j]-lp->orig_rh[j]) * (lp->ch_sign[j] ? 1.0 : -1.0) / (lp->scaling_used ? lp->scale[j] : 1.0),
  		        (lp->ch_sign[j]) ? ">=" : "<=");
#else
                fprintf(output, " %+.12g %s",
		        (double)my_chsign(lp->ch_sign[j], lp->orig_rh[j] / (lp->scaling_used ? lp->scale[j] : 1.0)),
			(lp->ch_sign[j]) ? "<=" : ">=");
#endif

	      }
#endif
  	    }
  	    if(a == -1)
  	      fprintf(output, " -");
  	    else if(a == 1)
  	      fprintf(output, " +");
  	    else
  	      fprintf(output, " %+.12g ", (double)a);
  	    fprintf(output, "%s", get_col_name(lp, i));
  	  }
        }
        for(i = 1; (!ok2) && (i <= lp->columns); i++)
  	  if(!is_splitvar(lp, i)) {
  	    fprintf(output, "%s: 0 %s", get_row_name(lp, j), get_col_name(lp, i));
  	    ok2 = TRUE;
#if 1
	    if ((lp->orig_upbo[j]) && (lp->orig_upbo[j] < lp->infinite)) {
#if 0
  	      fprintf(output, " %+.12g %s",
		      (lp->orig_upbo[j]-lp->orig_rh[j]) * (lp->ch_sign[j] ? 1.0 : -1.0) / (lp->scaling_used ? lp->scale[j] : 1.0),
  		      (lp->ch_sign[j]) ? ">=" : "<=");
#else
              fprintf(output, " %+.12g %s",
		      (double)my_chsign(lp->ch_sign[j], lp->orig_rh[j] / (lp->scaling_used ? lp->scale[j] : 1.0)),
		      (lp->ch_sign[j]) ? "<=" : ">=");
#endif
	    }
#endif
  	  }
        if(ok2) {
          if(lp->orig_upbo[j] == 0)
  	    fprintf(output, " = %.12g;\n",
	            (double)my_chsign(lp->ch_sign[j], lp->orig_rh[j] / (lp->scaling_used ? lp->scale[j] : 1.0)));
          else
#if 1
	    if ((lp->orig_upbo[j]) && (lp->orig_upbo[j] < lp->infinite))
  	      fprintf(output, " %s %+.12g;\n",
	              (lp->ch_sign[j]) ? "<=" : ">=",
		      (lp->orig_upbo[j]-lp->orig_rh[j]) * (lp->ch_sign[j] ? 1.0 : -1.0) / (lp->scaling_used ? lp->scale[j] : 1.0));
	    else
#endif
              fprintf(output, " %s %.12g;\n",
	          (lp->ch_sign[j]) ? ">=" : "<=",
	          (double)my_chsign(lp->ch_sign[j], lp->orig_rh[j] / (lp->scaling_used ? lp->scale[j] : 1.0)));
#if 0
          if ((lp->orig_upbo[j]) && (lp->orig_upbo[j] < lp->infinite)) {
  	    fprintf(output, "%s:", get_row_name(lp, j));
  	    fprintf(output, " %s %.12g;\n",
  		    (lp->ch_sign[j]) ? "<=" : ">=",
  		    (lp->orig_upbo[j]-lp->orig_rh[j]) * (lp->ch_sign[j] ? 1.0 : -1.0) / (lp->scaling_used ? lp->scale[j] : 1.0));
          }
#endif
        }
      }
      free(matrecrow);

      /* Write bounds on variables */
      for(i = lp->rows + 1; i <= lp->sum; i++)
        if(!is_splitvar(lp, i - lp->rows)) {
          if(lp->orig_lowbo[i] == lp->orig_upbo[i])
            fprintf(output, "%s = %.12g;\n", get_col_name(lp, i - lp->rows),
                    (double)lp->orig_upbo[i] * (lp->scaling_used ? lp->scale[i] : 1.0));
          else {
    	    if(lp->orig_lowbo[i] != 0)
    	      fprintf(output, "%s >= %.12g;\n", get_col_name(lp, i - lp->rows),
    		      (double)lp->orig_lowbo[i] * (lp->scaling_used && (lp->orig_lowbo[i] != -lp->infinite) ? lp->scale[i] : 1.0));
    	    if(lp->orig_upbo[i] != lp->infinite)
    	      fprintf(output, "%s <= %.12g;\n", get_col_name(lp, i - lp->rows),
    		      (double)lp->orig_upbo[i] * (lp->scaling_used ? lp->scale[i] : 1.0));
          }
        }

      /* Write optional integer section */
      i = 1;
      while(i <= lp->columns && !is_int(lp, i))
        i++;
      if(i <= lp->columns) {
        fprintf(output, "\nint %s", get_col_name(lp, i));
        i++;
        for(; i <= lp->columns; i++)
          if((!is_splitvar(lp, i)) && (is_int(lp, i))) {
  	    fprintf(output, ",%s", get_col_name(lp, i));
  	  }
        fprintf(output, ";\n");
      }

      /* Write optional SEC section */
      i = 1;
      while(i <= lp->columns && !is_semicont(lp, i))
        i++;
      if(i <= lp->columns) {
        fprintf(output, "\nsec %s", get_col_name(lp, i));
        i++;
        for(; i <= lp->columns; i++)
          if((!is_splitvar(lp, i)) && (is_semicont(lp, i))) {
  	    fprintf(output, ",%s", get_col_name(lp, i));
  	  }
        fprintf(output, ";\n");
      }

     /* Write optional SOS section */
      if(lp->sos_count)
        fprintf(output, "\nsos\n");
      for(b = 0, i = 0; i < lp->sos_count; /* b = lp->sos_list[i]->priority, */ i++) {
        fprintf(output, "%s: ",
                (lp->sos_list[i]->name == NULL) || (*lp->sos_list[i]->name==0) ? "SOS" : lp->sos_list[i]->name); /* formatnumber12((double) lp->sos_list[i]->priority) */

        for(a = 0.0, j = 1; j <= lp->sos_list[i]->size; /* a = lp->sos_list[i]->weights[j], */ j++)
          if(lp->sos_list[i]->weights[j] == ++a)
            fprintf(output, "%s%s",
  		    (j > 1) ? "," : "",
                    get_col_name(lp, lp->sos_list[i]->members[j]));
  	  else
  	    fprintf(output, "%s%s:%.12g",
  		    (j > 1) ? "," : "",
                    get_col_name(lp, lp->sos_list[i]->members[j]),
  		    lp->sos_list[i]->weights[j]);
        if(lp->sos_list[i]->priority == ++b)
          fprintf(output, " <= %d;\n", lp->sos_list[i]->type);
        else
  	  fprintf(output, " <= %d:%d;\n", lp->sos_list[i]->type, lp->sos_list[i]->priority);
      }

      ok = TRUE;
    }
  }
  return(ok);
}

int write_lp(lprec *lp, char *output)
{
  FILE *fpout = stdout;
  int ret = -1;

  if((output == NULL) || ((fpout = fopen(output,"w")) != NULL)) {
    ret = write_LP(lp,fpout);
    fclose(fpout);
  }
  return(ret);
}

static char numberbuffer[15];

static void number(char *str,REAL value)
 {
  char __str[80],*_str;
  int i;

  /* sprintf(_str,"%12.6G",value); */
  _str=__str+2;
  if (value>=0.0)
   if ((value!=0.0) && ((value>0.99999999e12) || (value<0.0001))) {
    int n=15;

    do {
     n--;
     i=sprintf(_str,"%*.*E",n,n-6,(double) value);
     if (i>12) {
      char *ptr=strchr(_str,'E');

      if (ptr!=NULL) {
       if (*(++ptr)=='-') ptr++;
       while ((i>12) && ((*ptr=='+') || (*ptr=='0'))) {
        strcpy(ptr,ptr+1);
        i--;
       }
      }
     }
    } while (i>12);
   }
   else if (value>=1.0e10) {
    int n=13;

    do {
     i=sprintf(_str,"%*.0f",--n,(double) value);
    } while (i>12);
   }
   else {
    if (((i=sprintf(_str,"%12.10f",(double) value))>12) && (_str[12]>='5')) {
     for (i=11;i>=0;i--)
      if (_str[i]!='.') {
       if (++_str[i]>'9') _str[i]='0';
       else break;
      }
     if (i<0) {
      *(--_str)='1';
      *(--_str)=' ';
     }
    }
   }
  else
   if ((value<-0.99999999e11) || (value>-0.0001)) {
    int n=15;

    do {
     n--;
     i=sprintf(_str,"%*.*E",n,n-7,(double) value);
     if (i>12) {
      char *ptr=strchr(_str,'E');

      if (ptr!=NULL) {
       if (*(++ptr)=='-') ptr++;
       while ((i>12) && ((*ptr=='+') || (*ptr=='0'))) {
        strcpy(ptr,ptr+1);
        i--;
       }
      }
     }
    } while (i>12);
   }
   else if (value<=-1.0e9) {
    int n=13;

    do {
     i=sprintf(_str,"%*.0f",--n,(double) value);
    } while (i>12);
   }
   else
    if (((i=sprintf(_str,"%12.9f",(double) value))>12) && (_str[12]>='5')) {
     for (i=11;i>=1;i--)
      if (_str[i]!='.') {
       if (++_str[i]>'9') _str[i]='0';
       else break;
      }
     if (i<1) {
      *_str='1';
      *(--_str)='-';
      *(--_str)=' ';
     }
    }
  strncpy(str,_str,12);
 }

static char *formatnumber12(double a)
{
/*
  int l;

  l = sprintf(numberbuffer, "%12g", a);
*/
  number(numberbuffer, a);
  return(numberbuffer);
}

int write_MPS(lprec *lp, FILE *output)
{
  int i, j, k, marker, putheader, ok = TRUE;
  MYBOOL names_used;
  REAL *column, a;

  if (MALLOC(column, lp->rows + 1, REAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    ok = FALSE;
  }
  else {
    /* check if there is no variable name where the first 8 charachters are equal to the first 8 characters of anothe variable */
    names_used = lp->names_used;
    if(names_used)
      for(i = 1; (i <= lp->columns) && (ok); i++)
        if((lp->col_name[i] != NULL) && (lp->col_name[i]->name != NULL) && (!is_splitvar(lp, i)) && (strlen(lp->col_name[i]->name) > 8))
          for(j = 1; (j < i) && (ok); j++)
	    if((lp->col_name[j] != NULL) && (lp->col_name[j]->name != NULL) && (!is_splitvar(lp, j)))
	      if(strncmp(lp->col_name[i]->name, lp->col_name[j]->name, 8) == 0)
	        ok = FALSE;

    if(!ok) {
      lp->names_used = FALSE;
      ok = TRUE;
    }

    marker = 0;
    fprintf(output, "NAME          %-.8s\n", lp->lp_name);
    fprintf(output, "ROWS\n");
    for(i = 0; i <= lp->rows; i++) {
      if(i == 0)
	fprintf(output, " N  ");
      else
	if(lp->orig_upbo[i] != 0) {
	  if(lp->ch_sign[i])
	    fprintf(output, " G  ");
	  else
	    fprintf(output, " L  ");
	}
	else
	  fprintf(output, " E  ");
      fprintf(output, "%-.8s\n", get_row_name(lp, i));
    }

    fprintf(output, "COLUMNS\n");

    for(i = 1; i <= lp->columns; i++) {
      if(!is_splitvar(lp, i)) {
	if(is_int(lp,i) && (marker % 2) == 0) {
	  fprintf(output,
		  "    MARK%04d  'MARKER'                 'INTORG'\n",
		  marker);
	  marker++;
	}
	if(!is_int(lp,i) && (marker % 2) == 1) {
	  fprintf(output,
		  "    MARK%04d  'MARKER'                 'INTEND'\n",
		  marker);
	  marker++;
	}
	/* this gets slow for large LP problems. Implement a sparse version? */
	get_column(lp, i, column);
	for(k = 1, j = 0; j <= lp->rows; j++)
	  if(column[j] != 0) {
	    k = 1 - k;
	    if (k == 0)
	      fprintf(output, "    %-8.8s  %-8.8s  %s",
		      get_col_name(lp, i),
		      get_row_name(lp, j),
		      formatnumber12((double) (column[j] * (j == 0 && lp->maximise ? -1 : 1))));
	    else
	      fprintf(output, "   %-8.8s  %s\n",
		      get_row_name(lp, j),
		      formatnumber12((double) (column[j] * (j == 0 && lp->maximise ? -1 : 1))));
	  }
	if(k == 0)
	  fprintf(output, "\n");
      }
    }
    if((marker % 2) == 1) {
      fprintf(output, "    MARK%04d  'MARKER'                 'INTEND'\n",
	      marker);
      /* marker++; */ /* marker not used after this */
    }

    fprintf(output, "RHS\n");
    for(k = 1, i = 1; i <= lp->rows; i++) {
      a = lp->orig_rh[i];
      if(a) {
	if(lp->scaling_used)
	  a /= lp->scale[i];

	if(lp->ch_sign[i])
	  a = -a;

	k = 1 - k;
	if(k == 0)
	  fprintf(output, "    RHS       %-8.8s  %s",
	          get_row_name(lp, i),
		  formatnumber12((double)a));
	else
	  fprintf(output, "   %-8.8s  %s\n",
	          get_row_name(lp, i),
		  formatnumber12((double)a));
      }
    }
    if(k == 0)
      fprintf(output, "\n");

    putheader = TRUE;
    for(k = 1, i = 1; i <= lp->rows; i++){
      a = 0;
      if((lp->orig_upbo[i] != lp->infinite) && (lp->orig_upbo[i] != 0.0))
        a = lp->orig_upbo[i];
      else if(lp->orig_lowbo[i] != 0.0)
        a = -lp->orig_lowbo[i];
      if(a) {
	if(putheader) {
	  fprintf(output, "RANGES\n");
	  putheader = FALSE;
	}
	if(lp->scaling_used)
	  a /= lp->scale[i];
        k = 1 - k;
	if(k == 0)
	  fprintf(output, "    RGS       %-8.8s  %s",
	          get_row_name(lp, i),
		  formatnumber12((double)a));
	else
	  fprintf(output, "   %-8.8s  %s\n",
	          get_row_name(lp, i),
		  formatnumber12((double)a));
      }
    }
    if(k == 0)
      fprintf(output, "\n");

    putheader = TRUE;
    for(i = lp->rows + 1; i <= lp->sum; i++)
      if(!is_splitvar(lp, i - lp->rows)) {
	j = i - lp->rows;
	if((lp->orig_lowbo[i] != 0) && (lp->orig_upbo[i] < lp->infinite) &&
	   (lp->orig_lowbo[i] == lp->orig_upbo[i]) && (!is_semicont(lp, j))) {
	  a = lp->orig_upbo[i];
	  if(lp->scaling_used)
	    a *= lp->scale[i];
	  if(putheader) {
	    fprintf(output, "BOUNDS\n");
	    putheader = FALSE;
	  }
	  fprintf(output, " FX BND       %-8.8s  %s\n",
		  get_col_name(lp, i - lp->rows), formatnumber12((double)a));
	}
	else if((lp->orig_lowbo[i] <= -lp->infinite) && (lp->orig_upbo[i] >= lp->infinite)) {
	  if(putheader) {
	    fprintf(output, "BOUNDS\n");
	    putheader = FALSE;
	  }
	  fprintf(output, " FR BND       %-8.8s\n",
		  get_col_name(lp, i - lp->rows));
	}
	else {
	  if((lp->orig_upbo[i] < lp->infinite) || (is_semicont(lp, j))) {
	    a = lp->orig_upbo[i];
	    if((lp->scaling_used) && (a < lp->infinite))
	      a *= lp->scale[i];
	    if(putheader) {
	      fprintf(output, "BOUNDS\n");
	      putheader = FALSE;
	    }
	    if(is_semicont(lp, j)) {
	      if(is_int(lp, j))
	        if (lp->orig_upbo[i] < lp->infinite)
		  fprintf(output, " SI BND       %-8.8s  %s\n",
			  get_col_name(lp, i - lp->rows), formatnumber12((double)a));
		else
		  fprintf(output, " SI BND       %-8.8s\n",
			  get_col_name(lp, i - lp->rows));
	      else
	        if (lp->orig_upbo[i] < lp->infinite)
		  fprintf(output, " SC BND       %-8.8s  %s\n",
			  get_col_name(lp, i - lp->rows), formatnumber12((double)a));
		else
		  fprintf(output, " SC BND       %-8.8s\n",
			  get_col_name(lp, i - lp->rows));
	    }
	    else
	      fprintf(output, " UP BND       %-8.8s  %s\n",
		      get_col_name(lp, i - lp->rows), formatnumber12((double)a));
	  }
	  if(lp->orig_lowbo[i] != 0) {
	    a = lp->orig_lowbo[i];
	    if(lp->scaling_used)
	      a *= lp->scale[i];
	    if(putheader) {
	      fprintf(output, "BOUNDS\n");
	      putheader = FALSE;
	    }
	    if(lp->orig_lowbo[i] != -lp->infinite)
	      fprintf(output, " LO BND       %-8.8s  %s\n",
		      get_col_name(lp, i - lp->rows), formatnumber12((double)a));
	    else
	      fprintf(output, " MI BND       %-8.8s\n",
		      get_col_name(lp, i - lp->rows));
	  }
	}
      }

   /* Write optional SOS section */
    putheader = TRUE;
    for(i = 0; i < lp->sos_count; i++) {
      if(putheader) {
        fprintf(output, "SOS\n");
        putheader = FALSE;
      }
      fprintf(output, " S%1d SOS       %-8.8s  %s\n",
              lp->sos_list[i]->type, lp->sos_list[i]->name, formatnumber12((double) lp->sos_list[i]->priority));

      for(j = 1; j <= lp->sos_list[i]->size; j++) {
        fprintf(output, "    SOS       %-8.8s  %s\n",
                get_col_name(lp, lp->sos_list[i]->members[j]), formatnumber12((double) lp->sos_list[i]->weights[j]));
      }
    }

    fprintf(output, "ENDATA\n");
    free(column);

    lp->names_used = names_used;
  }
  return(ok);
}

int write_mps(lprec *lp, char *output)
{
  FILE *fpout = stdout;
  int ret = -1;

  if((output == NULL) || ((fpout = fopen(output,"w")) != NULL)) {
    ret = write_MPS(lp, fpout);
    fclose(fpout);
  }
  return(ret);
}
