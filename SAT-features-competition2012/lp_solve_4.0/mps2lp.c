#include "lpkit.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>

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
  int i, maximize = FALSE, offset = 0;

  for (i = 1; i < argc; i++)
   if (strcmp(argv[i], "-min") == 0) {
     maximize = FALSE;
     offset++;
   }
   else if (strcmp(argv[i], "-max") == 0) {
     maximize = TRUE;
     offset++;
   }
   else if (argv[i][0] == '-') {
     printf("mps to lp file converter\n");
     printf("Usage: mps2lp [-min|-max] [inputfile.mps [outputfile.lp]] [<inputfile.mps] [>outputfile.lp]\n");
     return(1);
   }

  if (argc >= 2 + offset) {
    fpin = fopen(argv[1 + offset], "r");
    if (fpin == NULL) {
      fprintf(stderr, "Unable to open input file %s\n", argv[1 + offset]);
      return(2);
    }
  }
  else
    fpin = stdin;

  if (argc >= 3 + offset) {
    fpout = fopen(argv[2 + offset], "w");
    if (fpout == NULL) {
      fprintf(stderr, "Unable to open output file %s\n", argv[2 + offset]);
      return(3);
    }
  }
  else
    fpout = stdout;

  signal(SIGABRT,/* (void (*) OF((int))) */ SIGABRT_func);

  fprintf(stderr,"reading mps file\n");
  lp = read_mps(fpin, DETAILED);
  if (fpin != stdin)
    fclose(fpin);

  if (lp == NULL) {
    fprintf(stderr, "Unable to read mps file\n");
    return(4);
  }
  else {
    if (maximize) {
      set_maxim(lp);
      lp->ch_sign[0] = FALSE;
    }
/*
    if(lp->sos_count) {
      fprintf(stderr,"model contains SOS variables.\nUnable to store in lp format.\n");
      return(6);
    }
*/
/*
    for(i = get_Ncolumns(lp);(i > 0) && (!is_semicont(lp, i)); i--);
    if(i >= 1) {
      fprintf(stderr,"model contains semi-continious variables.\nUnable to store in lp format.\n");
      return(7);
    }
*/
    fprintf(stderr,"\rwriting lp file\n");
    if (!write_LP(lp, fpout)) {
      fprintf(stderr, "Unable to write lp file\n");
      return(5);
    }
  }
  if (fpout != stdout)
    fclose(fpout);

  return(0);
}
