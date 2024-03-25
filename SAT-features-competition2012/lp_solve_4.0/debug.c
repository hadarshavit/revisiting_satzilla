#include <stdarg.h>
#include <signal.h>
#include "lpkit.h"

static FILE *stream = NULL;

static void InitStream()
{
  if (stream == NULL)
    stream = stdout;
}

void *report(lprec *lp, short level, char *format, ...)
{
  va_list ap;

  va_start(ap, format);
  if ((lp == NULL) || (lp->writelog == NULL))
  {
    if((lp == NULL) || (lp->verbose >= level))
    {
      vfprintf(stderr, format, ap);
      fputc('\n', stderr);
    }
  }
  else if(lp->writelog != NULL)
  {
    if(lp->verbose >= level)
    {
      char buff[255];

      vsprintf(buff, format, ap);
      lp->writelog(lp, lp->loghandle, buff);
    }
  }
  va_end(ap);

  return(NULL);
}

static void print_indent(lprec *lp)
{
  int i;

  InitStream();

  fprintf(stream, "%2d", lp->Level);
  if(lp->Level < 50) /* useless otherwise */
    for(i = lp->Level; i > 0; i--)
      fprintf(stream, "--");
  else
    fprintf(stream, " *** too deep ***");
  fprintf(stream, "> ");
} /* print_indent */


void debug_print_solution(lprec *lp)
{
  int i;

  if(lp->debug){
    InitStream();

    for (i = lp->rows + 1; i <= lp->sum; i++) {
      print_indent(lp);
      fprintf(stream, "%-20s %g\n", get_col_name(lp, i - lp->rows),
	      (double)lp->solution[i]);
    }
  }
} /* debug_print_solution */


void debug_print_bounds(lprec *lp, REAL *upbo, REAL *lowbo)
{
  int i;

  if(lp->debug){
    InitStream();

    for(i = lp->rows + 1; i <= lp->sum; i++) {
      if(lowbo[i] == upbo[i]) {
	print_indent(lp);
	fprintf(stream, "%s = %g\n", get_col_name(lp, i - lp->rows),
		(double)lowbo[i] * ((lp->scaling_used) ? lp->scale[i] : 1.0));
      }
      else {
	if(lowbo[i] != 0) {
	  print_indent(lp);
	  fprintf(stream, "%s > %g\n", get_col_name(lp, i - lp->rows),
		  (double)lowbo[i] * ((lp->scaling_used) ? lp->scale[i] : 1.0));
	}
	if(upbo[i] != lp->infinite) {
	  print_indent(lp);
	  fprintf(stream, "%s < %g\n", get_col_name(lp, i - lp->rows),
		  (double)upbo[i] * ((lp->scaling_used) ? lp->scale[i] : 1.0));
	}
      }
    }
  }
} /* debug_print_bounds */


void debug_print(lprec *lp, char *format, ...)
{
  va_list ap;

  if(lp->debug) {
    InitStream();

    print_indent(lp);
    va_start(ap, format);
    vfprintf(stream, format, ap);
    fputc('\n', stream);
    va_end(ap);
  }
} /* debug_print */

void print_str(char *str)
{
  InitStream();

  fputs(str, stream);
}

void print_lp(lprec *lp)
{
  int i, j;
  REAL *fatmat;

  InitStream();

  if ((lp->columns > 0) && (CALLOC(fatmat, (lp->rows + 1) * lp->columns, REAL) != NULL)) {
    for(i = 1; i <= lp->columns_used; i++)
      for(j = lp->col_end[i - 1]; j < lp->col_end[i]; j++)
	fatmat[(i - 1) * (lp->rows + 1) + lp->mat[j].row_nr] = lp->mat[j].value;

    fprintf(stream, "\nModel name: %s\n", lp->lp_name);
    fprintf(stream, "          ");
    for(j = 1; j <= lp->columns; j++)
      fprintf(stream, "%8.8s ", get_col_name(lp, j));
    if(lp->maximise) {
      fprintf(stream, "\nMaximize  ");
      for(j = 0; j < lp->columns; j++)
	fprintf(stream, "%8g ", (double) -fatmat[j * (lp->rows + 1)]);
    }
    else {
      fprintf(stream, "\nMinimize  ");
      for(j = 0; j < lp->columns; j++)
	fprintf(stream, "%8g ", (double) fatmat[j * (lp->rows + 1)]);
    }
    fprintf(stream, "\n");
    for(i = 1; i <= lp->rows; i++) {
      fprintf(stream, "%-9s ", get_row_name(lp, i));
      for(j = 0; j < lp->columns; j++)
        fprintf(stream, "%8g ", (double)my_chsign(lp->ch_sign[i], fatmat[j * (lp->rows + 1) + i]));
      if(lp->orig_upbo[i] != 0) {
	if(lp->ch_sign[i])
	  fprintf(stream, ">= ");
	else
	  fprintf(stream, "<= ");
      }
      else
	fprintf(stream, " = ");
      fprintf(stream, "%8g", (double)my_chsign(lp->ch_sign[i], lp->orig_rh[i]));
      if(lp->orig_lowbo[i] != 0) {
	fprintf(stream, "  %s = %8g", (lp->ch_sign[i]) ? "lowbo" : "upbo",
	       (double)(lp->orig_lowbo[i]+lp->orig_rh[i])*(lp->ch_sign[i] ? 1.0 : -1.0));
      }
      if((lp->orig_upbo[i] != lp->infinite) && (lp->orig_upbo[i] != 0.0)) {
	fprintf(stream, "  %s = %8g", (lp->ch_sign[i]) ? "upbo" : "lowbo",
	       (double)(lp->orig_upbo[i]-lp->orig_rh[i])*(lp->ch_sign[i] ? 1.0 : -1.0));
      }
      fprintf(stream, "\n");
    }
    for(i = 0; i < lp->nr_lagrange; i++) {
      fprintf(stream, "lag[%-5d]", i);
      for(j = 1; j <= lp->columns; j++)
	fprintf(stream, "%8g ", (double)lp->lag_row[i][j]);
      if(lp->orig_upbo[i] == lp->infinite) {
	if(lp->lag_con_type[i] == GE)
	  fprintf(stream, ">= ");
	else if(lp->lag_con_type[i] == LE)
	  fprintf(stream, "<= ");
	else if(lp->lag_con_type[i] == EQ)
	  fprintf(stream, " = ");
      }
      fprintf(stream, "%8g\n", (double)lp->lag_rhs[i]);
    }

    fprintf(stream, "Type      ");
    for(i = 1; i <= lp->columns; i++)
      if(is_int(lp,i))
	fprintf(stream, "     Int ");
      else
	fprintf(stream, "    Real ");
    fprintf(stream, "\nupbo      ");
    for(i = 1; i <= lp->columns; i++)
      if(lp->orig_upbo[lp->rows + i] == lp->infinite)
	fprintf(stream, " Infinite");
      else
	fprintf(stream, "%8g ", (double)lp->orig_upbo[lp->rows + i]);
    fprintf(stream, "\nlowbo     ");
    for(i = 1; i <= lp->columns; i++)
      fprintf(stream, "%8g ", (double)lp->orig_lowbo[lp->rows + i]);
    fprintf(stream, "\n");

    free(fatmat);
  }

  fflush(stream);
}

void print_objective(lprec *lp)
{
  InitStream();

  fprintf(stream, "\nValue of objective function: %g\n",
	  (double)lp->best_solution[0]);
}

void print_solution(lprec *lp)
{
  int i;

  InitStream();

  fprintf(stream, "\nActual values of the variables:\n");
  /* print normal variables */
  for(i = 1; i <= lp->columns; i++)
    if(!is_splitvar(lp, i))
      fprintf(stream, "%-20s %g\n", get_col_name(lp, i),
	      (double)lp->best_solution[lp->rows + i]);

  fflush(stream);
} /* Print_solution */

void print_constraints(lprec *lp)
{
  int i;

  InitStream();

  fprintf(stream, "\nActual values of the constraints:\n");
  for(i = 1; i <= lp->rows; i++)
    fprintf(stream, "%-20s %g\n", get_row_name(lp, i),
	    (double)lp->best_solution[i]);

  fflush(stream);
}

void print_duals(lprec *lp)
{
  int i;

  InitStream();

  fprintf(stream, "\nObjective function limits:\n");
  fprintf(stream, "                              From            Till          FromValue\n");
  for(i = 1; i <= lp->columns; i++)
    if(!is_splitvar(lp, i))
      fprintf(stream, "%-20s  %15.7g %15.7g %15.7g\n", get_col_name(lp, i),
	      (double)lp->objfrom[i], (double)lp->objtill[i], (double)lp->objfromvalue[i]);

  fprintf(stream, "\nDual values with from - till limits:\n");
  fprintf(stream, "                          Dual value          From            Till\n");
  for(i = 1; i <= lp->rows + lp->columns; i++)
    if((i <= lp->rows) || (!is_splitvar(lp, i - lp->rows)))
      fprintf(stream, "%-20s %15.7g  %15.7g %15.7g\n", (i <= lp->rows) ? get_row_name(lp, i) : get_col_name(lp, i - lp->rows),
	      (double)lp->duals[i], (double)lp->dualsfrom[i], (double)lp->dualstill[i]);
  fflush(stream);
}

void print_scales(lprec *lp)
{
  int i;

  InitStream();

  if(lp->scaling_used) {
    fprintf(stream, "\nScale factors:\n");
    for(i = 0; i <= lp->rows + lp->columns; i++)
      if((i <= lp->rows) || (!is_splitvar(lp, i - lp->rows)))
        fprintf(stream, "%-20s scaled at %g\n", (i <= lp->rows) ? get_row_name(lp, i) : get_col_name(lp, i - lp->rows),
	        (double)lp->scale[i]);
  }
  fflush(stream);
}

int print_tableau(lprec *lp)
{
  int j, row_nr;
  REAL *prow = NULL;

  InitStream();

  if (CALLOC(prow,lp->sum + 1, REAL) == NULL) {
    lp->spx_status = OUT_OF_MEMORY;
    FREE(prow);
    return(FALSE);
  }

      fprintf(stream, "\n");
      fprintf(stream, "Optimal tableau:\n");

      for(j = 1; j <= lp->rows + lp->columns; j++)
	if (!lp->basis[j])
	  fprintf(stream, "%15d", (j <= lp->rows ? (j + lp->columns) * ((lp->orig_upbo[j] == 0) || (lp->ch_sign[j]) ? 1 : -1) : j - lp->rows) * (lp->lower[j] ? 1 : -1));
      fprintf(stream, "\n");

      for(row_nr = 1; (row_nr <= lp->rows + 1); row_nr++) {
        if (row_nr <= lp->rows)
          fprintf(stream, "%3d", (lp->bas[row_nr] <= lp->rows ? (lp->bas[row_nr] + lp->columns) * ((lp->orig_upbo[lp->bas[row_nr]] == 0) || (lp->ch_sign[lp->bas[row_nr]]) ? 1 : -1) : lp->bas[row_nr] - lp->rows) * (lp->lower[lp->bas[row_nr]] ? 1 : -1));
	else
	  fprintf(stream, "   ");
        setpivrow(lp, row_nr <= lp->rows ? row_nr : 0, prow); /* construct one row of the tableau */
	for(j = 1; j <= lp->rows + lp->columns; j++)
	  if (!lp->basis[j])
	    fprintf(stream, "%15.7f", prow[j] * (lp->lower[j] ? 1 : -1) * (row_nr <= lp->rows ? 1 : -1));
	fprintf(stream, "%15.7f", (REAL) lp->rhs[row_nr <= lp->rows ? row_nr : 0] * ((row_nr <= lp->rows) || (lp->maximise) ? 1 : -1));
	fprintf(stream, "\n");
      }
  FREE(prow);
  return(TRUE);
}

int print_file(char *filename)
{
  if ((stream != NULL) && (stream != stderr))
    fclose(stream);

  if (filename == NULL)
    stream = stderr;
  else
    stream = fopen(filename, "w");
  return(stream != NULL);
}
