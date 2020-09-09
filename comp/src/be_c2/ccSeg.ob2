(** Copyright (c) 1995,96 XDS Ltd, Russia. All Rights Reserved. *)
MODULE ccSeg;
(** Segments structure organization *)

(* Modifications:
   05/Apr/98 Vit  5.00  Comments inserted; some code/naming clarification done.
   (XDS versions below, BE - above)
   19-Jun-96 Ned        "exit" is optimized.
*)

IMPORT
  out := ccOut,
  xfs := xiFiles,
  env := xiEnv,
  dbg := pcVisIR
  ;

(*============================================================================
  Ever 'entered' segments store as a position/dependencies-ordered list
  (Segments) that match generation order.
  Def-use dependencies between segments are maintained via import list.
  Imported segments may not be entered themselves (do they?) and thus included
  into Segments, so for final code generation Segments is postfixly traversed
  by import links.
  Segments entering/exiting/stacking mechanism
=============================================================================*)

CONST
  stag_written = 0;
  stag_writing = 1;
  stag_entered = 2;

(*==========================================================================*)

TYPE
  SEGMENT*= POINTER TO SEGMENT_REC;
  SEGLIST = POINTER TO SEGLIST_REC;

  SEGMENT_REC =           (* Segment description *)
    RECORD
       pos- : env.TPOS;   -- start position to sort segments
       segm : out.SEGM;   -- segment text
       next : SEGMENT;    -- link to support stacking
     import : SEGLIST;    -- imported segments (that must be created before)
       tags : SET;        -- segment tags
         pr : INTEGER;    -- priority (for segments with equal position)
    END;

  SEGLIST_REC =           (* Segments list *)
    RECORD
        seg : SEGMENT;
       next : SEGLIST;
    END;

VAR
  Segments   : SEGMENT;  -- total segments list
  CurSegment-: SEGMENT;  -- stack of current (entered) segments; top is cur.
  LastSegment: SEGMENT;  -- last segment added to Segments (used for speedup)

-----------------------------------------------------------------------------

PROCEDURE NewSegment* ( ps: env.TPOS
                      ; pr: INTEGER         -- priority (for segs with same pos)
		      )   :         SEGMENT;
(*
   Create new segment with given position and priority
*)
VAR s: SEGMENT;

BEGIN
  NEW(s);
  s.pos   := ps;
  s.segm  := NIL;
  s.next  := NIL;
  s.import := NIL;
  s.tags  := {};
  s.pr    := pr;
  RETURN s;
END NewSegment;

-----------------------------------------------------------------------------

PROCEDURE EnterSegment* ( s: SEGMENT );
(*
   Open 's' for current processing:
   stack previous segment; make
*)
BEGIN
  ASSERT( ~(stag_entered IN s.tags) );  -- no repetitious entries
  INCL(s.tags,stag_entered);            -- mark as entered

  out.Push;                             -- push prev.current out.SEGM

  s.next := CurSegment;                 -- make 's' current segment
  CurSegment := s;                      -- push prev. segment
END EnterSegment;

-----------------------------------------------------------------------------

PROCEDURE ExitSegment*;
(*
   Exit segment from current processing:
   close and insert it in proper position order into Segments;
   restore previous entered segment
*)
  -------------------------------------------------
  PROCEDURE Lss (x,y: SEGMENT): BOOLEAN; -- T if x is earlier than y
  VAR i: INTEGER;
  BEGIN
    IF y = NIL THEN RETURN TRUE END;
    i := x.pos.cmp(y.pos);               -- positions are primary
    RETURN
      (i < 0) OR
      (i = 0) & (x.pr <= y.pr);          -- cmp priority, if positions equal
  END Lss;
  -------------------------------------------------

VAR s,l: SEGMENT;

BEGIN
  s := CurSegment;             -- restore previous segment from stack
  CurSegment := s.next;

  out.GetSegment(s.segm);      -- close current SEGM, save to s, pop SEGM
  out.Pop;
                              (* Insert s into ordered list Segments *)
  IF Lss(s,Segments) THEN      -- insert at the beginning
    s.next := Segments;
    Segments := s;
    RETURN;
  ELSIF (LastSegment = NIL)    -- use LastSegment (last segment added)
  OR Lss(s,LastSegment)        -- to speed-up insertion (usually immed.after)
   THEN l := Segments;         -- 'l' is search starting position
   ELSE l := LastSegment;
  END;
  LastSegment := s;            -- save this segment as Last
  LOOP                         -- loop of insertion
    IF Lss(s,l.next) THEN
      s.next := l.next;
      l.next := s;
      RETURN;
    END;
    l := l.next;
  END;
END ExitSegment;

-----------------------------------------------------------------------------

PROCEDURE ImportSegment* (s: SEGMENT);
(*
   Put 's' into an import list of CurSegment
*)
VAR l: SEGLIST;

BEGIN
  IF (s = NIL)                          -- dont import NIL
  OR (s = CurSegment) THEN RETURN END;  -- or current itself

  l := CurSegment.import;               -- seek if already imported
  WHILE l # NIL DO
    IF l.seg = s THEN RETURN END;
    l := l.next;
  END;

  NEW(l);                               -- insert into import list
  l.seg  := s;
  l.next := CurSegment.import;
  CurSegment.import := l;
END ImportSegment;

-----------------------------------------------------------------------------

PROCEDURE OutFileName (      pos : env.TPOS
                      ; VAR file : xfs.String );
(*
*)
VAR f: env.String;
    i,j: LONGINT;
    dir,fnm,ext: xfs.String;
BEGIN
  (* SC 6.0 fails if this line is before includes! *)
  IF pos.IsNull() THEN RETURN END;
  pos.unpack(f,i,j);
  IF file # f THEN
    file := f;
    IF ~env.config.Option("GENFULLFNAME") THEN
      xfs.sys.Get (f^, dir, fnm, ext);
      xfs.sys.Create ("", fnm^, ext^, f(*=>*));
    END;
    out.WrFmt ('#line %d "%s"', i+1, f^);
  ELSE
    out.WrFmt ("#line %d", i+1);
  END;
  out.WrNL;
END OutFileName;

-----------------------------------------------------------------------------

PROCEDURE WriteSegments* (lineno: BOOLEAN);
(*
   Output Segments list:
   Properly insert sorted .segm's in out.SEGMs list (import-dependent first)
*)
VAR fnm: env.String;

  PROCEDURE WriteSegment (s: SEGMENT);
  (*
    Recursive output of 's' and all imported segments in postfix order
  *)
  VAR l: SEGLIST;
  BEGIN
    IF stag_written IN s.tags THEN RETURN END; -- already written

    IF stag_writing IN s.tags THEN     -- if currently writing:
      env.errors.Warning(s.pos,353);   -- "dependence cycle in C code"
      RETURN;
    END;
    INCL(s.tags,stag_writing);         -- mark

    l := s.import;                     -- write all imported first
    WHILE l # NIL DO
      WriteSegment(l.seg);
      l := l.next;
    END;

    IF lineno THEN OutFileName (s.pos,fnm(*=>*)) END; -- write lineno for debug
    out.WriteSegm(s.segm);             -- write this segment
    EXCL(s.tags,stag_writing);         -- adjust marks
    INCL(s.tags,stag_written);
  END WriteSegment;
  -------------------------------------------------

BEGIN
  fnm := NIL;
  WHILE Segments # NIL DO              -- output Segments list
    WriteSegment (Segments);
    Segments := Segments.next;
  END;
END WriteSegments;

-----------------------------------------------------------------------------

PROCEDURE exi*;
(*
*)
BEGIN
  CurSegment  := NIL;
  Segments    := NIL;
  LastSegment := NIL;
END exi;

PROCEDURE ini*;
(*
*)
BEGIN
  exi;
END ini;

END ccSeg.
