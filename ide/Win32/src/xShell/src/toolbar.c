#include "util.h"
#include "shell.h"
#include "toolbar.h"
#include "config.h"

#define STANDARD_TOOLBAR 1

/* must define myself: VC 4.1 doesn't define this constant at all;
   VC 4.2 defines incorrectly as 0x1000
*/

#define _TBSTYLE_FLAT 0x800

#if STANDARD_TOOLBAR

WNDPROC StdToolBarProc;

int	items [100];
char *	texts [100];
int	nitems;
int	toolbar_timerid = 0;
#define TIMERID 100

LRESULT	CALLBACK SubToolBarProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HWND tip;
	TOOLINFO ti;
	LRESULT r;
	POINT p;

	int id;

	r = CallWindowProc (StdToolBarProc, hwnd, msg, wparam, lparam);
	if (msg == WM_MOUSEMOVE)	{
		if (toolbar_timerid)
			return r;
		toolbar_timerid = SetTimer (hwnd, TIMERID, 10, 0);
	} else if (msg == WM_TIMER)	{
		if (wparam != TIMERID)	return r;
	} else
		return r;
	id = -1;
	GetCursorPos (&p);
	if (WindowFromPoint (p) != hwnd)	{
		KillTimer (hwnd, TIMERID);
		toolbar_timerid = 0;
	} else	{
		tip = (HWND) SendMessage (hwnd, TB_GETTOOLTIPS, 0, 0);
		ti.cbSize = sizeof (TOOLINFO);
		if (SendMessage (tip, TTM_GETCURRENTTOOL, 0, (LPARAM) &ti))
			if (! (ti.uFlags & TTF_IDISHWND))
				id = (int) ti.uId;
	}
	if (id >= 0)
		SendMessage (FrameWindow, WM_MENUSELECT, id, 0);
	else
		SendMessage (FrameWindow, WM_MENUSELECT, 0xFFFF0000, 0);

	return r;
}

HWND	Make_empty_toolbar (HWND parent)
{
	HWND bar;

	InitCommonControls ();
	bar = CreateWindowEx (WS_EX_STATICEDGE,
			     TOOLBARCLASSNAME, "",
			     WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS|CCS_TOP |
			     (FlatToolBar? _TBSTYLE_FLAT : 0),
			     0, 0, 1500, 10, parent, 0,
			     MyInstance, NULL);

	if (!bar) return 0;
	SendMessage (bar, TB_BUTTONSTRUCTSIZE, sizeof (TBBUTTON), 0);
	StdToolBarProc = Subclass_control (bar, SubToolBarProc);
	nitems = 0;
	return bar;
}

int	find_toolbar_button (HWND bar, int id)
{
	return (int) SendMessage (bar, TB_COMMANDTOINDEX, id, 0);
}

void	Add_toolbar_button (HWND bar, HBITMAP bmp, HBITMAP dbmp, char * string, int cmd, int pause, int after)
{
	TBBUTTON b;
	TBADDBITMAP ab;
	BITMAP bm;
	int i;
	int nbefore;

	ab.hInst = 0;
	ab.nID = (UINT) bmp;

	nbefore = -1;
	if (after)
		nbefore = find_toolbar_button (bar, after);
	if (nbefore != -1)
		nbefore ++;

	if (SendMessage (bar, TB_BUTTONCOUNT, 0, 0) == 0)	{
		GetObject (bmp, sizeof (bm), &bm);
		SendMessage (bar, TB_SETBITMAPSIZE, 0, MAKELONG (bm.bmWidth, bm.bmHeight));
		SendMessage (bar, TB_AUTOSIZE, 0, 0);
	}
	
	i = (int) SendMessage (bar, TB_ADDBITMAP, 1, (LPARAM) &ab);
	if (i < 0) return;

	if (pause)	{
		b.iBitmap = -1;
		b.idCommand = 0;
		b.fsState = 0;
		b.fsStyle = TBSTYLE_SEP;
		b.dwData  = 0;
		b.iString = -1;
		if (nbefore != -1)	{
			SendMessage (bar, TB_INSERTBUTTON, nbefore, (LPARAM) &b);
			nbefore ++;
		} else
			SendMessage (bar, TB_ADDBUTTONS, 1, (LPARAM) &b);

	}
	b.iBitmap = i;
	b.idCommand = cmd;
	b.fsState = 0; /* disabled ; TBSTATE_ENABLED; */
	b.fsStyle = TBSTYLE_BUTTON;
	b.dwData  = 0;
	b.iString = -1;
	if (nbefore != -1)
		SendMessage (bar, TB_INSERTBUTTON, nbefore, (LPARAM) &b);
	else
		SendMessage (bar, TB_ADDBUTTONS, 1, (LPARAM) &b);
	for (i = 0; i < nitems; i++)
		if (items [i] == cmd) break;
	if (i == nitems)	{
		items [i] = cmd;
		nitems ++;
	} else
		free (texts [i]);
	texts [i] = string ? alloc_copy (string) : 0;
}

void	Add_toolbar (HWND bar, int Resource, int cmd, int pause, int after)
{
	HBITMAP bmp;
	char strbuf [1000];
	char * string;

	//bmp = LoadBitmap (MyInstance, MAKEINTRESOURCE (Resource));
	bmp = CreateMappedBitmap (MyInstance, Resource, 0, 0, 0);
	if (!bmp) return;
	string = 0;
	if (LoadString (MyInstance, cmd, strbuf, sizeof (strbuf)))
		string = strbuf;

	Add_toolbar_button (bar, bmp, bmp, string, cmd, pause, after);
}

void	Enable_toolbar_button (HWND bar, int cmd, BOOL enable)
{
	SendMessage (bar, TB_ENABLEBUTTON, cmd, MAKELONG (enable, 0));
}

void	show_toolbar_button (HWND bar, int cmd, BOOL show)
{
	SendMessage (bar, TB_HIDEBUTTON, cmd, MAKELONG (!show, 0));
}

void	Bind_status_bar (HWND toolbar, HWND statusbar)
{
}

void	Redither_toolbar (HWND bar)
{
}

void	Delete_toolbar_buttons (HWND bar, int from, int to)
{
	int i, n;
	TBBUTTON b;
	n = SendMessage (bar, TB_BUTTONCOUNT, 0, 0);
	for (i = n-1; i >= 0; i--)	{
		if (!SendMessage (bar, TB_GETBUTTON, i, (LPARAM) &b)) continue;
		if (b.idCommand >= from && b.idCommand <= to)
			SendMessage (bar, TB_DELETEBUTTON, i, 0);
	}
}

void	Set_flat_toolbar (HWND toolbar, BOOL flat)
{
	DWORD style = GetWindowLong (toolbar, GWL_STYLE);
	if (((style & _TBSTYLE_FLAT) != 0) == flat)
		return;
	style ^= _TBSTYLE_FLAT;
	SetWindowLong (toolbar, GWL_STYLE, style);
	ShowWindow (toolbar, SW_HIDE);
	ShowWindow (toolbar, SW_SHOWNORMAL);
}

#endif

void	Toolbar_notify (HWND bar, NMHDR * m)
{
	HMENU menu;
	TOOLTIPTEXT * t;
	int i;

	if (!bar) return;
	if (m->hwndFrom != (HWND) SendMessage (bar, TB_GETTOOLTIPS, 0, 0)) return;
	if (m->code != TTN_NEEDTEXT) return;
	t = (TOOLTIPTEXT*) m;

	menu = GetMenu (FrameWindow);
	t->szText[0] = 0;
	GetMenuString (menu, t->hdr.idFrom, t->szText, sizeof (t->szText), MF_BYCOMMAND);
	for (i = 0; t->szText [i]; i++)
		if (t->szText [i] == 8 || t->szText [i] == '\t')
			break;
	t->szText [i] = 0;
	t->hinst=0;
}
