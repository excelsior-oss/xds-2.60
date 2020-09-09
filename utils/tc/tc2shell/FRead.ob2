MODULE FRead;


IMPORT
  sys    := SYSTEM
 ,os     := xrtsOS
 ,fconst := xosFiles
 ,xlibOS
 ;

TYPE
  FILE*         = os.X2C_OSFILE;
  STR_ITERATOR* = PROCEDURE (str: ARRAY OF CHAR; progress : LONGINT);

--------------------------------------------------------------------------------
CONST
  TAB         = 11C;
  CR*         = 15C;
  LF*         = 12C;
  COMMENT     = '%';
  SPACE       = ' ';
  NEWLINE     = "" + CR + LF;
--------------------------------------------------------------------------------
  StringSIZE  = 1024;
  ReadBufSIZE = StringSIZE*4;

TYPE
  FILERD = RECORD
    file: FILE;
    buf:  ARRAY ReadBufSIZE+1 OF CHAR;
    len:  sys.CARD32;
    pos:  sys.CARD32;
    fpos: LONGINT;
  END; 
--------------------------------------------------------------------------------

PROCEDURE inS (VAR file: FILERD; VAR str: ARRAY OF CHAR): BOOLEAN;
VAR
  i, h: LONGINT;
BEGIN
  str[0] := 0C;
  -- skip second part of line separator
  IF file.buf[file.pos] = LF THEN
    file.pos := file.pos+1;
  END;

  i := 0;
  h := LEN(str);
  -- extract string from buffer
  WHILE i < h DO

    -- update buffer
    IF file.pos >= file.len THEN
      file.pos := 0;
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
      ELSIF file.buf[file.pos] = LF THEN
        file.pos := file.pos+1;
      END;
      file.fpos := file.fpos + ReadBufSIZE;
    END;

    -- check line separator
    IF file.buf[file.pos] = CR THEN
      str[i] := 0C;
      file.pos := file.pos+1;
      RETURN TRUE;
    END;

    str[i] := file.buf[file.pos];
    INC(i);
    file.pos := file.pos+1;
  END;

  ASSERT(FALSE);
END inS;

PROCEDURE readS (VAR file: FILERD; VAR str: ARRAY OF CHAR): BOOLEAN;
VAR res: BOOLEAN;
BEGIN
  REPEAT
    res := inS (file, str);
  UNTIL (NOT res) OR (str[0] # COMMENT);
  RETURN res;
END readS;

--------------------------------------------------------------------------------

PROCEDURE Open*(VAR file : FILE; name : ARRAY OF CHAR) : BOOLEAN;
BEGIN
  RETURN os.X2C_FileOpenRead(file, name) = 0;
END Open;

PROCEDURE GetFilePos*(file : FILE) : LONGINT;
VAR ofs : LONGINT;
BEGIN
  ofs := 0;
  os.X2C_FileSeek(file, ofs, fconst.X2C_fSeekCur);
  RETURN ofs;
END GetFilePos;

PROCEDURE GetFileSize*(file : FILE) : LONGINT;
VAR save, ofs: LONGINT;
BEGIN
  save := GetFilePos(file);
  ofs  := 0;
  os.X2C_FileSeek(file, ofs,  fconst.X2C_fSeekEnd);
  os.X2C_FileSeek(file, save, fconst.X2C_fSeekSet);
  RETURN ofs;
END GetFileSize;

PROCEDURE EnumFileLines*(file : FILE; siProc : STR_ITERATOR);
VAR
  rfile     : FILERD;
  str       : ARRAY StringSIZE OF CHAR;
  flen      : LONGINT;
BEGIN
  rfile.file := file;
  rfile.pos  := ReadBufSIZE;
  rfile.len  := ReadBufSIZE;
  rfile.fpos := 0 - ReadBufSIZE;
  flen       := GetFileSize(file);
  IF (flen # 0) THEN
    WHILE readS (rfile, str) DO
      siProc(str, (rfile.fpos + VAL(LONGINT, rfile.pos))*101 DIV flen);
    END;
  END;
END EnumFileLines;


PROCEDURE Close*(file : FILE);
BEGIN
  sys.EVAL(os.X2C_FileClose(file));
END Close;

(* returns 0 when error *)
PROCEDURE GetFileTime*(name-: ARRAY OF CHAR) : sys.CARD32;
VAR time: sys.CARD32;
    ok  : BOOLEAN;
BEGIN
  xlibOS.X2C_ModifyTime (name, time, ok);
  IF ok THEN
    RETURN time;
  ELSE
    RETURN 0;
  END;
END GetFileTime;

END FRead.
