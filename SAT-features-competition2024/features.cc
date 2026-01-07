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
int OrigNumVars = -1, OrigNumClauses = -1;
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

  bool weighted_cnf = false;
  bool first_line = true;
  char wordbuf[256];

  char tmp_outfile_name[512];  // Create a temporary cnf file for the other programs to read
  while (infile >> wordbuf)
  {
    if (wordbuf[0] == 'p')  // CNF formated
    {
        infile >> strbuf; // "cnf"
        infile >> OrigNumVars >> OrigNumClauses;  // line with p is the start, states file type then number of vars and number of clauses
        break;
    }
    else if (strstr(wordbuf, "c{"))  // MaxSat declaration line
    {
        weighted_cnf = true;
        sprintf(tmp_outfile_name, "%s", "/tmp");
        strcat(tmp_outfile_name, "/tmp_wcnf_to_cnf_XXXXXX");
        mkstemp(tmp_outfile_name);
        if (DEB)
            printf("c Processing WCNF file to CNF... \n");
        while (infile >> wordbuf)  // read until description is done
        {
            if (strstr(wordbuf, "c}"))  // Final descriptive word, break
            {
                break;
            }
            if (OrigNumVars == -1 && strstr(wordbuf, "nvars\":"))
            {
                infile >> OrigNumVars; 
            }
            else if (OrigNumClauses == -1 && strstr(wordbuf, "ncls\":"))
            {
                infile >> OrigNumClauses;  
            } // TODO: there are many other values that we could read
        }
        // TODO: Extracted the meta information, now format the rest of the file from wcnf to cnf
        std::ofstream tmp_outfile;
        tmp_outfile.open(tmp_outfile_name);
        tmp_outfile << "p cnf " << OrigNumVars << " " << OrigNumClauses << endl;
        for( std::string line; getline( infile, line ); )
        {
            // skip weight lines, comment lines and empty lines
            // NOTE: What does a line starting with "h" do?
            if (line[0] == 'w' || line[0] == 'c' || line.find_first_not_of(' ') == std::string::npos) { 
                continue;
            }
            int white_space_n = line.find_first_of(' ');
            // Remove the weight from the line, or the 'h' character (What it be doin tho?)
            line = line.substr(line.find_first_of(" \t") + 1);
            tmp_outfile << line << endl;
        }
        tmp_outfile.close();
        // For all other programs, our temporary file should be considered the inputfile
        filename = tmp_outfile_name;
        break;
    }
  }

  if (OrigNumVars == -1 || OrigNumClauses == -1)  // We did not find a value before EOF
  {
    fprintf(stderr, "\nc ERROR: Initial read did not find number of vars and clauses, reached Premature EOF in %s ", filename);
    exit(1);
  }

  if (DEB)
    printf("c Orignal number of variables is %d, number of clauses is %d \n", OrigNumVars, OrigNumClauses);
  bool solved = false;
  if (DEB)
    printf("c run SatELite as pre-processor ... \n");
  int returnVal;
  if (DEB)
    printf("c Input file is: %s. Output file is %s\n", filename, outfile);
  // TODO: Enable the below code to take in a file stream instead of a file name
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
  if (weighted_cnf == true){
    remove(tmp_outfile_name);
  }

  return 0;
}
