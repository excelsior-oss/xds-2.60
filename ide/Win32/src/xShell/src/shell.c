#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "edit.h"
#include "message.h"
#include "flist.h"
#include "res.h"
#include "xdsres.h"
#include "classes.h"
#include "toolbar.h"
#include "mdi.h"
#include "frame.h"
#include "var.h"
#include "open.h"
#include "proj.h"
#include "tbuf.h"
#include "find.h"
#include "config.h"
#include "tools.h"
#include "make.h"
#include "search.h"
#include "filter.h"
#include "syntax.h"
#include "optedit.h"
#include "logo.h"
#include "about.h"
#include "macro.h"
#include <time.h>

HINSTANCE  MyInstance;
HINSTANCE  ResInstance;
HWND	FrameWindow;
HWND	StatusBar;
HWND	StatusBarOwner;
HWND	ToolBar;
HWND	FoundHWND;
HACCEL	MainAccel;
HACCEL	ToolAccel;
HMENU	FileMenu;
HMENU	ToolsMenu;
HMENU	ProjectMenu;
HICON	Console_icon;
BOOL	Cancelled;
BOOL	DoReadFileMap = 1;
BOOL	DoWriteFileMap = 0;

BOOL	DemoMode = 1;
BOOL	HasExpired = 0;
long	ExpireDate = 0;

int	StatusBarHeight;

char HomeDir	[_MAX_PATH+1];
char StartDir	[_MAX_PATH+1];
char IniFile	[_MAX_PATH+1];
char HelpFile	[_MAX_PATH+1];
char IDEHelpFile [_MAX_PATH+1];
char ProjIni	[_MAX_PATH+1];
char RedFile	[_MAX_PATH+1];
char XDSMAIN	[_MAX_PATH+1];

char ShellName    [80];
char RedFileName  [80];
char XRedFileName [80];
char CfgFileName  [80];
char OptFileName  [80];
char IniFileName  [80];
char HelpFileName [80];
char IDEHelpFileName [80];

#define NADDHELPS 10
char * AddHelps [NADDHELPS][2] = {0};

void	Set_title (char * name)
{
	char s [200];
	if (!name || !name [0])
		strcpy (s, ShellName);
	else
		sprintf (s, "%s - %s", ShellName, name);
	SetWindowText (FrameWindow, s);
}

/* ------------------------------------------------------------------ */

void	Enable_item (int item, BOOL enable)
{
	Enable_toolbar_button (ToolBar, item, enable);
	EnableMenuItem  (GetMenu (FrameWindow), item, enable ?
			MF_ENABLED : MF_GRAYED);
}

/* ------------------------------------------------------------------ */

BOOL	create_status_bar (HWND parent)
{
	RECT rc;
	StatusBar = CreateWindow ("shellstatusbar", "",
			     WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
			     0, 0, 1500, 2, parent, 0,
			     MyInstance, NULL);

	if (!StatusBar) return FALSE;
	StatusBarOwner = 0;
	GetWindowRect (StatusBar, &rc);
	StatusBarHeight = rc.bottom - rc.top;
	SetWindowText (StatusBar, " ");
	return TRUE;
}

BOOL	create_tool_bar (HWND parent)
{
	ToolBar = Make_empty_toolbar (parent);

	if (!ToolBar) return FALSE;

	Add_toolbar (ToolBar, BMP_NEW,      IDM_NEW,  FALSE, 0);
	Add_toolbar (ToolBar, BMP_OPEN,     IDM_OPEN, FALSE, 0);
	Add_toolbar (ToolBar, BMP_SAVE,     IDM_SAVE, FALSE, 0);

	Add_toolbar (ToolBar, BMP_CUT,      IDM_CUT, TRUE, 0);
	Add_toolbar (ToolBar, BMP_COPY,     IDM_COPY, FALSE, 0);
	Add_toolbar (ToolBar, BMP_PASTE,    IDM_PASTE, FALSE, 0);
	Add_toolbar (ToolBar, BMP_UNDO,     IDM_UNDO, FALSE, 0);

	Add_toolbar (ToolBar, BMP_FIND,     IDM_FIND, TRUE, 0);
	Add_toolbar (ToolBar, BMP_FINDNEXT, IDM_FINDNEXT, FALSE, 0);

	Add_toolbar (ToolBar, BMP_RUN,      IDM_RUN, TRUE, 0);
	Add_toolbar (ToolBar, BMP_STOP,     IDM_STOP, FALSE, 0);
	Add_toolbar (ToolBar, BMP_DEBUG,    IDM_DEBUG, FALSE, 0);
	Add_toolbar (ToolBar, BMP_VIEWUSERS,IDM_VIEWUSERS, FALSE, 0);

	Add_toolbar (ToolBar, BMP_HELP,	    IDM_HELP, TRUE, 0);

	Create_tools_toolbar ();

	return TRUE;
}

/* ------------------------------------------------------------------ */
char	menu_save_buffer [300] = "";

#define MENU_SAVE_AND_CLEAR 0
#define MENU_RESTORE -1
#define MENU_CLEAR -2

int	CurMenuId = 0;
int	CurMenuSel = 0;

void	Set_status_bar (HWND owner, char * s)
{
	if (CurMenuSel)
		strcpy (menu_save_buffer, s);
	else	{
		SetWindowText (StatusBar, s);
		StatusBarOwner = owner;
	}
}

void	Clear_status_bar (HWND owner)
{
	if (StatusBarOwner == owner)	{
		if (CurMenuSel)
			strcpy (menu_save_buffer, " ");
		else	{
			SetWindowText (StatusBar, " ");
			StatusBarOwner = 0;
		}
	}
}

void	store_status_bar_text (char * t)
{
	SetWindowText (StatusBar, t);
}

void	set_menu_text (int id)
{
	char s [2000];
	char *d;

	CurMenuId = 0;
	if (id == MENU_SAVE_AND_CLEAR)	{
		CurMenuSel = -1;
		GetWindowText (StatusBar, menu_save_buffer, sizeof (menu_save_buffer));
		SetWindowText (StatusBar, "");
	} else if (id == MENU_RESTORE)	{
		CurMenuSel = 0;
		if (menu_save_buffer [0])
			SetWindowText (StatusBar, menu_save_buffer);
		menu_save_buffer [0] = 0;
	} else if (id == MENU_CLEAR)	{
		CurMenuSel = -1;
		if (!menu_save_buffer[0])
			GetWindowText (StatusBar, menu_save_buffer, sizeof (menu_save_buffer));
		SetWindowText (StatusBar, "");
	} else	{
		if (id == CurMenuSel) return;
		CurMenuSel = id;
		s[0]=0;
		if (id >= IDM_FIRST_FILEOPEN && id <= IDM_LAST_FILEOPEN)	{
			d = File_name_from_history (id-IDM_FIRST_FILEOPEN);
			if (d)
				wsprintf (s, "Open file %s", d);
		} else if (id >= IDM_FIRST_PROJOPEN && id <= IDM_LAST_PROJOPEN)	{
			d = Proj_name_from_history (id-IDM_FIRST_PROJOPEN);
			if (d)
				wsprintf (s, "Open project %s", d);
		} else if (id >= IDM_FIRST_TOOL && id <= IDM_LAST_TOOL)
			strcpy (s, Tool_get_comment (id - IDM_FIRST_TOOL));
		else if (id >= IDM_FIRST_HELP && id < IDM_FIRST_HELP + NADDHELPS)	{
			strcpy (s, "Open custom help file ");
			d = AddHelps [id-IDM_FIRST_HELP][1];
			if (d)
				strcat (s, d);
			else	{
				strcat (s, AddHelps [id - IDM_FIRST_HELP][0]);
				strcat (s, ".hlp");
			}
		} else if (id >= IDM_FIRSTCHILD && id <= IDM_FIRSTCHILD + 9)	{
			id -= IDM_FIRSTCHILD;
#if 0
			if (id == 9)
				strcpy (s, "Invoke \"Select window\" dialog with full window list");
			else if (id < win_cnt)	{
				strcpy (s, "Activate ");
				GetWindowText (win_list [id], s+strlen (s), sizeof(s)-strlen(s)-1);
			}
#endif
		} else if (LoadString (MyInstance, id, s, sizeof (s)))	{
			CurMenuId = id;
			s [sizeof (s)-1] = 0;
		} else
			s[0]=0;
		if (!menu_save_buffer[0])
			GetWindowText (StatusBar, menu_save_buffer, sizeof (menu_save_buffer));
		SetWindowText (StatusBar, s);
	}
}

/* ------------------------------------------------------------------ */

void	add_help_menu (HMENU menu)
{
	int i;
	for (i = 0; i < NADDHELPS && AddHelps [i][0]; i++)	{
		InsertMenu (menu, IDM_HELPABOUT, MF_ENABLED|MF_STRING|MF_BYCOMMAND,
			 IDM_FIRST_HELP+i, AddHelps [i][0]);
	}
	if (i)	InsertMenu (menu, IDM_HELPABOUT, MF_SEPARATOR, 0, 0);
}

void	call_help (int id)
{
	char s [STRLEN];
	id -= IDM_FIRST_HELP;
	if (id >= NADDHELPS) return;
	if (AddHelps [id][1])
		Make_line (AddHelps [id][1], s, sizeof (s));
	else
		wsprintf (s, "%s\\%s.hlp", HomeDir, AddHelps [id][0]);
	WinHelp (FrameWindow, s, HELP_CONTENTS, 0);
}

/* ------------------------------------------------------------------ */
void	Disable_all_items (void)
{
	Enable_item (IDM_PASTE, FALSE);
	Enable_item (IDM_COPY, FALSE);
	Enable_item (IDM_CUT, FALSE);
	Enable_item (IDM_ADD, FALSE);
	Enable_item (IDM_EXCHANGE, FALSE);
	Enable_item (IDM_UNDO, FALSE);
	Enable_item (IDM_RELOAD, FALSE);
	Enable_item (IDM_SAVE, FALSE);
	Enable_item (IDM_SAVEAS, FALSE);
	Enable_item (IDM_SHARE, FALSE);
	Enable_item (IDM_CLOSE, FALSE);
	Enable_item (IDM_PRINT, FALSE);
	Enable_item (IDM_FIND, FALSE);
	Enable_item (IDM_FINDNEXT, FALSE);
	Enable_item (IDM_FINDPREV, FALSE);
	Enable_item (IDM_REPLACE, FALSE);
	Enable_item (IDM_REPLACENEXT, FALSE);
	Enable_item (IDM_GOTOLINE, FALSE);
	Enable_item (IDM_NEXTMESS, FALSE);
	Enable_item (IDM_PREVMESS, FALSE);
	Enable_item (IDM_NEW, TRUE);
	Enable_item (IDM_OPEN, TRUE);
	Enable_item (IDM_HELP, TRUE);

	Tools_enable ();
}

HWND	ActiveChild (void)
{
	return (HWND) SendMessage (FrameWindow, WM_MDIGETACTIVE, 0, 0);
}

BOOL	QueryCloseProc (int num, HWND hwnd, BOOL free,
			BOOL docked, BOOL active, BOOL mdiactive)
{
	if (!hwnd) return TRUE;
	if (SendMessage (hwnd, WM_CLOSE, 0, 0))	{
		Cancelled = TRUE;
		return FALSE;
	}
	return TRUE;
}

BOOL	CloseAll (void)
{
	if (ProjIni [0])	{
		Write_windows (ProjIni);
		Write_file_list (ProjIni);
	}
	Cancelled = FALSE;
	SendMessage (FrameWindow, WM_MYMDI_ENUM, 0, (LPARAM) QueryCloseProc);

	if (!Cancelled)
		Clear_file_list ();
	return !Cancelled;
}

WNDPROC OldClientProc;

LRESULT	CALLBACK ClientWindowProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	RECT rc;
	HDC dc;
	HBRUSH br;

	switch (msg)	{
	case WM_PAINT:
		dc = BeginPaint (hwnd, &ps);
		GetClientRect (hwnd, &rc);
		br = CreateSolidBrush (AbsSysColor (ShellColor, COLOR_APPWORKSPACE));
		FillRect (dc, &rc, br);
		DeleteObject (br);
		EndPaint (hwnd, &ps);
		return 0;
	case WM_ERASEBKGND:
		GetClientRect (hwnd, &rc);
		br = CreateSolidBrush (AbsSysColor (ShellColor, COLOR_APPWORKSPACE));
		FillRect ((HDC) wparam, &rc, br);
		DeleteObject (br);
		return 1;
	}
	return CallWindowProc (OldClientProc, hwnd, msg, wparam, lparam);
}

BOOL	frame_command (WPARAM wparam, LPARAM lparam)
{
	HWND   child;

	switch (CONTROL_ID) {
	case IDM_NEW:	Edit_new  (); break;
	case IDM_OPEN:	Open (); break;
	case IDM_CLOSE:
		child = ActiveChild ();
		if (child)
			SendMessage (child, WM_CLOSE, 0, 0);
		break;
	case IDM_SAVEALL:
		Edit_save_all (TRUE);
		break;
	case IDM_EXIT:
		DestroyWindow (FrameWindow);
		break;
	case IDM_MOREFILES:
		MoreFiles ();
		break;
	case IDM_FINDFILES:
		Files_search ();
		break;
	case IDM_PROJNEW:
		Proj_new (0); break;
	case IDM_PROJOPEN:
		Proj_open (); break;
	case IDM_NEWSTYLE:
		Proj_new_style (); break;
	case IDM_PROJSAVESTYLE:
		Proj_save_as_style (); break;
	case IDM_PROJCLOSE:
		Proj_close (); break;
	case IDM_APPLYSTYLE:
		Proj_apply_style (); break;
	case IDM_EDITSTYLE:
		Proj_edit_style (); break;
	case IDM_DELETESTYLE:
		Proj_delete_style (); break;
	case IDM_PROJEDIT:
		Proj_edit (); break;
	case IDM_FILELIST:
		Show_file_list (); break;
	case IDM_PROJOPTIONS:
		OpenOptions (); break;
	case IDM_SHOWMESS:
		Show_messages (); break;
	case IDM_HIDEMESS:
		Hide_messages (); break;
	case IDM_NEXTMESS:
		Next_message (); break;
	case IDM_PREVMESS:
		Prev_message (); break;
	case IDM_TILEVERT:
		SendMessage (FrameWindow, WM_MDITILE, MDITILE_VERTICAL, 0);
		break;
	case IDM_TILEHORIZ:
		SendMessage (FrameWindow, WM_MDITILE, MDITILE_HORIZONTAL, 0);
		break;
	case IDM_CASCADE:
		SendMessage (FrameWindow, WM_MDICASCADE, 0, 0);
		break;
	case IDM_ARRANGE:
		SendMessage (FrameWindow, WM_MDIICONARRANGE, 0, 0);
		break;
	case IDM_CLOSEALL:
		CloseAll ();
		break;
	case IDM_DOCK:
	case IDM_UNDOCK:
		SendMessage (FrameWindow, WM_MYMDI_DOCK, (WPARAM) ActiveChild (), 0);
		break;
	case IDM_FREE:
		SendMessage (FrameWindow, WM_MYMDI_FREE, (WPARAM) ActiveChild (), 0);
		break;
	case IDM_MOREWINDOWS:
		MoreWindows ();
		break;
	case IDM_RUN:
		Proj_run ();
		break;
	case IDM_STOP:
		Proj_stop ();
		break;
	case IDM_DEBUG:
		Proj_debug ();
		break;
	case IDM_VIEWUSERS:
		Show_user_screen ();
		break;
	case IDM_RUNOPTIONS:
		Configure_run ();
		break;
	case IDM_CONFIGUREGENERAL:
		Configure_general ();
		break;
	case IDM_CONFIGUREWINDOWS:
		Configure_windows ();
		break;
	case IDM_CONFIGURETOOLS:
		Configure_tools ();
		break;
	case IDM_CONFIGUREPROJECT:
		Configure_new_project ();
		break;
	case IDM_CONFIGUREEDITOR:
		Configure_editor ();
		break;
	case IDM_COLOR:
		Configure_colors ();
		break;
	case IDM_CONFIGUREPRINT:
		Configure_print ();
		break;
	case IDM_CONFIGUREFILTERS:
		Configure_filters ();
		break;
	case IDM_CONFIGURE_LANGUAGES:
		Configure_languages ();
		break;
	case IDM_HELP:
		if (Find_help_file ())
			WinHelp (FrameWindow, HelpFile, HELP_FINDER, 0);
		break;
	case IDM_HELPKEYBOARD:
		WinHelp (FrameWindow, IDEHelpFile, HELP_CONTEXT, HELP_KEYBOARD);
		break;
	case IDM_HELPONHELP:
		WinHelp (FrameWindow, 0, HELP_HELPONHELP, 0);
		break;
	case IDM_HELPABOUT:
		Help_about ();
		break;
	default:
		if (CONTROL_ID >= IDM_FIRST_FILEOPEN && CONTROL_ID <= IDM_LAST_FILEOPEN) {
			Open_by_history (CONTROL_ID - IDM_FIRST_FILEOPEN);
			return TRUE;
		}
		if (CONTROL_ID >= IDM_FIRST_PROJOPEN && CONTROL_ID <= IDM_LAST_PROJOPEN) {
			Proj_open_by_history (CONTROL_ID - IDM_FIRST_PROJOPEN);
			return TRUE;
		}
		if (CONTROL_ID >= IDM_FIRST_HELP && CONTROL_ID <= IDM_LAST_HELP) {
			call_help (CONTROL_ID);
			return TRUE;
		}
		if (CONTROL_ID >= IDM_FIRST_TOOL && CONTROL_ID <= IDM_LAST_TOOL) {
			Call_tool (CONTROL_ID - IDM_FIRST_TOOL);
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

void	Toolbar_notify (HWND bar, NMHDR * nm);

LRESULT CALLBACK FrameProc (HWND hwnd, UINT msg,
			    WPARAM wparam, LPARAM lparam)
{
	int w, h;
	HMENU menu;
	char s [100];
	RECT rc;
	RECT * prc;
	NMHDR * nm;
	HBRUSH br;

	switch (msg) {
	case WM_CREATE:
		create_status_bar (hwnd);
		create_tool_bar (hwnd);
		Bind_status_bar (ToolBar, StatusBar);
		Init_frame (hwnd);
		break;
	case WM_CLOSE:
		if (!CloseAll ()) return 1;
		Write_search_history ();
		break;
	case WM_SIZE:
		if (ToolBar)
			SendMessage (ToolBar, TB_AUTOSIZE, 0, 0);
		w = LOWORD (lparam); h = HIWORD (lparam);
		MoveWindow (StatusBar, 0, h-StatusBarHeight,
				w, StatusBarHeight, 1);
		break;
	case WM_MYMDI_RECT:
		prc = (RECT*) lparam;
		if (ToolBar)	{
			GetWindowRect (ToolBar, &rc);
			prc->top += rc.bottom-rc.top;
		}
		prc->bottom -= StatusBarHeight;
		break;
	case WM_MYMDI_CLIENTPAINT:
	case WM_MYMDI_CLIENTERASE:
		br = CreateSolidBrush (AbsSysColor (ShellColor, COLOR_APPWORKSPACE));
		FillRect ((HDC) wparam, (RECT*) lparam, br);
		DeleteObject (br);
		return 1;
	case WM_MYMDI_ENTEREMPTY:
		Edit_new ();
		return 0;
	case WM_COMMAND:
		if (frame_command (wparam, lparam)) return 0;
		break;
	case WM_SYSCOLORCHANGE:
		Create_pens ();
		Redither_toolbar (ToolBar);
		break;
	case WM_ACTIVATEAPP:
		if (wparam)
			Edit_test_reload (TRUE);
		break;
	case WM_NOTIFY:
		nm = (NMHDR*) lparam;
		Toolbar_notify (ToolBar, nm);
		break;
	case WM_INITMENU:
		Make_windows_menu ();
		break;
	case WM_INITMENUPOPUP:
		if (HIWORD (lparam)) break; /* System menu flag */
		menu  = (HMENU) wparam;
		if (menu == FileMenu)
			Add_file_history (menu);
		else if (menu == ProjectMenu)
			Add_project_history (menu);
		break;
	case WM_MENUSELECT:
		if (HIWORD (wparam) == 0xFFFF)	{
			if (lparam == 0)
				set_menu_text (MENU_RESTORE);
			else
				set_menu_text (-100);
		}
		else if (HIWORD (wparam) & MF_POPUP)	{
			if (HIWORD (wparam) & MF_SYSMENU)
				set_menu_text (M_BASE+1);
			else
				set_menu_text (M_BASE + Find_popup (GetSubMenu ((HMENU) lparam, LOWORD (wparam))));
		}
		else if (HIWORD (wparam) & MF_SEPARATOR)
			set_menu_text (MENU_CLEAR);
		else
			set_menu_text ((int) LOWORD (wparam));
		break;
	case WM_HELP:
		if (CurMenuId) Menu_help (CurMenuId, TRUE);
		break;
	case WM_QUERYENDSESSION:
		return CloseAll ();
	case WM_DESTROY:
		GetWindowRect (hwnd, &rc);
		wsprintf (s, "%d,%d,%d,%d,%d", rc.left, rc.top, rc.right, rc.bottom,
			IsZoomed(hwnd) ? 2 : IsIconic (hwnd) ? 0 : 1);
		WritePrivateProfileString ("general", "main-window", s, IniFile);
		PostQuitMessage (0);
		break;
	}
	return MyDefMDIProc (hwnd, msg, wparam, lparam);
}

void	GetHomeDir (void)
{
	char fname  [_MAX_PATH+1];

	GetModuleFileName (MyInstance, fname, _MAX_PATH+1);
	strcpy (HomeDir,  dir_only (fname));
	GetCurrentDirectory (sizeof (StartDir), StartDir);
	StartDir [sizeof (StartDir)-1] = 0;
	strcpy (ProjDir, StartDir);
	Set_name ("homedir", HomeDir);
	Set_name ("startdir", StartDir);
	Set_name ("projdir", ProjDir);
}

void	find_ini_file (void)
{
	FILE * f;
	int l;
	strcpy (IniFile, StartDir);
	strcat (IniFile, "\\");
	strcat (IniFile, IniFileName);
	f = fopen (IniFile, "r");
	if (f)	{
		fclose (f);
		return;
	}
	GetWindowsDirectory (IniFile, sizeof (IniFile));
	l = strlen (IniFile);
	if (!l || IniFile [l-1] != '\\')
		strcat (IniFile, "\\");
	strcat (IniFile, IniFileName);
	f = fopen (IniFile, "r");
	if (f)	{
		fclose (f);
		return;
	}
	strcpy (IniFile, HomeDir);
	strcat (IniFile, "\\");
	strcat (IniFile, IniFileName);
	Set_name ("inifile", IniFile);
}

BOOL	Find_help_file (void)
{
	FILE * f;
	
	Make_line (XDSMAIN, HelpFile, sizeof (HelpFile));
	strcpy (HelpFile, XDSMAIN);
	dir_only (HelpFile);
	strcat (HelpFile, "\\");
	strcat (HelpFile, HelpFileName);
	f = fopen (HelpFile, "r");
	if (f)	{
		fclose (f);
		return TRUE;
	}
	strcpy (HelpFile, HomeDir);
	strcat (HelpFile, "\\");
	strcat (HelpFile, HelpFileName);
	f = fopen (HelpFile, "r");
	if (f)	{
		fclose (f);
		return TRUE;
	}
	HelpFile [0] = 0;
	return TRUE;
}

BOOL	InitApplication (void)
{
	char fname [_MAX_PATH+1];

	WNDCLASSEX wc;

	Create_pens ();
	clear_filedir ();
	GetHomeDir ();

	GetModuleFileName (MyInstance, fname, _MAX_PATH+1);

	if (! LoadString (MyInstance, IDSTR_ORIG_INIFILENAME, IniFileName, sizeof (IniFileName)))
		return FALSE;
	IniFileName [sizeof (IniFileName) - 1] = 0;
	find_ini_file ();

	GetPrivateProfileString ("general", "resource", "xdsres.dll", fname, sizeof (fname)-1, IniFile);
	ResInstance = LoadLibrary (fname);
	if (!ResInstance) return FALSE;

	if (! LoadString (ResInstance, IDSTR_INIFILENAME, IniFileName, sizeof (IniFileName)))
		return FALSE;
	IniFileName [sizeof (IniFileName) - 1] = 0;
	find_ini_file ();

	if (! LoadString (ResInstance, IDSTR_SHELLNAME, ShellName, sizeof (ShellName)))
		return FALSE;
	ShellName [sizeof (ShellName) - 1] = 0;
	strcat (ShellName, file_version (fname, FALSE));
#ifdef _EDUCVERSION
	strcat (ShellName, " EDU");
#endif
	if (DemoMode)
		strcat (ShellName, " DEMO");

	if (! LoadString (ResInstance, IDSTR_HELPFILENAME, HelpFileName, sizeof (HelpFileName)))
		return FALSE;

	HelpFileName [sizeof (HelpFileName) - 1] = 0;

	if (! LoadString (ResInstance, IDSTR_IDEHELPFILENAME, IDEHelpFileName, sizeof (IDEHelpFileName)))
		return FALSE;
	IDEHelpFileName [sizeof (IDEHelpFileName) - 1] = 0;

	if (! LoadString (ResInstance, IDSTR_XREDFILENAME, XRedFileName, sizeof (XRedFileName)))
		return FALSE;
	XRedFileName [sizeof (XRedFileName) - 1] = 0;

	strcpy (HelpFile, HomeDir);
	strcat (HelpFile, "\\");
	strcat (HelpFile, HelpFileName);

	strcpy (IDEHelpFile, HomeDir);
	strcat (IDEHelpFile, "\\");
	strcat (IDEHelpFile, IDEHelpFileName);
	
    /* register the frame window class   */

	wc.cbSize = sizeof (WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = (LPVOID) FrameProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = MyInstance;
	wc.hIcon = LoadIcon (ResInstance, MAKEINTRESOURCE (MAIN_ICON));
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = 0;//(HBRUSH) (COLOR_APPWORKSPACE+1);
	wc.lpszMenuName = "ShellMenu";
	wc.lpszClassName = "ShellClass";
	wc.hIconSm = LoadImage (MyInstance, MAKEINTRESOURCE (MAIN_ICON),
				IMAGE_ICON,
				GetSystemMetrics(SM_CXSMICON),
				GetSystemMetrics(SM_CYSMICON),
				0);
	if (!RegisterClassEx (&wc)) return FALSE;
	if (!Register_classes ()) return FALSE;
	if (!Register_edit    ()) return FALSE;
	if (!RegisterMessage  ()) return FALSE;
	if (!Register_file_list ()) return FALSE;
	if (!RegisterOptEdit  ()) return FALSE;
	if (!Register_config   ()) return FALSE;
	if (!Register_search_list ()) return FALSE;

	Init_tools ();

	Console_icon = LoadIcon (MyInstance, MAKEINTRESOURCE (CONSOLE_ICON));

	Read_windows_options ();
	Read_options ();
	Read_languages ();
	Read_macroes();
	Read_open_filter ();
	init_alpha ();

	return TRUE;
}

BOOL	InitMainWindow (int cmdshow)
{
	HMENU menu;
	char s [100];
	int x, y, w, h, m;
	GetPrivateProfileString ("general", "main-window", "", s, sizeof (s), IniFile);
	x = y  = 0;
	w = h = -1;
	m = -1;
	sscanf (s, " %d , %d , %d , %d , %d", &x, &y, &w, &h, &m);
	w -= x;
	h -= y;
	if (m < 0 || w <= 0 || h <= 0 || x+w <= 0 || y+h <= 0)
		x = y = w = h = CW_USEDEFAULT;

	cmdshow = m == 2 ? SW_MAXIMIZE :
		  m == 0 ? SW_MINIMIZE :
		  m == 1 ? SW_SHOWNORMAL : SW_SHOWDEFAULT;

	FrameWindow = CreateWindow (
			"ShellClass",
			ShellName,
			WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
			x, y, w, h,
			NULL,			/* parent window */
			NULL,                   /* menu handle */
			MyInstance,		/* program handle */
			NULL			/* create parms */
		);

	if (!FrameWindow) return FALSE;
	MainAccel = LoadAccelerators (MyInstance, "ShellAccel");
	ToolAccel = NULL;
	ShowWindow   (FrameWindow, cmdshow);
	UpdateWindow (FrameWindow);
	GetPrivateProfileString ("general", "no-read-file-map", "", s, sizeof (s), IniFile);
	DoReadFileMap = stricmp (s, "on") != 0;
	GetPrivateProfileString ("general", "no-write-file-map", "", s, sizeof (s), IniFile);
	DoReadFileMap = stricmp (s, "off") == 0;
	Read_file_history (IniFile);
	Read_open_history (IniFile);
	Read_proj_history (IniFile);
	Read_section (AddHelps, NADDHELPS, "Help", IniFile);
	menu = GetMenu (FrameWindow);
	add_help_menu (GetSubMenu (menu, HELP_MENU_POS));
	FileMenu = GetSubMenu (menu, FILE_MENU_POS);
	ProjectMenu = GetSubMenu (menu, PROJECT_MENU_POS);
	ToolsMenu = GetSubMenu (menu, TOOLS_MENU_POS);
	SetMenuContextHelpId(FileMenu, 1234);
	SetMenuContextHelpId(ProjectMenu, 1235);
	SetMenuContextHelpId(ToolsMenu, 1236);
	Proj_set_vars ("");
	Update_tools_for_file ("");
	Create_tools_menu ();
	Disable_all_items ();
	Proj_enable_menu ();
	return TRUE;
}

void	DoCmdLine (LPSTR cmdline)
{
	char * last;

	if (cmdline [0])	{
		if (cmdline [0] == '"')
			++ cmdline;
		if (cmdline [strlen (cmdline)-1] == '"')
			cmdline [strlen (cmdline)-1] = 0;
		if (!stricmp (ext_only (cmdline), "prj"))
			Proj_open_by_name (cmdline);
		else	{
			Open_null_project ();
			Open_by_name (cmdline, OPEN_EXISTING_BUFFER);
		}
	} else if (OpenLastProject)	{
		last = Last_project ();
		if (last && last [0])
			Proj_open_by_name (last);
		else
			Open_null_project ();
	} else
		Open_null_project ();
}

void	finish_app (void)
{
	OptEdit_cleanup ();
}

void	terminate_app (void)
{
	Reset_console ();
	Write_file_history (IniFile);
	Write_open_history (IniFile);
	Write_proj_history (IniFile);
}

void	test_demo_mode (void)
{
#if defined (DEMO_VERSION)
#elif defined (EXPIRE_DATE)
	time_t t = time (0);
	struct tm * tm = localtime (&t);
	ExpireDate = EXPIRE_DATE;
	if ((tm->tm_year+1900) * 10000 + (tm->tm_mon + 1) * 100 + tm->tm_mday < EXPIRE_DATE)
		DemoMode = 0;
	else
		HasExpired = 1;
#else
	DemoMode = 0;
#endif
}

int PASCAL WinMain (HINSTANCE hinst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	DWORD ver, ver0;
	MSG msg;
	int hiver, lover;

	MyInstance = hinst;

	Get_version_information ();
	ver0 = GetVersion ();
	hiver = LOBYTE (LOWORD (ver0));
	lover = HIBYTE (LOWORD (ver0));
	ver = (hiver << 16) + lover;
	if (ver < 0x40000) {
		MsgBox ("Sorry", MB_OK | MB_ICONEXCLAMATION,
			"This program requires WinNT 4.00 or higher, or Win95\n"
			"You only have %d.%d", hiver, lover);
		return 0;
	}

	test_demo_mode ();

	cmdshow = SW_MAXIMIZE;
	if (! InitApplication ())	{
		terminate_app ();
		return FALSE;
	}
	if (DemoMode) ShowLogo = TRUE;
#ifdef _EDUCVERSION
	ShowLogo = TRUE;
#endif
	if (ShowLogo) Display_logo ();
	if (! InitMainWindow (cmdshow))	{
		terminate_app ();
		return FALSE;
	}
	DoCmdLine (cmdline);
	if (ShowLogo) Hide_logo ();

	while (GetMessage (&msg, NULL, 0, 0)) {
		if (TranslateMyMDISysAccel (FrameWindow, &msg))
			continue;
		if (ToolAccel && TranslateAccelerator (FrameWindow, ToolAccel, &msg))
			continue;
		if (TranslateAccelerator (FrameWindow, MainAccel, &msg))
			continue;
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
	ToolBar = StatusBar = 0;
	finish_app ();
	terminate_app ();
	return msg.wParam;
}
