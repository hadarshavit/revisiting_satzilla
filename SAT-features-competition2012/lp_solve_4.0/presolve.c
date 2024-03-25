#include <stdio.h>
#include <string.h>
#include "lpkit.h"

#if FALSE
static int find_column(lprec *lp, int matindex)
{
  int j;

  for(j = 1; j <= lp->columns_used; j++) {
    if(matindex < lp->col_end[j])
      break;
  }
  return(j);
}
#endif

static short tighten_bounds(lprec *lp, int i, int j, int items,
                            REAL *LOvalue, REAL *UPvalue, int *count)
{
  REAL  RHlow, RHup, RHrange, LObound, UPbound, Value;
  MYBOOL  SCvar;
  int   elmnr, k, oldcount;

  SCvar = (MYBOOL) is_semicont(lp, j);
  if(SCvar)
    return(RUNNING);
  Value = get_mat(lp,i,j);
  if(Value == 0)
    return(RUNNING);

  /* Initialize and identify semicontinuous variable */
  LObound = get_lowbo(lp, j);
  UPbound = get_upbo(lp, j);
  if(SCvar)
    LObound = 0;

  /* Compute effective bounds for the row */
  RHrange = get_rh_range(lp, i);
  if(lp->ch_sign[i]) {
    RHlow = get_rh(lp, i);
    if(RHrange < lp->infinite)
      RHup = RHlow+RHrange;
    else
      RHup = lp->infinite;
  }
  else {
    RHup = get_rh(lp, i);
    if(RHrange < lp->infinite)
      RHlow = RHup-RHrange;
    else
      RHlow = -lp->infinite;
  }

  /* Change inequality type if the coefficient is negative */
  if(Value < 0) {
    RHrange = RHup;
    RHup = RHlow;
    RHlow = RHrange;
  }

  /* Get residual/slack row bounds for the implied effect of other row variables */
  if(items == 1)
    RHlow = my_max(LObound, RHlow / Value);
  else if(LOvalue[i] > -lp->infinite && LOvalue[i] < RHlow) {
    RHrange = LOvalue[i]-LObound*Value;
    if(fabs(LOvalue[i]) < lp->epsel || (fabs(RHrange) < lp->epsel))
      RHlow = -lp->infinite;
    else {
      if(RHrange < lp->infinite)
        RHlow -= RHrange+LObound*Value;
      if(RHlow > -lp->infinite) {
        RHlow /= Value;
        RHlow += LObound;
      }
    }
  }
  else
    RHlow = -lp->infinite;

  if(items == 1)
    RHup = my_min(UPbound, RHup / Value);
  else if(UPvalue[i] < lp->infinite && UPvalue[i] < RHup) {
    RHrange = UPvalue[i]-UPbound*Value;
    if(fabs(RHrange) < lp->epsel) {
      if(RHrange < lp->infinite)
        RHup -= RHrange+UPbound*Value;
      if(RHup < lp->infinite) {
        RHup /= Value;
        RHup += UPbound;
      }
    }
  }
  else
    RHup = lp->infinite;

  /* Compute effective bounds for the active variable */
  oldcount = (*count);
  if(RHlow > -lp->infinite && RHlow-lp->epsel > LObound) {
    if(is_int(lp, j))
      RHlow = (REAL) ceil(RHlow);
    if(LObound > -lp->infinite)
      for(elmnr = lp->col_end[j-1]; elmnr < lp->col_end[j]; elmnr++) {
        k = lp->mat[elmnr].row_nr;
        if(LOvalue[k] > -lp->infinite) {
          Value = my_chsign(lp->ch_sign[k], lp->mat[elmnr].value);
          if(lp->columns_scaled)
            Value /= lp->scale[k] * lp->scale[lp->rows + j];
          LOvalue[k] += (RHlow-LObound)*Value;
        }
      }
    LObound = RHlow;
    (*count)++;
  }
  if(RHup < lp->infinite && RHup+lp->epsel < UPbound) {
    if(is_int(lp, j))
      RHup = (REAL) floor(RHup);
    if(UPbound < lp->infinite)
      for(elmnr = lp->col_end[j-1]; elmnr < lp->col_end[j]; elmnr++) {
        k = lp->mat[elmnr].row_nr;
        if(UPvalue[k] < lp->infinite) {
          Value = my_chsign(lp->ch_sign[k], lp->mat[elmnr].value);
          if(lp->columns_scaled)
            Value /= lp->scale[k] * lp->scale[lp->rows + j];
          UPvalue[k] += (RHup-UPbound)*Value;
        }
      }
    UPbound = RHup;
    (*count)++;
  }

  /* Now set the new bounds, if they are tighter */
  if((*count) > oldcount) {
    if(LObound > UPbound) {
      if(LObound-UPbound < lp->epsel)
        LObound = UPbound;
      else {
        report(lp, SEVERE, "tighten_bounds found low bound >= upper bound in row %d, column %d",
                           i, j);
        return(INFEASIBLE);
      }
    }
    if(lp->trace || lp->verbose>DETAILED)
      report(lp, NORMAL, "tighten_bounds replaced bounds on column %d to [%g ... %g]",
                            j, LObound, UPbound);
    set_bounds(lp, j, LObound, UPbound);
  }

  return(RUNNING);
}

int presolve(lprec *lp)
{
  int  i, j, ix, nn, nc, nv, nb, ns, nt;
  int  *counts = NULL, *signs = NULL;
  REAL *uppers = NULL, *lowers = NULL, value, bound;
  short	status, rowtype;

  status = RUNNING;
  if(lp->do_presolve == status)
    return(status);

 /* Do traditional simple presolve */
  if(lp->do_presolve) {
    if(lp->trace || lp->debug)
      report(lp, FULL, "Entered presolve at iteration %d",lp->total_iter);

    /* lp->orig_rows = lp->rows; */

    nv = 0;
    nc = 0;
    nb = 0;
    ns = 0;
    nt = 0;

    /* identify infeasible SOS'es prior to any pruning */
    for(i = 1; i <= lp->sos_count; i++) {
      nn = SOS_infeasible(lp, i);
      if(nn > 0) {
        report(lp, NORMAL, "presolve found SOS %d (type %d) to be range-infeasible on variable %d",
			               i, SOS_get_type(lp, i), nn);
	status = INFEASIBLE;
	ns++;
      }
    }

    if(ns <= 0) {
     /* Initialize the row NZ count, signs and bounds arrays */
      ix = my_max(lp->rows, lp->columns);
      if ((CALLOC(counts, lp->rows + 1, int) == NULL) ||
          (CALLOC(signs, lp->rows + 1, int) == NULL) ||
          (CALLOC(uppers, ix + 1, REAL) == NULL) ||
          (CALLOC(lowers, lp->rows + 1, REAL) == NULL)) {
        FREE(lowers);
        FREE(uppers);
        FREE(signs);
        FREE(counts);
        lp->spx_status = OUT_OF_MEMORY;
        return(INFEASIBLE);
      }
      else {
        /* Remove empty columns */
        for(j = 1; j <= lp->columns_used; j++) {
    	  for(ix = lp->col_end[j - 1]; ix < lp->col_end[j]; ix++) {
    	    i = lp->mat[ix].row_nr;
            value = my_chsign(lp->ch_sign[i], lp->mat[ix].value);
            if(lp->scaling_used)
              value /= lp->scale[i] * lp->scale[lp->rows + j];

    	    /* Cumulate row counts */
            counts[i]++;
    	    if(lp->mat[ix].value < 0)
    	      signs[i]--;
    	    else
    	      signs[i]++;

            /* Cumulate effective upper row bound */
    	    bound = get_upbo(lp, j);
    	    if(uppers[i] < lp->infinite) {
    	      if(bound >= lp->infinite)
                uppers[i] = lp->infinite;
    	      else
    		uppers[i] += value*bound;
    	    }

            /* Cumulate effective lower row bound */
    	    bound = get_lowbo(lp, j);
    	    if(lowers[i] > -lp->infinite) {
    	      if(bound <= -lp->infinite)
                lowers[i] = -lp->infinite;
    	      else if(!is_semicont(lp, j))
    		lowers[i] += value*bound;
    	    }
    	  }
    	}

        do {
  	  nn = 0;

          /* Eliminate empty columns */
  	  for(j = lp->columns; j > 0; j--) {
  	    if((j > lp->columns_used) || (lp->col_end[j] == lp->col_end[j - 1])) {
	      if(lp->sos_count > 0 && SOS_is_member(lp, 0, j))
                report(lp, NORMAL, "presolve found empty variable %d as member of a SOS", j);
	      else {
                del_columnex(lp, j, TRUE);
    		nv++;
  		nn++;
              }
  	    }
          }

  	  /* eliminate empty rows, convert row singletons to bounds,
	   tighten bounds, and remove always satisfied rows */
  	  for(i = lp->rows; i > 0; i--) {
  	    rowtype = (short) get_constr_type(lp, i);

  	    /* First identify any full row infeasibilities */
  	    if(rowtype != LE)
  	      if(my_max(lowers[i], uppers[i]) < get_rh(lp, i) - lp->infinite) {
                report(lp, NORMAL, "presolve found upper bound infeasibility in row %d", i);
                FREE(lowers);
                FREE(uppers);
                FREE(signs);
                FREE(counts);
                return(INFEASIBLE);
  	      }
    	    if(rowtype != GE)
  	      if(my_min(lowers[i], uppers[i]) > get_rh(lp, i) + lp->infinite) {
                report(lp, NORMAL, "presolve found lower bound infeasibility in row %d", i);
                FREE(lowers);
                FREE(uppers);
                FREE(signs);
                FREE(counts);
                return(INFEASIBLE);
    	      }

  	    /* Then delete any empty or always satisfied row */
  	    j = counts[i];
  	    if(j == 0 || ((abs(signs[i]) == counts[i]) && (fabs(uppers[i]-lowers[i]) < lp->epsel))) {
  	      del_constraintex(lp, i, TRUE);
	      for(ix = i; ix <= lp->rows; ix++) {
                counts[ix] = counts[ix + 1];
		signs[ix] = signs[ix + 1];
		lowers[ix] = lowers[ix + 1];
		uppers[ix] = uppers[ix + 1];
              }
              nc++;
    	      nn++;
  	    }
            /* Convert row singletons to bounds */
            else if(j == 1) {
 	      for(j = 1; j <= lp->columns; j++)
 		if(get_mat_raw(lp, i, j) != 0)
 		  break;
 	      status = tighten_bounds(lp, i, j, counts[i], lowers, uppers, &nt);
 	      if(status == INFEASIBLE) {
                nn = 0;
		break;
	      }
	      del_constraintex(lp,i, TRUE);
	      for(ix = i; ix <= lp->rows; ix++) {
                counts[ix] = counts[ix + 1];
		signs[ix] = signs[ix + 1];
		lowers[ix] = lowers[ix + 1];
		uppers[ix] = uppers[ix + 1];
              }
              nb++;
    	      nn++;
  	    }
          }

          /* Try again if we were successful in this presolve loop */
        } while (nn > 0);

        /* Report summary information */
        if(fabs(uppers[0]-lowers[0]) < lp->epsel)
          report(lp, NORMAL, "presolve identified optimal solution");
        if(nc)
          report(lp, NORMAL, "presolve removed %d empty or redundant rows",nc);
        if(nb)
          report(lp, NORMAL, "presolve converted %d singleton rows to bounds",nb);
        if(nt)
          report(lp, NORMAL, "presolve tightened %d bounds",nt);
        if(nv)
          report(lp, NORMAL, "presolve removed %d empty columns",nv);

        /* Add MIP cut (***development placeholder***) */
        /*
        if(lp->int_count + lp->sc_count + lp->sos_count > 0) {
          get_row(lp, 0, uppers);
          if(lp->maximise)
    	    add_constraint(lp, uppers, GE, -lp->infinite);
          else
    	    add_constraint(lp, uppers, LE, lp->infinite);
        }
        */

        free(counts);
        free(signs);
        free(lowers);
        free(uppers);
      }
    }
  }

  return(status);

}
