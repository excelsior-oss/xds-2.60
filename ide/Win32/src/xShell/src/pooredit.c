#include <windows.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "tbuf.h"
#include "pooredit.h"
#include "lang.h"
#include "syntax.h"
#include "res.h"
#include "macro.h"

int scroll_width = 0;
int scroll_height = 0;

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x20A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

/*---------------------------------------------------------------------------*/
void	measure_font (HFONT f, int * dx, int * dy)
{
	TEXTMETRIC tm;
	HDC dc;

	dc = GetDC (0);
	f = SelectObject (dc, f);
	GetTextMetrics (dc, &tm);
	*dx = tm.tmAveCharWidth;
	*dy = tm.tmHeight;
	SelectObject (dc, f);
	ReleaseDC (0, dc);
}

/*---------------------------------------------------------------------------*/

#define EDIT_RIGHT		1
#define EDIT_LEFT		2
#define EDIT_UP			3
#define EDIT_DOWN		4
#define EDIT_PGUP		5
#define EDIT_PGDOWN		6
#define EDIT_LINESTART		7
#define EDIT_LINEEND		8
#define EDIT_FILESTART		9
#define EDIT_FILEEND		10
#define EDIT_PAGESTART		11
#define EDIT_PAGEEND		12
#define EDIT_RIGHTWORD		13
#define EDIT_LEFTWORD		14

#define EDIT_BLKRIGHT		21
#define EDIT_BLKLEFT		22
#define EDIT_BLKUP		23
#define EDIT_BLKDOWN		24
#define EDIT_BLKPGUP		25
#define EDIT_BLKPGDOWN		26
#define EDIT_BLKLINESTART	27
#define EDIT_BLKLINEEND		28
#define EDIT_BLKFILESTART	29
#define EDIT_BLKFILEEND		30
#define EDIT_BLKPAGESTART	31
#define EDIT_BLKPAGEEND		32
#define EDIT_BLKRIGHTWORD	33
#define EDIT_BLKLEFTWORD	34

#define EDIT_DEL		43
#define EDIT_BS			44
#define EDIT_TAB		45
#define EDIT_BACKTAB		46
#define EDIT_DELWORD		47
#define	EDIT_DELWORDLEFT	48

#define EDIT_FLIPINSERT		50
#define EDIT_NEXTLINE		51
#define EDIT_INSERTLINE		52
#define EDIT_CR			53
#define EDIT_DELETELINE		54
#define EDIT_SPLITLINE		55
#define EDIT_JOIN_LINES		56

// These commands work with a system clipboard, not with a local one
#define EDIT_CB_COPY		 60
#define EDIT_CB_CUT		   61
#define EDIT_CB_PASTE		 62
#define EDIT_CB_EXCHANGE 63
#define EDIT_CB_ADD		   64

#define EDIT_UNDO		65

#define EDIT_INDENT_LEFT	70
#define EDIT_INDENT_LEFT1	71
#define EDIT_INDENT_RIGHT	72
#define EDIT_INDENT_RIGHT1	73

#define EDIT_SETBOOKMARK1	81
#define EDIT_SETBOOKMARK2	82
#define EDIT_SETBOOKMARK3	83
#define EDIT_SETBOOKMARK4	84
#define EDIT_SETBOOKMARK5	85
#define EDIT_SETBOOKMARK6	86
#define EDIT_SETBOOKMARK7	87
#define EDIT_SETBOOKMARK8	88
#define EDIT_SETBOOKMARK9	89

#define EDIT_GOTOBOOKMARK1	91
#define EDIT_GOTOBOOKMARK2	92
#define EDIT_GOTOBOOKMARK3	93
#define EDIT_GOTOBOOKMARK4	94
#define EDIT_GOTOBOOKMARK5	95
#define EDIT_GOTOBOOKMARK6	96
#define EDIT_GOTOBOOKMARK7	97
#define EDIT_GOTOBOOKMARK8	98
#define EDIT_GOTOBOOKMARK9	99

#define	EDIT_UPPERCASE		100
#define	EDIT_FLIPCASE		101

// These commands work with a local clipboard, not with a system one
#define EDIT_PRIVATE_COPY		  102
#define EDIT_PRIVATE_CUT		  103
#define EDIT_PRIVATE_PASTE		104
#define EDIT_PRIVATE_EXCHANGE	105
#define EDIT_PRIVATE_ADD		  106

typedef struct	{
		int	action;
		int	key;
		int	mod;
	}
		StdEdAction;

StdEdAction std_ed_keys [] = {
	{ EDIT_RIGHT,		VK_RIGHT,	FVIRTKEY},
	{ EDIT_LEFT,		VK_LEFT,	FVIRTKEY},
	{ EDIT_UP,		VK_UP,		FVIRTKEY},
	{ EDIT_DOWN,		VK_DOWN,	FVIRTKEY},
	{ EDIT_PGUP,		VK_PRIOR,	FVIRTKEY},
	{ EDIT_PGDOWN,		VK_NEXT,	FVIRTKEY},
	{ EDIT_LINESTART,	VK_HOME,	FVIRTKEY},
	{ EDIT_LINEEND,		VK_END,		FVIRTKEY},
	{ EDIT_FILESTART,	VK_HOME,	FVIRTKEY | FCONTROL},
	{ EDIT_FILEEND,		VK_END,		FVIRTKEY | FCONTROL},
	{ EDIT_PAGESTART,	VK_PRIOR,	FVIRTKEY | FCONTROL},
	{ EDIT_PAGEEND,		VK_NEXT,	FVIRTKEY | FCONTROL},
	{ EDIT_RIGHTWORD,	VK_RIGHT,	FVIRTKEY | FCONTROL},
	{ EDIT_LEFTWORD,	VK_LEFT,	FVIRTKEY | FCONTROL},

	{ EDIT_BLKRIGHT,	VK_RIGHT,	FVIRTKEY | FSHIFT},
	{ EDIT_BLKLEFT,		VK_LEFT,	FVIRTKEY | FSHIFT},
	{ EDIT_BLKUP,		VK_UP,		FVIRTKEY | FSHIFT},
	{ EDIT_BLKDOWN,		VK_DOWN,	FVIRTKEY | FSHIFT},
	{ EDIT_BLKPGUP,		VK_PRIOR,	FVIRTKEY | FSHIFT},
	{ EDIT_BLKPGDOWN,	VK_NEXT,	FVIRTKEY | FSHIFT},
	{ EDIT_BLKLINESTART,	VK_HOME,	FVIRTKEY | FSHIFT},
	{ EDIT_BLKLINEEND,	VK_END,		FVIRTKEY | FSHIFT},
	{ EDIT_BLKFILESTART,	VK_HOME,	FVIRTKEY | FCONTROL | FSHIFT},
	{ EDIT_BLKFILEEND,	VK_END,		FVIRTKEY | FCONTROL | FSHIFT},
	{ EDIT_BLKPAGESTART,	VK_PRIOR,	FVIRTKEY | FCONTROL | FSHIFT},
	{ EDIT_BLKPAGEEND,	VK_NEXT,	FVIRTKEY | FCONTROL | FSHIFT},
	{ EDIT_BLKRIGHTWORD,	VK_RIGHT,	FVIRTKEY | FCONTROL | FSHIFT},
	{ EDIT_BLKLEFTWORD,	VK_LEFT,	FVIRTKEY | FCONTROL | FSHIFT},

	{ EDIT_DEL,		VK_DELETE,	FVIRTKEY},
	{ EDIT_BS,		VK_BACK,	FVIRTKEY},
	{ EDIT_TAB,		VK_TAB,		FVIRTKEY},
	{ EDIT_BACKTAB,		VK_TAB,		FVIRTKEY | FSHIFT},
	{ EDIT_DELWORD,		VK_DELETE,	FVIRTKEY | FCONTROL},
	{ EDIT_DELWORDLEFT,	VK_BACK,	FVIRTKEY | FCONTROL},
	
	{ EDIT_INDENT_LEFT,	VK_LEFT,	FVIRTKEY | FALT | FSHIFT},
	{ EDIT_INDENT_LEFT1,	VK_LEFT,	FVIRTKEY | FALT},
	{ EDIT_INDENT_RIGHT,	VK_RIGHT,	FVIRTKEY | FALT | FSHIFT},
	{ EDIT_INDENT_RIGHT1,	VK_RIGHT,	FVIRTKEY | FALT},

	{ EDIT_FLIPINSERT,	VK_INSERT,	FVIRTKEY},
	{ EDIT_NEXTLINE,	'N'-0x40,	0},
	{ EDIT_INSERTLINE,	'I'-0x40,	0},
	{ EDIT_CR,		VK_RETURN,	FVIRTKEY},
	{ EDIT_NEXTLINE,	VK_RETURN,	FVIRTKEY | FSHIFT},
	{ EDIT_DELETELINE,	'Y'-0x40,	0},
	{ EDIT_SPLITLINE,	'S'-0x40,	0},
	{ EDIT_JOIN_LINES,	'J'-0x40,	0},

	{ EDIT_CB_COPY,     VK_INSERT,	FVIRTKEY | FCONTROL},
	{ EDIT_CB_COPY,     'C'-0x40,	0},
	{ EDIT_CB_CUT,      VK_DELETE,	FVIRTKEY | FSHIFT},
	{ EDIT_CB_CUT,	    'X'-0x40,	0},
	{ EDIT_CB_PASTE,    VK_INSERT,	FVIRTKEY | FSHIFT},
	{ EDIT_CB_PASTE,    'V'-0x40,	0},
	{ EDIT_CB_EXCHANGE, VK_DELETE,	FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_CB_EXCHANGE, 'E'-0x40,	0},
	{ EDIT_CB_ADD,      VK_INSERT,	FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_CB_ADD,      'A'-0x40,	0},
	{ EDIT_UNDO,		VK_BACK,	FVIRTKEY | FALT},
	{ EDIT_UNDO,		'Z'-0x40,	0},

	{ EDIT_SETBOOKMARK1,	'1',		FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_SETBOOKMARK2,	'2',		FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_SETBOOKMARK3,	'3',		FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_SETBOOKMARK4,	'4',		FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_SETBOOKMARK5,	'5',		FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_SETBOOKMARK6,	'6',		FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_SETBOOKMARK7,	'7',		FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_SETBOOKMARK8,	'8',		FVIRTKEY | FSHIFT | FCONTROL},
	{ EDIT_SETBOOKMARK9,	'9',		FVIRTKEY | FSHIFT | FCONTROL},

	{ EDIT_GOTOBOOKMARK1,	'1',		FVIRTKEY | FCONTROL},
	{ EDIT_GOTOBOOKMARK2,	'2',		FVIRTKEY | FCONTROL},
	{ EDIT_GOTOBOOKMARK3,	'3',		FVIRTKEY | FCONTROL},
	{ EDIT_GOTOBOOKMARK4,	'4',		FVIRTKEY | FCONTROL},
	{ EDIT_GOTOBOOKMARK5,	'5',		FVIRTKEY | FCONTROL},
	{ EDIT_GOTOBOOKMARK6,	'6',		FVIRTKEY | FCONTROL},
	{ EDIT_GOTOBOOKMARK7,	'7',		FVIRTKEY | FCONTROL},
	{ EDIT_GOTOBOOKMARK8,	'8',		FVIRTKEY | FCONTROL},
	{ EDIT_GOTOBOOKMARK9,	'9',		FVIRTKEY | FCONTROL},

	{ EDIT_UPPERCASE,	'U'-0x40,	0},
	{ EDIT_FLIPCASE,	'U',		FVIRTKEY | FCONTROL | FSHIFT}
};

typedef struct	{
		int key;
		int mod;
		int action;
	}
		EdAction;

#define N_ED_KEYS 200

EdAction ed_keys [N_ED_KEYS];
int	 n_ed_keys = 0;

void	init_ed_keys (void)
{
	int i;

	n_ed_keys = sizeof (std_ed_keys) / sizeof (StdEdAction);
	for (i = 0; i < n_ed_keys; i++)	{
		ed_keys [i].key = std_ed_keys [i].key;
		ed_keys [i].action = std_ed_keys [i].action;
		ed_keys [i].mod = std_ed_keys [i].mod;
	}
}

BYTE	get_keyboard_modes()
{
	BYTE mod = 0;

	if (GetKeyState (VK_SHIFT)   < 0) mod |= FSHIFT;
	if (GetKeyState (VK_CONTROL) < 0) mod |= FCONTROL;
	if (GetKeyState (VK_MENU)    < 0) mod |= FALT;

	return mod;
}

int	lookup_ed_key (WPARAM key)
{
	int i;
	BYTE mod = get_keyboard_modes() | FVIRTKEY;

	for (i = 0; i < n_ed_keys; i++)
		if (ed_keys [i].key == (int) key && ed_keys [i].mod == mod)
			return ed_keys [i].action;
	return 0;
}

int	lookup_ed_char (WPARAM key)
{
	int i;

	for (i = 0; i < n_ed_keys; i++)
		if (ed_keys [i].key == (int) key && ed_keys [i].mod == 0)
			return ed_keys [i].action;
	return 0;
}

/*---------------------------------------------------------------------------*/

HCURSOR edit_move_cursors [9];
HICON	mouse_scroll_icon;
int move_hspot_x = 15;
int move_hspot_y = 15;

HCURSOR edit_get_move_cursor (int x, int y)
{
	x = x < 0 ? 0 : x > 0 ? 2 : 1;
	y = y < 0 ? 0 : y > 0 ? 2 : 1;
	return edit_move_cursors [y*3+x];
}

/*---------------------------------------------------------------------------*/
#define EDITOR_MAGIC 'EDIT'

typedef struct	_EDITOR {
		DWORD	magic;
		HWND	hwnd;
		TBUF *	buf;		/* text displayed in an editor */
		int	nlines;		/* current number of lines in buf */
		int	ncolumns;	/* maximal line length */
		int	left;		/* left margin */
		int	x0, y0;		/* up-left corner position, 0-based */
		int	x, y;		/* cursor position, 0-based */
		int	xsave;		/* phys. x cursor position for vertical move */
		int	sx, sy;		/* size of window in characters (incl. last line */
		int	xpage, ypage;	/* numberof fully visible lines */
		RECT	rc;		/* size of window in pizels */
		int	dx, dy;		/* char cell size */
		int	x1, y1;		/* beginning of block */
		int	x2, y2;		/* end of block */
		int	high_y;		/* highlighted line, -1 if none */
		BOOL	is_block;	/* block present */
		BOOL	cursor_visible;	/* cursor visible */
		BOOL	rect_block;	/* 1 - rect block; 0 - standard block */
		BOOL	persist_block;	/* 1 - do not drop block after typing */
		HFONT	font;
		BOOL	insert_mode;	/* TRUE == insert, FALSE == overwrite */
		int	tab_size;
		int	xshift_size;
		long	cursor_bookmark;
		long	block_bookmark1;
		long	block_bookmark2;
		BOOL	hscroll_present;
		BOOL	vscroll_present;
		int	wmsize_count;
		int	xrange, yrange;	/* scrolling ranges */
		EDIT_NOTIFY_PROC notify_handler;
		DWORD	notify_param;
		long	edit_code;
		long	bookmarks [9][2];
		DWORD	lang_token;
		int	wheel_sum;
		BOOL	in_mouse_scrolling;
		int	mouse_scroll_x;
		int	mouse_scroll_y;

		BOOL	bs_joins_lines;
		BOOL	del_joins_lines;
		BOOL	cr_splits_line;
		BOOL	tab_types;
		BOOL	tab_as_spaces;
		BOOL	smart_tabs;
		BOOL	cr_indents;

		BOOL	show_left_margin;
		BOOL	show_right_margin;
		BOOL	show_tabs;
		BOOL	right_margin;
		BOOL	show_bookmarks;

		int	paste_pos;
		int	block_type;
		int	block_paste;
		BOOL	read_only;

		BOOL	macro_toggle;
		EditMacroSet * macroset;
    char * clipboard;
	}
		EDITOR;

void	test_scrollbars (EDITOR * ed);
void	edit_char_typed (EDITOR * ed, char c);

EDIT_NOTIFY notify;
char spaces [129];

int	call_notify (EDITOR * ed, int code, int def_result)
{
	if (!ed -> notify_handler) return def_result;
 //return def_result;
	notify.is_block = ed->is_block;
	notify.x1 = ed->x1;
	notify.y1 = ed->y1;
	notify.x2 = ed->x2;
	notify.y2 = ed->y2;
	notify.insert_mode = ed->insert_mode;
	notify.nlines = ed->nlines;
	notify.has_focus = GetFocus () == ed->hwnd;
	notify.result = def_result;
	ed->notify_handler (code, &notify, ed->notify_param);
	return notify.result;
}

#define in_block(x, y) (ed->is_block &&	\
			y == ed->y1 && y == ed->y2 && x >= ed->x1 && x < ed->x2 || \
			y == ed->y1 && x >= ed->x1 || \
			y == ed->y2 && x <  ed->x2 || \
			y >  ed->y1 && y <= ed->y2)


int	log_to_phys (EDITOR * ed, int line, int logx)
{
	int len;
	char * s;
	int x, e;

	if (line < 0) line = 0;
	if (line >= ed->nlines) line = ed->nlines-1;
	if (! tbuf_get_line (ed->buf, line, &s, &len) || !len)
		return logx;
	e = 0;
	if (logx > len)	{
		e = logx - len;
		logx = len;
	}
	x = 0;
	while (logx--)	{
		if (*s == '\t')
			x = (x + ed->tab_size) / ed->tab_size * ed->tab_size;
		else ++x;
		++s;
	}
	return x + e;
}

int	phys_to_log (EDITOR * ed, int line, int x)
{
	int i, xphys;
	char * s;
	int len;

	if (line < 0) line = 0;
	if (line >= ed->nlines) line = ed->nlines-1;

	i = 0;
	if (! tbuf_get_line (ed->buf, line, &s, &len) || !len)
		return x;

	for (xphys = 0; xphys < x; ++i)
		if (i == len) return len + x - xphys;
		else if (s[i] == '\t')
			xphys = (xphys + ed->tab_size) / ed->tab_size * ed->tab_size;
		else ++ xphys;
	if (xphys > x) --i;
	return i;
	
}

int	starting_spaces (EDITOR * ed, int line)
{
	int len;
	char * s;
	int x, i;

	if (line < 0) return 0;
	if (line >= ed->nlines) return 0;
	if (! tbuf_get_line (ed->buf, line, &s, &len) || !len)
		return 0;
	x = 0;
	for (i = 0; i < len; i++)	{
		if (s[i] == '\t')
			x = (x + ed->tab_size) / ed->tab_size * ed->tab_size;
		else if (s[i] == ' ')
			++ x;
		else break;
	}
	return x;
}

void	edit_show_caret (EDITOR * ed)
{
	POINT p;
	if (ed->cursor_visible) return;
	GetCaretPos (&p);
	if (p.x >= ed->left)	{
		ShowCaret (ed->hwnd);
		ed->cursor_visible = TRUE;
	}
}

void	edit_hide_caret (EDITOR * ed)
{
	if (!ed->cursor_visible) return;
	HideCaret (ed->hwnd);
	ed->cursor_visible = FALSE;
}

void	edit_create_cursor (EDITOR * ed)
{
	int x = log_to_phys (ed, ed->y, ed->x);
	CreateCaret (ed->hwnd, 0, ed->insert_mode ? 2 : ed->dx, ed->dy);
	SetCaretPos ((x-ed->x0) * ed->dx + ed->left, (ed->y - ed->y0) * ed->dy);
	if (!ed->is_block)
		edit_show_caret (ed);
}

void	edit_update_caret (EDITOR * ed)
{
	if (GetFocus() != ed->hwnd)	return;
	edit_hide_caret (ed);
	DestroyCaret ();
	edit_create_cursor (ed);
}

void	edit_put_cursor (EDITOR * ed)
{
	int x;
	
	if (GetFocus () != ed->hwnd) return;
	x = log_to_phys (ed, ed->y, ed->x);	
	edit_hide_caret (ed);
	SetCaretPos ((x - ed->x0) * ed->dx + ed->left, (ed->y - ed->y0) * ed->dy);
	if (!ed->is_block)
		edit_show_caret (ed);
}

void	edit_error (EDITOR * ed)
{
	MessageBeep (MB_ICONEXCLAMATION);
}

void	edit_invalidate_line (EDITOR * ed, int i)
{
	RECT rc;

	if (i <  ed->y0) return;
	if (i >= ed->y0 + ed->sy) return;
	rc.top = (i-ed->y0) * ed->dy;
	rc.bottom = rc.top + ed->dy;
	rc.left = 0;
	rc.right = ed->rc.right;
	InvalidateRect (ed->hwnd, &rc, 0);
}

void	edit_invalidate_lines (EDITOR * ed, int first, int last)
{
	RECT rc;

	if (last <  ed->y0) return;
	if (first >= ed->y0 + ed->sy) return;
	if (first < ed->y0) first = ed->y0;
	if (last > ed->y0+ed->sy-1) last = ed->y0+ed->sy-1;
	rc.top = (first-ed->y0) * ed->dy;
	rc.bottom = (last-ed->y0+1) * ed->dy;
	rc.left = 0;
	rc.right = ed->rc.right;
	InvalidateRect (ed->hwnd, &rc, 0);
}

void	edit_invalidate_block (EDITOR * ed)
{
	RECT rc;
	int y1, y2;

	y1 = ed->y1; y2 = ed->y2;

	if (y2 < ed->y0) return;
	if (y1 >= ed->y0 + ed->sy) return;
	y1 -= ed->y0; if (y1 < 0) y1 = 0;
	y2 -= ed->y0; if (y2 >= ed->sy) y2 = ed->sy;
	rc.top = y1 * ed->dy;
	rc.bottom = (y2 + 1) * ed->dy + ed->dy;
	rc.left = 0;
	rc.right = ed->rc.right;
	InvalidateRect (ed->hwnd, &rc, 0);
}

void	edit_update_parsing (EDITOR * ed, BOOL all)
{
	PARSER_BLOCK b;
	LANG_BLOCK * lb;
	void * env;
	int start, fin;

	lb = Get_lang_block (ed->lang_token);
	if (!lb || !lb->module_parser) return;
	b.nlines = tbuf_get_nlines (ed->buf);
	if (all)	{
		start = 0;
		fin = b.nlines-1;
	} else
		if (!tbuf_query_inv_lines (ed->buf, &start, &fin)) return;
	tbuf_validate_inv_lines (ed->buf);
	b.buf = (PARSER_BUFFER) ed->buf;
	b.nlines = tbuf_get_nlines (ed->buf);
	b.data = tbuf_get_parsing_data (ed->buf);
	b.line_supply = (BOOL (*) (PARSER_BUFFER, int, char**, int*)) tbuf_get_line;
	b.redraw_lines = (void (*) (PARSER_BUFFER, int, int)) tbuf_redraw_lines;
	env = Get_lang_env (ed->lang_token);
	lb->module_parser (env, &b, start, fin);
}

typedef struct	{
			HBRUSH	 bkbrush;
			HPEN	 TabPen;
			OUTMODE	 modes [NMODES];
		}
			OUTMODEARRAY;

OUTMODEARRAY norm_mode;
OUTMODEARRAY sel_mode;
OUTMODEARRAY high_mode;
int right_margin_pos;

//HPEN TabPen, TabSelPen;

char parsing [2000];
int parsing_len;

LANG_BLOCK * lblock;
void * lenv;

/* Draw substring with the same block/highlight attribute;
   addr of substring is s;
   length is in len.
   returns number of characters written.
   *x becomes equal to next x position (in characters)
*/

int	draw_line_segment (HDC dc, EDITOR * ed, int y, int *x,
			   char * s, int i, char ** p, OUTMODEARRAY * m, int len)
{
	RECT rc;
	int x0;
	int i0, j;
	char oem [2000];
	HPEN pen;
	int curmode;

	if (len < 0) return i;
	curmode = 0;
	SelectObject (dc, m->modes [0].font);
	SetBkColor (dc, m->modes [0].bk);
	SetTextColor (dc, m->modes [0].fg);
	rc.top = y;
	rc.bottom = y+ed->dy;
	while (len && *x < ed->sx)	{
		if (i < parsing_len && parsing [i] != curmode)	{
			curmode = parsing [i] >= NMODES ? 0 : parsing [i];
			SelectObject (dc, m->modes [curmode].font);
			SetBkColor (dc, m->modes [curmode].bk);
			SetTextColor (dc, m->modes [curmode].fg);
		}
		if (s[i] == '\t')	{
			x0 = *x;
			*x = (x0 + ed->x0 + ed->tab_size) / ed->tab_size * ed->tab_size - ed->x0;
			++i;
			if (ed->show_tabs)	{
				rc.left	= x0 * ed->dx + ed->left;
				rc.right = *x * ed->dx + ed->left;
				FillRect (dc, &rc, m->bkbrush);
				pen = SelectObject (dc, m->TabPen);
				MoveToEx (dc, rc.left+1, rc.top+ed->dy/2, 0);
				LineTo (dc, rc.right-2, rc.top+ed->dy/2);
				MoveToEx (dc, rc.left+1, rc.top, 0);
				LineTo (dc, rc.left+1, rc.top+ed->dy);
				SelectObject (dc, pen);
				-- len;
			} else	{
				while (--len && *x < ed->sx && s[i] == '\t')	{
					*x += ed->tab_size;
					++ i;
				}
				rc.left	= x0 * ed->dx + ed->left;
				rc.right = *x * ed->dx + ed->left;
				FillRect (dc, &rc, m->bkbrush);
			}
		} else	{
			x0 = *x;
			if (!m->modes [curmode].oem_font)	{
				i0 = i;
				while (len && *x < ed->sx && s[i] != '\t' && (i >= parsing_len || parsing [i] == curmode))	{
					++*x;
					++i;
					-- len;
				}
				TextOut (dc, x0*ed->dx+ed->left, y, s+i0, i-i0);
			} else	{
				j = 0;
				while (len && *x < ed->sx && s[i] != '\t' && (i >= parsing_len || parsing [i] == curmode))	{
					++*x;
					oem [j++] = s[i++];
					-- len;
				}
        ansiToOem(oem, j);
        TextOut (dc, x0*ed->dx+ed->left, y, oem, j);
			}
		}
	}
	return i;
}

void	draw_editor_line (EDITOR * ed, int nline, int y, HDC dc, char * bookmarks)
{
	RECT lrc;
	int len, displen;
	int x, r;
	int i;
	char * s;
	unsigned char * p;
	HBRUSH br;
	
	GetClientRect (ed->hwnd, &lrc);

	lrc.left = 0;
	lrc.top = y;
	lrc.bottom = y + ed->dy;
	r = lrc.right;
	lrc.right = ed->left;
	if (ed->show_left_margin) -- lrc.right;
	if (nline == ed->high_y)
		FillRect (dc, &lrc, high_mode.bkbrush);
	else
		FillRect (dc, &lrc, norm_mode.bkbrush);
	lrc.left = ed->left;
	lrc.right = r;

	if (ed->show_bookmarks && bookmarks [nline-ed->y0])	{
		br = SelectObject (dc, high_mode.bkbrush);
		Ellipse (dc, ed->dx/2, y, ed->dx*5/2, y+ed->dy);
		SetBkMode (dc, TRANSPARENT);
		SetTextColor (dc, high_mode.modes [0].fg);
		TextOut (dc, ed->dx, y, &bookmarks [nline-ed->y0], 1);
		SetBkMode (dc, OPAQUE);
		SelectObject (dc, br);
	}

	if (! tbuf_get_line (ed->buf, nline, &s, &len)) return;
	if (!s) goto fill_trailing_space;
	i = phys_to_log (ed, nline, ed->x0);
	if (i>=len) goto fill_trailing_space;

	parsing_len = 1;
	parsing [0] = 0;

	if (lblock && lblock->line_parser)	{
		displen = min (len, i+ed->sx);
		if (displen >= sizeof (parsing)-10)
			displen = sizeof (parsing)-10;
		parsing_len = lblock->line_parser (lenv, s, parsing, len, displen, nline ? tbuf_get_parsing_data (ed->buf)[nline-1] : 0,
							tbuf_get_parsing_data (ed->buf)[nline]);
		if (!parsing_len) ++ parsing_len;
	}
	x = 0;

	if (nline == ed->high_y)
		draw_line_segment (dc, ed, y, &x, s, i, &p, &high_mode, len-i);
	else if (! ed->is_block || nline < ed->y1 || nline > ed -> y2)
		draw_line_segment (dc, ed, y, &x, s, i, &p, &norm_mode, len-i);
	else if (nline > ed -> y1 && nline < ed -> y2)	{
		draw_line_segment (dc, ed, y, &x, s, i, &p, &sel_mode, len-i);
	} else if (nline == ed->y1 && nline == ed->y2)	{
		i = draw_line_segment (dc, ed, y, &x, s, i, &p, &norm_mode, min (len, ed->x1) - i);
		i = draw_line_segment (dc, ed, y, &x, s, i, &p, &sel_mode,  min (len, ed->x2) - i);
		    draw_line_segment (dc, ed, y, &x, s, i, &p, &norm_mode, len - i);
	} else if (nline == ed->y1)	{
		i = draw_line_segment (dc, ed, y, &x, s, i, &p, &norm_mode, min (len, ed->x1) - i);
		    draw_line_segment (dc, ed, y, &x, s, i, &p, &sel_mode,  len - i);
	} else if (nline == ed->y2)	{
		i = draw_line_segment (dc, ed, y, &x, s, i, &p, &sel_mode,  min (len, ed->x2) - i);
		    draw_line_segment (dc, ed, y, &x, s, i, &p, &norm_mode, len - i);
	}
	lrc.left = x * ed->dx + ed->left;

fill_trailing_space:
	
	if (lrc.left < lrc.right)	{
		lrc.top = y;
		lrc.bottom = y + ed->dy;
		if (nline == ed->high_y)
			br = high_mode.bkbrush;
		else if (ed -> is_block && nline >= ed->y1 && nline < ed->y2)
			br = sel_mode.bkbrush;
		else
			br = norm_mode.bkbrush;
		if (lrc.left <= right_margin_pos)	{
			lrc.right = right_margin_pos;
			FillRect (dc, &lrc, br);
			lrc.left = right_margin_pos+1;
			lrc.right = ed->rc.right;
		}
		FillRect (dc, &lrc, br);
	}
}


void	draw_editor (EDITOR * ed, HDC dc, RECT * rcpaint)
{
	RECT rc;
	int i, x1, y1, y2;
	HFONT ofont;
	char bookmarks [1000];

	ofont = SelectObject (dc, ed->font);

	lblock = Get_lang_block (ed->lang_token);
	lenv = Get_lang_env (ed->lang_token);

	fill_edit_colors (ed->lang_token, norm_mode.modes, sel_mode.modes, high_mode.modes);

	norm_mode.bkbrush = CreateSolidBrush (norm_mode.modes [0].bk);
	sel_mode.bkbrush = CreateSolidBrush (sel_mode.modes  [0].bk);
	high_mode.bkbrush = CreateSolidBrush (high_mode.modes  [0].bk);

	norm_mode.TabPen = CreatePen (PS_DOT, 1, norm_mode.modes [0].fg);
	sel_mode.TabPen = CreatePen (PS_DOT, 1, sel_mode.modes [0].fg);
	high_mode.TabPen = CreatePen (PS_DOT, 1, high_mode.modes [0].fg);

	if (ed->show_bookmarks)	{
		memset (bookmarks, 0, sizeof (char) * ed->sy);
		for (i = 0; i < 9; i++)
			if (tbuf_query_bookmark (ed->buf, ed->bookmarks [i][0], &x1, &y1) &&
			    y1 >= ed->y0 && y1 < ed->y0 + ed->sy)
				bookmarks [y1-ed->y0] = i+'1';
	}

	if (!ed->show_right_margin || ed->right_margin<=ed->x0)
		right_margin_pos = -1;
	else
		right_margin_pos = (ed->right_margin-ed->x0)*ed->dx+ed->left;

	y1 = rcpaint->top / ed->dy;
	y2 = (rcpaint->bottom -1) / ed->dy;

	for (i = y1; i <= y2 && ed->y0+i < ed->nlines; i++)
		draw_editor_line (ed, ed->y0+i, i*ed->dy, dc, bookmarks);

	rc = ed->rc;
	rc.top = i*ed->dy;

	if (rc.top < rcpaint->bottom)	{
		RECT r;
		if (ed->show_left_margin)	{
			r = rc;
			rc.left = ed->left;
			r.right = ed->left-1;
			FillRect (dc, &r, norm_mode.bkbrush);
		}
		if (right_margin_pos >= ed->left)	{
			r = rc;
			rc.right = right_margin_pos;
			r.left = right_margin_pos+1;
			FillRect (dc, &r, norm_mode.bkbrush);
		}
		FillRect (dc, &rc, norm_mode.bkbrush);
	}

	SelectObject (dc, ofont);
	DeleteObject (norm_mode.TabPen);
	DeleteObject (sel_mode.TabPen);
	DeleteObject (high_mode.TabPen);

	if (right_margin_pos > 0 || ed->show_left_margin)	{
		HBITMAP bmp, obmp;
		HDC bdc;
		HBRUSH br;
		RECT r;
		bmp = CreateCompatibleBitmap (dc, 1, 2);
		bdc = CreateCompatibleDC (dc);
		obmp = SelectObject (bdc, bmp);
		SetPixel (bdc, 0, 0, 0xFF0000);
		SetPixel (bdc, 0, 1, 0xFFFF);
		SelectObject (bdc, obmp);
		DeleteDC (bdc);
		br = CreatePatternBrush (bmp);
		
		r.top = 0;
		r.bottom = ed->rc.bottom;

		if (right_margin_pos >= 0 && right_margin_pos < ed->rc.right)	{
			r.left  = right_margin_pos;
			r.right = right_margin_pos+1;
			FillRect (dc, &r, br);
		}
		if (ed->show_left_margin)	{
			r.left = ed->left-1;
			r.right = ed->left;
			FillRect (dc, &r, br);
		}
		DeleteObject (bmp);
		DeleteObject (br);
	}
	DeleteObject (norm_mode.bkbrush);
	DeleteObject (sel_mode.bkbrush);
	DeleteObject (high_mode.bkbrush);

	if (ed->in_mouse_scrolling)	{
		DrawIcon (dc, ed->mouse_scroll_x - move_hspot_x,
			  ed->mouse_scroll_y - move_hspot_y, mouse_scroll_icon);
	}
}

void	draw_editor_bmp (EDITOR * ed, HDC dc, RECT * rcpaint)
{
	RECT rc;
	HDC bdc;
	HBITMAP bmp, b;
	GetClientRect (ed->hwnd, &rc);
	bdc = CreateCompatibleDC (dc);
	bmp = CreateCompatibleBitmap (dc, rcpaint->right-rcpaint->left, rcpaint->bottom-rcpaint->top);
	b = SelectObject (bdc, bmp);
	SetWindowOrgEx (bdc, rcpaint->left, rcpaint->top, 0);
	draw_editor (ed, bdc, rcpaint);
	SetWindowOrgEx (bdc, 0, 0, 0);
	BitBlt (dc, rcpaint->left, rcpaint->top,
		rcpaint->right-rcpaint->left, rcpaint->bottom-rcpaint->top,
		bdc, 0, 0, SRCCOPY);
	SelectObject (bdc, b);
	DeleteObject (bmp);
	DeleteDC (bdc);
}

void	edit_set_lang_token (EDITOR * ed, DWORD token)
{
	ed->macroset = get_language_macro(token);

	if (ed->lang_token != token)	{
		ed->lang_token = token;
		edit_update_parsing (ed, TRUE);
		InvalidateRect (ed->hwnd, 0, 0);
	}
}

/* ========================= editor keyboard response ==================== */

#define PtoL(x, y) phys_to_log (ed, y, x)
#define LtoP(x, y) log_to_phys (ed, y, x)

void	edit_start_undo (EDITOR * ed)
{
	tbuf_start_undo_block (ed->buf);
	tbuf_store_cursor_pos (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2);
}

/* Cursor movement */

void	edit_gotoxy (EDITOR * ed, int x, int y, BOOL blk, BOOL final)
{
	int xphys, len;

	if (y >= ed->nlines) y = ed->nlines-1;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	len = tbuf_get_line_length (ed->buf, y);
	if (blk && x > len)	{
		if (ed->y1 != y || ed->y2 != y || ed->x1 < len)
			x = len;
		else
			blk = FALSE;
	}
	xphys = log_to_phys (ed, y, x);
	if (y < ed->y0)	{
		ed->y0 = y;
		InvalidateRect (ed->hwnd, 0, 0);
	}
	if (xphys < ed->x0)	{
		ed->x0 = xphys / ed->xshift_size * ed->xshift_size;
		InvalidateRect (ed->hwnd, 0, 0);
	}
	if (y >= ed->y0 + ed->ypage && ! (y == ed->y0 && ed->ypage == 0))	{
		ed->y0 = ed->ypage ? y - ed->ypage + 1 : y;
		InvalidateRect (ed->hwnd, 0, 0);
	}
	if (xphys >= ed->x0 + ed->xpage && ! (xphys == ed->x0 && ed->xpage == 0))	{
		ed -> x0 = ((xphys - ed->xpage) / ed->xshift_size + 1) * ed->xshift_size;
		if (xphys >= ed->x0 + ed->xpage)
			ed->x0 = x - ed->xpage + 1;
		InvalidateRect (ed->hwnd, 0, 0);
	}
	if (!ed->is_block && !blk)	{
		ed->y1 = ed->y2 = y;
		ed->x1 = ed->x2 = x;
	} else if (ed->is_block && !blk)	{
		edit_invalidate_block (ed);
		ed->is_block = FALSE;
		ed->y1 = ed->y2 = y;
		ed->x1 = ed->x2 = x;
	} else if (blk) {
		if (ed->is_block) edit_invalidate_block (ed);
		if (x > len) x = len;
		if (ed->y == ed->y1 && ed->x == ed->x1)	{
			ed->y1 = y; ed->x1 = x;
		} else if (ed->y == ed->y2 && ed->x == ed->x2)	{
			ed->y2 = y; ed->x2 = x;
		} else if (y > ed->y || y == ed->y && x > ed->x)	{
			ed->x1 = ed->x; ed->y1 = ed->y;
			ed->x2 = x;	ed->y2 = y;
		} else	{
			ed->x1 = x;	ed->y1 = y;
			ed->x2 = ed->x; ed->y2 = ed->y;
		}
		if (ed->y1 > ed->y2 || ed->y1 == ed->y2 && ed->x1 > ed->x2)	{
			int t;
			t = ed->x1; ed->x1 = ed->x2; ed->x2 = t;
			t = ed->y1; ed->y1 = ed->y2; ed->y2 = t;
		}
		ed->is_block = ed->x1 != ed->x2 || ed->y1 != ed->y2;
		if (ed->is_block)
			edit_invalidate_block (ed);
	}
	ed->x = x;
	ed->y = y;

	UpdateWindow (ed->hwnd);

	edit_put_cursor (ed);
	tbuf_change_bookmark (ed->buf, ed->cursor_bookmark, x, y);
	tbuf_change_bookmark (ed->buf, ed->block_bookmark1, ed->x1, ed->y1);
	tbuf_change_bookmark (ed->buf, ed->block_bookmark2, ed->x2, ed->y2);
	if (final) ed->xsave = xphys + ed->x0;
	test_scrollbars (ed);
	call_notify (ed, EDIT_NOTIFY_CURSOR, 0);
}

void	edit_set_block (EDITOR * ed, int x1, int y1, int x2, int y2)
{
	edit_gotoxy (ed, x2, y2, FALSE, TRUE);
	if (x1 != x2 || y1 != y2)
		edit_gotoxy (ed, x1, y1, TRUE, TRUE);
#if 0
	if (x1==x2 && y1==y2) return;
	ed->is_block = TRUE;
	ed->y1 = y1;
	ed->x1 = x1;
	ed->y2 = y2;
	ed->x2 = x2;
	edit_invalidate_lines (ed, y1, y2);
	edit_put_cursor (ed);
	call_notify (ed, EDIT_NOTIFY_CURSOR, 0);
#endif
}

void	edit_set_ypos (EDITOR * ed, int y0)
{
	if (y0 > ed->nlines-ed->sy) y0 = ed->nlines-ed->sy;
	if (y0 < 0) y0 = 0;
	if (y0 == ed->y0) return;
	ed->y0 = y0;
	InvalidateRect (ed->hwnd, 0, 0);
	test_scrollbars (ed);
}

void	edit_make_visible (EDITOR * ed, int x1, int y1, int x2, int y2)
{
	int y0;
	if (y2-y1+1 < ed->sy)	{
		if (ed->y0<=y1 && ed->y0+ed->ypage > y2)
			return;
		y0 = y1 - (ed->sy-(y2-y1+1))/2;
	} else
		y0 = y1;
	edit_set_ypos (ed, y0);
}

void	edit_right (EDITOR * ed, long blk)
{
	if (! blk && ed->is_block)
		edit_gotoxy (ed, ed->x2, ed->y2, FALSE, TRUE);
	else
		edit_gotoxy (ed, ed->x+1, ed->y, blk, TRUE);
}

void	edit_left (EDITOR * ed, long blk)
{
	if (! blk && ed->is_block)
		edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
	else
		edit_gotoxy (ed, ed->x-1, ed->y, blk, TRUE);
}

void	edit_rightword (EDITOR * ed, long blk)
{
	int x, y;
	x = ed->x;
	y = ed->y;
	tbuf_next_word (ed->buf, &x, &y, TRUE);
	if (x == ed->x && y == ed->y)
		edit_error (ed);
	else
		edit_gotoxy (ed, x, y, blk, TRUE);
}

void	edit_leftword (EDITOR * ed, long blk)
{
	int x, y;
	x = ed->x;
	y = ed->y;
	tbuf_prev_word (ed->buf, &x, &y, TRUE);
	if (x == ed->x && y == ed->y)
		edit_error (ed);
	else
		edit_gotoxy (ed, x, y, blk, TRUE);
}

void	edit_up	(EDITOR * ed, long blk)
{
	if (! blk && ed->is_block)
		edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
	edit_gotoxy (ed, PtoL (ed->xsave-ed->x0, ed->y-1), ed->y-1, blk, FALSE);
}

void	edit_down (EDITOR * ed, long blk)
{
	if (! blk && ed->is_block)
		edit_gotoxy (ed, ed->x2, ed->y2, FALSE, TRUE);
	edit_gotoxy (ed, PtoL (ed->xsave-ed->x0, ed->y+1), ed->y+1, blk, FALSE);
}

void	edit_pgup (EDITOR * ed, long blk)
{
	int ypage;
	int y, y0;

	if (! blk && ed->is_block)
		edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
	
	ypage = ed->ypage;
	if (! ypage) ypage = 1;

	y0 = ed->y0 - ypage;
	y  = ed->y - ypage;
	if (y < 0) y = 0;
	if (y0 > ed->nlines - ypage)
		y0 = ed->nlines - ypage;
	if (y0 < 0) y0 = 0;
	if (y0 != ed->y0)	{
		InvalidateRect (ed->hwnd, 0, 0);
		ed -> y0 = y0;
	}
	edit_gotoxy (ed, PtoL (ed->xsave - ed->x0, y), y, blk, FALSE);
}

void	edit_pgdown (EDITOR * ed, long blk)
{
	int ypage;
	int y, y0;

	if (! blk && ed->is_block)
		edit_gotoxy (ed, ed->x2, ed->y2, FALSE, TRUE);
	
	ypage = ed->ypage;
	if (! ypage) ypage = 1;
	y0 = ed->y0 + ypage;
	y  = ed->y  + ypage;
	if (y >= ed->nlines) y = ed->nlines - 1;
	if (y < 0) y = 0;
	if (y0 > ed->nlines - ypage)
		y0 = ed->nlines - ypage;
	if (y0 < 0) y0 = 0;
	if (y0 != ed->y0)	{
		InvalidateRect (ed->hwnd, 0, 0);
		ed -> y0 = y0;
	}
	edit_gotoxy (ed, PtoL (ed->xsave - ed->x0, y), y, blk, FALSE);
}

void	edit_linestart (EDITOR * ed, long blk)
{
	if (! blk && ed->is_block)
		edit_gotoxy (ed, 0, ed->y1, FALSE, TRUE);
	else
		edit_gotoxy (ed, 0, ed->y, blk, TRUE);
}

void	edit_lineend (EDITOR * ed, long blk)
{
	int len, y;

	if (! blk && ed->is_block)
		y = ed->y2;
	else
		y = ed->y;
	len = tbuf_get_line_length (ed->buf, y);
	edit_gotoxy (ed, len, y, blk, TRUE);
}

void	edit_filestart (EDITOR * ed, long blk)
{
	edit_gotoxy (ed, 0, 0, blk, TRUE);
}

void	edit_fileend (EDITOR * ed, long blk)
{
	edit_gotoxy (ed, 0, ed->nlines-1, blk, TRUE);
}

void	edit_pagestart (EDITOR * ed, long blk)
{
	edit_gotoxy (ed, 0, ed->y0, blk, TRUE);
}

void	edit_pageend (EDITOR * ed, long blk)
{
	edit_gotoxy (ed, 0, ed->y0 + ed->sy-1, blk, TRUE);
}

/* Special functions: CR, INS, DEL, et. */


void	edit_set_insert	(EDITOR * ed, long insert)
{
	if (ed->insert_mode == insert) return;
	ed->insert_mode = insert;
	edit_update_caret (ed);
}

void	edit_flipinsert	(EDITOR * ed, long param)
{
	edit_set_insert (ed, !ed->insert_mode);
}

void	edit_insertline (EDITOR * ed, long param)
{
	int x1, x2, y1, y2;
	if (ed->is_block && ed->y1 != ed->y2)	{
		edit_error (ed);
		return;
	}
	x1 = ed->x1; y1 = ed->y1; x2 = ed->x2; y2=ed->y2;
	edit_start_undo (ed);
	tbuf_insert_lines (ed->buf, ed->y1, 1);
	edit_set_block (ed, x1, y1+1, x2, y2+1);
	tbuf_finish_undo_block (ed->buf);
}

void	indent_cr (EDITOR * ed)
{
	int n;
	if (!ed->cr_indents)
		return;
	
	n = call_notify (ed, EDIT_NOTIFY_CR, starting_spaces (ed, ed->y-1));
	if (n)	{
		if (ed->insert_mode)	{
			if (!ed->tab_as_spaces)
				while (n >= ed->tab_size)	{
					edit_char_typed (ed, '\t');
					n -= ed->tab_size;
				}
			while (n--) edit_char_typed (ed, ' ');
		} else
			edit_gotoxy (ed, n, ed->y, FALSE, TRUE);
	}
}

void	edit_splitline (EDITOR * ed, long param)
{
	int len;
	
	edit_start_undo (ed);
	if (ed->is_block)	{
		if (ed->block_type == 0)	{
			tbuf_delete_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2);
			edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
		} else if (ed->block_type < 0)
			edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
		else
			edit_gotoxy (ed, ed->x2, ed->y2, FALSE, TRUE);
	}
	len = tbuf_get_line_length (ed->buf, ed->y);
	if (ed->x == 0)
		tbuf_insert_lines (ed->buf, ed->y, 1);
	else	{
		if (!tbuf_insert_lines (ed->buf, ed->y+1, 1)) return;
		if (ed->x < len)
			tbuf_move_text (ed->buf, ed->x, ed->y, len, ed->y, 0, ed->y+1);
		else
			edit_gotoxy (ed, 0, ed->y+1, FALSE, TRUE);
	}
	edit_gotoxy (ed, ed->x, ed->y, FALSE, TRUE);
	indent_cr (ed);
	tbuf_finish_undo_block (ed->buf);
}

void	edit_nextline (EDITOR * ed, long param)
{
	BOOL b;

	if (ed->read_only)	{
		if (ed->y <= ed->nlines-1)
			edit_gotoxy (ed, 0, ed->y+1, FALSE, TRUE);
		return;
	}

	if (ed->y == ed->nlines-1)	{
		edit_start_undo (ed);
		b = tbuf_insert_lines (ed->buf, ed->nlines, 1);
		tbuf_finish_undo_block (ed->buf);
		if (!b) return;
	}
	edit_gotoxy (ed, 0, ed->y+1, FALSE, TRUE);
	indent_cr (ed);
}

void	edit_cr (EDITOR * ed, long param)
{
	if (ed->insert_mode && ed->cr_splits_line && !ed->read_only)
		edit_splitline (ed, 0);
	else
		edit_nextline (ed, 0);
}

void	edit_join (EDITOR * ed, int y, int spaces)
{
	int len1, len2, xsave;
	char * s2;

	if (! tbuf_get_line (ed->buf, y+1, &s2, &len2)) return;
	xsave = ed->x;
	if (len2 != 0)	{
		len1 = tbuf_get_line_length (ed->buf, y);
		tbuf_insert_string (ed->buf, y, len1+spaces, s2, len2);
	} else
		spaces = 0;
	tbuf_move_bookmarks (ed->buf, 0, y+1, len2, y+1, len1+spaces, y);
	tbuf_delete_lines (ed->buf, y+1, 1);
	edit_gotoxy (ed, xsave, ed->y, FALSE, TRUE);
}

void	edit_join_lines (EDITOR * ed, long param)
{
	if (ed->is_block)	{
		edit_error (ed);
		return;
	}
	tbuf_start_undo_block (ed->buf);
	edit_join (ed, ed->y, 0);
	tbuf_finish_undo_block (ed->buf);
}

void	edit_delline (EDITOR * ed, long param)
{
	int n;
	if (ed -> nlines == 0) return;
	if (ed->y2 > ed->y1 && !ed->x2)
		-- ed->y2;
	n = ed->y2 - ed->y1 + 1;
	edit_start_undo (ed);
	edit_gotoxy (ed, 0, ed->y1, FALSE, TRUE);
	if (ed->y1+n == ed->nlines)	{
		tbuf_delete_string (ed->buf, ed->nlines-1, 0, LINE_END);
		-- n;
	}
	if (n)
		tbuf_delete_lines (ed->buf, ed->y1, n);
	tbuf_finish_undo_block (ed->buf);
}

void	edit_del (EDITOR * ed, long param)
{
	int len;
	if (ed->is_block)	{
		edit_start_undo (ed);
		tbuf_delete_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2);
		edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
		tbuf_finish_undo_block (ed->buf);
	} else	{
		len = tbuf_get_line_length (ed->buf, ed->y);
		if (ed->x < len)
			tbuf_delete_string (ed->buf, ed->y, ed->x, 1);
		else if (ed->del_joins_lines)	{
			edit_start_undo (ed);
			edit_join (ed, ed->y, ed->x-len);
			tbuf_finish_undo_block (ed->buf);
		}
	}
}

void	edit_delword (EDITOR * ed, long param)
{
	int x, y;
	if (ed->is_block)
		edit_error (ed);
	else	{
		x = tbuf_word_end (ed->buf, ed->x, ed->y);
		if (x == ed->x)	{
			y = ed->y;
			tbuf_next_word (ed->buf, &x, &y, FALSE);
			if (x == ed->x)
				x = tbuf_word_end (ed->buf, x, y);
		}
		if (x != ed->x)	{
			tbuf_start_undo_block (ed->buf);
			tbuf_delete_string (ed->buf, ed->y, ed->x, x-ed->x);
			tbuf_finish_undo_block (ed->buf);
		}
	}
}

void	edit_delwordleft (EDITOR * ed, long param)
{
	int x, y, x1;

	if (ed->is_block || ed->x == 0)
		edit_error (ed);
	else	{
		x = tbuf_word_start (ed->buf, ed->x, ed->y);
		if (x == ed->x)	{
			y = ed->y;
			tbuf_prev_word (ed->buf, &x, &y, FALSE);
			x1 = tbuf_word_end (ed->buf, x, y);
			if (x1 != ed->x)
				x = x1;
		}
		if (x != ed->x)	{
			edit_start_undo (ed);
			tbuf_delete_string (ed->buf, ed->y, x, ed->x-x);
			tbuf_finish_undo_block (ed->buf);
		}
	}
}


void	edit_bs (EDITOR * ed, long param)
{
	int len, y;

	if (ed->is_block)	{
		edit_start_undo (ed);
		tbuf_delete_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2);
		edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
		tbuf_finish_undo_block (ed->buf);
		return;
	}
	if (ed->x == 0)	{
		if (!ed->bs_joins_lines || !ed->y)	{
			edit_error (ed);
			return;
		}
		edit_start_undo (ed);
		y = ed->y-1;
		len = tbuf_get_line_length (ed->buf, y);
		edit_join (ed, y, 0);
		edit_gotoxy (ed, len, y, FALSE, TRUE);
	} else if (ed->x <= tbuf_get_line_length (ed->buf, ed->y))	{
		edit_start_undo (ed);
		tbuf_delete_string (ed->buf, ed->y, ed->x-1, 1);
	} else
		edit_gotoxy (ed, ed->x-1, ed->y, FALSE, TRUE);
	tbuf_finish_undo_block (ed->buf);
}

void	edit_tab (EDITOR * ed, long param)
{
	int x, xp, xp1, n, y;
	BOOL save_insert_mode;

	tbuf_start_undo_block (ed->buf);
	if (ed->insert_mode || ed->tab_types)	{
		x = ed->is_block ? ed->x1 : ed->x;
		y = ed->is_block ? ed->y1 : ed->y;
		xp = LtoP (x, y);
		xp1 = (xp / ed->tab_size + 1) * ed->tab_size;
		if (ed->smart_tabs)
			xp1 = call_notify (ed, EDIT_NOTIFY_TAB, xp1);
		save_insert_mode = ed->insert_mode;
		if (ed->is_block) ed->insert_mode = TRUE;
		if (!ed->tab_as_spaces)	{
			n = xp1/ed->tab_size - xp/ed->tab_size;
			while (n--)
				edit_char_typed (ed, '\t');
			x = ed->is_block ? ed->x1 : ed->x;
			xp = LtoP (x, y);
		}
		n = xp1 - xp;
		while (n--)
			edit_char_typed (ed, ' ');
		ed->insert_mode = save_insert_mode;
	} else	{
		if (ed -> is_block)
			edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
		x = LtoP (ed->x, ed->y);
		x = (x / ed->tab_size + 1) * ed->tab_size;
		edit_gotoxy (ed, PtoL (x, ed->y), ed->y, FALSE, TRUE);
	}
	tbuf_finish_undo_block (ed->buf);
}

void	edit_backtab (EDITOR * ed, long param)
{
	int x;
	if (ed -> is_block)
		edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
	x = LtoP (ed->x, ed->y);
	if (!x) return;
	x = (x-1) / ed->tab_size * ed->tab_size;
	edit_gotoxy (ed, PtoL (x, ed->y), ed->y, FALSE, TRUE);
}

BOOL	do_cb_copy (EDITOR * ed);
BOOL	do_cb_paste (EDITOR * ed);
BOOL	do_cb_exchange (EDITOR * ed);
BOOL	do_cb_add (EDITOR * ed);

void	edit_cb_copy (EDITOR * ed, long param)
{
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));
	if (!do_cb_copy (ed))
		edit_error (ed);
	SetCursor (cur);
}

void	edit_cb_cut (EDITOR * ed, long param)
{
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));
	if (!do_cb_copy (ed))
		edit_error (ed);
	else
		edit_del (ed, 0);
	SetCursor (cur);
}

void	edit_cb_paste (EDITOR * ed, long param)
{
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));
	if (!do_cb_paste (ed))
		edit_error (ed);
	SetCursor (cur);
}

void	edit_cb_exchange (EDITOR * ed, long param)
{
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));
	if (!do_cb_exchange (ed))
		edit_error (ed);
	SetCursor (cur);
}

void	edit_cb_add (EDITOR * ed, long param)
{
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));
	if (!do_cb_add (ed))
		edit_error (ed);
	SetCursor (cur);
}

BOOL	edit_undo (EDITOR * ed, long param)
{
	int x1, y1, x2, y2;
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));
#if 0
	if (tbuf_last_cursor_pos (ed->buf, &x1, &y1, &x2, &y2))	{
		if (x1 == x2 && y1 == y2)	{
			if (!ed->is_block && (ed->x != x1 || ed->y != y1))	{
				edit_gotoxy (ed, x1, y1, FALSE, TRUE);
				return TRUE;
			}
		} else	{
			if (ed->is_block && (ed->x1 != x1 || ed->y1 != y1 || ed->x2 != x2 || ed->y2 != y2))	{
				edit_set_block (ed, x1, y1, x2, y2);
				return TRUE;
			}
		}
	}
#endif
	if (!tbuf_undo (ed->buf, &x1, &y1, &x2, &y2))	{
		edit_error (ed);
		SetCursor (cur);
		return FALSE;
	}
	edit_set_block (ed, x1, y1, x2, y2);
	SetCursor (cur);
	return TRUE;
}

void	edit_setbookmark (EDITOR * ed, long param)
{
	int x1, y1;

	if (ed->bookmarks [param-1][0] &&
	    tbuf_query_bookmark (ed->buf, ed->bookmarks [param-1][0], &x1, &y1))
	{
		edit_invalidate_line (ed, y1);
		tbuf_change_bookmark (ed->buf, ed->bookmarks [param-1][0], ed->x1, ed->y1);
	} else
		ed->bookmarks [param-1][0] = tbuf_add_bookmark (ed->buf, ed->x1, ed->y1, TRUE);
	if (ed->bookmarks [param-1][1] &&
	    tbuf_query_bookmark (ed->buf, ed->bookmarks [param-1][1], &x1, &y1))
		tbuf_change_bookmark (ed->buf, ed->bookmarks [param-1][1], ed->x2, ed->y2);
	else
		ed->bookmarks [param-1][1] = tbuf_add_bookmark (ed->buf, ed->x2, ed->y2, TRUE);
	edit_invalidate_line (ed, ed->y1);
	call_notify (ed, EDIT_NOTIFY_SETBOOKMARK, param);
}

BOOL	edit_query_bookmark (EDITOR * ed, int num)
{
	int x1, y1, x2, y2;
	BOOL b1, b2;
	b1 = tbuf_query_bookmark (ed->buf, ed->bookmarks [num-1][0], &x1, &y1);
	b2 = tbuf_query_bookmark (ed->buf, ed->bookmarks [num-1][1], &x2, &y2);
	return b1 || b2;
}

void	edit_gotobookmark (EDITOR * ed, long param)
{
	int x1, y1, x2, y2;
	BOOL b1, b2;
	b1 = tbuf_query_bookmark (ed->buf, ed->bookmarks [param-1][0], &x1, &y1);
	b2 = tbuf_query_bookmark (ed->buf, ed->bookmarks [param-1][1], &x2, &y2);
	if (!b1 && !b2)
		edit_error (ed);
	else if (b1 && b2)
		edit_set_block (ed, x1, y1, x2, y2);
	else if (b1)
		edit_set_block (ed, x1, y1, x1, y1);
	else
		edit_set_block (ed, x2, y2, x2, y2);
}

void	edit_deletebookmark (EDITOR * ed, long param)
{
	int x1, y1;
	BOOL b1;
	b1 = tbuf_query_bookmark (ed->buf, ed->bookmarks [param-1][0], &x1, &y1);
	if (!b1)
		edit_error (ed);
	else	{
		tbuf_delete_bookmark (ed->buf, ed->bookmarks [param-1][0]);
		tbuf_delete_bookmark (ed->buf, ed->bookmarks [param-1][1]);
		ed->bookmarks [param-1][0] = 0;
		ed->bookmarks [param-1][1] = 0;
		edit_invalidate_line (ed, y1);
	}
}

int	find_nonspace (TBUF * buf, int y)
{
	char * p;
	int len, i;

	if (!tbuf_get_line (buf, y, &p, &len)) return -1;
	for (i = 0; i < len; i++)
		if (p [i] != '\t' && p [i] != ' ') break;
	return i >= len ? -1 : i;
}

void	edit_indent_line_left (EDITOR * ed, int y, int dx)
{
	int i, n, l, p;

	i = find_nonspace (ed->buf, y);
	if (i < 0) i = tbuf_get_line_length (ed->buf, y);
	n = starting_spaces (ed, y);
	if (n <= dx)	{
		tbuf_delete_string (ed->buf, y, 0, i);
		return;
	}
	l = PtoL (n-dx, y);
	p = LtoP (l, y);
	tbuf_delete_string (ed->buf, y, l, i-l);
	if (p < n-dx)
		tbuf_insert_string (ed->buf, y, l, spaces, n-dx-p);
}

void	edit_indent_left (EDITOR * ed, long param)
{
	int y, y2;
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));

	edit_start_undo (ed);
	y2 = ed->y2;
	if (ed->y1 != ed->y2 && ed->x2 == 0)
		-- y2;
	for (y = ed->y1; y <= y2; y++)
		edit_indent_line_left (ed, y, param ? 1 : ed->tab_size);
	if (ed->is_block)
		edit_set_block (ed, 0, ed->y1, ed->x2, ed->y2);
	tbuf_finish_undo_block (ed->buf);
	SetCursor (cur);
}

void	edit_indent_right (EDITOR * ed, long param)
{
	int y, y2, len, i;
	
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));

	edit_start_undo (ed);
	y2 = ed->y2;
	if (ed->y1 != ed->y2 && ed->x2 == 0)
		-- y2;

	if (param || ed->tab_as_spaces)	{
		if (param) len = 1; else len = ed->tab_size;
		for (y = ed->y1; y <= y2; y++)	{
			if (tbuf_get_line_length (ed->buf, y) == 0) continue;
			i = find_nonspace (ed->buf, y);
			if (i >= 0)
				tbuf_insert_string (ed->buf, y, i, spaces, len);
		}
	} else
		for (y = ed->y1; y <= y2; y++)
			if (tbuf_get_line_length (ed->buf, y))
				tbuf_insert_string (ed->buf, y, 0, "\t", 1);
	if (ed->is_block)
		edit_set_block (ed, 0, ed->y1, ed->x2, ed->y2);
	tbuf_finish_undo_block (ed->buf);
	SetCursor (cur);
}

BOOL	copy_fun (char * s, int len, DWORD p)
{
	memcpy (*(char**)p, s, len);
	*(char**)p += len;
	return TRUE;
}

BOOL	edit_process_word (EDITOR * ed, void (*process_proc) (char * buf, int len))
{
	long len;
	char *tmp, *ptr;
	int x1, y1, x2, y2;
	BOOL b;

	x1 = ed->x1; y1 = ed->y1; x2 = ed->x2; y2 = ed->y2;
	if (y2 == y1 && x2 == x1)	{
		x1 = tbuf_word_start (ed->buf, x1, y1);
		x2 = tbuf_word_end   (ed->buf, x1, y1);
	}
	if (y1 > y2 || y1 == y2 && x1 >= x2)
		return FALSE;

	len = tbuf_stream_out_block (ed->buf, x1, y1, x2, y2, "\n", 1, 0, 0);
	tmp = malloc (len+1);
	if (!tmp)
		return FALSE;

	ptr = tmp;
	tbuf_stream_out_block (ed->buf, x1, y1, x2, y2, "\n", 1, copy_fun, (DWORD)&ptr);
	*ptr = 0;
	process_proc (tmp, len);
	edit_start_undo (ed);
	tbuf_delete_block (ed->buf, x1, y1, x2, y2);
	b = tbuf_stream_in_text (ed->buf, tmp, len, x1, y1);
	if (b)
		edit_set_block (ed, x1, y1, x2, y2);

	free (tmp);
	tbuf_finish_undo_block (ed->buf);
	return TRUE;
}

void	uppercase_word (char * s, int len)
{
	for (;len--; s++)
		*s = toupper_table [(unsigned char)*s];
}

void	edit_uppercase (EDITOR * ed, long param)
{
	edit_process_word (ed, uppercase_word);
}

void	flipcase_word (char * s, int len)
{
	for (;len--; s++)
		*s = toothercase_table [(unsigned char)*s];
}

void	edit_flipcase (EDITOR * ed, long param)
{
	edit_process_word (ed, flipcase_word);
}


char * lookup_macro_key(EDITOR *ed, WPARAM key)
{
	BYTE mod = get_keyboard_modes() | FVIRTKEY;

	return LookupMacro(ed->macroset, (int)key, mod);
}

void	edit_playmacro_string(EDITOR * ed, char * s)
{
	int i;

	for (i=0; s[i]; i++)
		edit_char_typed(ed,s[i]);
}

void	edit_action_by_name (EDITOR * ed, char * name);

void	edit_play_macro(EDITOR * ed, char * s)
{
	int i = 0, j;
	char sep, cmdname[32];

	edit_start_undo(ed);
	while (s[i]) {
		if ( (s[i] == '\"') || (s[i] == '\'') ) {
			sep = s[i++];
			while ( (s[i]) && (s[i] != sep) ) {
				edit_char_typed(ed, s[i]);
				i++;
			}
			if ( s[i] ) i++;
		} else if ( (s[i] == ',') || (s[i] == ' ') ) {
			i++;
		} else {
			j = 0;
			while ( (s[i]) && (s[i] != ' ') && (s[i] != ',') ) {
				if (j<32) {
					cmdname[j++] = s[i++];
				} //if
			} //while
			if (j<32) 
				cmdname[j]=0;
			edit_action_by_name(ed, cmdname);
		} // if-else-if
	} // while
	tbuf_finish_undo_block (ed->buf);
}  // edit_play_macro

static BOOL do_private_copy (EDITOR * ed)
{
  long len;
	char * p;
	if (ed->is_block && ed->y1 <= ed->y2 && (ed->y1 != ed->y2 || ed->x1 < ed->x2))
  {
    len = tbuf_stream_out_block
      (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2, 0, 0);
    ed->clipboard = realloc(ed->clipboard, len + 1);
    if (ed->clipboard) {
      p = ed->clipboard;
      tbuf_stream_out_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2,
                             copy_fun, (DWORD)&p);
      *p = 0;
      return TRUE;
    }
  }
  ed->clipboard = realloc(ed->clipboard, 1);
  ed->clipboard[0] = 0;
  return FALSE;
}

static void edit_private_copy (EDITOR * ed, long param)
{
  do_private_copy (ed);
}

static void edit_private_paste (EDITOR * ed, long param)
{
  int x0, y0;
  if(ed->clipboard) {
    edit_start_undo (ed);
    if (ed->is_block)
      if (ed->block_paste == 0)
        tbuf_delete_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2);
      else if (ed->block_paste < 0)
        edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
      else
        edit_gotoxy (ed, ed->x2, ed->y2, FALSE, TRUE);
    x0 = ed->x1;
    y0 = ed->y1;
    if (tbuf_stream_in_text
      (ed->buf, ed->clipboard, strlen (ed->clipboard), x0, y0))
      if (ed->paste_pos < 0)
        edit_gotoxy (ed, x0, y0, FALSE, TRUE);
      else if (ed->paste_pos > 0)
        edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
      else
        edit_set_block (ed, x0, y0, ed->x1, ed->y1);
    tbuf_finish_undo_block (ed->buf);
  } else
    edit_error(ed);
}

static void edit_private_cut (EDITOR * ed, long param)
{
  if (!do_private_copy (ed))
		edit_error (ed);
	else
		edit_del (ed, 0);
}

static void edit_private_exchange (EDITOR * ed, long param)
{
  long len;
	char * buf, * ptr;
	int x0, y0;
	if (ed->is_block && ed->y1 <= ed->y2 && (ed->y1 != ed->y2 || ed->x1 < ed->x2))
    if (!ed->clipboard)
      do_private_copy (ed);
    else {
      len = tbuf_stream_out_block
        (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2, 0, 0);
      buf = malloc(len + 1);
      if (buf) {
        ptr = buf;
        tbuf_stream_out_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n",
                               2, copy_fun, (DWORD)&ptr);
        *ptr = 0;
        edit_start_undo (ed);
        x0 = ed->x1;
        y0 = ed->y1;
        if (ed->is_block)
          tbuf_delete_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2);
        if (tbuf_stream_in_text
          (ed->buf, ed->clipboard, strlen (ed->clipboard), x0, y0))
          if (ed->paste_pos < 0)
            edit_gotoxy (ed, x0, y0, FALSE, TRUE);
          else if (ed->paste_pos > 0)
            edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
          else
            edit_set_block (ed, x0, y0, ed->x1, ed->y1);
        tbuf_finish_undo_block (ed->buf);
        free(ed->clipboard);
        ed->clipboard = buf;
        return;
      }
    }
  edit_error(ed);
}

static void edit_private_add (EDITOR * ed, long param)
{
  long len0;
	char * ptr;
	if (ed->is_block && ed->y1 <= ed->y2 && (ed->y1 != ed->y2 || ed->x1 < ed->x2))
  {
    if (!ed->clipboard)
      do_private_copy (ed);
    len0 = strlen (ed->clipboard);
    ed->clipboard = realloc (ed->clipboard, len0 + tbuf_stream_out_block
      (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2, 0, 0) + 1);
    if (ed->clipboard) {
      ptr = ed->clipboard + len0;
      tbuf_stream_out_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2,
        copy_fun, (DWORD)&ptr);
      *ptr = 0;
      return;
    }
  }
  edit_error(ed);
}

typedef struct	{
		int	action;
		void	(* action_func) (EDITOR * ed, long param);
		long	param;
		BOOL	writes;
		char *	name;
} EditActionDesc;

EditActionDesc	edit_actions [] =
	{
		{0, 0, 0, 0, ""},
		{EDIT_RIGHT,		edit_right,		0, 0, "RIGHT"},
		{EDIT_LEFT,		edit_left,		0, 0, "LEFT"},
		{EDIT_UP,		edit_up,		0, 0, "UP"},
		{EDIT_DOWN,		edit_down,		0, 0, "DOWN"},
		{EDIT_PGUP,		edit_pgup,		0, 0, "PGUP"},
		{EDIT_PGDOWN,		edit_pgdown,		0, 0, "PGDOWN"},
		{EDIT_LINESTART,	edit_linestart,		0, 0, "LINESTART"},
		{EDIT_LINEEND,		edit_lineend,		0, 0, "LINEEND"},
		{EDIT_FILESTART,	edit_filestart,		0, 0, "FILESTART"},
		{EDIT_FILEEND,		edit_fileend,		0, 0, "FILEEND"},
		{EDIT_PAGESTART,	edit_pagestart,		0, 0, "PAGESTART"},
		{EDIT_PAGEEND,		edit_pageend,		0, 0, "PAGEEND"},
		{EDIT_RIGHTWORD,	edit_rightword,		0, 0, "RIGHTWORD"},
		{EDIT_LEFTWORD,		edit_leftword,		0, 0, "LEFTWORD"},

		{EDIT_BLKRIGHT,		edit_right,		1, 0, "BLKLEFT"},
		{EDIT_BLKLEFT,		edit_left,		1, 0, "BLKRIGHT"},
		{EDIT_BLKUP,		edit_up,		1, 0, "BLKUP"},
		{EDIT_BLKDOWN,		edit_down,		1, 0, "BLKDOWN"},
		{EDIT_BLKPGUP,		edit_pgup,		1, 0, "BLKPGUP"},
		{EDIT_BLKPGDOWN,	edit_pgdown,		1, 0, "BLKPGDOWN"},
		{EDIT_BLKLINESTART,	edit_linestart,		1, 0, "BLKLINESTART"},
		{EDIT_BLKLINEEND,	edit_lineend,		1, 0, "BLKLINEEND"},
		{EDIT_BLKFILESTART,	edit_filestart,		1, 0, "BLKFILESTART"},
		{EDIT_BLKFILEEND,	edit_fileend,		1, 0, "BLKFILEEND"},
		{EDIT_BLKPAGESTART,	edit_pagestart,		1, 0, "BLKPAGESTART"},
		{EDIT_BLKPAGEEND,	edit_pageend,		1, 0, "BLKPAGEEND"},
		{EDIT_BLKRIGHTWORD,	edit_rightword,		1, 0, "BLKRIGHTWORD"},
		{EDIT_BLKLEFTWORD,	edit_leftword,		1, 0, "BLKLEFTWORD"},

		{EDIT_DEL,		edit_del,		0, 1, "DEL"},
		{EDIT_BS,		edit_bs,		0, 1, "BS"},
		{EDIT_TAB,		edit_tab,		0, 1, "TAB"},
		{EDIT_BACKTAB,		edit_backtab,		0, 0, "BACKTAB"},
		{EDIT_DELWORD,		edit_delword,		0, 1, "DELWORD"},
		{EDIT_DELWORDLEFT,	edit_delwordleft,	0, 1, "DELWORDLEFT"},

		{EDIT_FLIPINSERT,	edit_flipinsert,	0, 0, "FLIPINSERT"},
		{EDIT_NEXTLINE,		edit_nextline,		0, 0, "NEXTLINE"},
		{EDIT_INSERTLINE,	edit_insertline,	0, 1, "INSERTLINE"},
		{EDIT_CR,		edit_cr,		0, 0, "CR"},
		{EDIT_DELETELINE,	edit_delline,		0, 1, "DELLINE"},
		{EDIT_SPLITLINE,	edit_splitline,		0, 1, "SPLITLINE"},
		{EDIT_JOIN_LINES,	edit_join_lines,	0, 1, "JOINLINES"},

		{EDIT_INDENT_LEFT,	edit_indent_left,	0, 1, "INDENTLEFT"},
		{EDIT_INDENT_LEFT1,	edit_indent_left,	1, 1, "INDENTLEFT1"},
		{EDIT_INDENT_RIGHT,	edit_indent_right,	0, 1, "INDENTRIGHT"},
		{EDIT_INDENT_RIGHT1,	edit_indent_right,	1, 1, "INDENTRIGHT1"},

		{EDIT_CB_COPY,		 edit_cb_copy,		 0, 0, "SYSCOPY"},
		{EDIT_CB_PASTE,		 edit_cb_paste,		 0, 1, "SYSPASTE"},
		{EDIT_CB_CUT,		   edit_cb_cut,		   0, 1, "SYSCUT"},
		{EDIT_CB_EXCHANGE, edit_cb_exchange, 0, 1, "SYSEXCHANGE"},
		{EDIT_CB_ADD,		   edit_cb_add,      0, 0, "SYSADD"},
		{EDIT_UNDO,		     edit_undo,		     0, 1, "UNDO"},
	
		{EDIT_SETBOOKMARK1,	edit_setbookmark,	1, 0, "SETBOOKMARK1"},
		{EDIT_SETBOOKMARK2,	edit_setbookmark,	2, 0, "SETBOOKMARK2"},
		{EDIT_SETBOOKMARK3,	edit_setbookmark,	3, 0, "SETBOOKMARK3"},
		{EDIT_SETBOOKMARK4,	edit_setbookmark,	4, 0, "SETBOOKMARK4"},
		{EDIT_SETBOOKMARK5,	edit_setbookmark,	5, 0, "SETBOOKMARK5"},
		{EDIT_SETBOOKMARK6,	edit_setbookmark,	6, 0, "SETBOOKMARK6"},
		{EDIT_SETBOOKMARK7,	edit_setbookmark,	7, 0, "SETBOOKMARK7"},
		{EDIT_SETBOOKMARK8,	edit_setbookmark,	8, 0, "SETBOOKMARK8"},
		{EDIT_SETBOOKMARK9,	edit_setbookmark,	9, 0, "SETBOOKMARK9"},

		{EDIT_GOTOBOOKMARK1,	edit_gotobookmark,	1, 0, "GOTOBOOKMARK1"},
		{EDIT_GOTOBOOKMARK2,	edit_gotobookmark,	2, 0, "GOTOBOOKMARK2"},
		{EDIT_GOTOBOOKMARK3,	edit_gotobookmark,	3, 0, "GOTOBOOKMARK3"},
		{EDIT_GOTOBOOKMARK4,	edit_gotobookmark,	4, 0, "GOTOBOOKMARK4"},
		{EDIT_GOTOBOOKMARK5,	edit_gotobookmark,	5, 0, "GOTOBOOKMARK5"},
		{EDIT_GOTOBOOKMARK6,	edit_gotobookmark,	6, 0, "GOTOBOOKMARK6"},
		{EDIT_GOTOBOOKMARK7,	edit_gotobookmark,	7, 0, "GOTOBOOKMARK7"},
		{EDIT_GOTOBOOKMARK8,	edit_gotobookmark,	8, 0, "GOTOBOOKMARK8"},
		{EDIT_GOTOBOOKMARK9,	edit_gotobookmark,	9, 0, "GOTOBOOKMARK9"},
		
		{EDIT_UPPERCASE,	edit_uppercase,		0, 1, "UPPERCASE"},
		{EDIT_FLIPCASE,		edit_flipcase,		0, 1, "FLIPCASE"},

    {EDIT_PRIVATE_COPY,     edit_private_copy,     0, 0, "COPY"},
		{EDIT_PRIVATE_PASTE,    edit_private_paste,    0, 1, "PASTE"},
		{EDIT_PRIVATE_CUT,      edit_private_cut,      0, 1, "CUT"},
		{EDIT_PRIVATE_EXCHANGE, edit_private_exchange, 0, 1, "EXCHANGE"},
		{EDIT_PRIVATE_ADD,      edit_private_add,      0, 0, "ADD"}
	};

void	do_edit_action(EDITOR * ed, EditActionDesc * action)
{
	if (!ed->read_only || !action->writes)	{
		action->action_func (ed, action->param);
		edit_update_parsing (ed, FALSE);
	}

}

void	edit_action (EDITOR * ed, int action)
{
	int i;
	if (!action) return;
	for (i = 0; i < sizeof (edit_actions) / sizeof (edit_actions [0]); i++)
		if (action == edit_actions [i].action)	{
			do_edit_action(ed, &edit_actions[i]);
			return;
		}
}

void	edit_action_by_name (EDITOR * ed, char * name)
{
	int i;
	if (!name || !name[0]) return;
	for (i = 1; i < sizeof (edit_actions) / sizeof (edit_actions [0]); i++)
		if ( !strcmp(name, edit_actions [i].name) )	{
			do_edit_action(ed, &edit_actions[i]);
			return;
		}
}

/* Typing */

void	edit_char_typed (EDITOR * ed, char c)
{
	BOOL b, blk;

	blk = ed->is_block;
	if (ed->is_block && ed->block_type == 0)	{
		edit_start_undo (ed);
		tbuf_delete_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2);
		edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
	}
	if (ed->is_block && ed->block_type != 0)	{
		if (ed->block_type < 0)	{
			tbuf_insert_string (ed->buf, ed->y1, ed->x1, &c, 1);
			edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
		} else	{
			tbuf_insert_string (ed->buf, ed->y2, ed->x2, &c, 1);
			edit_gotoxy (ed, ed->x2, ed->y2, FALSE, TRUE);
		}
	} else	if (ed -> insert_mode)	{
		tbuf_insert_string (ed->buf, ed->y, ed->x, &c, 1);
		edit_gotoxy (ed, ed->x, ed->y, FALSE, TRUE);
	} else	{
		b = tbuf_overwrite_char (ed->buf, ed->y, ed->x, c);
		if (!b) return;
		edit_gotoxy (ed, ed->x+1, ed->y, FALSE, TRUE);
	}
	if (blk) tbuf_finish_undo_block (ed->buf);
	edit_update_parsing (ed, FALSE);
}

void	edit_char (EDITOR * ed, WPARAM wparam)
{
	char c = (char) wparam;
	int action;

	action = lookup_ed_char (wparam);
	if (action)	{
		edit_action (ed, action);
		return;
	}
	if (!ed->read_only && (unsigned char) c >= ' ')
		edit_char_typed (ed, c);
}

void	edit_adjust_cursor (EDITOR * ed)
{
	int x, y, x1, y1, x2, y2;

	tbuf_query_bookmark (ed->buf, ed->cursor_bookmark, &x, &y);
	tbuf_query_bookmark (ed->buf, ed->block_bookmark1, &x1, &y1);
	tbuf_query_bookmark (ed->buf, ed->block_bookmark2, &x2, &y2);

	ed->x = x;
	ed->y = y;

	if (ed->is_block && ed->y1 == ed->y2 && ed->x1 == ed->x2)	{
		edit_invalidate_block (ed);
		ed->is_block = FALSE;
		ed->x = ed->y1;
		ed->y = ed->x1;
	}

	if (ed->cursor_visible)
		edit_put_cursor (ed);
	
	if (ed->y1 == y1 && ed->x1 != x1 && ed->is_block)
		edit_invalidate_line (ed, y1);

	if (ed->y2 == y2 && ed->x2 != x2 && ed->is_block)
		edit_invalidate_line (ed, y2);

	ed->y1 = y1;
	ed->x1 = x1;
	ed->y2 = y2;
	ed->x2 = x2;
}

/* ========================= clipboard operations ======================== */

int	my_strlen (char * s, int len)
{
	int i;
	for (i = 0; i < len; i++)
		if (!s[i]) return i;
	return len;
}

BOOL	do_cb_copy (EDITOR * ed)
{
	long len;
	HGLOBAL h;
	char * ptr, *p;

	if (!ed->is_block ||
	    ed->y1 > ed->y2 ||
	    ed->y1 == ed->y2 && ed->x1 >= ed->x2
	   )
		return FALSE;

	len = tbuf_stream_out_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2, 0, 0);
	if (! OpenClipboard (0))
		return FALSE;
	h = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, len+1);
	if (!h)	{
		CloseClipboard ();
		return FALSE;
	}
	ptr = p = GlobalLock (h);
	tbuf_stream_out_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2, copy_fun, (DWORD)&ptr);
	*ptr = 0;
	GlobalUnlock (h);
	EmptyClipboard ();
	SetClipboardData (CF_TEXT, h);
	h = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, sizeof (DWORD));
	if (h)	{
		ptr = GlobalLock (h);
		*(DWORD*)ptr = GetUserDefaultLCID ();
		GlobalUnlock (h);
		SetClipboardData (CF_LOCALE, h);
	}

	CloseClipboard ();
	call_notify (ed, EDIT_NOTIFY_CLIPBOARD, 0);
	return TRUE;
}

BOOL	do_cb_paste (EDITOR * ed)
{
	HANDLE h;
	char * ptr;
	int len;
	BOOL b;
	int x0, y0;

	if (!OpenClipboard (NULL))	return FALSE;
	if (! IsClipboardFormatAvailable (CF_TEXT))	{
		CloseClipboard ();
		return FALSE;
	}

	h = GetClipboardData (CF_TEXT);
	if (!h)	{
		CloseClipboard ();
		return FALSE;
	}
	edit_start_undo (ed);
	if (ed->is_block)
		if (ed->block_paste == 0)
			tbuf_delete_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2);
		else if (ed->block_paste < 0)
			edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
		else
			edit_gotoxy (ed, ed->x2, ed->y2, FALSE, TRUE);
	x0 = ed->x1; y0 = ed->y1;
	ptr = GlobalLock (h);
	len = my_strlen (ptr, GlobalSize (h));
	b = tbuf_stream_in_text (ed->buf, ptr, len, x0, y0);
	if (b)	{
		if (ed->paste_pos < 0)
			edit_gotoxy (ed, x0, y0, FALSE, TRUE);
		else if (ed->paste_pos > 0)
			edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
		else
			edit_set_block (ed, x0, y0, ed->x1, ed->y1);
	}
	tbuf_finish_undo_block (ed->buf);
	GlobalUnlock (h);
	CloseClipboard ();
	return b;
}

BOOL	do_cb_exchange (EDITOR * ed)
{
	long len;
	HGLOBAL h, h0;
	char * ptr;
	BOOL b;
	int x0, y0;

	if (!ed->is_block ||
	    ed->y1 > ed->y2 ||
	    ed->y1 == ed->y2 && ed->x1 >= ed->x2
	   )
		return FALSE;
	if (!OpenClipboard (NULL))	return FALSE;
	if (! IsClipboardFormatAvailable (CF_TEXT))	{
		CloseClipboard ();
		return do_cb_copy (ed);
	}
	h0 = GetClipboardData (CF_TEXT);
	if (!h0)	{
		CloseClipboard ();
		return do_cb_copy (ed);
	}

	len = tbuf_stream_out_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2, 0, 0);
	h = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, len+1);
	if (!h)	{
		CloseClipboard ();
		return FALSE;
	}
	ptr = GlobalLock (h);
	tbuf_stream_out_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2, copy_fun, (DWORD)&ptr);
	*ptr = 0;
	GlobalUnlock (h);
	ptr = GlobalLock (h0);
	len = my_strlen (ptr, GlobalSize (h0));
	edit_start_undo (ed);
	x0 = ed->x1; y0 = ed->y1;
	if (ed->is_block)
		tbuf_delete_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2);
	b = tbuf_stream_in_text (ed->buf, ptr, len, x0, y0);
	if (b)	{
		if (ed->paste_pos < 0)
			edit_gotoxy (ed, x0, y0, FALSE, TRUE);
		else if (ed->paste_pos > 0)
			edit_gotoxy (ed, ed->x1, ed->y1, FALSE, TRUE);
		else
			edit_set_block (ed, x0, y0, ed->x1, ed->y1);
	}
	tbuf_finish_undo_block (ed->buf);
	GlobalUnlock (h0);
	
	if (!b)	{
		GlobalFree (h);
		CloseClipboard ();
		return FALSE;
	}
	EmptyClipboard ();
	SetClipboardData (CF_TEXT, h);
	h = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, sizeof (DWORD));
	if (h)	{
		ptr = GlobalLock (h);
		*(DWORD*)ptr = GetUserDefaultLCID ();
		GlobalUnlock (h);
		SetClipboardData (CF_LOCALE, h);
	}
	CloseClipboard ();
	call_notify (ed, EDIT_NOTIFY_CLIPBOARD, 0);
	return TRUE;
}

BOOL	do_cb_add (EDITOR * ed)
{
	long len, len0;
	HGLOBAL h, h0;
	char * ptr, * ptr0;

	if (!ed->is_block ||
	    ed->y1 > ed->y2 ||
	    ed->y1 == ed->y2 && ed->x1 >= ed->x2
	   )
		return FALSE;
	if (! OpenClipboard (NULL))	return FALSE;
	if (! IsClipboardFormatAvailable (CF_TEXT))	{
		CloseClipboard ();
		return do_cb_copy (ed);
	}
	h0 = GetClipboardData (CF_TEXT);
	if (!h0)	{
		CloseClipboard ();
		return do_cb_copy (ed);
	}

	ptr0 = GlobalLock (h0);
	len0 = my_strlen (ptr0, GlobalSize (h0));
	len = tbuf_stream_out_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2, 0, 0);
	h = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, len0+len+1);
	if (!h)	{
		GlobalUnlock (h0);
		CloseClipboard ();
		return FALSE;
	}
	ptr  = GlobalLock (h);
	memcpy (ptr, ptr0, len0);
	ptr += len0;
	tbuf_stream_out_block (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, "\r\n", 2, copy_fun, (DWORD)&ptr);
	*ptr = 0;
	GlobalUnlock (h);
	GlobalUnlock (h0);
	EmptyClipboard ();
	SetClipboardData (CF_TEXT, h);
	h = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, sizeof (DWORD));
	if (h)	{
		ptr = GlobalLock (h);
		*(DWORD*)ptr = GetUserDefaultLCID ();
		GlobalUnlock (h);
		SetClipboardData (CF_LOCALE, h);
	}
	CloseClipboard ();
	call_notify (ed, EDIT_NOTIFY_CLIPBOARD, 0);
	return TRUE;
}

/* ========================= mouse messages processing =================== */

BOOL	about_to_drag_block = FALSE;
HCURSOR	cur_drag_cursor = NULL;

void	copy_cursor (HDC dc, int sy, int x, int y, HCURSOR cur)
{
	ICONINFO ic;
	BITMAP bmp;
	HBITMAP obmp;
	HDC bdc;

	GetIconInfo (cur, &ic);
	GetObject (ic.hbmMask, sizeof (bmp), &bmp);
	bdc = CreateCompatibleDC (dc);
	obmp = SelectObject (bdc, ic.hbmMask);
	x -= ic.xHotspot;
	y -= ic.yHotspot;
	BitBlt (dc, x, y, bmp.bmWidth, bmp.bmHeight/2, bdc, 0, 0, SRCCOPY);
	BitBlt (dc, x, y+sy, bmp.bmWidth, bmp.bmHeight/2, bdc, 0, bmp.bmHeight/2, SRCCOPY);
	SelectObject (bdc, obmp);
	DeleteDC (bdc);
}

HCURSOR	block_drag_cursor (EDITOR * ed, int xmouse, int ymouse)
{
#if 0
	int x1, y1, x2, y2, sx, sy, y;
	HDC dc, bdc;
	HBITMAP bmp, obmp;
	RECT rc;
	ICONINFO ic;
	HCURSOR cur;
#endif
	return LoadCursor (0, IDC_IBEAM);
#if 0	
	x1 = ed->x1;
	y1 = ed->y1;
	x2 = ed->x2;
	y2 = ed->y2;
	if (y1 < ed->y0)	{
		y1 = ed->y0;
		x1 = 0;
	} else
		x1 = LtoP (x1, y1);
	if (y2 > ed->y0+ed->sy)	{
		y2 = ed->y0+ed->sy+1;
		x2 = 0;
	} else
		x2 - LtoP (x2, y2);
	x1 -= ed->x0;
	x2 -= ed->y0;
	if (x1 < 0) x1 = 0;
	if (x2 < 0) x2 = 0;
	if (x1 > ed->sx)	{
		x1 = 0;
		y1 ++;
	}
	if (x2 > ed->sx+1) x2 = ed->sx+1;
	x1 *= ed->dx;
	x2 *= ed->dx;
	ymouse -= (y1-ed->y0) * ed->dy;

	dc = GetDC (0);
	bdc = CreateCompatibleDC (dc);
	ReleaseDC (0, dc);
	sx = ed->sx*ed->dx+1;
	sy = (y2-y1+1) * ed->dy+1;
	bmp = CreateCompatibleBitmap (bdc, sx, sy*2);
	obmp = SelectObject (bdc, bmp);
	rc.left = 0; rc.top = 0; rc.right = sx; rc.bottom = sy;
	FillRect (bdc, &rc, GetStockObject (WHITE_BRUSH));
	rc.top = sy; rc.bottom = sy * 2;
	FillRect (bdc, &rc, GetStockObject (BLACK_BRUSH));

	copy_cursor (bdc, sy, xmouse, ymouse, LoadCursor (0, IDC_ARROW));

	SelectObject (bdc, GetStockObject (WHITE_PEN));
	for (y = y1; y <= y2; y++)	{
		rc.left = 0;
		rc.top = (y-y1)*ed->dy+sy;
		rc.right = sx-1;
		rc.bottom = rc.top + ed->dy;
		if (y == y1) rc.left = x1;
		if (y == y2) rc.right = x2;
		MoveToEx (bdc, rc.left, rc.top, 0);
		LineTo (bdc, rc.right, rc.top);
		LineTo (bdc, rc.right, rc.bottom);
		LineTo (bdc, rc.left, rc.bottom);
		LineTo (bdc, rc.left, rc.top);
	}
	cur = LoadCursor (0, IDC_ARROW);
	GetIconInfo (cur, &ic);

//	DrawIcon (bdc, xmouse-ic.xHotspot, ymouse-ic.yHotspot+sy, cur);

	SelectObject (bdc, obmp);
	DeleteDC (bdc);
	ic.fIcon = FALSE;
	ic.xHotspot = xmouse;
	ic.yHotspot = ymouse;
	ic.hbmMask = bmp;
	ic.hbmColor = 0;
	cur = CreateIconIndirect (&ic);
	return cur;
#endif
}

void	edit_screen_to_char (EDITOR * ed, int * x, int * y)
{
	*y = *y / ed->dy + ed->y0;
	if (*y >= ed->y0 + ed->sy)
		*y = ed->y0 + ed->sy - 1;
	if (*y >= ed->nlines) *y = ed->nlines-1;
	*x = PtoL ((*x-ed->left+ed->dx/2) / ed->dx + ed->x0, *y);
}

void	edit_lbuttondown (EDITOR * ed, int xm, int ym, BOOL shift)
{
	int x, y, x1, y1, x2, y2;
	
	x = xm; y = ym;
	edit_screen_to_char (ed, &x, &y);
	if (xm < ed->left)	{
		if (y == ed->nlines-1)	{
			x1 = 0; y1 = y2 = y; x2 = tbuf_get_line_length (ed->buf, y);
		} else	{
			x1 = x2 = 0; y1 = y; y2 = y+1;
		}
		if (shift)	{
			if (y >= ed->y)
				edit_gotoxy (ed, x2, y2, TRUE, TRUE);
			else
				edit_gotoxy (ed, x1, y1, TRUE, TRUE);
		} else
			edit_set_block (ed, x1, y1, x2, y2);
	} else
		edit_gotoxy (ed, x, y, shift, TRUE);
}

#define MOUSE_SELECT_TIMER 1
#define MOUSE_SCROLL_TIMER 2

UINT	TimerId = 0;
UINT	timer_type = 0;
int	y_timer_delay;
int	y_timer_count;
int	y_scroll_speed;
int	x_timer_delay;
int	x_timer_count;
int	x_scroll_speed;
int	mouse_x, mouse_y;
int	mouse_x0, mouse_y0;
int	drag_x, drag_y;

void	calc_y_scroll_speed (EDITOR * ed, int y, int * speed, int * delay)
{
	if (y < ed->dy)	{
		* speed = 1;
		* delay = 4;
	} else if (y < 2 * ed->dy)	{
		* speed = 1;
		* delay = 1;
	} else if (y < 3 * ed->dy)	{
		* speed = 10;
		* delay = 1;
	} else	{
		* speed = 20;
		* delay = 1;
	}
}

void	calc_x_scroll_speed (EDITOR * ed, int x, int * speed, int * delay)
{
	if (x < 2 * ed->dx)	{
		* speed = 1;
		* delay = 4;
	} else if (x < 4 * ed->dx)	{
		* speed = 1;
		* delay = 1;
	} else	{
		* speed = 2;
		* delay = 1;
	}
}

void	edit_mouse_select (EDITOR * ed, int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	y_timer_delay = x_timer_delay = 0;

	if (about_to_drag_block)	{
		if (cur_drag_cursor)
			SetCursor (cur_drag_cursor);
		cur_drag_cursor = 0;
	}
	
	if (y < 0)	{
		calc_y_scroll_speed (ed, -y, &y_scroll_speed, &y_timer_delay);
		y_scroll_speed = -y_scroll_speed;
		y = 0;
	} else if (y >= ed->ypage * ed->dy)
		calc_y_scroll_speed (ed, y - ed->ypage * ed->dy, &y_scroll_speed, &y_timer_delay);

	if (x < ed->left)	{
		calc_x_scroll_speed (ed, -x, &x_scroll_speed, &x_timer_delay);
		x_scroll_speed = -x_scroll_speed;
		x = 0;
	} else if (x >= ed->xpage * ed->dx)
		calc_x_scroll_speed (ed, x - ed->xpage * ed->dx, &x_scroll_speed, &x_timer_delay);
	
	if (x_timer_delay || y_timer_delay)	{
		if (!TimerId)	{
			TimerId = SetTimer (ed->hwnd, 1, 100, 0);
			timer_type = MOUSE_SELECT_TIMER;
			y_timer_count = y_timer_delay;
			x_timer_count = x_timer_delay;
		}
	} else	{
		if (TimerId)	{
			KillTimer (ed->hwnd, 1);
			timer_type = 0;
			TimerId = 0;
		}
		edit_screen_to_char (ed, &x, &y);
		if (y < ed->y0 + ed->sy - 1)
			if (about_to_drag_block)	{
				drag_x = x;
				drag_y = y;
			} else
				edit_gotoxy (ed, x, y, TRUE, TRUE);
	}
}

void	edit_lbuttonup (EDITOR * ed, int x, int y)
{
	if (about_to_drag_block)	{
		tbuf_move_text (ed->buf, ed->x1, ed->y1, ed->x2, ed->y2, drag_x, drag_y);
		about_to_drag_block = FALSE;
	}
}

void	edit_mouse_select_timer (EDITOR * ed)
{
	int x0, y0;
	int x, y;

	BOOL need_gotoxy = FALSE;

	if (y_timer_delay && --y_timer_count == 0)	{
		need_gotoxy = TRUE;
		y_timer_count = y_timer_delay;
		y0 = ed->y0 + y_scroll_speed;
		if (y0 > ed->nlines - ed->ypage)
			y0 = ed->nlines - ed->ypage;
		if (y0 < 0) y0 = 0;
		if (y0 != ed->y0)	{
			ed->y0 = y0;
			InvalidateRect (ed->hwnd, 0, 0);
		}
	}
	if (x_timer_delay && --x_timer_count == 0)	{
		need_gotoxy = TRUE;
		x_timer_count = x_timer_delay;
		x0 = ed->x0 + x_scroll_speed;
//		if (x0 > ed->nlines - ed->ypage)
//			y0 = ed->nlines - ed->ypage;
		if (x0 < 0) x0 = 0;
		if (x0 != ed->x0)	{
			ed->x0 = x0;
			InvalidateRect (ed->hwnd, 0, 0);
		}
	}
	if (need_gotoxy)	{
		x = mouse_x;
		if (x_timer_delay)
			x = x_scroll_speed > 0 ? (ed->rc.right-1) : 0;
		y = mouse_y;
		if (y_timer_delay)
			y = y_scroll_speed > 0 ? (ed->rc.bottom-1) : 0;
		edit_screen_to_char (ed, &x, &y);
		edit_gotoxy (ed, x, y, !about_to_drag_block, TRUE);
	}
}

/* -------------------- Middle mouse button scrolling ------------------ */

void	edit_mbuttondown (EDITOR * ed, int xm, int ym, BOOL shift)
{
	mouse_x0 = ed->mouse_scroll_x = xm;
	mouse_y0 = ed->mouse_scroll_y = ym;
	ed->in_mouse_scrolling = TRUE;
	InvalidateRect (ed->hwnd, 0, 0);
	SetCursor (edit_get_move_cursor (0, 0));
}

void	stop_scrolling (EDITOR * ed)
{
	if (TimerId)	{
		KillTimer (ed->hwnd, 1);
		timer_type = 0;
		TimerId = 0;
	}
	if (ed->in_mouse_scrolling)	{
		ed->in_mouse_scrolling = FALSE;
		InvalidateRect (ed->hwnd, 0, 0);
	}
}

void	calc_y_middle_scroll_speed (EDITOR * ed, int dy, int * speed, int * delay)
{
	static int speed_array [] = {0, 1, 2, 3, 4, 6, 8, 10, 12, 16, 20, 24, 32, 40};
	int sign = dy >= 0 ? 1 : -1;
	if (dy < 0) dy = - dy;
	dy = (dy + ed->dy/2) / ed->dy;
	if (dy >= sizeof (speed_array) / sizeof (int))
		* speed = 100 * sign;
	else
		* speed = speed_array [dy] * sign;

	if (* speed == 0)
		* delay = 0;
	else if (dy == 1)
		* delay = 4;
	else if (dy == 2)
		*delay = 2;
	else
		*delay = 1;
}

void	calc_x_middle_scroll_speed (EDITOR * ed, int dx, int * speed, int * delay)
{
	static int speed_array [] = {0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 8, 10};
	int sign = dx >= 0 ? 1 : -1;
	if (dx < 0) dx = - dx;
	dx /= ed->dx;
	if (dx >= sizeof (speed_array) / sizeof (int))
		* speed = 10 * sign;
	else
		* speed = speed_array [dx] * sign;
	
	if (* speed == 0)
		* delay = 0;
	else if (dx <= 2)
		* delay = 4;
	else if (dx <= 4)
		*delay = 2;
	else
		*delay = 1;
}

void	edit_mouse_scroll (EDITOR * ed, int x, int y)
{
	mouse_x = x;
	mouse_y = y;

	calc_y_middle_scroll_speed (ed, y-mouse_y0, &y_scroll_speed, &y_timer_delay);
	calc_x_middle_scroll_speed (ed, x-mouse_x0, &x_scroll_speed, &x_timer_delay);

	SetCursor (edit_get_move_cursor (x_scroll_speed, y_scroll_speed));

	if (x_timer_delay || y_timer_delay)	{
		if (!TimerId)	{
			TimerId = SetTimer (ed->hwnd, 1, 100, 0);
			timer_type = MOUSE_SCROLL_TIMER;
			y_timer_count = y_timer_delay;
			x_timer_count = x_timer_delay;
		} else	{
			y_timer_count = min (y_timer_count, y_timer_delay);
			x_timer_count = min (x_timer_count, x_timer_delay);
		}
	} else	{
		if (TimerId)	{
			KillTimer (ed->hwnd, 1);
			timer_type = 0;
			TimerId = 0;
		}
	}
}

void	edit_mouse_scroll_timer (EDITOR * ed)
{
	int x0, y0;

	if (y_timer_delay && --y_timer_count <= 0)	{
		y_timer_count = y_timer_delay;
		y0 = ed->y0 + y_scroll_speed;
		if (y0 > ed->yrange)
			y0 = ed->yrange;
		if (y0 < 0) y0 = 0;
		if (y0 != ed->y0)	{
			ed->y0 = y0;
			InvalidateRect (ed->hwnd, 0, 0);
			SetScrollPos (ed->hwnd, SB_VERT, ed->y0, 1);
			test_scrollbars (ed);
		}
	}
	if (x_timer_delay && --x_timer_count <= 0)	{
		x_timer_count = x_timer_delay;
		x0 = ed->x0 + x_scroll_speed;
		if (x0 > ed->xrange)
			x0 = ed->xrange;
		if (x0 < 0) x0 = 0;
		if (x0 != ed->x0)	{
			ed->x0 = x0;
			InvalidateRect (ed->hwnd, 0, 0);
			SetScrollPos (ed->hwnd, SB_HORZ, ed->y0, 1);
			test_scrollbars (ed);
		}
	}
}

/* ========================= processing of text buffer notifications ===== */

void	edit_line_changed (void * edit, int i)
{
	EDITOR * ed = (EDITOR*) edit;
	edit_adjust_cursor (ed);
	edit_invalidate_line (ed, i);
	ed->ncolumns = tbuf_get_ncolumns (ed->buf);
	test_scrollbars (ed);
}

void	edit_lines_changed (void * edit, int i, int n)
{
	EDITOR * ed = (EDITOR*) edit;
	edit_adjust_cursor (ed);
	edit_invalidate_lines (ed, i, i+n-1);
	ed->ncolumns = tbuf_get_ncolumns (ed->buf);
	test_scrollbars (ed);
}

void	edit_lines_deleted (void * edit, int i, int n)
{
	EDITOR * ed = (EDITOR*) edit;
	ed->nlines   = tbuf_get_nlines (ed->buf);
	ed->ncolumns = tbuf_get_ncolumns (ed->buf);
	if (i+n <= ed->y0)
		ed->y0 -= n;
	else	{
		if (i < ed->y0)	{
			n -= ed->y0 - i;
			ed->y0 = i;
		}
		if (ed->y0 && ed->y0 + ed->sy > ed->nlines)	{
			if (ed->y0 > n)	{
				edit_invalidate_lines (ed, ed->y0, i+n-1);
				ed->y0 -= n;
			} else	{
				edit_invalidate_lines (ed, ed->y0, ed->y0+ed->sy-1);
				ed->y0 = 0;
			}
		} else
			edit_invalidate_lines (ed, i, ed->y0+ed->sy-1);
	}
	edit_adjust_cursor (ed);
	test_scrollbars (ed);
}

void	edit_lines_inserted (void * edit, int i, int n)
{
	EDITOR * ed = (EDITOR*) edit;
	ed->nlines   = tbuf_get_nlines (ed->buf);
	ed->ncolumns = tbuf_get_ncolumns (ed->buf);
	if (i <= ed->y0)
		ed->y0 += n;
	else if (i < ed->y0 + ed->sy)
		edit_invalidate_lines (ed, i, ed->y0+ed->sy-1);
	edit_adjust_cursor (ed);
	test_scrollbars (ed);
}

void	edit_dirty_changed (void * edit, BOOL dirty, BOOL forced)
{
	EDITOR * ed = (EDITOR*) edit;

	call_notify (ed, EDIT_NOTIFY_DIRTY, forced);
}

void	edit_undo_changed (void * edit, BOOL undo)
{
	EDITOR * ed = (EDITOR*) edit;

	call_notify (ed, EDIT_NOTIFY_UNDO, undo);
}

void	edit_tab_changed (void * edit)
{
	EDITOR * ed = (EDITOR*) edit;

	ed->tab_size = tbuf_tab_size (ed->buf);
	ed->ncolumns = tbuf_get_ncolumns (ed->buf);
	InvalidateRect (ed->hwnd, 0, 0);
	test_scrollbars (ed);
}

BOOL	edit_trunc_undo (void * edit, BOOL drop, BOOL dropping_mode)
{
	EDITOR * ed = (EDITOR*) edit;

	return (BOOL) call_notify (ed, drop ? EDIT_NOTIFY_DROPUNDO : EDIT_NOTIFY_TRUNCUNDO, dropping_mode);
}

void	edit_redraw_lines (void * edit, int start, int end)
{
	edit_invalidate_lines ((EDITOR*) edit, start, end);
}

EDIT_CALLBACKS EdCallBack = {
	edit_line_changed,
	edit_lines_changed,
	edit_lines_deleted,
	edit_lines_inserted,
	edit_dirty_changed,
	edit_undo_changed,
	edit_tab_changed,
	edit_trunc_undo,
	edit_redraw_lines
};

/* ------------------------- scrolling ----------------------------------- */

void	set_scroll (HWND hwnd, int scr, int range, int pos, int page)
{
//	int n, m;
	SCROLLINFO si;
	range += page;
	si.cbSize = sizeof (si);
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	GetScrollInfo (hwnd, scr, &si);
	if (pos == si.nPos && range == si.nMax && page == (int) si.nPage) return;
	si.nPos = pos;
	si.nMax = range;
	si.nPage = page;
	SetScrollInfo (hwnd, scr, &si, 1);
#if 0
	memset (&si, 0, sizeof (si));
	si.cbSize = sizeof (si);
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	n = GetScrollInfo (hwnd, scr, &si);
	{ char s [1000];
	sprintf (s, "%x (%x) pos=%x (%x) max=%x (%x) page=%x (%x)", m, n,
		pos, si.nPos, range, si.nMax, page, si.nPage);
	SetWindowText (StatusBar, s);
	}
#endif
}

void	test_scrollbars (EDITOR * ed)
{
	long sx, sy;
	int newy;
	BOOL need_h, need_v;
	int xsize, ysize;

	xsize = ed->rc.right-ed->left;
	ysize = ed->rc.bottom;
	
	if (ed->vscroll_present)
		xsize += scroll_width;
	if (ed->hscroll_present)
		ysize += scroll_height;

	sx = ed->ncolumns * ed->dx;
	sy = ed->nlines   * ed->dy;

	newy = ysize - scroll_height;
	need_h = ed->x0 || sx > xsize;
	if (need_h) ysize = newy;
	need_v = ed->y0 || sy > ysize;
	if (need_v) xsize -= scroll_width;
	need_h = ed->x0 || sx > xsize;
	if (need_h) ysize = newy;

	xsize /= ed->dx;
	ysize /= ed->dy;
	
	ed->xrange = ed->ncolumns > xsize ? ed->ncolumns - xsize : 0;
	ed->yrange = ed->nlines   > ysize ? ed->nlines   - ysize : 0;

	if (ed->x0 > ed->xrange) ed->xrange = ed->x0;
	if (ed->y0 > ed->yrange)	{
		ed -> y0 = ed->yrange;
		InvalidateRect (ed->hwnd, 0, 0);
	}

	if (ed->hscroll_present != need_h)	{
		set_scroll (ed->hwnd, SB_HORZ, need_h ? ed->xrange : 0, ed->x0, ed->sx);
		ed->hscroll_present=need_h;
		ShowScrollBar (ed->hwnd, SB_HORZ, need_h);
	} else if (ed -> hscroll_present)
		set_scroll (ed->hwnd, SB_HORZ, ed->xrange, ed->x0, ed->sx);

	if (ed->vscroll_present != need_v)	{
		set_scroll (ed->hwnd, SB_VERT, need_v ? ed->yrange : 0, ed->y0, ed->sy);
		ed->vscroll_present=need_v;
		ShowScrollBar (ed->hwnd, SB_VERT, need_v);
	} else if (ed -> vscroll_present)
		set_scroll (ed->hwnd, SB_VERT, ed->yrange, ed->y0, ed->sy);

	edit_put_cursor (ed);
}

void	edit_scroll_process (EDITOR * ed, int code, WPARAM wparam, LPARAM lparam, int*pos)
{
	int high, opos;
	int d, page, cmd;
	int curpos;
	SCROLLINFO si;

	opos = *pos;
	high = code == SB_HORZ ? ed->xrange : ed->yrange;
	d = 1;
	cmd = LOWORD (wparam);
	si.cbSize = sizeof (si);
	si.fMask = SIF_ALL;
	GetScrollInfo (ed->hwnd, code, &si);
	curpos = si.nTrackPos;

	switch (cmd)	{
	case SB_LINEDOWN:
		++ *pos;
		break;
	case SB_LINEUP:
		-- *pos;
		break;
	case SB_PAGEUP:
		d = -1;
	case SB_PAGEDOWN:
		if (code == SB_HORZ)
			page = ed->xpage;
		else
			page = ed->ypage;
		if (page <= 0) page = 1;
			*pos += page*d;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		*pos = curpos;
		break;
	case SB_TOP:
		*pos=0;
		break;
	case SB_ENDSCROLL:
		if (code == SB_VERT && ! ed -> vscroll_present ||
		    code == SB_HORZ && ! ed -> hscroll_present)
			InvalidateRect (ed->hwnd, 0, 0);
		return;
	}
	high = code == SB_HORZ ? ed->xrange : ed->yrange;
	if (*pos<0)*pos=0;
	if (*pos>=high) *pos=high;
	if (*pos == opos) return;
	SetScrollPos (ed->hwnd, code, *pos, 1);
	InvalidateRect (ed->hwnd, 0, 0);
}

/* ------------------------- mouse wheel processing ---------------------- */

void	process_wheel (EDITOR * ed)
{
	int num;
	int old;

	if (ed->wheel_sum >= 0)	{
		if (ed->wheel_sum < WHEEL_DELTA) return;
		num = ed->wheel_sum / WHEEL_DELTA;
		ed->wheel_sum -= num * WHEEL_DELTA;
	} else	{
		if (ed->wheel_sum < -WHEEL_DELTA) return;
		num = - (- ed->wheel_sum / WHEEL_DELTA);
		ed->wheel_sum -= num * WHEEL_DELTA;
	}
	old = ed->y0;
	ed->y0 -= num;
	if (ed->y0 < 0) ed->y0 = 0;
	if (ed->y0 >= ed->yrange) ed->y0 = ed->yrange;
	if (ed->y0 == old) return;
	SetScrollPos (ed->hwnd, SB_VERT, ed->y0, 1);
	InvalidateRect (ed->hwnd, 0, 0);
	test_scrollbars (ed);
}

/* ------------------------- creation of new editor ---------------------- */

EDITOR * create_editor (TBUF * buf, HWND hwnd)
{
	EDITOR * ed = malloc (sizeof (EDITOR));

	if (! ed) return NULL;
	memset (ed, 0, sizeof (EDITOR));
	ed -> magic = EDITOR_MAGIC;
	ed -> buf = buf;
	ed -> font = GetStockObject (SYSTEM_FIXED_FONT);
	measure_font (ed -> font, &ed -> dx, &ed -> dy);
	ed -> insert_mode = FALSE;
	ed -> tab_size = tbuf_tab_size (buf);
	ed -> xshift_size = 1;
	ed -> high_y = -1;
	ed -> nlines = tbuf_get_nlines (buf);
	ed -> ncolumns = tbuf_get_ncolumns (buf);
	ed -> hwnd = hwnd;
	SetWindowLong (hwnd, 0, (LONG) ed);
	ed->cursor_bookmark = tbuf_add_bookmark (buf, ed->x, ed->y, TRUE);
	ed->block_bookmark1 = tbuf_add_bookmark (buf, ed->x1, ed->y1, TRUE);
	ed->block_bookmark2 = tbuf_add_bookmark (buf, ed->x2, ed->y2, TRUE);
	ShowScrollBar (hwnd, SB_VERT, 0);
	ShowScrollBar (hwnd, SB_HORZ, 0);
	ed->hscroll_present = ed->vscroll_present = FALSE;

	ed->left = ed->dx/2;
	ed->show_bookmarks = FALSE;

	ed -> edit_code = tbuf_register_edit (buf, ed, &EdCallBack);

	ed->bs_joins_lines = TRUE;
	ed->del_joins_lines = TRUE;
	ed->cr_splits_line = TRUE;
	ed->tab_types = TRUE;
	ed->tab_as_spaces = FALSE;
	ed->smart_tabs = FALSE;
	ed->cr_indents = FALSE;

	ed->macro_toggle = FALSE;

  ed->clipboard = NULL;

	if (!ed -> edit_code) return NULL;
	return ed;
}

void	delete_editor (EDITOR * ed)
{
	SetWindowLong (ed->hwnd, 0, 0);
	tbuf_set_current_edit (ed->buf, 0);
  free (ed->clipboard);
	tbuf_delete_bookmark (ed->buf, ed->cursor_bookmark);
	tbuf_delete_bookmark (ed->buf, ed->block_bookmark1);
	tbuf_delete_bookmark (ed->buf, ed->block_bookmark2);
	tbuf_unregister_edit (ed->buf, ed);
	free (ed);
}

void	edit_set_buffer (EDITOR * ed, TBUF * buf)
{
	tbuf_set_current_edit (ed->buf, 0);
	tbuf_delete_bookmark (ed->buf, ed->cursor_bookmark);
	tbuf_delete_bookmark (ed->buf, ed->block_bookmark1);
	tbuf_delete_bookmark (ed->buf, ed->block_bookmark2);
	tbuf_unregister_edit (ed->buf, ed);
	ed->buf = buf;
	ed->edit_code = tbuf_register_edit (buf, ed, &EdCallBack);
	ed -> tab_size = tbuf_tab_size (buf);
	ed -> xshift_size = 1;
	ed -> nlines = tbuf_get_nlines (buf);
	ed -> ncolumns = tbuf_get_ncolumns (buf);
	edit_set_block (ed, ed->x1, ed->y1, ed->x2, ed->y2);
	ed->cursor_bookmark = tbuf_add_bookmark (buf, ed->x, ed->y, TRUE);
	ed->block_bookmark1 = tbuf_add_bookmark (buf, ed->x1, ed->y1, TRUE);
	ed->block_bookmark2 = tbuf_add_bookmark (buf, ed->x2, ed->y2, TRUE);
	InvalidateRect (ed->hwnd, 0, 0);
}

void	edit_calc_left (EDITOR * ed)
{
	ed->left = ed->show_bookmarks ? ed->dx*3 : ed->dx/2;
}

void	edit_recalc_dimentions (EDITOR * ed)
{
	ed->sx = (ed->rc.right - ed->left + ed->dx-1) / ed->dx;
	ed->sy = (ed->rc.bottom + ed->dy-1) / ed->dy;
	ed->xpage = (ed->rc.right - ed->left) / ed->dx;
	ed->ypage = ed->rc.bottom / ed->dy;
	edit_calc_left (ed);
}

void	edit_set_font (EDITOR * ed, HFONT font)
{
	if (font == ed->font) return;
	ed->font = font;
	measure_font (ed -> font, &ed -> dx, &ed -> dy);
	edit_recalc_dimentions (ed);
	test_scrollbars (ed);
	InvalidateRect (ed->hwnd, 0, 0);
	edit_update_caret (ed);
}

void	edit_set_param (EDITOR * ed, EDITPARAM param, DWORD value)
{
	switch (param)	{
	case BS_JOINS_LINES:
		ed->bs_joins_lines = (BOOL) value; break;
	case DEL_JOINS_LINES:
		ed->del_joins_lines = (BOOL) value; break;
	case CR_SPLITS_LINE:
		ed->cr_splits_line = (BOOL) value; break;
	case TAB_TYPES:
		ed->tab_types = (BOOL) value; break;
	case TAB_AS_SPACES:
		ed->tab_as_spaces = (BOOL) value; break;
	case SMART_TABS:
		ed->smart_tabs = (BOOL) value; break;
	case CR_INDENTS:
		ed->cr_indents = (BOOL) value; break;
	case EDITOR_FONT:
		edit_set_font (ed, (HFONT) value); break;
	case EDITOR_SHOW_TABS:
		if (ed->show_tabs != (BOOL) value)
			InvalidateRect (ed->hwnd, 0, 0);
		ed->show_tabs = (BOOL) value;
		break;
	case EDITOR_SHOW_RIGHT_MARGIN:
		if (ed->show_right_margin != (BOOL) value)
			InvalidateRect (ed->hwnd, 0, 0);
		ed->show_right_margin = (BOOL) value;
		break;
	case EDITOR_SHOW_LEFT_MARGIN:
		if (ed->show_left_margin != (BOOL) value)
			InvalidateRect (ed->hwnd, 0, 0);
		ed->show_left_margin = (BOOL) value;
		break;
	case EDITOR_RIGHT_MARGIN:
		if (ed->right_margin != (int) value)
			InvalidateRect (ed->hwnd, 0, 0);
		ed->right_margin = (int) value;
		break;
	case EDITOR_SHOW_BOOKMARKS:
		if (ed->show_bookmarks != (BOOL) value)	{
			ed->show_bookmarks = (BOOL) value;
			edit_recalc_dimentions (ed);
			test_scrollbars (ed);
			InvalidateRect (ed->hwnd, 0, 0);
			edit_update_caret (ed);
		}
		break;
	case EDITOR_PASTE_POS:
		ed->paste_pos = (int) (long) value;
		break;
	case EDITOR_BLOCK_TYPE:
		ed->block_type = (int) (long) value;
		break;
	case EDITOR_BLOCK_PASTE:
		ed->block_paste = (int) (long) value;
		break;
	case EDITOR_TAB_SIZE:
		tbuf_set_tab_size (ed->buf, (int) (long) value);
		break;
	}
}

BOOL	edit_beam_cursor (EDITOR * ed, int x, int y)
{
	if (x >= ed->rc.right)		return FALSE;
	if (y >= ed->rc.bottom)		return FALSE;
	if (x < ed->left)		return FALSE;
	y = y / ed->dy + ed->y0;
	if (y >= ed->y0 + ed->sy)	return FALSE;
	if (y >= ed->nlines)		return FALSE;
	return TRUE;
}


/* ------------------------- main editor window proc --------------------- */

LRESULT CALLBACK EditWinProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HDC dc;
	PAINTSTRUCT ps;
	CREATESTRUCT * cs;
	EDITOR * ed = (EDITOR*) GetWindowLong (hwnd, 0);
	TBUF * buf;
	MSG m;
	int action;
	SHORT delta;
	char * macrostr;

	if (msg == WM_CREATE)	{
		cs = (CREATESTRUCT*) lparam;
		buf = (TBUF*) cs -> lpCreateParams;
                if (! create_editor (buf, hwnd))
                        return (LRESULT) -1;
		return 0;
	}
	if (!ed) return DefWindowProc (hwnd, msg, wparam, lparam);

	switch (msg)	{
	case WM_ERASEBKGND:
		return 1;
	case WM_PAINT:
		dc = BeginPaint (hwnd, &ps);
		draw_editor (ed, dc, &ps.rcPaint);
		//draw_editor_bmp (ed, dc, &ps.rcPaint);
		EndPaint (hwnd, &ps);
		return 0;
	case WM_SIZE:
		GetClientRect (hwnd, &ed->rc);
		edit_recalc_dimentions (ed);
		if (ed->wmsize_count) return 0;
		ed->wmsize_count ++;
		test_scrollbars (ed);
		--ed->wmsize_count;
		return 0;
	case WM_KEYDOWN:
		ed -> wheel_sum = 0;

		if ( wparam == VK_SHIFT || wparam == VK_CONTROL || wparam == VK_MENU )
			return 0;

		if ( wparam == VK_F1 ) {
			if ( ed->macroset->togle && !ed->macro_toggle )
				ed->macro_toggle = TRUE;
			else {
				PostMessage(GetParent(ed->hwnd), WM_COMMAND, IDM_HELPKEYWORD, 0);
				ed->macro_toggle = FALSE;
			}
			return 0;
		}

		if ( !ed->macroset->togle || ed->macro_toggle ) {

			macrostr = lookup_macro_key(ed, wparam);
			if (macrostr) {
				while (PeekMessage (&m, hwnd, WM_CHAR, WM_CHAR, PM_REMOVE));
				while (PeekMessage (&m, hwnd, WM_SYSCHAR, WM_SYSCHAR, PM_REMOVE));

				edit_play_macro(ed, macrostr);
				ed->macro_toggle = FALSE;
				return 0;
			}
			if ( ed->macroset->togle ) return 0;
		}

		action = lookup_ed_key (wparam);
		if (action)	{
			while (PeekMessage (&m, hwnd, WM_CHAR, WM_CHAR, PM_REMOVE));
			while (PeekMessage (&m, hwnd, WM_SYSCHAR, WM_SYSCHAR, PM_REMOVE));

			edit_action (ed, action);

			ed->macro_toggle = FALSE;
		}
		return 0;
	case WM_SYSKEYDOWN:
		ed -> wheel_sum = 0;
		ed -> macro_toggle = FALSE;
		if (wparam == VK_MENU) break;
		action = lookup_ed_key (wparam);
		edit_action (ed, action);
		if (action)	{
			while (PeekMessage (&m, hwnd, WM_CHAR, WM_CHAR, PM_REMOVE));
			while (PeekMessage (&m, hwnd, WM_SYSCHAR, WM_SYSCHAR, PM_REMOVE));
		}
		break;
	case WM_CHAR:
		ed -> wheel_sum = 0;
		if ( !ed->macroset->togle || ed->macro_toggle ) {
			ed -> macro_toggle = FALSE;
			macrostr = LookupMacro(ed->macroset, (int)wparam, FVIRTKEY );
			if ( macrostr ) {
				edit_play_macro (ed, macrostr);
				return 0;
			}
		} 
		edit_char (ed, wparam);
		return 0;
	case WM_GETDLGCODE:
		return DLGC_WANTARROWS | DLGC_WANTTAB | DLGC_WANTALLKEYS | DLGC_WANTCHARS;
	case WM_LBUTTONDOWN:
		ed -> wheel_sum = 0;
		edit_lbuttondown (ed, (SHORT) LOWORD (lparam), (SHORT) HIWORD (lparam), (wparam & MK_SHIFT)!=0);
		SetCapture (ed->hwnd);
		return 0;
	case WM_LBUTTONUP:
		ed -> wheel_sum = 0;
		ReleaseCapture ();
		if (TimerId && timer_type == MOUSE_SELECT_TIMER) {
			KillTimer (ed->hwnd, 1);
			TimerId = 0;
			timer_type = 0;
		}
		edit_lbuttonup (ed, (SHORT) LOWORD (lparam), (SHORT) HIWORD (lparam));
		return 0;
	case WM_MBUTTONDOWN:
		ed -> wheel_sum = 0;
		edit_mbuttondown (ed, (SHORT) LOWORD (lparam), (SHORT) HIWORD (lparam), (wparam & MK_SHIFT)!=0);
		SetCapture (ed->hwnd);
		return 0;
	case WM_MBUTTONUP:
		ed -> wheel_sum = 0;
		ReleaseCapture ();
		stop_scrolling (ed);
		return 0;
	case WM_MOUSEWHEEL:
		delta = HIWORD (wparam);
		ed -> wheel_sum += delta;
		process_wheel (ed);
		return 0;
	case WM_MOUSEMOVE:
		if (wparam & MK_LBUTTON && GetCapture () == hwnd)
			edit_mouse_select (ed, (SHORT) LOWORD (lparam), (SHORT) HIWORD (lparam));
		else if (wparam & MK_MBUTTON && GetCapture () == hwnd)
			edit_mouse_scroll (ed, (SHORT) LOWORD (lparam), (SHORT) HIWORD (lparam));
		else
			SetCursor (LoadCursor (NULL, edit_beam_cursor (ed, LOWORD (lparam), HIWORD (lparam)) ? IDC_IBEAM : IDC_ARROW));
		return 0;
	case WM_TIMER:
		if (timer_type == MOUSE_SELECT_TIMER)
			edit_mouse_select_timer (ed);
		else if (timer_type == MOUSE_SCROLL_TIMER)
			edit_mouse_scroll_timer (ed);
		return 0;
	case WM_HSCROLL:
		ed -> wheel_sum = 0;
		edit_scroll_process (ed, SB_HORZ, wparam, lparam, &ed->x0);
		test_scrollbars (ed);
		return 0;
	case WM_VSCROLL:
		ed -> wheel_sum = 0;
		edit_scroll_process (ed, SB_VERT, wparam, lparam, &ed->y0);
		test_scrollbars (ed);
		return 0;
	case WM_SETFOCUS:
		ed -> wheel_sum = 0;
		ed -> macro_toggle = FALSE;
		tbuf_set_current_edit (ed->buf, ed->edit_code);
		edit_create_cursor (ed);
		call_notify (ed, EDIT_NOTIFY_FOCUS, 0);
		break;
	case WM_KILLFOCUS:
		ed -> macro_toggle = FALSE;
		stop_scrolling (ed);
		tbuf_set_current_edit (ed->buf, 0);
		edit_hide_caret (ed);
		DestroyCaret ();
		call_notify (ed, EDIT_NOTIFY_FOCUS, 0);
		break;
	case WM_SETCURSOR:
		if (LOWORD (lparam) != HTCLIENT)
			SetCursor (LoadCursor (0, IDC_ARROW));
		return 1;
	case WM_DESTROY:
		stop_scrolling (ed);
		delete_editor (ed);
		break;
	}
	return DefWindowProc (hwnd, msg, wparam, lparam);
}

/*--------------------------------------------------------------------------- */
/* --- External interface to the editor ------------------------------------- */

BOOL	Editor_register (void)
{
	WNDCLASS  wc;
	HINSTANCE MyInstance = GetModuleHandle (0);

	wc.style = 0;
	wc.lpfnWndProc = EditWinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof (EDITOR*);
	wc.hInstance = MyInstance;
	wc.hIcon = 0;
	wc.hCursor = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = "pooredit";
	if (! RegisterClass(&wc)) return FALSE;
	init_ed_keys ();
	scroll_width  = GetSystemMetrics (SM_CXVSCROLL);
	scroll_height = GetSystemMetrics (SM_CYHSCROLL);
	memset (spaces, ' ', sizeof (spaces)-1);
	spaces [sizeof (spaces)-1] = 0;

	edit_move_cursors [0] = LoadCursor (MyInstance, MAKEINTRESOURCE (IDC_MOVE_UL));
	edit_move_cursors [1] = LoadCursor (MyInstance, MAKEINTRESOURCE (IDC_MOVE_U));
	edit_move_cursors [2] = LoadCursor (MyInstance, MAKEINTRESOURCE (IDC_MOVE_UR));
	edit_move_cursors [3] = LoadCursor (MyInstance, MAKEINTRESOURCE (IDC_MOVE_L));
	edit_move_cursors [4] = LoadCursor (MyInstance, MAKEINTRESOURCE (IDC_MOVE_ANY));
	edit_move_cursors [5] = LoadCursor (MyInstance, MAKEINTRESOURCE (IDC_MOVE_R));
	edit_move_cursors [6] = LoadCursor (MyInstance, MAKEINTRESOURCE (IDC_MOVE_DL));
	edit_move_cursors [7] = LoadCursor (MyInstance, MAKEINTRESOURCE (IDC_MOVE_D));
	edit_move_cursors [8] = LoadCursor (MyInstance, MAKEINTRESOURCE (IDC_MOVE_DR));
	mouse_scroll_icon = LoadIcon (MyInstance, MAKEINTRESOURCE (SCROLL_ICON));

	return TRUE;
}

HWND	Editor_create (TBUF * buf, HWND owner)
{
	RECT rc;
	HWND wnd;
	int w, h;
	DWORD style;

	if (!buf)	{
		buf = create_tbuf (8);
		if (!buf)
			return NULL;
	}
	
	w = h = CW_USEDEFAULT;
	style = 0;
	if (owner)	{
		GetClientRect (owner, &rc);
		w = rc.right;
		h = rc.bottom;
		style = WS_CHILD;
	}

	wnd = CreateWindow ("pooredit", "",
			  WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|style,
			  0, 0, w, h,
			  owner,
			  NULL,
			  GetModuleHandle (0),
			  buf);
	return wnd;
}

void	Editor_set_notify_handler (HWND edit, EDIT_NOTIFY_PROC handler, DWORD param)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);
	ed -> notify_handler = handler;
	ed -> notify_param = param;
}

void	Editor_error (HWND edit)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);
	edit_error (ed);
}

void	Editor_set_block (HWND edit, int x1, int y1, int x2, int y2)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);
	edit_set_block (ed, x1, y1, x2, y2);
}

void	Editor_set_block_visible (HWND edit, int x1, int y1, int x2, int y2)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);
	edit_make_visible (ed, x1, y1, x2, y2);
	edit_set_block (ed, x1, y1, x2, y2);
}

BOOL	Editor_get_block (HWND edit, int *x1, int *y1, int *x2, int *y2)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);
	if (ed->is_block)	{
		*x1 = ed->x1;
		*y1 = ed->y1;
		*x2 = ed->x2;
		*y2 = ed->y2;
		return TRUE;
	} else	{
		*x1 = *x2 = ed->x;
		*y1 = *y2 = ed->y;
		return FALSE;
	}
}

void	Editor_get_line_screen_pos (HWND edit, int y, int * y1, int * y2)
{
	POINT p;
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	if (y >= ed->nlines) y = ed->nlines-1;
	if (y >= ed->y0 + ed->sy) y = ed->y0 + ed->sy-1;
	if (y < ed->y0) y = ed->y0;
	p.x = 0;
	p.y = (y-ed->y0) * ed->dy;
	ClientToScreen (ed->hwnd, &p);
	*y1 = p.y;
	*y2 = p.y + ed->sy;
}

void	Editor_set_cursor (HWND edit, int x, int y)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);
	edit_gotoxy (ed, x, y, FALSE, TRUE);
}

int	Editor_nlines (HWND edit)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	return ed->nlines;
}

int	Editor_log_to_phys (HWND edit, int line, int logx)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	return log_to_phys (ed, line, logx);
}

int	Editor_phys_to_log (HWND edit, int line, int x)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	return phys_to_log (ed, line, x);
}

BOOL	Editor_undo (HWND edit)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	return edit_undo (ed, 0);
}

void	Editor_copy (HWND edit)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_cb_copy (ed, 0);
}

void	Editor_paste (HWND edit)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_cb_paste (ed, 0);
}

void	Editor_cut (HWND edit)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_cb_cut (ed, 0);
}

void	Editor_exchange (HWND edit)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_cb_exchange (ed, 0);
}

void	Editor_add (HWND edit)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_cb_add (ed, 0);
}

void	Editor_set_insert (HWND edit, BOOL insert)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_set_insert (ed, insert);
}

void	Editor_set_param (HWND edit, EDITPARAM param, DWORD value)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_set_param (ed, param, value);
}

void	Editor_set_buf (HWND edit, TBUF * buf)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_set_buffer (ed, buf);
}

void	Editor_set_highlight (HWND edit, int line)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	if (ed->high_y >= 0) edit_invalidate_line (ed, ed->high_y);
	ed->high_y = line;
	if (line >= 0)	{
		edit_make_visible (ed, 0, line, 1, line);
		edit_invalidate_line (ed, ed->high_y);
	}
}

void	Editor_set_bookmark (HWND edit, int num)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_setbookmark (ed, num);
}

void	Editor_goto_bookmark (HWND edit, int num)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_gotobookmark (ed, num);
}

void	Editor_delete_bookmark (HWND edit, int num)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_deletebookmark (ed, num);
}

BOOL	Editor_test_bookmark (HWND edit, int num)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	return edit_query_bookmark (ed, num);
}

void	Editor_set_read_only (HWND edit, BOOL read_only)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	ed->read_only = read_only;
}

BOOL	Editor_get_read_only (HWND edit)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	return ed->read_only;
}

void	Editor_set_lang_token (HWND edit, DWORD token)
{
	EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);

	edit_set_lang_token (ed, token);
}

void Editor_encoding_Windows (HWND edit)
{
  EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);
  if (tbuf_is_oem (ed->buf)) {
    tbuf_set_oem (ed->buf, FALSE);
    tbuf_ansi_to_oem (ed->buf);
    InvalidateRect (ed->hwnd, 0, 0);
  }
}

void Editor_encoding_DOS (HWND edit)
{
  EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);
  if (!tbuf_is_oem (ed->buf)) {
    tbuf_set_oem (ed->buf, TRUE);
    tbuf_oem_to_ansi (ed->buf);
    InvalidateRect (ed->hwnd, 0, 0);
  }
}

void Editor_flip_encoding (HWND edit)
{
  EDITOR * ed = (EDITOR*) GetWindowLong (edit, 0);
	assert (ed && ed->magic == EDITOR_MAGIC);
  if (tbuf_is_oem (ed->buf)) {
    tbuf_set_oem (ed->buf, FALSE);
    tbuf_ansi_to_oem (ed->buf);
  } else {
    tbuf_set_oem (ed->buf, TRUE);
    tbuf_oem_to_ansi (ed->buf);
  }
  InvalidateRect (ed->hwnd, 0, 0);
}