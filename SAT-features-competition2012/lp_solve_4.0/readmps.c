#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lpkit.h"

/* defines */
#define VARNAME    0
#define CONSTRNAME 1

/* MPS defines */
#define MPSUNDEF       -2
#define MPSNAME        -1
#define MPSROWS         0
#define MPSCOLUMNS      1
#define MPSRHS          2
#define MPSBOUNDS       3
#define MPSRANGES       4
#define MPSSOS          5

/* global vars */
short  Column_ready;
short  Int_section;
lprec *Mlp;
char   Last_col_name[25];
int    Lineno;
short  Unconstrained_rows_found;

/*
A:  MPS format was named after an early IBM LP product and has emerged
as a de facto standard ASCII medium among most of the commercial LP
codes.  Essentially all commercial LP codes accept this format, but if
you are using public domain software and have MPS files, you may need
to write your own reader routine for this.  It's not too hard.  The
main things to know about MPS format are that it is column oriented (as
opposed to entering the model as equations), and everything (variables,
rows, etc.) gets a name.  MPS format is described in more detail in
Murtagh's book, referenced in another section. Also,

ftp://softlib.cs.rice.edu/pub/miplib/mps_format

is a nice short introduction.

MPS is an old format, so it is set up as though you were using punch
cards, and is not free format. Fields start in column 1, 5, 15, 25, 40
and 50.  Sections of an MPS file are marked by so-called header cards,
which are distinguished by their starting in column 1.  Although it is
typical to use upper-case throughout the file (like I said, MPS has
long historical roots), many MPS-readers will accept mixed-case for
anything except the header cards, and some allow mixed-case anywhere.
The names that you choose for the individual entities (constraints or
variables) are not important to the solver; you should pick names that
are meaningful to you, or will be easy for a post-processing code to
read.

Here is a little sample model written in MPS format (explained in more
detail below):

NAME          TESTPROB
ROWS
 N  COST
 L  LIM1
 G  LIM2
 E  MYEQN
COLUMNS
    XONE      COST                 1   LIM1                 1
    XONE      LIM2                 1
    YTWO      COST                 4   LIM1                 1
    YTWO      MYEQN               -1
    ZTHREE    COST                 9   LIM2                 1
    ZTHREE    MYEQN                1
RHS
    RHS1      LIM1                 5   LIM2                10
    RHS1      MYEQN                7
BOUNDS
 UP BND1      XONE                 4
 LO BND1      YTWO                -1
 UP BND1      YTWO                 1
ENDATA

means:

Optimize
 COST:    XONE + 4 YTWO + 9 ZTHREE
Subject To
 LIM1:    XONE + YTWO <= 5
 LIM2:    XONE + ZTHREE >= 10
 MYEQN:   - YTWO + ZTHREE  = 7
Bounds
 0 <= XONE <= 4
-1 <= YTWO <= 1
End

*/

/* copy a MPS name, only trailing spaces are removed. In MPS, names can have
   embedded spaces! */
static void namecpy(char *into, char *from)
{
  int i;

  /* copy at most 8 characters of from, stop at end of string or newline */
  for(i = 0; (from[i] != '\0') && (from[i] != '\n') && (from[i] != '\r') && (i < 8); i++)
    into[i] = from[i];

  /* end with end of string */
  into[i] = '\0';

  /* remove trailing spaces, if any */
  for(i--; (i >= 0) && (into[i] == ' '); i--)
    into[i] = '\0';
}

/* scan an MPS line, and pick up the information in the fields that are
   present */
static int scan_line(char* line, char *field1, char *field2, char *field3,
		     double *field4, char *field5, double *field6)
{
  int items = 0, line_len;
  char buf[16], *ptr1, *ptr2;

  line_len = (int) strlen(line);
  while ((line_len) && ((line[line_len-1] == '\n') || (line[line_len-1] == '\r') || (line[line_len-1] == ' ')))
   line_len--;

  if(line_len >= 1) { /* spaces or N/L/G/E or UP/LO */
    strncpy(buf, line, 4);
    buf[4] = '\0';
    sscanf(buf, "%s", field1);
    items++;
  }
  else
    field1[0] = '\0';

  line += 4;

  if(line_len >= 5) { /* name */
    namecpy(field2, line);
    items++;
  }
  else
    field2[0] = '\0';

  line += 10;

  if(line_len >= 14) { /* name */
    namecpy(field3, line);
    items++;
  }
  else
    field3[0] = '\0';

  line += 10;

  if(line_len >= 25) { /* number */
    strncpy(buf, line, 15);
    buf[15] = '\0';
    for(ptr1 = ptr2 = buf; ; ptr1++)
      if(!isspace((unsigned char) *ptr1))
        if((*(ptr2++) = *ptr1) == 0)
	  break;
    /* *field4 = atof(buf); */
    *field4 = strtod(buf, &ptr1);
    if(*ptr1)
      return(-1);
    items++;
  }
  else
    *field4 = 0;

  line += 15;

  if(line_len >= 40) { /* name */
    namecpy(field5, line);
    items++;
  }
  else
    field5[0] = '\0';
  line += 10;

  if(line_len >= 50) { /* number */
    strncpy(buf, line, 15);
    buf[15] = '\0';
    for(ptr1 = ptr2 = buf; ; ptr1++)
      if(!isspace((unsigned char) *ptr1))
        if((*(ptr2++) = *ptr1) == 0)
	  break;
    /* *field6 = atof(buf); */
    *field6 = strtod(buf, &ptr1);
    if(*ptr1)
      return(-1);
    items++;
  }
  else
    *field6 = 0;

  return(items);
}

static int find_row(lprec *lp, char *name, MYBOOL Unconstrained_rows_found)
{
  hashelem *hp;

  hp = findhash(name, lp->rowname_hashtab);

  if (hp == NULL) {
      if(Unconstrained_rows_found) { /* just ignore them in this case */
	return(-1);
      }
      else {
	report(lp, SEVERE, "Unknown row name (%s) on line %d", name, Lineno);
	return(-1);
      }
  }
  return(hp->index);
}

static int find_var(lprec *lp, char *name, MYBOOL verbose)
{
  hashelem *hp;

  hp = findhash(name, lp->colname_hashtab);

  if (hp == NULL) {
    if(verbose)
      report(lp, SEVERE, "Unknown variable name (%s) on line %d", name, Lineno);
    return(-1);
  }
  return(hp->index);
}

static int sortLast_rownrs(CONST void *elem1,CONST void *elem2)
{
  matrec *ELEM1 = (matrec *)elem1, *ELEM2 = (matrec *)elem2;

  return(ELEM1->row_nr - ELEM2->row_nr);
}

static int addmpscolumn(lprec *lp, MYBOOL Int_section, MYBOOL *Column_ready,
  matrec *Last_column, int *Nrows, char *Last_col_name)
{
  int ok = TRUE;

  if (*Column_ready) {
    qsort(Last_column, *Nrows, sizeof(*Last_column), sortLast_rownrs);
    if((!add_columnex(lp, NULL, FALSE, Last_column, *Nrows)) || (!set_col_name(lp, lp->columns, Last_col_name)))
      ok = FALSE;
    else
      set_int(lp, lp->columns, Int_section);
  }
  *Column_ready = FALSE;
/*
  for(i = 0; i <= lp->rows; i++)
    Last_column[i] = 0;
*/
  *Nrows = 0;
  return(ok);
}

lprec *read_MPS(char *input, short verbose)
{
  lprec *lp = NULL;
  FILE *fpin;

  fpin = fopen(input, "r");
  if(fpin != NULL) {
    lp = read_mps(fpin, verbose);
    fclose(fpin);
  }
  return(lp);
}

lprec *read_mps(FILE *input, short verbose)
{
  char field1[5], field2[10], field3[10], field5[10], line[BUFSIZ], tmp[15], *ptr;
  double field4, field6;
  int section = MPSUNDEF, variant = 0;
  int items;
  int row;
  MYBOOL Int_section;
  MYBOOL Column_ready, Column_ready1;
  matrec *Last_column = NULL;
  int  Nrows = 0;
  char Last_col_name[NAMELEN];
  int var, SOS = 0;
  char probname[12];
  lprec *Mlp;
  MYBOOL Unconstrained_rows_found = FALSE;
  MYBOOL OF_found = FALSE;

  Mlp = make_lp(0, 0);
  if (Mlp != NULL) {
    Mlp->verbose = verbose;
    strcpy(Last_col_name, "");
    Int_section = FALSE;
    Column_ready = FALSE;
    Lineno = 0;

    /* let's initialize line to all zero's */
    memset(line, '\0', BUFSIZ);

    while(fgets(line, BUFSIZ - 1, input)) {
      Lineno++;

      for(ptr = line; (*ptr) && (isspace((unsigned char) *ptr)); ptr++);

      /* skip lines which start with "*", they are comment */
      if((line[0] == '*') || (*ptr == 0) || (*ptr == '\n') || (*ptr == '\r')) {
        report(Mlp, FULL, "Comment on line %d: %s", Lineno, line);
	continue;
      }

      report(Mlp, FULL, "Line %d: %s", Lineno, line);

      /* first check for "special" lines: NAME, ROWS, BOUNDS .... */
      /* this must start in the first position of line */
      if(line[0] != ' ') {
	sscanf(line, "%s", tmp);
	if(strcmp(tmp, "NAME") == 0) {
	  section = MPSNAME;
	  sscanf(line, "NAME %s", probname);
	  if (!set_lp_name(Mlp, probname)) {
  	    FREE(Last_column);
            delete_lp(Mlp);
	    return(NULL);
	  }
	}
	else if(strcmp(tmp, "ROWS") == 0) {
	  section = MPSROWS;
	  report(Mlp, FULL, "Switching to ROWS section");
	}
	else if(strcmp(tmp, "COLUMNS") == 0) {
	  if (CALLOC(Last_column, Mlp->rows + 1, matrec) == NULL) {
            FREE(Last_column);
            delete_lp(Mlp);
	    return(NULL);
	  }
	  section = MPSCOLUMNS;
	  report(Mlp, FULL, "Switching to COLUMNS section");
	}
	else if(strcmp(tmp, "RHS") == 0) {
	  if (!addmpscolumn(Mlp, Int_section, &Column_ready, Last_column, &Nrows, Last_col_name)) {
            FREE(Last_column);
            delete_lp(Mlp);
	    return(NULL);
	  }
	  section = MPSRHS;
	  report(Mlp, FULL, "Switching to RHS section");
	}
	else if(strcmp(tmp, "BOUNDS") == 0) {
	  section = MPSBOUNDS;
	  report(Mlp, FULL, "Switching to BOUNDS section");
	}
	else if(strcmp(tmp, "RANGES") == 0) {
	  section = MPSRANGES;
	  report(Mlp, FULL, "Switching to RANGES section");
	}
        else if((strcmp(tmp, "SOS") == 0) || (strcmp(tmp, "SETS") == 0)) {
          section = MPSSOS;
	  if(strcmp(tmp, "SOS") == 0)
	    variant = 0;
	  else
	    variant = 1;
          report(Mlp, FULL, "Switching to %s section\n", tmp);
        }
	else if(strcmp(tmp, "ENDATA") == 0) {
	  report(Mlp, FULL, "Finished reading MPS file");
	}
	else { /* line does not start with space and does not match above */
          report(Mlp, IMPORTANT, "Unrecognized line %d: %s", Lineno, line);
          FREE(Last_column);
	  delete_lp(Mlp);
	  return(NULL);
	}
      }
      else { /* normal line, process */
	items = scan_line(line, field1, field2, field3, &field4, field5,
			  &field6);
	if(items < 0){
	  report(Mlp, IMPORTANT, "Syntax error on line %d: %s", Lineno, line);
          FREE(Last_column);
	  delete_lp(Mlp);
	  return(NULL);
	}

	switch(section) {

	case MPSNAME:
          report(Mlp, IMPORTANT, "Error, extra line under NAME line (line %d)", Lineno);
          FREE(Last_column);
	  delete_lp(Mlp);
	  return(NULL);
	  break;

        /* Process entries in the ROWS section */
	case MPSROWS:
	  /* field1: rel. operator; field2: name of constraint */

	  report(Mlp, FULL, "Rows line: %s %s", field1, field2);

	  if(strcmp(field1, "N") == 0) {
	    if(!OF_found) { /* take the first N row as OF, ignore others */
	      if (!set_row_name(Mlp, 0, field2)) {
                FREE(Last_column);
	        delete_lp(Mlp);
	        return(NULL);
	      }
	      OF_found = TRUE;
	    }
	    else if(!Unconstrained_rows_found) {
	      report(Mlp, IMPORTANT, "Unconstrained row %s on line %d ignored", field2, Lineno);
	      report(Mlp, IMPORTANT, "Further messages of this kind will be suppressed");
	      Unconstrained_rows_found = TRUE;
	    }
	  }
	  else if(strcmp(field1, "L") == 0) {
	    if ((!str_add_constraint(Mlp, "" ,LE ,0)) || (!set_row_name(Mlp, Mlp->rows, field2))) {
              FREE(Last_column);
	      delete_lp(Mlp);
	      return(NULL);
	    }
	  }
	  else if(strcmp(field1, "G") == 0) {
	    if ((!str_add_constraint(Mlp, "" ,GE ,0)) || (!set_row_name(Mlp, Mlp->rows, field2))) {
              FREE(Last_column);
	      delete_lp(Mlp);
	      return(NULL);
	    }
	  }
	  else if(strcmp(field1, "E") == 0) {
	    if ((!str_add_constraint(Mlp, "",EQ ,0)) || (!set_row_name(Mlp, Mlp->rows, field2))) {
              FREE(Last_column);
	      delete_lp(Mlp);
	      return(NULL);
	    }
	  }
	  else {
            report(Mlp, SEVERE, "Unknown relation code '%s' on line %d", field1, Lineno);
            FREE(Last_column);
	    delete_lp(Mlp);
	    return(NULL);
	  }
	  break;

        /* Process entries in the COLUMNS section */
	case MPSCOLUMNS:
	  /* field2: variable; field3: constraint; field4: coef */
	  /* optional: field5: constraint; field6: coef */

	  report(Mlp, FULL, "Columns line: %s %s %g %s %g",
		 field2, field3, field4, field5, field6);

	  if((items == 4) || (items == 6)) {
	    if(*field2) {
	      Column_ready1 = (MYBOOL) (strcmp(field2, Last_col_name) != 0);
	      if (Column_ready1)
	        if (find_var(Mlp, field2, FALSE) >= 0) {
                  report(Mlp, SEVERE, "Variable name (%s) on line %d is already used!", field2, Lineno);
                  FREE(Last_column);
		  delete_lp(Mlp);
		  return(NULL);
	        }

	      if (Column_ready1 && Column_ready) {
	        if (!addmpscolumn(Mlp, Int_section, &Column_ready, Last_column, &Nrows, Last_col_name)) {
                  FREE(Last_column);
		  delete_lp(Mlp);
		  return(NULL);
	        }
	      }
	      strcpy(Last_col_name, field2);
	      Column_ready = TRUE;
	    }
	    if((field4 != 0) && ((row = find_row(Mlp, field3, Unconstrained_rows_found)) >= 0)) {
	      Last_column[Nrows].value = (REAL)field4;
	      Last_column[Nrows++].row_nr = row;
	    }
	  }
	  if(items == 6) {
	    if((field6 != 0) && ((row = find_row(Mlp, field5, Unconstrained_rows_found)) >= 0)) {
	      Last_column[Nrows].value = (REAL)field6;
	      Last_column[Nrows++].row_nr = row;
	    }
	  }

	  if(items == 5) { /* there might be an INTEND or INTORG marker */
	    /* look for "    <name>  'MARKER'                 'INTORG'" */
	    /* or "    <name>  'MARKER'                 'INTEND'" */
	    if(strcmp(field3, "'MARKER'") ==0) {
	      if (!addmpscolumn(Mlp, Int_section, &Column_ready, Last_column, &Nrows, Last_col_name)) {
                FREE(Last_column);
		delete_lp(Mlp);
		return(NULL);
	      }
	      if(strcmp(field5, "'INTORG'") == 0) {
		Int_section = TRUE;
		report(Mlp, FULL, "Switching to integer section");
	      }
	      else if(strcmp(field5, "'INTEND'") == 0) {
		Int_section = FALSE;
		report(Mlp, FULL, "Switching to non-integer section");
	      }
	      else
		report(Mlp, IMPORTANT, "Unknown marker (ignored) at line %d: %s",
			Lineno, field5);
	    }
	  }

	  if((items != 4) && (items != 6) && (items != 5)) { /* Wrong! */
	    report(Mlp, CRITICAL, "Wrong number of items (%d) in COLUMNS section (line %d)",
		    items, Lineno);
            FREE(Last_column);
	    delete_lp(Mlp);
	    return(NULL);
	  }
	  break;

        /* Process entries in the RHS section */
        /* field2: uninteresting name; field3: constraint name */
        /* field4: value */
        /* optional: field5: constraint name; field6: value */
	case MPSRHS:

	  report(Mlp, FULL, "RHS line: %s %s %g %s %g",
		   field2, field3, field4, field5, field6);

	  if((items != 4) && (items != 6)) {
	    report(Mlp, CRITICAL, "Wrong number of items (%d) in RHS section line %d",
		    items, Lineno);
            FREE(Last_column);
	    delete_lp(Mlp);
	    return(NULL);
	  }

	  if((row = find_row(Mlp, field3, Unconstrained_rows_found)) >= 0) {
	    set_rh(Mlp, row, (REAL)field4);
	  }

	  if(items == 6) {
	    if((row = find_row(Mlp, field5, Unconstrained_rows_found)) >= 0) {
	      set_rh(Mlp, row, (REAL)field6);
	    }
	  }

	  break;

        /* Process entries in the BOUNDS section */
        /* field1: bound type; field2: uninteresting name; */
        /* field3: variable name; field4: value */
        case MPSBOUNDS:

	  report(Mlp, FULL, "BOUNDS line: %s %s %s %g",
		 field1, field2, field3, field4);

	  var = find_var(Mlp, field3, FALSE);
	  if(var < 0){ /* bound on undefined var in COLUMNS section ... */
	    Column_ready = TRUE;
	    if (!addmpscolumn(Mlp, FALSE, &Column_ready, Last_column, &Nrows, field3)) {
	      FREE(Last_column);
	      delete_lp(Mlp);
	      return(NULL);
	    }
	    Column_ready = TRUE;
	    var = find_var(Mlp, field3, TRUE);
	  }
	  if(var < 0) /* undefined var and could add ... */;
	  else if(strcmp(field1, "UP") == 0) {
	    /* upper bound */
	    set_bounds(Mlp, var, get_lowbo(Mlp, var), field4);
	  }
	  else if(strcmp(field1, "SC") == 0) {
	    /* upper bound */
	    set_bounds(Mlp, var, get_lowbo(Mlp, var), (field4) /* (items >= 4) */ ? field4 : Mlp->infinite);
    	    set_semicont(Mlp, var, TRUE);
	  }
	  else if(strcmp(field1, "SI") == 0) {
	    /* upper bound */
	    set_bounds(Mlp, var, get_lowbo(Mlp, var), (field4) /* (items >= 4) */ ? field4 : Mlp->infinite);
	    set_int(Mlp, var, TRUE);
    	    set_semicont(Mlp, var, TRUE);
	  }
	  else if(strcmp(field1, "LO") == 0) {
	    /* lower bound */
	    set_bounds(Mlp, var, field4, get_upbo(Mlp, var));
	  }
	  else if(strcmp(field1, "PL") == 0)   /* plus-ranged variable */
            set_bounds(Mlp, var, get_lowbo(Mlp, var), Mlp->infinite);
	  else if(strcmp(field1, "MI") == 0) { /* minus-ranged variable */
	    set_bounds(Mlp, var, -Mlp->infinite, get_upbo(Mlp, var));
	  }
	  else if(strcmp(field1, "FR") == 0) { /* free variable */
	    set_bounds(Mlp, var, -Mlp->infinite, Mlp->infinite);
	  }
	  else if(strcmp(field1, "FX") == 0) {
	    /* fixed, upper _and_ lower  */
	    set_bounds(Mlp, var, field4, field4);
	  }
	  else if(strcmp(field1, "BV") == 0) { /* binary variable */
	    set_bounds(Mlp, var, 0, 1);
	    set_int(Mlp, var, TRUE);
	  }
	  /* AMPL bounds type UI and LI added by E.Imamura (CRIEPI)  */
	  else if(strcmp(field1, "UI") == 0) { /* upper bound for integer variable */
	    set_bounds(Mlp, var, get_lowbo(Mlp, var), field4);
	    set_int(Mlp, var, TRUE);
	  }
	  else if(strcmp(field1, "LI") == 0) { /* lower bound for integer variable - corrected by KE */
	    set_bounds(Mlp, var, field4, get_upbo(Mlp, var));
	    set_int(Mlp, var, TRUE);
	  }
#if 0
	  /* hack for free and negative variables. Ugly, and does not
	     always work. MB */
	  else if(strcmp(field1, "FR") == 0) { /* free variable */
	    report(Mlp, NORMAL,
		    "Free variable %s is split in a positive part %s and a negative part %s_",
		    field3, field3, field3);
	    strcat(field3, "_");
	    get_column(Mlp, var, Last_column);
	    for (i = 0; i <= Mlp->rows; i++)
	      Last_column[i]*=-1;
	    if ((!add_columnex(Mlp, Last_column, FALSE, NULL, 0)) || (!set_col_name(Mlp, Mlp->columns, field3))) {
              FREE(Last_column);
	      delete_lp(Mlp);
	      return(NULL);
	    }
	    /* should lower and upper bounds of both variables be adjusted? What
	       if lower and upper bound are specified later? MB */
	  }
	  else if(strcmp(field1, "MI") == 0) { /* negative variable */
	    report(Mlp, NORMAL,
		    "Negative variable %s will be represented by - %s-",
		    field3, field3);
	    get_column(Mlp, var, Last_column);
	    del_column(Mlp, var);
	    strcat(field3, "-");
	    for (i = 0; i <= Mlp->rows; i++)
	      Last_column[i] *= -1;
	    if ((!add_columnex(Mlp, Last_column, FALSE, NULL, 0)) || (!set_row_name(Mlp, var, field3))) {
              FREE(Last_column);
	      delete_lp(Mlp);
	      return(NULL);
	    }
	    /* should lower and upper bounds of variable be adjusted? What if
	       lower and upper bound are specified later? (does not work!) MB */
	  }
#endif
	  else {
	    report(Mlp, CRITICAL, "BOUND type %s on line %d is not supported",
		   field1, Lineno);
            FREE(Last_column);
	    delete_lp(Mlp);
	    return(NULL);
	  }

	  break;


        /* Process entries in the BOUNDS section */

	/* We have to implement the following semantics:

	    D. The RANGES section is for constraints of the form: h <=
	    constraint <= u .  The range of the constraint is r = u - h .  The
	    value of r is specified in the RANGES section, and the value of u or
	    h is specified in the RHS section.  If b is the value entered in the
	    RHS section, and r is the value entered in the RANGES section, then
	    u and h are thus defined:

	    row type       sign of r       h          u
	    ----------------------------------------------
	       G            + or -         b        b + |r|
	       L            + or -       b - |r|      b
	       E              +            b        b + |r|
	       E              -          b - |r|      b            */

        /* field2: uninteresting name; field3: constraint name */
        /* field4: value */
        /* optional: field5: constraint name; field6: value */
	  case MPSRANGES:


	  report(Mlp, FULL, "RANGES line: %s %s %g %s %g",
		 field2, field3, field4, field5, field6);

	  if((items != 4) && (items != 6)) {
	    report(Mlp, CRITICAL, "Wrong number of items (%d) in RANGES section line %d",
		   items, Lineno);
            FREE(Last_column);
	    delete_lp(Mlp);
	    return(NULL);
	  }

	  if(((row = find_row(Mlp, field3, Unconstrained_rows_found)) >= 0) /* && (field4 != 0) */) {
	    /* find out constraint type. If ch_sign[row] is TRUE, it is GE. If
	       ch_sign[row] is FALSE, it is an equality constraint if
	       orig_upbo[row] == 0. For a LE constraint, orig_upbo[row] should be
	       +infinity */

	    if(my_abs(field4) >= Mlp->infinite) {
	      report(Mlp, IMPORTANT,
		     "Warning, Range for row %s >= infinity (value %g) on line %d, ignored",
		     field3, field4, Lineno);
	    }
	    else if(Mlp->ch_sign[row]) {
	      /* GE */
	      Mlp->orig_upbo[row] = my_abs(field4);
	    }
	    else if(Mlp->orig_upbo[row] == 0 && field4 >= 0) {
	      /*  EQ with positive sign of r value */
	      set_constr_type(Mlp, row, GE);
	      Mlp->orig_upbo[row] = field4;
	    }
	    else if(Mlp->orig_upbo[row] == Mlp->infinite) {
	      /* LE */
	      Mlp->orig_upbo[row] = my_abs(field4);
	    }
	    else if(Mlp->orig_upbo[row] == 0 && field4 < 0) {
	      /* EQ with negative sign of r value */
	      set_constr_type(Mlp, row, LE);
	      Mlp->orig_upbo[row] = -field4;
	    }
	    else { /* let's be paranoid */
	      report(Mlp, IMPORTANT,
		      "Cannot figure out row type, row = %d, ch_sign = %d, upbo = %g one line %d",
		      row, Mlp->ch_sign[row], (double)Mlp->orig_upbo[row], Lineno);
	    }
	  }

	  if(items == 6) {
	    if(((row = find_row(Mlp, field5, Unconstrained_rows_found)) >= 0) /* && (field6 != 0) */) {
	      /* find out constraint type. If ch_sign[row] is TRUE, it is GE. If
		 ch_sign[row] is FALSE, it is an equality constraint if
		 orig_upbo[row] == 0. For a LE constraint, orig_upbo[row] should
		 be +infinity */

	      if(my_abs(field6) >= Mlp->infinite) {
		report(Mlp, IMPORTANT,
			"Warning, Range for row %s >= infinity (value %g) on line %d, ignored",
			field5, field6, Lineno);
	      }
	      else if(Mlp->ch_sign[row]) {
		/* GE */
		Mlp->orig_upbo[row] = my_abs(field6);
	      }
	      else if(Mlp->orig_upbo[row] == 0 && field6 >= 0) {
		/*  EQ with positive sign of r value */
		set_constr_type(Mlp, row, GE);
		Mlp->orig_upbo[row] = field6;
	      }
	      else if(Mlp->orig_upbo[row] == Mlp->infinite) {
		/* LE */
		Mlp->orig_upbo[row] = my_abs(field6);
	      }
	      else if(Mlp->orig_upbo[row] == 0 && field6 < 0) {
		/* EQ with negative sign of r value */
		set_constr_type(Mlp, row, LE);
		Mlp->orig_upbo[row] = -field6;
	      }
	      else { /* let's be paranoid */
		report(Mlp, IMPORTANT,
			"Cannot figure out row type, row = %d, ch_sign = %d, upbo = %g on line",
			row, Mlp->ch_sign[row], (double)Mlp->orig_upbo[row], Lineno);
	      }
	    }
	  }

          break;

        /* Process entries in the SOS section */

        /* We have to implement the following semantics:

          E. The SOS section is for ordered variable sets of the form:
  	      x1, x2, x3 ... xn where only a given number of consequtive variables
          may be non-zero.  Each set definition is prefaced by type, name
  	      and priority data.  Each set member has an optional weight that
  	      determines its order.  There are two forms supported; a full format
  	      and a reduced CPLEX-like format.                                       */

        case MPSSOS:
          report(Mlp, FULL, "SOS line: %s %s %g %s %g",
                 field2, field3, field4, field5, field6);

          if((items == 0) || (items > 4)) {
            report(Mlp, IMPORTANT,
                   "Invalid number of items (%d) in SOS section line %d\n",
                   items, Lineno);
            FREE(Last_column);   /* Added by KE */
            delete_lp(Mlp);
            return(NULL);
  	  }

          if(strlen(field1) == 0) items--;  /* fix scanline anomoly! */

	  /* Check if this is the start of a new SOS */
          if(items == 1 || items == 4) {
            row = (int) (field1[1] - '0');
            if((row <= 0) || (row > 9)) {
              report(Mlp, IMPORTANT,
                     "Error: Invalid SOS type %s line %d\n", field1, Lineno);
              FREE(Last_column);   /* Added by KE */
              delete_lp(Mlp);
              return(NULL);
            }
            field1[0] = '\0';               /* fix scanline anomoly! */

	    /* lp_solve needs a name for the SOS */
	    if(variant == 0) {
		if(strlen(field3) == 0)  /* CPLEX format does not provide a SOS name; create one */
		sprintf(field3, "SOS_%d", Mlp->sos_count + 1);
	    }
	    else {                     /* Remap XPRESS format name */
		strcpy(field3, field1);
	    }
	    /* Obtain the SOS priority */
	    if(items == 4)
	      SOS = (int) field4;
	    else
	      SOS = 1;

            /* Define a new SOS instance */

	    SOS = add_SOS(Mlp, field3, (short) row, SOS, 0, NULL, NULL);
  	  }
	  /* Otherwise, add set members to the active SOS */
          else {
	    char *field = (items == 3) ? field3 /* Native lp_solve and XPRESS formats */ : field2 /* CPLEX format */;

            var = find_var(Mlp, field, FALSE);  /* Native lp_solve and XPRESS formats */
	    if(var < 0){ /* SOS on undefined var in COLUMNS section ... */
	      Column_ready = TRUE;
	      if (!addmpscolumn(Mlp, FALSE, &Column_ready, Last_column, &Nrows, field)) {
		FREE(Last_column);
		delete_lp(Mlp);
		return(NULL);
	      }
	      Column_ready = TRUE;
	      var = find_var(Mlp, field, TRUE);
	    }
	    if((var < 0) || (SOS < 1)) /* undefined var and could add ... */;
            else append_SOSrec(Mlp, Mlp->sos_list[SOS-1], 1, &var, &field4);
  	  }

          break;

        }
      }
    }
    FREE(Last_column);
  }
  return(Mlp);
}
