#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "mdi.h"
#include "frame.h"
#include "edit.h"
#include "fcache.h"
#include "tbuf.h"
#include "find.h"
#include "flist.h"
#include "config.h"
#include "open.h"
#include "red.h"
#include "res.h"
#include <process.h>

HBRUSH	search_brush = 0;

BOOL	MakeNewSearchWindow = FALSE;

#define SearchListMDIClass "search-results"

MDIPLACEMENT search_placement;
BOOL	was_search_list = FALSE;

char msearch_text [300];

Expr *	RegExpr;

int	* search_filelist = NULL;
int	search_nfiles;
int	search_a_nfiles;
int	* search_result = NULL;
int	search_count;

#define ERROR_TITLE "Search in Files"

typedef struct	{
			int	nfiles;
			int *	files;
			HWND	hwnd;
			HWND	listwnd;
			char *	pattern;
			BOOL	search_case;
			BOOL	search_prefix;
			BOOL	search_suffix;
		}
			SEARCH_LIST_DATA;

void	Search_in_window (HWND editwnd,
			  char search_text [],
			  BOOL search_case,
			  BOOL search_prefix,
			  BOOL search_suffix);


void	searchlist_dblclk (SEARCH_LIST_DATA * d)
{
	int k, sel;
	HWND hwnd;

	sel = SendMessage (d->listwnd, LB_GETCARETINDEX, 0, 0);
	if (sel < 0 || sel >= d->nfiles) return;
	k = (int) SendMessage (d->listwnd, LB_GETITEMDATA, sel, 0);
	hwnd = Open_by_name (filedir [d->files [k]], OPEN_EXISTING_BUFFER);
	if (!hwnd) return;
	if (d->pattern && d->pattern [0])
		Search_in_window (hwnd, d->pattern,
				  d->search_case,
				  d->search_prefix,
				  d->search_suffix);
}

void	update_search_brush (void)
{
	COLORREF bk = AbsColor (ListBackColor, AbsSysColor (WndBackColor, COLOR_WINDOW));
	HBRUSH b = CreateSolidBrush (bk);
	if (!b) return;
	if (search_brush) DeleteObject (search_brush);
	search_brush = b;
}

LRESULT CALLBACK MDISearchListProc (HWND hwnd, UINT msg,
				    WPARAM wparam, LPARAM lparam)
{
	RECT	rect;
	HDC	dc;
	int	k, l, c;
	char *	s;
	SEARCH_LIST_DATA * d;

	d = (SEARCH_LIST_DATA *) GetWindowLong (hwnd, 0);
	switch (msg) {
	case WM_CREATE:
		d = Malloc (sizeof (SEARCH_LIST_DATA));
		if (!d) return (LRESULT) -1;
		memset (d, 0, sizeof (SEARCH_LIST_DATA));
		d->hwnd = hwnd;

		GetClientRect (hwnd, &rect);
		d->listwnd = CreateWindow (
				"LISTBOX",
				NULL,
				WS_CHILD | WS_VISIBLE |
				WS_HSCROLL | WS_VSCROLL |
				LBS_NOTIFY | LBS_HASSTRINGS |
				LBS_NOINTEGRALHEIGHT|
				LBS_WANTKEYBOARDINPUT,
				0,
				0,
				rect.right-rect.left,
				rect.bottom-rect.top,
				hwnd,
				NULL,
				MyInstance,
				NULL
			);
		if (! d->listwnd)
			return -1;
		SetWindowLong (hwnd, 0, (LONG) d);
		break;
	case MSG_GETCLASS:
		s = (char*) lparam;
		strcpy (s, SearchListMDIClass);
		return 0;
	case WM_MYMDI_GETLONGTEXT:
		s = (char*) lparam;
		l = wparam;
		strcpy (s, "Search results: \"");
		k = strlen (s);
		s += k; l -= k;
		strncpy (s, d->pattern ? d->pattern : "", l-3);
		s[l-3] = 0;
		strcat (s, "\"");
		return 0;
	case WM_MYMDI_GETSHORTTEXT:
		s = (char*) lparam;
		strcpy (s, "Search results");
		return 0;
	case WM_SETFOCUS:
		if (SendMessage (d->listwnd, LB_GETCURSEL, 0, 0) == LB_ERR &&
		    SendMessage (d->listwnd, LB_GETCOUNT, 0, 0))
			SendMessage (d->listwnd, LB_SETCURSEL, 0, 0);
		SetFocus (d->listwnd);
		break;
	case WM_CTLCOLORLISTBOX:
		dc = (HDC) wparam;
		SetBkColor   (dc, AbsColor (ListBackColor, AbsSysColor (WndBackColor, COLOR_WINDOW)));
		SetTextColor (dc, AbsColor (ListForeColor, AbsSysColor (WndForeColor, COLOR_WINDOWTEXT)));
		return (LRESULT) search_brush;
	case WM_VKEYTOITEM:
		c = LOWORD (wparam);
		if (c == VK_RETURN)
			searchlist_dblclk (d);
		else if (c == VK_ESCAPE)
			PostMessage (hwnd, WM_CLOSE, 0, 0);
/*		else if (c >= 'A' && c <= 'Z' ||
			 c >= '0' && c <= '9' ||
			 c == VK_SPACE)	{
				alpha_char_pressed (c);
				return -1;
			 }
*/
		break;
	case WM_MDIACTIVATE:
/*
		if (WND_TO_ACTIVATE == hwnd)	{
			Flist_enable_items ();
			flist_selchange ();
		} else if (! WND_TO_ACTIVATE)
			Disable_all_items ();
*/
		break;
	case WM_COMMAND:
/*		if (CONTROL_ID == IDM_COPY)
			copy_file_list ();
		else if (CONTROL_ID == IDM_SAVEAS)
			save_file_list ();
*/
		if (CONTROL_HWND == d->listwnd && NOTIFY_CODE == LBN_DBLCLK)
			searchlist_dblclk (d);
/*		else if (CONTROL_HWND == FCharBox && NOTIFY_CODE == EN_CHANGE)
			charbox_changed ();
*/
		break;
	}
	return MyDefMDIChildProc (hwnd, msg, wparam, lparam);
}

BOOL	fill_search_list (HWND hwnd)
{
	int i, k;
	SEARCH_LIST_DATA * d = (SEARCH_LIST_DATA *) GetWindowLong (hwnd, 0);

	d->files = Malloc (search_count * sizeof (int));
	if (!d->files) return FALSE;
	d->nfiles = search_count;
	memcpy (d->files, search_result, search_count * sizeof (int));
	d->pattern = alloc_copy (msearch_text);
	if (!d->pattern) return FALSE;;
	d->search_case = search_case;
	d->search_prefix = search_prefix;
	d->search_suffix = search_suffix;

	SendMessage (d->listwnd, LB_RESETCONTENT, 0, 0);
	SendMessage (d->listwnd, WM_SETFONT, (WPARAM) ListFont, 0);
	SendMessage (d->listwnd, LB_SETHORIZONTALEXTENT, 10, 0);
	for (i = 0; i < d->nfiles; i++)	{
		k = (int) SendMessage (d->listwnd, LB_ADDSTRING, 0, (LPARAM) filedir [d->files [i]]);
		SendMessage (d->listwnd, LB_SETITEMDATA, k, i);
	}
	return TRUE;
}

void	clear_search_list (HWND hwnd)
{
	SEARCH_LIST_DATA * d = (SEARCH_LIST_DATA *) GetWindowLong (hwnd, 0);

	free (d->files);
	d->files = NULL;
	d->nfiles = 0;
	free (d->pattern);
	d->pattern = NULL;

	SendMessage (d->listwnd, LB_RESETCONTENT, 0, 0);
	SendMessage (d->listwnd, LB_SETHORIZONTALEXTENT, 10, 0);
}

HWND	create_search_list (char * title, MDIPLACEMENT * placement, char * extra)
{
	MDICREATESTRUCT	mdi;
	RECT r;

	update_search_brush ();

	mdi.szClass = SearchListClass;
	mdi.szTitle = 0;
	mdi.hOwner = MyInstance;
	mdi.style = WS_OVERLAPPEDWINDOW;
	mdi.lParam = 0;
	GetClientRect ((HWND) SendMessage (FrameWindow, WM_MYMDI_GETCLIENT, 0, 0), &r);
	mdi.x = r.right - r.right / 3;
	mdi.y = 0;
	mdi.cx = r.right / 3;
	mdi.cy = r.bottom * 3 / 4 - 1;

	if (placement)	{
		search_placement = *placement;
		was_search_list = TRUE;
	}
	if (DockSearchResults && ! was_search_list)	{
		memset (&search_placement, 0, sizeof (search_placement));
		search_placement.type = DOCKED;
		search_placement.dir  = DOCK_RIGHT;
		search_placement.docknum  = 1;
		was_search_list = TRUE;
	}

	return (HWND) SendMessage (FrameWindow, WM_MDICREATE,
			was_search_list ? (LPARAM) &search_placement : 0,
		     (LPARAM)(LPSTR)&mdi);
}

void	enable_msearch_items (HWND hwnd)
{
	BOOL b = IsDlgButtonChecked (hwnd, MSEARCH_DISK);
	EnableWindow (GetDlgItem (hwnd, MSEARCH_DIR), b);
	EnableWindow (GetDlgItem (hwnd, MSEARCH_SUBDIR), b);
	EnableWindow (GetDlgItem (hwnd, MSEARCH_BROWSE), b);
//	b = GetWindowTextLength (GetDlgItem (hwnd, MSEARCH_TEXT)) ||
//	    GetWindowTextLength (GetDlgItem (hwnd, MSEARCH_MASK));
//	EnableWindow (GetDlgItem (hwnd, IDOK), b);
}

BOOL	ckeck_valid_mask (HWND hwnd)
{
	char buf [STRLEN];
	int r, l;

	GetDlgItemText (hwnd, MSEARCH_MASK, buf, sizeof (msearch_mask));
	if (!buf[0])
		RegExpr = NULL;
	else	{
		r = compile_expr (buf, &RegExpr);
		if (r <= 0)	{
			MsgBox (ERROR_TITLE, MB_ICONEXCLAMATION | MB_OK, "Error in file mask");
			r = -r;
			l = strlen (buf);
			if (r > l) r = l;
			SendDlgItemMessage (hwnd, MSEARCH_MASK, EM_SETSEL, r, r);
			SetFocus (GetDlgItem (hwnd, MSEARCH_MASK));
			return FALSE;
		}
	}
	return TRUE;
}

BOOL	check_valid_dir (HWND hwnd)
{
	char buf [STRLEN], buf1 [STRLEN];
	DWORD attr, e;
	WIN32_FIND_DATA d;
	HANDLE h;

	if (!IsDlgButtonChecked (hwnd, MSEARCH_DISK)) return TRUE;
	GetDlgItemText (hwnd, MSEARCH_DIR, buf, sizeof (buf));
	if (!buf[0]) return TRUE;
	attr = GetFileAttributes (buf);
	if (attr == 0xFFFFFFFF)	{
		e = GetLastError ();
		switch (e)	{
		case ERROR_PATH_NOT_FOUND:
		case ERROR_FILE_NOT_FOUND:
			MsgBox (ERROR_TITLE, MB_OK | MB_ICONEXCLAMATION,
				"Error: directory \"%s\" does not exist", buf);
			break;
		case ERROR_INVALID_NAME:
			MsgBox (ERROR_TITLE, MB_OK | MB_ICONEXCLAMATION,
				"Error: \"%s\" isn't valid directory name", buf);
			break;
		default:
			MsgBox (ERROR_TITLE, MB_OK | MB_ICONEXCLAMATION,
				"Error opening directory \"%s\": %s", buf, EText (e));
			break;
		}
		return FALSE;
	}
	if (! (attr & FILE_ATTRIBUTE_DIRECTORY))	{
		MsgBox (ERROR_TITLE, MB_OK | MB_ICONEXCLAMATION,
			"Error: \"%s\" is not a directory", buf);
		return FALSE;
	}
	strcpy (buf1, buf);
	strcat (buf1, "\\*");
	h = FindFirstFile (buf1, &d);
	if (h==INVALID_HANDLE_VALUE)	{
		MsgBox (ERROR_TITLE, MB_OK | MB_ICONEXCLAMATION,
			"Error reading directory \"%s\": %s", buf, LastErrText ());
		return FALSE;
	}
	FindClose (h);
	return TRUE;
}

BOOL CALLBACK MSearchDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char buf [STRLEN];
	int r;
	BOOL open, proj, disk;

	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		Fill_history_box (GetDlgItem (hwnd, MSEARCH_TEXT),
				  search_history, SEARCH_HISTORY_LEN);
		SendDlgItemMessage (hwnd, MSEARCH_TEXT, EM_LIMITTEXT,
				    sizeof (msearch_text)-1, 0);
		CheckDlgButton (hwnd, MSEARCH_CASE, search_case);
		CheckDlgButton (hwnd, MSEARCH_ANYTEXT, !search_prefix && !search_suffix);
		CheckDlgButton (hwnd, MSEARCH_PREFIX,   search_prefix && !search_suffix);
		CheckDlgButton (hwnd, MSEARCH_SUFFIX,  !search_prefix &&  search_suffix);
		CheckDlgButton (hwnd, MSEARCH_WHOLEWORD,search_prefix &&  search_suffix);
		CheckDlgButton (hwnd, MSEARCH_INSERTCURRENT, search_insert_current);
		CheckDlgButton (hwnd, MSEARCH_NEWWINDOW, MakeNewSearchWindow);

		open = msearch_in_opened;
		proj = msearch_in_project;
		disk = msearch_in_files;

		r = Make_editor_list (NULL);
		EnableWindow (GetDlgItem (hwnd, MSEARCH_OPEN), r>0);
		if (open && !r)	{open = FALSE; proj = TRUE;}

		r = Get_file_list (NULL);
		EnableWindow (GetDlgItem (hwnd, MSEARCH_PROJFILES), r>0);
		if (proj && !r)	{proj = FALSE; disk = TRUE;}
		
		CheckDlgButton (hwnd, MSEARCH_OPEN, open);
		CheckDlgButton (hwnd, MSEARCH_PROJFILES, proj);
		CheckDlgButton (hwnd, MSEARCH_DISK, disk);
#if 0
		w = current_word (search_ed, &n);
		if (n && w[0] && search_insert_current)	{
			if (n >= sizeof (search_text)-1) n = sizeof (search_text)-1;
			memcpy (search_text, w, n);
			search_text [n] = 0;
		}
#endif
		SetDlgItemText (hwnd, MSEARCH_TEXT, msearch_text);
		SetDlgItemText (hwnd, MSEARCH_MASK, msearch_mask);
		CheckDlgButton (hwnd, MSEARCH_SUBDIR, msearch_subdir);
		if (msearch_path [0])
			SetDlgItemText (hwnd, MSEARCH_DIR,  msearch_path);
		else	{
			GetCurrentDirectory (sizeof (buf), buf);
			SetDlgItemText (hwnd, MSEARCH_DIR,  buf);
		}

		enable_msearch_items (hwnd);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			if (! check_valid_dir (hwnd))	{
				SetFocus (GetDlgItem (hwnd, MSEARCH_DIR));
				return 1;
			}
			if (! ckeck_valid_mask (hwnd)) return 1;
		
			search_case = IsDlgButtonChecked (hwnd, MSEARCH_CASE);
			if (IsDlgButtonChecked (hwnd, MSEARCH_ANYTEXT))
				search_prefix = search_suffix = FALSE;
			else if (IsDlgButtonChecked (hwnd, MSEARCH_PREFIX))	{
				search_prefix = TRUE; search_suffix = FALSE;
			} else if (IsDlgButtonChecked (hwnd, MSEARCH_SUFFIX))	{
				search_prefix = FALSE; search_suffix = TRUE;
			} else
				search_prefix = search_suffix = TRUE;
			GetDlgItemText (hwnd, MSEARCH_TEXT, msearch_text, sizeof (msearch_text));
			GetDlgItemText (hwnd, MSEARCH_MASK, msearch_mask, sizeof (msearch_mask));
			GetDlgItemText (hwnd, MSEARCH_DIR,  msearch_path, sizeof (msearch_path));
			Insert_history (search_history, SEARCH_HISTORY_LEN, msearch_text);
			msearch_in_opened = IsDlgButtonChecked (hwnd, MSEARCH_OPEN);
			msearch_in_project= IsDlgButtonChecked (hwnd, MSEARCH_PROJFILES);
			msearch_in_files  = IsDlgButtonChecked (hwnd, MSEARCH_DISK);
			msearch_subdir = IsDlgButtonChecked (hwnd, MSEARCH_SUBDIR);
			MakeNewSearchWindow = IsDlgButtonChecked (hwnd, MSEARCH_NEWWINDOW);
			Write_search_settings ();
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (MSEARCH_DIALOG);
		case MSEARCH_BROWSE:
			GetDlgItemText (hwnd, MSEARCH_DIR, buf, sizeof (buf));
			if (!buf[0])
				GetCurrentDirectory (sizeof (buf), buf);
			if (Browse_for_directory ("Select directory to search in", buf, buf))
				SetDlgItemText (hwnd, MSEARCH_DIR, buf);
			break;
		case MSEARCH_INSERTCURRENT:
			if (NOTIFY_CODE == BN_CLICKED)	{
				search_insert_current = IsDlgButtonChecked (hwnd, MSEARCH_INSERTCURRENT);
#if 0
				w = current_word (search_ed, &n);
				if (n && w[0] && search_insert_current)	{
					if (n >= sizeof (search_text)-1) n = sizeof (search_text)-1;
					memcpy (search_text, w, n);
					search_text [n] = 0;
					SetDlgItemText (hwnd, MSEARCH_TEXT, search_text);
				}
#endif
			}
			break;
		}
		enable_msearch_items (hwnd);
		return 1;
	case WM_HELP:
		return Control_help (MSEARCH_DIALOG, lparam);
	}
	return 0;
}

int	files_looked;
BOOL	Search_cancelled;
HWND	Cancel_dialog_hwnd;
HANDLE	Ready_event;

BOOL CALLBACK SearchCancelDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		Cancel_dialog_hwnd = hwnd;
		SetEvent (Ready_event);
		return 1;
	case WM_COMMAND:
		if (CONTROL_ID == IDCANCEL)
			Search_cancelled = TRUE;
		else if (CONTROL_ID == IDOK)
			EndDialog (hwnd, 1);
		return 1;
	}
	return 0;
}

void	CancelDialogThreadProc (void * param)
{
	DialogBox (MyInstance, MAKEINTRESOURCE (MSEARCH_CANCEL_DIALOG), 0, SearchCancelDlgProc);
}

void	open_cancel_dialog (void)
{
	Ready_event = CreateEvent (0, 0, 0, 0);
	_beginthread (CancelDialogThreadProc, 0, 0);
}

void	close_cancel_dialog (void)
{
	SendMessage (Cancel_dialog_hwnd, WM_COMMAND, IDOK, 0);
	Cancel_dialog_hwnd = 0;
	CloseHandle (Ready_event);
	Ready_event = 0;
}

BOOL	search_one_file (char * name)
{
	FILEMAP map;
	EDIT_DATA * ed;
	BOOL b;
	int r;

	if (RegExpr && !Match (RegExpr, name_only (name)))
		return FALSE;

	++ files_looked;
	
	SetDlgItemInt (Cancel_dialog_hwnd, MSEARCH_CANCEL_NFILES, files_looked, 0);
	SetDlgItemText (Cancel_dialog_hwnd, MSEARCH_CANCEL_CURFILE, name_only (name));
	if (!msearch_text [0]) return TRUE;

	ed = Edit_find_loaded_file_name (name);
	if (ed)	return Edit_test_line_found (ed, msearch_text,
						 search_case,
						 search_prefix,
						 search_suffix);
	else	{
try_again:	if (! Load_file (name, &map, FALSE))	{
			r = MsgBox (ERROR_TITLE, MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION,
				"Error reading file %s:\n%s",
				name, LastErrText ());
			if (r == IDRETRY) goto try_again;
			if (r == IDABORT) Search_cancelled = TRUE;
			return FALSE;
		}

		b = tbuf_file_word_search (map.ptr, map.len,
					   msearch_text, !search_case,
					   search_prefix, search_suffix);
		Unload_file (&map);
		return b;
	}

}

void	add_found_file (int file)
{
	int * p;
	set_real_file_name (filedir [file]);
	if (search_count == search_a_nfiles)	{
		p = realloc (search_result, (search_a_nfiles + 100) * sizeof (int));
		if (!p) return;
		search_result = p;
		search_a_nfiles += 100;
	}
	search_result [search_count++] = file;
	SetDlgItemInt (Cancel_dialog_hwnd, MSEARCH_CANCEL_NFOUND, search_count, 0);
}

BOOL	recursive_search_files (char * dir)
{
	char buf [STRLEN];
	WIN32_FIND_DATA d;
	HANDLE h;
	DWORD attr;
	int r;

	strcpy (buf, dir);
	strcat (buf, "\\*");
	h = FindFirstFile (buf, &d);
	if (h == INVALID_HANDLE_VALUE) return FALSE;
	SetDlgItemText (Cancel_dialog_hwnd, MSEARCH_CANCEL_CURDIR, dir);
	do	{
		if (!strcmp (d.cFileName, ".") || !strcmp (d.cFileName, ".."))
			continue;
		strcpy (buf, dir);
		strcat (buf, "\\");
		strcat (buf, d.cFileName);
		attr = GetFileAttributes (buf);
		if (attr == 0xFFFFFFFF) continue;
		if (attr & FILE_ATTRIBUTE_DIRECTORY)	{
try_again:		if (msearch_subdir && !recursive_search_files (buf))	{
				r = MsgBox (ERROR_TITLE, MB_ICONEXCLAMATION | MB_ABORTRETRYIGNORE,
					"Can't open directory \"%s\":\n %s",
					buf, LastErrText ());
				if (r == IDRETRY) goto try_again;
				if (r == IDABORT) Search_cancelled = TRUE;
			}
			continue;
		}
		if (search_one_file (buf))
			add_found_file (new_filedir (buf));
	}
		while (!Search_cancelled && FindNextFile (h, &d));
	FindClose (h);
	return TRUE;
}

BOOL	find_in_disk_files (void)
{
	char * p = msearch_path[0] ? msearch_path : ".";
	if (!recursive_search_files (p))	{
		MsgBox (ERROR_TITLE, MB_ICONEXCLAMATION | MB_OK,
			"Can't open directory \"%s\":\n %s",
			p, LastErrText ());
		return FALSE;
	}
	return TRUE;
}

HWND	CurSearchWindow;

BOOL	SearchResultEnum (int num, HWND hwnd, BOOL free, BOOL docked, BOOL active, BOOL mdiactive)
{
	IGNORE_PARAM (num);
	IGNORE_PARAM (free);
	IGNORE_PARAM (docked);
	IGNORE_PARAM (active);
	IGNORE_PARAM (mdiactive);
	if (is_of_class (hwnd, SearchListClass))	{
		CurSearchWindow = hwnd;
		return FALSE;
	}
	return TRUE;
}

HWND	Find_search_result (void)
{
	CurSearchWindow = NULL;
	SendMessage (FrameWindow, WM_MYMDI_ENUMZORDER, 0, (LPARAM) SearchResultEnum);
	return CurSearchWindow;
}

void	Files_search (void)
{
	int i, r;
	int * list;
	int n;
	HWND hwnd;

	RegExpr = NULL;
	r = Dialog (MSEARCH_DIALOG, MSearchDlgProc);
	if (!r)	return;

	files_looked = 0;
	search_a_nfiles = 100;
	search_result = Malloc (100 * sizeof (int));
	search_count = 0;
	Search_cancelled = FALSE;

	open_cancel_dialog ();
	if (WaitForSingleObject (Ready_event, 10000) == WAIT_TIMEOUT)	{
		FreeExpr (RegExpr);
		free (search_result);
		close_cancel_dialog ();
		return;
	}

	if (msearch_in_files)	{
		if (!find_in_disk_files ())	{
			FreeExpr (RegExpr);
			free (search_result);
			close_cancel_dialog ();
			return;
		}
	} else	{
		if (msearch_in_opened)
			n = Make_editor_list (&list);
		else if (msearch_in_project)
			n = Get_file_list (&list);
		else
			return;
		for (i = 0; i < n && !Search_cancelled; i++)
			if (search_one_file (filedir [list [i]]))
				add_found_file (list [i]);
	}
	FreeExpr (RegExpr);
	close_cancel_dialog ();

	if (msearch_text [0] && !files_looked)	{
		MsgBox (ERROR_TITLE, MB_OK | MB_ICONEXCLAMATION,
			"No files to search in");
		free (search_result);
		return;
	}
	if (!search_count)	{
		MsgBox (ERROR_TITLE, MB_OK | MB_ICONEXCLAMATION,
			"No files found");
		free (search_result);
		return;
	}
	hwnd = MakeNewSearchWindow ? NULL : Find_search_result ();
	if (!hwnd) hwnd = create_search_list ("", 0, 0);
	fill_search_list (hwnd);
	free (search_result);
	if (search_count)
		strcpy (search_text, msearch_text);
}

BOOL	tbuf_file_word_search (char * buf, int len,
			       char * string, BOOL ignorecase, BOOL prefix, BOOL suffix);

BOOL	Register_search_list (void)
{
	WNDCLASSEX wc;

	wc.cbSize 	 = sizeof (WNDCLASSEX);
	wc.style	 = 0;
	wc.lpfnWndProc   = (LPVOID) MDISearchListProc;
	wc.hInstance     = MyInstance;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof (SEARCH_LIST_DATA);
	wc.hIcon 	 = LoadIcon (MyInstance, MAKEINTRESOURCE (FLIST_ICON));
	wc.hCursor 	 = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = 0;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = SearchListClass;
	wc.lpszMenuName  = NULL;
	wc.hIconSm 	 = 
				LoadImage (MyInstance, MAKEINTRESOURCE (FLIST_ICON),
					   IMAGE_ICON,
					   GetSystemMetrics(SM_CXSMICON),
					   GetSystemMetrics(SM_CYSMICON),
					   0);	
	if (!RegisterClassEx (&wc)) return FALSE;
	Register_child_class (SearchListMDIClass, create_search_list);
	
	return TRUE;
}

