<* + M2EXTENSIONS *>

MODULE penta;

IMPORT SYSTEM,
       Strings,
       W := Windows,
       pat,
       solve,
       result,
       field,
       R := resource;

<* IF BACKEND = "C" THEN *>
  IMPORT WinRTL;  (* To allow compilation with XDS-C *)
<* END *>

FROM field IMPORT XFIELD, YFIELD, picture, squares_left;

FROM Windows IMPORT UINT, SHORT, LONG, PVOID, DWORD, BOOL,
             WPARAM, LPARAM, LRESULT, LOWORD, HIWORD,
             RECT, HWND, HDC, HBRUSH;

FROM pat IMPORT N, NFIGS, figs, SOLUTION;
FROM result IMPORT nresults;

CONST EXIT_THREAD_MSG  = W.WM_USER;
      NEW_SOLUTION_MSG = W.WM_USER+1;

VAR  field_wnd  : HWND;
     dialog_wnd : HWND;

CONST PAL_COLS = NFIGS DIV 2;
      PAL_ROWS = 2;

VAR Q : INTEGER;
    xsize_field, ysize_field : INTEGER;
    xsize_field_wnd, ysize_field_wnd : INTEGER;
    figures_x, figures_y : INTEGER;

VAR cur_pat    : ARRAY [1..NFIGS] OF INTEGER;
VAR fig_in_use : ARRAY [1..NFIGS] OF BOOLEAN;

VAR figures : ARRAY [1..NFIGS] OF RECORD
                                     fig : INTEGER;
                                     pat : INTEGER;
                                     x   : INTEGER;
                                     y   : INTEGER;
                                  END;
    nfigures : INTEGER;

VAR in_drawing : BOOLEAN;
    drawing_with : BOOLEAN;

VAR BorderPen, SquarePen : W.HPEN;
VAR PatBrush, InUseBrush, BadBrush, GrayBrush, SquareBrush : HBRUSH;

VAR field_bmp : W.HBITMAP;
    field_bmpdc : HDC;

VAR in_drag : BOOLEAN;
    drag_fig, drag_pat : INTEGER;
    drag_x, drag_y, drag_dx, drag_dy : INTEGER;

VAR draw_mode : BOOLEAN;
    squares_rowsize : INTEGER;
    squares_x, squares_y : INTEGER;

VAR solve_mode : BOOLEAN;

VAR xmin, ymin : INTEGER;

PROCEDURE enable_buttons; FORWARD;

VAR cur_result : INTEGER;

PROCEDURE pal_fig_x (i : INTEGER) : INTEGER;
BEGIN
    RETURN figures_x + (i-1) MOD PAL_COLS * Q * (N+1);
END pal_fig_x;

PROCEDURE pal_fig_y (i : INTEGER) : INTEGER;
BEGIN
    RETURN figures_y + (i-1) DIV PAL_COLS * Q * (N+1);
END pal_fig_y;

PROCEDURE field_to_screen_x (x : INTEGER) : INTEGER;
BEGIN
    RETURN x * Q;
END field_to_screen_x;

PROCEDURE field_to_screen_y (y : INTEGER) : INTEGER;
BEGIN
    RETURN y * Q;
END field_to_screen_y;

PROCEDURE fig_x (i : INTEGER) : INTEGER;
BEGIN
    RETURN field_to_screen_x (figures [i].x);
END fig_x;

PROCEDURE fig_y (i : INTEGER) : INTEGER;
BEGIN
    RETURN field_to_screen_y (figures [i].y);
END fig_y;

PROCEDURE recalc_field_sizes (xsize, ysize : INTEGER; style : W.WS_SET; exstyle : W.WS_EX_SET);
VAR sx, sy : INTEGER;
    rc : RECT;
    dc : HDC;
BEGIN
    sx := XFIELD; sy := YFIELD;
    IF sx < PAL_COLS * (N+1) - 1 THEN
        sx := PAL_COLS * (N+1) - 1;
    END;
    sy := sy + (N + 1) * PAL_ROWS + 1;
    Q := W.min (xsize DIV sx, ysize DIV sy);
    xsize_field := Q * XFIELD;
    ysize_field := Q * YFIELD;
    xsize_field_wnd := Q * sx;
    ysize_field_wnd := Q * sy;

    rc.top    := 0;
    rc.left   := 0;
    rc.right  := xsize_field_wnd + 1;
    rc.bottom := ysize_field_wnd + 1;
    W.AdjustWindowRectEx (rc, style, FALSE, exstyle);
    W.SetWindowPos (field_wnd, NIL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, W.SWP_NOMOVE + W.SWP_NOZORDER);

    figures_y := ysize_field + Q * 2;
    figures_x := (xsize_field_wnd - (PAL_COLS * (N+1) - 1) * Q) DIV 2;

    squares_rowsize := NFIGS;
    squares_y := figures_y;
    squares_x := (xsize_field_wnd - (squares_rowsize * 2 * Q - Q)) DIV 2;

    dc := W.GetDC (NIL);
    field_bmp := W.CreateCompatibleBitmap (dc, xsize_field_wnd+1, ysize_field_wnd+1);
    field_bmpdc := W.CreateCompatibleDC (dc);
    W.SelectObject (field_bmpdc, field_bmp);
    W.DeleteDC (dc);
END recalc_field_sizes;

PROCEDURE draw_square (dc : HDC; i : INTEGER);
    VAR x, y : INTEGER;
BEGIN
    y := squares_y + i MOD N * 2 * Q;
    x := squares_x + i DIV N * 2 * Q;
    
    W.SelectObject (dc, BorderPen);
    W.SelectObject (dc, SquareBrush);
    
    W.Rectangle (dc, x, y, x+Q, y+Q);
END draw_square;

PROCEDURE draw_gray_squares (dc : HDC);
    VAR rc : RECT;
    VAR x, y : INTEGER;
BEGIN
    FOR y := 0 TO YFIELD-1 DO
        FOR x := 0 TO XFIELD-1 DO
            IF picture [y][x] THEN
                rc.left   := x * Q + 1;
                rc.top    := y * Q + 1;
                rc.right  := rc.left + Q - 1;
                rc.bottom := rc.top  + Q - 1;
                W.FillRect (dc, rc, SquareBrush);
            END;
        END;
    END;
END draw_gray_squares;

PROCEDURE check_pat_visible (fig, pat : INTEGER; x, y : INTEGER; VAR rcclip : RECT) : BOOLEAN;
VAR rc : RECT;
BEGIN
    rc.left := x; rc.top := y;
    rc.right  := x + Q * figs [fig].pats [pat].sx + 1;
    rc.bottom := y + Q * figs [fig].pats [pat].sy + 1;
    RETURN W.IntersectRect (rc, rc, rcclip);
END check_pat_visible;

PROCEDURE draw_pat_square (dc : HDC; fig, pat : INTEGER;
                           x, y : INTEGER; i : INTEGER; brush : HBRUSH);
VAR rc : RECT;
BEGIN
    rc.left   := x + Q * figs [fig].pats [pat].x [i];
    rc.top    := y + Q * figs [fig].pats [pat].y [i];
    rc.right  := rc.left + Q;
    rc.bottom := rc.top + Q;
    W.FillRect (dc, rc, brush);
END draw_pat_square;

PROCEDURE draw_pat_interior (dc : HDC; fig, pat : INTEGER;
                             x, y : INTEGER; brush : HBRUSH);
VAR i  : INTEGER;
BEGIN
    FOR i := 1 TO N DO
        draw_pat_square (dc, fig, pat, x, y, i, brush);
    END;
END draw_pat_interior;

PROCEDURE draw_pat_border (dc : HDC; fig, pat : INTEGER; x, y : INTEGER);
VAR i, j : INTEGER;
    q : ARRAY [0..N+1], [0..N+1] OF BOOLEAN;
BEGIN
    FOR i := 0 TO N+1 DO
        FOR j := 0 TO N+1 DO
            q [i][j] := FALSE;
        END;
    END;

    FOR i := 1 TO N DO
        q [figs [fig].pats [pat].y [i]+1][figs [fig].pats [pat].x [i]+1] := TRUE;
    END;
    W.SelectObject (dc, BorderPen);
    FOR i := 0 TO N DO
        FOR j := 0 TO N DO
            IF q [i][j] # q [i][j+1] THEN
                W.MoveToEx (dc, x+Q*j, y+Q*i-Q, NIL);
                W.LineTo   (dc, x+Q*j, y+Q*i);
            END;
            IF q [i][j] # q [i+1][j] THEN
                W.MoveToEx (dc, x+Q*j-Q, y+Q*i, NIL);
                W.LineTo   (dc, x+Q*j,   y+Q*i);
            END;
        END;
    END;
END draw_pat_border;

PROCEDURE draw_pat (dc : HDC; fig, pat : INTEGER;
                    x, y : INTEGER; brush : HBRUSH; VAR rcclip : RECT);
BEGIN
    IF check_pat_visible (fig, pat, x, y, rcclip) THEN
        draw_pat_interior (dc, fig, pat, x, y, brush);
        draw_pat_border   (dc, fig, pat, x, y);
    END;
END draw_pat;

PROCEDURE draw_field_pat_interior (dc : HDC; fig, pat : INTEGER; xf, yf : INTEGER);
VAR i : INTEGER;
    x1, y1 : INTEGER;
    brush : HBRUSH;
BEGIN
    FOR i := 1 TO N DO
        x1 := xf + figs [fig].pats [pat].x [i];
        y1 := yf + figs [fig].pats [pat].y [i];
        IF (squares_left = 0) AND NOT picture [y1, x1] OR
           (field.fig_count [y1, x1] > 1)
        THEN
            brush := BadBrush;
        ELSE
            brush := PatBrush;
        END;
        draw_pat_square (dc, fig, pat,
                         field_to_screen_x (xf),
                         field_to_screen_y (yf),
                         i, brush);
    END;
END draw_field_pat_interior;

PROCEDURE draw_field_pat (dc : HDC; i : INTEGER; VAR rcclip : RECT);
VAR fig, pat : INTEGER;
    x, y : INTEGER;
BEGIN
    fig := figures [i].fig;
    pat := figures [i].pat;
    x := fig_x (i);
    y := fig_y (i);
    IF check_pat_visible (fig, pat, x, y, rcclip) THEN
        draw_field_pat_interior (dc, fig, pat, figures [i].x, figures [i].y);
        draw_pat_border   (dc, fig, pat, x, y);
    END;
END draw_field_pat;

PROCEDURE invalidate_pat (fig, pat : INTEGER; x, y : INTEGER);
VAR rc : RECT;
BEGIN
    rc.left := x;
    rc.top  := y;
    rc.right  := x + figs [fig].pats [pat].sx * Q + 1;
    rc.bottom := y + figs [fig].pats [pat].sy * Q + 1;
    W.InvalidateRect (field_wnd, rc, FALSE);
END invalidate_pat;

PROCEDURE invalidate_field_fig (i : INTEGER);
BEGIN
    invalidate_pat (figures [i].fig, figures [i].pat, fig_x (i), fig_y (i));
END invalidate_field_fig;

PROCEDURE invalidate_pal_fig (i : INTEGER);
BEGIN
    invalidate_pat (i, cur_pat [i], pal_fig_x (i), pal_fig_y (i));
END invalidate_pal_fig;

PROCEDURE init_pats;
VAR i : INTEGER;
BEGIN
    FOR i := 1 TO NFIGS DO
        cur_pat [i] := 1;
    END;
END init_pats;

PROCEDURE put_figure (fig, pat : INTEGER; x, y : INTEGER);
BEGIN
    INC (nfigures);
    fig_in_use [fig] := TRUE;
    figures [nfigures].fig := fig;
    figures [nfigures].pat := pat;
    figures [nfigures].x   := x;
    figures [nfigures].y   := y;
    field.Update_fig_count (fig, pat, x, y, 1);
    invalidate_field_fig (nfigures);
END put_figure;

PROCEDURE del_figure (n : INTEGER);
VAR i : INTEGER;
BEGIN
    field.Update_fig_count (figures [n].fig, figures [n].pat, figures [n].x, figures [n].y, -1);
    invalidate_field_fig (n);
    FOR i := n TO nfigures-1 DO
        figures [i] := figures [i+1];
    END;
    DEC (nfigures);
END del_figure;

PROCEDURE return_figures;
VAR i : INTEGER;
BEGIN
    IF nfigures # 0 THEN
        FOR i := 1 TO NFIGS DO
            fig_in_use [i] := FALSE;
        END;
        nfigures := 0;
        field.Clear_fig_count;
        W.InvalidateRect (field_wnd, NIL, FALSE);
    END;
END return_figures;

PROCEDURE test_in_use (i : INTEGER);
VAR j : INTEGER;
BEGIN
    LOOP FOR j := 1 TO nfigures DO
            IF figures [j].fig = i THEN
                EXIT
            END;
         END;
         RETURN;
    END;
    del_figure (j);
    fig_in_use [j] := FALSE;
    invalidate_pal_fig (i);
END test_in_use;

PROCEDURE paint_field (dc : HDC; VAR urc : RECT);
VAR rc : RECT;
    i, d : INTEGER;
    brush : HBRUSH;
BEGIN
    W.GetClientRect (field_wnd, rc);

    W.FillRect (dc, urc, GrayBrush);
    W.SelectObject (dc, BorderPen);
    W.MoveToEx (dc, 0, 0, NIL);
    W.LineTo (dc, xsize_field, 0);
    W.LineTo (dc, xsize_field, ysize_field);
    W.LineTo (dc, 0, ysize_field);
    W.LineTo (dc, 0, 0);

    W.SelectObject (dc, SquarePen);
    
    FOR i := 1 TO XFIELD-1 DO
        d := i * Q;
        IF (d >= urc.left) AND (d < urc.right) THEN
            W.MoveToEx (dc, d, 1, NIL);
            W.LineTo (dc, d, ysize_field);
        END;
    END;
    FOR i := 1 TO YFIELD-1 DO
        d := i * Q;
        IF (d >= urc.top) AND (d < urc.bottom) THEN
            W.MoveToEx (dc, 1, d, NIL);
            W.LineTo   (dc, xsize_field, d);
        END;
    END;

    draw_gray_squares (dc);

    IF draw_mode THEN
        FOR i := 0 TO squares_left-1 DO
            draw_square (dc, i);
        END;
    ELSE
        FOR i := 1 TO NFIGS DO
            IF fig_in_use [i] THEN
                brush := InUseBrush;
            ELSE
                brush := PatBrush;
            END;
            draw_pat (dc, i, cur_pat [i], pal_fig_x (i), pal_fig_y (i), brush, urc);
        END;

        FOR i := 1 TO nfigures DO
            draw_field_pat (dc, i, urc);
        END;

        IF in_drag THEN
            draw_pat (dc, drag_fig, drag_pat, drag_x, drag_y, PatBrush, urc);
        END;
    END;
END paint_field;

PROCEDURE test_hit_pat (x, y : INTEGER; fig, pat : INTEGER; x0, y0 : INTEGER) : BOOLEAN;
VAR j : INTEGER;
BEGIN
    IF (x < x0) OR (y < y0) OR (x > x0 + N * Q) OR (y > y0 + N * Q) THEN
        RETURN FALSE;
    END;
    DEC (x, x0);
    DEC (y, y0);
    FOR j := 1 TO N DO
        x0 := figs [fig].pats [pat].x [j] * Q;
        y0 := figs [fig].pats [pat].y [j] * Q;
        IF (x >= x0) AND (x <= x0 + Q) AND (y >= y0) AND (y <= y0 + Q) THEN
            RETURN TRUE;
        END;
    END;
    RETURN FALSE;
END test_hit_pat;

PROCEDURE test_hit_palette (x, y : INTEGER) : INTEGER;
VAR i : INTEGER;
BEGIN
    IF y < figures_y THEN
        RETURN 0;
    END;
    FOR i := 1 TO NFIGS DO
        IF test_hit_pat (x, y, i, cur_pat [i], pal_fig_x (i), pal_fig_y (i)) THEN
            RETURN i;
        END;
    END;
    RETURN 0;
END test_hit_palette;

PROCEDURE test_hit_field (x, y : INTEGER) : INTEGER;
VAR i : INTEGER;
BEGIN
    IF y > ysize_field THEN
        RETURN 0;
    END;
    FOR i := nfigures TO 1 BY -1 DO
        IF test_hit_pat (x, y, figures [i].fig, figures [i].pat, fig_x (i), fig_y (i)) THEN
            RETURN i;
        END;
    END;
    RETURN 0;
END test_hit_field;

PROCEDURE start_drag_palette (fig : INTEGER; x, y : INTEGER);
BEGIN
    test_in_use (fig);
    in_drag := TRUE;
    drag_fig := fig;
    drag_pat := cur_pat [fig];
    fig_in_use [fig] := TRUE;
    invalidate_pal_fig (fig);

    drag_x := pal_fig_x (fig);
    drag_y := pal_fig_y (fig);
    drag_dx := x - drag_x;
    drag_dy := y - drag_y;
    
    invalidate_pat (drag_fig, drag_pat, drag_x, drag_y);
    W.SetCapture (field_wnd);
END start_drag_palette;

PROCEDURE start_drag_field (i : INTEGER; x, y : INTEGER);
BEGIN
    in_drag := TRUE;
    drag_fig := figures [i].fig;
    drag_pat := figures [i].pat;

    drag_x := fig_x (i);
    drag_y := fig_y (i);
    drag_dx := x - drag_x;
    drag_dy := y - drag_y;

    del_figure (i);
    invalidate_pat (drag_fig, drag_pat, drag_x, drag_y);
    W.SetCapture (field_wnd);
END start_drag_field;

PROCEDURE move_drag_fig (x, y : INTEGER);
VAR xm, ym : INTEGER;
BEGIN
    IF NOT in_drag THEN
        RETURN;
    END;
    invalidate_pat (drag_fig, drag_pat, drag_x, drag_y);
    drag_x := x - drag_dx;
    drag_y := y - drag_dy;
    xm := xsize_field_wnd - figs [drag_fig].pats [drag_pat].sx * Q;
    ym := ysize_field_wnd - figs [drag_fig].pats [drag_pat].sy * Q;
    IF drag_x < 0  THEN drag_x := 0;  END;
    IF drag_y < 0  THEN drag_y := 0;  END;
    IF drag_x > xm THEN drag_x := xm; END;
    IF drag_y > ym THEN drag_y := ym; END;
    invalidate_pat (drag_fig, drag_pat, drag_x, drag_y);
END move_drag_fig;

PROCEDURE fin_drag;
VAR x, y, xm, ym : INTEGER;
BEGIN
    W.ReleaseCapture;
    invalidate_pat (drag_fig, drag_pat, drag_x, drag_y);

    in_drag := FALSE;
    x := (drag_x + Q DIV 2) DIV Q; y := (drag_y + Q DIV 2) DIV Q;

    IF y >= YFIELD THEN
        fig_in_use [drag_fig] := FALSE;
        invalidate_pal_fig (drag_fig);
        RETURN;
    END;
    xm := XFIELD - figs [drag_fig].pats [drag_pat].sx;
    ym := YFIELD - figs [drag_fig].pats [drag_pat].sy;

    IF x < 0  THEN x := 0;  END;
    IF y < 0  THEN y := 0;  END;
    IF x > xm THEN x := xm; END;
    IF y > ym THEN y := ym; END;

    put_figure (drag_fig, drag_pat, x, y);
END fin_drag;

PROCEDURE toggle_drag_pat;
BEGIN
    invalidate_pat (drag_fig, drag_pat, drag_x, drag_y);
    INC (drag_pat);
    IF drag_pat > figs [drag_fig].n THEN
        drag_pat := 1;
    END;
    invalidate_pat (drag_fig, drag_pat, drag_x, drag_y);
END toggle_drag_pat;

PROCEDURE toggle_palette_pat (fig : INTEGER);
BEGIN
    test_in_use (fig);
    invalidate_pal_fig (fig);
    INC (cur_pat [fig]);
    IF cur_pat [fig] > figs [fig].n THEN
        cur_pat [fig] := 1;
    END;
    invalidate_pal_fig (fig);
END toggle_palette_pat;

PROCEDURE set_square (x, y : INTEGER; state : BOOLEAN);
BEGIN
    IF state = picture [y][x] THEN
        RETURN;
    END;
    IF state THEN
        IF squares_left = 0 THEN
            RETURN;
        END;
        DEC (squares_left);
    ELSE
        INC (squares_left);
    END;
    picture [y][x] := state;
    return_figures;
    W.InvalidateRect (field_wnd, NIL, FALSE);
END set_square;

PROCEDURE test_hit_square (x, y : INTEGER; draw_with, set_draw : BOOLEAN);
BEGIN
    x := x DIV Q;
    y := y DIV Q;
    IF (x >= XFIELD) OR (y >= YFIELD) THEN
        RETURN;
    END;
    in_drawing := TRUE;
    IF set_draw THEN
        drawing_with := draw_with;
    END;
    set_square (x, y, drawing_with);
    enable_buttons;
END test_hit_square;

PROCEDURE clear_picture;
BEGIN
    field.Clear;
    W.InvalidateRect (field_wnd, NIL, FALSE);
END clear_picture;

PROCEDURE copy_fig_to_picture (i : INTEGER);
BEGIN
    field.Put_fig (figures [i].fig, figures [i].pat, figures [i].x, figures [i].y);
    W.InvalidateRect (field_wnd, NIL, FALSE);
END copy_fig_to_picture;

PROCEDURE put_figures (VAR res : SOLUTION);
VAR i : INTEGER;
BEGIN
    return_figures;
    FOR i := 1 TO NFIGS DO
        put_figure (i, res [i].pat, res [i].x+xmin-1, res [i].y+ymin-1);
    END;
    draw_mode := FALSE;
END put_figures;

PROCEDURE set_results_scroll;
BEGIN
    W.SetScrollRange (W.GetDlgItem (dialog_wnd, R.IDC_SCROLL), W.SB_CTL, 1, nresults, TRUE);
END set_results_scroll;

PROCEDURE set_cur_result (n : INTEGER);
VAR s : ARRAY [0..99] OF CHAR;
    sol : SOLUTION;
BEGIN
    IF n = 0 THEN
        W.SetDlgItemText (dialog_wnd, R.IDC_CURSOL, "");
    ELSIF cur_result # n THEN
        result.get_solution (n-1, sol);
        put_figures (sol);
        W.wsprintf (s, "%d", n);
        W.SetDlgItemText (dialog_wnd, R.IDC_CURSOL, s);
    END;
    cur_result := n;
END set_cur_result;

PROCEDURE scroll_process (code : W.SB_ENUM);
VAR
    si : W.SCROLLINFO;
    n : INTEGER;
BEGIN
    si.cbSize := SIZE (si);
    si.fMask := W.SIF_ALL;
    W.GetScrollInfo (W.GetDlgItem (dialog_wnd, R.IDC_SCROLL), W.SB_CTL, si);
    n := si.nPos;

    CASE code OF
    | W.SB_TOP       : n := 1;
    | W.SB_BOTTOM    : n := nresults;
    | W.SB_LINELEFT  : DEC (n);
    | W.SB_LINERIGHT : INC (n);
    | W.SB_PAGELEFT  : DEC (n, 10);
    | W.SB_PAGERIGHT : INC (n, 10);
    | W.SB_THUMBTRACK,
      W.SB_THUMBPOSITION: n := si.nTrackPos;
    ELSE
    END;
    IF n > nresults THEN n := nresults END;
    IF n < 1 THEN n := 1; END;
    W.SetScrollPos (W.GetDlgItem (dialog_wnd, R.IDC_SCROLL), W.SB_CTL, n, TRUE);
    set_cur_result (n);
END scroll_process;

PROCEDURE new_solution (VAR res : SOLUTION);
BEGIN
    IF NOT result.add_solution (res) THEN
        solve.stop_solving;
        RETURN;
    END;

    set_results_scroll;
    W.SetDlgItemInt (dialog_wnd, R.IDC_SOLUTIONS, nresults, FALSE);
    IF nresults = 1 THEN
        set_cur_result (1);
    END;
END new_solution;

VAR start_time, fin_time : LONG;

PROCEDURE solution_callback_proc (VAR result : SOLUTION);
BEGIN
    W.SendMessage (field_wnd, NEW_SOLUTION_MSG, 0,
                   SYSTEM.CAST (LPARAM, SYSTEM.ADR (result)));
END solution_callback_proc;

PROCEDURE [W.CALLBACK] SolveThreadProc (param : PVOID) : DWORD;
VAR b : BOOLEAN;
BEGIN
    start_time := W.GetTickCount ();
    b := solve.search (solution_callback_proc);
    fin_time := W.GetTickCount ();
    W.SendMessage (field_wnd, EXIT_THREAD_MSG, ORD (b), 0);
    RETURN 0;
END SolveThreadProc;

PROCEDURE solve_problem;
VAR
    VAR x, y : INTEGER;
    thread : W.HANDLE;
    ti : DWORD;
BEGIN
    field.Find_upper_left_point (xmin, ymin);

    solve.clear_solve_field;
    FOR y := 0 TO YFIELD-1 DO
        FOR x := 0 TO XFIELD-1 DO
            IF picture [y][x] THEN
                solve.set_solve_field (x-xmin+1, y-ymin+1);
            END;
        END;
    END;
    W.SetDlgItemText (dialog_wnd, R.IDC_SOLUTIONS, "0");
    W.SetDlgItemText (dialog_wnd, R.IDC_TIME, "Working...");
    nresults := 0;
    set_cur_result (0);
    set_results_scroll;
    thread := W.CreateThread (NIL, 0, SolveThreadProc, NIL, W.CREATE_SET {}, ti);
    W.CloseHandle (thread);
    draw_mode := FALSE;
    solve_mode := TRUE;
    W.InvalidateRect (field_wnd, NIL, FALSE);
END solve_problem;

PROCEDURE thread_stopped (normal : BOOLEAN);
VAR s : ARRAY [0..99] OF CHAR;
    t : LONG;
BEGIN
    t := fin_time - start_time;
    W.wsprintf (s, "Time: %ld.%03ld", t DIV 1000, t MOD 1000);
    IF NOT normal THEN
       Strings.Append (" (stop)", s);
    END;
    W.SetDlgItemText (dialog_wnd, R.IDC_TIME, s);
    solve_mode := FALSE;
END thread_stopped;

PROCEDURE enable_item (item : INTEGER; state : BOOLEAN);
BEGIN
    W.EnableWindow (W.GetDlgItem (dialog_wnd, item), state);
    IF state THEN
       W.EnableMenuItem (W.GetMenu (dialog_wnd), item, W.MF_ENABLED + W.MF_BYCOMMAND);
    ELSE
       W.EnableMenuItem (W.GetMenu (dialog_wnd), item, W.MF_GRAYED  + W.MF_BYCOMMAND);
    END;
END enable_item;

PROCEDURE enable_buttons;
BEGIN
    enable_item (R.IDC_PLAY, draw_mode AND ((squares_left = 0) OR (squares_left = NFIGS * N)));
    enable_item (R.IDC_HOME, NOT draw_mode AND (nfigures # 0));
    enable_item (R.IDC_DRAW, NOT draw_mode AND NOT solve_mode);
    enable_item (R.IDC_CLEAR, draw_mode AND (squares_left # NFIGS * N));
    enable_item (R.IDC_SOLVE, NOT solve_mode AND (squares_left = 0));
    enable_item (R.IDC_STOP,  solve_mode);
    enable_item (R.IDC_SCROLL, nresults > 0);
    enable_item (R.IDC_WRITE, squares_left = 0);
    enable_item (R.IDC_READ,  NOT solve_mode);
END enable_buttons;

PROCEDURE [W.CALLBACK] FieldWinProc (hwnd : HWND; msg : UINT; wparam : WPARAM; lparam : LPARAM) : LRESULT;
TYPE PSOLUTION = POINTER TO SOLUTION;
VAR
    dc : HDC;
    ps : W.PAINTSTRUCT;
    cs : W.PCREATESTRUCT;
    x, y, i, fig : INTEGER;
    p  : PSOLUTION;
BEGIN
    CASE msg OF
    | W.WM_CREATE:
        cs := SYSTEM.CAST (W.PCREATESTRUCT, lparam);
        field_wnd := hwnd;
        recalc_field_sizes (cs^.cx, cs^.cy, cs^.style, cs^.dwExStyle);
        RETURN 0;
    | W.WM_PAINT:
        dc := W.BeginPaint (hwnd, ps);
        paint_field (field_bmpdc, ps.rcPaint);
        W.BitBlt (dc, ps.rcPaint.left, ps.rcPaint.top,
                            ps.rcPaint.right  - ps.rcPaint.left,
                            ps.rcPaint.bottom - ps.rcPaint.top,
                        field_bmpdc, ps.rcPaint.left, ps.rcPaint.top,
                        W.SRCCOPY);
        W.EndPaint (hwnd, ps);
        RETURN 0;
    | W.WM_LBUTTONDOWN:
        x := VAL (INTEGER, SYSTEM.CAST (SHORT, LOWORD (lparam)));
        y := VAL (INTEGER, SYSTEM.CAST (SHORT, HIWORD (lparam)));
        IF draw_mode AND NOT in_drawing THEN
            test_hit_square (x, y, TRUE, TRUE);
            RETURN 0;
        END;
        fig := test_hit_palette (x, y);
        IF fig > 0 THEN
            start_drag_palette (fig, x, y);
            RETURN 0;
        END;
        i := test_hit_field (x, y);
        IF i > 0 THEN
            start_drag_field (i, x, y);
            RETURN 0;
        END;
        RETURN 0;
    | W.WM_MOUSEMOVE:
        x := VAL (INTEGER, SYSTEM.CAST (SHORT, LOWORD (lparam)));
        y := VAL (INTEGER, SYSTEM.CAST (SHORT, HIWORD (lparam)));
        IF draw_mode AND in_drawing THEN
            test_hit_square (x, y, FALSE, FALSE);
        ELSIF in_drag THEN
            move_drag_fig (x, y);
        END;
        RETURN 0;
    | W.WM_LBUTTONUP:
        IF draw_mode AND in_drawing AND drawing_with THEN
            in_drawing := FALSE;
        END;
        IF in_drag THEN
            fin_drag ();
            enable_buttons ();
        END;
        RETURN 0;
    | W.WM_RBUTTONDOWN:
        x := VAL (INTEGER, SYSTEM.CAST (SHORT, LOWORD (lparam)));
        y := VAL (INTEGER, SYSTEM.CAST (SHORT, HIWORD (lparam)));
        IF draw_mode AND NOT in_drawing THEN
            test_hit_square (x, y, FALSE, TRUE);
            RETURN 0;
        END;
        IF in_drag THEN
            toggle_drag_pat;
        ELSE
            fig := test_hit_palette (x, y);
            IF fig > 0 THEN
                toggle_palette_pat (fig);
            END;
        END;
        RETURN 0;
    | W.WM_RBUTTONUP:
        IF draw_mode AND in_drawing AND NOT drawing_with THEN
            in_drawing := FALSE;
        END;
        RETURN 0;
    | EXIT_THREAD_MSG:
        thread_stopped (wparam # 0);
        enable_buttons;
        RETURN 1;
    | NEW_SOLUTION_MSG:
        p := SYSTEM.CAST (PSOLUTION, lparam);
        new_solution (p^);
        enable_buttons;
        RETURN 1;
    ELSE
    END;
    RETURN W.DefWindowProc(hwnd, msg, wparam, lparam);
END FieldWinProc;

PROCEDURE [W.CALLBACK] MainDlgProc (hwnd : HWND; msg : UINT; wparam : WPARAM; lparam : LPARAM) : BOOL;
VAR i : INTEGER;
    rc, rcd : RECT;
    pt : W.POINT;
    icon, iconsm : W.HICON;
BEGIN
    CASE msg OF
    | W.WM_INITDIALOG:
        icon := W.LoadIcon (W.GetModuleHandle (NIL), "MAIN_ICON");
        iconsm := SYSTEM.CAST (W.HICON,
                               W.LoadImage (W.GetModuleHandle (NIL), "MAIN_ICON",
                                            W.IMAGE_ICON,
                                            W.GetSystemMetrics (W.SM_CXSMICON),
				                            W.GetSystemMetrics (W.SM_CYSMICON),
                                            W.LR_DEFAULTCOLOR));
        W.SendMessage (hwnd, W.WM_SETICON, 0, SYSTEM.CAST (LPARAM, iconsm));
        W.SendMessage (hwnd, W.WM_SETICON, 1, SYSTEM.CAST (LPARAM, icon));
        W.GetWindowRect (hwnd, rcd);
        W.GetWindowRect (field_wnd, rc);
        pt.x := rc.right;
        pt.y := rc.top;
        W.ScreenToClient (hwnd, pt);
        rcd.right := rc.right + pt.y;
        W.SetWindowPos (hwnd, NIL, 0, 0,
                              rcd.right - rcd.left,
                              rcd.bottom - rcd.top,
                              W.SWP_NOMOVE + W.SWP_NOZORDER);
        dialog_wnd := hwnd;
        field.Clear_fig_count;
        clear_picture;
        enable_buttons;
        RETURN TRUE;
    | W.WM_CLOSE:
        W.EndDialog (hwnd, 1);
        RETURN TRUE;
    | W.WM_HSCROLL:
        scroll_process (VAL (W.SB_ENUM, LOWORD (wparam)));
        RETURN TRUE;
    | W.WM_COMMAND:
        CASE LOWORD (wparam) OF
        | R.IDC_CLOSE:
            W.EndDialog (hwnd, 1);
            RETURN TRUE;
        | W.IDCANCEL,    (* prevents ESC or Enter keys from closingthe dialog *)
          W.IDOK:
            RETURN TRUE;
        | R.IDC_DRAW:
            IF nfigures # 0 THEN
                clear_picture;
                FOR i := 1 TO nfigures DO
                    copy_fig_to_picture (i);
                END;
            END;            
            draw_mode := TRUE;
            in_drawing := FALSE;
            W.InvalidateRect (field_wnd, NIL, FALSE);
        | R.IDC_HOME:
            return_figures;
        | R.IDC_CLEAR:
            clear_picture;
        | R.IDC_PLAY:
            draw_mode := FALSE;
            W.InvalidateRect (field_wnd, NIL, FALSE);
        | R.IDC_SOLVE:
            solve_problem;
        | R.IDC_STOP:
            solve.stop_solving;
        | R.IDC_READ:
            IF field.Read () THEN
               draw_mode := TRUE;
               return_figures;
               W.InvalidateRect (field_wnd, NIL, FALSE);
            END;
        | R.IDC_WRITE:
            field.Write;
        END;
        enable_buttons;
        RETURN TRUE;
    ELSE
    END;
    RETURN FALSE;
END MainDlgProc;

CONST ClassName = "FIELD";

PROCEDURE Register_classes () : BOOLEAN;
VAR wc : W.WNDCLASS;
BEGIN
    wc.style := W.CS_SET {};
    wc.lpfnWndProc := FieldWinProc;
    wc.cbClsExtra := 0;
    wc.cbWndExtra := 0;
    wc.hInstance := W.GetModuleHandle (NIL);
    wc.hIcon := NIL;
    wc.hCursor := W.LoadCursor (NIL, W.IDC_ARROW);
    wc.hbrBackground := NIL;
    wc.lpszMenuName :=  NIL;
    wc.lpszClassName := SYSTEM.ADR (ClassName);
    IF W.RegisterClass (wc) = 0 THEN
        RETURN FALSE;
    END;
    BorderPen   := W.CreatePen (W.PS_SOLID, 1, 0);
    SquarePen   := W.CreatePen (W.PS_SOLID, 1, W.GetSysColor (W.COLOR_3DSHADOW));
    GrayBrush   := W.CreateSolidBrush (W.GetSysColor (W.COLOR_3DFACE));
    SquareBrush := W.CreateSolidBrush (W.GetSysColor (W.COLOR_3DHIGHLIGHT));
    PatBrush    := W.CreateSolidBrush (W.RGB (255, 0, 0));
    InUseBrush  := W.CreateSolidBrush (W.RGB (255, 255, 255));
    BadBrush    := W.CreateSolidBrush (W.RGB (128, 0, 0));
    RETURN TRUE;
END Register_classes;

BEGIN
    IF Register_classes () THEN
        pat.makefigs;
        init_pats;
        W.DialogBox (W.GetModuleHandle (NIL), "MAIN_DIALOG", NIL, MainDlgProc);
    END;
END penta.
