(** Copyright (c) 1991,98 XDS Ltd, Russia. All Rights Reserved. *)
MODULE ccDcl;
<* IF ~ DEFINED(NODE_VALUE_REPRESENTATION) THEN *> <* NEW NODE_VALUE_REPRESENTATION- *>  <* END *>

(* Modifications:
   08/Jul/98 Vit  5.07  @4: enum usage by supertype is removed (see comment)
                        @5: warning for constant reexport
   25/Jun/98 Vit  5.06  @3: avoid naming pre-defined types
   20/Apr/98 Vit  5.04  @2: NODE-VALUE representation change
   05/Apr/98 Vit  5.00  Comments inserted; some code/naming clarification done.
                        Tracing inserted in DB_TRACE mode
   (XDS versions below, BE - above)
   08/Feb/96 Ned  2.12  op_gendll - X2C_DLL_TAG is generated instead of
                        X2C_IMPORT/EXPORT_FUNC/DATA.
   15/Mar/96 Ned  2.12  X2C_PROCLASS is generated for M2/O2 procedures only.
                        For ["Pascal"], ["StdCall"], ["SysCall"] the
                        compiler always generates X2C_PASCAL, X2C_STDCALL,
                        X2C_SYSCALL respectively, regardless of the value
                        of GENPROCLASS option.
                        Procedures affected: ModuleBodyDef,
                          TypeDesignatorNotype, Declarator, TypeConstructor
   17/Mar/96 Ned  2.12  almost all BNRP specific is removed.
   17/Mar/96 Ned  2.12  Comments copying is improved.
   27/Mar/96 Ned  2.14  PRO0046: GenConstAggr str: sb_len is used to get LEN.
   15-Apr-96 Ned        constants of proctype with value NIL are invented.
                        Fix in GenConstAggr.
*)

IMPORT
         Strings,
<* IF DB_TRACE THEN *>
  dbg := pcVisIR,
<* END *>
   pc := pcK,
   cc := ccK,
  nms := ccNames,
  cmt := ccComments,
  env := xiEnv,
  xfs := xiFiles,
  out := ccOut,
  seg := ccSeg,
  ccr := ccRec;

(*============================================================================
  Additional objects are created based on given program objects:
 ob_proc:
        0 _ret    - variable to return function's value
        1 _type   - type (for methods)
 ob_module:
	0 _init
        1 _BEGIN  - procedure to keep module body
        2 _desc   - module descriptor
	3 _offs
	4 _cmds
	5 _cnms
 ob_field:
        0..n      - path for variant fields
 ob_type:
        1 _desc   - type descriptor
        2 _offs   - list of offsets
        3 _proc   - list of methods
 ob_varpar:
	0 _type
=============================================================================*)

TYPE
  STR = out.STR;

  PRM_BUF* = ARRAY 256 OF RECORD
    mode*: SHORTINT;  	(* вид параметра: pm_*, см. CONST *)
    ref *: SET;    	(* мода генерации *)
    obj *: pc.OBJECT;
  END;

  TMP_VAR* = POINTER TO TMP_VAR_REC;
  TMP_VAR_REC = RECORD
    type: pc.STRUCT;
    len : LONGINT;      (* -1 normal, -2 pointer, else array *)
    inf-: nms.INFO;
    val : pc.VALUE;
    dcl : seg.SEGMENT;
    next: TMP_VAR;
  END;

CONST
  (* expression generation mode *)
  REF*  = 0;	(* генерить указатель вместо значения 			*)
  ANY*  = 1;    (* генерить указатель (или значение) произвольного С-типа *)
  LVL* 	= 2;    (* генерить l-value (или ссылку на него если REF) 	*)
  BSA*  = 3;    (* генерить правильный тип либо указатель на базовый тип *)
  CCH*  = 4;    (* строковые литералы генерить сишными                  *)
  NEG*  = 5;
  CNS*  = 6;    (* выражение-инициализация: допустимы агрегаты          *)
  CHK*  = 7;    (* constant expression                                  *)
  CRF   = 8;    (* reference                                            *)
  PLS*  = 9;    (* addition or multiplication context                   *)

  (* function argument mode *)
  pm_return    *= 0;
  pm_threat    *= 1;
  pm_thr_len   *= 2;
  pm_thr_type  *= 3;
  pm_param     *= 4;
  pm_len       *= 5;
  pm_seq       *= 7; (* "..." argument in C function *)
  pm_frec      *= 8;
  pm_type      *= 9;

VAR
  zz_tmp       *: pc.VALUE;
  rr_tmp       *: pc.VALUE;
<* IF ~NODE_VALUE_REPRESENTATION THEN *>
  type_ZZ    	: pc.STRUCT;
  type_RR    	: pc.STRUCT;
<* END *>
  CmpValue_tmp  : pc.VALUE;
  max_longint   : pc.VALUE;
  zz_zero       : pc.VALUE;


  SIZE_T       *: pc.STRUCT;
  LSET_T       *: pc.STRUCT;
  SEQU_T       *: pc.STRUCT;
  LSET_BITS    *: INTEGER;

  glo_segm     *: out.SEGM;
  loc_segm     *: out.SEGM;

  cur_proc     *: pc.STRUCT;
  cns_vars      : TMP_VAR;   -- used in ConstAggregate
  tmp_vars     *: TMP_VAR;
  tmp_busy     *: TMP_VAR;
  gen_def      *: BOOLEAN;  (* TRUE если идет генерация DEFINITION *)

  CurMod       *: pc.OBJECT; (* current module *)
  mod_usage    *: POINTER TO ARRAY OF BOOLEAN; (* must be included *)
  sl1_usage    *: BOOLEAN;  (* slRTS must be included *)

  GenValue     *: PROCEDURE (n: pc.NODE; p: INTEGER; md: SET);
  GlobObjectDef*: PROCEDURE (o: pc.OBJECT);

-----------------------------------------------------------------------------

PROCEDURE ^ ObjectDeclaration ( o: pc.OBJECT );

-----------------------------------------------------------------------------

PROCEDURE CmpValue* (  op: pc.SUB_MODE
		    ; x,y: pc.VALUE
		    )    : 		BOOLEAN;
(*
*)
BEGIN
(* move to initialization part!!!!!!!!!!!!!! *)
  IF CmpValue_tmp = NIL THEN
    CmpValue_tmp := pc.value.new (env.null_pos, pc.ZZ_type);
  END;
  CmpValue_tmp.binary (op, x, y);
  RETURN ~CmpValue_tmp.is_zero();
END CmpValue;

-----------------------------------------------------------------------------

PROCEDURE OInfo* (       o: pc.OBJECT
		 ;       n: INTEGER
		 ; suffix-: ARRAY OF CHAR
		 )	  : 		nms.INFO;
(*
*)
VAR i: nms.INFO;
BEGIN
  IF o.ext # NIL THEN
    i := o.ext (nms.INFO);
    REPEAT
      IF i.no = n+1 THEN RETURN i END;
      i := i.nxt;
    UNTIL i = NIL;
  END;
  ASSERT(   (o.mode = pc.ob_module)
         OR (o.mno = CurMod.mno)
         OR (o.flag IN cc.C_LIKE_FLAGS) & (n < 0)
         OR (o.host.mode = pc.ty_proctype)
         OR (o.host.mode = pc.ty_enum)     -- to pass consts from new symfiles
        );
  IF ((o.mode = pc.ob_module) OR (o.flag IN cc.C_LIKE_FLAGS)) & (n < 0)
   THEN i := nms.MakeInfo (o,  -1, suffix);
   ELSE i := nms.MakeInfo (o, n+1, suffix);
  END;
  RETURN i;
END OInfo;

-----------------------------------------------------------------------------

PROCEDURE OSecondName* (       o: pc.OBJECT
		       ;       n: INTEGER
		       ; suffix-: ARRAY OF CHAR
		       ;  VAR nm: ARRAY OF CHAR);
(*
*)
VAR i: nms.INFO;
BEGIN
  IF o.mno < pc.ZEROMno THEN
    IF n >= 0
     THEN out.StrFmt (nm, "X2C_%s%s%d", o.name^, suffix, n);
     ELSE out.StrFmt (nm, "X2C_%s%s", o.name^, suffix);
    END;
  ELSE
    i := OInfo (o, n, suffix);
    COPY (i.name^, nm);
  END;
END OSecondName;

-----------------------------------------------------------------------------

PROCEDURE OName* (      o: pc.OBJECT
		 ; VAR nm: ARRAY OF CHAR);
(*
*)
BEGIN
  OSecondName (o, -1, "", nm(*=>*));
END OName;

-----------------------------------------------------------------------------

PROCEDURE ObjectUsage* (      o: pc.OBJECT
		       ; VAR nm: ARRAY OF CHAR); -- => object name
(*
   Gets C object name, generates object/host declaration if not yet.
   Name may be a pointer to object (IsPtr(o) = TRUE),
   or reference to object (IsReference(o) = TRUE)
*)
VAR i: nms.INFO;
    n: nms.TREE;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (0, "ObjectUsage: '%s'", o.name^) END;
<* END *>

  (* Cases that do not require object declaration, return name only: *)
  IF (pc.otag_with IN o.tags)     -- WITH artificial parameter
  OR (o.mode = pc.ob_module)      -- module name
  OR (o.mno < pc.ZEROMno)         -- SYSTEM object
  THEN
    OName (o, nm(*=>*));
    RETURN;
  END;

  mod_usage[o.mno] := TRUE;       -- (mark other object usage)

  IF (o.mno # CurMod.mno)         -- imported objects
  OR (o.mode = pc.ob_label)       -- topspeed labes 
  OR (o.mode IN pc.VARs)          -- local variables from outer contexts
   & (o.lev > 0) & (o.lev <= cur_proc.obj.lev)
  THEN
    OName (o, nm(*=>*));
    RETURN;
  END;

  IF (o.mode = pc.ob_proc)        -- proc that has been already declared
   & (seg.CurSegment # NIL)       -- (same cant be done for ty_record!)
   & seg.CurSegment.pos.gtr (o.val.pos)
  OR (o.mode = pc.ob_cons)        -- or enum const
   & (o.host.mode = pc.ty_enum)
  THEN
    OName (o, nm(*=>*));
    n := o.type.ext (nms.TREE);
    seg.ImportSegment (n.seg);
    RETURN;
  END;

  (* Cases that require some object declaration: *)
  IF cc.otag_declared IN o.tags THEN  -- already declared
    -- nothing
  ELSIF o.host.mode = pc.ty_enum THEN -- enum element: declare host type
    ObjectDeclaration (o.host.obj);
    ASSERT( cc.otag_declared IN o.tags );
  ELSIF (o.host.mode = pc.ty_record)  -- record field (not method):
      & ~(o.mode IN pc.PROCs)         --   declare host record
  THEN
    GlobObjectDef (o.host.obj);
    ASSERT( cc.otag_declared IN o.tags );
  ELSE
    ObjectDeclaration (o);
  END;

  i := OInfo (o, -1, "");                -- import declaration segment and get name
  seg.ImportSegment (i.seg);
  COPY (i.name^, nm);
END ObjectUsage;

-----------------------------------------------------------------------------

PROCEDURE ModuleBodyName* (      m: pc.OBJECT
			  ; VAR nm: ARRAY OF CHAR);
(*
*)
VAR
  bf: STR;
  i : INTEGER;
BEGIN
  ASSERT( m.mode = pc.ob_module );
  OSecondName (m, 1, "_BEGIN", bf(*=>*));
  IF ~(cc.otag_versionkey IN m.tags) THEN
    COPY (bf, nm); RETURN;
  END;
  i := 0;
  IF cc.op_index16   THEN i := 1     END;
  IF cc.op_address16 THEN INC (i, 2) END;
  IF cc.op_int16     THEN INC (i, 4) END;
  out.StrFmt (nm, "%s_%c%.8X", bf, CHR (ORD ('A')+i), m.type.len);
END ModuleBodyName;

-----------------------------------------------------------------------------

PROCEDURE ModuleBodyDef* (   m: pc.OBJECT
			; def: BOOLEAN);
(*
*)
VAR nm: STR;
BEGIN
  IF ~cc.op_krc OR ~def THEN
    out.WrStr ("extern ")
  END;
  IF cc.op_gendll & ~def THEN out.WrFmt ("%s ", nms.x2c[nms.nm_dll_tag]^) END;
  out.WrStr ("void ");
  IF cc.op_proclass THEN
    out.WrFmt ("%s ", nms.x2c[nms.GetProcClass (m.type.flag)]^);
  END;
  ModuleBodyName (m, nm(*=>*));
  out.WrStr (nm);
  out.WrChr ('(');
  IF ~cc.op_krc THEN out.WrStr ("void") END;
  out.WrChr (')');
END ModuleBodyDef;

-----------------------------------------------------------------------------

PROCEDURE IsChar* ( t: pc.STRUCT
		  )  : 		BOOLEAN;
(*
*)

BEGIN
  IF t.mode = pc.ty_range THEN t := t.base END;
  RETURN t.mode = pc.ty_char;
END IsChar;

-----------------------------------------------------------------------------

PROCEDURE IsLong* ( t: pc.STRUCT
		  )  : 		BOOLEAN;
(*
   Is type size > 2 bytes?
   (Then l must be generated in spite of 'target16',
   else EXCL (& = ~) depends on size of int in C
*)
BEGIN
  RETURN
    ~(pc.ttag_c_type IN t.tags) &
    (pc.code.get_size (pc.su_bytes, t) > 2);
END IsLong;

-----------------------------------------------------------------------------

PROCEDURE Suffix* ( t: pc.STRUCT );
(*
*)
BEGIN
  IF ~cc.op_intsuffix THEN RETURN END;
  IF ~cc.op_krc THEN
    IF (t.mode IN pc.VPTRs+pc.TY_SET{pc.ty_set}) OR ~t.signed() THEN
      out.WrChr ('u');
    END;
  END;
  IF IsLong(t) THEN out.WrChr ('l') END;
END Suffix;

-----------------------------------------------------------------------------

PROCEDURE IsCarray* ( t: pc.STRUCT
		    )  : 	BOOLEAN;
(*
   Passed by reference in function params
*)
BEGIN
  RETURN (t.mode IN pc.ARRs)
    OR (t.mode = pc.ty_set) & (t.len > 32);
END IsCarray;

-----------------------------------------------------------------------------

PROCEDURE IsShortSet* ( t: pc.STRUCT ) : BOOLEAN;
(*
   Is type represented by C integer?
*)
BEGIN
  RETURN (t.mode = pc.ty_set) & (t.len <= 32)
END IsShortSet;

-----------------------------------------------------------------------------

PROCEDURE IsLongSet* ( t: pc.STRUCT ) : BOOLEAN;
(*
   Is type NOT represented by C integer?
*)
BEGIN
  RETURN (t.mode = pc.ty_set) & (t.len > 32)
END IsLongSet;

-----------------------------------------------------------------------------

PROCEDURE IsCnumber* ( t: pc.STRUCT ) : BOOLEAN;
(*
   Is type a C number?
*)
CONST
  CNUMs = pc.NUMs + pc.TY_SET{ pc.ty_boolean,
                               pc.ty_char,
                               pc.ty_range,
                               pc.ty_enum,
                               pc.ty_loc,
                               pc.ty_protection };
BEGIN
  RETURN (t.mode IN CNUMs)
      OR IsShortSet (t);
END IsCnumber;

-----------------------------------------------------------------------------

PROCEDURE IsCnumberAdr* ( t: pc.STRUCT ) : BOOLEAN;
(*
   Is type a C address?
*)
BEGIN
  RETURN (t.mode IN pc.PADRs)
      OR IsCnumber (t);
END IsCnumberAdr;

-----------------------------------------------------------------------------

PROCEDURE EquCtypes* ( x,y: pc.STRUCT
		     ; fst: BOOLEAN
		     )    : 	BOOLEAN;
(*
*)
VAR px,py: pc.OBJECT;
BEGIN
  WHILE x.use # NIL DO x := x.use.obj.type END;
  WHILE y.use # NIL DO y := y.use.obj.type END;

  IF x.mode = pc.ty_range THEN x := x.base         END;
  IF y.mode = pc.ty_range THEN y := y.base         END;
  IF x.is_ordinal()       THEN x := x.super_type() END;
  IF y.is_ordinal()       THEN y := y.super_type() END;
  IF x.obj # NIL          THEN x := x.obj.type     END;
  IF y.obj # NIL          THEN y := y.obj.type     END;
  IF x = y                THEN RETURN TRUE         END;
  CASE x.mode OF
  | pc.ty_pointer:
      RETURN  (y.mode = pc.ty_pointer)
             & EquCtypes (x.base, y.base, FALSE);
  | pc.ty_array:
      RETURN    fst
             & (y.mode = pc.ty_pointer)
             &  EquCtypes (x.base, y.base, FALSE)
             OR(y.mode = pc.ty_array)
             & (x.len = y.len)
             &  EquCtypes (x.base, y.base, FALSE);
  | pc.ty_shortint  , pc.ty_integer
  , pc.ty_longint   , pc.ty_shortcard
  , pc.ty_cardinal  , pc.ty_longcard
  , pc.ty_real      , pc.ty_longreal
  , pc.ty_ld_real   , pc.ty_complex
  , pc.ty_lcomplex  , pc.ty_void:
      RETURN x.mode = y.mode;
  | pc.ty_proctype:
      IF x.flag # y.flag THEN RETURN FALSE END;
      IF ~EquCtypes (x.base, y.base, TRUE) THEN RETURN FALSE END;
      px := x.prof;
      py := y.prof;
      WHILE (px # NIL) & (py # NIL) DO
        IF cc.otag_notype IN px.tags THEN RETURN FALSE END;
        IF cc.otag_notype IN py.tags THEN RETURN FALSE END;
        IF ~EquCtypes (px.type, py.type, TRUE) THEN RETURN FALSE END;
        px := px.next;
        py := py.next;
      END;
      RETURN (px = NIL) & (py = NIL);
  ELSE
    RETURN FALSE;
  END;
END EquCtypes;

-----------------------------------------------------------------------------

PROCEDURE OutLineNo* ( pos: env.TPOS
		     ; lev: INTEGER);
(*
*)
VAR
  fnm: env.String;
  i,j: LONGINT;
BEGIN
  IF ~cc.op_lineno THEN RETURN END;
  IF gen_def       THEN RETURN END;
  IF pos.IsNull()  THEN RETURN END;
  pos.unpack (fnm, i, j);
  IF ~cc.op_krc    THEN out.WrSpc (lev) END;
  out.WrStr ("#line ");
  out.WrInt (i+1);
  out.WrNL;
END OutLineNo;


(*============================ TYPES GENERATION ============================*)

PROCEDURE TypeDefinition* ( t: pc.STRUCT );
(*
  Generate (actually, record only) type definition
*)
BEGIN
  IF (t.obj = NIL) OR (t.obj.mode # pc.ob_type) THEN RETURN END;
  t := t.obj.type;                           -- required for SL1
  IF t.mno  # CurMod.mno   THEN RETURN END;  -- skip external types
  IF t.mode # pc.ty_record THEN RETURN END;  -- do only for records

  IF ~(cc.otag_pub_defined IN t.obj.tags) THEN
    GlobObjectDef (t.obj);                   -- calls def.ObjTypeDefinition
  END;
  IF t.ext # NIL THEN
    seg.ImportSegment (t.ext (nms.TREE).seg);
  END;
END TypeDefinition;

-----------------------------------------------------------------------------

PROCEDURE StdType* (      t: pc.STRUCT
                   ; VAR nm: ARRAY OF CHAR --> std type name
                   )       :     BOOLEAN;  --> T: name created
(*
   Generates name for standard type (having C denotation or defined in X2C.h)
*)
VAR n: INTEGER;

BEGIN
  IF t.mode = pc.ty_range THEN               -- may have own name!
    IF (t.obj # NIL) & (t.obj.mno >= 0) THEN RETURN FALSE END;
    t := t.base;
  END;

  IF (pc.ttag_c_type IN t.tags) & (t.obj # NIL) THEN -- SYSTEM type has C name
    COPY (t.obj.name^, nm);
    RETURN TRUE;
  END;

  CASE t.mode OF                             -- accept basic simple types
  | pc.ty_shortcard: n := nms.nm_shortcard;  -- (get names from .nms-file)
  | pc.ty_cardinal : n := nms.nm_cardinal;
  | pc.ty_longcard : n := nms.nm_longcard;
  | pc.ty_shortint : n := nms.nm_shortint;
  | pc.ty_integer  : n := nms.nm_integer;
  | pc.ty_longint  : n := nms.nm_longint;
  | pc.ty_ZZ       : n := nms.nm_longint;
  | pc.ty_real     : n := nms.nm_real;
  | pc.ty_longreal : n := nms.nm_longreal;
  | pc.ty_ld_real  : n := nms.nm_ld_real;
  | pc.ty_RR       : n := nms.nm_longreal;
  | pc.ty_complex  : n := nms.nm_complex;
  | pc.ty_lcomplex : n := nms.nm_lcomplex;
  | pc.ty_CC       : n := nms.nm_lcomplex;
  | pc.ty_char     : n := nms.nm_char;
  | pc.ty_AA       : n := nms.nm_AA;
  | pc.ty_loc      : n := nms.nm_loc;
  | pc.ty_protection:n := nms.nm_protection;
  | pc.ty_void     : n := nms.nm_void;
  | pc.ty_boolean:
     IF t.base.mode = pc.ty_shortcard
      THEN n := nms.nm_boolean;
      ELSE RETURN StdType (t.base, nm(*=>*));
     END;
  | pc.ty_set:
     IF IsLongSet (t) THEN RETURN FALSE END;  -- abandon long sets (C arrays)
     IF t.inx # NIL THEN RETURN StdType (t.inx, nm(*=>*)) END; -- inx = SETSIZE
     CASE pc.code.get_size (pc.su_bytes, t) OF
     | 1: n := nms.nm_set8;
     | 2: n := nms.nm_set16;
     | 4: n := nms.nm_set32;
     END;
  | pc.ty_process:
      out.StrFmt (nm, "struct %s", nms.x2c[nms.nm_xhandler_str]^);
      RETURN TRUE;
  ELSE
    IF (t.obj # NIL) & (t.obj.mno < 0) THEN
      OName (t.obj, nm(*=>*)); -- accept aggregate named system types
      RETURN TRUE;
    END;
    RETURN FALSE;
  END;
  COPY (nms.x2c[n]^, nm);
  RETURN TRUE;
END StdType;

-----------------------------------------------------------------------------

PROCEDURE TypeUsage* ( VAR nm: ARRAY OF CHAR    --> type name for usage pos
                     ;      t: pc.STRUCT        -- # NIL
	             ;    def: BOOLEAN          -- T: gen type definition
	             ;    ren: BOOLEAN          -- F: substitute enum for base type
                     )       :         BOOLEAN; --> F: type has no name
(*
   Returns TRUE:
     - for pre-defined or named type, its designator (name) => 'nm'
     - type declaration generated if required and not yet.
   Returns FALSE:
     - if type is user-defined and unnamed (no designator generated)
*)
VAR buf: STR;

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (0, "TypeUsage: [%X] => ", t) END;
<* END *>

  ASSERT( t # NIL );

  IF ((t.obj = NIL) OR (t.obj.mno < 0))
   & StdType (t, nm(*=>*)) THEN        -- get name for SYSTEM (x2c.h) type
  <* IF DB_TRACE THEN *>
    IF out.EnDebug THEN dbg.CallTrace (nm) END;
  <* END *>
    RETURN TRUE
  END;

  IF (t.obj = NIL)                   -- otherwise must be named
  OR (t.obj.mode # pc.ob_type) THEN
  <* IF DB_TRACE THEN *>
    IF out.EnDebug THEN dbg.CallTrace ("<no name>") END;
  <* END *>
    RETURN FALSE
  END;

  IF def THEN TypeDefinition (t) END;        -- gen type definition

(* @4:
   This was intended to bypass the problem that sizeof(enum X) is not defined.
   But known C/C++ compilers support this. Uncomment, if fail...
   Uncommented...
*)
  IF ~ren & ~cc.op_cpp & (t.mode = pc.ty_enum) THEN
    mod_usage[t.mno] := TRUE; -- !!
    RETURN TypeUsage (nm(*=>*), t.base, def, FALSE);
  END;

  ObjectUsage (t.obj, buf(*=>*));         -- use type object

  IF t.mode = pc.ty_record THEN
    ccr.Record (t);                       -- generate record structure
  END;

  (* add enum/struct/union prefixes: *)
  IF (cc.ttag_typedef IN t.tags)          -- C GENTYPEDEF+: no prefix
  OR cc.op_cpp THEN                       -- C++: no prefix
    COPY (buf, nm);
  ELSIF t.mode = pc.ty_enum THEN
    out.StrFmt (nm, "enum %s", buf);
  ELSIF (t.mode = pc.ty_record) OR
        (t.mode = pc.ty_array_of)         -- array_of translates to struct
  THEN
    IF cc.ttag_union IN t.tags
     THEN out.StrFmt (nm, "union %s", buf);
     ELSE out.StrFmt (nm, "struct %s", buf);
    END;
  ELSE
    COPY (buf, nm);
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTrace (nm) END;
<* END *>
  RETURN TRUE;
END TypeUsage;

-----------------------------------------------------------------------------

PROCEDURE ^ TypeConstructor (VAR res: ARRAY OF CHAR
			    ; ident-: ARRAY OF CHAR
			    ;      t: pc.STRUCT
			    ;    par: BOOLEAN
			    )       :            BOOLEAN;

----------------------------------------------------------

PROCEDURE TypeDesig* ( VAR res: ARRAY OF CHAR --> designator
		     ;  ident-: ARRAY OF CHAR -- ident to put in proper pos
		     ;       t: pc.STRUCT     -- type
		     ;     def: BOOLEAN       -- same as in TypeUsage
		     ;     par: BOOLEAN );    -- same as in TypeConstructor
(*
   Generates type designator with 'ident' (possibly empty):
   calls TypeUsage, if failed - TypeConstructor, if failed - error.
*)
VAR nm: STR;

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(TypeDesig: t = [%X] ident = '%s'", t, ident) END;
<* END *>

  IF (~par OR (t.mode # pc.ty_array_of))
   & TypeUsage (nm(*=>*), t, def, FALSE)
  THEN
    IF ident = ""
     THEN COPY (nm, res);
     ELSE out.StrFmt (res, "%s %s", nm, ident);
    END;
  ELSIF TypeConstructor (res(*=>*), ident, t, par) THEN
    -- nothing
  ELSE
    env.errors.Error (t.pos, 1016);
    res[0] := '';
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")TypeDesig '%s'", res) END;
<* END *>
END TypeDesig;

-----------------------------------------------------------------------------

PROCEDURE TypeDesignator* ( VAR nm: ARRAY OF CHAR
			  ; ident-: ARRAY OF CHAR
			  ;      t: pc.STRUCT );
(*
   Shortcut for TypeDesig: definition on, use as param off
*)
BEGIN
  TypeDesig (nm(*=>*), ident, t, TRUE, FALSE);
END TypeDesignator;

-----------------------------------------------------------------------------

PROCEDURE TypeDesignatorNotype* ( VAR res: ARRAY OF CHAR
			        ;  ident-: ARRAY OF CHAR
				; 	t: pc.STRUCT );
(*
  Generates designator of 'ident' with wrong (under-defined) type
*)
BEGIN
  CASE t.mode OF
  | pc.ty_proctype:
      IF t.flag IN cc.DFLT_PROCLASS THEN  -- proc with no parms and result
        COPY (nms.x2c[nms.nm_proc]^, res);
        IF ident # "" THEN
          nms.StrCat (res, ' ');
          nms.StrCat (res, ident);
        END;
      ELSIF t.flag IN cc.SPEC_PROCLASS THEN
        out.StrFmt (res, "void (%s *%s)()", nms.x2c[nms.GetProcClass (t.flag)]^, ident);
      ELSE
        out.StrFmt (res, "void (*%s)()", ident);
      END;
  | pc.ty_pointer:
      out.StrFmt (res, "void *%s", ident);   -- pointer to anything
  END;
END TypeDesignatorNotype;


(*=========================== TEMPORARY VARIABLES ===========================*)

PROCEDURE NewTempVar (      t: pc.STRUCT
                     ;    len: LONGINT
                     ; VAR nm: ARRAY OF CHAR ); --> name
(*
   Create a temp.var of type 't' and 'len'
*)
VAR v: TMP_VAR;
BEGIN
  NEW (v);
  v.type := t;
  v.len  := len;
  v.inf  := nms.MakeName (nm(*=>*) , cur_proc, NIL,
                          pc.ob_var, "tmp", 0);
  v.next := tmp_busy;
  tmp_busy := v;
END NewTempVar;

-----------------------------------------------------------------------------

PROCEDURE SearchTempVar(      t: pc.STRUCT
		       ;    len: LONGINT
		       ; VAR nm: ARRAY OF CHAR
		       )       : 	BOOLEAN;
(*
*)
VAR p,v: TMP_VAR;
BEGIN
  v := tmp_vars;
  p := NIL;
  IF len < 0 THEN
    WHILE (v # NIL) & ((v.type # t)
       OR (v.len # len))
    DO
      p := v;
      v := v.next
    END;
  ELSE
    WHILE (v # NIL) & ((v.type # t)
       OR (v.len < len))
    DO
      p := v;
      v := v.next
    END;
  END;
  IF v = NIL THEN RETURN FALSE END;
  IF p = NIL
   THEN tmp_vars := v.next
   ELSE p.next := v.next
  END;
  v.next   := tmp_busy;
  tmp_busy := v;
  COPY (v.inf.name^, nm);
  RETURN TRUE;
END SearchTempVar;

-----------------------------------------------------------------------------

PROCEDURE EnterStatement* ( VAR x: TMP_VAR );
(*
*)
BEGIN
  x := tmp_busy;
  tmp_busy := NIL;
  out.DropIndent;
END EnterStatement;

-----------------------------------------------------------------------------

PROCEDURE ExitStatement* ( x: TMP_VAR );
(*
*)
VAR v: TMP_VAR;
BEGIN
  IF tmp_busy # NIL THEN
    v := tmp_busy;
    WHILE v.next # NIL DO
      v := v.next
    END;
    v.next   := tmp_vars;
    tmp_vars := tmp_busy;
  END;
  tmp_busy := x;
END ExitStatement;

-----------------------------------------------------------------------------

PROCEDURE MakeTempVar* (      t: pc.STRUCT
                       ; VAR nm: ARRAY OF CHAR );
(*
   Create a temporary variable of type 't' in current C-scope
*)
VAR buf: STR;
BEGIN
  IF SearchTempVar (t, -1, nm(*=>*)) THEN RETURN END;
  NewTempVar (t, -1, nm(*=>*));
  out.Push;
  TypeDesignator (buf(*=>*), nm, t);
  out.WrSpc (1);
  out.WrStr (buf);
  out.WrChr (";");
  out.WrNL;
  out.AppendSegm (loc_segm);
  out.Pop;
END MakeTempVar;

-----------------------------------------------------------------------------

PROCEDURE MakeTempArr* (      t: pc.STRUCT
                       ;    len: LONGINT
		       ; VAR nm: ARRAY OF CHAR);
(*
*)
VAR buf,res: STR;
BEGIN
  ASSERT( len >= 0 );
  IF SearchTempVar (t, len, nm(*=>*)) THEN RETURN END;
  NewTempVar (t, len, nm(*=>*));
  out.Push;
  out.StrFmt (buf, "%s[%d]", nm, len);
  out.WrSpc (1);
  IF t = SEQU_T THEN
    out.WrFmt ("%s %s;", nms.x2c[nms.nm_seq_type]^, buf);
  ELSIF t = SIZE_T THEN
    out.WrFmt ("%s %s;", nms.x2c[nms.nm_index]^, buf);
  ELSE
    TypeDesignator (res(*=>*), buf, t);
    out.WrFmt ("%s;", res);
  END;
  out.WrNL;
  out.AppendSegm (loc_segm(*=>*));
  out.Pop;
END MakeTempArr;

-----------------------------------------------------------------------------

PROCEDURE MakeTempPtr* (      t: pc.STRUCT
		       ; VAR nm: ARRAY OF CHAR);
(*
*)
VAR buf,str: STR;
BEGIN
  IF SearchTempVar (t, -2, nm(*=>*)) THEN RETURN END;

  NewTempVar (t, -2, nm(*=>*));

  out.Push;
  out.StrFmt (str, "*%s", nm);
  TypeDesignator (buf(*=>*), str, t);
  out.WrSpc (1);
  out.WrStr (buf);
  out.WrChr (";");
  out.WrNL;
  out.AppendSegm (loc_segm(*=>*));
  out.Pop;
END MakeTempPtr;

(*-------------------------------- Literals ---------------------------------*)

PROCEDURE GenCnumber* ( n: LONGINT
		      ; t: pc.STRUCT);
(*
*)
BEGIN
  out.WrFmt ("%d", n);
  IF cc.op_intsuffix THEN
    t := t.super_type();
    IF ~cc.op_krc & ~t.signed() THEN out.WrChr ('u') END;
    IF IsLong (t)               THEN out.WrChr ('l') END;
  END
END GenCnumber;

-----------------------------------------------------------------------------

PROCEDURE GenFieldPath* ( inf: nms.INFO );
(*
*)
BEGIN
  IF inf = NIL THEN RETURN END;

  GenFieldPath (inf.nxt);
  out.WrStr (inf.name^); out.WrChr ('.');
END GenFieldPath;

-----------------------------------------------------------------------------

PROCEDURE SearchRecordVariant* ( VAR l: pc.NODE
			       ;     v: pc.VALUE);
(*
*)
VAR e,m: pc.NODE;

BEGIN
  e := NIL;
  LOOP
    IF l = NIL THEN EXIT END;
    ASSERT( l.mode = pc.nd_node );
    m := l.l;
    IF m = NIL THEN e := l END;
    LOOP
      IF m = NIL THEN EXIT END;
      IF m.mode = pc.nd_pair THEN
        IF CmpValue (pc.sb_geq, v, m.val) &
           CmpValue (pc.sb_leq, v, m.l.val) THEN
          EXIT
        END;
      ELSE
        ASSERT( m.val # NIL );
        IF CmpValue (pc.sb_equ, v, m.val) THEN EXIT END;
      END;
      m := m.next;
    END;
    IF m # NIL THEN EXIT END;
    l := l.next;
  END;
  IF l = NIL THEN l := e END;
END SearchRecordVariant;

-----------------------------------------------------------------------------

PROCEDURE GenSizeof* ( t: pc.STRUCT );
(*
   Generate "sizeof(type)" for not array_of
*)
VAR nm: STR;

BEGIN
  ASSERT( t.mode # pc.ty_array_of );

  TypeDesignator (nm(*=>*), "", t);
  out.WrFmt ("sizeof(%s)", nm);
END GenSizeof;

-----------------------------------------------------------------------------

PROCEDURE GenConstAggr* (       v: pc.VALUE
                        ;       t: pc.STRUCT
                        ;       p: INTEGER
                        ; def,cch: BOOLEAN );
(*
   def - aggregate in var definition, i.e. it can be { ... }
   cch - C string literal, do not double '\' char
*)
VAR tmp: STR;

  -----------------------------------------------
  PROCEDURE TmpIn (): BOOLEAN;
  (*
     Returns TRUE, if a constant is already declared; its name is written
     into output stream, so nothing more needs to be generated
  *)
  VAR
    buf: STR;
    c  : TMP_VAR;
  BEGIN
    tmp := "";
    IF def THEN RETURN FALSE END;
    c := cns_vars;
    LOOP
      IF c = NIL THEN EXIT END;
      IF c.type = t THEN
        IF CmpValue (pc.sb_equ, c.val, v) THEN
	  out.WrStr (c.inf.name^);
          seg.ImportSegment (c.dcl);
          RETURN TRUE;
	END;
      END;
      c := c.next;
    END;
    NEW (c);
    c.next := cns_vars;
    cns_vars := c;
    c.type := t;
    c.val := v;
    c.inf := nms.MakeName (tmp(*=>*), NIL, NIL, pc.ob_var, "_cnst", 0);
    c.dcl := seg.NewSegment (v.pos, 0);
    seg.ImportSegment (c.dcl);
    seg.EnterSegment (c.dcl);
    TypeDesignator (buf(*=>*), tmp, t);
    out.WrStr("static ");
    IF cc.op_cpp & (IsCnumber (t)) THEN out.WrStr ("const ") END;   --$$$ PR 200
    out.WrStr (buf);
    out.WrStr (" = ");
    RETURN FALSE;
  END TmpIn;

  -----------------------------------------------

  PROCEDURE TmpOut;
  (*
  *)
  BEGIN
    IF def THEN RETURN END;
    out.WrChr (";");
    out.WrNL;
    seg.ExitSegment;
    out.WrStr (tmp);
  END TmpOut;

  -----------------------------------------------

  PROCEDURE Enum;
  (*
  *)
  VAR l : pc.OBJECT;
      nm: STR;
  BEGIN
    l := t.obj.type.prof;
    WHILE l # NIL DO
      IF CmpValue (pc.sb_equ, l.val.val, v) THEN
        ObjectUsage (l, nm(*=>*));
        out.WrStr (nm);
        RETURN;
      END;
      l := l.next;
    END;
    v.value_to_str (nm, pc.flag_c);
    out.WrStr (nm);
  END Enum;

  -----------------------------------------------

  PROCEDURE Num;
  (*
  *)
  VAR
    str: STR;
    s  : pc.STRUCT;
  BEGIN
    IF t.mode # pc.ty_ZZ THEN
      s := t.super_type();
      ASSERT( s.mode IN pc.WHOLEs );
      IF (s.mode = pc.ty_longint) THEN               --$$$ PR266
<* IF NODE_VALUE_REPRESENTATION THEN *>
        IF CmpValue (pc.sb_equ, v, s.min.val) THEN
<* ELSE *>
        IF CmpValue (pc.sb_equ, v, s.min) THEN
<* END *>
          out.WrStr (nms.x2c[nms.nm_min_longint]^);
          RETURN
        END;
<* IF NODE_VALUE_REPRESENTATION THEN *>
        IF CmpValue (pc.sb_equ, v, s.max.val) THEN
<* ELSE *>
        IF CmpValue (pc.sb_equ, v, s.max) THEN
<* END *>
          out.WrStr (nms.x2c[nms.nm_max_longint]^);
          RETURN
        END;
      ELSIF s.mode = pc.ty_longcard THEN
<* IF NODE_VALUE_REPRESENTATION THEN *>
        IF CmpValue (pc.sb_equ, v, s.max.val) THEN
<* ELSE *>
        IF CmpValue (pc.sb_equ, v, s.max) THEN
<* END *>
          out.WrStr (nms.x2c[nms.nm_max_longcard]^);
          RETURN
        END;
      END;
    END;
    IF (p >= 11) & v.is_neg() THEN
      out.WrChr ('(')
    END;
    v.value_to_str (str, pc.flag_c);
    out.WrStr (str);
    Suffix (t);
    IF (p >= 11) & v.is_neg() THEN
      out.WrChr (')')
    END;
  END Num;

  -----------------------------------------------

  PROCEDURE Complex;
  (*
  *)
  VAR v1,v2: STR;

  BEGIN
    IF TmpIn() THEN RETURN END;
    rr_tmp.unary (pc.su_re, v);
    rr_tmp.value_to_str (v1, pc.flag_c);
    rr_tmp.unary (pc.su_im, v);
    rr_tmp.value_to_str (v2, pc.flag_c);
    out.WrFmt ("{%s,%s}", v1, v2);
    TmpOut;
  END Complex;

  -----------------------------------------------

  PROCEDURE Arr;
  (*
  *)
  VAR
    i : LONGINT;
    v1: pc.VALUE;
  BEGIN
    ASSERT( t.mode # pc.ty_array_of );
    ASSERT( t.base # NIL );
    IF TmpIn() THEN RETURN END;
    out.WrStr ("{ ");
    FOR i := 0 TO t.len-1 DO
      IF i # 0 THEN out.WrStr (", ") END;
      (* We must create new object for each array element,
	 it is used in global list of constants
      *)
      v1 := pc.value.new (v.pos, t.base);
      v1.index_get (i, v);
        GenConstAggr (v1, t.base, 0, TRUE, FALSE);
    END;
    out.WrStr (" }");
    TmpOut;
  END Arr;

  -----------------------------------------------

  PROCEDURE IsSimpleRecord (): BOOLEAN;
  (*
     RETURN TRUE if record can be generated as C constant aggregate
  *)
  VAR f: pc.OBJECT;

  BEGIN
    IF t.base # NIL THEN RETURN FALSE END;
    f := t.prof;
    WHILE f # NIL DO
      IF f.mode = pc.ob_header THEN RETURN FALSE END;
      f := f.next;
    END;
    RETURN TRUE;
  END IsSimpleRecord;

  -----------------------------------------------

  PROCEDURE RecordAgg;
  (*
  *)

  VAR
    nm: STR;
    n : LONGINT;
  ---------------------------

    PROCEDURE GenFld ( f: pc.OBJECT );
    (*
    *)
    VAR
      fld: STR;
      w  : pc.VALUE;
    BEGIN
      ASSERT( f.mode # pc.ob_header );
      w := pc.value.new (v.pos, f.type);
      w.index_get (n, v);
      IF IsCarray (f.type) THEN
	out.WrFmt ("%s(%s.", nms.x2c[nms.nm_memcpy]^, nm);
        ObjectUsage (f, fld(*=>*));
        GenFieldPath (f.ext(nms.INFO).nxt);
        out.WrFmt ("%s,", fld);
        GenConstAggr (w, f.type, 0, FALSE, FALSE);
        out.WrChr (',');
        GenSizeof (f.type);
        out.WrChr (')');
      ELSE
        out.WrFmt ("%s.", nm);
        ObjectUsage (f, fld(*=>*));
	GenFieldPath (f.ext (nms.INFO).nxt);
        out.WrFmt ("%s = ", fld);
        GenConstAggr (w, f.type, 1, FALSE, FALSE);
      END;
      out.WrStr (", ");
      INC (n);
    END GenFld;

    -------------------------

    PROCEDURE GenFldSeq ( f: pc.OBJECT );
    (*
    *)
    VAR
      l: pc.NODE;
      w: pc.VALUE;
    BEGIN
      WHILE f # NIL DO
	IF f.mode = pc.ob_header THEN
	  ASSERT( f.val.mode = pc.nd_case );
	  l := f.val.l;
	  w := pc.value.new (v.pos, f.type);
	  w.index_get (n, v);
	  SearchRecordVariant (l(*=>*), w);
	  ASSERT( l # NIL );
	  IF f.val.obj # NIL
           THEN GenFld (f.val.obj)
           ELSE INC (n)
          END;
	  GenFldSeq (l.obj);
	ELSE
	  GenFld (f);
	END;
	f := f.next;
      END;
    END GenFldSeq;

    -------------------------

    PROCEDURE GenLevel ( t: pc.STRUCT );
    (*
    *)
    BEGIN
      IF t.base # NIL THEN
        GenLevel (t.base)
      END;
      GenFldSeq (t.prof);
    END GenLevel;

    -------------------------

  BEGIN
    ASSERT( t.mode = pc.ty_record );
    MakeTempVar (t, nm(*=>*));
    out.WrChr ('(');
    n := 0;
    GenLevel (t);
    out.WrFmt ("%s)", nm);
  END RecordAgg;

  -----------------------------------------------

  PROCEDURE Record;
  (*
  *)
  VAR
    i: LONGINT;
    f: pc.OBJECT;
    w: pc.VALUE;
  BEGIN
    IF ~IsSimpleRecord() THEN
      RecordAgg;
      RETURN
    END;
    IF TmpIn() THEN RETURN END;
    out.WrChr ("{");
    f := t.prof;
    IF f = NIL THEN
      out.WrChr ('0');
    ELSE
      i := 0;
      REPEAT
	IF i # 0 THEN out.WrChr (",") END;
	ASSERT( f.mode # pc.ob_header );
	w := pc.value.new (v.pos, f.type);
	w.index_get (i, v);
        GenConstAggr (w, f.type, 0, TRUE, FALSE);
	INC (i);
        f := f.next;
      UNTIL f = NIL;
    END;
    out.WrChr ("}");
    TmpOut;
  END Record;

  -----------------------------------------------

  PROCEDURE LSet;
  (*
  *)
  VAR
    i,j: LONGINT;
    k  : SET;
    fst: BOOLEAN;
  BEGIN
    IF TmpIn() THEN RETURN END;
    out.WrChr ("{");
    j := 0; k := {}; fst := TRUE;
    FOR i := 0 TO t.len-1 DO
      zz_tmp.index_get (i, v);
      IF ~zz_tmp.is_zero() THEN INCL (k, j) END;
      INC (j);
      IF (j = LSET_BITS) OR (i = t.len-1) THEN
	IF fst
         THEN fst := FALSE
         ELSE out.WrChr (",")
        END;
	out.WrFmt ("0x%.*X", LSET_BITS DIV 4, k);
	Suffix (LSET_T);
	k := {}; j := 0;
      END;
    END;
    out.WrChr ("}");
    TmpOut;
  END LSet;

  -----------------------------------------------
  PROCEDURE SSet;
  (*
    Generate short set constructor
  *)
  VAR i: LONGINT;
      fst: BOOLEAN;
  BEGIN
    fst := TRUE;
    FOR i := 0 TO t.len-1 DO
      zz_tmp.index_get(i,v);
      IF ~zz_tmp.is_zero() THEN
        IF fst
         THEN out.WrChr('('); fst := FALSE;
         ELSE out.WrStr(' | ');
        END;
        zz_tmp.set_integer(i);
        out.WrStr("1<<");
        GenConstAggr(zz_tmp,t.base,0,FALSE,FALSE);
      END;
    END;
    IF fst
     THEN out.WrChr("0");
     ELSE out.WrChr(")");
    END;
  END SSet;

  -----------------------------------------------

  PROCEDURE Char (      ch: CHAR
                 ; str,cch: BOOLEAN);
  (*
  *)
    PROCEDURE Wr ( c: CHAR );
    (*
    *)
    BEGIN
      IF str
       THEN out.WrChr   (c);
       ELSE out.WrChrNB (c);
      END;
    END Wr;
    -------------------------
  VAR
    buf: ARRAY 5 OF CHAR;
    i  : INTEGER;
  BEGIN
    CASE ch OF
    | '"': IF  str THEN out.WrChrNB ('\') END; Wr ('"');
    | "'": IF ~str THEN out.WrChrNB ('\') END; Wr ("'");
(* On Macintosh \r and \n designate somthing else
    | 0DX: out.WrChrNB ('\'); Wr ("r");
    | 0AX: out.WrChrNB ('\'); Wr ("n");
*)
    | '\': out.WrChrNB ('\'); IF ~cch THEN Wr ('\') END;
    | 00X: out.WrChrNB ('\'); Wr ("0");
    ELSE
      IF (ch >= ' ') & (ch <= '~') THEN
	Wr (ch);
      ELSE
        out.StrFmt (buf, "\\%.3o", ch);
	ASSERT( LENGTH (buf) = 4 );
	FOR i := 0 TO 2 DO
          out.WrChrNB (buf[i])
        END;
	Wr (buf[3]);
      END;
    END;
  END Char;

  ---------------------------------------------------------------------------

  PROCEDURE Str;
  (*
  *)
  VAR
    i,len: LONGINT;
    ch   : CHAR;
    b    : BOOLEAN;
  BEGIN
    zz_tmp.binary (pc.sb_len, v, zz_zero);
    len := zz_tmp.get_integer();
    ASSERT( len <= t.len + 1 ); (* note: value may contain 0X in [t.len] *)
    zz_tmp.index_get (len-1, v);
    IF ~zz_tmp.is_zero() THEN
      ASSERT( len = t.len );
      Arr;
      RETURN
    END;

    IF cc.op_unicode THEN out.WrStr ('_T('); END;
    out.DisableWrap (b(*=>*));
    out.WrChr ('"');
    FOR i := 0 TO len-2 DO
      zz_tmp.index_get (i, v);
      ch := CHR (zz_tmp.get_integer());
      Char (ch, TRUE, cch);
    END;
    out.WrChr ('"');
    out.RestoreWrap (b);
    IF cc.op_unicode THEN out.WrChr (')'); END;
  END Str;
  -----------------------------------------------

VAR
  buf,be: STR;
       o: pc.OBJECT;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN
    dbg.CallTraceInd (0, "GenConstAggr: p=%d v = [%X].t = [%X]", p, v, t);
  END;
<* END *>

  IF t.mode = pc.ty_range THEN t := t.base END;
  CASE t.mode OF

  | pc.ty_char
  , pc.ty_loc:
    IF v.is_zero() THEN
      out.WrChr ('0')
    ELSE
      IF cc.op_unicode THEN out.WrStr ('_T('); END;
      out.WrChrNB ("'");
      Char (CHR (v.get_integer()), FALSE, FALSE);
      out.WrChr ("'");
      IF cc.op_unicode THEN out.WrChr (')'); END;
    END;

  | pc.ty_enum:
      Enum;

  | pc.ty_AA:
      out.WrChr ('0');

  | pc.ty_pointer
  , pc.ty_opaque
  , pc.ty_protection:
      v.value_to_str (buf, pc.flag_c);
      out.WrStr (buf);
      Suffix (t);

  | pc.ty_boolean:
      IF v.is_zero()
       THEN out.WrChr ('0')
       ELSE out.WrChr ('1')
      END;

  | pc.ty_array
  , pc.ty_SS:
      IF IsChar (t.base)
       THEN Str();       --$$$ PR200
       ELSE Arr();
      END;

  | pc.ty_real:
      v.value_to_str (buf, pc.flag_c);
      IF v.is_neg()
       THEN out.WrFmt ("(%sf)", buf)
       ELSE out.WrFmt ("%sf", buf)
      END;

  | pc.ty_longreal
  , pc.ty_ld_real
  , pc.ty_RR:
      v.value_to_str (buf, pc.flag_c);
      IF v.is_neg()
       THEN out.WrFmt ("(%s)", buf)
       ELSE out.WrStr (buf)
      END;

  | pc.ty_complex
  , pc.ty_lcomplex
  , pc.ty_CC:
      Complex;

  | pc.ty_set:
      IF IsLongSet (t)
       THEN LSet;
       ELSE SSet;
      END;

  | pc.ty_record:
      Record;

  | pc.ty_proctype:
      o := v.get_object();
      IF o = NIL THEN
        out.WrChr ('0');     --!! NULL!!
      ELSE
        ObjectUsage (o, buf(*=>*));
        IF EquCtypes (t, o.type, TRUE) THEN
          out.WrStr (buf);
        ELSE
          IF p > 13 THEN out.WrChr ('(') END;
          TypeDesignator (be (*=>*), "", t);
          out.WrChr ("("); out.WrStr (be); out.WrChr (')'); out.WrStr (buf);
          IF p > 13 THEN out.WrChr (')') END;
        END;
      END;
  ELSE
    IF t.mode IN pc.WHOLEs THEN
      Num;
    ELSE
      env.errors.Error (v.pos, 1013);
    END;
  END;
END GenConstAggr;

-----------------------------------------------------------------------------

PROCEDURE ConstHexNum* ( v: pc.VALUE
		       ; t: pc.STRUCT
		       ; p: INTEGER);
(*
*)
VAR i,j: LONGINT;

BEGIN
  zz_tmp.unary (pc.su_abs, v);
  IF CmpValue (pc.sb_gtr, zz_tmp, max_longint) THEN
    GenConstAggr (v, t, p, FALSE, FALSE);
  ELSE
    i := v.get_integer();
    IF (p >= 11) & (i < 0) THEN out.WrChr ('(') END;
    IF i < 0 THEN
      j := ABS (i);
      out.WrChr ('-')
    ELSE
      j := i
    END;
    out.WrFmt ("0x%X", j);
    Suffix (t);
    out.WrChr (' ');
    IF (p >= 11) & (i < 0) THEN out.WrChr (')') END;
  END
END ConstHexNum;

(*-------------------------------------------------------------------------*)

-----------------------------------------------------------------------------

PROCEDURE GetBytes* ( ps: pc.TPOS
		    ;  t: pc.STRUCT
		    )   : 	LONGINT;
(*
*)
VAR
  i : LONGINT;
  nm: STR;
BEGIN
  i := pc.code.get_size (pc.su_bytes, t);
  IF i < 0 THEN
    TypeDesig (nm(*=>*), "", t, FALSE, FALSE);
    env.errors.Error (ps, 1014, nm);
    i := 1;
  END;
  RETURN i;
END GetBytes;

-----------------------------------------------------------------------------

PROCEDURE GetBaseType* ( t: pc.STRUCT ) : pc.STRUCT;
  (* для типов, которые в С являются массивами, выдает базовай тип,
     т.е. тип, с указателем на который даннй тип совместим *)
BEGIN
  ASSERT( IsCarray (t) );
  IF t.mode = pc.ty_array_of THEN
    REPEAT
      t := t.base
    UNTIL t.mode # pc.ty_array_of;
    RETURN t;
  ELSIF t.mode IN pc.ARRs THEN
    (* !!!!!! а как насчет многомерных массивов ? *)
    RETURN t.base;
  ELSE
    ASSERT( t.mode = pc.ty_set );
    RETURN LSET_T;
  END;
END GetBaseType;

-----------------------------------------------------------------------------

PROCEDURE IsReference* (   o: pc.OBJECT
                       ; scp: pc.STRUCT
		       )    : 		BOOLEAN;
(*
   RETURN TRUE if name in C is reference to object of type 'o.type'
*)
VAR u: pc.USAGE;

BEGIN
  IF ~(o.mode IN pc.OB_SET{pc.ob_varpar, pc.ob_var}) THEN RETURN FALSE END;
  IF  cc.otag_reference IN o.tags                    THEN RETURN TRUE  END;
  IF ~cc.op_cpp                                      THEN RETURN FALSE END;
  IF  o.mno # CurMod.mno                             THEN RETURN FALSE END;
  IF  scp = NIL                                      THEN RETURN FALSE END;

  IF (o.mode = pc.ob_varpar) & ~IsCarray (o.type)
   & (o.host.flag IN pc.LangSet{pc.flag_o2, pc.flag_m2, pc.flag_sl1})
   & (o.flag      IN pc.LangSet{pc.flag_o2, pc.flag_m2, pc.flag_sl1})
  OR (o.lev = 0) & (o.attr # NIL)
  THEN
    INCL (o.tags, cc.otag_reference);
    RETURN TRUE;
  END;

  IF (o.lev > 0) & (scp.obj # NIL)
   & (o.lev <= scp.obj.lev) & ~IsCarray (o.type)
  THEN
    IF pc.otag_no_aliases IN o.tags THEN
      u := scp.use;
      WHILE u.obj # o DO
        u := u.next
      END;
      IF ~(pc.utag_write IN u.tags) THEN RETURN FALSE END;
    END;
    (* Do not use 'otag_reference' for scope dependent checks! *)
    RETURN TRUE;
  END;

  RETURN FALSE;
END IsReference;

-----------------------------------------------------------------------------

PROCEDURE IsPtr* (   o: pc.OBJECT
		 ; scp: pc.STRUCT
		 )    : 	BOOLEAN;
(*
   RETURN TRUE if name in C is pointer to object of type 'o.type'
*)
VAR u: pc.USAGE;

BEGIN
  IF cc.op_cpp & IsReference (o, scp) THEN RETURN FALSE END;

  CASE o.mode OF
  | pc.ob_varpar:
      RETURN ~IsCarray (o.type);

  | pc.ob_var:
      IF (o.attr # NIL)
       & ~(pc.otag_valpar IN o.tags)
      OR (o.type.mode = pc.ty_record)
       & (pc.otag_valpar IN o.tags)
       & (o.host.flag = pc.flag_p)
      THEN RETURN TRUE END;

      IF (scp # NIL) & (scp.obj # NIL)
       & (o.lev > 0) & (o.lev <= scp.obj.lev)
       & ~IsCarray (o.type)
      THEN
        IF pc.otag_no_aliases IN o.tags THEN
          u := scp.use;
          WHILE u.obj # o DO
            u := u.next;
          END;
          IF ~(pc.utag_write IN u.tags) THEN RETURN FALSE END;
        END;
        RETURN TRUE;
      END;
      RETURN FALSE;
  ELSE
    RETURN FALSE;
  END;
END IsPtr;

-----------------------------------------------------------------------------

PROCEDURE Declarator* (   o: pc.OBJECT
		      ; ptr: BOOLEAN);
(*
*)
  -------------------------------------------------
  PROCEDURE ResTypeDesig ( VAR res: ARRAY OF CHAR
  			 ;       t: pc.STRUCT
  			 ;     def: BOOLEAN );  -- same as in TypeUsage
  (*
     Generates function result type designator
     ??? Vit: Why not use TypeDesignator ???
  *)
  BEGIN
    IF TypeUsage (res(*=>*), t, def, FALSE) THEN RETURN END;

    CASE t.mode OF
    | pc.ty_pointer:
        ResTypeDesig (res(*=>*), t.base, FALSE);
        nms.StrCat (res, "*");
    | pc.ty_range:
        ResTypeDesig (res(*=>*), t.base, def);
    ELSE
      (* Vit: Why this stuff?? make empty name? *)
      ASSERT( t.obj = NIL );
      t.obj := pc.new_object_ex(t, CurMod.type, pc.ob_type, gen_def);
      ASSERT( TypeUsage (res(*=>*), t, def, FALSE) );
    END;
  END ResTypeDesig;
  -----------------------------------------------

VAR
  nm,buf,res: STR;
           n: INTEGER;
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (0, "Declarator: '%s'", o.name^) END;
<* END *>

  IF o.mode IN pc.PROCs THEN
    IF cc.op_proclass OR (o.type.flag IN cc.SPEC_PROCLASS)
     THEN n := nms.GetProcClass (o.type.flag);
     ELSE n := nms.nm_empty;
    END;
    IF ptr THEN
      OSecondName (o, 1, "_", buf(*=>*));
      out.StrFmt (nm, "(%s* %s)", nms.x2c[n]^, buf);
    ELSE
      OName (o, buf(*=>*));
      out.StrFmt (nm, "%s %s", nms.x2c[n]^, buf);
    END;
    IF IsCarray (o.type.base) THEN
      ResTypeDesig (buf(*=>*), GetBaseType (o.type.base), TRUE);
      out.StrFmt (res, "%s*\n%s", buf, nm);
    ELSE
      ResTypeDesig (buf(*=>*), o.type.base, TRUE);
      out.StrFmt (res, "%s\n%s", buf, nm);
    END;

  ELSE
    OName (o, nm(*=>*));
    IF IsReference (o, o.host) THEN
      out.StrFmt (buf, "&%s", nm);
      COPY (buf, nm);
    ELSIF IsPtr (o, o.host)    THEN
      out.StrFmt (buf, "*%s", nm);
      COPY (buf, nm);
    END;
    TypeDesignator (res(*=>*), nm, o.type);
  END;
  out.WrStr (res);
END Declarator;

-----------------------------------------------------------------------------

PROCEDURE FuncParams* ( VAR prm_buf: PRM_BUF
		      ;	VAR prm_cnt: INTEGER
		      ;           t: pc.STRUCT);
(*
*)
VAR buf_errs: BOOLEAN;

  -----------------------------------------------

  PROCEDURE BufErr;
  (*
  *)
  BEGIN
    IF buf_errs THEN RETURN END;
    env.errors.Error (t.pos, 1015);
    buf_errs := TRUE;
  END BufErr;

  -----------------------------------------------

  PROCEDURE Threat ( o: pc.OBJECT );
  (*
  *)

  VAR
    nm: STR;
     i: LONGINT;
  BEGIN
    IF ~(o.mode IN pc.VARs)     THEN RETURN         END;
    IF prm_cnt >= LEN (prm_buf) THEN BufErr; RETURN END;
    prm_buf[prm_cnt].mode := pm_threat;
    OName (o, nm(*=>*));
    nms.Disable (t, nm);
    prm_buf[prm_cnt].obj := o;
    prm_buf[prm_cnt].ref := {LVL};
    IF    IsReference (o, t) THEN INCL (prm_buf[prm_cnt].ref, CRF);
    ELSIF IsPtr (o, t)       THEN INCL (prm_buf[prm_cnt].ref, REF);
    ELSIF IsCarray (o.type)  THEN INCL (prm_buf[prm_cnt].ref, BSA);
    END;
    (* если array_of то надо передать длины *)
    IF (o.type.mode = pc.ty_array_of)
    THEN
      FOR i := 0 TO o.type.len-1 DO
        INC (prm_cnt);
	IF prm_cnt >= LEN (prm_buf) THEN BufErr; RETURN END;
	prm_buf[prm_cnt].mode := pm_thr_len;
	OSecondName (o, SHORT (i), "_len", nm(*=>*));
        nms.Disable (t, nm);
	prm_buf[prm_cnt].obj := o;
	prm_buf[prm_cnt].ref := {};
      END;
    ELSIF (o.type.flag = pc.flag_o2)
	& (o.type.mode = pc.ty_record)
	& (o.mode = pc.ob_varpar)
    THEN
      INC (prm_cnt);
      IF prm_cnt >= LEN (prm_buf) THEN BufErr; RETURN END;
      prm_buf[prm_cnt].mode := pm_thr_type;
      OSecondName (o, 0, "_type", nm(*=>*)); nms.Disable (t, nm);
      prm_buf[prm_cnt].obj := o;
      prm_buf[prm_cnt].ref := {};
    END;
    INC (prm_cnt);
  END Threat;

  -----------------------------------------------

  PROCEDURE Threatening ( u: pc.USAGE );
  (*
  *)
  BEGIN
    WHILE u # NIL DO
      Threat (u.obj);
      u := u.next
    END;
  END Threatening;
  -----------------------------------------------

VAR
  f: pc.OBJECT;
  i: INTEGER;

BEGIN
  buf_errs := FALSE;
  prm_cnt := 0;
  IF IsCarray (t.base) THEN
    ASSERT( t.base.mode # pc.ty_array_of );
    prm_buf[0].mode := pm_return;
    prm_buf[0].obj  := t.obj;
    prm_buf[0].ref  := {LVL};
    prm_cnt := 1;
  END;
  IF (t.obj # NIL) & (t.obj.mode # pc.ob_type) THEN
    Threatening (t.use)
  END;
  f := t.prof;
  WHILE f # NIL DO
    ASSERT( f.mode IN pc.VARs );
    IF prm_cnt >= LEN (prm_buf) THEN BufErr; RETURN END;
    prm_buf[prm_cnt].mode := pm_param;
    prm_buf[prm_cnt].obj  := f;
    prm_buf[prm_cnt].ref  := {};
    IF    IsReference (f, t) THEN
      INCL (prm_buf[prm_cnt].ref, CRF);
    ELSIF IsPtr (f, t)   THEN
      INCL (prm_buf[prm_cnt].ref, REF);
    ELSIF IsCarray (f.type)  THEN
      INCL (prm_buf[prm_cnt].ref, BSA);
    END;
    IF f.mode = pc.ob_varpar THEN
      INCL (prm_buf[prm_cnt].ref, LVL);
    END;
    IF  (f.type.mode = pc.ty_array_of)
     & ~(t.flag IN cc.C_LIKE_FLAGS)
    THEN
      FOR i := 0 TO SHORT (f.type.len-1) DO
	INC (prm_cnt);
	IF prm_cnt >= LEN (prm_buf) THEN BufErr; RETURN END;
        prm_buf[prm_cnt].mode := pm_len;
	prm_buf[prm_cnt].obj := f;
	prm_buf[prm_cnt].ref := {};
      END;
    ELSIF (f.mode = pc.ob_seq)
        & (t.flag IN pc.LangSet{pc.flag_c, pc.flag_syscall, pc.flag_stdcall})
    THEN
      prm_buf[prm_cnt].mode := pm_seq;
    ELSIF (f.mode = pc.ob_varpar)
	& (f.type.mode = pc.ty_record)
	& (f.type.flag = pc.flag_o2)
    THEN
      prm_buf[prm_cnt].mode := pm_frec;
      INC (prm_cnt);
      IF prm_cnt >= LEN (prm_buf) THEN BufErr; RETURN END;
      prm_buf[prm_cnt].mode := pm_type;
      prm_buf[prm_cnt].obj := f;
      prm_buf[prm_cnt].ref := {};
    ELSIF t.flag IN cc.C_LIKE_FLAGS THEN
      INCL (prm_buf[prm_cnt].ref, CCH);
    END;
    INC (prm_cnt);
    f := f.next;
  END;
END FuncParams;

-----------------------------------------------------------------------------

PROCEDURE FuncProfileDefinition* (       t: pc.STRUCT
				 ; VAR res: ARRAY OF CHAR
				 ; def,pdf: BOOLEAN);
(*
	def: TRUE если в профиле должны быть указаны имена параметров,
	     т.е. за профилем последует тело функции.
	pdf: FALSE собственно профиль
	     TRUE описание параметров (в стиле устаревшего С)
        !!! if def, "res" may contain '\n' - use ccOut.WrFmt(res) for output !!!
*)

VAR
  nm,str : STR;
  buf    : STR;
  i,j    : INTEGER;
  prm_buf: PRM_BUF;
  prm_cnt: INTEGER;

BEGIN
  res[0] := 0X;
  IF ~def &  cc.op_krc THEN RETURN END;
  IF  pdf & ~cc.op_krc THEN RETURN END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (0, "FuncProfileDef: [%X]", t) END;
<* END *>

  FuncParams (prm_buf(*=>*), prm_cnt(*=>*), t);
  IF prm_cnt = 0 THEN
    IF ~cc.op_krc THEN
      nms.StrCat (res, "void")
    END;
    RETURN;
  END;
  j := 0;
  FOR i := 0 TO prm_cnt-1 DO
    IF cc.op_krc & (prm_buf[i].mode = pm_seq) THEN RETURN END;
    IF (i > 0) & ~ pdf THEN
      nms.StrCat (res, ", ");
    END;
    IF cc.op_cpp & (pc.otag_RO IN prm_buf[i].obj.tags) THEN
      nms.StrCat (res, "/*const*/ ");
    END;
    IF    REF IN prm_buf[i].ref THEN str := "* ";
    ELSIF CRF IN prm_buf[i].ref THEN str := "& ";
    ELSE str := ""
    END;
    IF def THEN
      IF def & (prm_buf[i].mode IN {pm_param, pm_frec}) THEN
	INCL (prm_buf[i].obj.tags, cc.otag_declared)
      END;
      IF prm_buf[i].mode = pm_return THEN
	OSecondName (prm_buf[i].obj, 0, "_ret", nm(*=>*));
      ELSIF prm_buf[i].mode IN {pm_len, pm_thr_len} THEN
        OSecondName (prm_buf[i].obj, j, "_len", nm(*=>*));
        INC (j);
      ELSIF prm_buf[i].mode IN {pm_type, pm_thr_type} THEN
	OSecondName (prm_buf[i].obj, 0, "_type", nm(*=>*));
        INC (j);
      ELSE
	OName (prm_buf[i].obj, nm(*=>*)); j := 0;
      END;
      nms.StrCat (str, nm);
    ELSE
      IF str[0] # 0X THEN
        ASSERT( str[1] = ' ' );
        str[1] := 0X;
      END;
      nm := ""; -- assigment to suppress warning: used before definition
    END;
    IF cc.op_krc & ~pdf THEN
      COPY (nm, buf);  -- only parameter name
    ELSIF prm_buf[i].mode = pm_seq THEN
      buf := "...";
    ELSIF prm_buf[i].mode IN {pm_type, pm_thr_type} THEN
      out.StrFmt (buf, "%s %s", nms.x2c[nms.nm_td]^, str);
    ELSIF prm_buf[i].mode IN {pm_len, pm_thr_len} THEN
      TypeDesignator (buf(*=>*), str, prm_buf[i].obj.type.inx);
    ELSIF prm_buf[i].mode = pm_return THEN
      TypeDesig (buf(*=>*), str, prm_buf[i].obj.type.base, FALSE, FALSE);
    ELSIF cc.otag_notype IN prm_buf[i].obj.tags THEN
      TypeDesignatorNotype (buf(*=>*), str, prm_buf[i].obj.type);
    ELSE
      TypeDesig (buf(*=>*), str, prm_buf[i].obj.type, FALSE, TRUE);
    END;
    IF cc.op_krc & pdf THEN
      out.StrFmt (str, " %s;\n", buf);
      nms.StrCat (res, str);
    ELSE
      nms.StrCat (res, buf);
    END;
  END;
END FuncProfileDefinition;

-----------------------------------------------------------------------------

PROCEDURE TypeConstructor ( VAR res: ARRAY OF CHAR
			  ;  ident-: ARRAY OF CHAR
			  ;       t: pc.STRUCT
			  ;     par: BOOLEAN  -- T: desig will be used for param
			  )        : 		BOOLEAN;

(*
   Generates types constructor (not designator), if possible
*)

VAR
  buf, prm : STR;
   l: LONGINT;
  rf: BOOLEAN;
   n: INTEGER;
<* IF NODE_VALUE_REPRESENTATION THEN *>
  prl : STR;
  nmin, nmax: pc.NODE;
<* END *>

  -----------------------------------------------
  PROCEDURE GenValueToStr ( VAR str : STR
                          ;       n : pc.NODE );
  (*
     Generate expression 'n' into 'str'
  *)
  BEGIN
    out.Push;
    GenValue (n, 0, {});
    out.GetString (str(*=>*)); -- pack into one string
    out.Pop;
  END GenValueToStr;

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1,"(TypeConstr: [%X] ident = '%s'", t, ident) END;
<* END *>

  ASSERT( ident[0] # ' ' );
  rf := (ident[0] = '*') OR (ident[0] = '&');

  CASE t.mode OF
  | pc.ty_range:
      TypeDesig (res(*=>*), ident, t.base, TRUE, par);

  | pc.ty_pointer:
      t := t.base;
      buf := "*";
      IF  (t.mode = pc.ty_record)   -- prefix required as rec mb. fwd-declared:
        & (t.obj # NIL)             --  named
        & (t.use = NIL)             --  not renamed
        & (t.obj.mno = CurMod.mno)  --  not from other module (then def.alrdy)
      THEN
        ObjectUsage (t.obj, buf(*=>*));
        IF cc.ttag_union IN t.tags
         THEN out.StrFmt (res, "union %s*",  buf);
         ELSE out.StrFmt (res, "struct %s*", buf);
        END;
        CASE ident[0] OF
        | 0C:
        | '*', '&': out.StrFmt (res, "%s%s",  res, ident);
        ELSE        out.StrFmt (res, "%s %s", res, ident);
        END;
      ELSE
        IF (t.mode = pc.ty_array_of) & (t.flag = pc.flag_c) THEN
          t   := t.base;  -- ptr to open C array is generated as ptr to array base *)
          l   := 1;
          WHILE t.mode = pc.ty_array_of DO
            t := t.base;
            buf[l] := '*';
            INC (l);
          END;
          buf[l] := 0X;
        ELSE
          IF (t.mode = pc.ty_array) & ~cc.op_ptrtoarr THEN
            t := t.base;       -- ptr to array is generated as ptr to array base
          END;
        END;
        CASE ident[0] OF
        | 0C:
        | '*', '&': out.StrFmt (buf, "%s%s",  buf, ident);
        ELSE        out.StrFmt (buf, "%s %s", buf, ident);
        END;
        TypeDesig (res(*=>*), buf, t, FALSE, FALSE);
      END;

  | pc.ty_opaque:
      out.StrFmt (res, "void *%s", ident);

  | pc.ty_set:
      IF IsShortSet (t) THEN RETURN FALSE END;
      l := (t.len-1) DIV LSET_BITS +1;
      CASE LSET_BITS OF
      |  8: n := nms.nm_set8;
      | 16: n := nms.nm_set16;
      | 32: n := nms.nm_set32;
      END;
      IF rf
       THEN out.StrFmt (res, "%s (%s)[%d]", nms.x2c[n]^, ident, l);
       ELSE out.StrFmt (res, "%s %s[%d]",   nms.x2c[n]^, ident, l);
      END;

  | pc.ty_proctype:
      FuncProfileDefinition (t, prm(*=>*), FALSE, FALSE);
      IF cc.op_proclass OR (t.flag IN cc.SPEC_PROCLASS)
       THEN n := nms.GetProcClass (t.flag);
       ELSE n := nms.nm_empty;
      END;
      IF IsCarray (t.base) THEN
        out.StrFmt (buf, "* (%s* %s)(%s)", nms.x2c[n]^, ident, prm);
        TypeDesignator (res(*=>*), buf, GetBaseType (t.base));
      ELSE
        out.StrFmt (buf, "(%s* %s)(%s)", nms.x2c[n]^, ident, prm);
        TypeDesignator (res(*=>*), buf, t.base);
      END;

  | pc.ty_array
  , pc.ty_SS:
<* IF NODE_VALUE_REPRESENTATION THEN *>
      nmin := t.min;        -- all subnodes considered calculated: .val->VALUE
      nmax := t.max;
      IF t.mode = pc.ty_SS THEN
        out.StrFmt (prm, "%d", t.len);
      ELSE
        CASE nmin.val.get_integer() OF
        | 0:
          IF nmax.mode = pc.nd_value THEN
            out.StrFmt (prm, "%d", t.len);
          ELSIF (nmax.mode = pc.nd_binary) & (nmax.sub = pc.sb_minus) &
             (nmax.r.val.get_integer() = 1)
          THEN
            GenValueToStr (prm(*=>*), nmax.l);
          ELSE
            GenValueToStr (buf(*=>*), nmax);
            out.StrFmt (prm,"%s+1",buf);
          END;
        | 1:
           GenValueToStr (prm(*=>*), nmax);
        ELSE
           GenValueToStr (prl(*=>*), nmin);
           GenValueToStr (buf(*=>*), nmax);
           out.StrFmt (prm, "%s-(%s)+1", buf, prl);
        END;
      END;
<* ELSE *>
      out.StrFmt (prm, "%d", t.len);
<* END *>
      IF rf
       THEN out.StrFmt (buf, "(%s)[%s]", ident, prm);
       ELSE out.StrFmt (buf, "%s[%s]", ident, prm);
      END;
      TypeDesignator (res(*=>*), buf, t.base);

  | pc.ty_array_of:
      IF rf
       THEN out.StrFmt (buf, "(%s)[]", ident);
       ELSE out.StrFmt (buf, "%s[]", ident);
      END;
      REPEAT
        t := t.base
      UNTIL t.mode # pc.ty_array_of;
      TypeDesignator (res(*=>*), buf, t);

  ELSE
<* IF DB_TRACE THEN *>
    IF out.EnDebug THEN dbg.CallTraceInd (-1, ")TypeConstr => FALSE") END;
<* END *>
    RETURN FALSE;
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")TypeConstr '%s' => TRUE", res) END;
<* END *>
  RETURN TRUE;
END TypeConstructor;


(*=========================== Declarations ==============================*)

PROCEDURE VarAddrExpression* ( o: pc.OBJECT );
(*
*)
VAR
  buf: STR;
  ref: BOOLEAN;
BEGIN
  ASSERT( o.mode = pc.ob_var );
  ASSERT( o.lev = 0 );
  ASSERT( o.attr # NIL );

  ref := IsReference (o, o.host);
  IF (o.attr.mode = pc.nd_unary)
   & (o.attr.sub = pc.su_adr)
   & EquCtypes (o.type, o.attr.l.type, TRUE)
  THEN
    IF ref THEN
      IF IsCarray (o.type)
       THEN GenValue (o.attr.l, 1, {CNS, CHK, LVL, BSA});
       ELSE GenValue (o.attr.l, 1, {CNS, CHK, LVL});
      END;
    ELSE
      GenValue (o.attr.l, 1, {CNS, CHK, REF});
    END;
  ELSE
    IF ref THEN out.WrChr('*') END;
    out.WrChr ('(');
    TypeDesignator (buf(*=>*), "*", o.type);
    out.WrFmt ("%s)", buf);
    GenValue (o.attr, 13, {CNS, CHK});
  END;
END VarAddrExpression;

-----------------------------------------------------------------------------

PROCEDURE GlobVarDeclaration ( o: pc.OBJECT );
(*
*)
VAR buf: STR;
    cns,b,literal: BOOLEAN;
  -----------------------------------------------
  PROCEDURE CheckReexport (on: pc.NODE): BOOLEAN;
  (*
    Check if expr 'on' refers to object with same name as 'o' being declared
    (Warning reported; as equal C names result, use '#define' instead 'const')
  *)
  BEGIN
    IF on = NIL THEN RETURN FALSE END;
    CASE on.mode OF
    | pc.nd_var:
        IF (on.obj.mode = pc.ob_cons) &
           Strings.Equal (on.obj.name^,o.name^)
        THEN
          env.errors.Warning (o.pos, 355, o.name^);
          RETURN TRUE;
        END;
    | pc.nd_index
    , pc.nd_binary:
        RETURN CheckReexport (on.l)
            OR CheckReexport (on.r);
    | pc.nd_aggregate
    , pc.nd_unary
    , pc.nd_field
    , pc.nd_deref:
        RETURN CheckReexport (on.l);
    | pc.nd_node:
        RETURN CheckReexport (on.l)
            OR CheckReexport (on.next);
    | pc.nd_value
    , pc.nd_proc
    , pc.nd_type:
        RETURN FALSE;
    END;
    RETURN FALSE;
  END CheckReexport;

  -----------------------------------------------
  PROCEDURE IsMacro (): BOOLEAN;
  (*
     Check if a '#define' must be generated
  *)
  BEGIN
    IF o.mode # pc.ob_cons THEN RETURN FALSE END; -- no, if non-CONST

    IF CheckReexport(o.val) THEN RETURN TRUE END; -- yes, if names conflict

    IF (o.type.mode = pc.ty_proctype)             -- proc const: (value assumed)
     & (~cc.op_cpp OR (o.type.flag = pc.flag_c))  -- yes, if C function
    THEN
      RETURN TRUE
    END;

---    RETURN ~cc.op_cppconst;

    IF cc.op_cppconst THEN RETURN FALSE END;  -- no, if numeric #defs suppressed

    IF literal                                -- yes, if literal (type unknown)
    OR (o.val.mode # pc.nd_value) THEN        -- yes, if value is known
      RETURN TRUE
    END;

    (* #definition of numeric consts requires for C, as they may be used
       in 'case' or other "const" positions.
       It's left for C++ also, though not obligatory: turn cppconst+ to avoid *)

    RETURN o.is_public() & o.type.is_ordinal() -- yes, if exported C numeric
        OR (o.type.mode = pc.ty_enum)          -- or local enum, char, short set
        OR (o.type.mode = pc.ty_char)
        OR IsShortSet(o.type)                  -- no, otherwise
  END IsMacro;
  -----------------------------------------------

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(GlobVarDecl: '%s'", o.name^) END;
<* END *>

  INCL (o.tags, cc.otag_pub_defined);    -- mark as public part defined

  IF cc.op_comments THEN
    cmt.OutFirstComment (o.pos, 1, 0)
  END;

  IF (o.mode = pc.ob_cons)             -- literal C const char or ''
   & (o.type.mode = pc.ty_SS)  -- We can not distinguish between CHAR and
   & (o.type.len <= 2)         -- string[1], thus create both
   & (o.val.val # NIL)
   & (o.flag = pc.flag_c)
  THEN
    OName (o, buf(*=>*));
    zz_tmp.index_get (0, o.val.val);
   out.DisableWrap (b(*=>*));
    out.WrFmt ("#define chr_%s ", buf);
    GenConstAggr (zz_tmp, o.type.base, 14, FALSE, FALSE);
   out.RestoreWrap (b);
    out.WrNL;
  END;

  literal := (o.type.mode IN pc.TY_SET{pc.ty_ZZ, pc.ty_RR, pc.ty_AA, pc.ty_SS}); -- type unknown

  IF IsMacro() THEN                  -- If maybe #defined:
   out.DisableWrap (b(*=>*));        -- line breaks by '\' only
    out.WrStr ("#define ");
    OName (o, buf(*=>*));
    out.WrStr (buf);
    out.WrChr (' ');
    IF IsCarray (o.type)
     THEN GenValue (o.val, 14, {BSA});
     ELSE GenValue (o.val, 14, {});
    END;
   out.RestoreWrap (b);
    INCL (o.tags, cc.otag_pri_defined);   -- mark as private part defined

  ELSE                                    -- May not be #defined:
    cns := cc.op_cpp & (o.mode = pc.ob_cons) &
          IsCnumber (o.type);
    IF cns THEN                           -- C++ const object decl
      INCL (o.tags, cc.otag_pri_defined); --     &def
      out.WrStr ("static ");
      out.WrStr ("const ");
    ELSIF o.is_public() THEN                -- exported object decl only
      IF ~cc.op_krc THEN                    -- (def is in VarDefinition)
        out.WrStr ("extern ");
      END;
      IF cc.op_gendll THEN
        out.WrFmt ("%s ", nms.x2c[nms.nm_dll_tag]^);
      END;
    ELSE                                    -- local object decl
      ASSERT( o.mno = CurMod.mno );         --   check definition in CurMod
      INCL (o.tags, cc.otag_pri_defined);   --  & def
      out.WrStr ("static ");
    END;
    IF ~cc.op_krc
     & (o.mode = pc.ob_var)
     & (pc.otag_volatile IN o.tags) THEN
      out.WrStr ("volatile ");
    END;
    Declarator (o, FALSE);           -- generate const declarator
    IF cns OR ~o.is_public() THEN
      IF o.mode = pc.ob_cons THEN
        out.WrStr (" = ");
        IF IsCarray (o.type)
         THEN GenValue (o.val, 1, {CNS, CHK, BSA});
         ELSE GenValue (o.val, 1, {CNS, CHK});
        END;
      ELSIF o.attr # NIL THEN
        out.WrStr (" = ");
        VarAddrExpression (o);
      END;
      INCL (o.tags, cc.otag_pri_defined);
    END;
    out.WrChr (';');
    IF cc.op_comments THEN
      cmt.OutTailComment (o.end);
    END;
  END;
  out.WrNL;
  IF cc.op_comments THEN
    cmt.OutLastComment (o.end, 0);
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")GlobVarDecl") END;
<* END *>
END GlobVarDeclaration;

-----------------------------------------------------------------------------

PROCEDURE TypeDeclaration ( o: pc.OBJECT );
(*
   Preliminary type declaration
*)

  ---------------------------------------------------------

  PROCEDURE EnumDeclaration ( t: pc.STRUCT );
    -------------------------
    PROCEDURE LineNo ( ps-: env.TPOS ) : LONGINT;
    (*
    *)
    VAR
      fnm : env.String;
      l, c: LONGINT;
    BEGIN
      ps.unpack (fnm, l, c);
      RETURN l
    END LineNo;
    -------------------------

  VAR
    f,enums: pc.OBJECT;
    i      : nms.INFO;
    nm     : STR;
    cur    : LONGINT;
  -------------------------------------------------
  BEGIN
    ASSERT( t.obj.mno = CurMod.mno );
    OName (t.obj,nm(*=>*));
    IF cc.op_comments THEN
      cmt.OutFirstComment (t.pos, 1, 0)
    END;
    IF ~ cc.op_constenum THEN
      out.WrFmt ("enum %s {", nm)
    END;
    enums  := t.prof;
    t.prof := NIL; (* clear enum list - to use values in GlobVarDeclaration *)
    f      := enums;
    WHILE f # NIL DO
      ASSERT( f.mode = pc.ob_cons );
      ASSERT( f.host = t );
      ASSERT( f.type = t );
      i     := OInfo (f, -1, "");
      i.seg := seg.CurSegment;
      IF cc.op_constenum THEN
        GlobVarDeclaration (f);
      ELSE
        cur := LineNo (f.pos);
        out.WrStr (i.name^);
        IF f.next # NIL
         THEN out.WrStr (", ")
         ELSE out.WrStr ('};')
        END;
        IF (f.next = NIL) OR (LineNo (f.next.pos) # cur) THEN
          IF cc.op_comments THEN
            cmt.OutTailComment (f.pos);
          END;
          out.WrNL;
          IF f.next # NIL THEN out.WrSpc (1) END;
        END;
      END;
      INCL (f.tags, cc.otag_declared);
      INCL (f.tags, cc.otag_pub_defined);
      INCL (f.tags, cc.otag_pri_defined);
      f := f.next;
    END;
    t.prof := enums; (* restore enum list *)
    IF cc.op_comments THEN
      cmt.OutLastComment (t.end, 0)
    END;
    IF ~cc.op_constenum
     & ~cc.op_cpp
     & (cc.ttag_typedef IN t.tags) THEN  -- GENTYPEDEF+ for C
      out.WrNL;
      out.WrFmt ("typedef enum %s %s;", nm, nm);
      out.WrNL;
    END;
  END EnumDeclaration;

  -----------------------------------------------

  PROCEDURE ArrayofDeclaration ( t: pc.STRUCT );
  (*
  *)
  VAR
    buf,str,nm: STR;
    n : LONGINT;
    bs: pc.STRUCT;
  BEGIN
    ASSERT( t.mode = pc.ty_array_of );
    OName (t.obj, nm(*=>*));
    out.WrNL;
    out.WrFmt ("struct %s {", nm);
    out.WrNL;
    n := t.len;
    bs := t;
    WHILE n > 0 DO
      bs := bs.base;
      DEC (n)
    END;
    out.StrFmt (str, "* %s", nms.x2c[nms.nm_dynarr_addr]^);
    TypeDesig (buf(*=>*), str, bs, FALSE, FALSE);
    out.WrSpc (1);
    out.WrFmt ("%s;", buf);
    out.WrNL;
    FOR n := 0 TO t.len-1 DO
      IF n # 0 THEN
        out.WrSpc (1);
        out.WrFmt ("%s %s%d;", nms.x2c[nms.nm_index]^,
                   nms.x2c[nms.nm_dynarr_size]^, n);
        out.WrNL;
      END;
      out.WrSpc (1);
      out.WrFmt ("%s %s%d;", nms.x2c[nms.nm_index]^,
                 nms.x2c[nms.nm_dynarr_len]^, n);
        out.WrNL;
    END;
    out.WrFmt ("};");
    out.WrNL;
    IF ~cc.op_cpp & (cc.ttag_typedef IN t.tags) THEN  -- GENTYPEDEF+ for C
      out.WrFmt ("typedef struct %s %s;", nm, nm);
      out.WrNL;
    END;
  END ArrayofDeclaration;

  -----------------------------------------------
  PROCEDURE RecursionInParms ( p: pc.OBJECT ); -- p is ty_proc.prof list of parms
  (*
     Check if a cyclic definition dependancy exists in parms list, i.e.
     some param's named type is currently being declared
     (otag_notype is set in this case)
  *)
  BEGIN
    WHILE p # NIL DO
      IF (p.type.obj # NIL) & (cc.otag_declaring IN p.type.obj.tags) THEN
	INCL (p.tags, cc.otag_notype);
      END;
      p := p.next;
    END;
  END RecursionInParms;

  -----------------------------------------------
  PROCEDURE OutTypeDef ( buf-,nm-: STR );
  (*
  *)
  BEGIN
    IF cc.op_comments THEN
      cmt.OutFirstComment (o.pos, 1, 0)
    END;
    IF nm = ""
     THEN out.WrFmt ("typedef %s;", buf);       -- smth was wrong: minimize errors
     ELSE out.WrFmt ("typedef %s %s;", buf, nm);
    END;
    IF cc.op_comments THEN
      cmt.OutTailComment (o.pos)
    END;
    out.WrNL;
    IF cc.op_comments THEN
      cmt.OutLastComment (o.pos, 0);
    END;
  END OutTypeDef;
  -------------------------------------------------

VAR nm,buf: STR;

BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(TypeDecl: '%s'", o.name^) END;
<* END *>

  ASSERT( o.mode = pc.ob_type );
  ASSERT( o.type.obj # NIL );
  OName (o, nm(*=>*));

  IF o.type.obj # o THEN                  (* rename *)
    ASSERT( TypeUsage (buf(*=>*), o.type, TRUE, TRUE) );
    OutTypeDef (buf, nm);
    INCL (o.tags, cc.otag_pub_defined);
    INCL (o.tags, cc.otag_pri_defined);
  ELSIF o.type.mode = pc.ty_record THEN
    ccr.Record (o.type);                       -- gen record definition
    IF ~cc.op_cpp THEN                         -- gen predeclaration for C only:
      IF cc.ttag_union IN o.type.tags THEN
        out.WrFmt ("union %s;", nm);
        out.WrNL;
        IF cc.ttag_typedef IN o.type.tags THEN       -- GENTYPEDEF+
          out.WrFmt ("typedef union %s %s;", nm, nm);
          out.WrNL;
        END;
      ELSE
        out.WrFmt ("struct %s;", nm);
        out.WrNL;
        IF cc.ttag_typedef IN o.type.tags THEN       -- GENTYPEDEF+
          out.WrFmt ("typedef struct %s %s;", nm, nm);
          out.WrNL;
        END
      END
    END;
    IF o.type.flag = pc.flag_o2 THEN
      OSecondName (o, 1, "_desc", nm(*=>*));
      out.WrStr ("extern ");
      IF cc.op_gendll THEN out.WrFmt ("%s ", nms.x2c[nms.nm_dll_tag]^) END;
      out.WrFmt ("struct %s %s;", nms.x2c[nms.nm_td_str]^, nm);
      out.WrNL;
    END;

  ELSIF o.type.mode = pc.ty_enum THEN
    EnumDeclaration (o.type);
    INCL (o.tags, cc.otag_pub_defined);
    INCL (o.tags, cc.otag_pri_defined);

  ELSIF o.type.mode = pc.ty_array_of THEN
    ArrayofDeclaration (o.type);

  ELSIF StdType (o.type, buf(*=>*)) THEN
    OutTypeDef (buf, nm);
    INCL (o.tags, cc.otag_pub_defined);
    INCL (o.tags, cc.otag_pri_defined);

  ELSE
    IF o.type.mode = pc.ty_proctype THEN
      RecursionInParms (o.type.prof);
    END;
    IF TypeConstructor (buf(*=>*), nm, o.type, FALSE) THEN
      -- nothing
    ELSE
      env.errors.Error (o.pos, 1017);
      buf[0] := '';
    END;
    OutTypeDef (buf, "");
    INCL (o.tags, cc.otag_pub_defined);
    INCL (o.tags, cc.otag_pri_defined);
  END;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")TypeDecl") END;
<* END *>
END TypeDeclaration;

-----------------------------------------------------------------------------

PROCEDURE LocVarDeclaration ( o: pc.OBJECT );
(*
*)
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(LocVarDecl: '%s'", o.name^) END;
<* END *>

  ASSERT( cur_proc # NIL );
  ASSERT( o.host = cur_proc );
  ASSERT( ~(pc.otag_valpar IN o.tags) );
  ASSERT( o.mode # pc.ob_varpar );

  IF cc.op_comments THEN cmt.OutFirstComment (o.pos, 1, 1) END; -- $$$ PR323
  OutLineNo (o.pos, 1);
  out.WrSpc (1);

  Declarator (o, FALSE);
  IF (o.type.mode = pc.ty_pointer)
   & (o.type.flag = pc.flag_o2)
  THEN
    out.WrStr (" = 0");
  END;
  out.WrChr (";");
  IF cc.op_comments THEN
    cmt.OutTailComment (o.pos)
  END;
  out.WrNL;
  IF cc.op_comments THEN
    cmt.OutLastComment (o.pos, 1);
  END;
  INCL (o.tags, cc.otag_pub_defined);
  INCL (o.tags, cc.otag_pri_defined);

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")LocVarDecl") END;
<* END *>
END LocVarDeclaration;

-----------------------------------------------------------------------------

PROCEDURE FuncDeclaration ( o: pc.OBJECT );
(*
   предварительное описание функции
*)
VAR
  buf: ARRAY 2048 OF CHAR; (* must be large *)
BEGIN
<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (1, "(FuncDecl: '%s'", o.name^) END;
<* END *>

  IF (o.mode = pc.ob_eproc) & cc.op_noextern THEN
  ELSE
    IF cc.op_comments THEN
      cmt.OutFirstComment (o.pos, 1, 0);
    END;
    IF nms.FuncIsExtern (o) THEN          --$$$ PR359
      out.WrStr ("extern ")
    ELSE
      out.WrStr ("static ");
    END;
    IF cc.op_gendll
     & ((o.mode = pc.ob_xproc) OR nms.FuncIsExtern (o))
    THEN
      out.WrFmt ("%s ", nms.x2c[nms.nm_dll_tag]^);
    END;
    FuncProfileDefinition (o.type, buf(*=>*), FALSE, FALSE);
    Declarator (o, FALSE);
    out.WrFmt ("(%s);", buf);
    out.WrNL;
    IF o.host.mode = pc.ty_record THEN
      out.WrStr ("typedef ");
      Declarator (o, TRUE);
      out.WrFmt ("(%s);", buf);
      out.WrNL;
    END;
    IF cc.op_comments THEN
      cmt.OutLastComment (o.pos, 0);
    END;
    out.WrNL;
  END;
  INCL (o.tags, cc.otag_pub_defined);

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")FuncDecl") END;
<* END *>
END FuncDeclaration;

-----------------------------------------------------------------------------

PROCEDURE ObjectDeclaration* ( o: pc.OBJECT );
(*
   предварительное объявление объекта
*)
VAR
   i: nms.INFO;
  sv: TMP_VAR;
BEGIN
  IF cc.otag_declared IN o.tags THEN RETURN END;
  IF cc.otag_declaring IN o.tags THEN
    env.errors.Error (o.pos, 1018);
    ASSERT( o.mode = pc.ob_type );
    RETURN;
  END;
<* IF DB_TRACE THEN *>
IF out.EnDebug THEN dbg.CallTraceInd (1, "(ObjectDecl: '%s'", o.name^) END;
<* END *>

  ASSERT( o.mno = CurMod.mno );
  INCL (o.tags, cc.otag_declaring);
  sv := tmp_busy;
  tmp_busy := NIL;
  IF (o.mode = pc.ob_var) & (o.lev > 0) THEN
    out.Push;
  ELSE
    i := OInfo (o, -1, "");
    IF i.seg = NIL THEN
      ASSERT( gen_def );
      ASSERT( o.lev = 0 );
      (* declaring non public object in header file *)
      i.seg := seg.NewSegment (o.pos, 0);
    END;
    seg.EnterSegment (i.seg);
  END;

  CASE o.mode OF
  | pc.ob_var:
      IF o.lev = 0
       THEN GlobVarDeclaration(o);
       ELSE LocVarDeclaration (o);
      END;

  | pc.ob_cons:
       GlobVarDeclaration (o);

  | pc.ob_proc
  , pc.ob_xproc
  , pc.ob_eproc
  , pc.ob_lproc
  , pc.ob_cproc:
      FuncDeclaration (o);

  | pc.ob_type:
      TypeDeclaration (o);

  | pc.ob_module:
      IF ~(CurMod.type.flag IN cc.C_LIKE_FLAGS+pc.LangSet{pc.flag_sl1}) THEN
        out.WrNL;
        ModuleBodyDef (CurMod, FALSE);
        out.WrChr (';');
        out.WrNL;
        out.WrNL;
        INCL (o.tags, cc.otag_pub_defined);
      END;
  ELSE
    (* o.mode = pc.nd_xxx -> object is deleted! *)
    env.errors.Fault (o.pos, 1019);
  END;
  IF (o.mode = pc.ob_var) & (o.lev > 0) THEN
    out.AppendSegm (loc_segm(*=>*));
    out.Pop;
  ELSE
    seg.ExitSegment;
  END;

  EXCL (o.tags, cc.otag_declaring);
  INCL (o.tags, cc.otag_declared);
  IF tmp_busy # NIL THEN
    env.errors.Error (o.pos, 1019);
  END;
  tmp_busy := sv;

<* IF DB_TRACE THEN *>
  IF out.EnDebug THEN dbg.CallTraceInd (-1, ")ObjectDecl") END;
<* END *>
END ObjectDeclaration;

-----------------------------------------------------------------------------

<* IF ~NODE_VALUE_REPRESENTATION THEN *>

PROCEDURE NewIntValue*(x: LONGINT): pc.VALUE;
  VAR v: pc.VALUE;
BEGIN
  IF type_ZZ=NIL THEN
    NEW(type_ZZ);
    type_ZZ.mode:=pc.ty_ZZ;
    type_ZZ.pos:=env.null_pos;
  END;
  v:=pc.value.new(type_ZZ.pos,type_ZZ);
  v.set_integer(x);
  RETURN v;
END NewIntValue;

PROCEDURE NewRealValue*(x: LONGREAL): pc.VALUE;
  VAR v: pc.VALUE;
BEGIN
  IF type_RR=NIL THEN
    NEW(type_RR);
    type_RR.mode:=pc.ty_RR;
    type_RR.pos:=env.null_pos;
  END;
  v:=pc.value.new(type_RR.pos,type_RR);
  v.set_real(x);
  RETURN v;
END NewRealValue;
<* END *>

PROCEDURE ini*;
(*
*)
BEGIN
<* IF NODE_VALUE_REPRESENTATION THEN *>
  zz_tmp := ir.NewIntValue (0);
  rr_tmp := ir.NewRealValue (0.0);
  IF max_longint = NIL THEN
    max_longint := ir.NewIntValue (MAX (LONGINT))
  END;
  IF zz_zero = NIL THEN
    zz_zero := ir.NewIntValue (0)
  END;
  NEW (SIZE_T);
  NEW (LSET_T);
  NEW (SEQU_T);
  IF cc.op_index16
   THEN ir.IniType (SIZE_T, pc.ty_cardinal);
   ELSE ir.IniType (SIZE_T, pc.ty_longint);
  END;
  INCL (SIZE_T.tags, pc.ttag_c_type);
  ASSERT( SIZE_T.max # NIL );
  ASSERT( SIZE_T.min # NIL );
  ir.IniType (LSET_T, pc.ty_longcard);
  LSET_BITS := SHORT (pc.code.get_size (pc.su_bits, LSET_T));
<* ELSE *>
  zz_tmp:=NewIntValue(0);
  rr_tmp:=NewRealValue(0.0);
  IF max_longint = NIL THEN
    max_longint := NewIntValue(MAX(LONGINT));
  END;
  IF zz_zero = NIL THEN
    zz_zero := NewIntValue(0)
  END;

  NEW (SEQU_T);
  IF cc.op_index16
   THEN SIZE_T:=pc.new_type(pc.ty_cardinal);
   ELSE SIZE_T:=pc.new_type(pc.ty_longint);
  END;
  INCL (SIZE_T.tags, pc.ttag_c_type);
  ASSERT( SIZE_T.max # NIL );
  ASSERT( SIZE_T.min # NIL );
  LSET_T := pc.new_type(pc.ty_longcard);
  LSET_BITS := SHORT (pc.code.get_size (pc.su_bits, LSET_T));
<* END *>
  cns_vars  := NIL;
  tmp_vars  := NIL;
  tmp_busy  := NIL;
  mod_usage := NIL;
  sl1_usage := FALSE;
END ini;

-----------------------------------------------------------------------------

PROCEDURE exi*;
(*
*)

BEGIN
  zz_tmp  := NIL;
  rr_tmp  := NIL;
<* IF ~NODE_VALUE_REPRESENTATION THEN *>
  type_ZZ:=NIL;
  type_RR:=NIL;
<* END *>
  SIZE_T  := NIL;
  LSET_T  := NIL;
  SEQU_T  := NIL;
  out.GetSegment (glo_segm);
  out.GetSegment (glo_segm);
  out.GetSegment (loc_segm);
  cur_proc := NIL;
  cns_vars := NIL;
  tmp_vars := NIL;
  tmp_busy := NIL;
  CurMod   := NIL;
  mod_usage:= NIL;
END exi;

BEGIN
  CmpValue_tmp := NIL;
  max_longint  := NIL;
  zz_zero      := NIL;
END ccDcl.
