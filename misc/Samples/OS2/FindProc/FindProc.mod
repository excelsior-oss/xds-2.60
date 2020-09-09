(* Copyright (c) 1996 xTech Ltd, Russia. All Rights Reserved. *)

(* Sample tool for XDS IDE: Finding procedure headings        *)

<*+ M2EXTENSIONS *>

MODULE FindProc;

IMPORT SYSTEM;
IMPORT ProgramArgs, TextIO, STextIO, IOChan, RndFile, IOResult;
IMPORT Strings;

IMPORT IDE := xShell, xFilePos;


VAR
  ide  : BOOLEAN;
  name : ARRAY [0..255] OF CHAR;
  file : RndFile.ChanId;
  size : INTEGER;
  line : CARDINAL;

PROCEDURE FatalError(s: ARRAY OF CHAR);
BEGIN
  IF ide THEN
    IDE.String(s);
  ELSE
    STextIO.WriteString(s);
    STextIO.WriteLn;
  END;
  HALT;
END FatalError;

PROCEDURE OpenFile;
VAR
  res  : RndFile.OpenResults;
BEGIN
  IF NOT ProgramArgs.IsArgPresent() THEN
    FatalError("Too few arguments");
  END;
  TextIO.ReadToken(ProgramArgs.ArgChan(),name);
  RndFile.OpenOld(file,name,RndFile.text,res);
  IF res <> RndFile.opened THEN
    FatalError("Failed to open a file");
  END;
END OpenFile;

PROCEDURE ReadFile;
VAR
  buf   : ARRAY [0..255] OF CHAR;
  found : BOOLEAN;
  pos   : CARDINAL;
BEGIN
  line := 1;
  IF ide THEN
    IDE.Comment(name);
    IF xFilePos.PosToInt(size,RndFile.EndPos(file)) THEN
      IDE.StartJob("Scanning",size);
    END;
  END;
  LOOP
    TextIO.ReadString(file,buf);
    CASE IOResult.ReadResult(file) OF
    |IOResult.allRight:
      Strings.FindNext('PROCEDURE', buf, 0, found, pos);
      IF found THEN
        IF ide THEN
          IDE.Error(IDE.MSG_NOTICE,0,pos,line,name,buf);
          IDE.String(buf);
          IDE.String(15C+12C);
        ELSE
          STextIO.WriteString(buf);
          STextIO.WriteLn;
        END;
      END;
    |IOResult.endOfLine:
      TextIO.SkipLine(file); INC(line);
    |IOResult.endOfInput:
      EXIT
    END;
    IF ide THEN
      IF xFilePos.PosToInt(size,RndFile.CurrentPos(file)) THEN
        IDE.Progress(-VAL(INTEGER,line),-size);
      END
    END;
  END;
END ReadFile;

PROCEDURE CloseFile;
BEGIN
  RndFile.Close(file);
END CloseFile;

BEGIN
  ide := IDE.Start();
  IF ide THEN
    IDE.TurnSortingOff;
    IDE.Caption("Finding procedures...");
  END;
  OpenFile;
  ReadFile;
  CloseFile;
  IF ide THEN
    IDE.Caption("Done");
  END;
FINALLY
  IF ide THEN IDE.Finish; END;
END FindProc.
