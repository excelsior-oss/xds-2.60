(** Copyright (c) 1993,96 XDS Ltd, Russia. All Rights Reserved. *)
(** Expressions *)
MODULE ccExpr;
<* IF ~ DEFINED(NODE_VALUE_REPRESENTATION) THEN *> <* NEW NODE_VALUE_REPRESENTATION- *>  <* END *>

(* Modifications:
   20/Apr/98 Vit  5.04  @2: NODE-VALUE representation change
   17/Apr/98 Vit  5.03  @1: ==0 comparison in IF is restored
   05/Apr/98 Vit  5.00  Comments inserted; some code/naming clarification done.
			Renamed from ccE
   (XDS versions below, BE - above)
   29-Mar-96 Ned        back to RemStrExtension (see fix in pcConst)
   27-Mar-96 Ned        PRO0046: GenAssign.
                        is_string is used instead of RemStrExtension
   24-Mar-96 Ned        <*IF extvalue*> is deleted.
                        GenValueUnaryConv: ty_SS -> ordinal (for genconst)
   17/Mar/96 Ned  2.12  BNRP specific code is removed: GENNOBITREF,
                        bnrp_set_aggregate, set operations
*)

IMPORT
  pc  := pcK,
  cc  := ccK,
  nms := ccNames,
  out := ccOut,
  dcl := ccDcl,
  xfs := xiFiles,
<* IF DB_TRACE THEN *>
  dbg := pcVisIR,
<* END *>
  env := xiEnv,
  SYSTEM;

TYPE
  STR = out.STR;

VAR
  zz_tmp     : pc.VALUE;
  zz_128     : pc.VALUE;
  zz_m32     : pc.VALUE;
  zz_32      : pc.VALUE;
  zz_one     : pc.VALUE;
  zz_gvm     : pc.VALUE; (* is used in GenValueMinusMin *)

CONST
  LEN_AVAIL = pc.TY_SET{pc.ty_array, pc.ty_SS, pc.ty_set};


----------------------------------------------------------------------------

PROCEDURE ^ GenValue (  n: pc.NODE
                     ;  p: INTEGER
                     ; md: SET);

PROCEDURE ^ GenValueBinary (  n: pc.NODE
                           ;  p: INTEGER
                           ; md: SET);

PROCEDURE ^ GenSize (   n: pc.NODE
                    ; dim: INTEGER
                    ; p: INTEGER);

----------------------------------------------------------------------------

PROCEDURE IsResultOfCptrDeref ( l: pc.NODE )  :  BOOLEAN;
(*
*)
BEGIN
  WHILE (l.mode = pc.nd_index)
      & (l.l.type.mode = pc.ty_array_of)
  DO
    l := l.l
  END;
  RETURN (l.mode = pc.nd_deref)
       & (l.l.type.flag = pc.flag_c);
END IsResultOfCptrDeref;

----------------------------------------------------------------------------

PROCEDURE GenTypeCast ( bs-: ARRAY OF CHAR
                      ;   t: pc.STRUCT);
(*
   Generates cast operatot for type 't'
*)
VAR str: STR;

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenTypeCast: t = %X .mode = %d", t, t.mode)
  END;
<* END *>

  dcl.TypeDesignator (str(*=>*), bs, t);
  out.WrFmt ("(%s)", str);

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenTypeCast") END;
<* END *>
END GenTypeCast;

----------------------------------------------------------------------------

PROCEDURE GetSequenceLen ( n: pc.NODE )  :  LONGINT;
(*
*)
VAR
  l : pc.NODE;
  sz: LONGINT;
BEGIN
  l  := n.l;
  sz := 0;
  WHILE l # NIL DO
    IF l.type.mode IN pc.REALs THEN
      INC (sz, 2)
    ELSIF (l.type.mode IN pc.SEQ_ARRs) & ~IsResultOfCptrDeref (l) THEN
      INC (sz, 3)
    ELSE INC (sz, 1)
    END;
    l := l.next;
  END;
  RETURN sz;
END GetSequenceLen;

----------------------------------------------------------------------------

PROCEDURE GenCheckNeg (  n: pc.NODE          -- expr node
                      ; md: SET              -- tags
                      ;  p: INTEGER          -- priority
                      )   :         BOOLEAN; -- F if nothing generated
(*
   Generate negation expr if dcl.NEG IN md (T) else do nothing (F)
*)
BEGIN
  IF ~(dcl.NEG IN md) THEN RETURN FALSE END;
(* was for GenValue: ASSERT({dcl.LVL, dcl.REF}*md = {}); *)
  IF p > 13 THEN out.WrChr ('(') END;
  out.WrChr ('!');
  GenValue (n, 13, md-{dcl.NEG});
  IF p > 13 THEN out.WrChr (')') END;
  RETURN TRUE;
END GenCheckNeg;

----------------------------------------------------------------------------

PROCEDURE GenCheckNil (   n: pc.NODE      -- node
                      ; adr: BOOLEAN );   -- T: address check, F: proc check
(*
   Generate a NIL check for pointer/procedure
*)
  PROCEDURE ChkMode ( t: pc.STRUCT ): BOOLEAN;
  BEGIN
    IF adr
     THEN RETURN (t.mode IN pc.ADRs)
     ELSE RETURN t.mode = pc.ty_proctype;
    END;
  END ChkMode;

VAR nm: STR;
    nn: SHORTINT;
BEGIN
  ASSERT( ChkMode (n.type) );
  IF adr
   THEN nn := nms.nm_a_chk;
   ELSE nn := nms.nm_p_chk;
  END;
  dcl.TypeDesignator (nm(*=>*), "", n.type);
  out.WrFmt ("%s(%s,", nms.x2c[nn]^, nm);
  WHILE (n.mode = pc.nd_unary)
      & (n.sub = pc.su_conv)
      & ChkMode (n.l.type)
  DO
    n := n.l;
  END;
  GenValue (n, 0, {});
  out.WrChr (')');
END GenCheckNil;

----------------------------------------------------------------------------

PROCEDURE GenLenObj (    n: pc.NODE
                    ;  dim: LONGINT
                    ; type: pc.STRUCT);
(*
   Generate length of object, refernced by node n (.obj # NIL)
*)
VAR
  t : pc.STRUCT;
  nm: STR;
  i : LONGINT;
BEGIN
  t := n.obj.type;
  FOR i := 0 TO dim-1 DO
    ASSERT( t.mode IN pc.ARRs );
    t := t.base;
  END;

  IF t.mode IN LEN_AVAIL THEN
    dcl.GenCnumber (t.len, type);

  ELSIF t.mode = pc.ty_array_of THEN
    ASSERT( n.obj.type.mode = pc.ty_array_of );
    IF n.obj.type.flag IN cc.C_LIKE_FLAGS THEN
      env.errors.Error (n.pos, 1006, dim);
    ELSE
      dcl.OSecondName (n.obj, SHORT(dim), "_len", nm(*=>*));
      out.WrStr (nm);
    END;

  ELSE
    env.errors.Error (n.pos, 1006, dim);
  END;
END GenLenObj;

----------------------------------------------------------------------------

PROCEDURE GenLen* (    n: pc.NODE
                  ;  dim: LONGINT
                  ; type: pc.STRUCT
                  ;    p: INTEGER);
(*
   dim is indexation number, left to right
*)
VAR
  t,f: pc.STRUCT;
  i,j: LONGINT;
BEGIN
  t  := n.type;
  FOR i := 0 TO dim-1 DO
    ASSERT( t.mode IN pc.ARRs );
    t := t.base;
  END;

  IF t.mode IN LEN_AVAIL THEN
    dcl.GenCnumber (t.len, type);

  ELSIF t.mode = pc.ty_array_of THEN
    CASE n.mode OF
    | pc.nd_var:
        GenLenObj (n, dim, type);

    | pc.nd_deref:
        IF n.l.type.flag = pc.flag_c THEN
          env.errors.Error (n.pos, 142)
        END;
        dcl.TypeDefinition (n.type);
        IF pc.ntag_chk_range IN n.tags
         THEN GenCheckNil (n.l, TRUE);
         ELSE GenValue (n.l, 14, {});
	END;
	out.WrFmt ("->%s%d", nms.x2c[nms.nm_dynarr_len]^, n.type.len-dim-1);

    | pc.nd_index:
        GenLen (n.l, dim+1, type, p);

    | pc.nd_unary
    , pc.nd_lconv:
        IF (n.mode = pc.nd_unary) & (n.sub # pc.su_conv) THEN
          env.errors.Error (n.pos, 1006, dim);
        END;
        (* for all dimensions but last of a returned open array
           proper dimension must exist in cast operand.
           these dimensions length match original length.
           last dimension length is recalculated
        *)
        f := n.l.type;
        FOR i := 0 TO dim-1 DO
          ASSERT( f.mode IN pc.ARRs );
          f := f.base;
        END;
        IF (t.len > 1)
        OR (f.mode IN pc.ARRs)
         & (f.base = t.base)
        THEN
          GenLen (n.l, dim, type, p);
        ELSIF  (f.mode = pc.ty_array_of)
           OR ~cc.op_gensize
        THEN
          IF pc.code.get_size (pc.su_bytes, t.base) = 1 THEN
            GenSize (n.l, SHORT(dim), 12);
          ELSE
            IF p >= 12 THEN out.WrChr ('(') END;
            GenSize (n.l, SHORT(dim), 12);
            out.WrChr ('/');
            dcl.GenSizeof (t.base);
            IF p >= 12 THEN out.WrChr (')') END;
          END;
        ELSE
          i := dcl.GetBytes (n.pos, f);
          j := dcl.GetBytes (n.pos, t.base);
          dcl.GenCnumber (i DIV j, type);
          IF (i MOD j) # 0 THEN
            env.errors.Error (n.pos, 1006, dim)
          END;
        END;

    | pc.nd_sequence:
	IF p >= 12 THEN out.WrChr ('(') END;
	i := GetSequenceLen (n);
        IF i # 1 THEN
          dcl.GenCnumber (i, type);
        END;
	IF i # 0 THEN
	  IF i # 1 THEN out.WrChr ('*') END;
          out.WrFmt ("sizeof(%s)", nms.x2c[nms.nm_seq_type]^);
          j := dcl.GetBytes (n.pos, n.type.base);
	  IF j # 1 THEN
            out.WrChr ('/');
            dcl.GenCnumber (j, type);
	  END;
	END;
	IF p >= 12 THEN out.WrChr (')') END;
    ELSE
      env.errors.Error (n.pos, 1006, dim);
    END;

  ELSE
    env.errors.Error (n.pos, 1006, dim);
  END;
END GenLen;

----------------------------------------------------------------------------

PROCEDURE GenTypeName ( t: pc.STRUCT
                      ; p: INTEGER);
(*
*)
VAR nm: STR;
BEGIN
  ASSERT( t.mode = pc.ty_record );
  ASSERT( t.flag = pc.flag_o2 );
  IF p > 13
    THEN out.WrChr ('(')
    ELSIF p = 13
    THEN out.WrChr (' ')
  END;
  dcl.OSecondName (t.obj, 1, "_desc", nm(*=>*));
  out.WrChr ('&'); out.WrStr (nm);
  IF p > 13 THEN out.WrChr (')') END;
END GenTypeName;

----------------------------------------------------------------------------

PROCEDURE GenType ( n: pc.NODE
                  ; p: INTEGER);
(*
*)
VAR
  nm: STR;
  t : pc.STRUCT;
BEGIN
  t := n.dynamic_type();
  IF t # NIL THEN
    GenTypeName (t, p);
    RETURN
  END;
  n := n.dynamic_type_expr();
  IF n.type.mode = pc.ty_pointer THEN
    out.WrFmt ("%s(", nms.x2c[nms.nm_get_td]^);
    GenValue (n, 0, {});
    out.WrChr (')');
  ELSIF (n.mode = pc.nd_var) & (n.obj.mode = pc.ob_varpar) THEN
    dcl.OSecondName (n.obj, 0, "_type", nm(*=>*)); out.WrStr (nm);
  ELSE
    ASSERT( FALSE );
  END;
END GenType;

----------------------------------------------------------------------------

PROCEDURE GenSizeUsage*  (   o: pc.OBJECT
                         ; dim: INTEGER
                         ;   p: INTEGER);
(*
*)
VAR
  nm: STR;
  t : pc.STRUCT;
  i : INTEGER;
  sz: LONGINT;
BEGIN
  ASSERT( o.type.mode = pc.ty_array_of );

  IF p >= 12 THEN out.WrChr ('(') END;
  t := o.type;
  i := 0;
  WHILE i < dim DO
    ASSERT( t.mode IN pc.ARRs );
    t := t.base;
    INC (i)
  END;
  WHILE t.mode = pc.ty_array_of DO
    dcl.OSecondName (o, i, "_len", nm(*=>*));
    IF i > dim THEN out.WrChr ('*') END;
    out.WrStr (nm);
    t := t.base;
    INC (i);
  END;
  sz := pc.code.get_size (pc.su_size, t);
  IF (sz # 1) OR (i = dim) THEN
    IF i > dim THEN out.WrChr ('*') END;
    dcl.GenSizeof (t);
  END;
  IF p >= 12 THEN out.WrChr (')') END;
END GenSizeUsage;

----------------------------------------------------------------------------

PROCEDURE GenSize* (   n: pc.NODE
                   ; dim: INTEGER
                   ;   p: INTEGER);
(*
   dim - количество индексаций
*)
VAR
  t: pc.STRUCT;
  i: INTEGER;
BEGIN
  t := n.type;
  FOR i := 0 TO dim-1 DO
    ASSERT( t.mode IN pc.ARRs );
    t := t.base
  END;
  IF t.mode # pc.ty_array_of THEN
    dcl.GenSizeof (t);
    RETURN
  END;
  IF dim < n.type.len THEN
    CASE n.mode OF
    | pc.nd_deref:
	IF dim = 0 THEN
	  IF p >= 12 THEN out.WrChr ('(') END;
          GenLen (n, 0, dcl.SIZE_T, 12); out.WrChr ('*'); GenSize (n, 1, 12);
	  IF p >= 12 THEN out.WrChr (')') END;
	ELSE
	  dcl.TypeDefinition (n.type);
          IF pc.ntag_chk_range IN n.tags
           THEN GenCheckNil (n.l, TRUE);
           ELSE GenValue (n.l, 14, {});
	  END;
	  out.WrStr ("->");
	  out.WrFmt ("%s%d", nms.x2c[nms.nm_dynarr_size]^, n.type.len-dim);
	END;
	RETURN;

    | pc.nd_index:
        GenSize (n.l, dim+1, p);
	RETURN;

    | pc.nd_var:
        GenSizeUsage (n.obj, dim, p);
	RETURN;
    ELSE (* nothing *)
    END;
  END;
  env.errors.Error (n.pos, 1007, dim);
END GenSize;

----------------------------------------------------------------------------

PROCEDURE GenFieldOfs(n: pc.NODE);
  VAR s  : STR;
      sz : LONGINT;
BEGIN
  IF(n.mode # pc.nd_field) THEN
     env.errors.Error(n.pos,1008);
     RETURN;
  END;
  ASSERT(n.obj.mode = pc.ob_field);
  sz:=pc.code.get_offs(pc.su_bytes,n.obj);
  IF sz<0 THEN
    dcl.TypeDesignator(s,"",n.obj.host);
    out.WrFmt("%s(%s,%s)",nms.x2c[nms.nm_field_ofs], s, n.obj.name^);
  ELSE
    dcl.GenCnumber(sz, dcl.SIZE_T);
  END;
END GenFieldOfs;

----------------------------------------------------------------------------

PROCEDURE GenMin* ( t: pc.STRUCT );
(*
   Generate MIN value for type
*)
VAR
  n : INTEGER;
  nm: STR;
BEGIN
  IF t.mode = pc.ty_set THEN t := t.base END;
  n := nms.Min (t);
  ASSERT( dcl.TypeUsage (nm(*=>*), t, FALSE, TRUE) ); -- type is std or named!
  out.WrFmt ("X2C_MIN(%s,", nm);
  IF n # nms.nm_empty
   THEN out.WrStr (nms.x2c[n]^);
<* IF NODE_VALUE_REPRESENTATION THEN *>
   ELSE GenValue (t.min, 0, {});
<* ELSE *>
   ELSE t.min.value_to_str(nm,pc.flag_c);
        out.WrStr (nm);
<* END *>
  END;
  out.WrChr (")");
END GenMin;

----------------------------------------------------------------------------

PROCEDURE GenMax* ( t: pc.STRUCT );
(*
   Generate MAX value for type
*)
VAR
  n : INTEGER;
  nm: STR;
BEGIN
  IF t.mode = pc.ty_set THEN t := t.base END;
  n := nms.Max (t);
  ASSERT( dcl.TypeUsage (nm(*=>*), t, FALSE, TRUE) ); -- type is std or named!
  out.WrFmt ("X2C_MAX(%s,", nm);
  IF n # nms.nm_empty
   THEN out.WrStr (nms.x2c[n]^);
<* IF NODE_VALUE_REPRESENTATION THEN *>
   ELSE GenValue (t.max, 0, {});
<* ELSE *>
   ELSE t.max.value_to_str(nm,pc.flag_c);
        out.WrStr (nm);
<* END *>
  END;
  out.WrChr (")");
END GenMax;

----------------------------------------------------------------------------

PROCEDURE GenValueUsageConv (           o: pc.OBJECT
                            ;           p: INTEGER
                            ; adr,ref,lvl: BOOLEAN);
(*
*)
VAR nm: STR;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenValueUsageConv: p=%d o = %X .mode = %s", p,o,dbg.VisObjMode[o.mode]);
  END;
<* END *>

  dcl.ObjectUsage (o, nm(*=>*));

  IF cc.otag_notype IN o.tags THEN -- cast to "no" (wrong) type
    IF p > 13 THEN out.WrChr ('(') END;
    IF ref THEN
      IF    adr THEN GenTypeCast ("*", o.type);
      ELSIF lvl THEN
        out.WrChr ("*");
        GenTypeCast ("*", o.type);
      ELSE
        GenTypeCast ("", o.type);
        out.WrChr ("*");
      END;
    ELSE
      IF adr THEN
        GenTypeCast ("*", o.type);
        out.WrChr ("&");
      ELSIF lvl THEN
        out.WrChr ("*");
        GenTypeCast ("*", o.type);
        out.WrChr ("&");
      ELSE GenTypeCast ("", o.type);
      END;
    END;
    out.WrStr (nm);
    IF p > 13 THEN out.WrChr (')') END;
  ELSE
    IF adr THEN
      IF    p > 13 THEN out.WrChr ('(')
      ELSIF p = 13 THEN out.WrChr (' ')
      END;
      out.WrFmt ("&%s", nm);
      IF p > 13 THEN out.WrChr (')') END;
    ELSE
      out.WrStr (nm);
    END;
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenValueUsageConv") END;
<* END *>
END GenValueUsageConv;

----------------------------------------------------------------------------

PROCEDURE GenValueUsage (  o: pc.OBJECT
                        ;  p: INTEGER
                        ; md: SET);
(*
*)
VAR bf: STR;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenValueUsage: p=%d o = %X .mode = %s", p,o,dbg.VisObjMode[o.mode]);
  END;
<* END *>

  ASSERT( ~(dcl.NEG IN md) );
  IF cc.op_cpp & (pc.otag_with IN o.tags) THEN
    ASSERT( o.type.mode = pc.ty_pointer );
    ASSERT( ~dcl.IsPtr (o, dcl.cur_proc) );
    ASSERT( ~(dcl.REF IN md) );
    ASSERT( ~(cc.otag_notype IN o.tags) );
    IF    p > 13 THEN out.WrChr ('(');
    ELSIF p = 13 THEN out.WrChr (' ');
    END;
    dcl.ObjectUsage (o, bf(*=>*));
    out.WrFmt ("&%s", bf);
    IF p > 13 THEN out.WrChr (')') END;
  ELSIF dcl.IsPtr (o, dcl.cur_proc) THEN
    IF dcl.REF IN md THEN
      GenValueUsageConv (o, p, FALSE, TRUE, dcl.LVL IN md);
    ELSE
      IF    p > 13 THEN out.WrChr ('(')
      ELSIF p = 12 THEN out.WrChr (' ')
      END;
      out.WrChr ('*');
      GenValueUsageConv (o, 13, FALSE, TRUE, dcl.LVL IN md);
      IF p > 13 THEN out.WrChr (')') END;
    END;
  ELSIF dcl.IsCarray (o.type) THEN
    (* для C ARRs всегда ганерится BSA, ничего другого сгенерить нельзя *)
    ASSERT( (dcl.REF IN md) & (dcl.ANY IN md) OR (dcl.BSA IN md) );
    ASSERT( ~(cc.otag_notype IN o.tags) );
    dcl.ObjectUsage (o, bf(*=>*));
    IF (cc.otag_bitfield IN o.tags) & (dcl.REF IN md) THEN
      IF    p > 13 THEN out.WrChr ('(')
      ELSIF p = 13 THEN out.WrChr (' ')
      END;
      out.WrChr ('&'); out.WrStr (bf);
      IF p > 13 THEN out.WrChr (')') END;
    ELSE
      out.WrStr (bf);
    END;
  ELSIF dcl.REF IN md THEN
    GenValueUsageConv (o, p, TRUE, FALSE, dcl.LVL IN md);
  ELSE
    GenValueUsageConv (o, p, FALSE, FALSE, dcl.LVL IN md);
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenValueUsage") END;
<* END *>
END GenValueUsage;

----------------------------------------------------------------------------

PROCEDURE GenValueNotChar (  n: pc.NODE
                          ;  p: INTEGER
                          ; md: SET);
(*
*)
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenValueNotChar: p=%d n = %X .mode = %s", p,n,dbg.VisNodMode[n.mode]);
  END;
<* END *>

  IF ~dcl.IsChar (n.type) THEN
    GenValue (n, p, md);
<* IF NODE_VALUE_REPRESENTATION THEN *>
  ELSIF (n.val # NIL) & dcl.CmpValue (pc.sb_lss, n.val, zz_128) THEN
<* ELSE *>
  ELSIF (n.mode = pc.nd_value) & dcl.CmpValue (pc.sb_lss, n.val, zz_128) THEN
<* END *>
    GenValue (n, p, md);
  ELSE
    ASSERT( md*{dcl.REF, dcl.LVL} = {} );
    IF p > 13 THEN out.WrChr ('(') END;
    GenTypeCast ("", n.type.super_type());
    GenValue (n, 13, md);
    IF p > 13 THEN out.WrChr (')') END;
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenValueNotChar") END;
<* END *>
END GenValueNotChar;

----------------------------------------------------------------------------

PROCEDURE GenCall* (  n: pc.NODE
                   ;  p: INTEGER
                   ; md: SET);
VAR
  t      : pc.STRUCT;
  rt,db  : BOOLEAN;
  tmp_nm : STR;
  l,ll   : pc.NODE;
  i,j    : LONGINT;
  nm     : STR;
  prm_buf: dcl.PRM_BUF;
  prm_cnt: INTEGER;
  fst    : BOOLEAN;
  this   : pc.NODE;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenCall: p=%d n = %X .mode = %s", p,n,dbg.VisNodMode[n.mode]);
  END;
<* END *>

  IF n.l # NIL
   THEN t := n.l.type
   ELSE t := n.obj.type
  END;
  rt := dcl.IsCarray (t.base); -- T, if result is returned via extra parameter

  ASSERT(  ~(dcl.LVL IN md) );
  ASSERT(   (dcl.REF IN md) & (dcl.ANY IN md)
         OR (dcl.BSA IN md)
         OR ~rt );

  IF (dcl.REF IN md) OR rt THEN
    dcl.MakeTempVar (t.base, tmp_nm(*=>*));
    IF ~rt THEN
      IF p >= 0 THEN out.WrChr ('(') END;
      out.WrFmt ("%s = ", tmp_nm);
    END;
  ELSE
    tmp_nm := "";
  END;
  db := cc.op_debug;
  IF (n.l # NIL)
   & (n.l.mode # pc.nd_method)
   & (pc.ntag_chk_range IN n.tags)
  THEN
    db := FALSE;
  END;
  IF db THEN
    out.WrFmt ("(%s() ", nms.x2c[nms.nm_set_hinfo]^)
  END;
  this := NIL;
  IF n.l = NIL THEN
    dcl.ObjectUsage (n.obj, nm(*=>*));
    out.WrStr (nm);
  ELSIF n.l.mode = pc.nd_method THEN
    this := n.l.l;
    dcl.OSecondName (t.obj, 1, "_", nm(*=>*));
    out.WrFmt ("%s(%s,", nms.x2c[nms.nm_call]^, nm);
    GenType (this, 0);
    out.WrFmt (",%d)", t.len);
  ELSIF pc.ntag_chk_range IN n.tags THEN
    GenCheckNil (n.l, FALSE);
  ELSE
    GenValue (n.l, 14, {});
  END;

  out.WrStr ('(');
  IF n.type.mode = pc.ty_void THEN -- indent PROC parameters by the 1st
    out.SaveIndent;
  END;
  dcl.FuncParams (prm_buf(*=>*), prm_cnt(*=>*), t);

  l   := n.r;
  ll  := NIL;
  fst := TRUE;
  j   := -1;
  FOR i := 0 TO prm_cnt-1 DO
    CASE prm_buf[i].mode OF

    | dcl.pm_return :
        ASSERT( tmp_nm # "" );
	IF fst
         THEN fst := FALSE
         ELSE out.WrStr (", ")
        END;
	out.WrStr (tmp_nm);

    | dcl.pm_threat :
        IF fst
         THEN fst := FALSE
         ELSE out.WrStr (", ")
        END;
        GenValueUsage (prm_buf[i].obj, 0, prm_buf[i].ref); j := 0;

    | dcl.pm_param  :
        IF fst
         THEN fst := FALSE
         ELSE out.WrStr (", ")
        END;
	IF this # NIL
         THEN ll := this; this := NIL;
         ELSE ll := l;    l    := l.next;
        END;
        IF cc.otag_notype IN prm_buf[i].obj.tags THEN -- wrong type
	  IF dcl.REF IN prm_buf[i].ref THEN
            dcl.TypeDesignatorNotype (nm(*=>*), "*", prm_buf[i].obj.type);
	  ELSIF dcl.LVL IN prm_buf[i].ref THEN
            dcl.TypeDesignatorNotype (nm(*=>*), "*", prm_buf[i].obj.type);
            out.WrChr ('*');
            INCL (prm_buf[i].ref, dcl.REF);
	  ELSE
            dcl.TypeDesignatorNotype (nm(*=>*), "",  prm_buf[i].obj.type);
	  END;
	  out.WrFmt ("(%s)", nm);
          GenValue (ll, 13, prm_buf[i].ref);
(* слишком грязное решение
	ELSIF (t.flag = pc.flag_c) &
	      (ll.type.mode = pc.ty_pointer) &
	      (ll.type.base.mode = pc.ty_loc) &
              ~cc.op_cpp
	THEN
          ( *    подавление контроля типов для рараметров типа ADDRESS,
		в C++ так делать нельзя!
          * )
	  out.WrStr("(void * )");
          GenValue(ll, 13, prm_buf[i].ref);
*)
        ELSE
          GenValue (ll, 0, prm_buf[i].ref);
	END;
	j := 0;

    | dcl.pm_len    :
        ASSERT( j >= 0 );
        out.WrStr (", ");
        GenLen (ll, j, prm_buf[i].obj.type.inx, 0);
        INC (j);

    | dcl.pm_frec   :
        IF fst
         THEN fst := FALSE
         ELSE out.WrStr (", ")
        END;
	IF this # NIL
         THEN ll := this; this := NIL
         ELSE ll := l;    l    := l.next
        END;
        GenValue (ll, 0, prm_buf[i].ref);

    | dcl.pm_type    :
        out.WrStr (", ");
        GenType (ll, 0);

    | dcl.pm_thr_len:
        IF fst
         THEN fst := FALSE
         ELSE out.WrStr (", ")
        END;
	dcl.OSecondName (prm_buf[i].obj, SHORT (j), "_len", nm(*=>*));
	out.WrStr (nm);
        INC (j);

    | dcl.pm_thr_type:
        IF fst
         THEN fst := FALSE
         ELSE out.WrStr (", ")
        END;
	dcl.OSecondName (prm_buf[i].obj, 0, "_type", nm(*=>*));
	out.WrStr (nm);

    | dcl.pm_seq:
        ASSERT( l.next = NIL );
	ASSERT( l.type.mode = pc.ty_array_of );
	ll := l.l;
	WHILE ll # NIL DO
	  IF fst
            THEN fst := FALSE
            ELSE out.WrStr (", ")
          END;
          IF dcl.IsCarray (ll.type) THEN
              GenValue (ll, 0, {dcl.REF, dcl.ANY, dcl.LVL});
	  ELSE
            GenValueNotChar (ll, 0, {});
	  END;
	  ll := ll.next;
	END;
    END;
  END;
  out.WrChr (")");
  IF db THEN out.WrChr (")") END;
  IF (dcl.REF IN md) & ~rt THEN
    out.WrFmt (",&%s", tmp_nm);
    IF p >= 0 THEN out.WrChr (')') END;
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenCall") END;
<* END *>
END GenCall;

--------------------------------------------------------------------------------

PROCEDURE GenCallStorage* ( n: pc.NODE );
(*
*)
VAR
  ntype  : pc.STRUCT; (* pointer base type *)
  btype  : pc.STRUCT; (* dyn. array base type *)

  -----------------------------------------------
  PROCEDURE PrmPtr ( ptr: BOOLEAN );
  (*
  *)
  BEGIN
    IF ~ptr THEN out.WrChr ('*') END;
    out.WrChr ('('); out.WrStr (nms.x2c[nms.nm_AA]^); out.WrStr ('*)');
(*    IF ntype.mode = pc.ty_array_of THEN
      out.WrChr ('&'); GenValue (n.r, 13, {dcl.LVL});
    ELSE *)
      GenValue (n.r, 13, {dcl.REF, dcl.ANY, dcl.LVL});
(*    END; *)
  END PrmPtr;

  -----------------------------------------------
  PROCEDURE PrmSize ( l: pc.NODE );
  (*
  *)
  BEGIN
    IF l = NIL
     THEN dcl.GenSizeof (btype);
     ELSE GenValue (l, 0, {});
    END;
  END PrmSize;

  -----------------------------------------------

  PROCEDURE PrmDims ( t: pc.STRUCT );
  (*
  *)
  BEGIN
    ASSERT( ntype.mode = pc.ty_array_of );
    dcl.GenCnumber (ntype.len, t);
  END PrmDims;

  -----------------------------------------------

  PROCEDURE PrmLens (     l: pc.NODE
                     ; type: pc.STRUCT);
  (*
  *)
  VAR
    nm: STR;
    k : LONGINT;
  BEGIN
    IF type = NIL THEN
      type := dcl.SIZE_T
    END;
    dcl.MakeTempArr (type, ntype.len, nm(*=>*));
    k := 0;
    out.WrChr ("(");
    REPEAT
      out.WrFmt ("%s[%d] = ", nm, k);
      INC (k);
      GenValue (l, 1, {});
      l := l.next;
      out.WrChr (",");
    UNTIL l = NIL;
    out.WrStr (nm);
    out.WrChr (')');
  END PrmLens;
(*
  -----------------------------------------------

  PROCEDURE PrmLensCptr (l: pc.NODE);
  BEGIN
    GenValue (l, 1, {}); l := l.next;
    WHILE l # NIL DO
      out.WrChr ("+");
      GenValue (l, 1, {}); l := l.next;
    END;
  END PrmLensCptr;
*)
  -----------------------------------------------

  PROCEDURE PrmType ( sys: BOOLEAN );
  (*
  *)
  VAR b: pc.STRUCT;
  BEGIN
    b := btype;
    WHILE b.mode IN pc.ARRs DO
      b := b.base
    END;
    IF (b.flag # pc.flag_o2) OR sys THEN
      out.WrStr ("x2c_td_null");
    ELSIF b.mode = pc.ty_pointer THEN
      out.WrStr ("x2c_td_ptr");
    ELSE GenTypeName (b, 0);
    END;
  END PrmType;

  -----------------------------------------------
VAR
  l      : pc.NODE;
  nm     : STR;
  i,j,k  : INTEGER;
  prm_buf: dcl.PRM_BUF;
  prm_cnt: INTEGER;
  o2,new,sys: BOOLEAN;
  lt     : pc.STRUCT;
BEGIN
  l  := n.r;
  j  := 0;
  lt := l.type;
  IF lt.mode = pc.ty_opaque THEN
    lt := lt.base
  END;
  ntype := lt.base;
  btype := ntype;
  WHILE btype.mode = pc.ty_array_of DO
    btype := btype.base
  END;
  ASSERT( lt.mode = pc.ty_pointer );
  o2  := lt.flag = pc.flag_o2;
  new := (n.sub = pc.sp_new) OR (n.sub = pc.sp_sysnew);
  sys := n.sub = pc.sp_sysnew;
  IF n.obj = NIL THEN
    IF ntype.mode = pc.ty_array_of THEN
      IF lt.flag = pc.flag_c THEN
        IF new THEN
          out.WrFmt ("%s(", nms.x2c[nms.nm_dyncallocate]^);
          PrmPtr (TRUE); out.WrChr (',');
          PrmSize (NIL); out.WrChr (',');
          PrmLens (n.r.next, NIL); out.WrChr (',');
          PrmDims (dcl.SIZE_T); out.WrChr (')');

          (*
          PrmPtr(TRUE); out.WrFmt(',(');
          PrmLensCptr(n.r.next); out.WrFmt(')*');
          PrmSize(NIL);
          out.WrChr(')');
          *)
        ELSE
          out.WrFmt ("%s(", nms.x2c[nms.nm_dyncdeallocate]^);
          PrmPtr (TRUE); out.WrChr (')');
        END;
      ELSE
        IF new THEN
          IF o2 THEN
            out.WrFmt ("%s(", nms.x2c[nms.nm_new_open]^);
            PrmType (FALSE); out.WrChr (',');
          ELSE
            out.WrFmt ("%s(", nms.x2c[nms.nm_dynallocate]^);
          END;
          PrmPtr (TRUE); out.WrChr (',');
          PrmSize (NIL); out.WrChr (',');
          PrmLens (n.r.next, NIL); out.WrChr (',');
          PrmDims (dcl.SIZE_T);
          IF o2 THEN
            IF sys
             THEN out.WrStr (",1")
             ELSE out.WrStr (",0")
            END;
          END;
          out.WrChr (')');
        ELSIF o2 THEN
          out.WrFmt ("%s(", nms.x2c[nms.nm_dispose]^);
          PrmPtr (TRUE); out.WrChr (')');
        ELSE
          out.WrFmt ("%s(", nms.x2c[nms.nm_dyndeallocate]^);
          PrmPtr (TRUE); out.WrChr (')');
        END;
      END;
    ELSIF new THEN
      IF o2 THEN
	out.WrFmt ("%s(", nms.x2c[nms.nm_new]^);
        PrmType (sys & (n.r.next # NIL)); out.WrChr (',');
        PrmPtr (TRUE); out.WrChr (',');
	PrmSize (n.r.next);
	IF sys
         THEN out.WrStr (",1")
         ELSE out.WrStr (",0")
        END;
	out.WrChr (')');
      ELSE
	out.WrFmt ("%s(", nms.x2c[nms.nm_allocate]^);
        PrmPtr (TRUE); out.WrChr (',');
	PrmSize (n.r.next); out.WrChr (')');
      END;
    ELSIF o2 THEN
      out.WrFmt ("%s(", nms.x2c[nms.nm_dispose]^);
      PrmPtr (TRUE); out.WrChr (')');
    ELSE
      out.WrFmt ("%s(", nms.x2c[nms.nm_deallocate]^);
      PrmPtr (TRUE); out.WrChr (')');
    END;
  ELSE
    dcl.ObjectUsage (n.obj, nm(*=>*));
    out.WrStr (nm);
    out.WrChr ("(");
    dcl.FuncParams (prm_buf(*=>*), prm_cnt(*=>*), n.obj.type);
    k := -1;
    FOR i := 0 TO prm_cnt-1 DO
      IF i > 0 THEN out.WrStr (", ") END;
      CASE prm_buf[i].mode OF
      | dcl.pm_threat :
          GenValueUsage (prm_buf[i].obj, 0, prm_buf[i].ref);
          k := 0;
      | dcl.pm_thr_len:
          ASSERT( k >= 0 );
	  dcl.OSecondName (prm_buf[i].obj, k, "_len", nm(*=>*));
	  out.WrStr (nm);
          INC (k);
      | dcl.pm_param  :
	  CASE j OF
	  | 0:
              PrmPtr (dcl.REF IN prm_buf[i].ref);
	  | 1:
              PrmSize (NIL);
              l := l.next;
	  | 2:
              IF new
               THEN PrmLens (l, prm_buf[i].obj.type.inx);
	       ELSE PrmDims (prm_buf[i].obj.type);
	      END;
          | 3:
              PrmDims (prm_buf[i].obj.type);
	  END;
	  INC (j);
      | dcl.pm_len   :
	  ASSERT( j = 3 );
	  PrmDims (prm_buf[i].obj.type.inx);
      END;
    END;
    out.WrChr (")");
  END;
END GenCallStorage;

----------------------------------------------------------------------------

PROCEDURE GenValueMinusMin* (   m: pc.NODE
		            ;   p: INTEGER
                            ;  bt: pc.STRUCT
		            ; i32: BOOLEAN );
(*
*)
VAR
  n: pc.NODE;
  t: pc.STRUCT;
  v: pc.VALUE;
BEGIN
  ASSERT( m.type.is_ordinal() );

<* IF NODE_VALUE_REPRESENTATION THEN *>
  v := bt.min.val;
<* ELSE *>
  v := bt.min;
<* END *>
  n := m;
  WHILE (n.mode = pc.nd_unary)
      & (n.sub = pc.su_conv)
      & n.l.type.is_ordinal()
  DO
    n := n.l;
  END;
  IF ~i32 & cc.op_index16 THEN
    t := n.type.super_type();
    IF t.mode IN pc.TY_SET{pc.ty_longint, pc.ty_longcard} THEN
      IF p > 13 THEN out.WrChr ('(') END;
      out.WrFmt ("(%s)", nms.x2c[nms.nm_index]^);
      GenValueMinusMin (n, 13, bt, TRUE);
      IF p > 13 THEN out.WrChr (')') END;
      RETURN;
    END;
  END;
  IF v.is_zero() THEN
    GenValueNotChar (n, p, {});
<* IF NODE_VALUE_REPRESENTATION THEN *>
  ELSIF n.val # NIL THEN
<* ELSE *>
  ELSIF n.mode = pc.nd_value THEN
<* END *>
    zz_gvm.binary (pc.sb_minus, n.val, v);
    dcl.GenConstAggr (zz_gvm, m.type, p, FALSE, FALSE);
  ELSIF v.is_neg() THEN
    IF p >= 11 THEN out.WrChr ('(') END;
    GenValueNotChar (n, 11, {}); out.WrChr ('+');
    zz_gvm.unary (pc.su_neg, v);
    dcl.GenConstAggr (zz_gvm, m.type, 11, FALSE, FALSE);
    IF p >= 11 THEN out.WrChr (')') END;
  ELSE
    IF p >= 11 THEN out.WrChr ('(') END;
    GenValueNotChar (n, 11, {});
    out.WrChr ('-');
    dcl.GenConstAggr (v, m.type, 11, FALSE, FALSE);
    IF p >= 11 THEN out.WrChr (')') END;
  END;
END GenValueMinusMin;

----------------------------------------------------------------------------

PROCEDURE GenValueBinary (  n: pc.NODE
                         ;  p: INTEGER
                         ; md: SET);

  PROCEDURE Binary ( p1: INTEGER
                   ; s-: ARRAY OF CHAR
                   ; p2: INTEGER);
  (*
  *)
  VAR
    b: BOOLEAN;
    m: SET;
  BEGIN
    b := (p > p1) OR (p = p1) & (p IN {1..2, 8..12});
    IF b & (dcl.PLS IN md) THEN
      IF (p = 11) & (n.sub = pc.sb_plus) THEN b := FALSE END;
      IF (p = 12) & (n.sub = pc.sb_mul)  THEN b := FALSE END;
    END;
    m := md*{dcl.CHK};
    IF (n.sub = pc.sb_plus)
    OR (n.sub = pc.sb_mul)
    THEN
      INCL(m, dcl.PLS)
    END;

    IF b THEN out.WrChr ('(') END;
    GenValue (n.l, p1, m);
    out.WrStr (s);
    GenValue (n.r, p2, m);
    IF b THEN out.WrChr (')') END;
  END Binary;

  ----------------------------------------------------------------------------

  PROCEDURE BinaryBln ( p1: INTEGER
                      ; s-: ARRAY OF CHAR );
  BEGIN
    IF p >= p1 THEN out.WrChr ('(') END;
    GenValue (n.l, p1, md*{dcl.CHK});
    out.WrStr (s);
    GenValue (n.r, p1, md*{dcl.CHK});
    IF p >= p1 THEN out.WrChr (')') END;
  END BinaryBln;

  ----------------------------------------------------------------------------

  PROCEDURE BinaryCplx;
  (*
  *)
  BEGIN
    out.WrFmt ("%s(", nms.x2c[nms.CmplxFunc (n.l.type, n.sub)]^);
    GenValue (n.l, 0, {});
    out.WrChr (',');
    GenValue (n.r, 0, {});
    out.WrChr (')');
  END BinaryCplx;

  ----------------------------------------------------------------------------

  PROCEDURE BinaryLset ( s-: ARRAY OF CHAR );
  (*
  *)
  VAR nm: STR;
  BEGIN
    dcl.MakeTempVar (n.type, nm(*=>*));
    out.WrFmt ("%s(%s,", s, nm);
    GenValue (n.l, 0, {dcl.BSA});
    out.WrChr (',');
    GenValue (n.r, 0, {dcl.BSA});
    out.WrFmt (",%d)", (n.type.len+dcl.LSET_BITS-1) DIV dcl.LSET_BITS);
  END BinaryLset;

  ----------------------------------------------------------------------------

  PROCEDURE WrCmpSign;
  (*
  *)
  BEGIN
    IF dcl.NEG IN md THEN
      CASE n.sub OF
      | pc.sb_lss: out.WrStr (" >= ");
      | pc.sb_leq: out.WrStr (" > ");
      | pc.sb_gtr: out.WrStr (" <= ");
      | pc.sb_geq: out.WrStr (" < ");
      END;
    ELSE
      CASE n.sub OF
      | pc.sb_lss: out.WrStr (" < ");
      | pc.sb_leq: out.WrStr (" <= ");
      | pc.sb_gtr: out.WrStr (" > ");
      | pc.sb_geq: out.WrStr (" >= ");
      END;
    END;
  END WrCmpSign;

  ----------------------------------------------------------------------------

  PROCEDURE IsEmptyStr ( n: pc.NODE )  :  BOOLEAN;
  (*
  *)
  BEGIN
<* IF NODE_VALUE_REPRESENTATION THEN *>
    IF n.val = NIL THEN RETURN FALSE END;
<* ELSE *>
    IF n.mode # pc.nd_value THEN RETURN FALSE END;
<* END *>
    zz_tmp.index_get (0, n.val);
    RETURN zz_tmp.is_zero();
  END IsEmptyStr;

  -----------------------------------------------

VAR
  t : pc.STRUCT;
  nm: STR;
  b : BOOLEAN;
  l : pc.NODE;
  i : LONGINT;
  nn: INTEGER;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenValueBinary: p=%d n = %X .mode = %s", p,n,dbg.VisNodMode[n.mode]);
    dbg.CallTrace (" sub = ");
    dbg.VisNodSubMode (n);
  END;
<* END *>
  ASSERT( md-{dcl.NEG, dcl.BSA, dcl.CCH,
              dcl.CNS, dcl.CHK, dcl.PLS} = {} );
  ASSERT( ~(dcl.NEG IN md) OR (n.type.mode = pc.ty_boolean) );

  CASE n.sub OF
  | pc.sb_equ
  , pc.sb_neq:
      IF p >= 8 THEN out.WrChr ('(') END;
      IF (n.l.type.mode = pc.ty_set)
       & dcl.IsCarray (n.l.type)
      THEN
        out.WrFmt ("%s(", nms.x2c[nms.nm_set_equ]^);
        GenValue (n.l, 0, {dcl.BSA}); out.WrChr (',');
        GenValue (n.r, 0, {dcl.BSA}); out.WrFmt (",%d)", n.l.type.len);
	IF (n.sub = pc.sb_neq) # (dcl.NEG IN md) THEN
          out.WrStr (" == 0")
        END;
      ELSIF n.l.type.mode IN pc.CPLXs THEN
	BinaryCplx;
        IF (n.sub = pc.sb_equ) # (dcl.NEG IN md)
         THEN out.WrStr (" == 0");
         ELSE out.WrStr (" != 0");
	END;
      ELSIF (n.l.type.mode = pc.ty_record)
         OR dcl.IsCarray (n.l.type)
      THEN
        IF (n.l.type.mode IN pc.ARRs) & dcl.IsChar (n.l.type.base)
        THEN
          IF IsEmptyStr (n.l) THEN
            GenValue (n.r, 14, {dcl.BSA}); out.WrStr ("[0]");
          ELSIF IsEmptyStr (n.r) THEN
            GenValue (n.l, 14, {dcl.BSA}); out.WrStr ("[0]");
          ELSE
            out.WrFmt ("%s(", nms.x2c[nms.nm_strcmp]^);
            GenValue (n.l, 0, {dcl.BSA}); out.WrChr (',');
            GenLen (n.l, 0, dcl.SIZE_T, 0); out.WrChr (',');
            GenValue (n.r, 0, {dcl.BSA}); out.WrChr (',');
            GenLen (n.r, 0, dcl.SIZE_T, 0); out.WrChr (')');
          END;
	ELSE
	  out.WrFmt ("%s(", nms.x2c[nms.nm_memcmp]^);
          GenValue (n.l, 0, {dcl.REF, dcl.ANY}); out.WrChr (',');
          GenValue (n.r, 0, {dcl.REF, dcl.ANY}); out.WrChr (',');
          dcl.GenSizeof (n.l.type); out.WrChr (')');
	END;
        IF (n.sub = pc.sb_equ) # (dcl.NEG IN md)
         THEN out.WrStr (" == 0");
         ELSE out.WrStr (" != 0");
	END;
      ELSE
        GenValue (n.l, 8, {});
        IF (n.sub = pc.sb_equ) # (dcl.NEG IN md)
         THEN out.WrStr (" == ")
         ELSE out.WrStr (" != ")
	END;
        GenValue (n.r, 8, {});
      END;
      IF p >= 8 THEN out.WrChr (')') END;

  | pc.sb_lss
  , pc.sb_leq
  , pc.sb_gtr
  , pc.sb_geq:
      IF n.l.type.mode = pc.ty_set THEN
        IF GenCheckNeg (n, md, p) THEN
          -- nothing, already generated in checks
        ELSIF dcl.IsCarray (n.l.type) THEN
          out.WrFmt ("%s(", nms.x2c[nms.nm_set_leq]^);
          IF n.sub = pc.sb_leq THEN
            GenValue (n.l, 0, {dcl.BSA});
            out.WrChr (',');
            GenValue (n.r, 0, {dcl.BSA});
          ELSE
            ASSERT( n.sub = pc.sb_geq );
            GenValue (n.r, 0, {dcl.BSA});
            out.WrChr (',');
            GenValue (n.l, 0, {dcl.BSA});
          END;
          out.WrFmt (",%d)", n.l.type.len);
        ELSE
          IF p > 13 THEN out.WrChr ('(') END;
          IF n.sub = pc.sb_leq THEN
            out.WrStr ("!(");
            GenValue (n.l, 7, {});
            out.WrStr (" & ~");
            GenValue (n.r, 13, {});
          ELSE
            ASSERT( n.sub = pc.sb_geq );
            out.WrStr ("!(~");
            GenValue (n.l, 13, {});
            out.WrStr (" & ");
            GenValue (n.r, 7, {});
          END;
          out.WrChr (')');
          IF p > 13 THEN out.WrChr (')') END;
        END;
      ELSE
	IF p >= 9 THEN out.WrChr ('(') END;
        IF (n.l.type.mode = pc.ty_record)
        OR dcl.IsCarray (n.l.type)
        THEN
          IF (n.l.type.mode IN pc.ARRs) & dcl.IsChar (n.l.type.base) THEN
            out.WrFmt ("%s(", nms.x2c[nms.nm_strcmp]^);
            GenValue  (n.l, 0, {dcl.BSA}); out.WrChr (',');
            GenLen    (n.l, 0, dcl.SIZE_T, 0); out.WrChr (',');
            GenValue  (n.r, 0, {dcl.BSA}); out.WrChr (',');
            GenLen    (n.r, 0, dcl.SIZE_T, 0);
	  ELSE
	    out.WrFmt("%s(", nms.x2c[nms.nm_memcmp]^);
            GenValue (n.l, 0, {dcl.REF, dcl.ANY}); out.WrChr (',');
            GenValue (n.r, 0, {dcl.REF, dcl.ANY}); out.WrChr (',');
            dcl.GenSizeof (n.l.type);
	  END;
	  out.WrChr (')');
          WrCmpSign;
          out.WrChr ('0');
        ELSE
          GenValueNotChar (n.l, 9, {});
	  WrCmpSign;
          GenValueNotChar (n.r, 9, {});
	END;
	IF p >= 9 THEN out.WrChr (')') END;
      END;

  | pc.sb_plus
  , pc.sb_minus:
      IF cc.op_cpp & ~cc.op_constenum       -- C++: enum
       & (n.l.type.mode = pc.ty_enum)       -- +/-1 must use conversion
      THEN
        GenTypeCast ("", n.l.type);
        out.WrChr ("(");
        GenValue (n.l, 11, {dcl.LVL});
        IF n.sub = pc.sb_plus
         THEN out.WrStr (" + 1)");
         ELSE out.WrStr (" - 1)");
        END;
      ELSE
        IF n.l.type.mode IN pc.CPLXs THEN
          BinaryCplx;
        ELSIF n.sub = pc.sb_plus THEN
          Binary (11, " + ", 11);
        ELSE
          Binary (11, " - ", 11);
        END;
      END;

  | pc.sb_mul:
      IF n.l.type.mode IN pc.CPLXs
       THEN BinaryCplx;
       ELSE Binary (12, " * ", 12);
      END;

  | pc.sb_and:
      IF dcl.IsCarray (n.type)
       THEN BinaryLset (nms.x2c[nms.nm_and]^);
       ELSE Binary (7, " & ", 7);
      END;

  | pc.sb_xor:
      IF dcl.IsCarray (n.type)
       THEN BinaryLset (nms.x2c[nms.nm_xor]^);
       ELSE Binary (6, " ^ ", 6);
      END;

  | pc.sb_or:
      IF dcl.IsCarray (n.type)
       THEN BinaryLset (nms.x2c[nms.nm_or]^);
       ELSE Binary (5, " | ", 5);
      END;

  | pc.sb_bic:
      IF dcl.IsCarray (n.type)
       THEN BinaryLset (nms.x2c[nms.nm_bic]^); --$$$ excluded
       ELSE Binary (7, " & ~", 13);
      END;

  | pc.sb_cand:
      IF ~GenCheckNeg (n, md, p) THEN BinaryBln (4, " && ") END;

  | pc.sb_cor:
      IF ~GenCheckNeg (n, md, p) THEN BinaryBln (3, " || ") END;

  | pc.sb_ash:
      IF (n.r.val # NIL) &
         dcl.CmpValue (pc.sb_gtr, n.r.val, zz_m32) &
         dcl.CmpValue (pc.sb_lss, n.r.val, zz_32)
      THEN
	IF p >= 10 THEN out.WrChr ('(') END;
        GenValue (n.l, 10, md*{dcl.CHK});
        IF n.r.val.is_neg() THEN out.WrStr (" >> ") ELSE out.WrStr (" << ") END;
	out.WrFmt ("%d", ABS (n.r.val.get_integer()));
	IF p >= 10 THEN out.WrChr (')') END;
      ELSE
	out.WrFmt ("%s(", nms.x2c[nms.nm_ash]^);
        GenValue (n.l, 0, {}); out.WrChr (',');
        GenValue (n.r, 0, {}); out.WrChr (')');
      END;

  | pc.sb_pre_inc
  , pc.sb_pre_dec
  , pc.sb_post_inc
  , pc.sb_post_dec:
      IF GenCheckNeg (n, md, p) THEN
        -- nothing, already generated in checks
      ELSE
	IF pc.ntag_chk_range IN n.tags THEN
          out.WrStr (nms.x2c[nms.IncDec (n.sub, n.type)]^); out.WrChr ('(');
          IF cc.op_cpp & ~cc.op_constenum       -- C++ INC/DEC (enum)
           & (n.l.type.mode = pc.ty_enum)       -- must be casted
           & dcl.StdType (n.l.type.base, nm(*=>*)) -- gen base type
          THEN
            out.WrFmt("(%s*)",nm);
          END;
          GenValue (n.l, 0, {dcl.LVL, dcl.REF}); out.WrChr (',');
          GenValue (n.r, 0, {}); out.WrChr (',');
          GenMin (n.type); out.WrChr (',');
          GenMax (n.type); out.WrChr (')');
        ELSIF (n.r.val # NIL)
            & dcl.CmpValue (pc.sb_equ, n.r.val, zz_one)
        THEN
          IF cc.op_cpp & ~cc.op_constenum       -- C++ INC/DEC (enum)
           & (n.l.type.mode = pc.ty_enum)       -- must be casted +/- 1
          THEN
            GenValue (n.l, 0, {dcl.LVL});
            out.WrStr (" = ");
            GenTypeCast ("", n.l.type);
            out.WrChr ("(");
            GenValue (n.l, 11, {dcl.LVL});
            IF (n.sub = pc.sb_pre_inc)
            OR (n.sub = pc.sb_post_inc)
             THEN out.WrStr (" + 1)");
             ELSE out.WrStr (" - 1)");
            END;
          ELSE
            IF p >= 13 THEN out.WrChr ('(') END;
            IF (n.sub = pc.sb_pre_inc) THEN
              out.WrStr ('++');
            ELSIF (n.sub = pc.sb_pre_dec) THEN
              out.WrStr ('--');
            END;
            GenValue (n.l, 13, {dcl.LVL});
            IF (n.sub = pc.sb_post_inc) THEN
              out.WrStr ('++');
            ELSIF (n.sub = pc.sb_post_dec) THEN
              out.WrStr ('--');
            END;
            IF p >= 13 THEN out.WrChr (')') END;
          END
	ELSE
	  IF p >= 1 THEN out.WrChr ('(') END;
          GenValue (n.l, 1, {dcl.LVL});
          IF (n.sub = pc.sb_pre_inc)
          OR (n.sub = pc.sb_post_inc)
           THEN out.WrStr (' += ')
           ELSE out.WrStr (' -= ')
          END;
          GenValue (n.r, 1, {});
	  IF p >= 1 THEN out.WrChr (')') END;
	END;
      END;

  | pc.sb_addadr
  , pc.sb_subadr:
      IF p >= 11 THEN out.WrChr ('(') END;
      IF pc.ntag_chk_range IN n.tags
       THEN GenCheckNil (n.l, TRUE);
       ELSE GenValue (n.l, 11, {});
      END;
      (*
         hp9000 can not substruct unsigned long from pointer!,
	 so I have to cast expression to int
      *)
      IF n.sub = pc.sb_addadr
       THEN out.WrStr (' + (int)');
       ELSE out.WrStr (' - (int)');
      END;
      GenValue (n.r, 13, {});
      IF p >= 11 THEN out.WrChr (')') END;

  | pc.sb_difadr:
      Binary (11, ' - ', 11);

  | pc.sb_rot:
      ASSERT( n.type.mode IN pc.SETs );
      IF dcl.IsCarray (n.type) THEN
	out.WrFmt ("%s(", nms.x2c[nms.nm_rot_lset]^);
        dcl.MakeTempVar (n.type, nm(*=>*));
        out.WrFmt ("%s,", nm);
        GenValue (n.l, 0, {dcl.BSA});
        out.WrFmt (",%d,", n.type.len);
        GenValue (n.r, 0, {});
        out.WrChr (')');
      ELSE
	out.WrFmt ("%s(", nms.x2c[nms.nm_rot_set]^);
        GenValue (n.l, 0, {});
        out.WrFmt (",%d,", n.type.len);
        GenValue (n.r, 0, {});
        out.WrChr (')');
      END;

  | pc.sb_lsh:
      IF dcl.IsCarray (n.type) THEN
        ASSERT( n.type.mode = pc.ty_set );
	out.WrFmt ("%s(", nms.x2c[nms.nm_lsh_lset]^);
        dcl.MakeTempVar (n.type, nm(*=>*));
        out.WrFmt ("%s,", nm);
        GenValue (n.l, 0, {dcl.BSA});
        out.WrFmt (",%d,", n.type.len);
        GenValue (n.r, 0, {});
        out.WrChr (')');
      ELSE
        CASE n.type.mode OF
        | pc.ty_shortint
        , pc.ty_char
        , pc.ty_loc:
            i := 8;
        | pc.ty_integer:
            i := 16;
        | pc.ty_longint
        , pc.ty_ZZ:
            i := 32;
        | pc.ty_set:
            i := n.type.len;
        END;
        b := n.type.is_ordinal() & n.type.signed();
        IF (p > 13) & b THEN out.WrChr ('(')          END;
        IF  b           THEN GenTypeCast ("", n.type) END;
	out.WrFmt ("%s(", nms.x2c[nms.nm_lsh_set]^);
        GenValue (n.l, 0, {});
        out.WrFmt (",%d,", i);
        GenValue (n.r, 0, {});
        out.WrChr (')');
	IF (p > 13) & b THEN out.WrChr (')') END;
      END;

  | pc.sb_high:
      ASSERT( n.r.val # NIL );
      t := n.l.type;
      i := n.r.val.get_integer();
      WHILE i > 0 DO
        t := t.base;
        DEC (i)
      END;
      IF t.mode = pc.ty_array THEN
<* IF NODE_VALUE_REPRESENTATION THEN *>
        dcl.GenConstAggr (t.max.val, t.inx, p, FALSE, FALSE); (* !!! Gen MAX here? *)
<* ELSE *>
        dcl.GenConstAggr (t.max, t.inx, p, FALSE, FALSE); (* !!! Gen MAX here? *)
<* END *>
      ELSE
        ASSERT( n.l.type.mode = pc.ty_array_of );
        IF p >= 11 THEN out.WrChr ('(') END;
        GenLen (n.l, n.r.val.get_integer(), n.type, 11);
        out.WrStr ("-1");
        IF p >= 11 THEN out.WrChr (')') END;
      END;

  | pc.sb_len:
      ASSERT( n.r.val # NIL );
      GenLen (n.l, n.r.val.get_integer(), n.type, p);

  | pc.sb_in:
      t := n.r.type;
      b := dcl.IsCarray (t);
      IF ~b & (n.l.val # NIL) THEN
	IF p >= 8 THEN out.WrChr ('(') END;
        out.WrChr('(');
--- Doesn't work
---        GenValueMinusMin (n.l, 7, t, TRUE);
	zz_tmp.binary(pc.sb_minus, n.l.val, t.min);
	out.WrFmt("0x%X",{zz_tmp.get_integer()});
        dcl.Suffix(t);
---
        out.WrStr (" & "); GenValue (n.r, 7, {});
        out.WrChr (')');
        IF dcl.NEG IN md
         THEN out.WrStr (" == 0");
         ELSE out.WrStr (" != 0");
	END;
	IF p >= 8 THEN out.WrChr (')') END;
      ELSIF GenCheckNeg (n, md, p) THEN
        -- nothing, already generated in checks
      ELSE
        IF b
         THEN out.WrFmt ("%s(", nms.x2c[nms.nm_inl]^);
         ELSE out.WrFmt ("%s(", nms.x2c[nms.nm_in]^);
        END;
	l := n.l;
	IF dcl.IsLong (l.type) THEN
          GenValueMinusMin (l, 0, t, TRUE);
	ELSE
	  out.WrFmt ("(%s)", nms.x2c[nms.nm_longint]^);
          GenValueMinusMin (l, 13, t, TRUE);
	END;
	out.WrFmt (",%d,", t.len);
        IF b
         THEN GenValue (n.r, 0, {dcl.BSA})
         ELSE GenValue (n.r, 0, {})
        END;
	out.WrChr (')');
      END;

  | pc.sb_slash
  , pc.sb_div
  , pc.sb_mod
  , pc.sb_rem:
      IF n.l.type.mode IN pc.CPLXs THEN
	BinaryCplx;
      ELSE
	(* for ZZ-type '/' and '%' are used *)
	t := NIL;
	IF n.type.is_ordinal() THEN
	  t := n.type.super_type();
          IF (t.mode IN (pc.CARDs-pc.TY_SET{pc.ty_ZZ}))
          OR (t.mode = pc.ty_ZZ) & ~n.l.val.is_neg()
          OR cc.op_cdiv & ((n.sub = pc.sb_div)
          OR (n.sub = pc.sb_mod))
          THEN
	    t := NIL;
	  END;
	END;
	IF t # NIL THEN
	  IF t.mode # pc.ty_longint THEN
	    IF p > 13 THEN out.WrChr ('(') END;
            GenTypeCast ("", n.type);
	  END;
	  CASE n.sub OF
          | pc.sb_slash: nn := nms.nm_quo;
          | pc.sb_div  : nn := nms.nm_div;
          | pc.sb_rem  : nn := nms.nm_rem;
          | pc.sb_mod  : nn := nms.nm_mod;
	  END;
	  out.WrFmt ("%s(", nms.x2c[nn]^);
          GenValue (n.l, 0, {});
          out.WrChr (',');
          GenValue (n.r, 0, {});
          out.WrChr (')');
	  IF t.mode # pc.ty_longint THEN
	    IF p > 13 THEN out.WrChr (')') END;
	  END;
	ELSE
	  IF (n.sub = pc.sb_slash) & ((n.type.mode = pc.ty_real) OR 
	                               (n.type.mode = pc.ty_longreal)) THEN
            IF p>=12 THEN out.WrChr ('(') END;
            CASE n.type.mode OF
            |pc.ty_real    :  nn := nms.nm_divr;
            |pc.ty_longreal:  nn := nms.nm_divl;
            END;
    	    out.WrFmt ("%s(", nms.x2c[nn]^);
            GenValue (n.l, 0, {});
            out.WrChr (',');
            GenValue (n.r, 0, {});
            out.WrChr (')');
            IF p>=12 THEN out.WrChr (')') END;
          ELSE
            IF p >= 12 THEN out.WrChr ('(') END;
            GenValue (n.l, 12, {});
            IF (n.sub = pc.sb_slash)
            OR (n.sub = pc.sb_div)
            THEN out.WrStr (' / ');
            ELSE out.WrStr (' % ');
            END;
            GenValue (n.r, 12, {});
      	    IF p >= 12 THEN out.WrChr (')') END;
          END;
      	END;
      END;

  | pc.sb_exp:
      out.WrFmt ("%s(", nms.x2c[nms.Exp (n.r.type, n.type)]^);
      GenValue (n.l, 0, {});
      out.WrChr (',');
      GenValue (n.r, 0, {});
      out.WrChr (')');

  | pc.sb_cmplx:
      IF n.val # NIL THEN              -- const aggregate here (* @2 *)
        dcl.GenConstAggr (n.val, n.type, p, dcl.CNS IN md, FALSE);
      ELSE
        dcl.MakeTempVar (n.type, nm(*=>*));
        out.WrFmt ("(%s.re = ", nm);
        GenValue (n.l, 1, {});
        out.WrFmt (",%s.im = ", nm);
        GenValue (n.r, 1, {});
        out.WrFmt (',%s)', nm);
      END;

  | pc.sb_concat:
      ASSERT( n.type.mode = pc.ty_SS );
      GenValue (n.l, 0, md);
      GenValue (n.r, 0, md);
  ELSE
    env.errors.Error (n.pos, 1008);
  END;
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenValueBinary") END;
<* END *>
END GenValueBinary;

----------------------------------------------------------------------------

PROCEDURE CmpBaseTypes ( t1,t2: pc.STRUCT )  :   BOOLEAN;
(*
*)
BEGIN
  IF t1.mode = pc.ty_pointer
   THEN t1 := t1.base;
   ELSE t1 := dcl.GetBaseType (t1);
  END;
  IF t2.mode = pc.ty_pointer
   THEN t2 := t2.base;
   ELSE t2 := dcl.GetBaseType (t2);
  END;
  RETURN (t1 # NIL) & (t1 = t2);
END CmpBaseTypes;

----------------------------------------------------------------------------

PROCEDURE GenConvAdr (  n: pc.NODE
                     ;  p: INTEGER
                     ; md: SET );
(*
*)
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(GenConvAdr: p=%d md = %{}", p,md) END;
<* END *>

  ASSERT( n.type.mode IN pc.VPTRs );
  ASSERT( n.l.mode = pc.nd_unary );
  ASSERT( n.l.sub = pc.su_adr );

  IF (n.type.mode = pc.ty_pointer)
   &  dcl.IsCarray (n.l.l.type)
   &  dcl.EquCtypes (n.l.l.type, n.type, TRUE)
  THEN
    GenValue (n.l.l, p, {dcl.BSA, dcl.LVL}+md*{dcl.CCH});
  ELSIF (n.type.mode = pc.ty_pointer)
      & ~dcl.IsCarray (n.l.l.type)
      &  dcl.EquCtypes (n.l.l.type, n.type.base, FALSE)
  THEN
    GenValue (n.l.l, p, {dcl.REF, dcl.LVL}+md*{dcl.CCH});
  ELSE
    IF p > 13 THEN out.WrChr ('(') END;
    GenTypeCast ("", n.type);
    GenValue (n.l.l, 13, {dcl.REF, dcl.ANY, dcl.LVL}+md*{dcl.CCH});
    IF p > 13 THEN out.WrChr (')') END;
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenConvAdr") END;
<* END *>
END GenConvAdr;

----------------------------------------------------------------------------

PROCEDURE GenValueCast (  n: pc.NODE
                       ;  p: INTEGER
                       ; md: SET );
(*
   if types size don't match then additional bits are not defined
*)

  -----------------------------------------------

  PROCEDURE GenTempVar ( sz: LONGINT );
    (*
       assigns source expression into temporary variable with
       result type, generated text returns "void *" of temporary var
    *)
    VAR tmp: STR;

  BEGIN
    IF dcl.LVL IN md THEN
      env.errors.Error (n.pos, 1009);
    END;
    dcl.MakeTempVar (n.type, tmp(*=>*));
    out.WrFmt ("%s(&%s,", nms.x2c[nms.nm_memcpy]^, tmp);
    GenValue (n.l, 0, {dcl.REF, dcl.ANY}); out.WrChr (',');
    dcl.GenCnumber (sz, dcl.SIZE_T);
    out.WrChr (')');
  END GenTempVar;
  -----------------------------------------------

  PROCEDURE IsWholeInC ( t: pc.STRUCT )  :   BOOLEAN;
  (*
  *)
  BEGIN
    IF t.mode IN (pc.ORDs+pc.TY_SET{pc.ty_AA, pc.ty_loc, pc.ty_protection}) THEN
      RETURN TRUE;
    END;
    IF (t.mode = pc.ty_set) & (t.len <= 32) THEN RETURN TRUE END;
    RETURN FALSE;
  END IsWholeInC;

-------------------------------------------------

VAR
  nm   : STR;
  s1,s2: LONGINT;

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenValueCast: p=%d n = %X .mode = %s", p,n,dbg.VisNodMode[n.mode]);
    dbg.CallTrace (" sub = ");
    dbg.VisNodSubMode (n);
  END;
<* END *>

  IF ( md*{dcl.REF, dcl.LVL} = {})
   & (  IsWholeInC (n.type)
      & IsWholeInC (n.l.type)
     OR (n.type.mode IN pc.VPTRs)
      & (n.l.type.mode IN pc.VPTRs)
     OR (n.type.mode = pc.ty_proctype)
      & (n.l.type.mode = pc.ty_proctype)
    )
  THEN
    (* generate C type cast *)
    ASSERT( {dcl.BSA, dcl.NEG, dcl.ANY}*md = {} );
    IF (n.type.mode IN pc.VPTRs)
     & (n.l.mode = pc.nd_unary)
     & (n.l.sub = pc.su_adr)
    THEN
      (* оптимизнем cast к char* *)
      GenConvAdr (n, p, md);
    ELSE
      IF p > 13 THEN out.WrChr ('(') END;
      GenTypeCast ("", n.type);
      GenValueNotChar (n.l, 13, md*{dcl.CHK});
      IF p > 13 THEN out.WrChr (')') END;
    END;
  ELSE
    IF (n.type.mode IN pc.VPTRs)
     & (n.l.type.mode IN pc.VPTRs)
    OR (n.type.mode = pc.ty_proctype)
     & (n.l.type.mode = pc.ty_proctype)
    THEN
      s1 := 0;
      s2 := 0;
      (* не нужно знать реальные размеры если известно что они равны *)
    ELSE
      s1 := pc.code.get_size (pc.su_size, n.l.type);
      s2 := pc.code.get_size (pc.su_size, n.type);
    END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (0, "GenValueCast: s1 = %d s2 = %d", s1, s2)
  END;
<* END *>

    IF n.type.mode = pc.ty_array_of THEN
      ASSERT(   (dcl.BSA IN md)
             OR (dcl.REF IN md) & (dcl.ANY IN md) );
      IF (dcl.BSA IN md)
       & (n.type.flag IN cc.C_LIKE_FLAGS)
       & (n.l.type.mode = pc.ty_pointer)
       &  CmpBaseTypes (n.type, n.l.type)
      THEN
        GenValue (n.l, p, {});
      ELSIF (dcl.ANY IN md)
         OR (dcl.BSA IN md)
          &  dcl.IsCarray (n.l.type)
          &  CmpBaseTypes (n.type, n.l.type)
      THEN
        GenValue (n.l, p, md);
      ELSE
	IF p > 13 THEN out.WrChr ('(') END;
        GenTypeCast ("*", dcl.GetBaseType (n.type));
        GenValue (n.l, 13, {dcl.REF, dcl.ANY});
	IF p > 13 THEN out.WrChr (')') END;
      END;
    ELSIF  n.l.type.is_ordinal()
        & (n.l.val # NIL)
        &  n.l.val.is_zero()
    THEN
      ASSERT( ~(dcl.LVL IN md) );
      IF (p > 13)
       & (md*{dcl.REF, dcl.ANY} # {dcl.REF, dcl.ANY})
      THEN
        out.WrChr ("(")
      END;
      IF ~(dcl.REF IN md) THEN out.WrChr ("*") END;
      IF ~(dcl.ANY IN md) THEN GenTypeCast ("*", n.type) END;
      dcl.MakeTempVar (n.type, nm(*=>*));
      out.WrFmt ("memset(&%s,0,", nm);
      dcl.GenSizeof (n.type);
      out.WrChr (")");
      IF (p > 13)
       & (md*{dcl.REF, dcl.ANY} # {dcl.REF, dcl.ANY})
      THEN
        out.WrChr (")")
      END;
    ELSIF (s1 < 0) OR (s2 < 0) THEN
      env.errors.Warning (n.pos, 350);
      IF (p > 13) & ~(dcl.REF IN md) THEN
        out.WrChr ('(')
      END;
      IF ~(dcl.REF IN md) THEN
        out.WrChr ('*')
      END;
      out.WrFmt ("%s(", nms.x2c[nms.nm_cast]^);
      GenValue (n.l, 0, {dcl.REF, dcl.ANY});
      out.WrChr (",");
      dcl.TypeDesignator (nm(*=>*), "", n.l.type);
      out.WrStr (nm);
      out.WrChr (",");
      dcl.TypeDesignator (nm(*=>*), "", n.type);
      out.WrStr (nm);
      out.WrChr (",");
      dcl.TypeDesignator (nm(*=>*), "*", n.type);
      out.WrStr (nm);
      out.WrChr (")");
      IF (p > 13) & ~(dcl.REF IN md) THEN
        out.WrChr (')')
      END;
    ELSIF (dcl.REF IN md) & (dcl.ANY IN md) THEN

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (0, "GenValueCast: REF ANY") END;
<* END *>

      IF s2 > s1
       THEN GenTempVar (s1)
       ELSE GenValue (n.l, p, md)
      END;
    ELSE
      IF p > 13 THEN out.WrChr ('(') END;
        IF ~(dcl.REF IN md) THEN out.WrChr ('*') END;
        GenTypeCast ("*", n.type);
      IF s2 > s1
       THEN GenTempVar (s1);
       ELSE GenValue (n.l, 13, {dcl.REF, dcl.ANY});
      END;
      IF p > 13 THEN out.WrChr (')') END;
    END;
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenValueCast") END;
<* END *>
END GenValueCast;

----------------------------------------------------------------------------

PROCEDURE ConvIsCast ( fr,to: pc.STRUCT )  :  BOOLEAN;
(*
*)
VAR sf,st: pc.STRUCT;
BEGIN
  IF dcl.IsCarray (fr) & ((fr.mode # pc.ty_SS) OR (fr.len > 2))
  OR dcl.IsCarray (to)
  OR (fr.mode IN pc.VPTRs) & (to.mode IN pc.VPTRs)
  OR (fr.mode = pc.ty_record)
  OR (to.mode = pc.ty_record)
  OR (fr.mode = pc.ty_proctype) & (to.mode = pc.ty_proctype)
  THEN
    RETURN TRUE;
  END;
  IF fr.is_ordinal() THEN sf := fr.super_type() ELSE sf := fr END;
  IF to.is_ordinal() THEN st := to.super_type() ELSE st := to END;
  RETURN (sf.mode IN pc.TY_SET{pc.ty_shortint, pc.ty_shortcard, pc.ty_loc})
       & (st.mode = pc.ty_loc)
      OR (sf.mode = pc.ty_loc)
       & (st.mode IN pc.TY_SET{pc.ty_shortint, pc.ty_shortcard, pc.ty_loc});
END ConvIsCast;

----------------------------------------------------------------------------

PROCEDURE GenValueUnaryConv ( n: pc.NODE
                            ; p: INTEGER
                            ; md: SET );
(*
*)
  PROCEDURE Min ( x,y: pc.STRUCT ) : pc.STRUCT;
  (*
     Select type with minimal .max value
  *)
  BEGIN
    IF x = NIL THEN RETURN y END;
    IF y = NIL THEN RETURN x END;
<* IF NODE_VALUE_REPRESENTATION THEN *>
    zz_tmp.binary (pc.sb_lss, x.max.val, y.max.val);
<* ELSE *>
    zz_tmp.binary (pc.sb_lss, x.max, y.max);
<* END *>
    IF zz_tmp.is_zero()
     THEN RETURN y
     ELSE RETURN x
    END;
  END Min;
  -------------------------------------------------

  PROCEDURE Max ( x,y: pc.STRUCT ) : pc.STRUCT;
  (*
     Select type with of maximal .min value
  *)
  BEGIN
    IF x = NIL THEN RETURN y END;
    IF y = NIL THEN RETURN x END;
<* IF NODE_VALUE_REPRESENTATION THEN *>
    zz_tmp.binary (pc.sb_lss, x.min.val, y.min.val);
<* ELSE *>
    zz_tmp.binary (pc.sb_lss, x.min, y.min);
<* END *>
    IF zz_tmp.is_zero()
     THEN RETURN x
     ELSE RETURN y
    END;
  END Max;
  -------------------------------------------------

VAR
  sf,st    : pc.STRUCT;
  nf       : pc.NODE;
  tmin,tmax: pc.STRUCT;
  chk,cnv  : BOOLEAN;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenValueUnaryConv: p=%d n = %X", p,n)
  END;
<* END *>

  ASSERT( ~(dcl.NEG IN md) );

  sf := n.l.type;               (* from and to types determination *)
  st := n.type;
  IF sf.mode = pc.ty_range THEN sf := sf.base END;
  IF st.mode = pc.ty_range THEN st := st.base END;
  IF (sf.mode IN pc.TY_SET{pc.ty_boolean, pc.ty_char, pc.ty_enum}) THEN
    sf := sf.super_type();
  END;
  IF (sf.mode = pc.ty_ZZ)
   & (n.l.mode = pc.nd_unary)
   & (   (n.l.sub = pc.su_size)
      OR (n.l.sub = pc.su_bytes))
  THEN
    sf := dcl.SIZE_T; n.l.type := sf;    (* IRMOD *)
    ASSERT( n.l.type.max # NIL );
    ASSERT( n.l.type.min # NIL );
  END;

  (* Conversion cases: *)
  IF (sf.mode IN pc.WHOLEs-pc.TY_SET{pc.ty_ZZ})   -- from WHOLEs
   & (st.mode IN pc.ORDs)                -- to ORDinals   range check possible
  THEN (* 1 .. 4 *)
    ASSERT( n.l.type.max # NIL );
    ASSERT( n.l.type.min # NIL );
    IF ~(pc.ntag_chk_range IN n.tags)
     &   dcl.EquCtypes (sf, st, TRUE)
    THEN
      GenValue (n.l, p, md);
    ELSE
      chk  := (pc.ntag_chk_range IN n.tags) & ~(dcl.CHK IN md);
      nf   := n.l;
      tmin := Max (n.type, nf.type);
      tmax := Min (n.type, nf.type);
      WHILE (nf.mode = pc.nd_unary)
          & (nf.sub = pc.su_conv)
          & (nf.l.type.mode IN pc.ORDs-pc.TY_SET{pc.ty_ZZ})
      DO
        chk  := chk OR (pc.ntag_chk_range IN nf.tags) & ~(dcl.CHK IN md);
        nf   := nf.l;
        tmin := Max (tmin, nf.type);
        tmax := Min (tmax, nf.type);
        sf := nf.type.super_type();
      END;
      cnv := ~dcl.EquCtypes (sf, st, TRUE);
      IF cnv THEN
        IF p > 13 THEN out.WrChr ('(') END;
        GenTypeCast ("", n.type);
      END;
      IF chk THEN
        out.WrStr (nms.x2c[nms.RangeChk (sf)]^); out.WrChr ('(');
        GenValueNotChar (nf, 0, md*{dcl.CHK}); out.WrChr (',');
        GenMin (tmin); out.WrChr (',');
        GenMax (tmax); out.WrChr (')');
      ELSIF cnv THEN
        GenValueNotChar (nf, 13, md*{dcl.CHK});
      ELSE
        GenValueNotChar (nf, p, md*{dcl.CHK});
      END;
      IF cnv & (p > 13) THEN out.WrChr (')') END;
    END;

  ELSIF (sf.mode IN pc.NUMs) &     -- from NUMs   (no literals)
        (st.mode IN pc.REALs)      -- to REALs
     OR (sf.mode IN pc.VPTRs) &
        (st.mode IN pc.WHOLEs+pc.VPTRs)
     OR (sf.mode IN pc.WHOLEs) &
        (st.mode IN pc.VPTRs)
     OR (st.mode = pc.ty_proctype) &
        (sf.mode = pc.ty_proctype)
  THEN (* 5 *)
    IF (st.mode IN pc.VPTRs) &
       (n.l.mode = pc.nd_unary) &
       (n.l.sub = pc.su_adr)
    THEN
      GenConvAdr (n, p, md);
    ELSE
      IF (st.mode IN pc.VPTRs) &
         ~(sf.mode IN pc.VPTRs)
      THEN
        env.errors.Warning (n.pos, 350);
      END;
      IF p > 13 THEN out.WrChr ('(') END;
      GenTypeCast ("", n.type);
      GenValueNotChar (n.l, 13, md*{dcl.CHK});
      IF p > 13 THEN out.WrChr (')') END;
    END;

  ELSIF (sf.mode IN pc.REALs) &   -- from REALs  (no literals)
        (st.mode IN pc.CARDs)     -- to CARDs
  THEN
    IF p > 13 THEN out.WrChr ('(') END;
    GenTypeCast ("", n.type);
    out.WrFmt ("%s(", nms.x2c[nms.nm_truncc]^);
    GenValue (n.l, 0, md*{dcl.CHK}); out.WrChr (',');
    GenMin (n.type); out.WrChr (',');
    GenMax (n.type); out.WrChr (')');
    IF p > 13 THEN out.WrChr (')') END;

  ELSIF (sf.mode IN pc.REALs) &   -- from REALs  (no literals)
        (st.mode IN pc.INTs)      -- to INTs
  THEN
    IF p > 13 THEN out.WrChr ('(') END;
    GenTypeCast ("", n.type);
    out.WrFmt ("%s(", nms.x2c[nms.nm_trunci]^);
    GenValue (n.l, 0, md*{dcl.CHK}); out.WrChr (',');
    GenMin (n.type); out.WrChr (',');
    GenMax (n.type); out.WrChr (')');
    IF p > 13 THEN out.WrChr (')') END;

  ELSIF (sf.mode = pc.ty_AA) &                -- from NIL to address
        (st.mode IN pc.VPTRs+pc.TY_SET{pc.ty_proctype})
  THEN
     out.WrChr ('0');

  ELSIF (sf.mode = pc.ty_longcard) &    -- from LONGCARD
        (st.mode IN pc.VPTRs)           -- to pointer
  THEN
    (* SYSTEM.MAKEADR *)
    IF p > 13 THEN out.WrChr ('(') END;
    GenTypeCast ("", n.type);
    GenValue (n.l, 13, md*{dcl.CHK});
    IF p > 13 THEN out.WrChr (')') END;

  ELSIF (sf.mode = pc.ty_set) & ~dcl.IsCarray (sf) &  -- from set # C array
        (st.mode IN pc.ORDs-pc.TY_SET{pc.ty_ZZ})              -- to ORDinals (no literals)
  THEN (* 9 *)
    IF p > 13 THEN out.WrChr ('(') END;
    GenTypeCast ("", n.type);
    IF (pc.ntag_chk_range IN n.tags) & ~(dcl.CHK IN md) THEN
      (* Здесь генерится r_chk неправильного размера (size = 4) !!! *)
      out.WrFmt ("%s(", nms.x2c[nms.RangeChk (sf)]^);
      GenValue (n.l, 0, md*{dcl.CHK}); out.WrChr (',');
      GenMin (n.type); out.WrChr (',');
      GenMax (n.type); out.WrChr (')');
    ELSE
      GenValue (n.l, 13, md*{dcl.CHK});
    END;
    IF p > 13 THEN out.WrChr (')') END;

  ELSIF (sf.mode = pc.ty_complex) &     -- from COMPLEX   (no literals)
        (st.mode = pc.ty_lcomplex)      -- to LONGCOMPLEX
  THEN
    out.WrStr (nms.x2c[nms.nm_cplx_lcplx]^);
    out.WrChr ('(');
    GenValue  (n.l, 0, md*{dcl.CHK});
    out.WrChr (')');

  ELSIF (sf.mode = pc.ty_CC) THEN       -- from external named literal const
    GenValue (n.l, p, md);              -- (internal unnamed are cut in pcConst)

  -- Other casting:
  ELSIF ConvIsCast (sf, st) THEN
    GenValueCast (n, p, md*{dcl.CHK});

  -- Conversion of literal number:
  ELSIF (sf.mode = pc.ty_ZZ) &
        (st.mode IN pc.ORDs-pc.TY_SET{pc.ty_ZZ})
  THEN (* 1 .. 4 *)
    IF ~(pc.ntag_chk_range IN n.tags) OR (dcl.CHK IN md) THEN
      GenValue (n.l, p, md);
    ELSE
      IF p > 13 THEN out.WrChr ('(') END;
      GenTypeCast ("", n.type);
      IF st.signed()
       THEN out.WrStr (nms.x2c[nms.nm_r_chkl]^);
       ELSE out.WrStr (nms.x2c[nms.nm_r_chkul]^);
      END;
      out.WrChr ('(');
      GenValue (n.l, 0, md*{dcl.CHK});
      out.WrChr (',');
      GenMin (n.type);
      out.WrChr (',');
      GenMax (n.type);
      out.WrChr (')');
      IF p > 13 THEN out.WrChr (')') END;
    END;

  -- Conversion of literal string:
  ELSIF (sf.mode = pc.ty_SS)
      &  st.is_ordinal() (*(st.mode IN pc.WHOLEs)*)
  THEN
    IF n.l.mode = pc.nd_var THEN
      ASSERT( n.l.obj.mode = pc.ob_cons );
      zz_tmp.index_get (0, n.l.obj.val.val);
    ELSE
      ASSERT( n.l.val # NIL );
      zz_tmp.index_get (0, n.l.val);
    END;
    dcl.GenConstAggr (zz_tmp, n.l.type.base, p, FALSE, FALSE);

  ELSE
    env.errors.Error (n.pos, 1010);
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenValueUnaryConv") END;
<* END *>
END GenValueUnaryConv;

----------------------------------------------------------------------------

PROCEDURE GenConvVptr (  n: pc.NODE
                      ;  p: INTEGER
                      ; md: SET );
(*
*)
VAR conv: BOOLEAN;
BEGIN
  ASSERT( n.mode = pc.nd_unary );
  ASSERT( md*{dcl.NEG} = {} );
  CASE n.sub OF
  | pc.su_ptr2vptr:
      conv :=  (n.type.mode # pc.ty_pointer)
           OR ~(n.type.base.mode IN pc.TY_SET{pc.ty_loc, pc.ty_char});
      IF conv THEN
        IF p > 13 THEN out.WrChr ('(') END;
        GenTypeCast ("", n.type);
      END;
      out.WrStr ("VPTR(");
      GenValue (n.l, 0, {});
      out.WrChr (')');
      IF conv THEN
        IF p > 13 THEN out.WrChr (')') END;
      END;

  | pc.su_vptr2ptr:
      conv :=  (n.type.mode # pc.ty_pointer)
           OR ~(n.type.base.mode IN pc.TY_SET{pc.ty_loc, pc.ty_char});
      IF conv THEN
        IF p > 13 THEN out.WrChr ('(') END;
        GenTypeCast ("", n.type);
      END;
      out.WrStr ("RPTR(");
      GenValue (n.l, 0, {});
      out.WrChr (')');
      IF conv THEN
        IF p > 13 THEN out.WrChr (')') END;
      END;
  END;
END GenConvVptr;

----------------------------------------------------------------------------

PROCEDURE GenValueUnary (  n: pc.NODE
                        ;  p: INTEGER
                        ; md: SET );
(*
*)

VAR
  nm: STR;
  i : LONGINT;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenValueUnary: p=%d n = %X .mode = %s", p,n,dbg.VisNodMode[n.mode]);
    dbg.CallTrace (" sub = ");
    dbg.VisNodSubMode (n);
  END;
<* END *>

  ASSERT( md-{dcl.NEG, dcl.BSA, dcl.CCH, dcl.CNS,
              dcl.CHK, dcl.PLS} = {} );
  ASSERT( ~(dcl.NEG IN md) OR (n.type.mode = pc.ty_boolean) );

  CASE n.sub OF

  | pc.su_bits:
      IF p >= 12 THEN out.WrChr ('(') END;
      GenSize (n.l, 0, 12);
      out.WrStr ("*8");
      IF p >= 12 THEN out.WrChr (')') END;

  | pc.su_min:
      GenMin (n.l.type);

  | pc.su_max:
      GenMax (n.l.type);

  | pc.su_bytes
  , pc.su_size:
      GenSize (n.l, 0, p);

  | pc.su_abs:
      nms.Abs (n.type, pc.ntag_chk_overflow IN n.tags, nm(*=>*));
      out.WrStr (nm);
      out.WrChr ('(');
      GenValue (n.l, 0, {});
      out.WrChr (')');

  | pc.su_cap:
      out.WrFmt ("%s(", nms.x2c[nms.nm_cap]^);
      GenValue (n.l, 0, {});
      out.WrChr (')');

  | pc.su_neg:
      IF n.type.mode IN pc.CPLXs THEN
        IF n.type.mode = pc.ty_complex
         THEN out.WrFmt ("%s(", nms.x2c[nms.nm_cplx_neg]^);
         ELSE out.WrFmt ("%s(", nms.x2c[nms.nm_cplx_lneg]^);
	END;
        GenValue (n.l, 0, {});
        out.WrChr (')');
      ELSE
        IF p > 13 THEN
          out.WrChr ('(')
        ELSIF p = 13 THEN
          out.WrChr (' ')
        END;
        out.WrStr ('-'); GenValue (n.l, 13, {});
	IF p > 13 THEN out.WrChr (')') END;
      END;

  | pc.su_adr:
      ASSERT( md*{dcl.NEG} = {} );
      IF p > 13 THEN out.WrChr ('(') END;
        out.WrFmt ("(%s)", nms.x2c[nms.nm_AA]^);
        GenValue (n.l, 13, {dcl.REF, dcl.ANY, dcl.LVL}+md*{dcl.CCH});
      IF p > 13 THEN out.WrChr (')') END;

  | pc.su_adr_o2:
      ASSERT( md*{dcl.NEG} = {} );
      IF p > 13 THEN out.WrChr ('(') END;
      out.WrFmt ("(%s)", nms.x2c[nms.nm_longint]^);
      GenValue (n.l, 13, {dcl.REF, dcl.ANY, dcl.LVL});
      IF p > 13 THEN out.WrChr (')') END;

  | pc.su_conv:
      IF ~(pc.ntag_chk_range IN n.tags)
       &   dcl.EquCtypes (n.l.type, n.type, TRUE)
      THEN
        GenValue (n.l, p, md);
      ELSIF GenCheckNeg (n, md, p) THEN
        -- nothing, already generated in checks
      ELSE
        GenValueUnaryConv (n, p, md);
      END;

  | pc.su_odd:
      IF GenCheckNeg (n, md, p) THEN
        -- nothing, already generated in checks
      ELSE
          IF p > 13 THEN out.WrChr ('(') END;
          out.WrFmt ("(%s)(", nms.x2c[nms.nm_boolean]^);
          GenValue (n.l, 7, {}); out.WrStr ("&1)");
          IF p > 13 THEN out.WrChr (')') END;
      END;

  | pc.su_not:
      GenValue (n.l, p, md/{dcl.NEG});

  | pc.su_length:
        out.WrFmt ("%s(", nms.x2c[nms.nm_length]^);
        GenValue (n.l, 0, {dcl.REF, dcl.ANY});
        out.WrChr (',');
        GenLen (n.l, 0, n.type, 0);
        out.WrChr (')');

  | pc.su_compl:
      ASSERT( n.type.mode IN (pc.TY_SET{pc.ty_set}+pc.WHOLEs-pc.TY_SET{pc.ty_ZZ}) );
      IF dcl.IsCarray (n.type) THEN
	ASSERT( n.type.mode = pc.ty_set );
        dcl.MakeTempVar (n.type, nm(*=>*));
        out.WrFmt ("%s(%s,", nms.x2c[nms.nm_complement]^, nm);
        GenValue (n.l, 0, {dcl.BSA});
	out.WrFmt (",%d)", (n.type.len+dcl.LSET_BITS-1) DIV dcl.LSET_BITS);
      ELSE
	IF n.type.mode = pc.ty_set THEN
	  i := n.type.len;
	ELSE
          i := dcl.GetBytes (n.pos, n.type)*8;
	END;
	IF i = 32 THEN
	  (* Предпологаем, что промежуточный результат не превышает 32 бита *)
	  IF p > 13 THEN out.WrChr ('(') END;
          out.WrChr ("~"); GenValue (n.l, 13, {});
	  IF p > 13 THEN out.WrChr (')') END;
	ELSE
	  IF p > 6 THEN out.WrChr ('(') END;
          GenValue (n.l, 6, {});
	  out.WrFmt ("^0x%X", {0..i-1});
	  dcl.Suffix (n.type);
	  IF p > 6 THEN out.WrChr (')') END;
	END;
      END;

  | pc.su_re:
      GenValue (n.l, 14, {});
      out.WrStr (".re");

  | pc.su_im:
      GenValue (n.l, 14, {});
      out.WrStr (".im");

  | pc.su_entier:
      out.WrFmt ("%s(", nms.x2c[nms.nm_entier]^);
      GenValue (n.l, 0, {});
      out.WrChr (')');

  | pc.su_is:
      IF p >= 8 THEN out.WrChr ('(') END;
      GenType (n.l, 14);
      out.WrFmt ("->base[%d]", n.obj.type.len);
      IF dcl.NEG IN md
       THEN out.WrStr ("!= ")
       ELSE out.WrStr ("==")
      END;
      GenTypeName (n.obj.type, 8);
      IF p >= 8 THEN out.WrChr (')') END;

  | pc.su_ptr2vptr, pc.su_vptr2ptr:
      GenConvVptr (n, p, md);
  |pc.su_bit_offs, pc.su_byte_offs, pc.su_word_offs:
      GenFieldOfs(n.l);
(*
    |pc.su_cc    : GenValue (n.l);
--???    |pc.su_bit   :
*)
  ELSE
    env.errors.Error (n.pos, 1008);
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenValueUnary") END;
<* END *>
END GenValueUnary;

----------------------------------------------------------------------------

PROCEDURE GenValueOrAddress (  n: pc.NODE
                            ;  p: INTEGER
                            ; md: SET
                            )   : 	BOOLEAN;
(*
*)
VAR nm: STR;

BEGIN
  CASE n.mode OF
  | pc.nd_var:
      IF dcl.IsPtr (n.obj, dcl.cur_proc) THEN
        GenValueUsage (n.obj, p, md);
	RETURN TRUE;
      END;

  | pc.nd_replace:
      IF dcl.LVL IN md THEN
        ASSERT( dcl.REF IN md );
        GenValue (n, p, md);
        RETURN TRUE;
      END;

  | pc.nd_deref:
      IF  cc.op_cpp
       & (n.l.mode = pc.nd_var)
       & (pc.otag_with IN n.l.obj.tags)
      THEN
        dcl.OName (n.l.obj, nm(*=>*));
        out.WrStr (nm);
        RETURN FALSE;
      ELSE
        GenValue (n, p, md);
        RETURN TRUE;
      END;

  ELSE
  END;

  GenValue (n, p, md-{dcl.REF});
  RETURN FALSE;
END GenValueOrAddress;

----------------------------------------------------------------------------

PROCEDURE GenCheckIndex* (   n, l: pc.NODE -- n - expression, l - array or set
                         ; p, dim: INTEGER
                         ;    en: BOOLEAN );
(*
*)
VAR nm: INTEGER;
BEGIN
  IF en THEN
    IF (l.mode = pc.nd_deref) & (l.l.type.flag = pc.flag_c)
     THEN nm := nms.nm_chksl;
     ELSE nm := nms.IndexChk (l.type);
    END;
    out.WrStr (nms.x2c[nm]^); out.WrChr ('(');
    p := 0;
  END;
  GenValueMinusMin (n, p, l.type, en);
  IF en THEN
    IF ~((l.mode = pc.nd_deref)
     & (l.l.type.flag = pc.flag_c))
    THEN
      out.WrChr (',');
      GenLen (l, dim, dcl.SIZE_T, 0);
    END;
    out.WrChr (')');
  END;
END GenCheckIndex;

----------------------------------------------------------------------------

PROCEDURE GenIndex (  n: pc.NODE
                   ;  p: INTEGER
                   ; md: SET );
(*
*)
VAR
  l : pc.NODE;
  r : ARRAY 32 OF pc.NODE;
  no: INTEGER;
  i : INTEGER;
BEGIN
  ASSERT( md*{dcl.NEG, dcl.REF} = {} );

  l := n.l;
  r[0] := n.r;
  no := 1;
  WHILE (l.type.mode = pc.ty_array_of)
      & (l.mode = pc.nd_index)
      & (l.l.type.mode = pc.ty_array_of)
  DO
    r[no] := l.r;
    INC (no);
    l := l.l;
  END;

  IF (l.mode = pc.nd_deref)
   & (l.l.type.flag = pc.flag_c)
  THEN
    IF (n.type.mode = pc.ty_array_of) & (p >= 11) THEN
      out.WrChr ('(')
    END;
    GenValue (l, 11, {dcl.BSA});

    out.WrChr ('[');
    GenCheckIndex (r[no-1], l, 0, 0,
                   (pc.ntag_chk_range IN n.tags) & ~(dcl.CHK IN md));
    out.WrChr (']');
    FOR i := 1 TO no-1 DO
      out.WrChr ('[');
      GenCheckIndex (r[no-i-1], l, 11, i,
                     (pc.ntag_chk_range IN n.tags) & ~(dcl.CHK IN md));
      out.WrChr (']');
    END;

    IF (n.type.mode = pc.ty_array_of) & (p >= 11) THEN
      out.WrChr (')')
    END;
  ELSE
    IF n.type.mode = pc.ty_array_of THEN
      IF p >= 11 THEN out.WrChr ('(') END;
      GenValue (l, 11, {dcl.BSA}); out.WrStr ("+(");
    ELSE
      GenValue (l, 14, {dcl.BSA}); out.WrChr ("[");
    END;
    FOR i := 1 TO no-1 DO
      out.WrChr ('(')
    END;
    GenCheckIndex (r[no-1], l, 0, 0,
                   (pc.ntag_chk_range IN n.tags) & ~(dcl.CHK IN md));
    FOR i := 1 TO no-1 DO
      out.WrStr (')*');
      GenLen (l, i, l.type.inx, 12);
      out.WrChr ('+');
      GenCheckIndex (r[no-i-1], l, 11, i,
                     (pc.ntag_chk_range IN n.tags) & ~(dcl.CHK IN md));
    END;
    IF n.type.mode = pc.ty_array_of THEN
      out.WrChr (')');
      FOR i := 0 TO SHORT (n.type.len-1) DO
        out.WrChr ('*');
        GenLen (l, no+i, l.type.inx, 12);
      END;
      IF p >= 11 THEN out.WrChr (')') END;
    ELSE
      out.WrStr ("]");
    END;
  END;
END GenIndex;

----------------------------------------------------------------------------

PROCEDURE GenLsetAggregate ( n: pc.NODE
                           ; p: INTEGER );
(*
*)
VAR
  l,f,t: pc.NODE;
  nm   : STR;
BEGIN
  ASSERT( n.type.mode = pc.ty_set );
  ASSERT( dcl.IsCarray (n.type) );
  l := n.l;
  dcl.MakeTempVar (n.type, nm(*=>*));
  IF l # NIL THEN
    out.WrChr ('(');
  ELSE
    IF p > 13 THEN out.WrChr ('(') END;
    out.WrFmt ("(%s)", nms.x2c[nms.nm_lset]^);
  END;
  out.WrFmt ("memset(%s,0,", nm);
  dcl.GenSizeof (n.type);
  out.WrChr (')');
  IF l = NIL THEN
    IF p > 13 THEN out.WrChr (')') END;
    RETURN;
  END;
  REPEAT
    out.WrChr (',');
    IF l.mode = pc.nd_node THEN
      f := l.l;
      t := l.r;
      out.WrFmt ("%s(%s,", nms.x2c[nms.nm_longset]^, nm);
      GenValueMinusMin (f, 0, n.type, TRUE);
      out.WrChr (',');
      GenValueMinusMin (t, 0, n.type, TRUE);
      out.WrFmt (",%d)", n.type.len);
    ELSE
      f := l;
      out.WrFmt ("%s(%s,", nms.x2c[nms.nm_incl]^, nm);
      GenValueMinusMin (f, 0, n.type, TRUE);
      out.WrFmt (",%d)", n.type.len);
    END;
    l := l.next;
  UNTIL l = NIL;
  out.WrChr (')');
END GenLsetAggregate;

----------------------------------------------------------------------------

PROCEDURE GenSetAggregate (  n: pc.NODE
                          ;  p: INTEGER
                          ; md: SET );
(*
*)
VAR l,f,t: pc.NODE;
BEGIN
  ASSERT( n.type.mode = pc.ty_set );

  IF dcl.IsCarray (n.type) THEN
    GenLsetAggregate (n, p);
    RETURN
  END;
  l := n.l;
  IF l = NIL THEN
    out.WrChr ('0');
    RETURN
  END;

  IF p > 5 THEN out.WrChr ('(') END;
  REPEAT
    IF l # n.l THEN out.WrStr(' | ') END;
    IF l.mode = pc.nd_node THEN
      out.WrFmt ("%s(", nms.x2c[nms.nm_set_range]^);
      f := l.l;
      t := l.r;
      GenValueMinusMin (f, 0, n.type, TRUE);
      out.WrChr (',');
      GenValueMinusMin (t, 0, n.type, TRUE);
      out.WrFmt (",%d)", n.type.len);
    ELSE
      out.WrChr ("1"); dcl.Suffix (n.type);
      out.WrStr ("<<");
      GenCheckIndex (l, n, 10, 0,
                     (pc.ntag_chk_range IN n.tags) & ~(dcl.CHK IN md));
    END;
    l := l.next;
  UNTIL l = NIL;
  IF p > 5 THEN out.WrChr (')') END;
END GenSetAggregate;

----------------------------------------------------------------------------

PROCEDURE GenArrayAggregate ( n: pc.NODE );
(*
*)
VAR
  l,v: pc.NODE;
  nm : STR;
  i,j: LONGINT;
BEGIN
  ASSERT( n.type.mode = pc.ty_array );
  l := n.l;
  dcl.MakeTempVar (n.type, nm(*=>*));
  out.WrChr ('(');
  i := 0;
  WHILE l # NIL DO
    IF l.mode = pc.nd_node THEN
      ASSERT( l.r.val # NIL );
      j := l.r.val.get_integer();
      v := l.l;
    ELSE
      j := 1;
      v := l;
    END;
    IF v.type.mode = pc.ty_array THEN
      WHILE j > 0 DO
        out.WrFmt ("memcpy(%s[%d], ", nm, i);
        GenValue (v, 1, {});
        out.WrFmt (", sizeof(%s[%d])), ", nm, i);
        INC (i);
        DEC (j);
      END;
    ELSE
      WHILE j > 0 DO
        out.WrFmt ("%s[%d] = ", nm, i);
        INC (i);
        DEC (j);
      END;
      GenValue (v, 1, {});
      out.WrStr (", ");
    END;
    l := l.next;
  END;
  out.WrFmt ("%s)", nm);
  ASSERT( i = n.type.len );
END GenArrayAggregate;

----------------------------------------------------------------------------

PROCEDURE GenValueSequence (  n: pc.NODE
                           ; md: SET );
(*
*)
VAR
  sz,sz0: LONGINT;
  nm    : STR;
  l     : pc.NODE;
BEGIN
  ASSERT( ~(dcl.LVL IN md) );
  sz0 := GetSequenceLen (n);

  IF sz0 = 0 THEN
    out.WrChr ('0');
    RETURN
  END;
  dcl.MakeTempArr (dcl.SEQU_T, sz0, nm(*=>*));
  out.WrChr ('(');
  out.SaveIndent;
  l := n.l;
  sz := 0;
  WHILE l # NIL DO
    IF l.type.mode IN pc.REALs THEN
      out.WrFmt ("*(%s*)&%s[%d] = ", nms.x2c[nms.nm_longreal]^, nm, sz);
      INC (sz, 2);
      GenValue (l, 1, {});
    ELSIF l.type.mode IN pc.SEQ_ARRs THEN
      out.WrFmt ("%s[%d].adr = ", nm, sz);
      INC (sz);
      GenValue (l, 1, {dcl.REF, dcl.ANY});
      IF ~IsResultOfCptrDeref (l) THEN
        out.WrFmt (", %s[%d].val = 0, ", nm, sz);
        out.WrapMark (8);
        out.WrFmt ("%s[%d].val = ", nm, sz+1);
        INC (sz, 2);
        GenSize (l, 0, 11);
        out.WrStr ("-1");
      END;
    ELSIF l.type.mode IN pc.SEQ_PTRs THEN
      out.WrFmt ("%s[%d].adr = ", nm, sz);
      INC (sz);
      GenValue (l, 1, {});
    ELSE
      ASSERT( l.type.mode IN pc.TY_SET{pc.ty_longcard, pc.ty_longint} );
      out.WrFmt ("%s[%d].val = ", nm, sz);
      INC (sz);
      GenValue (l, 13, {});
    END;
    out.WrStr (", ");
    out.WrapMark (8);
    l := l.next;
  END;
  ASSERT( sz = sz0 );
  IF dcl.REF IN md THEN
    GenTypeCast ("*", n.type);
  ELSIF dcl.BSA IN md THEN
    GenTypeCast ("*", dcl.GetBaseType (n.type));
  ELSE
    GenTypeCast ("", n.type);
  END;
  out.WrStr (nm); out.WrChr (")");
END GenValueSequence;

----------------------------------------------------------------------------

PROCEDURE GenRecordAggregate ( n: pc.NODE );
(*
*)
VAR
  nm: STR;
  l: pc.NODE;

  -----------------------------------------------

  PROCEDURE GenFld ( f: pc.OBJECT );
  (*
  *)
  VAR fld: STR;
  BEGIN
    IF f.type.mode = pc.ty_array THEN
      out.WrStr ("memcpy(");
      dcl.ObjectUsage (f, fld(*=>*));
      dcl.GenFieldPath (f.ext (nms.INFO).nxt);
      out.WrFmt ("%s.%s, ", nm, fld);
      GenValue (l, 1, {});
      out.WrFmt (", sizeof(%s.%s))", nm, fld);
    ELSE
      out.WrFmt ("%s.", nm);
      dcl.ObjectUsage (f, fld(*=>*));
      dcl.GenFieldPath (f.ext (nms.INFO).nxt);
      out.WrFmt ("%s = ", fld);
      GenValue (l, 1, {});
    END;
    out.WrStr (", ");
    l := l.next;
  END GenFld;

  --------------------------------------
  PROCEDURE GenFldSeq ( f: pc.OBJECT );
  (*
  *)
  VAR v: pc.NODE;
  BEGIN
    WHILE f # NIL DO
      IF f.mode = pc.ob_header THEN
        ASSERT( f.val.mode = pc.nd_case );
        ASSERT( l.val # NIL );
        v := f.val.l;
        dcl.SearchRecordVariant (v(*=>*), l.val);
        ASSERT( v # NIL );
        IF f.val.obj # NIL
         THEN GenFld (f.val.obj)
         ELSE l := l.next
        END;
        GenFldSeq (v.obj);
      ELSE
        GenFld (f);
      END;
      f := f.next;
    END;
  END GenFldSeq;

  -----------------------------------------------

  PROCEDURE GenLevel ( t: pc.STRUCT );
  (*
  *)
  BEGIN
    IF t.base # NIL THEN GenLevel (t.base) END;
    GenFldSeq (t.prof);
  END GenLevel;
  -----------------------------------------------

BEGIN
  ASSERT( n.type.mode = pc.ty_record );
  dcl.MakeTempVar (n.type, nm(*=>*));
  out.WrChr ('(');
  l := n.l;
  GenLevel (n.type);
  ASSERT( l = NIL );
  out.WrFmt ("%s)", nm);
END GenRecordAggregate;

----------------------------------------------------------------------------

PROCEDURE RemStrExtension ( VAR n: pc.NODE ): BOOLEAN;
(*
*)
VAR r: BOOLEAN;
BEGIN
  r :=  (n.mode = pc.nd_unary)
      & (n.sub = pc.su_conv)
      & (n.type.mode IN pc.ARRs)
      & (n.l.type.mode IN pc.ARRs)
      &  dcl.IsChar (n.type.base)
      &  dcl.IsChar (n.l.type.base);
  IF r THEN n := n.l END;
  RETURN r;
END RemStrExtension;

----------------------------------------------------------------------------

PROCEDURE GenAssign* (  n: pc.NODE
                     ;  p: INTEGER
                     ; md: SET );
(*
*)
VAR
  r,l : pc.NODE;
  lt  : pc.STRUCT;
  nm  : STR;
  cv  : BOOLEAN;
  st  : BOOLEAN;
  zr  : BOOLEAN;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "GenAssign: p=%d n = %X .mode = %s", p,n,dbg.VisNodMode[n.mode]);
  END;
<* END *>

  ASSERT( md-{dcl.REF, dcl.ANY} = {} );

  IF n.obj # NIL
   THEN lt := n.obj.type;
   ELSE lt := n.l.type;
  END;
  r := n.r;
  l := n.l;

  IF dcl.IsCarray (lt) THEN
    ASSERT( (dcl.REF IN md) OR (dcl.BSA IN md) );
    st := RemStrExtension (r(*=>*));
    IF n.type.mode = pc.ty_void THEN
      cv := FALSE;
    ELSIF st THEN
      cv := (dcl.REF IN md) & ~(dcl.ANY IN md);
    ELSE
      cv := ~(dcl.ANY IN md);
    END;
    IF (p > 13) & cv THEN out.WrChr ('(') END;
    IF cv THEN
      IF dcl.BSA IN md
       THEN GenTypeCast ("*", dcl.GetBaseType (n.type));
       ELSE GenTypeCast ("*", n.type);
      END;
    END;
    IF st THEN
      zr := (n.type.mode = pc.ty_void) & (r.val # NIL);
      IF zr THEN
        zz_tmp.index_get (0, r.val);
        zr := zz_tmp.is_zero();
      END;
      IF zr THEN
        ASSERT( p < 0 );
        IF n.obj # NIL
         THEN GenValueUsage (n.obj, 14, {dcl.BSA, dcl.LVL});
         ELSE GenValue (l, 14, {dcl.BSA, dcl.LVL});
        END;
        out.WrStr ("[0] = 0");
      ELSE
        out.WrFmt ("%s(", nms.x2c[nms.nm_strncpy]^);
        out.SaveIndent;
        IF n.obj # NIL
         THEN GenValueUsage (n.obj, 0, {dcl.BSA, dcl.LVL});
         ELSE GenValue (l, 0, {dcl.BSA, dcl.LVL});
        END;
        out.WrChr (',');
        GenValue (r, 0, {dcl.BSA});
        out.WrChr (',');
        IF n.obj # NIL
         THEN GenLenObj (n, 0, dcl.SIZE_T);  -- object is given
         ELSE GenLen (l, 0, dcl.SIZE_T, 0);  -- designator is given
        END;
        out.WrChr (')');
      END;
    ELSE
      out.WrFmt ("%s(", nms.x2c[nms.nm_memcpy]^);
      out.SaveIndent;
      IF n.obj # NIL
       THEN GenValueUsage (n.obj, 0, {dcl.REF, dcl.ANY, dcl.LVL});
       ELSE GenValue (n.l, 0, {dcl.REF, dcl.ANY, dcl.LVL});
      END;
      out.WrChr (',');
      GenValue (n.r, 0, {dcl.REF, dcl.ANY});
      out.WrChr (',');
      GenSize (r, 0, 0);
      out.WrChr (')');
    END;
    IF (p > 13) & cv THEN out.WrChr (')') END;

  ELSIF (dcl.REF IN md) & (n.obj # NIL) THEN
    out.WrChr ("(");
    GenAssign (n, 1, {});
    out.WrChr (",");
    GenValueUsage (n.obj, 1, md);
    out.WrChr (")");

  ELSIF dcl.REF IN md THEN
    dcl.MakeTempVar (n.type, nm(*=>*));
    out.WrFmt ("(%s = ", nm);
    GenAssign (n, 1, md-{dcl.REF, dcl.ANY});
    out.WrFmt (", &%s)", nm);

  ELSE
    IF p >= 1 THEN out.WrChr ('(') END;
    IF n.obj # NIL
     THEN GenValueUsage (n.obj, 1, {dcl.LVL});
     ELSE GenValue (n.l, 1, {dcl.LVL})
    END;
    out.WrStr (" = ");
    out.SaveIndent;
    GenValue (n.r, 1, {});
    IF p >= 1 THEN out.WrChr (')') END;
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenAssign") END;
<* END *>
END GenAssign;

----------------------------------------------------------------------------

PROCEDURE GenValue* (  n: pc.NODE
                    ;  p: INTEGER
                    ; md: SET );
(*
*)
  -------------------------------------------------
  PROCEDURE GenCheckAdr ()  :   BOOLEAN;
  (*
     Generate reference to array if dcl.REF IN md (T) else do nothing (F)
  *)
  BEGIN
    IF ~(dcl.REF IN md) THEN RETURN FALSE END;
    IF dcl.IsCarray (n.type) THEN
      ASSERT( dcl.ANY IN md );
      md := md - {dcl.REF, dcl.ANY} + {dcl.BSA};
      RETURN FALSE;
    END;
    IF    p > 13 THEN out.WrChr ('(');
    ELSIF p = 13 THEN out.WrChr (' ');
    END;
    out.WrChr ('&');
    GenValue (n, 13, md-{dcl.REF, dcl.ANY}+{dcl.LVL} );
    IF p > 13 THEN out.WrChr (')') END;
    RETURN TRUE;
  END GenCheckAdr;

  -----------------------------------------------

  PROCEDURE GenCheckVref ()   :   BOOLEAN;
  (*
     Generate temp var for expr if dcl.REF IN md (T)
     or call GenCheckAdr for C array
     else do nothing (F)
  *)
  VAR nm: STR;
  BEGIN
    IF ~(dcl.REF IN md)      THEN RETURN FALSE         END;
    IF dcl.IsCarray (n.type) THEN RETURN GenCheckAdr() END;
    IF dcl.LVL IN md THEN env.errors.Error (n.pos, 1008) END;
    dcl.MakeTempVar (n.type, nm(*=>*));
    out.WrFmt ("(%s = ", nm);
    GenValue (n, 1, md-{dcl.REF, dcl.ANY, dcl.LVL});
    out.WrFmt (",&%s)", nm);
    RETURN TRUE;
  END GenCheckVref;
  -------------------------------------------------

VAR
  l   : pc.NODE;
  b,c : BOOLEAN;
  nm  : pc.NAME;
  inf : nms.INFO;
  i   : LONGINT;
  nn  : INTEGER;

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (1, "(GenValue: p=%d n = %X .mode = %s", p,n,dbg.VisNodMode[n.mode]);
  END;
<* END *>

  CASE n.mode OF

  | pc.nd_var:
      IF GenCheckNeg (n, md, p) THEN
        -- nothing, already generated in checks
      ELSE
        GenValueUsage (n.obj, p, md);
      END;

  | pc.nd_proc:
      IF GenCheckVref() THEN
        -- nothing, already generated in checks
      ELSE
        dcl.ObjectUsage (n.obj, nm(*=>*));
        out.WrStr (nm);
      END;

  | pc.nd_field:
      dcl.ObjectUsage (n.obj, nm(*=>*));

      ASSERT( cc.otag_declared IN n.obj.tags );
      ASSERT( n.obj.mode IN pc.OB_SET{pc.ob_field, pc.ob_field_bts} );

      IF (dcl.REF IN md)
       & (cc.otag_bitfield IN n.obj.tags)
      THEN
        ASSERT( ~(dcl.NEG IN md) );
        IF p > 13 THEN out.WrChr ("(") END;
        IF ~(dcl.ANY IN md) THEN GenTypeCast ("*", n.type) END;
	out.WrChr ("&");
        IF GenValueOrAddress (n.l, 14, {dcl.REF}+md*{dcl.LVL})
         THEN out.WrStr ("->");
	 ELSE out.WrChr (".");
	END;
        IF cc.otag_bitfield_nm IN n.obj.tags THEN
	  inf := n.obj.ext (nms.INFO).nxt;
	  dcl.GenFieldPath (inf.nxt);
	  out.WrStr (inf.name^);
        ELSE
          dcl.GenFieldPath (n.obj.ext(nms.INFO).nxt);
          out.WrStr (nm);
        END;
        IF p > 13 THEN out.WrChr (")") END;
      ELSIF (dcl.REF IN md) & (n.obj.attr # NIL) THEN (*
                                                 --------------$$$ PR150 *)
        ASSERT( ~(dcl.NEG IN md) );
        IF ~(dcl.ANY IN md) THEN
          IF p > 13 THEN out.WrChr ("(") END;
          GenTypeCast ("*", n.type);
          GenValue (n, 13, md+{dcl.ANY});
          IF p > 13 THEN out.WrChr (")") END;
        ELSE
          IF p >= 11 THEN out.WrChr ('(') END;
          i := n.obj.attr.l.val.get_integer();
          out.WrStr ("(long *)");
          GenValue (n.l, 13, md);
          out.WrFmt ("+%d", i);
          IF p >= 11 THEN out.WrChr (')') END;
        END;
      ELSIF GenCheckAdr()
         OR GenCheckNeg (n, md, p) THEN
	-- nothing, already generated in checks
      ELSE
        IF GenValueOrAddress (n.l, 14, {dcl.REF}+md*{dcl.LVL})
         THEN out.WrStr ("->");
         ELSE out.WrChr (".");
        END;
        dcl.GenFieldPath (n.obj.ext(nms.INFO).nxt);
        out.WrStr (nm);
      END
(*  END  *)

  | pc.nd_index:
      IF (* GenCheckVref()           -- $$$ PR301
      OR *) GenCheckAdr()
      OR    GenCheckNeg (n, md, p)
      THEN
	-- nothing, already generated in checks
      ELSE
        GenIndex (n, p, md);
      END;

  | pc.nd_deref:
      IF GenCheckNeg (n, md, p) THEN
	-- nothing, already generated in check
      ELSIF n.type.mode = pc.ty_array_of THEN
        dcl.TypeDefinition (n.l.type.base);
        IF GenCheckAdr() THEN
          -- nothing, already generated in check
        ELSE
          IF pc.ntag_chk_range IN n.tags
           THEN GenCheckNil (n.l, TRUE);
           ELSE GenValue (n.l, 14, {});
          END;
          ASSERT( dcl.BSA IN md );
          IF n.l.type.flag # pc.flag_c THEN
            out.WrStr ("->");
            out.WrStr (nms.x2c[nms.nm_dynarr_addr]^);
          END;
        END
      ELSE
        c := ~(dcl.REF IN md)
           &  (cc.op_ptrtoarr OR (n.type.mode # pc.ty_array));
        b := (p >= 13) & c;
        IF b THEN out.WrChr ('(') END;
        IF c THEN
          IF p = 12 THEN out.WrChr (' ') END; (* avoid '/*' *)
          out.WrChr ('*');
        END;
        IF pc.ntag_chk_range IN n.tags THEN
          GenCheckNil (n.l, TRUE);
        ELSIF b THEN
          GenValue (n.l, 13, {});
        ELSE
          GenValue (n.l, p, {});
        END;
        IF b THEN out.WrChr (')') END;
      END;

  | pc.nd_replace:
      IF GenCheckNeg (n, md, p) THEN
	-- nothing, already generated in check
      ELSE
        IF (dcl.LVL IN md) & ({dcl.REF, dcl.BSA}*md = {}) THEN
          IF p > 13 THEN out.WrChr ('(') END;
          out.WrStr ("*(");
          GenValue (n.l, 0, {});
          out.WrChr (',');
          GenValue (n.r, 0, md+{dcl.REF});
          out.WrChr (')');
          IF p > 13 THEN out.WrChr (')') END;
        ELSE
          IF p >= 0 THEN out.WrChr ('(') END;
          GenValue (n.l, 0, {});
          out.WrChr (',');
          GenValue (n.r, 0, md);
          IF p >= 0 THEN out.WrChr (')') END;
        END
      END;

  | pc.nd_binary:
      IF GenCheckVref() THEN
        -- nothing, already generated in checks
      ELSE
        GenValueBinary (n, p, md);
      END;

  | pc.nd_unary:
      IF  (n.sub = pc.su_cast)
      OR  (n.sub = pc.su_conv)
       & ~(pc.ntag_chk_range IN n.tags)
       &   ConvIsCast (n.l.type, n.type)
      THEN
        GenValueCast (n, p, md);
      ELSIF GenCheckVref() THEN
        -- nothing, already generated in checks
      ELSE
        GenValueUnary (n, p, md);
      END;

  | pc.nd_value:
      IF GenCheckVref()
      OR GenCheckNeg (n, md, p)
      THEN
	-- nothing, already generated in checks
      ELSE
        IF n.type.mode IN pc.VPTRs THEN
          IF n.val.is_zero() THEN
            out.WrChr ("0");
          ELSE
            IF p > 13 THEN out.WrChr ("(") END;
            GenTypeCast ("", n.type);
            dcl.GenConstAggr (n.val, n.type, 13, dcl.CNS IN md, dcl.CCH IN md);
            IF p > 13 THEN out.WrChr (")") END;
          END;
        ELSIF pc.ntag_hex IN n.tags THEN
          dcl.ConstHexNum (n.val, n.type, p);
        ELSE
          dcl.GenConstAggr (n.val, n.type, p, dcl.CNS IN md, dcl.CCH IN md);
        END
      END;

  | pc.nd_aggregate:
      IF GenCheckVref()
      OR GenCheckNeg (n, md, p)
      THEN
	-- nothing, already generated in checks
      ELSE
	IF n.val # NIL THEN              -- const aggregate (*@2:*)
          dcl.GenConstAggr (n.val, n.type, p, dcl.CNS IN md, dcl.CCH IN md);
	ELSE
          CASE n.type.mode OF
          | pc.ty_set   : GenSetAggregate (n, p, md);
          | pc.ty_array : GenArrayAggregate (n);
          | pc.ty_record: GenRecordAggregate (n);
          ELSE
            env.errors.Error (n.pos, 1011);
          END
	END
      END;

  | pc.nd_call:
      IF GenCheckVref()
      OR GenCheckNeg (n, md, p)
      THEN
	-- nothing, already generated in checks
      ELSE
        GenCall (n, p, md);
      END

  | pc.nd_assign:
      IF GenCheckNeg (n, md, p) THEN
	-- nothing, already generated in checks
      ELSE
        GenAssign (n, p, md);
      END;

  | pc.nd_sequence:
      GenValueSequence (n, md);

  | pc.nd_guard
  , pc.nd_eguard:
      IF (n.l.mode = pc.nd_deref) OR (n.type.mode = pc.ty_pointer) THEN
	ASSERT( n.type.flag = pc.flag_o2 );
	IF p > 13 THEN out.WrChr ('(') END;
	IF n.l.mode = pc.nd_deref THEN
          IF ~(dcl.REF IN md)
           THEN out.WrChr ('*')
           ELSE EXCL (md, dcl.REF)
          END;
          l := n.l.l;
          GenTypeCast ("*", n.type);
        ELSE
          l := n.l;
          ASSERT( l.type.mode = pc.ty_pointer );
          IF (dcl.LVL IN md) & ~(dcl.REF IN md) THEN
            out.WrChr ('*');
            INCL (md, dcl.REF);
          END;
          IF dcl.REF IN md
           THEN GenTypeCast ("*", n.type);
           ELSE GenTypeCast ("", n.type);
          END;
	END;
        ASSERT( l.type.mode = pc.ty_pointer );
	IF pc.ntag_chk_range IN n.tags THEN
          IF {dcl.REF, dcl.LVL}*md # {} THEN
            IF ~(dcl.REF IN md) THEN
              out.WrChr ('*');
              INCL (md, dcl.REF)
            END;
            out.WrChr ('(');
            dcl.MakeTempPtr (l.type, nm(*=>*));
            out.WrStr (nm); out.WrStr (' = ');
            GenValue (l, 0, md); out.WrChr (',');
            IF n.mode = pc.nd_guard
             THEN nn := nms.nm_guardp;
             ELSE nn := nms.nm_guardpe;
            END;
            out.WrFmt ("%s(*", nms.x2c[nn]^);
            out.WrStr (nm);
            out.WrChr (',');
            GenTypeName (n.type.base, 0);
            out.WrStr ('),');
            out.WrStr (nm);
            out.WrChr (')');
          ELSE
            IF n.mode = pc.nd_guard
             THEN nn := nms.nm_guardp;
             ELSE nn := nms.nm_guardpe;
            END;
            out.WrFmt ("%s(", nms.x2c[nn]^);
            GenValue (l, 0, md);
            out.WrChr (',');
            GenTypeName (n.type.base, 0);
            out.WrChr (')');
          END;
	ELSE
          GenValue (l, 13, md);
	END;
	IF p > 13 THEN out.WrChr (')') END;
      ELSE
	IF p > 13 THEN out.WrChr ('(') END;
	IF ~(dcl.REF IN md) THEN out.WrChr ('*') END;
        GenTypeCast ("*", n.type);
	IF pc.ntag_chk_range IN n.tags THEN
	  ASSERT( n.l.mode = pc.nd_var );
	  ASSERT( n.l.obj.mode = pc.ob_varpar );
	  ASSERT( n.l.obj.type.mode = pc.ty_record );
          IF n.mode = pc.nd_guard
           THEN nn := nms.nm_guardv;
           ELSE nn := nms.nm_guardve;
          END;
          out.WrFmt ("%s(", nms.x2c[nn]^);
          GenType (n.l, 0);
          out.WrChr (',');
          GenTypeName (n.type, 0);
          out.WrStr ("),");
          IF ~dcl.IsPtr (n.l.obj, dcl.cur_proc) THEN out.WrChr ('&') END;
          dcl.ObjectUsage (n.l.obj, nm(*=>*));
          out.WrFmt ("%s)", nm);
	ELSE
          GenValue (n.l, 13, {dcl.REF, dcl.ANY});
	END;
	IF p > 13 THEN out.WrChr (')') END;
      END;

  | pc.nd_prot:
      IF GenCheckVref() THEN
        -- nothing, already generated in checks
      ELSE
        out.WrFmt ("%s()", nms.x2c[nms.nm_prot]^);
      END;

  | pc.nd_lconv:
      GenValueCast (n, p, md);

  | pc.nd_if:
      IF p >= 2 THEN out.WrChr ('(') END;
      GenValue (n.l, 2, {});
      out.WrStr (" ? ");
      GenValue (n.r.l, 2, md);
      out.WrStr (" : ");
      GenValue (n.r.r, 2, md);
      IF p >= 2 THEN out.WrChr (')') END;
  ELSE
    env.errors.Error (n.pos, 1008);
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GenValue") END;
<* END *>
END GenValue;

----------------------------------------------------------------------------

PROCEDURE ini*;
BEGIN
<* IF NODE_VALUE_REPRESENTATION THEN *>
  zz_gvm := ir.NewIntValue (0);
  zz_tmp := ir.NewIntValue (0);
  zz_128 := ir.NewIntValue (128);
  zz_m32 := ir.NewIntValue (-32);
  zz_32  := ir.NewIntValue (32);
  zz_one := ir.NewIntValue (1);
<* ELSE *>
  zz_gvm := dcl.NewIntValue (0);
  zz_tmp := dcl.NewIntValue (0);
  zz_128 := dcl.NewIntValue (128);
  zz_m32 := dcl.NewIntValue (-32);
  zz_32  := dcl.NewIntValue (32);
  zz_one := dcl.NewIntValue (1);
<* END *>
END ini;

----------------------------------------------------------------------------

PROCEDURE exi*;
BEGIN
  zz_gvm := NIL;
  zz_tmp := NIL;
  zz_128 := NIL;
  zz_m32 := NIL;
  zz_32  := NIL;
  zz_one := NIL;
END exi;

BEGIN
  dcl.GenValue := GenValue;
END ccExpr.
