#include "BinSolver.h"
#include "global.h"

#include "model.h"

#include <stdio.h>
#include <string.h>

BinSolver *SolverSatelite;
BinSolver *SolverZchaff;
BinSolver *SolverSaps;
BinSolver *SolverGsat;

void BuildSolvers(char *strseed, char *outfile)
{

  // --vallst
  SolverSatelite = new BinSolver("sbva", 7, 2);
  SolverSatelite->argv[1] = "-i";
  SolverSatelite->argv[3] = "-o";
  SolverSatelite->argv[4] = outfile;
  SolverSatelite->argv[5] = "-t";
  SolverSatelite->argv[6] = SBVA_TIMEOUT;
  SolverSatelite->argv[7] = NULL;

  // -- zchaff07 for compute features
  SolverZchaff = new BinSolver("cadical2023", 3, 1);
  SolverSatelite->argv[2] = "--plain";
  SolverZchaff->argv[3] = NULL;

  SolverSaps = new BinSolver("ubcsat2006", 18, 2);
  SolverSaps->argv[1] = "-inst";
  SolverSaps->argv[3] = "-alg";
  SolverSaps->argv[4] = "sparrow";
  SolverSaps->argv[5] = "-noimprove";
  SolverSaps->argv[6] = "0.1n";
  SolverSaps->argv[7] = "-r";
  SolverSaps->argv[8] = "stats";
  SolverSaps->argv[9] = outfile;
  SolverSaps->argv[10] = "best[mean+cv],firstlmstep[mean+median+cv+q10+q90],bestavgimpr[mean+cv],firstlmratio[mean+cv],estacl,numsolve";
  SolverSaps->argv[11] = "-runs";
  SolverSaps->argv[12] = UBCSAT_NUM_RUNS;
  SolverSaps->argv[13] = "-gtimeout";
  SolverSaps->argv[14] = UBCSAT_TIME_LIMIT;
  SolverSaps->argv[15] = "-solve";
  SolverSaps->argv[16] = "-v";
  SolverSaps->argv[17] = "sat11";
  SolverSaps->argv[18] = NULL;

  SolverGsat = new BinSolver("ubcsat2006", 16, 2);
  SolverGsat->argv[1] = "-inst";
  SolverGsat->argv[3] = "-alg";
  SolverGsat->argv[4] = "gsat";
  SolverGsat->argv[5] = "-noimprove";
  SolverGsat->argv[6] = "0.5n";
  SolverGsat->argv[7] = "-r";
  SolverGsat->argv[8] = "stats";
  SolverGsat->argv[9] = outfile;
  SolverGsat->argv[10] = "best[mean+cv],firstlmstep[mean+median+cv+q10+q90],bestavgimpr[mean+cv],firstlmratio[mean+cv],estacl,numsolve";
  SolverGsat->argv[11] = "-runs";
  SolverGsat->argv[12] = UBCSAT_NUM_RUNS;
  SolverGsat->argv[13] = "-gtimeout";
  SolverGsat->argv[14] = UBCSAT_TIME_LIMIT;
  SolverGsat->argv[15] = "-solve";
  SolverGsat->argv[16] = NULL;
}