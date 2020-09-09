<* +MAIN *>
MODULE tc2shell;

IMPORT
  FRead
 ,DLG:= WinDlg
 ,P  := Printf
 ,PE := ProgEnv
 ,xs := xShell
 ,Strings
 ,DStrings
 ,SYSTEM
 ;

FROM Strings IMPORT CompareResults;
FROM WinDlg  IMPORT SORTMODE;
FROM xrTCov  IMPORT TestConditionType, TestConditionNames;

--------------------------------------------------------------------------------
-- Addition definitions to xrTCov.def:
--------------------------------------------------------------------------------

TYPE
  TTestConditionTypeReadableNames = ARRAY TestConditionType OF
                                    ARRAY 32 OF CHAR;
CONST
  TestConditionTypeReadableNames = TTestConditionTypeReadableNames {
     "file"

  -- C1 criterion
  ,  "procedure"
  ,  "module begin"
  ,  "module finally"

  ,  "if"
  ,  "else"
  ,  "elsif"
  ,  "end if"

  ,  "case"
  ,  "else in case"
  ,  "end case"

  ,  "with"
  ,  "else in with"
  ,  "end with"

  ,  "while"
  ,  "end while"

  ,  "repeat"
  ,  "end repeat"

  ,  "for"
  ,  "end for"

  ,  "loop"
  ,  "end loop"

  -- Iteration criterion
  ,  "max. while iteartions"
  ,  "max. repeat iteartions"
  ,  "max. loop_for iteartions"
  ,  "max. loop_loop iteartions"
  };

CONST
  TestConditionNPairs       = 4;
TYPE
  TTestConditionPairs    = ARRAY TestConditionNPairs OF TestConditionType;
  TTestConditionPairFmts = ARRAY TestConditionNPairs OF ARRAY 40 OF CHAR;
CONST
  TTestConditionPairHeads = TTestConditionPairs {
    cr_C1_While
  , cr_C1_Repeat
  , cr_C1_For
  , cr_C1_Loop
  };
  TTestConditionPairTails = TTestConditionPairs {
    cr_Itr_While
  , cr_Itr_Repeat
  , cr_Itr_For
  , cr_Itr_Loop
  };
  TestConditionPairFmts = TTestConditionPairFmts {
    "%s%-10u while (max. iterates %u)"
  , "%s%-10u repeat (max. iterates %u)"
  , "%s%-10u for (max. iterates %u)"
  , "%s%-10u loop (max. iterates %u)"
  };
  TestConditionFmt = "%s%-10u %s";

--------------------------------------------------------------------------------

CONST
  TAB       = 11C;
  FILESEP   = '/';
  MAX_PATH  = 256;

TYPE
  PFILEREC = POINTER TO FILEREC;
  FILEREC = RECORD
    szFile      : DStrings.String;
    statConds   : LONGINT;
    statCovered : LONGINT;
    next        : PFILEREC;
  END;

  PMESSAGE = POINTER TO MESSAGE;
  MESSAGE = RECORD
    file        : PFILEREC;
    condType    : TestConditionType;
    nCounter    : LONGINT;
    nPairCounter: LONGINT;
    nPairIdx    : LONGINT; -- -1 => no pair
    nLine       : LONGINT;
    nPos        : LONGINT;
    nBeg        : LONGINT;
    nEnd        : LONGINT;
    msgType     : CHAR;
    szText      : DStrings.String;
    hidden      : BOOLEAN;
    next        : PMESSAGE;
  END;


VAR
  msgList     : PMESSAGE;
  pairHead    : PMESSAGE;
  pFileRecs   : PFILEREC;
  xShellOn    : BOOLEAN;
  progrLast   : LONGINT;
  logFileName : ARRAY MAX_PATH OF CHAR;
  moduleName  : ARRAY MAX_PATH OF CHAR;
  grepLine    : ARRAY MAX_PATH OF CHAR;
  pathPrefix  : ARRAY MAX_PATH OF CHAR;
  sortMode    : SORTMODE;
  consoleMode : BOOLEAN;
  hideZer     : BOOLEAN;
  hide_zIni   : BOOLEAN;
  prcOnly     : BOOLEAN;

PROCEDURE usage(msg : ARRAY OF CHAR);
BEGIN
  P.printf("\nUsage:\n\n");
  P.printf("  tc2shell [options]\n");
  P.printf("    -c        - console mode (skip dialog)\n");
  P.printf("    -f file   - info file name\n");
  P.printf("    -g text   - grep with this text\n");
  P.printf("    -i file   - .ini file (default value is '.\tc2shellCfg.ini')\n");
  P.printf("    -m module - this module only\n");
  P.printf("    -p text   - path prefix\n");
  P.printf("    -po       - print coverage percentage only\n");
  P.printf("    -sc       - sort by counter\n");
  P.printf("    -sp       - sort by position\n");
  P.printf("    -st       - sort by condition type\n");
  P.printf("    -zh       - hide nested zeroes\n");
  P.printf("    -zs       - show nested zeroes (default)\n\n");
  P.printf("*** Error: %s\n\n", msg);
  HALT(1);
END usage;

PROCEDURE parseArgs():BOOLEAN;
VAR
  s   : ARRAY MAX_PATH OF CHAR;
  i   : LONGINT;
BEGIN
  logFileName := "";
  moduleName  := "";
  grepLine    := "";
  sortMode    := SORT_NOT_SET_MODE;
  consoleMode := FALSE;
  hideZer     := FALSE;
  hide_zIni   := FALSE;
  prcOnly     := FALSE;
  i := 0;
  LOOP
    PE.GetArg(i, s);
    IF ((s[0] = '-') OR (s[0] = '/')) THEN
      s[0] := '-';
      IF    (s = "-c") THEN
            consoleMode := TRUE;
      ELSIF (s = "-f") THEN
            INC(i);
            PE.GetArg(i, logFileName);
            IF (logFileName[0] = 0C) THEN RETURN FALSE; END;
      ELSIF (s = "-g") THEN
            INC(i);
            PE.GetArg(i, grepLine);
            IF (grepLine[0] = 0C) THEN RETURN FALSE; END;
      ELSIF (s = "-i") THEN
            INC(i);
            PE.GetArg(i, DLG.iniFileName);
            IF (DLG.iniFileName[0] = 0C) THEN RETURN FALSE; END;
      ELSIF (s = "-m") THEN
            INC(i);
            PE.GetArg(i, moduleName);
            IF (moduleName[0] = 0C) THEN RETURN FALSE; END;
      ELSIF (s = "-p") THEN
            INC(i);
            PE.GetArg(i, pathPrefix);
            IF (pathPrefix[0] = 0C) THEN RETURN FALSE; END;
      ELSIF (s = "-po") THEN
            prcOnly := TRUE;
      ELSIF (s = "-sc") THEN
            sortMode := SORT_COUNTER;
      ELSIF (s = "-sp") THEN
            sortMode := SORT_POSITION;
      ELSIF (s = "-st") THEN
            sortMode := SORT_CONDTYPE;
      ELSIF (s = "-zh") THEN
            hideZer   := TRUE;
            hide_zIni := TRUE;
      ELSIF (s = "-zs") THEN
            hideZer   := FALSE;
            hide_zIni := TRUE;
      ELSE
        RETURN FALSE;
      END;
    ELSE
      RETURN (s[0] = 0C);
    END;
    INC(i);
  END;
END parseArgs;

-- show dialog and adjust commandline options
PROCEDURE AdjustDataWithDialog();
VAR pdd : DLG.PDLG_DATA;
BEGIN
  NEW(pdd);
  -- merge commandline options and old profile config:
  DLG.ReadDlgDataFromProfile(pdd);
  IF (logFileName[0] # 0C) THEN
    Strings.Assign(logFileName, pdd.szLogFileName);
  END;
  IF (moduleName[0] # 0C) THEN
    Strings.Assign(moduleName,  pdd.szModuleName);
    pdd.checkModuleName := TRUE;
  END;
  IF (grepLine[0] # 0C) THEN
    Strings.Assign(grepLine,    pdd.szGrepLine);
    pdd.checkGrepLine   := TRUE;
  END;
  IF (pathPrefix[0] # 0C) THEN
    Strings.Assign(pathPrefix,  pdd.szPathPrefix);
  END;
  IF hide_zIni THEN
    pdd.hideZer := hideZer;
  END;
  IF (sortMode # SORT_NOT_SET_MODE) THEN
    pdd.sortMode := sortMode;
  END;

  IF NOT consoleMode THEN
    IF NOT DLG.Dialog(pdd) THEN
      HALT(0);
    END;
  END;

  Strings.Assign(pdd.szLogFileName, logFileName);
  Strings.Assign(pdd.szModuleName,  moduleName);
  IF pdd.checkModuleName THEN Strings.Assign(pdd.szModuleName, moduleName);
  ELSE                        Strings.Assign("",               moduleName); END;
  IF pdd.checkGrepLine   THEN Strings.Assign(pdd.szGrepLine,   grepLine);
  ELSE                        Strings.Assign("",               grepLine); END;
  Strings.Assign(pdd.szPathPrefix, pathPrefix);
  hideZer  := pdd.hideZer;
  sortMode := pdd.sortMode;
  DLG.SaveDlgDataToProfile(pdd);
END AdjustDataWithDialog;

PROCEDURE PrintStat();
VAR
  p        : PFILEREC;
  prc      : LONGINT;
  totConds : LONGINT;
  totCovs  : LONGINT;
  totMods  : LONGINT;
BEGIN
  P.printf("Conditions %covered  Module\n");
  P.printf("---------- -------- ----------------------------------------------------------\n");
  totConds := 0;
  totCovs  := 0;
  totMods  := 0;
  p        := pFileRecs;
  WHILE (p # NIL) DO
    IF (p.statConds # 0) THEN prc := p.statCovered * 100 DIV p.statConds;
    ELSE                      prc := 0; END;
    P.printf(" %+9u     %+3u%%  %s\n", p.statConds, prc, p.szFile^);
    INC(totConds, p.statConds);
    INC(totCovs,  p.statCovered);
    INC(totMods);
    p := p.next;
  END;
  IF (totConds # 0) THEN prc := totCovs * 100 DIV totConds;
  ELSE                   prc := 0; END;
  P.printf("---------- -------- ----------------------------------------------------------\n");
  P.printf(" %+9u     %+3u%%  Total %u modules\n\n", totConds, prc, totMods);
END PrintStat;

     VAR lastFileRec: PFILEREC;
PROCEDURE getFileRec(fname-: ARRAY OF CHAR): PFILEREC;
VAR p, pe : PFILEREC;
BEGIN
  IF (lastFileRec # NIL) AND Strings.Equal(fname, lastFileRec.szFile^) THEN
    RETURN lastFileRec;
  END;
  pe := NIL;
  p  := pFileRecs;
  LOOP
    IF (p = NIL) THEN
      NEW(p);
      DStrings.Assign(fname, p.szFile);
      p.statConds   := 0;
      p.statCovered := 0;
      p.next        := NIL;
      IF (pe # NIL) THEN pe.next   := p;
      ELSE               pFileRecs := p;
      END;
      EXIT;
    END;
    IF Strings.Equal(fname, p.szFile^) THEN
      EXIT;
    END;
    pe := p;
    p  := p.next;
  END;
  lastFileRec := p;
  RETURN p;
END getFileRec;
--------------------------------------------------------------------------------

PROCEDURE lss(a,b : PMESSAGE): BOOLEAN;
VAR cr : CompareResults;
BEGIN
  CASE (sortMode) OF
  |SORT_COUNTER:
    RETURN a.nCounter > b.nCounter;
  |SORT_CONDTYPE:
    IF    (a.condType # b.condType) THEN RETURN a.condType > b.condType;
    ELSE                                 RETURN a.nCounter > b.nCounter; END;
  |SORT_POSITION:
    cr := Strings.Compare(a.file.szFile^, b.file.szFile^);
    IF    (cr # equal)        THEN RETURN cr = less;
    ELSE                           RETURN a.nBeg < b.nBeg;  END;
  END;
END lss;

PROCEDURE SortAndSend();
VAR
  p             : PMESSAGE;
  total         : LONGINT;
  arr           : POINTER TO ARRAY OF PMESSAGE;
  i, prc, prc0  : LONGINT;
  blanks        : ARRAY 11 OF CHAR;
  txt           : ARRAY 256 OF CHAR;
  sortsave      : SORTMODE;
  ----
  PROCEDURE swap(a,b : LONGINT);
  VAR p : PMESSAGE;
  BEGIN
    p      := arr[a];
    arr[a] := arr[b];
    arr[b] := p;
  END swap;
  ----
  PROCEDURE QSort(l,r: LONGINT);
  VAR
    i,j:LONGINT;
  BEGIN
    WHILE r > l DO
      i := l+1;
      j := r;
      WHILE i <= j DO
        WHILE (i <= j) AND NOT lss(arr[l],arr[i]) DO INC(i) END;
        WHILE (i <= j) AND lss(arr[l],arr[j]) DO DEC(j) END;
        IF i <= j THEN swap(i,j); INC(i); DEC(j) END;
      END;
      IF j # l THEN swap(j,l) END;
      IF j+j > r+l THEN (* small one recursively *)
        QSort(j+1,r);
        r := j-1;
      ELSE
        QSort(l,j-1);
        l := j+1;
      END;
    END;
  END QSort;
  ----
  PROCEDURE HideZeroes();
  (* arr[0..total-1] is position-sorted now. HideZeroes() removes from this array
  not wanted elements and decreases 'total' *)
    PROCEDURE hid(i:LONGINT);
    VAR b,e,i2 : LONGINT;
        f      : PFILEREC;
        p      : PMESSAGE;
    BEGIN
      p := arr[i];
      IF (p.nCounter > 0) OR ((p.nPairIdx # -1) AND (p.nPairCounter > 0)) THEN
        RETURN;
      END;
      b := p.nBeg;
      e := p.nEnd;
      f := p.file;
      IF (b # e) THEN -- try process nested items
        i2 := i+1;
        LOOP
          IF (i2 = total) THEN EXIT; END;
          p := arr[i2];
          IF (p.file = f) AND (p.nBeg>b) AND (p.nEnd<=e) THEN
            IF (p.nCounter > 0) OR ((p.nPairIdx # -1) AND (p.nPairCounter > 0)) THEN
              RETURN; -- found nested no-null item
            END;
          END;
          IF (p.file # f) OR (p.nBeg >= e) OR (p.nEnd>e (* partial-nested? :) *)) THEN
            EXIT; -- all nested items processed
          END;
          INC(i2);
        END;
        INC(i); -- skip 1st
        WHILE (i<i2) DO -- clear all nested zero-counts items
          arr[i].hidden := TRUE;
          INC(i);
        END;
      END;
    END hid;
  VAR
    i : LONGINT;
  BEGIN
    FOR i:=0 TO total-1 DO
      IF NOT arr[i].hidden THEN
        hid(i);
      END;
    END;
  END HideZeroes;
  ----
  PROCEDURE digits(n : LONGINT): LONGINT;
  VAR d : LONGINT;
  BEGIN
    FOR d:=1 TO 5 DO
      n := n DIV 10;
      IF (n = 0) OR (d = 5) THEN RETURN d; END;
    END;
  END digits;
  ----
BEGIN
  total := 0;
  p     := msgList;
  WHILE (p # NIL) DO
    INC(total);
    p:=p.next;
  END;
  IF (total = 0) THEN RETURN; END;
  NEW(arr, total);
  p     := msgList;
  i     := total-1;
  WHILE (p # NIL) DO
    arr[i] := p;
    DEC(i);
    p:=p.next;
  END;
  -- hide nested zeroes
  IF (hideZer) THEN
    IF (xShellOn) THEN
      xs.StartJob("Hiding nested zeroes...", 100);
      xs.Progress (-1, -1);
    END;
    sortsave := sortMode;
    sortMode := SORT_POSITION;
    QSort(0,total-1);
    HideZeroes();
    sortMode := sortsave;
  END;
  -- sort arr[0..total-1]
  IF sortMode # SORT_NONE THEN
    IF (xShellOn) THEN
      xs.StartJob("Sorting messages...", 100);
      xs.Progress (-1, -1);
    END;
    QSort(0,total-1);
  END;
  -- send messages
  IF (xShellOn) THEN
    xs.StartJob ("Sending messages to shell...", 100);
  END;
  prc0 := -1;
  FOR i:=0 TO total-1 DO
    prc := (i+1) * 100 DIV total;
    p   := arr[i];
    blanks := "          ";
    blanks[10 - digits(p.nLine) - digits(p.nPos)] := 0C;
    IF (p.szText # NIL) THEN
      Strings.Assign(p.szText^, txt);
    ELSE
      IF (p.nPairIdx = -1) THEN
        P.sprintf(txt, TestConditionFmt, blanks, p.nCounter, TestConditionTypeReadableNames[p.condType]);
      ELSE
        P.sprintf(txt, TestConditionPairFmts[p.nPairIdx], blanks, p.nCounter, p.nPairCounter);
      END;
    END;
    --IF (p.hidden) THEN
    --  Strings.Append(" - HIDDEN", txt);
    --END;
    IF NOT p.hidden THEN
      IF (xShellOn) THEN
        xs.Error(p.msgType, 0, p.nPos, p.nLine, p.file.szFile^, txt);
        IF prc # prc0 THEN
          xs.Progress (-prc, -prc);
          prc0 := prc;
        END;
      ELSE
        P.printf("%u%% - %s [%u:%u]  %s\n", prc, p.file.szFile^, p.nLine, p.nPos, txt);
      END;
    END;
  END;
END SortAndSend;
--------------------------------------------------------------------------------


PROCEDURE filterModuleName(szFile-: ARRAY OF CHAR): BOOLEAN;
-- TRUE => skip this item
VAR l, fn, m: LONGINT;
BEGIN
  IF (prcOnly (*turns filters OFF*)) THEN RETURN FALSE; END;
  IF (moduleName[0] = 0C) THEN RETURN FALSE; END;
  l  := 0;
  fn := 0;
  WHILE (szFile[l] # 0C) DO
    IF (szFile[l] = FILESEP) THEN
      fn := l+1;
    END;
    INC(l);
  END;
  -- szFile[fn..l] = fname.ext 0C
  m := 0;
  LOOP
    IF (CAP(szFile[fn]) # CAP(moduleName[m])) THEN RETURN TRUE; END;
    INC(fn);
    INC(m);
    IF (moduleName[m] = 0C) THEN
      IF (szFile[fn] = '.') OR (szFile[fn] = 0C) THEN
        RETURN FALSE;
      ELSE
        RETURN TRUE;
      END;
    END;
  END;
END filterModuleName;


PROCEDURE filterGrepLine(str-: ARRAY OF CHAR): BOOLEAN;
-- TRUE => skip this item
VAR
  res : BOOLEAN;
  pos : SYSTEM.CARD32;
BEGIN
  IF (prcOnly (*turns filters OFF*)) THEN RETURN FALSE; END;
  IF (grepLine[0] = 0C) THEN RETURN FALSE; END;
  Strings.FindNext(grepLine, str, 0, res, pos);
  RETURN NOT res;
END filterGrepLine;


PROCEDURE InsertPathPrefix(VAR szFile: ARRAY OF CHAR);
VAR
  sz : ARRAY MAX_PATH OF CHAR;
  ch : CHAR;
BEGIN
  IF (pathPrefix[0] # 0C) THEN
    Strings.Assign(pathPrefix, sz);
    ch := pathPrefix[LENGTH(pathPrefix)-1];
    IF (ch#'\') AND (ch#'/') THEN
      Strings.Append("\", sz);
    END;
    Strings.Append(szFile, sz);
    Strings.Assign(sz, szFile);
  END;
END InsertPathPrefix;

--------------------------------------------------------------------------------

PROCEDURE is_digit(ch : CHAR):BOOLEAN;
BEGIN
  RETURN (ch>='0') AND (ch<='9');
END is_digit;

PROCEDURE skipbl(sz-: ARRAY OF CHAR; pos : LONGINT) : LONGINT;
BEGIN
  WHILE (sz[pos] = ' ') OR (sz[pos] = TAB) DO
    INC(pos);
  END;
  RETURN pos;
END skipbl;

PROCEDURE getNumBl(VAR d: LONGINT; str-: ARRAY OF CHAR; VAR pos : LONGINT): BOOLEAN;
VAR -- get number and skip blanks after it
  i : LONGINT;
BEGIN
  IF NOT is_digit(str[pos]) THEN RETURN FALSE; END;
  i   := 0;
  LOOP
    IF (is_digit(str[pos])) THEN
      i := i*10 + ORD(str[pos]) - ORD('0');
      INC(pos);
    ELSE
      d   := i;
      pos := skipbl(str, pos);
      RETURN TRUE;
    END;
  END;
END getNumBl;

PROCEDURE getStrBl(VAR d: ARRAY OF CHAR; str-: ARRAY OF CHAR; VAR pos : LONGINT): LONGINT;
-- get string (quoted or up to blank) and skip blanks after it
-- returns found string length
VAR
  dp    : LONGINT;
  q, ch : CHAR;
BEGIN
  dp:=0;
  IF (str[pos] = '"') OR (str[pos] = "'") THEN
    q := str[pos];
    INC(pos);
  ELSE
    q := 0C;
  END;
  LOOP
    ch := str[pos];
    IF (q = 0C) AND ((ch = 0C) OR (ch = ' ') OR (ch = TAB)) THEN
      EXIT;
    END;
    IF (q # 0C) AND ((ch = 0C) OR (ch = q)) THEN
      INC(pos);
      EXIT;
    END;
    d[dp] := ch;
    INC(pos);
    INC(dp);
  END;
  d[dp] := 0C;
  pos := skipbl(str, pos);
  RETURN dp;
END getStrBl;

--------------------------------------------------------------------------------

PROCEDURE stringReader(str : ARRAY OF CHAR; progress : LONGINT);
  ----
  PROCEDURE condString() : BOOLEAN;
    ----
    PROCEDURE parse_cond(szCondType : ARRAY OF CHAR; VAR condType: TestConditionType): BOOLEAN;
    VAR i : TestConditionType;
    BEGIN
      FOR i:=MIN(TestConditionType) TO MAX(TestConditionType) DO
        IF Strings.Equal(szCondType, TestConditionNames[i]) THEN
          condType := i;
          RETURN TRUE;
        END;
      END;
      RETURN FALSE;
    END parse_cond;
    ----
  VAR
    pos, i    : LONGINT;
    nCondNum  : LONGINT;
    szCondType: ARRAY MAX_PATH OF CHAR;
    condType  : TestConditionType;
    nCounter  : LONGINT;
    szFile    : ARRAY MAX_PATH OF CHAR;
    nLine     : LONGINT;
    nPos      : LONGINT;
    nLineE    : LONGINT;
    nPosE     : LONGINT;
    msg       : PMESSAGE;
  BEGIN
    ---- Filter -g grepline:
    IF filterGrepLine(str) THEN
      RETURN FALSE;
    END;
    -- process strings like this:
    --   <condNum> @<condType> = <counter> ["<filename>" <line>.<pos>]
    -- or
    --   <condNum> @<condType> = <counter> ["<filename>" <line>.<pos> <lineEnd>.<posEnd>]
    pos := skipbl(str, 0);
    IF NOT getNumBl(nCondNum, str, pos) THEN             ---> nCondNum
      RETURN FALSE;
    END;
    IF str[pos] # '@' THEN RETURN FALSE; END;
    INC(pos);
    IF getStrBl(szCondType, str, pos) < 1 THEN
      RETURN FALSE;
    ELSIF ~parse_cond(szCondType, condType (*=>*)) THEN  ---> condType
      RETURN FALSE;
    END;
    IF str[pos] # '=' THEN RETURN FALSE; END;
    pos := skipbl(str, pos+1);
    IF NOT getNumBl(nCounter, str, pos) THEN             ---> nCounter
      RETURN FALSE;
    END;
    IF str[pos] # '[' THEN RETURN FALSE; END;
    pos := skipbl(str, pos+1);
    IF getStrBl(szFile, str, pos) < 1 THEN               ---> szFile
      RETURN FALSE;
    END;
    InsertPathPrefix(szFile);
    IF NOT getNumBl(nLine, str, pos) THEN                ---> nLine
      RETURN FALSE;
    END;
    IF str[pos] # '.' THEN RETURN FALSE; END;
    INC(pos);
    IF NOT getNumBl(nPos, str, pos) THEN                 ---> nPos
      RETURN FALSE;
    END;
    IF (str[pos] = ']') THEN
      -- old format - w/o end position:
      nLineE := nLine;
      nPosE  := nPos;
    ELSE
      IF NOT getNumBl(nLineE, str, pos) THEN             ---> nLineE
        RETURN FALSE;
      END;
      IF str[pos] # '.' THEN RETURN FALSE; END;
      INC(pos);
      IF NOT getNumBl(nPosE, str, pos) THEN              ---> nPosE
        RETURN FALSE;
      END;
      IF str[pos] # ']' THEN RETURN FALSE; END;
    END;
    ---- Filter -m moduleName:
    IF filterModuleName(szFile) THEN
      RETURN FALSE;
    END;
    -----------
    NEW(msg);
    FOR i:=0 TO TestConditionNPairs-1 DO
      IF TTestConditionPairHeads[i] = condType THEN
        pairHead := msg;
      ELSIF (pairHead # NIL) AND
            (condType          = TTestConditionPairTails[i]) AND
            (pairHead.condType = TTestConditionPairHeads[i]) AND
            (pairHead.nLine    = nLine)        AND
            (pairHead.nPos     = nPos)         AND
            Strings.Equal(pairHead.file.szFile^, szFile)
      THEN
        pairHead.nPairCounter := nCounter;
        pairHead.nPairIdx     := i;
        pairHead              := NIL;
        RETURN TRUE;
      END;
    END;
    msg.file         := getFileRec(szFile);
    msg.condType     := condType;
    msg.nCounter     := nCounter;
    msg.nPairIdx     := -1;
    msg.nLine        := nLine;
    msg.nPos         := nPos;
    msg.nBeg         := nLine *256+nPos;
    msg.nEnd         := nLineE*256+nPosE;
    msg.msgType      := xs.MSG_NOTICE;
    msg.szText       := NIL;
    msg.hidden       := FALSE;
    msg.next         := msgList;
    msgList          := msg;
    INC(msg.file.statConds);
    IF (nCounter > 0) THEN
      INC(msg.file.statCovered);
    END;
    RETURN TRUE;
  END condString;
  ----
  PROCEDURE fileString();
  VAR
    pos       : LONGINT;
    nCounter  : LONGINT;
    szFile    : ARRAY MAX_PATH OF CHAR;
    nTime     : LONGINT;
    tt        : LONGINT;
    txt       : ARRAY MAX_PATH*2 OF CHAR;
    msg       : PMESSAGE;
  BEGIN
    -- process strings like this:
    --   @file = <counter> ["<filename>" <time> <day>.<month>.<year> <hours>:<minutes>]
    pos := skipbl(str, 0);
    IF str[pos] # '@' THEN RETURN; END;
    INC(pos);
    IF getStrBl(txt, str, pos) < 1 THEN  ---> 'file' expected
      RETURN;
    END;
    IF NOT Strings.Equal(txt, "file") THEN
      RETURN;
    END;
    IF str[pos] # '=' THEN RETURN; END;
    pos := skipbl(str, pos+1);
    IF NOT getNumBl(nCounter, str, pos) THEN    ---> nCounter
      RETURN;
    END;
    IF str[pos] # '[' THEN RETURN; END;
    pos := skipbl(str, pos+1);
    IF getStrBl(szFile, str, pos) < 1 THEN      ---> szFile
      RETURN;
    END;
    InsertPathPrefix(szFile);
    IF NOT getNumBl(nTime, str, pos) THEN       ---> nTime
      RETURN;
    END;
    -- .. ignore rest of line.
    IF filterModuleName(szFile) THEN
      RETURN;
    END;
    tt := VAL(LONGINT, FRead.GetFileTime(szFile));
    IF (tt = 0) THEN
      P.sprintf(txt, "Module not found: %s", szFile);
      NEW(msg);
      msg.file         := getFileRec(szFile);
      msg.condType     := cr_File; -- to sort it up
      msg.nCounter     := 7FFFFFFFH;
      msg.nPairIdx     := -1;
      msg.nLine        := 0;
      msg.nPos         := 0;
      msg.nBeg         := 0;
      msg.nEnd         := 0;
      msg.msgType      := xs.MSG_WARNING;
      DStrings.Assign(txt, msg.szText);
      msg.hidden       := FALSE;
      msg.next         := msgList;
      msgList          := msg;
    ELSIF (tt # nTime) THEN
      P.sprintf(txt, "Bad module time: %s", szFile);
      NEW(msg);
      msg.file         := getFileRec(szFile);
      msg.condType     := cr_File; -- to sort it up
      msg.nCounter     := 7FFFFFFFH;
      msg.nPairIdx     := -1;
      msg.nLine        := 0;
      msg.nPos         := 0;
      msg.nBeg         := 0;
      msg.nEnd         := 0;
      msg.msgType      := xs.MSG_ERROR;
      DStrings.Assign(txt, msg.szText);
      msg.hidden       := FALSE;
      msg.next         := msgList;
      msgList          := msg;
    END;
  END fileString;
  ----
BEGIN
  IF NOT condString() THEN
    fileString();
  END;
  IF xShellOn AND (progress # progrLast) THEN
    xs.Progress (-progress, -progress);
    progrLast := progress;
  END;
END stringReader;


PROCEDURE do();
VAR
  f  : FRead.FILE;
  sz : ARRAY MAX_PATH+32 OF CHAR;
BEGIN
  xShellOn := xs.Start();
  IF NOT xShellOn AND NOT consoleMode THEN
    P.printf("\n*** TConv is runned not from xds shell => debug mode.\n\n");
  END;
  IF (xShellOn) THEN
    xs.TurnSortingOff();
    P.sprintf(sz, "Read %s...", logFileName);
    xs.StartJob(sz, 100);
    xs.String ("Start...");
  END;
  IF NOT FRead.Open(f, logFileName) THEN
    P.sprintf(sz, "Can not open log file '%s'.", logFileName);
    usage(sz);
  END;
  progrLast := -1;
  FRead.EnumFileLines(f, stringReader);
  FRead.Close(f);
  PrintStat();
  IF NOT prcOnly THEN
    SortAndSend();
  END;
  IF (xShellOn) THEN
    xs.Progress (-100, -100);
    xs.String ("Done.");
  END;
END do;

BEGIN
  msgList    := NIL;
  pairHead   := NIL;
  pFileRecs  := NIL;
  lastFileRec:= NIL;
  IF NOT parseArgs() THEN
    usage("Invalid commandline.");
  END;
  AdjustDataWithDialog();
  do();
END tc2shell.
