IMPLEMENTATION MODULE m2test;

IMPORT STextIO, SWholeIO, c_test;

PROCEDURE ["C"] m2func(a,b: INTEGER): INTEGER;
BEGIN
  STextIO.WriteString("m2func(");
  SWholeIO.WriteInt(a,0); STextIO.WriteChar(',');
  SWholeIO.WriteInt(b,0); STextIO.WriteChar(')');
  STextIO.WriteLn;
  STextIO.WriteString("And now calling 'c_func' from Modula-2");
  STextIO.WriteLn;
  RETURN c_test.c_func (a, b)
END m2func;

END m2test.

