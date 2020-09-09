#include "util.h"
#include "fileutil.h"
#include <commctrl.h>
#include "shell.h"
#include "tools.h"
#include "make.h"
#include "classes.h"
#include "var.h"
#include "proj.h"
#include "edit.h"
#include "filter.h"
#include "message.h"
#include "toolbar.h"
#include "optedit.h"
#include "res.h"
#include "xdsres.h"

#define	CMDLINE_SIZE 1000

char * DebugCmd;

#define TOOLS_SECTION    "tools"

static char find_tool_filter[] =	"Executable files (*.exe)\0*.exe\0"
					"all files (*.*)\0*.*\0";

typedef struct	_Tool {
			char name [128];
			char category [128];
			char cmdline [CMDLINE_SIZE];
			char menu_string [256];
			char inactive_menu_string [256];
			char caption [256];
			char extensions [256];
			char comment [512];
			char filter [256];
			char startdir [CMDLINE_SIZE];

			BOOL popup_run;
			BOOL wait_ok;
			BOOL add_menu;
			BOOL add_inactive_menu;
			BOOL proj_open;
			BOOL file_open;
			int  standard;
			int  last_good_code;
			int  hotkey;
			HBITMAP bmp;
			HBITMAP dbmp;
		}
			Tool;

#define COMPILE_NAME		"Compile"
#define COMPILE_NOPROJ_NAME	"Compile_NoProj"
#define MAKE_NAME		"Make"
#define MAKE_NOPROJ_NAME	"Make_NoProj"
#define BUILD_NAME		"Build"
#define BUILD_NOPROJ_NAME	"Build_NoProj"
#define MAKELIST_NAME		"MakeList"
#define MAKELIST_NOPROJ_NAME	"MakeList_NoProj"
#define DEBUG_NAME		"Debug"

#define COMPILE_CATEGORY  "Compile"
#define MAKE_CATEGORY     "Make"
#define BUILD_CATEGORY    "Build"
#define MAKELIST_CATEGORY "MakeList"
#define DEBUG_CATEGORY    "Debug"

#define MAX_TOOLS 100

Tool *	Tools [MAX_TOOLS] = {0};
int	cur_ntools = 0;

Tool *	Tools2 [MAX_TOOLS] = {0};
int	cur_ntools2 = 0;

BOOL	Is_tool_enabled (Tool * t);

int	Find_tool (Tool ** t, int cnt, char * name)
{
	int i;
	for (i = 0; i < cnt; i++)
		if (! stricmp (t[i]->name, name))
			return i;
	return -1;
}

void	init_tool (Tool * t, char * name)
{
	memset (t, 0, sizeof (Tool));
	strcpy (t->name, name);
	strcpy (t->category, name);
	wsprintf (t -> comment, "Run custom tool %s", name);
	wsprintf (t -> menu_string, "&%s", name);
	wsprintf (t -> inactive_menu_string, "&%s", name);
	wsprintf (t -> caption, "Running tool: %s", name);

	t -> popup_run = TRUE;
	t -> wait_ok   = TRUE;
	t -> add_menu  = TRUE;
	t -> add_inactive_menu  = TRUE;
	t -> proj_open = TRUE;
	t -> file_open = FALSE;
	t -> last_good_code = 0;
}

Tool * create_tool (Tool ** tools, int * cnt, char * name, BOOL init)
{
	Tool * t;
	int i;

	i = Find_tool (tools, *cnt, name);
	if (i>=0)
		t = tools [i];
	else	{
		if (*cnt == MAX_TOOLS) return 0;
		t = tools [*cnt];
		if (!t)	{
			t = Malloc (sizeof (Tool));
			if (!t) return 0;
			tools [*cnt] = t;
		}
		++ *cnt;
		if (init)
			init_tool (t, name);
	}
	return t;
}

BOOL	copy_tools (void)
{
	int i;
	for (i = 0; i < cur_ntools; i++)	{
		if (! Tools2 [i])	{
			Tools2 [i] = Malloc (sizeof (Tool));
			if (! Tools2 [i]) return FALSE;
		}
		*Tools2 [i] = *Tools [i];
	}
	cur_ntools2 = cur_ntools;
	return TRUE;
}

void	save_changed_tools (void)
{
	int i;
	for (i = 0; i < cur_ntools2; i++)
		if (! Tools [i])	{
			Tools [i] = Tools2 [i];
			Tools2 [i] = 0;
		} else
			*Tools [i] = *Tools2 [i];
	cur_ntools = cur_ntools2;
}

Tool * add_std_tool (char * name, char * category, int n, HBITMAP bmp, HBITMAP dbmp, BOOL proj_open, BOOL file_open)
{
	Tool * t;
	char s [100];
	t = create_tool (Tools, &cur_ntools, name, FALSE);
	if (!t)	return 0;
	memset (t, 0, sizeof (Tool));
	t->proj_open = proj_open;
	t->standard = n;
	strcpy (t->name, name);
	strcpy (t->category, category);
	s[0] = 0;
	LoadString (ResInstance, IDSTR_COMMENT+n,   t->comment,     sizeof (t->comment));
	LoadString (ResInstance, IDSTR_MENU + n,    t->menu_string, sizeof (t->menu_string));
	LoadString (ResInstance, IDSTR_INV_MENU + n,t->inactive_menu_string, sizeof (t->inactive_menu_string));
	LoadString (ResInstance, IDSTR_TITLE + n,   t->caption,     sizeof (t->caption));
	LoadString (ResInstance, IDSTR_CMDLINE + n, t->cmdline,     sizeof (t->cmdline));
	LoadString (ResInstance, IDSTR_STARTDIR + n, t->startdir,   sizeof (t->startdir));
	LoadString (ResInstance, IDSTR_HOTKEY + n,  s,              sizeof (s));
	t->hotkey = 0;
	sscanf (s, "%X", &t->hotkey);
	t->file_open = file_open;
	if (file_open)
		LoadString (ResInstance, IDSTR_XC_EXTENSIONS, t->extensions, sizeof (t->extensions));
	t->add_menu = t->menu_string [0] != 0;
	t->add_inactive_menu = t->inactive_menu_string [0] != 0;
	t->wait_ok = t->popup_run = t->caption [0] != 0;
	t -> bmp  = bmp;
	t -> dbmp = dbmp;
	return t;
}

void	init_std_tools (void)
{
	Tool *t;
	static BOOL bmp_loaded = 0;
	static HBITMAP compile_bmp [2], make_bmp [2], build_bmp [2];

	if (!bmp_loaded)	{
		bmp_loaded = TRUE;
		Load_and_dither (BMP_COMPILE, &compile_bmp [0], &compile_bmp [1]);
		Load_and_dither (BMP_MAKE,    &make_bmp  [0],   &make_bmp  [1]);
		Load_and_dither (BMP_BUILD,   &build_bmp [0],   &build_bmp [1]);
	}

	t  = add_std_tool (COMPILE_NAME,	    COMPILE_CATEGORY, COMPILE, compile_bmp[0], compile_bmp[1], TRUE, TRUE);
	t->file_open = TRUE;
	LoadString (ResInstance, IDSTR_XC_EXTENSIONS, t->extensions, sizeof (t->extensions));

	add_std_tool (COMPILE_NOPROJ_NAME,  COMPILE_CATEGORY, COMPILE_NOPROJ, 0, 0, FALSE, TRUE);
	add_std_tool (MAKE_NAME,            MAKE_CATEGORY, MAKE, make_bmp [0], make_bmp [1], TRUE, FALSE);
	add_std_tool (MAKE_NOPROJ_NAME,     MAKE_CATEGORY, MAKE_NOPROJ, 0, 0, FALSE, TRUE);
	add_std_tool (BUILD_NAME,           BUILD_CATEGORY, BUILD, build_bmp [0], build_bmp [1], TRUE, FALSE);
	add_std_tool (BUILD_NOPROJ_NAME,    BUILD_CATEGORY, BUILD_NOPROJ, 0, 0, FALSE, TRUE);
	add_std_tool (MAKELIST_NAME,        MAKELIST_CATEGORY, MAKELIST, 0, 0, TRUE, FALSE);
	add_std_tool (MAKELIST_NOPROJ_NAME, MAKELIST_CATEGORY, MAKELIST_NOPROJ, 0, 0, FALSE, TRUE);
	t = add_std_tool (DEBUG_NAME,    DEBUG_CATEGORY, DEBUG, 0, 0, TRUE, FALSE);
	DebugCmd = t->cmdline;
}

Tool *	find_enabled_tool (char * category)
{
	int i;
	for (i = 0; i < cur_ntools; i++)
		if (!stricmp (Tools [i]->category, category) &&
		    Is_tool_enabled (Tools [i]))
			return Tools [i];
	return NULL;
}

void	read_opt (char * name, char * option, char * val, int len)
{
	char s [2000];
	char t [2000];
	strcpy (s, name);
	strcat (s, "-");
	strcat (s, option);
	GetPrivateProfileString (TOOLS_SECTION, s, val, t, sizeof (t), IniFile);
	strncpy (val, t, len);
	val [len-1] = 0;
}

void	read_bopt (char * name, char * option, BOOL * val)
{
	char s [10];
	s [0] = 0;
	read_opt (name, option, s, sizeof (s));
	if (!stricmp (s, "on")) *val = TRUE;
	else if (!stricmp (s, "off")) *val = FALSE;
}

BOOL	read_tool (int num)
{
	Tool * t;
	char s [100];
	char v [128];
	char c [2000];

	wsprintf (s, "tool%d", num+1);
	GetPrivateProfileString (TOOLS_SECTION, s, "", v, sizeof (v), IniFile);
	if (!v[0]) return FALSE;
	t = create_tool (Tools, &cur_ntools, v, TRUE);
	if (!t) return FALSE;
	GetPrivateProfileString (TOOLS_SECTION, v, t->cmdline, c, sizeof (c), IniFile);
	strcpy (t->cmdline, c);

	read_opt (t->name, "category",    t->category,    sizeof (t->category));
	read_opt (t->name, "menu-string", t->menu_string, sizeof (t->menu_string));
	read_opt (t->name, "inactive-menu-string", t->inactive_menu_string, sizeof (t->inactive_menu_string));
	read_opt (t->name, "comment",     t->comment,     sizeof (t->comment));
	read_opt (t->name, "caption",     t->caption,     sizeof (t->caption));
	read_opt (t->name, "extensions",  t->extensions,  sizeof (t->extensions));
	read_opt (t->name, "filter",	  t->filter,	  sizeof (t->filter));
	read_opt (t->name, "start-dir",	  t->startdir,	  sizeof (t->filter));

	read_bopt(t->name, "insert-in-menu",    &t->add_menu);
	read_bopt(t->name, "insert-in-inactive-menu", &t->add_inactive_menu);
	read_bopt(t->name, "popup",             &t->popup_run);
	read_bopt(t->name, "waitkey",           &t->wait_ok);
	read_bopt(t->name, "needs-project",     &t->proj_open);
	read_bopt(t->name, "needs-file",        &t->file_open);

	read_opt(t->name, "last-good-retcode", s, sizeof (s));
	sscanf (s, "%d", &t->last_good_code);

	read_opt(t->name, "hot-key", s, sizeof (s));
	sscanf (s, "%X", &t->hotkey);

	if (!t->category[0])
		strcpy (t->category, t->name);
	return TRUE;
}

void	Set_tools_vars (void)
{
	char s [STRLEN], x [STRLEN];

	Set_name ("xdsmain", XDSMAIN);
	Make_line (XDSMAIN, x, sizeof (x));
	change_ext (name_only (x), s, 0);
	Set_name ("xdsname", s);
	strcpy (s, x);
	dir_only (s);
	Set_name ("xdsdir", s);
	strcpy (RedFileName, name_only (x));
	change_ext (RedFileName, RedFileName, "red");
	strcpy (CfgFileName, name_only (x));
	change_ext (CfgFileName, CfgFileName, "cfg");
	strcpy (OptFileName, name_only (x));
	change_ext (OptFileName, OptFileName, "opt");
}

void	Init_tools (void)
{
	int i;
	read_ini_file (TOOLS_SECTION, "xdsmain", IDSTR_XDSMAIN, XDSMAIN, sizeof (XDSMAIN),  IniFile);
	Set_tools_vars ();
	init_std_tools ();
	if (!DemoMode)
		for (i = 0; read_tool (i); i++);
}

void	write_opt (char * name, char * option, char * val)
{
	char s [2000];
	strcpy (s, name);
	strcat (s, "-");
	strcat (s, option);
	WritePrivateProfileString (TOOLS_SECTION, s, val, IniFile);
}

#define write_bopt(name,option,b) write_opt(name,option,b?"on":"off")

void	write_tool (Tool * t)
{
	char s [2000];
	wsprintf (s, "\"%s\"", t->cmdline);
	WritePrivateProfileString (TOOLS_SECTION, t->name, s, IniFile);

	write_opt (t->name, "category", t->category);
	write_opt (t->name, "menu-string", t->menu_string);
	write_opt (t->name, "inactive-menu-string", t->inactive_menu_string);
	write_bopt(t->name, "insert-in-menu", t->add_menu);
	write_bopt(t->name, "insert-in-inactive-menu", t->add_inactive_menu);
	write_opt (t->name, "comment", t->comment);
	write_bopt(t->name, "popup", t->popup_run);
	write_bopt(t->name, "waitkey", t->wait_ok);
	write_opt (t->name, "caption", t->caption);
	write_bopt(t->name, "needs-project", t->proj_open);
	write_bopt(t->name, "needs-file", t->file_open);
	write_opt (t->name, "extensions", t->extensions);
	write_opt (t->name, "filter", t->filter);
	write_opt (t->name, "start-dir", t->startdir);
	wsprintf (s, "%d", t->last_good_code);
	write_opt(t->name, "last-good-retcode", s);
	wsprintf (s, "%04X", t->hotkey);
	write_opt (t->name, "hot-key", s);
}

void	write_tools_ini_file (void)
{
	int i;
	char s [20];
	WritePrivateProfileSection (TOOLS_SECTION, "\0", IniFile);
	WritePrivateProfileString (TOOLS_SECTION, "xdsmain", XDSMAIN, IniFile);
	for (i = 0; i < cur_ntools; i++)	{
		wsprintf (s, "tool%d", i+1);
		WritePrivateProfileString (TOOLS_SECTION, s, Tools [i]->name, IniFile);
	}

	for (i = 0; i < cur_ntools; i++)
		write_tool (Tools [i]);
}

BOOL	match_file_suffix (char * suffixes)
{
	char f [2000];
	Make_line ("$(file)", f, sizeof (f));
	if (!f[0]) return FALSE;
	return match_suffix (ext_only (f), suffixes);
}

char * hk_table [256] = {
0, "LButton", "RButton", "Cancel", "Mbutton", 0, 0, 0,	 		/* 0-7*/
"Back", "Tab", 0,0, "Clear", "Return", 0,0,		 		/* 8-F */
"Shift", "Ctrl", "Menu", "Pause", "CapsLock", 0,0,0,	 		/* 10-17 */
0,0,0,"Esc",0,0,0,0,					 		/* 18-1F */
"Space", "PageUp", "PageDn", "End", "Home", "Left", "Up", "Right",		/* 20-27 */
"Down", "Select", "Print", "Execute", "Snapshot", "Ins", "Del", "Help", /* 28-2F"*/
"0","1","2","3","4","5","6","7","8","9", 0,0,0,0,0,0,			/* 30-3F */
0,"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O",		/* 40-4F */
"P","Q","R","S","T","U","V","W","X","Y","Z", "LWin", "RWin", "Apps",0,0,/* 50-5F */
"Num0", "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7",		/* 60-67 */
"Num8", "Num9", "Num*", "Num+", "Separator", "Num-", "Num Del", "Num/",	/* 68-6F */
"F1","F2","F3","F4","F5","F6","F7","F8",				/* 70-77 */
"F9","F10","F11","F12","F13","F14","F15","F16",				/* 78-7F */
"F17","F18","F19","F20","F21","F22","F23","F24",			/* 80-87 */
0,0,0,0,0,0,0,0,
"NumLock", "ScrollLock", 0,0,0,0,0,0,0,0,0,0,0,0,0,0,			/* 90..9F */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,					/* A0..AF */
0,0,0,0,0,0,0,0,0,0,";","=",",","-",".","/",				/* B0..BF */
"`",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,					/* C0..CF */
0,0,0,0,0,0,0,0,0,0,0,"[","\\","]","'",0,				/* D0..DF */
0,0,"\\",0,0,0,0,0,0,0,0,0,0,0,0,0,					/* E0..EF */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};					/* F0..FF */

char * draw_hot_key (UINT hk)
{
	static char c [128];
	BYTE mod = HIBYTE (hk);
	hk = LOBYTE (hk);
	c [0] = 0;
	if (!hk_table [hk]) return 0;
	if (mod & HOTKEYF_CONTROL)
		strcat (c, "Ctrl-");
	if (mod & HOTKEYF_ALT)
		strcat (c, "Alt-");
	if (mod & HOTKEYF_SHIFT)
		strcat (c, "Shift-");
	strcat (c, hk_table [LOBYTE (hk)]);
	return c;
}

BOOL	Is_tool_enabled (Tool * t)
{
	if (! t -> cmdline [0]) return FALSE;
	if (!stricmp (t->category, DEBUG_CATEGORY)) return TRUE;
	if (t -> proj_open && !	Is_proj_opened ()) return FALSE;
	if (t -> file_open && !match_file_suffix (t->extensions))
		return FALSE;
	return TRUE;
}

void	Tools_enable (void)
{
	int i, j;
	BOOL state [100];
	BOOL make_enable;

	memset (state, 0, sizeof (state));

	make_enable = FALSE;
	for (i = 0; i < cur_ntools; i++)
		if (Is_tool_enabled (Tools [i]))	{
			for (j = 0; j < cur_ntools; j++)
				if (!stricmp (Tools [i]->category, Tools [j]->category))
					state [j] = TRUE;
			if (!stricmp (Tools [i]->category, MAKE_CATEGORY))
				make_enable = TRUE;
		}
	for (i = 0; i < cur_ntools; i++)
		Enable_item (IDM_FIRST_TOOL + i, state [i]);

	Enable_item (IDM_DEBUG, make_enable && find_enabled_tool (DEBUG_CATEGORY)!=NULL && RunProcess == 0);
	Enable_item (IDM_RUN, make_enable && RunProcess == 0);
}

void	Create_tools_menu (void)
{
	Tool * t;
	BOOL was_separator;
	BOOL am;
	char * m;
	int i, j;
	char s [2000];
	char * hname;
	HMENU menu;
	BYTE mod;
	ACCEL accel [200];
	int naccel = 0;

	menu = ToolsMenu;
	i = GetMenuItemCount (menu);
	while (i--)
		DeleteMenu (menu, 0, MF_BYPOSITION);
	was_separator = FALSE;
	for (i = 0; i < cur_ntools; i++)	{
		if (! Tools [i]->cmdline [0]) continue;
		if (Tools [i]->hotkey)	{
			accel [naccel].key = LOBYTE (Tools [i]->hotkey);
			accel [naccel].fVirt = FVIRTKEY;
			mod = HIBYTE (Tools [i]->hotkey);
			if (mod & HOTKEYF_SHIFT)
				accel [naccel].fVirt |= FSHIFT;
			if (mod & HOTKEYF_CONTROL)
				accel [naccel].fVirt |= FCONTROL;
			if (mod & HOTKEYF_ALT)
				accel [naccel].fVirt |= FALT;
			accel [naccel].cmd = IDM_FIRST_TOOL + i;
			++ naccel;
		}
		for (j = 0; j < i; j++)
			if (!stricmp (Tools [i]->category, Tools [j]->category))
				break;
		if (j < i) continue;
		t = Tools [i];
		for (j = i; j < cur_ntools; j++)
			if (!stricmp (Tools [i]->category, Tools [j]->category) &&
			    Is_tool_enabled (Tools [j]))
			{
				t = Tools [j];
				break;
			}
		if (Is_tool_enabled (t))	{
			am = t->add_menu;
			m  = t->menu_string;
		} else	{
			am = t->add_inactive_menu;
			m  = t->inactive_menu_string;
		} 

		if (! am || ! m [0])
			continue;

		if (! t->standard && ! was_separator)	{
			was_separator = TRUE;
			AppendMenu (menu, MF_SEPARATOR, 0, 0);
		}
		Make_line (m, s, sizeof (s)-10);
		hname = draw_hot_key (t->hotkey);
		if (hname)	{
			strcat (s, "\b");
			strcat (s, hname);
		}
		AppendMenu (menu, MF_STRING, IDM_FIRST_TOOL + i, s);
	}
	Tools_enable ();
	if (ToolAccel)	{
		DestroyAcceleratorTable (ToolAccel);
		ToolAccel = 0;
	}
	if (naccel)
		ToolAccel = CreateAcceleratorTable (accel, naccel);
}

void	Create_tools_toolbar (void)
{
	int i, a;
	BOOL space;
	Delete_toolbar_buttons (ToolBar, IDM_FIRST_TOOL, IDM_LAST_TOOL);
	a = IDM_FINDNEXT;
	space = TRUE;
	for (i = 0; i < cur_ntools; i++)
		if (Tools [i]->cmdline [0] && Tools [i] -> bmp)	{
			Add_toolbar_button (ToolBar, Tools [i] -> bmp, Tools [i] -> dbmp,
					    Tools [i] -> comment, IDM_FIRST_TOOL + i, space, a);
			space = FALSE;
			a = IDM_FIRST_TOOL + i;
		}
	show_toolbar_button (ToolBar, IDM_DEBUG, DebugCmd [0] != 0);
}

static	Tool *	CurTool;
static	int	CurSelect;
static	BOOL	tool_edited;

BOOL CALLBACK AdvancedToolDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	char s [256];
	static HWND hot;
	HWND stwnd;
	WORD hk;
	RECT rc;
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		stwnd = GetDlgItem (hwnd, ADVANCED_TOOL_HOTKEY);
		Get_child_pos (stwnd, &rc);
		rc.left ++; rc.right --;
		rc.top ++; rc.bottom --;
		hot = CreateWindow (
				HOTKEY_CLASS, NULL,
				WS_CHILD |WS_VISIBLE | WS_BORDER,
				rc.left, rc.top,
				rc.right-rc.left, rc.bottom-rc.top,
				hwnd, NULL, MyInstance,	NULL);
		SetWindowLong (hot, GWL_ID, ADVANCED_TOOL_HOTKEY);

		SendMessage (hot, HKM_SETRULES, HKCOMB_NONE/*|HKCOMB_S*/,
			     MAKELPARAM (HOTKEYF_ALT, 0));
		SendMessage (hot, HKM_SETHOTKEY, CurTool->hotkey, 0);
		CheckDlgButton (hwnd, ADVANCED_TOOL_CHECKRETURN, CurTool->last_good_code >= 0);
		if (CurTool->last_good_code >= 0)	{
			SetDlgItemInt (hwnd, ADVANCED_TOOL_MAXGOOD, CurTool->last_good_code, FALSE);
			EnableWindow (GetDlgItem (hwnd, ADVANCED_TOOL_MAXGOOD), TRUE);
		}
		CheckDlgButton (hwnd, ADVANCED_TOOL_CHECKRETURN, CurTool->last_good_code >= 0);
		SetDlgItemText (hwnd, ADVANCED_TOOL_CAPTION, CurTool -> comment);
		wsprintf (s, "Advanced configuration of %s", CurTool->name);
		SetWindowText (hwnd, s);
		SetDlgItemText (hwnd, ADVANCED_TOOL_POPUP_CAPTION, CurTool->caption);
		CheckDlgButton (hwnd, ADVANCED_TOOL_POPUP,  CurTool->popup_run);
		CheckDlgButton (hwnd, ADVANCED_TOOL_WAITOK, CurTool->popup_run && CurTool->wait_ok);
		Filter_fill_list (GetDlgItem (hwnd, ADVANCED_TOOL_FILTERS), TRUE, TRUE);
		i = Filter_search_name (CurTool->filter);
		if (i < 0)	{
			MsgBox ("Warning", MB_OK|MB_ICONEXCLAMATION,
				"Tool is configured to use message filter %s that is not available.\n"
				"Defining message filter to use as \"None\"");
			i = 0;
		}
		SendDlgItemMessage (hwnd, ADVANCED_TOOL_FILTERS, CB_SETCURSEL, i, 0);
		EnableWindow (GetDlgItem (hwnd, ADVANCED_TOOL_WAITOK), CurTool->popup_run);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			hk = LOWORD (SendMessage (hot, HKM_GETHOTKEY, 0, 0));
			for (i = 0; i < cur_ntools2; i++)
				if (hk && Tools2 [i]->hotkey == hk && Tools2[i] != CurTool )	{
					MsgBox ("Advanced tool configuration", MB_OK | MB_ICONEXCLAMATION,
						"This hot key has been assigned to another tool (%s).\nChoose another one",
						Tools2 [i]->name);
					SetFocus (hot);
					return 1;
				}

			CurTool->hotkey = hk;
			CurTool->last_good_code = IsDlgButtonChecked (hwnd, ADVANCED_TOOL_CHECKRETURN) ? 0 : -1;
			if (CurTool->last_good_code >= 0)
				CurTool->last_good_code = GetDlgItemInt (hwnd, ADVANCED_TOOL_MAXGOOD, 0, FALSE);
			GetDlgItemText (hwnd, ADVANCED_TOOL_POPUP_CAPTION, CurTool->caption, sizeof (CurTool->caption));
			CurTool->caption [sizeof (CurTool->caption)-1]     = 0;
			CurTool->popup_run = IsDlgButtonChecked (hwnd, ADVANCED_TOOL_POPUP);
			CurTool->wait_ok = IsDlgButtonChecked (hwnd, ADVANCED_TOOL_WAITOK);
			i = (int) SendDlgItemMessage (hwnd, ADVANCED_TOOL_FILTERS, CB_GETCURSEL, 0, 0);
			strncpy (CurTool->filter, Filter_get_name (i-1), sizeof (CurTool->filter));
			CurTool->filter [sizeof (CurTool->filter)-1] = 0;
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case ADVANCED_TOOL_POPUP:
			if (NOTIFY_CODE != BN_CLICKED)	break;
			EnableWindow (GetDlgItem (hwnd, ADVANCED_TOOL_WAITOK),
				      IsDlgButtonChecked (hwnd, ADVANCED_TOOL_POPUP));
			break;
		case ADVANCED_TOOL_CHECKRETURN:
			if (NOTIFY_CODE != BN_CLICKED)	break;
			if (IsDlgButtonChecked (hwnd, ADVANCED_TOOL_CHECKRETURN))	{
				SetDlgItemText (hwnd, ADVANCED_TOOL_MAXGOOD, "0");
				EnableWindow (GetDlgItem (hwnd, ADVANCED_TOOL_MAXGOOD), TRUE);
			} else	{
				SetDlgItemText (hwnd, ADVANCED_TOOL_MAXGOOD, "");
				EnableWindow (GetDlgItem (hwnd, ADVANCED_TOOL_MAXGOOD), FALSE);
			}
			break;
		case ADVANCED_TOOL_FILTERS:
			if (NOTIFY_CODE != CBN_SELCHANGE)
				return 1;
			i = (int) SendDlgItemMessage (hwnd, ADVANCED_TOOL_FILTERS, CB_GETCURSEL, 0, 0);
			EnableWindow (GetDlgItem (hwnd, ADVANCED_TOOL_EDITFILTER), i!=0);
			return 1;
		case ADVANCED_TOOL_EDITFILTER:
			i = (int) SendDlgItemMessage (hwnd, ADVANCED_TOOL_FILTERS, CB_GETCURSEL, 0, 0);
			Edit_filter (i-1);
			return 1;
		case ADVANCED_TOOL_NEWFILTER:
			i = New_filter ();
			if (i >= 0)	{
				SendDlgItemMessage (hwnd, ADVANCED_TOOL_FILTERS, CB_ADDSTRING, 0,
						    (LPARAM) Filter_get_name (i));
				SendDlgItemMessage (hwnd, ADVANCED_TOOL_FILTERS, CB_SETCURSEL, i+1, 0);
			}
			return 1;
		case IDHELP:
			return Dialog_help (ADVANCED_TOOL_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (ADVANCED_TOOL_DIALOG, lparam);
	}
	return 0;
}

void	cfgtools_advanced (HWND hwnd)
{
	Read_filters ();
	Dialog (ADVANCED_TOOL_DIALOG, AdvancedToolDialogProc);
	Clear_filters ();
}

static	int	cur_add_sel;
static	HWND	tools_dialog;

BOOL CALLBACK AddToolDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	char s [128];
	switch (msg)	{
	case WM_INITDIALOG:
		if (cur_add_sel >= 0)	{
			SetDlgItemText (hwnd, ADDTOOL_NAME,     Tools2 [cur_add_sel]->name);
			SetDlgItemText (hwnd, ADDTOOL_COMMENT,  Tools2 [cur_add_sel]->comment);
			SetDlgItemText (hwnd, ADDTOOL_CATEGORY, Tools2 [cur_add_sel]->category);
		}
		EnableWindow (GetDlgItem (hwnd, IDOK), cur_add_sel >= 0);
		Center_window (hwnd);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			GetDlgItemText (hwnd, ADDTOOL_NAME, s, sizeof (s));
			if (!s [0])	{
				MsgBox ("Add tool", MB_ICONEXCLAMATION | IDOK,
					"You must specify tool name");
				return 1;
			}

			i = Find_tool (Tools2, cur_ntools2, s);
			if (i < 0)	{
				if (! create_tool (Tools2, &cur_ntools2, s, TRUE))	{
					EndDialog (hwnd, 0);
					return 1;
				}
				i = cur_ntools2 - 1;
				SendDlgItemMessage (tools_dialog, CFGTOOLS_LIST, LB_ADDSTRING,
						    0, (LPARAM) Tools2 [i]->name);
			}
			GetDlgItemText (hwnd, ADDTOOL_COMMENT, Tools2 [i]->comment,
					sizeof (Tools2 [i]->comment));
			GetDlgItemText (hwnd, ADDTOOL_CATEGORY, Tools2 [i]->category,
					sizeof (Tools2 [i]->category));
			if (!Tools2 [i]->category [0])
				strcpy (Tools2 [i]->category, Tools2 [i]->name);
			cur_add_sel = i;
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case ADDTOOL_NAME:
			GetDlgItemText (hwnd, ADDTOOL_NAME, s, sizeof (s));
			EnableWindow (GetDlgItem (hwnd, IDOK), s[0]!=0);
			break;
		case IDHELP:
			return Dialog_help (ADDTOOL_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (ADDTOOL_DIALOG, lparam);
	}
	return 0;
}

BOOL	test_prefix (char * s, char * var)
{
	int i, c1, c2;
	char val [2000];
	char d [2000];
	Make_line (var, val, sizeof (val));
	for (i = 0; val [i]; i++)	{
		c1=toupper (s [i]);
		c2=toupper (val [i]);
		if (c1!=c2) return FALSE;
	}
	if (s [i] && s [i] != ' ' && s [i] != '\t' &&
	    s [i] != '\\' && s [i] != '/')
		return FALSE;
	strcpy (d, var);
	strcat (d, s+i);
	strcpy (s, d);
	return TRUE;
}

int	exe_filter_index = 1;

char SaveExeDir [STRLEN];

void	browse_string (HWND hwnd, int id, char * filter,
		       char * title, BOOL save_spaces, BOOL test_prefixes, BOOL test_xc)
{
	char f [2000];
	char ex [2000];
	char e [2000];
	int i;

	GetDlgItemText (hwnd, id,  f, sizeof (f));
	for (i = 0; f[i] && f[i]!= ' ' && f[i] != '\t'; i++);
	strcpy (e, f+i);
	f[i] = 0;
	Make_line (f, ex, sizeof (ex));

	if (!Browse_file_name (FALSE, title, ex, filter, ex, &exe_filter_index, SaveExeDir))
		return;

	if (test_xc)	{
		Make_line ("$(xds)", f, sizeof (f));
		if (! stricmp (f, ex))
			strcpy (ex, "$(xds)");
	}
	if (test_prefixes)	{
		test_prefix (ex, "$(homedir)");
		test_prefix (ex, "$(projdir)");
	}

	if (e[0] && save_spaces)
		strcat (ex, e);
	SetDlgItemText (hwnd, id,  ex);
}

void	check_add_par (HWND hwnd, int id, BOOL * edited, char * par)
{
	char s [2000];
	if (*edited) return;
	GetDlgItemText (hwnd, id, s, sizeof (s));
	if (!s[0]) return;
	*edited = TRUE;
	strcat (s, par);
	SetDlgItemText (hwnd, id, s);
	SendDlgItemMessage (hwnd, id, EM_SETSEL, 1, 1);
}

void	tool_dir_browse (HWND hwnd)
{
	char s [2000], d[2000];
	GetDlgItemText (hwnd, CFGTOOLS_DIR, s, sizeof (s));
	Make_line (s, d, sizeof (d));
	if (!Browse_for_directory ("Select directory to run tool in", d, s))
		return;
	test_prefix (s, "$(homedir)");
	test_prefix (s, "$(projdir)");
	test_prefix (s, "$(filedir)");
	SetDlgItemText (hwnd, CFGTOOLS_DIR, s);
}

BOOL	tool_sel_change (HWND hwnd, int i)
{
	BOOL disable_all;
	if (i == CurSelect) return TRUE;
	if (CurSelect >= 0)	{
		GetDlgItemText (hwnd, CFGTOOLS_CMDLINE, CurTool->cmdline, CMDLINE_SIZE);
		GetDlgItemText (hwnd, CFGTOOLS_MENUITEM, CurTool->menu_string, sizeof (CurTool->menu_string));
		GetDlgItemText (hwnd, CFGTOOLS_INACTIVE_MENUITEM, CurTool->inactive_menu_string, sizeof (CurTool->inactive_menu_string));
		GetDlgItemText (hwnd, CFGTOOLS_SUFFIX, CurTool->extensions, sizeof (CurTool->extensions));
		GetDlgItemText (hwnd, CFGTOOLS_DIR, CurTool->startdir, sizeof (CurTool->startdir));
		CurTool->cmdline     [sizeof (CurTool->cmdline)-1]     = 0;
		CurTool->menu_string [sizeof (CurTool->menu_string)-1] = 0;
		CurTool->inactive_menu_string [sizeof (CurTool->inactive_menu_string)-1] = 0;
		CurTool->extensions  [sizeof (CurTool->extensions)-1]  = 0;
		CurTool->add_menu  = IsDlgButtonChecked (hwnd, CFGTOOLS_MENU);
		CurTool->add_inactive_menu  = IsDlgButtonChecked (hwnd, CFGTOOLS_INACTIVE_MENU);
		CurTool->file_open = IsDlgButtonChecked (hwnd, CFGTOOLS_RUNFILE);
		CurTool->proj_open = IsDlgButtonChecked (hwnd, CFGTOOLS_RUNPROJECT);
	}
	CurSelect = i;
	if (i<0)	{
		CurTool = 0;
		return TRUE;
	}
	CurTool = Tools2 [i];
	disable_all = ! stricmp (CurTool->category, DEBUG_CATEGORY);
	tool_edited = CurTool->cmdline [0] != 0;
	SetDlgItemText (hwnd, CFGTOOLS_COMMENT,  CurTool->comment);
	SetDlgItemText (hwnd, CFGTOOLS_CMDLINE,  CurTool->cmdline);
	SetDlgItemText (hwnd, CFGTOOLS_MENUITEM, CurTool->menu_string);
	SetDlgItemText (hwnd, CFGTOOLS_INACTIVE_MENUITEM, CurTool->inactive_menu_string);
	SetDlgItemText (hwnd, CFGTOOLS_SUFFIX, CurTool->extensions);
	SetDlgItemText (hwnd, CFGTOOLS_CATEGORY, CurTool->category);
	SetDlgItemText (hwnd, CFGTOOLS_DIR, CurTool->startdir);
	CheckDlgButton (hwnd, CFGTOOLS_MENU, CurTool->add_menu);
	CheckDlgButton (hwnd, CFGTOOLS_INACTIVE_MENU, CurTool->add_inactive_menu);
	CheckDlgButton (hwnd, CFGTOOLS_RUNPROJECT, CurTool->proj_open);
	CheckDlgButton (hwnd, CFGTOOLS_RUNFILE, CurTool->file_open);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_MENU), !disable_all);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_INACTIVE_MENU), !disable_all);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_MENUITEM), !disable_all && CurTool -> add_menu);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_INACTIVE_MENUITEM), !disable_all && CurTool -> add_inactive_menu);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_SUFFIX),   !disable_all && CurTool -> file_open);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_ADVANCED),    !disable_all);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_RUNPROJECT),  !disable_all);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_RUNFILE),     !disable_all);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_REMOVE),      !CurTool->standard);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_DEFAULT),     CurTool->standard);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_DIR),         !disable_all);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_DIRBROWSE),   !disable_all);
	EnableWindow (GetDlgItem (hwnd, CFGTOOLS_DEFAULTDIR),  CurTool->standard && !disable_all);
	return TRUE;
}

void	add_tool (HWND hwnd)
{
	cur_add_sel = CurSelect;
	tools_dialog = hwnd;
	if (!Dialog (ADDTOOL_DIALOG, AddToolDialogProc))
		return;
	SendDlgItemMessage (hwnd, CFGTOOLS_LIST, LB_SETCURSEL, cur_add_sel, 0);
	tool_sel_change (hwnd, cur_add_sel);
	SetDlgItemText (hwnd, CFGTOOLS_COMMENT,  CurTool->comment);
}

void	remove_tool (HWND hwnd)
{
	Tool * t = Tools2 [CurSelect];
	int i, n;

	n = CurSelect;
	CurSelect = -1;
	CurTool = 0;
	SendDlgItemMessage (hwnd, CFGTOOLS_LIST, LB_DELETESTRING, n, 0);
	for (i = n; i < cur_ntools2-1; i++)
		Tools2 [i] = Tools2 [i+1];
	Tools2 [i] = t;
	-- cur_ntools2;
	if (n >= cur_ntools2) n --;
	SendDlgItemMessage (hwnd, CFGTOOLS_LIST, LB_SETCURSEL, n, 0);
	tool_sel_change (hwnd, n);
}

int	LastSelected = 0;

BOOL CALLBACK CfgToolsDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char s [2000];
	int i;
	switch (msg)	{
	case WM_INITDIALOG:
		CurTool = 0;
		CurSelect = -1;
		for (i = 0; i < cur_ntools2; i++)
			SendDlgItemMessage (hwnd, CFGTOOLS_LIST, LB_ADDSTRING, 0, (LPARAM) Tools2 [i]->name);
		if (LastSelected >= cur_ntools)
			LastSelected = 0;
		SendDlgItemMessage (hwnd, CFGTOOLS_LIST, LB_SETCURSEL, LastSelected, 0);
		tool_sel_change (hwnd, LastSelected);
		SetDlgItemText (hwnd, CFGTOOLS_XDSMAIN, XDSMAIN);
		GetDlgItemText (hwnd, CFGTOOLS_INIFILE, s, sizeof (s));
		strcat (s, IniFile);
		SetDlgItemText (hwnd, CFGTOOLS_INIFILE, s);
		Center_window (hwnd);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			LastSelected = CurSelect;
			GetDlgItemText (hwnd, CFGTOOLS_XDSMAIN, XDSMAIN, sizeof (XDSMAIN));
			if (tool_sel_change (hwnd, -1))
				EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			LastSelected = CurSelect;
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (CFGTOOLS_DIALOG);
		case CFGTOOLS_LIST:
			if (NOTIFY_CODE == LBN_SELCHANGE)	{
				i = SendDlgItemMessage (hwnd, CFGTOOLS_LIST, LB_GETCURSEL, 0, 0);
				if (i < 0 || i > cur_ntools2) break;
				if (!tool_sel_change (hwnd, i))
					SendDlgItemMessage (hwnd, CFGTOOLS_LIST, LB_SETCURSEL, CurSelect, 0);
			} else if (NOTIFY_CODE == LBN_DBLCLK)
				add_tool (hwnd);
			break;
		case CFGTOOLS_BROWSE:
			wsprintf (s, "Select \"%s\" executable file", CurTool->name);
			browse_string (hwnd, CFGTOOLS_CMDLINE, find_tool_filter, s, TRUE, TRUE, TRUE);
			break;
		case CFGTOOLS_XDSMAIN_BROWSE:
			browse_string (hwnd, CFGTOOLS_XDSMAIN, find_tool_filter,
				     "Select main XDS executable file", TRUE, FALSE, FALSE);
			GetDlgItemText (hwnd, CFGTOOLS_XDSMAIN, s, sizeof (s));
			Set_name ("xdsmain", s);
			break;
		case CFGTOOLS_XDSMAIN:
			if (NOTIFY_CODE != EN_CHANGE) break;
			GetDlgItemText (hwnd, CFGTOOLS_XDSMAIN, s, sizeof (s));
			Set_name ("xdsmain", s);
			break;
		case CFGTOOLS_CMDLINE:
			if (NOTIFY_CODE != EN_CHANGE) break;
			if (CurTool -> file_open)
				check_add_par  (hwnd, CFGTOOLS_CMDLINE, &tool_edited,
						" $(file)");
			else if (CurTool -> proj_open)
				check_add_par  (hwnd, CFGTOOLS_CMDLINE, &tool_edited,
				" $(project)");
			break;
		case CFGTOOLS_MENU:
			if (NOTIFY_CODE != BN_CLICKED) break;
			EnableWindow (GetDlgItem (hwnd, CFGTOOLS_MENUITEM),
				      IsDlgButtonChecked (hwnd, CFGTOOLS_MENU));
			break;
		case CFGTOOLS_RUNFILE:
			if (NOTIFY_CODE != BN_CLICKED) break;
			EnableWindow (GetDlgItem (hwnd, CFGTOOLS_SUFFIX),
				      IsDlgButtonChecked (hwnd, CFGTOOLS_RUNFILE));
			break;
		case CFGTOOLS_ADD:
			add_tool (hwnd);
			break;
		case CFGTOOLS_REMOVE:
			remove_tool (hwnd);
			break;
		case CFGTOOLS_ADVANCED:
			cfgtools_advanced (hwnd);
			break;
		case CFGTOOLS_DEFAULT:
			if (! CurTool) break;
			LoadString (ResInstance, IDSTR_CMDLINE + CurTool->standard,
				    CurTool->cmdline, sizeof (CurTool->cmdline));
			SetDlgItemText (hwnd, CFGTOOLS_CMDLINE, CurTool->cmdline);
			break;
		case CFGTOOLS_DEFAULTDIR:
			if (! CurTool) break;
			LoadString (ResInstance, IDSTR_STARTDIR + CurTool->standard,
				    CurTool->startdir, sizeof (CurTool->startdir));
			SetDlgItemText (hwnd, CFGTOOLS_DIR, CurTool->startdir);
			break;
		case CFGTOOLS_DIRBROWSE:
			tool_dir_browse (hwnd);
			break;
		}
		return 1;
	case WM_HELP:
		return Control_help (CFGTOOLS_DIALOG, lparam);
	}
	return 0;
}

void	Configure_tools (void)
{
	BOOL b;
	Init_tools ();
	Create_tools_menu ();

	if (!copy_tools ())	{
		MsgBox ("Configure Tools", MB_OK | MB_ICONSTOP, "Sorry, no enough memory for operation");
		return;
	}
	b = Dialog (CFGTOOLS_DIALOG, CfgToolsDialogProc);
	Set_tools_vars ();
	if (!b) return;
	need_reload_options = TRUE;
	if (!DemoMode)	{
		save_changed_tools ();
		write_tools_ini_file ();
		Create_tools_menu ();
		Create_tools_toolbar ();
		Tools_enable ();
	}
}

int * projects_made = NULL;
FILETIME * project_times = NULL;
int nprojects = 0;

void	store_last_make_time (void)
{
	FILETIME t;
	int i;
	void * p;

	if (ProjFile <= 0) return;
	if (! Get_file_time (filedir [ProjFile], &t))
		return;
	for (i = 0; i < nprojects; i++)
		if (projects_made[i]==ProjFile) break;
	if (i == nprojects)	{
		p = realloc (projects_made, (nprojects+1) * sizeof (int));
		if (!p) return;
		projects_made = p;
		p = realloc (project_times, (nprojects+1) * sizeof (FILETIME));
		if (!p) return;
		project_times = p;
		++ nprojects;
		projects_made [i] = ProjFile;
	}
	project_times [i] = t;
}

Tool * test_build_all (Tool * tool)
{
	int i, r;
	FILETIME t;

	if (stricmp (tool->category, MAKE_CATEGORY)) return tool;
	if (ProjFile <= 0) return tool;
	for (i = 0; i < nprojects; i++)
		if (projects_made[i]==ProjFile) break;
	if (i == nprojects)	{
		r = MsgBox ("Make Project", MB_YESNOCANCEL | MB_ICONEXCLAMATION,
			"This is the first time project %s\n"
			"is being made during this session.\n"
			"Would you like to rebuild all the files?",
			filedir [ProjFile]);
		if (r == IDCANCEL) return NULL;
		if (r == IDNO)	{
			store_last_make_time ();
			return tool;
		}
	} else	{
		if (! Get_file_time (filedir [ProjFile], &t))
			return tool;
		if (t.dwLowDateTime  == project_times [i].dwLowDateTime &&
		    t.dwHighDateTime == project_times [i].dwHighDateTime)
			return tool;
		r = MsgBox ("Make Project", MB_YESNOCANCEL | MB_ICONEXCLAMATION,
			"Project file %s has been changed\n"
			"since last time it had been successfully made.\n"
			"Would you like to rebuild all the files?",
			filedir [ProjFile]);
		if (r == IDCANCEL) return NULL;
		if (r == IDNO)	{
			store_last_make_time ();
			return tool;
		}
	}
	tool = find_enabled_tool (BUILD_CATEGORY);
	if (!tool)
		MsgBox ("Make Project", MB_OK | MB_ICONSTOP,
			"Sorry, build tool is not properly defined");
	return tool;
}

BOOL	RunTool (Tool * tool, BOOL wait)
{
	BOOL show_list = FALSE;
	if (!tool) return FALSE;
	tool = find_enabled_tool (tool->category);
	if (!tool) return FALSE;
	if (!strcmp (tool -> category, MAKELIST_CATEGORY))	{
		wait = FALSE;
		show_list = TRUE;
	}
	if (! Edit_save_all (TRUE)) return FALSE;
	tool = test_build_all (tool);
	if (!tool) return FALSE;
	if (!read_tool_filter (tool->filter))
		return FALSE;
	if (!compile_tool_filter ())
		return FALSE;
	CallTool (tool->cmdline, tool->startdir, tool->caption, tool -> popup_run,
		  wait && tool->wait_ok, show_list, tool -> last_good_code);
	delete_tool_filter();
	if (err_count+severe_count == 0 &&
	    (!stricmp (tool->category, MAKE_CATEGORY) ||
	     !stricmp (tool->category, BUILD_CATEGORY)))
		store_last_make_time ();
	Edit_test_reload (TRUE);
	return TRUE;
}

BOOL	Tool_make (BOOL wait)
{
	Tool * t = find_enabled_tool (MAKE_CATEGORY);
	if (!t) return FALSE;
	return RunTool (t, wait) && err_count+severe_count == 0;
}

BOOL	Call_tool (int i)
{
	return RunTool (Tools [i], TRUE);
}

char *	Tool_get_comment (int i)
{
	if (i < 0 || i >= cur_ntools) return "";
	return Tools [i]->comment;
}

void	Update_tools_for_file (char * s)
{
	File_set_vars (s);
	Create_tools_menu ();
	Tools_enable ();
}
