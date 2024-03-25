#include <stdio.h>

#include "lpkit.h"
#include "debug.h"

static char *buferror = NULL;
static FILE *fplogfile = NULL;

static void WINAPI logfunction(void *lp, void *userhandle, char *buf)
{
 if (fplogfile != NULL)
  fprintf(fplogfile, "%s\n", buf);
}

/* this will only work in the following condition:

   int = long
   REAL = double
*/

static void freebuferror()
 {
  if (buferror != NULL) {
   free(buferror);
   buferror = NULL;
  }
 }

void __declspec(dllexport) WINAPI _lasterror(char *str, long strsz)
 {
  if (strsz) {
   *str=0;
   if (buferror != NULL)
    strncpy(str,buferror,strsz);
  }
 }

/* return lp_solve version info */
void __declspec(dllexport) WINAPI _lp_solve_version(int *majorversion, int *minorversion, int *release, int *build)
 {
  lp_solve_version(majorversion, minorversion, release, build);
 }

void __declspec(dllexport) WINAPI _set_magic(long code, long param)
 {
  set_magic(code, param);
 }

/* create and initialise a lprec structure
   defaults:
   Empty (Rows * Columns) matrix,
   Minimise the objective function
   constraints all type <=
   Upperbounds all Infinite
   no integer variables
   floor first in B&B
   no scaling
   default basis */
lprec __declspec(dllexport) * WINAPI _make_lp(long rows, long columns)
 {
  lprec *lp;

  freebuferror();
  lp = make_lp(rows, columns);
  if(lp != NULL) {
   put_logfunc(lp, logfunction, NULL);
   if(fplogfile != NULL)
    set_verbose(lp, IMPORTANT);
  }
  return(lp);
 }

/* create and read an .lp file from input */
lprec __declspec(dllexport) * WINAPI _read_LP(char *filename, long verbose, char *lp_name)
 {
  lprec *lp;

  freebuferror();
  lp = read_LP(filename, (short) ((verbose == 0) ? FALSE : TRUE), lp_name);
  return(lp);
 }

/* Remove problem from memory */
void __declspec(dllexport) WINAPI _delete_lp(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   delete_lp(lp);
  }
 }

/* copy a lp structure */
lprec __declspec(dllexport) * WINAPI _copy_lp(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   return(copy_lp(lp));
  }
  else
   return(NULL);
 }

/* fill in element (Row,Column) of the matrix
   Row in [0..Rows] and Column in [1..Columns] */
long __declspec(dllexport) WINAPI _set_mat(lprec *lp, long row, long column, double value)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_mat(lp, row, column, value);
  }
  else
   ret = 0;
  return(ret);
 }

/* set the objective function (Row 0) of the matrix */
long __declspec(dllexport) WINAPI _set_obj_fn(lprec *lp, double *row)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_obj_fn(lp, row);
  }
  else
   ret = 0;
  return(ret);
 }

/* set the objective function (Row 0) of the matrix with string input */
long __declspec(dllexport) WINAPI _str_set_obj_fn(lprec *lp, char *row)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = str_set_obj_fn(lp, row);
  }
  else
   ret = 0;
  return(ret);
 }

/* Add a constraint to the problem,
   row is the constraint row,
   rh is the right hand side,
   constr_type is the type of constraint (LE (<=), GE(>=), EQ(=)) */
long __declspec(dllexport) WINAPI _add_constraint(lprec *lp, double *row, long constr_type, double rh)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = add_constraint(lp, row,(short) constr_type, rh);
  }
  else
   ret = 0;
  return(ret);
 }

/* Add a constraint to the problem with string input,
   row is the constraint row,
   rh is the right hand side,
   constr_type is the type of constraint (LE (<=), GE(>=), EQ(=)) */
long __declspec(dllexport) WINAPI _str_add_constraint(lprec *lp, char *row, long constr_type, double rh)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = str_add_constraint(lp, row,(short) constr_type, rh);
  }
  else
   ret = 0;
  return(ret);
 }

/* Remove constraint nr del_row from the problem */
long __declspec(dllexport) WINAPI _del_constraint(lprec *lp, long del_row)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = del_constraint(lp, del_row);
  }
  else
   ret = 0;
  return(ret);
 }

/* add a Lagrangian constraint of form Row' x contype Rhs */
long __declspec(dllexport) WINAPI _add_lag_con(lprec *lp, double *row, long con_type, double rhs)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = add_lag_con(lp, row, (short) con_type, rhs);
  }
  else
   ret = 0;
  return(ret);
 }

/* add a Lagrangian constraint of form Row' x contype Rhs with string input */
long __declspec(dllexport) WINAPI _str_add_lag_con(lprec *lp, char *row, long con_type, double rhs)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = str_add_lag_con(lp, row, (short) con_type, rhs);
  }
  else
   ret = -1;
  return(ret);
 }

/* Add a Column to the problem */
long __declspec(dllexport) WINAPI _add_column(lprec *lp, double *column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = add_column(lp, column);
  }
  else
   ret = 0;
  return(ret);
 }

/* Add a Column to the problem with string input */
long __declspec(dllexport) WINAPI _str_add_column(lprec *lp, char *column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = str_add_column(lp, column);
  }
  else
   ret = 0;
  return(ret);
 }

/* Delete a column */
long __declspec(dllexport) WINAPI _del_column(lprec *lp, long column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = del_column(lp, column);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_bounds_tighter(lprec *lp, long tighten)
{
  if (lp != NULL) {
   freebuferror();
   set_bounds_tighter(lp, (MYBOOL) tighten);
  }
}

long __declspec(dllexport) WINAPI _get_bounds_tighter(lprec *lp)
{
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_bounds_tighter(lp);
  }
  else
   ret = -1;
  return(ret);
}


/* Set the upperbound of a variable */
long __declspec(dllexport) WINAPI _set_upbo(lprec *lp, long column, double value)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_upbo(lp, column, value);
  }
  else
   ret = 0;
  return(ret);
 }

/* Get the upperbound of a variable */
double __declspec(dllexport) WINAPI _get_upbo(lprec *lp, long column)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_upbo(lp, column);
  }
  else
   ret = 0.0;
  return(ret);
 }

/* Set the lowerbound of a variable */
long __declspec(dllexport) WINAPI _set_lowbo(lprec *lp, long column, double value)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_lowbo(lp, column, value);
  }
  else
   ret = 0;
  return(ret);
 }

/* Get the lowerbound of a variable */
double __declspec(dllexport) WINAPI _get_lowbo(lprec *lp, long column)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_lowbo(lp, column);
  }
  else
   ret = 0.0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _set_bounds(lprec *lp, long column, double lower, double upper)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_bounds(lp, column, lower, upper);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_bounds(lprec *lp, long column, double *lower, double *upper)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_bounds(lp, column, lower, upper);
  }
  else
   ret = 0;
  return(ret);
 }

/* Set the upper range of a constraint */
long __declspec(dllexport) WINAPI _set_uprange(lprec *lp, long row, double value)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_uprange(lp, row, value);
  }
  else
   ret = 0;
  return(ret);
 }

/* Set the lower range of a constraint */
long __declspec(dllexport) WINAPI _set_lowrange(lprec *lp, long row, double value)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_lowrange(lp, row, value);
  }
  else
   ret = 0;
  return(ret);
 }

/* Set the type of variable, if must_be_int = TRUE then the variable must be integer */
long __declspec(dllexport) WINAPI _set_int(lprec *lp, long column, long must_be_int)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_int(lp, column, (short) ((must_be_int == 0) ? FALSE : TRUE));
  }
  else
   ret = 0;
  return(ret);
 }

/* check if var is integer */
long __declspec(dllexport) WINAPI _is_int(lprec *lp, long column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_int(lp, column);
  }
  else
   ret = 0;
  return(ret);
 }

/* set var semi-continious */
long __declspec(dllexport) WINAPI _set_semicont(lprec *lp, long column, long must_be_sc)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_semicont(lp, column, (short) ((must_be_sc == 0) ? FALSE : TRUE));
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_verbose(lprec *lp, long verbose)
 {
  if (lp != NULL) {
   freebuferror();
   set_verbose(lp, (short) verbose);
  }
 }

long __declspec(dllexport) WINAPI _get_verbose(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_verbose(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_timeout(lprec *lp, long sectimeout)
 {
  if (lp != NULL) {
   freebuferror();
   set_timeout(lp, sectimeout);
  }
 }

long __declspec(dllexport) WINAPI _get_timeout(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_timeout(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_print_sol(lprec *lp, long print_sol)
 {
  if (lp != NULL) {
   freebuferror();
   set_print_sol(lp, (short) ((print_sol == 0) ? FALSE : TRUE));
  }
 }

long __declspec(dllexport) WINAPI _is_print_sol(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_print_sol(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_debug(lprec *lp, long debug)
 {
  if (lp != NULL) {
   freebuferror();
   set_debug(lp, (short) ((debug == 0) ? FALSE : TRUE));
  }
 }

long __declspec(dllexport) WINAPI _is_debug(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_debug(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_print_at_invert(lprec *lp, long print_at_invert)
 {
  if (lp != NULL) {
   freebuferror();
   set_print_at_invert(lp, (short) ((print_at_invert == 0) ? FALSE : TRUE));
  }
 }

long __declspec(dllexport) WINAPI _is_print_at_invert(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_print_at_invert(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_trace(lprec *lp, long trace)
 {
  if (lp != NULL) {
   freebuferror();
   set_trace(lp, (short) ((trace == 0) ? FALSE : TRUE));
  }
 }

long __declspec(dllexport) WINAPI _is_trace(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_trace(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_anti_degen(lprec *lp, long anti_degen)
 {
  if (lp != NULL) {
   freebuferror();
   set_anti_degen(lp, (short) ((anti_degen == 0) ? FALSE : TRUE));
  }
 }

long __declspec(dllexport) WINAPI _is_anti_degen(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_anti_degen(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _is_do_presolve(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_do_presolve(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_do_presolve(lprec *lp, long do_presolve)
 {
  if (lp != NULL) {
   freebuferror();
   set_do_presolve(lp, (short) ((do_presolve == 0) ? FALSE : TRUE));
  }
 }

void __declspec(dllexport) WINAPI _set_max_num_inv(lprec *lp, long max_num_inv)
 {
  if (lp != NULL) {
   freebuferror();
   set_max_num_inv(lp, (int) max_num_inv);
  }
 }

long __declspec(dllexport) WINAPI _get_max_num_inv(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_max_num_inv(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_bb_rule(lprec *lp, long bb_rule)
 {
  if (lp != NULL) {
   freebuferror();
   set_bb_rule(lp, (short) bb_rule);
  }
 }

long __declspec(dllexport) WINAPI _get_bb_rule(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_bb_rule(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_obj_bound(lprec *lp, double obj_bound)
 {
  if (lp != NULL) {
   freebuferror();
   set_obj_bound(lp, (REAL) obj_bound);
  }
 }

double __declspec(dllexport) WINAPI _get_obj_bound(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = (double) get_obj_bound(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_mip_gap(lprec *lp, double mip_gap)
 {
  if (lp != NULL) {
   freebuferror();
   set_mip_gap(lp, (REAL) mip_gap);
  }
 }

double __declspec(dllexport) WINAPI _get_mip_gap(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = (double) get_mip_gap(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_floor_first(lprec *lp, long floor_first)
 {
  if (lp != NULL) {
   freebuferror();
   set_floor_first(lp, (short) floor_first);
  }
 }

long __declspec(dllexport) WINAPI _get_floor_first(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_floor_first(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_var_branch(lprec *lp, long column, long branch_mode)
 {
  if (lp != NULL) {
   freebuferror();
   set_var_branch(lp, column, (short) branch_mode);
  }
 }

long __declspec(dllexport) WINAPI _get_var_branch(lprec *lp, long column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_var_branch(lp, column);
  }
  else
   ret = -1;
  return(ret);
 }

long __declspec(dllexport) WINAPI _set_varweights(lprec *lp, double *weights)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_varweights(lp, weights);
  }
  else
   ret = -1;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_infinite(lprec *lp, double infinite)
 {
  if (lp != NULL) {
   freebuferror();
   set_infinite(lp, (REAL) infinite);
  }
 }

double __declspec(dllexport) WINAPI _get_infinite(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = (double) get_infinite(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_epsilon(lprec *lp, double epsilon)
 {
  if (lp != NULL) {
   freebuferror();
   set_epsilon(lp, (REAL) epsilon);
  }
 }

double __declspec(dllexport) WINAPI _get_epsilon(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = (double) get_epsilon(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_epsb(lprec *lp, double epsb)
 {
  if (lp != NULL) {
   freebuferror();
   set_epsb(lp, (REAL) epsb);
  }
 }

double __declspec(dllexport) WINAPI _get_epsb(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = (double) get_epsb(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_epsd(lprec *lp, double epsd)
 {
  if (lp != NULL) {
   freebuferror();
   set_epsd(lp, (REAL) epsd);
  }
 }

double __declspec(dllexport) WINAPI _get_epsd(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = (double) get_epsd(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_epsel(lprec *lp, double epsel)
 {
  if (lp != NULL) {
   freebuferror();
   set_epsel(lp, (REAL) epsel);
  }
 }

double __declspec(dllexport) WINAPI _get_epsel(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = (double) get_epsel(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_scalemode(lprec *lp, long scalemode)
 {
  if (lp != NULL) {
   freebuferror();
   set_scalemode(lp, (short) scalemode);
  }
 }

long __declspec(dllexport) WINAPI _get_scalemode(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_scalemode(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_improve(lprec *lp, long improve)
 {
  if (lp != NULL) {
   freebuferror();
   set_improve(lp, (short) improve);
  }
 }

long __declspec(dllexport) WINAPI _get_improve(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_improve(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_lag_trace(lprec *lp, long lag_trace)
 {
  if (lp != NULL) {
   freebuferror();
   set_lag_trace(lp, (short) lag_trace);
  }
 }

long __declspec(dllexport) WINAPI _is_lag_trace(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_lag_trace(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_piv_rule(lprec *lp, long piv_rule)
 {
  if (lp != NULL) {
   freebuferror();
   set_piv_rule(lp, (short) piv_rule);
  }
 }

long __declspec(dllexport) WINAPI _get_piv_rule(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_piv_rule(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_break_at_first(lprec *lp, long break_at_first)
 {
  if (lp != NULL) {
   freebuferror();
   set_break_at_first(lp, (short) break_at_first);
  }
 }

long __declspec(dllexport) WINAPI _is_break_at_first(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_break_at_first(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_break_at_value(lprec *lp, double break_at_value)
 {
  if (lp != NULL) {
   freebuferror();
   set_break_at_value(lp, break_at_value);
  }
 }

double __declspec(dllexport) WINAPI _get_break_at_value(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_break_at_value(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_negrange(lprec *lp, double negrange)
 {
  if (lp != NULL) {
   freebuferror();
   set_negrange(lp, negrange);
  }
 }

double __declspec(dllexport) WINAPI _get_negrange(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_negrange(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_splitnegvars(lprec *lp, short splitnegvars)
 {
  if (lp != NULL) {
   freebuferror();
   set_splitnegvars(lp, splitnegvars);
  }
 }

short __declspec(dllexport) WINAPI _get_splitnegvars(lprec *lp)
 {
  short ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_splitnegvars(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_epsperturb(lprec *lp, double epsperturb)
 {
  if (lp != NULL) {
   freebuferror();
   set_epsperturb(lp, epsperturb);
  }
 }

double __declspec(dllexport) WINAPI _get_epsperturb(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_epsperturb(lp);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_epspivot(lprec *lp, double epspivot)
 {
  if (lp != NULL) {
   freebuferror();
   set_epspivot(lp, epspivot);
  }
 }

double __declspec(dllexport) WINAPI _get_epspivot(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_epspivot(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_max_level(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_max_level(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_total_nodes(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_total_nodes(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_total_iter(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_total_iter(lp);
  }
  else
   ret = 0;
  return(ret);
 }

double __declspec(dllexport) WINAPI _get_objective(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_objective(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_ptr_variables(lprec *lp, double **variables)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_ptr_variables(lp, variables);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_variables(lprec *lp, double *variables)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_variables(lp, variables);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_ptr_constraints(lprec *lp, double **constraints)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_ptr_constraints(lp, constraints);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_constraints(lprec *lp, double *constraints)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_constraints(lp, constraints);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_ptr_lambda(lprec *lp, double **lambda)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_ptr_lambda(lp, lambda);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_lambda(lprec *lp, double *lambda)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_lambda(lp, lambda);
  }
  else
   ret = 0;
  return(ret);
 }

/* check if var is semi-continious */
long __declspec(dllexport) WINAPI _is_semicont(lprec *lp, long column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_semicont(lp, column);
  }
  else
   ret = 0;
  return(ret);
 }

/* Add SOS constraint */
long __declspec(dllexport) WINAPI _add_SOS(lprec *lp, char *name, long sostype, long priority, long count, int *sosvars, double *weights)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = add_SOS(lp, name, (short) sostype, priority, count, sosvars, weights);
  }
  else
   ret = 0;
  return(ret);
 }

/* check if var is SOS */
long __declspec(dllexport) WINAPI _is_SOS_var(lprec *lp, long column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_SOS_var(lp, column);
  }
  else
   ret = 0;
  return(ret);
 }

/* Set the name of the model */
long __declspec(dllexport) WINAPI _set_lp_name(lprec *lp, char *name)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_lp_name(lp, name);
  }
  else
   ret = 0;
  return(ret);
 }

/* Get the name of the model */
char __declspec(dllexport) *WINAPI _get_lp_name(lprec *lp)
 {
  char *ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_lp_name(lp);
  }
  else
   ret = NULL;
  return(ret);
 }

/* Set the right hand side of a constraint row */
long __declspec(dllexport) WINAPI _set_rh(lprec *lp, long row, double value)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_rh(lp, row, value);
  }
  else
   ret = 0;
  return(ret);
 }

/* Get the right hand side of a constraint row */
double __declspec(dllexport) WINAPI _get_rh(lprec *lp, long row)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_rh(lp, row);
  }
  else
   ret = 0.0;
  return(ret);
 }

/* Set the right hand side vector range */
long __declspec(dllexport) WINAPI _set_rh_range(lprec *lp, long row, double deltavalue)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_rh_range(lp, row, deltavalue);
  }
  else
   ret = 0;
  return(ret);
 }

/* Get the right hand side vector range */
double __declspec(dllexport) WINAPI _get_rh_range(lprec *lp, long row)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_rh_range(lp, row);
  }
  else
   ret = 0.0;
  return(ret);
 }

/* Set the right hand side vector */
void __declspec(dllexport) WINAPI _set_rh_vec(lprec *lp, double *rh)
 {
  if (lp != NULL) {
   freebuferror();
   set_rh_vec(lp, rh);
  }
 }

/* Set the right hand side vector with string input */
long __declspec(dllexport) WINAPI _str_set_rh_vec(lprec *lp, char *rh)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = str_set_rh_vec(lp, rh);
  }
  else
   ret = 0;
  return(ret);
 }

/* maximise the objective function */
void __declspec(dllexport) WINAPI _set_maxim(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   set_maxim(lp);
  }
 }

/* minimise the objective function */
void __declspec(dllexport) WINAPI _set_minim(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   set_minim(lp);
  }
 }

/* Set the type of constraint in row Row (LE, GE, EQ) */
long __declspec(dllexport) WINAPI _set_constr_type(lprec *lp, long row, long con_type)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_constr_type(lp, row, (short) con_type);
  }
  else
   ret = 0;
  return(ret);
 }

/* Get the type of constraint in row Row (LE, GE, EQ) */
long __declspec(dllexport) WINAPI _get_constr_type(lprec *lp, long row)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_constr_type(lp, row);
  }
  else
   ret = 0;
  return(ret);
 }

/* Set the name of a constraint row */
long __declspec(dllexport) WINAPI _set_row_name(lprec *lp, long row, char *new_name)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_row_name(lp, row, new_name);
  }
  else
   ret = 0;
  return(ret);
 }

/* Get the name of a constraint row */
char __declspec(dllexport) *WINAPI _get_row_name(lprec *lp, long row)
 {
  char *ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_row_name(lp, row);
  }
  else
   ret = NULL;
  return(ret);
 }

/* Set the name of a variable column */
long __declspec(dllexport) WINAPI _set_col_name(lprec *lp, long column, char *new_name)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = set_col_name(lp, column, new_name);
  }
  else
   ret = 0;
  return(ret);
 }

/* Get the name of a variable column */
char __declspec(dllexport) *WINAPI _get_col_name(lprec *lp, long column)
 {
  char *ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_col_name(lp, column);
  }
  else
   ret = NULL;
  return(ret);
 }

/* scale of the problem */
double __declspec(dllexport) WINAPI _scale(lprec *lp, double *myrowscale, double *mycolscale)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = scale(lp, myrowscale, mycolscale);
  }
  else
   ret = 0.0;
  return(ret);
 }

/* Automatic scaling of the problem */
double __declspec(dllexport) WINAPI _auto_scale(lprec *lp)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = auto_scale(lp);
  }
  else
   ret = 0.0;
  return(ret);
 }

/* Curtis-Reid scaling */
long __declspec(dllexport) WINAPI _scaleCR(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = scaleCR(lp);
  }
  else
   ret = 0;
  return(ret);
 }

/* Remove all scaling from the problem */
void __declspec(dllexport) WINAPI _unscale(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   unscale(lp);
  }
 }

/* Set the basis of a problem */
void __declspec(dllexport) WINAPI _set_basis(lprec *lp, int *bascolumn)
 {
  if (lp != NULL) {
   freebuferror();
   set_basis(lp, bascolumn);
  }
 }

/* Get the basis of a problem */
void __declspec(dllexport) WINAPI _get_basis(lprec *lp, int *bascolumn)
 {
  if (lp != NULL) {
   freebuferror();
   get_basis(lp, bascolumn);
  }
 }

/* Solve the problem */
long __declspec(dllexport) WINAPI _solve(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = solve(lp);
  }
  else
   ret = FAILURE;
  return(ret);
 }

/* Do NumIter iterations with Lagrangian relaxation constraints */
long __declspec(dllexport) WINAPI _lag_solve(lprec *lp, double start_bound, long num_iter, long verbose)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = lag_solve(lp, start_bound, num_iter, (short) ((verbose == 0) ? FALSE : TRUE));
  }
  else
   ret = FAILURE;
  return(ret);
 }

/* Reset the basis of a problem, can be usefull in case of degeneracy - JD */
void __declspec(dllexport) WINAPI _reset_basis(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   reset_basis(lp);
  }
 }

/* get a single element from the matrix */
double __declspec(dllexport) WINAPI _mat_elm(lprec *lp, long row, long column)
 {
  double ret;

  if (lp != NULL) {
   freebuferror();
   ret = mat_elm(lp, row, column);
  }
  else
   ret = 0.0;
  return(ret);
 }

/* fill row with the row row_nr from the problem */
long __declspec(dllexport) WINAPI _get_row(lprec *lp, long row_nr, double *row)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_row(lp, row_nr, row);
  }
  else
   ret = 0;
  return(ret);
 }

/* fill column with the column col_nr from the problem */
long __declspec(dllexport) WINAPI _get_column(lprec *lp, long col_nr, double *column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_column(lp, col_nr, column);
  }
  else
   ret = 0;
  return(ret);
 }

/* get the reduced costs vector */
long __declspec(dllexport) WINAPI _get_reduced_costs(lprec *lp, double *rc)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_reduced_costs(lp, rc);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_ptr_reduced_costs(lprec *lp, double **rc)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_ptr_reduced_costs(lp, rc);
  }
  else
   ret = 0;
  return(ret);
 }

/* get sensitivity objective function */
long __declspec(dllexport) WINAPI _get_ptr_sensitivity_obj(lprec *lp, double **objfrom, double **objtill)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_ptr_sensitivity_obj(lp, objfrom, objtill);
  }
  else
   ret = 0;
  return(ret);
 }

/* get sensitivity objective function */
long __declspec(dllexport) WINAPI _get_sensitivity_obj(lprec *lp, double *objfrom, double *objtill)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_sensitivity_obj(lp, objfrom, objtill);
  }
  else
   ret = 0;
  return(ret);
 }

/* get sensitivity objective function */
long __declspec(dllexport) WINAPI _get_ptr_sensitivity_objex(lprec *lp, double **objfrom, double **objtill, double **objfromvalue, double **objtillvalue)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_ptr_sensitivity_objex(lp, objfrom, objtill, objfromvalue, objtillvalue);
  }
  else
   ret = 0;
  return(ret);
 }

/* get sensitivity objective function */
long __declspec(dllexport) WINAPI _get_sensitivity_objex(lprec *lp, double *objfrom, double *objtill, double *objfromvalue, double *objtillvalue)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_sensitivity_objex(lp, objfrom, objtill, objfromvalue, objtillvalue);
  }
  else
   ret = 0;
  return(ret);
 }

/* get sensitivity RHS */
long __declspec(dllexport) WINAPI _get_ptr_sensitivity_rhs(lprec *lp, double **duals, double **dualsfrom, double **dualstill)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_ptr_sensitivity_rhs(lp, duals, dualsfrom, dualstill);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_orig_index(lprec *lp, int lp_index)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_orig_index(lp, lp_index);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_lp_index(lprec *lp, int orig_index)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_lp_index(lp, orig_index);
  }
  else
   ret = 0;
  return(ret);
 }

void __declspec(dllexport) WINAPI _set_solutionlimit(lprec *lp, int limit)
 {
  if (lp != NULL) {
   freebuferror();
   _set_solutionlimit(lp, limit);
  }
 }

long __declspec(dllexport) WINAPI _get_solutionlimit(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_solutionlimit(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_solutioncount(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_solutioncount(lp);
  }
  else
   ret = 0;
  return(ret);
 }

/* get sensitivity RHS */
long __declspec(dllexport) WINAPI _get_sensitivity_rhs(lprec *lp, double *duals, double *dualsfrom, double *dualstill)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_sensitivity_rhs(lp, duals, dualsfrom, dualstill);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_primal_solution(lprec *lp, double *pv)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_primal_solution(lp, pv);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_ptr_primal_solution(lprec *lp, double **pv)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_ptr_primal_solution(lp, pv);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_work_solution(lprec *lp, double *wv)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_work_solution(lp, wv);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_ptr_work_solution(lprec *lp, double **wv)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_ptr_work_solution(lp, wv);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_varpriority(lprec *lp, long column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_varpriority(lp, column);
  }
  else
   ret = FALSE;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_Nrows(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_Nrows(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_Norig_rows(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_Norig_rows(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_Lrows(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_Lrows(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_Ncolumns(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_Ncolumns(lp);
  }
  else
   ret = 0;
  return(ret);
 }

long __declspec(dllexport) WINAPI _get_Norig_columns(lprec *lp)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = get_Norig_columns(lp);
  }
  else
   ret = 0;
  return(ret);
 }

/* returns TRUE if the vector in values is a feasible solution to the lp */
long __declspec(dllexport) WINAPI _is_feasible(lprec *lp, double *values)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = is_feasible(lp, values);
  }
  else
   ret = FALSE;
  return(ret);
 }

/* returns TRUE if column is already present in lp. (Does not look at bounds
   and types, only looks at matrix values */
long __declspec(dllexport) WINAPI _column_in_lp(lprec *lp, double *column)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = column_in_lp(lp, column);
  }
  else
   ret = FALSE;
  return(ret);
 }

/* read a MPS file */
lprec __declspec(dllexport) * WINAPI _read_MPS(char *filename, long verbose)
 {
  lprec *lp;

  freebuferror();
  lp = read_MPS(filename, (short) ((verbose == 0) ? FALSE : TRUE));
  return(lp);
 }

/* write a MPS file to output */
long __declspec(dllexport) WINAPI _write_mps(lprec *lp, char *filename)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = write_mps(lp, filename);
  }
  return(ret);
 }

/* write a LP file to output */
long __declspec(dllexport) WINAPI _write_lp(lprec *lp, char *filename)
 {
  long ret;

  if (lp != NULL) {
   freebuferror();
   ret = write_lp(lp, filename);
  }
  return(ret);
 }

/* Print the current problem, only usefull in very small (test) problems.
  Shows the effect of scaling */
void __declspec(dllexport) WINAPI _print_lp(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   print_lp(lp);
  }
 }

/* Print the objective value */
void __declspec(dllexport) WINAPI _print_objective(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   print_objective(lp);
  }
 }

/* Print the solution */
void __declspec(dllexport) WINAPI _print_solution(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   print_solution(lp);
  }
 }

/* Print the constrataints */
void __declspec(dllexport) WINAPI _print_constraints(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   print_constraints(lp);
  }
 }

/* If scaling is used, print the scaling factors */
void __declspec(dllexport) WINAPI _print_scales(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   print_scales(lp);
  }
 }

/* Print the dual variables of the solution */
void __declspec(dllexport) WINAPI _print_duals(lprec *lp)
 {
  if (lp != NULL) {
   freebuferror();
   print_duals(lp);
  }
 }

/* file where results are printed to. Default stdout. If NULL then back stdout */
long __declspec(dllexport) WINAPI _print_file(char *filename)
 {
  freebuferror();
  return(print_file(filename));
 }

void __declspec(dllexport) WINAPI _log_file(char *filename)
 {
  freebuferror();
  if (fplogfile != NULL)
   fclose(fplogfile);
  if (filename == NULL)
   fplogfile = NULL;
  else
   fplogfile = fopen(filename, "w");
 }

/* print a string */
void __declspec(dllexport) WINAPI _print_str(char *str)
 {
  print_str(str);
 }

void __declspec(dllexport) WINAPI _put_abortfunc(lprec *lp, abortfunc newabort, void *aborthandle)
 {
  put_abortfunc(lp, newabort, aborthandle);
 }

void __declspec(dllexport) WINAPI _put_logfunc(lprec *lp, logfunc newlog, void *loghandle)
 {
  put_logfunc(lp, newlog, loghandle);
 }

void __declspec(dllexport) WINAPI _put_msgfunc(lprec *lp, msgfunc newmsg, void *msghandle, int mask)
 {
  put_msgfunc(lp, newmsg, msghandle, mask);
 }

int EndOfPgr(int exitcode)
 {
  return(0);
 }

void __declspec(dllexport) WINAPI __Fortify_EnterScope()
 {
#if defined FORTIFY
  Fortify_EnterScope();
#endif
 }

void __declspec(dllexport) WINAPI __Fortify_LeaveScope()
 {
#if defined FORTIFY
  Fortify_LeaveScope();
#endif
 }
