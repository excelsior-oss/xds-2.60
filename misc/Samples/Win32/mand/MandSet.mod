(*
  Copyrigth (C) 1995 xTech ltd
*)

<*+M2EXTENSIONS *>
<*+M2ADDTYPES *>
<*-IOVERFLOW *>
<*-COVERFLOW *>
<*-CHECKINDEX *>
<*-CHECKRANGE *>
<*-CHECKNIL *>
<*-GENDEBUG *>
<*+DOREORDER *>

MODULE MandSet;

IMPORT Windows, LongStr, WholeStr, Strings, Storage, SYSTEM;

FROM Windows IMPORT HDC,
                    MSG,
                    BYTE,
                    UINT,
                    HWND,
                    LONG,
                    RECT,
                    HMENU,
                    WPARAM,
                    LPARAM,
                    HANDLE,
                    LRESULT,
                    RGBQUAD,
                    HGLOBAL,
                    WNDCLASS,
                    HINSTANCE,
                    PAINTSTRUCT,
                    BITMAPINFOHEADER;

FROM Windows IMPORT LOWORD, HIWORD, min, max;

(*----------------------------------------------------------------------------*)

VAR     wnd_count: INTEGER;

CONST   MaxX = 1600;
        MaxY = 1280;
        ClassName = "MANClass";

TYPE    HUGEARR = ARRAY [0..MaxX*MaxY-1] OF SHORTCARD;
        PTR  = POINTER TO HUGEARR;
        DATA = RECORD
                   hwnd:                HWND;
                   SX0, SX1, SY0, SY1:  LONGREAL;
                   X0, X1, Y0, Y1:      LONGREAL;
                   x0, x1, y0, y1:      LONGREAL;
                   time:                INTEGER;
                   selected:            BOOLEAN;
                   sx, sy:              INTEGER;
                   image:               PTR;
                   image_sz:            INTEGER;
               END;
        PDATA = POINTER TO DATA;

CONST   MSG_RECALCULATE = Windows.WM_USER+1000;
        MSG_EXPAND      = Windows.WM_USER+1001;

        RECALC_ITEM = 1001;
        EXPAND_ITEM = 1002;
        COPY_ITEM   = 1003;
        EXIT_ITEM   = 1004;

        START_X0    = -2.0;
        START_X1    =  1.0;
        START_Y0    = -1.5;
        START_Y1    =  1.5;

TYPE    biType = RECORD
                    h:      BITMAPINFOHEADER;
                    colors: ARRAY [0..255] OF RGBQUAD;
                 END;

VAR     bi: biType;


(*----------------------------------------------------------------------------*)

PROCEDURE setcolors;
VAR i: INTEGER;
BEGIN
    FOR i:=0 TO 254 DO
        bi.colors [i].rgbRed      := VAL (BYTE, i DIV 64 * 64);
        bi.colors [i].rgbGreen    := VAL (BYTE, (i DIV 8 MOD 8) * 32);
        bi.colors [i].rgbBlue     := VAL (BYTE, (i MOD 8) * 32);
        bi.colors [i].rgbReserved := 0;
    END;
    bi.colors [255].rgbRed      := 0;
    bi.colors [255].rgbGreen    := 0;
    bi.colors [255].rgbBlue     := 0;
    bi.colors [255].rgbReserved := 0;
END setcolors;

(*----------------------------------------------------------------------------*)

PROCEDURE preparebi;
BEGIN
    bi.h.biSize          := SIZE (BITMAPINFOHEADER);
    bi.h.biWidth         := 0;
    bi.h.biHeight        := 0;
    bi.h.biPlanes        := 1;
    bi.h.biBitCount      := 8;
    bi.h.biCompression   := Windows.BI_RGB;
    bi.h.biSizeImage     := 0;
    bi.h.biXPelsPerMeter := 0;
    bi.h.biYPelsPerMeter := 0;
    bi.h.biClrUsed       := 0;
    bi.h.biClrImportant  := 0;
END preparebi;

(*----------------------------------------------------------------------------*)

PROCEDURE OutCoords (VAR s: ARRAY OF CHAR; x0, y0, x1, y1: LONGREAL);
VAR tmp: ARRAY [0..79] OF CHAR;
BEGIN
    Strings.Append ("(", s);
    LongStr.RealToFloat (x0, 4, tmp);
    Strings.Append (tmp, s);
    Strings.Append (",", s);
    LongStr.RealToFloat (y0, 4, tmp);
    Strings.Append (tmp, s);
    Strings.Append (")-(", s);
    LongStr.RealToFloat (x1, 4, tmp);
    Strings.Append (tmp, s);
    Strings.Append (",", s);
    LongStr.RealToFloat (y1, 4, tmp);
    Strings.Append (tmp, s);
    Strings.Append (")", s);
END OutCoords;

(*----------------------------------------------------------------------------*)

PROCEDURE draw_title (VAR data: DATA);
VAR s, tmp: ARRAY [0..255] OF CHAR;
BEGIN
    Strings.Assign ("Time: ", s);
    WholeStr.IntToStr (data.time, tmp);
    Strings.Append (tmp, s);
    Strings.Append (". Set borders: ", s);
    OutCoords (s, data.SX0, data.SY0, data.SX1, data.SY1);
    IF data.selected THEN
        Strings.Append ("; Selection: ", s);
        OutCoords (s, data.x0, data.y0, data.x1, data.y1);
    END;
    Windows.SetWindowText (data.hwnd, s);
END draw_title;

(*----------------------------------------------------------------------------*)

PROCEDURE Max (a, b: LONGREAL): LONGREAL;
BEGIN
    IF a >= b THEN
        RETURN a;
    ELSE
        RETURN b;
    END;
END Max;

(*----------------------------------------------------------------------------*)

PROCEDURE DrawBitmap (VAR data: DATA; dc: HDC);
BEGIN
    IF data.image <> NIL THEN
        bi.h.biWidth  := data.sx;
        bi.h.biHeight := data.sy;
        Windows.SetDIBitsToDevice (dc, 0, 0, data.sx, data.sy, 0, 0,
                                   0, data.sy, data.image^,
                                   bi.h,
                                   Windows.DIB_RGB_COLORS);
    END;
END DrawBitmap;

(*----------------------------------------------------------------------------*)

PROCEDURE DrawWin (VAR data: DATA);
VAR dc: HDC;
BEGIN
    dc := Windows.GetDC (data.hwnd);
    DrawBitmap (data, dc);
    Windows.ReleaseDC (data.hwnd, dc);
END DrawWin;

(*----------------------------------------------------------------------------*)

PROCEDURE TitleCalculated (perc: INTEGER; VAR data: DATA);
VAR s, tmp: ARRAY [0..79] OF CHAR;
BEGIN
    Strings.Assign ("Calculated ", s);
    WholeStr.IntToStr (perc, tmp);
    Strings.Append (tmp, s);
    Strings.Append ("%", s);
    Windows.SetWindowText (data.hwnd, s);
END TitleCalculated;

(*----------------------------------------------------------------------------*)

PROCEDURE CalcSet (VAR data: DATA; w, h: INTEGER; d: LONGREAL);
VAR
    x0, y0, x, y, x2, y2: LONGREAL;
    wb, i, j, n, perc: INTEGER;
    p: INTEGER;
BEGIN
    p  := 0;
    wb := (w + 3) DIV 4 * 4;
    perc := 0;
    FOR j:=0 TO h-1 DO
        y0 := LFLOAT (j) * d + data.Y0;
        FOR i:=0 TO wb-1 DO
            x0 := LFLOAT (i) * d + data.X0;
            x  := x0;
            y  := y0;
            n  := 0;
            LOOP
                x2 := x * x;
                y2 := y * y;
                IF x2 + y2 >= 4.0 THEN
                    EXIT;
                END;
                INC (n);
                IF n = 255 THEN
                    EXIT;
                END;
                y := 2.0 * x * y + y0;
                x := x2 - y2 + x0;
            END;
            data.image^[p] := VAL (SHORTCARD, n);
            INC (p);
        END;
        n := (j+1)*100/h;
        IF (n <> perc) & (n <> 100) THEN
            perc := n;
            TitleCalculated (perc, data);
            IF perc MOD 10 = 0 THEN
                DrawWin (data);
            END;
        END;
    END;
END CalcSet;

(*----------------------------------------------------------------------------*)

PROCEDURE BuildImage (VAR data: DATA);
VAR
    rc: RECT;
    t0, t1: LONG;
    w, h, wb: INTEGER;
    d: LONGREAL;
BEGIN
    Windows.GetClientRect (data.hwnd, rc);
    w := rc.right;
    h := rc.bottom;
    IF data.image <> NIL THEN
        Storage.DEALLOCATE (data.image, data.image_sz);
    END;
    wb := (w + 3) DIV 4 * 4;
    data.image_sz := wb * h;
    Storage.ALLOCATE (data.image, data.image_sz);
    IF data.image = NIL THEN
        Windows.SetWindowText (data.hwnd, "Sorry, no memory for a bitmap");
    ELSE
        Windows.FillMemory (data.image,  wb * h, 0);
        d := Max ((data.SX1 - data.SX0) / LFLOAT (w),
                  (data.SY1 - data.SY0) / LFLOAT (h));
        data.X0 := (data.SX0 + data.SX1 - d * LFLOAT (w)) / 2.0;
        data.X1 := data.X0 + d * LFLOAT (w);
        data.Y0 := (data.SY0 + data.SY1 - d * LFLOAT (h)) / 2.0;
        data.Y1 := data.Y0 + d * LFLOAT (h);
        data.sx := w;
        data.sy := h;
        t0 := Windows.GetTickCount ();
        CalcSet (data, w, h, d);
        t1 := Windows.GetTickCount ();
        data.time := t1-t0;
        draw_title (data);
    END;
END BuildImage;

(*----------------------------------------------------------------------------*)

PROCEDURE CopyBitmap (VAR data: DATA);
VAR
    l: CARDINAL;
    p: SYSTEM.ADDRESS;
    h: HGLOBAL;
BEGIN
    IF data.image <> NIL THEN
        l := (data.sx + 3) DIV 4 * 4 * data.sy;
        h := Windows.GlobalAlloc (Windows.GMEM_MOVEABLE+Windows.GMEM_DDESHARE, SIZE (bi) + l);
        IF h = NIL THEN
            RETURN;
        END;
        p := Windows.GlobalLock (h);
        SYSTEM.MOVE (SYSTEM.ADR (bi), p, SIZE (bi));
        SYSTEM.MOVE (data.image, SYSTEM.ADDADR (p, SIZE (bi)), l);
        Windows.GlobalUnlock (h);

        IF Windows.OpenClipboard (data.hwnd) THEN
                Windows.EmptyClipboard;
                Windows.SetClipboardData (Windows.CF_DIB,
                                          SYSTEM.CAST (HANDLE, h));
                Windows.CloseClipboard;
        ELSE
                Windows.GlobalFree (h);
        END;
    END;
END CopyBitmap;

(*----------------------------------------------------------------------------*)

VAR in_drag: BOOLEAN;
    rect_drawn: BOOLEAN;
    rx0, ry0, rx, ry: INTEGER;

(*----------------------------------------------------------------------------*)

PROCEDURE draw_rect (hwnd: HWND);
VAR
    rc: RECT;
    dc: HDC;
BEGIN
    dc := Windows.GetDC (hwnd);
    rc.left   := min (rx0, rx);
    rc.right  := max (rx0, rx);
    rc.top    := min (ry0, ry);
    rc.bottom := max (ry0, ry);
    Windows.DrawFocusRect (dc, rc);
    Windows.ReleaseDC (hwnd, dc);
END draw_rect;

(*----------------------------------------------------------------------------*)

PROCEDURE calc_borders (VAR data: DATA);
VAR
    DX, DY: LONGREAL;
    rc: RECT;
BEGIN
    Windows.GetClientRect (data.hwnd, rc);
    DX := data.X1 - data.X0;
    DY := data.Y1 - data.Y0;
    data.x0 := data.X0 + DX / VAL (LONGREAL, data.sx) *
                         VAL (LONGREAL, min (rx0, rx));
    data.x1 := data.X0 + DX / VAL (LONGREAL, data.sx) *
                         VAL (LONGREAL, max (rx0, rx));
    data.y0 := data.Y0 + DY / VAL (LONGREAL, data.sy) *
                         VAL (LONGREAL, min (rc.bottom - ry0,
                                             rc.bottom - ry));
    data.y1 := data.Y0 + DY / VAL (LONGREAL, data.sy) *
                         VAL (LONGREAL, max (rc.bottom - ry0,
                                             rc.bottom - ry));
    data.selected := TRUE;
    draw_title (data);
END calc_borders;

(*----------------------------------------------------------------------------*)

PROCEDURE check (hwnd: HWND; VAR x, y: INTEGER);
VAR rc: RECT;
BEGIN
    Windows.GetClientRect (hwnd, rc);
    IF x < 0 THEN
        x := 0;
    ELSIF x > rc.right THEN
        x := rc.right;
    END;
    IF y < 0 THEN
        y := 0;
    ELSIF y > rc.bottom THEN
        y := rc.bottom;
    END;
END check;

(*----------------------------------------------------------------------------*)

PROCEDURE StartDrag (VAR data: DATA; x, y: INTEGER);
BEGIN
    IF data.image <> NIL THEN
        IF rect_drawn THEN
            draw_rect (data.hwnd);
        END;
        check (data.hwnd, x, y);
        rx0 := x;
        ry0 := y;
        rx  := x;
        ry  := y;
        in_drag := TRUE;
        rect_drawn := FALSE;
        Windows.SetCapture (data.hwnd);
    END;
END StartDrag;

(*----------------------------------------------------------------------------*)

PROCEDURE InDrag (VAR data: DATA; x, y: INTEGER);
BEGIN
    IF in_drag THEN
        IF rect_drawn THEN
            draw_rect (data.hwnd);
        END;
        check (data.hwnd, x, y);
        rx := x;
        ry := y;
        draw_rect (data.hwnd);
        rect_drawn := TRUE;
	calc_borders (data);
    END;
END InDrag;

(*----------------------------------------------------------------------------*)

PROCEDURE EndDrag (VAR data: DATA; x, y: INTEGER);
BEGIN
    IF in_drag THEN
        in_drag := FALSE;
        IF rect_drawn THEN
            draw_rect (data.hwnd);
        END;
        rect_drawn := FALSE;
        Windows.ReleaseCapture;
        check (data.hwnd, x, y);
        rx := x;
        ry := y;
	calc_borders (data);
    END;
END EndDrag;

(*----------------------------------------------------------------------------*)

PROCEDURE DeactivateDrag (VAR data: DATA);
BEGIN
    IF in_drag THEN
        in_drag := FALSE;
        IF rect_drawn THEN
            draw_rect (data.hwnd);
        END;
        rect_drawn := FALSE;
        Windows.ReleaseCapture;
    END;
END DeactivateDrag;

(*----------------------------------------------------------------------------*)

PROCEDURE create_view (x0, x1, y0, y1: LONGREAL; recalc: BOOLEAN): HWND;
VAR data: PDATA;
    wnd:  HWND;
BEGIN
    wnd := Windows.CreateWindow (ClassName,
                                 "M-Set",
                                 Windows.WS_OVERLAPPEDWINDOW +
                                 Windows.WS_VISIBLE,
                                 Windows.CW_USEDEFAULT, Windows.CW_USEDEFAULT,
                                 Windows.CW_USEDEFAULT, Windows.CW_USEDEFAULT,
                                 NIL,
                                 NIL,
                                 Windows.GetModuleHandle (NIL),
                                 NIL);
    IF wnd <> NIL THEN
        data := SYSTEM.CAST (SYSTEM.ADDRESS, Windows.GetWindowLongA (wnd, Windows.GWL_USERDATA));
        data^.SX0 := x0;
        data^.SY0 := y0;
        data^.SX1 := x1;
        data^.SY1 := y1;
        IF recalc THEN
            Windows.PostMessage (wnd, MSG_RECALCULATE, 0, 0);
        END;
    END;
    RETURN wnd;
END create_view;

(*----------------------------------------------------------------------------*)

PROCEDURE ILOWORD (lparam: LPARAM): INTEGER;
BEGIN
    RETURN VAL (INTEGER, SYSTEM.CAST (SYSTEM.INT16, LOWORD (lparam)));
END ILOWORD;

(*----------------------------------------------------------------------------*)

PROCEDURE IHIWORD (lparam: LPARAM): INTEGER;
BEGIN
    RETURN VAL (INTEGER, SYSTEM.CAST (SYSTEM.INT16, HIWORD (lparam)));
END IHIWORD;

(*----------------------------------------------------------------------------*)

PROCEDURE [Windows.CALLBACK] MainWinProc (hwnd: HWND;
                                          msg: UINT;
                                          wparam: WPARAM;
                                          lparam: LPARAM): LRESULT;
VAR dc: HDC;
    menu: HMENU;
    ps: PAINTSTRUCT;
    data: PDATA;
BEGIN

    data := SYSTEM.CAST (SYSTEM.ADDRESS, Windows.GetWindowLongA (hwnd, Windows.GWL_USERDATA));
    CASE msg OF
    | Windows.WM_CREATE:
        Storage.ALLOCATE (data, SIZE (DATA));
        IF data = NIL THEN
            RETURN -1;
        END;
        data^.image    := NIL;
        data^.image_sz := 0;
        data^.hwnd     := hwnd;
        data^.selected := FALSE;
        Windows.SetWindowLongA (hwnd, Windows.GWL_USERDATA, SYSTEM.CAST (Windows.DWORD, data));
        INC (wnd_count);
        menu := Windows.GetSystemMenu (hwnd, Windows.FALSE);
        Windows.AppendMenu (menu, Windows.MF_STRING, RECALC_ITEM, "&Refresh");
        Windows.AppendMenu (menu, Windows.MF_STRING, EXPAND_ITEM, "&Expand");
        Windows.AppendMenu (menu, Windows.MF_STRING, COPY_ITEM,   "Cop&y");
        Windows.AppendMenu (menu, Windows.MF_STRING, EXIT_ITEM,   "E&xit");
        RETURN 0;
    | Windows.WM_PAINT:
        dc := Windows.BeginPaint (hwnd, ps);
        DrawBitmap (data^, dc);
        Windows.EndPaint (hwnd, ps);
        RETURN 0;
    | Windows.WM_CLOSE:
        IF data^.image <> NIL THEN
            Storage.DEALLOCATE (data^.image, data^.image_sz);
        END;
        Storage.DEALLOCATE (data, SIZE (DATA));
        DEC (wnd_count);
        IF wnd_count = 0 THEN
            Windows.PostQuitMessage (0);
        END;
    | Windows.WM_LBUTTONDOWN:
        StartDrag (data^, ILOWORD (lparam), IHIWORD (lparam));
        RETURN 0;
    | Windows.WM_LBUTTONUP:
        EndDrag (data^, ILOWORD (lparam), IHIWORD (lparam));
        RETURN 0;
    | Windows.WM_MOUSEMOVE:
        InDrag (data^, ILOWORD (lparam), IHIWORD (lparam));
        RETURN 0;
    | Windows.WM_ACTIVATE:
        IF LOWORD (wparam) = Windows.WA_INACTIVE THEN
            DeactivateDrag (data^);
        END;
    | Windows.WM_RBUTTONDOWN:
        IF data^.selected THEN
            Windows.SendMessage (hwnd, MSG_EXPAND, 0, 0);
        ELSE
            Windows.SendMessage (hwnd, MSG_RECALCULATE, 0, 0);
        END;
        RETURN 0;
    | MSG_RECALCULATE:
        BuildImage (data^);
        DrawWin (data^);
        RETURN 0;
    | MSG_EXPAND:
        IF data^.selected THEN
            IF create_view (data^.x0, data^.x1, data^.y0, data^.y1, TRUE) = NIL THEN
                Windows.MessageBox (NIL, "Couldn't create window", NIL,
                                    Windows.MB_OK +
                                    Windows.MB_SYSTEMMODAL +
                                    Windows.MB_ICONHAND);
            END;
            data^.selected := FALSE;
            draw_title (data^);
        END;
        RETURN 0;
    | Windows.WM_INITMENU:
        menu := Windows.GetSystemMenu (hwnd, Windows.FALSE);
        Windows.EnableMenuItem (menu, EXIT_ITEM,
                                Windows.MF_BYCOMMAND + Windows.MF_ENABLED);
        Windows.EnableMenuItem (menu, RECALC_ITEM,
                                Windows.MF_BYCOMMAND + Windows.MF_ENABLED);
        IF data^.image <> NIL THEN
            Windows.EnableMenuItem (menu, COPY_ITEM,
                                    Windows.MF_BYCOMMAND + Windows.MF_ENABLED);
        ELSE
            Windows.EnableMenuItem (menu, COPY_ITEM,
                                    Windows.MF_BYCOMMAND + Windows.MF_GRAYED);
        END;
        IF data^.selected THEN
            Windows.EnableMenuItem (menu, EXPAND_ITEM,
                                    Windows.MF_BYCOMMAND + Windows.MF_ENABLED);
        ELSE
            Windows.EnableMenuItem (menu, EXPAND_ITEM,
                                    Windows.MF_BYCOMMAND + Windows.MF_GRAYED);
        END;
        RETURN 0;
    | Windows.WM_SYSCOMMAND:
        CASE LOWORD (wparam) OF
        | RECALC_ITEM:
            Windows.PostMessage (hwnd, MSG_RECALCULATE, 0, 0);
            RETURN 0;
        | EXPAND_ITEM:
            Windows.PostMessage (hwnd, MSG_EXPAND, 0, 0);
            RETURN 0;
        | COPY_ITEM:
            CopyBitmap (data^);
            RETURN 0;
        | EXIT_ITEM:
            Windows.PostQuitMessage (0);
            RETURN 0;
        ELSE
        END;
    ELSE
    END;
    RETURN Windows.DefWindowProc (hwnd, msg, wparam, lparam);
END MainWinProc;

(*----------------------------------------------------------------------------*)
VAR wc:  Windows.WNDCLASS;
    msg: MSG;

BEGIN
    wc.style := Windows.CS_BYTEALIGNCLIENT + Windows.CS_OWNDC;
    wc.lpfnWndProc := MainWinProc;
    wc.cbClsExtra := 0;
    wc.cbWndExtra := 0;
    wc.hInstance := Windows.GetModuleHandle (NIL);
    wc.hIcon := NIL;
    wc.hCursor := Windows.LoadCursor (NIL, Windows.IDC_ARROW);
    wc.hbrBackground := Windows.CreateSolidBrush (Windows.RGB (0, 0, 0));
    wc.lpszMenuName  := NIL;
    wc.lpszClassName := SYSTEM.ADR (ClassName);
    IF Windows.RegisterClass (wc) = 0 THEN
        Windows.MessageBox (NIL, "Couldn't register Class", NIL,
                            Windows.MB_OK +
                            Windows.MB_SYSTEMMODAL +
                            Windows.MB_ICONHAND);
        HALT (1);
    END;
    setcolors;
    preparebi;
    in_drag    := FALSE;
    rect_drawn := FALSE;

    IF create_view (START_X0, START_X1, START_Y0, START_Y1, TRUE) = NIL THEN
        Windows.MessageBox (NIL, "Couldn't create main window", NIL,
                            Windows.MB_OK +
                            Windows.MB_SYSTEMMODAL +
                            Windows.MB_ICONHAND);
        HALT (1);
    END;
    WHILE Windows.GetMessage (msg, NIL, 0, 0) DO
        Windows.TranslateMessage (msg);
        Windows.DispatchMessage  (msg);
    END;
END MandSet.
