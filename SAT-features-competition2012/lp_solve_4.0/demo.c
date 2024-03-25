/*
 This program is originally made by by Jeroen J. Dirks (jeroend@tor.numetrix.com)
 Adapted by Peter Notebaert (peno@mailme.org)
*/

#include <stdio.h>

#include "lpkit.h"

#if defined FORTIFY
int EndOfPgr(int i)
{
    exit(i);
}
#endif

void press_ret(void)
{
  printf("[return]");
  getchar();
}

int main(void)
{
# if defined ERROR
#  undef ERROR
# endif
# define ERROR() { fprintf(stderr, "Error\n"); exit(1); }
  lprec *lp1,*lp2;
  int majorversion, minorversion, release, build;

#if defined FORTIFY
  Fortify_EnterScope();
#endif

  lp_solve_version(&majorversion, &minorversion, &release, &build);
  printf("lp_solve %d.%d.%d.%d demo\n\n", majorversion, minorversion, release, build);
  printf("This demo will show most of the features of lp_solve %d.%d.%d.%d\n", majorversion, minorversion, release, build);
  press_ret();
  printf("\nWe start by creating a new problem with 4 variables and 0 constraints\n");
  printf("We use: lp1=make_lp(0,4);\n");
  if ((lp1=make_lp(0,4)) == NULL)
    ERROR();
  press_ret();

  printf("We can show the current problem with print_lp(lp1)\n");
  print_lp(lp1);
  press_ret();
  printf("Now we add some constraints\n");
  printf("str_add_constraint(lp1, \"3 2 2 1\" ,LE,4)\n");
  printf("This is the string version of add_constraint. For the normal version\n");
  printf("of add_constraint see the file lpkit.h\n");
  if (!str_add_constraint(lp1, "3 2 2 1", LE, 4))
    ERROR();
  print_lp(lp1);
  press_ret();
  printf("str_add_constraint(lp1, \"0 4 3 1\" ,GE,3)\n");
  if (!str_add_constraint(lp1, "0 4 3 1", GE, 3))
    ERROR();
  print_lp(lp1);
  press_ret();
  printf("Set the objective function\n");
  printf("str_set_obj_fn(lp1, \"2 3 -2 3\")\n");
  if (!str_set_obj_fn(lp1, "2 3 -2 3"))
    ERROR();
  print_lp(lp1);
  press_ret();
  printf("Now solve the problem with printf(solve(lp1));\n");
  printf("%d",solve(lp1));
  press_ret();
  printf("The value is 0, this means we found an optimal solution\n");
  printf("We can display this solution with print_objective(lp1) and print_solution(lp1)\n");
  print_objective(lp1);
  print_solution(lp1);
  print_constraints(lp1);

  press_ret();
  printf("The dual variables of the solution are printed with\n");
  printf("print_duals(lp1);\n");
  print_duals(lp1);
  press_ret();
  printf("We can change a single element in the matrix with\n");
  printf("set_mat(lp1,2,1,0.5)\n");
  if (!set_mat(lp1,2,1,0.5))
    ERROR();
  print_lp(lp1);
  press_ret();
  printf("If we want to maximize the objective function use set_maxim(lp1);\n");
  set_maxim(lp1);
  print_lp(lp1);
  press_ret();
  printf("after solving this gives us:\n");
  solve(lp1);
  print_objective(lp1);
  print_solution(lp1);
  print_constraints(lp1);
  print_duals(lp1);
  press_ret();
  printf("Change the value of a rhs element with set_rh(lp1,1,7.45)\n");
  set_rh(lp1,1,7.45);
  print_lp(lp1);
  solve(lp1);
  print_objective(lp1);
  print_solution(lp1);
  print_constraints(lp1);
  press_ret();
  printf("We change %s to the integer type with\n", get_col_name(lp1, 4));
  printf("set_int(lp1, 4, TRUE)\n");
  set_int(lp1, 4, TRUE);
  print_lp(lp1);
  printf("We set branch & bound debugging on with set_debug(lp1, TRUE)\n");
  set_debug(lp1, TRUE);
  printf("and solve...\n");
  press_ret();
  solve(lp1);
  print_objective(lp1);
  print_solution(lp1);
  print_constraints(lp1);
  press_ret();
  printf("We can set bounds on the variables with\n");
  printf("set_lowbo(lp1,2,2); & set_upbo(lp1,4,5.3)\n");
  set_lowbo(lp1,2,2);
  set_upbo(lp1,4,5.3);
  print_lp(lp1);
  press_ret();
  solve(lp1);
  print_objective(lp1);
  print_solution(lp1);
  print_constraints(lp1);
  press_ret();
  printf("Now remove a constraint with del_constraint(lp1, 1)\n");
  del_constraint(lp1,1);
  print_lp(lp1);
  printf("Add an equality constraint\n");
  if (!str_add_constraint(lp1, "1 2 1 4", EQ, 8))
    ERROR();
  print_lp(lp1);
  press_ret();
  printf("A column can be added with:\n");
  printf("str_add_column(lp1,\"3 2 2\");\n");
  if (!str_add_column(lp1,"3 2 2"))
    ERROR();
  print_lp(lp1);
  press_ret();
  printf("A column can be removed with:\n");
  printf("del_column(lp1,3);\n");
  del_column(lp1,3);
  print_lp(lp1);
  press_ret();
  printf("We can use automatic scaling with:\n");
  printf("auto_scale(lp1);\n");
  auto_scale(lp1);
  print_lp(lp1);
  press_ret();
  printf("The function mat_elm(lprec *lp, int row, int column) returns a single\n");
  printf("matrix element\n");
  printf("%s mat_elm(lp1,2,3), mat_elm(lp1,1,1); gives\n","printf(\"%f %f\\n\",");
  printf("%f %f\n", (double)mat_elm(lp1,2,3), (double)mat_elm(lp1,1,1));
  printf("Notice that mat_elm returns the value of the original unscaled problem\n");
  press_ret();
  printf("If there are any integer type variables, then only the rows are scaled\n");
  printf("set_int(lp1,3,FALSE);\n");
  printf("auto_scale(lp1);\n");
  set_int(lp1,3,FALSE);
  auto_scale(lp1);
  print_lp(lp1);
  press_ret();
  solve(lp1);
  printf("print_objective, print_solution gives the solution to the original problem\n");
  print_objective(lp1);
  print_solution(lp1);
  print_constraints(lp1);
  press_ret();
  printf("Scaling is turned off with unscale(lp1);\n");
  unscale(lp1);
  print_lp(lp1);
  press_ret();
  printf("Now turn B&B debugging off and simplex tracing on with\n");
  printf("set_debug(lp1, FALSE), set_trace(lp1, TRUE) and solve(lp1)\n");
  set_debug(lp1, FALSE);
  set_trace(lp1, TRUE);
  press_ret();
  solve(lp1);
  printf("Where possible, lp_solve will start at the last found basis\n");
  printf("We can reset the problem to the initial basis with\n");
  printf("reset_basis(lp1). Now solve it again...\n");
  press_ret();
  reset_basis(lp1);
  solve(lp1);

  printf("It is possible to give variables and constraints names\n");
  printf("set_row_name(lp1,1,\"speed\"); & set_col_name(lp1,2,\"money\")\n");
  if (!set_row_name(lp1,1,"speed"))
    ERROR();
  if (!set_col_name(lp1,2,"money"))
    ERROR();
  print_lp(lp1);
  printf("As you can see, all column and rows are assigned default names\n");
  printf("If a column or constraint is deleted, the names shift place also:\n");
  press_ret();
  printf("del_column(lp1,1);\n");
  del_column(lp1,1);
  print_lp(lp1);
  press_ret();

  delete_lp(lp1);

  printf("A lp structure can be created and read from a .lp file\n");
  printf("lp2 = read_lp(\"lp_examples/demo_lag.lp\", TRUE);\n");
  printf("The verbose option is used\n");
  if ((lp2 = read_LP("lp_examples/demo_lag.lp", TRUE, "test")) == NULL)
    ERROR();
  press_ret();
  printf("lp2 is now:\n");
  print_lp(lp2);

  press_ret();
  printf("solution:\n");
  set_debug(lp2, TRUE);
  solve(lp2);
  set_debug(lp2, FALSE);
  print_objective(lp2);
  print_solution(lp2);
  print_constraints(lp2);
  press_ret();
  printf("You can see that branch & bound was used in this problem\n");
  printf("Now remove the last constraint and use lagrangian relaxation\n");
  printf("del_constraint(lp2,6);\n");
  printf("str_add_lag_con(lp2, \"1 1 1 0 0 0\", LE, 2);\n");
  del_constraint(lp2,6);
  if (!str_add_lag_con(lp2, "1 1 1 0 0 0", LE, 2))
    ERROR();
  print_lp(lp2);

  printf("Lagrangian relaxation is used in some heuristics. It is now possible\n");
  printf("to get a feasible integer solution without usage of branch & bound.\n");
  printf("Use lag_solve(lp2, 0, 30); 0 is the initial bound, 30 the maximum\n");
  printf("number of iterations, the last variable turns the verbose mode on.\n");
  press_ret();
  set_lag_trace(lp2, TRUE);
  printf("%d\n",lag_solve(lp2, 0, 30, FALSE));
  printf("The returncode of lag_solve is 6 or FEAS_FOUND. this means that a feasible\n");
  printf("solution has been found. For a list of other possible return values\n");
  printf("see \"lpkit.h\". Print this solution with print_objective, print_solution\n");
  print_objective(lp2);
  print_solution(lp2);
  print_constraints(lp2);

  delete_lp(lp2);

  press_ret();

#if defined FORTIFY
  Fortify_LeaveScope();
#endif

  return(0);
}
