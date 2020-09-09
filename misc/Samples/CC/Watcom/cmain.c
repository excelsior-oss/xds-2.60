/*
  Example of multi-language programming: M2 and C functions in one program
*/

#include <stdio.h>
#include "xds.h"
#include "m2test.h"

#define GCAUTO          1
#define HEAPLIMIT       4000000
#define GCTHRESHOLD     2000000

void cfunc(void)
{
        printf("Calling Modula-2 function with C calling conventions\n");
        printf("Result is %d (expected 4)\n",m2func(2,2));
}

int main(int argc, char **argv)
{
        XDS_Init(&argc,argv,GCAUTO,GCTHRESHOLD,HEAPLIMIT); /* always */
        XDS_Init_History();  /* if GENHISTORY+ */
/* call M2/O2 module's bodies - impossible if using MSVC */
#if ! defined (__MSVC__)
        m2test_BEGIN();
#endif
/* C code begin */
        cfunc();
/* C code end */
        XDS_Exit();          /* will never return */
	return 0;
}
