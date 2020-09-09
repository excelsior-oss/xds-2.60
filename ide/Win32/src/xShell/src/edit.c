#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "mdi.h"
#include "frame.h"
#include "var.h"
#include "tbuf.h"
#include "find.h"
#include "edit.h"
#include "open.h"
#include "proj.h"
#include "pooredit.h"
#include "fcache.h"
#include "projred.h"
#include "message.h"
#include "tools.h"
#include "syntax.h"
#include "config.h"
#include "res.h"

#ifndef MAX_EDIT_SIZE
#define MAX_EDIT_SIZE (20*1024)
#endif

int	EditCreateFile = NOFILE;
TBUF *	EditCreateBuf = NULL;
int	EditStatusBarLock = 0;
HICON	EditSmIcon;

HWND	Edit_share (EDIT_DATA * ed);
void	Edit_reload (EDIT_DATA * ed);

/* Configurable parameters */

struct	_EDIT_DATA {
			int  file;
			TBUF * buf;
			HWND editwnd;
			HWND hwnd;
			FILETIME time;
		};

void	DrawSelectionBar (EDIT_DATA * ed, int bookmark);
void	Print_editor (HWND hwnd, HWND editwnd, TBUF * buf, int file);

/* ---------------------------------------------------------------- */

void	store_name_var (int n)
{
	Update_tools_for_file (n>=0 ? filedir [n]:"");
}

/* ---------------------------------------------------------------- */

char * current_word (EDIT_DATA * ed, int * len)
{
	int x1, x2, y1, y2, n;
	char * p;

	Editor_get_block (ed->editwnd, &x1, &y1, &x2, &y2);
	if (!tbuf_get_line (ed->buf, y1, &p, &n))	{
		*len = 0;
		return 0;
	}
	if (y2 > y1)	{
		if (x1 >= n) x1 = n;
		x2 = n;
	} else	if (x2 == x1)	{
		x1 = tbuf_word_start (ed->buf, x1, y1);
		x2 = tbuf_word_end   (ed->buf, x1, y1);
	}
	*len = x2-x1;
	return p+x1;
}

/* ----------------- Search and replace in the edit control ---------- */

char *	search_history [SEARCH_HISTORY_LEN] = {NULL};
char	search_text [SEARCH_TEXT_LEN]="";
SEARCH_PATTERN * search_pattern = NULL;

EDIT_DATA * search_ed;

char * search_menu_def [][2] = {
	{"Tab", "\\t"},
	{"White space", "\\w"},
	{"Letter", "\\$"},
	{"Digit", "\\#"},
	{"Identifier", "\\I"},
	{"Number", "\\N"},
	{"Any character", "\\?"},
	{"Line break", "\\n"},
	{"Literal \"\\\"", "\\\\"}
};

#define SEARCH_MENU_BASE 20000
#define SEARCH_MENU_NUM sizeof (search_menu_def) / sizeof (search_menu_def [0])

void	insert_search_item (HWND hwnd, int i)
{
	int start, fin;
	char t [2000];
	char r [2000];
	GetDlgItemText (hwnd, SEARCH_TEXT, t, sizeof (t)-10);
//	SendDlgItemMessage (hwnd, SEARCH_TEXT, CB_GETEDITSEL, (WPARAM) &start, (LPARAM) &fin);
	start = fin = strlen (t);
	memcpy (r, t, start);
	r [start] = 0;
	strcat (r, search_menu_def [i][1]);
	start += strlen (search_menu_def [i][1]);
	strcat (r, t+fin);
	SetDlgItemText (hwnd, SEARCH_TEXT, r);
	SetFocus (GetDlgItem (hwnd, SEARCH_TEXT));
	SendDlgItemMessage (hwnd, SEARCH_TEXT, CB_SETEDITSEL, 0, MAKELPARAM (start, start));
}

void search_menu (HWND hwnd)
{
	HMENU m;
	RECT rc;
	int i;

	m = CreatePopupMenu ();
	for (i = 0; i < SEARCH_MENU_NUM; i++)
		AppendMenu (m, MF_STRING, SEARCH_MENU_BASE+i, search_menu_def[i][0]);
	GetWindowRect (GetDlgItem (hwnd, SEARCH_MENU), &rc);
	TrackPopupMenu (m, TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.right, rc.top,
			0, hwnd, 0);
	DestroyMenu (m);
}

BOOL CALLBACK SearchDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char * w;
	int n;
	static HBITMAP bmp = 0;

	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		if (!bmp) bmp = LoadBitmap (0, MAKEINTRESOURCE (OBM_MNARROW));
		SendDlgItemMessage (hwnd, SEARCH_MENU, BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) bmp);
		Fill_history_box (GetDlgItem (hwnd, SEARCH_TEXT),
				  search_history, SEARCH_HISTORY_LEN);
		SendDlgItemMessage (hwnd, SEARCH_TEXT, EM_LIMITTEXT,
				    sizeof (search_text)-1, 0);
		CheckDlgButton (hwnd, SEARCH_CASE, search_case);
		CheckDlgButton (hwnd, SEARCH_ANYTEXT, !search_prefix && !search_suffix);
		CheckDlgButton (hwnd, SEARCH_PREFIX,   search_prefix && !search_suffix);
		CheckDlgButton (hwnd, SEARCH_SUFFIX,  !search_prefix &&  search_suffix);
		CheckDlgButton (hwnd, SEARCH_WHOLEWORD,search_prefix &&  search_suffix);
		CheckDlgButton (hwnd, SEARCH_FORWARD,  search_fromcursor && !search_back);
		CheckDlgButton (hwnd, SEARCH_BACK,     search_fromcursor && search_back);
		CheckDlgButton (hwnd, SEARCH_WHOLETEXT, !search_fromcursor);
		CheckDlgButton (hwnd, SEARCH_INSERTCURRENT, search_insert_current);
		CheckDlgButton (hwnd, SEARCH_ADVANCED, search_advanced);
		ShowWindow (GetDlgItem (hwnd, SEARCH_MENU), search_advanced ? SW_SHOW : SW_HIDE);
		w = current_word (search_ed, &n);
		if (n && w[0] && search_insert_current)	{
			if (n >= sizeof (search_text)-1) n = sizeof (search_text)-1;
			memcpy (search_text, w, n);
			search_text [n] = 0;
		}
		SetDlgItemText (hwnd, SEARCH_TEXT, search_text);
		return 1;
	case WM_COMMAND:
		if (CONTROL_ID >= SEARCH_MENU_BASE && CONTROL_ID < SEARCH_MENU_BASE+SEARCH_MENU_NUM)	{
			insert_search_item (hwnd, CONTROL_ID - SEARCH_MENU_BASE);
			return 1;
		}
		switch (CONTROL_ID)	{
		case IDOK:
			if (GetWindowTextLength (GetDlgItem (hwnd, SEARCH_TEXT)) == 0)
				break;
			search_case = IsDlgButtonChecked (hwnd, SEARCH_CASE);
			search_advanced = IsDlgButtonChecked (hwnd, SEARCH_ADVANCED);
			if (IsDlgButtonChecked (hwnd, SEARCH_ANYTEXT))
				search_prefix = search_suffix = FALSE;
			else if (IsDlgButtonChecked (hwnd, SEARCH_PREFIX))	{
				search_prefix = TRUE; search_suffix = FALSE;
			} else if (IsDlgButtonChecked (hwnd, SEARCH_SUFFIX))	{
				search_prefix = FALSE; search_suffix = TRUE;
			} else
				search_prefix = search_suffix = TRUE;
			search_back = IsDlgButtonChecked (hwnd, SEARCH_BACK);
			search_fromcursor = !IsDlgButtonChecked (hwnd, SEARCH_WHOLETEXT);
			GetDlgItemText (hwnd, SEARCH_TEXT, search_text, sizeof (search_text));
			Insert_history (search_history, SEARCH_HISTORY_LEN, search_text);
			Write_search_settings ();
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (SEARCH_DIALOG);
		case SEARCH_ADVANCED:
			if (NOTIFY_CODE == BN_CLICKED)	{
				ShowWindow (GetDlgItem (hwnd, SEARCH_MENU),
						IsDlgButtonChecked (hwnd, SEARCH_ADVANCED)
						? SW_SHOW : SW_HIDE);
			}
			return 1;
		case SEARCH_INSERTCURRENT:
			if (NOTIFY_CODE == BN_CLICKED)	{
				search_insert_current = IsDlgButtonChecked (hwnd, SEARCH_INSERTCURRENT);
				w = current_word (search_ed, &n);
				if (n && w[0] && search_insert_current)	{
					if (n >= sizeof (search_text)-1) n = sizeof (search_text)-1;
					memcpy (search_text, w, n);
					search_text [n] = 0;
					SetDlgItemText (hwnd, SEARCH_TEXT, search_text);
				}
			}
			return 1;
		case SEARCH_MENU:
			search_menu (hwnd);
			return 1;
		}
		return 1;
	case WM_HELP:
		return Control_help (SEARCH_DIALOG, lparam);
	}
	return 0;
}

void	do_search (EDIT_DATA * ed, char search_text [], SEARCH_PATTERN * pat,
		        BOOL silently_stop,
			BOOL search_case,
			BOOL search_prefix,
			BOOL search_suffix,
			BOOL search_fromcursor,
			BOOL search_back)
{
	int x0, y0;
	int x1, y1, x2, y2;
	int nlines, lastlen;
	int r;
	BOOL second_time, found;
	HCURSOR cur;

	nlines = tbuf_get_nlines (ed->buf);
	lastlen = tbuf_get_line_length (ed->buf, nlines-1);
	Editor_get_block (ed->editwnd, &x1, &y1, &x2, &y2);
	if (search_fromcursor)	{
		x0 = x1;
		y0 = y1;
	} else
		x0 = y0 = 0;
	second_time = FALSE;

search_again:

	cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));
	if (pat)
		found = tbuf_word_pattern_search (ed->buf, x0, y0, pat, 
					&x1, &y1, &x2, &y2, 
					!search_case, search_back,
					search_prefix, search_suffix, !second_time);
	else
		found = tbuf_word_search (ed->buf, x0, y0, search_text, 
					&x1, &y1, &x2, &y2, 
					!search_case, search_back,
					search_prefix, search_suffix, !second_time);
	SetCursor (cur);

	if (!found)	{
		if (silently_stop) return;
		if (!second_time && !search_back && (x0 != 0 || y0 != 0))	{
			r = MsgBox ("SEARCH", MB_ICONEXCLAMATION|MB_YESNO,
				"Search string not found:\n\"%s\"\n"
				"Continue search from beginning of file?",
				search_text);
			if (r != IDYES)	return;
			x0 = y0 = 0;
			second_time = TRUE;
			goto search_again;
		}
		if (!second_time && search_back && (y0 != nlines-1 || x0 < lastlen))	{
			r = MsgBox ("SEARCH", MB_ICONEXCLAMATION|MB_YESNO,
				"Search string not found:\n\"%s\"\n"
				"Continue search from end of file?",
				search_text);
			if (r != IDYES)	return;
			y0 = nlines - 1;
			x0 = lastlen;
			second_time = TRUE;
			goto search_again;
		}
		MsgBox ("SEARCH", MB_ICONEXCLAMATION|MB_OK, "Search string not found:\n\"%s\"", search_text);
	} else
		Editor_set_block_visible (ed->editwnd, x1, y1, x2, y2);
}

void	Search_in_window (HWND editwnd,
			  char search_text [],
			  BOOL search_case,
			  BOOL search_prefix,
			  BOOL search_suffix)
{
	EDIT_DATA * ed = (EDIT_DATA *) GetWindowLong (editwnd, 0);
	
	do_search (ed, search_text, NULL, FALSE, search_case, search_prefix,
		   search_suffix, FALSE, FALSE);
}

void	Search (EDIT_DATA * ed)
{
	int r;
	search_ed = ed;
	r = Dialog (SEARCH_DIALOG, SearchDlgProc);
	if (!r)	return;
	if (search_pattern)
		free_pattern (search_pattern);
	search_pattern = NULL;
	if (search_advanced)
		search_pattern = compile_pattern (search_text);
	do_search (ed,  search_text,	search_pattern,
			FALSE,
			search_case,	search_prefix,
			search_suffix,	search_fromcursor,
			search_back);
}

void	SearchNext (EDIT_DATA * ed)
{
	if (!strlen (search_text))	{
		Search (ed);
		return;
	}
	do_search (ed,  search_text,	search_pattern,
			FALSE,
			search_case,
			search_prefix,
			search_suffix,
			TRUE,
			search_back);
}

void	SearchPrev (EDIT_DATA * ed)
{
	if (!strlen (search_text))	{
		Search (ed);
		return;
	}
	do_search (ed,  search_text,	search_pattern,
			FALSE,
			search_case,
			search_prefix,
			search_suffix,
			TRUE,
			!search_back);
}

BOOL	Edit_test_line_found (EDIT_DATA * ed,
			      char search_text [],
			      BOOL search_case,
			      BOOL search_prefix,
			      BOOL search_suffix)
{
	int x1, y1, x2, y2;

	return tbuf_word_search (ed->buf, 0, 0, search_text, 
				&x1, &y1, &x2, &y2, 
				!search_case, FALSE,
				search_prefix, search_suffix, FALSE);
}

/* ----------------------------------------------------------------------- */

char * replace_history [SEARCH_HISTORY_LEN] = {0};
char * replace_by_history [SEARCH_HISTORY_LEN] = {0};

char replace_text [300]="";
char replace_by_text [300]="";
BOOL replace_case = FALSE;
BOOL replace_words = FALSE;
BOOL replace_all = FALSE;
BOOL replace_prompt = TRUE;
BOOL replace_fromcursor = TRUE;
BOOL replace_whole = FALSE;
BOOL replace_selection = FALSE;
BOOL selection_enable = FALSE;
EDIT_DATA * replace_ed;

BOOL CALLBACK ReplaceDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char * w;
	int n;

	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		Fill_history_box (GetDlgItem (hwnd, REPLACE_TEXT),
				  replace_history, SEARCH_HISTORY_LEN);
		Fill_history_box (GetDlgItem (hwnd, REPLACE_BY_TEXT),
				  replace_by_history, SEARCH_HISTORY_LEN);
		SendDlgItemMessage (hwnd, REPLACE_TEXT, EM_LIMITTEXT,
				    sizeof (replace_text)-1, 0);
		SendDlgItemMessage (hwnd, REPLACE_BY_TEXT, EM_LIMITTEXT,
				    sizeof (replace_by_text)-1, 0);
		if (!selection_enable && replace_selection)	{
			replace_selection = FALSE;
			replace_fromcursor = TRUE;
		}
		EnableWindow (GetDlgItem (hwnd, REPLACE_SELECTION), selection_enable);
		CheckDlgButton (hwnd, REPLACE_CASE, replace_case);
		CheckDlgButton (hwnd, REPLACE_WORDS, replace_words);
		CheckDlgButton (hwnd, REPLACE_ALL, replace_all);
		CheckDlgButton (hwnd, REPLACE_PROMPT, replace_prompt);
		CheckDlgButton (hwnd, REPLACE_FROMCURSOR, replace_fromcursor);
		CheckDlgButton (hwnd, REPLACE_WHOLETEXT, replace_whole);
		CheckDlgButton (hwnd, REPLACE_SELECTION, replace_selection);
		w = current_word (replace_ed, &n);
		if (n && w[0] && replace_insert_current)	{
			if (n >= sizeof (replace_text)-1) n = sizeof (replace_text)-1;
			memcpy (replace_text, w, n);
			replace_text [n] = 0;
		}
		SetDlgItemText (hwnd, REPLACE_TEXT,    replace_text);
		SetDlgItemText (hwnd, REPLACE_BY_TEXT, replace_by_text);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			if (GetWindowTextLength (GetDlgItem (hwnd, REPLACE_TEXT)) == 0)
				break;
			replace_case       = IsDlgButtonChecked (hwnd, REPLACE_CASE);
			replace_words      = IsDlgButtonChecked (hwnd, REPLACE_WORDS);
			replace_all        = IsDlgButtonChecked (hwnd, REPLACE_ALL);
			replace_prompt     = IsDlgButtonChecked (hwnd, REPLACE_PROMPT);
			replace_fromcursor = IsDlgButtonChecked (hwnd, REPLACE_FROMCURSOR);
			replace_whole      = IsDlgButtonChecked (hwnd, REPLACE_WHOLETEXT);
			replace_selection  = IsDlgButtonChecked (hwnd, REPLACE_SELECTION);
			GetDlgItemText (hwnd, REPLACE_TEXT, replace_text, sizeof (replace_text));
			GetDlgItemText (hwnd, REPLACE_BY_TEXT, replace_by_text, sizeof (replace_by_text));
			Insert_history (replace_history, SEARCH_HISTORY_LEN, replace_text);
			if (replace_by_text [0])
				Insert_history (replace_by_history, SEARCH_HISTORY_LEN, replace_by_text);
			Write_search_settings ();
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (REPLACE_DIALOG);
		case REPLACE_INSERTCURRENT:
			if (NOTIFY_CODE == BN_CLICKED)	{
				replace_insert_current = IsDlgButtonChecked (hwnd, REPLACE_INSERTCURRENT);
				w = current_word (replace_ed, &n);
				if (n && w[0] && replace_insert_current)	{
					if (n >= sizeof (replace_text)-1) n = sizeof (replace_text)-1;
					memcpy (replace_text, w, n);
					replace_text [n] = 0;
					SetDlgItemText (hwnd, REPLACE_TEXT, replace_text);
				}
			}
			return 1;
		}
		return 1;
	case WM_HELP:
		return Control_help (REPLACE_DIALOG, lparam);
		return 1;
	}
	return 0;
}

RECT	block_rect;

BOOL CALLBACK ReplacePromptDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rc, rc0;
	int sy;
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		GetWindowRect (hwnd, &rc);
		if (IntersectRect (&rc0, &rc, &block_rect))	{
			sy = GetSystemMetrics (SM_CYFULLSCREEN);
			if (sy - block_rect.bottom > block_rect.top)
				rc.top = block_rect.bottom;
			else
				rc.top = block_rect.top - (rc.bottom - rc.top);
			SetWindowPos (hwnd, 0, rc.left, rc.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}
		SetDlgItemText (hwnd, REPLACE_PROMPT_TEXT1, replace_text);
		SetDlgItemText (hwnd, REPLACE_PROMPT_TEXT2, replace_by_text);
		if (! replace_all)
			EnableWindow (GetDlgItem (hwnd, REPLACE_PROMPT_YESALL), 0);
		return 1;
	case WM_COMMAND:
		EndDialog (hwnd, CONTROL_ID);
		return 1;
	case WM_HELP:
		return Control_help (REPLACE_PROMPT_DIALOG, lparam);
		return 1;
	}
	return 0;
}

void	do_replace (EDIT_DATA * ed)
{
	int x0, y0;
	int x1, y1, x2, y2, y, bx1, by1, bx2, by2;
	BOOL first, found, replaced;
	int prompt;
	int search_len, replace_len;
	int replace_count;
	char s [300];
	BOOL rep_prompt = replace_prompt;
	BOOL replace_suffix, replace_prefix;
	HCURSOR cur;

DWORD t1, t2;


	Editor_get_block (ed->editwnd, &bx1, &by1, &bx2, &by2);
	if (replace_whole)
		x0 = y0 = 0;
	else	{
		x0 = bx1;
		y0 = by1;
	}

	search_len = strlen (replace_text);
	replace_len = strlen (replace_by_text);
	replace_count = 0;
	replace_suffix = replace_prefix = replace_words;

	first = TRUE;
	replaced = FALSE;

	t1	 = GetTickCount();
	cur = SetCursor (LoadCursor (0, IDC_WAIT));

	if (!rep_prompt)
		tbuf_stop_line_notifications (ed->buf);
	
	for (;;)	{

		found = tbuf_word_search (ed->buf, x0, y0, replace_text,
					&x1, &y1, &x2, &y2, 
					!replace_case, FALSE,
					replace_prefix, replace_suffix, FALSE);

		if (!found || (replace_selection &&
			((y1 > by2) || (y1 == by2 && x1 > bx2 - search_len)))
		   )
		{
			SetCursor (cur);
			if (first)
				MsgBox ("REPLACE", MB_ICONSTOP|MB_OK, "Search string not found:\n\"%s\"",
					 replace_text);
			break;
		}
		first = FALSE;
		if (rep_prompt)	{
			Editor_set_block_visible (ed->editwnd, x1, y1, x2, y2);
			Editor_get_line_screen_pos (ed->editwnd, y1, &block_rect.top, &y);
			Editor_get_line_screen_pos (ed->editwnd, y2, &y, &block_rect.bottom);
			block_rect.left = 0; block_rect.right = 10000;
			SetCursor (cur);
			prompt = Dialog (REPLACE_PROMPT_DIALOG, ReplacePromptDlgProc);
			if (!prompt || prompt == IDCANCEL) break;
			if (prompt == IDNO)	{
				y0 = y1;
				x0 = x1+replace_len;
				SetCursor (LoadCursor (0, IDC_WAIT));
				continue;
			}
			if (prompt == REPLACE_PROMPT_YESALL)	{
				rep_prompt = FALSE;
				tbuf_stop_line_notifications (ed->buf);
			}
		}
		if (!replaced)	{
			replaced = TRUE;
			tbuf_start_undo_block (ed->buf);
			tbuf_store_cursor_pos (ed->buf, bx1, by1, bx2, by2);
		}
		tbuf_insert_string (ed->buf, y1, x1, replace_by_text, replace_len);
		tbuf_delete_string (ed->buf, y1, x1+replace_len, search_len);
		replace_count ++;
		if (y1 == by2 && x1 < bx2) bx2 += replace_len-search_len;
		if (y1 == by1 && x1 < bx1) bx1 += replace_len-search_len;
		if (!replace_all)	{
			SetCursor (cur);
			break;
		}
		y0 = y1;
		x0 = x1+replace_len;
	}
	tbuf_continue_line_notifications (ed->buf);
	t2 = GetTickCount ();
	Editor_set_block (ed->editwnd, bx1, by1, bx2, by2);
	if (replaced) tbuf_finish_undo_block (ed->buf);
	wsprintf (s, "Replaces: %d", replace_count);
	Set_status_bar (ed->hwnd, s);
}

void	Replace (EDIT_DATA * ed)
{
	int x1, y1, x2, y2;

	selection_enable = Editor_get_block (ed->editwnd, &x1, &y1, &x2, &y2);

	replace_ed = ed;
	if (!Dialog (REPLACE_DIALOG, ReplaceDlgProc))
		return;
	do_replace (ed);
}

void	ReplaceNext (EDIT_DATA * ed)
{
	if (!strlen (replace_text))
		Replace (ed);
	else
		do_replace (ed);
}

/* ----------------------------------------------------------------------- */
#define N_EDITORS 100
EDIT_DATA * editors [N_EDITORS];
int n_editors = 0;
EDIT_DATA * Cur_highlight_edit = 0;

BOOL	add_editor (EDIT_DATA * ed)
{
	if (n_editors == N_EDITORS) return FALSE;
	editors [n_editors++] = ed;
	return TRUE;
}

int	find_editor (EDIT_DATA * ed)
{
	int i;
	for (i = 0; i < n_editors; i++)
		if (editors [i] == ed) return i;
	return -1;
}

void	adjust_all_names (EDIT_DATA * ed)
{
	int i;
	for (i = 0; i < n_editors; i++)
		if (editors [i]->buf == ed->buf)
			SendMessage (editors [i]->hwnd, WM_MYMDI_TEXTCHANGED, 0, 0);
}

void	remove_editor (EDIT_DATA * ed)
{
	int i;
	if (ed == Cur_highlight_edit)
		Cur_highlight_edit = 0;
	i = find_editor (ed);
	if (i >= 0)	{
		memmove (editors + i, editors + i+1, (n_editors-i-1)*sizeof (EDIT_DATA*));
		-- n_editors;
	}
}

EDIT_DATA * Edit_find_loaded_buf (int file)
{
	int i;

	for (i = 0; i < n_editors; i++)
		if (editors [i]->file == file) return editors [i];
	return 0;
}

EDIT_DATA * Edit_find_loaded_file_name (char * filename)
{
	int i;
	char ff [STRLEN];
	LPSTR name_portion;

	if (!GetFullPathName (filename, sizeof (ff), ff, &name_portion))
		strcpy (ff, filename);

	for (i = 0; i < n_editors; i++)
		if (!stricmp (filedir [editors [i]->file], filename))
			return editors [i];
	return 0;
}

HWND	Edit_find_loaded_file (int file)
{
	EDIT_DATA * ed = Edit_find_loaded_buf (file);
	if (!ed) return 0;
	return ed->hwnd;
}

int	Make_editor_list (int ** list)
{
	int i, j, k, *p;
	p = Malloc (sizeof (int) * n_editors);
	if (!p) return 0;
	j = 0;
	for (i = 0; i < n_editors; i++)	{
		if (editors [i]->file == NOFILE) continue;
		p[j] = editors [i]->file;
		for (k = 0; k < j; k++)
			if (editors [k]->file == p[j])
				break;
		if (k == j) ++ j;
	}
	if (list)
		*list = p;
	else
		free (p);
	return j;
}

/* ----------------------------------------------------------------------- */

void	Edit_drop_highlight (void)
{
	if (Cur_highlight_edit)
		Editor_set_highlight (Cur_highlight_edit->editwnd, -1);
	Cur_highlight_edit = 0;
}

void	Edit_highlight (EDIT_DATA * ed, int y, int x)
{
	Edit_drop_highlight ();
	Editor_set_highlight (ed->editwnd, y);
	Cur_highlight_edit = ed;
}

/* ----------------------------------------------------------------------- */

void	e_process_errors (TBUF * buf, int file)
{
	ErrLine * err;
	BOOL dirty = tbuf_is_dirty (buf);

	for (err = err_list; err; err = err->next)
		if (err->present && err->file == file)	{
			err->bookmark = tbuf_add_bookmark (buf, err->x, err->y, FALSE);
			err->was_dirty = dirty;
		}
}

void	e_relocate_errors (EDIT_DATA * ed)
{
	int x, y;
	ErrLine * err;

	for (err = err_list; err; err = err->next)
		if (err->present && err->file == ed->file)
			if (err->was_dirty ||
			    !tbuf_query_bookmark (ed->buf, err->bookmark, &x, &y))
				err->present = FALSE;
			else	{
				err->x = x;
				err->y = y;
			}
}

void	e_remove_errors (EDIT_DATA * ed)
{
	ErrLine * err;
	for (err = err_list; err; err = err->next)
		if (err->file == ed->file)	{
			err->bookmark = 0;
			if (err->was_dirty)
				err->present = FALSE;
		}
}

long	Edit_process_error (int file, int x, int y, BOOL * was_dirty)
{
	EDIT_DATA * ed = Edit_find_loaded_buf (file);
	if (!ed) return 0;
	*was_dirty = tbuf_is_dirty (ed->buf);
	return tbuf_add_bookmark (ed->buf, x, y, FALSE);
}

void	Edit_remove_error (int file, long bookmark)
{
	EDIT_DATA * ed = Edit_find_loaded_buf (file);
	if (!ed) return;
	tbuf_delete_bookmark (ed->buf, bookmark);
}

void	Edit_highlight_error (HWND hwnd, long bookmark)
{
	int x, y;
	EDIT_DATA * ed = (EDIT_DATA *) GetWindowLong (hwnd, 0);

	if (tbuf_query_bookmark (ed->buf, bookmark, &x, &y))
		Edit_highlight (ed, y, x);
}

BOOL	Edit_goto_bookmark (HWND hwnd, long bookmark)
{
	int x, y;
	EDIT_DATA * ed = (EDIT_DATA *) GetWindowLong (hwnd, 0);

	if (!tbuf_query_bookmark (ed->buf, bookmark, &x, &y))
		return FALSE;
	Editor_set_block (ed->editwnd, x, y, x, y);
	return TRUE;
}

/* ----------------------------------------------------------------------- */

BOOL	EditSaveAs (EDIT_DATA * ed)
{
	int file, i;
	DWORD token;

	file = Ask_save_file_name (ed->file);
	if (file == NOFILE) return FALSE;
	
	if (file != ed -> file && Edit_find_loaded_buf (file))	{
		MsgBox (ShellName, MB_OK | MB_ICONEXCLAMATION,
			"File %s is opened: can't write over it",
			filedir [file]);
		return FALSE;
	}
	if (!Write_file_from_buf (ed->buf, file, TRUE))	{
		if (ErrCode == ERROR_CANCELLED) return FALSE;
		MsgBox ("Save file", MB_ICONSTOP | MB_OK,
			"Error saving file %s:\n%s", filedir [file], ErrText());
		return FALSE;
	}
	e_remove_errors (ed);
	tbuf_set_dirty (ed->buf, FALSE);
	if (ClearUndoOnSave) tbuf_clear_undo (ed->buf);
	Get_file_time (filedir [file], &ed->time);
	store_name_var (ed -> file);
	token = get_lang_token (filedir [file]);
	for (i = 0; i < n_editors; i++)
		if (editors [i]->buf == ed->buf)	{
			editors [i]->time = ed->time;
			editors [i]->file = file;
			SendMessage (editors [i]->hwnd, WM_MYMDI_TEXTCHANGED, 0, 0);
			Editor_set_lang_token (editors [i]->editwnd, token);
		}
	DrawSelectionBar (ed, 0);
	Insert_into_file_history (file);
	return TRUE;
}

BOOL	EditSave (EDIT_DATA * ed)
{
	int i;
	if (ed -> file == NOFILE)
		return EditSaveAs (ed);
	else	{
		if (!Write_file_from_buf (ed->buf, ed->file, FALSE))
			return FALSE;
		Get_file_time (filedir [ed -> file], &ed->time);
		for (i = 0; i < n_editors; i++)
			if (editors [i]->buf == ed->buf)
				editors [i]->time = ed->time;
		tbuf_set_dirty (ed->buf, FALSE);
		if (ClearUndoOnSave) tbuf_clear_undo (ed->buf);
		e_relocate_errors (ed);
		return TRUE;
	}
}

BOOL	check_file_save (EDIT_DATA * ed, BOOL prompt, BOOL test_all)
{
	int k, i;
	if (!tbuf_is_dirty (ed->buf)) return TRUE;
	for (i = 0; i < n_editors; i++)	{
		if (editors [i]==ed && !test_all) break;
		if (editors [i]->buf == ed->buf && editors [i]!=ed)
			return TRUE;
	}
	if (prompt && !AutoSave || ed->file == NOFILE)	{
		k = MsgBox (ShellName, MB_ICONQUESTION|MB_YESNOCANCEL | MB_ICONQUESTION,
				"%s has been modified.\nWould you like to save it?",
			    ed -> file == NOFILE ? "File": filedir [ed -> file]);

		if (k == IDNO) return TRUE;
		if (k == IDCANCEL) return FALSE;
	}
	return EditSave (ed);
}

BOOL	Edit_save_all (BOOL prompt)
{
	int i;
	for (i = 0; i < n_editors; i++)
		if (!check_file_save (editors [i], prompt, FALSE))
			return FALSE;
	return TRUE;
}

/* ----------------------------------------------------------------------- */

void	DrawSelectionBar (EDIT_DATA * ed, int bookmark)
{
	char s [200];
	int x1, x2, y1, y2;
	
	if (EditStatusBarLock || !ed->editwnd) return;
	Editor_get_block (ed->editwnd, &x1, &y1, &x2, &y2);
	if (bookmark)
		if (x1 == x2 && y1 == y2)
			wsprintf (s, "Bookmark #%d set at %ld:%ld", bookmark, y1+1, x1+1);
		else
			wsprintf (s, "Bookmark #%d set at %ld:%ld - %ld:%ld", bookmark,
				  y1+1, x1+1, y2+1, x2+1);
	else
		if (x1 == x2 && y1 == y2)
			wsprintf (s, "%s at %ld:%ld", filedir [ed -> file], y1+1, x1+1);
		else
			wsprintf (s, "%s at %ld:%ld - %ld:%ld", filedir [ed -> file],
				  y1+1, x1+1, y2+1, x2+1);
	Set_status_bar (ed->hwnd, s);
}

void	ClearStatusBar (EDIT_DATA * ed)
{
	if (!EditStatusBarLock)
		Clear_status_bar (ed->hwnd);
}

void	test_clipboard_enable (EDIT_DATA * ed, BOOL * copy, BOOL * paste, BOOL * exchange)
{
	int x1, y1, x2, y2;
	Editor_get_block (ed->editwnd, &x1, &y1, &x2, &y2);

	*copy = x1 != x2 || y1 != y2;
	*paste = FALSE;
	if (OpenClipboard (ed->editwnd)) {
		*paste = IsClipboardFormatAvailable (CF_TEXT);
		CloseClipboard();
	}
	*exchange = *copy && *paste;
}

void	cursor_moved (EDIT_DATA * ed, EDIT_NOTIFY * data)
{
	BOOL copy, paste, exchange;

	DrawSelectionBar (ed, 0);
	test_clipboard_enable (ed, &copy, &paste, &exchange);
	Enable_item (IDM_COPY, copy);
	Enable_item (IDM_CUT,  copy);
	Enable_item (IDM_ADD,  copy);
	Enable_item (IDM_EXCHANGE, exchange);
}

void	test_enable_paste (EDIT_DATA * ed)
{
	BOOL state = FALSE;
	if (OpenClipboard (ed->editwnd)) {
		state = IsClipboardFormatAvailable (CF_TEXT);
		CloseClipboard();
	}
	Enable_item (IDM_PASTE, state);
}

void	test_enable_undo (EDIT_DATA * ed)
{
	Enable_item (IDM_UNDO, tbuf_undo_available (ed->buf));
}

void	tab_pressed (EDIT_DATA * ed, EDIT_NOTIFY * data)
{
	char * p;
	int len, x;

	if (data->y1 == 0) return;
	if (!tbuf_get_line (ed->buf, data->y1-1, &p, &len))	return;
	x = Editor_log_to_phys (ed->editwnd, data->y1, data->x1);
	x = Editor_phys_to_log (ed->editwnd, data->y1-1, x);
	if (x >= len-1) return;
	while (x < len &&  p[x] != ' ' && p [x] != '\t') ++ x;
	while (x < len && (p[x] == ' ' || p [x] == '\t')) ++ x;
	data->result = Editor_log_to_phys (ed->editwnd, data->y1-1, x);
}

void	display_file_dirty (EDIT_DATA * ed)
{
	BOOL dirty;
	SendMessage (ed->hwnd, WM_MYMDI_TEXTCHANGED, 0, 0);
	dirty = tbuf_is_dirty (ed->buf);
	if (ed->file != NOFILE) Enable_item (IDM_SAVE, dirty);
}

void	e_handler (int code, EDIT_NOTIFY * data, DWORD param)
{
	EDIT_DATA * ed = (EDIT_DATA*) param;

	if (ed->editwnd == 0) return;
	switch (code)	{
	case EDIT_NOTIFY_CURSOR:
		if (data->has_focus)
			cursor_moved (ed, data);
		break;
	case EDIT_NOTIFY_FOCUS:
		if (data->has_focus)	{
			cursor_moved (ed, data);
			test_enable_paste (ed);
			test_enable_undo  (ed);
		}
		break;
	case EDIT_NOTIFY_TAB:
		tab_pressed (ed, data);
		break;
	case EDIT_NOTIFY_CLIPBOARD:
		test_enable_paste (ed);
		break;
	case EDIT_NOTIFY_DIRTY:
		display_file_dirty (ed);
		break;
	case EDIT_NOTIFY_UNDO:
		Enable_item (IDM_UNDO, data->result);
		break;
	case EDIT_NOTIFY_TRUNCUNDO:
		if (data->result) break;
		MsgBox ("Editor", MB_ICONEXCLAMATION | MB_OK,
			"Undo buffer full.\n"
			"Full undo will not be available now.");
		data->result = TRUE;
		break;
	case EDIT_NOTIFY_DROPUNDO:
		data->result = MsgBox ("Undo", MB_ICONEXCLAMATION | MB_OKCANCEL,
			"Undo buffer too small for current operation.\nDo you want to proceed without undo?") == IDOK;
		break;
	case EDIT_NOTIFY_SETBOOKMARK:
		DrawSelectionBar (ed, data->result);
		break;
	}
}

void	e_enable_items (EDIT_DATA * ed)
{
	Enable_item (IDM_RELOAD, ed->file != NOFILE);
	Enable_item (IDM_SAVE, ed->file == NOFILE || tbuf_is_dirty (ed->buf));
	Enable_item (IDM_SHARE, TRUE);
	Enable_item (IDM_SAVEAS, TRUE);
	Enable_item (IDM_CLOSE, TRUE);
	Enable_item (IDM_PRINT, TRUE);
	Enable_item (IDM_FIND, TRUE);
	Enable_item (IDM_FINDNEXT, TRUE);
	Enable_item (IDM_FINDPREV, TRUE);
	Enable_item (IDM_REPLACE, TRUE);
	Enable_item (IDM_REPLACENEXT, TRUE);
	Enable_item (IDM_GOTOLINE, TRUE);
}

void	Refresh_edits (void)
{
	int i;
	for (i = 0; i < n_editors; i++)
		Set_editor_params (editors [i]->editwnd);
}

void	Refresh_edit_tokens (void)
{
	int i;
	EDIT_DATA * ed;
	for (i = 0; i < n_editors; i++)	{
		ed = editors [i];
		Editor_set_lang_token (ed->editwnd, get_lang_token (ed->file ? filedir [ed->file] : NULL));
	}
}

BOOL	e_create (HWND hwnd)
{
	EDIT_DATA * ed;

	ed = Malloc (sizeof (EDIT_DATA));
	if (!ed) return -1;
	memset (ed, sizeof (EDIT_DATA), 0);
	ed -> hwnd = hwnd;
	ed -> file = EditCreateFile;
	ed -> buf  = EditCreateBuf;
	EditCreateFile = 0;
	EditCreateBuf = 0;
	ed->editwnd = Editor_create (ed->buf, hwnd);
	if (!ed -> editwnd)	{
		free (ed);
		return FALSE;
	}
	SetWindowLong (hwnd, 0, (DWORD) ed);
	Editor_set_notify_handler (ed->editwnd, e_handler, (DWORD) ed);
	add_editor (ed);
	Set_editor_params (ed->editwnd);
	Editor_set_lang_token (ed->editwnd, get_lang_token (ed->file ? filedir [ed->file] : NULL));
	Editor_set_insert (ed->editwnd, NewEditorInsert);
	e_enable_items (ed);
	return TRUE;
}

void	goto_line (EDIT_DATA * ed)
{
	int x1, x2, y1, y2;
	int line, count;

	Editor_get_block (ed->editwnd, &x1, &y1, &x2, &y2);
	line = y1+1;
	if (!Ask_number ("Enter line number:", "Go to line", &line,
			  ASK_INIT|ASK_SELECT|ASK_NUMBER|ASK_POSITIVE))
		return;
	count = Editor_nlines (ed->editwnd);
	if (line > count) line = count;
	if (line <= 0) line = 1;
	Editor_set_cursor (ed->editwnd, 0, line-1);
}

void	Keyword_help (EDIT_DATA * ed)
{
	char buf [2000];
	char * w;
	int n;

	w = current_word (ed, &n);
	if (n && w[0])	{
		if (n >= sizeof (buf)-1) n = sizeof (buf)-1;
		memcpy (buf, w, n);
		buf [n] = 0;
		if (Find_help_file ())
			WinHelp (FrameWindow, HelpFile, HELP_PARTIALKEY, (DWORD) buf);
	}
}

void	Search_current (EDIT_DATA * ed, BOOL down)
{
	char buf [2000];
	char * w;
	int n;

	w = current_word (ed, &n);
	if (n && w[0])	{
		if (n >= sizeof (buf)-1) n = sizeof (buf)-1;
		memcpy (buf, w, n);
		buf [n] = 0;
		do_search (ed, buf, NULL, TRUE, TRUE, TRUE, TRUE, TRUE, !down);
	}
}

#define IDME_SETBOOKMARK  10000
#define IDME_GOTOBOOKMARK 10020
#define IDME_REMOVEBOOKMARK 10040

void	right_button_menu (EDIT_DATA * ed, int x, int y)
{
	HMENU menu, setmenu, gotomenu, removemenu;
	POINT pt;
  BOOL copy, paste, exchange, oem = tbuf_is_oem (ed->buf);
	int i, n;
	char buf [2000];
	BOOL b;
	char * w;

	w = current_word (ed, &n);
	
	pt.x = x; pt.y = y;
	ClientToScreen (ed->editwnd, &pt);
	
	menu = CreatePopupMenu ();
  AppendMenu (menu, MF_STRING | (oem ? MF_UNCHECKED : MF_CHECKED),
              IDM_ENCODING_WINDOWS, "ANSII (Windows encoding)");
  AppendMenu (menu, MF_STRING | (oem ? MF_CHECKED : MF_UNCHECKED),
              IDM_ENCODING_DOS, "ASCII (DOS encoding)");
  AppendMenu (menu, MF_SEPARATOR, 0, 0);
  test_clipboard_enable (ed, &copy, &paste, &exchange);
	if (copy)  AppendMenu (menu, MF_STRING, IDM_COPY, "&Copy");
	if (copy)  AppendMenu (menu, MF_STRING, IDM_ADD, "&Add");
	if (paste) AppendMenu (menu, MF_STRING, IDM_PASTE, "&Paste");
	if (copy)   AppendMenu (menu, MF_STRING, IDM_CUT, "C&ut");
	if (exchange) AppendMenu (menu, MF_STRING, IDM_EXCHANGE, "&Exchange");
	if (copy || paste || exchange)
		AppendMenu (menu, MF_SEPARATOR, 0, 0);

	if (n && w[0])	{
		strcpy (buf, "Search down: ");
		if (n > 26)	{
			strncat (buf, w, 23);
			strcat (buf, "...");
		} else
			strncat (buf, w, n);
		AppendMenu (menu, MF_STRING, IDM_FINDSELNEXT, buf);
		strcpy (buf, "Search up: ");
		if (n > 29)	{
			strncat (buf, w, 26);
			strcat (buf, "...");
		} else
			strncat (buf, w, n);
		AppendMenu (menu, MF_STRING, IDM_FINDSELPREV, buf);
		AppendMenu (menu, MF_SEPARATOR, 0, 0);
	}
	
	setmenu = CreatePopupMenu ();	
	gotomenu = CreatePopupMenu ();	
	removemenu = CreatePopupMenu ();	
	for (i = 1; i <= 9; i++)	{
		b = Editor_test_bookmark (ed->editwnd, i);
		sprintf (buf, "&%d\tCtrl+Shift+%d", i, i);		
		if (b) strcat (buf, " *");
		AppendMenu (setmenu, MF_STRING, IDME_SETBOOKMARK+i, buf);
		if (b)	{
			sprintf (buf, "&%d\tCtrl+%d", i, i);
			AppendMenu (gotomenu, MF_STRING, IDME_GOTOBOOKMARK+i, buf);
			sprintf (buf, "&%d", i);
			AppendMenu (removemenu, MF_STRING, IDME_REMOVEBOOKMARK+i, buf);
		}
	}
	AppendMenu (menu, MF_POPUP,  (UINT) setmenu, "&Set bookmark");
	if (GetMenuItemCount (gotomenu))
		AppendMenu (menu, MF_POPUP,  (UINT) gotomenu, "&Go to bookmark");
	else
		DestroyMenu (gotomenu);
	if (GetMenuItemCount (removemenu))
		AppendMenu (menu, MF_POPUP,  (UINT) removemenu, "&Delete bookmark");
	else
		DestroyMenu (removemenu);
	AppendMenu (menu, MF_STRING, IDM_GOTOLINE, "Go to &line");	
	if (n && w[0])	{
		strcpy (buf, "Help on ");
		if (n > 32)	{
			strncat (buf, w, 29);
			strcat (buf, "...");
		} else
			strncat (buf, w, n);

		AppendMenu (menu, MF_SEPARATOR, 0, 0);
		AppendMenu (menu, MF_STRING, IDM_HELPKEYWORD, buf);
	}
	TrackPopupMenu (menu, TPM_LEFTALIGN, pt.x, pt.y, 0, ed->hwnd, 0);
	DestroyMenu (menu);
}

void	add_text_marks (EDIT_DATA * ed, char * s, int n)
{
	int i, num;
	BOOL dirty;
	char t [10];

	num = 0;
	for (i = 0; i < n_editors; i++)
		if (editors [i]->buf == ed->buf && editors [i]!=ed)
			++ num;
	dirty = tbuf_is_dirty (ed->buf);
	if (!num && !dirty) return;
	t [0] = ' '; t [1] = 0;
	if (num)	{
		num = 1;
		for (i = 0; i < n_editors && editors [i] != ed; i++)
			if (editors [i]->buf == ed->buf)
				++ num;
		sprintf (t+1, "#%d", num);
	}
	if (dirty) strcat (t, "*");
	strncat (s, t, n-strlen (s)-1);
}

BOOL	EditCommand (UINT cmd, EDIT_DATA * ed)
{
	switch (cmd)	{
  case IDM_ENCODING_DOS:
    Editor_encoding_DOS (ed->editwnd);
    return TRUE;
  case IDM_ENCODING_WINDOWS:
    Editor_encoding_Windows (ed->editwnd);
    return TRUE;
  case IDM_FLIPENCODING:
    Editor_flip_encoding (ed->editwnd);
    return TRUE;
  case IDM_UNDO:
		Editor_undo (ed->editwnd);
		return TRUE;
	case IDM_CUT:
		Editor_cut (ed->editwnd);
		return TRUE;
	case IDM_COPY:
		Editor_copy (ed->editwnd);
		return TRUE;
	case IDM_PASTE:
		Editor_paste (ed->editwnd);
		return TRUE;
	case IDM_ADD:
		Editor_add (ed->editwnd);
		return TRUE;
	case IDM_EXCHANGE:
		Editor_exchange (ed->editwnd);
		return TRUE;
	case IDM_RELOAD:
		Edit_reload (ed);
		break;
	case IDM_SAVE:
		EditSave (ed);
		return TRUE;
	case IDM_SAVEAS:
		EditSaveAs (ed);
		return TRUE;
	case IDM_SHARE:
		Edit_share (ed);
		return TRUE;
	case IDM_PRINT:
		Print_editor (ed->hwnd, ed->editwnd, ed->buf, ed->file);
		return TRUE;
	case IDM_FIND:
		Search (ed);
		return TRUE;
	case IDM_FINDNEXT:
		SearchNext (ed);
		return TRUE;
	case IDM_FINDPREV:
		SearchPrev (ed);
		return TRUE;
	case IDM_REPLACE:
		Replace (ed);
		return TRUE;
	case IDM_REPLACENEXT:
		ReplaceNext (ed);
		return TRUE;
	case IDM_GOTOLINE:
		goto_line (ed);
		return TRUE;
	case IDM_HELPKEYWORD:
		Keyword_help (ed);
		return TRUE;
	case IDM_FINDSELNEXT:
		Search_current (ed, TRUE);
		return TRUE;
	case IDM_FINDSELPREV:
		Search_current (ed, FALSE);
		return TRUE;
	default:
		if (cmd >= IDME_SETBOOKMARK+1 && cmd <= IDME_SETBOOKMARK+9) {
			Editor_set_bookmark (ed->editwnd, cmd - IDME_SETBOOKMARK);
			return TRUE;
		} else if (cmd >= IDME_GOTOBOOKMARK+1 && cmd <= IDME_GOTOBOOKMARK+9)	{
			Editor_goto_bookmark (ed->editwnd, cmd - IDME_GOTOBOOKMARK);
			return TRUE;
		} else if (cmd >= IDME_REMOVEBOOKMARK+1 && cmd <= IDME_REMOVEBOOKMARK+9)	{
			Editor_delete_bookmark (ed->editwnd, cmd - IDME_REMOVEBOOKMARK);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

LRESULT CALLBACK MDIEditProc (HWND hwnd, UINT msg,
			      WPARAM wparam, LPARAM lparam)
{
	EDIT_DATA * ed;
	int n, x1, x2, y1, y2;
	char * s;

	ed = (EDIT_DATA *) GetWindowLong (hwnd, 0);

	switch (msg) {
	case WM_CREATE:
		return e_create (hwnd) ? 0 : (LRESULT) -1;
	case WM_CLOSE:
		if (!check_file_save (ed, TRUE, TRUE))
			return 1;
		break;
	case WM_DESTROY:
		e_remove_errors (ed);
		remove_editor (ed);
		adjust_all_names (ed);
		ed -> editwnd = 0;
		break;
	case WM_QUERYENDSESSION:
		return check_file_save (ed, TRUE, TRUE);
	case WM_MYMDI_GETLONGTEXT:
		s = (char*) lparam;
		n = (int) wparam;
		if (! Test_special_title (ed->file, s, TRUE))	{
			strncpy (s, filedir [ed->file], n);
			s[n-1] = 0;
		}
		add_text_marks (ed, s, n);
		return 0;
	case WM_MYMDI_GETSHORTTEXT:
		s = (char*) lparam;
		n = (int) wparam;
		if (! Test_special_title (ed->file, s, FALSE))	{
			strncpy (s, name_only (filedir [ed->file]), n);
			s[n-1] = 0;
		}
		add_text_marks (ed, s, n);
		return 0;
	case MSG_GETSAVEDATA:
		Editor_get_block (ed->editwnd, &x1, &y1, &x2, &y2);
		sprintf ((char*) lparam, ",%ld,%ld,%ld,%ld", x1, y1, x2, y2);
		return 1;
	case MSG_GETCLASS:
		s = (char*) lparam;
		strcpy (s, "edit");
		return 0;
	case MSG_GETTITLE:
		if (!ed->file) return 0;
		s = (char*) lparam;
		n = (int) wparam;
		strncpy (s, filedir [ed->file], n-1);
		s [n-1] = 0;
		return 0;
	case WM_MDIACTIVATE:
		if (WND_TO_ACTIVATE == hwnd)	{
			store_name_var (ed -> file);
			e_enable_items (ed);
			Edit_drop_highlight ();
			SetFocus (ed->editwnd);
			DrawSelectionBar (ed, 0);
		} else	{
			store_name_var (-1);
			Disable_all_items ();
			ClearStatusBar (ed);
		}
		break;
	case WM_SETFOCUS:
		Edit_drop_highlight ();
		SetFocus (ed->editwnd);
		break;
	case WM_PARENTNOTIFY:
		if (LOWORD (wparam) == WM_RBUTTONDOWN)
			right_button_menu (ed, LOWORD (lparam), HIWORD (lparam));
		break;
	case WM_COMMAND:
		if (EditCommand (CONTROL_ID, ed))
			return 0;
		break;
	}
	return MyDefMDIChildProc (hwnd, msg, wparam, lparam);
}

EDIT_DATA * e_create_mdi (int file, TBUF * buf, MDIPLACEMENT * placement)
{
	HWND	hwnd;
	MDICREATESTRUCT mdi;
	
	mdi.szClass = EditClass;
	mdi.szTitle = "";
	mdi.hOwner = MyInstance;
	mdi.x = CW_USEDEFAULT;
	mdi.y = CW_USEDEFAULT;
	mdi.cx = CW_USEDEFAULT;
	mdi.cy = CW_USEDEFAULT;
	mdi.style = WS_OVERLAPPEDWINDOW;
	mdi.lParam = 0;

	EditCreateFile = file;
	EditCreateBuf  = buf;
	hwnd = (HWND) SendMessage (FrameWindow, WM_MDICREATE,
		(WPARAM) placement, (LPARAM) &mdi);

	if (!hwnd) return NULL;
	store_name_var (file);
	return (EDIT_DATA *) GetWindowLong (hwnd, 0);
}

void	e_activate (HWND hwnd)
{
	SendMessage (FrameWindow, WM_MDIACTIVATE, (WPARAM) hwnd, 0);
	if (IsIconic (hwnd))
		ShowWindow (hwnd, SW_RESTORE);
	SetFocus (hwnd);
}

HWND	e_create_new_mdi (MDIPLACEMENT * placement)
{
	TBUF * buf;
	EDIT_DATA * ed;

	buf = new_tbuf (TabSize);
	if (!buf) return 0;
	ed = e_create_mdi (NOFILE, buf, placement);
	if (!ed)	{
		free_tbuf (buf);
		return 0;
	}
	return ed->hwnd;
}

HWND	Edit_new (void)
{
	HWND hwnd = e_create_new_mdi (NULL);
	Edit_show (hwnd);
	return hwnd;
}

int	ask_make_new_file (char * name)
{
	int r;
	char fullname [STRLEN];

	r = MsgBox ("File open", MB_OKCANCEL | MB_ICONQUESTION,
		 "File %s not found.\nDo you want to create new file?", name);
	if (r == IDCANCEL) return -1;
	Redirect_create_file (name, fullname);
	return new_filedir (fullname);
}

EDIT_DATA * e_create_file (int file, char * origname, MDIPLACEMENT * placement, BOOL show)
{
	EDIT_DATA * ed;
	TBUF * buf;
	unsigned long len;
	int i, r;

	if (file != NOFILE)	{
		for (i = 0; i < n_editors; i++)
			if (editors [i]->file == file) break;
		if (i < n_editors)	{
			ed = e_create_mdi (file, editors [i]->buf, placement);
			if (!ed)	{
				MsgBox ("Share File", MB_ICONSTOP | MB_OK,
					"Can't create window for file %s.",
					filedir [ed->file]);
				return NULL;
			}
			Editor_set_read_only (ed->editwnd, Editor_get_read_only (editors [i]->editwnd));
			ed->time = editors [i]->time;
			if (show) Edit_show (ed->hwnd);
			adjust_all_names (ed);
			return ed;
		}
	}
read_again:
	buf = Read_file_to_buf (file, TRUE, &len);
	if (!buf)	{
		if (ErrCode == ERROR_FILE_NOT_FOUND)	{
			file = ask_make_new_file (origname ? origname : filedir [file]);
			if (file < 0) return NULL;
			buf = new_tbuf (TabSize);
			if (!buf)	{
				MsgBox ("Open File", MB_ICONSTOP | MB_OK,
					"Can't create buffer for file %s.",
					filedir [file]);
				return NULL;
			}
		} else	{
			r = MsgBox ("Open File", MB_ICONSTOP | MB_RETRYCANCEL,
				"Error opening file %s.\n%s",
				filedir [file], ErrText ());
			if (r == IDRETRY) goto read_again;
			return NULL;
		}
	}

	ed = e_create_mdi (file, buf, placement);
	if (!ed)	{
		free_tbuf (buf);
		MsgBox ("Open File", MB_ICONSTOP | MB_OK,
			"Can't create window for file %s.",
			filedir [file]);
		return NULL;
	}
	Get_file_time (filedir [file], &ed -> time);
	tbuf_set_dirty (ed->buf, FALSE);
	if (DemoMode && len >= MAX_EDIT_SIZE)
		Editor_set_read_only (ed->editwnd, TRUE);
	if (show) Edit_show (ed->hwnd);
	return ed;
}

void	Edit_set_buf (HWND hwnd, TBUF * buf)
{
	int i;
	TBUF * oldbuf;
	EDIT_DATA * ed = (EDIT_DATA *) GetWindowLong (hwnd, 0);
	oldbuf = ed->buf;
	for (i = 0; i < n_editors; i++)
		if (editors [i]->buf == oldbuf)	{
			Editor_set_buf (editors [i]->editwnd, buf);
			editors [i]->buf = buf;
		}
}

TBUF *	Edit_get_buf (HWND hwnd)
{
	EDIT_DATA * ed = (EDIT_DATA *) GetWindowLong (hwnd, 0);
	return ed->buf;
}

BOOL	e_reload (EDIT_DATA * ed)
{
	int i, r;
	TBUF * buf;
	unsigned long len;

read_again:
	buf = Read_file_to_buf (ed->file, TRUE, &len);
	if (!buf)	{
		r = MsgBox ("Open File", MB_ICONSTOP | MB_RETRYCANCEL,
			"Error reading file %s.\n%s",
			filedir [ed->file], ErrText ());
		if (r == IDRETRY) goto read_again;
		return FALSE;
	}
	for (i = 0; i < n_editors; i++)
		if (editors [i]->file == ed->file)	{
			Editor_set_buf (editors [i]->editwnd, buf);
			Editor_set_read_only (editors [i]->editwnd,
						DemoMode && len >= MAX_EDIT_SIZE);
			editors [i]->buf = buf;
			Get_file_time (filedir [ed->file], &editors [i]->time);
		}
	tbuf_set_dirty (buf, TRUE);
	tbuf_set_dirty (buf, FALSE);
	e_process_errors (ed->buf, ed->file);
	return TRUE;
}

HWND	Edit_read_text (int file, char * origname, int how, BOOL show)
{
	EDIT_DATA * ed;
	HWND hwnd;

	int r;

	ed = Edit_find_loaded_buf (file);
	if (ed)	{
		hwnd = ed->hwnd;

		if (how & OPEN_EXISTING_BUFFER)	{
			if (show) e_activate (hwnd);
			return hwnd;
		}
		if (how & SHARE_EXISTING_BUFFER)
			return Edit_share (ed);

		if (how & ASK_SHARE_EXISTING_BUFFER)	{
			r = MsgBox (ShellName, MB_YESNOCANCEL|MB_ICONQUESTION,
				    "File %s is already opened in the IDE.\n"
				    "Would you like to open another window with the\n"
				    "file?",
				    filedir [file]);
			if (r == IDCANCEL) return 0;
			if (r == IDYES)
				return Edit_share (ed);
		}

		if (tbuf_is_dirty (ed->buf))	{
			r = MsgBox (ShellName, MB_YESNOCANCEL|MB_ICONQUESTION,
				"File %s is opened for editing and modified.\n"
				"Would you like to reload it from disk?",
				filedir [file]);
			if (r == IDCANCEL) return 0;
			if (r == IDNO)	{
				if (show) e_activate (hwnd);
				return hwnd;
			}
			e_reload (ed);
		}
	} else	{
		ed = e_create_file (file, origname, NULL, show);
		if (!ed) return NULL;
		e_process_errors (ed->buf, file);
		hwnd = ed->hwnd;
	}
	if (hwnd && show)
		e_activate (hwnd);

	return hwnd;
}

void	Edit_show (HWND hwnd)
{
	if (!hwnd) return;
	SendMessage (FrameWindow, WM_MDIACTIVATE, (WPARAM) hwnd, 0);
}

HWND	Edit_share (EDIT_DATA * ed)
{
	EDIT_DATA * e = e_create_file (ed->file, filedir [ed->file], NULL, TRUE);
	return e ? e->hwnd : NULL;
}

void	Edit_reload (EDIT_DATA * ed)
{
	int r;
	if (ed->file <= 0) return;
	if (tbuf_is_dirty (ed->buf))	{
		r = MsgBox ("Reload file", MB_YESNO | MB_ICONQUESTION,
			"File %s is modified in the editor.\n"
			"Do you want to discard changes made in it\n"
			"and reload it from disk?",
			filedir [ed->file]);
		if (r == IDNO) return;
	}
	e_reload (ed);
}

HWND	e_create_window (char * title, MDIPLACEMENT * placement, char * extra)
{
	EDIT_DATA * ed;
	int file;
	int x1, y1, x2, y2;
	
	if (title && title [0])	{
		file = new_filedir (title);
		ed = e_create_file (file, title, placement, FALSE);
		if (!ed) return FALSE;
		x1 = atoi (get_word (&extra));
		y1 = atoi (get_word (&extra));
		x2 = atoi (get_word (&extra));
		y2 = atoi (get_word (&extra));
		Editor_set_block (ed->editwnd, x1, y1, x2, y2);
		return ed->hwnd;
	} else
		return e_create_new_mdi (placement);
}

void	e_test_reload  (EDIT_DATA * ed)
{
	int n;
	FILETIME t;

	if (ed->file == NOFILE)return;
	if (!ed -> time.dwLowDateTime && !ed -> time.dwHighDateTime)
		return;
	if (!Get_file_time (filedir [ed -> file], &t))
		return;
	if (!t.dwLowDateTime && !t.dwHighDateTime)
		return;
	if (t.dwLowDateTime == ed -> time.dwLowDateTime &&
	    t.dwHighDateTime == ed -> time.dwHighDateTime)
		return;
	if (tbuf_is_dirty (ed->buf))
		n = MsgBox ("Attention!", MB_OKCANCEL|MB_ICONEXCLAMATION, 
			    "File %s has been changed outside IDE.\n"
			    "It has been also modified from within IDE.\n"
			    "Reloading from disk would discard changes.\n"
			    "Would you like to reload it?", filedir [ed -> file]);
	else
		n = MsgBox ("Attention!", MB_OKCANCEL|MB_ICONEXCLAMATION, 
			    "File %s has been changed outside IDE.\n"
			    "Would you like to reload it?", filedir [ed -> file]);
	if (n == IDCANCEL)
		ed -> time = t;
	else
		e_reload (ed);
}

void	Edit_test_reload (BOOL prompt)
{
	int i;
	for (i = 0; i < n_editors; i++)
		e_test_reload (editors [i]);
}

BOOL	Register_edit (void)
{
	WNDCLASSEX wc;

	if (!Editor_register ()) return FALSE;
	
	wc.cbSize 	 = sizeof (WNDCLASSEX);
	wc.style 	 = 0;
	wc.lpfnWndProc   = (LPVOID) MDIEditProc;
	wc.hInstance     = MyInstance;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof (EDIT_DATA *);
	wc.hIcon 	 = LoadIcon (MyInstance, MAKEINTRESOURCE (EDIT_ICON));
	wc.hCursor 	 = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = 0;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = EditClass;
	wc.lpszMenuName  = NULL;
	wc.hIconSm 	 = EditSmIcon =
				LoadImage (MyInstance, MAKEINTRESOURCE (EDIT_ICON),
					   IMAGE_ICON,
					   GetSystemMetrics(SM_CXSMICON),
					   GetSystemMetrics(SM_CYSMICON),
					   0);	
	
	if (!RegisterClassEx (&wc)) return FALSE;

	Register_child_class ("edit", e_create_window);
	return TRUE;
}

