(** Copyright (c) 1991,98 XDS Ltd, Russia. All Rights Reserved. *)
MODULE ccCode;
<* IF ~ DEFINED(NODE_VALUE_REPRESENTATION) THEN *> <* NEW NODE_VALUE_REPRESENTATION- *>  <* END *>

(* Modifications:
   20/Apr/98 Vit  5.04  @2: NODE-VALUE representation change
   05/Apr/98 Vit  5.00  Comments inserted; some code/naming clarification done.
                        Trace and IR visualisation inserted in DB_TRACE mode
   (XDS versions below, BE - above)
   08/Feb/96 Ned  2.12  op_gendll in "GenIncludes"
   08/Feb/96 Ned  2.12  back-end version 4.10
   16/Mar/96 Ned  2.12  back-end version 4.11
   17/Mar/96 Ned  2.12  some BNRP specific is removed: GENNOBITREF,
                        for, include files.
   17/Mar/96 Ned  2.12  Comments copying is improved.
*)

IMPORT
  pc  := pcK,
  env := xiEnv,
         xcStr,
  xfs := xiFiles,
  cc  := ccK,
  nms := ccNames,
  out := ccOut,
  def := ccDef,
  dcl := ccDcl,
  cmt := ccComments,
  exp := ccExpr,
  seg := ccSeg,
  rec := ccRec,
<* IF DB_TRACE THEN *>
  dbg := pcVisIR,
<* END *>
  tim := SysClock,
  SYSTEM;

TYPE
  STR = out.STR;

VAR
  zz_tmp     : pc.VALUE;
  zz_gcr     : pc.VALUE; (* is used when CASE generating *)
  zz_abs_stp : pc.VALUE; (* is used in GenStatementFor *)

  default_alignment : SHORTINT;

-----------------------------------------------------------------------------

PROCEDURE NeedNL ( n: pc.NODE ) : BOOLEAN;
(*
   Checks if a line break needed between 'n' and 'n.next' (T, if not exist)
*)
VAR c,cn,l,ln: LONGINT;
    voidf: env.String;

BEGIN
  IF n.next = NIL THEN RETURN TRUE END;
  n.pos.unpack (voidf, l, c);
  n.next.pos.unpack (voidf, ln, cn);
  RETURN (l # ln)             -- if stats are on different lines
      OR (c = cn);            -- or pos-ns are equal (artific.actions inserted)
END NeedNL;

-----------------------------------------------------------------------------

PROCEDURE GenLabel ( nm-: ARRAY OF CHAR
		   ; ps-: pc.TPOS
                   )    :		pc.OBJECT;
(*
   Generate 'ob_label' object in current scope.
*)
VAR h: pc.STRUCT;
    o: pc.OBJECT;
BEGIN

  ASSERT( ~dcl.gen_def );

  IF dcl.cur_proc # NIL
   THEN h := dcl.cur_proc;
   ELSE h := dcl.CurMod.type;
  END;
  o := pc.new_object_ex (NIL, h, pc.ob_label, FALSE);
  NEW  (o.name, LENGTH (nm)+1);
  COPY (nm, o.name^);
  INCL (o.tags, cc.otag_declared);
  o.pos := ps;
  RETURN o;
END GenLabel;

-----------------------------------------------------------------------------

PROCEDURE ^ GenSequence (      n: pc.NODE
                        ;    lev: INTEGER
                        ; VAR dd: BOOLEAN
                        ; VAR rt: BOOLEAN
                        ;    nrt: BOOLEAN
                        ;    trt: BOOLEAN
                        ;   loop: pc.NODE );

-----------------------------------------------------------------------------

PROCEDURE GenBracedStats (      n: pc.NODE
                         ;    lev: INTEGER
                         ; VAR dd: BOOLEAN   -- dead end
                         ;    nrt: BOOLEAN   -- do not gen return
                         ;    trt: BOOLEAN   -- do gen return
                         ;   loop: pc.NODE );
(*
   nrt - оператор является последним в блоке: можно не генерить
         goto в операторе RETURN
   trt - по RETURN нужно выйти из процедуры оператором return,
         нет кода, который нужно исполнить после RETURN но до
         выхода из тела C-процедуры
*)
VAR rt: BOOLEAN;
BEGIN
  out.WrSpc (lev-1);
  out.WrChr ("{");
  out.WrNL;
  GenSequence (n, lev, dd(*=>*), rt(*=>*), nrt, trt, loop);
  out.WrSpc (lev-1);
  out.WrStr ("} ");
END GenBracedStats;

-----------------------------------------------------------------------------

PROCEDURE GenStatementFor (   n: pc.NODE
                          ; lev: INTEGER
                          ; trt: BOOLEAN );
(*
*)
VAR
  var_to,
  var_no,
  var_nm  : STR;
  stp     : pc.VALUE;
  assigned: BOOLEAN;
  l       : pc.NODE;

  -----------------------------------------------
  PROCEDURE GenValueTo;
  (*
  *)
  BEGIN
    IF n.obj.type.mode = pc.ty_char THEN
      out.WrFmt ("(%s)", nms.x2c[nms.nm_shortcard]^);
    END;
    IF var_to # ""
     THEN out.WrStr (var_to);
     ELSE exp.GenValue (l.r, 13, {});
    END;
  END GenValueTo;

  -----------------------------------------------
  PROCEDURE GenVarNm;
  (*
  *)
  BEGIN
    IF n.obj.type.mode = pc.ty_char THEN
      out.WrFmt ("(%s)", nms.x2c[nms.nm_shortcard]^);
    END;
    out.WrStr (var_nm);
  END GenVarNm;

  -----------------------------------------------
  PROCEDURE GenVarAssign;
  (*
  *)
  BEGIN
    ASSERT( ~ assigned );
    out.WrFmt ("%s = ", var_nm);
    exp.GenValue (l.l, 0, {});
    assigned := TRUE;
  END GenVarAssign;

  -----------------------------------------------
  PROCEDURE GenPreCheck ( en_if: BOOLEAN ): BOOLEAN;
  (*
     возвращает FALSE если цикл не исполняется ни разу
  *)
  VAR neg: BOOLEAN;
  BEGIN
    neg := stp.is_neg();
    IF (l.r.val # NIL) & (l.l.val # NIL) THEN
      IF neg
       THEN RETURN dcl.CmpValue (pc.sb_geq, l.l.val, l.r.val);
       ELSE RETURN dcl.CmpValue (pc.sb_leq, l.l.val, l.r.val);
      END;
    END;
    IF l.r.val # NIL THEN
      IF neg
<* IF NODE_VALUE_REPRESENTATION THEN *>
       THEN zz_tmp.binary (pc.sb_geq, n.obj.type.min.val, l.r.val);
       ELSE zz_tmp.binary (pc.sb_leq, n.obj.type.max.val, l.r.val);
<* ELSE *>
       THEN zz_tmp.binary (pc.sb_geq, n.obj.type.min, l.r.val);
       ELSE zz_tmp.binary (pc.sb_leq, n.obj.type.max, l.r.val);
<* END *>
      END;
      IF ~ zz_tmp.is_zero() THEN RETURN TRUE END;
    END;
    IF ~ en_if THEN RETURN TRUE END;
    IF ~ assigned THEN
      out.WrSpc (lev);
      GenVarAssign;
      out.WrChr (';');
      out.WrNL;
    END;
    out.WrSpc (lev);
    out.WrStr ("if (");
    GenVarNm;
    IF neg
     THEN out.WrStr (" >= ")
     ELSE out.WrStr (" <= ")
    END;
    GenValueTo; out.WrStr (") ");
    RETURN TRUE;
  END GenPreCheck;

  -----------------------------------------------
  PROCEDURE GenVarInc ( stp1: BOOLEAN );
  (*
  *)
  VAR
    sig: CHAR;
    str: STR;
  BEGIN
    IF pc.ntag_chk_range IN n.tags THEN
      out.WrFmt ("%s = %s (%s", var_nm,
                 nms.x2c[nms.RangeChk (n.obj.type)]^, var_nm);
      IF stp.is_neg()
       THEN out.WrStr ("-")
       ELSE out.WrStr ("+")
      END;
      dcl.GenConstAggr (zz_abs_stp, n.obj.type, 11, FALSE, FALSE);
      out.WrChr (',');
      exp.GenMin (n.obj.type);
      out.WrChr (',');
      exp.GenMax (n.obj.type);
      out.WrChr (")");
    ELSE
      out.WrStr (var_nm);
      IF stp1 THEN
        IF  cc.op_cpp & ~cc.op_constenum    -- C++ and enum:
         & (n.obj.type.mode = pc.ty_enum)
        THEN
          dcl.TypeDesignator (str(*=>*), "", n.obj.type);
          IF stp.is_neg()
           THEN sig := "-";
           ELSE sig := "+";
          END;
          out.WrFmt (" = (%s)(%s%c1)", str, var_nm, sig);
        ELSE
          IF stp.is_neg()
           THEN out.WrStr ("--")
           ELSE out.WrStr ("++")
          END;
        END
      ELSE
        IF stp.is_neg()
         THEN out.WrStr (" -= ")
         ELSE out.WrStr (" += ")
        END;
        dcl.GenConstAggr (zz_abs_stp, n.obj.type, 1, FALSE, FALSE);
      END;
    END;
  END GenVarInc;
  -----------------------------------------------

VAR
  dd,qq,rt: BOOLEAN;
  t       : pc.STRUCT;
  nm      : INTEGER;
BEGIN
  l   := n.l;
  stp := l.val;
  assigned := FALSE;
  IF stp = NIL THEN
<* IF NODE_VALUE_REPRESENTATION THEN *>
    stp := ir.ValInt1
<* ELSE *>
    stp := dcl.NewIntValue (1);
<* END *>
  END;
  zz_abs_stp.unary (pc.su_abs,stp);

  dcl.ObjectUsage (n.obj, var_nm(*=>*));

  var_to := "";
  IF (l.r.val = NIL) THEN
    dcl.MakeTempVar (n.obj.type, var_to(*=>*));
    out.WrSpc (lev);
    out.WrFmt ("%s = ", var_to);
    exp.GenValue (l.r, 0, {});
    out.WrChr (';');
    out.WrNL;
  END;
<* IF NODE_VALUE_REPRESENTATION THEN *>
  IF dcl.CmpValue (pc.sb_equ, zz_abs_stp, ir.ValInt1) THEN
<* ELSE *>
  IF dcl.CmpValue (pc.sb_equ, zz_abs_stp, dcl.NewIntValue (1)) THEN
<* END *>
    (* шаг равен +/- 1 *) (* или число повторений константа !!!!! *)
    qq := (l.r.val # NIL)
<* IF NODE_VALUE_REPRESENTATION THEN *>
        & dcl.CmpValue (pc.sb_neq, l.r.val, n.obj.type.min.val)
        & dcl.CmpValue (pc.sb_neq, l.r.val, n.obj.type.max.val);
<* ELSE *>
        & dcl.CmpValue (pc.sb_neq, l.r.val, n.obj.type.min)
        & dcl.CmpValue (pc.sb_neq, l.r.val, n.obj.type.max);
<* END *>
    IF ~ GenPreCheck (~ qq) THEN RETURN END;
    IF out.Cpos = 0 THEN out.WrSpc (lev) END;
    out.WrStr ("for (");
    IF ~assigned THEN GenVarAssign END;
    out.WrChr (";");
    IF qq THEN
      out.WrChr (" ");
      GenVarNm;
      IF stp.is_neg()
       THEN out.WrStr (" >= ")
       ELSE out.WrStr (" <= ")
      END;
      GenValueTo;
    END;
    out.WrStr ("; ");
    GenVarInc (TRUE);
    out.WrFmt (") {");
    out.WrNL;
    GenSequence (n.r, lev+1, dd(*=>*), rt(*=>*), FALSE, trt, n);
    (* значения zz_* испорчены ! *)
    IF ~ qq (*OR (bd.ntag_chkfor IN n.tags)*) THEN -- $$$ do it for low = ushort (0)
      out.WrSpc (lev+1);
      out.WrStr ("if (");
      GenVarNm;
      out.WrStr (" == ");
      GenValueTo;
      out.WrFmt (") break;");
      out.WrNL;
    END;
  ELSE
    IF var_to = ""
     THEN dcl.MakeTempVar (n.obj.type, var_no(*=>*));
     ELSE COPY (var_to, var_no);
    END;
    out.WrSpc (lev);
    GenVarAssign;
    out.WrChr (';');
    out.WrNL;
    IF ~ GenPreCheck (TRUE) THEN RETURN END;
    IF out.Cpos = 0 THEN out.WrSpc (lev) END;
    out.WrFmt ("for (%s = ", var_no);
    IF (l.l.val # NIL) & (l.r.val # NIL) THEN
      IF stp.is_neg()
       THEN zz_tmp.binary (pc.sb_minus, l.l.val, l.r.val);
       ELSE zz_tmp.binary (pc.sb_minus, l.r.val, l.l.val);
      END;
      zz_tmp.binary (pc.sb_div, zz_tmp, zz_abs_stp); (* число повторений *)
      dcl.GenConstAggr (zz_tmp, n.obj.type, 1, FALSE, FALSE);
    ELSE
      t := n.obj.type.super_type();
      CASE t.mode OF
      | pc.ty_shortint
      , pc.ty_shortcard: nm := nms.nm_shortcard;
      | pc.ty_integer
      , pc.ty_cardinal : nm := nms.nm_cardinal;
      | pc.ty_longint
      , pc.ty_longcard : nm := nms.nm_longcard;
      END;
      out.WrFmt ("(%s)(", nms.x2c[nm]^);
      IF stp.is_neg()
       THEN GenVarNm;   out.WrChr ("-"); GenValueTo;
       ELSE GenValueTo; out.WrChr ("-"); GenVarNm;
      END;
      out.WrStr (")/");
      dcl.GenConstAggr (zz_abs_stp, n.obj.type, 12, FALSE, FALSE);
    END;
    out.WrFmt (";;) {");
    out.WrNL;
    GenSequence (n.r, lev+1, dd(*=>*), rt(*=>*), FALSE, trt, n);
    (* значения zz_* испорчены ! *)
    zz_abs_stp.unary (pc.su_abs, stp);
    out.WrSpc (lev+1); out.WrFmt ("if (!%s) break;", var_no); out.WrNL;
    out.WrSpc (lev+1); out.WrFmt ("--%s;", var_no); out.WrNL;
    out.WrSpc (lev+1);
    GenVarInc (FALSE); out.WrChr (";"); out.WrNL;
  END;
  out.WrSpc (lev);
  out.WrFmt ("} ");
  def.OutComment ("FOR");
END GenStatementFor;

-----------------------------------------------------------------------------

PROCEDURE GenSproc (   n: pc.NODE
                   ; lev: INTEGER );
(*
   Generate standard procedure
*)
VAR
  l : pc.NODE;
  nm: STR;
  i : LONGINT;
  ch: CHAR;
  nn: INTEGER;
BEGIN
  CASE n.sub OF

  | pc.sp_put:
      IF dcl.IsCarray (n.r.next.type) THEN
        out.WrFmt ("%s(", nms.x2c[nms.nm_memcpy]^);
        out.SaveIndent;
        exp.GenValue (n.r, 0, {});
        out.WrChr (',');
        exp.GenValue (n.r.next, 0, {dcl.REF, dcl.ANY});
        out.WrChr (',');
        exp.GenSize (n.r.next, 0, 0);
        out.WrFmt (");");
      ELSE
        dcl.TypeDesignator (nm(*=>*), "*", n.r.next.type);
        out.WrFmt ("*(%s)", nm);
        exp.GenValue (n.r, 13, {});
        out.WrStr (" = ");
        out.SaveIndent;
        exp.GenValue (n.r.next, 1, {});
        out.WrChr (";");
      END;
      out.WrNL;

  | pc.sp_get:
      IF dcl.IsCarray (n.r.next.type) THEN
        out.WrFmt ("%s(", nms.x2c[nms.nm_memcpy]^);
        out.SaveIndent;
        exp.GenValue (n.r.next, 0, {dcl.REF, dcl.ANY, dcl.LVL});
        out.WrChr (',');
        exp.GenValue (n.r, 0, {});
        out.WrChr (',');
        exp.GenSize (n.r.next, 0, 0);
        out.WrFmt (");");
      ELSE
        exp.GenValue (n.r.next, 1, {dcl.LVL});
        out.WrStr (" = ");
        out.SaveIndent;
        dcl.TypeDesignator (nm(*=>*), "*", n.r.next.type);
        out.WrFmt ("*(%s)", nm);
        exp.GenValue (n.r, 13, {});
        out.WrFmt (";");
      END;
      out.WrNL;

  | pc.sp_new
  , pc.sp_sysnew
  , pc.sp_dispose:
      exp.GenCallStorage (n);
      out.WrChr (";");
      out.WrNL;

  | pc.sp_assert:
      out.WrStr ("if (");
      out.SaveIndent;
      exp.GenValue (n.r, -1, {dcl.NEG});
      out.WrFmt (") %s(", nms.x2c[nms.nm_assert]^);
      IF n.r.next # NIL
       THEN exp.GenValue (n.r.next, 0, {});
       ELSE out.WrChr ('0');
      END;
      out.WrStr (");");

  | pc.sp_incl
  , pc.sp_excl:
      ASSERT( n.r.type.mode = pc.ty_set );
      IF ~ dcl.IsCarray (n.r.type) THEN
        exp.GenValue (n.r, 1, {dcl.LVL});
        IF n.sub = pc.sp_incl
         THEN out.WrStr (" |= ")
         ELSE out.WrStr (" &= ~")
        END;
        IF n.r.val # NIL THEN
<* IF NODE_VALUE_REPRESENTATION THEN *>
          zz_tmp.binary (pc.sb_minus, n.r.next.val, n.r.type.min.val);
<* ELSE *>
          zz_tmp.binary (pc.sb_minus, n.r.next.val, n.r.type.min);
<* END *>
          out.WrFmt ("0x%X", {zz_tmp.get_integer()});
          dcl.Suffix (n.r.type);
          out.WrChr (";");
        ELSE
          out.WrStr ("(1");
          dcl.Suffix (n.r.type);
          out.WrStr ("<<");
          exp.GenCheckIndex (n.r.next, n.r, 10, 0, pc.ntag_chk_range IN n.tags);
          out.WrFmt (");");
        END;
      ELSE
        IF  n.sub = pc.sp_incl
         THEN nn := nms.nm_incl;
         ELSE nn := nms.nm_excl;
	END;
        out.WrFmt ("%s(", nms.x2c[nn]^);
        out.SaveIndent;
        exp.GenValue (n.r, 0, {dcl.LVL, dcl.BSA});
        out.WrChr (',');
	l := n.r.next;
        exp.GenValueMinusMin (l, 0, n.r.type, TRUE);
        out.WrFmt (",%d);", n.r.type.len);
      END;
      out.WrNL;

  | pc.sp_code:
      ASSERT(  n.r.val # NIL  );
      ch := 0X;
      IF cc.op_krc THEN
	i := 0;
	LOOP
	  IF i > n.r.type.len-2 THEN EXIT END;
          zz_tmp.index_get (i, n.r.val);
          ch := CHR (zz_tmp.get_integer());
	  IF ch # ' ' THEN EXIT END;
          INC (i);
        END;
      END;
      IF ch # '#' THEN
        out.WrSpc (lev)
      END;
      FOR i := 0 TO n.r.type.len-2 DO
        zz_tmp.index_get (i, n.r.val);
        out.WrChr (CHR (zz_tmp.get_integer()));
      END;
  ELSE
    out.WrFmt ("%s(", nms.x2c[nms.SprocName (n.pos, n.sub)]^);
    out.SaveIndent;
    l := n.r;
    WHILE l # NIL DO
      IF l # n.r THEN
        out.WrChr (",")
      END;
      IF l.type.mode IN pc.ARRs THEN
        exp.GenValue (l, 0, {dcl.BSA}); out.WrChr (',');
        exp.GenLen (l, 0, l.type.inx, 0);
      ELSE
        exp.GenValue (l, 0, {});
      END;
      l := l.next;
    END;
    out.WrFmt (");");
    out.WrNL;
  END
END GenSproc;

-----------------------------------------------------------------------------

VAR Wrap: BOOLEAN;         -- Wrap between statements flag

PROCEDURE GenStatement (      n: pc.NODE
                       ;    lev: INTEGER
                       ; VAR rt: BOOLEAN
                       ;    nrt: BOOLEAN
                       ;    trt: BOOLEAN
                       ;   loop: pc.NODE );
(*
   nrt - оператор является последним в блоке: можно не генерить
		goto в операторе RETURN
   trt - по RETURN нужно выйти из процедуры оператором return,
		нет кода, который нужно исполнить после RETURN но до
		выхода из тела C-процедуры
*)
VAR
  nm     : STR;
  buf    : STR;
  l,m,k  : pc.NODE;
  dd,mt,b: BOOLEAN;
  tmps   : dcl.TMP_VAR;
  nl     : BOOLEAN;      -- "NL after statement required" flag
  voidrt : BOOLEAN;      -- to receive unused 'rt' value
  t      : pc.STRUCT;

  -----------------------------------------------------
  PROCEDURE GenIfStat ( n: pc.NODE );
  (*
  *)
  BEGIN
    out.WrStr ("if (");
    out.SaveIndent;
    exp.GenValue (n.l, -1, {});
    out.WrStr (") ");
    out.WrNL;
    GenBracedStats (n.r.l, lev+1, dd(*=>*), nrt, trt, loop);
    IF n.r.r = NIL THEN
      def.OutComment ("IF");
    ELSE
      out.WrNL;
      out.WrSpc (lev);
      out.WrStr ("else ");
      IF (n.r.r.mode = pc.nd_if) & (n.r.r.next = NIL) THEN
        GenIfStat (n.r.r);         -- avoid cascading here
      ELSE
        out.WrNL;
        GenBracedStats (n.r.r, lev+1, dd(*=>*), nrt, trt, loop);
        def.OutComment ("IF");
      END;
    END;
  END GenIfStat;
  ------------------------------------------------

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(GenStmt [%X]", n) END;
<* END *>

  dcl.EnterStatement (tmps(*=>*));
  IF Wrap THEN
    IF cc.op_comments THEN cmt.OutNextComment (n.pos, lev) END;
    IF cc.op_lineno   THEN dcl.OutLineNo (n.pos, lev)      END;
    IF (n.mode # pc.nd_block)
     & (n.mode # pc.nd_return)
     &((n.mode # pc.nd_sproc) OR (n.sub # pc.sp_code))
     & (n.mode # pc.nd_for)
    THEN
      (* nd_return и nd_block могут вырождаться в пустую строку! *)
      out.WrSpc (lev);
    END;
  ELSE
    out.WrChr(" ");
  END;

  nl := TRUE;                              -- set newline flag as default
  rt := FALSE;                             -- drop return flag as default
  CASE n.mode OF
  | pc.nd_call:
      exp.GenCall (n, -1, {});
      out.WrFmt (";");

  | pc.nd_assign:
      IF dcl.IsCarray (n.r.type)
       THEN exp.GenAssign (n, -1, {dcl.REF, dcl.ANY});
       ELSE exp.GenAssign (n, -1, {});
      END;
      out.WrFmt (";");

  | pc.nd_while:
      out.WrStr ("while (");
      out.SaveIndent;
      exp.GenValue (n.l, -1, {});
      out.WrStr (") ");
      out.WrNL;
      GenBracedStats (n.r, lev+1, dd (*=>*), FALSE, trt, n);
      IF n.obj # NIL THEN
        out.WrNL;
        dcl.ObjectUsage (n.obj, nm (*=>*));
        out.WrSpc (lev); out.WrFmt ("%s:; ", nm)
      END;
      def.OutComment ("WHILE");
      nl := FALSE;

  | pc.nd_repeat:
      out.WrFmt ("do {");
      out.WrNL;
      GenSequence (n.l, lev+1, dd(*=>*), voidrt(*=>*), FALSE, trt, n);
      out.WrSpc (lev); out.WrStr ("} while (");
      out.SaveIndent;
      exp.GenValue (n.r, -1, {dcl.NEG});
      out.WrFmt (");");
      out.WrNL;
      IF n.obj # NIL THEN
        dcl.ObjectUsage (n.obj, nm(*=>*));
        out.WrSpc (lev); out.WrFmt ("%s:; ", nm)
      END;
      def.OutComment ("REPEAT");
      nl := FALSE;

  | pc.nd_loop:
      out.WrStr ("for (;;)");
      out.WrNL;
      GenBracedStats (n.r, lev+1, dd (*=>*), FALSE, trt, n);
      IF n.obj # NIL THEN
        out.WrNL;
        dcl.ObjectUsage (n.obj, nm(*=>*));
        out.WrSpc (lev); out.WrFmt ("%s:; ", nm)
      END;
      def.OutComment ("LOOP");
      nl := FALSE;

  | pc.nd_label:
      IF n.obj = NIL THEN
        n.obj := GenLabel ("label", n.pos);
      END;
      dcl.ObjectUsage (n.obj, nm(*=>*));
      out.WrFmt ("%s:;", nm);

  | pc.nd_block:
      IF ~trt OR ~nrt THEN
        GenSequence (n.r, lev, dd(*=>*), rt(*=>*), TRUE, FALSE, loop);
	IF n.obj # NIL THEN
          ASSERT( ~(pc.ntag_no_exit IN n.tags) ); -- as there is label,
          dcl.ObjectUsage (n.obj, nm(*=>*));      -- control flow reaches it
          out.WrSpc (lev); out.WrFmt ("%s:;", nm);
          out.WrNL;
          rt := FALSE;
	END;
      ELSE
        GenSequence (n.r, lev, dd(*=>*), rt(*=>*), TRUE, TRUE, loop);
        ASSERT( n.obj = NIL );
        (* block was gen-ed with trt=T: no exit label, as there is no goto
           but control flow may reach cur. C position, e.g. if block has no
           RETURNs at all, but this means that proc has no result and 'return'
           may be omitted *)
      END;
      nl := FALSE;

  | pc.nd_exit:
      IF n.r = loop THEN
        out.WrFmt ("break;");
      ELSE
        IF n.r.obj = NIL THEN
          n.r.obj := GenLabel ("loop_exit", n.pos);
        END;
        dcl.ObjectUsage (n.r.obj, nm(*=>*));
        out.WrFmt ("goto %s;", nm);
      END;

  | pc.nd_goto:
      IF n.l.obj = NIL THEN
        n.l.obj := GenLabel ("label", n.pos);
      END;
      dcl.ObjectUsage (n.l.obj, nm(*=>*));
      out.WrFmt ("goto %s;", nm);

  | pc.nd_return:
      IF (dcl.cur_proc # NIL) & dcl.IsCarray (dcl.cur_proc.base) THEN
        IF Wrap THEN out.WrSpc(lev) ELSE out.WrChr(" ") END;
        out.WrStr (nms.x2c[nms.nm_memcpy]^);
        out.WrChr ("(");
        dcl.OSecondName (dcl.cur_proc.obj, 0, "_ret", nm(*=>*));
        out.WrFmt ("%s,", nm);
        exp.GenValue (n.l, 0, {dcl.REF, dcl.ANY});
        out.WrChr (',');
        dcl.GenSizeof (dcl.cur_proc.base);
        out.WrFmt (");");
        IF trt THEN
          rt := TRUE;
          out.WrNL; out.WrSpc (lev); out.WrFmt ("return %s;", nm);
        END;
      ELSIF trt THEN
        rt := TRUE;
        IF Wrap THEN out.WrSpc(lev) ELSE out.WrChr(" ") END;
        out.WrStr ("return");
        IF n.l # NIL THEN
          out.WrChr (' ');
          exp.GenValue (n.l, -1, {})
        END;
        out.WrFmt (";");
      ELSIF n.l # NIL THEN
        ASSERT( dcl.cur_proc # NIL );
        dcl.OSecondName (dcl.cur_proc.obj, 0, "_ret", nm(*=>*));
        IF Wrap THEN out.WrSpc(lev) ELSE out.WrChr(" ") END;
        out.WrFmt ("%s = ", nm);
        exp.GenValue (n.l, 1, {});
        out.WrFmt (";");
        def.ret_emul := TRUE;
      END;

      IF ~nrt & ~rt THEN       -- gen goto to end of block
        out.WrNL;
        IF n.r.obj = NIL THEN
          n.r.obj := GenLabel ("label", n.pos);
        END;
        dcl.ObjectUsage (n.r.obj, nm(*=>*));
        out.WrSpc (lev); out.WrFmt ("goto %s;", nm);
      END;

  | pc.nd_for:
      GenStatementFor (n, lev, trt);
      nl := FALSE;

  | pc.nd_with:
      dcl.ObjectUsage (n.obj, nm(*=>*));
      IF pc.otag_with IN n.obj.tags THEN
        ASSERT( n.obj.type.mode = pc.ty_pointer );
        out.WrFmt ("{ ");
        def.OutComment ("WITH");
        out.WrSpc (lev+1);
        IF cc.op_cpp THEN
          out.StrFmt (buf, "&%s", nm);
          dcl.TypeDesignator (nm(*=>*), buf, n.obj.type.base);
          out.WrFmt ("%s = ", nm);
          exp.GenValue (n.l, 1, {});
        ELSE
          dcl.TypeDesignator (buf(*=>*), nm, n.obj.type);
          out.WrFmt ("%s = ", buf);
          exp.GenValue (n.l, 1, {dcl.REF});
        END;
        out.WrChr (";");
        out.WrNL;
        GenSequence (n.r, lev+1, dd(*=>*), rt(*=>*), nrt, trt, loop);
        out.WrSpc (lev);
        out.WrFmt ("} "); def.OutComment ("WITH");
      ELSE
        out.WrFmt  ("%s = ", nm);
        exp.GenValue (n.l, 1, {dcl.REF});
        out.WrChr (";");
        out.WrNL;
        GenSequence (n.r, lev, dd(*=>*), rt(*=>*), nrt, trt, loop);
      END;
      ASSERT( (pc.ntag_no_exit IN n.tags) = dd );
      nl := FALSE;

  | pc.nd_wtrap:
      out.WrFmt ("%s(%s);", nms.x2c[nms.nm_trap]^,
                            nms.x2c[nms.nm_guard_trap]^);

  | pc.nd_ftrap:
      out.WrFmt ("%s(%s);", nms.x2c[nms.nm_trap]^,
                            nms.x2c[nms.nm_return_trap]^);

  | pc.nd_if:
      GenIfStat (n);
      nl := FALSE;

  | pc.nd_case:
      m := n.r;
      ASSERT( (m.mode = pc.nd_casedo) OR (m.mode = pc.nd_caselse) );

      b := FALSE;  (* check if ranges are present in CASE: switch/if schemes *)
      l := m.l;
      WHILE l # NIL DO                 -- lookup through branches
        ASSERT( l.mode = pc.nd_node ); --  (dont break loop to check all!)
        k := l.l;
        REPEAT                         -- lookup through const list
          ASSERT( k.mode = pc.nd_pair );
          IF ~dcl.CmpValue (pc.sb_equ, k.val, k.l.val) THEN
            b := TRUE
          END;
          k := k.next;
        UNTIL k = NIL;
        l := l.next;
      END;

      IF ~b THEN                    -- No ranges in CASE: gen "switch"-form
        out.WrStr ("switch (");
        out.SaveIndent;
        t := n.l.type.super_type();
        IF t.mode = pc.ty_shortint THEN
          out.WrStr ("(int)");
          exp.GenValue (n.l, 13, {});
        ELSIF t.mode = pc.ty_shortcard THEN
          out.WrStr ("(unsigned)");
          exp.GenValue (n.l, 13, {});
        ELSE
          exp.GenValue (n.l, -1, {});
        END;
        out.WrChr (')');
        IF cc.op_comments THEN
          cmt.OutTailComment (n.l.pos);
        END;
        out.WrNL;
        out.WrSpc (lev); out.WrChr ("{");
        out.WrNL;
        l := m.l;
        WHILE l # NIL DO
          k := l.l;
          REPEAT
            out.WrSpc (lev);
            out.WrStr ("case ");
            out.SaveIndent;
            exp.GenValue (k.l, -1, {});
            out.WrFmt (":");
            IF cc.op_comments THEN
              cmt.OutTailComment (k.pos)
            END;
            out.WrNL;
(* Vit: equality assumed now for this scheme
            IF ~dcl.CmpValue (pc.sb_equ, k.l.val, k.r.val) THEN
              zz_gcr.binary (pc.sb_plus, k.l.val, ir.ValInt1);
              LOOP
                out.WrSpc (lev);
                out.WrStr ("case ");
                dcl.GenConstAggr (zz_gcr, k.type, 0, FALSE, FALSE);
                out.WrStr (":");
                IF cc.op_comments THEN cmt.OutTailComment (k.pos) END;
                out.WrNL;
                IF dcl.CmpValue (pc.sb_equ, zz_gcr, k.l.val) THEN EXIT END;
                zz_gcr.binary (pc.sb_plus, zz_gcr, ir.ValInt1);
              END
            END; *)
            k := k.next;
          UNTIL k = NIL;
          GenSequence (l.r, lev+1, dd(*=>*), voidrt(*=>*), FALSE, trt, n);
            -- nrt = FALSE, otherwise falls into next case, instd of return
          IF ~dd THEN
            out.WrSpc (lev+1);
            out.WrFmt ("break;");
            out.WrNL;
          END;
          l := l.next;
        END;

        IF m.mode = pc.nd_caselse THEN
          IF m.r # NIL THEN       -- empty ELSE
            out.WrSpc (lev);
            out.WrFmt ("default:");
            out.WrNL;
            GenSequence (m.r, lev+1, dd(*=>*), voidrt(*=>*), nrt, trt, n);
            IF ~dd THEN
              out.WrSpc (lev+1);
              out.WrFmt ("break;");
              out.WrNL;
            END;
              -- break or ';' after label required
          END;
        ELSE
          out.WrSpc (lev);
          out.WrFmt ("default:");
          out.WrNL;
          dcl.OutLineNo (n.pos, lev+1);
          out.WrSpc (lev+1);
          out.WrFmt ("%s(", nms.x2c[nms.nm_trap]^);
          out.WrFmt ("%s);", nms.x2c[nms.nm_case_trap]^);
          out.WrNL;
        END;
        out.WrSpc (lev); out.WrFmt ("} ");

      ELSE           -- Ranges exist in CASE stmt: gen "cascaded if"-scheme
        def.OutComment ("CASE");
        out.WrSpc (lev);
	mt := n.l.mode # pc.nd_var;   -- temp.var needed to evaluate expr.
	IF mt THEN
          dcl.MakeTempVar (n.l.type, nm(*=>*));
          out.WrFmt ("%s = ", nm);
	  exp.GenValue (n.l, -1, {});
          out.WrChr (';');
          out.WrNL;
          out.WrSpc (lev);
	END;
        l := m.l;
        WHILE l # NIL DO
          ASSERT( l.mode = pc.nd_node );
          out.WrFmt ("if (");
          out.SaveIndent;
          k := l.l;
	  b := k.next # NIL;
          REPEAT
            IF dcl.CmpValue (pc.sb_equ, k.val, k.l.val) THEN
              IF b THEN
                out.WrChr ('(')
              END;
              IF mt
               THEN out.WrStr (nm);
               ELSE exp.GenValue (n.l, 8, {});
              END;
              out.WrFmt (" == ");
	      exp.GenValue (k.l, 8, {});
              IF b THEN
               out.WrChr (')')
              END;
	    ELSE
              out.WrFmt ("%s(", nms.x2c[nms.nm_inrange]^);
              IF mt
               THEN out.WrStr (nm);
               ELSE exp.GenValue (n.l, 13, {});
              END;
              out.WrChr (',');
<* IF NODE_VALUE_REPRESENTATION THEN *>
	      exp.GenValue (k.val, 0, {});
              out.WrChr (',');
	      exp.GenValue (k.l.val, 0, {});
<* ELSE *>
              k.val.value_to_str(nm, pc.flag_c);
              out.WrStr (nm);
              out.WrChr (',');
              k.l.val.value_to_str(nm, pc.flag_c);
              out.WrStr (nm);
<* END *>
              out.WrChr (')');
            END;
            k := k.next;
            IF k # NIL THEN
              out.WrFmt (" || ")
            END;
          UNTIL k = NIL;
          out.WrChr (')');
          IF cc.op_comments THEN
            cmt.OutTailComment (n.l.pos);
          END;
          out.WrNL;
          GenBracedStats (l.r, lev+1, dd(*=>*), nrt, trt, n);
          l := l.next;
          IF l # NIL THEN
            out.WrNL;
            out.WrSpc (lev);
            out.WrFmt ("else ")
          END;
        END;

        IF m.mode = pc.nd_caselse THEN      -- empty ELSE may be omitted
          IF m.r # NIL THEN
            out.WrNL;
            out.WrSpc (lev);
            out.WrFmt ("else ");
            out.WrNL;
            GenBracedStats (m.r, lev+1, dd(*=>*), nrt, trt, n);
          END;
        ELSE
          out.WrNL;
          out.WrSpc (lev);
          out.WrFmt ("else");
          out.WrNL;
          dcl.OutLineNo (n.pos, lev+1);
          out.WrSpc (lev+1);
          out.WrFmt ("%s(", nms.x2c[nms.nm_trap]^);
          out.WrFmt ("%s);", nms.x2c[nms.nm_case_trap]^);
        END;
      END;
      def.OutComment ("CASE");
      nl := FALSE;

  | pc.nd_eval:
      IF dcl.IsCarray (n.l.type)
       THEN exp.GenValue (n.l, -1, {dcl.REF, dcl.ANY});
       ELSE exp.GenValue (n.l, -1, {});
      END;
      out.WrFmt (";");

  | pc.nd_sproc:
      GenSproc (n, lev);

  | pc.nd_protect:
      dcl.ObjectUsage (n.obj, nm(*=>*));
      out.WrFmt ("%s(&%s,", nms.x2c[nms.nm_protect]^, nm);
      dcl.GenConstAggr (n.r.val, n.r.type, 0, FALSE, FALSE);
      out.WrFmt (");");
      out.WrNL;
      GenSequence (n.l, lev, dd(*=>*), voidrt(*=>*), FALSE, FALSE, loop);
      out.WrSpc (lev);
      out.WrFmt ("%s(&%s,%s);", nms.x2c[nms.nm_protect]^, nm, nm);
      out.WrNL;
      nl := FALSE;

  | pc.nd_except:
      dcl.ObjectUsage (n.obj, nm(*=>*));
      out.WrFmt ("if (%s(&%s)) {", nms.x2c[nms.nm_xtry]^, nm);
      out.WrNL;
      GenSequence (n.l, lev+1, dd(*=>*), voidrt(*=>*), nrt, trt, loop);
      IF ~dd THEN
        out.WrSpc (lev+1);
        out.WrFmt ("%s();", nms.x2c[nms.nm_xoff]^);
        out.WrNL;
      END;
      out.WrSpc (lev);
      out.WrChr ("}");
      out.WrNL;
      IF n.r # NIL THEN
        out.WrSpc (lev);
        out.WrFmt ("else ");
        out.WrNL;
        GenBracedStats (n.r, lev+1, dd(*=>*), nrt, trt, loop);
        out.WrNL;
      END;
      nl := FALSE;

  | pc.nd_retry:
      out.WrFmt ("%s();", nms.x2c[nms.nm_xretry]^);

  | pc.nd_finally:
      dcl.ObjectUsage (n.obj, nm(*=>*));
      out.WrFmt ("%s(%s);", nms.x2c[nms.nm_finally]^, nm);

  | pc.nd_reraise:
      out.WrFmt ("%s();", nms.x2c[nms.nm_xremove]^);

  | pc.nd_activate:
      out.WrFmt ("%s();", nms.x2c[nms.nm_xon]^);

  ELSE
    env.errors.Error (n.pos, 1012);
  END;

  Wrap := cc.op_snglstat OR NeedNL(n);
  IF nl & Wrap THEN
    IF cc.op_comments THEN
     cmt.OutTailComment (n.pos)
    END;
    out.WrNL;
  END;
  dcl.ExitStatement (tmps(*=>*));

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenStmt") END;
<* END *>
END GenStatement;

-----------------------------------------------------------------------------

PROCEDURE GenSequence (      n: pc.NODE
                      ;    lev: INTEGER
                      ; VAR dd: BOOLEAN    -- dead end
                      ; VAR rt: BOOLEAN    -- last statement was "return"
                      ;    nrt: BOOLEAN    -- do not gen return
                      ;    trt: BOOLEAN    -- do gen return
                      ;   loop: pc.NODE ); -- for, do, while
(*
   nrt - оператор является последним в блоке: можно не генерить
		goto в операторе RETURN
   trt - по RETURN нужно выйти из процедуры оператором return,
		нет кода, который нужно исполнить после RETURN но до
		выхода из тела C-процедуры
*)
BEGIN
  dd := FALSE;
  rt := FALSE;
  Wrap := TRUE;
  WHILE n # NIL DO
    GenStatement (n, lev, rt(*=>*), (n.next = NIL) & nrt, trt, loop);
    dd := pc.ntag_no_exit IN n.tags;
    n  := n.next;
  END;
END GenSequence;

-----------------------------------------------------------------------------

PROCEDURE GenCopyright ( v-, name-: ARRAY OF CHAR
                       ;     code: BOOLEAN );
(*
*)

VAR
  s  : env.String;
  l,b: STR;
  dt : tim.DateTime;
CONST month = "JanFebMarAprMayJunJulAugSepOctNovDec";

BEGIN
  env.config.Equation ("COPYRIGHT", s(*=>*));
  IF s # NIL THEN def.OutComment (s^) END;
  IF code & tim.CanGetClock()
   & env.config.Option ("GENDATE")
  THEN
    (*	!!!! желательно писать в h-файл время записи сим-файла,
	текущее время в h-файл писать нельзя - не работает сравнение.
    *)
    tim.GetClock (dt);
    s := pc.code.code_ext;
    IF s[0] = "."
     THEN l := ""
     ELSE l := "." END;
    out.StrFmt (b, '"@( # )%s%s%s %.3.*s %2d %2d:%02d:%02d %d"',
                name, l, s^, (dt.month-1)*3, month, dt.day, dt.hour,
                dt.minute, dt.second, dt.year);
                def.OutComment (b);
  END;
  IF    dcl.CurMod.type.flag = pc.flag_o2   THEN l := "Oberon-2";
  ELSIF dcl.CurMod.type.flag = pc.flag_sl1  THEN l := "SL1";
  ELSIF dcl.CurMod.type.flag = pc.flag_m2   THEN l := "Modula-2";
  ELSE l := "";
  END;
  IF l # "" THEN
    out.StrFmt (b, "Generated by XDS %s to %s translator", l, v);
    def.OutComment (b); out.WrNL;
  END;
END GenCopyright;


(*======================== CODE METHODS IMPLEMENTATION ======================*)

TYPE
  CODE     = POINTER TO code_rec;
  code_rec = RECORD (pc.code_rec)
    inited : BOOLEAN;
  END;

-----------------------------------------------------------------------------

PROCEDURE ( c: CODE ) inp_object (  f: xfs.SymFile
                                 ;  o: pc.OBJECT
                                 ; id: LONGINT );
(*
*)
BEGIN
  nms.InpObject (f, o);
END inp_object;

-----------------------------------------------------------------------------

PROCEDURE ( c: CODE ) skip_object (  f: xfs.SymFile
                                  ; id: LONGINT);
(*
*)
BEGIN
  nms.SkipObject (f);
END skip_object;

-----------------------------------------------------------------------------

PROCEDURE ( c: CODE ) out_object ( file: xfs.SymFile
                               ;    o: pc.OBJECT );
(*
*)
BEGIN
  IF o.ext = NIL
   THEN file.WriteInt (0);  -- "GENDLL+" assumed
   ELSE o.ext.out (file);
  END;
END out_object;

-----------------------------------------------------------------------------

PROCEDURE ( c: CODE ) inp_struct (  f: xfs.SymFile
                               ;  s: pc.STRUCT
                               ; id: LONGINT );
(*
*)
BEGIN
  nms.InpStruct (f, s);
END inp_struct;

-----------------------------------------------------------------------------

PROCEDURE ( c: CODE ) skip_struct (  f: xfs.SymFile
                                ; id: LONGINT);
(*
*)
BEGIN
  nms.SkipStruct (f);
END skip_struct;

-----------------------------------------------------------------------------

PROCEDURE ( c: CODE ) selected;
(*
*)
BEGIN
  env.config.SetOption ("__GEN_C__", TRUE);
  env.config.Equation  ("CODE",   c.code_ext(*=>*));
  env.config.Equation  ("HEADER", c.head_ext(*=>*));
END selected;

-----------------------------------------------------------------------------
PROCEDURE bit_size( mode: pc.TY_MODE ) : LONGINT;
BEGIN
  CASE mode OF
  | pc.ty_shortint,
    pc.ty_shortcard,
    pc.ty_char    :  RETURN 8;
  | pc.ty_integer,
    pc.ty_cardinal:  RETURN 16;
  | pc.ty_longint,
    pc.ty_longcard:  RETURN 32;

  | pc.ty_longlongint,
    pc.ty_longlongcard,
    pc.ty_ZZ      :
                     RETURN 64;
  | pc.ty_uchar   :  RETURN 16;
  END;
END bit_size;

PROCEDURE (c: CODE) set_min_value( t: pc.STRUCT; VAR v: pc.VALUE);
BEGIN
  IF t.mode IN pc.TY_SET{ pc.ty_range, pc.ty_enum,
                               pc.ty_array_of, pc.ty_SS,
                               pc.ty_ZZ }
  THEN
    v.set_integer(0);
    RETURN;
  ELSIF NOT( t.mode IN pc.BOUNDED_TYPES ) THEN
    v := NIL;
    RETURN;
  END;

  CASE t.mode OF
  | pc.ty_boolean  : v.set_integer(0);
<* IF NOFLOAT THEN *>
  | pc.ty_real,
  | pc.ty_longreal,
  | pc.ty_ld_real  : v := NIL;
<* ELSE *>
  | pc.ty_real     :
    v.set_real( MIN(REAL) );
  | pc.ty_longreal,
    pc.ty_ld_real  :
    v.set_real( MIN(LONGREAL) );
<* END *>
  ELSE
    IF t.mode IN pc.ORDs THEN
      v.get_min(bit_size(t.mode), t.mode IN pc.SIGNED_WHOLEs);
    ELSE
      v.set_integer(0);
    END;
  END;
  RETURN;
END set_min_value;

PROCEDURE (c: CODE) set_max_value( t: pc.STRUCT; VAR v: pc.VALUE);
BEGIN
  IF t.mode IN pc.TY_SET{ pc.ty_range, pc.ty_enum,
                               pc.ty_array_of, pc.ty_SS,
                               pc.ty_ZZ }
  THEN
    v.set_integer(0);
    RETURN;
  ELSIF NOT( t.mode IN pc.BOUNDED_TYPES ) THEN
    v := NIL;
    RETURN;
  END;

  CASE t.mode OF
  | pc.ty_boolean  : v.set_integer(1);
<* IF NOFLOAT THEN *>
  | pc.ty_real,
    pc.ty_longreal,
    pc.ty_ld_real  : v := NIL;
<* ELSE *>
  | pc.ty_real     :
    v.set_real( MAX(REAL) );
  | pc.ty_longreal,
    pc.ty_ld_real  :
    v.set_real( MAX(LONGREAL) );
<* END *>
  ELSE
    IF t.mode IN pc.ORDs THEN
      v.get_min(bit_size(t.mode), t.mode IN pc.SIGNED_WHOLEs);
    ELSE
      v.set_integer(0);
    END;
  END;
  RETURN;
END set_max_value;

-----------------------------------------------------------------------------

PROCEDURE (c: CODE) ini;
  VAR val : pc.STRING;  i : LONGINT;
BEGIN
  IF ~c.inited THEN
    c.inited      := TRUE;
    rec.WORD_BITS := 0;

<* IF NODE_VALUE_REPRESENTATION THEN *>
    zz_tmp := ir.NewIntValue (0);
    zz_gcr := ir.NewIntValue (0);
    zz_abs_stp := ir.NewIntValue (0);
<* ELSE *>
    zz_tmp := dcl.NewIntValue (0);
    zz_gcr := dcl.NewIntValue (0);
    zz_abs_stp := dcl.NewIntValue (0);
<* END *>

    c.bits_per_loc := 8;
    c.locs_per_word:= 4;

    c.FRETs := pc.CNUMs+pc.SETs+pc.ORDs+pc.VPTRs+
               pc.TY_SET{pc.ty_array, pc.ty_loc, pc.ty_protection,
                 pc.ty_proctype, pc.ty_record};

<* IF NODE_VALUE_REPRESENTATION THEN *>
    c.max_loc   := ir.NewValNode (ir.NewIntValue (ASH (1, c.bits_per_loc)-1),
                                                   ir.TypZZ);
    c.min_sint  := ir.NewValNode (ir.NewIntValue (MIN (SHORTINT)), ir.TypZZ);
    c.max_sint  := ir.NewValNode (ir.NewIntValue (MAX (SHORTINT)), ir.TypZZ);
    c.min_int   := ir.NewValNode (ir.NewIntValue (MIN (INTEGER)),  ir.TypZZ);
    c.max_int   := ir.NewValNode (ir.NewIntValue (MAX (INTEGER)),  ir.TypZZ);
    c.min_lint  := ir.NewValNode (ir.NewIntValue (MIN (LONGINT)),  ir.TypZZ);
    c.max_lint  := ir.NewValNode (ir.NewIntValue (MAX (LONGINT)),  ir.TypZZ);
    c.max_scard := ir.NewValNode (ir.NewIntValue (0), ir.TypZZ);
    c.max_scard.val.binary (pc.sb_minus, c.max_sint.val, c.min_sint.val);
    c.max_card  := ir.NewValNode (ir.NewIntValue (0), ir.TypZZ);
    c.max_card.val.binary (pc.sb_minus, c.max_int.val, c.min_int.val);
    c.max_lcard := ir.NewValNode (ir.NewIntValue (0), ir.TypZZ);
    c.max_lcard.val.binary (pc.sb_minus, c.max_lint.val, c.min_lint.val);
    c.max_char  := ir.NewValNode (ir.NewIntValue (255), ir.TypZZ);
    c.max_real  := ir.NewValNode (ir.NewRealValue (MAX (REAL)),    ir.TypRR);
    c.min_real  := ir.NewValNode (ir.NewRealValue (MIN (REAL)),    ir.TypRR);
    c.max_lreal := ir.NewValNode (ir.NewRealValue (MAX (LONGREAL)), ir.TypRR);
    c.min_lreal := ir.NewValNode (ir.NewRealValue (MIN (LONGREAL)), ir.TypRR);
    IF c.index16
     THEN c.max_index := ir.NewValNode (ir.NewIntValue (0FFFFH),      ir.TypZZ);
     ELSE c.max_index := ir.NewValNode (ir.NewIntValue (MAX (LONGINT)), ir.TypZZ);
    END;
<* ELSE *>
    IF c.index16
     THEN c.max_index := dcl.NewIntValue (0FFFFH);
     ELSE c.max_index := dcl.NewIntValue (MAX (LONGINT));
    END;
<* END *>

    c.max_dim     := 8;
    c.max_ext_lev := 15;
    c.def_storage := TRUE;
    c.en_tmpvar   := TRUE;
    c.en_f_inline := FALSE;  (* do not inline functions in expressions -- LAZ *)
    c.max_sysflag := pc.flag_syscall;
    c.en_preopt   := ~env.config.Option ("NOOPTIMIZE");
    c.int16       := cc.op_int16;
    c.index16     := cc.op_index16;
    c.address16   := cc.op_address16;

    env.config.Equation("ALIGNMENT", val);
    IF (val = NIL) THEN -- ALIGNMENT not set
      default_alignment := 1;
    ELSIF NOT(xcStr.StrToInt(val^,i) & (i IN {0,1,2,4,8,16})) THEN
      env.errors.Fault(env.null_pos,450,"Invalid ALIGNMENT value specified");
    ELSE
      IF i # 0 THEN
        default_alignment := VAL(SHORTINT, i);
      ELSE
        default_alignment := 1;
      END;
    END;

  END;

  cc.ini;
  dcl.ini;
  def.ini;
  exp.ini;
  nms.ini;
  seg.ini;
END ini;

-----------------------------------------------------------------------------

PROCEDURE ( c: CODE ) exi;
(*
*)
BEGIN
  seg.exi;
  nms.exi;
  exp.exi;
  def.exi;
  dcl.exi;
  cc.exi;
END exi;


(*===========================================================================*)

PROCEDURE SetCu ( m: pc.OBJECT );
(*
*)
BEGIN
  dcl.CurMod    := m;
  rec.WORD_BITS := 32;
END SetCu;

-----------------------------------------------------------------------------

PROCEDURE GenIncludes (  imp: BOOLEAN
                      ; main: BOOLEAN );
(*
   Puts #includes list into a separate segment to insert into proper place
*)

VAR
  ext      : env.String;
  conv,enif: BOOLEAN;
  -----------------------------------------------

  PROCEDURE Gen (      nm-: ARRAY OF CHAR
                ; own,cstd: BOOLEAN );
  (*
     own - header file is generated by XDS
  *)
  VAR
    fnm    : env.String;
    ch0,ch1: CHAR;
    s      : ARRAY 4 OF CHAR;
  BEGIN
    IF ~ own & cstd
     THEN ch0 := '<'; ch1 := '>';
     ELSE ch0 := '"'; ch1 := '"';
    END;
    IF own & enif THEN
      out.WrFmt ("#ifndef %s_H_", nm);
      out.WrNL;
    END;
    IF conv & own THEN
      xfs.sys.Create ("", nm, ext^, fnm(*=>*));
      xfs.sys.ConvertToTarget (fnm^, fnm(*=>*));
      out.WrFmt ('#include %c%s%c', ch0, fnm^, ch1);
    ELSE
      IF ext[0] = "."
       THEN s := ""
       ELSE s := "."
      END;
      out.WrFmt ('#include %c%s%s%s%c', ch0, nm, s, ext^, ch1);
    END;
    out.WrNL;
    IF own & enif THEN
      out.WrFmt ("#endif");
      out.WrNL;
    END;
  END Gen;
-------------------------------------------------

VAR
  nm: STR;
  i : LONGINT;
  u : pc.USAGE;

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(GenIncludes") END;
<* END *>

  enif := dcl.CurMod.type.flag IN pc.LangSet{pc.flag_o2, pc.flag_m2, pc.flag_sl1};
  IF dcl.CurMod.type.flag = pc.flag_o2 THEN
    main := FALSE
  END;
  IF (dcl.CurMod.type.flag = pc.flag_sl1)
   & (~ imp OR main)
  THEN
    out.WrFmt ("#define X2C_SL1");
    out.WrNL;
  END;
  IF imp & (dcl.CurMod.type.flag IN pc.LangSet{pc.flag_o2, pc.flag_m2}) THEN
    IF cc.op_int16
     THEN out.WrFmt ("#define X2C_int16");
     ELSE out.WrFmt ("#define X2C_int32");
    END;
    out.WrNL;
    IF cc.op_debug   THEN
          out.WrFmt ("#define X2C_DEBUG");
          out.WrNL;
    END;
    IF cc.op_index16
     THEN out.WrFmt ("#define X2C_index16");
     ELSE out.WrFmt ("#define X2C_index32");
    END;
    out.WrNL;
  END;
  env.config.Equation ("HEADER", ext(*=>*));
  conv := env.config.Option ("CONVHDRNAME");
  IF ~ imp OR main THEN
      Gen ("X2C", TRUE, FALSE);
  END;
  IF imp & (cc.otag_headerfile IN dcl.CurMod.tags) THEN
    dcl.ObjectUsage (dcl.CurMod, nm(*=>*));
    Gen (nm, cc.ttag_ownheader IN dcl.CurMod.type.tags,
             cc.ttag_cstdlib   IN dcl.CurMod.type.tags);
  END;
  IF imp THEN                        --$$$ PR244
    (* probably, this can be do not, see ccDef *)
    u := dcl.CurMod.type.use;
    WHILE u # NIL DO
      dcl.mod_usage[u.obj.mno] := TRUE;
      u := u.next;
    END;
  END;
  IF imp THEN
    dcl.ObjectUsage (dcl.CurMod, nm(*=>*));
    out.WrFmt ("#define %s_C_", nm);
    out.WrNL;
  END;
  FOR i := 0 TO LEN (dcl.mod_usage^)-1 DO
    IF  dcl.mod_usage[i]
     & (i # dcl.CurMod.mno)
    THEN
      IF cc.otag_headerfile IN pc.mods[i].tags THEN
        dcl.ObjectUsage (pc.mods[i], nm (*=>*));
        Gen (nm, cc.ttag_ownheader IN pc.mods[i].type.tags,
                 cc.ttag_cstdlib   IN pc.mods[i].type.tags);
      ELSIF imp THEN
        dcl.ModuleBodyDef (pc.mods[i], FALSE);
        out.WrChr (";");
        out.WrNL;
      END;
    END;
  END;
  out.WrNL;
  (* generate in header file only *)
  IF ~imp & ~main & cc.op_gendll THEN (*Ned v2.12 *)
    dcl.ObjectUsage (dcl.CurMod, nm(*=>*));
    out.WrFmt ("#undef %s", nms.x2c[nms.nm_dll_tag]^);
    out.WrNL;
    out.WrFmt ("#ifdef %s", nms.x2c[nms.nm_gen_dll]^);
    out.WrNL;
    out.WrFmt ("#ifdef %s_C_", nm);
    out.WrNL;
    out.WrFmt ("#define %s %s", nms.x2c[nms.nm_dll_tag]^, nms.x2c[nms.nm_dll_export]^);
    out.WrNL;
    out.WrFmt ("#else");
    out.WrNL;
    out.WrFmt ("#define %s %s", nms.x2c[nms.nm_dll_tag]^, nms.x2c[nms.nm_dll_import]^);
    out.WrNL;
    out.WrFmt ("#endif");
    out.WrNL;
    out.WrFmt ("#else");
    out.WrNL;
    out.WrFmt ("#define %s", nms.x2c[nms.nm_dll_tag]^);
    out.WrNL;
    out.WrFmt ("#endif");
    out.WrNL;
    out.WrNL;
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenIncludes") END;
<* END *>
END GenIncludes;

-----------------------------------------------------------------------------

PROCEDURE ( c: CODE ) gen_code (   cu: INTEGER
                               ; main: BOOLEAN );
(*
*)
VAR
  nm  : STR;
  i   : INTEGER;
  text: out.SEGM;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd(0,"------------ gen_code ------------") END;
<* END *>

  dcl.cur_proc := NIL;
  SetCu (pc.mods[cu]);
  nms.EnableCount (dcl.CurMod.type.flag = pc.flag_sl1);
  NEW (dcl.mod_usage, pc.mod_cnt);
  FOR i := 0 TO SHORT (LEN (dcl.mod_usage^)-1) DO
    dcl.mod_usage[i] := FALSE;
  END;
  nms.DoSymList;
  out.ini;
  cmt.ini;

  def.GenDefinitions (main);
  out.GetSegment (text);

  dcl.ObjectUsage (dcl.CurMod, nm(*=>*));
  GenCopyright (c.vers, nm, TRUE);
  (* Must be after "GenDefinitions", see "mod_usage" *)
  GenIncludes (TRUE, main); -- moved to ccDef
  IF dcl.CurMod.type.flag = pc.flag_sl1 THEN
    out.WrFmt ("extern void %s_start() {}", dcl.CurMod.name^);
    out.WrNL;
  END;

  out.WriteSegm (text);
  IF dcl.CurMod.type.flag = pc.flag_sl1 THEN
    out.WrFmt ("extern void %s_end() {}", dcl.CurMod.name^);
    out.WrNL;
  END;
  cmt.exi;
  out.WriteFile (nm, TRUE, 0);
  out.exi;
END gen_code;

-----------------------------------------------------------------------------

PROCEDURE ( c: CODE ) allocate (       cu: INTEGER
                               ;     main: BOOLEAN
                               ; src_time: xfs.Time);
(*
*)
VAR
  nm  : STR;
  text: out.SEGM;
  i   : INTEGER;

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd(0, "---------------- allocate ----------------") END;
<* END *>

  SetCu (pc.mods[cu]);
  IF env.config.Option ("VERSIONKEY") THEN
    INCL (dcl.CurMod.tags, cc.otag_versionkey);
  END;
  nms.EnableCount (dcl.CurMod.type.flag = pc.flag_sl1);
  dcl.cur_proc := NIL;
  nms.DoSymList;
  IF ~cc.op_noinclude THEN
    out.ini;
    cmt.ini;
    NEW (dcl.mod_usage, pc.mod_cnt);
    FOR i := 0 TO SHORT (LEN (dcl.mod_usage^)-1) DO
      dcl.mod_usage[i] := FALSE;
    END;

    def.GenPublics;
    out.GetSegment (text);

    dcl.ObjectUsage (dcl.CurMod, nm(*=>*));

    GenCopyright (c.vers, nm, FALSE);
    out.WrFmt ("#ifndef %s_H_", nm);
    out.WrNL;
    out.WrFmt ("#define %s_H_", nm);
    out.WrNL;

    GenIncludes (FALSE, FALSE); -- moved to ccDef

    out.WriteSegm (text);
    out.WrNL;
    out.WrFmt ("#endif /* %s_H_ */", nm);
    out.WrNL;
    INCL (dcl.CurMod.tags, cc.otag_headerfile);
    cmt.exi;
    IF ~cc.op_noheader THEN
      out.WriteFile (nm, FALSE, src_time);
      INCL (dcl.CurMod.type.tags, cc.ttag_ownheader);
    ELSE
      INCL (dcl.CurMod.type.tags, cc.ttag_nocode);
      IF cc.op_cstdlib THEN
        INCL (dcl.CurMod.type.tags, cc.ttag_cstdlib);
      END;
      IF ~ (dcl.CurMod.type.flag IN cc.C_LIKE_FLAGS) THEN
        env.errors.Warning (dcl.CurMod.type.pos, 351);
      END;
    END;
    IF cc.op_nocode THEN
      INCL (dcl.CurMod.type.tags, cc.ttag_nocode);
      IF ~ (dcl.CurMod.type.flag IN cc.C_LIKE_FLAGS) THEN
        env.errors.Warning (dcl.CurMod.type.pos, 352);
      END;
    END;
    out.exi;
  END;
END allocate;

---------------------- SIZE & FIELDOFS routines ------------------------

PROCEDURE mk_align(VAR offs: LONGINT; align: SHORTINT; f : pc.OBJECT);
  VAR r: LONGINT;
BEGIN
  IF align > default_alignment THEN
    env.errors.Warning(f.pos, 325, f.name^, align, default_alignment);
  END;
  r := offs MOD align;
  IF r # 0 THEN INC(offs, align-r) END;
END mk_align;

PROCEDURE ^ bytes(t: pc.STRUCT; VAR sz : LONGINT; VAR align : SHORTINT);

VAR fld_OfsObject : pc.OBJECT;
    fld_found     : BOOLEAN;
    fld_Offset    : LONGINT;
    Size_Undefined: BOOLEAN;

PROCEDURE alloc_field(f : pc.OBJECT;
               VAR offs : LONGINT;
              VAR align : SHORTINT);
  VAR field_size: LONGINT; field_align: SHORTINT;
BEGIN
  IF ~cc.op_gensize THEN
    Size_Undefined := TRUE;
    RETURN;
  END;
  field_align := align;
  bytes(f.type, field_size, field_align);
  IF Size_Undefined THEN RETURN; END;
  IF field_align > align THEN
    env.errors.Warning(f.pos, 324, f.name^, field_align, align);
  ELSIF field_align < align THEN
    align := field_align;
  END;
  IF offs <= MAX(LONGINT) - field_align THEN
    mk_align(offs, field_align, f);
  ELSE
    Size_Undefined := TRUE;
    RETURN;
  END;
  IF f = fld_OfsObject THEN
    fld_found := TRUE;
    fld_Offset := offs;
  END;
  IF offs <= MAX(LONGINT) - field_size THEN
    INC(offs, field_size);
  ELSE
    Size_Undefined := TRUE;
    RETURN;
  END;
END alloc_field;

PROCEDURE alloc_flist (f : pc.OBJECT;
                VAR offs : LONGINT;
               VAR align : SHORTINT);
  VAR n : pc.NODE;
    mx_size, l_size : LONGINT;
    list_align, mx_align, l_align : SHORTINT;
BEGIN
  list_align := 1;
  WHILE f # NIL DO
    IF f.attr # NIL THEN
        mx_size := rec.Bytes(f);
        ASSERT(mx_size > 0);
        IF f = fld_OfsObject THEN
          fld_found := TRUE;
          fld_Offset := offs;
        END;
        INC(offs, mx_size);
    ELSIF f.mode = pc.ob_field THEN
      l_align := align;
      alloc_field(f, offs, l_align);
      IF Size_Undefined THEN RETURN; END;
      IF l_align > list_align THEN list_align := l_align END;
    ELSIF f.mode = pc.ob_field_bts THEN
      Size_Undefined := TRUE;
      RETURN;
    ELSIF f.mode = pc.ob_header THEN
      IF f.val.obj # NIL THEN
        l_align := align;
        alloc_field(f.val.obj, offs, l_align);
        IF Size_Undefined THEN RETURN; END;
        IF l_align > list_align THEN list_align := l_align END;
      END;
      mx_size := 0;
      mx_align := 1;
      n := f.val.l;
      WHILE n # NIL DO
        ASSERT(n^.mode = pc.nd_node);
        l_size := 0; l_align := align;
        alloc_flist(n.obj, l_size, l_align);
        IF Size_Undefined THEN RETURN; END;
        IF l_size > mx_size THEN mx_size := l_size END;
        IF l_align > mx_align THEN mx_align := l_align END;
        n := n.next;
      END;
      IF offs <= MAX(LONGINT) - mx_align THEN
        mk_align(offs, mx_align, f);
      ELSE
        Size_Undefined := TRUE;
        RETURN;
      END;

      IF offs <= MAX(LONGINT) - mx_size THEN
        INC(offs, mx_size);
      ELSE
        Size_Undefined := TRUE;
        RETURN;
      END;
      IF mx_align > list_align THEN list_align := mx_align END;
    ELSE
      ASSERT(FALSE); ---  invalid object mode
    END;
    f := f.next;
  END;
  align := list_align;
END alloc_flist;

PROCEDURE rec_size0 (t : pc.STRUCT; VAR size : LONGINT; VAR align : SHORTINT);
VAR
  base: pc.STRUCT;
  t_align, fl_align, b_align : SHORTINT;
BEGIN
(* -- IF pc.ttag_packed IN t.tags THEN ... END; *)
  size := 0;
  b_align := -1;
  base := t.base;
  t_align := t.align;
  IF t_align = 0 THEN t_align := align END;
  IF base # NIL THEN
    b_align := t_align;
    rec_size0(base, size, b_align);
    IF Size_Undefined THEN RETURN; END;
    ASSERT (size # 0);
  END;
  fl_align := t_align;
  alloc_flist(t.prof, size, fl_align);
  IF Size_Undefined THEN RETURN; END;
  IF (base # NIL) & (b_align > fl_align) THEN
    align := b_align;
  ELSE
    align := fl_align;
  END;
  IF size = 0 THEN size := 4; END;
END rec_size0;

PROCEDURE record_size (t : pc.STRUCT;
                VAR size : LONGINT; VAR align : SHORTINT);
BEGIN
  rec_size0(t, size, align);
  IF Size_Undefined THEN RETURN; END;
  ASSERT (size # 0);
  mk_align(size, align, t.obj);
END record_size;

-----------------------------------------------------------------------------
(*
   Calculate bytesize of type 't'
       (used to implement pc.code.get_size & get_offs methods).
   Size_Undefined=T if sizeof() must be generated by callee
       (enforced by ~cc.op_gensize)
*)

PROCEDURE bytes(t: pc.STRUCT; VAR sz : LONGINT; VAR align : SHORTINT);
  VAR n_align, t_align : SHORTINT;
BEGIN
  IF NOT cc.op_gensize &
     (t.mode IN pc.TY_SET{pc.ty_real, pc.ty_longreal,
                          pc.ty_complex, pc.ty_lcomplex,
                          pc.ty_protection, pc.ty_opaque, pc.ty_pointer,
                          pc.ty_proctype})
  THEN
    Size_Undefined := TRUE;
    RETURN;
  END;
  t_align := t.align;
  IF t_align = 0 THEN t_align := align END;
  CASE t.mode OF
  | pc.ty_shortcard, pc.ty_shortint :
      sz      := 1;
      n_align := 1;
  | pc.ty_cardinal, pc.ty_integer :
      sz      := 2;
      n_align := 2;
  | pc.ty_longcard, pc.ty_longint :
      sz      := 4;
      n_align := 4;
  | pc.ty_longlongint, pc.ty_longlongcard :
      sz      := 8;
      n_align := 8;
  | pc.ty_real :
      sz      := 4;
      n_align := 4;
  | pc.ty_longreal :
      sz      := 8;
      n_align := 8;
  | pc.ty_complex :
      sz      := 8;
      n_align := 4;
  | pc.ty_lcomplex :
      sz      := 16;
      n_align := 8;
  | pc.ty_boolean, pc.ty_range, pc.ty_enum :
      n_align := t_align;
      bytes(t.base, sz, n_align);
  | pc.ty_protection :
      sz      := 2;
      n_align := 2;
  | pc.ty_char :
      sz      := 1;
      n_align := 1;
  | pc.ty_opaque, pc.ty_pointer :
      sz      := 4;
      n_align := 4;
  | pc.ty_set :
      IF t.inx#NIL THEN
        n_align := t_align;
        bytes(t.inx, sz, n_align);
      ELSIF dcl.IsLongSet (t) THEN
        sz      := (t.len+dcl.LSET_BITS-1) DIV dcl.LSET_BITS * 
                                                 (dcl.LSET_BITS DIV 8);
        n_align := VAL(SHORTINT, (dcl.LSET_BITS DIV 8));
      ELSIF t.len <= 8 THEN
        sz      := 1;
        n_align := 1;
      ELSIF t.len <= 16 THEN
        sz      := 2;
        n_align := 2;
      ELSE
        sz      := 4;
        n_align := 4;
      END;
  | pc.ty_proctype :
      sz      := 4;
      n_align := 4;
  | pc.ty_array :
      bytes(t.base, sz, align);
      IF Size_Undefined THEN RETURN; END;
      IF t.len <= MAX(LONGINT) DIV sz THEN
        sz := sz * t.len;
      ELSE
        Size_Undefined := TRUE;
      END;
      RETURN;
  | pc.ty_record :
      record_size(t, sz, align); RETURN
  | pc.ty_loc :
      sz      := 1;
      n_align := 1;
  | pc.ty_SS :
      sz      := t.len;
      n_align := 1;
  ELSE
    Size_Undefined := TRUE;
    RETURN;
  END;
  IF t_align > n_align THEN t_align := n_align END;
  IF t_align < align   THEN align   := t_align END;
END bytes;

PROCEDURE (c: CODE) get_offs(op: pc.SUB_MODE; o: pc.OBJECT): LONGINT;
  VAR sz: LONGINT; align : SHORTINT;
BEGIN
  ASSERT( o.mode=pc.ob_field );   --- Only for FIELDOFS
  ASSERT( o.host # NIL );
  fld_found     := FALSE;
  fld_OfsObject := o;
  fld_Offset    := -1;
  Size_Undefined := FALSE;
  bytes(o.host, sz, align);
  IF Size_Undefined THEN RETURN -1; END;
  ASSERT(fld_found=TRUE);
  ASSERT(fld_Offset >= 0);
  CASE op OF
    | pc.su_bit_offs : RETURN fld_Offset*8;
    | pc.su_byte_offs: RETURN fld_Offset;
    | pc.su_word_offs: RETURN (fld_Offset+3) DIV 4;
  END;
END get_offs;

PROCEDURE (c: CODE) get_size(op: pc.SUB_MODE; t: pc.STRUCT): LONGINT;
  VAR sz: LONGINT; align : SHORTINT;
BEGIN
  align := default_alignment;
  Size_Undefined := FALSE;
  bytes(t, sz, align);
  IF Size_Undefined THEN RETURN -1; END;
  ASSERT(sz > 0);
  CASE op OF
    |pc.su_bits : RETURN sz*8;
    |pc.su_bytes: RETURN sz;
    |pc.su_size : RETURN sz;
    |pc.su_words: RETURN (sz+3) DIV 4;
  END;
END get_size;

----------------------------------------------------------------------

PROCEDURE Set*;
(*
*)
VAR code: CODE;

BEGIN
  NEW (code);
  pc.code := code;
  code.inited := FALSE;
  code.sym_ident := pc.sym_C;
  code.valid_idents := {pc.sym_C - pc.sym_base};

  COPY (cc.vers, code.vers);
  cc.DeclareOptions;
  code.selected;
END Set;
-----------------------------------------------------------------------------

BEGIN
  zz_tmp := NIL;
  zz_gcr := NIL;
  zz_abs_stp := NIL;
  def.GenSequence := GenSequence;
END ccCode.
