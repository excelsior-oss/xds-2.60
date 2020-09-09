
BOOL	Register_classes (void);
void	Create_pens (void);

extern	COLORREF GrayColor;
extern	COLORREF DarkCOlor;
extern	COLORREF LightColor;
extern	COLORREF TextColor;

extern	HPEN	LightPen;
extern	HPEN	DarkPen;
extern	HPEN	GrayPen;
extern	HPEN	TextPen;
extern	HBRUSH	GrayBrush;

extern	void	Draw_inset   (HDC hdc, RECT * rc);
extern	void	Draw_raised (HDC hdc, RECT * rc);
extern	void	Draw_3dframe (HDC hdc, RECT * rc);

/* Progress indicator messages */

#define	MSG_PROGRESS       (WM_USER+3000)
#define	MSG_PROGRESS_LIMIT (WM_USER+3001)
#define	MSG_PROGRESS_POS   (WM_USER+3002)
#define	MSG_PROGRESS_CLOSE (WM_USER+3003)

/* Tool Bar procedures */

BOOL	Load_and_dither (int res, HBITMAP * bmp, HBITMAP * dbmp);


/* Multi static procedures */

#define MSTATIC_LEFT	  0
#define MSTATIC_CENTER	  1
#define MSTATIC_RIGHT	  2
#define MSTATIC_INSET	100
#define MSTATIC_RAISED	101
#define MSTATIC_FRAME	102

BOOL	Add_static_item (HWND hwnd, RECT * rect, char * s, HFONT font, BOOL copy, int type);
void	Delete_statics (HWND hwnd, BOOL erase_all);
void	Clear_multi_static (HWND hwnd, RECT * rect);

void	Subclass_static (HWND hwnd);
int	Calc_font_height (HFONT f);

/* Colorbox procedures */

#define MSG_GET_DEFAULT_COLOR WM_USER+4000 /* wparam == control id */

void	Colorbox_fill (HWND hwnd, int id, COLORREF color);
void	Colorbox_update (HWND hwnd, int id, COLORREF color);
BOOL	Colorbox_measure (HWND hwnd, LPARAM lparam);
BOOL	Colorbox_draw_item (LPARAM lparam);
BOOL	Colorbox_notify (HWND hwnd, WPARAM wparam, LPARAM lparam, COLORREF * color);
COLORREF Colorbox_color (HWND hwnd, int id);
void	choose_color (HWND hwnd, COLORREF * color, char * title, BOOL solid);
