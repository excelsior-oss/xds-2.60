IMPLEMENTATION MODULE dll1;

IMPORT STextIO, SWholeIO;

PROCEDURE PrintVar;
BEGIN
  STextIO.WriteString("Var = ");
  SWholeIO.WriteCard(Var,0);
  STextIO.WriteLn;
END PrintVar;

PROCEDURE Proc;
BEGIN
  STextIO.WriteString("This is Proc");
  STextIO.WriteLn;
  PrintVar;
END Proc;

BEGIN
  STextIO.WriteString("dll1 initialization");
  STextIO.WriteLn;
  Var := 1;
  PrintVar;
FINALLY
  STextIO.WriteString("dll1 finalization");
  STextIO.WriteLn;
  PrintVar;
END dll1.