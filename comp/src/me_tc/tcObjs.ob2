--------------------------------------------------------------------------------
--                        Excelsior XDS Test Coverage
--                          (c) 2002, Excelsior Ltd.
--
-- Module:       tcObjs
-- Author:       Lvov Konstantin
-- Modified By:
-- Created:      11-Dec-2002
-- Purpose:      Test Coverage Objects Operation 
--               to hide pcO and other front-end modules  
-- Comment:      
-- Synonym:      tco
--------------------------------------------------------------------------------
<* IF ~ DEFINED(TESTCOVERAGE) THEN *>  <* NEW TESTCOVERAGE- *>
<* ELSE *>                             <*- TESTCOVERAGE *>
<* END *>
MODULE tcObjs;
IMPORT  pc := pcK,       env := xiEnv,      sys := SYSTEM
     ,  pcO,             pcB,               pcF
     ,  tc := xrTCov,    xcStr
     ;

--------------------------------------------------------------------------------
TYPE
  INT = sys.INT32;

CONST
   rtsRegistryModule     = "RegistryModule";

   rtsIncreaseCounter    = "IncreaseCounter";
   rtsIncreaseCounterExt = "IncreaseCounterExt";

   rtsInitIterationLocal       = "InitIterationLocal";
   rtsIncreaseIterationCounter = "IncreaseIterationCounter";

VAR
  ObjSYSModule  * : pc.OBJECT;
  ObjRTSModule  * : pc.OBJECT;

  ObjModuleInfo * : pc.OBJECT;
  ObjModuleName * : pc.OBJECT;

  ObjSourceRefs * : pc.OBJECT;
  ObjCounters   * : pc.OBJECT;

  ModuleModifyTime * : tc.TTime;

--------------------------------------------------------------------------------
PROCEDURE FindObject (module: pc.OBJECT; name-: ARRAY OF CHAR): pc.OBJECT;
VAR obj: pc.OBJECT;
BEGIN
  obj := NIL;

  IF module = NIL THEN
    pcO.fnd_vis(pcO.cur_scope, name, obj);
  ELSE
    pcO.fnd_qua (module.type, name, obj);
  END;

  IF obj # NIL THEN
    obj.pos := env.null_pos;
    obj.end := env.null_pos;
  END;
  RETURN obj;
END FindObject;


--------------------------------------------------------------------------------
TYPE   String * = env.String;

PROCEDURE MakeUniqueName * (name: ARRAY OF CHAR): String;
VAR uname: String;
    obj: pc.OBJECT;
    cnt: INTEGER;
BEGIN
  xcStr.dprn_txt (uname, "%s", name);
  cnt := 0;
  WHILE pcO.try_vis (pcO.cur_scope, uname^, obj) DO
    xcStr.dprn_txt (uname, "%s%d", name, cnt);
    INC(cnt);
  END;
  RETURN uname;
END MakeUniqueName;


--------------------------------------------------------------------------------
--                          Create IR Objects
--------------------------------------------------------------------------------
PROCEDURE NewObj (mode: pc.OB_MODE; name-: ARRAY OF CHAR): pc.OBJECT;
VAR obj: pc.OBJECT;
BEGIN
  obj := pcO.new_obj (mode);
  pcO.set_name (obj, name);
  obj.flag := pc.flag_m2;

  obj.pos := env.null_pos;
  obj.end := env.null_pos;

  RETURN obj;
END NewObj;


--------------------------------------------------------------------------------
PROCEDURE NewType (mode: pc.TY_MODE): pc.STRUCT;
VAR type: pc.STRUCT;
BEGIN
  type := pcO.new_type (mode);
  type.pos := env.null_pos;
  RETURN type;
END NewType;


--------------------------------------------------------------------------------
PROCEDURE NewNode (mode: pc.ND_MODE): pc.NODE;
VAR node: pc.NODE;
BEGIN
  pcO.new (node, mode);
  node.pos := env.null_pos;
  RETURN node;
END NewNode;


--------------------------------------------------------------------------------
PROCEDURE NewRTSCall (name-: ARRAY OF CHAR): pc.NODE;
VAR obj: pc.OBJECT;
    proc, pcall: pc.NODE;
BEGIN
  obj := FindObject (ObjRTSModule, name);
  pcB.gen_usage (obj, proc);
  pcO.new (pcall, pc.nd_call);

  pcall.pos  := env.null_pos;
  pcall.l    := proc;
  pcall.type := proc.type.base;

  RETURN pcall;
END NewRTSCall;


--------------------------------------------------------------------------------
--                        Create Programm Objects
--------------------------------------------------------------------------------
VAR type_string: pc.STRUCT;  -- reflection from pcS

PROCEDURE NewConst_String * (name-:ARRAY OF CHAR; str-: ARRAY OF CHAR): pc.OBJECT;
VAR obj:  pc.OBJECT;
    node: pc.NODE;
    type: pc.STRUCT;
    val:  pc.VALUE;
BEGIN
  obj := NewObj (pc.ob_cons, name);

  type_string.len := LENGTH(str) + 1;
  val := pc.value.new (env.null_pos, type_string);
  val.set_string (str);

  type := NewType (pc.ty_SS);
  type.len := type_string.len;

  pcO.new (node, pc.nd_value);
  node.type := type;
  node.val  := val;
  node.pos  := env.null_pos;

  obj.val  := node;
  obj.type := node.type;

  pcO.alloc (obj);
  pcO.dcl (pcO.cur_scope, obj);
  RETURN obj;
END NewConst_String;


--------------------------------------------------------------------------------
PROCEDURE NewConst_RTSInteger (tname-: ARRAY OF CHAR; value: tc.TIndex): pc.NODE;
VAR node: pc.NODE;
    type: pc.STRUCT;
    obj:  pc.OBJECT;
    val:  pc.VALUE;
BEGIN
  obj := FindObject (ObjRTSModule, tname);
  pcB.gen_usage (obj, node);
  type := node.type;

  val := pc.value.new (env.null_pos, pc.ZZ_type);
  val.set_integer (value);

  pcO.new (node, pc.nd_value);
  node.type := pcO.ZZ_type;
  node.val  := val;
  node.pos  := env.null_pos;

  pcB.convert (node, type);

  RETURN node;
END NewConst_RTSInteger;


--------------------------------------------------------------------------------
PROCEDURE NewVariable * ( name-: ARRAY OF CHAR
                        ; tmodule: pc.OBJECT
                        ; tname-: ARRAY OF CHAR ): pc.OBJECT;
VAR obj:  pc.OBJECT;
    tobj: pc.OBJECT;
BEGIN
  obj := NewObj (pc.ob_var, name);
  obj.host := pcO.cur_scope;

  tobj := FindObject (tmodule, tname);
  obj.type := tobj.type;

  pcO.alloc (obj);
  pcO.dcl (pcO.cur_scope, obj);
  RETURN obj;
END NewVariable;


--------------------------------------------------------------------------------
PROCEDURE New_ModuleInfoPtr (): pc.NODE;
VAR node: pc.NODE;
    tnode: pc.NODE;
    ref_arg: ARRAY 1 OF pc.NODE;
    obj:  pc.OBJECT;
BEGIN
  obj := FindObject (ObjSYSModule, "REF");
  pcB.gen_usage (obj, node);

  pcB.gen_usage (ObjModuleInfo, ref_arg[0]);
  pcF.gen_system_function (node, ref_arg, 1, FALSE);

  obj := FindObject (ObjRTSModule, "TModuleInfoPtr");
  pcB.gen_usage (obj, tnode);

  pcB.cast (node, tnode.type, FALSE);

  RETURN node;
END New_ModuleInfoPtr;


--------------------------------------------------------------------------------
-- Массив.
--      mode    ty_array
--      min     -> минимальное значение индекса  <----|
--      max     -> максимальное значение индекса <--| |
--      base    -> базовый тип                      | |
--      inx     -> тип индекса                      | |
--                      max     --------------------| |
--                      min     ----------------------|
--------------------------------------------------------------------------------
PROCEDURE AddArrayType * ( name-: ARRAY OF CHAR
                         ; len: tc.TIndex
                         ; base_tname-: ARRAY OF CHAR );
VAR obj, base_obj:  pc.OBJECT;
    type: pc.STRUCT;
    min, max: pc.VALUE;
BEGIN
  obj  := NewObj (pc.ob_type, name);
  type := NewType (pc.ty_array);
  type.obj := obj;

  -- array index
  min := pc.value.new (env.null_pos, pc.ZZ_type);
  min.set_integer (0);

  max := pc.value.new (env.null_pos, pc.ZZ_type);
  max.set_integer (len-1);

  type.inx := NewType (pc.ty_range);
  type.inx.base := pcO.longcard;
  type.inx.min  := min;
  type.inx.max  := max;
  type.inx.end  := env.null_pos;

  -- array base type
  base_obj := FindObject (ObjRTSModule, base_tname);
  type.base := base_obj.type;
  type.end  := env.null_pos;

  pcO.set_type_base (type, type.base);
  type.min := type.inx.min;
  type.max := type.inx.max;
  type.len := len;

  obj.type := type;

  pcO.alloc (obj);
  pcO.dcl (pcO.cur_scope, obj);
END AddArrayType;


--------------------------------------------------------------------------------
TYPE
  TestConditionTypeNamesArray = ARRAY tc.TestConditionType OF
                                ARRAY 32 OF CHAR;

CONST
  TestConditionTypeNames = TestConditionTypeNamesArray {
    "cr_File"
  -- C1 criterion
  , "cr_C1_Procedure"
  , "cr_C1_Module"
  , "cr_C1_Finally"

  , "cr_C1_IF"
  , "cr_C1_IF_else"
  , "cr_C1_IF_elsif"
  , "cr_C1_IF_end"

  , "cr_C1_Case"
  , "cr_C1_Case_else"
  , "cr_C1_Case_end"

  , "cr_C1_OberonWith"
  , "cr_C1_OberonWith_else"
  , "cr_C1_OberonWith_end"

  , "cr_C1_While"
  , "cr_C1_While_end"

  , "cr_C1_Repeat"
  , "cr_C1_Repeat_end"

  , "cr_C1_For"
  , "cr_C1_For_end"

  , "cr_C1_Loop"
  , "cr_C1_Loop_end"

  -- Iteration criterion
  , "cr_Itr_While"
  , "cr_Itr_Repeat"
  , "cr_Itr_For"
  , "cr_Itr_Loop"
  };

--------------------------------------------------------------------------------
PROCEDURE NewConst_SourceRefs * ( name-: ARRAY OF CHAR
                            ; srcrefs-: ARRAY OF tc.TSourceRef
                            ; base_tname-: ARRAY OF CHAR ): pc.OBJECT;

    -- 1 -- NewConst_Fixups ----------------------------------------------------
    PROCEDURE NewRecordConst ( type: pc.STRUCT
                             ; index: tc.TIndex ): pc.NODE;
    VAR node: pc.NODE;
        tail: pc.NODE;
        expr: pc.NODE;
        field: pc.OBJECT;

        -- 2 -- NewRecordConst -------------------------------------------------
        PROCEDURE AddField ();
        BEGIN
          pcB.assign_compatibility (env.null_pos, field.type, expr);
          pcO.app (node.l, tail, expr);
          field := field.next;
        END AddField;

    -- 1 -- NewRecordConst -----------------------------------------------------
    VAR  obj:  pc.OBJECT;
    BEGIN
      node := NewNode (pc.nd_aggregate);
      node.type := type;

      field := type.prof;
      tail := NIL;

      -- line: TSourceLine;
      expr := NewNode (pc.nd_value);
      expr.type := pcO.ZZ_type;
      expr.val  := pc.value.new (env.null_pos, pc.ZZ_type);
      expr.val.set_integer (srcrefs[index].line);
      AddField ();

      -- col:  TSourceCol;
      expr := NewNode (pc.nd_value);
      expr.type := pcO.ZZ_type;
      expr.val  := pc.value.new (env.null_pos, pc.ZZ_type);
      expr.val.set_integer (srcrefs[index].col);
      AddField ();

      -- end_line: TSourceLine;
      expr := NewNode (pc.nd_value);
      expr.type := pcO.ZZ_type;
      expr.val  := pc.value.new (env.null_pos, pc.ZZ_type);
      expr.val.set_integer (srcrefs[index].end_line);
      AddField ();

      -- end_col:  TSourceCol;
      expr := NewNode (pc.nd_value);
      expr.type := pcO.ZZ_type;
      expr.val  := pc.value.new (env.null_pos, pc.ZZ_type);
      expr.val.set_integer (srcrefs[index].end_col);
      AddField ();

      -- type: TestCriterionType;
      obj := FindObject (ObjRTSModule, TestConditionTypeNames[srcrefs[index].type]);
      pcB.gen_usage (obj, expr);
      AddField ();

      RETURN node;
    END NewRecordConst;

-- 0 -- NewConst_Fixups --------------------------------------------------------
VAR obj, base_obj:  pc.OBJECT;
    node: pc.NODE;
    tail: pc.NODE;
    expr: pc.NODE;
    type: pc.STRUCT;
    i: INT;
BEGIN
  obj  := NewObj (pc.ob_cons, name);
  base_obj := FindObject (NIL, base_tname);
  type := base_obj.type;

  node := NewNode (pc.nd_aggregate);
  node.type := type;

  tail := NIL;
  FOR i:= 0 TO type.len-1 DO
    expr := NewRecordConst (type.base, i);
    pcB.assign_compatibility (env.null_pos, type.base, expr);
    pcO.app (node.l, tail, expr);
  END;

  obj.val  := node;
  obj.type := node.type;

  pcO.alloc (obj);
  pcO.dcl (pcO.cur_scope, obj);
  RETURN obj;
END NewConst_SourceRefs;


--------------------------------------------------------------------------------
--                    Create New RTS Procedure Calls
--------------------------------------------------------------------------------

PROCEDURE NewRTSCall_RegistryModule * (): pc.NODE;
VAR pcall:  pc.NODE;
    fparam: pc.OBJECT; -- formal procedure parameters
    ptail:  pc.NODE;   -- tail of actual parameters list

    -- 1 -- NewRTSCall_RegistryModule ------------------------------------------
    PROCEDURE AddParameter (arg: pc.NODE);
    BEGIN
      pcB.parameter_compatibility (fparam, arg);
      IF ptail = NIL THEN  pcall.r    := arg;
      ELSE                 ptail.next := arg;
      END;
      ptail  := arg;
      fparam := fparam.next;
    END AddParameter;

-- 0 -- NewRTSCall_RegistryModule ----------------------------------------------
VAR arg: pc.NODE;
BEGIN
  pcall := NewRTSCall (rtsRegistryModule);

  -- create arguments
  fparam := pcall.l.type.prof;
  ptail  := NIL;

  -- ( module: ModuleInfoPtr
  arg := New_ModuleInfoPtr();
  AddParameter (arg);

  -- ; modname: ARRAY OF CHAR
  pcB.gen_usage (ObjModuleName, arg);
  AddParameter (arg);

  -- ; srcrefs:  ARRAY OF TSourceRef 
  pcB.gen_usage (ObjSourceRefs, arg);
  AddParameter (arg);

  -- ; counters: ARRAY OF TCounter )
  pcB.gen_usage (ObjCounters, arg);
  AddParameter (arg);

  pcall.obj := pcall.l.obj;
  pcall.l   := NIL;

  RETURN pcall;
END NewRTSCall_RegistryModule;


--------------------------------------------------------------------------------
PROCEDURE NewRTSCall_InitIterationLocal * (local_counter: pc.OBJECT): pc.NODE;
VAR pcall:  pc.NODE;
    fparam: pc.OBJECT; -- formal procedure parameters
    ptail:  pc.NODE;   -- tail of actual parameters list

    -- 1 -- NewRTSCall_InitIterationLocal --------------------------------------
    PROCEDURE AddParameter (arg: pc.NODE);
    BEGIN
      pcB.parameter_compatibility (fparam, arg);
      IF ptail = NIL THEN  pcall.r    := arg;
      ELSE                 ptail.next := arg;
      END;
      ptail  := arg;
      fparam := fparam.next;
    END AddParameter;

-- 0 -- NewRTSCall_InitIterationLocal ------------------------------------------
VAR arg:    pc.NODE;
BEGIN
  pcall := NewRTSCall (rtsInitIterationLocal);

  -- create arguments
  fparam := pcall.l.type.prof;
  ptail  := NIL;

  -- (VAR local_counter: TCounter);
  pcB.gen_usage (local_counter, arg);
  AddParameter (arg);

  pcall.obj := pcall.l.obj;
  pcall.l   := NIL;

  RETURN pcall;
END NewRTSCall_InitIterationLocal;


--------------------------------------------------------------------------------
PROCEDURE NewIncompleteRTSCall_IncreaseIterationCounter * ( index: tc.TIndex
                                                          ; local_counter: pc.OBJECT
                                                          ): pc.NODE;
VAR pcall:  pc.NODE;
    fparam: pc.OBJECT; -- formal procedure parameters
    ptail:  pc.NODE;    -- tail of actual parameters list

    -- 1 -- NewRTSCall_IncreaseCounter -----------------------------------------
    PROCEDURE AddParameter (arg: pc.NODE);
    BEGIN
      pcB.parameter_compatibility (fparam, arg);
      IF ptail = NIL THEN  pcall.r    := arg;
      ELSE                 ptail.next := arg;
      END;
      ptail  := arg;
      fparam := fparam.next;
    END AddParameter;

-- 0 -- NewRTSCall_IncreaseCounter ---------------------------------------------
VAR arg: pc.NODE;
BEGIN
  pcall := NewRTSCall (rtsIncreaseIterationCounter);

  -- create arguments
  fparam := pcall.l.type.prof;
  ptail  := NIL;

  -- ( index: TIndex
  arg := NewConst_RTSInteger ("TIndex", index);
  AddParameter (arg);

  -- ; VAR local_counter: TCounter
  pcB.gen_usage (local_counter, arg);
  AddParameter (arg);

  RETURN pcall;
END NewIncompleteRTSCall_IncreaseIterationCounter;


--------------------------------------------------------------------------------
PROCEDURE NewIncompleteRTSCall_IncreaseCounter * ( index: tc.TIndex
                                                 ; heavy: BOOLEAN ): pc.NODE;
VAR pcall:  pc.NODE;
    fparam: pc.OBJECT; -- formal procedure parameters
    ptail:  pc.NODE;    -- tail of actual parameters list

    -- 1 -- NewRTSCall_IncreaseCounter -----------------------------------------
    PROCEDURE AddParameter (arg: pc.NODE);
    BEGIN
      pcB.parameter_compatibility (fparam, arg);
      IF ptail = NIL THEN  pcall.r    := arg;
      ELSE                 ptail.next := arg;
      END;
      ptail  := arg;
      fparam := fparam.next;
    END AddParameter;

-- 0 -- NewRTSCall_IncreaseCounter ---------------------------------------------
VAR arg: pc.NODE;
BEGIN
  IF heavy THEN
    pcall := NewRTSCall (rtsIncreaseCounterExt);
  ELSE
    pcall := NewRTSCall (rtsIncreaseCounter);
  END;

  -- create arguments
  fparam := pcall.l.type.prof;
  ptail  := NIL;

  -- ( index: TIndex
  arg := NewConst_RTSInteger ("TIndex", index);
  AddParameter (arg);

  RETURN pcall;
END NewIncompleteRTSCall_IncreaseCounter;


--------------------------------------------------------------------------------
PROCEDURE CompleteRTSCall * (pcall: pc.NODE; heavy: BOOLEAN);
VAR fparam: pc.OBJECT; -- formal procedure parameters
    ptail:  pc.NODE;    -- tail of actual parameters list

    -- 1 -- NewRTSCall_IncreaseCounter -----------------------------------------
    PROCEDURE AddParameter (arg: pc.NODE);
    BEGIN
      pcB.parameter_compatibility (fparam, arg);
      IF ptail = NIL THEN  pcall.r    := arg;
      ELSE                 ptail.next := arg;
      END;
      ptail  := arg;
      fparam := fparam.next;
    END AddParameter;

-- 0 -- NewRTSCall_IncreaseCounter ---------------------------------------------
VAR arg: pc.NODE;
BEGIN
  -- skip already generated arguments
  IF pcall.r = NIL THEN
    fparam := pcall.l.type.prof;
    ptail  := NIL;
  ELSE
    fparam := pcall.l.type.prof.next;
    ptail  := pcall.r;
    WHILE ptail.next # NIL DO
      ptail := ptail.next;
      fparam := fparam.next;
    END;
  END;

  -- ; VAR counters: ARRAY OF TCounter
  pcB.gen_usage (ObjCounters, arg);
  AddParameter (arg);

  IF heavy THEN
    -- ; modtime:  TTime
    arg := NewConst_RTSInteger ("TTime", ModuleModifyTime);
    AddParameter (arg);

    -- ; module: ModuleInfoPtr
    arg := New_ModuleInfoPtr();
    AddParameter (arg);

    -- ; modname: ARRAY OF CHAR
    pcB.gen_usage (ObjModuleName, arg);
    AddParameter (arg);

    -- ; srcrefs:  ARRAY OF TSourceRef  )
    pcB.gen_usage (ObjSourceRefs, arg);
    AddParameter (arg);
  END;

  pcall.obj := pcall.l.obj;
  pcall.l   := NIL;
END CompleteRTSCall;


--------------------------------------------------------------------------------
PROCEDURE Reset * ();
BEGIN
  ObjSYSModule     := NIL;
  ObjRTSModule     := NIL;

  ObjModuleInfo := NIL;
  ObjModuleName := NIL;

  ObjSourceRefs := NIL;
  ObjCounters   := NIL;

  ModuleModifyTime := 0;
END Reset;


--------------------------------------------------------------------------------
BEGIN
  -- cut and paste from pcS
  NEW(type_string);      type_string.mode := pc.ty_SS;
  NEW(type_string.base); type_string.base.mode := pc.ty_char;
  type_string.len := 0;
END tcObjs.