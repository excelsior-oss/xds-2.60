<* M2EXTENSIONS+ *>

IMPLEMENTATION MODULE dll2;

IMPORT STextIO, SWholeIO;

PROCEDURE PrintVar;
BEGIN
  STextIO.WriteString("Var = ");
  SWholeIO.WriteCard(Var,0);
  STextIO.WriteLn;
END PrintVar;

PROCEDURE ["C"] Proc;
BEGIN
  STextIO.WriteString("This is Proc");
  STextIO.WriteLn;
  PrintVar;
END Proc;

BEGIN
  STextIO.WriteString("dll2 initialization");
  STextIO.WriteLn;
  Var := 1;
  PrintVar;
FINALLY
  STextIO.WriteString("dll2 finalization");
  STextIO.WriteLn;
  PrintVar;
END dll2.