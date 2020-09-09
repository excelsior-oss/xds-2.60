<* + M2EXTENSIONS *>

IMPLEMENTATION MODULE field;

IMPORT SYSTEM;
IMPORT W:=Windows;
IMPORT Strings;
IMPORT IOChan, SeqFile, ChanConsts, TextIO, IOResult;
FROM pat IMPORT N, NFIGS, figs;

PROCEDURE Clear_fig_count;
VAR x, y : INTEGER;
BEGIN
    FOR y := 0 TO YFIELD-1 DO
        FOR x := 0 TO XFIELD-1 DO
            fig_count [y, x] := 0;
        END;
    END;
END Clear_fig_count;

PROCEDURE Update_fig_count (fig, pat : INTEGER; x, y : INTEGER; inc : INTEGER);
VAR j : INTEGER;
BEGIN
    FOR j := 1 TO N DO
        INC (fig_count [figs [fig].pats [pat].y [j] + y,
                        figs [fig].pats [pat].x [j] + x], inc);
    END;
END Update_fig_count;

PROCEDURE clear_pic (VAR pic : PICTURE);
VAR x, y : INTEGER;
BEGIN
    FOR y := 0 TO YFIELD-1 DO
        FOR x := 0 TO XFIELD-1 DO
            pic [y, x] := FALSE;
        END;
    END;
END clear_pic;

PROCEDURE Clear;
BEGIN
    clear_pic (picture);
    squares_left := NFIGS * N;
END Clear;

PROCEDURE Put_fig (fig, pat, x, y : INTEGER);
VAR j : INTEGER;
BEGIN
    FOR j := 1 TO N DO
        picture [figs [fig].pats [pat].y [j] + y,
                 figs [fig].pats [pat].x [j] + x] := TRUE;
    END;
    DEC (squares_left, N);
END Put_fig;

PROCEDURE find_min_max (VAR pic : PICTURE;
                        VAR xmin, ymin, xmax, ymax : INTEGER);
VAR x, y : INTEGER;
BEGIN
    xmin := 1000;
    ymin := 1000;
    xmax := 0;
    ymax := 0;
    FOR y := 0 TO YFIELD-1 DO
        FOR x := 0 TO XFIELD-1 DO
            IF pic [y][x] THEN
                IF y < ymin THEN ymin := y; END;
                IF x < xmin THEN xmin := x; END;
                IF y > ymax THEN ymax := y; END;
                IF x > xmax THEN xmax := x; END;
            END;
        END;
    END;
END find_min_max;

PROCEDURE Find_upper_left_point (VAR xmin, ymin : INTEGER);
VAR xmax, ymax : INTEGER;
BEGIN
    find_min_max (picture, xmin, ymin, xmax, ymax);
END Find_upper_left_point;

PROCEDURE center_image (VAR src, dest : PICTURE);
VAR x, y,
    xmin, xmax,
    ymin, ymax,
    xdest, ydest : INTEGER;
BEGIN
    find_min_max (src, xmin, ymin, xmax, ymax);
    IF xmin >= XFIELD THEN RETURN END;
    xdest := (XFIELD - (xmax-xmin+1)) DIV 2;
    ydest := (YFIELD - (ymax-ymin+1)) DIV 2;
    FOR y := ymin TO ymax DO
        FOR x := xmin TO xmax DO
            dest [y-ymin+ydest, x-xmin+xdest] := src [y, x];
        END;
    END;
END center_image;

PROCEDURE read_file  (name : ARRAY OF CHAR) : BOOLEAN;
VAR cid  : IOChan.ChanId;
    res  : ChanConsts.OpenResults;
    str  : ARRAY [0..XFIELD-1] OF CHAR;
    x, y : INTEGER;
    pic  : PICTURE;
TYPE CHARSET = SET OF CHAR;
BEGIN
    SeqFile.OpenRead (cid, name, SeqFile.FlagSet {}, res);
    IF res # ChanConsts.opened THEN
        RETURN FALSE;
    END;
    Clear;
    clear_pic (pic);
    y := 0;
    LOOP
        IF y = YFIELD THEN
            EXIT
        END;
        TextIO.ReadString (cid, str);
        IF IOResult.ReadResult (cid) = IOResult.endOfInput THEN
            EXIT;
        END;
        TextIO.SkipLine (cid);
        FOR x := 0 TO VAL (INTEGER, Strings.Length (str)) - 1 DO
             IF (str[x] IN CHARSET {'*', '#', '$', 'x', 'X'}) AND (squares_left > 0) THEN
                 pic [y, x] := TRUE;
                 DEC (squares_left);
             END;
        END;
        INC (y);
    END;
    SeqFile.Close (cid);
    center_image (pic, picture);
    RETURN TRUE;
END read_file;

PROCEDURE write_file (name : ARRAY OF CHAR) : BOOLEAN;
VAR x, y : INTEGER;
    cid : IOChan.ChanId;
    res  : ChanConsts.OpenResults;
    xmin, xmax,
    ymin, ymax : INTEGER;
BEGIN
    SeqFile.OpenWrite (cid, name, SeqFile.old, res);
    IF res # ChanConsts.opened THEN
        RETURN FALSE;
    END;

    find_min_max (picture, xmin, ymin, xmax, ymax);
    FOR y := ymin TO ymax DO
        FOR x := xmin TO xmax DO
            IF picture [y,x] THEN
               TextIO.WriteChar (cid, '#');
            ELSE
               TextIO.WriteChar (cid, ' ');
            END;
        END;
        TextIO.WriteLn (cid);
    END;
    SeqFile.Close (cid);
    RETURN TRUE;
END write_file;

CONST filter = "Pattern files (*.pat)\*.pat\"+
               "All files (*.*)\*.*\";
      defext = "";

VAR filename : ARRAY [0..1000] OF CHAR;

PROCEDURE make_filter (VAR d : ARRAY OF CHAR; s : ARRAY OF CHAR);
VAR i : CARDINAL;
BEGIN
    Strings.Assign (s, d);
    FOR i := 0 TO HIGH (s) DO
        IF d[i] = "\" THEN
            d[i] := 0C;
        END;
    END;
END make_filter;

PROCEDURE Browse (write : BOOLEAN; VAR filename : ARRAY OF CHAR; title : ARRAY OF CHAR) : BOOLEAN;
VAR of : W.OPENFILENAME;
    f : ARRAY [0..1000] OF CHAR;
BEGIN
    filename [SIZE (filename)-1] := 0C;
    make_filter (f, filter);
	SYSTEM.FILL (SYSTEM.ADR (of), 0, SIZE (of));
	of.lStructSize := SIZE (of);
	of.hwndOwner   := W.GetActiveWindow ();
	of.lpstrFilter := SYSTEM.ADR (f);
	of.lpstrDefExt := SYSTEM.ADR (defext);
	of.lpstrFile   := SYSTEM.ADR (filename);
	of.nMaxFile    := SIZE (filename)-1;
	of.lpstrTitle  := SYSTEM.ADR (title);
	of.Flags       := W.OFN_HIDEREADONLY;
	IF write THEN
		RETURN W.GetSaveFileName (of);
	ELSE
		RETURN W.GetOpenFileName (of);
    END;
END Browse;

PROCEDURE Read () : BOOLEAN;
VAR errstr : ARRAY [0..1500] OF CHAR;
BEGIN
    IF NOT Browse (FALSE, filename, "Select pattern to load") THEN
       RETURN FALSE;
    END;
    IF NOT read_file (filename) THEN
        Strings.Assign ("Can't open: ", errstr);
        Strings.Append (filename, errstr);
        W.MessageBox (W.GetActiveWindow (),  errstr, "File open error", W.MB_OK + W.MB_ICONSTOP);
        RETURN FALSE;
    END;
    RETURN TRUE;
END Read;

PROCEDURE Write;
VAR errstr : ARRAY [0..1500] OF CHAR;
BEGIN
    IF NOT Browse (TRUE, filename, "Select file name to save pattern") THEN
       RETURN;
    END;
    IF NOT write_file (filename) THEN
        Strings.Assign ("Can't create: ", errstr);
        Strings.Append (filename, errstr);
        W.MessageBox (W.GetActiveWindow (),  errstr, "File create error", W.MB_OK + W.MB_ICONSTOP);
    END;
END Write;

PROCEDURE InitDir;
VAR dir : ARRAY [0..2000] OF CHAR;
    i, j : INTEGER;
BEGIN
    W.GetModuleFileName (NIL, dir, SIZE(dir));
    j := 0;
    i := 0;
    WHILE dir [i] # 0C DO
        IF dir [i] = '\' THEN j := i+1 END;
        INC (i);
    END;
    dir [j] := 0C;
    Strings.Append ("patterns", dir);
    W.SetCurrentDirectory (dir);
END InitDir;

BEGIN
    InitDir;
    filename [0] := 0C;
END field.
