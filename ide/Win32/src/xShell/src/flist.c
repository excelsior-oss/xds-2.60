#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "mdi.h"
#include "frame.h"
#include "flist.h"
#include "edit.h"
#include "open.h"
#include "var.h"
#include "fcache.h"
#include "config.h"
#include "tools.h"
#include "open.h"
#include "res.h"

#define FListMDIClass "file-list"

#define SHORT_HEADER "Files"
#define VALID_HEADER "Project files list"
#define INVALID_HEADER "Project files list (not updated)"

HWND	FlistWnd = NULL;
HWND	FlistBox = NULL;
HWND	FCharBox  = NULL;

int *	filelist = 0;
int	cur_nfiles = 0;
int	alloc_nfiles = 0;
BOOL	files_valid = 0;
BOOL	files_error = FALSE;
BOOL	files_drawn = 0;

int	flist_extent = 0;

MDIPLACEMENT flist_placement;
BOOL	was_file_list = FALSE;

HBRUSH	flist_brush = 0;

HICON	FListSmIcon;

WNDPROC StdListBoxProc;

void	Clear_file_list (void)
{
	files_valid = FALSE;
	files_error = FALSE;
	cur_nfiles = 0;
	flist_extent = 0;
	if (FlistWnd)
		SendMessage (FlistWnd, WM_MYMDI_TEXTCHANGED, 0, 0);
	if (FlistBox)
		SendMessage (FlistBox, LB_RESETCONTENT, 0, 0);
	files_drawn = TRUE;
}

void	Add_file_list (int file)
{
	int i;
	int * f;

	for (i = 0; i < cur_nfiles; i++)
		if (filelist [i] == file) return;
	if (cur_nfiles == alloc_nfiles)	{
		f = realloc (filelist, (alloc_nfiles + 10) * sizeof (int));
		if (!f)	{
			files_error = TRUE;
			return;
		}
		alloc_nfiles += 10;
		filelist = f;
	}
	for (i = 0; i < cur_nfiles; i++)
		if (stricmp (filedir [file], filedir [filelist [i]]) < 0) break;
	memmove (filelist+i+1, filelist+i, (cur_nfiles-i)*sizeof (int));
	filelist [i] = file;
	files_drawn = FALSE;
	++ cur_nfiles;
}

void	Commit_file_list (void)
{
	files_valid = !files_error;
}

int	Get_file_list (int ** list)
{
	if (!list) return cur_nfiles;
	*list = NULL;
	if (!cur_nfiles) return 0;
	*list = Malloc (cur_nfiles * sizeof (int));
	if (!*list) return 0;
	memmove (*list, filelist, cur_nfiles * sizeof (int));
	return cur_nfiles;
}


void	Update_file_list (void)
{
	int i, x;
	if (! FlistBox) return;
	flist_extent = 0;
	SendMessage (FlistBox, LB_RESETCONTENT, 0, 0);
	for (i = 0; i < cur_nfiles; i++)	{
		SendMessage (FlistBox, LB_ADDSTRING, 0,
			     (LPARAM) filedir [filelist [i]]);
		x = text_width (NULL, filedir [filelist [i]]);
		if (x > flist_extent)
			flist_extent = x;
	}
	SendMessage (FlistBox, LB_SETHORIZONTALEXTENT, flist_extent, 0);
	files_drawn = TRUE;
	
	SendMessage (FlistWnd, WM_MYMDI_TEXTCHANGED, 0, 0);
	if (SendMessage (FlistBox, LB_GETCURSEL, 0, 0) == (LRESULT)-1)
		SendMessage (FlistBox, LB_SETCURSEL, 0, 0);
}

void	flist_dblclk (void)
{
	long sel;

	sel = SendMessage (FlistBox, LB_GETCARETINDEX, 0, 0);
	if (sel < 0 || sel >= cur_nfiles) return;
	Open_by_name (filedir [filelist [sel]], OPEN_EXISTING_BUFFER);
}

void	flist_selchange (void)
{
	long sel;

	sel = SendMessage (FlistBox, LB_GETCARETINDEX, 0, 0);
	if (sel < 0 || sel >= cur_nfiles)
		Update_tools_for_file ("");
	else
		Update_tools_for_file (filedir [filelist [sel]]);
}

void	copy_file_list (void)
{
	int i, len;
	HANDLE h;
	char * t;

	len = 1;
	if (! OpenClipboard (0))	{
		MsgBox ("Copy file list", MB_OK | MB_ICONEXCLAMATION,
			 "Can't open clipboard");
		return;
	}

	for (i = 0; i < cur_nfiles; i++)
		len += strlen (filedir [filelist [i]])+2;
	h = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, len);
	if (!h)	{
		CloseClipboard ();
		MsgBox ("Copy file list", MB_OK | MB_ICONEXCLAMATION,
			 "No enough memory for operation");
		return;
	}
	len = 0;
	t = GlobalLock (h);
	for (i = 0; i < cur_nfiles; i++)	{
		strcpy (t+len, filedir [filelist [i]]);
		len += strlen (filedir [filelist [i]]);
		strcpy (t+len, "\r\n");
		len += 2;
	}
	GlobalUnlock (h);
	EmptyClipboard ();
	SetClipboardData (CF_TEXT, h);
	CloseClipboard ();
}

int	flist_filter_index = 1;

BOOL	wr_file_list (int file)
{
	int i;

	if (!Put_file_start ())	return FALSE;

	for (i = 0; i < cur_nfiles; i++)
		if (!Put_file (filedir [filelist [i]], strlen (filedir [filelist [i]])))
			return FALSE;
	return Put_file_fin (file, TRUE);
}

void	save_file_list (void)
{
	char name [STRLEN];
	int file;

	if (!Browse_file_name (TRUE, "", "", 0, name, &flist_filter_index, EditCurDir))
		return;

	file = new_filedir (name);
	if (!wr_file_list (file))
		MsgBox ("Write file list", MB_OK,
			"There was an error writing file %s:\n%s", filedir [file], ErrText ());
}

void	Flist_enable_items (void)
{
	Enable_item (IDM_PASTE, FALSE);
	Enable_item (IDM_COPY, TRUE);
	Enable_item (IDM_CUT, FALSE);
	Enable_item (IDM_UNDO, FALSE);
	Enable_item (IDM_FIND, FALSE);
	Enable_item (IDM_FINDNEXT, FALSE);
	Enable_item (IDM_REPLACE, FALSE);
	Enable_item (IDM_REPLACENEXT, FALSE);
	Enable_item (IDM_SAVE, FALSE);
	Enable_item (IDM_CLOSE, FALSE);
	Enable_item (IDM_PRINT, FALSE);
	Enable_item (IDM_FIND, FALSE);
	Enable_item (IDM_FINDNEXT, FALSE);
	Enable_item (IDM_GOTOLINE, FALSE);
}

WNDPROC StdCharBoxProc;

LRESULT CALLBACK CharBoxProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_CHAR:
		if (wparam == VK_RETURN || wparam == VK_ESCAPE)	{
			DestroyWindow (hwnd);
			if (wparam == VK_RETURN)
				flist_dblclk ();
			return 0;
		}
		break;
	case WM_KILLFOCUS:
		DestroyWindow (hwnd);
		return 0;
	case WM_DESTROY:
		FCharBox = 0;
		break;
	}

	return CallWindowProc (StdCharBoxProc, hwnd, msg, wparam, lparam);
}

char * file_name_only (int i)
{
	char * p, * q;
	for (p = q = filedir [filelist [i]]; *q; q++)
		if (*q == '\\') p = q + 1;
	return p;
}

void alpha_char_pressed (int c)
{
	char t [2];
	int i;
	int x, y;
	RECT rc;
	LONG u = GetDialogBaseUnits ();
	x = LOWORD (u) * 100 / 4;
	y = HIWORD (u) * 13 / 8;
	GetClientRect (FlistBox, &rc);
	if (x < rc.right) x = rc.right;

	if (cur_nfiles <= 1) return;
	FCharBox = CreateWindow (
				"EDIT",
				NULL,
				WS_VISIBLE|WS_POPUP|WS_BORDER|
				ES_AUTOHSCROLL|ES_LOWERCASE,
				200, 200, x, y,
				FlistWnd,
				NULL,
				MyInstance,
				NULL
			);
	if (! FCharBox) return;
	StdCharBoxProc = Subclass_control (FCharBox, CharBoxProc);
	SendMessage (FCharBox, EM_LIMITTEXT, 100, 0);
	if (c == VK_SPACE)	{
		i = SendMessage (FlistBox, LB_GETCURSEL, 0, 0);
		if (i < 0) i = 0;
		SetWindowText (FCharBox, file_name_only (i));
		SendMessage (FCharBox, EM_SETSEL, 0, 100);
	} else	{
		t [0] = (char) c;
		t [1] = 0;
		SetWindowText (FCharBox, t);
		SendMessage (FCharBox, EM_SETSEL, 1, 1);
	}
}

void	charbox_changed (void)
{
	int i, j, k;
	int m, mpos;
	char * q;
	RECT rc, rect;
	POINT p;
	char s [110];
	if (!FCharBox)	return;

	GetWindowText (FCharBox, s, sizeof (s));
	if (!s[0])	{
		DestroyWindow (FCharBox);
		return;
	}
	i = SendMessage (FlistBox, LB_GETCURSEL, 0, 0);
	if (i<0) i = 0;
	j = i;
	mpos = i;
	m = 0;
	for (;;)	{
		q = file_name_only (j);
		for (k = 0; s[k] && tolower(q[k])==tolower(s[k]); k++);
		if (!s[k]) break;
		if (k>m)	{
			mpos = j;
			m = k;
		}
		j++;
		if (j==cur_nfiles) j = 0;
		if (j == i)	{
			long start, fin;
			s[m] = 0;
			SendMessage (FCharBox, EM_GETSEL, (WPARAM)&start, (LPARAM)&fin);
			if (start > m) start = m;
			if (fin > m) fin = m;
			SetWindowText (FCharBox, s);
			SendMessage (FCharBox, EM_SETSEL, start, fin);
			j = mpos;
			break;
		}
	}
	SendMessage (FlistBox, LB_SETCURSEL, j, 0);
	SendMessage (FlistBox, LB_GETITEMRECT, j, (LPARAM) &rc);
	GetClientRect (FCharBox, &rect);
	p.x = rc.left + 50;
	p.y = rc.top - rect.bottom;
	ClientToScreen (FlistBox, &p);
	SetWindowPos (FCharBox, 0, p.x, p.y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
}

void	update_flist_brush (void)
{
	COLORREF bk = AbsColor (ListBackColor, AbsSysColor (WndBackColor, COLOR_WINDOW));
	HBRUSH b = CreateSolidBrush (bk);
	if (!b) return;
	if (flist_brush) DeleteObject (flist_brush);
	flist_brush = b;
}

HWND	wnd_before_flist = 0;
void	Draw_window_label (HWND hwnd);

LRESULT CALLBACK MDIFileListProc (HWND hwnd, UINT msg,
				 WPARAM wparam, LPARAM lparam)
{
	RECT	rect;
	HDC	dc;
	int	c;
	char *	s;

	switch (msg) {
	case WM_CREATE:
		GetClientRect (hwnd, &rect);
		FlistBox = CreateWindow (
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
		if (! FlistBox)
			return -1;
		SendMessage (FlistBox, WM_SETFONT, (WPARAM) ListFont, 0);
		SendMessage (FlistBox, LB_SETHORIZONTALEXTENT, 10, 0);
		FlistWnd = hwnd;
		break;
	case WM_CLOSE:
		SendMessage (hwnd, WM_MYMDI_GETPLACEMENT, 0, (LPARAM) &flist_placement);
		was_file_list = TRUE;
		FlistWnd = FlistBox = 0;
		break;
	case MSG_GETCLASS:
		s = (char*) lparam;
		strcpy (s, FListMDIClass);
		return 0;
	case WM_MYMDI_GETLONGTEXT:
		s = (char*) lparam;
		strcpy (s, files_valid ? VALID_HEADER : INVALID_HEADER);
		return 0;
	case WM_MYMDI_GETSHORTTEXT:
		s = (char*) lparam;
		strcpy (s, SHORT_HEADER);
		return 0;
	case WM_SETFOCUS:
		if (SendMessage (FlistBox, LB_GETCURSEL, 0, 0) == LB_ERR &&
		    SendMessage (FlistBox, LB_GETCOUNT, 0, 0))
			SendMessage (FlistBox, LB_SETCURSEL, 0, 0);
		SetFocus (FlistBox);
		break;
	case WM_CTLCOLORLISTBOX:
		dc = (HDC) wparam;
		SetBkColor   (dc, AbsColor (ListBackColor, AbsSysColor (WndBackColor, COLOR_WINDOW)));
		SetTextColor (dc, AbsColor (ListForeColor, AbsSysColor (WndForeColor, COLOR_WINDOWTEXT)));
		return (LRESULT) flist_brush;
	case WM_VKEYTOITEM:
		c = LOWORD (wparam);
		if (c == VK_RETURN)
			flist_dblclk ();
		else if (c == VK_ESCAPE)
			SendMessage (FrameWindow, WM_MDIACTIVATE, (WPARAM) wnd_before_flist, 0);
		else if (c >= 'A' && c <= 'Z' ||
			 c >= '0' && c <= '9' ||
			 c == VK_SPACE)	{
				alpha_char_pressed (c);
				return -1;
			 }
		break;
	case WM_MDIACTIVATE:
		if (WND_TO_ACTIVATE == hwnd)	{
			if (WND_TO_DEACTIVATE)
				wnd_before_flist = WND_TO_DEACTIVATE;
			Flist_enable_items ();
			flist_selchange ();
		} else if (! WND_TO_ACTIVATE)
			Disable_all_items ();
		break;
	case WM_COMMAND:
		if (CONTROL_ID == IDM_COPY)
			copy_file_list ();
		else if (CONTROL_ID == IDM_SAVEAS)
			save_file_list ();
		else if (CONTROL_HWND == FlistBox && NOTIFY_CODE == LBN_DBLCLK)
			flist_dblclk ();
		else if (CONTROL_HWND == FlistBox && NOTIFY_CODE == LBN_SELCHANGE)
			flist_selchange ();
		else if (CONTROL_HWND == FCharBox && NOTIFY_CODE == EN_CHANGE)
			charbox_changed ();
		break;
	}
	return MyDefMDIChildProc (hwnd, msg, wparam, lparam);
}

HWND	create_file_list (char * title, MDIPLACEMENT * placement, char * extra)
{
	MDICREATESTRUCT	mdi;
	RECT r;

	update_flist_brush ();
	
	mdi.szClass = FileListClass;
	mdi.szTitle = 0;
	mdi.hOwner = MyInstance;
	mdi.style = WS_OVERLAPPEDWINDOW;
	mdi.lParam = 0;
	GetClientRect ((HWND) SendMessage (FrameWindow, WM_MYMDI_GETCLIENT, 0, 0), &r);
	mdi.x = 0;
	mdi.y = 0;
	mdi.cx = r.right / 3;
	mdi.cy = r.bottom * 3 / 4 - 1;
	if (placement)	{
		flist_placement = *placement;
		was_file_list = TRUE;
	}
	if (DockFList && ! was_file_list)	{
		memset (&flist_placement, 0, sizeof (flist_placement));
		flist_placement.type = DOCKED;
		flist_placement.dir  = DOCK_LEFT;
		flist_placement.docknum  = 1;
		was_file_list = TRUE;
	}

	SendMessage (FrameWindow, WM_MDICREATE,
		     was_file_list ? (LPARAM) &flist_placement : 0,
		     (LPARAM)(LPSTR)&mdi);
	if (! FlistWnd) return 0;
	Update_file_list ();
	return FlistWnd;
}

void	Show_file_list (void)
{
	if (FlistWnd)	{
		if (! files_drawn)
			Update_file_list ();
		SendMessage (FrameWindow, WM_MDIACTIVATE,
			     (WPARAM) FlistWnd, 0);
	} else
		create_file_list (0, 0, 0);
}

BOOL	Hide_file_list (void)
{
	if (!FlistWnd)
		return FALSE;
	SendMessage (FlistWnd, WM_CLOSE, 0, 0);
	return TRUE;
}


#define SECT_LEN 1024

void	Read_file_list (char * ini_file)
{
	int i, n;
	char * sect [SECT_LEN][2];

	memset (sect, 0, sizeof (sect));
	Read_section (sect, SECT_LEN, "Project files", ini_file);
	Clear_file_list ();
	for (i = 0; sect [i][0]; i++)	{
		if (sect [i][1] && sect [i][1][0])	{
			n = new_filedir (sect [i][1]);
			if (n >= 0) Add_file_list (n);
			free (sect [i][0]);
			free (sect [i][1]);
		}
	}
	Update_file_list ();
}

void	Write_file_list (char * ini_file)
{
	int i;
	char * sect [SECT_LEN][2];

	memset (sect, 0, sizeof (sect));
	for (i = 0; i < SECT_LEN && i < cur_nfiles; i++)	{
		sect [i][0] = "file";
		sect [i][1] = filedir [filelist [i]];
	}
	Write_section (sect, SECT_LEN, "Project files", ini_file);
}

void	Refresh_file_list (void)
{
	int i, x;
	SendMessage (FlistBox, WM_SETFONT, (WPARAM) ListFont, 0);
	update_flist_brush ();

	flist_extent = 0;
	for (i = 0; i < cur_nfiles; i++)	{
		x = text_width (ListFont, filedir [filelist [i]]);
		if (x > flist_extent)
			flist_extent = x;
	}
	SendMessage (FlistBox, LB_SETHORIZONTALEXTENT, flist_extent, 0);

	InvalidateRect (FlistBox, 0, 1);
}

BOOL	Register_file_list (void)
{
	WNDCLASSEX wc;

	wc.cbSize 	 = sizeof (WNDCLASSEX);
	wc.style	 = 0;
	wc.lpfnWndProc   = (LPVOID) MDIFileListProc;
	wc.hInstance     = MyInstance;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hIcon 	 = LoadIcon (MyInstance, MAKEINTRESOURCE (FLIST_ICON));
	wc.hCursor 	 = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = 0;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = FileListClass;
	wc.lpszMenuName  = NULL;
	wc.hIconSm 	 = FListSmIcon =
				LoadImage (MyInstance, MAKEINTRESOURCE (FLIST_ICON),
					   IMAGE_ICON,
					   GetSystemMetrics(SM_CXSMICON),
					   GetSystemMetrics(SM_CYSMICON),
					   0);	
	if (!RegisterClassEx (&wc)) return FALSE;
	Register_child_class (FListMDIClass, create_file_list);
	
	return TRUE;
}
