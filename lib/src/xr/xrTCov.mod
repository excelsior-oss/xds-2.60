<*+ M2EXTENSIONS *>
--------------------------------------------------------------------------------
--                        Excelsior XDS Test Coverage
--                          (c) 2002, Excelsior Ltd.
--
-- Module:       xrTCov
-- Author:       Lvov Konstantin
-- Modified By:
-- Created:      10-Dec-2002
-- Purpose:      Test Coverage Run-Time Support
-- Functions:
-- Comment:      
-- Synonym:      tc
--------------------------------------------------------------------------------
<* IF ~ DEFINED(TESTCOVERAGE) THEN *>  <* NEW TESTCOVERAGE- *>
<* ELSE *>                             <*- TESTCOVERAGE *>
<* END *>
<* IF __GEN_C__ THEN *>
<*- gendebug    *> (* Don enable! History would not work (SYSTEM.CODE) *)
<* END *>
IMPLEMENTATION MODULE xrTCov;

IMPORT  ProgEnv,            xmRTS
     ,  xrFName,            os := xtcsOS                     
     ,  sys := SYSTEM
--     ,  io := Printf
     ;

--------------------------------------------------------------------------------
--                  Accumulate Test Coverage Information
--------------------------------------------------------------------------------
VAR
  RegisteredModuleList: TModuleInfoPtr;

PROCEDURE ["C"] X2C_EXIT_TESTCOVERAGE (); FORWARD;

--------------------------------------------------------------------------------
PROCEDURE RegistryModule ( module:    TModuleInfoPtr
                         ; modtime:   TTime
                         ; modname-:  ARRAY OF CHAR
                         ; srcrefs-:  ARRAY OF TSourceRef 
                         ; counters-: ARRAY OF TCounter 
                         );
BEGIN
  ASSERT( module # NIL );
  IF (module^.count # 0) THEN  RETURN  END;
  ASSERT( HIGH(srcrefs) = HIGH(counters) );

  IF RegisteredModuleList = NIL THEN
    os.X2C_atexit(X2C_EXIT_TESTCOVERAGE);
  END;

  module^.count    := HIGH(srcrefs)+1;

  module^.modtime  := modtime;
  module^.modname  := sys.REF(modname);
  module^.srcrefs  := sys.REF(srcrefs);
  module^.counters := sys.REF(counters);
  sys.FILL( module^.counters, 0, SIZE(TCounter) * module^.count );

  module^.next         := RegisteredModuleList;
  RegisteredModuleList := module;
END RegistryModule;


--------------------------------------------------------------------------------
PROCEDURE IncreaseCounter ( index: TIndex
                          ; VAR counters: ARRAY OF TCounter 
                          );
BEGIN
  ASSERT( (index >= 0) AND (index < LEN(counters)) );

  IF counters[index] < MAX(TCounter) THEN
    INC(counters[index]);
  END;
END IncreaseCounter;


--------------------------------------------------------------------------------
PROCEDURE IncreaseCounterExt ( index: TIndex 
                             ; VAR counters: ARRAY OF TCounter 
                             ; modtime:   TTime
                             ; module:    TModuleInfoPtr
                             ; modname-:  ARRAY OF CHAR
                             ; srcrefs-:  ARRAY OF TSourceRef 
                             );
BEGIN
  ASSERT( module # NIL );
  IF ( module^.count = 0                    ) OR
     ( module^.modtime  # modtime           ) OR
     ( module^.modname  # sys.ADR(modname)  ) OR
     ( module^.srcrefs  # sys.ADR(srcrefs)  ) OR
     ( module^.counters # sys.ADR(counters) )
  THEN  
    RegistryModule (module, modtime, modname, srcrefs, counters);
  END;
  IncreaseCounter (index, counters);
END IncreaseCounterExt;


--------------------------------------------------------------------------------
PROCEDURE InitIterationLocal (VAR local_counter: TCounter);
BEGIN
  local_counter := 0;
END InitIterationLocal;


--------------------------------------------------------------------------------
PROCEDURE IncreaseIterationCounter ( index: TIndex
                                   ; VAR local_counter: TCounter
                                   ; VAR counters: ARRAY OF TCounter 
                                   );
BEGIN
  ASSERT( (index >= 0) AND (index < LEN(counters)) );
  ASSERT( local_counter <= counters[index] ); 

  IF local_counter < MAX(TCounter) THEN
    INC(local_counter);
    IF local_counter > counters[index] THEN
      INC(counters[index]);
    END;
  END;
END IncreaseIterationCounter;



--------------------------------------------------------------------------------
--                   Auxiliary Objects and Procedures
--------------------------------------------------------------------------------
CONST
  malloc = os.X2C_malloc;
  free   = os.X2C_free;
  strcmp = xmRTS.X2C_STRCMP_PROC; 

--------------------------------------------------------------------------------
-- identical to M2 ISO Strings.Append 
PROCEDURE Append (s: ARRAY OF CHAR; VAR d: ARRAY OF CHAR);
  VAR pos, i, len: CARDINAL;
BEGIN
  pos:=LENGTH(d);
  len:=LENGTH(s);
  IF pos+len >HIGH(d)+1 THEN len:=HIGH(d)+1 - pos END;
  i:=0;
  WHILE i<len DO d[pos]:=s[i]; INC(i); INC(pos) END;
  IF pos<=HIGH(d) THEN d[pos]:=0C END;
END Append;


--------------------------------------------------------------------------------
PROCEDURE NewModuleInfo (count: TIndex; modname-: ARRAY OF CHAR): TModuleInfoPtr;
VAR module: TModuleInfoPtr;
BEGIN
  IF count < 0 THEN  RETURN NIL END;

  module := malloc (SIZE(TModuleInfo));
  sys.FILL( module, 0, SIZE(TModuleInfo) );
  module^.modname  := malloc (LENGTH(modname)+1);
  module^.srcrefs  := malloc (SIZE(TSourceRef) * sys.CARD32(count));
  module^.counters := malloc (SIZE(TCounter) * sys.CARD32(count));

  module^.count := count;
  COPY (modname, TModuleName(module^.modname)^);

  RETURN module; 
END NewModuleInfo;


--------------------------------------------------------------------------------
PROCEDURE DisposeModule (VAR module: TModuleInfoPtr);
BEGIN
  IF module = NIL THEN RETURN END;

  free (module^.modname,  LENGTH(TModuleName(module^.modname)^)+1);
  free (module^.srcrefs,  SIZE(TSourceRef) * sys.CARD32(module^.count)); 
  free (module^.counters, SIZE(TCounter) * sys.CARD32(module^.count));
  free (module, SIZE(TModuleInfo));

  module := NIL;
END DisposeModule;


--------------------------------------------------------------------------------
--                       Process  Messages and Errors 
--------------------------------------------------------------------------------
CONST
 StdOutS  = os.X2C_StdOutS;
 StdOutD  = os.X2C_StdOutD;
 StdOutLn = os.X2C_StdOutN;

 MAX_MessageLen = 41;

TYPE 
  TMessage = (
    msgFileOpen
  , msgMakeBackup 
  , msgIncompatibleModuleInfo
  , msgIncorrectModuleInfo
  , msgInconsistencyModuleInfo
  , msgInvalidFileName
  , msgUpdate
  , msgDifferentModifyTime
  );

  TMessageText = ARRAY TMessage OF ARRAY [0..MAX_MessageLen] OF CHAR;

CONST
 MessageText = TMessageText {
   "File open error "
 , "Backup creation error "
 , "Incompatible module logs "
 , "Incorrect module log "
 , "Inconsistency module logs "
 , "Invalid file name "
 , "Update: "
 , "Different source code modification time"
 };

 MessagePrefix = "#RTS Test Coverage: ";
 MergeError    = "merge informatiom error: ";
 EmptyPrefix   = "                    "; 

VAR
  ErrorCnt: sys.INT32;


--------------------------------------------------------------------------------
PROCEDURE Error (msg: TMessage; str-: ARRAY OF CHAR; index:=-1: TIndex);
BEGIN
  StdOutLn ();
  StdOutS (MessagePrefix, 1);
  StdOutS (MergeError, 1);
  StdOutS (str, 1);
  StdOutLn ();

  StdOutS (EmptyPrefix, 1);
  StdOutS (MessageText[msg], 1);
  IF index >= 0 THEN
    StdOutS (" at index ", 1);
    StdOutD (index, 1);
  END;
  StdOutLn ();
  INC (ErrorCnt);
END Error;


--------------------------------------------------------------------------------
PROCEDURE Message (msg: TMessage; str-: ARRAY OF CHAR);
BEGIN
  StdOutLn ();
  StdOutS  (MessagePrefix, 1);
  StdOutS  (MessageText[msg], 1);
  StdOutS  (str, 1);
  StdOutLn ();
END Message;


--------------------------------------------------------------------------------
--                     Open/Close File Operation
--------------------------------------------------------------------------------
TYPE
  FILE = os.X2C_OSFILE;
  TNumberBuf = ARRAY [0..11] OF CHAR;

CONST
  BADFILE = FILE(NIL);

  LogFileExt = ".tc"; 
  BackUpExt  = ".tcb";

  MAXPATHLEN = 1024;

VAR
  UseCurrentDir: BOOLEAN; 

--------------------------------------------------------------------------------
PROCEDURE ReplaceExtension (VAR name: ARRAY OF CHAR; ext-: ARRAY OF CHAR);
VAR f: xrFName.Format;
BEGIN
  xrFName.X2C_ParseFileName (name, f);
  IF f.ok THEN
    IF f.extPos > 0 THEN
      name[f.extPos-1] := 0C;
    END;
    Append (ext, name);
  ELSE
    Error (msgInvalidFileName, name);
    ASSERT( FALSE );
  END;
END ReplaceExtension;


--------------------------------------------------------------------------------
PROCEDURE GetInfoFileName (VAR name: ARRAY OF CHAR);
BEGIN
  name[0] := 0C;
  ProgEnv.String(TCSOUT_ENV_VAR, name);
  IF name[0] = 0C THEN
    ProgEnv.ProgramName(name);
    IF UseCurrentDir THEN
      xrFName.X2C_ExtractBaseName (name, name);
    Append (LogFileExt, name);
  ELSE
    ReplaceExtension (name, LogFileExt);
  END;
  END;
END GetInfoFileName;


--------------------------------------------------------------------------------
PROCEDURE MakeBackUp (fname-: ARRAY OF CHAR);
VAR newfname: ARRAY [0..MAXPATHLEN-1] OF CHAR;
--    buf: TNumberBuf;
--    i, pos: sys.CARD32;
BEGIN
  IF UseCurrentDir THEN
    xrFName.X2C_ExtractBaseName (fname, newfname);
    Append (BackUpExt, newfname);
  ELSE
    COPY (fname, newfname);
    ReplaceExtension (newfname, BackUpExt);
  END;

  IF os.X2C_Exists (newfname) THEN
    os.X2C_Remove (newfname);
  END;
  IF os.X2C_Rename (fname, newfname) # 0 THEN
    Error (msgMakeBackup, newfname);
  END;

(*
  i := 1;   
  WHILE os.X2C_Rename (fname, newfname) # 0 DO
    pos := 0;
    xrFName.X2C_ExtractBaseName (fname, newfname);
    os.X2C_DecToStr (buf, pos, i);
    Append (buf, newfname);
    Append (BackUpExt, newfname);
    INC(i);
  END;
*)
END MakeBackUp;


--------------------------------------------------------------------------------
PROCEDURE OpenFile (name-: ARRAY OF CHAR): FILE;
VAR file: FILE;
BEGIN
  IF os.X2C_FileOpenRead (file, name) # 0 THEN
    Error (msgFileOpen, name);
    file := BADFILE;
  END;
  RETURN file;
END OpenFile;

 
--------------------------------------------------------------------------------
PROCEDURE CreateFile (name-: ARRAY OF CHAR): FILE;
VAR file: FILE;
BEGIN
  IF os.X2C_FileOpenWrite (file, name) # 0 THEN
    Error (msgFileOpen, name);
    file := BADFILE;
  END;
  RETURN file;
END CreateFile;

 
--------------------------------------------------------------------------------
PROCEDURE CloseFile (VAR file: FILE);
BEGIN
  IF file # BADFILE THEN
    sys.EVAL( os.X2C_FileClose(file) );
    file := BADFILE;
  END;
END CloseFile;


--------------------------------------------------------------------------------
--                        Read/Write File Operation
--------------------------------------------------------------------------------
CONST
  TAB     = 11C;
  CR      = 15C;
  LF      = 12C;
  COMMENT = '%';
  SPACE   = ' ';
  SPACES  = "        ";

<* IF __GEN_C__ THEN *>
  NEWLINE = "" + LF;
<* ELSE *>
  NEWLINE = "" + CR + LF;
<* END *>

--------------------------------------------------------------------------------
  StringSIZE  = 1024;
  ReadBufSIZE = StringSIZE*3;

TYPE
  TString = ARRAY [0..StringSIZE] OF CHAR;

  -- must be passed as VAR-parameter only
  FILERD = RECORD
    file: FILE;
    buf:  ARRAY [0..ReadBufSIZE] OF CHAR;
    len:  sys.CARD32;
    pos:  sys.CARD32;
  END; 


--------------------------------------------------------------------------------
PROCEDURE outS (file: FILE; str-: ARRAY OF CHAR; len: CARDINAL);
BEGIN
  IF (len > 0) THEN
    IF os.X2C_FileWrite (file, sys.ADR(str), len) # 0 THEN
      ASSERT(FALSE);
    END;
  END;
END outS;


--------------------------------------------------------------------------------
PROCEDURE writeLn (file: FILE);
BEGIN
  outS (file, NEWLINE, LENGTH(NEWLINE));
END writeLn;


--------------------------------------------------------------------------------
PROCEDURE writeS (file: FILE; str-: ARRAY OF CHAR; width: CARDINAL);
VAR len: CARDINAL;
BEGIN
  len := LENGTH (str);
  outS (file, str, len);

  IF width > len THEN
    WHILE (width - len) > 8 DO  outS (file, SPACES, 8); INC(len, 8) END;
    IF width > len THEN outS (file, SPACES, width - len) END;
  END;
END writeS;


--------------------------------------------------------------------------------
PROCEDURE writeDec (file: FILE; number: sys.INT32; width: CARDINAL);
VAR buf: TNumberBuf;
    len: sys.CARD32;
BEGIN
  len := 0;
  os.X2C_DecToStr (buf, len, number);
  outS (file, buf, len);

  IF width > len THEN
    WHILE (width - len) > 8 DO  outS (file, SPACES, 8); INC(len, 8) END;
    IF width > len THEN outS (file, SPACES, width - len) END;
  END;
END writeDec;


--------------------------------------------------------------------------------
PROCEDURE AttachFILERD (VAR rfile: FILERD; file: FILE);
BEGIN
  rfile.file := file;
  rfile.pos  :=  ReadBufSIZE;
  rfile.len  :=  ReadBufSIZE;
END AttachFILERD;


--------------------------------------------------------------------------------
PROCEDURE inS (VAR file: FILERD; VAR str: ARRAY OF CHAR): BOOLEAN;
VAR i: sys.CARD32;
    last_ch: CHAR;
BEGIN
  str[0] := 0C;

  i := 0;
  -- extract string from buffer
  WHILE i < HIGH(str) DO
    -- update buffer
    IF file.pos >= file.len THEN
      file.pos := 0;
      last_ch := file.buf[file.len-1];
      IF file.len < ReadBufSIZE THEN
        RETURN FALSE;
      ELSIF (os.X2C_FileRead(file.file, sys.ADR(file.buf), file.len) = 0) &
            (file.len = 0) 
      THEN
        IF i = 0 THEN
          RETURN FALSE;
        ELSE
          str[i] := 0C;
          RETURN TRUE;
        END;
      ELSIF (last_ch = CR) & (file.buf[file.pos] = LF) THEN
        -- line separator has got on boundary of 'file.buf'
        -- skip second part of line separator
        INC(file.pos);
      END;
    END;

    -- check line separator
    IF file.buf[file.pos] = CR THEN
      str[i] := 0C;
      INC(file.pos);
      IF (file.buf[file.pos] = 0C) THEN
        RETURN TRUE;
      END;
      -- skip second part of line separator
      ASSERT(file.buf[file.pos] = LF);
      INC(file.pos);
      RETURN TRUE;
    ELSIF file.buf[file.pos] = LF THEN
      str[i] := 0C;
      INC(file.pos);
      RETURN TRUE;
    END;

    str[i] := file.buf[file.pos];
    INC(i);
    INC(file.pos);
  END;

  ASSERT(FALSE);
--  RETURN FALSE;
END inS;


--------------------------------------------------------------------------------
PROCEDURE readS (VAR file: FILERD; VAR str: ARRAY OF CHAR): BOOLEAN;
VAR res: BOOLEAN;
BEGIN
  REPEAT
    res := inS (file, str);
  UNTIL (NOT res) OR (str[0] # COMMENT);
  RETURN res;
END readS;



--------------------------------------------------------------------------------
--                    Read Test Coverage Information
--------------------------------------------------------------------------------
TYPE 
  TModuleInfoHeader = RECORD
    count:   TIndex;
    modtime: TTime;
    modname: TString;
  END;

  TStringPos = sys.CARD32;


--------------------------------------------------------------------------------
PROCEDURE FindField ( str: ARRAY OF CHAR
                    ; start_sym, end_sym: CHAR
                    ; VAR start_pos, end_pos: TStringPos
                    ): BOOLEAN;
VAR str_len: TStringPos; 
BEGIN
  str_len := LENGTH(str);
  -- skip to start symbol
  IF start_sym = "" THEN
    WHILE (start_pos < str_len) AND (str[start_pos] = ' ') DO 
      INC(start_pos) 
    END;
  ELSE
    WHILE (start_pos < str_len) AND (str[start_pos] # start_sym) DO 
      INC(start_pos) 
    END;
    INC(start_pos);
  END;
  IF start_pos = str_len THEN RETURN FALSE END;

  end_pos := start_pos;
  -- skip to end symbol
  WHILE (end_pos < str_len) AND (str[end_pos] # end_sym) DO 
    INC(end_pos);  
  END;
--  IF (end_sym # "") AND (end_pos >= str_len) THEN RETURN FALSE END;
  DEC(end_pos);
  IF start_pos > end_pos THEN RETURN FALSE END;

  RETURN TRUE;
END FindField;


--------------------------------------------------------------------------------
PROCEDURE StrToType ( str-: ARRAY OF CHAR
                    ; VAR pos: TStringPos
                    ; VAR type: TestConditionType ): BOOLEAN;
VAR start_pos: TStringPos;
    itype: TestConditionType;
BEGIN
  start_pos := pos;
  IF NOT FindField (str, '@', ' ', start_pos, pos) THEN RETURN FALSE END;

  -- find corresponding type 
  FOR itype := MIN(TestConditionType) TO MAX(TestConditionType) DO
    IF strcmp ( sys.ADR(TestConditionNames[itype])
              , LENGTH(TestConditionNames[itype])
              , sys.ADR(str[start_pos])
              , pos-start_pos+1 
              ) = 0
    THEN
      type := itype;
      INC(pos);
      RETURN TRUE;
    END;
  END;

  RETURN FALSE;
END StrToType;


--------------------------------------------------------------------------------
PROCEDURE StrToCounter ( str-: ARRAY OF CHAR
                       ; VAR pos: TStringPos
                       ; VAR count: TCounter ): BOOLEAN;
VAR start_pos: TStringPos;
    i: TStringPos;
BEGIN
  start_pos := pos;
  IF NOT FindField (str, '=', ' ', start_pos, pos) THEN RETURN FALSE END;

  count := 0;
  FOR i := start_pos TO pos DO
    count := count*10 + TCounter(ORD(str[i]) - ORD('0'));
  END;

  INC(pos);
  RETURN TRUE;
END StrToCounter;


--------------------------------------------------------------------------------
PROCEDURE StrToName ( str-: ARRAY OF CHAR
                    ; VAR pos: TStringPos
                    ; VAR name: ARRAY OF CHAR ): BOOLEAN;
VAR start_pos: TStringPos;
    i, len: TStringPos;
BEGIN
  start_pos := pos;
  IF NOT FindField (str, '"', '"', start_pos, pos) THEN RETURN FALSE END;

  len := pos - start_pos;
  ASSERT( len < HIGH(name) );

  FOR i := 0 TO len DO
    name[i] := str[start_pos+i];
  END;
  name[len+1] := 0C;

  INC(pos, 2);   -- symbol '"' is part of name and must be skipped 
  RETURN TRUE;
END StrToName;


--------------------------------------------------------------------------------
PROCEDURE StrToModTime ( str-: ARRAY OF CHAR
                       ; VAR pos:  TStringPos
                       ; VAR time: TTime ): BOOLEAN;
VAR start_pos: TStringPos;
    i: TStringPos;
BEGIN
  start_pos := pos;
--  IF NOT FindField (str, '', ']', start_pos, pos) THEN RETURN FALSE END;
  IF NOT FindField (str, '', ' ', start_pos, pos) THEN RETURN FALSE END;
  -- in order to be independ from ShowTextTime On/Off cases
  IF (str[pos] = ']') THEN DEC(pos) END;

  time := 0;
  FOR i := start_pos TO pos DO
    time := time*10 + TTime(ORD(str[i]) - ORD('0'));
  END;

  INC(pos);
  RETURN TRUE;
END StrToModTime;            


--------------------------------------------------------------------------------
PROCEDURE StrToIndex ( str-: ARRAY OF CHAR
                     ; VAR pos: TStringPos
                     ; VAR index: TIndex
                     ): BOOLEAN;
VAR start_pos: TStringPos;
    i: TStringPos;
BEGIN
  start_pos := pos;
  IF NOT FindField (str, '', ' ', start_pos, pos) THEN RETURN FALSE END;

  index := 0;
  FOR i := start_pos TO pos DO
    index := index*10 + TIndex(ORD(str[i]) - ORD('0'));
  END;

  INC(pos);
  RETURN TRUE;
END StrToIndex;


--------------------------------------------------------------------------------
PROCEDURE StrToLine ( str-: ARRAY OF CHAR
                    ; VAR pos: TStringPos
                    ; VAR line: TSourceLine
                    ): BOOLEAN;
VAR start_pos: TStringPos;
    i: TStringPos;
BEGIN
  start_pos := pos;
  IF NOT FindField (str, '', '.', start_pos, pos) THEN RETURN FALSE END;

  line := 0;
  FOR i := start_pos TO pos DO
    line := line*10 + TSourceLine(ORD(str[i]) - ORD('0'));
  END;

  INC(pos);
  RETURN TRUE;
END StrToLine;


--------------------------------------------------------------------------------
PROCEDURE StrToCol ( str-: ARRAY OF CHAR
                   ; VAR pos: TStringPos
                   ; VAR col: TSourceCol
                   ): BOOLEAN;
VAR start_pos: TStringPos;
    i: TStringPos;
BEGIN
  start_pos := pos;
  IF NOT FindField (str, '.', ' ', start_pos, pos) THEN RETURN FALSE END;
  IF (str[pos] = ']') THEN DEC(pos) END;

  col := 0;
  FOR i := start_pos TO pos DO
    col := col*10 + TSourceCol(ORD(str[i]) - ORD('0'));
  END;

  INC(pos);
  RETURN TRUE;
END StrToCol;


--------------------------------------------------------------------------------
PROCEDURE StrToModuleInfoHeader ( str-: ARRAY OF CHAR
                               ; VAR header: TModuleInfoHeader ): BOOLEAN;
VAR pos: TStringPos; 
    type: TestConditionType;
BEGIN
  pos := 0;
  IF StrToType(str, pos, type) AND (type = cr_File) AND
     StrToCounter(str, pos, header.count)           AND
     StrToName(str, pos, header.modname)            AND
     StrToModTime(str, pos, header.modtime)            
  THEN
    RETURN TRUE;
  ELSE   
    RETURN FALSE;
  END;
END StrToModuleInfoHeader;



--------------------------------------------------------------------------------
--                    Merge Test Coverage Information
--------------------------------------------------------------------------------

PROCEDURE FindModuleInfoByName ( name: ARRAY OF CHAR
                               ; regModuleList, unregModuleList: TModuleInfoPtr
                               ): TModuleInfoPtr;
VAR module: TModuleInfoPtr;
    len: sys.CARD32;

    -- 1 --  FindModuleInfoByName -----------------------------------------------
    PROCEDURE Search (): BOOLEAN;
    BEGIN
      WHILE module # NIL DO 
        IF strcmp ( module^.modname, LENGTH(TModuleName(module^.modname)^)
                  , sys.ADR(name), len 
                  ) = 0
        THEN 
          RETURN TRUE;
        END;
        module := module^.next;
      END;
      RETURN FALSE;
    END Search;


-- 0 --  FindModuleInfoByName ---------------------------------------------------
BEGIN
  len := LENGTH(name);

  module := regModuleList;
  IF Search () THEN
    RETURN module;
  END;

  module := unregModuleList;
  IF Search () THEN
    RETURN module;
  END;

  RETURN NIL;
END FindModuleInfoByName;


--------------------------------------------------------------------------------
PROCEDURE IsComptibleLogs ( module: TModuleInfoPtr
                          ; header-: TModuleInfoHeader ): BOOLEAN;
BEGIN
  IF module^.modtime # header.modtime THEN
    Error (msgDifferentModifyTime, header.modname);
  ELSIF module^.count # header.count THEN
    Error (msgIncompatibleModuleInfo, header.modname);
  ELSE
    RETURN TRUE;
  END;
  RETURN FALSE;
END IsComptibleLogs;

 
--------------------------------------------------------------------------------
PROCEDURE MergeModuleInfo (VAR file: FILERD; module: TModuleInfoPtr);

    -- 1 -- MergeModuleInfo -----------------------------------------------------
    PROCEDURE MaxCounter (count1, count2: TCounter): TCounter;
    BEGIN
      IF (count1 > count2) THEN   RETURN  count1;
      ELSE                        RETURN  count2;
      END;
    END MaxCounter;
       
-- 0 -- MergeModuleInfo ---------------------------------------------------------
VAR i: TIndex;
    str: TString;
    pos: TStringPos; 
    index: TIndex;
    type:  TestConditionType;
    count: TCounter;
    name:  TString;
    line, end_line: TSourceLine;
    col, end_col:   TSourceCol;
    modname_len: sys.CARD32;
BEGIN
  modname_len := LENGTH(TModuleName(module^.modname)^);
  FOR i := 0 TO module^.count-1 DO
    pos := 0;
    IF readS (file, str)                 AND
       StrToIndex   (str, pos, index)    AND
       StrToType    (str, pos, type)     AND
       StrToCounter (str, pos, count)    AND
       StrToName    (str, pos, name)     AND
       StrToLine    (str, pos, line)     AND
       StrToCol     (str, pos, col)      AND    
       StrToLine    (str, pos, end_line) AND
       StrToCol     (str, pos, end_col)        
    THEN
      IF (index = i)                                            AND
         (line = TSourceRefs(module^.srcrefs)^[i].line)         AND
         (col  = TSourceRefs(module^.srcrefs)^[i].col)          AND
         (end_line = TSourceRefs(module^.srcrefs)^[i].end_line) AND
         (end_col  = TSourceRefs(module^.srcrefs)^[i].end_col)  AND
         (strcmp ( module^.modname, modname_len
                 , sys.ADR(name), LENGTH(name) 
                 ) = 0
         )
      THEN
        TCounters(module^.counters)^[i] := 
          MaxCounter (count, TCounters(module^.counters)^[i] );
      ELSE
        Error(msgInconsistencyModuleInfo, TModuleName(module^.modname)^, i); 
        RETURN; 
      END;
    ELSE
      Error(msgIncorrectModuleInfo, TModuleName(module^.modname)^, i); 
      RETURN; 
    END;
  END;
END MergeModuleInfo;

 
--------------------------------------------------------------------------------
PROCEDURE ReadUnRegisteredModuleInfo ( VAR file: FILERD
                                     ; header-: TModuleInfoHeader
                                     ; VAR unregModuleList: TModuleInfoPtr );
VAR module: TModuleInfoPtr;
    modname_len: sys.CARD32;
    i: TIndex;
    str: TString;
    pos: TStringPos; 
    index: TIndex;
    type:  TestConditionType;
    count: TCounter;
    name:  TString;
    line, end_line: TSourceLine;
    col, end_col:   TSourceCol;
BEGIN
  module := NewModuleInfo (header.count, header.modname);
  IF module = NIL THEN RETURN END;

  module^.modtime := header.modtime;

  modname_len := LENGTH(TModuleName(module^.modname)^);
  FOR i := 0 TO module^.count-1 DO
    pos := 0;
    IF readS (file, str)                 AND
       StrToIndex   (str, pos, index)    AND
       StrToType    (str, pos, type)     AND
       StrToCounter (str, pos, count)    AND
       StrToName    (str, pos, name)     AND
       StrToLine    (str, pos, line)     AND
       StrToCol     (str, pos, col)      AND   
       StrToLine    (str, pos, end_line) AND
       StrToCol     (str, pos, end_col)         
    THEN
      IF strcmp ( module^.modname, modname_len
                , sys.ADR(name), LENGTH(name) 
                ) = 0
      THEN
        TSourceRefs(module^.srcrefs)^[index].type     := type;
        TSourceRefs(module^.srcrefs)^[index].line     := line;
        TSourceRefs(module^.srcrefs)^[index].col      := col;
        TSourceRefs(module^.srcrefs)^[index].end_line := end_line;
        TSourceRefs(module^.srcrefs)^[index].end_col  := end_col;
        TCounters(module^.counters)^[index]           := count;
      ELSE
        Error(msgInconsistencyModuleInfo, TModuleName(module^.modname)^, i); 
        DisposeModule (module);
        RETURN; 
      END;
    ELSE
      Error(msgIncorrectModuleInfo, TModuleName(module^.modname)^, i); 
      DisposeModule (module);
      RETURN; 
    END;
  END;

  module^.next    := unregModuleList;
  unregModuleList := module;
END ReadUnRegisteredModuleInfo;
 

--------------------------------------------------------------------------------
PROCEDURE ReadStatistics ( file: FILE
                         ; regModuleList: TModuleInfoPtr
                         ; VAR unregModuleList: TModuleInfoPtr );
VAR rfile: FILERD;
    str: TString;
    modheader: TModuleInfoHeader;
    module: TModuleInfoPtr;
BEGIN
  AttachFILERD (rfile, file);
  WHILE readS (rfile, str) DO
    IF StrToModuleInfoHeader (str, modheader) THEN
      module := FindModuleInfoByName ( modheader.modname
                                     , regModuleList, unregModuleList );
      IF module # NIL THEN
        IF IsComptibleLogs (module, modheader) THEN
          MergeModuleInfo (rfile, module);
        END;
      ELSE
        ReadUnRegisteredModuleInfo (rfile, modheader, unregModuleList);
      END;
    END;
  END;
END ReadStatistics;
 

--------------------------------------------------------------------------------
PROCEDURE ReadTCFile ( fname-: ARRAY OF CHAR
                     ; regModuleList: TModuleInfoPtr
                     ; VAR unregModuleList: TModuleInfoPtr );
VAR file: FILE;
BEGIN
  file := OpenFile (fname);
  IF file # BADFILE THEN
    ReadStatistics (file, regModuleList, unregModuleList);
    CloseFile (file);
  END;
END ReadTCFile;


--------------------------------------------------------------------------------
--                     Save Test Coverage Information
--------------------------------------------------------------------------------

PROCEDURE WriteModuleModifyTime (file: FILE; time: TTime);
VAR ini, res: os.X2C_TimeStruct;
BEGIN
  writeDec (file, time, 0);
  writeS   (file, ' ', 1);

  os.X2C_GetTime (ini);
  ini.year  := 1970;
  ini.month := 1;
  ini.day   := 1;
  ini.hour  := 0;
  ini.min   := 0;
  ini.sec   := 0;
  ini.fracs := 0;
  os.X2C_TimeSecAdd (ini, time, res);

  writeDec (file, res.day, 1);
  writeS   (file, '.', 1);
  writeDec (file, res.month, 1);
  writeS   (file, '.', 1);
  writeDec (file, res.year, 1);
  writeS   (file, ' ', 1);

  writeDec (file, res.hour, 1);
  writeS   (file, ':', 1);
  writeDec (file, res.min, 1);
END WriteModuleModifyTime;


--------------------------------------------------------------------------------
PROCEDURE WriteModuleHeader (file: FILE; module: TModuleInfoPtr);
BEGIN
  writeS   (file, '     @', 1);
  writeS   (file, TestConditionNames[cr_File], MAX_TypeNameLen);
  writeS   (file, ' =', 1);
  writeDec (file, module^.count, 8);
  writeS   (file, ' ["', 1);
  writeS   (file, TModuleName(module^.modname)^, 1);
  writeS   (file, '" ', 1);
  WriteModuleModifyTime (file, module^.modtime);
  writeS   (file, ']', 1);
  writeLn  (file);
END WriteModuleHeader;


--------------------------------------------------------------------------------
PROCEDURE WriteModule (file: FILE; module: TModuleInfoPtr);
VAR i: TIndex;
BEGIN
  WriteModuleHeader (file, module);
  FOR i:= 0 TO module^.count-1 DO
    -- "%d @%s =%d [%s %d.%d]"
    writeDec (file, i, 4);
    writeS   (file, ' @', 1);
    writeS   (file, TestConditionNames[TSourceRefs(module^.srcrefs)^[i].type], MAX_TypeNameLen);
    writeS   (file, ' =', 1);
    writeDec (file, TCounters(module^.counters)^[i], 8);
    writeS   (file, ' ["', 1);
    writeS   (file, TModuleName(module^.modname)^, 1);
    writeS   (file, '" ', 1);
    writeDec (file, TSourceRefs(module^.srcrefs)^[i].line, 1);
    writeS   (file, '.', 1);
    writeDec (file, TSourceRefs(module^.srcrefs)^[i].col, 1);
    writeS   (file, ' ', 1);
    writeDec (file, TSourceRefs(module^.srcrefs)^[i].end_line, 1);
    writeS   (file, '.', 1);
    writeDec (file, TSourceRefs(module^.srcrefs)^[i].end_col, 1);
    writeS   (file, ']', 1);
    writeLn  (file);
  END;
END WriteModule;


--------------------------------------------------------------------------------
PROCEDURE WriteTCFile ( fname-: ARRAY OF CHAR
                      ; regModuleList: TModuleInfoPtr
                      ; VAR unregModuleList: TModuleInfoPtr
                      );
VAR file: FILE;
    tmp: TModuleInfoPtr;
BEGIN
  file := CreateFile (fname);
  IF file = BADFILE THEN  RETURN  END;

  WHILE regModuleList # NIL DO 
    WriteModule (file, regModuleList);
    regModuleList := regModuleList^.next;
  END;

  WHILE unregModuleList # NIL DO 
    WriteModule (file, unregModuleList);
    tmp := unregModuleList;
    unregModuleList := unregModuleList^.next;
    DisposeModule (tmp);
  END;

  CloseFile (file);
END WriteTCFile;


--------------------------------------------------------------------------------
PROCEDURE CancelTCFileOperation (VAR unregModuleList: TModuleInfoPtr);
VAR tmp: TModuleInfoPtr;             
BEGIN
  WHILE unregModuleList # NIL DO 
    tmp := unregModuleList;
    unregModuleList := unregModuleList^.next;
    DisposeModule (tmp);
  END;
END CancelTCFileOperation;


--------------------------------------------------------------------------------
-- Must be call earliest as possible. Discard module registration.
PROCEDURE ["C"] X2C_INIT_TESTCOVERAGE ();
BEGIN
  RegisteredModuleList := NIL;
END X2C_INIT_TESTCOVERAGE;


--------------------------------------------------------------------------------
-- Must be call latest as possible. Merge and store statistics. 
PROCEDURE ["C"] X2C_EXIT_TESTCOVERAGE ();
VAR fname: ARRAY [0..MAXPATHLEN-1] OF CHAR;
    unregModuleList: TModuleInfoPtr;
BEGIN
  IF RegisteredModuleList = NIL THEN RETURN END;

  ErrorCnt := 0;
  unregModuleList := NIL;
  UseCurrentDir := FALSE; 

  GetInfoFileName (fname);
  IF os.X2C_Exists (fname) THEN
    ReadTCFile (fname, RegisteredModuleList, unregModuleList);
    MakeBackUp (fname);
  END;

  WriteTCFile (fname, RegisteredModuleList, unregModuleList);
  Message (msgUpdate, fname);
END X2C_EXIT_TESTCOVERAGE;


--------------------------------------------------------------------------------
--                     Support Test Coverage Utilities
--------------------------------------------------------------------------------
PROCEDURE EnumFileLines (fname-: ARRAY OF CHAR; action: FILE_LINE_ITERATOR);
VAR file: FILE;
    rfile: FILERD;
    str: TString;
BEGIN
  file := OpenFile (fname);
  IF file # BADFILE THEN
    AttachFILERD (rfile, file);
    WHILE inS (rfile, str) DO
      action (str);
    END;
    CloseFile (file);
  END;
END EnumFileLines;


END xrTCov.
