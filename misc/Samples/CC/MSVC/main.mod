MODULE main;

IMPORT Test, InOut;

VAR str: ARRAY [0..79] OF CHAR;

BEGIN
    str := "This string will be converted to uppercase";
    Test._StrToUpper (str);
    InOut.WriteString (str);
    InOut.WriteLn;
END main.
