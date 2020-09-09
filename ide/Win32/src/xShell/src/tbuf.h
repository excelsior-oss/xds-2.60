#ifndef _TBUF_H
#define _TBUF_H

typedef struct _TBUF TBUF;

#define LINE_END 0x3FFFFFFF

typedef enum	{
		EOL_DEFAULT,
		EOL_ZERO,
		EOL_CR,
		EOL_LF,
		EOL_CRLF
	}
		EOL_TYPE;

typedef	struct	{
		void (* line_changed) (void * ed, int i);
		void (* lines_changed) (void * ed, int i, int n);
		void (* lines_deleted) (void * ed, int i, int n);
		void (* lines_inserted) (void * ed, int i, int n);
		void (* dirty_changed) (void * ed, BOOL dirty, BOOL forced);
		void (* undo_changed) (void * ed, BOOL undo);
		void (* tab_changed) (void * ed);
		BOOL (* undo_trunc) (void * ed, BOOL drop, BOOL dropping_mode);
		void (* redraw_lines) (void * ed, int start, int fin);
	}
		EDIT_CALLBACKS;

typedef BOOL	(* TBUF_STREAM_FUNC) (char * s, int len, DWORD lparam);

extern		 BOOL	alpha [256];
extern	unsigned char	tolower_table [256];
extern	unsigned char	toupper_table [256];
extern	unsigned char	toothercase_table [256];

void	init_alpha (void);

TBUF *	create_tbuf	(int tab_size);
TBUF *	new_tbuf	(int tab_size);
void	tbuf_add_destructor (TBUF * buf, void * data, void (*fun) (void*));
void	free_tbuf	(TBUF * buf);
void	tbuf_addref	(TBUF * buf);
void	tbuf_release	(TBUF * buf);
void	clear_tbuf	(TBUF * buf);
int	tbuf_tab_size	(TBUF * buf);
void	tbuf_set_tab_size (TBUF * buf, int tab_size);
DWORD *	tbuf_get_parsing_data (TBUF * buf);
BOOL	tbuf_query_inv_lines (TBUF * buf, int * start, int * fin);
void	tbuf_validate_inv_lines (TBUF * buf);
void	tbuf_redraw_lines (TBUF * buf, int start, int fin);

void	tbuf_stop_line_notifications (TBUF * buf);
void	tbuf_continue_line_notifications (TBUF * buf);

BOOL	tbuf_set_text (TBUF * buf, char * p, unsigned long len, BOOL copy, BOOL eof);

void	tbuf_set_dirty	(TBUF * buf, BOOL dirty);
BOOL	tbuf_is_dirty	(TBUF * buf);
EOL_TYPE tbuf_eol_type	(TBUF * buf);
void	tbuf_set_eol_type (TBUF * buf, EOL_TYPE type);

long	tbuf_stream_out (TBUF * buf, char * eol, int eollen, TBUF_STREAM_FUNC fun, DWORD lparam);
long	tbuf_stream_out_block (TBUF * buf, int x1, int y1, int x2, int y2, char * eol, int eollen, TBUF_STREAM_FUNC fun, DWORD param);

BOOL	tbuf_add_line (TBUF * b, char * s, int len, BOOL copy);
BOOL	tbuf_get_nlines (TBUF * b);
BOOL	tbuf_get_ncolumns (TBUF * b);

BOOL	tbuf_get_line (TBUF * b, int i, char ** p, int * len);
int	tbuf_get_line_length (TBUF * b, int i);

long	tbuf_register_edit (TBUF * buf, void * edit, EDIT_CALLBACKS * cb);
void	tbuf_unregister_edit (TBUF * buf, void * edit);
void	tbuf_set_current_edit (TBUF * buf, long edit_code);

long	tbuf_add_bookmark	(TBUF * buf, int x, int y, BOOL keep);
void	tbuf_delete_bookmark	(TBUF * buf, long code);
void	tbuf_change_bookmark	(TBUF * buf, long code, int x, int y);
BOOL	tbuf_query_bookmark	(TBUF * buf, long code, int *x, int *y);
void	tbuf_move_bookmarks	(TBUF * buf, int x1, int y1, int x2, int y2, int x, int y);

BOOL	tbuf_overwrite_char	(TBUF * buf, int i, int x, char c);
BOOL	tbuf_insert_string	(TBUF * buf, int i, int x, char *s, int len);
BOOL	tbuf_delete_string	(TBUF * buf, int i, int x, int len);

BOOL	tbuf_delete_block	(TBUF * buf, int x1, int y1, int x2, int y2);
void	tbuf_delete_lines	(TBUF * buf, int i, int n);
BOOL	tbuf_insert_lines	(TBUF * buf, int i, int n);
BOOL	tbuf_move_text		(TBUF * buf, int x1, int y1, int x2, int y2, int x, int y);

BOOL	tbuf_stream_in_text	(TBUF * buf, char * p, int len, int x, int y);
void	tbuf_next_word		(TBUF * buf, int *x0, int *y0, BOOL multiline);
void	tbuf_prev_word		(TBUF * buf, int *x, int *y, BOOL multiline);
int	tbuf_word_end		(TBUF * buf, int x, int y);
int	tbuf_word_start		(TBUF * buf, int x, int y);

BOOL	tbuf_undo		(TBUF * buf, int *x1, int *y1, int *x2, int *y2);
BOOL	tbuf_last_cursor_pos	(TBUF * buf, int *x1, int *y1, int *x2, int *y2);
void	tbuf_start_undo_block	(TBUF * buf);
void	tbuf_finish_undo_block	(TBUF * buf);
BOOL	tbuf_store_cursor_pos	(TBUF * buf, int x1, int y1, int x2, int y2);
BOOL	tbuf_undo_available	(TBUF * buf);
void	tbuf_clear_undo		(TBUF * buf);

void tbuf_set_oem     (TBUF * buf, BOOL oem);
BOOL tbuf_is_oem      (const TBUF * buf);
void tbuf_oem_to_ansi (TBUF * buf);

#endif /* _TBUF_H */
