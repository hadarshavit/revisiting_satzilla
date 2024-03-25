#include <string.h>
#include <time.h>
#include <signal.h>
#include "lpkit.h"
#include "patchlevel.h"

int EndOfPgr(int exitcode)
 {
  exit(EXIT_FAILURE);
 }

void SIGABRT_func(int sig)
 {
  EndOfPgr(EXIT_FAILURE);
 }

void print_help(char *argv[])
{
  printf("Usage of %s version %d.%d.%d.%d:\n", argv[0], MAJORVERSION, MINORVERSION, RELEASE, BUILD);
  printf("%s [options] [[<]input_file]\n", argv[0]);
  printf("List of options:\n");
  printf("-h\t\tprints this message\n");
  printf("-v <level>\tverbose mode, gives flow through the program.\n");
  printf("\t\t if level not provided (-v) then -v4 (NORMAL) is taken.\n");
  printf("\t -v1: CRITICAL\n");
  printf("\t -v2: SEVERE\n");
  printf("\t -v3: IMPORTANT (default)\n");
  printf("\t -v4: NORMAL\n");
  printf("\t -v5: DETAILED\n");
  printf("\t -v6: FULL\n");
  printf("-d\t\tdebug mode, all intermediate results are printed,\n\t\tand the branch-and-bound decisions\n");
  printf("-p\t\tprint the values of the dual variables\n");
  printf("-b <bound>\tspecify a lower bound for the objective function\n\t\tto the program. If close enough, may speed up the\n\t\tcalculations.\n");
  printf("-i\t\tprint all intermediate valid solutions.\n\t\tCan give you useful solutions even if the total run time\n\t\tis too long\n");
  printf("-e <number>\tspecifies the epsilon which is used to determine whether a\n\t\tfloating point number is in fact an integer.\n\t\tShould be < 0.5\n");
  printf("-g <number>\tspecifies the MIP gap for branch-and-bound.\n\t\tThis specifies the allowed tolerance on the object function.\n\t\tCan result in faster solving times.\n");
  printf("-f\t\tspecifies that branch-and-bound algorithm stops at first found\n");
  printf("\t\tsolution\n");
  printf("-o <value>\tspecifies that branch-and-bound algorithm stops when objective\n");
  printf("\t\tvalue is better than value\n");
  printf("-c\t\tduring branch-and-bound, take the ceiling branch first\n");
  printf("-ca\t\tduring branch-and-bound, the algorithm chooses branch\n");
  printf("-B <rule>\tspecify branch-and-bound rule\n");
  printf("\t -B0: Select Lowest indexed non-integer column (default)\n");
  printf("\t -B1: Select Random non-integer column\n");
  printf("\t -B2: Select Largest deviation from an integer value\n");
  printf("\t -B3: Select Best ???\n");
  printf("\t -B4: Select Median value deviation from an integer value\n");
  printf("\t -B5: Select Greedy ???\n");
  printf("-piv <rule>\tspecify simplex pivot rule\n");
  printf("\t -piv0: Select first\n");
  printf("\t -piv3: Select most negative (default)\n");
  printf("\t -piv5: Select greedy\n");
  printf("-s <mode>\tuse automatic problem scaling.\n");
  printf("\t  -s:\n");
  printf("\t -s0: Numerical range-based scaling\n");
  printf("\t -s1: Geometric scaling\n");
  printf("\t -s2: Curtis-reid scaling\n");
  printf("-sp\t\talso do power scaling.\n");
  printf("-sl\t\talso do Lagrange scaling.\n");
  printf("-si\t\talso do Integer scaling.\n");
  printf("-I\t\tprint info after reinverting\n");
  printf("-t\t\ttrace pivot selection\n");
  printf("-lp\t\tread from LP file (default)\n");
  printf("-mps\t\tread from MPS file\n");
#if defined CPLEX
  printf("-lpt\t\tread from CPLEX file\n");
#endif
  printf("-degen\t\tuse perturbations to reduce degeneracy,\n\t\tcan increase numerical instability\n");
  printf("-trej <Trej>\tset minimum pivot value\n");
  printf("-epsd <epsd>\tset minimum tolerance for reduced costs\n");
  printf("-epsb <epsb>\tset minimum tolerance for the RHS\n");
  printf("-epsel <epsel>\tset tolerance\n");
  printf("-parse_only\tparse input file but do not calculate (ie check)\n");
  printf("-presolve\tpresolve problem before start optimizing\n");
  printf("-improve <level>\titerative improvement level\n");
  printf("\t -improve0: none (default)\n");
  printf("\t -improve1: FTRAN only\n");
  printf("\t -improve2: BTRAN only\n");
  printf("\t -improve3: FTRAN + BTRAN\n");
  printf("-time\t\tPrint CPU time to parse input and to calculate result.\n");
  printf("-timeout <sec>\tTimeout after sec seconds when not solution found.\n");
  printf("-timeoutok\tIf timeout, take the best yet found solution.\n");
  printf("-min\t\tMinimize the lp problem (overrules setting in file)\n");
  printf("-max\t\tMaximize the lp problem (overrules setting in file)\n");
  printf("-S <detail>\tPrint solution. If detail ommited, then -S2 is used.\n");
  printf("\t -S0: Print nothing\n");
  printf("\t -S1: Only objective value\n");
  printf("\t -S2: Obj value+variables (default)\n");
  printf("\t -S3: Obj value+variables+constraints\n");
  printf("\t -S4: Obj value+variables+constraints+duals\n");
  printf("\t -S5: Obj value+variables+constraints+duals+lp model\n");
  printf("\t -S6: Obj value+variables+constraints+duals+lp model+lp scales\n");
}

void print_cpu_times(const char *info)
{
  static clock_t last_time = 0;
  clock_t new_time;

  new_time = clock();
  fprintf(stderr, "CPU Time for %s: %gs (%gs total since program start)\n",
	  info, (new_time - last_time) / (double) CLOCKS_PER_SEC,
	  new_time / (double) CLOCKS_PER_SEC);
  last_time = new_time;
}

#if 0
int myabortfunc(lprec *lp, void *aborthandle)
{
  /* printf("%f\n",lp->rhs[0]*(lp->maximise ? 1 : -1)); */
  return(0);
}
#endif

int main(int argc, char *argv[])
{
  lprec *lp;
  char *filen;
  int i;
  short verbose = IMPORTANT /* CRITICAL */;
  short debug = FALSE;
  short print_sol = FALSE;
  short PRINT_DUALS = FALSE;
  short floor_first;
  short break_at_first;
  short scaling = 0;
  short print_at_invert;
  short tracing;
  short filetype = 0;
  short anti_degen;
  short print_timing = FALSE;
  short parse_only = FALSE;
  short do_presolve;
  short objective = 0;
  short PRINT_SOLUTION = 2;
  short improve;
  short piv_rule;
  short bb_rule;
  short scalemode;
  short timeoutok = FALSE;
  long sectimeout;
  int result;
  REAL obj_bound;
  REAL mip_gap;
  REAL epsilon;
  REAL epspivot;
  REAL epsd;
  REAL epsb;
  REAL epsel;
  REAL break_at_value;
  FILE *fpin = stdin;

  /* read command line arguments */

# if defined FORTIFY
   Fortify_EnterScope();
# endif

  lp = make_lp(0, 0);
  floor_first = get_floor_first(lp);
  break_at_first = is_break_at_first(lp);
  print_at_invert = is_print_at_invert(lp);
  tracing = is_trace(lp);
  anti_degen = is_anti_degen(lp);
  do_presolve = is_do_presolve(lp);
  improve = get_improve(lp);
  piv_rule = get_piv_rule(lp);
  bb_rule = get_bb_rule(lp);
  scalemode = get_scalemode(lp);
  sectimeout = get_timeout(lp);
  obj_bound = get_obj_bound(lp);
  mip_gap = get_mip_gap(lp);
  epsilon = get_epsilon(lp);
  epspivot = get_epspivot(lp);
  epsd = get_epsd(lp);
  epsb = get_epsb(lp);
  epsel = get_epsel(lp);
  break_at_value = get_break_at_value(lp);
  delete_lp(lp);

  for(i = 1; i < argc; i++) {
    if(strncmp(argv[i], "-v", 2) == 0) {
      if (argv[i][2])
        verbose = (short) atoi(argv[i] + 2);
      else
        verbose = NORMAL;
    }
    else if(strcmp(argv[i], "-d") == 0)
      debug = TRUE;
    else if(strcmp(argv[i], "-i") == 0)
      print_sol = TRUE;
    else if(strcmp(argv[i], "-c") == 0)
      floor_first = BRANCH_CEILING;
    else if(strcmp(argv[i], "-ca") == 0)
      floor_first = BRANCH_AUTOMATIC;
    else if(strncmp(argv[i], "-B", 2) == 0) {
      if (argv[i][2])
        bb_rule = (short) atoi(argv[i] + 2);
      else
        bb_rule = FIRST_SELECT;
    }
    else if((strcmp(argv[i], "-b") == 0) && (i + 1 < argc))
      obj_bound = atof(argv[++i]);
    else if((strcmp(argv[i], "-g") == 0) && (i + 1 < argc))
      mip_gap = atof(argv[++i]);
    else if((strcmp(argv[i], "-e") == 0) && (i + 1 < argc)) {
      epsilon = atof(argv[++i]);
      if((epsilon <= 0.0) || (epsilon >= 0.5)) {
	fprintf(stderr, "Invalid epsilon %g; 0 < epsilon < 0.5\n",
		(double)epsilon);
	exit(EXIT_FAILURE);
      }
    }
    else if((strcmp(argv[i], "-o") == 0) && (i + 1 < argc))
      break_at_value = atof(argv[++i]);
    else if(strcmp(argv[i], "-f") == 0)
      break_at_first = TRUE;
    else if(strcmp(argv[i], "-p") == 0)
      PRINT_DUALS = TRUE;
    else if(strcmp(argv[i], "-timeoutok") == 0)
      timeoutok = TRUE;
    else if(strcmp(argv[i], "-h") == 0) {
      print_help(argv);
      exit(EXIT_SUCCESS);
    }
    else if(strcmp(argv[i], "-sp") == 0)
      scalemode = (short) (scalemode | POWERSCALE);
    else if(strcmp(argv[i], "-sl") == 0)
      scalemode = (short) (scalemode | LAGRANGESCALE);
    else if(strcmp(argv[i], "-si") == 0)
      scalemode = (short) (scalemode | INTEGERSCALE);
    else if(strncmp(argv[i], "-s", 2) == 0) {
      scaling = 1;
      scalemode &= ~(MMSCALING | GEOSCALING | CURTISREIDSCALE);
      if (argv[i][2]) {
        switch (atoi(argv[i] + 2)) {
        case 1:
	 scalemode |= GEOSCALING;
	 break;
	case 2:
	 scalemode |= CURTISREIDSCALE;
	 break;
        }
      }
    }
    else if(strcmp(argv[i], "-I") == 0)
      print_at_invert = TRUE;
    else if(strcmp(argv[i], "-t") == 0)
      tracing = TRUE;
    else if(strncmp(argv[i], "-S", 2) == 0) {
      if (argv[i][2])
        PRINT_SOLUTION = (short) atoi(argv[i] + 2);
      else
        PRINT_SOLUTION = 2;
    }
    else if(strncmp(argv[i], "-improve", 8) == 0) {
      if (argv[i][8])
        improve = (short) atoi(argv[i] + 8);
      else
        improve = 0;
    }
    else if(strncmp(argv[i], "-piv", 4) == 0) {
      if (argv[i][4])
        piv_rule = (short) atoi(argv[i] + 4);
      else
        piv_rule = BEST_SELECT;
    }
    else if(strcmp(argv[i],"-lp") == 0)
      filetype = 0;
    else if(strcmp(argv[i],"-mps") == 0)
      filetype = 1;
#if defined CPLEX
    else if(strcmp(argv[i],"-lpt") == 0)
      filetype = 2;
#endif
    else if(strcmp(argv[i],"-degen") == 0)
      anti_degen = TRUE;
    else if(strcmp(argv[i],"-time") == 0) {
      if(clock() == -1)
	fprintf(stderr, "CPU times not available on this machine\n");
      else
	print_timing = TRUE;
    }
    else if((strcmp(argv[i],"-timeout") == 0) && (i + 1 < argc))
      sectimeout = atol(argv[++i]);
    else if((strcmp(argv[i],"-trej") == 0) && (i + 1 < argc))
      epspivot = atof(argv[++i]);
    else if((strcmp(argv[i],"-epsd") == 0) && (i + 1 < argc))
      epsd = atof(argv[++i]);
    else if((strcmp(argv[i],"-epsb") == 0) && (i + 1 < argc))
      epsb = atof(argv[++i]);
    else if((strcmp(argv[i],"-epsel") == 0) && (i + 1 < argc))
      epsel = atof(argv[++i]);
    else if(strcmp(argv[i],"-parse_only") == 0)
      /* only useful for parser software development */
      parse_only = TRUE;
    else if(strcmp(argv[i],"-presolve") == 0)
      do_presolve = TRUE;
    else if(strcmp(argv[i],"-min") == 0)
      objective = -1;
    else if(strcmp(argv[i],"-max") == 0)
      objective =  1;
    else if(fpin == stdin) {
      filen = argv[i];
      if(*filen == '<')
        filen++;
      if((fpin = fopen(filen, "r")) == NULL) {
	fprintf(stderr,"Error, Unable to open input file '%s'\n",
		argv[i]);
	print_help(argv);
	exit(EXIT_FAILURE);
      }
    }
    else {
      filen = argv[i];
      if(*filen != '>') {
        fprintf(stderr, "Error, Unrecognized command line argument '%s'\n",
		argv[i]);
        print_help(argv);
        exit(EXIT_FAILURE);
      }
    }
  }

  signal(SIGABRT,/* (void (*) OF((int))) */ SIGABRT_func);

  switch(filetype) {
  case 0:
    lp = read_lp(fpin, verbose, "lp" );
    break;
  case 1:
    lp = read_mps(fpin, verbose);
    break;
#if defined CPLEX
  case 2:
    lp = read_cplex(fpin, verbose, "lp" );
    break;
#endif
  }

  if(fpin != stdin)
    fclose(fpin);

  if(print_timing)
    print_cpu_times("Parsing input");

  if(lp == NULL) {
    fprintf(stderr, "Unable to read model.\n");
#   if defined FORTIFY
      Fortify_LeaveScope();
#   endif
    exit(EXIT_FAILURE);
  }

  if(parse_only) {
    delete_lp(lp);
#   if defined FORTIFY
      Fortify_LeaveScope();
#   endif
    exit(0);
  }

  if(objective != 0) {
    if(objective == 1)
      set_maxim(lp);
    else
      set_minim(lp);
  }

  if(PRINT_SOLUTION >= 5)
    print_lp(lp);

#if 0
  put_abortfunc(lp,(abortfunc *) myabortfunc, NULL);
#endif

  if(sectimeout>0)
    set_timeout(lp, sectimeout);
  set_print_sol(lp, print_sol);
  set_epsilon(lp, epsilon);
  set_epspivot(lp, epspivot);
  set_epsd(lp, epsd);
  set_epsb(lp, epsb);
  set_epsel(lp, epsel);
  set_debug(lp, debug);
  set_floor_first(lp, floor_first);
  set_print_at_invert(lp, print_at_invert);
  set_trace(lp, tracing);
  set_obj_bound(lp, obj_bound);
  set_break_at_value(lp, break_at_value);
  set_break_at_first(lp, break_at_first);
  set_mip_gap(lp, mip_gap);
  set_anti_degen(lp, anti_degen);
  set_do_presolve(lp, do_presolve);
  set_improve(lp, improve);
  set_piv_rule(lp, piv_rule);
  set_scalemode(lp, scalemode);
  set_bb_rule(lp, bb_rule);

  if(scaling)
    auto_scale(lp);

  if(PRINT_SOLUTION >= 6)
    print_scales(lp);

  result = solve(lp);

  if(print_timing)
    print_cpu_times("solving");

  if((timeoutok) && (result == TIMEOUT) && (get_solutioncount(lp) > 0))
    result = OPTIMAL;

  switch(result) {
  case OPTIMAL:
    if (PRINT_SOLUTION >= 1)
      print_objective(lp);

    if (PRINT_SOLUTION >= 2)
      print_solution(lp);

    if (PRINT_SOLUTION >= 3)
      print_constraints(lp);

    if ((PRINT_SOLUTION >= 4) || (PRINT_DUALS))
      print_duals(lp);

    if(tracing)
      fprintf(stderr,
	      "Branch & Bound depth: %d\nNodes processed: %d\nSimplex pivots: %d\n",
	      get_max_level(lp), get_total_nodes(lp), get_total_iter(lp));
    break;
  case INFEASIBLE:
    if (PRINT_SOLUTION >= 1)
      printf("This problem is infeasible\n");
    break;
  case UNBOUNDED:
    if (PRINT_SOLUTION >= 1)
      printf("This problem is unbounded\n");
    break;
  case TIMEOUT:
    if (PRINT_SOLUTION >= 1)
      printf("Timeout\n");
    break;
  case USERABORT:
    if (PRINT_SOLUTION >= 1)
      printf("User aborted\n");
    break;
  default:
    if (PRINT_SOLUTION >= 1)
      printf("lp_solve failed\n");
    break;
  }

  delete_lp(lp);

# if defined FORTIFY
   Fortify_LeaveScope();
# endif

  return(result);
}
