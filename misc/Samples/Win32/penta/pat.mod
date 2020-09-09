<* + M2EXTENSIONS *>

IMPLEMENTATION MODULE pat;

CONST CX = N;
      CY = N;

CONST figdef =
    "I*****;" +
    "L****/*;" +
    "Y****/.*;" +
    "V***/*/*;" +
    "P***/**;" +
    "U***/*.*;" +
    "T***/.*/.*;" +
    "N***/..**;" +
    "Z**/.*/.**;" +
    "R**/.**/.*;" +
    "W**/.**/..*;" +
    "X.*/***/.*;";

TYPE RULESTYPE = ARRAY [1..8], [1..4] OF CHAR;

CONST rules = RULESTYPE {
    "+X+Y",
    "-Y+X",
    "-X-Y",
    "+Y-X",
    "-X+Y",
    "-Y-X",
    "+X-Y",
    "+Y+X"};


PROCEDURE equ_pat (VAR p, q : PAT) : BOOLEAN;
VAR i, j : INTEGER;
BEGIN
    FOR i := 1 TO N DO
        LOOP
            FOR j := 1 TO N DO
                IF (p.x[i] = q.x [j]) AND (p.y [i] = q.y [j]) THEN
                    EXIT
                END;
            END;
            RETURN FALSE;
        END;
    END;
    RETURN TRUE;
END equ_pat;

PROCEDURE normalize_pat (VAR p : PAT);
VAR
    xmin, xmax, ymin, ymax, i : INTEGER;
BEGIN
    xmin := CX;
    ymin := CY;
    xmax := 0;
    ymax := 0;
    FOR i := 1 TO N DO
        IF p.x [i] < xmin THEN xmin := p.x [i]; END;
        IF p.x [i] > xmax THEN xmax := p.x [i]; END;
        IF p.y [i] < ymin THEN ymin := p.y [i]; END;
        IF p.y [i] > ymax THEN ymax := p.y [i]; END;
    END;
    p.sx := xmax - xmin + 1;
    p.sy := ymax - ymin + 1;
    p.y0 := CY;
    FOR i := 1 TO N DO
        DEC (p.x [i], xmin);
        DEC (p.y [i], ymin);
        IF (p.x [i] = 0) AND (p.y [i] < p.y0) THEN p.y0 := p.y [i] END;
    END;
END normalize_pat;

PROCEDURE makeimages (VAR f : FIG);
VAR
    i, j, k, x, y, X, Y : INTEGER;
BEGIN
    normalize_pat (f.pats [1]);
    j := 2;
    FOR i := 2 TO 8 DO
        f.pats [j].sx := CX;
        f.pats [j].sy := CY;

        FOR k := 1 TO N DO
            x := f.pats [1].x [k];
            y := f.pats [1].y [k];
            IF rules [i][2] = 'X' THEN X := x ELSE X := y; END;
            IF rules [i][4] = 'X' THEN Y := x ELSE Y := y; END;
            IF rules [i][1] = '-' THEN X := CX - X - 1; END;
            IF rules [i][3] = '-' THEN Y := CY - Y - 1; END;
            f.pats [j].x[k] := X;
            f.pats [j].y[k] := Y;
        END;
        normalize_pat (f.pats [j]);
        LOOP
            FOR k := 1 TO j-1 DO
                IF equ_pat (f.pats [k], f.pats [j]) THEN
                    EXIT;
                END;
            END;
            INC (j);
            EXIT;
        END;
    END;
    f.n := j-1;
END makeimages;

PROCEDURE readpat (VAR p : PAT; s : ARRAY OF CHAR; VAR pos : INTEGER);
VAR
    x, y, mx, n : INTEGER;
BEGIN
    x := 0;
    y := 0;
    mx := 0;
    n := 0;
    WHILE (s[pos] <> 0C) AND (s[pos] <> ';') DO
        IF s[pos] = '/'THEN
            INC (y);
            IF x > mx THEN mx := x; END;
            x := 0;
        ELSE
            IF s[pos] = '*' THEN
                INC (n);
                p.x [n] := x;
                p.y [n] := y;
            END;
            INC (x);
        END;
        INC (pos);
    END;
    IF x > mx THEN mx := x; END;
    p.sx := mx;
    p.sy := y+1;
    IF s[pos] <> 0C THEN
       INC (pos);
    END;
    ASSERT (n = N);
END readpat;

PROCEDURE makefig (VAR f : FIG; s : ARRAY OF CHAR; VAR pos : INTEGER);
BEGIN
    f.c := s [pos];
    INC (pos);
    readpat (f.pats [1], s, pos);
    makeimages (f);
END makefig;

PROCEDURE makefigs;
VAR i   : INTEGER;
    pos : INTEGER;
BEGIN
    pos := 0;
    i := 0;
    WHILE figdef [pos] <> 0C DO
        INC (i);
        makefig (figs [i], figdef, pos);
    END;
    ASSERT (i = NFIGS);
END makefigs;

END pat.
