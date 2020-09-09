#include "util.h"
#include "fileutil.h"
#include "res.h"
#include "shell.h"
#include "message.h"
#include "classes.h"
#include "flist.h"
#include "winmake.h"
#include "var.h"
#include "fcache.h"
#include "filter.h"
#include "make.h"
#include "config.h"
#include "about.h"
#include "open.h"
#include <shellapi.h>		/* for functions determining EXE file type */

/* ------------------------------------------------------------------ */
HWND	CompileWindow;
HWND	CompileListBox;
HWND	ProgressIndicator;
char	CurrentJobComment [128];
long	cur_progress, cur_comment_progress;
BOOL	progress_present;
HANDLE	readpipe;
HANDLE	MakerProcess;
HANDLE	ThreadHandle;
HANDLE	RunProcess = 0;
DWORD	RunProcessId = 0;
HANDLE	RunThread;
char	run_cmdline [2000];
char	run_startdir [2000];
int	GoodResCode;
char	title_str [200];
int	lbox_extent = 0;
BOOL    WaitForKey;
BOOL	StopState = 0; /* 0 = close; 1=stop */
int	ListBoxSX, ListBoxSY;

BOOL ConsoleCreated = FALSE;
HWND ConsoleHandle;

static char * StopTitle [] = {"&Close", "&Stop"};

#define WMSG_START	(WM_USER+1001)
#define	WMSG_CONNECT    (WM_USER+1002)
#define	WMSG_DISCONNECT (WM_USER+1003)
#define WMSG_SEVERE	(WM_USER+1004)

struct	_linelist {
			char * s;
			struct _linelist * next;
		}
			* LineList = 0;
struct _linelist ** NextLineList = &LineList;

void	clear_list (void)
{
	struct _linelist * p;
	while (LineList)	{
		p = LineList -> next;
		free (LineList);
		LineList = p;
	}
	NextLineList = &LineList;
}

void	update_lbox_extent (char * s)
{
	int w;
	w = text_width (CompileFont, s);
	if (w > lbox_extent)
		lbox_extent = w;
}

void	recalc_extent (void)
{
	struct _linelist * p;

	lbox_extent = 10;
	for (p = LineList; p; p=p->next)
		update_lbox_extent (p->s);
	SendMessage (CompileListBox, LB_SETHORIZONTALEXTENT, lbox_extent, 0);
}

void    addline (char * s)
{
	int l;
	struct _linelist * q;

	q = Malloc (sizeof (*q));
	if (!q) return;

	q->s = alloc_copy (s);
	if (!q->s)	{
		free (q);
		return;
	}
	for (l = strlen (q->s); l > 0 && (unsigned char) q->s[l-1] < ' '; l--);
	q->s[l] = 0;

	q->next = NULL;
	*NextLineList = q;
	NextLineList = &q->next;

	l = SendMessage (CompileListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)q->s);
	SendMessage (CompileListBox, LB_SETTOPINDEX, l, 0);
	update_lbox_extent (q->s);
}

void	copy_output (void)
{
	struct _linelist * p;
	int len;
	HANDLE h;
	char * t;

	if (! OpenClipboard (0))	{
		MsgBox ("Copy program output", MB_OK | MB_ICONEXCLAMATION,
			 "Can't open clipboard");
		return;
	}
	len = 1;
	for (p = LineList; p; p=p->next)
		len += strlen (p->s)+2;
	h = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, len);
	if (!h)	{
		CloseClipboard ();
		MsgBox ("Copy program output", MB_OK | MB_ICONEXCLAMATION,
			 "No enough memory for operation");
		return;
	}
	len = 0;
	t = GlobalLock (h);
	for (p = LineList; p; p=p->next) {
		strcpy (t+len, p->s);
		len += strlen (p->s);
		strcpy (t+len, "\r\n");
		len += 2;
	}
	GlobalUnlock (h);
	EmptyClipboard ();
	SetClipboardData (CF_TEXT, h);
	CloseClipboard ();
}

int	output_filter_pos = 1;

BOOL	wr_output (int file)
{
	struct _linelist * p;

	if (!Put_file_start ()) return FALSE;
	for (p = LineList; p; p=p->next)
		if (!Put_file (p->s, strlen (p->s)))
			return FALSE;
	return Put_file_fin (file, TRUE);
}

void	save_output (void)
{
	char name [STRLEN];
	int file;

	if (!Browse_file_name (TRUE, "", "", 0, name, &output_filter_pos, EditCurDir))
		return;
	file = new_filedir (name);
	if (! wr_output (file))
		MsgBox ("Write file", MB_OK, "There was an error writing file %s:\n%s",name, ErrText ());
}

void	choose_output_font (void)
{
	if (! choose_font (&CompileFont, &CompileFontSize, "Select a font for compiler output window", FALSE))
		return;
	Write_compiler_font ();
	SendMessage (CompileListBox, WM_SETFONT, (WPARAM) CompileFont, MAKELPARAM (1, 0));
	recalc_extent ();
}

void	ReceiveString (char * s)
{
	addline (s);
}

void	ReceiveComment (char * s)
{
	SetDlgItemText (CompileWindow, COMPILE_COMMENT, s);
}

void	ReceiveCaption (char * s)
{
	SetDlgItemText (CompileWindow, COMPILE_CAPTION, s);
}

void	add_error_ln (char * s, long errnum, int x, int y, int f, ERRCLASS class)
{
	int c;
	char * p;
	for (;;)	{
		p = s;
		while (*p && *p != '\r' && *p != '\n') ++p;
		c = *p;
		*p = 0;
		Add_error (s, errnum, x, y, f, class);
		if (!c) return;
		class = MSG_TEXT;
		s = p+1;
		if (c == '\r' && *s == '\n') ++ s;
		if (!*s) return;
	}
}

void	ReceiveError (char * filename, char * body, long errnum, long x, long y, ERRCLASS class)
{
	int f;
	char s [32];

	if (y > 0) -- y;
	if (x > 0) -- x;

	if (filename [0])
		f = new_filedir (filename);
	else
		f = 0;
	add_error_ln (body, errnum, x, y, f, class);
	if (class == MSG_WARNING)       {
		wsprintf (s, "Warnings: %d", warn_count);
		SetDlgItemText (CompileWindow, COMPILE_WARNINGS, s);
	} else if (class == MSG_ERROR || class == MSG_SEVERE) 	{
		wsprintf (s, "Errors: %d", err_count+severe_count);
		SetDlgItemText (CompileWindow, COMPILE_ERRORS, s);
	}
	if (class == MSG_SEVERE)
		PostMessage (CompileWindow, WMSG_SEVERE, 0, 0);
}

void	DrawProgressComment (void)
{
	char s [300];
	wsprintf (s, CurrentJobComment, cur_comment_progress);
	SetDlgItemText (CompileWindow, COMPILE_PROGRESS_COMMENT, s);
}

int progress_count;

void	ReceiveProgress (long comment_progress, long progress)
{
	if (progress_present)
		if (progress > 0)
			SendMessage (ProgressIndicator, MSG_PROGRESS, 0, progress);
		else if (progress < 0)
			SendMessage (ProgressIndicator, MSG_PROGRESS_POS, 0, - progress);
	if (CurrentJobComment [0])	{
		if (comment_progress == 0)
			return;
		if (comment_progress >= 0)
			cur_comment_progress += comment_progress;
		else
			cur_comment_progress = -comment_progress;
		DrawProgressComment ();
	}
}

void	ReceiveJob (long progress_limit, char * comment)
{
	scpy (CurrentJobComment, comment);
	cur_comment_progress = 0;
	progress_count=0;

	DrawProgressComment ();
	if (progress_limit)	{
		SendMessage (ProgressIndicator, MSG_PROGRESS_LIMIT, 0, progress_limit);
		if (!progress_present)	{
			progress_present = TRUE;
			ShowWindow (ProgressIndicator, SW_NORMAL);
		}
	} else
		if (progress_present)	{
			ShowWindow (ProgressIndicator, SW_HIDE);
			progress_present = FALSE;
		}
}

void	DisconnectPeer (void)
{
	if (MakerProcess)	{
		TerminateProcess (MakerProcess, 1);
		TerminateThread  (ThreadHandle, 1);
		CloseHandle (ThreadHandle);
		CloseHandle (readpipe);
	}
	PostMessage (CompileWindow, WMSG_DISCONNECT, 0, 0);
}

#define GetLong(addr) (*(LONG*)(addr))
#define GetWord(addr) (*(WORD*)(addr))

BOOL	readbuf (char * buf, int len)
{
	DWORD n;
	while (len > 0)	{
		if (! ReadFile (readpipe, buf, len, &n, 0))
			return FALSE;
		if (!n) return FALSE;
		len -= n;
		buf += n;
	}
	return TRUE;
}

BOOL	readstring (char * s, int size)
{
  int len;
  char ch;
  for (len=0; len<size; len++) {
    if (!readbuf (s+len, 1)) return FALSE;
    if (!s[len])             return TRUE;
  }
  s[size-1] = (char)0;
  while(TRUE) {
    if (!readbuf (&ch, 1)) return FALSE;
    if (!ch)               return TRUE;
  }
}

BOOL readint(int *d)
{
  char ch;
  *d = 0;
  while (1)
  {
    if (!readbuf (&ch, 1))
      return FALSE;
    if (ch=='_')
      return TRUE;
    if ((ch<'0') || (ch>'9'))
      return FALSE;
    *d = *d * 10 + (ch-'0');
  }
}

BOOL	was_warning_bad_cmd = FALSE;

BOOL	GetCommand (char cmd)
{
	//int l, lt;
	char buf [32];
	char s [1024];
	char f [1024];
	long len, x, y, errnum, lenFn;
	ERRCLASS errclass;

	switch (cmd)	{
	case 'S':
	case 'C':
	case 'M':	
			if (!readstring (s, sizeof (s)))	
        return FALSE;
			(cmd == 'S' ? addline :
			 cmd == 'C' ? ReceiveCaption : ReceiveComment) (s);
			return TRUE;

	case 'E':
      if (  !readint(&errnum) 
         || !readint(&y) 
         || !readint(&x) 
         || !readbuf (&errclass, 1))
        return FALSE;
			if (!readstring (f, sizeof (f))) return FALSE;
			if (!readstring (s, sizeof (s))) return FALSE;
			ReceiveError (f, s, errnum, x, y, errclass);
			return TRUE;

	case 'J':
      if (!readint(&x))                return FALSE; // progress_limit
			if (!readstring (s, sizeof (s))) return FALSE;
			ReceiveJob (x, s);
			return TRUE;

	case 'P':
      if (  !readint(&x)  // comment_progress
         || !readint(&y)) // progress
        return FALSE;
			ReceiveProgress (x, y);
			return TRUE;
	case 'F':
			Clear_file_list ();
			return TRUE;
	case 'f':
			if (!readstring (s, sizeof (s))) return FALSE;
			Add_file_list (new_filedir (s));
			return TRUE;
	case 'X':
			Commit_file_list ();
			return TRUE;
	case 'm':
			if (!readbuf (buf, 1)) return FALSE;
			switch (buf[0])	{
			case SORT_OFF:	sort_errors = FALSE; break;
			case SORT_ON:	sort_errors = TRUE; break;
			}
			return TRUE;
	}
	if (!was_warning_bad_cmd)	{
		was_warning_bad_cmd = TRUE;
		wsprintf (s, "Unknown escape command encountered: %c (%02X)", cmd, cmd);
		ReceiveError ("", s, 0, -1, -1, MSG_WARNING);
	}
  if (!readstring(s, sizeof(s))) // try to skip some data..
    return FALSE;
	return TRUE;
}

void    check_filter_error (char * str)
{
	char * body, * filename;
	int bodylen, fnamelen, line, col;
	ERRCLASS errclass;
	char s [1024];
	char f [1024];
	
	if (!line_matches_filter (str, &body, &bodylen, &filename, &fnamelen,
			         &line, &col, &errclass))
		return;
	if (bodylen >= sizeof (s)) bodylen = sizeof (s)-1;
	if (fnamelen >= sizeof (f)) fnamelen = sizeof (f)-1;
	memcpy (f, filename, fnamelen);
	f[fnamelen] = 0;
	memcpy (s, body, bodylen);
	s[bodylen] = 0;
	ReceiveError (f, s, 0, col, line, errclass);
}

void    addbuf (char * s, int * n)
{
	s [*n] = 0;
	*n = 0;
	addline (s);
	check_filter_error (s);
}


DWORD   WINAPI ListenPipe (LPVOID param)
{
	char buf [1024];
	char b;
	int j, i = 0;
	DWORD n;

	IGNORE_PARAM (param);
	PostMessage (CompileWindow, WMSG_CONNECT, 0, 0);
	j = ReadFile (readpipe, &b, 1, &n, 0) && n;
	SetDlgItemText (CompileWindow, COMPILE_CAPTION, "");
	was_warning_bad_cmd = FALSE;
	if (j)
		do	{
			if (b == 1) {
				if (i)
					addbuf (buf, &i);
				if (!ReadFile (readpipe, &b, 1, &n, 0) || !n ||
				    !GetCommand (b))
					break;
			} else if (b == '\n')
			       addbuf (buf, &i);
			else if (b != '\r' && i < sizeof (buf)-2)
			       buf [i++] = b;
		} while (ReadFile (readpipe, &b, 1, &n, 0) && n);
	if (i)  addbuf (buf, &i);

	CloseHandle (readpipe);
	PostMessage (CompileWindow, WMSG_DISCONNECT, 0, 0);
	ExitThread (0);
	return 0;
}

void	remember_help_file (char * name)
{
	char hfile [STRLEN];
	FILE * f;
	change_ext (name, hfile, "hlp");
	f = fopen (hfile, "r");
	if (!f)	{
		strcpy (hfile, HomeDir);
		strcpy (hfile, "\\");
		strcpy (hfile, name_only (name));
		change_ext (hfile, hfile, "hlp");
		f = fopen (hfile, "r");
	}
	if (f)	{
		fclose (f);
		errors_help_file = new_filedir (hfile);
	} else
		errors_help_file = 0;
}

BOOL	RunMakerError (char * line1, char * line2, DWORD err)
{
	ReceiveError ("", "Error starting maker", 0, -1, -1, MSG_SEVERE);
	addline (line1);
	if (line2)  addline (line2);
	addline (EText (err));
	SetDlgItemText (CompileWindow, COMPILE_CAPTION, "");
	return FALSE;
}

DWORD	was_full_screen;

void	set_console_windowed (void)
{
	DWORD type;
	DWORD val;
	DWORD len;
	LONG err;
	HKEY key;

	was_full_screen = 0;
	key = 0;
	err = RegOpenKeyEx (HKEY_CURRENT_USER, "Console", 0,
			    KEY_QUERY_VALUE | KEY_SET_VALUE, &key);

	if (err != ERROR_SUCCESS) return;

	type = val = 0;
	len = sizeof (DWORD);
	err = RegQueryValueEx (key, "FullScreen", 0,
    			       &type, (char*) &val, &len);
	if (err == ERROR_SUCCESS && type == REG_DWORD)
		was_full_screen = val;

	val = 0;
	err = RegSetValueEx (key, "FullScreen", 0,
			     REG_DWORD, (char*) &val, sizeof (DWORD));

	RegCloseKey (key);
}

void	reset_console_windowed (void)
{
	LONG err;
	HKEY key;

	if (!was_full_screen) return;
	key = 0;
	err = RegOpenKeyEx (HKEY_CURRENT_USER, "Console", 0,
			    KEY_QUERY_VALUE | KEY_SET_VALUE, &key);

	if (err != ERROR_SUCCESS) return;

	err = RegSetValueEx (key, "FullScreen", 0,
			     REG_DWORD, (char*) &was_full_screen, sizeof (DWORD));
	RegCloseKey (key);
}

DWORD WINAPI reset_console_thread (LPVOID process)
{
	WaitForSingleObject ((HANDLE) process, 1000);
	reset_console_windowed ();
	return 0;
}

BOOL	RunMaker (void)
{
	HANDLE writepipe, h, dummyreadpipe, dummywritepipe;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD	ThreadId;
	DWORD	err;
	BOOL b;
	SECURITY_ATTRIBUTES sa;
	char new_cmd_line [STRLEN];
	char exename [STRLEN];

	lbox_extent = 0;
	sort_errors = TRUE;

	err = prepare_cmd_line (run_cmdline, new_cmd_line, exename);
	if (err)
		return RunMakerError ("Couldn't open executable file:", new_cmd_line, err);

	remember_help_file (exename);

	sa.nLength = sizeof (sa);
	sa.lpSecurityDescriptor = 0;
	sa.bInheritHandle = TRUE;

	if (! CreatePipe (&readpipe, &writepipe, &sa, 1024))
		return RunMakerError ("Can't create pipe", NULL, GetLastError ());

	if (! CreatePipe (&dummyreadpipe, &dummywritepipe, &sa, 1024))
		dummyreadpipe = dummywritepipe = INVALID_HANDLE_VALUE;

	if (dummywritepipe != INVALID_HANDLE_VALUE)
		CloseHandle (dummywritepipe);

	SetDlgItemText (CompileWindow, COMPILE_CAPTION, "Loading compiler");
	memset (&si, 0, sizeof (si));
	si.cb = sizeof (si);
	si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	si.hStdInput = dummyreadpipe;/*(HANDLE)-1;*/
	si.hStdOutput = writepipe;
	si.hStdError = writepipe;
	si.lpTitle = title_str;
	si.wShowWindow = SW_HIDE;

	set_console_windowed ();
	SetEnvironmentVariable ("__XDS_SHELL__", exename);
	b = CreateProcess (NULL, new_cmd_line, 0, 0, TRUE,
			CREATE_DEFAULT_ERROR_MODE |/* DETACHED_PROCESS |*/ CREATE_SEPARATE_WOW_VDM,
			0, run_startdir, &si, &pi);
	SetEnvironmentVariable ("__XDS_SHELL__", 0);
	err = GetLastError ();
	CloseHandle (writepipe);
	if (dummyreadpipe != INVALID_HANDLE_VALUE)
		CloseHandle (dummyreadpipe);
	if (!b)	{
		CloseHandle (readpipe);
		reset_console_windowed ();
		return RunMakerError ("Couldn't start process:", new_cmd_line, err);
	}
	MakerProcess = pi.hProcess;
	if (was_full_screen)	{
		h = CreateThread (NULL, 16384, reset_console_thread, (LPVOID) MakerProcess, 0, &ThreadId);
		if (!h)	{
			Sleep (1000);
//			reset_console_windowed ();
		} else
			CloseHandle (h);
	}

	ThreadHandle = CreateThread (NULL, 16384, ListenPipe, 0, 0, &ThreadId);
	if (!ThreadHandle)
		return RunMakerError ("Couldn't start thread", NULL, GetLastError());
	return TRUE;
}

BOOL	run_tool_in_window (void)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD	err;
	int	r;
	BOOL b;
	SECURITY_ATTRIBUTES sa;
	char new_cmd_line [STRLEN];
	char exename [STRLEN];

again:
	err = prepare_cmd_line (run_cmdline, new_cmd_line, exename);
	if (err)	{
		r = MsgBox ("Running tool", MB_RETRYCANCEL | MB_ICONSTOP,
			"Couldn't open executable file:%s\n%s", new_cmd_line, EText (err));
		if (r == IDRETRY)
			goto again;
		return FALSE;
	}

	remember_help_file (exename);

	sa.nLength = sizeof (sa);
	sa.lpSecurityDescriptor = 0;
	sa.bInheritHandle = TRUE;

	memset (&si, 0, sizeof (si));
	si.cb = sizeof (si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_NORMAL;
	si.lpTitle = title_str;
	b = CreateProcess (NULL, new_cmd_line, 0, 0, FALSE,
			CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE |
			CREATE_SEPARATE_WOW_VDM, 0, run_startdir, &si, &pi);

	if (!b)	{
		r = MsgBox ("Running tool", MB_RETRYCANCEL | MB_ICONSTOP,
			    "Couldn't start process:%s\n%s", run_cmdline,
			    LastErrText ());
		if (r == IDRETRY) goto again;
		return FALSE;
	}
	MakerProcess = pi.hProcess;
	CloseHandle (MakerProcess);
#if 0
	ThreadHandle = CreateThread (NULL, 16384, ListenPipe, 0, 0, &ThreadId);
	if (!ThreadHandle)
		return RunMakerError ("Couldn't start thread", NULL, GetLastError());

#endif
	return TRUE;
}

struct {
	int id;
	int x1, y1, x2, y2;
	int xdir1, ydir1;
	int xdir2, ydir2;
}
	bounds [20];

int	nbounds = 0;

int	add_bound (HWND hwnd, int id, int xdir1, int ydir1, int xdir2, int ydir2)
{
	RECT rc, rc0;
	HWND w = GetDlgItem (hwnd, id);
	Get_child_pos (w, &rc);
	GetClientRect (hwnd, &rc0);
	bounds [nbounds].x1 = xdir1 < 0 ? rc.left  : xdir1 > 0 ? rc0.right  - rc.left  : rc.left - rc0.right / 2;
	bounds [nbounds].x2 = xdir2 < 0 ? rc.right : xdir2 > 0 ? rc0.right  - rc.right : rc.right - rc0.right / 2;
	bounds [nbounds].y1 = ydir1 < 0 ? rc.top   : ydir1 > 0 ? rc0.bottom - rc.top   : rc.top - rc0.bottom / 2;
	bounds [nbounds].y2 = ydir2 < 0 ? rc.bottom: ydir2 > 0 ? rc0.bottom - rc.bottom: rc.bottom - rc0.bottom / 2;
	bounds [nbounds].xdir1 = xdir1;
	bounds [nbounds].xdir2 = xdir2;
	bounds [nbounds].ydir1 = ydir1;
	bounds [nbounds].ydir2 = ydir2;
	bounds [nbounds].id = id;
	return nbounds ++;
}

void	calc_resize (HWND hwnd, int i, RECT * r)
{
	RECT rc;
	int x1, y1, x2, y2;

	GetClientRect (hwnd, &rc);
	x1 = bounds [i].x1;
	y1 = bounds [i].y1;
	x2 = bounds [i].x2;
	y2 = bounds [i].y2;
	x1 = bounds [i].xdir1 < 0 ? x1 : bounds [i].xdir1 > 0 ? rc.right - x1 : rc.right / 2 + x1;
	x2 = bounds [i].xdir2 < 0 ? x2 : bounds [i].xdir2 > 0 ? rc.right - x2 : rc.right / 2 + x2;
	y1 = bounds [i].ydir1 < 0 ? y1 : bounds [i].ydir1 > 0 ? rc.bottom - y1 : rc.bottom / 2 + y1;
	y2 = bounds [i].ydir2 < 0 ? y2 : bounds [i].ydir2 > 0 ? rc.bottom - y2 : rc.bottom / 2 + y2;
	r->left = x1; r->right = x2;
	r->top = y1; r->bottom = y2;
}

void	process_resize (HWND hwnd)
{
	RECT rc;
	int i;

	for (i = 0; i < nbounds; i++)	{
		calc_resize (hwnd, i, &rc);
		SetWindowPos (GetDlgItem (hwnd, bounds [i].id), 0, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER);
		InvalidateRect (GetDlgItem (hwnd, bounds [i].id), 0, 0);
	}
}

BOOL CALLBACK CompileProc   (HWND hwnd, UINT msg,
			    WPARAM wparam, LPARAM lparam)
{
	char s [100];
	RECT rc;
	DWORD rescode;
	HMENU menu;
	HBRUSH brush;
	HELPINFO * h;
	static POINT min_size;

	IGNORE_PARAM (lparam);

	switch (msg) {
	case WM_CLOSE:
		CompileWindow = 0;
		CompileListBox = 0;
		EndDialog (hwnd, 1);
		return 1;
	case WM_INITDIALOG:
		nbounds = 0;
		add_bound (hwnd, COMPILE_CAPTION, -1, -1, 1, -1);
		add_bound (hwnd, COMPILE_ERRORS, -1, -1, -1, -1);
		add_bound (hwnd, COMPILE_WARNINGS, 1, -1, 1, -1);
		add_bound (hwnd, COMPILE_LISTBOX, -1, -1, 1, 1);
		add_bound (hwnd, IDCANCEL, -1, 1, -1, 1);
		add_bound (hwnd, IDOK, 1, 1, 1, 1);
		add_bound (hwnd, COMPILE_PROGRESS_COMMENT, -1, -1, 0, -1);
		add_bound (hwnd, COMPILE_COMMENT, -1, -1, 1, -1);
		add_bound (hwnd, COMPILE_PROGRESS_INDICATOR, 0, -1, 1, -1);
		add_bound (hwnd, COMPILE_CAPTION_FRAME, -1, -1, 1, -1);
		add_bound (hwnd, COMPILE_COMMENT_FRAME, -1, -1, 1, -1);
		add_bound (hwnd, COMPILE_SMALL_FRAME, -1, -1, 1, -1);
		add_bound (hwnd, COMPILE_BIG_FRAME, -1, -1, 1, 1);
		add_bound (hwnd, COMPILE_BOTTOM_FRAME, -1, 1, 1, 1);
		GetWindowRect (hwnd, &rc);
		min_size.x = rc.right - rc.left;
		min_size.y = rc.bottom - rc.top;
		Center_and_resize_window (hwnd, 75, 75);
		MakerProcess = 0;
		CompileWindow = hwnd;
		CompileListBox = GetDlgItem (hwnd, COMPILE_LISTBOX);
		SendMessage (CompileListBox, WM_SETFONT, (WPARAM) CompileFont, 0);
		ProgressIndicator = GetDlgItem (hwnd, COMPILE_PROGRESS_INDICATOR);
		SetWindowText (hwnd, title_str);
		SetDlgItemText (CompileWindow, COMPILE_COMMENT, "");
		SetDlgItemText (CompileWindow, COMPILE_PROGRESS_COMMENT, "");
		progress_present = FALSE;
		ShowWindow (ProgressIndicator, SW_HIDE);
		SetDlgItemText (hwnd, IDCANCEL, StopTitle [StopState = TRUE]);
		SetFocus (GetDlgItem (hwnd, IDCANCEL));
		GetWindowRect (CompileListBox, &rc);
		ListBoxSX = rc.right - rc.left;
		ListBoxSY = rc.bottom - rc.top;
		recalc_extent ();
		PostMessage (hwnd, WMSG_START, 0, 0);
		menu = GetSystemMenu (hwnd, FALSE);
		AppendMenu (menu,  MF_SEPARATOR, (WPARAM)-1, "");
		AppendMenu (menu,  MF_STRING, COMPILE_COPY_OUTPUT, "Copy program output");
		AppendMenu (menu,  MF_STRING, COMPILE_SAVE_OUTPUT, "Save program output");
		AppendMenu (menu,  MF_STRING, COMPILE_CHOOSE_FONT, "Change font");
		EnableMenuItem (menu, COMPILE_COPY_OUTPUT, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem (menu, COMPILE_SAVE_OUTPUT, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem (menu, COMPILE_CHOOSE_FONT, MF_ENABLED | MF_BYCOMMAND);
		EnableWindow (GetDlgItem (hwnd, IDOK), FALSE);
		clear_list ();
		return 1;
	case WM_GETMINMAXINFO:
		((MINMAXINFO *) lparam)->ptMinTrackSize = min_size;
		return 0;
	case WM_SIZE:
		process_resize (hwnd);
		InvalidateRect (hwnd, 0, 1);
		if (wparam == SIZE_MINIMIZED)
			ShowWindow (GetParent (hwnd), SW_MINIMIZE);
		break;
	case WM_ERASEBKGND:
		GetClientRect (hwnd, &rc);
		brush = CreateSolidBrush (GetSysColor (COLOR_3DFACE));
		FillRect ((HDC) wparam, &rc, brush);
		DeleteObject (brush);
		return 1;
	case WMSG_START:
		if (!RunMaker ()) return 1;
		return 1;
	case WMSG_CONNECT:
		SetDlgItemText (hwnd, IDCANCEL, StopTitle [StopState = TRUE]);
		SetFocus (GetDlgItem (hwnd, IDCANCEL));
		return 1;
	case WMSG_DISCONNECT:
		rescode = 0;
		if (MakerProcess)	{
			if (! GetExitCodeProcess (MakerProcess, &rescode))
				rescode = 0;
			if (rescode == STILL_ACTIVE)	{
				TerminateProcess (MakerProcess, 1);
				WaitForSingleObject (MakerProcess, 2000);
			}
			CloseHandle (MakerProcess);
			MakerProcess = 0;
		}

		if (severe_count)
			SetDlgItemText (hwnd, COMPILE_CAPTION, "Severe error detected: make aborted");
		else if (err_count)
			SetDlgItemText (hwnd, COMPILE_CAPTION, "Errors detected");
		else if (rescode == STILL_ACTIVE)	{
			SetDlgItemText (hwnd, COMPILE_CAPTION, "Connection lost");
			wsprintf (s, "Output pipe closed or improperly formatted");
			Add_error (s, 0, -1, -1, 0, MSG_SEVERE);
			addline (s);
		} else if (GoodResCode >= 0 && (int) rescode > GoodResCode)	{
			SetDlgItemText (hwnd, COMPILE_CAPTION, "Bad return code");
			wsprintf (s, "Bad return code from maker: %ld", rescode);
			Add_error (s, 0, -1, -1, 0, MSG_SEVERE);
			addline (s);
		} else if (warn_count)
			SetDlgItemText (hwnd, COMPILE_CAPTION, "Success (with warnings)");
		else
			SetDlgItemText (hwnd, COMPILE_CAPTION, "Success");
		if (!WaitForKey)
			EndDialog (hwnd, 1);
		SetDlgItemText (hwnd, IDCANCEL, StopTitle [StopState = FALSE]);
		EnableWindow (GetDlgItem (hwnd, IDOK), msg_count != 0);
		SendMessage (CompileListBox, LB_SETHORIZONTALEXTENT, lbox_extent+10, 0);
		SendMessage (CompileListBox, LB_SETTOPINDEX, SendMessage (CompileListBox, LB_GETCOUNT, 0, 0)-1, 0);
		menu = GetSystemMenu (hwnd, FALSE);
		EnableMenuItem (menu, COMPILE_COPY_OUTPUT, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem (menu, COMPILE_SAVE_OUTPUT, MF_ENABLED | MF_BYCOMMAND);
		sort_errors = TRUE;
		if (err_count+severe_count)
			SetFocus (GetDlgItem (hwnd, IDOK));
		else
			SetFocus (GetDlgItem (hwnd, IDCANCEL));
		return 1;
	case WMSG_SEVERE:
		DisconnectPeer ();
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDCANCEL:
			if (StopState)	{
				DisconnectPeer ();
				Add_error ("Project make aborted", 0, -1, -1, 0, MSG_SEVERE);
				addline ("Make aborted");
			} else	{
				clear_list ();
				EndDialog (hwnd, 0);
			}
			return 1;
		case IDOK:
			clear_list ();
			EndDialog (hwnd, 1);
			return 1;
		}

		return 1;
	case WM_SYSCOMMAND:
		switch (CONTROL_ID)	{
		case COMPILE_COPY_OUTPUT: copy_output (); return 1;
		case COMPILE_SAVE_OUTPUT: save_output (); return 1;
		case COMPILE_CHOOSE_FONT: choose_output_font (); return 1;
		case SC_CLOSE:
			SendMessage (hwnd, WM_COMMAND, IDCANCEL, 0);
			return 1;
		}
		return 0;
	case WM_HELP:
		h = (HELPINFO*) lparam;
		if (h->iContextType == HELPINFO_MENUITEM)	{
			if (h->iCtrlId <= 20000)
				Control_id_help (COMPILE_DIALOG, h->iCtrlId);
		} else if (h->iCtrlId == IDCANCEL && StopState)
			Control_id_help (COMPILE_DIALOG, COMPILE_STOP);
		else
			Control_help (COMPILE_DIALOG, lparam);
		return 1;
	}
	return 0;
}

BOOL	Hide_file_list (void);

void	CallTool (char * Cmd, char * dir, char * title, BOOL popup, BOOL wait, BOOL show_list, int good_code)
{
	int r;

	if (!Cmd || !Cmd[0]) return;
	GoodResCode = good_code;
	Make_line (Cmd, run_cmdline, sizeof (run_cmdline));
	Make_line (dir, run_startdir, sizeof (run_startdir));
	if (!run_startdir[0])
		strcpy (run_startdir, ProjDir);

	if (title) Make_line (title, title_str, sizeof (title_str));

	Clear_messages ();
	if (popup)	{
		WaitForKey = wait;
		r = Dialog (COMPILE_DIALOG, CompileProc);
	} else
		run_tool_in_window ();
	Update_file_list ();
	if (show_list) Show_file_list ();
	if (r)	{
		Show_messages ();
		First_message ();
	}
}

DWORD   WINAPI RunThreadProc (LPVOID param)
{
	IGNORE_PARAM (param);

	Enable_item (IDM_RUN,   FALSE);
	Enable_item (IDM_DEBUG, FALSE);
	Enable_item (IDM_STOP,  TRUE);
	Enable_item (IDM_VIEWUSERS, TRUE);
	WaitForSingleObject (RunProcess, INFINITE);
	CloseHandle (RunProcess);
	RunProcess = 0;
	RunProcessId = 0;
	Enable_item (IDM_RUN,  TRUE);
	Enable_item (IDM_DEBUG, DebugCmd [0]!=0);
	Enable_item (IDM_STOP, FALSE);
	Enable_item (IDM_VIEWUSERS, SameConsole && ConsoleHandle);
	if (SameConsole)
		SetConsoleTitle ("XDS console: program terminated");
	if (!SameConsole && ConsoleHandle)
		Reset_console ();
	RunThread = 0;
	ExitThread (0);
	return 0;
}

#define CONSOLE_TITLE "XDS console"

BOOL CALLBACK EnumTProc (HWND hwnd, LPARAM lparam)
{
	char s [100];
	DWORD pid, mypid;

	IGNORE_PARAM (lparam);

	mypid = GetCurrentProcessId ();
	GetWindowThreadProcessId (hwnd, &pid);
	GetWindowText (hwnd, s, sizeof (s));
	if (pid == mypid && !strcmp (s, CONSOLE_TITLE))
		ConsoleHandle = hwnd;
	return TRUE;
}

BOOL CALLBACK EventHandler (DWORD type)
{
	return type == CTRL_C_EVENT || type == CTRL_BREAK_EVENT;
}

void	create_console (void)
{
	HMENU menu;

	AllocConsole ();
	SetConsoleTitle (CONSOLE_TITLE);
	Sleep (40);
	ConsoleHandle = 0;
	EnumWindows (EnumTProc, 0);
	if (!ConsoleHandle)
		ConsoleHandle = GetForegroundWindow ();
	menu = GetSystemMenu (ConsoleHandle, FALSE);
	DeleteMenu (menu, SC_CLOSE, MF_BYCOMMAND);
	SendMessage (ConsoleHandle, WM_SETICON, 0, (LPARAM) Console_icon);
	ConsoleCreated = TRUE;
	SetConsoleCtrlHandler (EventHandler, TRUE);
}

void	Reset_console (void)
{
	SetConsoleCtrlHandler (EventHandler, FALSE);
	GetSystemMenu (ConsoleHandle, TRUE);
	if (RunProcess) return;
	FreeConsole ();
	ConsoleCreated = FALSE;
//	SendMessage (ConsoleHandle, WM_SYSCOMMAND, SC_CLOSE, 0);
	ConsoleHandle = 0;
	Enable_item (IDM_VIEWUSERS, RunProcess != 0);
}

/*
  Returns:      IMAGE_SUBSYSTEM_NATIVE
		IMAGE_SUBSYSTEM_WINDOWS_GUI
		IMAGE_SUBSYSTEM_WINDOWS_CUI
		IMAGE_SUBSYSTEM_OS2_CUI
		IMAGE_SUBSYSTEM_POSIX_CUI
	or
		-1
*/

int TypeOfExe (LPSTR filename)
{
	HANDLE hFile;
	HANDLE hFileMapping;
	LPVOID lpFileBase;
	PIMAGE_DOS_HEADER dosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	int type;

	hFile = CreateFile (filename, GENERIC_READ, FILE_SHARE_READ, NULL,
				      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return -1;
	hFileMapping = CreateFileMapping (hFile, NULL, PAGE_READONLY, 0, 0,
					  NULL);
	if (hFileMapping == 0) {
		CloseHandle (hFile);
		return -1;
	}
	lpFileBase = MapViewOfFile (hFileMapping, FILE_MAP_READ, 0, 0, 0);
	if (lpFileBase == 0) {
		CloseHandle (hFileMapping);
		CloseHandle (hFile);
		return -1;
	}
	type = -1;
	dosHeader = (PIMAGE_DOS_HEADER) lpFileBase;
	if (dosHeader -> e_magic == IMAGE_DOS_SIGNATURE) {
		pNTHeader = (PIMAGE_NT_HEADERS) ((DWORD) dosHeader +
							 dosHeader -> e_lfanew);
		if (! IsBadReadPtr (pNTHeader, sizeof (IMAGE_NT_HEADERS)) &&
		    pNTHeader -> Signature == IMAGE_NT_SIGNATURE)
			type = ((PIMAGE_OPTIONAL_HEADER)
				 & pNTHeader -> OptionalHeader) -> Subsystem;
	}
	UnmapViewOfFile (lpFileBase);
	CloseHandle (hFileMapping);
	CloseHandle (hFile);
	return type;
}

LONG	binary_type;
int	image_type;
BOOL	is_console_app;

typedef	enum	{
		UNKNOWN,
		WIN32GUI,
		WIN32CONSOLE,
		POSIX,
		OS2,
		WIN16,
		DOS}
			EXETYPE;

EXETYPE	ExeType;

#define exestr(b) (b==UNKNOWN?	"UNKNOWN":	\
		   b==WIN32GUI? "Win32GUI":	\
		   b==WIN32CONSOLE? "Win32Console":	\
		   b==POSIX ? "Posix":			\
		   b==OS2 ?"OS/2":			\
		   b==WIN16 ? "Win16":			\
		   b==DOS ? "DOS":"???")

#define bin_str(b) (b == SCS_32BIT_BINARY? "A Win32-based application":		\
		    b == SCS_DOS_BINARY?   "An MS-DOS - based application":	\
		    b == SCS_OS216_BINARY? "A 16-bit OS2-based application":	\
		    b == SCS_PIF_BINARY?   "A PIF file that executes an MS-DOS - based application":\
		    b == SCS_POSIX_BINARY? "A POSIX - based application":	\
		    b == SCS_WOW_BINARY?   "A 16-bit Windows-based application":"unknown type")

#define img_str(i) (i == IMAGE_SUBSYSTEM_NATIVE ? "Native":			\
		    i == IMAGE_SUBSYSTEM_WINDOWS_GUI? "Windows GUI":		\
		    i == IMAGE_SUBSYSTEM_WINDOWS_CUI? "Windows Console":	\
		    i == IMAGE_SUBSYSTEM_OS2_CUI?    "OS2 console":		\
		    i == IMAGE_SUBSYSTEM_POSIX_CUI ? "Posix console":"unknown")

/*
0				Nonexecutable file or an error condition.
LOWORD = NE or PE
HIWORD = 3.0, 3.5, or 4.0	Windows application

LOWORD = MZ
HIWORD = 0	MS-DOS .EXE, .COM or .BAT file

LOWORD = PE
HIWORD = 0	Win32 console application
*/

BOOL	determine_exe_type (char * f)
{
	DWORD dw;

	if (Verinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)	{
		dw = SHGetFileInfo (f, 0, 0, 0, SHGFI_EXETYPE);
		if (!dw)	{
			MsgBox ("Run program", MB_OK|MB_ICONSTOP,
				 "Executable file %s is not executable", f);
			return FALSE;
		}
		if (LOWORD (dw) == 'P'+'E'*256)
			ExeType = HIWORD (dw) ? WIN32GUI : WIN32CONSOLE;
		else if (LOWORD (dw) == 'N'+'E'*256)
			ExeType = WIN16;
		else if (LOWORD (dw) == 'M'+'Z'*256)
			ExeType = DOS;
		else
			ExeType = UNKNOWN;
		is_console_app = ExeType != WIN16 && ExeType != WIN32GUI;
		return TRUE;
	} else	{
		if (! GetBinaryType (f, &binary_type))	{
			MsgBox ("Run program", MB_OK|MB_ICONSTOP,
				 "Executable file %s is not executable\n%s", f, LastErrText ());
			return FALSE;
		}
		image_type = TypeOfExe (f);
		if (binary_type == SCS_32BIT_BINARY)
			is_console_app = image_type != IMAGE_SUBSYSTEM_WINDOWS_GUI;
		else
			is_console_app = binary_type != SCS_WOW_BINARY;
		return TRUE;
	}
}

void	RunUserProcess (char * cmdline, int runmode)
{
	DWORD ThreadId;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char startdir [2000];
	char new_cmd_line [STRLEN];
	char exename [STRLEN];
	long err;

	err = prepare_cmd_line (cmdline, new_cmd_line, exename);
	if (err)	{
		MsgBox ("Run program", MB_OK|MB_ICONSTOP,
			 "Couldn't open executable file:%s\n%s",
			 new_cmd_line, EText (err));
		return;
	}

	if (! determine_exe_type (exename))
		return;

	Make_line (RunStartDir, startdir, sizeof (startdir));
	RunThread = CreateThread (NULL, 16384, RunThreadProc, 0, CREATE_SUSPENDED, &ThreadId);
	if (!RunThread)	{
		MsgBox ("Run program", MB_ICONSTOP|MB_OK, "Couldn't start thread\n%s",
			LastErrText ());
		return;
	}

	RunProcess = 0;
	RunProcessId = 0;
	memset (&si, 0, sizeof (si));
	si.cb = sizeof (si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SameConsole ? SW_NORMAL : runmode;

	if (is_console_app)	{
		if (!ConsoleCreated && SameConsole)
			create_console ();
	} else
		Reset_console ();

	if (!CreateProcess (NULL, new_cmd_line, 0, 0, TRUE,
			    CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP |
			    CREATE_SEPARATE_WOW_VDM,
			    0, startdir [0] ? startdir : ProjDir, &si, &pi))
	{
		MsgBox ("Run program", MB_OK|MB_ICONSTOP,
			 "Couldn't start process:%s\n%s",
			 cmdline, LastErrText ());
		TerminateThread (RunThread, 0);
		CloseHandle (RunThread);
		return;
	}
	if (SameConsole)	{
		char s [500];
		sprintf (s, "XDS console: %s", cmdline);
		SetConsoleTitle (s);
		ShowWindow (ConsoleHandle, SW_NORMAL);
		BringWindowToTop (ConsoleHandle);
		SetForegroundWindow (ConsoleHandle);
	}

	RunProcess = pi.hProcess;
	RunProcessId = pi.dwProcessId;
	ResumeThread (RunThread);
}

void	RunRun (void)
{
	char cmdline [2000];
	int l;

	Read_run_options ();
	if (!ExeFile [0]) return;
	Make_line (ExeFile, cmdline, sizeof (run_cmdline));
	cmdline [sizeof (cmdline)-2] = 0;
	l = strlen (cmdline);
	if (RunArguments [0])	{
		cmdline [l++] = ' ';
		Make_line (RunArguments, cmdline+l, sizeof (cmdline)-l);
	}
	RunUserProcess (cmdline, RunMode);
}

void	RunDebug (void)
{
	char cmdline [2000];
	BOOL save_sameconsole;

	Read_run_options ();
	if (!DebugCmd [0]) return;
	Make_line (DebugCmd, cmdline, sizeof (run_cmdline));
	if (SameConsole) Reset_console ();
	save_sameconsole = SameConsole;
	SameConsole = FALSE;
	RunUserProcess (cmdline, SW_NORMAL);
	SameConsole = save_sameconsole;
}

void	RunStop (void)
{
	if (!RunProcess)	{
		MsgBox ("STOP", MB_OK|MB_ICONEXCLAMATION, "No process to terminate");
		return;
	}
	TerminateProcess (RunProcess, 1);
//	CloseHandle (RunProcess);
	CloseHandle (RunThread);
	RunThread = 0;
}

HWND	ProcessMainWindow;

BOOL CALLBACK FindProcessWndProc (HWND hwnd, LPARAM lparam)
{
	DWORD pid;
	GetWindowThreadProcessId (hwnd, &pid);
	if (pid == (DWORD) lparam)
		ProcessMainWindow = hwnd;
	return TRUE;
}

void	Show_user_screen (void)
{
	ProcessMainWindow = 0;
	if (RunProcess)
		EnumWindows (FindProcessWndProc, (LPARAM) RunProcessId);
	if (! ProcessMainWindow && SameConsole)
		ProcessMainWindow = ConsoleHandle;
	if (ProcessMainWindow)	{
		SetForegroundWindow (ProcessMainWindow);
		BringWindowToTop (ProcessMainWindow);
	}
}

