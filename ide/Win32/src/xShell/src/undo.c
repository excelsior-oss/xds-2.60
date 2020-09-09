#include <windows.h>
#include <stdlib.h>
#include "undo.h"

struct	_UNDO_BUFFER {
		char *	base_addr;
		long	limit;
		long	alloc;
		long	ptr;
		long	block_num;
		long	skipped_block;
		int	block_nest;
		BOOL	dirty_start;
		BOOL	dropping_undo;
		QUERY_UNDO_TRUNC_FUNC trunc_func;
		void *	param;
};

#define PAGE_ALIGN_UP(x) ((x) + page_size - 1) / page_size * page_size

typedef struct { int x0, y0; } COORD_REC;

#define GET_REC(b, rec) memcpy (&rec, (UNDO_REC*)(b->base_addr+b->ptr)-1, sizeof (UNDO_REC))
#define PUT_REC(b, rec) memcpy ((UNDO_REC*)(b->base_addr+b->ptr)-1, &rec, sizeof (UNDO_REC))
long	page_size;
long	cur_block_num = 0;
BOOL	just_dropped = FALSE;
long	UndoAllocLimit = 16*1024*1024;

UNDO_BUFFER * create_undo_buffer (long limit, QUERY_UNDO_TRUNC_FUNC f, void * param)
{
	SYSTEM_INFO si;
	UNDO_BUFFER * b = malloc (sizeof (UNDO_BUFFER));
	if (!b) return NULL;
	GetSystemInfo (&si);
	page_size = si.dwPageSize;
	limit = PAGE_ALIGN_UP (limit);
	if (limit > UndoAllocLimit) limit = UndoAllocLimit;
	b->base_addr = VirtualAlloc (NULL, UndoAllocLimit, MEM_RESERVE, PAGE_READWRITE);
	if (!b->base_addr)	{
		free (b);
		return NULL;
	}
	b->limit = limit;
	b->alloc = 0;
	b->ptr = 0;
	b->block_num = 0;
	b->skipped_block = 0;
	b->block_nest = 0;
	b->dirty_start = FALSE;
	b->trunc_func = f;
	b->param = param;
	b->dropping_undo = FALSE;
	return b;
}

void	clear_undo_buffer (UNDO_BUFFER * b)
{
	b->ptr = 0;
	b->dirty_start = FALSE;
}

void	free_undo_buffer (UNDO_BUFFER * b)
{
	VirtualFree (b->base_addr, 0, MEM_RELEASE);
	free (b);
}

BOOL	undo_check_alloc (UNDO_BUFFER * b, long size, long limit)
{
	long d;

	if (size > limit)	{
		if (!b->trunc_func (b->param, TRUE, b->dropping_undo))
			return FALSE;
		b->dirty_start = b->dirty_start || b->ptr != 0;
		b->ptr = 0;
		just_dropped = TRUE;
		b->skipped_block = b->block_num;
		return FALSE;
	}
	if (b->ptr + size > limit)	{
		if (!b->trunc_func (b->param, FALSE, b->dropping_undo))
			return FALSE;
		b->dropping_undo = TRUE;
		d = PAGE_ALIGN_UP (b->ptr + size - limit);
		b->dirty_start = b->dirty_start || b->ptr != 0;
		if (b->ptr < d)
			b->ptr = 0;
		else	{
			memmove (b->base_addr, b->base_addr + d, b->ptr-d);
			b->ptr -= d;
		}
	}
	return TRUE;
}

char *	alloc_undo_buffer (UNDO_BUFFER * b, long size)
{
	long a;

	just_dropped = FALSE;
	if (b->ptr + size <= b->alloc)	{
		b->ptr += size;
		return b->base_addr + b->ptr - size;
	}
	if (!undo_check_alloc (b, size, b->limit))
		return NULL;
	a = PAGE_ALIGN_UP (b->ptr+size-b->alloc);
	if (!VirtualAlloc (b->base_addr+b->alloc, a, MEM_COMMIT, PAGE_READWRITE))	{
		if (!undo_check_alloc (b, size, b->alloc))
			return NULL;
	} else
		b->alloc += a;
	b->ptr += size;
	return b->base_addr + b->ptr - size;
}

BOOL	undo_is_dirty (UNDO_BUFFER * b)
{
	if (b->ptr) return TRUE;
	return b->dirty_start;
}

void	start_undo_block (UNDO_BUFFER * b)
{
	if (!b->block_num)
		b->block_num = ++ cur_block_num;
	b->block_nest ++;
	b->skipped_block = 0;
}

void	finish_undo_block (UNDO_BUFFER * b)
{
	if (--b->block_nest <= 0) {
		b->block_num = 0;
		b->block_nest = 0;
	}
	b->skipped_block = 0;
}

BOOL	test_undo (UNDO_BUFFER * b)
{
	UNDO_REC rec;
	int n;

	if (!b->ptr) return FALSE;
	if (b->ptr < sizeof (UNDO_REC))	{
		b->ptr = 0;
		return FALSE;
	}
	GET_REC (b, rec);
	switch (rec.oper)	{
	case UNDO_INSERT: return TRUE;
	case UNDO_INSERT_LINES: return TRUE;
	case UNDO_OVERWRITE:
	case UNDO_DELETE:
	case UNDO_DELETE_BLOCK: n = rec.n; break;
	case UNDO_INSERT_BLOCK:
	case UNDO_CURSOR_MOVE: n = sizeof (COORD_REC); break;
	default: return TRUE;
	}
	if (b->ptr < n)	{
		b->ptr = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL	last_undo_record (UNDO_BUFFER * b, UNDO_REC * rec)
{
	if (!b->ptr) return FALSE;
	GET_REC (b, *rec);
	return TRUE;
}

char * 	last_undo_string (UNDO_BUFFER * b, int n)
{
	if (!b->ptr) return NULL;
	return b->base_addr + b->ptr - sizeof (UNDO_REC) - n;
}

void	change_last_undo (UNDO_BUFFER * b, UNDO_REC * rec)
{
	if (b->ptr)
		PUT_REC (b, *rec);
}

void	drop_last_undo (UNDO_BUFFER * b)
{
	if (b->ptr)
		b->ptr -= sizeof (UNDO_REC);
}

void	drop_last_coord_undo (UNDO_BUFFER * b)
{
	if (b->ptr)
		b->ptr -= sizeof (UNDO_REC) + sizeof (COORD_REC);
}

void	drop_last_undo_string (UNDO_BUFFER * b, int n)
{
	UNDO_REC r;
	if (!b->ptr) return;
	GET_REC (b, r);
	b->ptr -= n;
	PUT_REC (b, r);
}

BOOL	undo_store_insert (UNDO_BUFFER * b, int n, long code, int x, int y)
{
	UNDO_REC rec;
	if (b->ptr)	{
		GET_REC (b, rec);
		if (rec.code == code && rec.oper == UNDO_INSERT &&
		    rec.block_num == b->block_num &&
		    rec.y == y && rec.x == x-n)
		{
			rec.n += n;
			rec.x += n;
			PUT_REC (b, rec);
			return TRUE;
		}
	}
	if (! alloc_undo_buffer (b, sizeof (UNDO_REC))) return just_dropped;
	rec.x = x;
	rec.y = y;
	rec.oper = UNDO_INSERT;
	rec.n = n;
	rec.code = code;
	rec.block_num = b->block_num;
	PUT_REC (b, rec);
	return TRUE;
}

BOOL	undo_store_overwrite (UNDO_BUFFER * b, int n, long code, int x, int y, char * c)
{
	UNDO_REC rec;
	if (b->ptr)	{
		GET_REC (b, rec);
		if (rec.code == code && rec.oper == UNDO_OVERWRITE &&
		    rec.block_num == b->block_num &&
		    rec.y == y && rec.x == x-n)
		{
			if (! alloc_undo_buffer (b, n)) return just_dropped;
			memcpy (b->base_addr + b->ptr - sizeof (UNDO_REC) - n, c, n);
			rec.n += n;
			rec.x += n;
			PUT_REC (b, rec);
			return TRUE;
		}
	}
	if (! alloc_undo_buffer (b, n + sizeof (UNDO_REC))) return just_dropped;
	rec.x = x;
	rec.y = y;
	rec.oper = UNDO_OVERWRITE;
	rec.n = n;
	rec.code = code;
	rec.block_num = b->block_num;
	memcpy (b->base_addr + b->ptr - sizeof (UNDO_REC) - n, c, n);
	PUT_REC (b, rec);
	return TRUE;
}

BOOL	undo_store_delete (UNDO_BUFFER * b, int n, long code, int x, int y, char * c)
{
	UNDO_REC rec;
	if (b->ptr)	{
		GET_REC (b, rec);
		if (rec.code == code && rec.oper == UNDO_DELETE &&
		    rec.block_num == b->block_num &&
		    rec.y == y && rec.x == x)
		{
			if (! alloc_undo_buffer (b, n)) return just_dropped;
			memcpy (b->base_addr + b->ptr - sizeof (UNDO_REC) - n, c, n);
			rec.n += n;
			PUT_REC (b, rec);
			return TRUE;
		}
	}
	if (! alloc_undo_buffer (b, n + sizeof (UNDO_REC))) return just_dropped;
	rec.x = x;
	rec.y = y;
	rec.oper = UNDO_DELETE;
	rec.n = n;
	rec.code = code;
	rec.block_num = b->block_num;
	memcpy (b->base_addr + b->ptr - sizeof (UNDO_REC) - n, c, n);
	PUT_REC (b, rec);
	return TRUE;
}

BOOL	undo_store_insert_lines (UNDO_BUFFER * b, int n, long code, int y)
{
	UNDO_REC rec;
	if (! alloc_undo_buffer (b, sizeof (UNDO_REC))) return just_dropped;
	rec.x = 0;
	rec.y = y;
	rec.oper = UNDO_INSERT_LINES;
	rec.n = n;
	rec.code = code;
	rec.block_num = b->block_num;
	PUT_REC (b, rec);
	return TRUE;
}

BOOL	undo_store_delete_block	(UNDO_BUFFER * b, int n, long code, int x, int y, char ** d)
{
	UNDO_REC rec;
	*d = NULL;
	if (! alloc_undo_buffer (b, n + sizeof (UNDO_REC))) return just_dropped;
	rec.x = x;
	rec.y = y;
	rec.oper = UNDO_DELETE_BLOCK;
	rec.n = n;
	rec.code = code;
	rec.block_num = b->block_num;
	PUT_REC (b, rec);
	*d = b->base_addr + b->ptr - sizeof (UNDO_REC) - n;
	return TRUE;
}

void	put_coord_rec (UNDO_BUFFER * b, int x0, int y0)
{
	COORD_REC c;
	c.x0 = x0;
	c.y0 = y0;
	memcpy (b->base_addr + b->ptr - sizeof (UNDO_REC) - sizeof (COORD_REC),
		&c, sizeof (COORD_REC));
}

void	get_coord_rec (UNDO_BUFFER * b, int *x0, int *y0)
{
	COORD_REC c;
	memcpy (&c, b->base_addr + b->ptr - sizeof (UNDO_REC) - sizeof (COORD_REC),
		sizeof (COORD_REC));
	*x0 = c.x0;
	*y0 = c.y0;
}

BOOL	undo_store_insert_block	(UNDO_BUFFER * b, long code, int x1, int y1, int x2, int y2)
{
	UNDO_REC rec;
	if (! alloc_undo_buffer (b, sizeof (UNDO_REC) + sizeof (COORD_REC))) return just_dropped;
	rec.x = x1;
	rec.y = y1;
	rec.oper = UNDO_INSERT_BLOCK;
	rec.n = 0;
	rec.code = code;
	rec.block_num = b->block_num;
	PUT_REC (b, rec);
	put_coord_rec (b, x2, y2);
	return TRUE;
}

BOOL	undo_store_cursor (UNDO_BUFFER * b, long code, int x1, int y1, int x2, int y2)
{
	UNDO_REC rec;
	if (! alloc_undo_buffer (b, sizeof (UNDO_REC) + sizeof (COORD_REC))) return just_dropped;
	rec.x = x1;
	rec.y = y1;
	rec.oper = UNDO_CURSOR_MOVE;
	rec.n = 0;
	rec.code = code;
	rec.block_num = b->block_num;
	PUT_REC (b, rec);
	put_coord_rec (b, x2, y2);
	return TRUE;
}
