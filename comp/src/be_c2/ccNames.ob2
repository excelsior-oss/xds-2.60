(** Copyright (c) 1991,98 XDS Ltd, Russia. All Rights Reserved. *)
(** C back-end: visibility, names, standard names, keywords *)
MODULE ccNames; (** Sem 22-Sep-93. *)

(* Modifications:
   15/Apr/98 Vit  5.03  Names import from .nms file is implemented.
   05/Apr/98 Vit  5.00  Comments inserted; some code/naming clarification done.
                        sn_xxx constants removed as not used
			Renamed from ccN
   (XDS versions below, BE - above)
   15/Feb/96 Ned  2.12  proc GetProcClass added instead of nm_proclass
                        spec.set consts added: SPEC_PROCLASS, DFLT_PROCLASS.
   17/Mar/96 Ned  2.12  object and type tags are moved to ccK.
*)

IMPORT     xcMain
  ,        CharClass
  , fmt := xcStr
  , xfs := xiFiles
  , env := xiEnv
  ,  pc := pcK
  ,  cc := ccK
  , seg := ccSeg
  , out := ccOut
  , dbg := pcVisIR
  ;

(** Object names *)

CONST
  INDEX_LEN = 256;

TYPE
  NAME* = POINTER TO NAME_REC;   -- OBJECT extension types
  RENM* = POINTER TO RENM_REC;
  INFO* = POINTER TO INFO_REC;
  TREE* = POINTER TO TREE_REC;

  NAME_REC = RECORD (pc.bext_rec)
    name*: pc.STRING;
    l    : NAME;
    r    : NAME;
    bal  : INTEGER;
  END;

  RENM_REC = RECORD (NAME_REC)
    renm : pc.STRING;
  END;

  INFO_REC = RECORD (NAME_REC)
    obj : pc.OBJECT;
    no *: INTEGER;
    seg*: seg.SEGMENT;
    nxt*: INFO;
  END;

  TREE_REC = RECORD (pc.bext_rec)
    root : NAME;
    index: POINTER TO ARRAY INDEX_LEN OF NAME;
    node*: pc.BEXT;
    seg *: seg.SEGMENT; (* record type: definition segment      *)
  END;

CONST

(*
                declaration     public def.     private def.

ob_proc         profile         -               body
ob_xproc        profile         -               body
ob_cproc        profile         -               body
ob_cproc        profile         -               -
ob_type
  rename        typedef         -               -
  ty_record     struct X        struct X { }    type descriptor
  ty_array_of   struct X        struct X { }    -
  other         typedef         -               -
ob_module       profile         -               body, descriptor

*)
  nm_empty         *=   0;
  nm_shortcard     *=   1;
  nm_cardinal      *=   2;
  nm_longcard      *=   3;
  nm_shortint      *=   4;
  nm_integer       *=   5;
  nm_longint       *=   6;
  nm_real          *=   7;
  nm_longreal      *=   8;
  nm_ld_real       *=   9;
  nm_boolean       *=  10;
  nm_char          *=  11;
  nm_AA            *=  12;
  nm_loc           *=  13;
  nm_protection    *=  14;
  nm_set8          *=  15;
  nm_set16         *=  16;
  nm_set32         *=  17;
  nm_complex       *=  18;
  nm_lcomplex      *=  19;
  nm_void          *=  20;
  nm_seq_type      *=  21;
  nm_lset          *=  22;
  nm_cast          *=  23;
  nm_a_chk         *=  24;
  nm_p_chk         *=  25;
  nm_protect       *=  26;
  nm_xon           *=  27;
  nm_xoff          *=  28;
  nm_xtry          *=  29;
  nm_xretry        *=  30;
  nm_finally       *=  31;
  nm_xremove       *=  32;
  nm_dynarr_addr   *=  33;
  nm_dynarr_len    *=  34;
  nm_dynarr_size   *=  35;
  nm_memcmp        *=  36;
  nm_memcpy        *=  37;
  nm_strcmp        *=  38;
  nm_quo           *=  39;
  nm_div           *=  40;
  nm_rem           *=  41;
  nm_mod           *=  42;
  nm_cap           *=  43;
  nm_in            *=  44;  (* IN SET *)
  nm_inl           *=  45;  (* IN LONG SET *)
  nm_alloc_param   *=  46;
  nm_free_param    *=  47;
  nm_rts_body      *=  48;
  nm_length        *=  49;
  nm_set_range     *=  50;
  nm_assert        *=  51;
  nm_trap          *=  52;
  nm_case_trap     *=  53;
  nm_return_trap   *=  54;
  nm_guard_trap    *=  55;
  nm_rot_set       *=  56;
  nm_rot_lset      *=  57;
  nm_trunci        *=  58;
  nm_truncc        *=  59;
  nm_proc          *=  60;
  nm_cplx_cmp      *=  61;
  nm_cplx_add      *=  62;
  nm_cplx_sub      *=  63;
  nm_cplx_mul      *=  64;
  nm_cplx_div      *=  65;
  nm_cplx_lcmp     *=  66;
  nm_cplx_ladd     *=  67;
  nm_cplx_lsub     *=  68;
  nm_cplx_lmul     *=  69;
  nm_cplx_ldiv     *=  70;
  nm_i_chk         *=  71;
  nm_i_chkl        *=  72;
  nm_r_chk         *=  73;
  nm_r_chkl        *=  74;
  nm_r_chku        *=  75;
  nm_r_chkul       *=  76;
  nm_halt          *=  77;
  nm_abort         *=  78;
  nm_move          *=  79;
  nm_copy          *=  80;
  nm_allocate      *=  81;
  nm_deallocate    *=  82;
  nm_dynallocate   *=  83;
  nm_dyndeallocate *=  84;
  nm_new_open      *=  85;
  nm_new           *=  86;
  nm_index         *=  87;
  nm_dispose       *=  88;
  nm_cplx_neg      *=  89;
  nm_cplx_lneg     *=  90;
  nm_strncpy       *=  91;
  nm_lsh_set       *=  92;
  nm_lsh_lset      *=  93;
  nm_ash           *=  94;
  nm_fill           =  95;
  nm_dyncallocate*  =  96;
  nm_dyncdeallocate*=  97;
  nm_incl          *=  98;
  nm_excl          *=  99;
  nm_ofs           *= 100;
  nm_ofs_rec       *= 101;
  nm_ofs_arr       *= 102;
  nm_dll_tag       *= 103;
  nm_gen_dll       *= 104;
  nm_dll_export    *= 105;
  nm_dll_import    *= 106;
  nm_proclass       = 107;
  nm_pascal         = 108;
  nm_stdcall        = 109;
  nm_syscall        = 110;
  nm_get_td        *= 111;
  nm_inc_char       = 112;
  nm_inc_shortint   = 113;
  nm_inc_integer    = 114;
  nm_inc_longint    = 115;
  nm_inc_shortcard  = 116;
  nm_inc_cardinal   = 117;
  nm_inc_longcard   = 118;
  nm_dec_char       = 119;
  nm_dec_shortint   = 120;
  nm_dec_integer    = 121;
  nm_dec_longint    = 122;
  nm_dec_shortcard  = 123;
  nm_dec_cardinal   = 124;
  nm_dec_longcard   = 125;
  nm_abs_int8       = 126;
  nm_abs_int16      = 127;
  nm_abs_int32      = 128;
  nm_min_real       = 129;
  nm_min_longreal   = 130;
  nm_min_ld_real    = 131;
  nm_max_real       = 132;
  nm_max_longreal   = 133;
  nm_max_ld_real    = 134;
  nm_and           *= 135;
  nm_or            *= 136;
  nm_xor           *= 137;
  nm_bic           *= 138;
  nm_exp_ci         = 139;
  nm_exp_cr         = 140;
  nm_exp_li         = 141;
  nm_exp_lr         = 142;
  nm_exp_ri         = 143;
  nm_exp_rr         = 144;
  nm_complement    *= 145;
  nm_entier        *= 146;
  nm_chksl         *= 147;
  nm_longset       *= 148;
  nm_guardp        *= 149;
  nm_guardpe       *= 150;
  nm_guardv        *= 151;
  nm_guardve       *= 152;
  nm_set_hinfo     *= 153;
  nm_call          *= 154;
  nm_set_equ       *= 155;
  nm_set_leq       *= 156;
  nm_prot          *= 157;
  nm_xhandler_str  *= 158;
  nm_ofs_end       *= 159;
  nm_td_str        *= 160;
  nm_profile_str   *= 161;
  nm_prf           *= 162;
  nm_proc_prf      *= 163;
  nm_proc_inp      *= 164;
  nm_proc_out      *= 165;
  nm_md_str        *= 166;
  nm_stack_limit   *= 167;
  nm_module        *= 168;
  nm_exit          *= 169;
  nm_min_longint   *= 170;
  nm_max_longint   *= 171;
  nm_max_longcard  *= 172;
  nm_td            *= 173;
  nm_cplx_lcplx    *= 174;
  nm_inrange       *= 175;
  nm_field_ofs     *= 176;
  nm_divr          *= 177;
  nm_divl          *= 178;

  MAXNM = 178;           (* dont forget to adjust to last nm_xxx! *)

VAR
  ident_len  : LONGINT;
  NamesCnt   : LONGINT;
  EnCntNames : BOOLEAN;   -- switch to enable names counting for SL1
  sys_tree   : TREE;
  glo_tree   : TREE;
  sym_list   : INFO;

  x2c*  : ARRAY MAXNM+1 OF pc.STRING;  -- List of X2C names (read from .nms file)

----------------------------------------------------------------------------

PROCEDURE FuncIsExtern* ( o: pc.OBJECT )   :   BOOLEAN;
(*
*)
BEGIN
  ASSERT( o.mode IN pc.PROCs );
  RETURN (o.mode = pc.ob_eproc)
      OR (o.mode = pc.ob_lproc)
      OR (pc.otag_public IN o.tags)
      OR (o.mode = pc.ob_xproc) & cc.op_gendll
      OR (o.host.mode = pc.ty_record)
       & (pc.otag_public IN o.host.obj.tags);
END FuncIsExtern;

----------------------------------------------------------------------------

PROCEDURE StrCat* ( VAR d: ARRAY OF CHAR
                  ;    s-: ARRAY OF CHAR );
(*
*)
VAR i,j: LONGINT;
BEGIN
  i := 0;
  WHILE (i < LEN (d)) & (d[i] # '') DO
    INC (i)
  END;
  j := 0;
  WHILE (j < LEN (s)) & (s[j] # '') & (i < LEN (d)-1) DO
    d[i] := s[j];
    INC (i);
    INC (j)
  END;
  d[i] := '';
END StrCat;

----------------------------------------------------------------------------

PROCEDURE StrCmp ( x-: ARRAY OF CHAR
                 ; y-: ARRAY OF CHAR
                 )   : 		INTEGER;
(*
*)
VAR i: INTEGER;
BEGIN
  i := 0;
  WHILE (x[i] # '') & (x[i] = y[i]) DO
    INC (i)
  END;
  IF x[i] > y[i] THEN RETURN  1 END;
  IF x[i] < y[i] THEN RETURN -1 END;
  RETURN 0;
END StrCmp;

----------------------------------------------------------------------------

PROCEDURE AppBal (     x: NAME
                 ; VAR p: NAME
                 ; VAR h: BOOLEAN
                 )      :      NAME;
(*
   Recursive component: insert object 'o' into balanced subtree 'p'
   => h=T -> tree height has increased
      p.bal = height(p.r)-height(p.l)
   => NIL if object inserted, #NIL-object, if was found
*)
VAR
  p1,p2,y: NAME;
  r: LONGINT;
BEGIN
  IF p = NIL THEN
    p     := x;
    x.l   := NIL;
    x.r   := NIL;
    h     := TRUE;
    x.bal := 0;
    RETURN NIL;
  END;
  r := StrCmp (p.name^, x.name^);
  IF r > 0 THEN
    y := AppBal (x, p.l(*=>*), h(*=>*));
    IF h THEN
      CASE p.bal OF
      | 1:
          p.bal := 0;
          h     := FALSE;
      | 0:
          p.bal := -1;
      |-1:
	  p1 := p.l;
	  IF p1.bal = -1 THEN
            p.l   := p1.r;
            p1.r  := p;
            p.bal := 0;
            p     := p1;
	  ELSE
            p2    := p1.r;
            p1.r  := p2.l;
            p2.l  := p1;
            p.l   := p2.r;
            p2.r  := p;
            IF p2.bal = -1
             THEN p.bal := +1
             ELSE p.bal := 0
            END;
            IF p2.bal = +1
             THEN p1.bal := -1
             ELSE p1.bal := 0
            END;
	    p := p2;
	  END;
          p.bal := 0;
          h     := FALSE;
      END;
    END;
    RETURN y;
  ELSIF r < 0 THEN
    y := AppBal (x, p.r(*=>*), h(*=>*));
    IF h THEN
      CASE p.bal OF
      |-1:
           p.bal := 0;
           h     := FALSE;
      | 0:
           p.bal := +1;
      |+1:
           p1 := p.r;
           IF p1.bal = +1 THEN
             p.r   := p1.l;
             p1.l  := p;
             p.bal := 0;
             p     := p1;
           ELSE
             p2   := p1.l;
             p1.l := p2.r;
             p2.r := p1;
             p.r  := p2.l;
             p2.l := p;
             IF p2.bal = +1
              THEN p.bal := -1
              ELSE p.bal := 0
             END;
             IF p2.bal = -1
              THEN p1.bal := +1
              ELSE p1.bal := 0
             END;
             p := p2;
           END;
           p.bal := 0;
           h     := FALSE;
      END;
    END;
    RETURN y;
  ELSE
    h := FALSE;
    RETURN p;
  END;
END AppBal;

----------------------------------------------------------------------------

PROCEDURE Search (  t: TREE
                 ; n-: ARRAY OF CHAR
                 )   : 		NAME;
(*
*)
VAR
  j: LONGINT;
  i: NAME;
BEGIN
  IF t = NIL THEN t := glo_tree END;
  IF t.index # NIL THEN
    j := ORD (n[0]);
    IF j # 0 THEN
      j := (j+ORD (n[1])) MOD INDEX_LEN
    END;
    i := t.index[j];
  ELSE
    i := t.root;
  END;
  LOOP
    IF i = NIL THEN RETURN NIL END;
    j := StrCmp (i.name^, n);
    IF j = 0 THEN RETURN i END;
    IF j < 0
     THEN i := i.r
     ELSE i := i.l
    END;
  END;
END Search;

----------------------------------------------------------------------------

PROCEDURE Insert ( tree: TREE
                 ;    i: NAME
                 )     : 	NAME;
(*
*)
VAR
  j: LONGINT;
  h: BOOLEAN;
BEGIN
  ASSERT( i # NIL );
  IF tree = NIL THEN
    tree := glo_tree
  END;
  i.l := NIL;
  i.r := NIL;
  IF tree.index # NIL THEN
    j := ORD (i.name[0]);
    IF j # 0 THEN
      j := (j+ORD (i.name[1])) MOD INDEX_LEN
    END;
    RETURN AppBal (i, tree.index[j](*=>*), h(*=>*));
  ELSE
    RETURN AppBal (i, tree.root(*=>*), h(*=>*));
  END;
END Insert;

----------------------------------------------------------------------------

PROCEDURE NewTree* ( VAR t: TREE );
(*
*)
BEGIN
  NEW (t);
END NewTree;

----------------------------------------------------------------------------

PROCEDURE GetTree (   h: pc.STRUCT
                   ; md: pc.OB_MODE
                   )   : 		TREE;
(*
*)
VAR l: TREE;
BEGIN
  IF h = NIL THEN RETURN glo_tree END;
  IF md IN (pc.PROCs+pc.OB_SET{pc.ob_type, pc.ob_cons}) THEN
    RETURN glo_tree
  END;
  IF h.mode = pc.ty_enum THEN
    WHILE h.use # NIL DO h := h.use.obj.type END; -- get orig type for renamed
    ASSERT( h.obj.mode = pc.ob_type );
    h := h.obj.host
  END;
  IF h.mode = pc.ty_module THEN RETURN glo_tree END;
  IF (h.obj # NIL) THEN
    h := h.obj.type
  END;
  IF h.ext # NIL THEN RETURN h.ext (TREE) END;
  ASSERT(   (h.mode = pc.ty_record)
         OR (h.mode = pc.ty_proctype) );
  NewTree (l(*=>*));
  h.ext := l;
  RETURN l;
END GetTree;

----------------------------------------------------------------------------

PROCEDURE Disable* (   h: pc.STRUCT
                   ; nm-: ARRAY OF CHAR );
(*
*)
VAR
  t: TREE;
  i: NAME;
BEGIN
  t := GetTree (h, pc.ob_var);
  NEW (i);
  NEW (i.name, LENGTH (nm)+1);
  COPY (nm, i.name^);
  i := Insert (t, i);
END Disable;

----------------------------------------------------------------------------

PROCEDURE MakeName* ( VAR res: ARRAY OF CHAR --> resulting name
                    ;    host: pc.STRUCT     -- scope
                    ;     mod: pc.STRUCT     -- if #NIL, prefix by module name
                    ;    mode: pc.OB_MODE    -- used to define C-visibility
                    ;   from-: ARRAY OF CHAR -- name pattern
                    ;      no: INTEGER       -- id no; C name, if < 0
                    )        : 		INFO;
(*
*)
VAR
  fr,nm,sf: pc.NAME;
       n,m: LONGINT;
       loc: TREE;
         i: INFO;
         j: NAME;

  -------------------------------------------------
  PROCEDURE ChkName ()   :   BOOLEAN;
  (*
  *)
  VAR i: INTEGER;
  BEGIN
    i := 0;
    LOOP
      CASE from[i] OF
      | 0X:
        fr[i] := 0X;
        EXIT;

      | '.'
      , '$'
      , '#':
        fr[i] := '_';

      ELSE
        IF cc.IsLetDig[ORD (from[i])]
         THEN fr[i] := from[i];
         ELSE RETURN FALSE;
	END;

      END;

      INC (i);
    END;
    RETURN i > 0;
  END ChkName;
  -------------------------------------------------

BEGIN
dbg.Trace("\nMakeName: from=%s mode=%d no=%d ", from, mode, no);
  NEW (i);
  i.l   := NIL;
  i.r   := NIL;
  i.seg := seg.CurSegment;
  loc   := GetTree (host, mode);
  IF no >= 0 THEN
    n := 0;
    IF cc.op_uniqnames THEN                        --!! renamed enumerators
      IF mod # NIL THEN                            --!! matching IS a problem!
        ASSERT( mod.mode = pc.ty_module );
        WHILE (mod.obj.name^[n] # 0X) & (n < LEN (nm)-2) DO
          nm[n] := mod.obj.name^[n];
          INC (n);
        END;
        nm[n] := '_';
        INC (n);
      END;
    END;
    IF ~ChkName() THEN fr := "anonym" END;
    m := 0;
    WHILE (n < LEN (nm)-1) & (fr[m] # 0X) DO
      nm[n] := fr[m];
      INC (n);
      INC (m);
    END;
    nm[n] := 0X;
    m := -1;
    LOOP
      IF m >= 0 THEN
        fmt.prn_txt (sf(*=>*), "%d", m);
        fmt.prn_txt (res(*=>*), "%.*s%s", ident_len-LENGTH (sf), nm, sf);
      ELSIF n <= ident_len THEN
        COPY (nm, res);
      ELSE
        fmt.prn_txt (res(*=>*), "%.*s", ident_len, nm);
      END;
      IF mode IN pc.OB_SET{pc.ob_eproc, pc.ob_lproc} THEN EXIT END;
      j := Search (sys_tree, res);
      WHILE (j # NIL) & (j IS RENM) DO
        COPY (j(RENM).renm^, res);
        j := Search (sys_tree, res);
      END;
      IF (j # NIL) OR (Search (loc, res) # NIL) THEN
        (* nothing *)
      ELSIF (loc = glo_tree)
         OR (Search (glo_tree, res) = NIL)
      THEN
        EXIT
      END;
      IF (m >= 10) & (EnCntNames) THEN
        IF m >= NamesCnt THEN
          NamesCnt := m+1
        END;
        m := NamesCnt;
        INC (NamesCnt);
      ELSE
        INC (m);
      END;
    END;
    i.no := no;
    NEW (i.name, LENGTH (res)+1);
    COPY (res, i.name^);
    j := Insert (loc, i);
    ASSERT( (j = NIL) OR (mode IN pc.OB_SET{pc.ob_eproc, pc.ob_lproc}) );
  ELSE
    i.no := 0;
    COPY (from, res);
    NEW (i.name, LENGTH (res)+1);
    COPY (res, i.name^);
    (* C name can be equal to reserved name *)
    j := Insert (loc, i);
  END;
dbg.Trace("=> %s\n", res);
  RETURN i;
END MakeName;

----------------------------------------------------------------------------

PROCEDURE MakeInfo* (       o: pc.OBJECT
                    ;      no: INTEGER
                    ; suffix-: ARRAY OF CHAR
                    )        : 		INFO;
(*
*)

(* no = -2 : use suffix only *)

VAR
  m     : pc.STRUCT;
  res,nm: ARRAY 128 OF CHAR;
  i     : INFO;
  j     : NAME;
BEGIN

  IF ~(o.flag IN pc.LangSet{pc.flag_o2, pc.flag_m2}) THEN -- Module object definition
    m := NIL;
  ELSIF o.mode IN pc.PROCs THEN
    IF FuncIsExtern (o)
     THEN m := pc.mods[o.mno].type
     ELSE m := NIL
    END;
  ELSIF (o.mode = pc.ob_cons)
     OR (pc.otag_public IN o.tags)
     OR (o.mode = pc.ob_type)
      & (o.type.mode = pc.ty_record)
      & (o.type.flag = pc.flag_o2)
     OR (o.host # NIL)
      & (o.host.mode = pc.ty_enum)
      & (pc.otag_public IN o.host.obj.tags)
  THEN
    m := pc.mods[o.mno].type;
  ELSE
    m := NIL;
  END;

  IF (o.name = NIL) OR (no = -2) THEN -- Append suffix to a name
    COPY (suffix, nm);
  ELSE
    fmt.prn_txt (nm(*=>*), "%s%s", o.name^, suffix);
  END;
  j := Search (sys_tree, o.name^);
  IF (j # NIL)  & (no = -1) THEN
     no := 0;
  END;

  IF ~(o.flag IN pc.LangSet{pc.flag_o2, pc.flag_m2})
   & cc.op_noheader THEN                   -- non-M2/O2 objects with NOHEADER+
    no := -1;                              -- force no renaming
  END;
  i := MakeName (res(*=>*), o.host, m, o.mode, nm, no);                                                  --..$$$

  j := Search (sys_tree, i.name^);
  IF j # NIL THEN
    env.errors.Warning (o.pos, 354, i.name^);
  END;

  i.obj := o;
  IF o.ext # NIL THEN
    i.nxt := o.ext (INFO)
  END;
  o.ext := i;
  RETURN i;
END MakeInfo;


(*===================== CODE METHODS IMPLEMENTATION ========================*)

PROCEDURE ( i: INFO ) out* ( f: xfs.SymFile );
(*
   out_object method implementation for class CODE
*)
VAR
  j: INFO;
  n: LONGINT;
BEGIN
  IF i.obj.lev > 0 THEN
    (* for field path info .obj = NIL !!!!! *)
    f.WriteInt (0);
  ELSE
    j := i;
    n := 1;
    WHILE j.nxt # NIL DO
      j := j.nxt;
      INC (n)
    END;
    f.WriteInt (n);
    j := i;
    WHILE j # NIL DO
      f.WriteString (j.name^);
      f.WriteInt (j.no);
      j := j.nxt;
    END;
  END;
END out;

----------------------------------------------------------------------------

PROCEDURE ( t: TREE ) out* ( f: xfs.SymFile );
(*
   out_object method implementation for class CODE
*)
BEGIN
  (* nothing *)
END out;

----------------------------------------------------------------------------

PROCEDURE SkipObject* ( f: xfs.SymFile );
(*
   skip_object method implementation for class CODE
*)
VAR
  s  : pc.NAME;
  i,j: LONGINT;
BEGIN
  f.ReadInt (i(*=>*));
  WHILE i > 0 DO
    f.ReadString (s(*=>*));
    f.ReadInt (j(*=>*));
    DEC (i);
  END;
END SkipObject;

----------------------------------------------------------------------------

PROCEDURE InpObject* ( f: xfs.SymFile
                     ; o: pc.OBJECT);
(*
   inp_object method implementation for class CODE
*)
VAR
  i  : INFO;
  n,l: LONGINT;
  bf : out.STR;
BEGIN
  f.ReadInt (n(*=>*));
  WHILE n > 0 DO
    NEW (i);
    f.ReadString (bf(*=>*));
    NEW (i.name, LENGTH (bf)+1);
    COPY (bf, i.name^);
    f.ReadInt (l(*=>*));
    i.no  := SHORT (l);
    i.obj := o;
    i.nxt := sym_list;
    sym_list := i;
    DEC (n);
  END;
END InpObject;

----------------------------------------------------------------------------

PROCEDURE SkipStruct* ( f: xfs.SymFile );
(*
   skip_struct method implementation for class CODE
*)
BEGIN
  (* nothing *)
END SkipStruct;

----------------------------------------------------------------------------

PROCEDURE InpStruct* ( f: xfs.SymFile
                     ; s: pc.STRUCT);
(*
   inp_struct method implementation for class CODE
*)
BEGIN
  (* nothing *)
END InpStruct;


(*==========================================================================*)

PROCEDURE DoSymList*;
(*
*)
VAR
  j,i : INFO;
  n   : NAME;
  tree: TREE;
BEGIN
  WHILE sym_list # NIL DO
    i := sym_list;
    sym_list := i.nxt;
    IF i.obj.ext = NIL
     THEN i.nxt := NIL
     ELSE i.nxt := i.obj.ext (INFO)
    END;
    i.obj.ext := i;
    tree := GetTree (i.obj.host, i.obj.mode );
    n    := Insert (tree, i);
    IF n # NIL THEN
      IF n IS INFO
       THEN j := n (INFO)
       ELSE j := NIL
      END;
      IF (i.obj.mode IN pc.FIELDs) & (i.no >= 0)
      OR (i.obj.mode = pc.ob_eproc)
      OR (j # NIL) & (j.obj # NIL) & (j.obj.mode = pc.ob_eproc)
      OR (i.obj.flag IN cc.C_LIKE_FLAGS)
      OR (j # NIL) & (j.obj # NIL) & (j.obj.flag IN cc.C_LIKE_FLAGS)
      THEN
	-- nothing
      ELSIF (j # NIL) & (j.obj # NIL) THEN
        env.errors.Error (i.obj.pos, 1003, pc.mods[i.obj.mno].name^,
                          i.obj.name^, pc.mods[j.obj.mno].name^, j.obj.name^);
      ELSE
        env.errors.Error (i.obj.pos, 1004, pc.mods[i.obj.mno].name^,
                          i.obj.name^);
      END;
    END;
  END;
END DoSymList;

----------------------------------------------------------------------------

PROCEDURE SprocName* ( ps: pc.TPOS
                     ;  p: pc.SUB_MODE
                     )   : 		INTEGER;
(*
   Returns standard procedure name token
*)
BEGIN
  CASE p OF
  | pc.sp_halt  : RETURN nm_halt;
  | pc.sp_abort : RETURN nm_abort;
  | pc.sp_move  : RETURN nm_move;
  | pc.sp_fill  : RETURN nm_fill;
  | pc.sp_copy  : RETURN nm_copy;
  ELSE
    env.errors.Error (ps, 1005);
    RETURN nm_empty;
  END;
END SprocName;

----------------------------------------------------------------------------

PROCEDURE GetProcClass* ( flag: pc.Lang )   :   INTEGER;
(*
   Returns procedure class modifier token
*)
BEGIN
  IF    flag IN cc.DFLT_PROCLASS THEN RETURN nm_proclass;
  ELSIF flag IN cc.SPEC_PROCLASS THEN
    IF    flag = pc.flag_p       THEN RETURN nm_pascal;
    ELSIF flag = pc.flag_stdcall THEN RETURN nm_stdcall;
    ELSIF flag = pc.flag_syscall THEN RETURN nm_syscall;
    ELSE ASSERT ( FALSE );
    END;
  ELSE RETURN nm_empty;
  END;
END GetProcClass;

----------------------------------------------------------------------------

PROCEDURE CmplxFunc* (  t: pc.STRUCT
                     ; sb: pc.SUB_MODE
                     )   : 		INTEGER;
(*
   Returns complex procedure name token
*)
BEGIN
  IF t.mode = pc.ty_complex THEN
    CASE sb OF
    | pc.sb_equ
    , pc.sb_neq  : RETURN nm_cplx_cmp;
    | pc.sb_plus : RETURN nm_cplx_add;
    | pc.sb_minus: RETURN nm_cplx_sub;
    | pc.sb_mul  : RETURN nm_cplx_mul;
    | pc.sb_slash: RETURN nm_cplx_div;
    END
  ELSE
    CASE sb OF
    | pc.sb_equ
    , pc.sb_neq  : RETURN nm_cplx_lcmp;
    | pc.sb_plus : RETURN nm_cplx_ladd;
    | pc.sb_minus: RETURN nm_cplx_lsub;
    | pc.sb_mul  : RETURN nm_cplx_lmul;
    | pc.sb_slash: RETURN nm_cplx_ldiv;
    END;
  END;
END CmplxFunc;

----------------------------------------------------------------------------

PROCEDURE IndexChk* ( t: pc.STRUCT
                    )  : 	INTEGER; --> proper check name
(*
   Returns index check name token. (t - array's type)
*)
VAR tb: pc.STRUCT;
BEGIN
  ASSERT( t.mode IN pc.TY_SET{pc.ty_array, pc.ty_array_of, pc.ty_SS, pc.ty_set} );
  IF t.mode = pc.ty_array_of THEN
    tb := t.inx.super_type();
    IF tb.mode IN pc.TY_SET{pc.ty_longint, pc.ty_longcard}
     THEN RETURN nm_i_chkl;
     ELSE RETURN nm_i_chk;
    END;
  ELSE
    IF t.len >= 10000H
     THEN RETURN nm_i_chkl;
     ELSE RETURN nm_i_chk;
    END
  END;
END IndexChk;

----------------------------------------------------------------------------

PROCEDURE RangeChk* ( t: pc.STRUCT )   :   INTEGER;
(*
   Returns range check name token
*)
BEGIN
  IF t.mode # pc.ty_set THEN    (* !!!! see to.do *)
    t := t.super_type()
  END;

  CASE t.mode OF
  | pc.ty_shortint
  , pc.ty_integer  : RETURN nm_r_chk;
  | pc.ty_longint  : RETURN nm_r_chkl;
  | pc.ty_shortcard
  , pc.ty_cardinal : RETURN nm_r_chku;
  | pc.ty_longcard
  , pc.ty_set      : RETURN nm_r_chkul;
  END;
END RangeChk;

----------------------------------------------------------------------------

PROCEDURE IncDec* ( m: pc.SUB_MODE
                  ; t: pc.STRUCT
                  )  : 		INTEGER;
(*
   Returns inc/dec name token
*)
VAR md: pc.TY_MODE;
BEGIN
  WHILE t.mode = pc.ty_range DO
    t := t.base
  END;
  IF t.mode IN pc.TY_SET{pc.ty_char, pc.ty_boolean, pc.ty_loc} THEN
    md := pc.ty_char;
  ELSE
    t  := t.super_type();
    md := t.mode;
  END;
  IF (m = pc.sb_pre_inc)
  OR (m = pc.sb_post_inc) THEN
    CASE md OF
    | pc.ty_char     : RETURN nm_inc_char;
    | pc.ty_shortint : RETURN nm_inc_shortint;
    | pc.ty_integer  : RETURN nm_inc_integer;
    | pc.ty_longint  : RETURN nm_inc_longint;
    | pc.ty_shortcard: RETURN nm_inc_shortcard;
    | pc.ty_cardinal : RETURN nm_inc_cardinal;
    | pc.ty_longcard : RETURN nm_inc_longcard;
    END;
  ELSE
    CASE md OF
    | pc.ty_char     : RETURN nm_dec_char;
    | pc.ty_shortint : RETURN nm_dec_shortint;
    | pc.ty_integer  : RETURN nm_dec_integer;
    | pc.ty_longint  : RETURN nm_dec_longint;
    | pc.ty_shortcard: RETURN nm_dec_shortcard;
    | pc.ty_cardinal : RETURN nm_dec_cardinal;
    | pc.ty_longcard : RETURN nm_dec_longcard;
    END;
  END;
END IncDec;

----------------------------------------------------------------------------

PROCEDURE Min* ( t: pc.STRUCT ) : INTEGER;
(*
   Returns type's MIN name token
*)
BEGIN
  CASE t.mode OF
  | pc.ty_real     : RETURN nm_min_real;
  | pc.ty_longreal : RETURN nm_min_longreal;
  | pc.ty_ld_real  : RETURN nm_min_ld_real;
  | pc.ty_longint  : RETURN nm_min_longint;
  ELSE               RETURN nm_empty;
  END;
END Min;

----------------------------------------------------------------------------

PROCEDURE Max* ( t: pc.STRUCT ) : INTEGER;
(*
   Returns type's MAX name token
*)
BEGIN
  CASE t.mode OF
  | pc.ty_real     : RETURN nm_max_real;
  | pc.ty_longreal : RETURN nm_max_longreal;
  | pc.ty_ld_real  : RETURN nm_max_ld_real;
  | pc.ty_longint  : RETURN nm_max_longint;
  | pc.ty_longcard : RETURN nm_max_longcard;
  ELSE               RETURN nm_empty;
  END;
END Max;

----------------------------------------------------------------------------

PROCEDURE Abs* (     t: pc.STRUCT        -- type
               ;   chk: BOOLEAN          -- T: check needed
               ; VAR s: ARRAY OF CHAR ); --> proper ABS function name
(*
   Returns abs name (not token here!)
*)
BEGIN
  IF ~(t.mode IN pc.REALs) THEN
    t := t.super_type()
  END;
  IF t.mode IN pc.REALs THEN
    CASE t.mode OF
    | pc.ty_real     : fmt.prn_txt (s(*=>*), "(%s)fabs", x2c[nm_real]^);
    | pc.ty_longreal
    , pc.ty_RR       : COPY ("fabs", s);
    | pc.ty_ld_real  : COPY ("fabsl", s);
    END;
  ELSIF chk THEN
    CASE t.mode OF
    | pc.ty_shortint : COPY (x2c[nm_abs_int8]^, s);
    | pc.ty_integer  : COPY (x2c[nm_abs_int16]^, s);
    | pc.ty_longint
    , pc.ty_ZZ       : COPY (x2c[nm_abs_int32]^, s);
    END;
  ELSE
    CASE t.mode OF
    | pc.ty_shortint
    , pc.ty_integer  : COPY ("abs", s);
    | pc.ty_longint
    , pc.ty_ZZ       : COPY ("labs", s);
    END;
  END;
END Abs;

----------------------------------------------------------------------------

PROCEDURE Exp* ( te,tr: pc.STRUCT          -- exp and result types
               )      :           INTEGER;
(*
   Returns exp name
*)
BEGIN
  IF tr.mode = pc.ty_complex THEN
    IF    te.mode IN pc.WHOLEs THEN RETURN nm_exp_ci;
    ELSIF te.mode IN pc.REALs  THEN RETURN nm_exp_cr;
    ELSE                            ASSERT( FALSE );
    END;
  ELSIF tr.mode IN pc.TY_SET{pc.ty_lcomplex, pc.ty_CC} THEN
    IF    te.mode IN pc.WHOLEs THEN RETURN nm_exp_li;
    ELSIF te.mode IN pc.REALs  THEN RETURN nm_exp_lr;
    ELSE                            ASSERT( FALSE );
    END;
  ELSIF tr.mode IN pc.REALs THEN
    IF    te.mode IN pc.WHOLEs THEN RETURN nm_exp_ri;
    ELSIF te.mode IN pc.REALs  THEN RETURN nm_exp_rr;
    ELSE                            ASSERT( FALSE );
    END;
  ELSE
    ASSERT( FALSE );
  END
END Exp;


(*========================= KWD/NMS FILES READING ===========================*)

PROCEDURE SetName ( nm: INTEGER
                  ; s-: ARRAY OF CHAR);
(*
   Stores text 's' for standard name 'nm' in x2c global list
*)
VAR id: NAME;
BEGIN
  ASSERT( nm <= MAXNM );
  ASSERT( x2c[nm] = NIL );
  NEW (id);
  NEW (id.name, LENGTH (s)+1);
  COPY (s, id.name^);
  x2c[nm] := id.name;
  id := Insert (sys_tree, id);
END SetName;

-------------------------------------------------------------------

TYPE ScanNms = RECORD (xcMain.Scan) END;

-----------------------------------------------------------------------------

PROCEDURE ( VAR s: ScanNms ) Do() : BOOLEAN;
(*
   Scan method to parse actual lines; return FALSE
*)
VAR
  i,n,k: INTEGER;
  c    : CHAR;

BEGIN
  i := 0;
  WHILE CharClass.IsWhiteSpace (s.linebf[i]) DO        -- skip blanks
    INC (i)
  END;
  IF (s.linebf[i] = 0X)        -- EOL or comment
  OR (s.linebf[i] = '%')
  THEN RETURN FALSE
  END;
  n := 0;
  WHILE (s.linebf[i] >= '0') & (s.linebf[i] <= '9') DO -- read number 'n'
    n := n*10+ (ORD (s.linebf[i])-ORD ('0'));
    INC (i);
  END;

  WHILE CharClass.IsWhiteSpace (s.linebf[i]) DO INC (i) END; -- skip blanks

  c := s.linebf[i]; INC (i);                           -- string delimeter
  ASSERT( (c = '"') OR (c = "'") );
  k := 0;                                              -- parse "name"
  WHILE (s.linebf[i] # 0X) & (s.linebf[i] # c) DO      -- copy name
    s.linebf[k] := s.linebf[i];
    INC (i); INC (k);
  END;
  ASSERT( s.linebf[i] = c );
  s.linebf[k] := 0X;
  SetName (n, s.linebf^);             -- Store name in internal tables
  RETURN FALSE;
END Do;

------------------------------------------------------------------------------

PROCEDURE ReadNames;
(*
   Read BE_C generation names from "nms" file.
   Format of each line is : {SP}num{SP}"name" commentEOL
                       or : {SP}%commentEOL
   Preprocessor's conditionals ruled by options set due to XDS conventions
*)
VAR
  fnm: env.String;
  f  : xfs.TextFile;
  sc : ScanNms;
  i  : INTEGER;
BEGIN
  FOR i := 0 TO MAXNM DO   -- Init names table
    x2c[i] := NIL
  END;
  SetName (nm_empty, '');
  xfs.sys.SysLookup ("nms", fnm(*=>*));
  xfs.text.Open (fnm^, FALSE);
  IF xfs.text.file = NIL THEN
    env.errors.Fault (env.null_pos, 425, xfs.text.msg^);
  END;
  f := xfs.text.file (xfs.TextFile);
  IF sc.ReadText (f) THEN END;              -- fail on incorrect file.nms
  f.Close;
END ReadNames;

------------------------------------------------------------------------------

PROCEDURE ReadKeywords;
(*
   Read standard C keywords from "kwd" file.
   Format: words separated by blanks and EOLs; % starts endline comment.
          (or 'word = renamer's)
*)
  ---------------------------------------
  PROCEDURE Key ( fr-,to-: ARRAY OF CHAR );
  (*
     Stores keyword name 'fr' with renamer 'to' in global tree
  *)
  VAR
    id: NAME;
    j : RENM;
  BEGIN
    IF to = "" THEN                  -- No renamer specified
      NEW (id);
      NEW (id.name, LENGTH (fr)+1);
      COPY (fr, id.name^);
      id := Insert (sys_tree, id);
    ELSE                             --
      NEW (j);
      NEW (j.name, LENGTH (fr)+1);
      NEW (j.renm, LENGTH (to)+1);
      COPY (fr, j.name^);
      COPY (to, j.renm^);
      id := Insert (sys_tree, j);
    END;
  END Key;
  ---------------------------------------

VAR
  fnm    : env.String;
  f      : xfs.TextFile;
  str,nm,to: out.STR;
  i,j    : INTEGER;
BEGIN
  xfs.sys.SysLookup ("kwd", fnm(*=>*));
  xfs.text.Open (fnm^, FALSE);
  IF xfs.text.file = NIL THEN
    env.errors.Fault (env.null_pos, 425, xfs.text.msg^);
  END;
  f := xfs.text.file (xfs.TextFile);
  LOOP
    f.ReadString (str(*=>*));
    CASE f.readRes OF
    | xfs.endOfInput:
        EXIT;
    | xfs.allRight:
        i := 0;
        LOOP
          WHILE CharClass.IsWhiteSpace (str[i]) DO          -- skip blanks
            INC (i)
          END;
          IF (str[i] = 0X) OR (str[i] = '%') THEN EXIT END; -- EOL or comment
          j := 0;
          WHILE str[i] > ' ' DO                             -- parse a word 'nm'
            nm[j] := str[i];
            INC (i);
            INC (j);
          END;
          nm[j] := 0X;
          IF j > 0 THEN
            to := "";
            IF str[i] = '=' THEN
              INC (i);
              j := 0;
              WHILE str[i] > ' ' DO            -- parse a renamer 'to'
                to[j] := str[i];
                INC (i);
                INC (j);
              END;
              to[j] := 0X;
            END;
            Key (nm, to);            -- Store word,renamer in internal tables
          END
        END
    ELSE
    END
  END;
  f.Close;
END ReadKeywords;

--------------------------------------------------------------------------

PROCEDURE EnableCount* ( en: BOOLEAN );
(*
   Enable names counting for SL1
*)
BEGIN
  EnCntNames := en;
END EnableCount;

----------------------------------------------------------------------------

PROCEDURE ini*;
(*
*)
VAR str: pc.STRING;
BEGIN
  NamesCnt := 0;
  EnableCount (FALSE);

  sym_list := NIL;
  NewTree (glo_tree(*=>*));
  NEW (glo_tree.index);

  IF sys_tree = NIL THEN
    NewTree (sys_tree(*=>*));
    NEW (sys_tree.index);
    ReadKeywords;       -- Import reserved C keywords from .kwd file
    ReadNames;          -- Import names for generated C realities from .nms file
  END;

  env.config.Equation ("GENIDLEN", str(*=>*));
  IF (str = NIL) OR ~fmt.StrToInt (str^, ident_len(*=>*)) THEN
    ident_len := 30;
  ELSIF ident_len < 6 THEN
    ident_len := 6;
  ELSIF ident_len >= pc.name_size THEN
    ident_len := pc.name_size-1;
  END;
END ini;

----------------------------------------------------------------------------

PROCEDURE exi*;
(*
*)
BEGIN
  glo_tree := NIL;
  sym_list := NIL;
END exi;

----------------------------------------------------------------------------

BEGIN
  ident_len := 8;
  sys_tree  := NIL;
END ccNames.
