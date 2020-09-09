<*+M2ADDTYPES   *>
<*+M2EXTENSIONS *>

MODULE MGdemo;

IMPORT Graph,  Lib, IO, SYSTEM;

FROM SYSTEM IMPORT SHIFT, ADDRESS, CARD8 , CAST;

FROM LongMath  IMPORT cos,sin,round,pi;

TYPE
   SmallColorArray = ARRAY [0..3]  OF CARDINAL;

VAR
  Patterns  :ARRAY [0..4] OF LONGINT;
  spotlist  :ARRAY [0..5] OF ARRAY [0..5] OF Graph.GraphCoords;
  NumPColor :CARDINAL;


PROCEDURE TestKey();
VAR
  c: CHAR;
BEGIN
  c:=IO.RdKey();
  IF (c = CHAR('x')) OR (c = CHAR('X')) THEN
        HALT;
  END;
END TestKey;

PROCEDURE RandColor(): CARDINAL;
BEGIN
  RETURN  Lib.RANDOM ( NumPColor );
END RandColor;


(*------------------------------------- Animation -----------------------------------------*)

PROCEDURE DrawCircles(patnum, start, depth, width: INTEGER);
VAR
  n: INTEGER;
  x, y, xpos, ypos: CARDINAL;
BEGIN
    n:=0;
    LOOP
        x:=spotlist[patnum-1][n].xcoord;
        IF x = 0 THEN EXIT END;
        xpos:=CARDINAL(SHIFT(BITSET(width),-2))*x;
        y:=spotlist[patnum-1][n].ycoord;
        ypos:=CARDINAL(SHIFT(BITSET(depth),-3))*y+CARDINAL(start);
        Graph.Disc (xpos, ypos, 4, NumPColor-1);
        INC(n);
        IF n = 6 THEN
            EXIT;
        END;
    END;
END DrawCircles;


PROCEDURE MoveSideways(image: Graph.HBITMAP; pos: ARRAY OF CARDINAL;
                       limit: CARDINAL; incr: INTEGER): CARDINAL;

VAR
    x, y: CARDINAL;
    ret: INTEGER;
BEGIN
    x:=0;
    y:=0;
    ret:=0;
    LOOP
        IF x = pos[limit] THEN EXIT END;
        Graph.PutImage(x, y, image, Graph._GXOR);
        INC(x, incr);
        Graph.PutImage(x, y, image, Graph._GPSET);
    END;
    RETURN ret;
END MoveSideways;

PROCEDURE MoveDown(image: Graph.HBITMAP; pos: ARRAY OF CARDINAL;
                   limit: CARDINAL; xpos: CARDINAL; incr: INTEGER): INTEGER;

VAR
    y: CARDINAL;
BEGIN
    y:=0;
    LOOP
        IF y = pos[limit] THEN EXIT END;
        Graph.PutImage(xpos, y, image, Graph._GXOR);
        INC(y, incr);
        Graph.PutImage(xpos, y, image, Graph._GPSET);
    END;
    RETURN 0;
END MoveDown;

PROCEDURE DrawSpots(number, start, depth, width: INTEGER);
VAR
    pattern: ARRAY [0..2] OF CARDINAL;
    n: CARDINAL;
BEGIN
    IF number = 0 THEN RETURN END;
    CASE number OF
        | 1:
            pattern[0]:=1;
            pattern[1]:=0;
        | 2:
            pattern[0]:=2;
            pattern[1]:=0;
        | 3:
            pattern[0]:=2;
            pattern[1]:=1;
            pattern[2]:=0;
        | 4:
            pattern[0]:=4;
            pattern[1]:=0;
        | 5:
            pattern[0]:=4;
            pattern[1]:=1;
            pattern[2]:=0;
        | 6:
            pattern[0]:=6;
            pattern[1]:=0;
    END;
    n:=0;
    WHILE pattern[n] # 0 DO
        DrawCircles(pattern[n], start, depth, width);
        INC(n);
    END;
END DrawSpots;


PROCEDURE DrawDomino(depth, width: INTEGER);
VAR
  spots: INTEGER;
BEGIN
  Graph.Rectangle(0, 0, width, depth, 0, Graph._GFILLINTERIOR);
  Graph.Rectangle(0, 0, width, depth, NumPColor-1, Graph._GBORDER);
  Graph.Line(0, depth DIV 2, width, depth DIV 2, NumPColor-1);
  spots:=Lib.RANDOM(7);
  DrawSpots(spots, 0, depth, width);
  spots:=Lib.RANDOM(7);
  DrawSpots(spots, depth DIV 2, depth, width);
END DrawDomino;


PROCEDURE MakeDomino(depth, width: INTEGER) :Graph.HBITMAP;
VAR
  handle :Graph.HBITMAP;
BEGIN
    DrawDomino(depth, width);
    Graph.GetImage(0, 0, width, depth, handle);
    RETURN handle;
END MakeDomino;

PROCEDURE Animation();
VAR
  ColPosition: ARRAY [0..10] OF CARDINAL;
  RowPosition: ARRAY [0..2] OF CARDINAL;
  count, width, depth, incr: INTEGER;
  MaxRow, MaxCol, clear: INTEGER;
  CurrentRow, CurrentCol: INTEGER;
  image :Graph.HBITMAP;
BEGIN
    spotlist[0][0]:= Graph.GraphCoords{2, 2};
    spotlist[1][0]:= Graph.GraphCoords{1, 1};
    spotlist[1][1]:= Graph.GraphCoords{3, 3};
    spotlist[2][0]:= Graph.GraphCoords{0, 0};
    spotlist[3][0]:= Graph.GraphCoords{1, 1};
    spotlist[3][1]:= Graph.GraphCoords{3, 1};
    spotlist[3][2]:= Graph.GraphCoords{1, 3};
    spotlist[3][3]:= Graph.GraphCoords{3, 3};
    spotlist[4][0]:= Graph.GraphCoords{0, 0};
    spotlist[5][0]:= Graph.GraphCoords{1, 1};
    spotlist[5][1]:= Graph.GraphCoords{3, 1};
    spotlist[5][2]:= Graph.GraphCoords{1, 2};
    spotlist[5][3]:= Graph.GraphCoords{3, 2};
    spotlist[5][4]:= Graph.GraphCoords{1, 3};
    spotlist[5][5]:= Graph.GraphCoords{3, 3};
    Graph.ClearScreen(Graph._GCLEARSCREEN);
    Graph.SetStdFillMask(Patterns[2]);
        width:=50;
        depth:=100;
        MaxRow:=2;
        MaxCol:=9;
        clear:=20;
        incr:=10;
    count:=0;
    WHILE count <= MaxCol DO
        ColPosition[count]:=count*(width+10);
        INC(count);
    END;
    count:=0;
    WHILE count <= MaxRow DO
        RowPosition[count]:=count*(depth+clear);
        INC(count);
    END;
    CurrentCol:=MaxCol;
    CurrentRow:=MaxRow;

    LOOP
        IF CurrentCol < 0 THEN EXIT END;
        image:=MakeDomino(depth, width);
        IF CurrentCol > 0 THEN
            IF MoveSideways(image, ColPosition, CurrentCol, incr) # 0 THEN
                EXIT;
            END;
        END;
        IF CurrentRow > 0 THEN
            IF MoveDown(image, RowPosition, CurrentRow, ColPosition[CurrentCol], incr) # 0 THEN
                EXIT;
            END;
            DEC(CurrentRow);
        ELSE
            DEC(CurrentCol);
            CurrentRow:=MaxRow;
        END;

        Graph.DelImage ( image );
        IF IO.KeyPressed() THEN EXIT END;
    END;
    TestKey();
END Animation;


(*------------------------------------- Lines -----------------------------------------*)

PROCEDURE DrawLines();

VAR
    x, y, col: CARDINAL;
    MaxX, MaxY: CARDINAL;
    top, left, bottom, right: CARDINAL;
    i, limit: INTEGER;
BEGIN
    Graph.ClearScreen(Graph._GCLEARSCREEN);
    top:=0;
    left:=0;
    bottom:=Graph.Depth - 1;
    right :=Graph.Width - 1;
    MaxX:=Graph.Width - 1;
    MaxY:=Graph.Depth - 1;
    limit:=NumPColor-1;
    col:=0;
    i := 0;
    LOOP
    WHILE i <= limit DO
        INC(col);
        Graph.SetClipRgn(left, top, right, bottom);
        INC(top, 4);
        INC(left, 4);
        DEC(bottom, 4);
        DEC(right, 4);
        col := RandColor();
        x := 0;
        WHILE x <= MaxX DO
            Graph.Line(x, 0, MaxX - x,MaxY, col);
            INC(x, 4);
        END;
        y := 0;
        WHILE y <= MaxY DO
            Graph.Line(MaxX, y, 0, MaxY - y, col);
            INC(y, 2);
        END;
        INC(i);
        IF IO.KeyPressed() THEN
            EXIT;
        END;

    END;
    EXIT;
    END; (* LOOP *)
    Graph.CancelClipRgn;
    Graph.ClearScreen(Graph._GCLEARSCREEN);
    TestKey();
END DrawLines;



(*-------------------------------------- Ellipses ---------------------------------------*)

PROCEDURE DrawEllipses();

VAR
  n :CARDINAL;
  f: BOOLEAN;
  x1, x2, y1, y2: CARDINAL;
  c :CARDINAL;
BEGIN
    n:=0;
    f:=Graph._GFILLINTERIOR;
    LOOP
        IF (n = 1000) THEN EXIT END;
        INC(n);

        IF IO.KeyPressed() THEN
            EXIT;
        END;
        Lib.Delay ( 50 );

        Graph.SetStdFillMask(Patterns[Lib.RANDOM(5)]);
        x1:=Lib.RANDOM(Graph.Width);
        y1:=Lib.RANDOM(Graph.Depth);
        x2:=Lib.RANDOM(Graph.Width);
        y2:=Lib.RANDOM(Graph.Depth);
        IF (f = Graph._GFILLINTERIOR)
          THEN f:=Graph._GBORDER;
          ELSE f:=Graph._GFILLINTERIOR;
        END;

        c := Graph.SetBkColor ( RandColor() );

        Graph.Ellipse (x1, y1, x2, y2, RandColor(), f);
    END;
    TestKey();
END DrawEllipses;

PROCEDURE Ellipses(scale: CARDINAL);
VAR
  c :CARDINAL;
BEGIN
  Lib.RANDOMIZE;
  Graph.CancelClipRgn();
  Graph.ClearScreen(Graph._GCLEARSCREEN);
  DrawEllipses();

  c := Graph.SetBkColor ( Graph._BLACK );
  Graph.ClearScreen(Graph._GCLEARSCREEN);

  Graph.Rectangle(49 DIV scale, 24 DIV scale, 501 DIV scale, 291 DIV scale,
                  Graph._clrBRIGHTWHITE, Graph._GBORDER);
  Graph.SetClipRgn(50 DIV scale, 25 DIV scale, 500 DIV scale, 290 DIV scale);
  DrawEllipses();
  Graph.CancelClipRgn();

  c := Graph.SetBkColor ( Graph._BLACK );
  Graph.ClearScreen(Graph._GCLEARSCREEN);
END Ellipses;


(*-------------------------------- Triangles ---------------------------------------------*)

PROCEDURE InitPalette();
VAR
  i :CARDINAL;
  red, green, blue :CARDINAL;
  color  :LONGCARD;
  colors :ARRAY [0..255] OF LONGCARD;
BEGIN
    i:=0;
    WHILE i < 256 DO

       red     := VAL (CARD8, i MOD 16 * 16);
       green   := VAL (CARD8, (i DIV 8 MOD 8) * 32);
       blue    := VAL (CARD8, (i DIV 32) * 32);

       color := LONGCARD( SHIFT(BITSET(red),16)) +
                LONGCARD( SHIFT(BITSET(green),8)) + LONGCARD(blue);
      colors[i]:=color;
      INC(i);
    END;
    color := Graph.RemapAllPalette(colors);
END InitPalette;


(*------------------------------------ Intro ----------------------------------------*)

PROCEDURE DrawLogo (scale :INTEGER; Cols- :SmallColorArray);
BEGIN
    Graph.Rectangle(140 DIV scale, 20 DIV scale, 500 DIV scale, 330 DIV scale, Cols[0], Graph._GFILLINTERIOR);
    Graph.Line(225 DIV scale, 110 DIV scale, 320 DIV scale, 140 DIV scale, Cols[1]);
    Graph.Line(320 DIV scale, 140 DIV scale, 415 DIV scale, 110 DIV scale, Cols[1]);
    Graph.Line(415 DIV scale, 110 DIV scale, 460 DIV scale, 125 DIV scale, Cols[1]);
    Graph.Line(460 DIV scale, 125 DIV scale, 460 DIV scale, 225 DIV scale, Cols[1]);
    Graph.Line(460 DIV scale, 225 DIV scale, 365 DIV scale, 255 DIV scale, Cols[1]);
    Graph.Line(365 DIV scale, 255 DIV scale, 365 DIV scale, 285 DIV scale, Cols[1]);
    Graph.Line(365 DIV scale, 285 DIV scale, 320 DIV scale, 300 DIV scale, Cols[1]);
    Graph.Line(320 DIV scale, 300 DIV scale, 180 DIV scale, 255 DIV scale, Cols[1]);
    Graph.Line(180 DIV scale, 255 DIV scale, 180 DIV scale, 190 DIV scale, Cols[1]);
    Graph.Line(180 DIV scale, 190 DIV scale, 225 DIV scale, 205 DIV scale, Cols[1]);
    Graph.Line(225 DIV scale, 205 DIV scale, 225 DIV scale, 235 DIV scale, Cols[1]);
    Graph.Line(225 DIV scale, 235 DIV scale, 270 DIV scale, 250 DIV scale, Cols[1]);
    Graph.Line(270 DIV scale, 250 DIV scale, 270 DIV scale, 190 DIV scale, Cols[1]);
    Graph.Line(270 DIV scale, 190 DIV scale, 180 DIV scale, 160 DIV scale, Cols[1]);
    Graph.Line(180 DIV scale, 160 DIV scale, 180 DIV scale, 125 DIV scale, Cols[1]);
    Graph.Line(180 DIV scale, 125 DIV scale, 225 DIV scale, 110 DIV scale, Cols[1]);

    Graph.Line(180 DIV scale, 125 DIV scale, 320 DIV scale, 170 DIV scale, Cols[1]);
    Graph.Line(320 DIV scale, 170 DIV scale, 460 DIV scale, 125 DIV scale, Cols[1]);
    Graph.Line(320 DIV scale, 170 DIV scale, 320 DIV scale, 300 DIV scale, Cols[1]);

    Graph.Line(365 DIV scale, 190 DIV scale, 415 DIV scale, 175 DIV scale, Cols[1]);
    Graph.Line(415 DIV scale, 175 DIV scale, 415 DIV scale, 210 DIV scale, Cols[1]);
    Graph.Line(415 DIV scale, 210 DIV scale, 365 DIV scale, 225 DIV scale, Cols[1]);
    Graph.Line(365 DIV scale, 225 DIV scale, 365 DIV scale, 190 DIV scale, Cols[1]);
    Graph.Line(365 DIV scale, 190 DIV scale, 415 DIV scale, 210 DIV scale, Cols[1]);

    Graph.Line(225 DIV scale, 205 DIV scale, 270 DIV scale, 190 DIV scale, Cols[1]);
    Graph.Line(225 DIV scale, 235 DIV scale, 270 DIV scale, 220 DIV scale, Cols[1]);
    Graph.Line(180 DIV scale, 190 DIV scale, 225 DIV scale, 175 DIV scale, Cols[1]);

    Graph.FloodFill(250 DIV scale, 130 DIV scale, Cols[3], Cols[1]);
    Graph.FloodFill(230 DIV scale, 180 DIV scale, Cols[3], Cols[1]);
    Graph.FloodFill(260 DIV scale, 240 DIV scale, Cols[3], Cols[1]);
    Graph.FloodFill(380 DIV scale, 210 DIV scale, Cols[3], Cols[1]);

    Graph.FloodFill(200 DIV scale, 230 DIV scale, Cols[2], Cols[1]);
    Graph.FloodFill(250 DIV scale, 220 DIV scale, Cols[2], Cols[1]);
    Graph.FloodFill(400 DIV scale, 190 DIV scale, Cols[2], Cols[1]);
    Graph.FloodFill(440 DIV scale, 190 DIV scale, Cols[2], Cols[1]);
END DrawLogo;


PROCEDURE Intro();
VAR
  c: SmallColorArray;
BEGIN
  NumPColor := Graph.NumPColor;
  IF ( NumPColor > 256 ) THEN NumPColor := 256 END;

  c := SmallColorArray {9, 8, 15, 2};
  Graph.RawOutText( 200, 420, Graph._clrLIGHTRED, 'XDS TopSpeed-like Graphics');
  DrawLogo(1, c);
  TestKey();
  Graph.ClearScreen(Graph._GCLEARSCREEN);
END Intro;


(*-------------------------------------  Pies -----------------------------------------*)

PROCEDURE Bars ( scale :INTEGER );
VAR
  x1, y1,x2, y2, x3, y3, x4, y4: CARDINAL;

CONST
  nParts = 6;
TYPE
  tParts  = ARRAY [0..nParts-1] OF LONGREAL;
  tColors = ARRAY [0..nParts-1] OF LONGCARD;
CONST
  Parts  = tParts  { 60.0, 50.0, 80.0, 60.0, 40.0, 70.0 };
  Colors = tColors { Graph._clrLIGHTYELLOW,  Graph._clrLIGHTBLUE,  Graph._clrLIGHTGREEN,
                     Graph._clrLIGHTMAGENTA, Graph._clrLIGHTRED, Graph._clrBRIGHTWHITE };

TYPE apt = ARRAY [0..nParts-1] OF Graph.FillMaskType;

CONST
  Patts = apt { Graph.FillMaskType{0AAH,  55H,  0AAH,  55H,  0AAH,  55H,  0AAH,  55H},
                Graph.FillMaskType{088H, 088H, 088H, 088H, 088H, 088H, 088H, 088H},
                Graph.FillMaskType{0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH},
                Graph.FillMaskType{0FFH, 0CCH,  0CCH, 0FFH,  0FFH, 0CCH,  0CCH, 0FFH},
                Graph.FillMaskType{0FFH, 0FFH, 0FFH, 000H, 0FFH, 0FFH, 0FFH, 000H},
                Graph.FillMaskType{01H,  02H,  04H,  08H,  10H,  20H,  40H,  80H}    };

VAR
  i        :CARDINAL;
  curCoord , preCoord :RECORD
                         x  : INTEGER;
                         y  : INTEGER;
                       END;

BEGIN
    Graph.CancelClipRgn();
    Graph.ClearScreen(Graph._GCLEARSCREEN);

    (* bars *)

    x1:=19 DIV scale;
    y1:=10 DIV scale;
    x2:=x1;
    y2:=321 DIV scale;
    x3:=600 DIV scale;
    y3:=y2;

    Graph.Line(x1, y1, x2, y2, Graph._clrBRIGHTWHITE);
    Graph.Line(x2, y2, x3, y3, Graph._clrBRIGHTWHITE);
    Graph.SetStdFillMask(Graph.PATSYM_DIAG2);

    x1:=20 DIV scale;
    y1:=20 DIV scale;
    x2:=59 DIV scale;
    y2:=320 DIV scale;
    Graph.Cube(Graph._GTOP, x1, y1, x2, y2, 20 DIV scale, Graph._clrGREEN, Graph._GFILLINTERIOR);
    Graph.Cube(Graph._GTOP, x1, y1, x2, y2, 20 DIV scale, Graph._clrGREEN, Graph._GBORDER);

    x1:=60 DIV scale;
    y1:=120 DIV scale;
    x2:=99 DIV scale;
    y2:=320 DIV scale;
    Graph.Cube(Graph._GTOP, x1, y1, x2, y2, 20 DIV scale, Graph._clrGRAY, Graph._GFILLINTERIOR);
    Graph.Cube(Graph._GTOP, x1, y1, x2, y2, 20 DIV scale, Graph._clrGRAY, Graph._GBORDER);

    x1:=100 DIV scale;
    y1:=80 DIV scale;
    x2:=139 DIV scale;
    y2:=320 DIV scale;
    Graph.Cube(Graph._GTOP, x1, y1, x2, y2, 20 DIV scale, Graph._clrBLUE, Graph._GFILLINTERIOR );
    Graph.Cube(Graph._GTOP, x1, y1, x2, y2, 20 DIV scale, Graph._clrBLUE, Graph._GBORDER );


    x1:=140 DIV scale;
    y1:=200 DIV scale;
    x2:=179 DIV scale;
    y2:=320 DIV scale;
    Graph.Cube(Graph._GTOP, x1, y1, x2, y2, 20 DIV scale, Graph._clrMAGENTA, Graph._GFILLINTERIOR);
    Graph.Cube(Graph._GTOP, x1, y1, x2, y2, 20 DIV scale, Graph._clrMAGENTA, Graph._GBORDER);

    (* pies *)

    preCoord.x:=450;
    preCoord.y:=180;
    FOR i := 0 TO nParts-2 DO
      Graph.SetFillMask ( Patts[i] );
      curCoord.x:=round(FLOAT( preCoord.x - 420 )*cos( Parts[i]*pi/180.0 ) )+
                  round(FLOAT( preCoord.y - 180 )*sin( Parts[i]*pi/180.0 ) ) +420;

      curCoord.y:=-round(FLOAT(preCoord.x - 420 )*sin( Parts[i]*pi/180.0))+
                   round(FLOAT(preCoord.y - 180 )*cos( Parts[i]*pi/180.0))+180;

      Graph.Pie ( 420, 180,  80,  80, SYSTEM.CAST(LONGCARD ,preCoord.x), SYSTEM.CAST(LONGCARD ,preCoord.y) , SYSTEM.CAST(LONGCARD ,curCoord.x),  SYSTEM.CAST(LONGCARD ,curCoord.y), Colors[i], TRUE );
      preCoord.x:=curCoord.x;
      preCoord.y:=curCoord.y;
    END;

    Graph.SetFillMask ( Patts[0] );
    curCoord.x:=round(FLOAT( preCoord.x - 420 )*cos( Parts[nParts-1]*pi/180.0 ) )+
                round(FLOAT( preCoord.y - 180 )*sin( Parts[nParts-1]*pi/180.0 ) ) +420+10;

    curCoord.y:=-round(FLOAT(preCoord.x - 420 )*sin( Parts[nParts-1]*pi/180.0))+
                 round(FLOAT(preCoord.y - 180 )*cos( Parts[nParts-1]*pi/180.0))+180+10;

    INC(preCoord.x,10);
    INC(preCoord.y,10);
    Graph.Pie ( 430, 190,  80,  80, SYSTEM.CAST(LONGCARD ,preCoord.x), SYSTEM.CAST(LONGCARD ,preCoord.y) , SYSTEM.CAST(LONGCARD ,curCoord.x),  SYSTEM.CAST(LONGCARD ,curCoord.y), Colors[nParts-1], TRUE );


    preCoord.x:=450;
    preCoord.y:=180;
    FOR i := 0 TO nParts-2 DO
      curCoord.x:=round(FLOAT( preCoord.x - 420 )*cos( Parts[i]*pi/180.0 ) )+
                  round(FLOAT( preCoord.y - 180 )*sin( Parts[i]*pi/180.0 ) ) +420;

      curCoord.y:=-round(FLOAT(preCoord.x - 420 )*sin( Parts[i]*pi/180.0))+
                   round(FLOAT(preCoord.y - 180 )*cos( Parts[i]*pi/180.0))+180;

      Graph.Pie ( 420, 180,  80,  80, SYSTEM.CAST(LONGCARD ,preCoord.x), SYSTEM.CAST(LONGCARD ,preCoord.y) , SYSTEM.CAST(LONGCARD ,curCoord.x),  SYSTEM.CAST(LONGCARD ,curCoord.y), 0, FALSE );
      preCoord.x:=curCoord.x;
      preCoord.y:=curCoord.y;
    END;

    Graph.RawOutText (40, 400, Graph._clrLIGHTRED, "Bar Graphs and Pie Charts");
    Graph.RawOutText (40, 400, Graph._clrLIGHTRED, "Bar Graphs and Pie Charts");


    TestKey();
END Bars;

BEGIN
  IF NOT  Graph.Init ( 50, 50, 640, 480 )
    THEN HALT;
  END;

  Patterns[0] := Graph.PATSYM_DIAG2;
  Patterns[1] := Graph.PATSYM_DENSE1;
  Patterns[2] := Graph.PATSYM_SOLID;
  Patterns[3] := Graph.PATSYM_DENSE2;
  Patterns[4]  := Graph.PATSYM_VERT;

  Intro();

  InitPalette();

  Ellipses(1);
  DrawLines();
  Animation();

  Graph.InitStdPalette();

  
  Bars(1);



END MGdemo.
