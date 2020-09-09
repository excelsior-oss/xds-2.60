/* 
------------------------------------------------------------------
   This file is provided on AS IS basis. No warranties applied.
   Interface to XDS run-rime system can be changed at any moment.
------------------------------------------------------------------
*/

/*
  Example of multi-language programming: M2 and C functions in one program
*/

extern void XDS_Init (int *argc, char **argv,
                      int gcauto, long gcthreshold, long heap_limit);
extern void XDS_Exit (void);
extern void XDS_Init_History (void);

