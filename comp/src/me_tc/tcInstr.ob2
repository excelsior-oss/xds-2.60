--------------------------------------------------------------------------------
--                        Excelsior XDS Test Coverage
--                          (c) 2002, Excelsior Ltd.
--
-- Module:       tcInstr
-- Author:       Lvov Konstantin
-- Modified By:
-- Created:      11-Dec-2002
-- Purpose:      Test Coverage Instrumentation  
-- Comment:      
-- Synonym:      tci
--------------------------------------------------------------------------------
<* IF ~ DEFINED(TESTCOVERAGE) THEN *>  <* NEW TESTCOVERAGE- *>
<* ELSE *>                             <*- TESTCOVERAGE *>
<* END *>
MODULE tcInstr;
IMPORT  env := xiEnv,       pc := pcK
     ,  tco := tcObjs,      tc := xrTCov
     ,  sys := SYSTEM
     ,  pcO
     ;

--------------------------------------------------------------------------------
TYPE
  INT = sys.INT32;
  TSourceRefs   = POINTER TO ARRAY OF tc.TSourceRef;

  TRTSCall = RECORD
    pcall: pc.NODE;
    heavy: BOOLEAN;
  END;
  TRTSCalls = POINTER TO ARRAY OF TRTSCall;


VAR
  -- test condition counters number
  TestCount - : tc.TCounter;
  -- test condition source code reference
  SourceRefs: TSourceRefs;

  RTSCallCount - : tc.TCounter;
  RTSCalls       : TRTSCalls;


--------------------------------------------------------------------------------
PROCEDURE MakeTestCondition ( type: tc.TestConditionType
                            ; pos-, end_pos-: pc.TPOS ): tc.TIndex;

    -- 1 -- MakeTestCondition --------------------------------------------------
    PROCEDURE  AdjustSourceRefsStorage ();
    VAR tmp: TSourceRefs;
        len, i: INT;
    BEGIN
      IF SourceRefs = NIL THEN
        NEW(SourceRefs, 5000);
      ELSIF TestCount >= LEN(SourceRefs^) THEN
        tmp := SourceRefs;
        len := LEN(tmp^);
        NEW(SourceRefs, len*2);
        FOR i := 0 TO len-1 DO
          SourceRefs[i] := tmp[i];
        END;
      END;
    END AdjustSourceRefsStorage;


-- 0 -- MakeTestCondition ------------------------------------------------------
VAR fname: env.String;
    line, end_line: tc.TSourceLine;
    col, end_col: tc.TSourceCol;
    index: tc.TIndex;
BEGIN
  pos.unpack (fname, line, col);
  end_pos.unpack (fname, end_line, end_col);

  AdjustSourceRefsStorage ();
  index := TestCount;
  INC(TestCount);

  SourceRefs[index].line     := line+1;
  SourceRefs[index].col      := col+1;
  SourceRefs[index].end_line := end_line+1;
  SourceRefs[index].end_col  := end_col+1;
  SourceRefs[index].type     := type;

  RETURN index;
END MakeTestCondition;



--------------------------------------------------------------------------------
PROCEDURE SaveIncompleteRTSCall ( pcall: pc.NODE
                                ; UseHeavyRTSCall: BOOLEAN );

    -- 1 --  SaveIncompleteRTSCall ---------------------------------------------
    PROCEDURE  AdjustRTSCallsStorage ();
    VAR tmp: TRTSCalls;
        len, i: INT;
    BEGIN
      IF RTSCalls = NIL THEN
        NEW(RTSCalls, 5000);
      ELSIF RTSCallCount >= LEN(RTSCalls^) THEN
        tmp := RTSCalls;
        len := LEN(tmp^);
        NEW(RTSCalls, len*2);
        FOR i := 0 TO len-1 DO
          RTSCalls[i] := tmp[i];
        END;
      END;
    END AdjustRTSCallsStorage;


-- 0 --  SaveIncompleteRTSCall -------------------------------------------------
BEGIN
  IF pcall = NIL THEN RETURN END;

  AdjustRTSCallsStorage ();
  RTSCalls[RTSCallCount].pcall := pcall;
  RTSCalls[RTSCallCount].heavy := UseHeavyRTSCall;
  INC(RTSCallCount);
END SaveIncompleteRTSCall;


--------------------------------------------------------------------------------
PROCEDURE FinalizeRTSCalls * ();
VAR i: INT;
BEGIN
  IF RTSCalls # NIL THEN
    FOR i := 0 TO RTSCallCount-1 DO
      tco.CompleteRTSCall ( RTSCalls[i].pcall
                          , RTSCalls[i].heavy );
    END;
  END;
END FinalizeRTSCalls;


--------------------------------------------------------------------------------
--                 Statement Instrumentation for C1 criterion
--               начало последовательности некоторых операторов
--------------------------------------------------------------------------------

PROCEDURE MakeTestCondition_C1 * ( type: tc.TestConditionType
                                 ; pos-, end_pos-: pc.TPOS
                                 ; UseHeavyRTSCall:=FALSE: BOOLEAN ): pc.NODE;
VAR pcall: pc.NODE;
    index: tc.TIndex;
BEGIN
  index := MakeTestCondition (type, pos, end_pos);
  pcall := tco.NewIncompleteRTSCall_IncreaseCounter (index, UseHeavyRTSCall);
  SaveIncompleteRTSCall (pcall, UseHeavyRTSCall);
  RETURN pcall;
END MakeTestCondition_C1;


--------------------------------------------------------------------------------
--             Statement Instrumentation for Iteration criterion
--   покрытие циклов (максимально количество последовательных итераций цикла)
--------------------------------------------------------------------------------

PROCEDURE MakeIterationLocal * ( type: tc.TestConditionType
                               ; VAR local_counter: pc.OBJECT
                               ): pc.NODE;
VAR obj_name: tco.String;
    pcall: pc.NODE;
BEGIN
  -- create iteration local counter
  CASE type OF
  | tc.cr_Itr_While:
      obj_name := tco.MakeUniqueName ("X2C_TC_itr_while");
  | tc.cr_Itr_Repeat:
      obj_name := tco.MakeUniqueName ("X2C_TC_itr_repeat");
  | tc.cr_Itr_For:
      obj_name := tco.MakeUniqueName ("X2C_TC_itr_for");
  | tc.cr_Itr_Loop:
      obj_name := tco.MakeUniqueName ("X2C_TC_itr_loop");
  END;
  local_counter := tco.NewVariable ( obj_name^
                                   , tco.ObjRTSModule
                                   , "TCounter" );

  -- create initialization of iteration local counter
  pcall := tco.NewRTSCall_InitIterationLocal (local_counter);
  RETURN pcall;
END MakeIterationLocal;


--------------------------------------------------------------------------------
PROCEDURE MakeTestCondition_Itr * ( type: tc.TestConditionType
                                  ; pos-: pc.TPOS
                                  ; local_counter: pc.OBJECT ): pc.NODE;
VAR pcall: pc.NODE;
    index: tc.TIndex;
BEGIN
  index := MakeTestCondition (type, pos, pos);
  pcall := tco.NewIncompleteRTSCall_IncreaseIterationCounter (index, local_counter);
  SaveIncompleteRTSCall (pcall, FALSE (*UseHeavyRTSCall*));
  RETURN pcall;
END MakeTestCondition_Itr;


--------------------------------------------------------------------------------
--                         Module Registration
--------------------------------------------------------------------------------

PROCEDURE AddModuleRegistration * (module: pc.NODE);
VAR rts_call: pc.NODE;
BEGIN
  rts_call := tco.NewRTSCall_RegistryModule ();
  rts_call.next := module.r;
  module.r := rts_call;
END AddModuleRegistration;


--------------------------------------------------------------------------------
--                 Add Global Constants and Variables
--------------------------------------------------------------------------------

TYPE   TTime * = tc.TTime;

--------------------------------------------------------------------------------
PROCEDURE ModuleModifyTime * (time: TTime);
BEGIN
  tco.ModuleModifyTime := time;
END ModuleModifyTime;


--------------------------------------------------------------------------------
PROCEDURE AddGlobalConst_ModuleName * (fname-: ARRAY OF CHAR);
VAR obj_name: tco.String;
BEGIN
  obj_name := tco.MakeUniqueName ("X2C_TC_ModName");
  tco.ObjModuleName := tco.NewConst_String (obj_name^, fname);
END AddGlobalConst_ModuleName;


--------------------------------------------------------------------------------
PROCEDURE AddGlobalVar_ModuleInfo * ();
VAR obj_name: tco.String;
BEGIN
  obj_name := tco.MakeUniqueName ("X2C_TC_ModuleInfo");
  tco.ObjModuleInfo := tco.NewVariable ( obj_name^
                                      , tco.ObjRTSModule
                                      , "TModuleInfo" );
END AddGlobalVar_ModuleInfo;


--------------------------------------------------------------------------------
PROCEDURE AddGlobalSourceRefsAndCounters * ();
VAR obj_name, type_name: tco.String;
BEGIN
  -- create Test Condition Counters
  type_name := tco.MakeUniqueName ("X2C_TC_TCounters");
  tco.AddArrayType (type_name^, TestCount, "TCounter");

  obj_name := tco.MakeUniqueName ("X2C_TC_Counters");
  tco.ObjCounters := tco.NewVariable (obj_name^, NIL, type_name^);


  -- create Source Code Refrences
  type_name := tco.MakeUniqueName ("X2C_TC_TSourceRefs");
  tco.AddArrayType (type_name^, TestCount, "TSourceRef");

  obj_name := tco.MakeUniqueName ("X2C_TC_SourceRefs");
  tco.ObjSourceRefs := tco.NewConst_SourceRefs (obj_name^, SourceRefs^, type_name^);
END AddGlobalSourceRefsAndCounters;


--------------------------------------------------------------------------------
--                     Extend Module Import Section
--------------------------------------------------------------------------------

PROCEDURE ImportModule (cu: pc.OBJECT; modname-: ARRAY OF CHAR): pc.OBJECT;
VAR new_mod: pc.OBJECT;
    import: pc.USAGE;
BEGIN
  pcO.inp_sym_file (modname, FALSE, FALSE, new_mod);
  IF new_mod.mno < pc.ZEROMno THEN RETURN NIL END;

  -- check module in import list
  import := cu.type.use;
  WHILE import # NIL DO
    IF import.obj = new_mod THEN RETURN new_mod END;
    import := import.next;
  END;

  -- add module in import list
  NEW(import);
  import.obj  := new_mod;
  import.tags := pc.UTAG_SET{};
  import.next := cu.type.use;
  cu.type.use := import;

  pcO.dcl_ref (cu.type, new_mod);
  RETURN new_mod;
END ImportModule;


--------------------------------------------------------------------------------
PROCEDURE ExtendModuleImport * (cu: pc.OBJECT);
BEGIN
  tco.ObjRTSModule := ImportModule (cu, "xrTCov");
  pcO.inp_sym_file ("SYSTEM", FALSE, FALSE, tco.ObjSYSModule);
END ExtendModuleImport;


--------------------------------------------------------------------------------
--
--------------------------------------------------------------------------------

PROCEDURE Reset * ();
VAR i: INT;
BEGIN
  tco.Reset ();

  TestCount := 0;

  IF RTSCalls # NIL THEN
    FOR i := 0 TO RTSCallCount-1 DO
      RTSCalls[i].pcall := NIL;
    END;
  END;
  RTSCallCount := 0;
END Reset;


--------------------------------------------------------------------------------
BEGIN
  SourceRefs := NIL;
  RTSCalls   := NIL;
END tcInstr.