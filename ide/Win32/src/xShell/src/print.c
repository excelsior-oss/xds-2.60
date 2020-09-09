#include "util.h"
#include "tbuf.h"
#include "pooredit.h"
#include "res.h"
#include "shell.h"
#include "fileutil.h"
#include "config.h"

#ifndef SS_PATHELLIPSIS
#define SS_PATHELLIPSIS     0x00008000L
#endif

char * print_job_name;

static HWND AbortDlg = NULL;
static BOOL AbortFlag = FALSE;

char * test_logging (void)
{
	return getenv ("__PRINTLOG__");
}

void print_log (char * s, ...)
{
	va_list arglist;
	FILE * logfile = 0;

	va_start (arglist, s);

	if (logfile == 0)	{
		char * fname = test_logging ();
		if (! fname) return;
		logfile = fopen (fname, "a");
	}
	if (! logfile) return;

	vfprintf (logfile, s, arglist);
	fflush (logfile);
}

BOOL CALLBACK AbortProc (HDC hdc, int Code)
{
	MSG msg;

	if (!AbortDlg) return TRUE;

	while (!AbortFlag && PeekMessage (&msg, NULL, 0, 0, TRUE))
		if (! IsDialogMessage (AbortDlg, &msg))	{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	return !AbortFlag;
}

LRESULT CALLBACK AbortDlgProc (HWND hdlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HWND w;
	switch(msg)	{
		case WM_COMMAND:	return AbortFlag = TRUE;
		case WM_INITDIALOG:
			w = GetDlgItem (hdlg, PRINTABORT_NAME);
			SetWindowLong (w, GWL_STYLE, GetWindowLong (w, GWL_STYLE) | SS_PATHELLIPSIS);
			Center_window (hdlg);
			SetFocus (GetDlgItem (hdlg, IDCANCEL));
			SetWindowText (w, print_job_name);
			return TRUE;
		case WM_HELP:
			return Dialog_help (PRINTABORT_DIALOG);
	}
	return FALSE;
}

HFONT	make_print_font (HDC dc)
{
	LOGFONT lf = PrintLogFont;
	lf.lfHeight = -MulDiv (PrintFontSize, GetDeviceCaps (dc, LOGPIXELSY), 720);

	print_log ("Creating font: size=%d\n", lf.lfHeight);
	
	return CreateFontIndirect (&lf);
}

void	setup_print_page (HDC dc, RECT * rc)
{
	int phys_x, phys_y, log_x, log_y, offset_x, offset_y, res_x, res_y;
	int l, t, r, b;

	res_x =	GetDeviceCaps (dc, LOGPIXELSX),
	res_y =	GetDeviceCaps (dc, LOGPIXELSY),
	log_x =	GetDeviceCaps (dc, HORZRES),
	log_y =	GetDeviceCaps (dc, VERTRES),
	phys_x = GetDeviceCaps (dc, PHYSICALWIDTH),
	phys_y = GetDeviceCaps (dc, PHYSICALHEIGHT),
	offset_x = GetDeviceCaps (dc, PHYSICALOFFSETX),
	offset_y = GetDeviceCaps (dc, PHYSICALOFFSETY);

	print_log ("res_x=%d; res_y=%d; log_x=%d; log_y=%d\n",
		res_x, res_y, log_x, log_y);

	print_log ("phys_x=%d; phys_y=%d; offset_x=%d; offset_y=%d\n",
		phys_x, phys_y, offset_x, offset_y);

	print_log ("PrintMargins: %d %d %d %d\n",
		PrintMargins.left, PrintMargins.right,
		PrintMargins.top, PrintMargins.bottom);

	l = PrintMargins.left  * res_x / 2540;
	r = PrintMargins.right * res_x / 2540;
	t = PrintMargins.top   * res_y / 2540;
	b = PrintMargins.bottom * res_y / 2540;
	
	print_log ("PrintMargins in puxels: %d %d %d %d\n", l, r, t, b);

	if (l < offset_x) l = offset_x;
	if (t < offset_y) t = offset_y;
	
	rc->left = l - offset_x;
	rc->top  = t - offset_y;
	rc->right = phys_x - offset_x - r;
	rc->bottom = phys_y - offset_y - b;

	if (rc->right > log_x) rc->right = log_x;
	if (rc->bottom > log_y) rc->bottom = log_y;

	print_log ("Final rc: %d %d %d %d\n", rc->left, rc->top, rc->right, rc->bottom);
}

int	get_text_to_buf (TBUF * buf, char * s, int y, int x, int size, int tab_size, BOOL * extra)
{
	char * p;
	int i, len, xphys, newxphys;

	if (! tbuf_get_line (buf, y, &p, &len) || !len)
		return 0;

	i = 0;
	for (xphys = 0; xphys < x; ++i)
		if (i == len) return 0;
		else if (p[i] == '\t')
			xphys = (xphys + tab_size) / tab_size * tab_size;
		else ++ xphys;
	if (xphys > x)
		memset (s, ' ', xphys-x);
	while (xphys < x+size)	{
		if (i == len) return xphys - x;
		else if (p[i] == '\t')	{
			newxphys = (xphys + tab_size) / tab_size * tab_size;
			memset (s+xphys-x, ' ', newxphys-xphys);
			xphys = newxphys;
		} else
			s[xphys++-x] = p[i];
		++i;
	}
	if (i < len) *extra = TRUE;
	return size;
}

void	newpage (HDC dc, RECT* rc, int page, int subpage)
{
	char s [100], t [100];
	char * p;
	SIZE size;
	RECT r;
	HPEN pen;

	if (subpage)
		sprintf (s, "%d/%d", page, subpage+1);
	else
		sprintf (s, "%d", page);
	strcpy (t, "Page ");
	strcat (t, s);

	SetDlgItemText (AbortDlg, PRINTABORT_PAGE, s);
	if (!PrintFileName && !PrintPageNumbers) return;
	r = *rc;
	if (PrintPageNumbers)	{
		SetTextAlign (dc, TA_RIGHT);
		TextOut (dc, rc->right, rc->top, t, strlen(t));
		SetTextAlign (dc, TA_LEFT);
		strcat(t, " ");
		GetTextExtentPoint32 (dc, t, strlen (t), &size);
		r.right -= size.cx;
	}
	if (PrintFileName)	{
		p = PrintFilePath ? print_job_name : name_only (print_job_name);
		DrawText (dc, p, -1, &r, DT_PATH_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
	}
	pen = SelectObject (dc, GetStockObject (BLACK_PEN));
	MoveToEx (dc, rc->left, rc->bottom, 0);
	LineTo (dc, rc->right, rc->bottom);
	SelectObject (dc, pen);
}

void	print_edit (TBUF * buf, char * docname,
		    int x1, int y1, int x2, int y2, HDC dc,
		    int ncopies)
{
	HCURSOR cur_save;
	DOCINFO di;
	int dx, dy, sx, sx2, sy, x0, y0, y, copy, nline;
	int npage, nsubpage;
	DWORD err;
	RECT RCpage, RCnum, RCtext, RCtext2, RCheader;
	BOOL more_print;
	char * p;
	char s [100];
	int len, tab_size;
	HFONT font, ofont;
	TEXTMETRIC tm;

	font = make_print_font (dc);
	ofont = SelectObject (dc, font);
	GetTextMetrics (dc, &tm);
	dx = tm.tmAveCharWidth;
	dy = tm.tmHeight;
	setup_print_page (dc, &RCpage);

	print_log ("font metrics: width=%d height=%d\n", dx, dy);

	if (PrintFileName || PrintPageNumbers)	{
		RCheader = RCpage;
		RCpage.top += dy + dy/2;
		RCheader.bottom = RCheader.top + dy + dy/4;
	}	
	
	RCtext2 = RCtext = RCnum = RCpage;
	RCnum.right = RCnum.left + dx * 6;
	if (PrintLineNumbers) RCtext.left = RCnum.right + dx;

	sx = (RCtext.right  - RCtext.left)  / dx;
	sx2 =(RCtext2.right - RCtext2.left) / dx;
	sy = (RCtext.bottom - RCtext.top)   / dy;

	print_log ("final RCtext: %d %d %d %d", RCtext.left, RCtext.top, RCtext.right, RCtext.bottom);

//	if (RCtext.right < RCtext.left || (RCtext.right - RCtext.left) / sx < 20 ||
//	    RCtext.bottom < RCtext.top || (RCtext.bottom - RCtext.top) / sy < 10)

	print_log ("dx=%d dy=%d sx=%d sy=%d sx2=%d", dx, dy, sx, sy, sx2);

	if (RCtext.right < RCtext.left || sx < 10 ||
	    RCtext.bottom < RCtext.top || sy < 5)
	{
		print_log ("Big font condition happened");
		MsgBox ("Print Error", MB_OK, "Too big printer font selected.\n"
			"Select smaller print font and try again", MB_OK);
		return;
	}
	print_log ("Big font condition not happened");
	cur_save = SetCursor (LoadCursor (0, IDC_WAIT));
	
	AbortFlag = FALSE;
	SetAbortProc (dc, AbortProc);

	memset (&di, 0, sizeof (di));
	di.cbSize = sizeof (di);
	di.lpszDocName = docname;

	if (StartDoc(dc, &di) < 0)	{
		err = GetLastError();
		SetCursor(cur_save);
		if (err != ERROR_CANCELLED)
			MsgBox ("Print Error", MB_OK|MB_ICONSTOP,
				"Unable to start print job\n%s", EText (err));
		return;
	}

	print_job_name = docname;
	AbortDlg = CreateDialog (MyInstance, MAKEINTRESOURCE (PRINTABORT_DIALOG),
				 FrameWindow, (DLGPROC)AbortDlgProc);
	ShowWindow (AbortDlg, SW_NORMAL);
	UpdateWindow (AbortDlg);

	SetCursor (cur_save);
	SetBkMode (dc, TRANSPARENT);
	tab_size = tbuf_tab_size (buf);
	p = Malloc (sx + tab_size);
	npage = 0;
	nsubpage = 1;
	for (copy = 0; copy < ncopies && !AbortFlag; ++copy)
		if (PrintOnNextLine)	{
			y = y1;
			x0 = 0;
			while (y < y2 && !AbortFlag)	{
				++ npage;
				StartPage(dc);
				newpage (dc, &RCheader, npage, 0);
				for (nline = 0; nline < sy && y < y2; nline++)	{
					if (PrintLineNumbers && !x0)	{
						sprintf (s, "%6d", y+1);
						SetTextAlign (dc, TA_RIGHT);
						TextOut (dc, RCnum.right, nline*dy+RCnum.top, s, strlen(s));
						SetTextAlign (dc, TA_LEFT);
					}
					more_print = FALSE;
					len = get_text_to_buf (buf, p, y, x0, sx, tab_size, &more_print);
					if (len)
						TextOut (dc, RCtext.left, nline*dy+RCtext.top, p, len);
					if (more_print)
						x0 += len;
					else	{
						++y;
						x0 = 0;
					}
				}
				EndPage (dc);
			}
		} else
			for (y0 = y1; y0 < y2 && !AbortFlag; y0 += sy)	{
				StartPage(dc);
				++ npage;
				nsubpage = 1;
				newpage (dc, &RCheader, npage, 0);
				more_print = FALSE;
				for (y = y0; y < y2; y++)	{
					if (PrintLineNumbers)	{
						sprintf (s, "%6d", y+1);
						SetTextAlign (dc, TA_RIGHT);
						TextOut (dc, RCnum.right, (y-y0)*dy+RCnum.top, s, strlen(s));
						SetTextAlign (dc, TA_LEFT);
					}
					len = get_text_to_buf (buf, p, y, 0, sx, tab_size, &more_print);
					if (len)
						TextOut (dc, RCtext.left, (y-y0)*dy+RCtext.top, p, len);
				}
				EndPage (dc);
				x0 += sx;
				if (!PrintOnNextPage) more_print = FALSE;
				while (!AbortFlag && more_print)	{
					StartPage(dc);
					newpage (dc, &RCheader, npage, nsubpage);
					more_print = FALSE;
					for (y = y0; y < y2 && y < y0+sy; y++)	{
						len = get_text_to_buf (buf, p, y, x0, sx2, tab_size, &more_print);
						if (len)
							TextOut (dc, RCtext2.left, (y-y0)*dy+RCtext2.top, p, len);
					}
					x0 += sx2;
					EndPage (dc);
					++ nsubpage;
				}
			}
	free (p);
	SelectObject (dc, ofont);
	DeleteObject (font);
	if (!AbortFlag)
		EndDoc(dc);
	DestroyWindow (AbortDlg);
}

void	Print_editor (HWND hwnd, HWND editwnd, TBUF * buf, int file)
{
	PRINTDLG dlg;
	int x1, x2, y1, y2;

	Editor_get_block (editwnd, &x1, &y1, &x2, &y2);

	memset (&dlg, 0, sizeof (dlg));
	dlg.lStructSize = sizeof (dlg);
	dlg.hwndOwner = FrameWindow;
	dlg.hInstance = MyInstance;
	dlg.Flags = PD_RETURNDC | PD_SHOWHELP | PD_HIDEPRINTTOFILE | PD_NOPAGENUMS | PD_COLLATE;

	if (x1 != x2 || y1 != y2)
		dlg.Flags |= PD_SELECTION;
	
	lock_help ++;
	if (!PrintDlg (&dlg))	{
		lock_help --;
		return;
	}
	lock_help --;

	EnableWindow (hwnd, 0);
	EnableWindow (editwnd, 0);

	print_edit (buf,
			filedir [file],
			(dlg.Flags & PD_SELECTION) ? x1 : 0,
			(dlg.Flags & PD_SELECTION) ? y1 : 0,
			(dlg.Flags & PD_SELECTION) ? x2 : 0,
			(dlg.Flags & PD_SELECTION) ? y2 : Editor_nlines (editwnd),
			 dlg.hDC,
			 (dlg.Flags & PD_COLLATE) ? dlg.nCopies : 1);

	DeleteDC (dlg.hDC);
	EnableWindow (hwnd, 1);
	EnableWindow (editwnd, 1);
}
