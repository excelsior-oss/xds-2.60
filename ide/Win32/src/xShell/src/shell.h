#ifndef WIN32
#error --- to be compiled foe Win32 only ---
#endif

/* ---------- class names for MDI child windows ------------- */

#define EditClass     "ShellMDIEdit"
#define MessageClass  "ShellMDIMessage"
#define FileListClass "ShellMDIFileList"
#define SearchListClass "ShellMDISearchList"

/* ---------------------------------------------------------- */

extern	HICON	EditSmIcon;
extern	HICON	FListSmIcon;
extern	HICON	MessageSmIcon;
extern	HICON	Console_icon;

/* ---------------------------------------------------------- */

#define DEFAULT_COLOR (COLORREF) (0xFFFFFFFF)
#define AbsColor(color, def) ((color) == DEFAULT_COLOR ? (def) : (color))
#define AbsSysColor(color, sys) ((color) == DEFAULT_COLOR ? (GetSysColor (sys)) : (color))


extern	BOOL	DemoMode;
extern	BOOL	HasExpired;
extern	long	ExpireDate;
extern	HWND	FrameWindow;
extern	HWND	FoundHWND;
extern	int	SystemFontHeight;
extern	int	WinVersion;

char HomeDir	[_MAX_PATH+1];
char StartDir	[_MAX_PATH+1];
char IniFile	[_MAX_PATH+1];
char HelpFile	[_MAX_PATH+1];
char IDEHelpFile [_MAX_PATH+1];
char ProjDir	[_MAX_PATH+1];
char ProjIni	[_MAX_PATH+1];
char RedFile	[_MAX_PATH+1];
char XDSMAIN	[_MAX_PATH+1];

extern	char	ShellName	[];
extern	char	OptFileName	[];
extern	char	RedFileName	[];
extern	char	XRedFileName	[];
extern	char	IniFileName	[];
extern	char	CfgFileName	[];
extern	char	FileName	[];

extern	int	ProjFile;
extern	int	OptFile;
extern	BOOL	Cancelled;
extern	BOOL	DoWriteFileMap;
extern	BOOL	DoReadFileMap;
extern	BOOL	SameConsole;

extern	HWND	ToolBar, StatusBar;

extern	HMENU	ToolsMenu;
extern	HACCEL	ToolAccel;

extern char * DebugCmd;

void	Set_status_bar	 (HWND owner, char * s);
void	Clear_status_bar (HWND owner);


void	Set_title (char * name);
void    Enable_item (int item, BOOL enable);
void	Disable_all_items (void);
void	Read_windows  (char * inifile);
void	Write_windows (char * inifile);
BOOL	CloseAll (void);

BOOL	Find_help_file (void);

int	Dialog (int dlg, DLGPROC proc);
void	PropertyDialog (PROPSHEETHEADER * psh);
int	MsgBox (char * header, UINT style, char * fmt, ...);
