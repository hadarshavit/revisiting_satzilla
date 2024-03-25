/*
  Main header file of the LP_SOLVE toolkit.

  Original by Jeroen Dirks, 21-2-95
  Maintained by Michel Berkelaar

  Starting at version 3.0, LP_Solve is released under the LGPL license.
  For full information, see the enclosed file LGPL.txt.

  See CHANGELOG file for a tracking of changes

*/

#ifndef __LPKIT_H__
#define __LPKIT_H__

/* let's please C++ users */
#ifdef __cplusplus
extern "C" {
#endif

#define _WINAPI

#if defined _WINDOWS || defined LPSOLVEDLL
# include <windows.h>
# if defined LPSOLVEDLL && !defined CYGWIN
#  undef _WINAPI
#  define _WINAPI WINAPI
# endif
#else
# if defined WINAPI
#  undef WINAPI
# endif
# define WINAPI
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "hash.h"

#if defined FORTIFY
# include "fortify.h"
#endif

#ifndef NULL
#define NULL 	        0
#endif

#define CONST const

#define FALSE           0
#define TRUE            1
#define AUTOMATIC       2

#define BRANCH_CEILING   0
#define BRANCH_FLOOR     1
#define BRANCH_AUTOMATIC 2
#define BRANCH_DEFAULT   3

#define ISREAL          0
#define ISINTEGER       1
#define ISSEMI          2
#define ISSOS           4
#define ISSOSTEMPINT    8

#define ROWNAMEMASK     "r_%d"
#define COLNAMEMASK     "v_%d"

/* REPORT defines */
#define CRITICAL        1
#define SEVERE          2
#define IMPORTANT       3
#define NORMAL          4
#define DETAILED        5
#define FULL            6

/* B&B active variable codes */
#define BB_INT          0
#define BB_SC           1
#define BB_SOS          2

/* SOS constraint defines */
#define SOS1            1
#define SOS2            2
#define SOS3           -1
#define SOS_START_SIZE 10     /* start size of SOS_list array; realloced if needed */

/* MESSAGE defines */
#define MSG_NONE             0
#define MSG_PRESOLVE         1
#define MSG_ITERATION        2
#define MSG_INVERT           4
#define MSG_LPFEASIBLE       8
#define MSG_LPEQUAL         16
#define MSG_LPBETTER        32
#define MSG_MILPFEASIBLE    64
#define MSG_MILPEQUAL      128
#define MSG_MILPBETTER     256
#define MSG_MILPSTRATEGY   512

#ifndef DEFNUMINV
# define DEFNUMINV      30
#endif

#ifndef HASHSIZE
# define HASHSIZE       10007  /* A prime number! */
#endif

#ifndef INITIAL_MAT_SIZE
# define INITIAL_MAT_SIZE 10000
#endif

#ifndef DELTACOLALLOC
# define DELTACOLALLOC  50
#endif

/* Improvement defines */
#define IMPROVE_NONE    0
#define IMPROVE_FTRAN   1
#define IMPROVE_BTRAN   2
#define IMPROVE_FULL    (IMPROVE_FTRAN + IMPROVE_BTRAN)

/* Scaling defines */
#define MMSCALING       0
#define GEOSCALING      1
#define POWERSCALE      2
#define CURTISREIDSCALE 4
#define LAGRANGESCALE   8
#define INTEGERSCALE   16

/* B&B strategies */
#define FIRST_NI	0
#define RAND_NI		1

#define FIRST_SELECT    0
#define RAND_SELECT	1
#define WORST_SELECT    2
#define BEST_SELECT     3
#define MEDIAN_SELECT   4
#define GREEDY_SELECT   5
/* unused ...
#define AUTO_SELECT     6
#define USER_SELECT     7
*/

/* solve status values */
#define UNKNOWN        -5
#define NOTRUN         -4
#define USERABORT      -3
#define TIMEOUT        -2
#define IGNORED        -1
#define OPTIMAL     	0
#define MILP_FAIL   	1
#define INFEASIBLE  	2
#define UNBOUNDED   	3
#define FAILURE     	4
#define RUNNING     	5

/* lag_solve extra status values */
#define FEAS_FOUND   	6
#define NO_FEAS_FOUND 	7
#define BREAK_BB        8

/* Status values internal to the solver */
#define	SWITCH_TO_PRIMAL         9
#define SINGULAR_BASIS          10
#define	LOST_PRIMAL_FEASIBILITY 11
#define	OUT_OF_MEMORY           12

#define LE      0
#define EQ      1
#define GE      2
#define OF      3

/* Solver parameters and tolerances */
#ifndef RESIZEFACTOR
# define RESIZEFACTOR          4
#endif

#ifndef DEFNUMINV
# define DEFNUMINV            30 /* maximum number of pivots before reinversion */
#endif
#ifndef DEF_MAXRELAX
# define DEF_MAXRELAX          4 /* maximum number of non-BB relaxations in MILP */
#endif

#ifndef DEF_MAXSINGULARITIES
# define DEF_MAXSINGULARITIES  10 /* maximum number of singularities in inversion */
#endif

#ifndef LAG_SINGULARLIMIT
# define LAG_SINGULARLIMIT     5 /* Number of times the objective does not change
                                    before it is assumed that the Lagrangean constraints
                                    are non-binding, and therefore impossible to converge;
                                    upper iteration limit is divided by this threshold */
#endif

#ifndef DEF_INFINITE
# define DEF_INFINITE  1e24     /* limit for dynamic range */
#endif

#ifndef DEF_NEGRANGE
# define DEF_NEGRANGE     0     /* downward limit for expanded variable range
                                   before the variable is split into positive and
                                   negative components */
#endif

#ifndef DEF_SPLITNEGVARS
# define DEF_SPLITNEGVARS AUTOMATIC /* split negative variables in a positive and negative component
				       Can be TRUE or AUTOMATIC
				       TRUE is always split negative variables
				       AUTOMATIC is only split when negative lower bound and positive upper bound */
#endif

#ifndef DEF_EPSPIVOT
# define DEF_EPSPIVOT  1e-5     /* 5 pivot reject (try others first)  */
#endif

#ifndef DEF_EPSEL
# ifdef ORGPARAM
#  define DEF_EPSEL     1e-8    /* for rounding other values (vectors) to 0 */
# else
#  define DEF_EPSEL     1e-11    /* 10 for rounding other values (vectors) to 0 */
# endif
#endif

#ifndef DEF_EPSB
# ifdef ORGPARAM
#  define DEF_EPSB   5.01e-7    /* 8 for rounding RHS values to 0;
                                   determine infeasibility basis */
# else
#  define DEF_EPSB   5.01e-9    /* for rounding RHS values to 0 determine
				   infeasibility basis */
# endif
#endif

#ifndef DEF_EPSD
# ifdef ORGPARAM
#  define DEF_EPSD      1e-6    /* for rounding reduced costs to zero */
# else
#  define DEF_EPSD      1e-9    /* 7 for rounding reduced costs to 0 */
# endif
#endif

#ifndef RANDSCALE
# define RANDSCALE     100      /* Randomization scaling range */
#endif

#ifndef DEF_PERTURB
# define DEF_PERTURB   1e-5     /* Perturbation scalar for degenerative problems;
                                   must at least be RANDSCALE greater than DEF_EPSB */
#endif

#ifndef SOLUTIONEPS
# define SOLUTIONEPS   1e-5     /* Margin of error for solution bounds */
#endif

#ifndef DEF_EPSILON
# ifdef ORGPARAM
#  define DEF_EPSILON  1e-3     /* to determine if a float value is integer */
# else
#  define DEF_EPSILON  1e-6     /* to determine if a float value is integer */
# endif
#endif

#ifndef DEF_MIP_GAP
# define DEF_MIP_GAP   1e-9     /* The default MIP gap */
#endif

#ifndef DEF_LAGACCEPT
# define DEF_LAGACCEPT 1e-3     /* Default Lagrangian convergence acceptance criterion */
#endif

#ifndef MINSCALAR
# define MINSCALAR    1e-10     /* Smallest allowed scaling adjustment */
#endif

#ifndef MAXSCALAR
# define MAXSCALAR     1e+10    /* Largest allowed scaling adjustment */
#endif

#define MINTIMEPIVOT  5e-2      /* Minimum time per pivot for pivot optimization purposes */
#define SCALINGEPS    1e-2      /* Scaling convergence criterion */

#define	my_abs(x)       ((x) < 0 ? -(x) : (x))
#define my_min(x, y)    ((x) < (y) ? (x) : (y))
#define my_max(x, y)    ((x) > (y) ? (x) : (y))
#define my_if(t, x, y)  ((t) ? (x) : (y))
#define my_chsign(t, x) ( ((t) && (x) != 0) ? -(x) : (x))

#define MAX_WARN_COUNT 20

#ifdef CHECK
extern int Warn_count;
#define my_round(val, eps) { \
    REAL absv; \
        absv = ((val) < 0 ? -(val) : (val)); \
        if(absv < (eps)) \
          val = 0; \
    if(Warn_count < MAX_WARN_COUNT) \
      { \
	if(absv > 0.5 * (eps) && absv < 2 * (eps)) \
	  { \
	Warn_count++; \
	report(NULL, NORMAL, \
	    "Warning Value close to epsilon V: %e E: %e\n", \
	    (double)absv, (double)(eps)); \
	if(Warn_count == MAX_WARN_COUNT) \
	  { \
	    report(NULL, NORMAL, \
		"*** Surpressing further rounding warnings\n"); \
	  } \
	  } \
      } \
}

#else
#define my_round(val,eps) if (((val) < 0 ? -(val) : (val)) < (eps)) val = 0;
#endif

#define my_precision(val,eps) ((fabs(val)) < (eps) ? 0 : (val))

#ifndef REAL /* to allow -DREAL=<float type> while compiling */
#define REAL double
#endif

#ifndef LREAL
#define LREAL long double
#endif

#ifndef RREAL
#define RREAL long double
#endif

#ifndef MYBOOL
/*#define MYBOOL  unsigned short */
#define MYBOOL     unsigned char
#endif

#ifndef STATUS
#define STATUS   short
#endif

#define NAMELEN 25
#define MAXSTRL (NAMELEN-1)
#define STD_ROW_NAME_PREFIX "r_"

#define MALLOC(ptr, nr, type)\
  ((((nr) == 0) || ((ptr = (type *) malloc((size_t)((nr) * sizeof(*ptr)))) == NULL)) ? \
   report(NULL, CRITICAL, "malloc of %d bytes failed on line %d of file %s",\
           (nr) * sizeof(*ptr), __LINE__, __FILE__), (ptr = NULL /* (void *) 0 */) : \
   ptr\
  )

#define CALLOC(ptr, nr, type)\
  ((((nr) == 0) || ((ptr = (type *) calloc((size_t)(nr), sizeof(*ptr))) == NULL)) ? \
   report(NULL, CRITICAL, "calloc of %d bytes failed on line %d of file %s",\
           (nr) * sizeof(*ptr), __LINE__, __FILE__), (ptr = NULL /* (void *) 0 */) : \
   ptr\
  )

#define REALLOC(ptr, nr, type)\
  ((((nr) == 0) || ((ptr = (type *) realloc(ptr, (size_t)((nr) * sizeof(*ptr)))) == NULL)) ? \
   report(NULL, CRITICAL, "realloc of %d bytes failed on line %d of file %s",\
           (nr) * sizeof(*ptr), __LINE__, __FILE__), (ptr = NULL /* (void *) 0 */) : \
   ptr\
  )

#define FREE(ptr) if (ptr != NULL) {free(ptr), ptr = NULL;} else

#define MALLOCCPY(nptr, optr, nr, type)\
  (MALLOC(nptr, nr, type), (nptr != NULL) ? memcpy(nptr, optr, (size_t)((nr) * sizeof(*optr))) : 0, nptr)

#define MEMCPY(nptr, optr, nr)\
  memcpy(nptr, optr, (size_t)((nr) * sizeof(*optr)));

typedef char nstring[NAMELEN];

typedef struct _matrec
{
  int row_nr;
  REAL value;
} matrec;

/* SOS storage structure */

#define LINEARSEARCH 0

typedef struct _SOSrec
{
  int   tagorder;
  char  *name;
  short type;
  int   size;
  int   priority;
  int   *members;
  REAL  *weights;
  int   *membersSorted;
  int   *membersMapped;
} SOSrec;

/* Prototypes for call-back functions*/
typedef int WINAPI abortfunc(void *lp, void *userhandle);
typedef void WINAPI logfunc(void *lp, void *userhandle, char *buf);
typedef void * WINAPI msgfunc(void *lp, void *userhandle, int message);

/* fields indicated with ## may be modified directly. Note however this is strongly discouraged.
   Use the set_* function to set a value and get_* to retrieve a value. This ways you are less version dependend. */
/* pointers will have their array size in the comments */

typedef struct _lprec
{
  char      *lp_name;   	/* the name of the lp */
  short     verbose;            /* ## Set amount of run-time messages and results */
  MYBOOL    print_sol;          /* ## Set TRUE to print optimal solution */
  MYBOOL    debug;              /* ## Set TRUE to print extra debug information */
  short     print_at_invert;    /* ## Print information at every reinversion */
  MYBOOL    trace;              /* ## Print information on simplex progression */
  MYBOOL    anti_degen;	    	/* ## Set TRUE to do perturbations to avoid cycling */
  MYBOOL    do_presolve;        /* ## Set TRUE to perform matrix presolving */

  int	    rows;               /* Nr of constraint rows in the problem */
  int       rows_alloc;      	/* The allocated memory for Rows sized data */
  int       columns;            /* The number of columns (= variables) */
  int       columns_alloc;
  int       columns_used;
  int       sum;                /* The size of the variables + the slacks */
  int       sum_alloc;

  MYBOOL    names_used;         /* Flag to indicate if names for rows and columns are used */

  hashelem  **row_name;		/* rows_alloc+1 */
  hashelem  **col_name;		/* columns_alloc+1 */

 /* Row[0] of the sparce matrix is the objective function */

  int       non_zeros;          /* The number of elements in the sparse matrix*/
  int       mat_alloc;		/* The allocated size for matrix sized structures */
  matrec    *mat;               /* mat_alloc :The sparse matrix */
  int       *col_end;           /* columns_alloc+1 :Cend[i] is the index of the
                                   first element after column i.
                		   column[i] is stored in elements
		                   col_end[i-1] to col_end[i]-1 */
  int       *col_no;            /* mat_alloc :From Row 1 on, col_no contains the
                		   column nr. of the nonzero elements, row by row */
  MYBOOL    row_end_valid;	/* true if row_end & col_no are valid */
  int       *row_end;           /* rows_alloc+1 :row_end[i] is the index of the
                		   first element in Colno after row i */
  REAL      *orig_rh;           /* rows_alloc+1 :The RHS after scaling & sign
		                   changing, but before 'Bound transformation' */
  REAL      *rh;	        /* rows_alloc+1 :As orig_rh, but after Bound transformation */
  RREAL     *rhs;	        /* rows_alloc+1 :The RHS of the current simplex tableau */
  MYBOOL    *must_be_int;       /* sum_alloc+1 :TRUE if variable must be Integer */
  REAL      *orig_upbo;         /* sum_alloc+1 :Bound before transformations */
  REAL      *orig_lowbo;        /*  "       "                   */
  REAL      *upbo;              /*  " " :Upper bound after transformation & B&B work */
  REAL      *lowbo;             /*  "       "  :Lower bound after transformation & B&B work */

  MYBOOL    basis_valid;        /* TRUE is the basis is still valid */
  int       *bas;               /* rows_alloc+1 :The basis column list */
  MYBOOL    *basis;             /* sum_alloc+1 : basis[i] is TRUE if the column is in the basis */
  MYBOOL    *lower;             /*  "       "  :TRUE if the variable is at its
		                   lower bound (or in the basis), it is FALSE
                		   if the variable is at its upper bound */

  MYBOOL    eta_valid;          /* TRUE if current Eta structures are valid */
  int       eta_alloc;          /* The allocated memory for Eta non-zero values */
  int       eta_size;           /* The number of Eta columns */
  int       num_inv;            /* Number of pivots since last refactorization */
  int       max_num_inv;        /* ## Number of pivots between reinversions of the ETA matrix */
  REAL      *eta_value;         /* eta_alloc : Structure containing values of Eta */
  int       *eta_row_nr;        /*  "     "  : Structure containing row indexes of Eta */
  int       *eta_col_end;       /* rows_alloc + MaxNumInv : eta_col_end[i] is
                		   the start index of the next Eta column */

  MYBOOL    bb_rule;            /* ## Set rule for selecting B&B variables:
                                      FIRST_SELECT : Lowest indexed non-integer column
                                      RAND_SELECT  : Random non-integer column
                                      WORST_SELECT : Largest deviation from an integer value
                                      MEDIAN_SELECT: Median value deviation from an integer value */

  REAL      obj_bound;          /* ## Set initial "at least better than" guess for objective function
                                   (can in particular speed up B&B iterations) */
  int       iter;               /* Number of iterations in the simplex solver (LP) */
  int       total_iter;         /* Number of iterations (B&B) (Integer LP) */
  int       max_level;          /* The Deepest B&B level of the last solution */
  int	    total_nodes;        /* Total number of nodes processed in B&B */
  REAL      *solution;          /* sum_alloc+1 : Solution array of the next to optimal LP,
                                   Index   0           : Objective function value,
                                   Indeces 1..rows     : Slack variable values,
                		   Indeced rows+1..sum : Variable values */
  REAL      *best_solution;     /* sum_alloc+1 : Solution array of optimal 'Integer' LP,
                                   structured as the solution array above */
  REAL      real_solution;      /* Optimal non-MIP solution base */
  REAL      limMIP_solution;
  REAL      *duals;             /* sum_alloc+1 :The dual variables/reduced costs of the last LP */

  MYBOOL    maximise;           /* TRUE if the goal is to maximise the objective function */
  MYBOOL    floor_first;        /* ## Set BRANCH_FLOOR for B&B to set variables to floor bound first;
                                   conversely with BRANCH_CEILING, the ceiling value is set first */
  MYBOOL    *bb_varbranch;      /*    Determines branching strategy at the individual variable level;
                                      the setting here overrides the floor_first setting */
  MYBOOL    *ch_sign;           /* rows_alloc+1 :TRUE if the Row in the matrix has changed sign
                                   (a`x > b, x>=0) is translated to
	          		    s + -a`x = -b with x>=0, s>=0) */

  MYBOOL    scaling_used;       /* TRUE if scaling is used */
  MYBOOL    columns_scaled;     /* TRUE if the columns are scaled too, Only use
                	 	   if all variables are non-integer */
  REAL      *scale;             /* sum_alloc+1:0..Rows the scaling of the Rows,
                		   Rows+1..Sum the scaling of the columns */

  int	    nr_lagrange;        /* Nr. of Langrangian relaxation constraints */
  REAL	    **lag_row;	        /* NumLagrange, columns+1:Pointer to pointer of rows */
  REAL      *lag_rhs;	        /* NumLagrange :Pointer to pointer of Rhs */
  REAL      *lambda;	        /* NumLagrange :Lambda Values */
  MYBOOL    *lag_con_type;      /* NumLagrange :TRUE if constraint type EQ */
  REAL      lag_bound;	        /* The Lagrangian lower bound */

  MYBOOL    valid;	        /* Has this lp pased the 'test' */
  REAL      infinite;           /* ## limit for dynamic range */
  REAL      epsilon;            /* ## to determine if a float value is integer */
  REAL      epsb;               /* ## for rounding RHS values to 0/infeasibility */
  REAL      epsd;               /* ## for rounding reduced costs to zero */
  REAL      epsel;              /* ## for rounding other values (vectors) to 0 */
  hashstruct *rowname_hashtab;  /* hash table to store row names */
  hashstruct *colname_hashtab;  /* hash table to store column names */

  REAL      *dualsfrom;         /* sum_alloc+1 :The sensitivity on dual variables/reduced costs of the
                		   last LP */
  REAL      *dualstill;         /* sum_alloc+1 :The sensitivity on dual variables/reduced costs of the
                		   last LP */
  REAL      *objfrom;           /* columns_alloc+1 :The sensitivity on object function of the
		                   last LP */
  REAL      *objtill;           /* columns_alloc+1 :The sensitivity on object function of the
                                   last LP */
  REAL      *objfromvalue;

  int       orig_rows;          /* Number of actual problem rows before deletions */
  int       orig_columns;       /* Number of actual problem columns before working columns */
  short     spx_status;         /* Simplex solver feasibility/mode code */
  short     lag_status;         /* Extra status variable for lag_solve */
  int       solutioncount;      /* number of equal-valued solutions found (up to solutionlimit) */
  int       solutionlimit;      /* upper number of equal-valued solutions kept track of */
  int       num_refact;         /* Number of times the basis was refactored */
  MYBOOL    scalemode;          /* ## Set 0 for max-min, 1 for geometric */
  MYBOOL    improve;            /* ## Set to non-zero for iterative improvement */
  MYBOOL    lag_trace;          /* ## Print information on Lagrange progression */
  MYBOOL    piv_rule;           /* ## Set rule for selecting row and column entering/leaving */
  MYBOOL    break_at_first;     /* ## Set TRUE to stop at first feasible solution */
  REAL      break_at_value;     /* ## Set value for the objective function deemed
                                      to be sufficiently good in an integer problem */
  int       int_count;          /* Number of integers required */
  int       *var_is_free;       /* columns+1: Index of twin variable if variable is free */

  REAL      Extrad;
  MYBOOL    doIterate;          /* Perform iteration at next opportunity */
  MYBOOL    doInvert;           /* Force basis reinversion immediateluy */
  MYBOOL    justInverted;       /* The basis was recently reinverted */
  MYBOOL    wasprocessed;       /* The solve preprocessing was performed */

  MYBOOL    Break_bb;           /* Solver working variable */
  int       Level;              /* Solver B&B working variable (recursion depth) */

  REAL      lag_accept;         /* The Lagrangial convergence acceptance criterion */

  REAL      negrange;           /* ## limit for negative variable range */

  short     splitnegvars;       /* ## split negative variables in a positive and negative component
				   Can be TRUE or AUTOMATIC
				    TRUE is always split negative variables
				    AUTOMATIC is only split when negative lower bound and positive upper bound */

  REAL      epsperturb;         /* ## perturbation scalar */

  REAL	    epspivot;           /* ## Pivot reject tolerance (try others first) */

  REAL      mip_gap;            /* ## MIP gap */

  /* Time/timer variables */
  long      sectimeout;
  double    timestart;
  double    timeend;
  double    time_refactstart;   /* Time since start of last refactorization-pivots cyle */

  /* Message processing callbacks */
  abortfunc *abort;
  void      *aborthandle;
  logfunc   *writelog;
  void      *loghandle;
  logfunc   *debuginfo;
  msgfunc   *usermessage;
  int       msgmask;
  void      *msghandle;
  int       *var_to_orig;       /* sum_alloc+1 : Mapping of variables from solution to
				   best_solution to account for removed variables and
				   rows during presolve; a non-positive value indicates
                                   that the constraint or variable was removed */
  int       *orig_to_var;
  int       *var_priority;      /* columns: Priority-mapping of variables */
  int       sc_count;           /* Number of semi-continuous variables */
  int       sos_alloc;          /* Size allocated to specially ordered sets (SOS1, SOS2...) */
  int       sos_count;          /* Number of specially ordered sets (SOS1, SOS2...) */
  int       sos_ints;           /* Number of integers in SOS'es above */
  REAL      *var_is_sc;         /* sum_columns+1 : TRUE if variable is semi-continuous;
                                   value replaced by conventional lower bound during solve */
  SOSrec    **sos_list;         /* Array of pointers to SOS lists */
  int       sos_vars;           /* Number of variables in the sos_nodes list */
  int       *sos_priority;      /* Priority-sorted list of variables (no duplicates) */
  MYBOOL    tighten_on_set;     /* Specify if bounds will be tightened or overriden at bound setting */
} lprec;



/* function interface for the user */

void _WINAPI lp_solve_version(int *majorversion,
                              int *minorversion,
                              int *release,
                              int *build);
void _WINAPI set_magic(int code, int param);

lprec * _WINAPI make_lp(int rows, int columns);
/* create and initialise a lprec structure
   defaults:
   Empty (Rows * Columns) matrix,
   Minimize the objective function
   constraints all type <=
   Upperbounds all Infinite
   no integer variables
   floor first in B&B
   no scaling
   default basis */

lprec * _WINAPI read_LP(char *input, short verbose, char *lp_name);
#if !defined LPSOLVEDLL
lprec * _WINAPI read_lp(FILE *input, short verbose, char *lp_name);
#endif
/* create and read an .lp file from input (input must be open) */

#define read_lp_file read_lp /* old name */

#if defined CPLEX
lprec * _WINAPI read_CPLEX(char *input, short verbose, char *lp_name);
#if !defined LPSOLVEDLL
lprec * _WINAPI read_cplex(FILE *input, short verbose, char *lp_name);
#endif
#endif

void _WINAPI delete_lp(lprec *lp);
/* Remove problem from memory */

lprec * _WINAPI copy_lp(lprec *lp);
/* copy a lp structure */

int _WINAPI set_mat(lprec *lp, int row, int column, REAL value);
/* fill in element (Row,Column) of the matrix
   Row in [0..Rows] and Column in [1..Columns] */

int _WINAPI set_obj_fn(lprec *lp, REAL *row);
/* set the objective function (Row 0) of the matrix */
int _WINAPI str_set_obj_fn(lprec *lp, char *row);
/* The same, but with string input */

int _WINAPI add_constraint(lprec *lp, REAL *row, short constr_type, REAL rh);
/* Add a constraint to the problem,
   row is the constraint row,
   rh is the right hand side,
   constr_type is the type of constraint (LE (<=), GE(>=), EQ(=)) */
int _WINAPI str_add_constraint(lprec *lp, char *row_string ,short constr_type, REAL rh);
/* The same, but with string input */

int _WINAPI del_constraint(lprec *lp,int del_row);
/* Remove constraint nr del_row from the problem */

int _WINAPI add_lag_con(lprec *lp, REAL *row, short con_type, REAL rhs);
/* add a Lagrangian constraint of form Row' x contype Rhs */
int _WINAPI str_add_lag_con(lprec *lp, char *row, short con_type, REAL rhs);
/* The same, but with string input */

int _WINAPI add_column(lprec *lp, REAL *column);
/* Add a Column to the problem */
int _WINAPI str_add_column(lprec *lp, char *col_string);
/* The same, but with string input */

int _WINAPI del_column(lprec *lp, int column);
/* Delete a column */

void set_bounds_tighter(lprec *lp, MYBOOL tighten);
MYBOOL get_bounds_tighter(lprec *lp);
/* allow to set more relaxed bounds via set_upbo, set_lowbo */

int _WINAPI set_upbo(lprec *lp, int column, REAL value);
/* Set the upperbound of a variable */

int _WINAPI set_lowbo(lprec *lp, int column, REAL value);
/* Set the lowerbound of a variable */

int _WINAPI set_bounds(lprec *lp, int column, REAL lower, REAL upper);

int _WINAPI get_bounds(lprec *lp, int column, REAL *lower, REAL *upper);

int _WINAPI set_uprange(lprec *lp, int row, REAL value);
/* Set the upper range of a constraint */

int _WINAPI set_lowrange(lprec *lp, int row, REAL value);
/* Set the lower range of a constraint */

int _WINAPI set_int(lprec *lp, int column, short must_be_int);
/* Set the type of variable, if must_be_int = TRUE then the variable must be integer */

int _WINAPI is_int(lprec *lp, int column);

int _WINAPI set_varweights(lprec *lp, REAL *weights);

int _WINAPI set_semicont(lprec *lp, int column, short must_be_sc);

int _WINAPI is_semicont(lprec *lp, int column);

void _WINAPI set_verbose(lprec *lp, short verbose);
short _WINAPI get_verbose(lprec *lp);

void _WINAPI set_timeout(lprec *lp, long sectimeout);
long _WINAPI get_timeout(lprec *lp);

void _WINAPI set_print_sol(lprec *lp, short print_sol);
short _WINAPI is_print_sol(lprec *lp);

void _WINAPI set_debug(lprec *lp, short debug);
short _WINAPI is_debug(lprec *lp);

void _WINAPI set_print_at_invert(lprec *lp, short print_at_invert);
short _WINAPI is_print_at_invert(lprec *lp);

void _WINAPI set_trace(lprec *lp, short trace);
short _WINAPI is_trace(lprec *lp);

void _WINAPI set_anti_degen(lprec *lp, short anti_degen);
short _WINAPI is_anti_degen(lprec *lp);

void _WINAPI set_do_presolve(lprec *lp, short do_presolve);
short _WINAPI is_do_presolve(lprec *lp);

void _WINAPI set_max_num_inv(lprec *lp, int max_num_inv);
int _WINAPI get_max_num_inv(lprec *lp);

void _WINAPI set_bb_rule(lprec *lp, short bb_rule);
short _WINAPI get_bb_rule(lprec *lp);

void _WINAPI set_obj_bound(lprec *lp, REAL obj_bound);
REAL _WINAPI get_obj_bound(lprec *lp);

void _WINAPI set_mip_gap(lprec *lp, REAL mip_gap);
REAL _WINAPI get_mip_gap(lprec *lp);

void _WINAPI set_floor_first(lprec *lp, short floor_first);
short _WINAPI get_floor_first(lprec *lp);

int _WINAPI set_var_branch(lprec *lp, int column, short branch_mode);
short _WINAPI get_var_branch(lprec *lp, int column);

void _WINAPI set_infinite(lprec *lp, REAL infinite);
REAL _WINAPI get_infinite(lprec *lp);

void _WINAPI set_epsilon(lprec *lp, REAL epsilon);
REAL _WINAPI get_epsilon(lprec *lp);

void _WINAPI set_epsb(lprec *lp, REAL epsb);
REAL _WINAPI get_epsb(lprec *lp);

void _WINAPI set_epsd(lprec *lp, REAL epd);
REAL _WINAPI get_epsd(lprec *lp);

void _WINAPI set_epsel(lprec *lp, REAL epsel);
REAL _WINAPI get_epsel(lprec *lp);

void _WINAPI set_scalemode(lprec *lp, short scalemode);
short _WINAPI get_scalemode(lprec *lp);

void _WINAPI set_improve(lprec *lp, short improve);
short _WINAPI get_improve(lprec *lp);

void _WINAPI set_lag_trace(lprec *lp, short lag_trace);
short _WINAPI is_lag_trace(lprec *lp);

void _WINAPI set_piv_rule(lprec *lp, short piv_rule);
short _WINAPI get_piv_rule(lprec *lp);

void _WINAPI set_break_at_first(lprec *lp, short break_at_first);
short _WINAPI is_break_at_first(lprec *lp);

void _WINAPI set_break_at_value(lprec *lp, REAL break_at_value);
REAL _WINAPI get_break_at_value(lprec *lp);

void _WINAPI set_negrange(lprec *lp, REAL negrange);
REAL _WINAPI get_negrange(lprec *lp);

void _WINAPI set_splitnegvars(lprec *lp, short splitnegvars);
short _WINAPI get_splitnegvars(lprec *lp);

void _WINAPI set_epsperturb(lprec *lp, REAL epsperturb);
REAL _WINAPI get_epsperturb(lprec *lp);

void _WINAPI set_epspivot(lprec *lp, REAL epspivot);
REAL _WINAPI get_epspivot(lprec *lp);

int _WINAPI get_max_level(lprec *lp);

int _WINAPI get_total_nodes(lprec *lp);

int _WINAPI get_total_iter(lprec *lp);

REAL _WINAPI get_objective(lprec *lp);

int _WINAPI get_variables(lprec *lp, REAL *var);

int _WINAPI get_ptr_variables(lprec *lp, REAL **var);

int _WINAPI get_constraints(lprec *lp, REAL *constr);

int _WINAPI get_ptr_constraints(lprec *lp, REAL **constr);

int _WINAPI get_sensitivity_rhs(lprec *lp, REAL *duals, REAL *dualsfrom, REAL *dualstill);

int _WINAPI get_ptr_sensitivity_rhs(lprec *lp, REAL **duals, REAL **dualsfrom, REAL **dualstill);

int _WINAPI get_sensitivity_obj(lprec *lp, REAL *objfrom, REAL *objtill);
int _WINAPI get_ptr_sensitivity_obj(lprec *lp, REAL **objfrom, REAL **objtill);

int _WINAPI get_sensitivity_objex(lprec *lp, REAL *objfrom, REAL *objtill, REAL *objfromvalue, REAL *objtillvalue);
int _WINAPI get_ptr_sensitivity_objex(lprec *lp, REAL **objfrom, REAL **objtill, REAL **objfromvalue, REAL **objtillvalue);

int _WINAPI get_primal_solution(lprec *lp, REAL *pv);
int _WINAPI get_ptr_primal_solution(lprec *lp, REAL **pv);

int _WINAPI get_work_solution(lprec *lp, REAL *pv);
int _WINAPI get_ptr_work_solution(lprec *lp, REAL **pv);

void _WINAPI set_solutionlimit(lprec *lp, int limit);
int _WINAPI get_solutionlimit(lprec *lp);
int _WINAPI get_solutioncount(lprec *lp);

int _WINAPI get_orig_index(lprec *lp, int lp_index);
int _WINAPI get_lp_index(lprec *lp, int orig_index);

int _WINAPI get_Nrows(lprec *lp);

int _WINAPI get_Norig_rows(lprec *lp);

int _WINAPI get_Lrows(lprec *lp);

int _WINAPI get_Ncolumns(lprec *lp);

int _WINAPI get_Norig_columns(lprec *lp);

/* Add SOS constraints */
int _WINAPI add_SOS(lprec *lp, char *name, short sostype, int priority, int count, int *sosvars, REAL *weights);
int _WINAPI is_SOS_var(lprec *lp, int column);

int _WINAPI set_lp_name(lprec *lp, char *name);

char * _WINAPI get_lp_name(lprec *lp);

int _WINAPI set_rh(lprec *lp, int row, REAL value);
REAL _WINAPI get_rh(lprec *lp, int row);
/* Set the right hand side of a constraint row */

int _WINAPI set_rh_range(lprec *lp, int row, REAL deltavalue);
REAL _WINAPI get_rh_range(lprec *lp, int row);
/* Set the RHS range; i.e. the lower and upper bounds of a constraint row */

void _WINAPI set_rh_vec(lprec *lp, REAL *rh);
/* Set the right hand side vector */
int _WINAPI str_set_rh_vec(lprec *lp, char *rh_string);
/* The same, but with string input */

void _WINAPI set_maxim(lprec *lp);
/* maximise the objective function */

void _WINAPI set_minim(lprec *lp);
/* minimize the objective function */

int _WINAPI set_constr_type_ex(lprec *lp, int fromrow, int torow, short *con_type);
/* Set the type of constraint in row Row (LE, GE, EQ) */
int _WINAPI set_constr_type(lprec *lp, int row, short con_type);
/* Set the type of constraint in row Row (LE, GE, EQ) */

int _WINAPI get_constr_type(lprec *lp, int row);
/* get the type of constraint in row Row (LE, GE, EQ) */

int _WINAPI set_row_name(lprec *lp, int row, char *new_name);
/* Set the name of a constraint row, make sure that the name has < 25 characters */

char * _WINAPI get_row_name(lprec *lp, int row);

int _WINAPI set_col_name(lprec *lp, int column, char *new_name);
/* Set the name of a varaible column, make sure that the name has < 25 characters */

char * _WINAPI get_col_name(lprec *lp, int column);

REAL _WINAPI scale(lprec *lp, REAL *myrowscale, REAL *mycolscale);
/* Automatic scaling of the problem */

REAL _WINAPI auto_scale(lprec *lp);
/* Automatic scaling of the problem */

int _WINAPI scaleCR(lprec *lp);
/* Curtis-Reid scaling */

void _WINAPI unscale(lprec *lp);
/* Remove all scaling from the problem */

/* Set/Get basis for a re-solved system */  /* Added by KE */
void _WINAPI set_basis(lprec *lp, int *bascolumn);
void _WINAPI get_basis(lprec *lp, int *bascolumn);

int _WINAPI solve(lprec *lp);
/* Solve the problem */

int _WINAPI lag_solve(lprec *lp, REAL start_bound, int num_iter, short verbose);
/* Do NumIter iterations with Lagrangian relaxation constraints */

void _WINAPI reset_basis(lprec *lp);
/* Reset the basis of a problem, can be usefull in case of degeneracy - JD */

REAL _WINAPI mat_elm(lprec *lp, int row, int column);
/* get a single element from the matrix */

int _WINAPI get_row(lprec *lp, int row_nr, REAL *row);
int _WINAPI get_matrecrow(lprec *lp, int row_nr, matrec *matrecrow);
/* fill row with the row row_nr from the problem */

int _WINAPI get_column(lprec *lp, int col_nr, REAL *column);
/* fill column with the column col_nr from the problem */

int _WINAPI get_reduced_costs(lprec *lp, REAL *rc);
int _WINAPI get_ptr_reduced_costs(lprec *lp, REAL **rc);
/* get the reduced costs vector */

int _WINAPI get_lambda(lprec *lp, REAL *lambda);
int _WINAPI get_ptr_lambda(lprec *lp, REAL **lambda);
/* Get the primal, dual/reduced costs and Lamdba vectors */

int _WINAPI is_feasible(lprec *lp, REAL *values);
/* returns TRUE if the vector in values is a feasible solution to the lp */

int _WINAPI column_in_lp(lprec *lp, REAL *column);
/* returns TRUE if column is already present in lp. (Does not look at bounds
   and types, only looks at matrix values */

int _WINAPI get_varpriority(lprec *lp, int column);

lprec * _WINAPI read_MPS(char *input, short verbose);
#if !defined LPSOLVEDLL
lprec * _WINAPI read_mps(FILE *input, short verbose);
#endif
/* read a MPS file */

int _WINAPI write_mps(lprec *lp, char *output);
#if !defined LPSOLVEDLL
int _WINAPI write_MPS(lprec *lp, FILE *output);
#endif
/* write a MPS file to output */

int _WINAPI write_lp(lprec *lp, char *output);
#if !defined LPSOLVEDLL
int _WINAPI write_LP(lprec *lp, FILE *output);
#endif
/* write a LP file to output */

void _WINAPI print_lp(lprec *lp);
/* Print the current problem, only usefull in very small (test) problems.
  Shows the effect of scaling */

void _WINAPI print_objective(lprec *lp);
/* Print the value of the objective */

void _WINAPI print_solution(lprec *lp);
/* Print the values of the variables */

void _WINAPI print_constraints(lprec *lp);
/* Print the values of the constraints */

void _WINAPI print_duals(lprec *lp);
/* Print the dual variables of the solution */

void _WINAPI print_scales(lprec *lp);
/* If scaling is used, print the scaling factors */

int _WINAPI print_file(char *filename);
/* file where results are printed to. Default stderr. If NULL then back stderr */

/* Allow the user to define an interruption callback function */
void _WINAPI put_abortfunc(lprec *lp, abortfunc newabort, void *aborthandle);

/* Allow the user to define a logging function */
void _WINAPI put_logfunc(lprec *lp, logfunc newlog, void *loghandle);

/* Allow the user to define an event-driven message/reporting */
void _WINAPI put_msgfunc(lprec *lp, msgfunc newmsg, void *msghandle, int mask);


/* functions used internaly by the lp toolkit */
lprec *make_lpext(int rows, int columns, int non_zeros, int mat_alloc, char *lp_name);
int del_columnex(lprec *lp, int column, MYBOOL forpresolve);
int del_constraintex(lprec *lp,int del_row, MYBOOL forpresolve);
void *report(lprec *lp, short level, char *format, ...);
void unscale_columns(lprec *lp);
void btran(lprec *lp, REAL *row, REAL roundzero);
int presolve(lprec *lp);
MYBOOL isvalid(lprec *lp);
REAL get_mat(lprec *lp, int row, int column);
REAL get_upbo(lprec *lp, int column);
REAL get_lowbo(lprec *lp, int column);
int preprocess(lprec *lp);
void postprocess(lprec *lp);
int set_matrix(lprec *lp, int Row, int Column, REAL Value, MYBOOL doscale);
int SOS_infeasible(lprec *lp, int sosindex);
int SOS_get_type(lprec *lp, int sosindex);
int SOS_is_member(lprec *lp, int sosindex, int column);
int SOS_fix_unmarked(lprec *lp, int variable, int sosindex, REAL *bound, REAL value, MYBOOL isupper, int *diffcount);
int append_SOSrec(lprec *lp, SOSrec *SOS, int size, int *variables, REAL *weights);
MYBOOL SOS_set_marked(lprec *lp, int sosindex, int column, MYBOOL islive);
MYBOOL SOS_is_full(lprec *lp, int sosindex, int column, MYBOOL activeonly);
REAL get_mat_raw(lprec *lp, int row, int column);
MYBOOL SOS_unmark(lprec *lp, int sosindex, int column);
MYBOOL SOS_is_member_of_type(lprec *lp, int column, short sostype);
int SOS_is_satisfied(lprec *lp, int sosindex, REAL *solution);
MYBOOL SOS_can_mark(lprec *lp, int sosindex, int column);
int add_columnex(lprec *lp, REAL *column, int del_splitv, matrec *matreccolum, int Nrows);
int is_splitvar(lprec *lp, int column);
void setpivrow(lprec *lp, int   row_nr, REAL *prow);
int build_row_end(lprec *lp);

#ifdef __cplusplus
};
#endif

#endif
