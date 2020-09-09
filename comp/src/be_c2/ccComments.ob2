(** Copyright (c) 1991,98 XDS Ltd, Russia. All Rights Reserved. *)
MODULE ccComments;

(* Modifications:
   02/Apr/98 Vit  5.00  Comments inserted; some code/naming clarification done.
                        Files comparison eliminated in OutFullLineComments:
                         comments are assumed to be in the compiled file only.
                        Module param is eliminated from OutFullLineComments:
                         comments are saved for CurMod only
   (XDS versions below, BE - above)
   17/Mar/96 Ned  2.12  Almost all functions are got from BE_C for BNRP.
                        Comments copying is improved.
*)

IMPORT
  pc  := pcK
, cc  := ccK
, out := ccOut
, seg := ccSeg
, env := xiEnv
<* IF DB_TRACE THEN *>
, dbg := pcVisIR
<* END *>
;

CONST
  ctag_ok = 20;

(*============================ GENERAL NOTES ===============================

 pc.comments: H1 -> { C11 ->..-> C1L } ->..-> HN -> {CN1 ->..C-> CNL} -> H1

 A comment represents as a char stream, including both braces ('--' for a line
 comment); pc.EOL chars denote internal line breaks.

 When storing, this stream is split to a consequent .next-linked list of
 fragments of length 256-9 (or less, if last):
 - fragment 1 is a head H that keeps comment's non-null beg and end positions:
     H.pos/end.IsNull() = FALSE
 - some consequent fragments are continuations Ci with null positions:
     Ci.pos/end.IsNull() = TRUE
 - last continuation CL may be found checking for a subsequent head:
     CL.next.pos/end.IsNull() = FALSE     (valid due to cyclicity)
 Sample:
      beg
      |             pc.comments: -> .. ->
    (*ab...cd            H: '(*ab...cd < EOL > ef', beg,      end      ->
    efg...klm     ==>   C1: 'g..klm < EOL > nop..', null_pos, null_pos ->
    nop...xyz*)|        CL: '...xyz*)',             null_pos, null_pos -> ..
               end

 All heads are linked into a sorted AVL-tree CmtTree (via .l, .r)
 and into a sorted linear list CmtList (via .nxt).

 Comments already generated are flagged with ctag_ok and no more processed.

============================================================================*)


(*============= Maintenance of balanced AVL-tree of comments ================
   Balanced trees concept and insertion algorithm are described in
   D.Knuth "Iskusstvo programmirovanija dlya EVM", v.3, p.538-542
*)

TYPE
  AVLTREE = POINTER TO AVLTREE_REC;
  AVLTREE_REC =
    RECORD
      cmt: pc.Comment;  -- comment, sorting key is its position
      l,r: AVLTREE;     -- subtrees
      bal: INTEGER;     -- = height(r)-height(l) = -1,0,+1 (AVL tree property)
      nxt: AVLTREE;     -- link to build a sorted linear order of nodes
    END;

VAR
  CmtTree: AVLTREE;    -- comments linked to an AVL-tree
  CmtList: AVLTREE;    -- comments linked to a sorted list

-------------------------------------------------------------------------

PROCEDURE InsertBalanced (     x : AVLTREE     -- new node to insert
                         ; VAR p : AVLTREE     -- current subtree to insert
                         ; VAR h : BOOLEAN );  -- <TRUE, if tree's height grows
(*
   Insert new comment node 'x' into a balanced AVL-tree 'p'
*)
VAR
  p1,p2 : AVLTREE;
      r : INTEGER;

BEGIN
  IF p = NIL THEN                 -- leaf (node insertion) position
    p    := x;
    x.l  := NIL;
    x.r  := NIL;
    x.bal:= 0;
    h    := TRUE;                 --   height grows
    RETURN;
  END;

  r := p.cmt.pos.cmp (x.cmt.pos);
  IF r > 0 THEN
    InsertBalanced (x, p.l(*=>*), h(*=>*));   -- insert into the left subtree
    IF h THEN                         -- balance:
      CASE p.bal OF
      | +1: p.bal := 0; h := FALSE;   -- right was higher, now equal
      |  0: p.bal := -1;              -- were equal, now left is higher
      | -1:                           -- left became more higher
        p1 := p.l;
        IF p1.bal = -1 THEN               -- 2 cases to check:
                                          --      p(-)         p := p1(0)
          p.l   := p1.r;                  --      /   \            /   \
          p1.r  := p;                     --  p1(-)   [C]  ==>   [A]  p(0)
          p.bal := 0;                     --   /   \   h         h+1  /   \
          p     := p1;                    -- [A]   [B]              [B]   [C]
                                          -- h+1    h                h     h
        ELSE -- 1, 0 is impossible
          p2   := p1.r;                   --      p(-)         p := p2:(0)
          p1.r := p2.l;                   --      /   \             /   \
          p2.l := p1;                     --  p1(+)   [D]  ==> p1(-0)   p(0+)
          p.l  := p2.r;                   --   /   \            /  \      /  \
          p2.r := p;                      -- [A] p2(+-)       [A]  [B]  [C]  [D]
          CASE p2.bal OF                  --  h    /  \        h -:h-1 0: h   h
          | +1: p1.bal := -1; p.bal := 0; --     [B]  [C]          0: h  +:h-1
          | -1: p1.bal := 0;  p.bal := +1;--    +:h-1   h
          |  0: p1.bal := 0;  p.bal := 0; --    -: h   h-1
          END;
          p := p2;
        END;
        p.bal := 0;
        h := FALSE;
      END;
    END

  ELSIF r < 0 THEN
    InsertBalanced (x, p.r(*=>*), h(*=>*));   -- insert into the left subtree
    IF h THEN                    	      -- balance:
      CASE p.bal OF
      | -1: p.bal := 0; h := FALSE;   -- left was higher, now equal
      |  0: p.bal := +1;              -- were equal, now right is higher
      | +1:                           -- right became more higher
        p1 := p.r;
        IF p1.bal = +1 THEN               -- 2 cases to chec k:
                                          --   p(+)           p := p1(0)
          p.r  := p1.l;                   --   /   \              /   \
          p1.l := p;                      -- [A] p1(+)   ==>    p(0)   [C]
          p.bal:= 0;                      --  h   /   \         /   \  h+1
          p    := p1;                     --    [B]   [C]     [A]   [B]
                                          --     h    h+1      h     h
        ELSE -- -1, 0 is impossible
          p2   := p1.l;                   --    p(+)          p := p2:(0)
          p1.l := p2.r;                   --    /   \              /   \
          p2.r := p1;                     --  [A] p1(-)  ==>   p(-0)   p1(0+)
          p.r  := p2.l;                   --   h   /   \        /  \      /  \
          p2.l := p;                      --   p2(+-)  [D]    [A]  [B]  [C]  [D]
          CASE p2.bal OF                  --     /  \   h      h -:h-1 0: h   h
          | +1: p1.bal := 0;  p.bal := -1;--   [B]  [C]          0: h  +:h-1
          | -1: p1.bal := +1; p.bal := 0; -- +:h-1   h
          |  0: p1.bal := 0;  p.bal := 0; -- -: h   h-1
          END;
          p := p2;
        END;
        p.bal := 0;
        h := FALSE;
      END
    END
  ELSE
    ASSERT( FALSE );            -- comments positions matching forbidden
  END;
END InsertBalanced;

-------------------------------------------------------------------------

PROCEDURE InsertTree ( c: pc.Comment );
(*
  Insert comment 'c' into the balanced tree CmtTree
*)
VAR
  h: BOOLEAN;
  p: AVLTREE;

BEGIN
  ASSERT( ~c.pos.IsNull() );
  NEW (p);
  p.cmt := c;
  InsertBalanced (p, CmtTree(*=>*), h(*=>*));
END InsertTree;

-------------------------------------------------------------------------

PROCEDURE SortTree ( x : AVLTREE );
(*
   Used to linearize CmtTree into CmtList via .nxt link
*)
BEGIN
  IF x = NIL THEN RETURN END;
  SortTree (x.r);               -- traversing a sorted tree in infix order
  x.nxt := CmtList;             -- restores linear order
  CmtList := x;
  SortTree (x.l);
END SortTree;

-------------------------------------------------------------------------

PROCEDURE OutComment (  cmt: pc.Comment   -- comment head
                     ; tail: BOOLEAN      -- is tailing or full-line comment
                     ;  lev: INTEGER );   -- current program indentation level
(*
   Generate a comment (head + all continuations) with proper embracing.
   Tailing comment is output with preceding space
*)
VAR
  scpp   : BOOLEAN;   -- T: put single line comment in C++ style
  prev,ch: CHAR;      -- previous & current chars
  last   : BOOLEAN;   -- is current fragment last?
  skip   : LONGINT;   -- number of spaces to skip for indentation
  indent : LONGINT;   -- comment indentation counter
  len    : LONGINT;   -- fragment length
  sp     : BOOLEAN;   -- space needed before */
  f_void : env.String;
  l_void,i: LONGINT;

BEGIN
  INCL (cmt.tags, ctag_ok);                -- flag comment as processed
  prev := 0X;
  cmt.pos.unpack (f_void, l_void, indent); -- indent as of cmt body beg.pos.
--dbg.Trace("\n*** (%d,%d,'%s') ",l_void,indent,cmt.str^); (*!!!*)

  IF tail
   THEN out.WrChrNB (' ')            -- separate tail comment from code
   ELSE out.WrSpc (lev);             -- full line: align to code indent level
  END;
  scpp := cc.op_cpp              -- C++/C-style used for -- comment
        & (LENGTH (cmt.str^) >= 2)
        & (cmt.str[0] = '-')
        & (cmt.str[1] = '-');

    IF scpp
     THEN out.WrStr ('//')
     ELSE out.WrStr ('/*')
    END;
                                         -- Loop by all comment fragments:
  skip := 0;                             --
  i := 2;                                -- skip heading brace or -- in head
  LOOP
    last := ~cmt.next.pos.IsNull();      -- last continuation flag
    len  := LENGTH (cmt.str^);
    sp   := FALSE;
    IF last THEN
      IF (len >= 2) &                    -- braced comment:
         (cmt.str[len-2] = '*') & (cmt.str[len-1] = ')') THEN
        DEC (len, 2);                    -- just cut *)
      ELSIF (len >= 1) &
            (cmt.str[len-1] = pc.EOL) THEN -- line comment:
        DEC (len); sp := TRUE;           -- cut EOL, put space before */ later
      END;
    END;

    WHILE i < len DO                     -- Output fragment body:
      ch := cmt.str[i];
      IF (skip > 0) & (ch = ' ') THEN    -- cut not more than skip leading blanks
        DEC (skip);                      --   for indentation alignment
      ELSE
        IF skip > 0 THEN skip := 0 END;  -- drop skipping, if non-blank
        IF (prev = '/') & (ch = '*')
        OR (prev = '*') & (ch = '/') THEN-- check for /* and */ in Modula cmts
          ch := ' ';
        END;
        IF ch = pc.EOL THEN
          out.WrNL; out.WrSpc (lev);     -- EOL: newline with indent
--dbg.Trace("\\n<%d>",lev); (*!!!*)
          skip := indent;          (* !! difference indent vs. lev may cause *)
        ELSE                       (* mis-alignment in lines *)
          out.WrChrNB (ch);              -- by-char output here
--dbg.Trace("%c",ch); (*!!!*)
        END;
      END;
      prev := ch;
      INC (i);
    END;
    IF sp THEN out.WrChrNB (' ') END;    -- delayed space after line comment

    IF last THEN EXIT END;               -- no more continuations
    i := 0;
    cmt := cmt.next;
  END;

  IF ~scpp THEN out.WrStr ('*/') END;
  IF ~tail THEN out.WrNL END;            -- tail cmt ends with EOL, wl done alrdy
END OutComment;

-------------------------------------------------------------------------

PROCEDURE OutFullLineComments*;
(*
   Generate all residual comments as full-line zero-indented.

   Actually is invoked to generate full-line comments between declaration
   part items. Object-relating and code-relating internal and tail comments
   are supposed to be earlier processed by OutCmtRange (see callers below).
*)
VAR
  s: seg.SEGMENT;
  p: AVLTREE;

BEGIN
  p := CmtList;
  WHILE p # NIL DO                       -- process heads in sorted order
    IF ~(ctag_ok IN p.cmt.tags) THEN     -- if not yet generated
      s := seg.NewSegment (p.cmt.pos, 0);-- put each comment to sep.segment
      seg.EnterSegment (s);
      OutComment (p.cmt, FALSE, 0);      --> generate full-line, zero-indented
      seg.ExitSegment;
    END;
    p := p.nxt;
  END;
END OutFullLineComments;

-------------------------------------------------------------------------

PROCEDURE OutRangeComments ( fr-,to-: pc.TPOS       -- range of positions
                           ;    tail: BOOLEAN       -- tail flag
                           ;     lev: INTEGER );    -- indentation level
(*
   Generate all comments in 'fr..to' range of positions.

   Called to create object-related and code-related comments by special
   callers below; but may also create full-line comments within code.
*)
VAR
  cur: AVLTREE;
  cmt: pc.Comment;
  i  : LONGINT;

BEGIN
  cur := CmtTree;                    -- Find last head before or first after fr
  IF cur = NIL THEN RETURN END;      -- (Search tree, not list for speed)
  LOOP
    ASSERT( ~cur.cmt.end.IsNull() ); -- extra check for non-cont. in tree
    i := fr.cmp (cur.cmt.pos);
    IF i < 0 THEN                    -- fr < cur: move earlier, or stop, if no
      IF cur.l = NIL THEN EXIT END;
      cur := cur.l;
    ELSIF i > 0 THEN                 -- fr > cur: move later, or stop, if no
      IF cur.r = NIL THEN EXIT END;
      cur := cur.r;
    ELSE -- i = 0                    -- fr = cur: stop here
      EXIT
    END;
  END;

  WHILE (cur # NIL)
      & fr.gtr (cur.cmt.pos)         -- Skip to actually first after fr
  DO
    cur := cur.nxt;
  END;

  LOOP                                   -- Generate until first after to or end
    IF cur = NIL THEN RETURN END;        -- actual end of list
    cmt := cur.cmt;
    ASSERT( ~cmt.pos.IsNull() );         -- extra check for non-cont. in tree
    IF cmt.pos.gtr (to) THEN RETURN END; -- 'to' passed...
    IF ~(ctag_ok IN cmt.tags) THEN
      OutComment (cmt, tail, lev);       --> Generate with given rulers
    END;
    cur := cur.nxt;
  END;
END OutRangeComments;


(*== PROCESSING COMMENTS BINDED TO DATA STRUCTURES AND COMPOUND STATEMENTS ==*)

TYPE
  POSTACK = POINTER TO POSTACK_REC;
  POSTACK_REC =         (* Stack structure to keep positions save levels *)
    RECORD
      pos : pc.TPOS;
      nxt : POSTACK;
    END;
VAR
  CurPos: POSTACK;      -- top (current position) of positions stack

-----------------------------------------------------------------------------

PROCEDURE OutFirstComment* ( to-: pc.TPOS     -- current position
                           ; df : INTEGER     -- cur.line decrement # to make fr fr
                           ; lev: INTEGER );  -- indentation level
(*
   Generate all comments in range '<beg of cur.line-df>..to' as full-line
   Push current positions save level and open new one
*)
VAR
  fnm: env.String;
  l,c: LONGINT;
  fr : pc.TPOS;
  cp : POSTACK;

BEGIN
  IF ~to.IsNull() THEN
    to.unpack (fnm, l, c);
    IF l > df
     THEN l := l-df
     ELSE l := 0
    END;
    fr.pack (fnm^, l, 0);
    OutRangeComments (fr, to, FALSE, lev); --> Process range
  END;

  NEW (cp);                                -- push 'to' as cur.pos. of new level
  cp.nxt := CurPos;
  CurPos := cp;
  cp.pos := to;
END OutFirstComment;

-------------------------------------------------------------------------

PROCEDURE OutNextComment* (  to: pc.TPOS     -- current position
                          ; lev: INTEGER );  -- indentation level
(*
   Generate all comments in range 'CurPos..to' as full-line
*)
BEGIN
  IF (CurPos = NIL) OR CurPos.pos.IsNull()   -- if save level not opened
  OR to.IsNull()                             -- or null position
  OR ~to.gtr (CurPos.pos)                    -- or invalid range
  THEN RETURN END;                           -- .. do nothing

  OutRangeComments (CurPos.pos, to, FALSE, lev); --> Process range
  CurPos.pos := to;                          -- save 'to' as new range beg
END OutNextComment;

-------------------------------------------------------------------------

PROCEDURE OutLastComment* (        fr: pc.TPOS     -- current position
                          ;       lev: INTEGER     -- indentation level
                          ; next := NIL: pc.OBJECT); -- next object
(*
   Generate all comments in range 'fr.. < next line,col8>/next.pos' as full-line
   Pop previous positions save level
*)
VAR
  fnm: env.String;
  l,c: LONGINT;
  to : pc.TPOS;

BEGIN
  IF CurPos = NIL THEN RETURN END;
  IF ~fr.IsNull() & ~CurPos.pos.IsNull() THEN
    IF next = NIL THEN               -- no next object specified:
      fr.unpack (fnm, l, c);         --   set 'to' to next line beg+8
      to.pack (fnm^, l+1, 8);        --   cant be more to preserve tail cmt
    ELSE                             -- next object specified:
      to := next.pos;                --   dont overrun its position
    END;
    IF to.gtr (CurPos.pos) THEN      -- if range is valid:
      OutRangeComments (CurPos.pos, to, FALSE, lev); --> Process range
    END
  END;
  CurPos := CurPos.nxt;              -- pop previous positions level
END OutLastComment;

-------------------------------------------------------------------------

PROCEDURE OutTailComment* (fr-: pc.TPOS);
(*
   Generate all comments in range 'fr.. < EOL>' as tailing
*)
VAR fnm: env.String;
    l,c: LONGINT;
    to : pc.TPOS;

BEGIN
  IF fr.IsNull() THEN RETURN END;
  fr.unpack (fnm, l, c);                -- set 'to' as line end
  to.pack (fnm^, l, MAX (INTEGER));
  OutRangeComments (fr, to, TRUE, 0);   --> Process range, tailing, lev = 0
END OutTailComment;

-------------------------------------------------------------------------

PROCEDURE ini*;
(*
   Start comments generation: build CmtTree and CmdList.
 - Entry: 'pc.comments' -> cyclic list of comments (some last cont-n actually)
 - Exit: heads linked into CmtTree and CmtList for actual generation.
*)
VAR
  curC,endC: pc.Comment;

BEGIN
  CurPos := NIL;
  IF pc.comments = NIL THEN RETURN END;

  CmtTree := NIL;           -- Build CmtTree:
  endC := pc.comments;
  WHILE endC.pos.IsNull() DO       -- skip to the 1st head,
    endC := endC.next;             --   as pc.comments points to last cmt end
  END;

  curC := endC;
  REPEAT                           -- loop until endC encounters again (cycle!)
    IF ~(ctag_ok IN curC.tags) THEN
      InsertTree (curC);           -- insert cmt head, if not yet processed
    END;
    curC := curC.next;
    WHILE curC.pos.IsNull() DO
      curC := curC.next;           -- skip continuations until next head
    END;
  UNTIL curC = endC;

  CmtList := NIL;           -- Sort CmtTree to CmtList
  SortTree (CmtTree);
END ini;

-------------------------------------------------------------------------

PROCEDURE exi*;
(*
   Finish comments generation: just delete superstructures built here
*)
BEGIN
  CmtTree := NIL;
  CmtList := NIL;
  CurPos  := NIL;
END exi;

END ccComments.
