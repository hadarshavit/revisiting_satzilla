#include "lpkit.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>

#if defined MSDOS || defined _WIN32 || defined WIN32 ||defined _WINDOWS || defined WINDOWS
# define PATH_DELIMITER '\\'
#else
# define PATH_DELIMITER '/'
#endif

int EndOfPgr()
 {
  exit(EXIT_FAILURE);
  return(0);
 }

void SIGABRT_func(int sig)
 {
  EndOfPgr();
 }

int main(int argc, char *argv[])
{
  lprec *lp;
  FILE *fpin, *fpout;
  int i;
  char *ptr, name[9];

  for (i = 1; i < argc; i++)
   if (argv[i][0] == '-') {
    printf("lp to mps file converter\n");
    printf("Usage: lp2mps [inputfile.lp [outputfile.mps]] [<inputfile.lp] [>outputfile.mps]\n");
    return(1);
   }

  if (argc >= 2) {
    fpin = fopen(argv[1], "r");
    if (fpin == NULL) {
      fprintf(stderr, "Unable to open input file %s\n", argv[1]);
      return(2);
    }
    if((ptr = strrchr(argv[1], PATH_DELIMITER)) != NULL)
      ptr++;
    else
      ptr = argv[1];
    strncpy(name, ptr, 8);
    name[8]=0;
    if((ptr = strrchr(name, '.')) != NULL)
      *ptr = 0;
  }
  else {
    fpin = stdin;
    strcpy(name, "unknown");
  }

  if (argc >= 3) {
    fpout = fopen(argv[2], "w");
    if (fpout == NULL) {
      fprintf(stderr, "Unable to open output file %s\n", argv[2]);
      return(3);
    }
  }
  else
    fpout = stdout;

  signal(SIGABRT, SIGABRT_func);

  fprintf(stderr,"reading lp file\n");
  lp = read_lp(fpin, FULL, name);
  if (fpin != stdin)
    fclose(fpin);

  if (lp == NULL) {
    fprintf(stderr, "Unable to read lp file\n");
    return(4);
  }
  else {
    fprintf(stderr,"writing mps file\n");
    if (!write_MPS(lp, fpout)) {
      fprintf(stderr, "Unable to write mps file\n");
      return(5);
    }
  }
  if (fpout != stdout)
    fclose(fpout);

  return(0);
}
