<* +M2EXTENSIONS *>
<* +M2ADDTYPES *>

MODULE GraphDemo2;

IMPORT VTerm,
       SysClock,
       SYSTEM,
       STextIO,
       SWholeIO,
       IOConsts,
       SLongIO,
       SIOResult;

FROM LongMath IMPORT round,sin,cos,ln,pi,sqrt;
FROM Strings IMPORT Length;
FROM SYSTEM IMPORT SHIFT, ADDRESS, CARD8;

TYPE
  Proc = PROCEDURE (LONGREAL):LONGREAL;
CONST
  WIDTH = 600;
  DEPTH = 400;

VAR
   ch  : CHAR;




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











(**********************************)
PROCEDURE Wait;
BEGIN
 WHILE NOT VTerm.KeyPressed() DO END;
 ch:=VTerm.GetKey();
 VTerm.ClearScreen(0);
END Wait;
(**********************************)
PROCEDURE Clear;
VAR
 i         : CARDINAL;
BEGIN
 FOR i:=1 TO 20 DO
   STextIO.WriteLn;
 END;
END Clear;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                                                                                             *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
VAR
   Rt,Lt,Up,Dn,t   : LONGREAL;
   Color           : LONGCARD;
(**********************************)
PROCEDURE StoX(i: LONGREAL):LONGREAL;
BEGIN
 RETURN Lt+i*(Rt-Lt)/WIDTH;
END StoX;
(**********************************)
PROCEDURE YtoS(y:LONGREAL):LONGREAL;
BEGIN
 RETURN (y-Dn)*DEPTH/(Up-Dn);
END YtoS;
(**********************************)
PROCEDURE XtoS(x:LONGREAL):LONGREAL;
VAR
  i: INTEGER;
BEGIN
  RETURN (x-Lt)*WIDTH/(Rt-Lt);
END XtoS;

(**********************************)
PROCEDURE Cub(x:LONGREAL): LONGREAL;
BEGIN
  RETURN x*x*x;
END Cub;
(**********************************)
PROCEDURE LN(x:LONGREAL): LONGREAL;
BEGIN
  IF x>0.0 THEN RETURN ln(x);
           ELSE RETURN 0.0; END;
END LN;
(**********************************)
PROCEDURE DrawFunc(function :Proc;Color :LONGCARD);
VAR
   i,q 	            : LONGREAL;
BEGIN
    i:=0.00;
    WHILE i<=FLOAT(WIDTH)  DO;
     q:=YtoS(function(StoX(i)));
     IF round(q)<DEPTH THEN
       VTerm.Plot(round(i),DEPTH-round(q),Color);
     END;
     i:=i+1;
    END;
END DrawFunc;
(**********************************)
PROCEDURE Gra;
VAR
  res       : IOConsts.ReadResults;
  Bol       : BOOLEAN;
BEGIN
  Bol:=TRUE;

  STextIO.WriteString("Enter left x (for example -5.0) : ");
  SLongIO.ReadReal(Lt);
  res:=SIOResult.ReadResult();
  IF (res = IOConsts.wrongFormat)
   OR(res = IOConsts.notKnown)
   OR(res = IOConsts.outOfRange)
  THEN STextIO.WriteString("Error enter");Bol:=FALSE; END;
  STextIO.SkipLine;

  IF Bol THEN
   STextIO.WriteString("Enter right x (10.0): ");
   SLongIO.ReadReal(Rt);
   res:=SIOResult.ReadResult();
   IF (res = IOConsts.wrongFormat)
    OR(res = IOConsts.notKnown)
    OR(res = IOConsts.outOfRange)
   THEN STextIO.WriteString("Error enter");Bol:=FALSE; END;
   STextIO.SkipLine;
  END;

  IF Bol THEN
   STextIO.WriteString("Enter down y (-10.0) : ");
   SLongIO.ReadReal(Dn);
   res:=SIOResult.ReadResult();
   IF (res = IOConsts.wrongFormat)
    OR(res = IOConsts.notKnown)
    OR(res = IOConsts.outOfRange)
   THEN STextIO.WriteString("Error enter");Bol:=FALSE; END;
   STextIO.SkipLine;
  END;

  IF Bol THEN
   STextIO.WriteString("Enter up y (10.0)   : ");
   SLongIO.ReadReal(Up);
   res:=SIOResult.ReadResult();
   IF (res = IOConsts.wrongFormat)
    OR(res = IOConsts.notKnown)
   OR(res = IOConsts.outOfRange)
   THEN STextIO.WriteString("Error enter");Bol:=FALSE; END;
   STextIO.SkipLine;
  END;
  IF NOT Bol THEN
   Rt:=10.0;
   Lt:=-5.0;
   Up:=10.0;
   Dn:=-10.0;
  END;

  IF Lt>Rt THEN
    t:=Lt;
    Lt:=Rt;
    Rt:=t;
  END;
  IF Dn>Up THEN
    t:=Up;
    Up:=Dn;
    Dn:=t;
  END;

  Color:=2;
  DrawFunc(sin,VTerm._clrGREEN);
  DrawFunc(cos,VTerm._clrCYAN);
  DrawFunc(Cub,VTerm._clrLIGHTYELLOW);
  DrawFunc(LN,VTerm._clrLIGHTRED);

  STextIO.WriteLn;
  STextIO.WriteString("Green  - sin ");
  STextIO.WriteLn;
  STextIO.WriteString("Cyan   - cos ");
  STextIO.WriteLn;
  STextIO.WriteString("Yellow - x*x*x ");
  STextIO.WriteLn;
  STextIO.WriteString("Red    - Ln ");
  STextIO.WriteLn;
  VTerm.Line(0,DEPTH-round(YtoS(0.0)),WIDTH,DEPTH-round(YtoS(0.0)),15);
  VTerm.Line(round(XtoS(0.0)),0,round(XtoS(0.0)),DEPTH,15);
  Wait;
  VTerm.SetTextWindow (1,1,70,30);
  VTerm.ClearScreen(2);
  Clear;
END Gra;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                                                                                             *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Papor;
VAR
  a,b,c,d,e,f,r,x,y,i,k   : REAL;
  newx,newy               : REAL;
BEGIN
  x:=0.0;
  y:=0.0;
  VTerm.RawOutText(400,20,14,"Папоротник");
  RANDOMIZE;
  LOOP
   r:=RAND();
   IF r<=0.1 THEN
           a:=0.0; b:=0.0 ; c:=0.0; d:=0.16; e:=0.0; f:=0.0;
   ELSIF (r<=0.85) THEN
           a:=0.85; b:=0.04; c:=-0.04; d:=0.85; e:=0.0; f:=1.60;
   ELSIF (r<=0.92) THEN
           a:=0.20; b:=-0.26; c:=0.23; d:=0.22; e:=0.0; f:=1.60;
   ELSE
           a:=-0.15; b:=0.28; c:=0.26; d:=0.24; e:=0.0; f:=0.44;
   END;
   newx:=(a*x)+(b*y)+e;
   IF newx<-3.0 THEN EXIT;     END;
   newy:=(c*x)+(d*y)+f;
   x:=newx;
   y:=newy;
   i:=100.0-x*100.0+192.0;
   k:=680.0-y*67.0;
   VTerm.Plot(round(i),round(k),2);
   IF VTerm.KeyPressed() THEN
      RETURN
   END;
  END;
END Papor;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                                                                                             *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Square;
VAR
  i                      : INTEGER;			
  alfa                   : REAL;	
  px,py,pz               : ARRAY [0..4] OF REAL;
  rezr,a,b               : REAL;
  ppx,ppy                : ARRAY [0..4] OF LONGCARD;
(**********************************)
PROCEDURE Polygon;
VAR
  i 		    : LONGCARD;
BEGIN
   VTerm.Polygon(5,ppx,ppy,0,TRUE);
   FOR i:=0 TO 3 DO
    ppx[i]:=round(px[i])+100;
    ppy[i]:=round(py[i])+100;
   END;
   ppx[4]:=ppx[0];
   ppy[4]:=ppy[0];

   VTerm.Polygon(5,ppx,ppy,VTerm._clrLIGHTRED,TRUE);
END Polygon;
(**********************************)
VAR
   len1,len2     : REAL;
   k             : CARDINAL;
BEGIN
  STextIO.WriteLn;
  STextIO.WriteString("You can rotate this square");
  STextIO.WriteLn;
  STextIO.WriteString("e - Up");
  STextIO.WriteLn;
  STextIO.WriteString("f - Right");
  STextIO.WriteLn;
  STextIO.WriteString("d - Down");
  STextIO.WriteLn;
  STextIO.WriteString("s - Left");
  STextIO.WriteLn;
  STextIO.WriteString("Another key - exit");

  alfa:=15.0;
  px[0]:=50.0;       py[0]:=50.0;
  len1:=100.0;
  len2:=100.0;
  px[1]:=px[0]+2*len1;  py[1]:=py[0];
  px[2]:=px[0]+2*len1;  py[2]:=py[0]+2*len2;
  px[3]:=px[0];         py[3]:=py[0]+2*len2;
  px[4]:=px[0];         py[4]:=py[0];
  FOR i:=0 TO 4 DO      pz[i]:=0.0;END;
  a:=px[0]+len1;
  b:=py[0]+len2;

  Polygon;
  LOOP
  IF VTerm.KeyPressed() THEN
    ch:=VTerm.GetKey();
    CASE ch OF
     'f':
      FOR i:=0 TO 3 DO
       rezr:=px[i];
       px[i]:=(px[i]-a)*cos(pi/alfa)+pz[i]*sin(pi/alfa)+a;
       pz[i]:=(a-rezr)*sin(pi/alfa)+pz[i]*cos(pi/alfa);
      END;|
     'e':
       FOR i:=0 TO 3 DO
         rezr:=py[i];
         py[i]:=(py[i]-b)*cos(pi/alfa)-pz[i]*sin(pi/alfa)+b;
         pz[i]:=(rezr-b)*sin(pi/alfa)+pz[i]*cos(pi/alfa);
       END;|
     's':
      FOR i:=0 TO 3 DO
       rezr:=px[i];
       px[i]:=(px[i]-a)*cos(pi/alfa)-pz[i]*sin(pi/alfa)+a;
       pz[i]:=(rezr-a)*sin(pi/alfa)+pz[i]*cos(pi/alfa);
      END;|
     'd':
       FOR i:=0 TO 3 DO
         rezr:=py[i];
         py[i]:=(py[i]-b)*cos(pi/alfa)+pz[i]*sin(pi/alfa)+b;
         pz[i]:=(b-rezr)*sin(pi/alfa)+pz[i]*cos(pi/alfa);
       END;
    ELSE
      EXIT;
    END; (*  Case *)
  Polygon;
  END;
 END; (*  While*)
 Clear;
 VTerm.ClearScreen(0);
END Square;
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                                                                                             *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
PROCEDURE Sphera;
VAR
   i,red,green              : CARDINAL;
   color                    : LONGCARD;
   k                        : CARDINAL;  	
   p                        : REAL;
   x,y                      : CARDINAL;
BEGIN
  i:=170;
  p:=0.0;
  x:=WIDTH DIV 2;
  y:=DEPTH DIV 2;
  LOOP
    k:=VAL(CARD8,round(p));
    color  := LONGCARD( SHIFT(BITSET(k),16));
    red    := VTerm.RemapPalette(VTerm._clrRED,color);
    p:=p+1.5;
    VTerm.Disc(x,y,i,VTerm._clrRED);
    IF i<1 THEN EXIT;END;
    DEC(i);
    IF i MOD 5 =0 THEN INC(x);
    DEC(y); END;
  END;
  red:=VTerm.RemapPalette(VTerm._clrRED,VTerm._RED);
END Sphera;

(*/////////////////////////////////////////////////////////////////////////////////////////////*)
(*                                                                                             *)
(*/////////////////////////////////////////////////////////////////////////////////////////////*)
CONST
   Len   = 50;
   BkColor = VTerm._clrBLACK;
   Begx   = 151;
   Begy   = 51;

VAR
   h1                :  VTerm.HBITMAP;
   count             :  INTEGER;--CARDINAL;	
   Delay,Step        :  INTEGER;
PROCEDURE Image;
VAR
  h1,h2  : VTerm.HBITMAP;
  i      : CARDINAL;	
  x,y,col: LONGCARD;

(**********************************)
PROCEDURE Right(x,y : INTEGER;pos : INTEGER);
VAR
   i  : INTEGER;
BEGIN
  DEC(x,Len);
  VTerm.GetImage(x,y,x+Len,y+Len,h1);
  FOR i:=2 TO pos DIV Step DO
     VTerm.PutImage(x+i*Step,y,h1,VTerm._GPSET);
     VTerm.Rectangle(x+(i-2)*Step,y,x+i*Step,y+Len,BkColor,TRUE);
     VTerm.Delay(Delay);
  END;
  VTerm.PutImage(x+Len,y,h1,VTerm._GPSET);
  VTerm.Rectangle(x,y,x,y,BkColor,TRUE);
  VTerm.DelImage(h1);
END Right;
(**********************************)
PROCEDURE Left(x,y : INTEGER;pos : INTEGER);
VAR
  i   : INTEGER;
BEGIN
  INC(x,Len);
  VTerm.GetImage(x,y,x+Len,y+Len,h1);
  FOR i:=2 TO pos DIV Step DO
     VTerm.PutImage(x-i*Step,y,h1,VTerm._GPSET);
     VTerm.Rectangle(x+Len-i*Step,y,x+Len-(i-2)*Step,y+Len,BkColor,TRUE);
     VTerm.Delay(Delay);
  END;
  VTerm.PutImage(x-Len,y,h1,VTerm._GPSET);
  VTerm.Rectangle(x,y,x+Len,y+Len,BkColor,TRUE);
  VTerm.DelImage(h1);
END Left;
(**********************************)
PROCEDURE Up(x,y : INTEGER;pos : INTEGER);
VAR
  i   : INTEGER;
BEGIN
  VTerm.GetImage(x,y-Len,x+Len,y,h1);
  FOR i:=2 TO pos DIV Step DO
     VTerm.PutImage(x,y-Len+i*Step,h1,VTerm._GPSET);
     VTerm.Rectangle(x,y-Len+(i-2)*Step,x+Len,y-Len+i*Step,BkColor,TRUE);
     VTerm.Delay(Delay);
  END;
  VTerm.PutImage(x,y,h1,VTerm._GPSET);
  VTerm.Rectangle(x,y-Len,x+Len,y,BkColor,TRUE);
  VTerm.DelImage(h1);
END Up;
(**********************************)
PROCEDURE Down(x,y : INTEGER;pos : INTEGER);
VAR
  i   : INTEGER;
BEGIN
  INC(y,Len);
  VTerm.GetImage(x,y,x+Len,y+Len,h1);
  FOR i:=2 TO pos DIV Step DO
     VTerm.PutImage(x,y-i*Step,h1,VTerm._GPSET);
     VTerm.Rectangle(x,y+Len-i*Step,x+Len,y+Len-(i-2)*Step,BkColor,TRUE);
     VTerm.Delay(Delay);
  END;
  VTerm.PutImage(x,y-Len,h1,VTerm._GPSET);
  VTerm.Rectangle(x,y,x+Len,y+Len,BkColor,TRUE);
  VTerm.DelImage(h1);
END Down;
(**********************************)
CONST
   Amount = 100;
VAR
   k,p   : INTEGER;--CARDINAL;
   Mem   : ARRAY [1..Amount] OF CARDINAL;
BEGIN

  Color:=VTerm._clrLIGHTYELLOW;;

  VTerm.Rectangle(WIDTH DIV 2-151,DEPTH DIV 2-151,WIDTH DIV 2+152,DEPTH DIV 2+152,VTerm._clrBRIGHTWHITE,TRUE);
  VTerm.Circle(WIDTH DIV 2,DEPTH DIV 2,150,(*VTerm._clrBRIGHTWHITE*)4);
  VTerm.Line(WIDTH DIV 2,0,WIDTH DIV 2,DEPTH,4);
  VTerm.Line(0,DEPTH DIV 2,WIDTH,DEPTH DIV 2,4);

  VTerm.Rectangle(Begx,Begy,Begx+Len,Begy+Len,BkColor,TRUE);

  x:=Begx;
  y:=Begy;
  VTerm.Rectangle(x,y,x+Len,y+Len,BkColor,TRUE);
  Delay:=0;
  Step:=30;

  count:=0;
  RANDOMIZE;
  LOOP
    k:=RANDOM(3000)+1;
    CASE (k MOD 4)+1 OF
     1: IF x<Begx+300-Len-1 THEN
           Left(x,y,Len);
           INC(x,Len);
           INC(count);
           Mem[count]:=2;
        END;|
     2: IF x>Begx+Len+1       THEN
           Right(x,y,Len);
           DEC(x,Len);
           INC(count);
           Mem[count]:=1;
        END;|
     3: IF y>Begy+Len+1       THEN
           Up(x,y,Len);
           DEC(y,Len);
           INC(count);
           Mem[count]:=4;
        END;|
     4: IF y<Begy+300-Len-1 THEN
           Down(x,y,Len);
           INC(y,Len);
           INC(count);
           Mem[count]:=3;
        END;
    END;
    IF count=Amount THEN EXIT; END;
  END;
  VTerm.Rectangle(x,y,x+Len,y+Len,BkColor,TRUE);
  k:=Amount+1;
  Delay:=5;
  Step:=2;
  LOOP
    DEC(k);
    IF k=0 THEN EXIT; END;
    CASE Mem[k] OF
     1:
        Left(x,y,Len);
        INC(x,Len);|

     2:
        Right(x,y,Len);
        DEC(x,Len); |

     3:
        Up(x,y,Len);
        DEC(y,Len); |

     4:
        Down(x,y,Len);
        INC(y,Len);

    END; -- Case
  END; --LOOP

END Image;
(**********************************)
BEGIN
  IF NOT  VTerm.Init ( 0, 0, WIDTH, DEPTH , 200) THEN HALT END;
  VTerm.ClearScreen(0);

  Gra;
  Square;
  Papor;
  Wait;
  Sphera;
  Wait;
  Image;

END GraphDemo2.