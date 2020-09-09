<* M2EXTENSIONS+ *>

MODULE exe2;

IMPORT STextIO;

IMPORT dllRTS;

VAR
  hmod: dllRTS.HMOD;
  Proc: PROC;
  Var : POINTER TO CARDINAL;

BEGIN 
  hmod := dllRTS.LoadModule(".\DLL2.DLL");
  IF hmod <> dllRTS.InvalidHandle THEN
    Proc := dllRTS.GetProcAdr(hmod,"Proc");
    IF Proc = NIL THEN
      STextIO.WriteString("Failed to locate Proc");
      STextIO.WriteLn;
      HALT;
    END;
    Var  := dllRTS.GetVarAdr(hmod,"Var");
    IF Var = NIL THEN
      STextIO.WriteString("Failed to locate Var");
      STextIO.WriteLn;
      HALT;
    END;
    Var^ := 2;
    Proc;
  ELSE
    STextIO.WriteString("Failed to load DLL");
    STextIO.WriteLn;
  END;
END exe2.