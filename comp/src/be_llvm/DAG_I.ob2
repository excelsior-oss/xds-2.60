<* IF ~TARGET_PPC AND ~TARGET_LLVM THEN *> This module can be used only in TARGET_PPC <* END *>
<* +o2addkwd *>
MODULE DAG_I;

FROM   SYSTEM IMPORT PRED,  SUCC;

IMPORT sys := SYSTEM;
IMPORT  pc := pcK;
IMPORT pcO; 

IMPORT  ir, DAG, Color;
IMPORT  at := opAttrs;
IMPORT  RD := RDefs;
IMPORT def := opDef;
IMPORT prc := opProcs;

IMPORT abi := ABI;
IMPORT RDD := RDefs_D;
IMPORT tps := beTypes;
(* !!!Hady2022 IMPORT Emit := EmitPPC; *)

IMPORT nts := BurgNT;
IMPORT bts := BurgTables;

--------------------------------------------------------------------------------
TYPE
  DAG_PPC_IDB = POINTER TO RECORD(DAG.DAG_IDB_Rec) END;


--------------------------------------------------------------------------------
PROCEDURE (idb: DAG_PPC_IDB) MakeLeaf(t: ir.ParamPtr): RD.DAGNODE;
VAR w: RD.DAGNODE;
BEGIN
  w := idb.MakeLeaf^(t);
  w.dreg  := t.dreg;
  w.align := 0;
  RETURN w;
END MakeLeaf;

--------------------------------------------------------------------------------
PROCEDURE (idb: DAG_PPC_IDB) InitNode(p: ir.TriadePtr): RD.DAGNODE;
VAR q : RD.DAGNODE;
BEGIN
  q := idb.InitNode^(p);
  q.dreg  := RDD.UNDEF_REG;
  q.align := 0;
  RETURN q;
END InitNode;

--------------------------------------------------------------------------------
PROCEDURE (idb: DAG_PPC_IDB) CalcArity(p: ir.TriadePtr): ir.INT;
BEGIN
  IF p.Params = NIL THEN
    RETURN 0;
  ELSIF p.Op = ir.o_putpar THEN
    RETURN 1;
  ELSE
    RETURN idb.CalcArity^(p);
  END;
END CalcArity;

--------------------------------------------------------------------------------
PROCEDURE (idb: DAG_PPC_IDB) NeedCallDagging(): BOOLEAN;
BEGIN
  RETURN TRUE;
END NeedCallDagging;

--------------------------------------------------------------------------------
PROCEDURE (idb: DAG_PPC_IDB) SetInMem(i: ir.VarNum);
BEGIN
  RD.Loc[i].tag := nts.NTlocal;
  IF i < Color.NNonTempVars THEN
    RD.Loc[i].mem  := Color.Allocation[i].Location;
    RD.Loc[i].offs := Color.Allocation[i].Offset;
  END;
END SetInMem;

--------------------------------------------------------------------------------
PROCEDURE (idb: DAG_PPC_IDB) InitLocations();
VAR i: ir.VarNum;
    s: ir.SizeType;
    l: ir.Local;
BEGIN
  IF ir.NVars # ir.ZEROVarNum THEN
    NEW(RD.Loc, ir.NVars);
    FOR i := ir.ZEROVarNum TO PRED(Color.NNonTempVars) DO
      RD.Loc[i].temp := FALSE;
      RD.Loc[i].tag := nts.NTreg;
      RD.Loc[i].reg := RDD.UNDEF_REG;
    END;

    -- сразу создаем локалы для всех переменных, которым локалы еще
    -- не сопоставлены
    FOR i := Color.NNonTempVars TO PRED(ir.NVars) DO
      RD.Loc[i].temp := FALSE;
      RD.Loc[i].tag := nts.NTreg;
      RD.Loc[i].reg := RDD.UNDEF_REG;
      s := ir.Vars[i].Def.ResSize;
      l := ir.AddLocal (NIL, ir.TmpScope, def.get_align_by_size(s));
      ir.Locals[l].VarSize := VAL (LONGINT, s);
      ir.Locals[l].VarType := ir.Vars[i].Def.ResType;
      RD.Loc[i].mem  := l;
      RD.Loc[i].offs := 0;
    END;
  ELSE
    RD.Loc := NIL;
  END;
END InitLocations;

--------------------------------------------------------------------------------
-- Еще до начала первой итерации разместить часть переменных в памяти
PROCEDURE (idb: DAG_PPC_IDB) SetLocations();
VAR i: ir.VarNum;
BEGIN
  FOR i := ir.ZEROVarNum TO PRED(ir.NVars) DO
    IF ir.o_ForCtr IN ir.Vars[i].Options THEN
      RD.Loc[i].tag := nts.NTctr;
      RD.Loc[i].reg := RDD.UNDEF_REG;
    ELSIF NOT RD.Loc[i].temp THEN
      IF (Color.CalcRegProfit(i) >= 0)
        & NOT (at.NoRegVars IN at.COMP_MODE)
      THEN
        RD.Loc[i].tag := nts.NTreg;
        RD.Loc[i].reg := RDD.UNDEF_REG;
      ELSE
        idb.SetInMem(i);
      END;
    ELSE
      RD.Loc[i].tag := nts.NTreg;
      RD.Loc[i].reg := RDD.UNDEF_REG;
    END;
  END;
END SetLocations;

--------------------------------------------------------------------------------
-- Перед началом итерации очистить разметку регистров; кроме того,
-- сбросить в память выгруженные на предыдущем шаге переменные
-- (а для вытесненных байтами - установить, что регистры для них
-- надо выбирать, начиная со словных)
PROCEDURE (idb: DAG_PPC_IDB) ClearLocations(last_iteration: BOOLEAN);
BEGIN
  ASSERT(FALSE);
(* !!!Hady2022 
VAR i: ir.VarNum;
    r: Emit.RegNum;
BEGIN
  FOR i := ir.ZEROVarNum TO PRED(ir.NVars) DO
    IF RD.Trees[i] # NIL THEN
      RD.Trees[i].place.InitVCReg();
      RD.Trees[i].a.base.InitVCReg();
      RD.Trees[i].a.index.InitVCReg();
      RD.Trees[i].fa.base.InitVCReg();
      RD.Trees[i].fa.index.InitVCReg();
    END;
    IF RD.Loc[i].tag = nts.NTctr THEN
    ELSIF RD.Loc[i].tag # nts.NTlocal THEN
      r := RD.Loc[i].reg;
      RD.Loc[i].reg := RDD.UNDEF_REG;
      IF (RD.Loc[i].tag = nts.NTreg) & (r = RDD.SPILLED_REG) THEN
        idb.SetInMem(i);
      ELSIF (RD.Loc[i].tag = nts.NTmem) & (r = RDD.SPILLED_REG) THEN
        idb.SetInMem(i);
      ELSE
        RD.Loc[i].tag := nts.NTreg;
      END;
    END;
  END;
*)
END ClearLocations;


--------------------------------------------------------------------------------
--                  "Потоковый" анализ уже построенного DAG'а
--------------------------------------------------------------------------------
PROCEDURE (idb: DAG_PPC_IDB) AnalyzeDAG();
VAR iterate: BOOLEAN;
    VarsAlign: POINTER TO ARRAY OF ir.AlignType;
    VarsVisited: POINTER TO ARRAY OF BOOLEAN;
--  UsedVars: BitVect.BitVector;

  -- 1 -- AnalyzeDAG -----------------------------------------------------------
  PROCEDURE getAlignBySize (size: ir.SizeType): ir.AlignType;
  VAR align: ir.AlignType;
  BEGIN
    align := def.get_align_by_size (size);
    RETURN align;
  END getAlignBySize;

  -- 1 -- AnalyzeDAG -----------------------------------------------------------
  PROCEDURE getAlignByValue (value: LONGINT): ir.AlignType;
  BEGIN
    RETURN  tps.GetValueAlignment(value, abi.MAX_ALIGNMENT);
  END getAlignByValue;

  -- 1 -- AnalyzeDAG -----------------------------------------------------------
  PROCEDURE getObjectTypeAlign (obj: pc.OBJECT): ir.AlignType;
  VAR align: ir.AlignType;
      type: pc.STRUCT;
  BEGIN
    CASE obj.type.mode OF 
    | pc.ty_AA:       type := pcO.loc;
    | pc.ty_pointer:  type := obj.type.base;
    ELSE              type := obj.type;
    END;
    align := def.type_align(type, (*bySize*)FALSE);
--    align := def.type_align(type);
    RETURN align;
  END getObjectTypeAlign;

  -- 1 -- AnalyzeDAG -----------------------------------------------------------
  PROCEDURE getParamTypeAlign (p: ir.ParamPtr): ir.AlignType;
  VAR align: ir.AlignType;
      obj: pc.OBJECT;
      local_no: ir.Local;
  BEGIN
    align := getAlignBySize (ir.ParamSize(p.triade, p.paramnumber));
    CASE p.tag OF
    | ir.y_AddrConst
    , ir.y_RealVar:    local_no := p.name;
    | ir.y_Variable:   local_no := ir.Vars[p.name].LocalNo;
    ELSE               local_no := ir.UNDEFINED;
    END;
    IF (local_no # ir.UNDEFINED) AND (ir.Locals[local_no].VarType = ir.t_ref)
    THEN
      obj := ir.Locals[local_no].Obj;
      IF obj # NIL THEN
        align := getObjectTypeAlign(obj);
      END;
    END;
    RETURN align;
  END getParamTypeAlign;

  -- 1 -- AnalyzeDAG -----------------------------------------------------------
  PROCEDURE getValueAlign (d: RD.DAGNODE): ir.AlignType;
  VAR align: ir.AlignType;

    -- 2 -- AnalyzeDAG.getValueAlign -------------------------------------------
    PROCEDURE getProcRetValueAlign (p: ir.ParamPtr): ir.AlignType;
    VAR proc_no: prc.ProcNum;
        proto: prc.Proto;
        obj: pc.OBJECT;
        align: ir.AlignType;
    BEGIN
      ASSERT( p.tag = ir.y_ProcConst );
      align := 1;
      IF (ir.popt_RefAligned IN p.options) THEN
        proc_no := VAL(prc.ProcNum, p.name);
        proto   := prc.ProtoList[prc.ProcProtoNum (proc_no)];
        IF proto.ret_type = ir.t_ref THEN
          obj := prc.ProcObj (proc_no);
          IF pc.otag_ref_aligned IN obj.tags THEN
            align := def.type_align (obj.type.base.base);
          END;
        END;
      END;
      RETURN align;
    END getProcRetValueAlign;

    PROCEDURE ^ getVarValueAlign (var_no: ir.VarNum): ir.AlignType;

    -- 2 -- AnalyzeDAG.getValueAlign -------------------------------------------
    PROCEDURE getParamValueAlign (p: ir.ParamPtr): ir.AlignType;
    VAR palign: ir.AlignType;
        value: LONGINT;
    BEGIN
      CASE p.tag OF
      | ir.y_NumConst:
           value := tps.Par2Cardinal (p);
                         palign := getAlignByValue(value);
      | ir.y_AddrConst
      , ir.y_RealVar:
                         palign := getAlignByValue(ir.Locals[p.name].Offset + p.offset);
           IF palign > ir.Locals[p.name].Align THEN
                         palign := ir.Locals[p.name].Align;
           END;
      | ir.y_ProcConst:
                         palign := getProcRetValueAlign(p);
      | ir.y_Variable:
           IF (ir.popt_RefAligned IN p.options) THEN
                         palign := getParamTypeAlign(p);
           ELSE 
                         palign := getVarValueAlign(p.name);
           END;
      ELSE -- CASE t.tag OF
                         palign := 1;
      END; -- CASE t.tag OF
      RETURN palign;
    END getParamValueAlign;

    -- 2 -- AnalyzeDAG.getValueAlign -------------------------------------------
    PROCEDURE getVarValueAlign (var_no: ir.VarNum): ir.AlignType;
    VAR palign, palign2: ir.AlignType;
        local_no: ir.Local;
        triade: ir.TriadePtr;
        i: LONGINT;
        obj: pc.OBJECT;
    BEGIN
      local_no := ir.Vars[var_no].LocalNo;
      IF (local_no # ir.UNDEFINED)
       & (ir.Locals[local_no].VarType = ir.t_ref)
      THEN
        IF (ir.o_DiscardRefAligned IN ir.Vars[var_no].Options) THEN
          IF (VarsAlign[var_no] # 0) THEN
                     palign := VarsAlign[var_no];
          ELSE
                     palign := 1;
          END;
        ELSIF (ir.o_RefAligned IN ir.Locals[local_no].Options) THEN
          obj := ir.Locals[local_no].Obj;
          IF obj # NIL THEN
                     palign := getObjectTypeAlign(obj);
          ELSE
                     palign := abi.MAX_ALIGNMENT;
          END;
        ELSE         palign := 1;
        END;
      ELSE
        IF VarsVisited[var_no] AND (VarsAlign[var_no] # 0) THEN
                     palign := VarsAlign[var_no];
        ELSE
--        BitVect.Incl(UsedVars, var_no);
          VarsVisited[var_no] := TRUE;
          IF VarsAlign[var_no] # 0 THEN
                    palign := VarsAlign[var_no];
          ELSE
                    palign := abi.MAX_ALIGNMENT;
          END;
          triade := ir.Vars[var_no].Def;
          CASE triade.Op OF
          | ir.o_fi:
              IF VarsAlign[var_no] = 0 THEN
                VarsAlign[var_no] := palign;
              END;
              FOR i:=0 TO LEN(triade.Params^)-1 DO
                palign2 := getParamValueAlign (triade.Params[i]);
                IF palign2 < palign THEN
                    palign := palign2;
                END;
              END;
          | ir.o_call
          , ir.o_val
          , ir.o_cast
          , ir.o_assign:
                    palign := getParamValueAlign (triade.Params[0]);
          | ir.o_getpar:
              IF triade.align # ir.AlignUNDEFINED THEN
                    palign := triade.align;
              ELSE
                    palign := 1;
              END;
          ELSE
              IF triade.align # ir.AlignUNDEFINED THEN
                    palign := triade.align;
--            ELSIF triade.Params # NIL THEN
--              FOR i:=0 TO LEN(triade.Params^)-1 DO
--                IF (triade.Params[i].tag = ir.y_Variable) AND
--                   BitVect.In(UsedVars, triade.Params[i].name)
--                THEN
--                  palign := 1;
--                END;
--              END; -- FOR i:=0 TO LEN(triade.Params^)-1
              END;
          END; -- CASE triade.Op
          VarsAlign[var_no] := palign;
--        BitVect.Excl(UsedVars, var_no);
        END;
      END;
      RETURN palign;
    END getVarValueAlign;

  -- 1 -- AnalyzeDAG.getValueAlign----------------------------------------------
  BEGIN
    align := d.align;
    CASE d.op OF
    | ir.o_par:
        align := getParamValueAlign (d.par);
    | ir.o_getpar
    , ir.o_call:
        IF (d.tr # NIL) AND (d.tr.Tag = ir.y_Variable) THEN
          align := getVarValueAlign (d.tr.Name);
        ELSE
          align := 1;
        END;

    | ir.o_assign
    , ir.o_val
    , ir.o_cast:
        IF (d.tr # NIL) AND (d.tr.ResType = ir.t_ref) AND (d.tr.Tag = ir.y_Variable) THEN
          align := getVarValueAlign (d.tr.Name);
        ELSE
          align := getValueAlign (d.l);
        END;

    ELSE
    END;
    RETURN align;
  END getValueAlign;

  -- 1 -- AnalyzeDAG -----------------------------------------------------------
  PROCEDURE analyze (p: RD.DAGNODE; byValue:=FALSE: BOOLEAN);

    -- 2 -- AnalyzeDAG.analyze -------------------------------------------------
    PROCEDURE setAlign (p: RD.DAGNODE; align: ir.AlignType);
    BEGIN
      IF p.align # align THEN
        iterate := TRUE;
        p.align := align;
        IF (p.tr # NIL) AND (p.tr.Tag = ir.y_Variable) THEN
          VarsAlign[p.tr.Name] := align;
        END;
      END;
      IF (p.tr # NIL) THEN
        IF (p.tr.align = ir.AlignUNDEFINED) OR (align < p.tr.align) THEN
          iterate    := TRUE;
          p.tr.align := align;
        END;
      END;
    END setAlign;

    -- 2 -- AnalyzeDAG.analyze -------------------------------------------------
    PROCEDURE isRefAligned (p: RD.DAGNODE): BOOLEAN;
    VAR var_no: ir.VarNum;
        local_no: ir.Local;
    BEGIN
      IF p.tr # NIL THEN
        CASE p.tr.Tag OF
        | ir.y_RealVar:
            local_no := p.tr.Name;

        | ir.y_Variable:
            var_no := p.tr.Name;
            local_no := ir.Vars[var_no].LocalNo;
            IF (ir.o_DiscardRefAligned IN ir.Vars[var_no].Options) THEN
              RETURN FALSE;
            END;

        ELSE
            RETURN FALSE;
        END;
        RETURN (local_no # ir.UNDEFINED) 
             & (ir.o_RefAligned IN ir.Locals[local_no].Options);      
      END;  
      RETURN FALSE;
    END isRefAligned;  

  -- 1 -- AnalyzeDAG.analyze ---------------------------------------------------
  VAR align, align_l, align_r: ir.AlignType;
  BEGIN
    CASE bts.NTarity [p.op] OF
    | 0:
        CASE p.op OF
        | ir.o_par:
            IF byValue THEN
              align := getValueAlign (p);
            ELSE
              align := getParamTypeAlign (p.par);
            END;
        | ir.o_getpar:
            align := getValueAlign (p);
        ELSE
            align := 1;
        END;
    | 1:
        IF (p.op = ir.o_loadr) THEN  analyze (p.l, TRUE);
        ELSE                         analyze (p.l, byValue);
        END;
        CASE p.op OF
        | ir.o_alloca:
            align := abi.MAX_ALIGNMENT;
        | ir.o_loadr:
            IF (ir.o_RefAligned IN p.tr.Options) AND (p.tr.ResType = ir.t_ref) THEN
              align := getAlignBySize(p.tr.ResSize);
            ELSE;
              align := 1;
            END;
        | ir.o_not
        , ir.o_loset
        , ir.o_hiset
        , ir.o_sgnext
        , ir.o_neg:
            align := 1;

        | ir.o_assign
        , ir.o_val
        , ir.o_cast:
            align := getValueAlign (p);

        ELSE
            align := getValueAlign (p.l);
        END;
    | 2:
        CASE p.op OF
        | ir.o_copy
        , ir.o_storer:  analyze (p.l, TRUE);
        ELSE            analyze (p.l, byValue);
        END;
        analyze (p.r, byValue);
        CASE p.op OF
        | ir.o_mul:
            align_l := getValueAlign (p.l);
            align_r := getValueAlign (p.r);
            align   := getAlignByValue (align_l * align_r);
        | ir.o_div
        , ir.o_dvd:
            align_l := getValueAlign (p.l);
            align_r := getValueAlign (p.r);
            align   := align_r DIV align_l;
            IF align = 0 THEN
              align := 1;
            END;
        | ir.o_andnot:
            align := 1;
        | ir.o_shl:
            align := getValueAlign (p.r) * 2;
        | ir.o_incl
        , ir.o_excl
        , ir.o_shr
        , ir.o_sar
        , ir.o_rol
        , ir.o_ror
        , ir.o_move_eq
        , ir.o_move_le:
            align := 1;
        | ir.o_shift:
            CASE p.tr.Op OF
            | ir.o_shl:  align := getValueAlign (p.r) * 2;
            ELSE         align := 1;
            END;
        | ir.o_call:
            align := getValueAlign (p);
        | ir.o_storer:
            IF (ir.o_RefAligned IN p.tr.Options) & isRefAligned(p.l) THEN
              align := getAlignBySize(p.tr.ResSize);
            ELSE
              align := getValueAlign (p.l);
            END;
        | ir.o_logical:
            align_l := getValueAlign (p.l);
            align_r := getValueAlign (p.r);
            CASE p.tr.Op OF
            | ir.o_and:
                IF (align_l > align_r) THEN  align := align_l;
                ELSE                         align := align_r;
                END;
            | ir.o_andnot:
                align := 1;
            ELSE
                IF (align_l < align_r) THEN  align := align_l;
                ELSE                         align := align_r;
                END;
            END;
        | ir.o_copy:
            align_l := getValueAlign (p.l.l);
            align_r := getValueAlign (p.l.r);
            IF (align_l < align_r) THEN  align := align_l;
            ELSE                         align := align_r;
            END;
        ELSE
            align_l := getValueAlign (p.l);
            align_r := getValueAlign (p.r);
            IF (align_l < align_r) THEN  align := align_l;
            ELSE                         align := align_r;
            END;
        END;
    END;
    setAlign (p, align);
  END analyze;

--- 0 -- AnalyzeDAG ------------------------------------------------------------
VAR i: ir.TSNode;
    p: RD.DAGNODE;
BEGIN
  idb.AnalyzeDAG^();
  IF ir.NVars # ir.ZEROVarNum THEN
    NEW(VarsAlign,  ir.NVars);
    sys.FILL(sys.ADR(VarsAlign^), 0, SIZE(VarsAlign^));
    NEW(VarsVisited,  ir.NVars);
--  UsedVars := BitVect.New (ir.NVars, FALSE);
  ELSE
    VarsAlign   := NIL;
    VarsVisited := NIL;
--  UsedVars    := NIL;
  END;
  iterate := TRUE;
  WHILE iterate DO
    iterate := FALSE;
    FOR i:=ir.StartOrder TO VAL(ir.TSNode, PRED(ir.Nnodes)) DO
      p := RD.Dag[ir.Order[i]];
      WHILE p # NIL DO
        IF VarsVisited # NIL THEN
          sys.FILL(sys.ADR(VarsVisited^), FALSE, SIZE(VarsVisited^));
--        BitVect.Fill(UsedVars, FALSE, ir.NVars);
        END;
        analyze(p);
        p := p.next;
      END;
    END;
  END; -- WHILE iterate
END AnalyzeDAG;


--------------------------------------------------------------------------------
VAR IDB: DAG_PPC_IDB;
BEGIN
  NEW(IDB);
  DAG.IDB := IDB;
END DAG_I.
