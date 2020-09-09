#define WIN32_LEAN_AND_MEAN	/* do not include a lot of windows stuff */
#define NOIME			/* prevent #include <ime.h> */
#define NOMCX			/* prevent #include <mcx.h> */
#define NOSERVICE		/* prevent #include <winsvc.h>	*/
#define OEMRESOURCE

#include "windows.h"
#include <commdlg.h>
#include <commctrl.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define STRLEN 2000

extern	HINSTANCE MyInstance;
extern	HINSTANCE ResInstance;
extern	int	lock_help;

#define scpy(dest,src) zscpy ((dest), (src), sizeof (dest))

#define IGNORE_PARAM(x) (void)(x)

void	skip_spaces (char ** s);
char *  zscpy (char * dest, char * src, size_t len);
char *	alloc_copy (char * src);

#define NOFILE 0

extern char ** filedir;

void	clear_filedir (void);
int	add_filedir  (char * filename);
int	find_filedir (char * filename);
int	new_filedir  (char * filename);
void	set_real_file_name (char * filename);

void	text_size (HFONT font, char * text, int * x, int * y);
int	text_width (HFONT font, char * text);

COLORREF light_color (COLORREF c);

BOOL	is_of_class (HWND hwnd, char * class);
void	Get_child_pos (HWND hwnd, RECT * rc);
void	Center_window (HWND hwnd);
void	Center_and_resize_window (HWND hwnd, int xs, int ys);
WNDPROC	Subclass_control (HWND wnd, WNDPROC newproc);
void	Set_lb_extent (HWND hwnd);

BOOL	Ask_text   (char * prompt, char * title, char * result, int result_len, UINT flags);
BOOL	Ask_number (char * prompt, char * title, int * result, UINT flags);

#define ASK_INIT 1
#define ASK_SELECT 2
#define ASK_NUMBER 4
#define ASK_POSITIVE 8

int	Find_popup (HMENU menu);
int	Dialog_help	(int id);
int	Menu_help	(int id, BOOL popup);
void	Control_id_help (int dlg_id, int id);
int	Control_help	(int dlg_id, LPARAM lparam);
int	Page_Control_help (HWND hwnd, int dlg_id, LPARAM lparam);

void * Malloc (int size);

/*----------------------------------------------------------------------------*/
void	Read_section (char * section [][2], int maxlen, char * name, char * file);

BOOL	Add_section_elem (char * section [][2], int maxlen,
			  char * key, char * value,
			  char * name, char * file);

void	Delete_section_item (char * section [][2], int maxlen,
			     int i, char * name, char * file);


BOOL	Write_section (char * section [][2], int maxlen,
		       char * name, char * file);

void	read_ini_file (char * section, char * item, int Res, char * dest, int len, char * file);

/*----------------------------------------------------------------------------*/

void	Insert_history (char * history [], int maxlen, char * s);
void	Fill_history_box (HWND hwnd, char * history [], int maxlen);
BOOL	Add_history_to_menu (HMENU menu, char * history [], int maxlen,
			     int id_after, int first_id);

void	Write_history (char * history [], int maxlen, char * name, char * file);
void	Read_history  (char * history [], int maxlen, int elemlen,
			char * name, char * file);
/*----------------------------------------------------------------------------*/

void	tracemsg (char * prefix, UINT msg, WPARAM wparam, LPARAM lparam);

/* ---------------------------------------------------------- */
/* Parameters for WM_COMMAND message */

#define CONTROL_ID   ((UINT) LOWORD (wparam))
#define NOTIFY_CODE  ((UINT) HIWORD (wparam))
#define CONTROL_HWND ((HWND) lparam)

/* Parameters for WM_MDIACTIVATE message */

#define WND_TO_ACTIVATE   ((HWND) lparam)
#define WND_TO_DEACTIVATE ((HWND) wparam)

/* ------------------------------------------------------------ */

extern	int	ErrCode;

char * EText	   (DWORD err);
char * ErrText     (void);
char * LastErrText (void);


/* ------------------------------------------------------------ */

HPALETTE CreateDIBPalette (LPBITMAPINFO lpbmi, int * NumColors);
HBITMAP LoadResourceBitmap (HINSTANCE hInstance, LPSTR lpString,
			    HPALETTE * lphPalette);

void ansiToOem(char *text, int len);
void oemToAnsi(char *text, int len);