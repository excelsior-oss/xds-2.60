IMPLEMENTATION MODULE solve;
FROM pat IMPORT N, NFIGS, PAT, SOLUTION, figs;
IMPORT EXCEPTIONS;

CONST XSIZE = 64;
      YSIZE = 64;

VAR field : ARRAY [0..YSIZE-1], [0..XSIZE-1] OF CHAR;
    SX, SY : INTEGER;
    used : ARRAY [1..NFIGS] OF BOOLEAN;
    solution_callback : REPORT_PROC;
    solution : SOLUTION;
    abort_flag : BOOLEAN;
    exception_source : EXCEPTIONS.ExceptionSource;

PROCEDURE put_fig (VAR p : PAT; x, y : INTEGER; c : CHAR);
VAR i : INTEGER;
BEGIN
    FOR i := 1 TO N DO
        field [y+p.y[i], x+p.x[i]] := c;
    END;
END put_fig;

PROCEDURE find_hole (x0 : INTEGER; VAR xstart, ystart : INTEGER) : BOOLEAN;
VAR x, y : INTEGER;
BEGIN
    FOR x := x0 TO SX DO
        FOR y := 1 TO SY DO
            IF field [y, x] = 0C THEN
                 xstart := x;
                 ystart := y;
                 RETURN TRUE;
            END;
        END;
    END;
    RETURN FALSE;
END find_hole;

PROCEDURE deep (d, x0 : INTEGER); FORWARD;

PROCEDURE try_put (fig, pat : INTEGER; d, x, y : INTEGER);
VAR i : INTEGER;
BEGIN
    IF abort_flag THEN
        EXCEPTIONS.RAISE (exception_source, 0, "");
    END;

    y := y - figs [fig].pats [pat].y0;
    IF (y < 1) OR (x + figs [fig].pats [pat].sx > SX+1) OR
                  (y + figs [fig].pats [pat].sy > SY+1)
    THEN
       RETURN;
    END;

    FOR i := 1 TO N DO
        IF field [y + figs [fig].pats [pat].y [i],
                  x + figs [fig].pats [pat].x [i]] <> 0C
        THEN
            RETURN;
        END;
    END;

	solution [fig].pat := pat;
	solution [fig].x := x;
	solution [fig].y := y;

    put_fig (figs [fig].pats [pat], x, y, figs [fig].c);

    IF d = 1 THEN
        solution_callback (solution);
    ELSE
        deep (d-1, x);
    END;
    put_fig (figs [fig].pats [pat], x, y, 0C);
END try_put;

PROCEDURE deep (d, x0 : INTEGER);
VAR
    i, j, x, y : INTEGER;
BEGIN
    IF NOT find_hole (x0, x, y) THEN
        RETURN;
    END;

    FOR i := 1 TO NFIGS DO
        IF NOT used [i] THEN
            used [i] := TRUE;
            FOR j := 1 TO figs [i].n DO
                try_put (i, j, d, x, y);
            END;
            used [i] := FALSE;
        END;
    END;
END deep;

PROCEDURE search (report_proc : REPORT_PROC) : BOOLEAN;
VAR
    i : INTEGER;
BEGIN
    abort_flag := FALSE;
    solution_callback := report_proc;

    FOR i := 1 TO NFIGS DO
        used [i] := FALSE;
    END;

    deep (NFIGS, 1);
    RETURN TRUE;
EXCEPT
    IF EXCEPTIONS.IsCurrentSource (exception_source) THEN
        RETURN FALSE;
    END;
END search;

PROCEDURE clear_solve_field;
VAR x, y : INTEGER;
BEGIN
    FOR y := 0 TO YSIZE-1 DO
        FOR x := 0 TO XSIZE-1 DO
            field [y, x]:='*';
        END;
    END;
    SX := 0;
    SY := 0;
END clear_solve_field;

PROCEDURE set_solve_field (x, y : INTEGER);
BEGIN
    field [y][x] := 0C;
    IF x > SX THEN SX := x; END;
    IF y > SY THEN SY := y; END;
END set_solve_field;

PROCEDURE stop_solving;
BEGIN
    abort_flag := TRUE;
END stop_solving;

BEGIN
    EXCEPTIONS.AllocateSource (exception_source);
END solve.
