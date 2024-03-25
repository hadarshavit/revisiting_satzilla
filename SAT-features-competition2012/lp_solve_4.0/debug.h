#ifndef __DEBUG_H__
#define __DEBUG_H__

/* prototypes for debug printing by other files */

void print_str(char *str);
void debug_print(lprec *lp, char *format, ...);
void debug_print_solution(lprec *lp);
void debug_print_bounds(lprec *lp, REAL *upbo, REAL *lowbo);

#endif
