#ifndef __BIN_SOLVER_H__
#define __BIN_SOLVER_H__

class BinSolver
{
public:
  const char* name; //Solver name
  int argc; // number of arguments
  char** argv;
  int inputFileParam;

  char outFileName[512];
  char solverName[512];
  
  bool outFileCreated;

  BinSolver(const char* _name, int _argc, int _inputFileParam);
  ~BinSolver();

  int spawnBinary(char* binFile, char* argv[], char* outFileName, int timeout);

  virtual int execute(char* inputFile, int timeout);
  virtual void cleanup();
};



extern BinSolver* SolverSaps;
extern BinSolver* SolverGsat;
extern BinSolver* SolverZchaff;
extern BinSolver* SolverSatelite;


void BuildSolvers(char* strseed, char* outfile);


#endif
