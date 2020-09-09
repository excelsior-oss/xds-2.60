#include <windows.h>
#include <stdlib.h>
#include <assert.h>

#include "tbuf.h"
#include "tbuf_impl.h"

#define Malloc malloc
#define Free free
#define Realloc realloc

/* ---------------------------------------------------------- */

	 BOOL	alpha   [256];
	 BOOL	numeric [256];
	 BOOL	alnum   [256];
	 BOOL	hexnum  [256];

unsigned char	tolower_table [256];
unsigned char	toupper_table [256];
unsigned char	toothercase_table [256];

BOOL	alpha_initialized = FALSE;

void	init_alpha (void)
{
	int i;
	if (alpha_initialized) return;
	for (i = 0; i < 256; i++)	{
		alnum [i] = alpha [i] = alpha [i] = IsCharAlpha ((char)i);
		numeric [i] = hexnum [i] = 0;
		tolower_table [i] = (char) CharLower ((LPTSTR)(DWORD)(unsigned char)i);
		toupper_table [i] = (char) CharUpper ((LPTSTR)(DWORD)(unsigned char)i);
		toothercase_table [i] = IsCharLower ((CHAR)i) ? toupper_table [i] : tolower_table [i];
	}
	for (i = '0'; i <= '9'; i++)
		numeric [i] = alnum [i] = hexnum [i] = TRUE;

	alpha ['_'] = alnum ['_'] = TRUE;
	for (i = 'A'; i <= 'H'; i++)
		hexnum [i] = TRUE;
	for (i = 'a'; i <= 'h'; i++)
		hexnum [i] = TRUE;

	alpha_initialized = TRUE;
}

/* ---------------------------------------------------------- */

long	last_bookmark_code = 0;
long	last_edit_code = 0;

/* ----------- editor window notifications --------------------- */

void	report_line_changed (TBUF * buf, int i)
{
	EdRegister * p;
	if (i < buf->inv_start) buf->inv_start = i;
	if (i > buf->inv_end)   buf->inv_end   = i;
	if (buf->no_line_notifications) return;
	for (p = buf -> edlist; p; p = p -> next)
		p -> p.line_changed (p->ed, i);
}

void	report_lines_changed (TBUF * buf, int i, int n)
{
	EdRegister * p;

	if (i < buf->inv_start) buf->inv_start = i;
	if (i+n-1 > buf->inv_end) buf->inv_end = i+n-1;
	for (p = buf -> edlist; p; p = p -> next)
		p -> p.lines_changed (p->ed, i, n);
}

void	report_lines_deleted (TBUF * buf, int i, int n)
{
	EdRegister * p;
	if (i < buf->inv_start) buf->inv_start -= min (n, buf->inv_start-i);
	if (i < buf->inv_end)   buf->inv_end   -= min (n, buf->inv_end-i);
	if (i < buf->inv_start) buf->inv_start = i;
	if (i > buf->inv_end)   buf->inv_end   = i;

	for (p = buf -> edlist; p; p = p -> next)
		p -> p.lines_deleted (p->ed, i, n);
}

void	report_lines_inserted (TBUF * buf, int i, int n)
{
	EdRegister * p;
	if (i <= buf->inv_start) buf->inv_start += n;
	if (i <= buf->inv_end) buf->inv_end     += n;
	if (i < buf->inv_start) buf->inv_start = i;
	if (i+n-1 > buf->inv_end) buf->inv_end = i+n-1;

	for (p = buf -> edlist; p; p = p -> next)
		p -> p.lines_inserted (p->ed, i, n);
}

void	report_tab_changed (TBUF * buf)
{
	EdRegister * p;
	for (p = buf -> edlist; p; p = p -> next)
		p -> p.tab_changed (p->ed);
}

void	report_dirty_changed (TBUF * buf, BOOL dirty, BOOL forced)
{
	EdRegister * p;
	for (p = buf -> edlist; p; p = p -> next)
		p -> p.dirty_changed (p->ed, dirty, forced);
}

void	report_undo_changed (TBUF * buf, BOOL undo)
{
	EdRegister * p;
	for (p = buf -> edlist; p; p = p -> next)
		p -> p.undo_changed (p->ed, undo);
}


BOOL	report_undo_trunc (TBUF * buf, BOOL drop, BOOL dropping_mode)
{
	int i = 0;
	EdRegister * p;
	for (p = buf -> edlist; p; p = p -> next)
		if (p->code == buf->cur_edit)
			return p -> p.undo_trunc (p->ed, drop, dropping_mode);
	if (!buf->edlist) return TRUE;
	return p -> p.undo_trunc (buf->edlist->ed, drop, dropping_mode);
}

void	tbuf_redraw_lines (TBUF * buf, int start, int fin)
{
	EdRegister * p;
	for (p = buf -> edlist; p; p = p -> next)
		p -> p.redraw_lines (p->ed, start, fin);
}

/* -------------------------------------------------------------------- */

TBUF *	create_temp_tbuf (int tab_size)
{
	TBUF * b = Malloc (sizeof (TBUF));
	if (!b) return NULL;
	memset (b, 0, sizeof (TBUF));
	b -> magic = TBUF_MAGIC;
	b -> tab_size = tab_size;
	b -> cut_spaces = TRUE;
	b -> eol_type = EOL_DEFAULT;
	b -> inv_start = 0x3FFFFFFF;
	b -> inv_end = -2;
  b -> oem = FALSE;
	return b;
}

BOOL	tbuf_query_undo_trunc_proc (void * param, BOOL drop, BOOL dropping_mode)
{
	TBUF * b = (TBUF*) param;
	return report_undo_trunc (b, drop, dropping_mode);
}

TBUF *	create_tbuf (int tab_size)
{
	TBUF * b;
	init_alpha ();
	b = create_temp_tbuf (tab_size);
	if (!b) return NULL;
	b->undo_buffer = create_undo_buffer (1*1024*1024, tbuf_query_undo_trunc_proc, (void*) b);
	if (! b->undo_buffer)
		return NULL;
	b->undo = b->undo_buffer;
	return b;
}

TBUF * new_tbuf (int tab_size)
{
	TBUF * buf = create_tbuf (tab_size);
	if (!buf) return 0;
	if (!tbuf_add_line (buf, "", 0, TRUE))	{
		free_tbuf (buf);
		return NULL;
	}
	buf->dirty = FALSE;
	return buf;
}

void	tbuf_add_destructor (TBUF * buf, void * data, void (*fun) (void*))
{
	DESTRUCTOR * d = Malloc (sizeof (DESTRUCTOR));
	if (!d) return;
	d->data = data;
	d->fun = fun;
	d->next = buf->destr_list;
	buf->destr_list = d;
}

void	clear_tbuf (TBUF * buf)
{
	int i;
	DESTRUCTOR * d;

	for (i = 0; i < buf->nlines; i++)
		if (! (NOT_ALLOCATED & buf->lines [i].flags))
			Free (buf->lines [i].s);
	buf->nlines = 0;
	buf->inv_start = 0x3FFFFFFF;
	buf->inv_end = -2;
	if (buf->undo_buffer)
		clear_undo_buffer (buf->undo_buffer);
	while (buf->destr_list)	{
		d = buf->destr_list;
		buf->destr_list = d->next;
		d->fun (d->data);
		free (d);
	}
	buf->dirty = FALSE;
}

void	free_tbuf (TBUF * buf)
{
	clear_tbuf (buf);
	Free (buf->lines);
	Free (buf->parse_info);

	if (buf->undo_buffer)
		free_undo_buffer (buf->undo_buffer);
	Free (buf);
}

void	tbuf_set_undo (TBUF * buf)
{
	BOOL available;

	if (buf->undo_buffer)	{
		available = test_undo (buf->undo_buffer);
		if (available != buf->undo_available)	{
			buf->undo_available = available;
			report_undo_changed (buf, available);
		}
	}

}

void	tbuf_clear_undo (TBUF * buf)
{
	clear_undo_buffer (buf->undo_buffer);
	tbuf_set_undo (buf);
}

void	tbuf_set_dirty	(TBUF * buf, BOOL dirty)
{
	if (dirty != buf->dirty)	{
		buf->dirty = dirty;
		report_dirty_changed (buf, dirty, TRUE);
	}
	tbuf_set_undo (buf);
}

void	tbuf_make_dirty	(TBUF * buf, BOOL dirty)
{
	if (dirty != buf->dirty)	{
		buf->dirty = dirty;
		report_dirty_changed (buf, dirty, FALSE);
	}
	tbuf_set_undo (buf);
}

BOOL	tbuf_is_dirty (TBUF * buf)
{
	return buf->dirty;
}

EOL_TYPE tbuf_eol_type (TBUF * buf)
{
	return buf->eol_type;
}

void	tbuf_set_eol_type (TBUF * buf, EOL_TYPE type)
{
	buf->eol_type = type;
}

int	tbuf_tab_size (TBUF * buf)
{
	return buf->tab_size;
}

DWORD *	tbuf_get_parsing_data (TBUF * buf)
{
	return buf->parse_info;
}

BOOL	tbuf_query_inv_lines (TBUF * buf, int * start, int * fin)
{
	*start = buf->inv_start;
	*fin = buf->inv_end;
	return buf->inv_start <= buf->inv_end;
}

void	tbuf_validate_inv_lines (TBUF * buf)
{
	buf->inv_start = 0x3FFFFFFF;
	buf->inv_end = -2;
}

void	tbuf_addref (TBUF * buf)
{
	buf -> ref_count ++;
}

void	tbuf_release (TBUF * buf)
{
	buf->ref_count --;
	if (!buf->ref_count)
		free_tbuf (buf);
}

long	tbuf_register_edit (TBUF * buf, void * edit, EDIT_CALLBACKS * cb)
{
	EdRegister * p = Malloc (sizeof (EdRegister));
	if (!p) return 0;
	p -> ed = edit;
	p -> p = * cb;
	p -> next = buf -> edlist;
	p -> code = ++last_edit_code;
	buf -> edlist = p;
	tbuf_addref (buf);
	return last_edit_code;
}

void	tbuf_unregister_edit (TBUF * buf, void * edit)
{
	EdRegister ** p, *q;
	for (p = & buf->edlist; *p; p = & (*p)->next)
		if ((*p)->ed == edit)	{
			q = *p;
			*p = q -> next;
			Free (q);
			tbuf_release (buf);
			return;
		}
}

void	tbuf_set_current_edit (TBUF * buf, long edit_code)
{
	buf->cur_edit = edit_code;
}

void	tbuf_stop_line_notifications (TBUF * buf)
{
	buf->no_line_notifications = TRUE;
}

void	tbuf_continue_line_notifications (TBUF * buf)
{
	buf->no_line_notifications = FALSE;
	if (buf->inv_start <= buf->inv_end)
		report_lines_changed (buf, buf->inv_start, buf->inv_end-buf->inv_start+1);
}

long	tbuf_add_bookmark (TBUF * buf, int x, int y, BOOL keep)
{
	BOOKMARK * b = Malloc (sizeof (BOOKMARK));
	if (!b) return 0;
	++ last_bookmark_code;
	b -> code = last_bookmark_code;
	b -> x = x;
	b -> y = y;
	b -> keep = keep;
	b -> next = buf -> bookmarks;
	buf -> bookmarks = b;
	return last_bookmark_code;
}

void	tbuf_delete_bookmark (TBUF * buf, long code)
{
	BOOKMARK * b, * bb;
	b = buf -> bookmarks;
	if (!b) return;
	if (b -> code == code)	{
		buf -> bookmarks = b -> next;
		Free (b);
	} else	{
		while (b->next && b->next->code != code) b = b->next;
		if (b->next)	{
			bb = b->next;
			b->next = bb->next;
			Free (bb);
		}
	}
}

void	tbuf_change_bookmark (TBUF * buf, long code, int x, int y)
{
	BOOKMARK * b;
	for (b = buf -> bookmarks; b; b = b->next)
		if (b->code == code)	{
			b->x = x;
			b->y = y;
			return;
		}
}

BOOL	tbuf_query_bookmark (TBUF * buf, long code, int *x, int *y)
{
	BOOKMARK * b;
	if (!code) return FALSE;
	for (b = buf -> bookmarks; b; b = b->next)
		if (b->code == code)	{
			*x = b->x;
			*y = b->y;
			return TRUE;
		}
	return FALSE;
}

BOOL	in_block (int x, int y, int x1, int y1, int x2, int y2)
{
	if (y < y1 || y > y2) return FALSE;
	if (y == y1 && x < x1) return FALSE;
	if (y == y2 && x >= x2) return FALSE;
	return TRUE;
}

void	tbuf_swap_bookmarks (TBUF * buf, int x1, int y1, int x2, int y2, int x3, int y3)
{
	BOOKMARK * b;
	for (b = buf -> bookmarks; b; b = b->next)	{
		if (b->y < y1 || b->y == y1 && b->x < x1) continue;
		if (b->y > y3) continue;
		if (b->y == y3 && b->x >= x3)	{
			b->x = b->x - x3 + x2;
			continue;
		}
		if (b->y == y1)	{
			b->y += y3-y2;
			b->x = b->x - x1 + x3;
			continue;
		}
		if (b->y < y2 || b->y == y2 && b->x < x2)	{
			b->y += y3-y2;
			continue;
		}
		if (b->y == y2)
			b->x = b->x - x2 + x1;
		b->y -= y2-y1;
	}
}

void	tbuf_move_bookmarks (TBUF * buf, int x1, int y1, int x2, int y2, int x, int y)
{
	if (y < y1 || y == y1 && x < x1)
		tbuf_swap_bookmarks (buf, x, y, x1, y1, x2, y2);
	else if (y > y2 || y == y2 && x > x2)
		tbuf_swap_bookmarks (buf, x1, y1, x2, y2, x, y);
}

void	delete_bookmarks (TBUF * buf, int x1, int y1, int x2, int y2)
{
	BOOKMARK * b, * bb;
	
	for (b = buf -> bookmarks; b; b = bb)	{
		bb = b->next;
		if (b->y < y1) continue;
		if (b->y == y1 && b->x < x1) continue;
		if (b->y > y2)	{
			b->y -= y2-y1;
			continue;
		}
		if (b->y == y2 && b->x >= x2)	{
			b->y = y1;
			b->x = x1 + b->x - x2;
			continue;
		}
		if (b->keep)	{
			b->x = x1;
			b->y = y1;
		} else
			tbuf_delete_bookmark (buf, b->code);
	}
}

int	calc_line_physical_length (char * s, int len, int tab_size)
{
	int l = 0;
	while (len--)
		if (*s++ == '\t')
			l = (l / tab_size + 1) * tab_size;
		else
			l++;
	return l;
}

void	tbuf_set_tab_size (TBUF * buf, int tab_size)
{
	int i, k, ml, p;
	k = 0;
	ml = 0;

	if (tab_size == buf->tab_size) return;
	buf->tab_size = tab_size;
	for (i = 0; i < buf->nlines; i++)	{
		p = calc_line_physical_length (buf->lines [i].s, buf->lines [i].len, tab_size);
		buf->lines [i].p_len = p;
		if (p > ml)	{
			ml = p;
			k = i;
		}
	}
	buf->max_line_length = ml;
	buf->longest_line = k;
	report_tab_changed (buf);
}

void	new_longest_line (TBUF * b)
{
	int j, k, ml;
	k = 0;
	ml = 0;
	for (j = 0; j < b->nlines; j++)
		if (b->lines [j].p_len > ml)	{
			ml = b->lines [j].p_len;
			k = j;
		}
	b->max_line_length = ml;
	b->longest_line = k;
}

void	check_line_length (TBUF * b, int i)
{
	int plen;
	
	plen = calc_line_physical_length (b->lines[i].s, b->lines[i].len, b->tab_size);
	b->lines[i].p_len = plen;	
	if (plen > b->max_line_length)	{
		b->max_line_length = plen;
		b->longest_line = i;
	} else if (i == b->longest_line && plen < b->max_line_length)
		new_longest_line (b);
}

BOOL	add_line_space (TBUF * b, int n)
{
	TLINE * l;
	DWORD * d;
	int a;

	if (b -> a_nlines - b -> nlines < n)	{
		a = (b -> nlines + n + TEXT_ALLOC_DELTA-1) / TEXT_ALLOC_DELTA * TEXT_ALLOC_DELTA;
		l = Realloc (b -> lines, a * sizeof (TLINE));
		if (!l) return FALSE;
		b -> lines = l;
		d = Realloc (b -> parse_info, a * sizeof (DWORD));
		if (!d) return FALSE;
		b -> parse_info = d;
		b -> a_nlines = a;
	}
	return TRUE;
}

void	init_line (TBUF * buf, int i)
{
	memset (&buf->lines [i], 0, sizeof (TLINE));
	buf->parse_info [i] = 0;
}

void	init_lines (TBUF * buf, int i, int n)
{
	memset (&buf->lines [i], 0, n * sizeof (TLINE));
	memset (&buf->parse_info [i], 0, n * sizeof (DWORD));
}

BOOL	add_empty_lines (TBUF * b, int n)
{
	if (! add_line_space (b, n)) return FALSE;
	init_lines (b, b->nlines, n);
	b->nlines += n;
	return TRUE;
}

BOOL	set_line (TBUF * b, int i, char * s, int len)
{
	int a_len;
	char * p;
	TLINE * l = &b->lines [i];

	if (len == 0)	{
		if (! (l->flags & NOT_ALLOCATED)) Free (l->s);
		l->s = NULL;
		l->a_len = 0;
	} else if (len <= l->a_len)
		memcpy (l->s, s, len);
	else	{
		a_len = (len + LINE_ALLOC_DELTA-1) / LINE_ALLOC_DELTA * LINE_ALLOC_DELTA;
		p = Malloc (a_len);
		if (!p) return FALSE;
		if (! (l->flags & NOT_ALLOCATED)) Free (l->s);
		l->a_len = a_len;
		l->s = p;
		memcpy (p, s, len);
	}
	if (len && b->cut_spaces)
		while (len && s[len-1]==' ') -- len;

	l->len = len;
	check_line_length (b, i);
	return TRUE;
}

BOOL	tbuf_add_line (TBUF * buf, char * s, int len, BOOL copy)
{
	if (! add_line_space (buf, 1)) return FALSE;
	init_line (buf, buf->nlines++);
	if (copy)
		return set_line (buf, buf->nlines-1, s, len);
	else	{
		buf->lines [buf->nlines-1].s = s;
		buf->lines [buf->nlines-1].len = len;
		buf->lines [buf->nlines-1].flags = NOT_ALLOCATED;
		check_line_length (buf, buf->nlines-1);
		return TRUE;
	}
}

BOOL	realloc_line (TBUF * buf, int i, int len)
{
	int a_len;
	char * p;
	TLINE * l = &buf->lines [i];

	a_len = (len+1 + LINE_ALLOC_DELTA-1) / LINE_ALLOC_DELTA * LINE_ALLOC_DELTA;
	if (l->flags & NOT_ALLOCATED)	{
		p = Malloc (a_len);
		if (!p) return FALSE;
		if (l->len) memcpy (p, l->s, min (len, l->len));
	} else	{
		p = Realloc (l->s, a_len);
		if (!p) return FALSE;
	}
	l->a_len = a_len;
	l->s = p;
	return TRUE;
}

BOOL	check_alloc (TBUF * buf, int i)
{
	if (buf->lines [i].flags & NOT_ALLOCATED)
		return realloc_line (buf, i, buf->lines [i].len);
	return TRUE;
}

BOOL	tbuf_get_nlines (TBUF * b)
{
	return b -> nlines;
}

BOOL	tbuf_get_ncolumns (TBUF * b)
{
	return b -> max_line_length;
}

BOOL	tbuf_get_line (TBUF * b, int i, char ** p, int * len)
{
	assert (i >= 0);
	if (i >= b->nlines)	{
		*p = "";
		*len = 0;
		return FALSE;
	}
	*len = b->lines [i].len;
	*p   = b->lines [i].s;
	if (!*len) *p = "";
	return TRUE;
}

int	tbuf_get_line_length (TBUF * b, int i)
{
	assert (i >= 0);
	if (i >= b->nlines)	return 0;
	return b->lines [i].len;
}

typedef BOOL	(* TBUF_STREAM_FUNC) (char * s, int len, DWORD param);

long	tbuf_stream_out_block (TBUF * buf, int x1, int y1, int x2, int y2, char * eol, int eollen, TBUF_STREAM_FUNC fun, DWORD param)
{
	long len;

	if (y1 < 0) y1 = 0;
	if (y2 >= buf->nlines) y2 = buf->nlines-1;
	if (y1 > y2) return 0;
	if (x1 < 0) x1 = 0;
	if (x2 > buf->lines [y2].len) x2 = buf->lines [y2].len;
	if (y1 == y2)	{
		if (x1 >= x2) return 0;
		if (fun) fun (buf->lines [y1].s+x1, x2-x1, param);
		return x2-x1;
	}
	if (!eol)	{
		eol = "\r\n";
		eollen = 2;
	}
	len = 0;
	if (x1 < buf->lines [y1].len)	{
		if (fun && !fun (buf->lines [y1].s+x1, buf->lines [y1].len-x1, param)) return len;
		len += buf->lines [y1].len-x1;
	}
	if (fun && !fun (eol, eollen, param)) return len;
	len += eollen;
	while (++y1 < y2)	{
		if (fun && !fun (buf->lines [y1].s, buf->lines [y1].len, param)) return len;
		len += buf->lines [y1].len;
		if (fun && !fun (eol, eollen, param)) return len;
		len += eollen;
	}
	if (x2)	{
		if (fun && !fun (buf->lines [y2].s, x2, param)) return len;
		len += x2;
	}
	return len;
}

long	tbuf_stream_out (TBUF * buf, char * eol, int eollen, TBUF_STREAM_FUNC fun, DWORD param)
{
	int y;
	long len;
	if (!buf->nlines) return 0;
	if (!eol)	{
		eol = "\r\n";
		eollen = 2;
	}
	len = 0;
	for (y = 0; y < buf->nlines-1; y++)	{
		if (fun && !fun (buf->lines [y].s, buf->lines [y].len, param)) return len;
		len += buf->lines [y].len;
		if (fun && !fun (eol, eollen, param)) return len;
		len += eollen;
	}
	if (fun && !fun (buf->lines [y].s, buf->lines [y].len, param)) return len;
	len += buf->lines [y].len;

	return len;
}

/* ----------- edit operations --------------------------------- */

void	test_line_spaces (TBUF * buf, int i)
{
	char * s;
	int len;

	if (! buf->cut_spaces) return;
	s = buf->lines [i].s;
	len = buf->lines [i].len;
	while (len && s [len-1] == ' ') -- len;
	buf->lines [i].len = len;
}

/* ----- Operations with character strings within one line ------ */

BOOL	swap_blocks (char* s, int x1, int x2)
{
	char * p;
	if (x1 < x2-x1)	{
		p = Malloc (x1);
		if (!p) return FALSE;
		memcpy  (p, s, x1);
		memmove (s, s+x1, x2-x1);
		memcpy  (s+x2-x1, p, x1);
	} else	{
		p = Malloc (x2-x1);
		if (!p) return FALSE;
		memcpy  (p, s+x1, x2-x1);
		memmove (s+x2-x1, s, x1);
		memcpy  (s, p, x2-x1);
	}
	Free (p);
	return TRUE;
} 

BOOL	do_insert_string (TBUF * buf, int i, int x, char *s, int inslen)
{
	int len0, len;
	
	len = len0 = buf->lines [i].len;
	if (x > len)
		len = x;
	len += inslen;
	if (! realloc_line (buf, i, len))
		return FALSE;
	if (buf->cur_edit && buf->undo &&
	    !undo_store_insert (buf->undo, inslen, buf->cur_edit, x+inslen, i))
		return FALSE;
	tbuf_make_dirty (buf, TRUE);
	if (x < len0)
		memmove (buf->lines [i].s+x+inslen, buf->lines [i].s+x, len0-x);
	else
		memset (buf->lines [i].s + len0, ' ', x - len0);
	memmove (buf->lines [i].s+x, s, inslen);
	buf->lines [i].len = len;
	test_line_spaces (buf, i);
	check_line_length (buf, i);
	return TRUE;
}

BOOL	tbuf_overwrite_char (TBUF * buf, int i, int x, char c)
{
	char  o;

	if (x >= buf->lines [i].len)	{
		if (! do_insert_string (buf, i, x, &c, 1)) return FALSE;
	} else	{
		o = buf->lines [i].s [x];
		if (o == c) return TRUE;
		if (! check_alloc (buf, i)) return FALSE;
		if (buf->cur_edit && buf->undo &&
		    !undo_store_overwrite (buf->undo, 1, buf->cur_edit, x+1, i, &o))
		return FALSE;
		tbuf_make_dirty (buf, TRUE);
		buf->lines [i].s [x] = c;
		test_line_spaces (buf, i);
		if (o == '\t' || c == '\t')
			check_line_length (buf, i);
	}
	report_line_changed (buf, i);
	return TRUE;
}

BOOL	tbuf_insert_string (TBUF * buf, int i, int x, char *s, int len)
{
	BOOKMARK * b;
	
	if (! do_insert_string (buf, i, x, s, len)) return FALSE;
	for (b = buf->bookmarks; b; b = b->next)
		if (b -> y == i && b->x >= x)
			b -> x += len;
	report_line_changed (buf, i);
	return TRUE;
}

BOOL	do_delete_string (TBUF * buf, int i, int x, int len)
{
	if (x >= buf->lines [i].len)
		return TRUE;
	if (len >= buf->lines [i].len-x) len = buf->lines [i].len-x;
	if (!len) return TRUE;
	if (!check_alloc (buf, i)) return FALSE;
	if (buf->cur_edit && buf->undo &&
	    !undo_store_delete (buf->undo, len, buf->cur_edit, x, i, buf->lines [i].s+x))
		return FALSE;
	tbuf_make_dirty (buf, TRUE);
	memmove (buf->lines [i].s+x, buf->lines [i].s+x+len, buf->lines [i].len-x-len);
	buf->lines [i].len -= len;
	buf->lines [i].s [buf->lines [i].len] = '*';
	test_line_spaces (buf, i);
	check_line_length (buf, i);
	return TRUE;
}

BOOL	tbuf_delete_string (TBUF * buf, int i, int x, int len)
{
	if (! do_delete_string (buf, i, x, len)) return FALSE;
	delete_bookmarks (buf, x, i, x+len, i);
	report_line_changed (buf, i);
	return TRUE;
}

/* --------------- Operations with several lines of text ----------------- */

BOOL	undo_delete_fun (char * s, int len, DWORD lparam)
{
	char ** ptr = (char**) lparam;
	memcpy (*ptr, s, len);
	*ptr += len;
	return TRUE;
}

BOOL	do_store_undo_delete_block (TBUF * buf, int x1, int y1, int x2, int y2)
{
	long len;
	char * ptr;
	if (!buf->cur_edit || !buf->undo) return TRUE;
	len = tbuf_stream_out_block (buf, x1, y1, x2, y2, "\n", 1, 0, 0);
	if (!undo_store_delete_block (buf->undo, len, buf->cur_edit, x1, y1, &ptr))
		return FALSE;
	if (!ptr) return TRUE;
	tbuf_make_dirty (buf, TRUE);
	tbuf_stream_out_block (buf, x1, y1, x2, y2, "\n", 1, undo_delete_fun, (DWORD)&ptr);
	return TRUE;
}

void	do_delete_lines (TBUF * buf, int i, int n)
{
	int j;

	if (buf->nlines - i < n) n = buf->nlines - i;
	for (j = 0; j < n; j++)
		if (! (buf -> lines [i+j].flags & NOT_ALLOCATED))
			Free (buf -> lines [i+j].s);
	memmove (&buf -> lines [i], &buf -> lines [i+n],
		(buf->nlines - i - n) * sizeof (TLINE));
	memmove (&buf -> parse_info[i], &buf -> parse_info[i+n],
		(buf->nlines - i - n) * sizeof (DWORD));
	buf -> nlines -= n;
	if (buf->longest_line >= i+n)
		-- buf->longest_line;
	else if (buf->longest_line >= i)
		new_longest_line (buf);
}

void	tbuf_delete_lines (TBUF * buf, int i, int n)
{
	if (buf->nlines - i < n) n = buf->nlines - i;
	if (! do_store_undo_delete_block (buf, 0, i, 0, i+n))
		return;
	do_delete_lines (buf, i, n);
	delete_bookmarks (buf, 0, i, 0, i+n);
	report_lines_deleted (buf, i, n);
}

BOOL	tbuf_delete_block (TBUF * buf, int x1, int y1, int x2, int y2)
{
	UNDO_BUFFER * save_undo;
	if (y1 > y2) return TRUE;
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x1 > buf->lines [y1].len) x1 = buf->lines [y1].len;
	if (x2 > buf->lines [y2].len) x2 = buf->lines [y2].len;
	if (y1 == y2 && x1 >= x2) return TRUE;
	if (y1 == y2)
		return tbuf_delete_string (buf, y1, x1, x2-x1);
	if (! do_store_undo_delete_block (buf, x1, y1, x2, y2))
		return FALSE;
	save_undo = buf->undo;
	buf->undo = NULL;

	if (x1 == 0)	{
		if (! do_delete_string (buf, y2, 0, x2))	{
			buf->undo = save_undo;
			return FALSE;
		}
		do_delete_lines (buf, y1, y2-y1);
		delete_bookmarks (buf, x1, y1, x2, y2);
		report_lines_deleted (buf, y1, y2-y1);
		report_line_changed  (buf, y1);
	} else	{
		if (! realloc_line (buf, y1, x1 + buf->lines [y2].len - x2))	{
			buf->undo = save_undo;
			return FALSE;
		}
		memcpy (buf->lines [y1].s+x1, buf->lines [y2].s+x2, buf->lines [y2].len - x2);
		buf->lines [y1].len = x1 + buf->lines [y2].len - x2;
		test_line_spaces (buf, y1);
		check_line_length (buf, y1);
		delete_bookmarks (buf, x1, y1, x2, y2);
		do_delete_lines (buf, y1+1, y2-y1);
		report_lines_deleted (buf, y1+1, y2-y1);
		report_line_changed (buf, y1);
	}
	buf->undo = save_undo;
	return TRUE;
}

BOOL	do_insert_lines_without_undo (TBUF * buf, int i, int n)
{
	if (! add_line_space (buf, n)) return FALSE;
	memmove (buf->lines+i+n, buf->lines+i, (buf->nlines-i) * sizeof (TLINE));
	memmove (buf->parse_info+i+n, buf->parse_info+i, (buf->nlines-i) * sizeof (DWORD));
	buf->nlines += n;
	init_lines (buf, i, n);
	if (buf->longest_line >= i) buf->longest_line += n;
	return TRUE;
}

BOOL	do_insert_lines (TBUF * buf, int i, int n)
{
	if (! add_line_space (buf, n)) return FALSE;
	if (buf->cur_edit && buf->undo &&
	    !undo_store_insert_lines (buf->undo, n, buf->cur_edit, i+n))
		return FALSE;
	if (!do_insert_lines_without_undo (buf, i, n)) return FALSE;
	tbuf_make_dirty (buf, TRUE);
	return TRUE;
}

BOOL	tbuf_insert_lines (TBUF * buf, int i, int n)
{
	BOOKMARK * b;
	if (! do_insert_lines (buf, i, n)) return FALSE;
	for (b = buf->bookmarks; b; b = b->next)
		if (b -> y >= i)
			b -> y += n;
	report_lines_inserted (buf, i, n);
	return TRUE;
}

BOOL	move_in_line (TBUF * buf, int y, int x1, int x2, int x)
{
	BOOL res;
	BOOKMARK * b;

	if (x < x1)
		res = swap_blocks (buf->lines [y].s+x, x1-x, x2-x);
	else if (x > x2)
		res = swap_blocks (buf->lines [y].s+x1, x2-x1, x-x1);
	else
		return TRUE;
	if (!res) return FALSE;
	for (b = buf->bookmarks; b; b = b->next)
		if (b->y == y)	{
			if (b->x >= x1 && b->x < x2)
				b->x = b->x-x1+x;
			else if (x >= b->x && b->x < x1)
				b->x += x2-x1;
			else if (b->x >= x2 && b->x < x)
				b->x -= x2-x1;
		}
	test_line_spaces (buf, y);
	report_line_changed (buf, y);
	return TRUE;
}

/* y1-> AB     AD
 * y2-> CD  => EB
 * y -> EF     CF
*/

BOOL	move_cyclic (TBUF * buf, int x1, int y1, int x2, int y2, int x, int y)
{
	TLINE * l1, * l2, *l;
	char * s1, * s2, *s;
	int len1, len2, len;
	l1 = &buf->lines [y1];
	l2 = &buf->lines [y2];
	l  = &buf->lines [y];
	len1 = x1+l2->len-x2;
	len2 = x +l1->len-x1;
	len  = x2+l ->len-x;
	s1 = Malloc (len1+1);
	s2 = Malloc (len2+1);
	s  = Malloc (len+1);
	if (!s1 || !s2 || !s)	{
		Free (s1); free (s2); free (s);
		return FALSE;
	}
	memcpy (s1, l1->s, x1);
	memcpy (s2, l->s,  x);
	memcpy (s,  l2->s, x2);
	memcpy (s1+x1, l2->s+x2, l2->len-x2);
	memcpy (s2+x,  l1->s+x1, l1->len-x1);
	memcpy (s+x2,  l ->s+x,  l ->len-x);
	s1[len1] = '*';
	s2[len2] = '*';
	s [len]  = '*';
	Free (l1->s);	l1->s = s1;	l1->len = len1;
	Free (l2->s);	l2->s = s2;	l2->len = len2;
	Free (l ->s);	l ->s = s;	l ->len = len;
	test_line_spaces (buf, y1);
	test_line_spaces (buf, y2);
	test_line_spaces (buf, y);
	check_line_length (buf, y1);
	check_line_length (buf, y2);
	check_line_length (buf, y);
	return TRUE;
}

BOOL	tbuf_move_text (TBUF * buf, int x1, int y1, int x2, int y2, int x, int y)
{
	BOOL res;

	if (y1 == y2)	{
		if (y == y1)	{
			if (x < x1)
				res = swap_blocks (buf->lines [y].s+x, x1-x, x2-x);
			else if (x > x2)
				res = swap_blocks (buf->lines [y].s+x1, x2-x1, x-x1);
			else
				return TRUE;

			if (!res) return FALSE;
			test_line_spaces (buf, y);
			tbuf_move_bookmarks (buf, x1, y1, x2, y2, x, y);
			report_line_changed (buf, y);
			return TRUE;
		} else	{
			if (!do_insert_string (buf, y, x, buf->lines [y1].s+x1, x2-x1))
				return FALSE;
			do_delete_string (buf, y1, x1, x2-x1);
			tbuf_move_bookmarks (buf, x1, y1, x2, y2, x, y);
			report_line_changed (buf, y);
			report_line_changed (buf, y1);
			return TRUE;
		}
	} else	{
		if (y == y1 || y == y2) return FALSE;
		if (! move_cyclic (buf, x1, y1, x2, y2, x, y))
			return FALSE;
		report_line_changed (buf, y1);
		report_line_changed (buf, y2);
		report_line_changed (buf, y);
		return TRUE;
	}
	return FALSE;
}

void	tbuf_copy_text (TBUF * buf, int x1, int y1, int x2, int y2, int x, int y)
{
}

/* ======================================================================= */

int	tbuf_count_lines (char * p, unsigned long len, BOOL eof)
{
	char * fin = p+len;
	char * q;
	int count;

	count = 0;
	for (;;)	{
		for (q = p; q<fin && *q && *q != '\r' && *q != '\n' && !(*q == 0x1A && eof); q++);
		++ count;
		if (q >= fin || *q == 0x1A && eof) break;
		p = q;

		if (p+1 < fin && *p == '\r' && p[1] == '\n')
			p += 2;
		else
			p ++;
	}
	return count;
}

BOOL	tbuf_make_buffer (TBUF * buf, char * p, unsigned long len, int nlines, BOOL copy, BOOL eof)
{
	char * fin = p+len;
	char * q;
	int zero, cr, lf, crlf, m;

	clear_tbuf (buf);
	if (! add_line_space (buf, nlines+1))	{
		report_lines_changed (buf, 1, buf->nlines);
		return FALSE;
	}
	zero = cr = lf = crlf = 0;
	for (;;)	{
		for (q = p; q<fin && *q && *q != '\r' && *q != '\n' && !(*q == 0x1A && eof); q++);
		if (! tbuf_add_line (buf, p, q-p, copy)) return FALSE;
		if (q >= fin || *q == 0x1A && eof) break;
		p = q;

		if (p+1 < fin && *p == '\r' && p[1] == '\n')	{
			++ crlf;
			p += 2;
		} else	{
			if (!*p) ++ zero;
			else if (*p=='\r') ++ cr;
			else if (*p=='\n') ++ lf;
			p ++;
		}
	}
	if (buf->nlines) buf->lines [buf->nlines-1].len = q-p;
	if (!zero && !cr && !lf && !crlf)
		buf->eol_type = EOL_DEFAULT;
	else	{
		buf->eol_type = EOL_ZERO; m = zero;
		if (cr > m)	{
			buf->eol_type = EOL_CR; m = cr;
		}
		if (lf > m)	{
			buf->eol_type = EOL_LF; m = lf;
		}
		if (crlf > m)
			buf->eol_type = EOL_CRLF;
	}
	buf->inv_start = 0;
	buf->inv_end = buf->nlines-1;
	return TRUE;
}

DWORD count_time;

BOOL	tbuf_set_text (TBUF * buf, char * p, unsigned long len, BOOL copy, BOOL eof)
{
	int count;

	count = tbuf_count_lines (p, len, eof);
	return tbuf_make_buffer (buf, p, len, count, copy, eof);
}

BOOL	tbuf_stream_in_text (TBUF * buf, char * p, int len, int x, int y)
{
	TBUF * t;
	BOOKMARK * b;
	int lastlen, count, l, i;
	BOOL undo_allocated = FALSE;

	count = tbuf_count_lines (p, len, FALSE);
	if (count == 1)
		return tbuf_insert_string (buf, y, x, p, len);

	if (! add_line_space (buf, count-1)) return FALSE;
	t = create_tbuf (buf -> tab_size);
	if (!t) return FALSE;
	if (! tbuf_make_buffer (t, p, len, count, TRUE, FALSE))
		goto failure;

	lastlen = t->lines [count-1].len;
	if (x == 0 && lastlen == 0)	{
		if (!do_insert_lines (buf, y, count-1))
			return FALSE;
		memmove (&buf->lines [y], &t->lines [0], (count-1) * sizeof (TLINE));
		memmove (&buf->parse_info[y], &t->parse_info[0], (count-1) * sizeof (DWORD));
		memset (&t->lines [0], 0, (count-1) * sizeof (TLINE));
		for (i = 0; i < count-1; i++)
			check_line_length (buf, y+i);
	} else	{
		if (buf->cur_edit && buf->undo &&
		    !undo_store_insert_block (buf->undo, buf->cur_edit, x, y, lastlen, y+count-1))
			goto failure;
		undo_allocated = TRUE;

		l = buf->lines [y].len-x;
		if (l < 0) l = 0;
		if (! realloc_line (t, count-1, lastlen + l))
			goto failure;
		if (buf->lines [y].len > x)	{
			memcpy (t->lines [count-1].s+lastlen, buf->lines [y].s+x, buf->lines [y].len-x);
			t->lines [count-1].len += buf->lines [y].len-x;
		}
		if (! realloc_line (buf, y, x+t->lines [0].len))
			goto failure;
		if (buf->lines [y].len < x)
			memset (buf->lines [y].s+buf->lines [y].len, ' ', x-buf->lines [y].len);
		memcpy (buf->lines [y].s+x, t->lines [0].s, t->lines [0].len);
		buf->lines [y].len = x + t->lines [0].len;

		do_insert_lines_without_undo (buf, y+1, count-1);
		memmove (&buf->lines [y+1], &t->lines [1], (count-1) * sizeof (TLINE));
		memmove (&buf->parse_info[y+1], &t->parse_info[1], (count-1) * sizeof (DWORD));
		memset (&t->lines [1], 0, (count-1) * sizeof (TLINE));

		test_line_spaces (buf, y);
		test_line_spaces (buf, y+count-1);
		for (i = 0; i < count; i++)
			check_line_length (buf, y+i);
	}
	free_tbuf (t);
	for (b = buf->bookmarks; b; b = b->next)
		if (b->y > y)
			b->y += count - 1;
		else if (b->y == y && b->x >= x)	{
			b->y += count - 1;
			b->x = b->x - x + lastlen;
		}
	report_lines_inserted (buf, y+1, count-1);
	report_line_changed (buf, y+count-1);
	report_line_changed (buf, y);
	tbuf_make_dirty (buf, TRUE);
	return TRUE;

failure:
	if (t)
		free_tbuf (t);
	if (undo_allocated)
		drop_last_coord_undo (buf->undo);
	return FALSE;
}

/* ======================================================================= */

int	tbuf_word_end (TBUF * buf, int x, int y)
{
	for (; x < buf->lines [y].len; x++)
		if (! IS_ALNUM (buf->lines [y].s[x]))
			break;
	return x;
}

int	tbuf_word_start (TBUF * buf, int x, int y)
{
	if (x >= buf->lines [y].len)
		return x;
	if (! IS_ALNUM (buf->lines [y].s[x]))
		return x;
	for (;x > 0; x--)
		if (! IS_ALNUM (buf->lines [y].s[x-1]))
			break;
	return x;
}

void	tbuf_next_word (TBUF * buf, int *x0, int *y0, BOOL multiline)
{
	int x = *x0;
	int y = *y0;
	int y1;

	/* Remark from Hady, Dec 1, 2004. Here was a bug that can be reproduced as follows:
	   a) run XDS, b) create new file, c) move caret one step right, c) try to mark previous word - you'll get a crash.
	   The "if" right below is a fix.
	 */
	if ( buf->lines[y].s == 0 ) 
		return;

	x = tbuf_word_end (buf, x, y);

	y1 = multiline ? buf->nlines : y+1;
	for (; y < y1; y++, (x=0))
		for (;x < buf->lines [y].len; x++)
			if (IS_ALNUM (buf->lines [y].s[x]))	{
				*x0 = x;
				*y0 = y;
				return;
			}
	if (y == y1)	{
		--y;
		x = buf->lines [y].len;
	}
	*x0 = x;
	*y0 = y;
}

void	tbuf_prev_word (TBUF * buf, int *x0, int *y0, BOOL multiline)
{
	int x = *x0;
	int y = *y0;
	int y1;

	/* Remark from Hady, Dec 1, 2004. Here was a bug that can be reproduced as follows:
	   a) run XDS, b) create new file, c) move caret one step right, c) try to mark previous word - you'll get a crash.
	   The "if" right below is a fix.
	 */
	if ( buf->lines[y].s == 0 ) 
		return;

	x = tbuf_word_start (buf, x, y) - 1;
	y1 = multiline ? 0 : y;
	for (; y >= y1; y--, (x=buf->lines [y].len-1))
		for (;x >= 0; x--)
			if (IS_ALNUM (buf->lines [y].s[x]))
				goto found_alpha;
found_alpha:
	if (y < y1)	{
		*x0 = 0;
		*y0 = y1;
		return;
	}
	x = tbuf_word_start (buf, x, y);
	*x0 = x;
	*y0 = y;
}

/* ---------------------------------------------------------------- */

BOOL	tbuf_undo_insert (TBUF * buf, UNDO_REC * rec, int *x, int *y)
{
	*x = rec->x;
	*y = rec->y;

	if (!tbuf_delete_string (buf, rec->y, rec->x-1, 1))
		return FALSE;
	-- rec->n;
	-- rec->x;
	*x = rec->x;
	if (! rec->n)
		drop_last_undo (buf->undo_buffer);
	else
		change_last_undo (buf->undo_buffer, rec);
	return TRUE;
}

BOOL	tbuf_undo_insert_many (TBUF * buf, UNDO_REC * rec, int *x, int *y)
{
	*x = rec->x;
	*y = rec->y;

	if (!tbuf_delete_string (buf, rec->y, rec->x-rec->n, rec->n))
		return FALSE;
	rec->x -= rec->n;
	*x = rec->x;
	drop_last_undo (buf->undo_buffer);
	return TRUE;
}

BOOL	tbuf_undo_overwrite (TBUF * buf, UNDO_REC * rec, int *x, int *y)
{
	char * p = last_undo_string (buf->undo_buffer, 1);
	*x = rec->x;
	*y = rec->y;

	if (! tbuf_overwrite_char (buf, rec->y, rec->x-1, *p))
		return FALSE;
	-- rec->n;
	-- rec->x;
	*x = rec->x;
	drop_last_undo_string (buf->undo_buffer, 1);
	if (! rec->n)
		drop_last_undo (buf->undo_buffer);
	else
		change_last_undo (buf->undo_buffer, rec);
	return TRUE;
}

BOOL	tbuf_undo_delete (TBUF * buf, UNDO_REC * rec, int *x, int *y)
{
	char * p = last_undo_string (buf->undo_buffer, 1);
	*x = rec->x;
	*y = rec->y;
	if (! tbuf_insert_string (buf, rec->y, rec->x, p, 1))
		return FALSE;
	-- rec->n;
	*x = rec->x;
	drop_last_undo_string (buf->undo_buffer, 1);
	if (! rec->n)
		drop_last_undo (buf->undo_buffer);
	else
		change_last_undo (buf->undo_buffer, rec);
	return TRUE;
}

BOOL	tbuf_undo_delete_many (TBUF * buf, UNDO_REC * rec, int *x, int *y)
{
	char * p = last_undo_string (buf->undo_buffer, rec->n);
	*x = rec->x;
	*y = rec->y;
	if (! tbuf_insert_string (buf, rec->y, rec->x, p, rec->n))
		return FALSE;
	drop_last_undo_string (buf->undo_buffer, rec->n);
	drop_last_undo (buf->undo_buffer);
	return TRUE;
}

BOOL	tbuf_undo_insert_lines (TBUF * buf, UNDO_REC * rec, int *x, int *y)
{
	*x = rec->x;
	*y = rec->y;

	tbuf_delete_lines (buf, rec->y-rec->n, rec->n);
	drop_last_undo (buf->undo_buffer);
	return TRUE;
}

BOOL	tbuf_undo_delete_block (TBUF * buf, UNDO_REC * rec, int *x, int *y)
{
	char * p = last_undo_string (buf->undo_buffer, rec->n);
	*x = rec->x;
	*y = rec->y;

	if (!tbuf_stream_in_text (buf, p, rec->n, rec->x, rec->y))
		return FALSE;
	drop_last_undo_string (buf->undo_buffer, rec->n);
	drop_last_undo (buf->undo_buffer);
	return TRUE;
}

BOOL	tbuf_undo_insert_block (TBUF * buf, UNDO_REC * rec, int *x, int *y)
{
	int x1, y1;
	*x = rec->x;
	*y = rec->y;
	get_coord_rec (buf->undo_buffer, &x1, &y1);

	if (!tbuf_delete_block (buf, *x, *y, x1, y1))
		return FALSE;
	drop_last_coord_undo (buf->undo_buffer);
	return TRUE;
}

BOOL	tbuf_undo_cursor_move (TBUF * buf, UNDO_REC * rec, int *x1, int *y1, int *x2, int *y2)
{
	*x1 = rec->x;
	*y1 = rec->y;
	get_coord_rec (buf->undo_buffer, x2, y2);
	drop_last_coord_undo (buf->undo_buffer);
	return TRUE;
}

BOOL	tbuf_undo (TBUF * buf, int *x1, int *y1, int *x2, int *y2)
{
	UNDO_REC rec;
	BOOL b;
	long block;

	if (! test_undo (buf->undo_buffer)) return FALSE;
	if (!last_undo_record (buf->undo_buffer, &rec))
		return FALSE;
	block = rec.block_num;
	b = FALSE;
	buf->undo = NULL;
	for (;;)	{
		switch (rec.oper)	{
		case UNDO_INSERT:
			if (block)
				b = tbuf_undo_insert_many (buf, &rec, x1, y1);
			else
				b = tbuf_undo_insert (buf, &rec, x1, y1);
			break;
		case UNDO_OVERWRITE:
			b = tbuf_undo_overwrite (buf, &rec, x1, y1);
			break;
		case UNDO_DELETE:
			if (block)
				b = tbuf_undo_delete_many (buf, &rec, x1, y1);
			else
				b = tbuf_undo_delete (buf, &rec, x1, y1);
			break;
		case UNDO_INSERT_LINES:
			b = tbuf_undo_insert_lines (buf, &rec, x1, y1);
			break;
		case UNDO_DELETE_BLOCK:
			b = tbuf_undo_delete_block (buf, &rec, x1, y1);
			break;
		case UNDO_INSERT_BLOCK:
			b = tbuf_undo_insert_block (buf, &rec, x1, y1);
			break;
		case UNDO_CURSOR_MOVE:
			b = tbuf_undo_cursor_move (buf, &rec, x1, y1, x2, y2);
			break;
		}
		if (rec.oper != UNDO_CURSOR_MOVE)	{
			*x2 = *x1;
			*y2 = *y1;
		}

		if (!block) break;
		tbuf_set_undo (buf);
		if (!buf->undo_available) break;
		if (!last_undo_record (buf->undo_buffer, &rec))
			break;
		if (rec.block_num != block) break;
	}

	buf->undo = buf->undo_buffer;
	if (! undo_is_dirty (buf->undo))
		tbuf_make_dirty (buf, FALSE);
	return b;
}

BOOL	tbuf_last_cursor_pos (TBUF * buf, int *x1, int *y1, int *x2, int *y2)
{
	UNDO_REC rec;
	if (!last_undo_record (buf->undo_buffer, &rec))
		return FALSE;
	if (rec.code != buf->cur_edit)
		return FALSE;
	*x1 = rec.x;
	*x2 = rec.x;
	*y1 = rec.y;
	*y2 = rec.y;
	return TRUE;
}

void	tbuf_start_undo_block (TBUF * buf)
{
	if (buf->undo)
		start_undo_block (buf->undo);
}

void	tbuf_finish_undo_block (TBUF * buf)
{
	if (buf->undo)
		finish_undo_block (buf->undo);
}

BOOL	tbuf_store_cursor_pos (TBUF * buf, int x1, int y1, int x2, int y2)
{
	if (!buf->cur_edit || !buf->undo) return TRUE;

	return undo_store_cursor (buf->undo, buf->cur_edit, x1, y1, x2, y2);
}

BOOL	tbuf_undo_available (TBUF * buf)
{
	return test_undo (buf->undo_buffer);
}

void tbuf_set_oem(TBUF * buf, BOOL oem)
{
  buf->oem = oem;
}

BOOL tbuf_is_oem(const TBUF * buf)
{
  return buf->oem;
}

void tbuf_oem_to_ansi (TBUF * buf)
{
  int y;
  for(y = 0; y < buf->nlines; ++y)
    oemToAnsi(buf->lines[y].s, buf->lines[y].len);
}

void tbuf_ansi_to_oem (TBUF * buf)
{
  int y;
  for(y = 0; y < buf->nlines; ++y)
    ansiToOem(buf->lines[y].s, buf->lines[y].len);
}

/* ---------------------------------------------------------------- */

/* yet to be implemented:

clear_tbuf ()
set_line
add_line
do_delete_lines
move_in_line
move_text
stream_in_text
*/