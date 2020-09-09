#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "res.h"
#include "options.h"
#include "classes.h"
#include "parse.h"
#include "optedit.h"
#include "open.h"
#include "config.h"

WNDPROC	StdLBProc;
WNDPROC StdEditBoxProc, StdComboBoxProc, StdCheckBoxProc;

HWND	OptionsDlg;
HWND	OptionsListBox;
HWND	OptionsWindow;
HWND	OptionsFrame;
HWND	DefPaneWindow;
RECT	OptRect;
HFONT	OptFont;
int	OptFontHeight;

HWND PackagesDlg;
HWND UserOptDlg;
HWND EquDlg;
HWND ModuleDlg;

BOOL	ModifiedSpecial;

HBITMAP BitMapOpen = 0;
HBITMAP BitMapClose = 0;
HBITMAP BitMapItem = 0;
BITMAP  BMInfo;
int	base_dy;
int	base_dx;
int	OptionItemHeight;
int	nitems;
Node *	CurNode;
int	LastListSel = 0;

HFONT	BigFont   [2][2] = {{0,0},{0,0}};
HFONT   SmallFont [2][2] = {{0,0},{0,0}};

#define EDITBOX_DELTA  8
#define CHECKBOX_DELTA 4
#define COMBOBOX_DELTA 8
#define DX 10

HWND	mstatic = 0;

struct	{ Option * opt; HWND present, item; } items [30];

BOOL	need_reload_options = FALSE;

HFONT MakeFont (int big, int bold, int italic)
{
	HFONT * f = (big ? BigFont : SmallFont) [bold!=0] + (italic!=0);
	if (*f) return *f;
	*f = CreateFont (big ? OptFontHeight * 2 : OptFontHeight,
			 0,0,0,bold?FW_BOLD:0,italic,0,0,0,0,0,PROOF_QUALITY,0,
			 "MS Sans Serif");
	return *f;
}

void	DrawDefault (Option * opt)
{
	char s [200];
	char * d;
	if (!opt)	{
		SetWindowText (DefPaneWindow, "");
		return;
	}
	switch (opt -> type)	{
	case Equation:	  d = opt -> defstring;		     break;
	case Enumeration: d = opt -> values [opt -> defval]; break;
	case Boolean:	  d = opt -> defval ? "on" : "off";  break;
	default: SetWindowText (DefPaneWindow, ""); return;
	}
	wsprintf (s, "%s: default=%s", opt -> name, d);
	SetWindowText (DefPaneWindow, s);
}

Option * CurrMouseOpt = NULL;

Option * find_option (HWND option)
{
	Option * opt;
	int i = 0;

	opt = NULL;
	for (i = 0; i < nitems; i++)
		if (items [i].item == option)	{
			opt = items [i].opt;
			break;
		}
	return opt;
}

BOOL	is_present (HWND w)
{
	int i = 0;

	for (i = 0; i < nitems; i++)
		if (items [i].present == w)
			return TRUE;
	return FALSE;
}

void	TimerProc (void)
{
	POINT p;
	HWND hwnd;
	Option * opt;

	GetCursorPos (&p);
	hwnd = WindowFromPoint (p);
	opt = find_option (hwnd);
	if (opt == CurrMouseOpt) return;
	DrawDefault (CurrMouseOpt = opt);
}

BOOL	Option_help (HWND hwnd, LPARAM lparam)
{
	HELPINFO * h = (HELPINFO*) lparam;
	Option * opt;

	if (is_present (h->hItemHandle))	{
		Control_id_help (OPT_DIALOG, OPT_PRESENT);
		return TRUE;
	}

	opt = find_option (h->hItemHandle);
	if (!opt) return FALSE;
	if (Find_help_file ())
		WinHelp (FrameWindow, HelpFile, HELP_PARTIALKEY, (DWORD) opt->name);
	return TRUE;
}

LRESULT	CALLBACK NumberEditBoxProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_CHAR && !
	    ( wparam >= '0' && wparam <= '9' || wparam < ' '))
		return 0;
	return CallWindowProc (StdEditBoxProc, hwnd, msg, wparam, lparam);
}

LRESULT	CALLBACK HexEditBoxProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_CHAR)	{
		wparam = toupper (wparam);
		if (! (wparam >= '0' && wparam <= '9' ||
		       wparam >= 'A' && wparam <= 'F' ||
		       wparam < ' '
		   ))
			return 0;
	}
	return CallWindowProc (StdEditBoxProc, hwnd, msg, wparam, lparam);
}

Node *	GetNodeInBox (int pos)
{
	LPARAM l = SendMessage (OptionsListBox, LB_GETITEMDATA, pos, 0);
	if (l == LB_ERR) return NULL;
	return (Node *) l;
}

void	CalcFrames (void)
{
	Get_child_pos (OptionsFrame, &OptRect);
	OptRect.top ++;
	OptRect.bottom --;
	OptRect.left ++;
	OptRect.right --;
}

void	CalcCheckBox (char * text, RECT * rect)
{
	rect->left = rect->top = 0;
	rect->right = text_width (NULL, text) + base_dx * 4;
	rect->bottom = base_dy + CHECKBOX_DELTA;
}

void	CalcStatic (char * text, HFONT font, RECT * rect)
{
	int w, h;
	rect->left = rect->top = 0;
	text_size (font, text, &w, &h);
	rect->right = w;
	rect->bottom = h;
}

HWND	CreateControl (char * class, char * text, UINT style, UINT exstyle, RECT * rect)
{
	HWND w = CreateWindowEx (exstyle,
				class,
				text,
				WS_CHILD | style,
				rect -> left, rect -> top,
				rect -> right - rect -> left,
				rect -> bottom - rect -> top,
				OptionsWindow,
				NULL,
				MyInstance,
				NULL);

	SendMessage (w, WM_SETFONT, (WPARAM) OptFont, 0);
	return w;
}

HWND	CreateCheckBox (char * text, unsigned long style, RECT * rect)
{
	return CreateControl ("BUTTON", text, style | WS_TABSTOP | WS_GROUP, 0, rect);
}

HWND	CreateEditBox (RECT * rect)
{
	return CreateControl ("EDIT", "",
				WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | WS_GROUP,
				WS_EX_CLIENTEDGE, rect);
}

HWND	CreateComboBox (RECT * rect, int nalts)
{
	RECT rc;
	int dy;
	rc = *rect;
	if (nalts < 2) nalts = 2;
	dy = (nalts * OptFontHeight) + base_dy + COMBOBOX_DELTA * 2;
	if (dy > rc.right - rc.left)
		dy = rc.right - rc.left;
	rc.bottom = rc.top + dy;
	return CreateControl ("COMBOBOX", "",
				WS_TABSTOP | CBS_HASSTRINGS | CBS_DROPDOWNLIST | WS_GROUP | WS_VSCROLL,
				WS_EX_CLIENTEDGE, &rc);
}


void	align_rect (RECT * base, RECT * rc, int y, BOOL right)
{
	int dy = rc -> bottom - rc -> top;
	int w = rc->right - rc->left;
	rc -> top = y;
	rc -> bottom = y + dy;
	if (right)	{
		rc -> left  = base->right - base_dx - w;
		rc -> right = base->right - base_dx;
	} else	{
		rc -> left  = base->left + base_dx;
		rc -> right = base->left + base_dx + w;
	}
	if (rc->left < base->left + DX) rc->left = base->left + DX;
	if (rc->right > base->right - DX) rc->right = base->right - DX;
	if (rc->bottom > base->bottom - DX) rc->bottom = base->bottom - DX;
}

void	align_two_rects_y (RECT * r1, RECT * r2)
{
	int dy1 = r1 -> bottom - r1 -> top;
	int dy2 = r2 -> bottom - r2 -> top;
	dy1 = (dy1 - dy2) / 2;
	if (dy1 > 0)	{
		r2 -> top += dy1;
		r2 -> bottom += dy1;
	} else	{
		r1 -> top += dy1;
		r1 -> bottom += dy1;
	}
}

BOOL	manual_set_text = FALSE;

void	SetControlData (HWND hwnd, HWND present, Option * o)
{
	char * s;
	int pr = o -> present;
	switch (o -> type)	{
	case Boolean:
		SendMessage (hwnd, BM_SETCHECK, pr ? (o -> curval != 0) : 2, 0);
		break;
	case Equation:
		s = pr ? o -> curstring : o -> defstring;
		if (!s) s = "";
		manual_set_text = TRUE;
		SendMessage (hwnd, WM_SETTEXT, 0, (LPARAM) (LPCSTR) s);
		manual_set_text = FALSE;
		SendMessage (present, BM_SETCHECK, pr, 0);
		UpdateWindow (present);
		break;
	case Enumeration:
		SendMessage (hwnd, CB_SETCURSEL, pr ? o -> curval : o -> defval, 0);
		SendMessage (present, BM_SETCHECK, pr, 0);
		UpdateWindow (present);
		break;
	}
	UpdateWindow (hwnd);
}

void	UpdatePresent (int i, UINT state)
{
	items [i].opt -> present = state;
	if (!items [i].present) return;
	SendMessage (items [i].present, BM_SETCHECK, state, 0);
	UpdateWindow (items [i].present);
}

void	setwinpos (int id0, int id, int y0, BOOL show)
{
	RECT rc0, rc;
	Get_child_pos (GetDlgItem (OptionsWindow, id0), &rc0);
	Get_child_pos (GetDlgItem (OptionsWindow, id), &rc);
	rc.top = rc.top - rc0.top + y0;

	SetWindowPos (GetDlgItem (OptionsWindow, id), 0, rc.left, rc.top, 0, 0,
		      SWP_NOZORDER | SWP_NOSIZE |
		     (show ? SWP_SHOWWINDOW : (SWP_HIDEWINDOW | SWP_NOREDRAW)));
}

/* ----------------------------------------------------------- */

#ifndef ListView_SetCheckState
   #define ListView_SetCheckState(hwndLV, i, fCheck) \
      ListView_SetItemState(hwndLV, i, \
      INDEXTOSTATEIMAGEMASK((fCheck)+1), LVIS_STATEIMAGEMASK)
#endif

char * get_lv_text (HWND hwnd, int id, int i, int j)
{
	static char s [STRLEN];

	LV_ITEM item;
	HWND list = GetDlgItem (hwnd, id);
	memset (&item, 0, sizeof (item));
	item.mask = LVIF_TEXT;
	item.iItem = i;
	item.iSubItem = j;
	item.pszText = s;
	item.cchTextMax = sizeof (s);
	ListView_GetItem (list, &item);
	return s;
}

void	set_lv_text (HWND hwnd, int i, int subitem, char * text)
{
	LV_ITEM item;
	
	memset (&item, 0, sizeof (item));
	item.mask = LVIF_TEXT;
	item.iItem = i;
	item.iSubItem = subitem;
	item.pszText = text;
	ListView_SetItem (hwnd, &item);
}

void	set_lv_color (HWND list)
{
	if (GetSysColor (COLOR_WINDOW) == GetSysColor (COLOR_3DFACE)) {
		ListView_SetBkColor (list, GetSysColor (COLOR_3DHILIGHT));
		ListView_SetTextBkColor (list, GetSysColor (COLOR_3DHILIGHT));
	}
}

/* ----- Module list editing --------------------------------- */

int module_selected;

void	module_change_selected (void)
{
	int i = ListView_GetNextItem (GetDlgItem (ModuleDlg, MODULE_LIST), -1, LVNI_FOCUSED);
	if (i == module_selected) return;
	module_selected = i;
	EnableWindow (GetDlgItem (ModuleDlg, MODULE_REMOVE), i>=0);
	EnableWindow (GetDlgItem (ModuleDlg, MODULE_EDIT), i>=0);
	EnableWindow (GetDlgItem (ModuleDlg, MODULE_BROWSE), i>=0);
}

void	module_browse (void)
{
	char name [STRLEN];
	char * s;

	if (module_selected < 0) return;
	
	s = get_lv_text (ModuleDlg, MODULE_LIST, module_selected, 0);
	strcpy (name, s);

	if (Browse_file_name (FALSE, "Select module file name", name,
			  open_filter, name, &edit_filter_index, EditCurDir))
	{
		if (Edit_proj_module (EditProj, module_selected, name)) {
			set_lv_text (GetDlgItem (ModuleDlg, MODULE_LIST), module_selected, 0, name);
			ModifiedSpecial = TRUE;
		}
	}
}

void	add_module_line (char * name)
{
	LV_ITEM item;
	HWND list = GetDlgItem (ModuleDlg, MODULE_LIST);

	memset (&item, 0, sizeof (item));
	item.mask = LVIF_TEXT;
	item.iItem = ListView_GetItemCount (list);
	item.iSubItem = 0;
	item.state = 0;
	item.pszText = name;
	ListView_InsertItem (list, &item);
}

BOOL	add_module_to_list (char * name)
{
	add_module_line (name);
	return TRUE;
}


BOOL CALLBACK ModuleDlgProc (HWND hwnd, UINT msg,
			     WPARAM wparam, LPARAM lparam)
{
	HWND list;
	LV_COLUMN lvc;
	LPNMHDR nm;
	BOOL b;
	int i;
	LV_DISPINFO * lvd;
	NM_LISTVIEW * nml;
	RECT rect;

	list = GetDlgItem (hwnd, MODULE_LIST);
	
	switch (msg) {
	case WM_INITDIALOG:
		ModuleDlg = hwnd;
		set_lv_color (list);
		SendMessage (list, LVM_SETEXTENDEDLISTVIEWSTYLE,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
		);
		SetWindowLong (list, GWL_STYLE, GetWindowLong (list, GWL_STYLE) | LVS_SHOWSELALWAYS);
		GetClientRect (list, &rect);
		memset (&lvc, 0, sizeof (lvc));
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.fmt = LVCFMT_LEFT; 
		lvc.cx = rect.right - rect.left;
		lvc.iSubItem = 0;
		lvc.pszText = "Module name";
		ListView_InsertColumn (list, 0, &lvc);
		Enum_proj_modules (add_module_to_list);
		module_selected = -2;
		ListView_SetItemState (list, 0, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		ListView_EnsureVisible (list, 0, FALSE);
		module_change_selected ();
		return 0;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case MODULE_ADD:
			i = ListView_GetItemCount (list);
			b = Add_proj_module (EditProj, "");
			if (b) {
				add_module_line ("");
				ListView_SetItemState (list, i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				ListView_EnsureVisible (list, i, FALSE);
				SetFocus (list);
				ListView_EditLabel (list, i);
			}
			ModifiedSpecial = TRUE;
			return 1;
		case MODULE_EDIT:
			if (module_selected >= 0) {
				SetFocus (list);
				ListView_EditLabel (list, module_selected);
			}
			return 1;
		case MODULE_REMOVE:
			if (module_selected < 0)
				return TRUE;
			i = module_selected;
			Delete_proj_module (EditProj, i);
			ListView_DeleteItem (list, i);
			module_change_selected ();
			ModifiedSpecial = TRUE;
			if (i >= ListView_GetItemCount (list))
				--i;
			if (i >= 0) {
				ListView_SetItemState (list, i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				ListView_EnsureVisible (list, i, FALSE);
			}
			return TRUE;
		case MODULE_BROWSE:
			module_browse ();
			return 1;
		}
		return 0;
	case WM_NOTIFY:
		nm = (LPNMHDR) lparam;
		if (nm -> idFrom != MODULE_LIST)
			return 0;
		switch (nm -> code) {
		case LVN_ITEMCHANGED:
			nml = (NM_LISTVIEW*) nm;
			if ((nml -> uChanged & LVIF_STATE) == 0)
				return FALSE;
			module_change_selected ();
			return TRUE;
		case LVN_ENDLABELEDIT:
			lvd = (LV_DISPINFO*) nm;
			if (!lvd -> item.pszText) return TRUE;
			if (Edit_proj_module (EditProj, lvd -> item.iItem, lvd -> item.pszText))
				set_lv_text (list, lvd->item.iItem, 0, lvd -> item.pszText);
			ModifiedSpecial = TRUE;
			return TRUE;
		}
		return FALSE;

	}
	return 0;
}

/* ----- User options editing --------------------------------- */

int useropt_selected;

void	useropt_change_selected ()
{
	int i = ListView_GetNextItem (GetDlgItem (UserOptDlg, USEROPT_LIST), -1, LVNI_FOCUSED);
	if (i == useropt_selected) return;
	useropt_selected = i;
	EnableWindow (GetDlgItem (UserOptDlg, USEROPT_DELETE), i >= 0);
}

void	set_option_value_text (int i, BOOL b)
{
	LV_ITEM item;
	HWND list = GetDlgItem (UserOptDlg, USEROPT_LIST);
	memset (&item, 0, sizeof (item));
	item.mask = LVIF_TEXT;
	item.iItem = i;
	item.iSubItem = 1;
	item.pszText = b ? "True" : "False";
	ListView_SetItem (list, &item);
}

BOOL	get_user_option_value (int i)
{
	return Get_option (EditProj, get_lv_text (UserOptDlg, USEROPT_LIST, i, 0));
}

void	remove_user_option (int i)
{
	Delete_option (EditProj, get_lv_text (UserOptDlg, USEROPT_LIST, i, 0));
	ModifiedSpecial = TRUE;
}

void	set_user_option_value (int i, BOOL value)
{
	Set_user_option (EditProj, get_lv_text (UserOptDlg, USEROPT_LIST, i, 0), value);
	ModifiedSpecial = TRUE;
}

void	flip_user_option_value (int i)
{
	BOOL b = !get_user_option_value (i);
	set_user_option_value (i, b);
	set_option_value_text (i, b);
	ListView_SetCheckState (GetDlgItem (UserOptDlg, USEROPT_LIST), i, b);
}

void	add_user_option_line (char * name, BOOL value)
{
	LV_ITEM item;
	HWND list = GetDlgItem (UserOptDlg, USEROPT_LIST);

	memset (&item, 0, sizeof (item));
	item.mask = LVIF_TEXT;
	item.iItem = ListView_GetItemCount (list);
	item.iSubItem = 0;
	item.state = 0;
	item.pszText = name;
	ListView_InsertItem (list, &item);
	set_option_value_text (item.iItem, value);
	ListView_SetCheckState (list, item.iItem, value);
}

BOOL	add_user_option_to_list (char * name)
{
	add_user_option_line (name, Get_option (EditProj, name));
	return TRUE;
}

BOOL CALLBACK UserOptDlgProc (HWND hwnd, UINT msg,
			       WPARAM wparam, LPARAM lparam)
{
	char s [STRLEN];
	HWND list;
	LV_COLUMN lvc;
	RECT rect;
	LPNMHDR nm;
	NM_LISTVIEW * nml;
	int i;
	BOOL b;
	static BOOL detect_check_change = FALSE;

	list = GetDlgItem (hwnd, USEROPT_LIST);

	switch (msg) {
	case WM_INITDIALOG:
		UserOptDlg = hwnd;
		set_lv_color (list);
		SendMessage (list, LVM_SETEXTENDEDLISTVIEWSTYLE,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES
		);
		SetWindowLong (list, GWL_STYLE, GetWindowLong (list, GWL_STYLE) | LVS_SHOWSELALWAYS);
		detect_check_change = FALSE;
		GetClientRect (list, &rect);
		memset (&lvc, 0, sizeof (lvc));
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.fmt = LVCFMT_LEFT; 
		lvc.cx = (rect.right - rect.left) * 2 / 3;
		lvc.iSubItem = 0;
		lvc.pszText = "Option";
		ListView_InsertColumn (list, 0, &lvc);
		lvc.cx = rect.right - rect.left - lvc.cx;
		lvc.iSubItem = 1;
		lvc.pszText = "Value";
		ListView_InsertColumn (list, 1, &lvc);
		Enum_user_options (add_user_option_to_list);
		useropt_selected = -2;
		ListView_SetItemState (list, 0, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		ListView_EnsureVisible (list, 0, FALSE);
		useropt_change_selected ();
		detect_check_change = TRUE;
		return 0;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case USEROPT_ADD:
			i = ListView_GetItemCount (list);
			b = Ask_text ("Type a name for new option", "New option", s, sizeof (s), 0);
			if (!b || !s[0]) return TRUE;
			if (Test_option_presence (EditProj, s))	{
				MsgBox ("Add option", MB_OK | MB_ICONEXCLAMATION, "Option or equation %s already exists", s);
				return TRUE;
			}
			Set_user_option (EditProj, s, TRUE);
			detect_check_change = FALSE;
			add_user_option_line (s, FALSE);
			detect_check_change = TRUE;
			ListView_SetItemState (list, i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			ListView_EnsureVisible(list, i, FALSE);
			SetFocus (list);
			ModifiedSpecial = TRUE;
			return TRUE;
		case USEROPT_DELETE:
			if (useropt_selected < 0)
				return TRUE;
			i = useropt_selected;
			remove_user_option (i);
			ListView_DeleteItem (list, i);
			useropt_change_selected ();
			if (i >= ListView_GetItemCount (list))
				--i;
			if (i >= 0) {
				ListView_SetItemState (list, i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				ListView_EnsureVisible (list, i, FALSE);
			}
			return TRUE;
		}
		return 0;
	case WM_NOTIFY:
		nm = (LPNMHDR) lparam;
		if (nm -> idFrom != USEROPT_LIST)
			return 0;
		nml = (NM_LISTVIEW*) nm;
		switch (nm -> code) {
		case LVN_ITEMCHANGED:
			if ((nml -> uChanged & LVIF_STATE) == 0)
				return FALSE;
			if (detect_check_change && (nml -> uNewState ^ nml -> uOldState) & LVIS_STATEIMAGEMASK) {
				BOOL b = (nml -> uNewState & INDEXTOSTATEIMAGEMASK (2)) != 0;
				set_user_option_value (nml -> iItem, b);
				set_option_value_text (nml -> iItem, b);

			}
			useropt_change_selected ();
			return TRUE;
		}
		return FALSE;
	}
	return 0;
}

/* ----- User equations editing ------------------------------- */

int equ_selected;
BOOL test_value_changed = FALSE;

void	equ_change_selected ()
{
	int i = ListView_GetNextItem (GetDlgItem (EquDlg, EQU_LIST), -1, LVNI_FOCUSED);
	if (i == equ_selected) return;
	equ_selected = i;
	EnableWindow (GetDlgItem (EquDlg, EQU_VALUE), i >= 0);
	EnableWindow (GetDlgItem (EquDlg, EQU_DELETE), i >= 0);
	
	test_value_changed = FALSE;
	if (i < 0) {
		SetDlgItemText (EquDlg, EQU_NAME, "Value");
		SetDlgItemText (EquDlg, EQU_VALUE, "");
	} else {
		char t [STRLEN];
		char * n = get_lv_text (EquDlg, EQU_LIST, i, 0);
		char * s = Get_equation (EditProj, n);
		sprintf (t, "Value of %s:", n);
		SetDlgItemText (EquDlg, EQU_NAME,  t);
		if (!s) s = "";
		SetDlgItemText (EquDlg, EQU_VALUE,  s);
	}
	test_value_changed = TRUE;
}

void	add_user_equation_line (char * name, char * value)
{
	LV_ITEM item;
	HWND list = GetDlgItem (EquDlg, EQU_LIST);
	memset (&item, 0, sizeof (item));
	item.mask = LVIF_TEXT;
	item.iItem = ListView_GetItemCount (list);
	item.iSubItem = 0;
	item.state = 0;
	item.pszText = name;
	ListView_InsertItem (list, &item);
	item.iSubItem = 1;
	item.pszText = value;
	ListView_SetItem (list, &item);
}

BOOL	add_equ_to_list (char * name)
{
	char * s = Get_equation (EditProj, name);
	if (!s) s = "";
	add_user_equation_line (name, s);
	return TRUE;
}

BOOL CALLBACK EquDlgProc (HWND hwnd, UINT msg,
			       WPARAM wparam, LPARAM lparam)
{
	char s [STRLEN];
	HWND list;
	LV_COLUMN lvc;
	RECT rect;
	LPNMHDR nm;
	int i;
	BOOL b;

	list = GetDlgItem (hwnd, EQU_LIST);

	switch (msg) {
	case WM_INITDIALOG:
		EquDlg = hwnd;
		set_lv_color (list);
		SendMessage (list, LVM_SETEXTENDEDLISTVIEWSTYLE,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
		);
		SetWindowLong (list, GWL_STYLE, GetWindowLong (list, GWL_STYLE) | LVS_SHOWSELALWAYS);

		GetClientRect (list, &rect);
		memset (&lvc, 0, sizeof (lvc));
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.fmt = LVCFMT_LEFT; 
		lvc.cx = (rect.right - rect.left) / 2;
		lvc.iSubItem = 0;
		lvc.pszText = "Name";
		ListView_InsertColumn (list, 0, &lvc);
		lvc.cx = rect.right - rect.left - lvc.cx;
		lvc.iSubItem = 1;
		lvc.pszText = "Value";
		ListView_InsertColumn (list, 1, &lvc);
		Enum_user_equations (add_equ_to_list);
		equ_selected = -2;
		test_value_changed = FALSE;
		ListView_SetItemState (list, 0, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		equ_change_selected ();
		return 0;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case EQU_ADD:
			i = ListView_GetItemCount (list);
			b = Ask_text ("Type a name for new equation", "New equation", s, sizeof (s), 0);
			if (!b || !s[0]) return TRUE;
			if (Test_option_presence (EditProj, s))	{
				MsgBox ("Add option", MB_OK | MB_ICONEXCLAMATION, "Equation or option %s already exists", s);
				return TRUE;
			}
			Set_user_equation (EditProj, s, "");
			add_user_equation_line (s, "");
			ListView_SetItemState (list, i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			ListView_EnsureVisible(list, i, FALSE);
			SetFocus (list);
			ModifiedSpecial = TRUE;
			return TRUE;
		case EQU_DELETE:
			if (equ_selected < 0)
				return TRUE;
			i = equ_selected;
			Delete_option (EditProj, get_lv_text (hwnd, EQU_LIST, i, 0));
			ModifiedSpecial = TRUE;
			ListView_DeleteItem (list, i);
			if (i >= ListView_GetItemCount (list))
				--i;
			if (i >= 0) {
				ListView_SetItemState (list, i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				ListView_EnsureVisible (list, i, FALSE);
			}
			equ_change_selected ();
			return TRUE;
		case EQU_VALUE:
			if (test_value_changed && NOTIFY_CODE == EN_CHANGE) {
				GetDlgItemText (hwnd, EQU_VALUE, s, sizeof (s));
				if (equ_selected == -1)
					return 1;
				if (!Set_user_equation (EditProj, get_lv_text (hwnd, EQU_LIST, equ_selected, 0), s))
					return 1;
				set_lv_text (list, equ_selected, 1, s);
				ModifiedSpecial = TRUE;
				return 1;
			}
			return 0;
		}
		return 0;
	case WM_NOTIFY:
		nm = (LPNMHDR) lparam;
		if (nm -> idFrom != EQU_LIST)
			return 0;
		switch (nm -> code) {
		case LVN_ITEMCHANGED:
			equ_change_selected ();
			return TRUE;
		}
		return FALSE;
	}
	return 0;
}

/* ----- Packages editing ------------------------------------- */

int package_selected;

void	package_change_selected ()
{
	int i = ListView_GetNextItem (GetDlgItem (PackagesDlg, PACKAGE_LIST), -1, LVNI_FOCUSED);
	if (i == package_selected) return;
	package_selected = i;
	EnableWindow (GetDlgItem (PackagesDlg, PACKAGE_DIR), i >= 0);
	EnableWindow (GetDlgItem (PackagesDlg, PACKAGE_MASK), i >= 0);
	EnableWindow (GetDlgItem (PackagesDlg, PACKAGE_DELETE), i >= 0);
	
	if (i < 0) {
		SetDlgItemText (PackagesDlg, PACKAGE_DIR,  "");
		SetDlgItemText (PackagesDlg, PACKAGE_MASK, "");
	} else {
		SetDlgItemText (PackagesDlg, PACKAGE_DIR,  Get_proj_package_file (EditProj, i));
		SetDlgItemText (PackagesDlg, PACKAGE_MASK, Get_proj_package_mask (EditProj, i));
	}
}


BOOL	add_package_to_list (char * dir, char * mask)
{
	LV_ITEM item;
	HWND list = GetDlgItem (PackagesDlg, PACKAGE_LIST);
	memset (&item, 0, sizeof (item));
	item.mask = LVIF_TEXT;
	item.iItem = ListView_GetItemCount (list);
	item.iSubItem = 0;
	item.state = 0;
	item.pszText = dir;
	ListView_InsertItem (list, &item);
	item.iSubItem = 1;
	item.pszText = mask;
	ListView_SetItem (list, &item);
	return TRUE;
}

BOOL CALLBACK PackagesDlgProc (HWND hwnd, UINT msg,
			       WPARAM wparam, LPARAM lparam)
{
	char s [STRLEN], *p, *q;
	HWND list;
	LV_COLUMN lvc;
	RECT rect;
	LPNMHDR nm;
	int i;

	list = GetDlgItem (hwnd, PACKAGE_LIST);

	switch (msg) {
	case WM_INITDIALOG:
		PackagesDlg = hwnd;
		set_lv_color (list);
		SendMessage (list, LVM_SETEXTENDEDLISTVIEWSTYLE,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES,
			LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
		);
		SetWindowLong (list, GWL_STYLE, GetWindowLong (list, GWL_STYLE) | LVS_SHOWSELALWAYS);

		GetClientRect (list, &rect);
		memset (&lvc, 0, sizeof (lvc));
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.fmt = LVCFMT_LEFT; 
		lvc.cx = (rect.right - rect.left) * 2 / 3;
		lvc.iSubItem = 0;
		lvc.pszText = "Directory";
		ListView_InsertColumn (list, 0, &lvc);
		lvc.cx = rect.right - rect.left - lvc.cx;
		lvc.iSubItem = 1;
		lvc.pszText = "File mask";
		ListView_InsertColumn (list, 1, &lvc);
		Enum_proj_packages (add_package_to_list);
		package_selected = -2;
		ListView_SetItemState (list, 0, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		package_change_selected ();
		return 0;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case PACKAGE_ADD:
			i = ListView_GetItemCount (list);
			q = "*.class";
			if (i > 0) {
				p = Get_proj_package_mask (EditProj, i-1);
				if (p && p[0])
					q = p;
			}
			if (!Add_proj_package (EditProj, "", q))
				return TRUE;
			add_package_to_list ("", q);
			ListView_SetItemState (list, i, LVIS_FOCUSED, LVIS_FOCUSED);
			ListView_EnsureVisible (list, i, FALSE);
			SetFocus (GetDlgItem (hwnd, PACKAGE_DIR));
			ModifiedSpecial = TRUE;
			return 1;
		case PACKAGE_DELETE:
			if (package_selected < 0)
				return TRUE;
			i = package_selected;
			Delete_proj_package (EditProj, i);
			ListView_DeleteItem (list, i);
			if (i >= ListView_GetItemCount (list))
				--i;
			if (i >= 0) {
				ListView_SetItemState (list, i, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				ListView_EnsureVisible (list, i, FALSE);
			}
			package_change_selected ();
			return TRUE;
		case PACKAGE_DIR:
			if (NOTIFY_CODE == EN_CHANGE) {
				GetDlgItemText (hwnd, PACKAGE_DIR, s, sizeof (s));
				if (package_selected == -1)
					return 1;
				if (!Edit_proj_package_file (EditProj, package_selected, s))
					return TRUE;
				set_lv_text (list, package_selected, 0, s);
				ModifiedSpecial = TRUE;
				return 1;
			}
			return 0;
		case PACKAGE_MASK:
			if (NOTIFY_CODE == EN_CHANGE) {
				GetDlgItemText (hwnd, PACKAGE_MASK, s, sizeof (s));
				if (package_selected == -1)
					return 1;
				if (!Edit_proj_package_mask (EditProj, package_selected, s))
					return TRUE;
				set_lv_text (list, package_selected, 1, s);
				ModifiedSpecial = TRUE;
				return 1;
			}
			return 0;
		}
		return 0;
	case WM_NOTIFY:
		nm = (LPNMHDR) lparam;
		if (nm -> idFrom != PACKAGE_LIST)
			return 0;
		switch (nm -> code) {
		case LVN_ITEMCHANGED:
			package_change_selected ();
			return TRUE;
		}
		return FALSE;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

void	show_special_box (int n, int x0, int y0)
{
	HWND wnd;

	switch (n)	{
	case SPECIAL_USEROPTIONS:
		if (!UserOptDlg)
			CreateDialog (MyInstance, MAKEINTRESOURCE (USEROPT_DIALOG), OptionsDlg, UserOptDlgProc);
		wnd = UserOptDlg;
		break;
	case SPECIAL_USEREQUATIONS:
		if (!EquDlg)
			CreateDialog (MyInstance, MAKEINTRESOURCE (EQU_DIALOG), OptionsDlg, EquDlgProc);
		wnd = EquDlg;
		break;
	case SPECIAL_MODULELIST:
		if (!ModuleDlg)
			CreateDialog (MyInstance, MAKEINTRESOURCE (MODULE_DIALOG), OptionsDlg, ModuleDlgProc);
		wnd = ModuleDlg;
		break;
	case SPECIAL_PACKAGES:
		if (!PackagesDlg)
			CreateDialog (MyInstance, MAKEINTRESOURCE (PACKAGE_DIALOG), OptionsDlg, PackagesDlgProc);
		wnd = PackagesDlg;
		break;
	default:
		return;
	}
	SetWindowPos (wnd, HWND_TOP, x0, y0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}

void	clear_special_box (void)
{
	ShowWindow (PackagesDlg, SW_HIDE);
	ShowWindow (UserOptDlg, SW_HIDE);
	ShowWindow (EquDlg, SW_HIDE);
	ShowWindow (ModuleDlg, SW_HIDE);
}

void	NewListSelection (Node * p)
{
	RECT rc_present, rc, orig_rc_present;
	Option * o;
	int y;
	int i, j;
	int groupcnt;
	int wasstatic;
	RECT group_rect;
	HFONT font;
	HWND zwnd;
	int group_style;

	if (CurNode == p) return;
	CurNode = p;
	o = p -> optlist;
	clear_special_box ();
	for (i = 0; i < nitems; i ++)	{
		if (items [i].present)	 SetWindowPos (items [i].present, 0,0,0,0,0,
					 SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW);
		if (items [i].item)	 SetWindowPos (items [i].item, 0,0,0,0,0,
					 SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW);
		if (items [i].present)	 DestroyWindow (items [i].present);
		if (items [i].item)      DestroyWindow (items [i].item);
	}
	memset (items, 0, sizeof (items));
	if (mstatic)	{
		Clear_multi_static (mstatic, &OptRect);
		Delete_statics (mstatic, FALSE);
	}
	if (!mstatic)	{
		GetClientRect (OptionsWindow, &rc);
		mstatic = CreateControl ("multistatic", "", WS_VISIBLE, 0, &rc);
	}
	Get_child_pos (OptionsFrame, &rc);
	Add_static_item (mstatic, &rc, 0, 0, 0, MSTATIC_INSET);

	CalcCheckBox ("Present", &orig_rc_present);

	y = OptRect.top + base_dy/2;
	groupcnt = -1;
	group_style = 0;
	group_rect = OptRect;
	wasstatic = 0;
	for (i = 0; ; o = o -> next)	{
		if ((!o || o -> type != Static) && wasstatic)
			y += base_dy * 3/4;
		wasstatic = 0;
		if (groupcnt == 0 || groupcnt > 0 && !o)	{
			group_rect.bottom = y;
			if (group_style != GROUP_NULL)
				Add_static_item (mstatic, &group_rect, 0, 0, FALSE,
						 group_style == GROUP_RAISED ? MSTATIC_RAISED:
						 group_style == GROUP_INSET  ? MSTATIC_INSET:
									   MSTATIC_FRAME);
			group_rect = OptRect;
			y += base_dy / 2;
		}

		if (!o) break;
		groupcnt --;
		items [i].opt = o;
		rc_present = orig_rc_present;
		EnableWindow (GetDlgItem (OptionsWindow, OPT_UNDOPAGE), o->type != Special);
		switch (o -> type)	{
		case Boolean:
			CalcCheckBox (o -> comment, &rc);
			align_rect (&group_rect, &rc, y, FALSE);
			items [i].item = CreateCheckBox (o -> comment, BS_AUTO3STATE, &rc);
			y = rc.bottom;
			break;
		case Equation:
			rc.left = group_rect.left + base_dx;
			rc.top = y;
			rc.bottom = rc.top + base_dy;
			rc.right = group_rect.right - DX;
			Add_static_item (mstatic, &rc, o->comment, OptFont, TRUE, MSTATIC_LEFT);
			y = rc.bottom + EDITBOX_DELTA/2;
			align_rect (&group_rect, &rc_present, y, TRUE);
			rc.top = y;
			rc.bottom = rc.top + base_dy + EDITBOX_DELTA;
			rc.left = group_rect.left + base_dx;
			rc.right = rc_present.left - base_dx;
			align_two_rects_y (&rc, &rc_present);
			items [i].item = CreateEditBox (&rc);
			if (o->defval == '%')	{
				StdEditBoxProc = Subclass_control (items [i].item,
						  NumberEditBoxProc);
			} else if (o->defval == '$')	{
				StdEditBoxProc = Subclass_control (items [i].item,
						  HexEditBoxProc);
			}
			items [i].present = CreateCheckBox ("Present",
				BS_AUTOCHECKBOX, &rc_present);

			y = rc.bottom;
			break;
		case Enumeration:
			rc.top = y;
			rc.bottom = rc.top + base_dy;
			rc.left = group_rect.left + base_dx;
			rc.right = group_rect.right - DX;
			Add_static_item (mstatic, &rc, o->comment, OptFont, TRUE, MSTATIC_LEFT);
			y = rc.top = rc.bottom + COMBOBOX_DELTA / 2;
			align_rect (&group_rect, &rc_present, y, TRUE);
			rc.top = y;
			rc.bottom = y + base_dy + COMBOBOX_DELTA;
//			rc.left = group_rect.left + base_dx;
			rc.right = rc_present.left - base_dx;
			y = rc.bottom;
			align_two_rects_y (&rc, &rc_present);
			items [i].item = CreateComboBox (&rc, o->nalts);
			items [i].present = CreateCheckBox ("Present",
					BS_AUTOCHECKBOX, &rc_present);
			SendMessage (items [i].item, CB_RESETCONTENT, 0, 0);
			for (j = 0; j < o -> nalts; j ++)
				SendMessage (items [i].item, CB_ADDSTRING,
					     0, (LPARAM)(LPCSTR)o -> values [j]);

			break;
		case Static:
			if (!o->comment [0])	{
				if (o -> defval & STATIC_BIG)
					y += base_dy;
				else
					y += base_dy/2;
				continue;
			}
			font = MakeFont (o -> defval & STATIC_BIG,
					 o -> defval & STATIC_BOLD,
					 o -> defval & STATIC_ITALIC);
			CalcStatic (o -> comment, font, &rc);
			align_rect (&group_rect, & rc, y, FALSE);
			rc.right = group_rect.right - DX;
			Add_static_item (mstatic, &rc, o->comment, font, TRUE,
					o -> defval & STATIC_CENTER ? MSTATIC_CENTER:
					o -> defval & STATIC_RIGHT ? MSTATIC_RIGHT:
								     MSTATIC_LEFT);
			y = rc.bottom;
			wasstatic = 1;
			continue;
		case Group:
			if (groupcnt >= 0) continue;
			groupcnt = o -> nalts;
			group_rect = OptRect;
			group_rect.left += base_dx;
			group_rect.right -= base_dx *2;
			group_rect.top = y;
			group_rect.bottom -= base_dy;
			group_style = o -> defval;
			break;
		case Special:
			show_special_box (o->defval, group_rect.left, y);
			break;
		}
		SetControlData (items [i].item, items [i].present, o);
		y += base_dy / 2;
		i++;
	}
	nitems = i;
	zwnd = OptionsListBox;
	for (i = 0; i < nitems; i ++)	{
		if (items [i].item)	{
			SetWindowPos (items [i].item, zwnd, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			zwnd = items [i].item;
		}
		if (items [i].present)	{
			SetWindowPos (items [i].present, zwnd, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			zwnd = items [i].present;
		}
	}
}

BOOL	ProcessControl (HWND hwnd, UINT notify)
{
	char s [512];
	Option * o;
	int i, x;
	for (i = 0; i < nitems; i ++)	{
		o = items [i].opt;
		if (items [i].present == hwnd)	{
			if (notify != BN_CLICKED) return 0;
			o -> present = SendMessage (hwnd, BM_GETSTATE, 0, 0) & 3;
			SetControlData (items [i].item, items [i].present, o);
			o -> modified = TRUE;
			return 1;
		} else if (items [i].item == hwnd)	{
			switch (o -> type)	{
			case Boolean:
				if (notify != BN_CLICKED) return 0;
				x = SendMessage (hwnd, BM_GETSTATE, 0, 0) & 3;
				o -> present = x != 2;
				if (x != 2) o -> curval = x;
				o -> modified = TRUE;
				return 1;
			case Enumeration:
				if (notify != CBN_SELCHANGE) return 0;
				o -> curval = SendMessage (items [i].item, CB_GETCURSEL, 0, 0);
				UpdatePresent (i, TRUE);
				o -> modified = TRUE;
				return 1;
			case Equation:
				switch (notify)	{
				case EN_CHANGE: if (manual_set_text) return 0;
						UpdatePresent (i, TRUE);
						set_string_val (o, NULL);
						return 1;
				case EN_KILLFOCUS:
					if (o -> curstring) return 0;
					SendMessage (items [i].item, WM_GETTEXT,
						     sizeof (s), (LPARAM) (LPSTR) s);
					s [sizeof (s)-1] = 0;
					set_string_val (o, s);
					o -> modified = TRUE;
					return 1;
				}
				break;
			}
			return 0;
		}
	}
	return 0;
}

void	UndoPage (void)
{
	int i;
	for (i = 0; i < nitems; i ++)	{
		FillOptVal (items [i].opt);
		SetControlData (items [i].item, items [i].present, items [i].opt);
		items [i].opt -> modified = FALSE;
	}
}

void	ExpandOptionsBox (Node * p, int level, int before)
{
	int i;

	while (p && p -> level > level)	{
		i = SendMessage (OptionsListBox, LB_INSERTSTRING, before, 0);
		SendMessage (OptionsListBox, LB_SETITEMDATA, i, (LPARAM) p);
		if (before != -1) before ++;
		if (p -> expanded)
			p = p -> next;
		else	{
			i = p -> level;
			do
				p = p -> next;
			while (p && p ->level > i);
		}
	}
}

void	ShrinkOptionsBox (Node * node, int level, int pos)
{
	Node * p;
	while (p = GetNodeInBox (pos))
		if(p -> level > level)
			SendMessage (OptionsListBox, LB_DELETESTRING, pos, 0);
		else
			break;
}

void	UpdateOptionsBox (void)
{
	SendMessage (OptionsListBox, LB_RESETCONTENT, 0, 0);
	ExpandOptionsBox (NodeList, -1, -1);
}

void	OptionsOp (UINT cmd)
{
	long sel = SendMessage (OptionsListBox, LB_GETCURSEL, 0, 0);
	Node * node;
	if (cmd == LBN_DBLCLK)	{
		node =  GetNodeInBox (sel);
		if (!node) return;
		if (!node -> expanded)	{
			ExpandOptionsBox (node->next, node->level, sel+1);
			node -> expanded = TRUE;
		} else if (node -> next && node -> next -> level > node -> level)	{
			ShrinkOptionsBox (node -> next, node -> level, sel+1);
			node -> expanded = FALSE;
		}
	} else if (cmd == LBN_SELCHANGE)	{
		if (sel == -1) return;
		node = GetNodeInBox (sel);
		NewListSelection (node);
		LastListSel = sel;
		SetFocus (OptionsListBox);
	}
}

void	DrawListBoxItem (LPDRAWITEMSTRUCT d)
{
	Node * p;
	HDC bmdc;
	HBITMAP bmp;
	HBRUSH brush;
	RECT rc;
	int n = d -> itemID;
	if (n == -1) return;

	p = GetNodeInBox (n);
	rc = d -> rcItem;

	brush = CreateSolidBrush (GetSysColor (COLOR_WINDOW));
	FillRect (d -> hDC, &rc, brush);
	DeleteObject (brush);

	rc.left = BMInfo.bmWidth * (p -> level + 1);
	if (d -> itemState & ODS_SELECTED)	{
		brush = CreateSolidBrush (GetSysColor (COLOR_HIGHLIGHT));
		FillRect (d -> hDC, & rc, brush);
		DeleteObject (brush);
		SetTextColor (d -> hDC, GetSysColor (COLOR_HIGHLIGHTTEXT));
		SetBkColor   (d -> hDC, GetSysColor (COLOR_HIGHLIGHT));
	} else	{
		SetTextColor (d -> hDC, GetSysColor (COLOR_WINDOWTEXT));
		SetBkColor   (d -> hDC, GetSysColor (COLOR_WINDOW));
	}
	TextOut (d -> hDC, rc.left, rc.top + (OptionItemHeight - SystemFontHeight) / 2,
		 p -> string, strlen (p -> string));

	bmdc = CreateCompatibleDC (d -> hDC);
	if (p -> next && p -> next -> level > p -> level)
		bmp = p -> expanded ? BitMapOpen : BitMapClose;
	else
		bmp = BitMapItem;

	SelectObject (bmdc, bmp);
	BitBlt (d -> hDC,
		rc.left - BMInfo.bmWidth,
		rc.top + (OptionItemHeight - BMInfo.bmHeight) / 2,
		BMInfo.bmWidth, BMInfo.bmHeight,
		bmdc,  0, 0, SRCAND);
	DeleteDC (bmdc);
	if (d -> itemState & ODS_FOCUS)
		DrawFocusRect (d -> hDC, &rc);
}


BOOL	Box_MouseDown (int x, int y)
{
	Node * p;
	y = y / OptionItemHeight + SendMessage (OptionsListBox, LB_GETTOPINDEX, 0, 0);
	p = GetNodeInBox (y);
	if (!p) return FALSE;
	return x >= BMInfo.bmWidth * p -> level && x < BMInfo.bmWidth * (p -> level + 1);
}

LRESULT	CALLBACK ListBoxProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)    {
	case WM_CHAR:
		if (wparam == ' ')	{
			OptionsOp (LBN_DBLCLK);
			return 0;
		}
		break;
	case WM_LBUTTONDOWN:
		if (Box_MouseDown (LOWORD (lparam), HIWORD (lparam)))	{
			CallWindowProc (StdLBProc, hwnd, msg, wparam, lparam);
			OptionsOp (LBN_DBLCLK);
			return 0;
		}
		break;
	}
	return CallWindowProc (StdLBProc, hwnd, msg, wparam, lparam);
}

BOOL	RegisterOptEdit (void)
{
	BitMapOpen  = LoadBitmap (MyInstance, MAKEINTRESOURCE (BMP_OPENED));
	BitMapClose = LoadBitmap (MyInstance, MAKEINTRESOURCE (BMP_CLOSED));
	BitMapItem  = LoadBitmap (MyInstance, MAKEINTRESOURCE (BMP_ITEM));

	if (!BitMapOpen || !BitMapClose || !BitMapItem) return FALSE;
	GetObject (BitMapOpen, sizeof (BMInfo), &BMInfo);
	OptionItemHeight = max (BMInfo.bmHeight+2, SystemFontHeight+2);
	return TRUE;
}

void	OptEdit_cleanup (void)
{
	int i, j;
	if (BitMapOpen)  DeleteObject (BitMapOpen);
	if (BitMapClose) DeleteObject (BitMapClose);
	if (BitMapItem)  DeleteObject (BitMapItem);

	for (i = 0; i < 2; i ++)
		for (j = 0; j < 2; j ++)	{
			if (SmallFont[i][j])  DeleteObject (SmallFont[i][j]);
			if (BigFont[i][j])    DeleteObject (BigFont[i][j]);
		}
}

UINT	TimerId;
BOOL	store_options (void);
int	PendingSelChange = 0;
void	Add_edit_proj_name (char *s);
char * OptBoxName;

BOOL CALLBACK OptDlgProc   (HWND hwnd, UINT msg,
			    WPARAM wparam, LPARAM lparam)
{
	HELPINFO * h;
	RECT rc;
	char s [_MAX_PATH*2];
	LPMEASUREITEMSTRUCT mi;

	switch (msg) {
	case WM_DESTROY:
		KillTimer (hwnd, TimerId);
		return 0;
	case WM_CLOSE:
		mstatic = 0;
		OptionsWindow = 0;
		OptionsFrame = 0;
		EndDialog (hwnd, 0);
		return 1;
	case WM_INITDIALOG:
		OptionsDlg = hwnd;
		PackagesDlg = NULL;
		UserOptDlg = NULL;
		EquDlg = NULL;
		ModuleDlg = NULL;
		OptFont = (HFONT) SendMessage (hwnd, WM_GETFONT, 0, 0);
		OptFontHeight = Calc_font_height (OptFont);
		base_dy = OptFontHeight;
		base_dx = base_dy / 2;
		ModifiedSpecial = FALSE;
		strcpy (s, OptBoxName);
		Add_edit_proj_name (s);
		SetWindowText (hwnd, s);
		Center_window (hwnd);
		OptionsListBox = GetDlgItem (hwnd, OPT_LISTBOX);
		OptionsFrame   = GetDlgItem (hwnd, OPT_FRAME);
		DefPaneWindow  = GetDlgItem (hwnd, OPT_DEFPANE);
		ShowWindow (OptionsFrame, SW_HIDE);
		OptionsWindow = hwnd;
		StdLBProc = (WNDPROC) GetWindowLong (OptionsListBox, GWL_WNDPROC);
		SetWindowLong (OptionsListBox, GWL_WNDPROC, (LONG) ListBoxProc);
		CalcFrames ();
		CurNode = 0;
		memset (items, 0, sizeof (items));
		mstatic = 0;
		UpdateOptionsBox ();
		SendMessage (OptionsListBox, LB_SETCURSEL, LastListSel, 0);
		OptionsOp (LBN_SELCHANGE);
		PendingSelChange = 0;
		SetFocus (OptionsListBox);
		TimerId = SetTimer (hwnd, 1, 50, 0);
		return 0;
	case WM_KEYDOWN:
		if (wparam == 0x1B)
			EndDialog (hwnd, 0);
		break;
	case WM_MEASUREITEM:
		mi = (LPMEASUREITEMSTRUCT) lparam;
		mi -> itemHeight = OptionItemHeight;
		return 1;;
	case WM_DRAWITEM:
		DrawListBoxItem ((LPDRAWITEMSTRUCT) lparam);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:	   if ((store_options () || ModifiedSpecial) && ! Save_edited_project ())
					return 1;
				   EndDialog (hwnd, 1); return 1;
		case IDCANCEL:     EndDialog (hwnd, 0); return 1;
		case OPT_UNDOPAGE: UndoPage (); return 1;
		case OPT_RELOAD:   clear_options ();
				   EndDialog (hwnd, 2);
				   return 1;
		case OPT_LISTBOX:
			switch (NOTIFY_CODE)	{
			case LBN_SELCHANGE:
				PendingSelChange = 1;
				return 1;
			case LBN_DBLCLK:
				OptionsOp (NOTIFY_CODE);
				return 1;
			}
			return 0;

		case IDHELP:
			return Dialog_help (OPT_DIALOG);
		}
		return ProcessControl (CONTROL_HWND, NOTIFY_CODE);
	case WM_HELP:
		h = (HELPINFO*) lparam;
		if (Option_help (hwnd, lparam))
			return 1;
		if (!h->iCtrlId)	{
			GetWindowRect (DefPaneWindow, &rc);
			if (PtInRect (&rc, h->MousePos))
				Control_id_help (OPT_DIALOG, OPT_DEFPANE);
			return 1;
		}
		return Control_help (OPT_DIALOG, lparam);
	case WM_TIMER:
		TimerProc ();
		if (PendingSelChange == 2)	{
			PendingSelChange = 0;
			OptionsOp (LBN_SELCHANGE);
		} else if (PendingSelChange)
			PendingSelChange = 2;
		return 0;
	}
	return 0;
}

void	FillValues (void)
{
	Node * p;
	Option * opt;
	for (p = NodeList; p; p = p -> next)
		for (opt = p -> optlist; opt; opt = opt -> next)
			if (opt -> type == Equation || opt -> type == Boolean ||
			    opt -> type == Enumeration)	{
				opt -> modified = FALSE;
				FillOptVal (opt);
			}
}

BOOL	store_options (void)
{
	Node * p;
	Option * opt;
	BOOL b;
	b = FALSE;
	for (p = NodeList; p; p = p -> next)
		for (opt = p -> optlist; opt; opt = opt -> next)
			if (opt -> modified &&
			    (opt -> type == Equation || opt -> type == Boolean ||
			     opt -> type == Enumeration)
			   )
			{
				StoreOptVal (opt);
				b = TRUE;
			}
	return b;
}

BOOL	Show_option_box (char * title)
{
	HWND hwnd;
	int rescode;
	OptBoxName = title;

	if (need_reload_options)	{
		clear_options ();
		need_reload_options = FALSE;
	}

	do	{
		if (!NodeList)	{
			read_options ();
			LastListSel = 0;
		}
		FillValues ();
		hwnd = GetActiveWindow ();
		rescode = Dialog (OPT_DIALOG, OptDlgProc);
		SetActiveWindow (hwnd);
	} while (rescode == 2);
	return rescode == 1;
}
