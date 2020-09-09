--------------------------------------------------------------------------------
--                        Excelsior XDS Test Coverage
--                          (c) 2002, Excelsior Ltd.
--
-- Module:       tcmerge
-- Author:       Lvov Konstantin
-- Modified By:
-- Created:      20-Feb-2003
-- Purpose:      Test coverage statistics merge utility
--------------------------------------------------------------------------------
<*+ MAIN *>
<*+ o2addkwd *>
<*+ o2extensions *>

MODULE tcmerge;

IMPORT  sys := SYSTEM,        pe := ProgEnv,      xosFS
     ,  dstr := DStrings,     io  := Printf,      tc := xrTCov
     ;

CONST
  print = io.printf;

TYPE
  INT    = sys.INT32;
  String = dstr.String;

VAR
  ArgNumber:  INT;
  MergedInfo: tc.TModuleInfoPtr;


--------------------------------------------------------------------------------
--                         Process  Errors
--------------------------------------------------------------------------------
TYPE
  TError = (
    errUnrecognizedOption
  , errArgumentMissing
  , errNoSuchFile
  );

CONST
  MAX_ErrorMessageLen = 32;

TYPE
  TErrorMessages = ARRAY TError OF ARRAY MAX_ErrorMessageLen OF CHAR;

CONST
  ErrorMessages = TErrorMessages {
    'unrecognized option "%s"'
  , 'argument to "%s" is missing'
  , '"%s": no such file'
  };


--------------------------------------------------------------------------------
PROCEDURE Error (err: TError; SEQ x: sys.BYTE);
BEGIN
  print ("\n# ");
  print (ErrorMessages[err], x);
  print ("\");
END Error;


--------------------------------------------------------------------------------
PROCEDURE FatalError (err: TError; SEQ x: sys.BYTE);
BEGIN
  Error (err, x);
  HALT();
END FatalError;


--------------------------------------------------------------------------------
CONST
  Version   = "1.0";
  CopyRight = "(c) 2003 Excelsior, LLC";

PROCEDURE ShowHeader ();
BEGIN
  print ("XDS Test Coverage Merge Utility v%s %s.\n", Version, CopyRight);
END ShowHeader;


--------------------------------------------------------------------------------
--                         Process  Options
--------------------------------------------------------------------------------
TYPE
  TOptions = RECORD
    outputFileName: String;
    verbose       : BOOLEAN;
    firstFileName : String;
  END;

VAR
  Option: TOptions;


--------------------------------------------------------------------------------
PROCEDURE (VAR opt: TOptions) PrintHelp ();
BEGIN
  print ("Usage: tcmerge [options] file1 file2 ...\n");
  print ("Options:\n");
  print ("  -o <file>   Place the output into <file>\n");
  print ("  -v          Verbose: compare merge result with first file in list\n");
END PrintHelp;


--------------------------------------------------------------------------------
PROCEDURE (VAR opt: TOptions) Default ();
BEGIN
  opt.outputFileName := NIL;
  opt.verbose        := FALSE;
  opt.firstFileName  := NIL;
END Default;


--------------------------------------------------------------------------------
PROCEDURE (VAR opt: TOptions) AddFile (fname-: ARRAY OF CHAR);
BEGIN
  IF opt.outputFileName = NIL THEN
    dstr.Assign (fname, opt.outputFileName);
  END;
  IF opt.firstFileName = NIL THEN
    dstr.Assign (fname, opt.firstFileName);
  END;
END AddFile;


--------------------------------------------------------------------------------
PROCEDURE (VAR opt: TOptions) ParseOption (VAR argInx: INT; argValue-: ARRAY OF CHAR);

  -- 1 -- ParseOption ----------------------------------------------------------
  PROCEDURE CheckArgument ();
  BEGIN
    IF (argInx >= ArgNumber) THEN
      FatalError (errArgumentMissing, argValue);
    END;
  END CheckArgument;

-- 0 -- ParseOption ------------------------------------------------------------
BEGIN
  IF argValue = "-o" THEN
    INC (argInx);
    CheckArgument ();
    NEW (opt.outputFileName, pe.ArgLength(argInx) + 1);
    pe.GetArg (argInx, opt.outputFileName^);
  ELSIF argValue = "-v" THEN
    opt.verbose := TRUE;
  ELSE
    FatalError (errUnrecognizedOption, argValue);
  END;
END ParseOption;


--------------------------------------------------------------------------------
--               Read and Merge Test Coverage Information
--------------------------------------------------------------------------------
PROCEDURE ReadTCFile (name-: ARRAY OF CHAR; VAR info: tc.TModuleInfoPtr);
BEGIN
  IF xosFS.X2C_Exists (name) THEN
    tc.ReadTCFile (name, NIL, info);
  ELSE
    Error (errNoSuchFile, name);
  END;
END ReadTCFile;


--------------------------------------------------------------------------------
--                  Write Merged Test Coverage Information
--------------------------------------------------------------------------------
PROCEDURE WriteTCFile (name-: ARRAY OF CHAR; VAR info: tc.TModuleInfoPtr);
VAR rlist, next: tc.TModuleInfoPtr;
BEGIN
  IF xosFS.X2C_Exists (name) THEN
    tc.MakeBackUp (name);
  END;

  -- restore original module order
  rlist := NIL;
  WHILE info # NIL DO
    next      := info.next;
    info.next := rlist;
    rlist := info;
    info  := next;
  END;
  info := rlist;

  tc.WriteTCFile (name, NIL, info);
END WriteTCFile;


--------------------------------------------------------------------------------
--              Verbose: Print Test Coverage Changes Information
--------------------------------------------------------------------------------
VAR
  totalCount, totalCov, totalDiff: INT;
  totalMod, totalChanged: INT;

PROCEDURE Rate * (all, used: INT): INT;
BEGIN
  IF (all # 0) THEN  RETURN used * 100 DIV all;
  ELSE               RETURN 0;
  END;
END Rate;


--------------------------------------------------------------------------------
PROCEDURE GetModuleInfo ( list: tc.TModuleInfoPtr
                        ; modname-: ARRAY OF CHAR ): tc.TModuleInfoPtr;
BEGIN
  WHILE (list # NIL) AND (modname # sys.VAL(tc.TModuleName, list.modname)^) DO
    list := list.next;
  END;
  RETURN list;
END GetModuleInfo;


--------------------------------------------------------------------------------
PROCEDURE PrintInfoChanges_Module (new, old: tc.TModuleInfoPtr);

  -- 1 -- PrintInfoChanges_Module ----------------------------------------------
  PROCEDURE Covered (mod: tc.TModuleInfoPtr): tc.TIndex;
  VAR i, res: INT;
  BEGIN
    res := 0;
    FOR i:= 0 TO mod.count-1 DO
      IF sys.VAL(tc.TCounters, mod.counters)[i] > 0 THEN
        INC(res);
      END;
    END;
    RETURN res;
  END Covered;

-- 0 -- PrintInfoChanges_Module ------------------------------------------------
VAR new_cov, old_cov, diff: INT;
    wasChanges: BOOLEAN;
BEGIN
  IF (old = NIL) OR (new.modtime # old.modtime) OR (new.count # old.count) THEN
    new_cov := Covered (new);
    diff := new_cov;
    wasChanges := TRUE;
  ELSE
    new_cov := Covered (new);
    old_cov := Covered (old);
    diff := new_cov - old_cov;
    wasChanges := diff # 0;
  END;

  IF wasChanges THEN
    print ( " %+9u    %+3u%%  %+9d  %+3d%%  %s\n"
          , new.count, Rate(new.count, new_cov)
          , diff, Rate(new.count, diff)
          , sys.VAL(tc.TModuleName, new.modname)^);
    INC(totalChanged);
  END;

  INC(totalMod);
  INC(totalCount, new.count);
  INC(totalCov, new_cov);
  INC(totalDiff, diff);
END PrintInfoChanges_Module;


--------------------------------------------------------------------------------
PROCEDURE PrintInfoChanges ();
VAR verboseInfo: tc.TModuleInfoPtr;
    old, new: tc.TModuleInfoPtr;
BEGIN
  totalMod     := 0;
  totalChanged := 0;
  totalCount   := 0;
  totalCov     := 0;
  totalDiff    := 0;

  verboseInfo := NIL;
  ReadTCFile (Option.firstFileName^, verboseInfo);
  IF verboseInfo = NIL THEN RETURN END;

  print ("\n");
  print ("Conditions Covered AddCovered Add %  Module\n");
  print ("---------- ------- ---------- -----  ------------------------------------------\n");

  new := MergedInfo;
  WHILE new # NIL DO
    old := GetModuleInfo (verboseInfo, sys.VAL(tc.TModuleName, new.modname)^);
    PrintInfoChanges_Module (new, old);
    new := new.next;
  END;

  print ("---------- ------- ---------- -----  ------------------------------------------\n");
  print (" %+9u    %+3u%%  %+9d  %+3d%%  Cnanges in %d from %d module(s)\n"
        , totalCount, Rate(totalCount, totalCov)
        , totalDiff, Rate(totalCount, totalDiff)
        , totalChanged, totalMod );

  tc.CancelTCFileOperation (verboseInfo);
END PrintInfoChanges;


--------------------------------------------------------------------------------
VAR 
  ArgValue:  String;
  i: INT;

BEGIN
  ShowHeader ();
  Option.Default ();
  MergedInfo := NIL;

  ArgNumber := sys.VAL(INT, pe.ArgNumber());
  IF ArgNumber = 0 THEN
    Option.PrintHelp ();
    RETURN;
  END;

  i := 0;
  WHILE i < ArgNumber DO
    NEW (ArgValue, pe.ArgLength(i) + 1);
    pe.GetArg (i, ArgValue^); 
    IF (ArgValue[0] = "-") THEN
      Option.ParseOption (i, ArgValue^);
    ELSE
      Option.AddFile (ArgValue^);
      ReadTCFile (ArgValue^, MergedInfo);
    END;
    INC (i);
  END; 

  IF MergedInfo = NIL THEN
    HALT(1);
  END;
  IF Option.verbose THEN
    PrintInfoChanges ();
  END;

  WriteTCFile (Option.outputFileName^, MergedInfo);
  print ('\nFiles merged into "%s"\n', Option.outputFileName^);

FINALLY
  tc.CancelTCFileOperation (MergedInfo);
END tcmerge.