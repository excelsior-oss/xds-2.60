#include "util.h"
#include "shell.h"
#include "res.h"
#include "fileutil.h"
#include <stdarg.h>
#include <limits.h>
#include <dlgs.h>

int	LastError;
int	ErrCode;
int	lock_help = 0;

/* ---------------- General string functions -------- */

void	skip_spaces (char ** s)
{
	while (isspace (**s) && **s != '\n') ++ *s;
}

char * alloc_copy (char * src)
{
	char * d = Malloc (strlen (src)+1);
	if (!d) return 0;
	strcpy (d, src);
	return d;
}

char *	zscpy (char * dest, char * src, size_t num)
{
	strncpy (dest, src, num - 1);
	dest [num - 1] = 0;
	return dest;
}

/* ---------------- Error functions ----------------- */

char * EText (DWORD e)
{
	static char etext [1024];
	etext [0] = 0;

	FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, NULL, e,
		       MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		       etext, sizeof (etext), NULL);
	return etext;
}

char * ErrText (void)
{
	return EText (ErrCode);
}

char * LastErrText (void)
{
	return EText (GetLastError ());
}

/* ---------------- General Windows functions ------- */

BOOL	is_of_class (HWND hwnd, char * class)
{
	char s [200];
	if (!hwnd) return FALSE;
	GetClassName (hwnd, s, sizeof (s));
	return !stricmp (class, s);

}

void	Get_child_pos (HWND hwnd, RECT * rc)
{
	POINT pt;

	GetWindowRect (hwnd, rc);
	hwnd = GetParent (hwnd);
	pt.x = rc->left;
	pt.y = rc->top;
	ScreenToClient (hwnd, &pt);
	rc->left = pt.x;
	rc->top = pt.y;
	pt.x = rc->right;
	pt.y = rc->bottom;
	ScreenToClient (hwnd, &pt);
	rc->right = pt.x;
	rc->bottom = pt.y;
}

void	Center_window (HWND hwnd)
{
	RECT rc;
	RECT rcdesk;

//	ShowWindow (GetDlgItem (hwnd, 9), SW_HIDE);

	GetWindowRect (GetDesktopWindow (), &rcdesk);
	GetWindowRect (hwnd, &rc);
	SetWindowPos (hwnd, 0,
		      (rcdesk.right+rcdesk.left+rc.left-rc.right)/2,
		      (rcdesk.bottom+rcdesk.top+rc.top-rc.bottom)/2,
		       0, 0, SWP_NOSIZE| SWP_NOZORDER);
}

void	Center_and_resize_window (HWND hwnd, int xs, int ys)
{
	RECT rc;
	RECT rcdesk;

	GetWindowRect (GetDesktopWindow (), &rcdesk);
	GetWindowRect (hwnd, &rc);
	if ((rcdesk.right - rcdesk.left) * xs / 100 > rc.right - rc.left)
		rc.right = rc.left + (rcdesk.right - rcdesk.left) * xs / 100;
	if ((rcdesk.bottom - rcdesk.top) * ys / 100 > rc.bottom - rc.top)
		rc.bottom = rc.top + (rcdesk.bottom - rcdesk.top) * ys / 100;
	SetWindowPos (hwnd, 0,
		      (rcdesk.right+rcdesk.left+rc.left-rc.right)/2,
		      (rcdesk.bottom+rcdesk.top+rc.top-rc.bottom)/2,
		       rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER);
}

WNDPROC	Subclass_control (HWND wnd, WNDPROC newproc)
{
	WNDPROC p = (WNDPROC) GetWindowLong (wnd, GWL_WNDPROC);
	SetWindowLong (wnd, GWL_WNDPROC, (LONG) newproc);
	return p;
}

/* ---------------- File directory function  -------- */

char ** filedir = NULL;
int filedir_len = 0;
int filedir_alloclen = 0;

void	normalize_filename (char * s)
{
	for (; *s; s++) if (*s == '/') *s = '\\';
}

int	add_filedir (char * filename)
{
	char * s;
	if (filedir_len == filedir_alloclen)
		filedir = realloc (filedir, (filedir_alloclen += 10) * sizeof (char *));
	s = alloc_copy (filename);
	normalize_filename (s);
	filedir [filedir_len] = s;
	return filedir_len ++;
}

void	clear_filedir (void)
{
	int i;
	for (i = 0; i < filedir_len; i ++)
		free (filedir [i]);
	filedir_len = 0;
	add_filedir ("New file");
}

int	find_filedir (char * filename)
{
	int i;
	char s [_MAX_PATH];
	strncpy (s, filename, sizeof (s));
	s[sizeof (s)-1] = 0;
	normalize_filename (s);
	for (i = 0; i < filedir_len; i ++)
		if (! stricmp (filedir [i], s))
			return i;
	return -1;
}

int	new_filedir (char * filename)
{
	char ff [2000];
	LPSTR name_portion;
	int i;

	if (!GetFullPathName (filename, sizeof (ff), ff, &name_portion))
		strcpy (ff, filename);

	i = find_filedir (ff);
	if (i >= 0) return i;
	return add_filedir (ff);
}

void	set_real_file_name (char * filename)
{
	HANDLE h;
	WIN32_FIND_DATA fd;
	LPSTR name_portion;
	char ff [2000];
	int i;
	
	if (!GetFullPathName (filename, sizeof (ff), ff, &name_portion))
		strcpy (ff, filename);

	h = FindFirstFile (ff, &fd);
	if (h == INVALID_HANDLE_VALUE) return;
	FindClose (h);
	strcpy (name_only (ff), name_only (fd.cFileName));
	i = find_filedir (ff);
	if (i <= 0)
		add_filedir (ff);
	else
		if (strlen (filedir [i]) != strlen (ff))	{
			free (filedir [i]);	
			filedir	[i] = alloc_copy (ff);
		} else
			strcpy (filedir [i], ff);
}



/* -------------------- Ini file section operations ------------------- */

void	Read_section (char * section [][2], int maxlen, char * name, char * file)
{
	char *s, *p, *q;
	int i;

	memset (section, 0, maxlen * sizeof (char*) * 2);
	
	s = Malloc (10000);
	if (!s) return;

	GetPrivateProfileSection (name, s, 10000, file);
	p = s;
	for (i = 0; i < maxlen; i++)	{
		if (!*p) break;
		for (q = p; *q && *q != '='; q++);
		if (*q == '=') *q++ = 0;
		if (! (section [i][0] = alloc_copy (p))) break;
		if (*q)	{
			if (!(section [i][1] = alloc_copy (q)))	{
				free (section [i][0]);
				break;
			}
		} else
			section [i][1] = 0;
		p = q + strlen (q)+1;
	}
	free (s);
}

BOOL	Add_section_elem (char * section [][2], int maxlen,
			  char * key, char * value,
			  char * name, char * file)
{
	int i;
	for (i = 0; i < maxlen; i++)
		if (!section [i][0]) break;
	if (i == maxlen) return FALSE;
	if (!key) return FALSE;
	if (!(section [i][0] = alloc_copy (key))) return FALSE;
	if (!value || !value [0])
		section [i][1] = 0;
	else	{
		if (!(section [i][1] = alloc_copy (value)))	{
			free (section [i][0]);
			section [i][0] = 0;
			return FALSE;
		}
	}
	return WritePrivateProfileString (name, key, value?value:"", file);
}

void	Delete_section_item (char * section [][2], int maxlen,
			     int i, char * name, char * file)
{
	if (!section [i][0]) return;
	if (!WritePrivateProfileString (name, section [i][0], 0, file)) return;
	free (section [i][0]);
	free (section [i][1]);
	for (; i < maxlen - 1; i++)	{
		section [i][0] = section [i+1][0];
		section [i][1] = section [i+1][1];
	}
	section [maxlen-1][0] = section [maxlen-1][1] = 0;
}

BOOL	Write_section (char * section [][2], int maxlen,
		       char * name, char * file)
{
	int i, l;
	char * s;
	l = 1;
	for (i = 0; i < maxlen; i++)
		if (section [i][0])	{
			l += strlen (section [i][0])+2;
			if (section[i][1]) l += strlen (section[i][1]);
		}
	s = Malloc (l);
	if (!s)	{
		WritePrivateProfileSection (name, "\0", file);
		for (i = 0; i < maxlen; i++)
			if (section [i][0])
				WritePrivateProfileString (name, section [i][0],
							   section[i][1]?section[i][1]:"",
							   file);
		return TRUE;
	}
	l = 0;

	for (i = 0; i < maxlen; i++)
		if (section [i][0])	{
			strcpy (s+l, section [i][0]);
			l += strlen (section [i][0]);
			s [l++] = '=';
			if (section[i][1])	{
				strcpy (s+l, section[i][1]);
				l += strlen (section[i][1]) + 1;
			}
		}
	s [l] = 0;
	WritePrivateProfileSection (name, s, file);
	free (s);
	return TRUE;
}

void	read_ini_file (char * section, char * item, int Res, char * dest, int len, char * IniFile)
{
	char dflt [512];

	if (! LoadString (ResInstance, Res, dflt, sizeof (dflt)))
		dflt[0]=0;
	dflt [sizeof (dflt) - 1] = 0;
	GetPrivateProfileString (section, item, dflt, dest, len, IniFile);
}

/* -------------------- History box operation ------------------------ */

void	Insert_history (char * history [], int maxlen, char * s)
{
	int i;
	char * p = 0;
	for (i = 0; i < maxlen && history [i]; i ++)
		if (!strcmp (history [i], s))	{
			p = history [i];
			memmove (history+i, history+i+1,
				(maxlen-i-1) * sizeof (char*));
			break;
		}
	if (!p)	{
		p = Malloc (strlen (s)+1);
		if (!p) return;
		strcpy (p, s);
		free (history [maxlen-1]);
	}
	memmove (history+1, history, (maxlen-1) * sizeof (char*));
	history [0] = p;
}

void	Fill_history_box (HWND hwnd, char * history [], int maxlen)
{
	int i;
	for (i = 0; i < maxlen && history [i]; i ++)
		SendMessage (hwnd, CB_ADDSTRING, 0,
			     (LPARAM)(LPSTR) history [i]);
}

BOOL	Add_history_to_menu (HMENU menu, char * history [], int maxlen,
			     int id_after, int first_id)
{
	int i;
	int cnt;
	char s [1024];

	cnt = GetMenuItemCount (menu);
	for (i = cnt-1; i>= 0; i--)
		if ((int) GetMenuItemID (menu, i) >= first_id)
			DeleteMenu (menu, i, MF_BYPOSITION);
	cnt = GetMenuItemCount (menu);
	if (cnt)
		if (! GetMenuItemID (menu, cnt-1))
			DeleteMenu (menu, cnt-1, MF_BYPOSITION);

#if 0
	for (i = 0; i < cnt; i++)
		if (GetMenuItemID (menu, i) == id_after)	{
			i++;
			while (i < cnt--)
				DeleteMenu (menu, i, MF_BYPOSITION);
			break;
		}
#endif
	if (history [0])
		AppendMenu (menu, MF_SEPARATOR, 0, 0);

	for (i = 0; i < maxlen && history [i]; i++)	{
		wsprintf (s, "&%d %s", i+1, history [i]);
		AppendMenu (menu, MF_ENABLED|MF_STRING, first_id+i, s);
	}
	return history [i] != NULL;
}

void	Write_history (char * history [], int maxlen, char * name, char * file)
{
	char num [10];
	int i;

	WritePrivateProfileSection (name, "\0", file);
	for (i = 0; i < maxlen && history [i]; i++)	{
		wsprintf (num, "%d", i+1);
		WritePrivateProfileString (name, num, history [i], file);
	}
}

void	Read_history  (char * history [], int maxlen, int elemlen,
			char * name, char * file)
{
	char *s, *p;
	char num [10];
	int i;

	char * lbuf;

	memset (history, 0, maxlen * sizeof (char*));
	s = Malloc (elemlen+1);
	lbuf = Malloc (10000);
	i = GetPrivateProfileSection (name, lbuf, 10000, file);
	free (lbuf);

	for (i = 0; i < maxlen; i++)	{
		wsprintf (num, "%d", i+1);
		GetPrivateProfileString (name, num, "", s, elemlen+1, file);
		if (!*s) break;
		s [elemlen] = 0;
		p = Malloc (strlen (s)+1);
		if (!p) break;
		strcpy (p, s);
		free (history [i]);
		history [i] = p;
	}
	free (s);
}

/* ----------------------------------------------------------------------- */

void * Malloc (int size)
{
	void * p = malloc (size);
	if (!p) return 0;
	memset (p, 0xDD, size);
	return p;
}

/* ----------------------------------------------------------------------- */

COLORREF light_color (COLORREF c)
{
	int r, g, b;
	r = GetRValue (c) + 128;
	g = GetGValue (c) + 128;
	b = GetBValue (c) + 128;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	c = RGB (r, g, b);
	if (c == 0xFFFFFF) c = 0xC0C0C0;
	return c;
}

/* ----------------------------------------------------------------------- */

void	text_size (HFONT font, char * text, int * x, int * y)
{
	SIZE s;
	HDC hdc = GetDC (NULL);
	HFONT ofont;

	if (font)
		ofont = SelectObject (hdc, font);
	GetTextExtentPoint (hdc, text, strlen (text), &s);
	*x = s.cx;
	*y = s.cy;
	if (font)
		SelectObject (hdc, ofont);
	ReleaseDC (NULL, hdc);
}

int	text_width (HFONT font, char * text)
{
	int x, y;
	text_size (font, text, &x, &y);
	return x;
}

void	mtext_size (HFONT font, char * text, int * x, int * y)
{
	char * p, c;
	int X, Y;
	*x = *y = 0;
	while (*text)	{
		for (p = text; *p && *p != '\n'; p++);
		c = *p;
		*p = 0;
		text_size (font, text, &X, &Y);
		*y += Y;
		*x = max (*x, X);
		*p = c;
		text = p + (c != 0);
	}
}

void	number_validate (HWND hwnd, char * s, BOOL positive)
{
	int i, j;
	i = j = 0;
	if (!positive && s [0] == '-')
		i = j = 1;
	for (; s[i]; i++)
		if (isdigit (s[i]))
			s [j++] = s[i];
	s[j]=0;
	if (i!=j)
		SetWindowText (hwnd, s);
}

static char * ask_prompt, * ask_title, * ask_result;
static int ask_result_len;
static UINT ask_flags;

#define ASK_INIT 1
#define ASK_SELECT 2
#define ASK_NUMBER 4
#define ASK_POSITIVE 8

BOOL CALLBACK AskBoxProc   (HWND hwnd, UINT msg,
			    WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_INITDIALOG:
		Center_window (hwnd);
		SetWindowText (hwnd, ask_title);
		SetDlgItemText (hwnd, ASKTEXT_PROMPT, ask_prompt);
		if (ask_flags & ASK_INIT)	{
			SetDlgItemText (hwnd, ASKTEXT_EDIT,   ask_result);
			if (ask_flags & ASK_SELECT)
				SendDlgItemMessage (hwnd, ASKTEXT_EDIT, EM_SETSEL, 0, 1000);
		}
		SendDlgItemMessage (hwnd, ASKTEXT_EDIT, EM_LIMITTEXT, ask_result_len, 0);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case ASKTEXT_EDIT:
			if (!(ask_flags & ASK_NUMBER)) break;
			if (NOTIFY_CODE == EN_CHANGE)	{
				char s [100];
				GetDlgItemText (hwnd, ASKTEXT_EDIT, s, sizeof (s));
				number_validate (GetDlgItem (hwnd, ASKTEXT_EDIT),
						      s, ask_flags & ASK_POSITIVE);
				return 1;
			}
			break;
		case IDOK:
			GetDlgItemText (hwnd, ASKTEXT_EDIT, ask_result, ask_result_len);
			ask_result [ask_result_len-1]=0;
			EndDialog (hwnd, 1);
			return 1;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			return 1;
		case IDHELP:
			return Dialog_help (ASKTEXT_DIALOG);
		}
		return 0;
	case WM_HELP:
		return Control_help (ASKTEXT_DIALOG, lparam);
	}
	return 0;
}

BOOL	Ask_text (char * prompt, char * title, char * result, int result_len, UINT flags)
{
	int i;
	ask_prompt = prompt;
	ask_title  = title;
	ask_result = result;
	ask_result_len = result_len;
	ask_flags = flags;
	i = Dialog (ASKTEXT_DIALOG, AskBoxProc);
	return i == 1;
}

BOOL	Ask_number (char * prompt, char * title, int * result, UINT flags)
{
	char buf [10];
	if (*result == INT_MIN)
		buf [0] = 0;
	else
		wsprintf (buf, "%d", *result);
	if (!Ask_text (prompt, title, buf, sizeof (buf), flags)) return FALSE;
	*result = atoi (buf);
	return TRUE;
}

#if 0

char * Text;
char * Title;

BOOL CALLBACK MsgBoxProc   (HWND hwnd, UINT msg,
			    WPARAM wparam, LPARAM lparam)
{
	int x, y;
	int x1, y1;
	HFONT font;
	HWND swnd, fwnd;
	RECT r, rf;
	switch (msg) {
	case WM_INITDIALOG:
		swnd = GetDlgItem (hwnd, MSGBOX_TEXT);
		fwnd = GetDlgItem (hwnd, MSGBOX_FRAME);
		font = (HFONT) SendMessage (swnd, WM_GETFONT, 0, 0);
		mtext_size (font, Text, &x, &y);
		SetWindowPos (swnd, 0, 0, 0, x+2, y+2, SWP_NOMOVE);
		Get_child_pos (fwnd, & rf);
		Get_child_pos (swnd, & r);
		if (r.right >= rf.right - 10)
			rf.right = r.right + 10;
		if (r.bottom >= rf.bottom - 10)
			rf.bottom = r.bottom + 10;
		SetWindowPos (fwnd, 0, 0, 0, rf.right-rf.left, rf.bottom-rf.top, SWP_NOMOVE);
		GetWindowRect (hwnd, &r);
		GetWindowRect (fwnd, &rf);
		if (r.right < rf.right + 10)
			r.right = rf.right + 10;
		if (r.bottom < rf.bottom + 10)
			r.bottom = rf.bottom + 10;
		SetWindowPos (hwnd, 0, 0, 0, r.right-r.left, r.bottom-r.top, SWP_NOMOVE);
		SetWindowText (hwnd, Title);
		SetWindowText (swnd, Text);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			EndDialog (hwnd, 1);
			return 1;
		}
		return 0;
	}
	return 0;
}

void	MsgBox (HWND owner, char * text, char * title, UINT style)
{
	Text = text;
	Title = title;
	DialogBox (MyInstance, MAKEINTRESOURCE (MSGBOX_DIALOG), FrameWindow,
		   MsgBoxProc);
}
#endif

int	Find_popup (HMENU menu)
{
	HMENU main, s, d;
	int n, i, j, k, m, add;
	UINT id;

	if (menu == GetSystemMenu (FrameWindow, FALSE)) return 1;

	main = GetMenu (FrameWindow);
	n = GetMenuItemCount (main);
	s = GetSubMenu (main, 0);
	id = GetMenuItemID (s, 0);
	add = 1;
	if (id != 0xFFFFFFFF && id >= 0xF000)	{
		if (s == menu) return 2;
		add = 0;
	}

	for (i = 0; i < n; i++)	{
		s = GetSubMenu (main, i);
		if (!s) continue;
		if (s == menu) return (i+add)*10;
		m = GetMenuItemCount (s);
		k = 0;
		for (j = 0; j < m; j++)	{
			d = GetSubMenu (s, j);
			if (!d) continue;
			k++;
			if (d == menu) return (i+add)*10+k;
		}
	}
	return -3;
}

int	Dialog_help (int id)
{
//	MsgBox ("Help", MB_OK, "About to display help on dialog %d", id);
//	MsgBox ("Help", MB_OK|MB_ICONEXCLAMATION,
//		"Sorry, dialog helps are notimplemented yet");
	WinHelp (FrameWindow, IDEHelpFile, HELP_CONTEXT, id);
	return 1;
}

int	Menu_help (int id, BOOL popup)
{
	int m;
	popup=0;
	m = WinHelp (FrameWindow, IDEHelpFile, popup ? HELP_CONTEXTPOPUP : HELP_CONTEXT, id);
	return 1;
}

void	Control_id_help (int dlg_id, int id)
{
	if (lock_help) return;
	if (id == IDOK)
		id = dlg_id + 1;
	else if (id == IDCANCEL)
		id = dlg_id + 2;
	else if (id == IDNO)
		id = dlg_id + 4;
	WinHelp (FrameWindow, IDEHelpFile, HELP_CONTEXTPOPUP, id);
}

int	Control_help (int dlg_id, LPARAM lparam)
{
	HELPINFO * h = (HELPINFO*) lparam;
	HWND w;

	if (lock_help) return 1;
	if (h->iContextType != HELPINFO_WINDOW)
		return Dialog_help (dlg_id);
	
	if (h->iCtrlId <=0 ) return Dialog_help (dlg_id);
	w = GetDlgItem (GetActiveWindow (), h->iCtrlId);
//	if (!w) return 1;
	w = h->hItemHandle;
	if (! is_of_class (w, "button") && 
		! is_of_class (w, "static") &&
		! is_of_class (w, "edit") &&
		! is_of_class (w, "listbox") &&
		! is_of_class (w, "combobox")) return 1;
	Control_id_help (dlg_id, h->iCtrlId);
	return 1;
}

int	Page_Control_help (HWND hwnd, int dlg_id, LPARAM lparam)
{
	HELPINFO * h = (HELPINFO*) lparam;
	HWND w;

	if (lock_help) return 1;
	if (h->iContextType == HELPINFO_WINDOW)	{
		if (h->iCtrlId <=0 ) return 1;
	        w = GetDlgItem (hwnd, h->iCtrlId);
		if (!w) return 1;
		if (! is_of_class (w, "button") && 
			! is_of_class (w, "static") &&
			! is_of_class (w, "edit") &&
			! is_of_class (w, "listbox") &&
			! is_of_class (w, "combobox")) return 1;
		Control_id_help (dlg_id, h->iCtrlId);
	}
	return 1;
}

HPALETTE CreateDIBPalette (LPBITMAPINFO lpbmi, int * NumColors)
{
	LPBITMAPINFOHEADER  lpbi;
	HPALETTE hPal;
	struct	{
			LOGPALETTE lp;
			PALETTEENTRY e [256];
		} p;
	int i;

	hPal = NULL;
	lpbi = (LPBITMAPINFOHEADER) lpbmi;
	if (lpbi->biBitCount <= 8)
		*NumColors = (1 << lpbi->biBitCount);
	else
		*NumColors = 0;

	if (*NumColors)	{
		p.lp.palVersion    = 0x300;
		p.lp.palNumEntries = *NumColors;

		for (i = 0;  i < *NumColors;  i++)	{
			p.lp.palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
			p.lp.palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
			p.lp.palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
			p.lp.palPalEntry[i].peFlags = 0;
		}
		hPal = CreatePalette (&p.lp);
	}

	return hPal;
}

HBITMAP LoadResourceBitmap (HINSTANCE hInstance, LPSTR lpString,
			    HPALETTE * lphPalette)
{
	HRSRC  hRsrc;
	HGLOBAL hGlobal;
	HBITMAP hBitmapFinal = NULL;
	LPBITMAPINFOHEADER  lpbi;
	HDC hdc;
	int iNumColors;

	if (hRsrc = FindResource (hInstance, lpString, RT_BITMAP))	{
		hGlobal = LoadResource (hInstance, hRsrc);
		lpbi = (LPBITMAPINFOHEADER) LockResource(hGlobal);

		hdc = GetDC (NULL);
		*lphPalette =  CreateDIBPalette ((LPBITMAPINFO)lpbi, &iNumColors);
		if (*lphPalette)	{
			SelectPalette (hdc,*lphPalette,FALSE);
			RealizePalette (hdc);
		}

		hBitmapFinal = CreateDIBitmap   (hdc,
						(LPBITMAPINFOHEADER) lpbi,
						(LONG) CBM_INIT,
						(LPSTR) lpbi + lpbi->biSize + iNumColors * sizeof(RGBQUAD),
						(LPBITMAPINFO)lpbi,
						DIB_RGB_COLORS);

		ReleaseDC (NULL,hdc);
		UnlockResource (hGlobal);
		FreeResource(hGlobal);
	}
	return hBitmapFinal;
}

void	Set_lb_extent (HWND hwnd)
{
	int i, n, l, len, extent, x;
	char * buf, * p;
	HFONT font;
	
	font = (HFONT) SendMessage (hwnd, WM_GETFONT, 0, 0);
	extent = 10;
	len = 100;
	buf = Malloc (len);
	n = (int) SendMessage (hwnd, LB_GETCOUNT, 0, 0);
	for (i = 0; i < n; i++)	{
		l = (int) SendMessage (hwnd, LB_GETTEXTLEN, 0, 0);
		if (l+1 > len)	{
			p = realloc (buf, l+20);
			if (p)	{
				len = l+20;
				buf = p;
			}
		}
		if (l+1 <= len)	{
			SendMessage  (hwnd, LB_GETTEXT, i, (LPARAM)buf);
			x = text_width (font, buf);
			if (x > extent) extent = x;
		}
	}
	SendMessage  (hwnd, LB_SETHORIZONTALEXTENT, extent, 0);
}

#if 0

struct	{char * name; int msg;} msgnames [] = {
	{"",					0x0000},
	{"WM_CREATE",				0x0001},
	{"WM_DESTROY",				0x0002},
	{"WM_MOVE",				0x0003},
	{"WM_SIZE",				0x0005},
	{"WM_ACTIVATE",				0x0006},
	{"WM_SETFOCUS",				0x0007},
	{"WM_KILLFOCUS",			0x0008},
	{"WM_ENABLE",				0x000A},
	{"WM_SETREDRAW",			0x000B},
	{"WM_SETTEXT",				0x000C},
	{"WM_GETTEXT",				0x000D},
	{"WM_GETTEXTLENGTH",			0x000E},
	{"WM_PAINT",				0x000F},
	{"WM_CLOSE",				0x0010},
	{"WM_QUERYENDSESSION",			0x0011},
	{"WM_QUIT",				0x0012},
	{"WM_QUERYOPEN",			0x0013},
	{"WM_ERASEBKGND",			0x0014},
	{"WM_SYSCOLORCHANGE",			0x0015},
	{"WM_ENDSESSION",			0x0016},
	{"WM_SHOWWINDOW",			0x0018},
	{"WM_WININICHANGE",			0x001A},
	{"WM_DEVMODECHANGE",			0x001B},
	{"WM_ACTIVATEAPP",			0x001C},
	{"WM_FONTCHANGE",			0x001D},
	{"WM_TIMECHANGE",			0x001E},
	{"WM_CANCELMODE",			0x001F},
	{"WM_SETCURSOR",			0x0020},
	{"WM_MOUSEACTIVATE",			0x0021},
	{"WM_CHILDACTIVATE",			0x0022},
	{"WM_QUEUESYNC",			0x0023},
	{"WM_GETMINMAXINFO",			0x0024},
	{"WM_PAINTICON",			0x0026},
	{"WM_ICONERASEBKGND",			0x0027},
	{"WM_NEXTDLGCTL",			0x0028},
	{"WM_SPOOLERSTATUS",			0x002A},
	{"WM_DRAWITEM",				0x002B},
	{"WM_MEASUREITEM",			0x002C},
	{"WM_DELETEITEM",			0x002D},
	{"WM_VKEYTOITEM",			0x002E},
	{"WM_CHARTOITEM",			0x002F},
	{"WM_SETFONT",				0x0030},
	{"WM_GETFONT",				0x0031},
	{"WM_SETHOTKEY",			0x0032},
	{"WM_GETHOTKEY",			0x0033},
	{"WM_QUERYDRAGICON",			0x0037},
	{"WM_COMPAREITEM",			0x0039},
	{"WM_COMPACTING",			0x0041},
	{"WM_OTHERWINDOWCREATED",		0x0042},
	{"WM_OTHERWINDOWDESTROYED",		0x0043},
	{"WM_COMMNOTIFY",			0x0044},
	{"WM_WINDOWPOSCHANGING",		0x0046},
	{"WM_WINDOWPOSCHANGED",			0x0047},
	{"WM_POWER",				0x0048},
	{"WM_COPYDATA",				0x004A},
	{"WM_CANCELJOURNAL",			0x004B},
	{"WM_NCCREATE",				0x0081},
	{"WM_NCDESTROY",			0x0082},
	{"WM_NCCALCSIZE",			0x0083},
	{"WM_NCHITTEST",			0x0084},
	{"WM_NCPAINT",				0x0085},
	{"WM_NCACTIVATE",			0x0086},
	{"WM_GETDLGCODE",			0x0087},
	{"WM_NCMOUSEMOVE",			0x00A0},
	{"WM_NCLBUTTONDOWN",			0x00A1},
	{"WM_NCLBUTTONUP",			0x00A2},
	{"WM_NCLBUTTONDBLCLK",			0x00A3},
	{"WM_NCRBUTTONDOWN",			0x00A4},
	{"WM_NCRBUTTONUP",			0x00A5},
	{"WM_NCRBUTTONDBLCLK",			0x00A6},
	{"WM_NCMBUTTONDOWN",			0x00A7},
	{"WM_NCMBUTTONUP",			0x00A8},
	{"WM_NCMBUTTONDBLCLK",			0x00A9},

	{"EM_GETSEL",				0x00B0},
	{"EM_SETSEL",				0x00B1},
	{"EM_GETRECT",				0x00B2},
	{"EM_SETRECT",				0x00B3},
	{"EM_SETRECTNP",			0x00B4},
	{"EM_SCROLL",				0x00B5},
	{"EM_LINESCROLL",			0x00B6},
	{"EM_SCROLLCARET",			0x00B7},
	{"EM_GETMODIFY",			0x00B8},
	{"EM_SETMODIFY",			0x00B9},
	{"EM_GETLINECOUNT",			0x00BA},
	{"EM_LINEINDEX",			0x00BB},
	{"EM_SETHANDLE",			0x00BC},
	{"EM_GETHANDLE",			0x00BD},
	{"EM_GETTHUMB",				0x00BE},
	{"EM_LINELENGTH",			0x00C1},
	{"EM_REPLACESEL",			0x00C2},
	{"EM_GETLINE",				0x00C4},
	{"EM_LIMITTEXT",			0x00C5},
	{"EM_CANUNDO",				0x00C6},
	{"EM_UNDO",				0x00C7},
	{"EM_FMTLINES",				0x00C8},
	{"EM_LINEFROMCHAR",			0x00C9},
	{"EM_SETTABSTOPS",			0x00CB},
	{"EM_SETPASSWORDCHAR",			0x00CC},
	{"EM_EMPTYUNDOBUFFER",			0x00CD},
	{"EM_GETFIRSTVISIBLELINE",		0x00CE},
	{"EM_SETREADONLY",			0x00CF},
	{"EM_SETWORDBREAKPROC",			0x00D0},
	{"EM_GETWORDBREAKPROC",			0x00D1},
	{"EM_GETPASSWORDCHAR",			0x00D2},

	{"BM_GETCHECK",				0x00F0},
	{"BM_SETCHECK",				0x00F1},
	{"BM_GETSTATE",				0x00F2},
	{"BM_SETSTATE",				0x00F3},
	{"BM_SETSTYLE",				0x00F4},

	{"WM_KEYDOWN",				0x0100},
	{"WM_KEYUP",				0x0101},
	{"WM_CHAR",				0x0102},
	{"WM_DEADCHAR",				0x0103},
	{"WM_SYSKEYDOWN",			0x0104},
	{"WM_SYSKEYUP",				0x0105},
	{"WM_SYSCHAR",				0x0106},
	{"WM_SYSDEADCHAR",			0x0107},
	{"WM_KEYLAST",				0x0108},
	{"WM_INITDIALOG",			0x0110},
	{"WM_COMMAND",				0x0111},
	{"WM_SYSCOMMAND",			0x0112},
	{"WM_TIMER",				0x0113},
	{"WM_HSCROLL",				0x0114},
	{"WM_VSCROLL",				0x0115},
	{"WM_INITMENU",				0x0116},
	{"WM_INITMENUPOPUP",			0x0117},
	{"WM_MENUSELECT",			0x011F},
	{"WM_MENUCHAR",				0x0120},
	{"WM_ENTERIDLE",			0x0121},
	{"WM_CTLCOLORMSGBOX",			0x0132},
	{"WM_CTLCOLOREDIT",			0x0133},
	{"WM_CTLCOLORLISTBOX",			0x0134},
	{"WM_CTLCOLORBTN",			0x0135},
	{"WM_CTLCOLORDLG",			0x0136},
	{"WM_CTLCOLORSCROLLBAR",		0x0137},
	{"WM_CTLCOLORSTATIC",			0x0138},

	{"CB_GETEDITSEL",			0x0140},
	{"CB_LIMITTEXT",			0x0141},
	{"CB_SETEDITSEL",			0x0142},
	{"CB_ADDSTRING",			0x0143},
	{"CB_DELETESTRING",			0x0144},
	{"CB_DIR",				0x0145},
	{"CB_GETCOUNT",				0x0146},
	{"CB_GETCURSEL",			0x0147},
	{"CB_GETLBTEXT",			0x0148},
	{"CB_GETLBTEXTLEN",			0x0149},
	{"CB_INSERTSTRING",			0x014A},
	{"CB_RESETCONTENT",			0x014B},
	{"CB_FINDSTRING",			0x014C},
	{"CB_SELECTSTRING",			0x014D},
	{"CB_SETCURSEL",			0x014E},
	{"CB_SHOWDROPDOWN",			0x014F},
	{"CB_GETITEMDATA",			0x0150},
	{"CB_SETITEMDATA",			0x0151},
	{"CB_GETDROPPEDCONTROLRECT",		0x0152},
	{"CB_SETITEMHEIGHT",			0x0153},
	{"CB_GETITEMHEIGHT",			0x0154},
	{"CB_SETEXTENDEDUI",			0x0155},
	{"CB_GETEXTENDEDUI",			0x0156},
	{"CB_GETDROPPEDSTATE",			0x0157},
	{"CB_FINDSTRINGEXACT",			0x0158},
	{"CB_SETLOCALE",			0x0159},
	{"CB_GETLOCALE",			0x015A},
	{"CB_MSGMAX",				0x015B},

	{"STM_SETICON",				0x0170},
	{"STM_GETICON",				0x0171},
	{"STM_MSGMAX",				0x0172},

	{"LB_ADDSTRING",			0x0180},
	{"LB_INSERTSTRING",			0x0181},
	{"LB_DELETESTRING",			0x0182},
	{"LB_SELITEMRANGEEX",			0x0183},
	{"LB_RESETCONTENT",			0x0184},
	{"LB_SETSEL",				0x0185},
	{"LB_SETCURSEL",			0x0186},
	{"LB_GETSEL",				0x0187},
	{"LB_GETCURSEL",			0x0188},
	{"LB_GETTEXT",				0x0189},
	{"LB_GETTEXTLEN",			0x018A},
	{"LB_GETCOUNT",				0x018B},
	{"LB_SELECTSTRING",			0x018C},
	{"LB_DIR",				0x018D},
	{"LB_GETTOPINDEX",			0x018E},
	{"LB_FINDSTRING",			0x018F},
	{"LB_GETSELCOUNT",			0x0190},
	{"LB_GETSELITEMS",			0x0191},
	{"LB_SETTABSTOPS",			0x0192},
	{"LB_GETHORIZONTALEXTENT",		0x0193},
	{"LB_SETHORIZONTALEXTENT",		0x0194},
	{"LB_SETCOLUMNWIDTH",			0x0195},
	{"LB_ADDFILE",				0x0196},
	{"LB_SETTOPINDEX",			0x0197},
	{"LB_GETITEMRECT",			0x0198},
	{"LB_GETITEMDATA",			0x0199},
	{"LB_SETITEMDATA",			0x019A},
	{"LB_SELITEMRANGE",			0x019B},
	{"LB_SETANCHORINDEX",			0x019C},
	{"LB_GETANCHORINDEX",			0x019D},
	{"LB_SETCARETINDEX",			0x019E},
	{"LB_GETCARETINDEX",			0x019F},
	{"LB_SETITEMHEIGHT",			0x01A0},
	{"LB_GETITEMHEIGHT",			0x01A1},
	{"LB_FINDSTRINGEXACT",			0x01A2},
	{"LB_SETLOCALE",			0x01A5},
	{"LB_GETLOCALE",			0x01A6},
	{"LB_SETCOUNT",				0x01A7},

	{"LB_MSGMAX",				0x01A8},

	{"WM_MOUSEMOVE",			0x0200},
	{"WM_LBUTTONDOWN",			0x0201},
	{"WM_LBUTTONUP",			0x0202},
	{"WM_LBUTTONDBLCLK",			0x0203},
	{"WM_RBUTTONDOWN",			0x0204},
	{"WM_RBUTTONUP",			0x0205},
	{"WM_RBUTTONDBLCLK",			0x0206},
	{"WM_MBUTTONDOWN",			0x0207},
	{"WM_MBUTTONUP",			0x0208},
	{"WM_MBUTTONDBLCLK",			0x0209},
	{"WM_PARENTNOTIFY",			0x0210},
	{"WM_ENTERMENULOOP",			0x0211},
	{"WM_EXITMENULOOP",			0x0212},
	{"WM_MDICREATE",			0x0220},
	{"WM_MDIDESTROY",			0x0221},
	{"WM_MDIACTIVATE",			0x0222},
	{"WM_MDIRESTORE",			0x0223},
	{"WM_MDINEXT",				0x0224},
	{"WM_MDIMAXIMIZE",			0x0225},
	{"WM_MDITILE",				0x0226},
	{"WM_MDICASCADE",			0x0227},
	{"WM_MDIICONARRANGE",			0x0228},
	{"WM_MDIGETACTIVE",			0x0229},
	{"WM_MDISETMENU",			0x0230},
	{"WM_DROPFILES",			0x0233},
	{"WM_MDIREFRESHMENU",			0x0234},
	{"WM_CUT",				0x0300},
	{"WM_COPY",				0x0301},
	{"WM_PASTE",				0x0302},
	{"WM_CLEAR",				0x0303},
	{"WM_UNDO",				0x0304},
	{"WM_RENDERFORMAT",			0x0305},
	{"WM_RENDERALLFORMATS",			0x0306},
	{"WM_DESTROYCLIPBOARD",			0x0307},
	{"WM_DRAWCLIPBOARD",			0x0308},
	{"WM_PAINTCLIPBOARD",			0x0309},
	{"WM_VSCROLLCLIPBOARD",			0x030A},
	{"WM_SIZECLIPBOARD",			0x030B},
	{"WM_ASKCBFORMATNAME",			0x030C},
	{"WM_CHANGECBCHAIN",			0x030D},
	{"WM_HSCROLLCLIPBOARD",			0x030E},
	{"WM_QUERYNEWPALETTE",			0x030F},
	{"WM_PALETTEISCHANGING",		0x0310},
	{"WM_PALETTECHANGED",			0x0311},
	{"WM_HOTKEY",				0x0312},
	{"WM_PENWINFIRST",			0x0380},
	{"WM_PENWINLAST",			0x038F},
	{"", -1}
};

void	tracemsg (char * prefix, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	static FILE * f = NULL;
	if (!f)	{
		 f = fopen ("log", "a");
		 fprintf (f, "Start log\n");
	}
	for (i = 0; msgnames [i].msg !=-1 && msgnames [i].msg != (int) msg; i++);
	fprintf (f, "%s: %04X %20s : w=%04X l=%08lX\n", prefix, msg,
		 msgnames [i].msg == -1? "" : msgnames [i].name, wparam, lparam);
	fflush (f);
}
#endif

// one-to-one conversion, in contrast to CharToOem - OemToChar
static const char oemToAnsiTable[256] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
  0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

  // 0x80 - 0xAF -> 'À' - 'ï'
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
  0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
  0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
  0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,

  // 0xB0 - 0xB2
  0x80, 0x81, 0x82,

  // 0xB3 -> '¦'
                    0xA6,

  // 0xB4 - 0xC4
                          0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
  0x90, 0x91, 0x92, 0x93, 0x94,

  // 0xC5 - 0xCF
                                0x96, 0x97, 0x98,
  0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0,

  // 0xD0
  0xA3,

  // 0xD1 - 0xD3
        0xA5, 0x83, 0xA7,

  // 0xD4
                          0xA9,

  // 0xD5 - 0xD8
                                0xAB, 0xAC, 0xAD,
  0xAE,

  // 0xD9 - 0xDE
        0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6,

  // 0xDF
                                            0xBB,

  // 0xE0 - 0xEF -> 'ð' - 'ÿ'
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
  0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,

  // 0xF0 -> '¨'
  0xA8,

  // 0xF1 -> '¸'
        0xB8,

  // 0xF2 -> 'ª'
              0xAA,

  // 0xF3 -> 'º'
                    0xBA,

  // 0xF4 -> '¯'
                          0xAF,

  // 0xF5 -> '¿'
                                0xBF,

  // 0xF6 - 0xF7 -> '¡' - '¢'
                                      0xA1, 0xA2,

  // 0xF8 -> '°'
  0xB0,

  // 0xF9 -> '•'
        0x95,

  // 0xFA -> '·'
              0xB7,
  // 0xFB
                    0xBC,

  // 0xFC -> '¹'
                          0xB9,

  // 0xFD -> '¤'
                                0xA4,

  // 0xFE - 0xFF
                                      0xBD, 0xBE
};

// one-to-one conversion, in contrast to CharToOem - OemToChar
static const char ansiToOemTable[256] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
  0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

  // 0x80 - 0x94
  0xB0, 0xB1, 0xB2, 0xD2, 0xB4, 0xB5, 0xB6, 0xB7,
  0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4,

  // 0x95 -> '•'
                                0xF9,

  // 0x96 - 0xA0
                                      0xC5, 0xC6,
  0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE,
  0xCF,

  // 0xA1 - 0xA2 -> '¡' - '¢'
        0xF6, 0xF7,

  // 0xA3
                    0xD0,

  // 0xA4 -> '¤'
                          0xFD,

  // 0xA5
                                0xD1,

  // 0xA6 -> '¦'
                                      0xB3,

  // 0xA7
                                            0xD3,

  // 0xA8 -> '¨'
  0xF0,

  // 0xA9
        0xD4,

  // 0xAA -> 'ª'
              0xF2,

  // 0xAB - 0xAE
                    0xD5, 0xD6, 0xD7, 0xD8,

  // 0xAF -> '¯'
                                            0xF4,

  // 0xB0 -> '°'
  0xF8,

  // 0xB1 - 0xB6
        0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE,

  // 0xB7 -> '·'
                                            0xFA,

  // 0xB8 -> '¸'
  0xF1,

  // 0xB9 -> '¹'
        0xFC,

  // 0xBA -> 'º'
              0xF3,

  // 0xBB
                    0xDF,

  // 0xBC
                          0xFB,

  // 0xBD - 0xBE
                                0xFE, 0xFF,

  // 0xBF -> '¿'
                                            0xF5,

  // 0xC0 - 0xEF -> 'À' - 'ï'
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
  0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
  0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
  0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,

  // 0xF0 - 0xFF -> 'ð' - 'ÿ'
  0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
  0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF
};

void ansiToOem(unsigned char *text, int len)
{
  int i;
  for(i = 0; i < len; ++i)
    text[i] = ansiToOemTable[text[i]];
}

void oemToAnsi(unsigned char *text, int len)
{
  int i;
  for(i = 0; i < len; ++i)
    text[i] = oemToAnsiTable[text[i]];
}