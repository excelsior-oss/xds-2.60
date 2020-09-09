
typedef struct _EDITOR EDITOR;

typedef struct	{
		int x1, x2, y1, y2;
		BOOL is_block;
		BOOL insert_mode;
		int nlines;
		BOOL has_focus;
		int result;
	}
		EDIT_NOTIFY;

#define EDIT_NOTIFY_CURSOR	1
#define EDIT_NOTIFY_TAB		2
#define EDIT_NOTIFY_CR		3
#define EDIT_NOTIFY_FOCUS	4
#define EDIT_NOTIFY_CLIPBOARD	5
#define EDIT_NOTIFY_DIRTY	6
#define EDIT_NOTIFY_UNDO	7
#define EDIT_NOTIFY_TRUNCUNDO	8
#define EDIT_NOTIFY_DROPUNDO	9
#define EDIT_NOTIFY_SETBOOKMARK 10

typedef void (* EDIT_NOTIFY_PROC) (int code, EDIT_NOTIFY *data, DWORD param);

BOOL	Editor_register		(void);
HWND	Editor_create		(TBUF * buf, HWND owner);
void	Editor_error		(HWND edit);
int	Editor_nlines		(HWND edit);
void	Editor_set_block	(HWND edit, int x1, int y1, int x2, int y2);
void	Editor_set_block_visible (HWND edit, int x1, int y1, int x2, int y2);
BOOL	Editor_get_block	(HWND edit, int *x1, int *y1, int *x2, int *y2);
void	Editor_get_line_screen_pos (HWND edit, int y, int * y1, int * y2);
void	Editor_set_cursor	(HWND edit, int x, int y);
void	Editor_set_notify_handler (HWND edit, EDIT_NOTIFY_PROC handler, DWORD param);
int	Editor_log_to_phys	(HWND edit, int line, int logx);
int	Editor_phys_to_log	(HWND edit, int line, int x);
BOOL	Editor_undo		(HWND edit);

void	Editor_copy	(HWND edit);
void	Editor_paste	(HWND edit);
void	Editor_cut	(HWND edit);
void	Editor_exchange (HWND edit);
void	Editor_add	(HWND edit);

void	Editor_set_insert (HWND edit, BOOL insert);

typedef enum	{
		BS_JOINS_LINES,
		DEL_JOINS_LINES,
		CR_SPLITS_LINE,
		TAB_TYPES,
		TAB_AS_SPACES,
		SMART_TABS,
		CR_INDENTS,
		EDITOR_FONT,
		EDITOR_SHOW_LEFT_MARGIN,
		EDITOR_SHOW_RIGHT_MARGIN,
		EDITOR_RIGHT_MARGIN,
		EDITOR_SHOW_TABS,
		EDITOR_SHOW_BOOKMARKS,
		EDITOR_PASTE_POS,
		EDITOR_BLOCK_TYPE,
		EDITOR_BLOCK_PASTE,
		EDITOR_TAB_SIZE
	}
		EDITPARAM;

void	Editor_set_param (HWND edit, EDITPARAM param, DWORD value);
void	Editor_set_buf	 (HWND edit, TBUF * buf);
void	Editor_set_highlight (HWND edit, int line);

void	Editor_set_bookmark    (HWND edit, int num);
void	Editor_goto_bookmark   (HWND edit, int num);
BOOL	Editor_test_bookmark   (HWND edit, int num);
void	Editor_delete_bookmark (HWND edit, int num);

void	Editor_set_read_only (HWND edit, BOOL read_only);
BOOL	Editor_get_read_only (HWND edit);

void	Editor_set_lang_token (HWND edit, DWORD token);

void Editor_encoding_Windows (HWND edit);
void Editor_encoding_DOS (HWND edit);
void Editor_flip_encoding (HWND edit);