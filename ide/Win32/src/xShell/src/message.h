#define ERRSIZE 128

extern	HWND	MessageWnd;

typedef	struct	_ErrLine {
		char	body [ERRSIZE];
		int	file;
		long	errnum;
		long	x, y;
		long	bookmark;
		BOOL	present;
		BOOL	was_dirty;
		int	num;
		int	msg_class;
		int	helpfile;
		struct _ErrLine * next;
		struct _ErrLine * prev;
	}
		ErrLine;

extern ErrLine * err_list;
extern ErrLine * cur_err;

extern	int	msg_count;
extern	int	warn_count;
extern	int	err_count;
extern	int	severe_count;

extern	BOOL	sort_errors;
extern	int	errors_help_file;

BOOL	RegisterMessage (void);
void	Set_help_file (int file);
void	Add_error (char * body, long errnum, long x, long y, int file, int class);
void	Go_to_error (void);
void	Clear_messages (void);
void	Show_messages (void);
void	Hide_messages (void);
void	First_message (void);
void	Next_message (void);
void	Prev_message (void);
void	Msg_proj_enable (BOOL proj);
void	Refresh_message_window (void);
