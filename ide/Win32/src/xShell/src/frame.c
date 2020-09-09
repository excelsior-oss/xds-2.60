#include "util.h"
#include "shell.h"
#include "res.h"
#include "mdi.h"
#include "frame.h"
#include "edit.h"
#include "config.h"
#include "logo.h"

BOOL	read_bool_option (char * section, char * name, BOOL def);

HMENU	WindowsMenu;

BOOL	LongMenu = TRUE;
BOOL	MultiColumnMenu = FALSE;

DWORD	numbering_style;

DWORD	winbar_style = MDITAB_TEXT | MDITAB_NUMBER | MDITAB_ICON;
BOOL	winbar_present = TRUE;
BOOL	dock_small = TRUE;

#define WINDOWLIST "&Window list"
#define MOREWINDOWS "More &Windows"

/* --------------------------------------------------- */

BOOL	DisableEnum (int num, HWND hwnd, BOOL free, BOOL docked, BOOL active, BOOL mdiactive)
{
	EnableWindow (hwnd, FALSE);
	return TRUE;
}

BOOL	EnableEnum (int num, HWND hwnd, BOOL free, BOOL docked, BOOL active, BOOL mdiactive)
{
	EnableWindow (hwnd, TRUE);
	return TRUE;
}

int	dialog_cnt = 0;

int	DialogParent (HWND hwnd, int dlg, DLGPROC proc)
{
	int r;
	HWND wnd;
	HWND fwnd;
	fwnd = GetFocus ();
	SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) DisableEnum);
	EnableWindow (FrameWindow, FALSE);
	++ dialog_cnt;
	r = DialogBox (MyInstance, MAKEINTRESOURCE (dlg),
		   hwnd, proc);
	-- dialog_cnt;
	wnd = (HWND) SendMessage (FrameWindow, WM_MDIGETACTIVE, 0, 0);
	SendMessage (FrameWindow, WM_MDIACTIVATE, (WPARAM) wnd, 0);
	if (dialog_cnt == 0)	{
		SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) EnableEnum);
		EnableWindow (FrameWindow, TRUE);
	} else
		SetActiveWindow (hwnd);
	SetFocus (fwnd);
	return r;
}

int	Dialog (int dlg, DLGPROC proc)
{
	HWND wnd;
	
	if (dialog_cnt)
		wnd = GetActiveWindow ();
	else	{
		wnd = (HWND) SendMessage (FrameWindow, WM_MDIGETACTIVE, 0, 0);
		if (!wnd) wnd = FrameWindow;
	}

	return DialogParent (wnd, dlg, proc);
}

void	PropertyDialog (PROPSHEETHEADER * psh)
{
	HWND wnd;
	SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) DisableEnum);
	EnableWindow (FrameWindow, FALSE);
	++ dialog_cnt;
	PropertySheet (psh);
	-- dialog_cnt;
	if (dialog_cnt == 0)	{
		SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) EnableEnum);
		EnableWindow (FrameWindow, TRUE);
	}
	wnd = (HWND) SendMessage (FrameWindow, WM_MDIGETACTIVE, 0, 0);
	SendMessage (FrameWindow, WM_MDIACTIVATE, (WPARAM) wnd, 0);
}

int	MsgBox (char * header, UINT style, char * fmt, ...)
{
	HWND wnd;
	int r;
	char buf [1024];

	va_list v;
	va_start (v, fmt);
	vsprintf (buf, fmt, v);
	Hide_logo_now ();
	wnd = GetActiveWindow ();
	SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) DisableEnum);
	EnableWindow (FrameWindow, FALSE);
	++ dialog_cnt;
	r = MessageBox (wnd, buf, header, style | MB_TASKMODAL);
	-- dialog_cnt;
	if (dialog_cnt == 0)	{
		SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) EnableEnum);
		EnableWindow (FrameWindow, TRUE);
	}
	wnd = (HWND) SendMessage (FrameWindow, WM_MDIGETACTIVE, 0, 0);
	SendMessage (FrameWindow, WM_MDIACTIVATE, (WPARAM) wnd, 0);

	return r;
}

/* --------------------------------------------------- */


char *	register_strings [10];
REGISTERPROC register_procs [10];
int	register_cnt = 0;

#define MAX_NWINS 100
char *	wins [MAX_NWINS][2];
int	wins_count;

void	Register_child_class (char * class, REGISTERPROC pr)
{
	register_strings [register_cnt] = class;
	register_procs   [register_cnt] = pr;
	++ register_cnt;
}

BOOL	SaveEnum (int num, HWND hwnd, BOOL free, BOOL docked, BOOL active, BOOL mdiactive)
{
	char title [2000];
	char number [20];
	char line [3000];
	char class [20];
	MDIPLACEMENT pl;

	class [0] = title [0] = 0;
	SendMessage (hwnd, MSG_GETCLASS, sizeof (class), (LPARAM) class);
	if (!class [0]) return TRUE;
	SendMessage (hwnd, MSG_GETTITLE, sizeof (title), (LPARAM) title);
	SendMessage (hwnd, WM_MYMDI_GETPLACEMENT, 0, (LPARAM) &pl);
	sprintf (number, "%d", num);
	sprintf (line, "%s,\"%s\",%s", class, title, docked ? "docked" : free ? "free" : "frame");

	if (docked)
		sprintf (line+strlen(line), ",%s,%d",
			pl.dir == DOCK_UP ? "up" :
			pl.dir == DOCK_DOWN ? "down" :
			pl.dir == DOCK_RIGHT ? "right" : "left",
			pl.docksize);
	else
		sprintf (line+strlen(line), ",%s,%d,%d,%d,%d,%d,%d",
			IsIconic (hwnd) ? "min" : IsZoomed (hwnd) ? "max" : "normal",
			pl.p.rcNormalPosition.left,  pl.p.rcNormalPosition.top,
			pl.p.rcNormalPosition.right, pl.p.rcNormalPosition.bottom,
			pl.p.ptMinPosition.x, pl.p.ptMinPosition.y);

	SendMessage (hwnd, MSG_GETSAVEDATA, 0, (LPARAM) (line+strlen(line)));
	wins [wins_count][1] = alloc_copy (line);
	if (wins [wins_count][1])
		wins [wins_count][0] = alloc_copy (number);
	++ wins_count;
	return TRUE;
}

void	make_win_section (void)
{
	WritePrivateProfileString ("windows", 0, 0, ".\\test.ini");
}

void	free_section (void)
{
	int i;
	for (i = 0; i < MAX_NWINS && wins [i][0]; i++)	{
		free (wins [i][0]);
		free (wins [i][1]);
	}
}

void	Write_windows (char * inifile)
{
	memset (wins, 0, sizeof (wins));
	wins_count = 0;
	SendMessage (FrameWindow, WM_MYMDI_ENUMZORDER, 0, (LPARAM) SaveEnum);
	Write_section (wins, MAX_NWINS, "windows", inifile);
	free_section ();
}

/* ---------------------------------------------------------- */

char * get_word (char **s)
{
	char *p;
	while (**s && isspace (**s)) ++*s;
	p = *s;
	if (!*p) return p;
	if (**s == '"')	{
		p = ++*s;
		while (**s && **s != '"') ++ *s;
		if (!**s) return p;
		**s = 0;
		++ *s;
		while (**s && **s != ',') ++ *s;
		if (**s == ',') ++ *s;
	} else	{
		while (**s && **s != ',') ++ *s;
		if (!**s) return p;
		**s = 0;
		++ *s;
	}
	return p;
}

void	read_one_window (char * winnum, char * s)
{
	MDIPLACEMENT pl;
	char * class, * title, * type, * state, * dir;
	REGISTERPROC pr;
	int i;

	memset (&pl, 0, sizeof (pl));
	pl.winnum = atoi (winnum);
	class = get_word (&s);
	for (i = 0; i < register_cnt; i++)
		if (! stricmp (register_strings [i], class))
			break;
	if (i >= register_cnt) return;
	pr = register_procs [i];
	title = get_word (&s);
	type  = get_word (&s);
	if (!stricmp (type, "docked"))
		pl.type = DOCKED;
	else if (!stricmp (type, "free"))
		pl.type = FREE;
	else
		pl.type = FRAME;
	if (pl.type == DOCKED)	{
		dir = get_word (&s);
		if (! stricmp (dir, "right"))     pl.dir = DOCK_RIGHT;
		else if (! stricmp (dir, "left")) pl.dir = DOCK_LEFT;
		else if (! stricmp (dir, "up"))   pl.dir = DOCK_UP;
		else pl.dir = DOCK_DOWN;
		pl.docksize = atoi (get_word (&s));
	} else	{
		state = get_word (&s);
		if (!stricmp (state, "min")) pl.p.showCmd = SW_SHOWMINIMIZED;
		else if (!stricmp (state, "max")) pl.p.showCmd = SW_SHOWMAXIMIZED;
		else pl.p.showCmd = SW_SHOWNORMAL;
		pl.p.rcNormalPosition.left  = atoi (get_word (&s));
		pl.p.rcNormalPosition.top   = atoi (get_word (&s));
		pl.p.rcNormalPosition.right = atoi (get_word (&s));
		pl.p.rcNormalPosition.bottom= atoi (get_word (&s));
		pl.p.ptMinPosition.x	    = atoi (get_word (&s));
		pl.p.ptMinPosition.y	    = atoi (get_word (&s));
	}
	pr (title, &pl, s);
}

void	Read_windows (char * inifile)
{
	int n;
	HWND hwnd;

	++ EditStatusBarLock;
	Set_status_bar (FrameWindow, "Reading desktop configuration");
	memset (wins, 0, sizeof (wins));
	Read_section (wins, MAX_NWINS, "windows", inifile);
	for (n = 0; n < MAX_NWINS && wins [n][0]; n++);
	for (--n; n >= 0; --n)
		read_one_window (wins [n][0], wins [n][1]);
	SendMessage (FrameWindow, WM_MYMDI_COMPACTNUMBERS, 0, 0);
	free_section ();
	Clear_status_bar (FrameWindow);
	-- EditStatusBarLock;
	hwnd = (HWND) SendMessage (FrameWindow, WM_MDIGETACTIVE, 0, 0);
	if (hwnd)
		SetFocus (hwnd);
}

/* --------------------------------------------------- */

HWND	morewindows_list;
int	morewindows_extent;
HFONT	morewindows_font;

BOOL	MoreWindowsEnum (int num, HWND hwnd, BOOL free, BOOL docked, BOOL active, BOOL mdiactive)
{
	char s [1000];
	int i, x;

	sprintf (s, "%d : ", num);
	if (hwnd)	{
		SendMessage (hwnd, WM_MYMDI_GETLONGTEXT,
				(WPARAM) (sizeof (s)-strlen (s)),
				(LPARAM) (s+strlen (s)));
		s [sizeof (s)-1] = 0;
		if (free)
			strcat (s, " (free)");
		else if (docked)
			strcat (s, " (docked)");
	}
	i = SendMessage (morewindows_list, LB_ADDSTRING, 0, (LPARAM) s);
	SendMessage (morewindows_list, LB_SETITEMDATA, i, (LPARAM) num);
	if (active)
		SendMessage (morewindows_list, LB_SETCURSEL, i, 0);

	x = text_width (morewindows_font, s);
	if (x > morewindows_extent)
		morewindows_extent = x;
	
	return TRUE;
}

void	fill_windows_list (HWND hwnd)
{
	int cursel = (int) SendMessage (morewindows_list, LB_GETCURSEL, 0, 0);
	int n;

	morewindows_extent = 0;
	morewindows_font = (HFONT) SendMessage (morewindows_list, WM_GETFONT, 0, 0);
	SendMessage (morewindows_list, WM_SETREDRAW, 0, 0);
	SendMessage (morewindows_list, LB_RESETCONTENT, 0, 0);
	SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) MoreWindowsEnum);
	SendMessage (morewindows_list, LB_SETHORIZONTALEXTENT, morewindows_extent+10, 0);
	SendMessage (morewindows_list, WM_SETREDRAW, 1, 0);
	n = (int) SendMessage (morewindows_list, LB_GETCOUNT, 0, 0);
	if (cursel >= 0)	{
		if (cursel >= n) cursel = n-1;
		SendMessage (morewindows_list, LB_SETCURSEL, cursel, 0);
	}
	InvalidateRect (morewindows_list, 0, 0);
	SetActiveWindow (hwnd);
}

void	enable_morewindows_buttons (HWND hwnd)
{
	MDIPLACEMENT p;
	HWND child;
	int i;
	char cl [30];

	i = SendDlgItemMessage (hwnd, MOREWINDOWS_LIST, LB_GETCURSEL, 0, 0);
	if (i < 0)
		child = NULL;
	else	{
		i = (int) SendDlgItemMessage (hwnd, MOREWINDOWS_LIST, LB_GETITEMDATA, i, 0);
		child = (HWND) SendMessage (FrameWindow, WM_MYMDI_WINDOWBYNUM, (WPARAM) i, 0);
	}
	EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_CLOSE), child != 0);
	if (child)	{
		memset (&p, 0, sizeof (p));
		SendMessage (child, WM_MYMDI_GETPLACEMENT, 0, (LPARAM) &p);
		if (p.type == FRAME)	{
			EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_DOCK), TRUE);
			EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_FREE), TRUE);
			SetDlgItemText (hwnd, MOREWINDOWS_DOCK, "Dock");
			SetDlgItemText (hwnd, MOREWINDOWS_FREE, "Free");
		} else if (p.type == DOCKED)	{
			EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_DOCK), TRUE);
			EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_FREE), FALSE);
			SetDlgItemText (hwnd, MOREWINDOWS_DOCK, "UnDock");
			SetDlgItemText (hwnd, MOREWINDOWS_FREE, "Free");
		} else	{
			EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_DOCK), FALSE);
			EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_FREE), TRUE);
			SetDlgItemText (hwnd, MOREWINDOWS_DOCK, "Dock");
			SetDlgItemText (hwnd, MOREWINDOWS_FREE, "Return to frame");
		}
		cl [0] = 0;
		SendMessage (child, MSG_GETCLASS, sizeof (cl), (LPARAM) cl);
		EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_SAVE), !stricmp (cl, "edit"));
	} else	{
		EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_DOCK), FALSE);
		EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_FREE), FALSE);
		EnableWindow (GetDlgItem (hwnd, MOREWINDOWS_SAVE), FALSE);
		SetDlgItemText (hwnd, MOREWINDOWS_DOCK, "Dock");
		SetDlgItemText (hwnd, MOREWINDOWS_FREE, "Free");
	}
}

int	num_to_activate;

BOOL CALLBACK MoreWindowsDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HWND	child;
	int	i;

	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		morewindows_list = GetDlgItem (hwnd, MOREWINDOWS_LIST);
		fill_windows_list (hwnd);
		enable_morewindows_buttons (hwnd);
		return 1;
	case WM_COMMAND:
		i = SendDlgItemMessage (hwnd, MOREWINDOWS_LIST, LB_GETCURSEL, 0, 0);
		if (i < 0)
			i = 0;
		else
			i = (int) SendDlgItemMessage (hwnd, MOREWINDOWS_LIST, LB_GETITEMDATA, i, 0);
		child = (HWND) SendMessage (FrameWindow, WM_MYMDI_WINDOWBYNUM, (WPARAM) i, 0);
		switch (CONTROL_ID)	{
		case MOREWINDOWS_LIST:
			switch (NOTIFY_CODE)	{
			case LBN_SELCHANGE:
				enable_morewindows_buttons (hwnd);
				return 1;
			case LBN_DBLCLK:
				SendMessage (hwnd, WM_COMMAND, IDOK, 0);
				return 1;
			}
			break;
		case IDOK:
			num_to_activate = i;
			EndDialog (hwnd, 1);
			return 1;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			return 1;
		case MOREWINDOWS_DOCK:
			if (!child) return 1;
			SendMessage (FrameWindow, WM_MYMDI_DOCK, (WPARAM) child, 0);
			fill_windows_list (hwnd);
			enable_morewindows_buttons (hwnd);
			return 1;
		case MOREWINDOWS_FREE:
			if (!child) return 1;
			SendMessage (FrameWindow, WM_MYMDI_FREE, (WPARAM) child, 0);
			fill_windows_list (hwnd);
			enable_morewindows_buttons (hwnd);
			return 1;
		case MOREWINDOWS_SAVE:
			if (!child) return 1;
			SendMessage (child, WM_COMMAND, IDM_SAVE, 0);
			return 1;
		case MOREWINDOWS_CLOSE:
			if (!child) return 1;
			SendMessage (child, WM_CLOSE, 0, 0);
			fill_windows_list (hwnd);
			enable_morewindows_buttons (hwnd);
			SetFocus (GetDlgItem (hwnd, MOREWINDOWS_LIST));
			return 1;
		case IDHELP:
			return Dialog_help (MOREWINDOWS_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (MOREWINDOWS_DIALOG, lparam);
	}
	return 0;
}

void	MoreWindows (void)
{
	HWND child;
	if (Dialog (MOREWINDOWS_DIALOG, MoreWindowsDialogProc))	{
		if (! num_to_activate) return;
		child = (HWND) SendMessage (FrameWindow, WM_MYMDI_WINDOWBYNUM, (int) num_to_activate, 0);
		if (child && IsIconic (child))
			ShowWindow (child, SW_RESTORE);
		SendMessage (FrameWindow, WM_COMMAND, IDM_FIRSTCHILD+num_to_activate-1, 0);
	}
}

int	screen_capacity, lines_count;

BOOL	MenuEnum (int num, HWND hwnd, BOOL free, BOOL docked, BOOL active, BOOL mdiactive)
{
	char s [1000];
	DWORD style;

	sprintf (s, "&%d : ", num);
	if (hwnd)	{
		SendMessage (hwnd, WM_MYMDI_GETSHORTTEXT,
				(WPARAM) (sizeof (s)-strlen (s)),
				(LPARAM) (s+strlen (s)));

		s [sizeof (s)-1] = 0;
		if (free)
			strcat (s, " (free)");
		else if (docked)
			strcat (s, " (docked)");
	}
	style = MF_STRING;
	++ lines_count;
	if (MultiColumnMenu && lines_count % screen_capacity == 0)
		style |= MF_MENUBARBREAK;
	AppendMenu (WindowsMenu, style, IDM_FIRSTCHILD+num-1, s);

	if (active)
		CheckMenuItem (WindowsMenu, IDM_FIRSTCHILD+num-1, MF_CHECKED | MF_BYCOMMAND);
	if (! MultiColumnMenu && lines_count >= screen_capacity) return FALSE;
	return TRUE;
}

#define IDM_WINMENUSEPARATOR 99

void	Make_windows_menu (void)
{
	int i;
	int n, sy, dy;
	HWND child;
	MDIPLACEMENT p;

	DeleteMenu (WindowsMenu, IDM_MOREWINDOWS, MF_BYCOMMAND);
	n = GetMenuItemCount (WindowsMenu);
	for (i = n-1; i >= 0 && GetMenuItemID (WindowsMenu, i) >= IDM_FIRSTCHILD; i--)
		DeleteMenu (WindowsMenu, i, MF_BYPOSITION);

	DeleteMenu (WindowsMenu, IDM_DOCK, MF_BYCOMMAND);	
	DeleteMenu (WindowsMenu, IDM_UNDOCK, MF_BYCOMMAND);	
	DeleteMenu (WindowsMenu, IDM_FREE, MF_BYCOMMAND);	
	child = (HWND) SendMessage (FrameWindow, WM_MDIGETACTIVE, 0, 0);
	if (child)	{
		SendMessage (child, WM_MYMDI_GETPLACEMENT, 0, (LPARAM) &p);
		if (p.type == FRAME)	{
			AppendMenu (WindowsMenu, MF_STRING, IDM_DOCK, "&Dock");
			AppendMenu (WindowsMenu, MF_STRING, IDM_FREE, "&Free");
		} else if (p.type == DOCKED)
			AppendMenu (WindowsMenu, MF_STRING, IDM_UNDOCK, "&Undock");
	}
	DeleteMenu (WindowsMenu, IDM_WINMENUSEPARATOR, MF_BYCOMMAND);
	
	n = SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, 0);
	if (!n)
		return;
	if (! LongMenu)	{
		if (n < 10)
			AppendMenu (WindowsMenu, MF_STRING, IDM_MOREWINDOWS, WINDOWLIST);
		AppendMenu (WindowsMenu, MF_SEPARATOR, IDM_WINMENUSEPARATOR, 0);
		lines_count = 0;
		screen_capacity = 9;
		SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) MenuEnum);
		if (n >= 10)
			AppendMenu (WindowsMenu, MF_STRING, IDM_MOREWINDOWS, MOREWINDOWS);
		return;
	}

	lines_count = GetMenuItemCount (WindowsMenu) + 1;
	sy = GetSystemMetrics (SM_CYSCREEN);
	dy = GetSystemMetrics (SM_CYMENU)+1;
	screen_capacity = sy / dy;
	if (MultiColumnMenu)	{
		AppendMenu (WindowsMenu, MF_STRING, IDM_MOREWINDOWS, WINDOWLIST);
		AppendMenu (WindowsMenu, MF_SEPARATOR, IDM_WINMENUSEPARATOR, 0);
		lines_count ++;
		SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) MenuEnum);
		return;
	}
	if (lines_count + n + 1 <= screen_capacity)
		AppendMenu (WindowsMenu, MF_STRING, IDM_MOREWINDOWS, WINDOWLIST);
	lines_count ++;
	AppendMenu (WindowsMenu, MF_SEPARATOR, IDM_WINMENUSEPARATOR, 0);
	i = (int) SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) MenuEnum);
	if (i < n)
		AppendMenu (WindowsMenu, MF_STRING, IDM_MOREWINDOWS, MOREWINDOWS);
}

void	Init_frame (HWND hwnd)
{
	MDICURSORS	cur;
	HMENU main = GetMenu (hwnd);
	WindowsMenu = GetSubMenu (main, WINDOW_MENU_POS);	
	SendMessage (hwnd, WM_MYMDI_MENU, 0, IDM_FIRSTCHILD);
	memset (&cur, 0, sizeof (cur));
	cur.hsize_cursor = LoadCursor (MyInstance, MAKEINTRESOURCE (HSIZE_CURSOR));
	cur.vsize_cursor = LoadCursor (MyInstance, MAKEINTRESOURCE (VSIZE_CURSOR));
	cur.moving_cursor = LoadCursor (MyInstance, MAKEINTRESOURCE (HANDCLOSED_CURSOR));
	SendMessage (hwnd, WM_MYMDI_CURSOR, 0, (LPARAM) &cur);
	SendMessage (hwnd, WM_MYMDI_NUMBERING, numbering_style, 0);
	SendMessage (hwnd, WM_MYMDI_TAB, winbar_present, winbar_style);
	SendMessage (hwnd, WM_MYMDI_DOCKSTYLE, dock_small, 0);
}

#define WINOPT_SECTION "windows-options"

void	Read_windows_options (void)
{
	BOOL	nshow;
	char s [200];

	GetPrivateProfileString (WINOPT_SECTION, "numbering-style", "", s, sizeof (s), IniFile);
	if (! stricmp (s, "mdi-noflip"))
		numbering_style = MDINUMBER_NOFLIP;
	else if (!stricmp (s, "new-first"))
		numbering_style = MDINUMBER_REVERSE | MDINUMBER_NOFLIP;
	else if (!stricmp (s, "active-first"))
		numbering_style = MDINUMBER_ACTIVEFIRST | MDINUMBER_NOFLIP;
	else if (!stricmp (s, "fixed-numbers"))
		numbering_style = MDINUMBER_FIXEDNUMBERS | MDINUMBER_NOFLIP;
	else
		numbering_style = 0;

	GetPrivateProfileString (WINOPT_SECTION, "windows-menu", "", s, sizeof (s), IniFile);
	LongMenu = MultiColumnMenu = FALSE;
	if (!stricmp (s, "multi-column"))	{
		MultiColumnMenu = TRUE;
		LongMenu = TRUE;
	} else if (! stricmp (s, "long-menu"))
		LongMenu = TRUE;

	nshow = read_bool_option (WINOPT_SECTION, "show-numbers", TRUE);
	if (nshow) numbering_style |= MDINUMBER_DRAWNUMBERS;
	DockMessages = read_bool_option (WINOPT_SECTION, "dock-messages", TRUE);
	DockFList    = read_bool_option (WINOPT_SECTION, "dock-file-list", TRUE);
	DockSearchResults = read_bool_option (WINOPT_SECTION, "dock-search-results", TRUE);

	winbar_style = 0;
	GetPrivateProfileString (WINOPT_SECTION, "winbar", "bottom", s, sizeof (s), IniFile);
	winbar_present = stricmp (s, "none") != 0;
	if (! stricmp (s, "top"))
		winbar_style |= MDITAB_TOP;

	GetPrivateProfileString (WINOPT_SECTION, "winbar-numbers", "on", s, sizeof (s), IniFile);
	if (stricmp (s, "off"))
		winbar_style |= MDITAB_NUMBER;

	GetPrivateProfileString (WINOPT_SECTION, "winbar-icons",   "on", s, sizeof (s), IniFile);
	if (stricmp (s, "off"))
		winbar_style |= MDITAB_ICON;

	GetPrivateProfileString (WINOPT_SECTION, "winbar-titles",  "on", s, sizeof (s), IniFile);
	if (stricmp (s, "off"))
		winbar_style |= MDITAB_TEXT;
	GetPrivateProfileString (WINOPT_SECTION, "small-docked-caption",  "on", s, sizeof (s), IniFile);
	dock_small = stricmp (s, "off") != 0;
}

void	write_windows_options (void)
{
	char * s;

	if (numbering_style & MDINUMBER_REVERSE)
		s = "new-first";
	else if (numbering_style & MDINUMBER_ACTIVEFIRST)
		s = "active-first";
	else if (numbering_style & MDINUMBER_FIXEDNUMBERS)
		s = "fixed-numbers";
	else if (numbering_style & MDINUMBER_NOFLIP)
		s = "mdi-noflip";
	else
		s = "classic-mdi";
	WritePrivateProfileString (WINOPT_SECTION, "numbering-style", s, IniFile);
	if (MultiColumnMenu)
		s = "multi-column";
	else if (LongMenu)
		s = "long-menu";
	else
		s = "more-windows";
	WritePrivateProfileString (WINOPT_SECTION, "windows-menu", s, IniFile);

	WritePrivateProfileString (WINOPT_SECTION, "show-numbers",
		(numbering_style & MDINUMBER_DRAWNUMBERS) ? "on" : "off", IniFile);
	WritePrivateProfileString (WINOPT_SECTION, "dock-messages", DockMessages ? "on" : "off", IniFile);
	WritePrivateProfileString (WINOPT_SECTION, "dock-file-list", DockFList ? "on" : "off", IniFile);
	WritePrivateProfileString (WINOPT_SECTION, "dock-search-results", DockSearchResults ? "on" : "off", IniFile);
	if (!winbar_present)
		s = "none";
	else if (winbar_style & MDITAB_TOP)
		s = "top";
	else
		s = "bottom";
	WritePrivateProfileString (WINOPT_SECTION, "winbar", s, IniFile);
	WritePrivateProfileString (WINOPT_SECTION, "winbar-numbers",
		(winbar_style & MDITAB_NUMBER) ? "on" : "off", IniFile);
	WritePrivateProfileString (WINOPT_SECTION, "winbar-icons",
		(winbar_style & MDITAB_ICON) ? "on" : "off", IniFile);
	WritePrivateProfileString (WINOPT_SECTION, "winbar-titles",
		(winbar_style & MDITAB_TEXT) ? "on" : "off", IniFile);
	WritePrivateProfileString (WINOPT_SECTION, "small-docked-caption",
		dock_small ? "on" : "off", IniFile);
}

int	cfgwin_active_page;

BOOL CALLBACK CfgWinNumberingDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	DWORD num;

	switch (msg)	{
	case WM_INITDIALOG:
		if (numbering_style & MDINUMBER_FIXEDNUMBERS)
			i = CFGWIN_NUMBERING_FIXED;
		else if (numbering_style & MDINUMBER_ACTIVEFIRST)
			i = CFGWIN_NUMBERING_ACTIVEFIRST;
		else if (numbering_style & MDINUMBER_REVERSE)
			i = CFGWIN_NUMBERING_NEWFIRST;
		else if (numbering_style & MDINUMBER_NOFLIP)
			i = CFGWIN_NUMBERING_MDINOFLIP;
		else
			i = CFGWIN_NUMBERING_MDI;
		CheckDlgButton (hwnd, i, TRUE);
		CheckDlgButton (hwnd, CFGWIN_NUMBERING_SHOW, numbering_style & MDINUMBER_DRAWNUMBERS);
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_APPLY:
			if (IsDlgButtonChecked (hwnd, CFGWIN_NUMBERING_MDINOFLIP))
				num = MDINUMBER_NOFLIP; 
			else if (IsDlgButtonChecked (hwnd, CFGWIN_NUMBERING_NEWFIRST))
				num = MDINUMBER_REVERSE | MDINUMBER_NOFLIP;
			else if (IsDlgButtonChecked (hwnd, CFGWIN_NUMBERING_ACTIVEFIRST))
				num = MDINUMBER_ACTIVEFIRST | MDINUMBER_NOFLIP;
			else if (IsDlgButtonChecked (hwnd, CFGWIN_NUMBERING_FIXED))
				num = MDINUMBER_FIXEDNUMBERS | MDINUMBER_NOFLIP;
			else
				num = 0;

			if (IsDlgButtonChecked (hwnd, CFGWIN_NUMBERING_SHOW))
				num |= MDINUMBER_DRAWNUMBERS;

			if (num != numbering_style)	{
				numbering_style = num;
				SendMessage (FrameWindow, WM_MYMDI_NUMBERING, numbering_style, 0);
			}
			return 1;
		case PSN_SETACTIVE:
			cfgwin_active_page = CFGWIN_NUMBERING_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGWIN_NUMBERING_DIALOG);
		}
		return 0;

	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED)
			PropSheet_Changed (GetParent (hwnd), hwnd);
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGWIN_DIALOG, lparam);
	}
	return 0;
}

BOOL CALLBACK CfgWinMenuDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		CheckDlgButton (hwnd, CFGWIN_MENU_MOREWINDOWS, !LongMenu);
		CheckDlgButton (hwnd, CFGWIN_MENU_LONG, LongMenu && ! MultiColumnMenu);
		CheckDlgButton (hwnd, CFGWIN_MENU_MULTICOLUMN, LongMenu && MultiColumnMenu);
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_APPLY:
			MultiColumnMenu = IsDlgButtonChecked (hwnd, CFGWIN_MENU_MULTICOLUMN);
			LongMenu = MultiColumnMenu || IsDlgButtonChecked (hwnd, CFGWIN_MENU_LONG);
			return 1;
		case PSN_SETACTIVE:
			cfgwin_active_page = CFGWIN_MENU_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGWIN_MENU_DIALOG);
		}
		return 0;
	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED)
			PropSheet_Changed (GetParent (hwnd), hwnd);
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGWIN_DIALOG, lparam);
	}
	return 0;
}

BOOL CALLBACK CfgWinDockedDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	BOOL dsmall;
	switch (msg)	{
	case WM_INITDIALOG:
		CheckDlgButton (hwnd, CFGWIN_DOCKED_MESSAGES, DockMessages);
		CheckDlgButton (hwnd, CFGWIN_DOCKED_FILELIST, DockFList);
		CheckDlgButton (hwnd, CFGWIN_DOCKED_SEARCHRES, DockSearchResults);
		CheckDlgButton (hwnd, CFGWIN_SMALL_DOCKED, dock_small);
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_APPLY:
			DockMessages = IsDlgButtonChecked (hwnd, CFGWIN_DOCKED_MESSAGES);
			DockFList    = IsDlgButtonChecked (hwnd, CFGWIN_DOCKED_FILELIST);
			DockSearchResults = IsDlgButtonChecked (hwnd, CFGWIN_DOCKED_SEARCHRES);
			dsmall       = IsDlgButtonChecked (hwnd, CFGWIN_SMALL_DOCKED);
			if (dock_small != dsmall)
				SendMessage (FrameWindow, WM_MYMDI_DOCKSTYLE, dsmall, 0);
			dock_small = dsmall;
			return 1;
		case PSN_SETACTIVE:
			cfgwin_active_page = CFGWIN_DOCKED_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGWIN_DOCKED_DIALOG);
		}
		return 0;
	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED)
			PropSheet_Changed (GetParent (hwnd), hwnd);
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGWIN_DIALOG, lparam);
	}
	return 0;
}

BOOL CALLBACK CfgWinBarDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	DWORD style;
	BOOL  present;

	switch (msg)	{
	case WM_INITDIALOG:
		if (!winbar_present)
			CheckDlgButton (hwnd, CFGWIN_WINBAR_NONE, 1);
		else if (winbar_style & MDITAB_TOP)
			CheckDlgButton (hwnd, CFGWIN_WINBAR_TOP, 1);
		else
			CheckDlgButton (hwnd, CFGWIN_WINBAR_BOTTOM, 1);
		CheckDlgButton (hwnd, CFGWIN_WINBAR_ICON,   (winbar_style & MDITAB_ICON) != 0);
		CheckDlgButton (hwnd, CFGWIN_WINBAR_TEXT,   (winbar_style & MDITAB_TEXT) != 0);
		CheckDlgButton (hwnd, CFGWIN_WINBAR_NUMBER, (winbar_style & MDITAB_NUMBER) != 0);
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_APPLY:
			style = 0;
			if (IsDlgButtonChecked (hwnd, CFGWIN_WINBAR_ICON)) style |= MDITAB_ICON;
			if (IsDlgButtonChecked (hwnd, CFGWIN_WINBAR_TEXT)) style |= MDITAB_TEXT;
			if (IsDlgButtonChecked (hwnd, CFGWIN_WINBAR_NUMBER)) style |= MDITAB_NUMBER;
			if (IsDlgButtonChecked (hwnd, CFGWIN_WINBAR_TOP)) style |= MDITAB_TOP;
			present = IsDlgButtonChecked (hwnd, CFGWIN_WINBAR_TOP) ||
				  IsDlgButtonChecked (hwnd, CFGWIN_WINBAR_BOTTOM);
			if (style != winbar_style || present != winbar_present)	{
				winbar_style = style;
				winbar_present = present;
				SendMessage (FrameWindow, WM_MYMDI_TAB, winbar_present, winbar_style);
			}
			return 1;
		case PSN_SETACTIVE:
			cfgwin_active_page = CFGWIN_WINBAR_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGWIN_WINBAR_DIALOG);
		}
		return 0;
	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED)
			PropSheet_Changed (GetParent (hwnd), hwnd);
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGWIN_DIALOG, lparam);
	}
	return 0;
}

int	cfgwin_n_active_page = 0;

void	Configure_windows (void)
{
	int i;
	PROPSHEETHEADER psh;
	PROPSHEETPAGE pages [4];

	memset (pages, 0, sizeof (pages));
	pages [0].dwSize = sizeof (pages [0]);
	pages [0].dwFlags = PSP_HASHELP;
	pages [0].hInstance = MyInstance;
	pages [3] = pages [2] = pages [1] = pages [0];

	pages [0].pfnDlgProc = CfgWinNumberingDialogProc;
	pages [1].pfnDlgProc = CfgWinMenuDialogProc;
	pages [2].pfnDlgProc = CfgWinDockedDialogProc;
	pages [3].pfnDlgProc = CfgWinBarDialogProc;
	pages [0].pszTemplate = MAKEINTRESOURCE (CFGWIN_NUMBERING_DIALOG);
	pages [1].pszTemplate = MAKEINTRESOURCE (CFGWIN_MENU_DIALOG);
	pages [2].pszTemplate = MAKEINTRESOURCE (CFGWIN_DOCKED_DIALOG);
	pages [3].pszTemplate = MAKEINTRESOURCE (CFGWIN_WINBAR_DIALOG);

	memset (&psh, 0, sizeof (psh));
	psh.dwSize = sizeof (psh); 
	psh.dwFlags = PSH_PROPSHEETPAGE;
	psh.hwndParent = FrameWindow;
	psh.hInstance = MyInstance;
	psh.pszIcon = 0;
	psh.pszCaption = "Configure Windows System";
	psh.nPages = 4;
        psh.pStartPage = 0;
	psh.nStartPage = cfgwin_n_active_page;
	psh.ppsp = pages;
	
	PropertyDialog (&psh);
	for (i = 0; i < 4; i ++)
		if (pages [i].pszTemplate == MAKEINTRESOURCE (cfgwin_active_page))
			cfgwin_n_active_page = i;

	write_windows_options ();
}
