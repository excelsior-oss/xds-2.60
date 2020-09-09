--------------------------------------------------------------------------------
--                        Excelsior XDS Test Coverage
--                          (c) 2002, Excelsior Ltd.
--
-- Module:       tcMain
-- Author:       Lvov Konstantin
-- Modified By:
-- Created:      06-Dec-2002
-- Purpose:      Test Coverage main module of programm instrumentation.
--               Performs programm tree traversal.
-- Comment:      It's a part of XDS O2/M2 compiler 'xc'. Set option 
--               '-target_testcoverage:+' to build in this tool into compiler.
--------------------------------------------------------------------------------
<* IF ~ DEFINED(TESTCOVERAGE) THEN *>  <* NEW TESTCOVERAGE- *>
<* ELSE *>                             <*- TESTCOVERAGE *>
<* END *>
MODULE tcMain;
IMPORT  env := xiEnv,       pc := pcK,        pcO
     ,  tci := tcInstr,     tc := xrTCov
     ,  xfs := xiFiles
--     ,  io := Printf
     ;

VAR UseModuleConstructor: BOOLEAN;
    ModuleBudyExist: BOOLEAN;

--------------------------------------------------------------------------------
PROCEDURE SkipInstrumentations (pos-: pc.TPOS): BOOLEAN;
BEGIN
  RETURN NOT env.config.OptionAt (pos, "TESTCOVERAGE");
END SkipInstrumentations;


--------------------------------------------------------------------------------
--                       Statement Process
--------------------------------------------------------------------------------

PROCEDURE ^ ProcessStatement_right (stmt: pc.NODE);
PROCEDURE ^ ProcessStatement_left  (stmt: pc.NODE);

--------------------------------------------------------------------------------
TYPE
  TChild = (chLeft, chNext, chRight);

  TStatement = RECORD
    parent: pc.NODE;
    node:   pc.NODE;  -- current statement
    child:  TChild;   -- 'node' is left, right or next son of 'parent'
    icode:  pc.NODE;  -- InstrCode to insert into 'node.next'
  END;

--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Assign_next ( parent: pc.NODE
                                             ; full_mode:= FALSE: BOOLEAN );
BEGIN
  IF full_mode AND (stmt.icode # NIL) THEN
    stmt.icode.next := stmt.node.next;
    stmt.node.next  := stmt.icode;
  END;
  stmt.parent := parent;
  stmt.node   := parent.next;
  stmt.child  := chNext;
  stmt.icode  := NIL;
END Assign_next;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Assign_left (parent: pc.NODE);
BEGIN
  stmt.parent := parent;
  stmt.node   := parent.l;
  stmt.child  := chLeft;
  stmt.icode  := NIL;
END Assign_left;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Assign_right (parent: pc.NODE);
BEGIN
  stmt.parent := parent;
  stmt.node   := parent.r;
  stmt.child  := chRight;
  stmt.icode  := NIL;
END Assign_right;



--------------------------------------------------------------------------------
PROCEDURE InsertInstrCode_right (node: pc.NODE; icode: pc.NODE);
BEGIN
  IF icode = NIL THEN  RETURN END;

  icode.next := node.r;
  node.r     := icode;
END InsertInstrCode_right;


--------------------------------------------------------------------------------
PROCEDURE InsertInstrCode_left (node: pc.NODE; icode: pc.NODE);
BEGIN
  IF icode = NIL THEN  RETURN END;

  icode.next := node.l;
  node.l     := icode;
END InsertInstrCode_left;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) InsertInstrCode_left (icode: pc.NODE);
BEGIN
  IF icode = NIL THEN  RETURN END;

  InsertInstrCode_left (stmt.node, icode);
END InsertInstrCode_left;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) InsertInstrCode_right (icode: pc.NODE);
BEGIN
  IF icode = NIL THEN  RETURN END;

  InsertInstrCode_right (stmt.node, icode);
END InsertInstrCode_right;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) InsertInstrCode_top (icode: pc.NODE);
BEGIN
  IF icode = NIL THEN  RETURN END;

  ASSERT( stmt.parent # NIL );
  CASE stmt.child OF
  | chLeft:
       icode.next       := stmt.parent.l;
       stmt.parent.l    := icode;
  | chRight:
       icode.next       := stmt.parent.r;
       stmt.parent.r    := icode;
  | chNext:
       icode.next       := stmt.parent.next;
       stmt.parent.next := icode;
  END;
END InsertInstrCode_top;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) InsertInstrCode_next (icode: pc.NODE);
BEGIN
  IF icode = NIL THEN  RETURN END;

  stmt.icode := icode;
END InsertInstrCode_next;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_END ( type: tc.TestConditionType
                                                      ): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 (type, stmt.node.end, stmt.node.end);
END MakeInstrCode_C1_END;


--------------------------------------------------------------------------------
-- Оператор Oberon2 WITH
--    Представляется как
--        IF ... IS ... THEN
--        ELSIF ... IS ... THEN
--                ...
--        ELSE
--        END
--    Если ELSE-части нет, то она все равно создается - там паявляется wtrap
--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_OberonWITH (): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 ( tc.cr_C1_OberonWith
                                  , stmt.node.pos, stmt.node.r.end);
END MakeInstrCode_C1_OberonWITH;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_OberonWITH_ELSE ( end_pos-: pc.TPOS
                                                                  ): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.r.end) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 ( tc.cr_C1_OberonWith_else
                                  , stmt.node.r.end, end_pos);
END MakeInstrCode_C1_OberonWITH_ELSE;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Process_OberonWITH ();

    -- 1 -- Process_OberonWITH -------------------------------------------------
    PROCEDURE Is_Another_WITH_Branch (stmt: pc.NODE): BOOLEAN;
    BEGIN
      RETURN (stmt # NIL)           AND
             (stmt.mode = pc.nd_if) AND
             (pc.ntag_elsif_node IN stmt.tags);
    END Is_Another_WITH_Branch;

    -- 1 -- Process_OberonWITH -------------------------------------------------
    PROCEDURE Is_ELSE_Part (stmt: pc.NODE): BOOLEAN;
    BEGIN
      RETURN (stmt = NIL) OR (stmt.mode # pc.nd_wtrap);
    END Is_ELSE_Part;

-- 0 -- Process_OberonWITH -----------------------------------------------------
VAR branch: TStatement;
    icode: pc.NODE;
    end_pos: pc.TPOS;
BEGIN
  IF stmt.node = NIL THEN RETURN END;
  end_pos := stmt.node.end;

  branch := stmt;
  LOOP
    IF (branch.node.r = NIL) THEN  EXIT  END;

    -- process WITH-branch body
    icode := branch.MakeInstrCode_C1_OberonWITH ();
    ProcessStatement_left (branch.node.r);
    InsertInstrCode_left (branch.node.r, icode);

    IF Is_Another_WITH_Branch (branch.node.r.r) THEN
      -- get next branch of WITH statement
      branch.Assign_right (branch.node.r);
    ELSIF Is_ELSE_Part (branch.node.r.r) THEN
      -- process ELSE-part of WITH statement
      icode := branch.MakeInstrCode_C1_OberonWITH_ELSE (end_pos);
      ProcessStatement_right (branch.node.r);
      InsertInstrCode_right (branch.node.r, icode);
      EXIT;
    ELSE
      EXIT;
    END;
  END;

  -- process END statement
  icode := stmt.MakeInstrCode_C1_END (tc.cr_C1_OberonWith_end);
  stmt.InsertInstrCode_next (icode);
END Process_OberonWITH;


--------------------------------------------------------------------------------
-- Оператор IF
--      mode    nd_if
--      l       -> условное выражение
--      r       -> NODE
--                      mode    nd_node
--                      l       -> TRUE-часть
--                      r       -> ELSE-часть
-- В случае ELSIF ELSE-часть явялется такой же конструкцией.
--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_IF (): pc.NODE;
VAR end_pos: pc.TPOS;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  IF stmt.node.r.r = NIL THEN
    -- 'if' statement without 'elsif' and 'else' parts
    end_pos := stmt.node.end;
  ELSIF stmt.node.r.r.mode = pc.nd_if THEN
    -- 'if' statement with 'elsif' part
    end_pos := stmt.node.r.r.pos;
  ELSE
    -- 'if ... else ' statement
    end_pos := stmt.node.r.pos;
  END;
  RETURN tci.MakeTestCondition_C1 (tc.cr_C1_IF, stmt.node.pos, end_pos);
END MakeInstrCode_C1_IF;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_ELSIF (): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 (tc.cr_C1_IF_elsif, stmt.node.pos, stmt.node.end);
END MakeInstrCode_C1_ELSIF;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_IF_ELSE (): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.r.pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 (tc.cr_C1_IF_else, stmt.node.r.pos, stmt.node.r.end);
END MakeInstrCode_C1_IF_ELSE;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Process_IF ();

    -- 1 -- Process_IF ---------------------------------------------------------
    PROCEDURE Is_OberonWITH (stmt: pc.NODE): BOOLEAN;
    BEGIN
      RETURN (stmt.mode = pc.nd_if) AND
             (pc.ntag_substitute(*o2_with*) IN stmt.tags);
    END Is_OberonWITH;

    -- 1 -- Process_IF ---------------------------------------------------------
    PROCEDURE Is_ELSE_Part (stmt: pc.NODE): BOOLEAN;
    BEGIN
      RETURN (stmt.mode # pc.nd_if) OR
             NOT (pc.ntag_elsif_node IN stmt.tags);
    END Is_ELSE_Part;

-- 0 -- Process_IF -------------------------------------------------------------
VAR branch: TStatement;
    icode: pc.NODE;
BEGIN
  IF stmt.node = NIL THEN RETURN END;
  IF Is_OberonWITH (stmt.node) THEN
    stmt.Process_OberonWITH ();
    RETURN;
  END;

  -- process TRUE-part
  IF stmt.node.r.l # NIL THEN
    icode := stmt.MakeInstrCode_C1_IF ();
    ProcessStatement_left (stmt.node.r);
    InsertInstrCode_left (stmt.node.r, icode);
  END;

  -- process ELSE-part
  branch := stmt;
  LOOP
    IF (branch.node.r.r = NIL) THEN
      EXIT;
    ELSIF Is_ELSE_Part (branch.node.r.r) THEN
      -- process ELSE-part
      icode := branch.MakeInstrCode_C1_IF_ELSE ();
      ProcessStatement_right (branch.node.r);
      InsertInstrCode_right (branch.node.r, icode);
      EXIT;
    ELSE
      -- process ELSIF-part
      branch.Assign_right (branch.node.r);
      ASSERT( branch.node.mode = pc.nd_if );
      IF (branch.node.r.l # NIL) THEN
        icode := branch.MakeInstrCode_C1_ELSIF ();
        ProcessStatement_left (branch.node.r);
        InsertInstrCode_left (branch.node.r, icode);
      END;
    END;
  END;

  -- process END statement
  icode := stmt.MakeInstrCode_C1_END (tc.cr_C1_IF_end);
  stmt.InsertInstrCode_next (icode);
END Process_IF;


--------------------------------------------------------------------------------
-- WHILE
--      mode    nd_while
--      l       -> условное выражение
--      r       -> тело
--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_WHILE (): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 (tc.cr_C1_While, stmt.node.pos, stmt.node.end);
END MakeInstrCode_C1_WHILE;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_Itr_WHILE (): pc.NODE;
VAR local_counter: pc.OBJECT;
    ini_code: pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  ini_code := tci.MakeIterationLocal (tc.cr_Itr_While, local_counter);
  stmt.InsertInstrCode_top (ini_code);
  RETURN tci.MakeTestCondition_Itr (tc.cr_Itr_While, stmt.node.pos, local_counter);
END MakeInstrCode_Itr_WHILE;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Process_WHILE ();
VAR icode: pc.NODE;
    icode2: pc.NODE;
BEGIN
  IF stmt.node = NIL THEN RETURN END;

  icode  := stmt.MakeInstrCode_C1_WHILE ();
  icode2 := stmt.MakeInstrCode_Itr_WHILE ();

  ProcessStatement_right (stmt.node);

  stmt.InsertInstrCode_right (icode2);
  stmt.InsertInstrCode_right (icode);

  -- process END statement
  icode := stmt.MakeInstrCode_C1_END (tc.cr_C1_While_end);
  stmt.InsertInstrCode_next (icode);
END Process_WHILE;


--------------------------------------------------------------------------------
-- REPEAT
--      mode    nd_repeat
--      r       -> условное выражение
--      l       -> тело
--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_REPEAT (): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 (tc.cr_C1_Repeat, stmt.node.pos, stmt.node.end);
END MakeInstrCode_C1_REPEAT;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_Itr_REPEAT (): pc.NODE;
VAR local_counter: pc.OBJECT;
    ini_code: pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  ini_code := tci.MakeIterationLocal (tc.cr_Itr_Repeat, local_counter);
  stmt.InsertInstrCode_top (ini_code);
  RETURN tci.MakeTestCondition_Itr (tc.cr_Itr_Repeat, stmt.node.pos, local_counter);
END MakeInstrCode_Itr_REPEAT;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Process_REPEAT ();
VAR icode: pc.NODE;
    icode2: pc.NODE;
BEGIN
  IF stmt.node = NIL THEN RETURN END;

  icode  := stmt.MakeInstrCode_C1_REPEAT ();
  icode2 := stmt.MakeInstrCode_Itr_REPEAT ();

  ProcessStatement_left (stmt.node);

  stmt.InsertInstrCode_left (icode2);
  stmt.InsertInstrCode_left (icode);

  -- process END statement
  icode := stmt.MakeInstrCode_C1_END (tc.cr_C1_Repeat_end);
  stmt.InsertInstrCode_next (icode);
END Process_REPEAT;


--------------------------------------------------------------------------------
-- LOOP
--      mode    nd_loop
--      r       -> тело
--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_LOOP (): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 (tc.cr_C1_Loop, stmt.node.pos, stmt.node.end);
END MakeInstrCode_C1_LOOP;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_Itr_LOOP (): pc.NODE;
VAR local_counter: pc.OBJECT;
    ini_code: pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  ini_code := tci.MakeIterationLocal (tc.cr_Itr_Loop, local_counter);
  stmt.InsertInstrCode_top (ini_code);
  RETURN tci.MakeTestCondition_Itr (tc.cr_Itr_Loop, stmt.node.pos, local_counter);
END MakeInstrCode_Itr_LOOP;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Process_LOOP ();
VAR icode: pc.NODE;
    icode2: pc.NODE;
BEGIN
  IF stmt.node = NIL THEN RETURN END;

  icode  := stmt.MakeInstrCode_C1_LOOP ();
  icode2 := stmt.MakeInstrCode_Itr_LOOP ();

  ProcessStatement_right (stmt.node);

  stmt.InsertInstrCode_right (icode2);
  stmt.InsertInstrCode_right (icode);

  -- process END statement
  icode := stmt.MakeInstrCode_C1_END (tc.cr_C1_Loop_end);
  stmt.InsertInstrCode_next (icode);
END Process_LOOP;


--------------------------------------------------------------------------------
-- FOR
--      mode    nd_for
--      obj     -> переменная цикла
--      l       -> NODE
--                      mode    nd_node
--                      l       -> 'from' expression
--                      r       -> 'to'   expression
--                      val     -> 'by'   value
--      r       -> тело
--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_FOR (): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 (tc.cr_C1_For, stmt.node.pos, stmt.node.end);
END MakeInstrCode_C1_FOR;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_Itr_FOR (): pc.NODE;
VAR local_counter: pc.OBJECT;
    ini_code: pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  ini_code := tci.MakeIterationLocal (tc.cr_Itr_For, local_counter);
  stmt.InsertInstrCode_top (ini_code);
  RETURN tci.MakeTestCondition_Itr (tc.cr_Itr_For, stmt.node.pos, local_counter);
END MakeInstrCode_Itr_FOR;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Process_FOR ();
VAR icode: pc.NODE;
    icode2: pc.NODE;
BEGIN
  IF stmt.node = NIL THEN RETURN END;

  icode  := stmt.MakeInstrCode_C1_FOR ();
  icode2 := stmt.MakeInstrCode_Itr_FOR ();

  ProcessStatement_right (stmt.node);

  stmt.InsertInstrCode_right (icode2);
  stmt.InsertInstrCode_right (icode);

  -- process END statement
  icode := stmt.MakeInstrCode_C1_END (tc.cr_C1_For_end);
  stmt.InsertInstrCode_next (icode);
END Process_FOR;


--------------------------------------------------------------------------------
-- CASE
--      mode    nd_case
--      l       -> выражение-селектор
--      r       -> NODE
--                      mode    nd_casedo (nd_caselse, если есть ELSE-часть)
--                      r       -> ELSE-часть (если есть)
--                      next    -> NODE
--                              До 3.0 было так
--                                      mode    nd_pair
--                                      val     -> minimum
--                                      l       -> NODE
--                                                      mode    nd_value
--                                                      val     -> maximum
--                              В 3.0 стало так
--                                      mode    nd_pair
--                                      l       -> NODE
--                                                      mode    nd_value
--                                                      val     -> minimum
--                                      r       -> NODE
--                                                      mode    nd_value
--                                                      val     -> maximum
--                      l       -> список вариантов
--                              Узел списка.
--                                      mode    nd_node
--                                      next    -> следующий
--                                      r       -> тело
--                                      l       -> список диапазонов
--                                              Узел списка.
--                                                      mode    nd_pair
--                                                      type    -> тип селектора
--                                                      До 3.0 было так
--                                                      val     -> minimum
--                                                      l       -> NODE
--                                                                      mode    nd_value
--                                                                      val     -> maximum
--                                                                      type    -> тип селектора
--                                                      В 3.0 стало так
--                                                      l       -> minimum (выражение)
--                                                      r       -> maximum (выражение)
--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_CASE (): pc.NODE;
BEGIN
  IF SkipInstrumentations(stmt.node.pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 (tc.cr_C1_Case, stmt.node.pos, stmt.node.end);
END MakeInstrCode_C1_CASE;

--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) MakeInstrCode_C1_CASE_ELSE (): pc.NODE;
VAR pos: pc.TPOS;
    case_branch: pc.NODE;
BEGIN
  IF stmt.node.l # NIL THEN
    case_branch := stmt.node.l;
    WHILE case_branch.next # NIL DO
      case_branch := case_branch.next;
    END;
    pos := case_branch.end;
  ELSE
    pos := stmt.node.pos;
  END;

  IF SkipInstrumentations(pos) THEN RETURN NIL END;

  RETURN tci.MakeTestCondition_C1 (tc.cr_C1_Case_else, pos, stmt.node.end);
END MakeInstrCode_C1_CASE_ELSE;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Process_CASE ();
VAR icode: pc.NODE;

    -- 1 -- Process_CASE -------------------------------------------------------
    PROCEDURE Process_CASE_BRANCH (branch-: TStatement);
    BEGIN
      icode := branch.MakeInstrCode_C1_CASE ();
      ProcessStatement_right (branch.node);
      branch.InsertInstrCode_right (icode);
    END Process_CASE_BRANCH;

-- 0 -- Process_CASE -----------------------------------------------------------
VAR branch: TStatement;
BEGIN
  IF stmt.node = NIL THEN RETURN END;

  -- process CASE-branches
  branch.Assign_left (stmt.node.r);
  WHILE branch.node # NIL DO
    Process_CASE_BRANCH (branch);
    branch.Assign_next (branch.node);
  END;

  -- process ELSE-part
  branch.Assign_right (stmt.node);
  IF (stmt.node.r.mode = pc.nd_caselse) THEN
    icode := branch.MakeInstrCode_C1_CASE_ELSE ();
    ProcessStatement_right (branch.node);
    branch.InsertInstrCode_right (icode);
  END;

  -- process END statement
  icode := stmt.MakeInstrCode_C1_END (tc.cr_C1_Case_end);
  stmt.InsertInstrCode_next (icode);
END Process_CASE;


--------------------------------------------------------------------------------
-- EXCEPT блок.
--      mode    nd_except
--      type    -> void
--      next    -> следующий
--      obj     -> OBJECT
--                      mode    ob_var
--                      name    "trap"
--                      type    -> pcO.process
--      r       -> список операторов EXCEPT части
--      l       -> операторы BEGIN или FINALLY части (см. Блок операторов)
--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) Process_EXCEPT ();
BEGIN
  IF stmt.node = NIL THEN RETURN END;

  ProcessStatement_left  (stmt.node);
  ProcessStatement_right (stmt.node);
END Process_EXCEPT;



--------------------------------------------------------------------------------
--                       Statement Traversal
--------------------------------------------------------------------------------

PROCEDURE (VAR stmt: TStatement) Process ();
BEGIN
  IF stmt.node = NIL THEN RETURN END;

  CASE stmt.node.mode OF
  | pc.nd_if:      stmt.Process_IF     ();
  | pc.nd_while:   stmt.Process_WHILE  ();
  | pc.nd_repeat:  stmt.Process_REPEAT ();
  | pc.nd_loop:    stmt.Process_LOOP   ();
  | pc.nd_for:     stmt.Process_FOR    ();
  | pc.nd_case:    stmt.Process_CASE   ();
  | pc.nd_except:  stmt.Process_EXCEPT ();
  | pc.nd_with,
    pc.nd_block:   ProcessStatement_right (stmt.node);
  | pc.nd_protect: ProcessStatement_left  (stmt.node);
  ELSE
  END;
END Process;


--------------------------------------------------------------------------------
PROCEDURE (VAR stmt: TStatement) ProcessList ();
BEGIN
  WHILE stmt.node # NIL DO
    stmt.Process ();
    stmt.Assign_next (stmt.node, TRUE (* process 'stmt.icode' *));
  END;
END ProcessList;


--------------------------------------------------------------------------------
PROCEDURE ProcessStatement_right (node: pc.NODE);
VAR stmt: TStatement;
BEGIN
  IF node = NIL THEN RETURN END;

  stmt.Assign_right (node);
  stmt.ProcessList ();
END ProcessStatement_right;


--------------------------------------------------------------------------------
PROCEDURE  ProcessStatement_left  (node: pc.NODE);
VAR stmt: TStatement;
BEGIN
  IF node = NIL THEN RETURN END;

  stmt.Assign_left (node);
  stmt.ProcessList ();
END ProcessStatement_left;


--------------------------------------------------------------------------------
--                       Procedure Traversal
--------------------------------------------------------------------------------

PROCEDURE ^ ProcessProc_list (procs: pc.NODE);


--------------------------------------------------------------------------------
-- 0) Блок операторов (представляет BEGIN или FINALLY части).
--    a) если в списке операторов есть RETURN, ASSERT или HALT
--         mode    nd_block
--         type    -> void
--         next    -> следующий
--         r       -> список операторов
--    b) если их нет, то это просто список операторов (тот, что висит на ссылке r)
--
-- 1) EXCEPT блок.
--      mode    nd_except
--         type    -> void
--         next    -> следующий
--         obj     -> OBJECT
--                         mode    ob_var
--                         name    "trap"
--                         type    -> pcO.process
--      r       -> список операторов EXCEPT части
--      l       -> операторы BEGIN или FINALLY части (см. Блок операторов)
--
-- 2) PROTECT блок.
--         mode    nd_protect
--         type    -> void
--         next    -> следующий
--         l       -> на ЕХCEPT блок или блок операторов
--         r       -> NODE
--                         mode    nd_value
--                         type    -> pcO.protection
--                         val     -> protection number
--         obj     -> OBJECT
--                         mode    ob_var
--                         type    -> pcO.protection
--------------------------------------------------------------------------------
PROCEDURE InsertInstrCode_BeginProc (proc: pc.NODE; icode: pc.NODE);
CONST ProcBlocks = pc.NODE_SET {pc.nd_block, pc.nd_except, pc.nd_protect};
BEGIN
  IF (proc.r # NIL) AND (proc.r.mode IN ProcBlocks) THEN
    proc := proc.r;
  END;

  IF (proc.mode = pc.nd_except) OR (proc.mode = pc.nd_protect) THEN
    IF (proc.l # NIL) AND (proc.l.mode = pc.nd_block) THEN
      InsertInstrCode_right (proc.l, icode);
    ELSE
      InsertInstrCode_left (proc, icode);
    END;
  ELSE
    InsertInstrCode_right (proc, icode);
  END;
END InsertInstrCode_BeginProc;


--------------------------------------------------------------------------------
-- Процедура.                  <-------------------+---------------------------|
--      mode    ob_?proc                           |                           |
--      type    -> на STRUCT                       |          <--------------| |
--                      mode    ty_proctype        |                         | |
--                      obj     -------------------|                         | |
--                      len     если метод, то его номер                     | |
--                      base    -> тип возвращаемого значения                | |
--                      prof    -> список параметров (см. Процедурный тип),  | |
--                                 для метода первый параметр - this         | |
--                      mem     -> локалы                                    | |
--                      inx     -> для метода ссылается на такой же STRUCT   | |
--                                 от перекрытого метода                     | |
--                                                                           | |
--      val     -> на NODE                                                   | |   
--                      mode    nd_proc                                      | |   
--                      type    ---------------------------------------------| |
--                      obj     -----------------------------------------------|
--                      l       -> список локальных процедур 
--                                 (точно таких же NODE)
--                      r       -> тело - разного вида блоки (Блок операторов,
--                                 EXCEPT блок, PROTECT блок) провязаные по next,
--                                 причем в конце списка висит NODE
--                                      mode    nd_reraise
--                                      r       -> указвает на EXCEPT блок, если
--                                                 же его нет, то = NIL
--
--------------------------------------------------------------------------------
PROCEDURE MakeInstrCode_ProcedureEntry ( proc: pc.NODE
                                       ; UseHeavyRTSCall: BOOLEAN): pc.NODE;
VAR type: tc.TestConditionType;
BEGIN
  IF SkipInstrumentations(proc.obj.pos) THEN RETURN NIL END;

  IF (proc.obj.name^ = "FINALLY") THEN   type := tc.cr_C1_Finally;
  ELSE                                   type := tc.cr_C1_Procedure
  END;
  RETURN tci.MakeTestCondition_C1 (type, proc.obj.pos, proc.obj.end, UseHeavyRTSCall);
END MakeInstrCode_ProcedureEntry;


--------------------------------------------------------------------------------
PROCEDURE ProcessProc (proc: pc.NODE);

    -- 1 -- ProcessProc --------------------------------------------------------
    PROCEDURE Is_Nested_Proc (): BOOLEAN;
    BEGIN
      RETURN proc.obj.lev > 0;
    END Is_Nested_Proc;

-- 0 -- ProcessProc ------------------------------------------------------------
VAR useHeavyRTSCall: BOOLEAN;
    icode: pc.NODE;
BEGIN
  IF (proc = NIL) THEN  RETURN  END;
  
  pcO.enter_scope (proc.type);

  -- process local procedures
  ProcessProc_list(proc.l);

  -- process procedure body
  useHeavyRTSCall := NOT Is_Nested_Proc() AND
                     (pc.otag_public IN proc.obj.tags) AND
                     ( NOT UseModuleConstructor  OR
                       NOT (proc.type.flag IN pc.LangsWithModuleConstructors) OR
                       NOT ModuleBudyExist
                     );
  icode := MakeInstrCode_ProcedureEntry (proc, useHeavyRTSCall);

  ProcessStatement_right (proc);

  InsertInstrCode_BeginProc (proc, icode);

  pcO.exit_scope();
END ProcessProc;


--------------------------------------------------------------------------------
PROCEDURE ProcessProc_list (procs: pc.NODE);
BEGIN
  WHILE procs # NIL DO
    ProcessProc(procs);
    procs := procs.next;
  END;
END ProcessProc_list;


--------------------------------------------------------------------------------
--                          Module Traversal
--------------------------------------------------------------------------------

PROCEDURE MakeInstrCode_ModuleEntry (module: pc.NODE): pc.NODE;
BEGIN
  -- module must registred in run-time system
  RETURN tci.MakeTestCondition_C1 ( tc.cr_C1_Module, module.pos, module.end
                                  , TRUE (*UseHeavyRTSCall*) );
END MakeInstrCode_ModuleEntry;


--------------------------------------------------------------------------------
PROCEDURE ProcessModuleBody (module: pc.NODE);
VAR icode: pc.NODE;
BEGIN
   IF NOT UseModuleConstructor THEN  RETURN END;

  icode := MakeInstrCode_ModuleEntry (module);

  -- process module body
  ProcessStatement_right (module);

  InsertInstrCode_BeginProc (module, icode);
END ProcessModuleBody;


--------------------------------------------------------------------------------
PROCEDURE BeginTraversal ();
VAR time: tci.TTime;
    done: BOOLEAN;
BEGIN
  xfs.sys.ModifyTime (env.info.file^, time, done);
  ASSERT( done );
  tci.ModuleModifyTime (time);

  tci.AddGlobalVar_ModuleInfo ();
END BeginTraversal;


--------------------------------------------------------------------------------
PROCEDURE EndTraversal ();
BEGIN
  IF tci.TestCount > 0 THEN
    tci.AddGlobalConst_ModuleName (env.info.file^);
    tci.AddGlobalSourceRefsAndCounters ();
    tci.FinalizeRTSCalls ();
  END;
END EndTraversal;


--------------------------------------------------------------------------------
PROCEDURE ProcessModule * (cu: pc.OBJECT);
BEGIN
  IF NOT env.config.Option("TESTCOVERAGE") OR pcO.def THEN
    RETURN;
  END;
  UseModuleConstructor := (cu.flag IN pc.LangsWithModuleConstructors);
  ModuleBudyExist := cu.val.r # NIL;

  tci.Reset();
  tci.ExtendModuleImport (cu);

  pcO.enter_scope (cu.type);
  BeginTraversal ();

  ProcessProc_list (cu.val.l);
  ProcessModuleBody (cu.val);

  EndTraversal ();
  pcO.exit_scope();

END ProcessModule;


END tcMain.