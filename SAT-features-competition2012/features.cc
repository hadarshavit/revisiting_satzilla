#include "SATinstance.h"
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string.h>
#include "global.h"
using namespace std;

Stopwatch gSW;
int gTimeOut;
double preTime;
int OrigNumVars, OrigNumClauses;
double myTime = 0.0;
const char *mypath;

// int numFeats=64;
int getTimeOut(void)
{
  char *value = getenv("SATTIMEOUT");
  if (value == NULL)
    return (2419200); // -- 4 weeks

  return atoi(value);
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    cerr << "Usage: features {  [-all] | [-base] |[-sp] [-Dia] [-Cl] [-unit] [-ls] [-lobjois] } infile [outfile]" << endl;
    return 1;
  }

  bool doBase = false;
  bool doUnitProbe = false;
  bool doLSProbe = false;
  bool doCl = false;
  bool doDia = false;
  bool doSp = false;
  bool doLobjois = false;
  bool doLP = false;
  bool doComp = true;
  int nextParam = 1;
  string fullPath(argv[0]);
  size_t lastSlashPos = fullPath.find_last_of('/');
  string myownpath = fullPath.substr(0, lastSlashPos);

  if (lastSlashPos == -1)
  {
    myownpath = ".";
  }
  mypath = myownpath.c_str();

  if (strcmp(argv[nextParam], "-all") == 0)
  {
    doBase = true;
    doUnitProbe = true;
    doLSProbe = true;
    doLobjois = true;
    doCl = true;
    doDia = true;
    doSp = true;
    doLP = true;
    nextParam++;
    if (nextParam == argc)
    {
      cerr << "Usage: features { [-all] | [-base] [-sp] [-dia] [-cl] [-lp] [-unit] [-ls] [-lobjois] } infile [outfile] [preprocessing_time] [ls_time]" << endl;
      return 1;
    }
  }
  else
  {
    if (strcmp(argv[nextParam], "-base") == 0)
    {
      doBase = true;
      nextParam++;
      if (nextParam == argc)
      {
        cerr << "Usage: features { [-all] | [-base] [-sp] [-dia] [-cl] [-lp] [-unit] [-ls] [-lobjois] } infile [outfile]" << endl;
        return 1;
      }
    }

    if (strcmp(argv[nextParam], "-unit") == 0)
    {
      doUnitProbe = true;
      nextParam++;
      if (nextParam == argc)
      {
        cerr << "Usage: features { [-all] | [-base] [-sp] [-dia] [-cl] [-lp] [-unit] [-ls] [-lobjois] } infile [outfile]" << endl;
        return 1;
      }
    }
    if (strcmp(argv[nextParam], "-lp") == 0)
    {
      doLP = true;
      nextParam++;
      if (nextParam == argc)
      {
        cerr << "Usage: features { [-all] | [-base] [-sp] [-dia] [-cl] [-lp] [-unit] [-ls] [-lobjois] } infile [outfile]" << endl;
        return 1;
      }
    }

    if (strcmp(argv[nextParam], "-sp") == 0)
    {
      doSp = true;
      nextParam++;
      if (nextParam == argc)
      {
        cerr << "Usage: features { [-all] | [-base] [-sp] [-dia] [-cl] [-lp] [-unit] [-ls] [-lobjois] } infile [outfile]" << endl;
        return 1;
      }
    }

    if (strcmp(argv[nextParam], "-dia") == 0)
    {
      doDia = true;
      nextParam++;
      if (nextParam == argc)
      {
        cerr << "Usage: features { [-all] | [-base] [-sp] [-dia] [-cl] [-lp] [-unit] [-ls] [-lobjois] } infile [outfile]" << endl;
        return 1;
      }
    }

    if (strcmp(argv[nextParam], "-cl") == 0)
    {
      doCl = true;
      nextParam++;
      if (nextParam == argc)
      {
        cerr << "Usage: features { [-all] | [-base] [-sp] [-dia] [-cl] [-LP] [-unit] [-ls] [-lobjois] } infile [outfile]" << endl;
        return 1;
      }
    }

    if (strcmp(argv[nextParam], "-ls") == 0)
    {
      doLSProbe = true;
      nextParam++;
      if (nextParam == argc)
      {
        cerr << "Usage: features { [-all] | [-base] [-sp] [-dia] [-cl] [-lp] [-unit] [-ls] [-lobjois] } infile [outfile]" << endl;
        return 1;
      }
    }

    if (strcmp(argv[nextParam], "-lobjois") == 0)
    {
      doLobjois = true;
      nextParam++;
      if (nextParam == argc)
      {
        cerr << "Usage: features { [-all] | [-base] [-sp] [-dia] [-cl] [-lp] [-unit] [-ls] [-lobjois] } infile [outfile]" << endl;
        return 1;
      }
    }
  }
  bool letsgo = true;
  bool prep_sucessfull = false;
  if (!(doBase || doSp || doCl || doDia || doLobjois || doLP || doLSProbe || doUnitProbe))
    doBase = true;

  char *filename;
  char outfile[512];
  filename = argv[nextParam++];

  sprintf(outfile, "%s", "/tmp");
  strcat(outfile, "/outputXXXXXX");
  mkstemp(outfile);

  gTimeOut = getTimeOut();
  BuildSolvers("123456", outfile);
  gSW.Start();

  // before doing anything, count number of variables and clauses
  ifstream infile(filename);
  if (!infile)
  {
    fprintf(stderr, "c Error: Could not read from input file %s.\n", filename);
    exit(1);
  }
  char chbuf;
  char strbuf[1024];
  infile.get(chbuf);
  while (chbuf != 'p')
  {
    infile.ignore(1000, '\n');
    infile.get(chbuf);
    if (!infile)
    {
      fprintf(stderr, "c ERROR: Premature EOF reached in %s\n", filename);
      exit(1);
    }
  }
  infile >> strbuf; // "cnf"
  infile >> OrigNumVars >> OrigNumClauses;
  if (DEB)
    printf("c Orignal number of varibales is %d, number of clauses is %d \n", OrigNumVars, OrigNumClauses);
  bool solved = false;
  if (DEB)
    printf("c run SatELite as pre-processor ... \n");
  int returnVal;
  if (DEB)
    printf("c Input file is: %s. Output file is %s\n", filename, outfile);
  returnVal = SolverSatelite->execute(filename, 35);
  
  if (returnVal == 10 || returnVal == 20)
  {
    if (DEB)
      printf("c This instance is solved by pre-processor with %d!\n", returnVal);
    solved = true;
    doComp = false;
  }

  SolverSatelite->cleanup();
  SATinstance *sat;
  if (returnVal == 137)
  {
      sat = new SATinstance(filename, doComp);
  }
  else
  {
      // SATinstance* sat = new SATinstance(outfile);
    sat = new SATinstance(outfile, doComp);
    prep_sucessfull = true;
  }
  preTime = gSW.TotalLap() - myTime;
  sat->start_computation(solved, preTime);

  myTime = gSW.TotalLap();
  if (DEB)
    printf("c Pre-process time is %f second\n", preTime);

  if (doBase && letsgo)
  {
    returnVal = sat->computeFeatures(doComp);
    //   cout <<sat->getNumVals()<<"  "<< sat->getNumClaus()<<endl;
    if (sat->getNumVals() == 0 || sat->getNumClaus() == 0)
    {
      doComp = false;
      if (DEB)
        cout << "c Instance can be solved by unit propodation alone!!!\n";
    }
  }

  bool timeout = gSW.TotalLap() > TOTAL_TIMEOUT;
  if (doDia && letsgo && !timeout && returnVal != VCG_TIMEOUT_CODE)
    sat->init_diameter(doComp);

  timeout = gSW.TotalLap() > TOTAL_TIMEOUT;
  if (doCl && letsgo && !timeout)
  {
    if (prep_sucessfull)
        sat->cl_prob(outfile, doComp);
    else
        sat->cl_prob(filename, doComp);

  }

  timeout = gSW.TotalLap() > TOTAL_TIMEOUT;
  if (doSp && letsgo && !timeout)
    sat->sp(doComp);

  timeout = gSW.TotalLap() > TOTAL_TIMEOUT;
  if (doUnitProbe && letsgo && !timeout)
    sat->unitPropProbe(false, doComp);

  timeout = gSW.TotalLap() > TOTAL_TIMEOUT;
  if (doLP && letsgo && !timeout)
    sat->compute_lp(doComp);

  timeout = gSW.TotalLap() > TOTAL_TIMEOUT;
  if (doLSProbe && letsgo && !timeout)
  {
    if (prep_sucessfull){
    sat->localSearchProbeSaps(outfile, doComp);
    sat->localSearchProbeGsat(outfile, doComp);
    }
    else{
      sat->localSearchProbeSaps(filename, doComp);
      sat->localSearchProbeGsat(filename, doComp);
    }
  }

  timeout = gSW.TotalLap() > TOTAL_TIMEOUT;
  if (doLobjois && letsgo && !timeout)
    sat->lobjoisProbe(false, doComp);

  sat->finish_computation();
  if (letsgo)
    if (argc > nextParam)
    {
      sat->writeFeatNamesToFile(argv[nextParam]);
      sat->writeFeaturesToFile(argv[nextParam]);
    }
    else
    {
      sat->writeFeatNamesToFile(stdout);
      sat->writeFeaturesToFile(stdout);
    }

  delete sat;
  remove(outfile);

  return 0;
}
