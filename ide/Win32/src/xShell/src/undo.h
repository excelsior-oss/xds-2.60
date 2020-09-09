#ifndef _UNDO_H
#define _UNDO_H

typedef BOOL (* QUERY_UNDO_TRUNC_FUNC) (void * param, BOOL drop, BOOL dropping_mode);

typedef struct _UNDO_BUFFER UNDO_BUFFER;

typedef struct	{
		long code;
		int x, y;	/* cursor after operation */
		int n;
		long block_num;
		char oper;
	}
		UNDO_REC;

#define UNDO_INSERT	1
/* len = number of characters inserted */

#define UNDO_OVERWRITE	2
/* len = number of characters overwritten;
   old characters are before record */

#define UNDO_DELETE	3
/* len = number of characters deleted (within one line)
   old characters are before record */

#define UNDO_INSERT_LINES 4

#define UNDO_DELETE_BLOCK 5

#define UNDO_INSERT_BLOCK 6
/* x,y define beginning of block; x0, y0 in coord structure define end of block */

#define UNDO_CURSOR_MOVE 7
/* x,y define beginning of block; x0, y0 in coord structure define end of block */

UNDO_BUFFER * create_undo_buffer (long limit, QUERY_UNDO_TRUNC_FUNC f, void * param);
void	clear_undo_buffer	(UNDO_BUFFER * b);
void	free_undo_buffer	(UNDO_BUFFER * b);

void	start_undo_block	(UNDO_BUFFER * b);
void	finish_undo_block	(UNDO_BUFFER * b);

BOOL	undo_is_dirty		(UNDO_BUFFER * b);

BOOL	undo_store_insert	(UNDO_BUFFER * b, int n, long code, int x, int y);
BOOL	undo_store_overwrite	(UNDO_BUFFER * b, int n, long code, int x, int y, char * c);
BOOL	undo_store_delete	(UNDO_BUFFER * b, int n, long code, int x, int y, char * c);
BOOL	undo_store_insert_lines (UNDO_BUFFER * b, int n, long code, int y);
BOOL	undo_store_delete_block	(UNDO_BUFFER * b, int n, long code, int x, int y, char ** d);
BOOL	undo_store_insert_block	(UNDO_BUFFER * b, long code, int x1, int y1, int x2, int y2);
BOOL	undo_store_cursor	(UNDO_BUFFER * b, long code, int x1, int y1, int x2, int y2);

BOOL	test_undo (UNDO_BUFFER * b);
BOOL	last_undo_record (UNDO_BUFFER * b, UNDO_REC * rec);
void	change_last_undo (UNDO_BUFFER * b, UNDO_REC * rec);
void	drop_last_undo	 (UNDO_BUFFER * b);
char * 	last_undo_string (UNDO_BUFFER * b, int n);
void	drop_last_undo_string	(UNDO_BUFFER * b, int n);
void	drop_last_coord_undo	(UNDO_BUFFER * b);

void	put_coord_rec (UNDO_BUFFER * b, int  x0, int  y0);
void	get_coord_rec (UNDO_BUFFER * b, int *x0, int *y0);

#endif /* _UNDO_H */
