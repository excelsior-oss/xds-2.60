/*
  Example of multi-language programming: M2 and C functions in one program
*/

#include <stdio.h>
#include "m2test.h"

#define GCAUTO          1
#define HEAPLIMIT       4000000
#define GCTHRESHOLD     2000000

extern void X2C_BEGIN(int *argc, char **argv,
                      int gcauto, long gcthreshold, long heap_limit);
extern void X2C_InitFPP(void);
extern void X2C_INIT_HISTORY(void);
extern void X2C_EXIT(void);

void cfunc(void)
{
        printf("Calling Modula-2 function with C calling conventions\n");
        printf("Result is %d (expected 4)\n",m2func(2,2));
}

int main(int argc, char **argv)
{
        X2C_InitFPP();       /* if program uses floating point */
        X2C_INIT_HISTORY();  /* if GENHISTORY+ */
        X2C_BEGIN(&argc,argv,GCAUTO,GCTHRESHOLD,HEAPLIMIT); /* always */
/* call M2/O2 module's bodies */
        m2test_BEGIN();
/* C code begin */
        cfunc();
/* C code end */
        X2C_EXIT();          /* will never return */
	return 0;
}
