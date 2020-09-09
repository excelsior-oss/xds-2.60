#include "util.h"
#include "shell.h"
#include "fileutil.h"
#include "tbuf.h"
#include "fcache.h"
#include "winmake.h"
#include "filter.h"
#include "res.h"

#define FILTERS_NUM 100
char *	filters [FILTERS_NUM+1][2] = {0};

#define FILTERS_NAME "filters"
#define FILTER_SUFFIX ".fil"

static char write_filter_filter[] =	"Messages filters (*.fil)\0*.fil\0"
					"All files (*.*)\0*.*\0";
int	filter_filter_index = 0;

typedef struct	{
		char * s;
		char * compiled;
		ERRCLASS errclass;
	}
		FILTER_LINE;

typedef struct	{
		FILTER_LINE * lines;
		int nlines;
		int a_nlines;
	}
		FILTER;

#define FILTER_ALLOC_DELTA 16

#define T_LINE0	16
#define T_LINE	17
#define T_COL0	18
#define T_COL	19
#define T_FILE	20
#define T_NUM	21
#define T_BODY	22
#define T_SEQ	23
#define T_ANY	24

void	prepare_filter (char * s, char * d)
{
	while (*s)	{
		switch (*s)	{
		case '\\':
			if (s[1])	{
				*d++ = s[1];
				s+=2;
			} else
				*d++ = *s++;
			break;
		case '*':
			*d++ = T_SEQ;
			s++;
			break;
		case '?':
			*d++ = T_ANY;
			s++;
			break;
		case '&':
			if (!strnicmp (s+1, "line0", 5))	{
				*d++ = T_LINE0;
				s += 6;
			} else if (!strnicmp (s+1, "line", 4))	{
				*d++ = T_LINE;
				s += 5;
			} else if (!strnicmp (s+1, "col0", 4))	{
				*d++ = T_COL0;
				s += 5;
			} else if (!strnicmp (s+1, "col", 3))	{
				*d++ = T_COL;
				s += 4;
			} else if (!strnicmp (s+1, "file", 4))	{
				*d++ = T_FILE;
				s += 5;
			} else if (!strnicmp (s+1, "num", 3))	{
				*d++ = T_NUM;
				s += 4;
			} else if (!strnicmp (s+1, "body", 4))	{
				*d++ = T_BODY;
				s += 5;
			} else
				*d++ = *s++;
			break;
		default:
			*d++ = *s++;
		}
	}
	*d = 0;
}

char *	err_body;
int	err_body_len;
int	err_line, err_col;
char *	err_file;
int	err_file_len;

int	match_filter (char * f, char * s)
{
	long l;
	int len, i;

	while (*f)	{
		if (!*s) return 0;
		switch (*f)	{
		case T_ANY:
			++s;
			++f;
			break;
		case T_LINE0:
		case T_LINE:
		case T_COL0:
		case T_COL:
		case T_NUM:
			if (!isdigit (*s)) return 0;
			l = 0;
			while (isdigit (*s))	{
				l = l*10+*s-'0';
				++s;
			}
			switch (*f)	{
				case T_LINE0:	err_line = l+1; break;
				case T_LINE:	err_line = l; break;
				case T_COL0:	err_col = l+1; break; 
				case T_COL:	err_col = l; break;
			}
			++f;
			break;
		case T_FILE:
		case T_BODY:
		case T_SEQ:
			len = strlen (s);
			if (f[1])	{
				i = *f == T_FILE;
				for (; i < len; i++)
					if (match_filter (f+1, s+i))
						break;
				if (i == len) return 0;
				len = i;
			}
			switch (*f)	{
			case T_FILE: err_file = s; err_file_len = len; break;
			case T_BODY: err_body = s; err_body_len = len; break;
			}
			return 1;
		default:
			if (*s++ != *f++)
				return 0;
		}
	}
	return *s == 0;
}

void	Read_filters (void)
{
	Read_section  (filters, FILTERS_NUM,
			FILTERS_NAME, IniFile);
}

void	make_full_filter_name (char * src, char * dst)
{
	add_dir (IniFile, src, dst);
}

void	Clear_filters (void)
{
	int i;

	for (i = 0; filters [i][0]; i++)	{
		free (filters [i][0]);
		free (filters [i][1]);
		filters [i][0] = filters [i][1] = NULL;
	}
}

struct	{
		ERRCLASS errclass;
		char * code;
		char * disp;
	}
		errclass_names [] =
	{
		{MSG_TEXT,	"Text",		"Text"},
		{MSG_NOTICE,	"Notice",	"Notice"},
		{MSG_WARNING,	"Warning",	"Warning"},
		{MSG_ERROR,	"Error",	"Error"},
		{MSG_SEVERE,    "Severe",	"Severe Error"}
	};

#define ERRCLASS_NUM (sizeof (errclass_names) / sizeof (errclass_names[0]))

BOOL	cmp_prefix (char ** p, char * s)
{
	int l = strlen (s);
	if (!strnicmp (*p, s, l) && (*p)[l]=='=')	{
		*p += l+1;
		return TRUE;
	}
	return FALSE;
}

ERRCLASS decode_err_class (char ** p, int len)
{
	int i;
	while (len-- && isspace (**p)) ++*p;
	for (i = 0; i < ERRCLASS_NUM; i++)
		if (cmp_prefix (p, errclass_names [i].code))
			return errclass_names [i].errclass;
	return MSG_ERROR;
}

BOOL	filter_from_buf (FILTER * f, TBUF * b)
{
	char * p, * q;
	int i, j, len, n;

	n = b? tbuf_get_nlines (b) : 0;
	f->a_nlines = (n+FILTER_ALLOC_DELTA-1) / FILTER_ALLOC_DELTA * FILTER_ALLOC_DELTA;
	f->lines = Malloc (f->a_nlines * sizeof (FILTER_LINE));
	if (!f->lines)
		return FALSE;
	memset (f->lines, 0, f->a_nlines * sizeof (FILTER_LINE));
	j = 0;
	for (i = 0; i < n; i++)	{
		if (!tbuf_get_line (b, i, &p, &len)) break;
		q = p;
		f->lines [j].errclass = decode_err_class (&q, len);
		len -= q-p;
		if (len == 0) continue;
		f->lines [j].s = Malloc (len+1);
		if (!f->lines [j].s)
			return FALSE;
		memcpy (f->lines [j].s, q, len);
		f->lines [j].s [len] = 0;
		++j;
	}
	f->nlines = j;
	return TRUE;
}

FILTER	tool_filter;
BOOL	tool_filter_present;

void	delete_tool_filter (void)
{
	if (!tool_filter_present) return;
	tool_filter_present = FALSE;
}

BOOL	read_tool_filter (char * name)
{
	int file, r, i;
	TBUF * buf;
	char fullname [STRLEN];

	tool_filter_present = FALSE;
	if (!name || !name[0] || !stricmp (name, "none")) return TRUE;
	Read_filters ();
	for (i = 0; filters [i][0]; i++)
		if (!stricmp (filters [i][0], name))
			break;
	if (!filters [i][0] || !filters [i][1])	{
		r = MsgBox ("Message filter", MB_YESNO | MB_ICONEXCLAMATION,
			"Message filter %s not defined. Continue anyway?",
			name);
		Clear_filters ();
		return r == IDYES;
	}
	make_full_filter_name (filters [i][1], fullname);
	file = new_filedir (fullname);
	Clear_filters ();
	buf = Find_or_read_file (file);
	if (!buf)	{
		r = MsgBox ("Message filter", MB_YESNO | MB_ICONEXCLAMATION,
			"Message filter file %s not found. Continue anyway?",
			filedir [file]);
		return r == IDYES;
	}
	if (!filter_from_buf (&tool_filter, buf))	{
		tbuf_release (buf);
		r = MsgBox ("Message filter", MB_YESNO | MB_ICONEXCLAMATION,
			"No enough memory to load Message filter %s. Continue anyway?",
			name);
		return r == IDYES;
	}
	tbuf_release (buf);
	tool_filter_present = TRUE;
	return TRUE;
}

BOOL	compile_tool_filter (void)
{
	int i, r;
	char * s;

	if (!tool_filter_present) return TRUE;

	for (i = 0; i < tool_filter.nlines; i++)	{
		s = Malloc (strlen (tool_filter.lines [i].s)+10);
		if (!s)	{
			r = MsgBox ("Message filter", MB_YESNO | MB_ICONEXCLAMATION,
				"No enough memory to vompile message filter. Continue anyway?");
			delete_tool_filter();
			return r == IDYES;
		}
		prepare_filter (tool_filter.lines [i].s, s);
		tool_filter.lines [i].compiled = s;
	}
	return TRUE;
}


BOOL	line_matches_filter (char * src, char ** body, int * bodylen, char ** filename, int * fnamelen,
			     int * line, int * col, ERRCLASS * errclass)
{
	int i;

	if (!tool_filter_present) return FALSE;

	for (i = 0; i < tool_filter.nlines; i++)	{
		err_body = "1";
		err_body_len = 0;
		err_line = 0;
		err_col = 0;
		err_file = "?";
		err_file_len = 1;
		if (match_filter(tool_filter.lines [i].compiled, src))	{
			*body = err_body;
			*bodylen = err_body_len;
			*filename = err_file;
			*fnamelen = err_file_len;
			*line = err_line;
			*col = err_col;
			*errclass = tool_filter.lines [i].errclass;
			return TRUE;
		}
	}
	return FALSE;
}


FILTER	cur_filter;
int	cur_filter_file;
char *	cur_filter_name;

void	activate_editfilter_line (HWND hwnd, int i)
{
	int j, r;
	if (i >= cur_filter.nlines) i = cur_filter.nlines-1;
	if (i != (int) SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_GETCURSEL, 0, 0))
		SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_SETCURSEL, i, 0);
	if (i < 0 || !cur_filter.lines[i].s)
		SetDlgItemText (hwnd, EDITFILTER_TEXT, "");
	else
		SetDlgItemText (hwnd, EDITFILTER_TEXT, cur_filter.lines[i].s);
	r = -1;
	if (i >= 0)
		for (j = 0; j < sizeof (errclass_names) / sizeof (errclass_names[0]); j++)
			if (errclass_names [j].errclass == cur_filter.lines[i].errclass)
				r = j;
	SendDlgItemMessage (hwnd, EDITFILTER_CLASS, CB_SETCURSEL, (WPARAM)r, 0);
	EnableWindow (GetDlgItem (hwnd, EDITFILTER_TEXT), i >= 0);
	EnableWindow (GetDlgItem (hwnd, EDITFILTER_MENU), i >= 0);
	EnableWindow (GetDlgItem (hwnd, EDITFILTER_MODIFY), 0);
	EnableWindow (GetDlgItem (hwnd, EDITFILTER_REMOVE), i >= 0);
	EnableWindow (GetDlgItem (hwnd, EDITFILTER_UP), i > 0);
	EnableWindow (GetDlgItem (hwnd, EDITFILTER_DOWN), i < cur_filter.nlines-1);
}

BOOL	filter_modify (HWND hwnd)
{
	int i, j, len;
	char * p;

	i = (int) SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_GETCURSEL, 0, 0);
	if (i < 0) return FALSE;
	len = SendDlgItemMessage (hwnd, EDITFILTER_TEXT, WM_GETTEXTLENGTH, i, 0);
	p = Malloc (len+2);
	if (!p) return FALSE;
	GetDlgItemText (hwnd, EDITFILTER_TEXT, p, len+1);
	p[len] = 0;
	j = (int) SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_INSERTSTRING, i,
				    (LPARAM)(LPSTR)p);
	if (j < 0)	{
		free (p);
		return FALSE;
	}
	SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_DELETESTRING, i+1, 0);
	SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_SETCURSEL, i, 0);
	free (cur_filter.lines [i].s);
	cur_filter.lines [i].s = p;
	Set_lb_extent (GetDlgItem (hwnd, EDITFILTER_LIST));
	activate_editfilter_line (hwnd, i);
	return TRUE;
}

void	filter_remove (HWND hwnd)
{
	int i;

	i = (int) SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_GETCURSEL, 0, 0);
	if (i < 0) return;
	SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_DELETESTRING, i, 0);
	free (cur_filter.lines [i].s);
	memmove (cur_filter.lines+i, cur_filter.lines+i+1, (cur_filter.nlines-i-1) * sizeof (FILTER_LINE));
	--cur_filter.nlines;
	activate_editfilter_line (hwnd, i);
	Set_lb_extent (GetDlgItem (hwnd, EDITFILTER_LIST));
}

BOOL	filter_add (HWND hwnd)
{
	int i, j;
	FILTER_LINE * f;
	char * s;
	ERRCLASS errclass;

	i = (int) SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_GETCURSEL, 0, 0);
	if (i < 0) i = cur_filter.nlines;
	if (cur_filter.nlines == cur_filter.a_nlines)	{
		f = realloc (cur_filter.lines, (cur_filter.a_nlines + FILTER_ALLOC_DELTA) * sizeof (FILTER_LINE));
		if (!f) return FALSE;
		cur_filter.lines = f;
		cur_filter.a_nlines += FILTER_ALLOC_DELTA;
	}
	if (i >= cur_filter.nlines)	{
		s = NULL;
		errclass = MSG_ERROR;
	} else	{
		s = alloc_copy (cur_filter.lines [i].s);
		errclass = cur_filter.lines [i].errclass;
	}

	j = SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_INSERTSTRING, i, (LPARAM) (s?s:""));
	if (j < 0)	{
		free (s);
		return FALSE;
	}
	memmove (cur_filter.lines+i+1, cur_filter.lines+i, (cur_filter.nlines-i) * sizeof (FILTER_LINE));
	++ cur_filter.nlines;
	cur_filter.lines [i].s = s;
	cur_filter.lines [i].errclass = errclass;
	activate_editfilter_line (hwnd, i);
	SetFocus (GetDlgItem (hwnd, EDITFILTER_TEXT));
	return TRUE;
}

BOOL	filter_up (HWND hwnd, int i)
{
	char * s;
	FILTER_LINE l;
	int j;

	if (i <= 0) return FALSE;
	s = cur_filter.lines [i].s ? cur_filter.lines [i].s : "";
	j = SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_INSERTSTRING, i-1, (LPARAM) s);
	if (j < 0) return FALSE;
	SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_DELETESTRING, i+1, 0);
	l = cur_filter.lines [i-1];
	cur_filter.lines [i-1] = cur_filter.lines [i];
	cur_filter.lines [i] = l;
	activate_editfilter_line (hwnd, i-1);
	return TRUE;
}

BOOL	filter_down (HWND hwnd, int i)
{
	if (i >= cur_filter.nlines-1) return FALSE;
	if (!filter_up (hwnd, i+1)) return FALSE;
	activate_editfilter_line (hwnd, i+1);
	return TRUE;
}

char * filter_menu_def [][2] = {
	{"File name", "&file"},
	{"Line number", "&line"},
	{"Column number", "&col"},
	{"Line number zero-based", "&line0"},
	{"Column number zero-based", "&col0"},
	{"Message body", "&body"},
	{"Any character", "?"},
	{"Any string", "*"},
	{"Literal \"&&\"", "\\&"},
	{"Literal \"*\"", "\\*"},
	{"Literal \"?\"", "\\?"},
	{"Literal \"\\\"", "\\\\"}
};

#define FILTER_MENU_BASE 20000
#define FILTER_MENU_NUM sizeof (filter_menu_def) / sizeof (filter_menu_def [0])

void	insert_filter_item (HWND hwnd, int i)
{
	SendDlgItemMessage (hwnd, EDITFILTER_TEXT, EM_REPLACESEL, 1, (LPARAM) filter_menu_def [i][1]);
	SetFocus (GetDlgItem (hwnd, EDITFILTER_TEXT));
}

void	filter_menu (HWND hwnd)
{
	HMENU m;
	RECT rc;
	int i;

	m = CreatePopupMenu ();
	for (i = 0; i < FILTER_MENU_NUM; i++)
		AppendMenu (m, MF_STRING, FILTER_MENU_BASE+i, filter_menu_def[i][0]);
	GetWindowRect (GetDlgItem (hwnd, EDITFILTER_MENU), &rc);
	TrackPopupMenu (m, TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.right, rc.top,
			0, hwnd, 0);
	DestroyMenu (m);
}

BOOL	save_filter (void)
{
	int i, j, k, l, m;
	char * p;
	m = 0;
	for (i = 0; i < cur_filter.nlines; i++)	{
		l = strlen (cur_filter.lines [i].s)+1;
		if (l>m) m = l;
	}
	p = Malloc (m+20);
	if (!p)	{
		ErrCode = ERROR_NOT_ENOUGH_MEMORY;
		return FALSE;
	}
	Put_file_start ();
	for (i = 0; i < cur_filter.nlines; i++)	{
		k = 0;
		for (j = 0; j < ERRCLASS_NUM; j++)
			if (errclass_names [j].errclass == cur_filter.lines [i].errclass)
				k = j;
		strcpy (p, errclass_names [k].code);
		strcat (p, "=");
		strcat (p, cur_filter.lines [i].s);
		if (!Put_file (p, strlen (p)))
			return FALSE;
	}
	free (p);
	if (Put_file_fin (cur_filter_file, FALSE))
		return TRUE;
	ErrCode = 0;
	return FALSE;
}

int	Filter_fill_list (HWND list, BOOL combo, BOOL addnone)
{
	int i;
	UINT cmd = combo ? CB_ADDSTRING : LB_ADDSTRING;

	if (addnone)
		SendMessage (list, cmd, 0, (LPARAM) "None");
	for (i = 0; i < FILTERS_NUM && filters [i][0]; i ++)
		SendMessage (list, cmd, 0, (LPARAM)(LPSTR) filters [i][0]);
	return i;
}

int	Filter_search_name (char * name)
{
	int i;
	if (!name || !name[0] || !stricmp (name, "none"))
		return 0;
	for (i = 0; filters [i][0]; i++)
		if (!stricmp (filters [i][0], name))
			return i+1;
	return -1;
}

char *	Filter_get_name (int i)
{
	return i>=0 ? filters [i][0] : "";
}

BOOL CALLBACK EditFilterDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i, j;
	char s [1000];
	HFONT font;
	static HBITMAP bmp = 0;
	
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		if (!bmp) bmp = LoadBitmap (0, MAKEINTRESOURCE (OBM_MNARROW));
		SendDlgItemMessage (hwnd, EDITFILTER_MENU, BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) bmp);
		font = GetStockObject (SYSTEM_FIXED_FONT);
		SendDlgItemMessage (hwnd, EDITFILTER_LIST, WM_SETFONT, (WPARAM) font, 0);
		SendDlgItemMessage (hwnd, EDITFILTER_TEXT, WM_SETFONT, (WPARAM) font, 0);
		sprintf (s, "Edit message filter: %s", cur_filter_name);
		SetWindowText (hwnd, s);
		for (i = 0; i < cur_filter.nlines; i++)
			SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_ADDSTRING, 0,
					    (LPARAM)(LPSTR)cur_filter.lines [i].s);
		for (i = 0; i < ERRCLASS_NUM; i++)
			SendDlgItemMessage (hwnd, EDITFILTER_CLASS, CB_ADDSTRING, 0,
					    (LPARAM)(LPSTR)errclass_names [i].disp);
		SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_SETCURSEL, 0, 0);
		activate_editfilter_line (hwnd, cur_filter.nlines > 0 ? 0 : -1);
		SetFocus (GetDlgItem (hwnd, EDITFILTER_TEXT));
		return 1;
	case WM_COMMAND:
		if (CONTROL_ID >= FILTER_MENU_BASE && CONTROL_ID < FILTER_MENU_BASE+FILTER_MENU_NUM)	{
			insert_filter_item (hwnd, CONTROL_ID - FILTER_MENU_BASE);
			return 1;
		}
		switch (CONTROL_ID)	{
		case IDOK:
			if (!save_filter ())	{
				if (ErrCode)
					MsgBox ("Edit filter", MB_OK, "Can't save filter to file %s:\n%s",
						filedir [cur_filter_file], ErrText ());
			} else
				EndDialog (hwnd, 1);
			return 1;
		case EDITFILTER_LIST:
			if (NOTIFY_CODE == LBN_SELCHANGE)	{
				i = (int) SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_GETCURSEL, 0, 0);
				activate_editfilter_line (hwnd, i);
			} else if (NOTIFY_CODE == LBN_DBLCLK)	{
				if (!filter_modify (hwnd))
					MessageBeep (MB_ICONHAND);
			}
			return 1;
		case EDITFILTER_CLASS:
			if (NOTIFY_CODE == CBN_SELCHANGE)	{
				i = (int) SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_GETCURSEL, 0, 0);
				if (i < 0) return 1;
				j = (int) SendDlgItemMessage (hwnd, EDITFILTER_CLASS, CB_GETCURSEL, 0, 0);
				if (i < 0) return 1;
				cur_filter.lines [i].errclass = errclass_names [j].errclass;
			}
			return 1;
		case EDITFILTER_TEXT:
			if (NOTIFY_CODE != EN_CHANGE)
				return 1;
			EnableWindow (GetDlgItem (hwnd, EDITFILTER_MODIFY), 1);
			return 1;
		case EDITFILTER_MODIFY:
			if (!filter_modify (hwnd))
				MessageBeep (MB_ICONHAND);
			return 1;
		case EDITFILTER_REMOVE:
			filter_remove (hwnd);
			return 1;
		case EDITFILTER_ADD:
			if (!filter_add (hwnd))
				MessageBeep (MB_ICONHAND);
			return 1;
		case EDITFILTER_UP:
			i = (int) SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_GETCURSEL, 0, 0);
			if (!filter_up (hwnd, i))
				MessageBeep (MB_ICONHAND);
			return 1;
		case EDITFILTER_DOWN:
			i = (int) SendDlgItemMessage (hwnd, EDITFILTER_LIST, LB_GETCURSEL, 0, 0);
			if (!filter_down (hwnd, i))
				MessageBeep (MB_ICONHAND);
			return 1;
		case EDITFILTER_MENU:
			filter_menu (hwnd);
			return 1;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (EDITFILTER_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (EDITFILTER_DIALOG, lparam);
	}
	return 0;
}
 
BOOL	modify_filter (char * fname, char * name, BOOL newfile, int base)
{
	int file, r;
	TBUF * buf;
	char fullname [STRLEN];

	make_full_filter_name (fname, fullname);
	file = new_filedir (fullname);
	buf = NULL;
	if (newfile)	{
		if (base)	{
			buf = Find_or_read_file (base);
			if (!buf)	{
				r = MsgBox ("Edit Filter", MB_YESNO | MB_ICONQUESTION,
					"File %s is not found.\n"
					"Would you like to create a blank filter?",
					filedir [base]);
				if (r != IDYES) return FALSE;
			}
		}
	} else	{
		buf = Find_or_read_file (file);
		if (!buf)	{
			r = MsgBox ("Edit Filter", MB_YESNO | MB_ICONQUESTION,
				"File %s is not found.\n"
				"Would you like to create a new file?",
				filedir [file]);
			if (r != IDYES) return FALSE;
		}
	}
	if (!filter_from_buf (&cur_filter, buf))	{
		if (buf) tbuf_release (buf);
		return FALSE;
	}
	if (buf) tbuf_release (buf);
	cur_filter_name = name;
	cur_filter_file = file;
	return Dialog (EDITFILTER_DIALOG, EditFilterDialogProc);
}

BOOL	Edit_filter (int i)
{
	if (i < 0) return FALSE;
	return modify_filter (filters [i][1], filters [i][0], FALSE, 0); 
}

char	newfilter_name [300];
int	newfilter_file;
int	newfilter_base;

BOOL CALLBACK NewFilterDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char fname [_MAX_PATH], full_fname [_MAX_PATH];
	static BOOL filename_edited;

	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		SendDlgItemMessage (hwnd, NEWFILTER_NAME, EM_LIMITTEXT, sizeof (newfilter_name)-1, 0);
		SendDlgItemMessage (hwnd, NEWFILTER_FILE, EM_LIMITTEXT,  sizeof (fname)-1, 0);
		EnableWindow (GetDlgItem (hwnd, IDOK), 0);
		Filter_fill_list (GetDlgItem (hwnd, NEWFILTER_LIST), FALSE, TRUE);
		SendDlgItemMessage (hwnd, NEWFILTER_LIST, LB_SETCURSEL, 0, 0);
		filename_edited = FALSE;
		EnableWindow (GetDlgItem (hwnd, NEWFILTER_FILE), 0);
		EnableWindow (GetDlgItem (hwnd, NEWFILTER_BROWSE), 0);
		SetFocus (GetDlgItem (hwnd, NEWFILTER_NAME));
		return 0;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case NEWFILTER_FILE:
			if (NOTIFY_CODE == EN_CHANGE)
				filename_edited = TRUE;
			return 1;
		case NEWFILTER_NAME:
			if (NOTIFY_CODE != EN_CHANGE) return 1;
			GetDlgItemText (hwnd, NEWFILTER_NAME, newfilter_name, sizeof (newfilter_name)-1);
			EnableWindow (GetDlgItem (hwnd, IDOK), newfilter_name [0]!=0);
			EnableWindow (GetDlgItem (hwnd, NEWFILTER_FILE), newfilter_name [0]!=0);
			EnableWindow (GetDlgItem (hwnd, NEWFILTER_BROWSE), newfilter_name [0]!=0);
			if (filename_edited) return 1;
			if (!newfilter_name[0])
				fname [0] = 0;
			else	{
				strcpy (fname, newfilter_name);
				strcat (fname, FILTER_SUFFIX);
			}
			SetDlgItemText (hwnd, NEWFILTER_FILE, fname);
			filename_edited = FALSE;
			return 1;
		case IDOK:
			GetDlgItemText (hwnd, NEWFILTER_NAME, newfilter_name, sizeof (newfilter_name)-1);
			GetDlgItemText (hwnd, NEWFILTER_FILE,  fname, sizeof (fname)-1);
			newfilter_name [sizeof (newfilter_name)-1] = fname [sizeof (fname)-1] = 0;
			if (!newfilter_name[0])	{
				MsgBox ("Error", MB_ICONEXCLAMATION|MB_OK,
					"You should specify some filter name");
				SetFocus (GetDlgItem (hwnd, NEWFILTER_NAME));
				break;
			}
			if (!fname[0])	{
				strcpy (fname, newfilter_name);
				strcat (fname, FILTER_SUFFIX);
				SetDlgItemText (hwnd, NEWFILTER_FILE, fname);
			}
			make_full_filter_name (fname, full_fname);
			if (!Test_overwrite (full_fname, TRUE))	{
				SetFocus (GetDlgItem (hwnd, NEWFILTER_NAME));
				break;
			}
			newfilter_base = SendDlgItemMessage (hwnd, NEWFILTER_LIST, LB_GETCURSEL, 0, 0);
			newfilter_file = new_filedir (full_fname);
			EndDialog (hwnd, 1);
			return 1;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case NEWFILTER_BROWSE:
			GetDlgItemText (hwnd, NEWFILTER_FILE, fname, _MAX_PATH);
			Browse_file_name (TRUE, "Select name for filter file",
					  fname, write_filter_filter, fname, &filter_filter_index, NULL);
			SetDlgItemText (hwnd, NEWFILTER_FILE,  fname);
			filename_edited = TRUE;
			break;
		case IDHELP:
			return Dialog_help (NEWFILTER_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (NEWFILTER_DIALOG, lparam);
	}
	return 0;
}

BOOL	make_new_filter (void)
{
	int f;
	Read_filters ();
	if (!Dialog (NEWFILTER_DIALOG, NewFilterDialogProc))
		return FALSE;
	f = 0;
	if (newfilter_base && filters [newfilter_base-1][1])
		f = new_filedir (filters [newfilter_base-1][1]);
	return modify_filter (filedir [newfilter_file], newfilter_name, TRUE, f);
}

int	New_filter (void)
{
	int i;

	if (!make_new_filter ())
		return -1;
	if (Add_section_elem (filters, FILTERS_NUM, newfilter_name, filedir [newfilter_file],
			      FILTERS_NAME, IniFile))
	{
		for (i = 0; filters [i][0]; i++);
		return i-1;
	}
	return -1;
}

void	remove_filter (HWND hwnd, int i)
{
	int r, n;

	r = MsgBox ("Remove filter", MB_YESNO | MB_ICONQUESTION,
		    "Do you really want to remove message filter %s?", filters [i][0]);
	if (r != IDYES) return;
	r = MsgBox ("Remove filter", MB_YESNO | MB_ICONQUESTION,
		    "Do you also want to delete file %s?", filters [i][1]);
	if (r == IDYES)
		DeleteFile (filters [i][1]);
	Delete_section_item (filters, FILTERS_NUM, i, FILTERS_NAME, IniFile);
	SendDlgItemMessage (hwnd, FILTERS_LIST, LB_DELETESTRING, i, 0);
	n = (int) SendDlgItemMessage (hwnd, FILTERS_LIST, LB_GETCOUNT, 0, 0);
	if (i >= n) i = n-1;
	SendDlgItemMessage (hwnd, FILTERS_LIST, LB_SETCURSEL, i, 0);
	EnableWindow (GetDlgItem (hwnd, FILTERS_REMOVE), i>=0);
	EnableWindow (GetDlgItem (hwnd, FILTERS_EDIT), i>=0);
}

BOOL CALLBACK FiltersDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;

	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		i = Filter_fill_list (GetDlgItem (hwnd, FILTERS_LIST), FALSE, FALSE);
		SendDlgItemMessage (hwnd, FILTERS_LIST, LB_SETCURSEL, i>0 ? 0 : (WPARAM) -1, 0);
		EnableWindow (GetDlgItem (hwnd, FILTERS_REMOVE), i!=0);
		EnableWindow (GetDlgItem (hwnd, FILTERS_EDIT), i!=0);
		SetFocus (GetDlgItem (hwnd, FILTERS_LIST));
		return 0;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case FILTERS_NEW:
			i = New_filter ();
			if (i >= 0)
				SendDlgItemMessage (hwnd, FILTERS_LIST, LB_ADDSTRING, 0,
					     (LPARAM)(LPSTR) filters [i][0]);
			return 1;
		case FILTERS_LIST:
			if (NOTIFY_CODE != LBN_DBLCLK)
				return 1;
		case FILTERS_EDIT:
			i = (int) SendDlgItemMessage (hwnd, FILTERS_LIST, LB_GETCURSEL, 0, 0);
			if (i < 0) return 1;
			if (!filters [i][1]) return 1;
			modify_filter (filters [i][1], filters [i][0], FALSE, 0);
			return 1;
		case FILTERS_REMOVE:
			i = (int) SendDlgItemMessage (hwnd, FILTERS_LIST, LB_GETCURSEL, 0, 0);
			if (i < 0) return 1;
			remove_filter (hwnd, i);
			return 1;
		case IDOK:
			EndDialog (hwnd, 1);
			return 1;
		case IDHELP:
			return Dialog_help (FILTERS_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (FILTERS_DIALOG, lparam);
	}
	return 0;
}

void	Configure_filters (void)
{
	Read_filters ();
	if (!Dialog (FILTERS_DIALOG, FiltersDialogProc))
		return;
	Clear_filters ();
}