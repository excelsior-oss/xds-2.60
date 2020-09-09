#ifndef TBUF_DEFINED
#include "tbuf.h"
#endif

#define OPEN_EXISTING_BUFFER		0x01
#define SHARE_EXISTING_BUFFER		0x02
#define ASK_SHARE_EXISTING_BUFFER	0x04

typedef struct	_EDIT_DATA EDIT_DATA;

extern	int		EditStatusBarLock;
extern	EDIT_DATA *	ProjData;

#define SEARCH_TEXT_LEN 300
#define SEARCH_HISTORY_LEN 100

extern	char	search_text [SEARCH_TEXT_LEN];
extern	char *	search_history [SEARCH_HISTORY_LEN];

BOOL		Register_edit	(void);

HWND		Edit_find_loaded_file	(int file);
HWND		Edit_read_text		(int file, char * origname, int how, BOOL show);
HWND		Edit_new		(void);
void		Edit_show		(HWND hwnd);
void		Edit_highlight		(EDIT_DATA * ed, int y, int x);
void		Edit_drop_highlight	(void);
BOOL		Edit_save_all		(BOOL prompt);
void		Edit_test_reload	(BOOL prompt);
void		Edit_set_buf		(HWND hwnd, TBUF * buf);
TBUF *		Edit_get_buf		(HWND hwnd);

int	Make_editor_list (int ** list);
EDIT_DATA * Edit_find_loaded_buf (int file);
EDIT_DATA * Edit_find_loaded_file_name (char * filename);

long	Edit_process_error (int file, int x, int y, BOOL * was_dirty);
void	Edit_remove_error	(int file, long bookmark);
void	Edit_highlight_error	(HWND hwnd, long bookmark);
BOOL	Edit_goto_bookmark	(HWND hwnd, long bookmark);
BOOL	Edit_test_line_found (EDIT_DATA * ed,
			      char search_text [],
			      BOOL search_case,
			      BOOL search_prefix,
			      BOOL search_suffix);

BOOL	EditSave (EDIT_DATA * ed);
void	Refresh_edits (void);
void	Refresh_edit_tokens (void);
