(** Copyright (c) 1991,98 XDS Ltd, Russia. All Rights Reserved. *)
MODULE ccOut;
(** Low level code output support (internal organization of a segment) *)

(* Modifications:
   09/Jun/98 Vit 5.06  WriteFile nomore returns full file name.
   09/Apr/98 Vit 5.00  Introduced comments, mnemonics, rearrangements.
               POS, enter_statement, pack_statement eliminated
               Stack implemented dynamically
               Renamed from ccL
*)
IMPORT
    SYSTEM
  , pc  := pcK
  , cc  := ccK
<* IF DB_TRACE THEN *>
  , dbg := pcVisIR
<* END *>
  , xfs := xiFiles
  , env := xiEnv
  , xcStr
  , FormOut
  ;

(*===========================================================================

  Overall generated code is organized into a tree of segments by module ccSeg.
  This module deals with a single current segment, created at the moment.

  A SEGMent is a double-linked list of STRings.
  A STRing is a pack of code LINEs, appended together for memory saving.
(?) Only whole lines wrap to new STR when place exceed!

============================================================================*)

(* Line characteristics *)
CONST
  MINLINELEN = 30;     -- minimal generated LINE length
  DEFLINELEN = 77;     -- default
  MAXLINELEN = 255;    -- maximal (in LINENO+ mode)

  MININDENT  = 0;      -- minimal indentation size
  DEFINDENT  = 3;      -- default
  MAXINDENT  = 51;     -- maximal

  STR_SIZE   = 4096;   -- (packed) STR length

TYPE
  STR* = ARRAY 256 OF CHAR;

VAR
  MaxLineLen : LONGINT;   -- defined maximal line length for auto-Wrap
  IndentStep : LONGINT;   -- defined indentation step

  NLstr : ARRAY 8 OF CHAR;       -- newline sequence '\n'
  WPstr : ARRAY 8 OF CHAR;       -- C line wrap sequence '\\\n'
  LFchr : CHAR;                  -- last char in line, usually 0AX
  Wraps : ARRAY 256 OF SHORTINT; -- wrap priorities by chars

(* SEGMent structure *)
TYPE
  SEGM* = POINTER TO
    RECORD
       str : pc.STRING;  -- STR: pack of LINEs
      cpos : INTEGER;    -- last LINE length = cur.char relative position
      next : SEGM;       -- links
      prev : SEGM;
    END;

VAR
  CurSegm : SEGM;        -- currently generated SEGMent (completed STRings)
  CurStr  : pc.STRING;   -- current incomplete STRing: pack of LINEs
             -- CurStr info:
  Lpos : INTEGER;        --  STRing length = cur.char abs.position
  Cpos*: INTEGER;        --  last LINE length = cur.char relative position
  Wpos : INTEGER;        --  +1 = starting position to wrap to a new STR
  Wlev : INTEGER;        --  current wrap priority
  W_en : BOOLEAN;        --  enable automatic LF
  Ind  : INTEGER;        -- statement indentation length
  Ipos : INTEGER;        -- wrap indentation length
  Fresh: BOOLEAN;        -- fresh line: T to suppress leading blanks

(* Stack of currently generated segments *)
TYPE
  STK = POINTER TO
    RECORD
       str : pc.STRING;  -- current STRing info save positions
      lpos : INTEGER;    --
      cpos : INTEGER;    --
      wpos : INTEGER;    --
      wlev : INTEGER;    --
      w_en : BOOLEAN;    --
      segm : SEGM;       -- current segment save
      prev : STK;        -- link
    END;

VAR
  Stack : STK;           -- Sack of current segments


<* IF DB_TRACE THEN *>
  EnDebug* : BOOLEAN;    -- Enable tracing BE routines
  EnCvis  : BOOLEAN;     -- Enable tracing generated C code
<* END *>


(*========================= STACK OF CURRENT SEGMENTS ======================*)

PROCEDURE Push* ();
(*
   Push current SEGMent/STRing and make new current empty.
*)
VAR s: STK;
BEGIN
  s := Stack;
  NEW (Stack);
  Stack.prev := s;
  Stack.str  := CurStr;
  Stack.lpos := Lpos;
  Stack.cpos := Cpos;
  Stack.wpos := Wpos;
  Stack.wlev := Wlev;
  Stack.w_en := W_en;
  Stack.segm := CurSegm;

  CurSegm := NIL;
  Lpos := 0;
  Cpos := 0;
  Wpos := 0;
  Wlev := -1;
  W_en := TRUE;
  NEW (CurStr, STR_SIZE);
END Push;

-----------------------------------------------------------------------------

PROCEDURE Pop*;
(*
   Pop current segment
*)
BEGIN
  ASSERT( CurStr # NIL );
  ASSERT( Stack # NIL );
  ASSERT( CurSegm = NIL );
  ASSERT( Lpos = 0 );

  CurSegm := Stack.segm;
  CurStr  := Stack.str;
  Lpos    := Stack.lpos;
  Cpos    := Stack.cpos;
  Wpos    := Stack.wpos;
  Wlev    := Stack.wlev;
  W_en    := Stack.w_en;

  Stack   := Stack.prev;
END Pop;


(*==================== ACCUMULATION OF CURRENT SEGMENT ======================*)

PROCEDURE AppendDbL ( VAR root: SEGM
                    ;     segm: SEGM );
(*
   Append dbl-linked 'segm' to dbl-linked 'root'.
*)
VAR re,se: SEGM;

BEGIN
  IF root = NIL THEN
    root := segm;
  ELSIF segm.next = segm THEN
    segm.next := root;
    segm.prev := root.prev;
    segm.next.prev := segm;
    segm.prev.next := segm;
  ELSE
    se := segm.prev;
    re := root.prev;
    segm.prev := re;
    se.next   := root;
    re.next   := segm;
    root.prev := se;
  END;
END AppendDbL;

------------------------------------------------------------------------------

PROCEDURE FlushStr;
(*
   Close current segment: append curS.STRing to cur.SEGMent (flush extra memory)
*)
VAR
  i   : LONGINT;
  str : pc.STRING;
  segm: SEGM;
BEGIN
  IF Lpos = 0 THEN RETURN END; -- empty STR

  NEW (str, Lpos);             -- copy CurStr to a buffer of actual size
  FOR i := 0 TO Lpos-1 DO      -- (economize memory, not time!)
    str[i] := CurStr[i];
  END;

  NEW (segm);                  -- append flusged cur.STRing to CurSegm
  segm.str  := str;
  segm.cpos := Cpos;
  segm.next := segm;
  segm.prev := segm;
  AppendDbL (CurSegm(*=>*), segm);

  Lpos := 0;                   -- make empty line
  Wlev := -1;
  Fresh:= TRUE;
END FlushStr;

------------------------------------------------------------------------------

PROCEDURE WriteSegm* ( segm: SEGM );
(*
   Close cur.SEGMent and append 'segm' to it
*)
BEGIN
  IF segm = NIL THEN RETURN END;
  FlushStr;
  AppendDbL (CurSegm(*=>*), segm);
  Cpos := segm.prev.cpos;   -- set rel.position as of last column in cur.segm.
  W_en := TRUE;             -- enable wrapping
END WriteSegm;

------------------------------------------------------------------------------

PROCEDURE GetSegment* ( VAR segm: SEGM );
(*
   Close cur.SEGMent and get it out; clear new cur.SEGMent.
*)
BEGIN
  FlushStr;
  segm := CurSegm;
  CurSegm := NIL;
  Cpos := 0;                -- empty line
  W_en := TRUE;             -- enable wrapping
END GetSegment;

------------------------------------------------------------------------------

PROCEDURE AppendSegm* ( VAR segm: SEGM );
(*
  Close and append cur.segment to 'segm'.
*)
VAR cur: SEGM;

BEGIN
  GetSegment (cur);
  IF cur # NIL THEN
    AppendDbL (segm(*=>*), cur);
  END;
END AppendSegm;

------------------------------------------------------------------------------

PROCEDURE GetString* ( VAR d: ARRAY OF CHAR );
(*
  Pack current segment into one line.
  Used to output array length expression
  !!! no bound checks
*)
VAR l,root: SEGM;
    s: pc.STRING;
    i,j: INTEGER;
BEGIN
  i := 0;
  GetSegment (root);
  l := root;
  REPEAT
    s := l.str;
    j := 0;
    WHILE j < LEN (s^) DO
      ASSERT (i < LEN (d));
      IF s[j] # ' ' THEN
        d[i] := s[j];
        INC (i);
      END;
      INC (j);
    END;
    l := l.next;
  UNTIL l = root;
  d[i] := 0X;
END GetString;


(*========================= WRITING TO CURRENT LINE =========================*)

PROCEDURE WrapMark* ( lev: INTEGER );
(*
   Marks current position as wrap marker with priority 'lev'
*)
BEGIN
  IF (*W_en & *)(lev >= Wlev) THEN  --  if wrap priority is same or higher
    Wlev := lev;                -- save priority
    Wpos := Lpos-1;             -- save last wrap position
  END;
END WrapMark;

------------------------------------------------------------------------------

PROCEDURE WrChrNB* ( c: CHAR );
(*
   Put character of a symbolic literal.
   Used instead WrChr0 when line breaks are forbidden.
*)
BEGIN
  ASSERT( c # LFchr );          -- LFs are forbidden

--  IF Fresh & (c = ' ') THEN RETURN END; -- suppress leading blanks
  Fresh := FALSE;

<* IF DB_TRACE THEN *>
  IF EnCvis THEN dbg.CodeTrace ('%c', c) END; -- Gen.code trace on: echo char
<* END *>

  IF Lpos >= STR_SIZE THEN
    FlushStr;
  END;
  CurStr[Lpos] := c;
  INC (Lpos);
  INC (Cpos);
END WrChrNB;

------------------------------------------------------------------------------

PROCEDURE WrChr0 ( c: CHAR );
(*
   Put char without checking of current position
*)

BEGIN
  IF Fresh & (c = ' ') THEN RETURN END; -- suppress leading blanks
  Fresh := FALSE;

<* IF DB_TRACE THEN *>
  IF EnCvis THEN dbg.CodeTrace ('%c', c) END; -- Gen.code trace on: echo char
<* END *>

  IF Lpos >= STR_SIZE THEN
    FlushStr;
  END;
  CurStr[Lpos] := c;
  INC (Lpos);

  IF c = LFchr THEN
    Cpos := 0;
    Wlev := -1;
    IF Lpos >= (STR_SIZE*3 DIV 4) THEN
      FlushStr;
    END;
  ELSE
    IF (Wlev < 100) & ((c = '"') OR (c = "'")) THEN  --$$ Megos
      IF c = '"'
       THEN WrapMark (100);
       ELSE WrapMark (101);
      END;
      INC (Cpos);
      RETURN;
    END;                        --$$

    INC (Cpos);

    IF ((Wlev = 100) & (c = '"'))               --$$ Megos
    OR ((Wlev = 101) & (c = "'")) THEN
      Wlev := Wraps[ORD(c)];
      Wpos := Lpos-1;
      RETURN;
    END;                                        --$$
    WrapMark (Wraps[ORD(c)]);
  END;
END WrChr0;

------------------------------------------------------------------------------

PROCEDURE WrStr0 ( s-: ARRAY OF CHAR );
(*
   Put string without checking of current position
*)
VAR i: INTEGER;

BEGIN
  i := 0;
  WHILE s[i] # 0X DO
    WrChr0 (s[i]);
    INC (i);
  END;
END WrStr0;

------------------------------------------------------------------------------

PROCEDURE WrNL*;
(*
   Write newline
*)
BEGIN
  WrStr0 (NLstr);
END WrNL;

------------------------------------------------------------------------------

PROCEDURE DropIndent*;
(*
   Set wrap indentation to current statement indentation
*)
BEGIN
  Ipos := Ind;
END DropIndent;

------------------------------------------------------------------------------

PROCEDURE WrapStr;
(*
   Put line break: wrap a line in proper Wpos wrap position or
   output wrap symbol.
*)
VAR
  i,j,l: INTEGER;
  buf  : ARRAY MAXLINELEN+1 OF CHAR;
BEGIN
  IF ~W_en THEN         -- wrapping forbidden:
    WrStr0 (WPstr);     --  write a '\' break, continue ignoring indents
    RETURN;
  ELSIF (Wlev < 0)      -- no proper wrap char, continue
  OR (Wpos-(Lpos-Cpos) < MINLINELEN) THEN -- dont leave less than min.length
    RETURN;
  END;

  IF (Wlev < 20) & (Wlev >= 10) THEN   -- wrap at statement pos: stmt indent
    DropIndent;
  END;

  i := Wpos+1;                         -- i..l - wrapping interval in CurStr
  l := Lpos;
  WHILE (i <= l) & (CurStr[i] = ' ') DO -- skip blanks after wrap pos.
    INC (i)
  END;
  IF l-i >= LEN(buf) THEN RETURN END;  -- too long tail: do nothing

  j := 0;                              -- copy tail to buf
  WHILE (i < l) DO
    buf[j] := CurStr[i];
    INC (i);
    INC (j);
  END;
  buf[j] := 0X;

  Lpos := Wpos+1;                          -- back to wrap pos to rewrite line:
  WrNL;                                    --  line break
  Fresh := FALSE;
  FOR i := 0 TO Ipos-1 DO WrChrNB(' ') END;--  wrap indent
  Fresh := TRUE;
  WrStr0 (buf);                            --  tail
END WrapStr;

------------------------------------------------------------------------------

PROCEDURE DisableWrap* ( VAR prev: BOOLEAN );
(*
   Disables automatic wrapping; save previous status.
*)
BEGIN
  prev := W_en;
  W_en := FALSE;
END DisableWrap;

------------------------------------------------------------------------------

PROCEDURE RestoreWrap* ( prev: BOOLEAN );
(*
   Restores previous wrap status
*)
BEGIN
  W_en := prev;
  IF Cpos >= MaxLineLen THEN WrapStr END;   --$$ Megos
END RestoreWrap;

------------------------------------------------------------------------------

PROCEDURE SaveIndent*;
(*
   Save current position as indentation for possible wrapping
*)
BEGIN
  IF Cpos < MAXINDENT
   THEN Ipos := Cpos;
   ELSE Ipos := Ind + SHORT(IndentStep);
  END;
END SaveIndent;

------------------------------------------------------------------------------

PROCEDURE WrChr* ( c: CHAR );
(*
   Put char with wrap check.
*)
BEGIN
  IF Cpos >= MaxLineLen THEN
    WrapStr;
  END;
  WrChr0 (c);
END WrChr;

-----------------------------------------------------------------------------

PROCEDURE WrStr* ( s-: ARRAY OF CHAR );
(*
   Put string with wrap check
*)
VAR i: LONGINT;

BEGIN
  i := 0;
  WHILE s[i] # 0X DO
    IF Cpos >= MaxLineLen THEN
      WrapStr;
    END;
    WrChr0 (s[i]);
    INC (i);
  END;
END WrStr;

------------------------------------------------------------------------------

PROCEDURE WrInt* ( n: LONGINT );
(*
   Write integer
*)
BEGIN
  IF n < 0 THEN
    WrChr ('-');
    n := ABS(n);
  END;
  IF n >= 10 THEN
    WrInt (n DIV 10);
  END;
  WrChr (CHR (ORD ('0')+n MOD 10));
END WrInt;

------------------------------------------------------------------------------

PROCEDURE WrSpc* ( n: INTEGER );
(*
   Put indentation spaces
*)
BEGIN
  ASSERT( Cpos = 0 );
  n := n * SHORT(IndentStep);
  IF n > 51 THEN n := 51 END;
  Ind  := n;
  Ipos := n;
  IF Lpos+n >= STR_SIZE THEN
    FlushStr;
  END;
  INC (Cpos, n);
  INC (n, Lpos);
  WHILE Lpos < n DO
    CurStr[Lpos] := ' ';
    INC (Lpos);
  END;
  Fresh := TRUE;
END WrSpc;

------------------------------------------------------------------------------
<*+ WOFF301 *>

PROCEDURE [1] WrFmt_proc(  x: SYSTEM.ADDRESS
                        ; s-: ARRAY OF CHAR
                        ;  l: LONGINT );
(*
   Char output procedure for WrFmt
*)
VAR i: INTEGER;

BEGIN
  FOR i := 0 TO VAL (INTEGER, l-1) DO
    IF Cpos >= MaxLineLen THEN
      WrapStr;
    END;
    WrChr0 (s[i]);
  END;
END WrFmt_proc;

<*- WOFF301 *>

-----------------------------------------------------------------------------
PROCEDURE WrFmt* (   fmt-: ARRAY OF CHAR
                 ; SEQ x : SYSTEM.BYTE );
(*
   Write with WrFmt_proc sequence 'x' by format 'fmt'
*)
BEGIN
  FormOut.format (NIL,
                  WrFmt_proc,
                  fmt,
                  FormOut.default,
                  SYSTEM.ADR (x),
                  SIZE (x) );
END WrFmt;

------------------------------------------------------------------------------

PROCEDURE StrFmt* ( VAR buf : ARRAY OF CHAR
                  ;     fmt-: ARRAY OF CHAR
                  ;   SEQ x : SYSTEM.BYTE );
BEGIN
  xcStr.prn_bin (buf(*=>*), fmt, x);
END StrFmt;

------------------------------------------------------------------------------

PROCEDURE WriteFile* (   fname: ARRAY OF CHAR
                     ;     obj: BOOLEAN
                 ; srctime: xfs.Time
                     );
(*
   Write all segms accumulated in CurSegm into file 'fname'
*)
VAR
  file: xfs.TextFile;
  fn  : xfs.String;
  err : xfs.String;
  ext : xfs.String;
  tm  : xfs.Time;
  was : BOOLEAN;
  p   : SEGM;
BEGIN
  ASSERT( Stack = NIL );

  IF obj
   THEN ext := pc.code.code_ext;
   ELSE ext := pc.code.head_ext;
  END;
  xfs.sys.Create ('', fname, ext^, fn(*=>*));
  xfs.sys.UseFirst (fn^, fn(*=>*));
  IF obj THEN
    was := FALSE;
    tm := MIN (xfs.Time);
  ELSE
    xfs.sys.ModifyTime (fn^, tm(*=>*), was(*=>*));
  END;
  xfs.text.Open (fn^, TRUE);
  IF xfs.text.file = NIL THEN
    env.errors.Fault (env.null_pos, 424, xfs.text.msg^); -- "file create error"
  ELSE
    file := xfs.text.file (xfs.TextFile);
    FlushStr;
    IF CurSegm # NIL THEN
      p := CurSegm;
      REPEAT
        file.WriteString (p.str^, LEN (p.str^));
    p := p.next;
      UNTIL p = CurSegm;
    END;
    file.CloseNew (env.errors.err_cnt = 0,
                   ~obj & was & (tm > srctime),
                   TRUE,
                   err(*=>*));
    IF err # NIL THEN
      env.errors.Fault (env.null_pos, 448, err^); -- "file write error"
    END;
    IF obj & (env.errors.err_cnt = 0) THEN
      env.info.code_file := fn;
    END;
  END;
END WriteFile;

------------------------------------------------------------------------------

PROCEDURE exi*;
(*
*)
BEGIN
  Stack   := NIL;
  CurSegm := NIL;
  NEW (CurStr, STR_SIZE);
  Lpos := 0;
  Cpos := 0;
  Wpos := -1;
  Wlev := -1;
  W_en := TRUE;
  Fresh:= TRUE;
END exi;

------------------------------------------------------------------------------

PROCEDURE ini*;
(*
*)
VAR par: env.String;

BEGIN
<* IF DB_TRACE THEN *>
  EnDebug := env.config.Option ('C_TRACE');
  EnCvis  := EnDebug OR env.config.Option ('C_VIS');
  dbg.Ini;
  IF EnDebug THEN dbg.Trace ("\n====== BE-C code generation ======") END;
<* END *>

  exi;  -- Initialize current variables

  StrFmt (NLstr, '\n');                   -- define actual line breaks
  StrFmt (WPstr, '\\\n');                 -- define actual wrapper
  LFchr := NLstr[LENGTH (NLstr)-1];       -- define actual LF char

  env.config.Equation ('GENINDENT', par(*=>*)); -- Set indent size:
  IF (par = NIL)
  OR ~xcStr.StrToInt (par^, IndentStep(*=>*))   -- given
  THEN
    IndentStep := DEFINDENT;              -- default, if no or invalid
  ELSIF IndentStep < MININDENT THEN
    IndentStep := MININDENT;              -- min
  ELSIF IndentStep > MAXINDENT THEN
    IndentStep := MAXINDENT;              -- max
  END;

  IF cc.op_lineno THEN              -- Set max.line length for auto-wrap:
    MaxLineLen := MAXLINELEN;       -- debug mode overruns GENWIDTH!
  ELSE
    env.config.Equation ('GENWIDTH', par(*=>*)); -- else look for given
    IF (par = NIL)
    OR ~xcStr.StrToInt (par^, MaxLineLen (*=>*)) THEN
      MaxLineLen := DEFLINELEN;                  -- not specified or invalid
    ELSIF MaxLineLen < MINLINELEN THEN
      MaxLineLen := MINLINELEN;                  -- min
    ELSIF MaxLineLen > MAXLINELEN THEN
      MaxLineLen := MAXLINELEN;                  -- max
    END;
  END;
END ini;

------------------------------------------------------------------------------

VAR i: INTEGER;

BEGIN
  exi;     (* what for here?!*)

  FOR i := 0 TO 255 DO Wraps[i] := -2 END; -- set wrap preferences:
  Wraps[ORD ('}')] := 15;                  -- wrapping starts after symbol
  Wraps[ORD (';')] := 10;                  -- with higher priority
  Wraps[ORD (',')] := 7;                   -- -1 is undefined value, so
  Wraps[ORD (')')] := 5;                   -- -2 means: never wrap on this char
  Wraps[ORD (']')] := 4;
  Wraps[ORD ('"')] := 2;
  Wraps[ORD ("'")] := 1;
  Wraps[ORD (' ')] := 0;
END ccOut.
