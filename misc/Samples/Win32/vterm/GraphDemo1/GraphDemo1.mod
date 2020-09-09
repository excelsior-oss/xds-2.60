<* +M2EXTENSIONS *>
<* +M2ADDTYPES *>


MODULE GraphDemo1;
IMPORT VTerm,
       SysClock,
       SYSTEM,
       STextIO,
       SIOResult,
       Printf,
       IOConsts,
       SRealIO;

FROM RealMath IMPORT round,sqrt;

CONST
  W = 600;
  D = 400;
  dev = 10;
  dev1 = 20;
VAR
   Color               : LONGCARD;
   WIDTH,DEPTH         : INTEGER;
   ch                  : CHAR;
   Pattern             : ARRAY [0..4] OF  LONGINT;





(**********************************************************)
(*   RND                                                  *)
(**********************************************************)
<* PUSH *>
<* COVERFLOW - *>

CONST
  HistoryMax = 54;
VAR
  History    : ARRAY [0..HistoryMax] OF CARDINAL;
  HistoryPtr : CARDINAL;
  LowerPtr   : CARDINAL;

PROCEDURE SetUpHistory(Seed: CARDINAL);
VAR
  x : LONGCARD;
  i : CARDINAL;
BEGIN
  x := VAL(LONGCARD, Seed);
  FOR  i:= 0 TO HistoryMax DO
    x := x * 3141592621 + 17;
    History[i] := CARDINAL(x DIV 10000H);
  END;

  HistoryPtr := HistoryMax;
  LowerPtr := 23;
END SetUpHistory;


PROCEDURE RANDOM (Range :CARDINAL) :CARDINAL;
VAR
  res:CARDINAL;
BEGIN
  IF HistoryPtr = 0 THEN
    IF LowerPtr = 0 THEN
      SetUpHistory(12345);
    ELSE
      HistoryPtr := HistoryMax;
      DEC (LowerPtr);
    END;
  ELSE
    DEC (HistoryPtr);
    IF LowerPtr = 0 THEN
      LowerPtr := HistoryMax;
    ELSE
      DEC (LowerPtr);
    END;
  END;
  res := History[HistoryPtr]+History[LowerPtr];
  History[HistoryPtr] := res;
  IF Range = 0 THEN
    RETURN res;
  ELSE
    RETURN res MOD Range;
  END;
END RANDOM;

PROCEDURE RANDOMIZE;
VAR d :SysClock.DateTime;
BEGIN
  SysClock.GetClock (d);
  SetUpHistory( CARDINAL(d.fractions) * CARDINAL(d.second) );
END RANDOMIZE;


PROCEDURE RAND(): REAL;
VAR
  x: LONGCARD;
BEGIN
  x := RANDOM(MAX(SYSTEM.CARD16)) + RANDOM(MAX(SYSTEM.CARD16))*10000H;
  RETURN VAL(REAL, x) / (VAL (REAL, MAX(LONGCARD)) + 1.1);
END RAND;


<* POP *> (* Restore overflow checks *)




PROCEDURE Clear;
VAR
  i: CARDINAL;
BEGIN
  FOR i:=1 TO 20 DO
    STextIO.WriteLn;
  END;
END Clear;









(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                                                                                             *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Wait;
BEGIN
 REPEAT   UNTIL VTerm.KeyPressed(); 
 ch:=VTerm.GetKey();
 VTerm.ClearScreen(0);
 VTerm.Rectangle(dev,dev1,WIDTH,DEPTH,VTerm._clrBRIGHTWHITE,FALSE);
 VTerm.RawOutText(WIDTH DIV 2-8*16,DEPTH+1,14,"Press any key to continue");
END Wait;
(**********************************************************)
PROCEDURE Colors;
VAR
   beg,i,curr,w          : INTEGER;
BEGIN
 beg:=40;
 w:=(DEPTH-20) DIV 16;
 FOR i:=1 TO (WIDTH-2*beg) DIV 5 DO
   IF VTerm.KeyPressed() THEN RETURN  END;
   curr:=30;
   VTerm.RawOutText(beg,curr,VTerm._clrBLUE,"BLUE");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrBLUE,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrGREEN,"GREEN");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrGREEN,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrCYAN,"CYAN");
   VTerm.Rectangle((i-1)*5+beg,curr,i*5+beg,curr+w,VTerm._clrCYAN,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrRED,"RED");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrRED,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrMAGENTA,"MAGENTA");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrMAGENTA,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrBROWN,"BROWN");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrBROWN,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrWHITE,"WHITE");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrWHITE,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrGRAY,"GRAY");
   VTerm.Rectangle((i-1)*5+beg,curr,i*5+beg,curr+w,VTerm._clrGRAY,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrLIGHTBLUE,"LIGHTBLUE");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrLIGHTBLUE,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrLIGHTGREEN,"LIGHTGREEN ");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrLIGHTGREEN,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrLIGHTCYAN,"LIGHTCYAN");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrLIGHTCYAN,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrLIGHTRED,"LIGHTRED");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrLIGHTRED,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrLIGHTMAGENTA,"LIGHTMAGENTA");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrLIGHTMAGENTA,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrLIGHTYELLOW,"LIGHTYELLOW");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrLIGHTYELLOW,TRUE);
   INC(curr,w);

   VTerm.RawOutText(beg,curr,VTerm._clrBRIGHTWHITE,"BRIGHTWHITE");
   VTerm.Rectangle((i-1)*5+beg ,curr,i*5+beg,curr+w,VTerm._clrBRIGHTWHITE,TRUE);
   VTerm.Delay(30);
 END;  (*  For *)
END Colors;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*            Rectangles and Colors demonstration                                              *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_Rectangle_AND_Colors;
BEGIN
 VTerm.RawOutText(WIDTH DIV 2-8*13,2,VTerm._clrLIGHTYELLOW,"Rectangles demonstration");
 Colors;
END Demo_Rectangle_AND_Colors;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*            Line and FillMask Styles demonstration                                           *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_Style;
VAR
   beg,step,y        : CARDINAL;
   end               : CARDINAL;
BEGIN
  (*   DEMO Line Style                        *)
  beg:=40;
  end:=WIDTH-40;
  y:=60;
  step:=25;
  Color:=VTerm._clrBRIGHTWHITE;;
  VTerm.RawOutText(WIDTH DIV 2-8*16,2,VTerm._clrLIGHTYELLOW,"Line and Fill Style demonstration");

  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"SOLID - Line Style ");
  VTerm.SetLineStyle(VTerm.LNSTYLE_SOLID);
  VTerm.HLine(beg,y,end,Color);

  INC(y,step);
  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"DOT - Line Style ");
  VTerm.SetLineStyle(VTerm.LNSTYLE_DOT);
  VTerm.HLine(beg,y,end,Color);

  INC(y,step);
  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"SHORTDASH - Line Style ");
  VTerm.SetLineStyle(VTerm.LNSTYLE_SHORTDASH);
  VTerm.HLine(beg,y,end,Color);

  INC(y,step);
  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"DASHDOT - Line Style ");
  VTerm.SetLineStyle(VTerm.LNSTYLE_DASHDOT);
  VTerm.HLine(beg,y,end,Color);

  INC(y,step);
  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"DASHDOUBLEDOT - Line Style ");
  VTerm.SetLineStyle(VTerm.LNSTYLE_DASHDOUBLEDOT);
  VTerm.HLine(beg,y,end,Color);


  (*   DEMO FillMask                          *)
  end:=180;
  y:=200;
  step:=60;
  Color:=VTerm._clrLIGHTRED;
  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"HORIZ - FillMask");
  VTerm.SetStdFillMask(VTerm.PATSYM_HORIZ);
  VTerm.Rectangle(beg,y,end,y+40,Color,TRUE);
  INC(y,step);

  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"VERT - FillMask");
  VTerm.SetStdFillMask(VTerm.PATSYM_VERT);
  VTerm.Rectangle(beg,y,end,y+40,Color,TRUE);
  INC(y,step);

  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"DENSE1 - FillMask");
  VTerm.SetStdFillMask(VTerm.PATSYM_DENSE1);
  VTerm.Rectangle(beg,y,end,y+40,Color,TRUE);

  INC(beg,170);
  INC(end,170);
  y:=200;
  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"DENSE2 - FillMask");
  VTerm.SetStdFillMask(VTerm.PATSYM_DENSE2);
  VTerm.Rectangle(beg,y,end,y+40,Color,TRUE);
  INC(y,step);

  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"DIAG1 - FillMask");
  VTerm.SetStdFillMask(VTerm.PATSYM_DIAG1);
  VTerm.Rectangle(beg,y,end,y+40,Color,TRUE);
  INC(y,step);

  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"DIAG2 - FillMask");
  VTerm.SetStdFillMask(VTerm.PATSYM_DIAG2);
  VTerm.Rectangle(beg,y,end,y+40,Color,TRUE);

  INC(beg,170);
  INC(end,170);
  y:=200+step;
  VTerm.RawOutText(beg,y-15,VTerm._clrLIGHTYELLOW,"SOLID - FillMask");
  VTerm.SetStdFillMask(VTerm.PATSYM_SOLID);
  VTerm.Rectangle(beg,y,end,y+40,Color,TRUE);

  VTerm.SetLineStyle(VTerm.LNSTYLE_SOLID);
END Demo_Style;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                     User FillMask demonstration                                             *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_User_FillMask;
VAR
   UserMask            : VTerm.FillMaskType;
   beg,w,r             : INTEGER;

BEGIN
   VTerm.RawOutText(WIDTH DIV 2-8*12,2,VTerm._clrLIGHTYELLOW,"User FillMask demonstration");

   beg:=80;
   r:=100;
   w:=r+50;
   Color:=VTerm._clrLIGHTRED;

   UserMask[0]:=0FEH;
   UserMask[1]:=0F8H;
   UserMask[2]:=0F0H;
   UserMask[3]:=0F8H;
   UserMask[4]:=0DCH;
   UserMask[5]:=08EH;
   UserMask[6]:=087H;
   UserMask[7]:=003H;
   VTerm.SetFillMask(UserMask);
   VTerm.Rectangle(beg,beg,beg+r,beg+r,Color,TRUE);

   UserMask[0]:=0F0H;
   UserMask[1]:=0F0H;
   UserMask[2]:=0F0H;
   UserMask[3]:=0F0H;
   UserMask[4]:=00FH;
   UserMask[5]:=00FH;
   UserMask[6]:=00FH;
   UserMask[7]:=00FH;
   VTerm.SetFillMask(UserMask);
   VTerm.Rectangle(beg+w,beg,beg+w+r,beg+r,Color,TRUE);

   UserMask[0]:=0H;
   UserMask[1]:=010H;
   UserMask[2]:=010H;
   UserMask[3]:=07CH;
   UserMask[4]:=010H;
   UserMask[5]:=010H;
   UserMask[6]:=000H;
   UserMask[7]:=000H;
   VTerm.SetFillMask(UserMask);
   VTerm.Rectangle(beg,beg+w,beg+r,beg+w+r,Color,TRUE);

   UserMask[0]:=000H;
   UserMask[1]:=000H;
   UserMask[2]:=03CH;
   UserMask[3]:=03CH;
   UserMask[4]:=03CH;
   UserMask[5]:=03CH;
   UserMask[6]:=000H;
   UserMask[7]:=000H;
   VTerm.SetFillMask(UserMask);
   VTerm.Rectangle(beg+w,beg+w,beg+w+r,beg+w+r,Color,TRUE);

   UserMask[0]:=000H;
   UserMask[1]:=010H;
   UserMask[2]:=028H;
   UserMask[3]:=044H;
   UserMask[4]:=028H;
   UserMask[5]:=010H;
   UserMask[6]:=000H;
   UserMask[7]:=000H;
   VTerm.SetFillMask(UserMask);
   VTerm.Rectangle(beg+w+w,beg,beg+w+w+r,beg+r,Color,TRUE);

   UserMask[0]:=000H;
   UserMask[1]:=000H;
   UserMask[2]:=000H;
   UserMask[3]:=018H;
   UserMask[4]:=018H;
   UserMask[5]:=000H;
   UserMask[6]:=000H;
   UserMask[7]:=000H;
   VTerm.SetFillMask(UserMask);
   VTerm.Rectangle(beg+w+w,beg+w,beg+w+w+r,beg+w+r,Color,TRUE);
END Demo_User_FillMask;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                             Pie  demonstration                                              *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_Pie;
VAR
  r        : REAL;
  k        : INTEGER;
BEGIN
 VTerm.RawOutText(WIDTH DIV 2-8*8,2,VTerm._clrLIGHTYELLOW,"Pie demonstration");
 r:=12.0;
 Color:=1;
 k:=WIDTH DIV 5;
 WHILE r<=360.0 DO
  VTerm.Pie(WIDTH DIV 2-2*k,DEPTH DIV 2-k,WIDTH DIV 2+2*k,DEPTH DIV 2+k,r,20.0,Color,TRUE);
  r:=r+24.0;
  INC(Color,1);
 END;
END Demo_Pie;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                  Cube      demonstration                                                    *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_Cube;
VAR
  i,len,fill    : CARDINAL;
BEGIN
  VTerm.RawOutText(WIDTH DIV 2-8*9,2,VTerm._clrLIGHTYELLOW,"Cube demonstration");
  RANDOMIZE;
  FOR i:=1 TO   10 DO
   len:=RANDOM(280)+40;
   Color:=RANDOM(6)+10;
   fill:=RANDOM(5);
   VTerm.SetStdFillMask(Pattern[fill]);
   VTerm.Cube(FALSE,i*40,len,(i+1)*40,350,20,Color,TRUE);
   VTerm.Cube(TRUE,i*40,len,(i+1)*40,350,20,Color,FALSE);
  END;
  VTerm.SetStdFillMask(VTerm.PATSYM_SOLID);
  VTerm.Line(40,30,40,350,VTerm._clrBRIGHTWHITE);
  VTerm.HLine(40,350,500,VTerm._clrBRIGHTWHITE);
END Demo_Cube;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                FloodFill   demonstration                                                    *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_FloodFill;
VAR
 Boundary        : LONGCARD;
BEGIN
  VTerm.RawOutText(WIDTH DIV 2-8*12,2,VTerm._clrLIGHTYELLOW,"FloodFill demonstration");
  Boundary:=VTerm._clrLIGHTYELLOW;
  VTerm.Ellipse(WIDTH DIV 2, DEPTH DIV 2,WIDTH DIV 2-50,DEPTH DIV 2-50,Boundary,FALSE);
  VTerm.Circle(WIDTH DIV 2-100, DEPTH DIV 2,80,Boundary);
  VTerm.Line(WIDTH DIV 2+100,21,WIDTH DIV 2+100,DEPTH,Boundary);
  VTerm.Delay(300);
  VTerm.FloodFill (WIDTH DIV 2,DEPTH DIV 2,VTerm._clrLIGHTMAGENTA,Boundary);
  VTerm.Delay(300);
  VTerm.FloodFill (WIDTH DIV 2-100,DEPTH DIV 2,VTerm._clrCYAN,Boundary);
  VTerm.Delay(300);
  VTerm.FloodFill (WIDTH DIV 2+105,DEPTH DIV 2,VTerm._clrLIGHTRED,Boundary);
END Demo_FloodFill;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                  Palette         demonstration                                              *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_Palette;
VAR
    k,i,p,l  :  LONGCARD;
    Col,j    :  LONGCARD;
    red,blue : CARDINAL;

BEGIN
 VTerm.RawOutText(WIDTH DIV 2-13*8,0,VTerm._clrLIGHTYELLOW,"RemapPalette demonstration");
 l:=45;
 p:=DEPTH DIV 2-50;
 Col:=0F0F0F0H+15*0010101H;
 j:=WIDTH DIV 2-125;
 FOR i:=0 TO 250 DO
   Col:=Col+0010101H;
   k:=VTerm.RemapPalette(VTerm._clrRED,Col);
   VTerm.Line(j+i,p,j+i,p+l,VTerm._clrRED);
   IF VTerm.KeyPressed() THEN
       k:=VTerm.RemapPalette(VTerm._clrRED,VTerm._RED);
       RETURN
   END;
 END;
 INC(p,l);
 Col:=0F0H+15;
 FOR i:=0 TO 250 DO
   Col:=Col+1;
   k:=VTerm.RemapPalette(VTerm._clrRED,Col);
   VTerm.Line(j+i,p,j+i,p+l,VTerm._clrRED);
   IF VTerm.KeyPressed() THEN
       k:=VTerm.RemapPalette(VTerm._clrRED,VTerm._RED);
       RETURN
   END;
 END;
 INC(p,l);
 Col:=0F00000H+15*0010000H;
 FOR i:=0 TO 250 DO
   Col:=Col+0010000H;
   k:=VTerm.RemapPalette(VTerm._clrRED,Col);
   VTerm.Line(j+i,p,j+i,p+l,VTerm._clrRED);
   IF VTerm.KeyPressed() THEN
       k:=VTerm.RemapPalette(VTerm._clrRED,VTerm._RED);
       RETURN
   END;
 END;
 Wait;
 k:=VTerm.RemapPalette(VTerm._clrRED,VTerm._RED);
END Demo_Palette;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                          Plot    demonstration                                              *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_Plot;
VAR
   s       : ARRAY [1..10000] OF RECORD
                                   x  : CARDINAL;
                                   y  : CARDINAL;
                                 END;
   i       : INTEGER;
   x,y,col : LONGCARD;

BEGIN
  RANDOMIZE;
  VTerm.RawOutText(WIDTH DIV 2-8*9,2,VTerm._clrLIGHTYELLOW,"Plot demonstration");
  FOR i:=1 TO 10000 DO
    x:=RANDOM(WIDTH-dev)+dev+1;
    y:=RANDOM(DEPTH-dev1)+dev1;
    col:=RANDOM(16);
    s[i].x:=x;
    s[i].y:=y;
    VTerm.Plot(x,y,col);
  END;
 VTerm.Rectangle(dev,dev1,WIDTH,DEPTH,VTerm._clrBRIGHTWHITE,FALSE);
  VTerm.Delay(1500);
  FOR i:=1 TO 10000 DO
    VTerm.Plot(s[i].x,s[i].y,0);
  END;
 VTerm.Rectangle(dev,dev1,WIDTH,DEPTH,VTerm._clrBRIGHTWHITE,FALSE);
END Demo_Plot;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                          Line    demonstration                                              *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_Line;
VAR
  dX,dY,i        : INTEGER;
BEGIN
  VTerm.RawOutText(WIDTH DIV 2-8*9,2,VTerm._clrLIGHTYELLOW,"Line demonstration");
  VTerm.SetLineStyle(VTerm.LNSTYLE_SOLID);
  dX:=W DIV 40;
  dY:=D DIV 40;
  Color := VTerm._clrGREEN;
  FOR i:=0 TO 20 DO
   VTerm.Line(W DIV 2,D DIV 2-i*dY,W-i*dX,D DIV 2,Color);
   VTerm.Line(W DIV 2,D DIV 2+i*dY,i*dX,D DIV 2,Color);
   VTerm.Line(W DIV 2,D DIV 2-i*dY,i*dX,D DIV 2,Color);
   VTerm.Line(W DIV 2,D DIV 2+i*dY,W-i*dX,D DIV 2,Color);
  END;
  VTerm.Line(W DIV 2,D DIV 2,W DIV 2,D,Color);
END Demo_Line;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                    Polygon       demonstration                                              *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo_Polygon;
VAR
  px,py                      : ARRAY [0..7] OF LONGCARD;
  col,i,j                    : CARDINAL;	
  r,ha,hb,w,s,c,xC,yC,xj,yj  : REAL;
  x,y                        : ARRAY [0..5] OF REAL;

BEGIN
  VTerm.RawOutText(WIDTH DIV 2-8*10,2,VTerm._clrLIGHTYELLOW,"Polygon demonstration");
  w:=30.0;
  ha:=70.0;
  r:=ha*sqrt(3.0)/3.0;
  c:=-0.5;
  s:=0.5*sqrt(3.0);
  hb:=0.5*w/s;
  xC:=REAL(W)/2;
  yC:=REAL(D)/2-r;

  x[0]:=-ha-2*w/s-hb;  y[0]:=-r-w;
  x[1]:=x[0]+hb;  y[1]:=-r-2*w;
  x[2]:=-x[1];  y[2]:=y[1];
  x[3]:=hb;  y[3]:=2*r+w;
  x[4]:=0.0;  y[4]:=2*r;
  x[5]:=ha+hb;  y[5]:=y[0];
  col:=VTerm._clrRED;
  FOR i:=0 TO 2 DO
   FOR j:=0 TO 5 DO
    xj:=x[j];yj:=y[j];
    px[j]:=round((xC+xj));
    py[j]:=round((D)-(yC+yj));
    (*  turn on 120  degree *)
    x[j]:=c*xj-s*yj;
    y[j]:=s*xj+c*yj;
   END;
   px[6]:=px[0];
   py[6]:=py[0];
   CASE i OF
    0:col:=VTerm._clrLIGHTGREEN;|
    1:col:=VTerm._clrLIGHTRED;|
    2:col:=VTerm._clrLIGHTBLUE;
   END;
   VTerm.Polygon(7,px,py,col,TRUE);
   INC(col);
  END;
END Demo_Polygon;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                        Disc      demonstration                                              *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE  Demo_Disc;
VAR
  i,R,j           : CARDINAL;
  blue,red,t	  : LONGCARD;
BEGIN
 VTerm.RawOutText(WIDTH DIV 2-8*8,2,VTerm._clrLIGHTYELLOW,"Disc demonstration");
 blue:=VTerm._clrBLUE;
 red:=VTerm._clrRED;
 LOOP
   R:=160;
   FOR i:=1 TO 8 DO
     VTerm.Disc(WIDTH DIV 2,DEPTH DIV 2,R,blue);
     DEC(R,10);
     VTerm.Disc(WIDTH DIV 2,DEPTH DIV 2,R,red);
     DEC(R,10);
   END;
   t:=blue;
   blue:=red;
   red:=t;
   IF VTerm.KeyPressed() THEN RETURN  END;
   VTerm.Delay(150);
 END;
END Demo_Disc;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                 PutImage and GetImage demonstration                                         *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE  Demo_Get_Put_Image;
VAR i       : INTEGER;
    x,y,col : LONGCARD;
    h1,h2   : VTerm.HBITMAP;
    str: ARRAY [1..4] OF CHAR;

BEGIN
  RANDOMIZE;
  VTerm.RawOutText(WIDTH DIV 2-18*8,2,VTerm._clrLIGHTYELLOW,"PutImage and GetImage demonstration");
  FOR i:=1 TO 20000 DO
    x:=RANDOM(WIDTH-dev)+dev;
    y:=RANDOM(DEPTH-dev1)+dev1;
    col:=RANDOM(16);
    VTerm.Plot(x,y,col);
  END;
  VTerm.Rectangle(dev,dev1,WIDTH,DEPTH,VTerm._clrBRIGHTWHITE,FALSE);
  VTerm.GetImage(21,21,80,50,h1);
  VTerm.RawOutText(21,21,10,"Demo");
  VTerm.RawOutText(21,35,10,"Bitmap");
  VTerm.GetImage(21,21,80,50,h2);
  VTerm.PutImage(21,21,h1,VTerm._GPSET);
  LOOP
    x:=RANDOM(WIDTH-59)+11;
    y:=RANDOM(DEPTH-39)+21;
    VTerm.GetImage(x,y,x+69,y+29,h1);
    VTerm.PutImage(x,y,h2,VTerm._GOR);
    VTerm.Delay(500);
    VTerm.DelImage(h2);
    VTerm.PutImage(x,y,h1,VTerm._GPSET);
    VTerm.DelImage(h1);
    IF VTerm.KeyPressed() THEN RETURN END;
  END;
END Demo_Get_Put_Image;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                   Business diagram demonstration                                            *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Business_Diagramm_Demo;
TYPE
   Mas = ARRAY [1..5],[0..20] OF CHAR;
   Col = ARRAY [1..6] OF CARDINAL;
CONST
   Name  = Mas{"Gasprom",
               "RAO ES",
               "Lukoil",
               "Sidanko",
               "Almaz Russia Sacha"};

   Color = Col{VTerm._clrLIGHTGREEN,
               VTerm._clrLIGHTBLUE,
               VTerm._clrLIGHTRED,
               VTerm._clrLIGHTYELLOW,
               VTerm._clrLIGHTMAGENTA,
               VTerm._clrLIGHTCYAN};

VAR

  i,len,fill    : CARDINAL;
  Company       : ARRAY [1..6] OF
                    RECORD
                     Cost : REAL;
                     Pie  : REAL;
                    END;
  beg		: INTEGER;
  angle         : REAL;	
  res           : IOConsts.ReadResults;
  Bol           : BOOLEAN;
  tmpCol        : LONGCARD;

  PROCEDURE NotRight():BOOLEAN;  
  BEGIN
    res:=SIOResult.ReadResult();
    IF (res = IOConsts.wrongFormat) 
     OR(res = IOConsts.outOfRange) 
     OR(res = IOConsts.notKnown)
    THEN RETURN TRUE; END;
    RETURN FALSE;
  END NotRight;



BEGIN
  tmpCol:=VTerm.SetBkColor(0);
  VTerm.ClearScreen(0);
  VTerm.RawOutText(WIDTH DIV 2-8*9,2,VTerm._clrLIGHTYELLOW,"Business diagram demonstration");
  STextIO.SkipLine();

  Company[1].Pie:=25.0;
  Company[2].Pie:=5.0;
  Company[3].Pie:=4.0;
  Company[4].Pie:=10.0;
  Company[5].Pie:=3.0;
  Company[6].Pie:=53.0;


  Bol:=TRUE;
  FOR i:=1 TO 5  DO 
   IF Bol THEN 
     STextIO.WriteLn;
     STextIO.WriteString("Enter cost of one share for   ");
     STextIO.WriteString(Name[i]);
     STextIO.WriteString(" company :  ");
     SRealIO.ReadReal(Company[i].Cost);
     IF NotRight() THEN  Bol:=FALSE END;
     STextIO.SkipLine; 
   END;
  END;

  IF NOT Bol THEN
   Company[1].Cost:=100.10;
   Company[2].Cost:=75.32;
   Company[3].Cost:=98.47;
   Company[4].Cost:=12.56;
   Company[5].Cost:=50.32;
  END;

  -- Draw cube

  VTerm.SetStdFillMask(VTerm.PATSYM_DENSE1);
  beg:=20;
  FOR i:=5 TO 1 BY -1   DO
   VTerm.Cube(FALSE,beg,i*40,beg+round(Company[i].Cost),(i+1)*40,20,Color[i],TRUE);
   VTerm.Cube(TRUE,beg,i*40,beg+round(Company[i].Cost),(i+1)*40,20,Color[i],FALSE);
   VTerm.RawOutText(beg+beg,i*40+10,VTerm._clrLIGHTYELLOW,Name[i]);
  END;

  VTerm.SetStdFillMask(VTerm.PATSYM_SOLID);
  VTerm.Line(beg,30,beg,350,VTerm._clrBRIGHTWHITE);

  --Draw pie

  angle:=0.0;
  FOR i:=1 TO 6 DO
    VTerm.Pie(350,150,550,350,angle,Company[i].Pie*360.0/100.0,Color[i],TRUE);
    angle:=angle+Company[i].Pie*360.0/100.0;
  END;
  VTerm.RawOutText(480,200,VTerm._clrBRIGHTWHITE,"25%");
  VTerm.RawOutText(440,120,VTerm._clrBRIGHTWHITE,"5%");

  VTerm.Line(440,140,430,170,VTerm._clrBRIGHTWHITE);

  VTerm.RawOutText(395,125,VTerm._clrBRIGHTWHITE,"4%");
  VTerm.Line(395,145,405,170,VTerm._clrBRIGHTWHITE);

  VTerm.RawOutText(375,190,VTerm._clrBRIGHTWHITE,"10%");
  VTerm.RawOutText(325,205,VTerm._clrBRIGHTWHITE,"3%");
  VTerm.Line(345,215,360,217,VTerm._clrBRIGHTWHITE);

  VTerm.RawOutText(430,270,VTerm._clrBRIGHTWHITE,"51%");

  VTerm.RawOutText(350,370,VTerm._clrBRIGHTWHITE,"Part of VNP");

END Business_Diagramm_Demo;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                      Say good by                                                            *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE GoodBy;
BEGIN
  VTerm.RawOutText(WIDTH DIV 2-8*7,DEPTH DIV 2,VTerm._clrLIGHTYELLOW,"That's all!");
  Clear;

END GoodBy;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                                                                                             *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Demo;
BEGIN
 Demo_Rectangle_AND_Colors;
 Wait;
 Demo_Style;
 Wait;
 Demo_User_FillMask;
 Wait;
 Demo_Pie;
 Wait;
 Demo_Cube;
 Wait;
 Demo_FloodFill;
 Wait;
 Demo_Palette;
 Demo_Plot;
 Wait;
 Demo_Line;
 Wait;
 Demo_Polygon;
 Wait;
 Demo_Disc;
 Wait;
 Demo_Get_Put_Image;
 Wait;
 Business_Diagramm_Demo;
 Wait;
 GoodBy;
 Wait;
END Demo;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                                  Init                                                       *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
BEGIN
 WIDTH:=W-dev;
 DEPTH:=D-dev-5;
 IF NOT  VTerm.Init ( 0, 0, W, D , 200) THEN HALT END;
 VTerm.InitStdPalette;
 VTerm.ClearScreen(0);
 VTerm.Rectangle(dev,dev1,WIDTH,DEPTH,VTerm._clrBRIGHTWHITE,FALSE);
 VTerm.RawOutText(WIDTH DIV 2-8*16,DEPTH+1,14,"Press any key to continue");

 Pattern[0]:=VTerm.PATSYM_DENSE1;
 Pattern[1]:=VTerm.PATSYM_DENSE2;
 Pattern[2]:=VTerm.PATSYM_DIAG1;
 Pattern[3]:=VTerm.PATSYM_DIAG2;
 Pattern[4]:=VTerm.PATSYM_SOLID;

 Demo;
END GraphDemo1.