(* 
------------------------------------------------------------------
   This file is provided on AS IS basis. No warranties applied.
   Interface to XDS run-rime system can be changed at any moment.
------------------------------------------------------------------
*)

<*+M2EXTENSIONS*>
IMPLEMENTATION MODULE XDS;

PROCEDURE ["C"] / X2C_BEGIN (argc:        PINT;
                             argv:        PPCHAR;
                             gcauto:      INTEGER;
                             gcthreshold: INTEGER;
                             heap_limit:  INTEGER);
PROCEDURE ["C"] / X2C_INIT_HISTORY;
PROCEDURE ["C"] / X2C_EXIT;

PROCEDURE ["C"] XDS_Init (argc:        PINT;
                          argv:        PPCHAR;
                          gcauto:      INTEGER;
                          gcthreshold: INTEGER;
                          heap_limit:  INTEGER);
BEGIN
    X2C_BEGIN (argc, argv, gcauto, gcthreshold, heap_limit);
END XDS_Init;

PROCEDURE ["C"] XDS_Init_History;
BEGIN
    X2C_INIT_HISTORY;
END XDS_Init_History;

PROCEDURE ["C"] XDS_Exit;
BEGIN
    X2C_EXIT;
END XDS_Exit;

(*
  Microsoft Visual C interface
*)

PROCEDURE ["C"] _XDS_Init* (argc:        PINT;
                            argv:        PPCHAR;
                            gcauto:      INTEGER;
                            gcthreshold: INTEGER;
                            heap_limit:  INTEGER);
BEGIN
    XDS_Init (argc, argv, gcauto, gcthreshold, heap_limit);
END _XDS_Init;

PROCEDURE ["C"] _XDS_Init_History*;
BEGIN
    XDS_Init_History;
END _XDS_Init_History;

PROCEDURE ["C"] _XDS_Exit*;
BEGIN
    XDS_Exit;
END _XDS_Exit;

END XDS.
