(*
   ========================================================
   ==            JPI-TopSpeed Modula-2 V2                ==
   ==            ------------------------                ==
   ==                 demo program:                      ==
   ==                 ------------                       ==
   ==            ( ported to XDS Modula-2 )              ==
   ==                                                    ==
   ==  Window Demonstration:                             ==
   ==                                                    ==
   ==  XDS TS-like Window and ISO Processes Modules      ==
   ==                                                    ==
   ========================================================
*)

MODULE WinDemoM;

<* IF NOT multithread THEN *>
  "Make in multithread model only"
<* END *>


IMPORT SYSTEM,
       Processes, Semaphores,
       Window, IO, Lib, Str;




PROCEDURE Intro ;
(* Display startup and demonstration description *)

TYPE
  W   = RECORD
          x,y :CARDINAL;
        END ;
  WW  = ARRAY[0..29] OF W;

CONST
  ww = WW {
            W{0,0} , W{5,0} ,W{10,0},W{15,0},W{20,0},
            W{10,2} ,W{10,4},W{10,6},W{10,8},W{10,10},
            W{5,10} ,W{0,10},W{35,4},W{35,2},W{35,0},
            W{40,0}, W{45,0},W{50,2},W{50,4},W{45,6},
            W{40,6}, W{35,6},W{35,8},W{35,10},W{65,0},
            W{65,2}, W{65,4},W{65,6},W{65,8},W{65,10}
          };

PROCEDURE IntroText ;

  PROCEDURE SetC ( cc, bwc :Window.Color ) ;
  BEGIN
    Window.TextColor (cc) ;
  END SetC ;

BEGIN
  SetC(Window.White,Window.LightGray);
  IO.WrLn ;  IO.WrStr('       This is a demonstration of the ');
  SetC(Window.Yellow,Window.White);    IO.WrStr('Window') ;
  SetC(Window.White,Window.LightGray); IO.WrStr(' and ') ;
  SetC(Window.Yellow,Window.White) ;   IO.WrStr('Process') ;
  SetC(Window.White,Window.LightGray); IO.WrStr(' Modules') ;
  IO.WrLn ;
  IO.WrLn ;
  SetC(Window.LightGray,Window.LightGray);
  IO.WrStr('           6 time-sliced processes are created, which output to,');IO.WrLn;
  IO.WrStr('           move, re-size and re-colour  4 overlapping windows');IO.WrLn;
  IO.WrLn ;
  SetC(Window.White,Window.White);
  IO.WrStr('     Press any key to start, Esc to finish and SpaceBar to freeze ');
END IntroText ;

VAR
   WD      : Window.WinDef ;
   w       : Window.WinType ;
   title   : ARRAY[0..39] OF CHAR ;
   Win     : ARRAY[0..29] OF Window.WinType ;
   i,j,wn  : CARDINAL ;
   num     : ARRAY[0..6] OF CHAR ;
   b,up    : BOOLEAN ;
   DescWin : Window.WinType ;
   delay   : CARDINAL ;
   exit,
   change  : BOOLEAN ;
CONST
   targetX = 32 ;
   targetY = 5 ;
BEGIN
  Window.CursorOff;
  FOR i := 0 TO 29 DO
    WITH WD DO
      IF i<30 THEN
        Background := Window.Color(i MOD 8) ;
        Foreground := Window.Color(15-i MOD 8) ;
        X1 := ww[i].x ; Y1 := ww[i].y ;
        X2 := X1+9 ;  Y2 := Y1+3 ;
      END ;
      CursorOn   := FALSE ;
      WrapOn     := FALSE ;
      Hidden     := FALSE ;
      FrameOn    := TRUE ;
      FrameDef   := Window.SingleFrame ;
      IF (Background=Window.Black) THEN
        FrameBack  := Window.Green ;
      ELSE
        FrameBack  := Background ;
      END ;
      (* Window.Color(ORD(Foreground)MOD 8) *) ;
      IF FrameBack=Window.LightGray THEN FrameFore  := Window.Black ;
                                    ELSE FrameFore  := Window.LightGray ;
      END ;
    END ;
    Win[i] := Window.Open(WD) ;
    Window.SetTitle(Win[i],'JPI',Window.CenterUpperTitle);
    Str.CardToStr(LONGCARD(i+1),num,10,b) ;
    (*
    IF (i=29)OR(ww[i].x+5<>ww[i+1].x) THEN IO.WrChar(' '); END ;
    *)
    IO.WrStr('TopSpeed');
    IO.WrLn ;
    IO.WrStr('Modula-2');
  END ;
  WD := Window.WinDef {0,15,75,24,Window.White,Window.Black,
                       FALSE,TRUE,FALSE,TRUE,
                       Window.DoubleFrame,Window.Blue,Window.LightGray};
  DescWin := Window.Open(WD) ;
  Window.SetTitle( DescWin, ' JPI TopSpeed Modula-2 : Window Demonstration ',Window.CenterUpperTitle);
  IntroText ;
  delay := 1000 ;
  i := 0 ;
  LOOP
    IF IO.KeyPressed() THEN EXIT END;
    IF delay>100 THEN DEC(delay,20)
    ELSIF delay>1 THEN DEC(delay) ;
    END ;
    Lib.Delay(delay) ;
    Window.PutOnTop(Win[i]) ;
    i := (i+1)MOD 30 ;
  END ;
  REPEAT
    exit := TRUE ;
    FOR i := 0 TO 29 DO
      Window.Info(Win[i],WD) ;
      change := TRUE ;
      IF    WD.X1<targetX-3 THEN INC(WD.X1,3);INC(WD.X2,3);
      ELSIF WD.X1>targetX+3 THEN DEC(WD.X1,3);DEC(WD.X2,3);
      ELSIF WD.Y1=targetY THEN change := FALSE ;
      END ;
      IF    WD.Y1<targetY THEN INC(WD.Y1);INC(WD.Y2);
      ELSIF WD.Y1>targetY THEN DEC(WD.Y1);DEC(WD.Y2);
      END ;
      IF change THEN
         Window.Change(Win[i],WD.X1,WD.Y1,WD.X2,WD.Y2) ;
         exit := FALSE ;
      END ;
    END ;
  UNTIL exit ;
  FOR i := 0 TO 30 DO
    w := Window.Top() ;
    Lib.Delay(10) ;
    Window.Close(w) ;
   END ;
  IF IO.RdKey()=CHR(27) THEN HALT END ;
END Intro;


(* Actual demo starts here *)
(* ----------------------- *)


(*---------------------------- process management ------------------------------------*)

(* Rough start/stop sheduler mechanism ( from the original Process module )
   can be implemented neatly through the ISO Semaphores module *)

VAR
  pid1, pid2,
  pid3, pid4,
  pid5, pid6 :Processes.ProcessId;

  signal     :Semaphores.SEMAPHORE;

  nStopProc  :CARDINAL;
CONST
  totalNProc = 6;


PROCEDURE Starter;    (* to start all process simultaneously *)
BEGIN
  nStopProc := 0;

  Processes.Activate ( pid1 );
  Processes.Activate ( pid2 );
  Processes.Activate ( pid3 );
  Processes.Activate ( pid4 );

  Processes.Activate ( pid5 );
  Processes.Activate ( pid6 );

  Processes.StopMe();
END Starter;


PROCEDURE RunStarter;
VAR
  pidStart :Processes.ProcessId;
BEGIN
  Processes.Start ( Starter, 16384, 5, NIL, pidStart );
END RunStarter;


PROCEDURE Stopper;
BEGIN
  IF ( IO.KeyPressed() )
    THEN INC (nStopProc);
         IF (nStopProc = totalNProc)
           THEN Semaphores.Release ( signal );
         END;
         Processes.SuspendMe();
  END;
END Stopper ;


(*------------------------------ window handling ------------------------------------*)

VAR
    W1,W2,W3,W4  :Window.WinType;
    pal          :Window.PaletteDef;


(* Random Colouring *)

  PROCEDURE RandPalette ( VAR p : Window.PaletteDef ) ;
  VAR
    i : SHORTCARD ;
  TYPE  BW3 = ARRAY[0..2] OF Window.Color ;
  CONST bw3 = BW3{Window.Black,Window.LightGray,Window.White};
  BEGIN
      FOR i := 0 TO Window.PaletteMax DO
            p[i].Fore := VAL(Window.Color,Lib.RANDOM(16)) ;
          REPEAT
            p[i].Back := VAL(Window.Color,Lib.RANDOM(8)) ;
          UNTIL p[i].Back<>p[i].Fore ;
      END ;
  END RandPalette ;

(* Random Movement *)

  PROCEDURE Rmove ( WH :Window.WinType ) ;
  VAR
    dx1,dy1     :CARDINAL;
    ix,iy,nx,ny :CARDINAL;
    W           :Window.p2WDescriptor;
  BEGIN
    W := Window.GetWDescriptor ( WH );
      REPEAT
        Stopper ;
        dx1 := W^.WDef.X1+Lib.RANDOM(16);
        dy1 := W^.WDef.Y1+Lib.RANDOM(8);
      UNTIL (dx1<80) AND (dy1<25); -- AND (dx1+W^.OWidth<=80) AND (dy1+W^.ODepth<=24);


      Window.Change ( WH, dx1, dy1, dx1+W^.OWidth-1, dy1+W^.ODepth-1 );
  END Rmove ;

(* Random Change Size *)

  PROCEDURE RChangeSize ( WH :Window.WinType ) ;
 (* this randomly changes the size of a window *)
  VAR
    nx,ny :CARDINAL ;
    W     :Window.p2WDescriptor;
  BEGIN
    W := Window.GetWDescriptor ( WH );

      REPEAT
        Stopper ;
        <* PUSH *>
        <* COVERFLOW - *>
        nx := W^.WDef.X2+(Lib.RANDOM(16)-8)*2 ;
        ny := W^.WDef.Y2+Lib.RANDOM(16)-8;
      UNTIL (nx-W^.WDef.X1<70) AND (ny-W^.WDef.Y1<18) AND
            (nx-W^.WDef.X1>10) AND (ny-W^.WDef.Y1>4);
        <* POP *> (* Restore overflow checks *)
      Window.Change(WH,W^.WDef.X1,W^.WDef.Y1,nx,ny);
  END RChangeSize ;


(*//////////////////////////////// Processes ////////////////////////////////*)

(* P1 prints out numbers in Hex, Dec and Binary *)

  PROCEDURE P1;
  VAR
    S :ARRAY[0..30] OF CHAR;
    i :CARDINAL;
    b :BOOLEAN ;
  BEGIN
    Window.Use(W1);
    LOOP
      FOR i := 0 TO 1000 DO
        Str.CardToStr(VAL(LONGCARD,i),S,2,b); IO.WrStr(S);
        IO.WrCharRep(' ',16-Str.Length(S));
        IO.WrCard(i,5) ;
        Stopper;
        IO.WrStr('   ') ;
        Str.CardToStr(VAL(LONGCARD,i),S,16,b); IO.WrStr(S);
        IO.WrCharRep(' ',5-Str.Length(S));
        IO.WrLn;
      END;
    END ;
  END P1;

(* P2 displays out a sideways barchart *)

  PROCEDURE P2;
  VAR i,j : CARDINAL;
      V : ARRAY[1..9] OF CARDINAL ;
      WD : Window.WinDef ;

    PROCEDURE DrawBar ( x,y : CARDINAL ; val : CARDINAL ; on : BOOLEAN ) ;
    BEGIN
      Stopper;
      IF ODD(y) THEN
        Window.TextColor(Window.Red) ;
      ELSE
        Window.TextColor(Window.LightGray) ;
      END ;
      IF val>0 THEN
        DEC(val) ;
        Window.GotoXY(x+val DIV 2,y) ;
        IF on THEN
          IF ODD(val) THEN IO.WrChar(CHR(219)) ELSE IO.WrChar(CHR(221)) END ;
        ELSE
          IF ODD(val) THEN IO.WrChar(CHR(221)) ELSE IO.WrChar(' ') END ;
        END ;
      END ;
    END DrawBar ;

  BEGIN
    Window.Use(W2) ;
    i := 0 ;
    FOR i := 1 TO 9 DO
      V[i] := Lib.RANDOM(50) ;
      FOR j := 0 TO V[i] DO
        DrawBar(1,i,j,TRUE) ;
      END ;
    END ;
    LOOP
      FOR i := 1 TO 9 DO
        Window.Info(W2,WD) ;
        IF i<WD.X2-WD.X1-2 THEN
          IF Lib.RANDOM(2)=0 THEN
            IF V[i]<50 THEN INC(V[i]) ; DrawBar(1,i,V[i],TRUE) END ;
          ELSE
            IF V[i]>0 THEN DrawBar(1,i,V[i],FALSE) ; DEC(V[i]) ; END ;
          END ;
        END ;
      END;
    END ;
  END P2;

(* P3 displays a character grid with randomly changing pallette colours *)

  PROCEDURE P3;
  VAR
      X,Y,D : CARDINAL;
      CH : CHAR;
  BEGIN
    CH := '*' ;
    D := 0 ;
    Window.Use(W3) ;
    LOOP
      FOR X := 1 TO 8 DO
        FOR Y := 1 TO 8 DO
          Window.SetPaletteColor(VAL(SHORTCARD,(Y*7+X+D))MOD 8) ;
          Window.GotoXY(X,Y);
          IO.WrChar(CH);
          Window.GotoXY(1,10);
          Window.SetPaletteColor(0) ;
          IO.WrCard(X,2);
          IO.WrChar(',');
          IO.WrCard(Y,2);
          Stopper ;
        END;
      END;
      RandPalette ( pal ) ;
      Window.SetPalette(W3,pal) ;
      INC(D) ;
    END ;
  END P3;

(* P4 displays some text *)

  PROCEDURE P4;
  VAR
    i   :CARDINAL ;
    w4d :Window.p2WDescriptor;
  BEGIN
    w4d := Window.GetWDescriptor ( W4 );
    Window.Use(W4) ;
    LOOP
      FOR i := 1 TO 50 DO
        IO.WrStr('This is a test of 6 time-sliced processes, and also of window writing, moving, rearranging and resizing.');
        w4d^.WDef.Foreground := Window.White ;
        IO.WrStr(' Press any key to terminate, SpaceBar to freeze.  ');
        w4d^.WDef.Foreground := Window.Black ;
        Stopper;
      END;
    END;
  END P4;

(* P5 Re-sizes and Re-arranges text *)

  PROCEDURE P5;
  BEGIN
    LOOP
      Stopper;
      CASE Lib.RANDOM(4) OF
        0 : Window.PutOnTop(W1) |
        1 : Window.PutOnTop(W2) |
        2 : Window.PutOnTop(W3) |
        3 : Window.PutOnTop(W4)
      END ;

      CASE Lib.RANDOM(4) OF
        0 : RChangeSize(W1) |
        1 : |
        2 : |
        3 : RChangeSize(W4)
      END ;
    END  ;
  END P5;

(* P6 Moves window 1 *)

  PROCEDURE P6;
  BEGIN
    LOOP
      Rmove (W1);
      Rmove (W2);
      Rmove (W3);
      Rmove (W4);
    END;
  END P6;





PROCEDURE Demo ;
VAR
   WD        :Window.WinDef ;
   TW        :Window.WinType;
   cE        :CHAR;
   notCurSI  :CARDINAL;

PROCEDURE flushBuf;
(* flush key buffer *)
BEGIN
  WHILE IO.KeyPressed() DO cE := IO.RdKey() END;
END flushBuf;

PROCEDURE waitKey() :CHAR;
VAR
  c :CHAR;
BEGIN
  WHILE NOT IO.KeyPressed() DO END;
  c := IO.RdKey();
  IF ( c = 0C )
    THEN c := IO.RdKey();
  END;
  RETURN c;
END waitKey;



BEGIN
   Window.CursorOff ;
    WITH WD DO
      X1 := 10 ; Y1 := 2 ;
      X2 := 60 ; Y2 := 8 ;
      Foreground := Window.White ;
      Background := Window.Red ;
      CursorOn   := FALSE ;
      WrapOn     := TRUE ;
      Hidden     := FALSE ;
      FrameOn    := TRUE ;
      FrameDef   := Window.SingleFrame ;
      FrameFore  := Window.White ;
      FrameBack  := Background ;
    END ;
    W1 := Window.Open(WD) ;
    Window.SetTitle(W1,' Process 1 ',Window.CenterUpperTitle);
    WITH WD DO
      X1 := 40 ; Y1 := 2 ;
      X2 := 70 ; Y2 := 12 ;
      Foreground := Window.White ;
      Background := Window.Blue ;
      CursorOn   := FALSE ;
      WrapOn     := TRUE ;
      Hidden     := FALSE ;
      FrameOn    := TRUE ;
      FrameDef   := Window.DoubleFrame ;
      FrameFore  := Window.Black ;
      FrameBack  := Window.Cyan ;
    END ;
    W2 := Window.Open(WD) ;
    Window.SetTitle(W2,' Process 2 ',Window.CenterUpperTitle);
    RandPalette(pal) ;
    WITH WD DO
      X1 := 40 ; Y1 := 6 ;
      X2 := 50 ; Y2 := 17 ;
      pal[0].Fore:= Window.Yellow ;
      pal[0].Back:= Window.Black ;
      CursorOn   := FALSE ;
      WrapOn     := TRUE ;
      Hidden     := FALSE ;
      FrameOn    := TRUE ;
      FrameDef   := Window.SingleFrame ;
      pal[1].Fore:= Window.White ;
      pal[1].Back:= Window.Green ;
    END ;
    W3 := Window.PaletteOpen(WD,pal) ;
    Window.SetTitle(W3,'Process 3',Window.CenterUpperTitle);
    WITH WD DO
      X1 := 10 ; Y1 := 10 ;
      X2 := 70 ; Y2 := 20 ;
      Foreground := Window.Black ;
      Background := Window.LightGray ;
      CursorOn   := FALSE ;
      WrapOn     := TRUE ;
      Hidden     := FALSE ;
      FrameOn    := TRUE ;
      FrameDef   := Window.DoubleFrame ;
      FrameFore  := Window.White ;
      FrameBack  := Window.Green ;
    END ;
    W4 := Window.Open(WD);
    Window.SetTitle ( W4,' Process 4 ',Window.CenterUpperTitle);


    Processes.Create ( P1, 16384, 2, NIL, pid1 );
    Processes.Create ( P2, 16384, 2, NIL, pid2 );
    Processes.Create ( P3, 16384, 2, NIL, pid3 );
    Processes.Create ( P4, 16384, 2, NIL, pid4 );
    Processes.Create ( P5, 16384, 2, NIL, pid5 );
    Processes.Create ( P6, 16384, 2, NIL, pid6 );

    Semaphores.Create ( signal, 0 );
    flushBuf;

    LOOP
      RunStarter;

      Semaphores.Claim ( signal );

      cE := waitKey();
      IF ( cE # ' ' ) THEN EXIT END;
      cE := waitKey();
    END;

    Window.Close ( W1 );
    Window.Close ( W2 );
    Window.Close ( W3 );
    Window.Close ( W4 );

    Semaphores.Destroy ( signal );
    flushBuf;

END Demo;

BEGIN
  Intro;
  Demo;
END WinDemoM.

