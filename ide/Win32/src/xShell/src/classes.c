#include "util.h"
#include "shell.h"
#include "classes.h"

int	SystemFontHeight;

COLORREF GrayColor;
COLORREF DarkColor;
COLORREF LightColor;
COLORREF TextColor;

HPEN	LightPen  = 0;
HPEN	DarkPen   = 0;
HPEN	GrayPen   = 0;
HPEN	TextPen   = 0;
HBRUSH	GrayBrush = 0;
HFONT	NonBoldFont = 0;
int	NonBoldFontSize;

DWORD	custom_colors [16];

#define STANDARD_TOOLBAR 1

/* ---------------- Common subroutines ------------------------------ */

int	Calc_font_height (HFONT f)
{
	TEXTMETRIC tm;
	HDC dc = GetDC (NULL);
	HFONT of = SelectObject (dc, f);
	GetTextMetrics (dc, &tm);
	SelectObject (dc, of);
	ReleaseDC (NULL, dc);
	return tm.tmHeight;
}

void	calc_system_font_height (void)
{
	TEXTMETRIC tm;
	HDC dc = GetDC (NULL);
	GetTextMetrics (dc, &tm);
	ReleaseDC (NULL, dc);
	SystemFontHeight = tm.tmHeight;
}

void	inset_frame (HDC hdc, RECT * rc, HPEN light, HPEN dark)
{
	HPEN old;
	rc->right --; rc->bottom --;
	old = SelectObject (hdc, dark);
	MoveToEx (hdc, rc->right-1, rc->top, NULL);
	LineTo (hdc, rc->left, rc->top);
	LineTo (hdc, rc->left, rc->bottom);
	SelectObject (hdc, light);
	MoveToEx (hdc, rc->left, rc->bottom, NULL);
	LineTo (hdc, rc->right, rc->bottom);
	LineTo (hdc, rc->right, rc->top-1);
	rc->left ++; rc->top ++;
	SelectObject (hdc, old);
}

void	raised_frame (HDC hdc, RECT * rc, HPEN light, HPEN dark)
{
	HPEN old;
	rc->right --;
	rc->bottom --;
	old = SelectObject (hdc, light);
	MoveToEx (hdc, rc->right, rc->top, NULL);
	LineTo (hdc, rc->left, rc->top);
	LineTo (hdc, rc->left, rc->bottom+1);
	SelectObject (hdc, dark);
	MoveToEx (hdc, rc->left+1, rc->bottom, NULL);
	LineTo (hdc, rc->right, rc->bottom);
	LineTo (hdc, rc->right, rc->top);
	rc->left ++; rc->top ++;
	SelectObject (hdc, old);
}

void	Draw_inset (HDC hdc, RECT * rc)
{
	inset_frame (hdc, rc, LightPen, DarkPen);
}

void	Draw_raised (HDC hdc, RECT * rc)
{
	raised_frame (hdc, rc, LightPen, DarkPen);
}

void	Draw_3dframe (HDC hdc, RECT * rc)
{
	inset_frame  (hdc, rc, LightPen, DarkPen);
	raised_frame (hdc, rc, LightPen, DarkPen);
}

#define BRIGHTNESS(c)	(GetRValue (c)+GetGValue(c)+GetBValue(c))

HBITMAP	dither_bitmap (HBITMAP bmp)
{
	BITMAP b;
	HDC dc;
	int i, j, k, l, s;
	ULONG thres1, thres2;
	COLORREF c, d;
	BYTE arr [1000][3];
	BITMAPINFOHEADER bi;

	if (!GetObject (bmp, sizeof (b), &b)) return bmp;
	dc = GetDC (0);

	bi.biSize = sizeof (bi);
	bi.biWidth = b.bmWidth;
	bi.biHeight = b.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 3 * b.bmHeight * b.bmWidth;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	GetDIBits (dc, bmp, 0, b.bmHeight, 0,   (LPBITMAPINFO) &bi, DIB_RGB_COLORS);
	GetDIBits (dc, bmp, 0, b.bmHeight, arr, (LPBITMAPINFO) &bi, DIB_RGB_COLORS);

	thres1 = BRIGHTNESS (DarkColor);
	thres2 = (BRIGHTNESS (GrayColor) + BRIGHTNESS (LightColor))/2;
	s = b.bmHeight * b.bmWidth;
	for (i = 0; i < s; i ++)	{
		c = arr [i][0] + arr [i][1] + arr [i][2];
		if (c <= thres1)
			c = DarkColor;
		else if (c <= thres2)
			c = GrayColor;
		else
			c = LightColor;
//		arr [i][0] = GetRValue (c);
//		arr [i][1] = GetGValue (c);
//		arr [i][2] = GetBValue (c);
		arr [i][0] = GetBValue (c);
		arr [i][1] = GetGValue (c);
		arr [i][2] = GetRValue (c);

	}
	for (i = 1; i < b.bmHeight; i ++)
		for (j = b.bmWidth-2; j >= 0; j --)	{
			k = i*b.bmWidth+j;
			l = k-b.bmWidth+1;
			c = RGB (arr [k][0], arr [k][1], arr [k][2]);
			if (c == GrayColor) continue;
			d = RGB (arr [l][0], arr [l][1], arr [l][2]);
			if (d != GrayColor) continue;
			d = c == LightColor ? DarkColor : LightColor;
			arr [l][0] = GetRValue (d);
			arr [l][1] = GetGValue (d);
			arr [l][2] = GetBValue (d);
		}

	bmp = CreateDIBitmap(dc, &bi, CBM_INIT, arr, (LPBITMAPINFO) &bi, DIB_RGB_COLORS);
	ReleaseDC (0, dc);
	if (bmp==0) exit(0);
	return bmp;
}

BOOL	Load_and_dither (int res, HBITMAP * bmp, HBITMAP * dbmp)
{
	* bmp = CreateMappedBitmap (MyInstance, res, 0, 0, 0);
	if (!*bmp) return FALSE;
	*dbmp = dither_bitmap (*bmp);
	if (!*dbmp)	{
		DeleteObject (*bmp);
		*bmp = 0;
		return FALSE;
	}
	return TRUE;
}

/* --------------------- Tool bar class ----------------------------- */

#define BAR_MAX_ITEMS 40
#define BARSTRINGSIZE 200

typedef	struct	{
		HBITMAP bmp;
		HBITMAP dbmp;
		int	cmd;
		BOOL	enabled;
		BOOL	present;
		char  * string;
		RECT	rect;
	}
		ToolBarItem;

typedef struct	{
		int	  count;
		int	  pressed;
		int	  tracked;
		ToolBarItem * items;
		HWND	  hwnd;
		HWND	  status_bar;
		char    * save_buffer;
	}
		ToolBarData;

void	draw_frame (HDC dc, RECT * rc, HPEN pen)
{
	HPEN oldpen = SelectObject (dc, pen);
	MoveToEx (dc, rc->left, rc->top, 0);
	LineTo (dc, rc->right-1, rc->top);
	LineTo (dc, rc->right-1, rc->bottom-1);
	LineTo (dc, rc->left, rc->bottom-1);
	LineTo (dc, rc->left, rc->top);
	SelectObject (dc, oldpen);
}

void	paint_toolbar (ToolBarData * d)
{
	HDC dc, bmpdc;
	PAINTSTRUCT ps;
	RECT rc;
	HPEN old;
	int i, s;

	dc = BeginPaint (d->hwnd, &ps);
	bmpdc = CreateCompatibleDC (dc);

	for (i = 0; i < d->count; i ++)	{
		if (! d->items [i].present) continue;
		rc = d->items [i].rect;
		draw_frame (dc, &rc, TextPen);
		++ rc.left; -- rc.right;
		++ rc.top;  -- rc.bottom;

		s = i == d->pressed;
		(s ? Draw_inset : Draw_raised) (dc, &rc);
		if (s)	{
			old = SelectObject (dc, DarkPen);
			MoveToEx (dc, rc.right-1, rc.top, NULL);
			LineTo (dc, rc.left, rc.top);
			LineTo (dc, rc.left, rc.bottom);
			SelectObject (dc, old);
			rc.left += s;
			rc.top += s;
		}
		draw_frame (dc, &rc, GrayPen);
		rc.left ++; rc.top ++;
		if (!s)	{
			rc.bottom --;
			rc.right --;
		}

		SelectObject (bmpdc, d->items [i].enabled ? d->items [i].bmp :
							    d->items [i].dbmp);
		BitBlt (dc, rc.left, rc.top,
			rc.right-rc.left, rc.bottom-rc.top, bmpdc, 0, 0, SRCCOPY);

		ExcludeClipRect (dc, d->items[i].rect.left, d->items[i].rect.top,
				     d->items[i].rect.right, d->items[i].rect.bottom);
	}
	DeleteDC (bmpdc);

	GetClientRect (d->hwnd, &rc);
	Draw_raised (dc, &rc);
	FillRect (dc, &rc, GrayBrush);

	EndPaint (d->hwnd, &ps);
}

int	button_by_point (ToolBarData * d, int x, int y)
{
	int i;
	for (i = 0; i < d->count; i++)
		if (d->items [i].present &&
		    x >= d->items [i].rect.left &&
		    x <  d->items [i].rect.right &&
		    y >= d->items [i].rect.top &&
		    y <  d->items [i].rect.bottom)
			return i;
	return -1;
}

void	set_status_bar (ToolBarData * d, int i)
{
	char lbuf [BARSTRINGSIZE];

	if (i == d->tracked) return;
	if (!d->save_buffer || !d->status_bar) return;
	if (i >= 0)	{
		if (d->tracked < 0)	{
			GetWindowText (d->status_bar, d->save_buffer, BARSTRINGSIZE);
			d->save_buffer [BARSTRINGSIZE-1] = 0;
		}
		SetWindowText (d->status_bar, d->items [i].string ? d->items [i].string : "");
	} else	{
		GetWindowText (d->status_bar, lbuf, BARSTRINGSIZE);
		lbuf [BARSTRINGSIZE-1] = 0;
		if (!strcmp (lbuf, d->items [d->tracked].string ? d->items [d->tracked].string : ""))
			SetWindowText (d->status_bar, d->save_buffer);

	}
	d->tracked = i;
}

ToolBarData * TData;

LRESULT	CALLBACK ToolBarProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	int x, y;
	RECT rc;
	ToolBarData * d = (ToolBarData *) GetWindowLong (hwnd, 0);

	switch (msg)	{
	case WM_CREATE:
			d = Malloc (sizeof (ToolBarData));
			if (!d)
				return -1;
			d->items = Malloc (sizeof (ToolBarItem) * BAR_MAX_ITEMS);
			if (!d->items)	{
				free (d);
				return -1;
			}
			TData = d;
			d->count = 0;
			d->status_bar = 0;
			d->hwnd = hwnd;
			d->pressed=-1;
			d->tracked=-1;
			d->save_buffer = Malloc (BARSTRINGSIZE);
			if (d->save_buffer)
				d->save_buffer[0] = 0;
			SetWindowLong (hwnd, 0, (DWORD) d);
			return 0;
	case WM_DESTROY:
			for (i = 0; i < d->count; i ++)	{
				DeleteObject (d->items [i].bmp);
				DeleteObject (d->items [i].dbmp);
				free (d->items [i].string);
			}
			free (d->items);
			free (d->save_buffer);
			free (d);
			return 0;

	case WM_SIZE:
			InvalidateRect (hwnd, 0, 0);
			return 0;

	case WM_PAINT:
			paint_toolbar (d);
			return 0;

	case WM_LBUTTONDOWN:
			SetActiveWindow (GetParent (hwnd));
			x = LOWORD (lparam); y = HIWORD (lparam);
			i = button_by_point (d, x, y);
			if (i==d->pressed) return 0;
			if (i>=0)	{
				if (d->pressed!=-1)
					InvalidateRect (hwnd, &d->items [d->pressed].rect, 0);
				if (d->items [i].enabled)
					InvalidateRect (hwnd, &d->items [d->pressed=i].rect, 0);
			}
			return 0;

	case WM_LBUTTONUP:
			if (d->pressed==-1) return 0;
			InvalidateRect (hwnd, &d->items [d->pressed].rect, 0);
			i = d->items [d->pressed].cmd;
			d->pressed=-1;
			set_status_bar (d, -1);
			ReleaseCapture ();
			UpdateWindow (hwnd);
			SendMessage (FrameWindow, WM_COMMAND, i, 0);
			return 0;
	case WM_MOUSEMOVE:
			GetClientRect (hwnd, &rc);
			x = LOWORD (lparam); y = HIWORD (lparam);
			if (x < rc.left || x >= rc.right ||
			    y < rc.top || y >= rc.bottom)	{
				ReleaseCapture ();
				set_status_bar (d, -1);
				if (d->pressed >= 0)	{
					InvalidateRect (hwnd, &d->items [d->pressed].rect, 0);
					d -> pressed = -1;
				}
				return 0;
			}
			SetCapture (hwnd);

			i = button_by_point (d, x, y);
			set_status_bar (d, i);

			if (wparam & MK_LBUTTON && i!=d->pressed)	{
				if (d->pressed != -1)
					InvalidateRect (hwnd, &d->items [d->pressed].rect, 0);
				d->pressed = -1;
				if (i != -1 && d->items [i].enabled)
					InvalidateRect (hwnd, &d->items [d->pressed=i].rect, 0);
			}
			return 0;

	}
	return DefWindowProc (hwnd, msg, wparam, lparam);
}

void	delete_toolbar_button (HWND bar, int n)
{
	ToolBarData * d;
	int dx, i;

	d = (ToolBarData *) GetWindowLong (bar, 0);
	set_status_bar (d, -1);
	free (d -> items [n].string);
	if (n)
		dx = d -> items [n-1].rect.right;
	else
		dx = d -> items [n].rect.left;
	dx = d -> items [n].rect.right - dx;

	for (i = n; i < d->count-1; i++)	{
		d -> items [i] = d -> items [i+1];
		d -> items [i].rect.left -= dx;
		d -> items [i].rect.right -= dx;
	}
	-- d -> count;
	InvalidateRect (bar, 0, 0);
}

#if !STANDARD_TOOLBAR

void	Delete_toolbar_buttons (HWND bar, int from, int to)
{
	int i;
	ToolBarData * d;

	d = (ToolBarData *) GetWindowLong (bar, 0);
	for (i = d->count-1; i >= 0; i--)
		if (d->items [i].cmd >= from && d->items [i].cmd <= to)
			delete_toolbar_button (bar, i);

}

void	Add_toolbar_button (HWND bar, HBITMAP bmp, HBITMAP dbmp, char * string, int cmd, int pause, int after)
{
	int cx, cy, dx;
	int x, i;
	RECT rc;
	ToolBarData * d;
	BITMAP b;
	int num;

	d = (ToolBarData *) GetWindowLong (bar, 0);
	set_status_bar (d, -1);
	num = d -> count;
	if (after)
		for (i = 0; i < d -> count; i ++)
			if (d -> items [i].cmd == after)	{
				num = i+1;
				break;
			}
	GetObject (bmp, sizeof (b), &b);
	cx=b.bmWidth;
	cy=b.bmHeight;
	x = 4;

	cx += 6;
	cy += 6;
	dx = cx+1;
	if (num)	{
		x = d->items [num-1].rect.right + 1;
		if (pause) 	{
			x += cx/2;
			dx += cx/2;
		}
	}

	for (i = d -> count - 1; i >= num; i--)	{
		d -> items [i].rect.left += dx;
		d -> items [i].rect.right += dx;
		d -> items [i+1] = d -> items [i];
	}

	d->items [num].bmp = bmp;
	d->items [num].dbmp = dbmp;
	d->items [num].enabled = TRUE;
	d->items [num].cmd = cmd;
	d->items [num].string = string ? alloc_copy (string) : 0;
	d->items [num].rect.left = x;
	d->items [num].rect.right = x+cx;
	d->items [num].rect.top = 2;
	d->items [num].rect.bottom = 2+cy;
	d->items [num].present = TRUE;

	d->count ++;
	GetClientRect (bar, &rc);
	cy += 5;
	if (cy > rc.bottom)
		SetWindowPos (bar, NULL, 0, 0,
			      rc.right, cy, SWP_NOZORDER|SWP_NOMOVE);
	InvalidateRect (bar, 0, 0);
}

void	Add_toolbar (HWND bar, int Resource, int cmd, int pause, int after)
{
	HBITMAP bmp, dbmp;
	char strbuf [BARSTRINGSIZE];
	char * string;
	Load_and_dither (Resource, &bmp, &dbmp);
	string = 0;
	if (LoadString (MyInstance, cmd, strbuf, sizeof (strbuf)))
		string = strbuf;
	Add_toolbar_button (bar, bmp, dbmp, string, cmd, pause, after);
}

BOOL	Enable_toolbar_button (HWND bar, int cmd, BOOL enable)
{
	int i;
	ToolBarData * d;

	if (! bar) return FALSE;
	d = (ToolBarData*) GetWindowLong (bar, 0);
	for (i = 0; i < d->count; i ++)
		if (d->items [i].cmd == cmd)	{
			if (d->items [i].enabled == enable)
				return FALSE;
			d->items [i].enabled = enable;
			InvalidateRect (bar, &d->items [i].rect, 0);
			return TRUE;
		}
	return TRUE;
}

void	show_toolbar_button (HWND bar, int cmd, BOOL show)
{
	int i, dx;
	ToolBarData * d;

	d = (ToolBarData*) GetWindowLong (bar, 0);
	for (i = 0; i < d->count; i ++)
		if (d->items [i].cmd == cmd) break;
	if (i == d -> count) return;
	if (d->items [i].present == show) return;
	d->items [i].present = show;
	dx = d->items [i].rect.right - d->items [i].rect.left;
	if (!show) dx = -dx;
	for (i = i+1; i < d->count; i++)	{
		d->items [i].rect.left += dx;
		d->items [i].rect.right += dx;
	}
	InvalidateRect (bar, 0, 0);
}

void	Bind_status_bar (HWND toolbar, HWND statusbar)
{
	ToolBarData * d;
	d = (ToolBarData*) GetWindowLong (toolbar, 0);
	d -> status_bar = statusbar;
}

void	Redither_toolbar (HWND bar)
{
	ToolBarData * d;
	int i;
	HBITMAP bmp;

	d = (ToolBarData*) GetWindowLong (bar, 0);
	for (i = 0; i < d->count; i ++)	{
		bmp = dither_bitmap (d->items [i].bmp);
		if (bmp)	{
			DeleteObject (d->items [i].dbmp);
			d->items [i].dbmp = bmp;
		}
	}
}

HWND	Make_empty_toolbar (HWND parent)
{
	return CreateWindow ("shelltoolbar", "",
			     WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
			     0, 0, 1500, 10, parent, 0,
			     MyInstance, NULL);

}
#endif

/* --------------------- Quick Static class ------------------------- */

#define Q_BITMAP 0
#define Q_FONT 4
#define Q_SIZE 8

void	RedrawQBitmap (HWND w)
{
	char t [300];
	HBITMAP bmp;
	HBRUSH br;
	HFONT f, of;
	HDC hdc, dc;
	RECT rc;

	GetWindowText (w, t, sizeof (t)-1);
	t [sizeof (t)-1] = 0;
	bmp = (HBITMAP) GetWindowLong (w, Q_BITMAP);
	dc = GetDC (w);
	hdc = CreateCompatibleDC (dc);
	SelectObject (hdc, bmp);
	br = (HBRUSH) SendMessage (GetParent (w), WM_CTLCOLORSTATIC,
		(WPARAM) hdc, (LPARAM) w);
	if (!br)
		br = GetStockObject (WHITE_BRUSH);
	GetClientRect (w, &rc);
	FillRect (hdc, &rc, br);
	f = (HFONT) GetWindowLong (w, Q_FONT);
	if (f) of = SelectObject (hdc, f);
	TextOut (hdc, 0, 0, t, strlen (t));
	if (f) SelectObject (hdc, of);
	BitBlt (dc, 0, 0, rc.right, rc.bottom, hdc, 0, 0, SRCCOPY);
	DeleteDC (hdc);
	ReleaseDC (w, dc);
}

void	PutBitmap (HWND hwnd, HDC hdc)
{
	HDC dc;
	HBITMAP bmp;
	RECT rc;

	bmp = (HBITMAP) GetWindowLong (hwnd, Q_BITMAP);
	GetClientRect (hwnd, &rc);
	dc = CreateCompatibleDC (hdc);
	SelectObject (dc, bmp);
	BitBlt (hdc, 0, 0, rc.right, rc.bottom, dc, 0, 0, SRCCOPY);
	DeleteDC (dc);
}

LRESULT	CALLBACK QStaticProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HBITMAP bmp;
	RECT rc;
	HDC hdc;
	LRESULT lr;
	switch (msg)	{
	case WM_CREATE:
			GetClientRect (hwnd, &rc);
			hdc = GetDC (hwnd);
			bmp = CreateCompatibleBitmap (hdc, rc.right, rc.bottom);
			ReleaseDC (hwnd, hdc);
			SetWindowLong (hwnd, Q_BITMAP, (LONG) bmp);
			RedrawQBitmap (hwnd);
			break;
	case WM_SIZE:
			GetClientRect (hwnd, &rc);
			hdc = GetDC (hwnd);
			bmp = CreateCompatibleBitmap (hdc, rc.right, rc.bottom);
			ReleaseDC (hwnd, hdc);
			DeleteObject ((HBITMAP) GetWindowLong (hwnd, Q_BITMAP));
			SetWindowLong (hwnd, Q_BITMAP, (LONG) bmp);
			RedrawQBitmap (hwnd);
			break;
		
	case WM_SETFONT:
			SetWindowLong (hwnd, Q_FONT, wparam);
			break;
	case WM_SETTEXT:
			lr = DefWindowProc (hwnd, msg, wparam, lparam);
			RedrawQBitmap (hwnd);
			return lr;
	case WM_PAINT:
			hdc = BeginPaint (hwnd, &ps);
			PutBitmap (hwnd, hdc);
			EndPaint (hwnd, &ps);
			return 0;
	case WM_DESTROY:
			DeleteObject ((HBITMAP) GetWindowLong (hwnd, Q_BITMAP));
			return 0;
	}
	return DefWindowProc (hwnd, msg, wparam, lparam);
}

/* --------------------- 3D progress indicator class ---------------- */

void	DrawProgress (HWND hwnd, HDC hdc, long pos, long limit)
{
	RECT rcb, rcf;

	GetClientRect (hwnd, &rcb);
	Draw_inset (hdc, & rcb);
	rcf = rcb;
	if (limit)
		rcf.right = rcb.left = (int) (rcf.right * pos / limit);
	Draw_raised (hdc, & rcf);
	Draw_raised (hdc, & rcf);
	FillRect (hdc, &rcf, GrayBrush);
	FillRect (hdc, &rcb, GrayBrush);
}

LRESULT	CALLBACK ProgressProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	long pos, limit;
	switch (msg)	{
	case WM_CREATE:
			SetWindowLong (hwnd, 0, 0);
			SetWindowLong (hwnd, sizeof (long), 100);
			return 0;
	case WM_PAINT:
			hdc = BeginPaint (hwnd, &ps);
			pos = GetWindowLong (hwnd, 0);
			limit = GetWindowLong (hwnd, sizeof (long));
			if (pos > limit) pos = limit;
			DrawProgress (hwnd, hdc, pos, limit);
			EndPaint (hwnd, &ps);
			return 0;
	case MSG_PROGRESS_LIMIT:
			SetWindowLong (hwnd, 0, 0);
			SetWindowLong (hwnd, sizeof (long), lparam);
			UpdateWindow (hwnd);
			break;
	case MSG_PROGRESS:
			SetWindowLong (hwnd, 0, GetWindowLong (hwnd, 0) + lparam);
			InvalidateRect (hwnd, 0, 0);
			UpdateWindow (hwnd);
			break;
	case MSG_PROGRESS_CLOSE:
			lparam = GetWindowLong (hwnd, sizeof (long));
	case MSG_PROGRESS_POS:
			SetWindowLong (hwnd, 0, lparam);
			InvalidateRect (hwnd, 0, 0);
			UpdateWindow (hwnd);
	}
	return DefWindowProc (hwnd, msg, wparam, lparam);
}

/* --------------------- Status bar class --------------------------- */

#define SBAR_SIZE 200
#define SBAR_DX 2
#define SBAR_DY 2


void	paint_status_bar (HWND hwnd, char * s)
{
	RECT rc, irc, trc;
	PAINTSTRUCT ps;
	HDC dc;
	HFONT f;

	GetClientRect (hwnd, &rc);
	dc = BeginPaint (hwnd, &ps);


//	Draw_raised (dc, &rc);
	irc = rc;
	irc.left   += SBAR_DX;
	irc.right  -= SBAR_DX;
	irc.top    += SBAR_DY;
	irc.bottom -= SBAR_DY;

	if (irc.bottom - irc.top > 2 && irc.right - irc.left > 2)	{
		trc = irc;
		Draw_inset (dc, &trc);
		SetTextColor (dc, TextColor);
		SetBkColor (dc, GrayColor);
		f = SelectObject (dc, NonBoldFont);
		ExtTextOut (dc, SBAR_DX * 3/2, SBAR_DY * 3/2, ETO_CLIPPED | ETO_OPAQUE,
			    &trc, s, strlen (s), 0);
		SelectObject (dc, f);
		ExcludeClipRect (dc, irc.left, irc.top, irc.right, irc.bottom);
	}
	FillRect (dc, &rc, GrayBrush);
	EndPaint (hwnd, &ps);
}

LRESULT CALLBACK StatusBarProc (HWND hwnd, UINT msg,
				 WPARAM wparam, LPARAM lparam)
{
	char * s;
	int l;
	RECT rc;

	switch (msg) {
	case WM_CREATE:
		s = Malloc (SBAR_SIZE);
		if (!s) return -1;
		s[0] = 0;
		SetWindowLong (hwnd, 0, (DWORD) s);
		SetWindowPos (hwnd, 0, 0, 0, 2000,
			      NonBoldFontSize + 3 * SBAR_DY, SWP_NOZORDER | SWP_NOMOVE);
		return 0;
	case WM_DESTROY:
		s = (char*) GetWindowLong (hwnd, 0);
		free (s);
		return 0;
	case WM_SETTEXT:
		s = (char*) GetWindowLong (hwnd, 0);
		strncpy (s, (char*) lparam, SBAR_SIZE);
		s [SBAR_SIZE-1] = 0;
		GetClientRect (hwnd, &rc);
		rc.left   += SBAR_DX+2;
		rc.right  -= SBAR_DX+2;
		rc.top    += SBAR_DY+2;
		rc.bottom -= SBAR_DY+2;
		InvalidateRect (hwnd, &rc, 0);
		UpdateWindow (hwnd);
		return 0;
	case WM_GETTEXTLENGTH:
		s = (char*) GetWindowLong (hwnd, 0);
		return strlen (s);
	case WM_GETTEXT:
		s = (char*) GetWindowLong (hwnd, 0);
		l = strlen (s);
		if (l < (int) wparam)	{
			strcpy ((char*) lparam, s);
			return l+1;
		}
		strncpy ((char*) lparam, s, wparam);
		return wparam;
	case WM_PAINT:
		s = (char*) GetWindowLong (hwnd, 0);
		paint_status_bar (hwnd, s);
		return 0;
	case WM_SIZE:
		InvalidateRect (hwnd, 0, 0);
		return 0;
	}
	return DefWindowProc (hwnd, msg, wparam, lparam);
}

/* --------------------- Multi static class ------------------------- */

typedef struct	_StaticItem {
		char * s;
		RECT rc;
		HFONT font;
		BOOL copied;
		int type;
		struct _StaticItem * next;
	}
		StaticItem;

void	paint_static_item (HDC dc, StaticItem * item)
{
	RECT rc;
	int x;
	if (item -> type <100)	{
		SetTextColor (dc, TextColor);
		SetBkColor   (dc, GrayColor);

		SelectObject (dc, item->font);
		switch (item->type)	{
		case MSTATIC_RIGHT:	SetTextAlign (dc, TA_RIGHT);
					x = item -> rc.right;
					break;
		case MSTATIC_CENTER:	SetTextAlign (dc, TA_CENTER);
					x = (item->rc.left + item->rc.right)/2;
					break;
		default:		SetTextAlign (dc, TA_LEFT);
					x = item -> rc.left;
					break;
		}
		ExtTextOut (dc, x, item->rc.top, ETO_CLIPPED | ETO_OPAQUE,
			    &item->rc, item->s, strlen (item->s), 0);
	} else	{
		rc = item -> rc;
		(item -> type == MSTATIC_FRAME ? Draw_3dframe :
		 item -> type == MSTATIC_INSET ? Draw_inset :
						Draw_raised) (dc, &rc);
	}
}

void	paint_multi_static (HWND hwnd, StaticItem * item)
{
	PAINTSTRUCT ps;
	HDC dc;
	dc = BeginPaint (hwnd, &ps);

	while (item)	{
		paint_static_item (dc, item);
		item = item->next;
	}

	EndPaint (hwnd, &ps);
}

void	erase_multi_static (HWND hwnd, StaticItem * item)
{
	HDC dc;
	RECT rc;

	if (!item) return;
	dc = GetDC (hwnd);

	while (item)	{
		(item->type < 100 ? FillRect:FrameRect) (dc, &item->rc, GrayBrush);
		if (item->type == MSTATIC_FRAME)	{
			rc = item -> rc;
			rc.top ++; rc.bottom --;
			rc.left ++; rc.right --;
			FrameRect (dc, &rc, GrayBrush);
		}
		item = item -> next;
	}

	ReleaseDC (hwnd, dc);
}

void	delete_statics (HWND hwnd)
{
	StaticItem * item, * p;
	item = (StaticItem*) GetWindowLong (hwnd, 0);
	while (item)	{
		p = item -> next;
		if (item -> copied) free (item -> s);
		free (item);
		item = p;
	}
	SetWindowLong (hwnd, 0, 0);
}

LRESULT CALLBACK MultiStaticProc (HWND hwnd, UINT msg,
				 WPARAM wparam, LPARAM lparam)
{
	StaticItem * item;

	switch (msg) {
	case WM_CREATE:
		SetWindowLong (hwnd, 0, 0);
		return 0;
	case WM_DESTROY:
		delete_statics (hwnd);
		return 0;
	case WM_PAINT:
		item = (StaticItem*) GetWindowLong (hwnd, 0);
		paint_multi_static (hwnd, item);
		return 0;
	}
	return DefWindowProc (hwnd, msg, wparam, lparam);
}

BOOL	Add_static_item (HWND hwnd, RECT * rect, char * s, HFONT font, BOOL copy, int type)
{
	StaticItem * item;

	item = Malloc (sizeof (StaticItem));
	if (!item) return FALSE;
	if (copy)	{
		item -> s = Malloc (strlen (s)+1);
		if (!item -> s)	{
			free (item);
			return FALSE;
		}
		strcpy (item -> s, s);
	} else
		item -> s = s;
	item -> type = type;
	item -> copied = copy;
	item -> font = font ? font : GetStockObject (SYSTEM_FONT);
	item -> rc = *rect;
	item -> next = (StaticItem*) GetWindowLong (hwnd, 0);
	SetWindowLong (hwnd, 0, (DWORD) item);
	InvalidateRect (hwnd, 0, 0);
	return TRUE;
}

void	Delete_statics (HWND hwnd, BOOL erase)
{
	if (erase) erase_multi_static (hwnd, (StaticItem *) GetWindowLong (hwnd, 0));
	delete_statics (hwnd);
}

void	Clear_multi_static (HWND hwnd, RECT * rect)
{
	HDC dc = GetDC (hwnd);
	FillRect (dc, rect, GrayBrush);
	ReleaseDC (hwnd, dc);
}

WNDPROC OldStaticProc;

LRESULT CALLBACK StaticFrameProc (HWND hwnd, UINT msg,
				  WPARAM wparam, LPARAM lparam)
{
	DWORD style;
	RECT rc;
	PAINTSTRUCT ps;
	HDC dc;
	switch (msg) {
	case WM_PAINT:
		style = GetWindowLong (hwnd, GWL_STYLE) & SS_TYPEMASK;
		GetClientRect (hwnd, &rc);
		dc = BeginPaint (hwnd, &ps);

		((style == SS_BLACKFRAME) ? Draw_inset :
		 (style == SS_WHITEFRAME) ? Draw_raised :
					    Draw_3dframe) (dc, &rc);
		EndPaint (hwnd, &ps);

		return 0;
	}
	return CallWindowProc (OldStaticProc, hwnd, msg, wparam, lparam);
}

void	Subclass_static (HWND hwnd)
{
	OldStaticProc = Subclass_control (hwnd, (WNDPROC) StaticFrameProc);
}

/* --------------------- Color selection combo boxes ---------------- */

#define N_BASIC_COLORS 16

char * ColorDlgTitle;

UINT CALLBACK ColorTitleHook (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	IGNORE_PARAM (wparam);
	IGNORE_PARAM (lparam);
	IGNORE_PARAM (hwnd);

	if (msg == WM_INITDIALOG)
		SetWindowText (hwnd, ColorDlgTitle);
	return 0;
}

void	choose_color (HWND hwnd, COLORREF * color, char * title, BOOL solid)
{
	CHOOSECOLOR c;
	HDC dc;

	memset (&c, 0, sizeof (c));
	c.lStructSize = sizeof (c);
	c.hwndOwner = hwnd;
	c.hInstance = (HWND) MyInstance;
	c.rgbResult = *color;
	c.lpCustColors = custom_colors;
	c.lpfnHook = ColorTitleHook;
	ColorDlgTitle = title;
	c.Flags = CC_RGBINIT|CC_SHOWHELP|CC_ENABLEHOOK;

	lock_help ++;
	if (ChooseColor (&c))	{
		*color = c.rgbResult;
		if (solid)	{
			dc = GetDC (0);
			*color = GetNearestColor (dc, *color);
			ReleaseDC (0, dc);
		}
	}
	lock_help --;
}

COLORREF basic_colors [N_BASIC_COLORS] = {
	RGB (0, 0, 0),
	RGB (128, 0, 0),
	RGB (0, 128, 0),
	RGB (128, 128, 0),
	RGB (0, 0, 128),
	RGB (128, 0, 128),
	RGB (0, 128, 128),
	RGB (192, 192, 192),
	RGB (128, 128, 128),
	RGB (255, 0, 0),
	RGB (0, 255, 0),
	RGB (255, 255, 0),
	RGB (0, 0, 255),
	RGB (255, 0, 255),
	RGB (0, 255, 255),
	RGB (255, 255, 255)
};

char * color_names [N_BASIC_COLORS] = {
	"Black", "Dark Red", "Dark Green", "Brown", "Dark Blue", "Dark Magenta", "Dark Cyan", "Gray",
	"Dark Gray", "Red", "Green", "Yellow", "Blue", "Magenta", "Cyan", "White"
	};

void	Colorbox_fill (HWND hwnd, int id, COLORREF color)
{
	int i, j;
	SendDlgItemMessage (hwnd, id, CB_ADDSTRING, 0, DEFAULT_COLOR);
	j = N_BASIC_COLORS+1;
	for (i = 0; i < N_BASIC_COLORS; i++)	{
		SendDlgItemMessage (hwnd, id, CB_ADDSTRING, 0, (LPARAM) basic_colors [i]);
		if (basic_colors [i] == color) j = i+1;
	}
	if (color == DEFAULT_COLOR)	{
		j = 0;
		color = (COLORREF) SendMessage (hwnd, MSG_GET_DEFAULT_COLOR, id, 0);
	}
	SendDlgItemMessage (hwnd, id, CB_ADDSTRING, 0, color | 0x80000000);
	SendDlgItemMessage (hwnd, id, CB_SETCURSEL, j, 0);
}

void	Colorbox_update (HWND hwnd, int id, COLORREF color)
{
	int i, j;
	j = N_BASIC_COLORS+1;
	if (color == DEFAULT_COLOR)	{
		j = 0;
		color = (COLORREF) SendMessage (hwnd, MSG_GET_DEFAULT_COLOR, id, 0);
	} else
		for (i = 0; i < N_BASIC_COLORS; i++)
			if (basic_colors [i] == color) j = i+1;
	
	SendDlgItemMessage (hwnd, id, CB_SETITEMDATA, N_BASIC_COLORS+1, color | 0x80000000);
	SendDlgItemMessage (hwnd, id, CB_SETCURSEL, j, 0);
}

BOOL	Colorbox_measure (HWND hwnd, LPARAM lparam)
{
	MEASUREITEMSTRUCT * m = (MEASUREITEMSTRUCT *) lparam;
	HFONT f = (HFONT) SendDlgItemMessage (hwnd, m->CtlID, WM_GETFONT, 0, 0);
	m->itemHeight = Calc_font_height (f)+4;
	return TRUE;
}

BOOL	Colorbox_draw_item (LPARAM lparam)
{
	int i;
	RECT rc;
	COLORREF c;
	int br;
	char *s;
	DRAWITEMSTRUCT * d = (DRAWITEMSTRUCT *) lparam;

	i = d->itemID;
	rc = d->rcItem;
	draw_frame (d->hDC, &rc, GetStockObject (BLACK_PEN));
	rc.top ++; rc.bottom --; rc.left ++; rc.right--;
	if (d -> itemState & ODS_FOCUS)
		Draw_inset (d->hDC, &rc);
	else
		Draw_raised (d->hDC, &rc);

	c = d->itemData;
	if (c == DEFAULT_COLOR)	{
		c = (COLORREF) SendMessage (GetParent (d->hwndItem), MSG_GET_DEFAULT_COLOR, d->CtlID, 0);
		s = "Auto";
	} else if (c & 0xFF000000)	{
		c &= 0xFFFFFF;
		s = "Custom";
	} else	{
		s = "";
		for (i = 0; i < N_BASIC_COLORS; i++)
			if (basic_colors [i] == c)	{
				s = color_names [i];
				break;
			}
	}

	SetBkColor   (d -> hDC, c);
	br = GetRValue (c) + GetGValue (c) + GetBValue (c);
	SetTextColor (d -> hDC, br >= 384 ? 0 : 0xFFFFFF);
	ExtTextOut (d->hDC, rc.left+1, rc.top+1, ETO_OPAQUE, &rc, s, strlen (s), 0);
	return 1;
}

BOOL	Colorbox_notify (HWND hwnd, WPARAM wparam, LPARAM lparam, COLORREF * color)
{
	int i;
	int id = CONTROL_ID;
	COLORREF c;

	if (NOTIFY_CODE != CBN_CLOSEUP)
		return 0;
	i = (int) SendDlgItemMessage (hwnd, id, CB_GETCURSEL, 0, 0);
	c = (COLORREF) SendDlgItemMessage (hwnd, id, CB_GETITEMDATA, i, 0);
	if ((c & 0xFF000000) == 0x80000000)	{
		c &= 0xFFFFFF;
		choose_color (hwnd, &c, "Choose custom color", TRUE);
		*color = c;
		c |= 0x80000000;
		SendDlgItemMessage (hwnd, id, CB_SETITEMDATA, i, c);
	} else
		*color = c;
	return 1;
}

COLORREF Colorbox_color (HWND hwnd, int id)
{
	int i = (int) SendDlgItemMessage (hwnd, id, CB_GETCURSEL, 0, 0);
	COLORREF color = (COLORREF) SendDlgItemMessage (hwnd, id, CB_GETITEMDATA, i, 0);
	if (color == DEFAULT_COLOR)
		color = (COLORREF) SendMessage (hwnd, MSG_GET_DEFAULT_COLOR, id, 0);
	return color & 0xFFFFFF;
}


/* --------------------- Register all classes ----------------------- */

BOOL	Register_classes (void)
{
	int i;
	WNDCLASS wc;

	calc_system_font_height ();

	wc.style = 0;
	wc.lpfnWndProc = (LPVOID) ProgressProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 2 * sizeof (long);
	wc.hInstance = MyInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "ProgressIndicator";
	if (!RegisterClass (&wc)) return FALSE;

	wc.style = 0;
	wc.lpfnWndProc = (LPVOID) QStaticProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = Q_SIZE;
	wc.hInstance = MyInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "QStatic";
	if (!RegisterClass (&wc)) return FALSE;

	wc.style = 0;
	wc.lpfnWndProc = (LPVOID) ToolBarProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof (ToolBarData*);
	wc.hInstance = MyInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "shelltoolbar";
	if (!RegisterClass (&wc)) return FALSE;

	wc.style = 0;
	wc.lpfnWndProc = (LPVOID) StatusBarProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof (char*);
	wc.hInstance = MyInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "shellstatusbar";
	if (!RegisterClass (&wc)) return FALSE;

	wc.style = 0;
	wc.lpfnWndProc = (LPVOID) MultiStaticProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof (char*);
	wc.hInstance = MyInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "multistatic";
	if (!RegisterClass (&wc)) return FALSE;
	for (i = 0; i < 16; i++)
		custom_colors [i] = RGB (i*16, i*16, i*16);

	return TRUE;
}

void	Create_pens (void)
{
	int h;
	HDC dc;
	if (LightPen) DeleteObject (LightPen);
	if (DarkPen) DeleteObject (DarkPen);
	if (GrayPen) DeleteObject (GrayPen);
	if (TextPen) DeleteObject (TextPen);
	if (GrayBrush) DeleteObject (GrayBrush);
	if (NonBoldFont) DeleteObject (NonBoldFont);
	GrayColor  = GetSysColor (COLOR_3DFACE);
	DarkColor  = GetSysColor (COLOR_3DSHADOW);
	LightColor = GetSysColor (COLOR_3DHIGHLIGHT);
	TextColor  = GetSysColor (COLOR_BTNTEXT);
	LightPen   = CreatePen (PS_SOLID, 1, LightColor);
	DarkPen    = CreatePen (PS_SOLID, 1, DarkColor);
	GrayPen    = CreatePen (PS_SOLID, 1, GrayColor);
	TextPen    = CreatePen (PS_SOLID, 1, TextColor);
	GrayBrush  = CreateSolidBrush (GrayColor);
	dc = GetDC (0);
	h = -MulDiv (8, GetDeviceCaps(dc, LOGPIXELSY), 72);
	ReleaseDC (0, dc);
	NonBoldFont = CreateFont (h,0,0,0,0,0,0,0,0,0,0,PROOF_QUALITY,0,
			 "MS Sans Serif");
	NonBoldFontSize = Calc_font_height (NonBoldFont);
}

