/* messages for MDI windows */

#define MSG_GETCLASS WM_USER+500
#define MSG_GETTITLE WM_USER+501
#define MSG_GETSAVEDATA WM_USER+502

typedef HWND	(* REGISTERPROC) (char * title, MDIPLACEMENT * pl, char * extra);
void	Register_child_class (char * class, REGISTERPROC pr);

char * get_word (char **s);

void	Init_frame (HWND hwnd);
void	Make_windows_menu (void);
void	MoreWindows (void);
void	Read_windows_options (void);
void	Configure_windows (void);
