MODULE Golygon;

IMPORT STextIO, SWholeIO, IOConsts, SIOResult, StdChans;

IMPORT VTerm;

CONST
  WIDTH = 600;
  DEPTH = 300;
  x0    = WIDTH DIV 2;
  y0    = DEPTH DIV 2;

PROCEDURE FindGolygon(l : INTEGER; x,y: INTEGER; h: BOOLEAN): BOOLEAN;
BEGIN
  IF l = 0 THEN
    RETURN (x=0) AND (y=0)
  END;
  IF h THEN
    IF FindGolygon(l-1, x+l, y, FALSE) THEN
      VTerm.Line(x0+x,y0+y,x0+x+l,y0+y, VTerm._clrLIGHTCYAN);
      RETURN TRUE 
    ELSIF FindGolygon(l-1, x-l, y, FALSE) THEN
      VTerm.Line(x0+x,y0+y,x0+x-l,y0+y, VTerm._clrLIGHTCYAN);
      RETURN TRUE 
    END;
  ELSE
    IF FindGolygon(l-1, x, y+l, TRUE) THEN
      VTerm.Line(x0+x,y0+y,x0+x,y0+y+l, VTerm._clrLIGHTCYAN);
      RETURN TRUE
    ELSIF FindGolygon(l-1, x, y-l, TRUE) THEN 
      VTerm.Line(x0+x,y0+y,x0+x,y0+y-l, VTerm._clrLIGHTCYAN);
      RETURN TRUE 
    END;
  END;
  RETURN FALSE; 
END FindGolygon;
      
VAR
  l : INTEGER;
  res : IOConsts.ReadResults;

BEGIN
  IF NOT  VTerm.Init ( 0, 0, WIDTH, DEPTH , 200) THEN HALT END;
  VTerm.SetLineStyle(VTerm.LNSTYLE_SOLID);

  LOOP
    STextIO.WriteString("Enter the last segment length (0 for exit): ");
    LOOP
      SWholeIO.ReadInt(l); 
      res:=SIOResult.ReadResult();
      IF res=IOConsts.endOfLine THEN STextIO.SkipLine ELSE EXIT END;
    END;
    IF res=IOConsts.endOfInput THEN EXIT END;
    IF res # IOConsts.allRight THEN 
      STextIO.SkipLine 
    ELSE
      IF l=0 THEN EXIT END;
      VTerm.ClearScreen(VTerm._GCLEARSCREEN);
      STextIO.WriteString("Golygon ");
      IF FindGolygon(l-1, l, 0, FALSE) THEN
        VTerm.Line(x0,y0,x0+l,y0, VTerm._clrLIGHTCYAN);
      ELSE
        STextIO.WriteString("NOT ");
      END;
      STextIO.WriteString("found.");
      STextIO.WriteLn();
    END;
  END;

END Golygon.
