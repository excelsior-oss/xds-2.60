(** Copyright (c) 1991,98 XDS Ltd, Russia. All Rights Reserved. *)
(** Definitions *)
MODULE ccDef;

(* Modifications:
   09/Jul/98 Vit  5.07  @3: explicit casting "return 0" to enum type required
   20/Apr/98 Vit  5.04  @2: NODE-VALUE representation change
   05/Apr/98 Vit  5.00  Comments inserted; some code/naming clarification done.
                        Tracing inserted in DB_TRACE mode
   (XDS versions below, BE - above)
   08/Feb/96 Ned  2.12  op_gendll - X2C_DLL_TAG is generated instead of
                        X2C_IMPORT/EXPORT_FUNC/DATA for externals.
   17/Mar/96 Ned  2.12  Comments copying is improved.
   03/Apr/96 Ned  2.14  RecordDefinition: process ccRec.m_dummy.
*)

IMPORT
  pc  := pcK,
  env := xiEnv,
  xfs := xiFiles,
  cc  := ccK,
  nms := ccNames,
  out := ccOut,
  dcl := ccDcl,
  cmt := ccComments,
  ccr := ccRec,
  seg := ccSeg,
  exp := ccExpr,
<* IF DB_TRACE THEN *>
  dbg := pcVisIR,
<* END *>
  xcStr;

TYPE
  STR = out.STR;

  RecListPtr = POINTER TO RecList;
  RecList    = RECORD
    rec : pc.STRUCT;
    next: RecListPtr;
  END;

VAR
  udf_segm	 : out.SEGM; (* inserted at the end of procedure *)
  gen_main       : BOOLEAN;
  md_desc        : STR; (* current module type descriptor *)
  last_td    	 : pc.OBJECT; (* last defined type descriptor *)
  eq_glomark     : env.String;
  GenSizeUsage*  : PROCEDURE (   o: pc.OBJECT
                             ; dim: INTEGER
                             ;   p: INTEGER);
  ret_emul      *: BOOLEAN; (* TRUE если нужно сгенерить код для освобождения
				локальных переменных динамического размера
				или код удаления ловушки,
				т.е. нельзя генерить return
                                в текущей процедуре *)
  GenSequence*   : PROCEDURE (      n: pc.NODE
		             ;    lev: INTEGER
			     ; VAR dd: BOOLEAN
			     ; VAR rt: BOOLEAN
			     ;nrt,trt: BOOLEAN
			     ;   loop: pc.NODE);

  reclist        : RecListPtr;

-----------------------------------------------------------------------------

PROCEDURE OutComment* ( s-: ARRAY OF CHAR );
(*
*)
BEGIN
  IF cc.op_cpp
   THEN out.WrStr ("// ");
   ELSE out.WrStr ("/* ");
  END;
  out.WrStr (s);
  IF ~cc.op_cpp THEN out.WrStr (" */") END;
  out.WrNL;
END OutComment;

(*------------------------ Definitions -----------------------------------*)

PROCEDURE OffsetDefinition ( bs-,rec-: ARRAY OF CHAR
                           ;        t: pc.STRUCT);
(*
*)
  -----------------------------------------------

  PROCEDURE Ofs ( nm-: ARRAY OF CHAR );
  (*
  *)
  BEGIN
    out.WrSpc (1);
    IF rec = ""
     THEN out.WrFmt ("&%s,", nm);
     ELSE out.WrFmt ("%s(%s,%s),", nms.x2c[nms.nm_ofs]^, rec, nm);
    END;
    out.WrNL;
  END Ofs;

  -----------------------------------------------
VAR bf0,bf1,nm: STR;

BEGIN
  ASSERT( t.flag = pc.flag_o2 );
  IF t.mode = pc.ty_record THEN
    dcl.OSecondName (t.obj, 1, "_desc", nm(*=>*));
    out.WrSpc (1);
    out.WrFmt ("%s,", nms.x2c[nms.nm_ofs_rec]^);
    out.WrNL;
    out.WrSpc (1);
    out.WrFmt ("&%s,", nm);
    out.WrNL;
    Ofs (bs);
  ELSIF t.mode = pc.ty_pointer THEN
    Ofs (bs);
  ELSIF t.mode IN pc.ARRs THEN
    out.WrSpc (1);
    out.WrFmt ("%s,", nms.x2c[nms.nm_ofs_arr]^);
    out.WrNL;
    COPY (bs, bf0);
    COPY (bs, bf1);
    WHILE t.mode IN pc.ARRs DO
      out.StrFmt (nm, "[%d]", t.len-1);
      nms.StrCat (bf1, nm);
      nms.StrCat (bf0, "[0]");
      t := t.base;
    END;
    Ofs (bf1);
    OffsetDefinition (bf0, rec, t);
  ELSE
    ASSERT( FALSE );
  END;
END OffsetDefinition;

-----------------------------------------------------------------------------

PROCEDURE OutStr ( s: ARRAY OF CHAR );
(*
*)
VAR b: BOOLEAN;

BEGIN
 out.DisableWrap (b(*=>*));
  out.WrChr ('"');
  out.WrStr (s);
  out.WrChr ('"');
 out.RestoreWrap (b);
END OutStr;

-----------------------------------------------------------------------------

PROCEDURE IsO2var ( o: pc.OBJECT )  :  BOOLEAN;
(*
*)
BEGIN
  RETURN (o.mode = pc.ob_var)
       & (o.type.flag = pc.flag_o2)
       & (o.type.mode IN (pc.TY_SET{pc.ty_record, pc.ty_pointer}+pc.ARRs));
END IsO2var;

-----------------------------------------------------------------------------

PROCEDURE IniVarAddr ( l: pc.OBJECT );
(*
   генерит инициализацию указателей для переменных,
   у которых явно задан адрес
*)
VAR nm: STR;

BEGIN
  WHILE l # NIL DO
    IF (l.mode = pc.ob_var) & (l.attr # NIL) THEN
      dcl.ObjectUsage (l, nm(*=>*));
      out.WrSpc (1); out.WrFmt ("%s = ", nm);
      dcl.TypeDesignator (nm(*=>*), "*", l.type);
      out.WrChr ('('); out.WrStr (nm); out.WrChr (')');
      exp.GenValue (l.attr, 13, {});
      out.WrChr (";");
      out.WrNL;
    END;
    l := l.next;
  END;
END IniVarAddr;

-----------------------------------------------------------------------------

PROCEDURE Sl1GloMark;
(*
*)
BEGIN
  IF eq_glomark = NIL THEN RETURN END;
  IF eq_glomark^ = "" THEN RETURN END;
  out.WrFmt ("/* %s */", eq_glomark^);
  out.WrNL;
END Sl1GloMark;

-----------------------------------------------------------------------------

PROCEDURE OutTypeBases ( r: pc.STRUCT );
(*
   Static initialization
*)
VAR
  nm: STR;
  i : LONGINT;

  -----------------------------------------------

  PROCEDURE OutBase ( r: pc.STRUCT );
  (*
  *)
  BEGIN
    IF (r = NIL) OR (r.flag # pc.flag_o2) THEN RETURN END;
    OutBase (r.base);
    IF i > 0 THEN out.WrStr (", ") END;
    ASSERT( r.mode = pc.ty_record );
    dcl.ObjectUsage (r.obj, nm(*=>*)); (* force declaration *)
    dcl.OSecondName (r.obj, 1, "_desc", nm(*=>*)); out.WrFmt ("&%s", nm);
    INC (i);
  END OutBase;
  -----------------------------------------------

VAR x: RecListPtr;

BEGIN
  i := 0;
  out.WrStr ("{ ");
  IF cc.op_gendll THEN
    NEW (x);
    x.rec := r;
    x.next := reclist;
    reclist := x;

    (* to force declaration only *)
    WHILE (r # NIL) & (r.flag = pc.flag_o2) DO
      ASSERT( r.mode = pc.ty_record );
      dcl.ObjectUsage (r.obj, nm(*=>*));
      dcl.OSecondName (r.obj, 1, "_desc", nm(*=>*));
      r := r.base;
    END;
  ELSE
    OutBase (r);
  END;
  WHILE i <= pc.code.max_ext_lev DO
    IF i > 0 THEN
      out.WrStr (", ")
    END;
    out.WrStr ("0");
    INC (i)
  END;
  out.WrStr (" }");
END OutTypeBases;

-----------------------------------------------------------------------------

PROCEDURE InitBases ( r: pc.STRUCT );
(*
   Dynamic initialization for GENDLL
*)
VAR nm,ds: STR;

BEGIN
  ASSERT( cc.op_gendll );
  dcl.OSecondName (r.obj, 1, "_desc", ds(*=>*));
  WHILE (r # NIL) & (r.flag = pc.flag_o2) DO
    ASSERT( r.mode = pc.ty_record );
    dcl.ObjectUsage (r.obj, nm(*=>*)); (* force declaration *)
    dcl.OSecondName (r.obj, 1, "_desc", nm(*=>*));
    out.WrSpc (1);
    out.WrFmt ("%s.base[%d] = &%s;", ds, r.len, nm);
    out.WrNL;
    r := r.base;
  END;
END InitBases;

-----------------------------------------------------------------------------

PROCEDURE InitRecordsBases;
(*
*)
BEGIN
  WHILE reclist # NIL DO
    InitBases (reclist.rec);
    reclist := reclist.next;
  END;
END InitRecordsBases;

-----------------------------------------------------------------------------

PROCEDURE TypeDescDefinition ( t: pc.STRUCT );
(*
*)
VAR
  bf     : ARRAY 1024 OF pc.OBJECT;
  bf_cnt : LONGINT;
  type_nm: STR;
  md     : pc.OBJECT;
  -----------------------------------------------------------------------------

  PROCEDURE OutProcs;
  (*
  *)
  VAR
    i,j: LONGINT;
    nm : STR;
  BEGIN
    j := bf_cnt-1;
    IF j < 0 THEN INC (j) END;
    FOR i := 0 TO j DO
      out.WrSpc (1);
      IF bf[i] = NIL THEN
        out.WrStr ("0");
      ELSE
        dcl.ObjectUsage (bf[i], nm(*=>*));
        out.WrFmt ("(%s)%s", nms.x2c[nms.nm_proc]^, nm);
      END;
      IF i # j THEN
        out.WrChr (',')
      END;
      out.WrNL;
    END;
  END OutProcs;

  -----------------------------------------------------------------------------

  PROCEDURE OutPtrsRec ( rec-: ARRAY OF CHAR
                       ;    r: pc.STRUCT);
  (*
  *)
  VAR
    o : pc.OBJECT;
    nm: STR;
  BEGIN
    IF (r.base # NIL) & (r.base.flag = pc.flag_o2) THEN
      OutPtrsRec (rec, r.base);
    END;
    o := r.prof;
    WHILE o # NIL DO
      ASSERT( o.mode IN pc.OB_SET{pc.ob_field, pc.ob_field_bts} );
      IF (o.type.flag = pc.flag_o2)
       & (o.type.mode IN (pc.TY_SET{pc.ty_record, pc.ty_pointer}+pc.ARRs))
      THEN
        dcl.ObjectUsage  (o, nm(*=>*));
	OffsetDefinition (nm, rec, o.type);
      END;
      o := o.next;
    END;
  END OutPtrsRec;

  -----------------------------------------------------------------------------

  PROCEDURE OutPtrs ( rec-: ARRAY OF CHAR );
  (*
  *)
  BEGIN
    OutPtrsRec (rec, t);
    out.WrSpc (1);
    out.WrFmt ("%s", nms.x2c[nms.nm_ofs_end]^);
    out.WrNL;
  END OutPtrs;
  -----------------------------------------------

  PROCEDURE GetProcs ( b: pc.STRUCT );
  (*
  *)
  VAR
    o: pc.OBJECT;
    i: LONGINT;
  BEGIN
    bf_cnt := 0;
    FOR i := 0 TO LEN (bf)-1 DO
      bf[i] := NIL
    END;
    WHILE b # NIL DO
      o := b.mem;
      WHILE o # NIL DO
	IF o.type.len >= bf_cnt THEN
          bf_cnt := o.type.len+1
        END;
	IF bf[o.type.len] = NIL THEN
          bf[o.type.len] := o
        END;
	o := o.next;
      END;
      b := b.base;
    END;
  END GetProcs;
  -----------------------------------------------

VAR ds,ps,of,ls: STR;

BEGIN
  md := pc.mods[t.mno];
  IF md_desc = "" THEN
    dcl.OSecondName (md, 2, "_desc", md_desc(*=>*))
  END;
  ASSERT( t.obj # NIL );
  ASSERT( t.mode = pc.ty_record );
  ASSERT( t.obj.mno = dcl.CurMod.mno );
  ASSERT( t.flag = pc.flag_o2 );
  ASSERT( dcl.TypeUsage (type_nm(*=>*), t, FALSE, FALSE) );
  GetProcs (t);
  dcl.OSecondName (t.obj, 1, "_desc", ds(*=>*));
  dcl.OSecondName (t.obj, 2, "_offs", of(*=>*));
  dcl.OSecondName (t.obj, 3, "_proc", ps(*=>*));
  out.WrFmt ("static %s %s[] = {", nms.x2c[nms.nm_proc]^, ps);
  out.WrNL;
  OutProcs;
  out.WrFmt ("};");
  out.WrNL;
  out.WrFmt ("static void * %s[] = {", of);
  out.WrNL;
  OutPtrs   (type_nm);
  out.WrFmt ("};");
  out.WrNL;
  out.WrFmt ("struct %s %s = {", nms.x2c[nms.nm_td_str]^, ds);
  out.WrNL;
  out.WrSpc (1);
  out.WrFmt ("sizeof(%s), ", type_nm);
  OutStr (t.obj.name^);
  out.WrChr (",");
  out.WrNL;
  out.WrSpc (1);
  out.WrFmt ("&%s, ", md_desc);
  IF last_td = NIL THEN
    out.WrStr ("0, ");
  ELSE
    dcl.ObjectUsage (last_td, ls(*=>*));
    dcl.OSecondName (last_td, 1, "_desc", ls(*=>*));
    out.WrFmt ("&%s, ", ls);
  END;
  last_td := t.obj;
  out.WrFmt ("%d, %d,", bf_cnt, t.len);
  out.WrNL;
  out.WrSpc (1); OutTypeBases (t); out.WrChr (',');
  out.WrNL;
  out.WrSpc (1); out.WrFmt ("%s, %s, 0, 0, 0, &%s, 0x93678150l", ps, of, ds);
  IF ~ cc.op_krc THEN
    out.WrChr ('u')
  END;
  out.WrNL;
  out.WrFmt ("};");
  out.WrNL;
END TypeDescDefinition;

-----------------------------------------------------------------------------

PROCEDURE RecordDefinition ( t: pc.STRUCT );
  -----------------------------------------------------------------------------

  PROCEDURE NormalField (    f: pc.OBJECT
                        ;  lev: INTEGER
                        ; path: nms.INFO);
  (*
  *)
  BEGIN
<* IF DB_TRACE THEN *>
    IF out.EnDebug THEN dbg.CallTraceInd (1, "(normal field: %s", f.name^) END;
<* END *>

    IF cc.op_comments THEN
      cmt.OutNextComment (f.pos, lev)
    END;
    out.WrSpc (lev);
    dcl.Declarator (f, FALSE);
    out.WrChr (';');
    IF cc.op_comments THEN
      cmt.OutTailComment (f.pos)
    END;
    out.WrNL;
    INCL (f.tags, cc.otag_declared);
    f.ext (nms.INFO).nxt := path;
    (* nms.Disable (t,fnm); ?!!!!! *)

<* IF DB_TRACE THEN *>
    IF out.EnDebug THEN dbg.CallTraceInd (-1, ")normal field") END;
<* END *>
  END NormalField;

  ----------------------------------------------------------------

  PROCEDURE BitField (    f: pc.OBJECT
                     ;  lev: INTEGER
                     ; path: nms.INFO);
  (*
  *)
  VAR buf, bts: STR;
  BEGIN
    IF (f.type.mode IN pc.TY_SET{pc.ty_range, pc.ty_enum})
     & (f.type.obj # NIL)
    THEN
      dcl.ObjectUsage (f.type.obj, buf(*=>*));
      out.StrFmt (bts, "BITS_%s", buf);
    ELSIF f.type.mode = pc.ty_char THEN
      bts := "BITS_char";
    ELSIF f.type.mode = pc.ty_boolean THEN
      bts := "BITS_bool";
    ELSE
      out.StrFmt (bts, "%d", f.attr.val.get_integer());
    END;
    out.WrSpc (lev);
    dcl.Declarator (f, FALSE);
    out.WrFmt (" : %s;", bts);
    IF cc.op_comments THEN
      cmt.OutTailComment (f.pos)
    END;
    out.WrNL;
    INCL (f.tags, cc.otag_declared);
    f.ext(nms.INFO).nxt := path;
  END BitField;

  -----------------------------------------------------------------------------

  PROCEDURE BitFieldOrFiller (       l: ccr.Node
                             ; lev,ofs: INTEGER
                             ;    path: nms.INFO);
  BEGIN
    IF l.mode = ccr.m_filler THEN
      out.WrSpc (lev);
      out.WrFmt ("int _%02d_%02d: %d;", l.offs, ofs, l.bits);
      out.WrNL;
    ELSE
      BitField (l.obj, lev, path);
    END;
  END BitFieldOrFiller;

  --------------------------------------------------------------

  PROCEDURE WordBck (    l: ccr.Node
                    ;  lev: INTEGER
                    ; path: nms.INFO);
  (*
  *)
  VAR ofs: INTEGER;

  BEGIN
    ASSERT( l # NIL );
    ofs := ccr.WORD_BITS;
    REPEAT
      DEC (ofs, l.bits);
      BitFieldOrFiller (l, lev, ofs, path);
      l := l.next;
    UNTIL l = NIL;
  END WordBck;

  ------------------------------------------------------------------

  PROCEDURE WordFwd (       l: ccr.Node
                    ; lev,ofs: INTEGER
                    ;    path: nms.INFO);
  (*
  *)
  BEGIN
    IF l = NIL THEN RETURN END;
    DEC (ofs, l.bits);
    WordFwd (l.next, lev, ofs, path);
    BitFieldOrFiller (l, lev, ofs, path);
  END WordFwd;

  ---------------------------------------------------------

  PROCEDURE Gen (    n: ccr.Node
                ;  lev: INTEGER
                ; path: nms.INFO);
  (*
  *)
  VAR
    nm: STR;
    l : ccr.Node;
    i : INTEGER;
    inf,inm: nms.INFO;
  BEGIN
    CASE n.mode OF
    | ccr.m_struct
    , ccr.m_union:
        out.WrSpc (lev);
        IF n.mode = ccr.m_struct
         THEN out.WrStr ("struct ");
         ELSE out.WrStr ("union ");
        END;
        IF lev = 0 THEN
          dcl.OName (t.obj, nm(*=>*));
          out.WrStr (nm); out.WrChr (' ');
          inf := path;
          inm := NIL;
        ELSIF (n.mode = ccr.m_union) & cc.op_cpp THEN
          inf := path;
          inm := NIL;
        ELSE
          IF path = NIL
           THEN i := 1
           ELSE i := path.no+1
          END;
          inf := nms.MakeName (nm(*=>*), t, NIL, pc.ob_field, "_", i);
	  inf.nxt := path;
          inm := inf;
        END;
        out.WrChr ("{");
        out.WrNL;
        l := n.down;
        IF l = NIL THEN (* empty variant *)
          env.errors.Error (t.obj.pos, 1017);
        END;
        WHILE l # NIL DO
          Gen (l, lev+1, inf);
          l := l.next
        END;
        out.WrSpc (lev);
        out.WrChr ('}');
        IF inm # NIL THEN
          out.WrChr (' ');
          out.WrStr (inm.name^)
        END;
        out.WrChr (";");
         out.WrNL;
    | ccr.m_dummy:
        out.WrSpc (lev);
        out.WrFmt ("%s _dummy_;", nms.x2c[nms.nm_longint]^);
        out.WrNL;
    | ccr.m_sl1:
        (* filler for proper word offset must be generated
           before field
        *)
    | ccr.m_field:
        (* word offset must be ignored *)
        NormalField (n.obj, lev, path);
    | ccr.m_filler:
          COPY (nms.x2c[nms.nm_longint]^, nm);
        out.WrSpc (lev);
        IF n.size = 1
         THEN out.WrFmt ("%s _%d_;",     nm, n.offs);
         ELSE out.WrFmt ("%s _%d_[%d];", nm, n.offs, n.size);
        END;
        out.WrNL;
    | ccr.m_word:
        ASSERT( n.size = 1 );
          out.WrSpc (lev); INC (lev);
          out.WrStr ("#ifdef X2C_XE"); out.WrNL;
        WordBck (n.down, lev, path);
          out.WrSpc (lev-1); out.WrStr ("#else"); out.WrNL;
          WordFwd (n.down, lev, ccr.WORD_BITS, path); DEC (lev);
          out.WrSpc (lev); out.WrStr ("#endif"); out.WrNL;
    END;
  END Gen;
  -----------------------------------------------

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(RecordDefinition: t = [%X]", t) END;
<* END *>

   IF ~(cc.otag_pub_defined IN t.obj.tags) THEN
      out.WrNL;
   END;
  IF cc.op_comments THEN
    cmt.OutFirstComment (t.pos, 1, 0)
  END;

  IF ~ (cc.otag_pub_defined IN t.obj.tags) THEN
    ccr.Record (t);
    Gen (t.ext(nms.TREE).node(ccr.Node), 0, NIL);
  END;

  IF ~ (cc.otag_pri_defined IN t.obj.tags)
   & ~ dcl.gen_def
   & (t.flag = pc.flag_o2)
  THEN
    TypeDescDefinition (t);
  END;

    IF cc.op_comments THEN
      cmt.OutLastComment (t.end, 0);
    END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")RecordDefinition") END;
<* END *>
END RecordDefinition;

-----------------------------------------------------------------------------

PROCEDURE ObjTypeDefinition ( o: pc.OBJECT );
(*
*)
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(ObjTypeDef: '%s'", o.name^) END;
<* END *>

  IF (o.type.obj # o) THEN
      dcl.ObjectDeclaration (o);
  ELSIF o.type.mode = pc.ty_record THEN
   RecordDefinition (o.type);
  ELSE
    dcl.ObjectDeclaration (o);
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")ObjTypeDef") END;
<* END *>
END ObjTypeDefinition;

-----------------------------------------------------------------------------

PROCEDURE VarDefinition ( o: pc.OBJECT );
(*
*)
BEGIN
  IF dcl.gen_def THEN RETURN END;
  dcl.ObjectDeclaration (o);
  IF cc.otag_pri_defined IN o.tags THEN RETURN END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(VarDefinition: '%s'", o.name^) END;
<* END *>

  ASSERT( o.lev = 0 );
  ASSERT( o.mno = dcl.CurMod.mno );
  IF ~o.is_public() THEN
    out.WrStr ("static ");
  END;
  IF ~cc.op_krc
   & (o.mode = pc.ob_var)
   & (pc.otag_volatile IN o.tags) THEN
    out.WrStr ("volatile ");
  END;
  dcl.Declarator (o, FALSE);
  IF o.mode = pc.ob_cons THEN
    out.WrFmt (" = ");
    exp.GenValue (o.val, 1, {dcl.CNS});
  ELSIF o.attr # NIL THEN
    ASSERT( o.mode = pc.ob_var );
    out.WrFmt(" = ");
    dcl.VarAddrExpression (o);
    INCL (o.tags, cc.otag_pri_defined);
  ELSE
    -- nothing
  END;
  out.WrChr (";");
  out.WrNL;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")VarDefinition") END;
<* END *>
END VarDefinition;

-----------------------------------------------------------------------------

PROCEDURE CopyFuncParams;
(*
*)
VAR
  nm,buf: STR;
  l     : pc.OBJECT;
BEGIN
  (* copy params *)
  IF dcl.cur_proc.flag IN pc.LangSet{pc.flag_o2, pc.flag_m2} THEN
    l := dcl.cur_proc.prof;
    WHILE l # NIL DO
      IF  dcl.IsCarray (l.type)
       &  (l.mode = pc.ob_var)
       & ~(pc.otag_RO IN l.tags)
      THEN
	dcl.OName (l, nm(*=>*));
	IF l.type.mode = pc.ty_array_of THEN
	  ret_emul := TRUE;
	  out.WrSpc (1);
          out.WrFmt ("%s((void **)&%s,", nms.x2c[nms.nm_alloc_param]^, nm);
          exp.GenSizeUsage (l, 0, 0);
          out.WrFmt (");");
	ELSE
          dcl.TypeDesignator (buf(*=>*), "*", dcl.GetBaseType (l.type));
	  out.WrSpc (1); out.WrFmt ("%s = (%s)", nm, buf);
          dcl.MakeTempVar (l.type, buf(*=>*));
	  out.WrFmt ("%s(%s,%s,", nms.x2c[nms.nm_memcpy]^, buf, nm);
          dcl.GenSizeof (l.type);
          out.WrFmt (");");
	END;
        out.WrNL;
      END;
      l := l.next;
    END;
  END;
  (* ini local vars with a paticular address *)
  IniVarAddr (dcl.cur_proc.mem);
END CopyFuncParams;

-----------------------------------------------------------------------------

PROCEDURE FreeFuncParams;
(*
*)
VAR
  nm: STR;
  l : pc.OBJECT;
BEGIN
  (* free params *)
  IF dcl.cur_proc.flag < pc.flag_c THEN
    l := dcl.cur_proc.prof;
    WHILE l # NIL DO
      IF (l.type.mode = pc.ty_array_of)
       & (l.mode = pc.ob_var)
       & ~(pc.otag_RO IN l.tags)
      THEN
	dcl.OName (l, nm(*=>*));
	out.WrSpc (1);
        out.WrFmt ("%s(%s);", nms.x2c[nms.nm_free_param]^, nm);
        out.WrNL;
      END;
      l := l.next;
    END;
  END;
END FreeFuncParams;

-----------------------------------------------------------------------------

PROCEDURE ClearO2Vars ( o: pc.OBJECT );
(*
*)
VAR nm: STR;

BEGIN
  WHILE o # NIL DO
    IF IsO2var (o)
     & (o.type.mode # pc.ty_pointer)
     & (pc.ttag_has_o2_ptr IN o.type.tags)
    THEN
      dcl.ObjectUsage (o, nm(*=>*));
      out.WrSpc (1); out.WrStr ("memset(");
      IF ~dcl.IsCarray (o.type) THEN
        out.WrChr ('&')
      END;
      out.WrFmt ("%s,0,sizeof(%s));", nm, nm);
      out.WrNL;
    END;
    o := o.next;
  END;
END ClearO2Vars;

-----------------------------------------------------------------------------

PROCEDURE FuncDefinition ( o: pc.OBJECT );
(*
   Procedure position (used to place comments correctly):
        o.pos           - position of forward declaration
        o.type.pos..end - positions of normal declaration (last ';')
        o.val.pos       - position of BEGIN
        o.end           - position of END
*)
  -----------------------------------
  PROCEDURE FullName ( o: pc.OBJECT );
  (*
  *)
  BEGIN
    IF o.host # NIL THEN
      FullName (o.host.obj);
      out.WrChr ('.')
    END;
    out.WrStr (o.name^);
  END FullName;

  ------------------------------------
  PROCEDURE LastCmtInDecl;
  (*
     Output correctly possible multi-line comment after the last local name
  *)
  VAR l1,l2,c: LONGINT;
      fnm    : env.String;
  BEGIN
    o.type.pos.unpack (fnm, l1, c);
    o.val.pos.unpack  (fnm, l2, c);
    ASSERT( l2 >= l1 );
    cmt.OutFirstComment (o.val.pos, SHORT(l2-l1), 1); -- decrement l2-l1 lines
  END LastCmtInDecl;

VAR
  nm     : STR;
  dd,rt,b: BOOLEAN;
  buf    : ARRAY 2048 OF CHAR;
  tmps   : dcl.TMP_VAR;
  v      : pc.OBJECT;

  s_loc_segm: out.SEGM;
  s_cur_proc: pc.STRUCT;
  s_tmp_vars: dcl.TMP_VAR;
  s_tmp_busy: dcl.TMP_VAR;
  s_ret_emul: BOOLEAN;
  isloc     : BOOLEAN;

BEGIN
  ASSERT( o.mode IN (pc.PROCs-pc.OB_SET{pc.ob_eproc}) );
  IF dcl.gen_def THEN RETURN END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(FuncDefinition: '%s'", o.name^) END;
<* END *>

  s_loc_segm := dcl.loc_segm;
  s_cur_proc := dcl.cur_proc;
  s_tmp_vars := dcl.tmp_vars;
  s_tmp_busy := dcl.tmp_busy;
  s_ret_emul := ret_emul;

  dcl.tmp_vars := NIL;
  dcl.tmp_busy := NIL;

  out.WrNL;
  IF cc.op_comments THEN
    cmt.OutFirstComment (o.type.pos, 1, 0);
  END;
  dcl.OutLineNo (o.type.pos, 0);
  IF nms.FuncIsExtern (o) THEN
    Sl1GloMark;
    IF ~cc.op_krc THEN
      out.WrStr ("extern ")
    END;
  ELSE
    out.WrStr ("static ");
  END;
  dcl.Declarator (o, FALSE);
  dcl.FuncProfileDefinition (o.type, buf(*=>*),
                             TRUE, FALSE);
  out.WrFmt ("(%s)", buf);
  out.WrNL;
  dcl.FuncProfileDefinition (o.type, buf(*=>*), TRUE, TRUE);
  out.WrStr (buf);
  IF cc.op_comments THEN
    cmt.OutLastComment (o.type.end, 0);
  END;

  out.WrChr ("{");
  out.WrNL;
  dcl.cur_proc := o.type;
  ASSERT( dcl.cur_proc # NIL );
  ret_emul := FALSE;
  out.GetSegment (dcl.loc_segm);   (* is used in params copy *)
  out.GetSegment (udf_segm);

  v := o.type.mem;                  (* Locals processing *)
  isloc := FALSE;
  WHILE v # NIL DO
    IF (v.mode = pc.ob_var)
    OR (v.mode = pc.ob_cons)
     & ~(pc.otag_no_threat IN v.tags)
       (* если константы не описывать заранее то возникают проблемы
          с константами, которые описаны уровнем выше, чем уровень
          использования
       *)
    THEN
      dcl.ObjectDeclaration (v);
      isloc := TRUE;
    END;
    v := v.next;
  END;

  IF cc.op_comments THEN LastCmtInDecl END;

  IF isloc THEN
    OutComment ("BEGIN");
  END;
  dcl.EnterStatement (tmps(*=>*));

  IF cc.op_debug OR cc.op_profile THEN
    ret_emul := TRUE;
    dcl.OutLineNo (o.val.pos, 1);
    IF cc.op_profile THEN
     out.Push;
     out.DisableWrap (b(*=>*));
      out.WrSpc (1);
      out.WrFmt ('static struct %s %s = { ',
                 nms.x2c[nms.nm_profile_str]^,
                 nms.x2c[nms.nm_prf]^);
      ASSERT( o.host # NIL );
      out.WrChr ('"');
      FullName (o);
      out.WrStr ('", 0, 0, 0, 0, 0 };');
     out.RestoreWrap (b);
      out.WrNL;
      out.AppendSegm (dcl.loc_segm(*=>*));
     out.Pop;
      out.WrSpc (1);
      out.WrFmt ("%s(&%s);",
                nms.x2c[nms.nm_proc_prf]^,
                nms.x2c[nms.nm_prf]^);
    ELSE
      out.WrSpc (1);
      out.WrFmt ("%s();", nms.x2c[nms.nm_proc_inp]^);
    END;
    out.WrNL;
  END;

  CopyFuncParams;                -- =>ret_emul

  GenSequence (o.val.r, 1, dd(*=>*), rt(*=>*), TRUE, ~ret_emul, NIL); -- stats

  ASSERT( dcl.cur_proc # NIL );
  ASSERT( dcl.cur_proc.base # NIL );

  FreeFuncParams;

  IF cc.op_debug OR cc.op_profile THEN
    dcl.OutLineNo (o.end, 1);
    out.WrSpc (1);
    out.WrFmt ("%s();", nms.x2c[nms.nm_proc_out]^);
    out.WrNL;
  END;

  IF (dcl.cur_proc.base.mode # pc.ty_void) & ~rt THEN
    -- We must generate return statement even if control flow never reach it:
    -- C compiler is not obliged to do exectly the same control flow analisys
    out.WrSpc (1); out.WrStr ("return ");
    IF ~ret_emul & dcl.IsCnumberAdr (dcl.cur_proc.base) THEN
      IF dcl.cur_proc.base.mode = pc.ty_enum THEN
        dcl.TypeDesignator (nm(*=>*), "", dcl.cur_proc.base);
        out.WrFmt ("(%s)", nm);
      END;
      out.WrChr ('0');
    ELSE
      dcl.OSecondName (dcl.cur_proc.obj, 0, "_ret", nm(*=>*));
      out.WrStr (nm);
      IF ~dcl.IsCarray (dcl.cur_proc.base) THEN -- create var for ret.value
        dcl.TypeDesignator (buf(*=>*), nm, dcl.cur_proc.base);
        out.Push;
          out.WrSpc (1);
          out.WrFmt ("%s;", buf);
          out.WrNL;
          out.AppendSegm (dcl.loc_segm(*=>*));
        out.Pop;
      END;
    END;
    out.WrChr (";");
    out.WrNL;
  END;
  out.Push;
  ClearO2Vars (dcl.cur_proc.mem);
  out.AppendSegm (dcl.loc_segm(*=>*));
  out.Pop;
  out.AppendSegm (dcl.loc_segm(*=>*));
  dcl.cur_proc := NIL;
  out.WriteSegm (dcl.loc_segm);
  IF cc.op_comments THEN
    cmt.OutNextComment (o.end, 0)
  END;
  out.WriteSegm (udf_segm);
  out.WrStr ("} ");
  dcl.OName (o, nm(*=>*));
  OutComment (nm);
  IF cc.op_comments THEN
    cmt.OutLastComment (o.end, 0);
  END;

  dcl.ExitStatement (tmps(*=>*));
  dcl.tmp_vars := NIL;
  ASSERT( dcl.tmp_busy = NIL );

  dcl.loc_segm := s_loc_segm;
  dcl.cur_proc := s_cur_proc;
  dcl.tmp_vars := s_tmp_vars;
  dcl.tmp_busy := s_tmp_busy;
  ret_emul     := s_ret_emul;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")FuncDefinition") END;
<* END *>
END FuncDefinition;

-----------------------------------------------------------------------------

PROCEDURE CodeFuncDefinition ( o: pc.OBJECT );
(*
*)
VAR ret: STR;

  -----------------------------------------------
  PROCEDURE Protocol ( v: pc.VALUE );
  (*
  *)
  VAR
    ps : LONGINT;
    ch : CHAR;
    nm : STR;
    loc: STR;

    ----------------------------------------
    PROCEDURE Get;
    (*
    *)
    BEGIN
      dcl.zz_tmp.index_get (ps, v);
      ch := CHR (dcl.zz_tmp.get_integer());
      IF ch = 0X THEN RETURN END;
      INC (ps);
    END Get;

    ----------------------------------------
    PROCEDURE GetNm;
    (*
    *)
    VAR i: INTEGER;
    BEGIN
      WHILE ch = ' ' DO Get END;
      i := 0;
      WHILE (ch # ':')
          & (ch # 0X)
          & (ch # ' ')
      DO
	nm[i] := ch;
        INC (i);
        Get;
      END;
      nm[i] := 0X;
    END GetNm;

    ----------------------------------------
    PROCEDURE GetColon;
    (*
    *)
    BEGIN
      WHILE ch = ' ' DO Get END;
      IF ch = ':' THEN
        Get;
        RETURN
      END;
    END GetColon;

    ----------------------------------------
    PROCEDURE GetRegister;
    (*
    *)
    VAR i: INTEGER;

    BEGIN
      WHILE ch = ' ' DO Get END;
      i := 0;
      WHILE (ch # ',')
          & (ch # 0X )
          & (ch # ' ')
          & (ch # ')')
      DO
	loc[i] := ch;
        INC (i);
        Get;
      END;
      loc[i] := 0X;
    END GetRegister;

    ----------------------------------------
    PROCEDURE GetLocation;
    (*
    *)
    BEGIN
      GetRegister;
    END GetLocation;
    ----------------------------------------

  VAR
    str: STR;
    l  : pc.OBJECT;
  BEGIN
    ps := 0;
    Get;
    LOOP
      GetNm;
      IF nm = "" THEN RETURN END;
      IF nm = "USE" THEN
	GetColon;
        WHILE (ch # ')') & (ch # 0X) DO
          Get
        END;
        Get;
      ELSIF nm = "RETURN" THEN
	GetColon;
        GetRegister;
        ret := loc;
      ELSE
	l := o.type.prof;
	WHILE (l # NIL) & (l.name^ # nm) DO
          l := l.next
        END;
	IF l = NIL THEN
	  env.errors.Error (v.pos, 1001, nm);
	  RETURN;
	END;
	GetColon;
        GetLocation;
        dcl.OName (l, str(*=>*));
	out.WrSpc (1);
        out.WrFmt ("asm mov %s, DWORD PTR[%s];", loc, str);
        out.WrNL;
      END;
    END;
  END Protocol;
  -----------------------------------------------

VAR
  buf,str: STR;
  l      : pc.NODE;
  i      : LONGINT;
BEGIN
  IF dcl.gen_def THEN RETURN END;
  ASSERT( o.mode = pc.ob_cproc );
  out.WrNL;
  IF nms.FuncIsExtern (o) THEN
    IF ~cc.op_krc THEN
      out.WrStr ("extern ")
    END;
    IF cc.op_gendll THEN     (*Ned v2.12*)
      out.WrFmt ("%s ", nms.x2c[nms.nm_dll_tag]^)
    END;
  ELSE
    out.WrStr ("static ");
  END;
  out.WrChr (' ');
  dcl.Declarator (o, FALSE);
  dcl.FuncProfileDefinition (o.type, buf(*=>*), TRUE, FALSE);
  out.WrFmt ("(%s)", buf);
  out.WrNL;
  dcl.FuncProfileDefinition (o.type, buf(*=>*), TRUE, TRUE);
  out.WrFmt ("%s{", buf);
  out.WrNL;
  out.GetSegment (dcl.loc_segm);
  dcl.cur_proc := o.type;
  l   := o.val.l;
  ret := "";
  IF (l # NIL) & (l.type.mode = pc.ty_SS) THEN
    Protocol (l.val);
    l := l.next;
  END;
  WHILE l # NIL DO
    ASSERT( l.val # NIL );
    out.WrSpc (1);
    IF l.type.mode = pc.ty_SS THEN
      FOR i := 0 TO l.type.len-1 DO
	dcl.zz_tmp.index_get (i, l.val);
	IF ~ dcl.zz_tmp.is_zero() THEN
          out.WrChr (CHR (dcl.zz_tmp.get_integer()));
        END;
      END;
      out.WrNL;
    ELSE
      out.WrFmt ("asm db 0x%02X;", l.val.get_integer());
      out.WrNL;
    END;
    l := l.next;
  END;
  IF ret # "" THEN
    dcl.MakeTempVar (o.type.base, str(*=>*));
    out.WrSpc (1);
    out.WrFmt ("asm mov DWORD PTR [%s],%s;", str, ret);
    out.WrNL;
    out.WrSpc (1);
    out.WrFmt ("return %s;", str);
    out.WrNL;
  END;
  dcl.cur_proc := NIL;
  out.AppendSegm (dcl.loc_segm(*=>*));
  out.WriteSegm (dcl.loc_segm);
  out.WrChr ("}");
  out.WrNL;
  out.WrNL;
END CodeFuncDefinition;

-----------------------------------------------------------------------------

PROCEDURE GenSizeChecks ( o: pc.OBJECT );
(*
*)
VAR
  nm,fn: STR;
  sz   : LONGINT;
BEGIN
  COPY (nms.x2c[nms.nm_assert]^, fn);
  WHILE o # NIL DO
    IF (o.mode = pc.ob_type)
     & (o.type.obj = o)
     & (cc.otag_declared IN o.tags)
     &~(o.type.mode IN pc.TY_SET{pc.ty_range, pc.ty_enum})
    THEN
      sz := pc.code.get_size (pc.su_bytes, o.type);
      IF sz >= 0 THEN
        ASSERT( dcl.TypeUsage (nm(*=>*), o.type, TRUE, FALSE) ); --type is named
        out.WrSpc (1);
        out.WrFmt ("if (sizeof(%s)!= %d) %s(0);", nm, sz, fn);
        out.WrNL;
      END;
    END;
    o := o.next;
  END;
END GenSizeChecks;

-----------------------------------------------------------------------------

PROCEDURE ChkO2Vars ( o: pc.OBJECT )  :  BOOLEAN;
(*
*)
BEGIN
  WHILE o # NIL DO
    IF IsO2var (o) THEN RETURN TRUE END;
    o := o.next;
  END;
  RETURN FALSE;
END ChkO2Vars;

-----------------------------------------------------------------------------

PROCEDURE ModuleDescDefinition ( m,types: pc.OBJECT );
  -----------------------------------------------

  PROCEDURE ListOffs ( o: pc.OBJECT );
  (*
  *)
  VAR nm: STR;

  BEGIN
    WHILE o # NIL DO
      IF IsO2var (o) THEN
        dcl.ObjectUsage (o, nm(*=>*));
	OffsetDefinition (nm, "", o.type);
      END;
      o := o.next;
    END;
  END ListOffs;

  -----------------------------------------------------------------------------

  PROCEDURE ListCmds (   o: pc.OBJECT
                     ; adr: BOOLEAN);
  (*
  *)
  VAR nm: STR;

  BEGIN
    WHILE o # NIL DO
      IF (o.flag IN pc.LangsAllowCommands)
       & (o.mode = pc.ob_xproc)
       & (o.type.prof = NIL)
       & (o.type.base.mode = pc.ty_void)
      THEN
	ASSERT( pc.otag_public IN o.tags );
        IF adr THEN
          dcl.ObjectUsage (o, nm(*=>*));
          out.WrStr (nm);
	ELSE
          OutStr (o.name^);
	END;
	out.WrChr (',');
      END;
      o := o.next;
    END;
  END ListCmds;

  -----------------------------------------------

VAR of,ls,cmd,cnm: STR;

BEGIN
  IF md_desc = "" THEN
    dcl.OSecondName (m, 2, "_desc", md_desc(*=>*))
  END;
  dcl.OSecondName (m, 3, "_offs", of(*=>*));
  dcl.OSecondName (m, 4, "_cmds", cmd(*=>*));
  dcl.OSecondName (m, 5, "_cnms", cnm(*=>*));

  out.WrNL;
  (* адреса глобальных переменных - указателей *)
  out.WrFmt ("static void * %s[] = {", of);
  out.WrNL;
  ListOffs (m.type.prof);
  ListOffs (m.type.mem);
  out.WrSpc (1);
  out.WrFmt ("%s", nms.x2c[nms.nm_ofs_end]^);
  out.WrNL;
  out.WrFmt ("};");
  out.WrNL;

  (* адреса глобальных процедур - команд *)
  out.WrFmt ("static %s %s[] = { ", nms.x2c[nms.nm_proc]^, cmd);
  ListCmds  (m.type.prof, TRUE);
  out.WrFmt ("0 };");
  out.WrNL;

  (* имена глобальных процедур - команд *)
  out.WrFmt ("static X2C_CHAR * %s[] = { ", cnm);
  ListCmds (m.type.prof, FALSE);
  out.WrFmt ("0 };");
  out.WrNL;

  out.WrFmt ("struct %s %s = {\n  0, 0, ", nms.x2c[nms.nm_md_str]^, md_desc);
  OutStr (dcl.CurMod.name^);
  out.WrFmt (",%s,%s,%s,", of, cmd, cnm);
  IF types = NIL THEN
    out.WrStr ("0");
  ELSE
    dcl.OSecondName (types, 1, "_desc", ls(*=>*));
    out.WrFmt ("&%s", ls);
  END;
  out.WrNL;
  out.WrFmt ("};");
  out.WrNL;
END ModuleDescDefinition;

-----------------------------------------------------------------------------

PROCEDURE ModuleBodyDefinition ( main: BOOLEAN );
(*
*)
VAR
  nm,ini	: STR;
  dd,rt,gc_auto : BOOLEAN;
  heap_lim,stk_lim,gc_thres: LONGINT;
  str		: env.String;
  u		: pc.USAGE;
BEGIN
  IF dcl.gen_def THEN RETURN END;
  IF (dcl.CurMod.type.flag = pc.flag_o2)
  OR (md_desc # "")
  OR ChkO2Vars (dcl.CurMod.type.prof)
  OR ChkO2Vars (dcl.CurMod.type.mem)
  THEN
    ModuleDescDefinition (dcl.CurMod, last_td);
  END;
  ASSERT( dcl.tmp_vars = NIL );
  dcl.cur_proc := NIL;
  IF dcl.CurMod.type.flag IN cc.C_LIKE_FLAGS THEN RETURN END;
  out.WrNL;
  IF ~main THEN
    IF dcl.CurMod.type.flag = pc.flag_sl1 THEN
      ASSERT( dcl.CurMod.val.r = NIL );
      RETURN;
    END;
    Sl1GloMark;
    dcl.ModuleBodyDef (dcl.CurMod, TRUE);
    out.WrNL;
    out.WrChr ("{");
    out.WrNL;
    out.GetSegment (dcl.loc_segm);

    (* initialization check *)
    IF ~cc.op_noinit THEN
      dcl.OSecondName (dcl.CurMod, 0, "_init", ini(*=>*));
      out.WrSpc (1);
      out.WrFmt ("static int %s = 0;", ini);
      out.WrNL;
      dcl.OutLineNo (dcl.CurMod.val.pos, 1);
      out.WrSpc (1);
      out.WrFmt ("if (%s) return;", ini);
      out.WrNL;
      dcl.OutLineNo (dcl.CurMod.val.pos, 1);
      out.WrSpc (1);
      out.WrFmt ("%s = 1;", ini);
      out.WrNL;
    END;
  ELSE
    env.config.Equation ("STACKLIMIT", str(*=>*));

    IF (str = NIL)
    OR ~xcStr.StrToInt (str^, stk_lim(*=>*))
    THEN stk_lim := -1
    END;
    env.config.Equation ("HEAPLIMIT", str(*=>*));

    IF (str = NIL)
    OR ~xcStr.StrToInt (str^, heap_lim(*=>*))
    THEN heap_lim := 0
    END;
    env.config.Equation ("GCTHRESHOLD", str(*=>*));

    IF (str = NIL)
    OR ~xcStr.StrToInt (str^, gc_thres(*=>*))
    THEN gc_thres := 0
    END;
    gc_auto := env.config.Option ("GCAUTO");

    IF stk_lim >= 0 THEN
      out.WrFmt ("%s(%dl)", nms.x2c[nms.nm_stack_limit]^, stk_lim);
      out.WrNL;
    END;

    IF cc.op_krc THEN
      out.WrFmt ("int main(argc, argv)");
      out.WrNL;
      out.WrFmt ("  int argc;");
      out.WrNL;
      out.WrFmt ("  char **argv;");
    ELSE
      out.WrFmt ("extern int main(int argc, char **argv)");
    END;
    out.WrNL;
    out.WrChr ('{');
    out.WrNL;
    out.GetSegment (dcl.loc_segm);

    IF ~cc.op_noinit THEN
      dcl.OutLineNo (dcl.CurMod.val.pos, 1);
      out.WrSpc (1);
      out.WrFmt ("%s(&argc,argv,%d,%dl,%dl);",
                nms.x2c[nms.nm_rts_body]^, gc_auto, gc_thres, heap_lim);
      out.WrNL;
    END;
    ret_emul := TRUE;
  END;
  IF cc.op_debug THEN
    dcl.OutLineNo (dcl.CurMod.val.pos, 1);
    out.WrSpc (1);
    out.WrFmt ("%s();", nms.x2c[nms.nm_proc_inp]^);
    out.WrNL;
  END;

  IF ~cc.op_noinit THEN
    GenSizeChecks (dcl.CurMod.type.prof);
    GenSizeChecks (dcl.CurMod.type.mem);
    InitRecordsBases;
    (* import initialization *)
    u := dcl.CurMod.type.use;
    WHILE u # NIL DO
      IF (u.obj.mode = pc.ob_module) &
         ~(u.obj.type.flag IN cc.C_LIKE_FLAGS + pc.LangSet{pc.flag_sl1}) THEN
        dcl.ModuleBodyName (u.obj, nm(*=>*));
        dcl.OutLineNo (dcl.CurMod.val.pos, 1);
        out.WrSpc (1);
        out.WrFmt ("%s();", nm);
        out.WrNL;
      END;
      u := u.next;
    END;
  END;

  IF md_desc # "" THEN
    (* module descriptor *)
    out.WrSpc (1);
    out.WrFmt ("%s(&%s);", nms.x2c[nms.nm_module]^, md_desc);
    out.WrNL;
  END;

(*global var address is generated as constant initializer
  IniVarAddr (dcl.CurMod.type.prof);
  IniVarAddr(dcl.CurMod.type.mem); *)

  IF cc.op_comments THEN
    cmt.OutFirstComment (dcl.CurMod.val.pos, 0, 0);
  END;
  (* statements *)
  GenSequence (dcl.CurMod.val.r, 1, dd(*=>*), rt(*=>*), TRUE, FALSE, NIL);
  out.AppendSegm (dcl.loc_segm(*=>*));
  out.WriteSegm (dcl.loc_segm);

  IF cc.op_comments THEN
    cmt.OutNextComment (dcl.CurMod.end, 0)
  END;
  IF cc.op_debug THEN
    dcl.OutLineNo (dcl.CurMod.end, 1);
    out.WrSpc (1);
    out.WrFmt ("%s();", nms.x2c[nms.nm_proc_out]^);
    out.WrNL;
  END;
  IF main THEN
    dcl.OutLineNo (dcl.CurMod.end, 1);
    IF ~cc.op_noinit THEN
      out.WrSpc (1);
      out.WrFmt ("%s();", nms.x2c[nms.nm_exit]^);
      out.WrNL;
    END;
    out.WrSpc (1);
    out.WrFmt ("return 0;");
    out.WrNL;
  END;
  out.WrChr ("}");
  out.WrNL;
  IF cc.op_comments THEN
    cmt.OutLastComment (dcl.CurMod.end, 0);
  END;

  dcl.tmp_vars := NIL;
  <* IF ASSERT THEN *>
    IF dcl.tmp_busy # NIL THEN
      env.info.print ("tmp_busy -> %s\n", dcl.tmp_busy.inf.name^);
    END;
  <* END *>
  ASSERT( dcl.tmp_busy = NIL );
END ModuleBodyDefinition;


-----------------------------------------------------------------------------

PROCEDURE GlobObjectDefinition ( o: pc.OBJECT );
(*
*)
VAR s: seg.SEGMENT; (*is_local: BOOLEAN;*)

BEGIN
  IF cc.otag_pub_defined IN o.tags THEN
    IF dcl.gen_def THEN RETURN END;
    IF cc.otag_pri_defined IN o.tags THEN RETURN END;
  END;
  IF pc.OTAG_SET{cc.otag_defining, cc.otag_declaring}*o.tags # pc.OTAG_SET{} THEN
    env.errors.Error (o.pos, 1002);
    ASSERT( o.mode = pc.ob_type );
    RETURN;
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (+1, "(GlobObjectDefinition: '%s'", o.name^) END;
<* END *>

  IF o.type.obj = o THEN
    seg.EnterSegment (o.type.ext(nms.TREE).seg);
  ELSE
    s := seg.NewSegment (o.pos, 1);
    seg.EnterSegment (s);
  END;
  INCL (o.tags, cc.otag_defining);
  CASE o.mode OF
  | pc.ob_var
  , pc.ob_cons  : VarDefinition (o);
  | pc.ob_proc
  , pc.ob_xproc
  , pc.ob_lproc : FuncDefinition (o);
  | pc.ob_cproc : CodeFuncDefinition (o);
  | pc.ob_type  : ObjTypeDefinition (o);
  | pc.ob_module: ModuleBodyDefinition (gen_main);
  | pc.ob_eproc : (* nothing *)
  END;
  EXCL (o.tags, cc.otag_defining);
  INCL (o.tags, cc.otag_pub_defined);
  IF ~ dcl.gen_def THEN INCL (o.tags, cc.otag_pri_defined) END;
    seg.ExitSegment;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GlobObjectDefinition") END;
<* END *>
END GlobObjectDefinition;

-----------------------------------------------------------------------------

TYPE
  GENDEF = RECORD (pc.RIDER) END;
  INIOBJ = RECORD (pc.RIDER) END;

-----------------------------------------------------------------------------

PROCEDURE NoSegment ( o: pc.OBJECT )  :  BOOLEAN;
(*
*)
BEGIN
  RETURN (o.mode IN pc.OB_SET{pc.ob_field, pc.ob_field_bts, pc.ob_label, pc.ob_header})
      OR (o.mode IN pc.VARs) & (o.lev > 0)
      OR ((o.host # NIL) & (o.host.mode = pc.ty_enum))
      OR (pc.otag_with IN o.tags)
      OR (o.mode = pc.ob_cons) & (o.flag = pc.flag_sl1) &
         (*~cc.op_constprop &*) pc.code.en_preopt;
END NoSegment;

-----------------------------------------------------------------------------

PROCEDURE InHeader ( o: pc.OBJECT )  :  BOOLEAN;
(*
   object declaration is in header file
*)
BEGIN
  RETURN (o.mode # pc.ob_lproc) & o.is_public();
END InHeader;

-----------------------------------------------------------------------------

PROCEDURE ( VAR r: INIOBJ ) object ( o: pc.OBJECT );
VAR
  i: nms.INFO;
  n: nms.TREE;
BEGIN
  IF NoSegment(o) THEN RETURN END;
  i := dcl.OInfo (o, -1, "");
  IF dcl.gen_def # InHeader (o) THEN
    i.seg := NIL;
  ELSE
    IF dcl.gen_def & (o.mode = pc.ob_module)
     THEN i.seg := seg.NewSegment (o.end, 0);
     ELSE i.seg := seg.NewSegment (o.pos, 0);
    END;
  END;
  IF (o.type.obj = o) THEN
    IF o.type.ext = NIL THEN
      nms.NewTree (n(*=>*)); o.type.ext := n;
    ELSE
      n := o.type.ext (nms.TREE);
    END;
    IF (o.mode IN (pc.PROCs+pc.OB_SET{pc.ob_module})) & (o.val # NIL)
     THEN n.seg := seg.NewSegment (o.val.pos, 1);
     ELSE n.seg := seg.NewSegment (o.pos, 1);
    END;
  END;
END object;

-----------------------------------------------------------------------------

PROCEDURE ( VAR r: GENDEF ) object ( o: pc.OBJECT );
BEGIN
  IF NoSegment(o) THEN RETURN END;
  IF dcl.gen_def THEN
    IF ~ InHeader (o) THEN RETURN END;
    dcl.ObjectDeclaration (o);
  END;
  GlobObjectDefinition (o);
END object;

-----------------------------------------------------------------------------

PROCEDURE GenDefinitions* ( main: BOOLEAN );
(*
*)
VAR
  d: GENDEF;
  i: INIOBJ;
BEGIN
  dcl.gen_def := FALSE;
  gen_main    := main;
  md_desc     := "";
  out.GetSegment (dcl.glo_segm);
  dcl.CurMod.type.objects (i);
  i.object (dcl.CurMod);
  dcl.CurMod.type.objects (d);
  d.object (dcl.CurMod);
  IF md_desc # "" THEN
    out.WrStr ("extern ");
    IF cc.op_gendll THEN
      out.WrFmt ("%s ", nms.x2c[nms.nm_dll_tag]^)
    END;
    out.WrFmt ("struct %s %s;", nms.x2c[nms.nm_md_str]^, md_desc);
    out.WrNL;
  END;
  out.WriteSegm (dcl.glo_segm);
  IF cc.op_comments THEN
    cmt.OutFullLineComments ((*dcl.CurMod*))
  END;

  seg.WriteSegments (cc.op_lineno);
END GenDefinitions;

-----------------------------------------------------------------------------

PROCEDURE GenPublics*;
(*
*)
VAR
  d: GENDEF;
  i: INIOBJ;
BEGIN
  dcl.gen_def := TRUE;
  md_desc     := "";
  out.GetSegment (dcl.glo_segm);
  dcl.CurMod.type.objects (i);
  i.object (dcl.CurMod);
  dcl.CurMod.type.objects (d);
  d.object (dcl.CurMod);
  out.WriteSegm (dcl.glo_segm);
  IF cc.op_comments (*MP004: & (dcl.CurMod.type.flag = pc.flag_m2)*) THEN
    cmt.OutFullLineComments ((*dcl.CurMod*));
  END;

  seg.WriteSegments (FALSE);
  ASSERT( md_desc = "" );
END GenPublics;

-----------------------------------------------------------------------------

PROCEDURE exi*;
(*
*)
BEGIN
  last_td := NIL;
  eq_glomark := NIL;
  reclist := NIL;
END exi;

-----------------------------------------------------------------------------

PROCEDURE ini*;
(*
*)
BEGIN
  exi;
  env.config.Equation ("SL1GLOBALMARK", eq_glomark(*=>*));
END ini;

BEGIN
  dcl.GlobObjectDef := GlobObjectDefinition;
END ccDef.
