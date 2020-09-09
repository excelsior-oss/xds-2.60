#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "mdi.h"
#include "frame.h"
#include "message.h"
#include "fcache.h"
#include "edit.h"
#include "winmake.h"
#include "config.h"
#include "tools.h"
#include "open.h"
#include "res.h"

#define MessagesMDIClass "messages"

HWND	MessageWnd = NULL;
HWND	MessageListBox = NULL;
HWND	wnd_before_message = NULL;

ErrLine * err_list = NULL;
ErrLine * cur_err = NULL;
int	msg_extent = 0;

int	msg_count;
int	err_count;
int	warn_count;
int	severe_count;

BOOL	sort_errors = TRUE;
int	errors_help_file = 0;

HBRUSH	message_brush = 0;

MDIPLACEMENT message_placement;
BOOL	was_message_box = FALSE;

HICON	MessageSmIcon;

ErrLine * alloc_error (char * body, long errnum, long x, long y, int file, int class)
{
	ErrLine * p = Malloc (sizeof (ErrLine));
	if (!p) return 0;

	scpy (p->body, body);
	p -> file = file;
	p -> errnum = errnum;
	p -> x = x;
	p -> y = y;
	p -> bookmark = 0;
	p -> present = TRUE;
	p -> msg_class = class;
	p -> next = p -> prev = NULL;
	return p;
}

void	msg_enable_buttons (void)
{
	Enable_item (IDM_PREVMESS, cur_err && cur_err->prev);
	Enable_item (IDM_NEXTMESS, cur_err && cur_err->next);
}

void	Clear_messages (void)
{
	ErrLine * p;
	while (err_list)	{
		p = err_list -> next;
		if (err_list->present)
			Edit_remove_error (err_list->file, err_list->bookmark);
		free (err_list);
		err_list = p;
	}
	if (MessageListBox)
		SendMessage (MessageListBox, LB_RESETCONTENT, 0, 0);
	cur_err = NULL;
	msg_extent = 0;
	err_count = 0;
	warn_count = 0;
	msg_count = 0;
	severe_count = 0;
	msg_enable_buttons ();
}

void	draw_msg_line (ErrLine * l, char * s)
{
	s [0] = 0;
	switch (l -> msg_class)	{
	case MSG_SEVERE: strcat (s, "Severe Error"); break;
	case MSG_ERROR:	 strcat (s, "Error"); break;
	case MSG_WARNING: strcat (s, "Warning"); break;
	}
	if (l -> msg_class != MSG_TEXT)	{
		if (l->file)	{
			if (strlen(s)) strcat (s, " in ");
			strcat (s, name_only (filedir [l->file]));
			if (l -> y != -1)	{
				wsprintf (s+strlen(s), " (%ld", l->y+1);
				if (l -> x != -1)
					wsprintf (s+strlen(s), ":%ld", l->x+1);
				strcat (s, ")");
			}
		}
		if (strlen (s))	strcat (s, ": ");
	}
	strcat (s, l->body);
}

void	disp_msg_line (ErrLine * l)
{
	char s [1024];
	int x;

	draw_msg_line (l, s);
	l -> num = SendMessage (MessageListBox, LB_ADDSTRING,
				0, (LPARAM) (LPCSTR) s);
	x = text_width (MsgFont, s);
	if (x > msg_extent) msg_extent = x;
}

void	Add_error (char * body, long errnum, long x, long y, int file, int class)
{
	ErrLine * p;
	ErrLine * q, *prev;
	p = alloc_error (body, errnum, x, y, file, class);
	p -> helpfile = errors_help_file;
	prev = NULL;
	if (sort_errors)	{
		for (q = err_list; q; q = q -> next)	{
			if (file && q -> file == file) break;
			prev = q;
		}
		if (q)  {
			while (q && q -> file == file &&
			       (q->y < p->y || q->y == p->y && q->x <= p->x))
			{
				prev = q;
				q = q -> next;
			}
			if (prev && prev -> file == file &&
				prev -> msg_class == class &&
				prev -> y == y && prev -> x == x &&
				! strcmp (body, prev -> body))
			{
				free (p);
				return;
			}
		}
	} else
		for (q = err_list; q; q = q -> next)
			prev = q;

	if (prev)   {
		p -> next = prev -> next;
		prev -> next = p;
		p -> prev = prev;
	} else  {
		p -> next = err_list;
		err_list = p;
	}
	if (p -> next)
		p -> next -> prev = p;
	if (class == MSG_SEVERE)
		severe_count ++;
	if (class == MSG_ERROR)
		err_count ++;
	else if (class == MSG_WARNING)
		warn_count ++;
	msg_count ++;
	if (MessageWnd) {
		disp_msg_line (p);
		SendMessage (MessageListBox, LB_SETHORIZONTALEXTENT, msg_extent, 0);
	}
	p->bookmark = Edit_process_error (p->file, p->x, p->y, &p->was_dirty);
}

void	Go_to_error (void)
{
	HWND	ed;

	if (!cur_err->present || cur_err -> msg_class == MSG_TEXT) return;
	Edit_drop_highlight ();
	msg_enable_buttons ();
	if (MessageListBox) SendMessage (MessageListBox, LB_SETCURSEL, cur_err->num, 0);
	Set_status_bar (MessageWnd, cur_err -> body);
	if (!cur_err -> file) return;
	ed = Edit_find_loaded_file (cur_err -> file);
	++ EditStatusBarLock;
	if (!ed)	{
		ed = Edit_read_text (cur_err -> file, NULL, OPEN_EXISTING_BUFFER, TRUE);
		if (!ed)	{
			-- EditStatusBarLock;
			return;
		}
	}
	if (cur_err -> bookmark)
		Edit_goto_bookmark (ed, cur_err -> bookmark);
	Edit_show (ed);
	-- EditStatusBarLock;
}

void	disp_error (void)
{
	HWND edwnd;

	msg_enable_buttons ();

	Edit_drop_highlight ();
	Set_status_bar (MessageWnd, cur_err -> body);
	if (!cur_err -> file || !cur_err->present || !cur_err->bookmark) return;
	edwnd = Edit_find_loaded_file (cur_err -> file);
	if (!edwnd) return;
	++ EditStatusBarLock;
	Edit_highlight_error (edwnd, cur_err -> bookmark);
	-- EditStatusBarLock;
}

void	Next_message (void)
{
	ErrLine * e = cur_err;

	while (e && e -> next)	{
		e = e -> next;
		if (e -> msg_class != MSG_TEXT && e -> file)	{
			cur_err = e;
			Go_to_error ();
			return;
		}
	}
}

void	Prev_message (void)
{
	ErrLine * e = cur_err;

	while (e && e -> prev)	{
		e = e -> prev;
		if (e -> msg_class != MSG_TEXT && e -> file)	{
			cur_err = e;
			Go_to_error ();
			return;
		}
	}
}

void	UpdateMessageWnd (void)
{
	ErrLine * p;

	msg_extent = 0;
	SendMessage (MessageListBox, LB_RESETCONTENT, 0, 0);
	for (p = err_list; p; p = p -> next)
		disp_msg_line (p);
	SendMessage (MessageListBox, LB_SETHORIZONTALEXTENT, msg_extent, 0);
}

void	ListBoxOp (UINT op)
{
	long sel;
	ErrLine * p;

	sel = SendMessage (MessageListBox, LB_GETCARETINDEX, 0, 0);
	for (p = err_list; p && p -> num != sel; p = p -> next);
	if (!p) return;
	cur_err = p;
	switch (op)	{
	case LBN_SELCHANGE:
		disp_error ();
		break;
	case LBN_DBLCLK:
		Go_to_error ();
		break;
	}
}

void	copy_messages (void)
{
	char s [1024];
	int len;
	HANDLE h;
	char * t;
	ErrLine * p;

	len = 1;
	if (! OpenClipboard (0))	{
		MsgBox ("Copy messages", MB_OK | MB_ICONEXCLAMATION,
			 "Can't open clipboard");
		return;
	}

	for (p = err_list; p; p = p -> next)	{
		draw_msg_line (p, s);
		len += strlen (s)+2;
	}
	h = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, len);
	if (!h)	{
		CloseClipboard ();
		MsgBox ("Copy messages", MB_OK | MB_ICONEXCLAMATION,
			 "No enough memory for operation");
		return;
	}

	len = 0;
	t = GlobalLock (h);
	for (p = err_list; p; p = p -> next)	{
		draw_msg_line (p, s);
		strcpy (t+len, s);
		len += strlen (s);
		strcpy (t+len, "\r\n");
		len += 2;
	}
	GlobalUnlock (h);
	EmptyClipboard ();
	SetClipboardData (CF_TEXT, h);
	CloseClipboard ();
}

int	messages_filter_index = 1;

BOOL	wr_messages (int file)
{
	char s [1024];
	ErrLine * p;

	if (!Put_file_start ())	return FALSE;
	for (p = err_list; p; p = p -> next)	{
		draw_msg_line (p, s);
		if (!Put_file (s, strlen(s))) return FALSE;
	}
	return Put_file_fin (file, TRUE);
}

void	save_messages (void)
{
	char name [STRLEN];
	int file;

	if (!Browse_file_name (TRUE, "", "", 0, name, &messages_filter_index, EditCurDir))
		return;

	file = new_filedir (name);
	if (!wr_messages (file))
		MsgBox ("Write messages", MB_OK,
			"There was an error writing file %s:\n%s", filedir [file], ErrText ());
}

void	message_help (void)
{
	if (! cur_err || cur_err -> errnum == 0 || cur_err -> helpfile == 0) return;
	WinHelp (FrameWindow, filedir [cur_err -> helpfile], HELP_CONTEXT, cur_err -> errnum);
}

void	Messages_enable_items (void)
{
	Enable_item (IDM_PASTE, FALSE);
	Enable_item (IDM_COPY, TRUE);
	Enable_item (IDM_CUT, FALSE);
	Enable_item (IDM_UNDO, FALSE);
	Enable_item (IDM_SAVE, FALSE);
	Enable_item (IDM_SAVEAS, TRUE);
	Enable_item (IDM_CLOSE, FALSE);
	Enable_item (IDM_PRINT, FALSE);
	Enable_item (IDM_FIND, FALSE);
	Enable_item (IDM_FINDNEXT, FALSE);
	Enable_item (IDM_REPLACE, FALSE);
	Enable_item (IDM_REPLACENEXT, FALSE);
	Enable_item (IDM_FIND, FALSE);
	Enable_item (IDM_FINDNEXT, FALSE);
	Enable_item (IDM_GOTOLINE, FALSE);
	Tools_enable ();
}

void	update_messages_brush (void)
{
	COLORREF bk = AbsColor (MsgBackColor, AbsSysColor (WndBackColor, COLOR_WINDOW));
	HBRUSH b = CreateSolidBrush (bk);
	if (!b) return;
	if (message_brush) DeleteObject (message_brush);
	message_brush = b;
}

void	Draw_window_label (HWND hwnd);

LRESULT CALLBACK MDIMessageProc (HWND hwnd, UINT msg,
				 WPARAM wparam, LPARAM lparam)
{
	HDC	dc;
	RECT	rect;
	char * s;

	switch (msg) {
	case WM_CLOSE:
		SendMessage (hwnd, WM_MYMDI_GETPLACEMENT, 0, (LPARAM) &message_placement);
		was_message_box = TRUE;
		Enable_item (IDM_HIDEMESS, FALSE);
		MessageWnd = 0;
		break;
	case MSG_GETCLASS:
		s = (char*) lparam;
		strcpy (s, MessagesMDIClass);
		return 0;
	case WM_CREATE:
		GetClientRect (hwnd, &rect);
		MessageListBox = CreateWindow (
				"LISTBOX",			/* class */
				NULL,				/* no caption */
				WS_CHILD | WS_VISIBLE |
				WS_HSCROLL | WS_VSCROLL |
				LBS_NOTIFY | LBS_HASSTRINGS |
				LBS_NOINTEGRALHEIGHT|
				LBS_WANTKEYBOARDINPUT,
				0,
				0,
				rect.right-rect.left,
				rect.bottom-rect.top,
				hwnd,				/* parent window */
				NULL,				/* menu */
				MyInstance,
				NULL
			);
		if (! MessageListBox)
			return -1;
		SendMessage (MessageListBox, WM_SETFONT, (WPARAM) MsgFont, 0);
		SendMessage (MessageListBox, LB_SETHORIZONTALEXTENT, 600, 0);
		MessageWnd = hwnd;
		UpdateMessageWnd ();
		break;
	case WM_SETFOCUS:
		if (SendMessage (MessageListBox, LB_GETCURSEL, 0, 0) == LB_ERR &&
		    SendMessage (MessageListBox, LB_GETCOUNT, 0, 0))
			SendMessage (MessageListBox, LB_SETCURSEL, 0, 0);
		SetFocus (MessageListBox);
		break;
	case WM_VKEYTOITEM:
		if (LOWORD (wparam) == VK_RETURN)
			ListBoxOp (LBN_DBLCLK);
		else if (LOWORD (wparam) == VK_ESCAPE)
			SendMessage (FrameWindow, WM_MDIACTIVATE,
				     (WPARAM) wnd_before_message, 0);
		break;
	case WM_MDIACTIVATE:
		if (WND_TO_ACTIVATE == hwnd)	{
			Messages_enable_items ();
			if (WND_TO_DEACTIVATE)
				wnd_before_message = WND_TO_DEACTIVATE;
		} else if (! WND_TO_ACTIVATE)
			Disable_all_items ();
		break;
	case WM_CTLCOLORLISTBOX:
		dc = (HDC) wparam;
		SetBkColor   (dc, AbsColor (MsgBackColor, AbsSysColor (WndBackColor, COLOR_WINDOW)));
		SetTextColor (dc, AbsColor (MsgForeColor, AbsSysColor (WndForeColor, COLOR_WINDOWTEXT)));
		return (LRESULT) message_brush;
	case WM_HELP:
		message_help ();
		return 0;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDM_COPY:
			copy_messages ();
			break;
		case IDM_SAVEAS:
                	save_messages ();
			break;
		case IDM_HELPKEYWORD:
			message_help ();
			break;
		default:
			if (CONTROL_HWND == MessageListBox)	{
				switch (NOTIFY_CODE)	{
				case LBN_SELCHANGE:
				case LBN_DBLCLK:
					ListBoxOp (NOTIFY_CODE);
					return 0;
				}
			}
		}
		break;
	}
	return MyDefMDIChildProc (hwnd, msg, wparam, lparam);
}

void	Refresh_message_window (void)
{
	ErrLine * p;
	char s [1024];
	int x;

	update_messages_brush ();
	SendMessage (MessageListBox, WM_SETFONT, (WPARAM) MsgFont, 0);
	msg_extent = 0;
	for (p = err_list; p; p = p -> next)	{
		draw_msg_line (p, s);
		x = text_width (MsgFont, s);
		if (x > msg_extent) msg_extent = x;
	}
	SendMessage (MessageListBox, LB_SETHORIZONTALEXTENT, msg_extent, 0);
	InvalidateRect (MessageListBox, 0, 1);
}

HWND	create_messages_window (char * title, MDIPLACEMENT * placement, char * extra)
{
	MDICREATESTRUCT	mdi;
	RECT r;

	update_messages_brush ();
	
	mdi.szClass = MessageClass;
	mdi.szTitle = "Messages";
	mdi.hOwner = MyInstance;
	mdi.style = WS_OVERLAPPEDWINDOW;
	mdi.lParam = 0;
	GetClientRect ((HWND) SendMessage (FrameWindow, WM_MYMDI_GETCLIENT, 0, 0), &r);
	mdi.x = 0;
	mdi.y = r.bottom * 3 / 4 - 1;
	mdi.cx = r.right;
	mdi.cy = r.bottom/4;
	if (placement)	{
		message_placement = *placement;
		was_message_box = TRUE;
	}
	if (DockMessages && ! was_message_box)	{
		memset (&message_placement, 0, sizeof (message_placement));
		message_placement.type = DOCKED;
		message_placement.dir  = DOCK_DOWN;
		message_placement.docknum  = 0;
		was_message_box = TRUE;
	}

	SendMessage (FrameWindow, WM_MDICREATE,
		     was_message_box ? (WPARAM) &message_placement : 0,
		     (LPARAM)(LPSTR)&mdi);
	if (! MessageWnd) return 0;
	Enable_item (IDM_HIDEMESS, TRUE);

	return MessageWnd;
}

void	Show_messages (void)
{
	if (MessageWnd)
		SendMessage (FrameWindow, WM_MDIACTIVATE,
			     (WPARAM) MessageWnd, 0);
	else
		create_messages_window (0, 0, 0);
}

void	Hide_messages (void)
{
	if (MessageWnd)
		SendMessage (MessageWnd, WM_CLOSE, 0, 0);
}

void	First_message (void)
{
	ErrLine * q;
	cur_err = err_list;
	if (! err_list)	{
		msg_enable_buttons ();
		return;
	}

	if (err_count + severe_count)
		for (q = err_list; q; q = q -> next)
			if (q -> msg_class == MSG_ERROR ||
			    q -> msg_class == MSG_SEVERE)	{
				cur_err = q;
				break;
			}
	if (MessageListBox) SendMessage (MessageListBox, LB_SETCURSEL, cur_err->num, 0);
	disp_error ();
}

void	Msg_proj_enable (BOOL proj)
{
	Enable_item (IDM_SHOWMESS, proj);
	Enable_item (IDM_HIDEMESS, proj && MessageWnd);
	if (!proj)	{
		Enable_item (IDM_NEXTMESS, FALSE);
		Enable_item (IDM_PREVMESS, FALSE);
	} else
		msg_enable_buttons ();
}

BOOL	RegisterMessage (void)
{
	WNDCLASSEX wc;

	wc.cbSize 	 = sizeof (WNDCLASSEX);
	wc.style	 = 0;
	wc.lpfnWndProc   = (LPVOID) MDIMessageProc;
	wc.hInstance     = MyInstance;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	MessageSmIcon = wc.hIcon = LoadIcon (MyInstance, MAKEINTRESOURCE (MSG_ICON));
	wc.hCursor 	 = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = 0;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = MessageClass;
	wc.lpszMenuName  = NULL;
	wc.hIconSm 	 = 0;
	if (!RegisterClassEx (&wc)) return FALSE;
	Register_child_class (MessagesMDIClass, create_messages_window);
	return TRUE;
}
