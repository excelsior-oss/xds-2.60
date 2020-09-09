#include "tbuf.h"

/* General options */

extern	BOOL	OpenLastProject;
extern	BOOL	ShowLogo;
extern	BOOL	FlatToolBar;

extern	char *	open_filter;

/* Colors used */

extern	COLORREF ShellColor;

extern	COLORREF WndForeColor, WndBackColor;

extern	COLORREF ListForeColor, ListBackColor;
extern	COLORREF MsgForeColor, MsgBackColor;

/* Edit versions of colors */

extern	COLORREF temp_ShellColor;

extern	COLORREF temp_WndForeColor, temp_WndBackColor;

extern	COLORREF temp_ListForeColor, temp_ListBackColor;
extern	COLORREF temp_MsgForeColor, temp_MsgBackColor;

/* Fonts used */

extern	HFONT	 EditFont, MsgFont, ListFont;
extern	HFONT	 CompileFont;
extern	int	 CompileFontSize;

extern	HFONT	temp_EditFont;

/* Editor options */

extern	int	TabSize;
extern	BOOL	AutoSave;
extern	BOOL	BakFiles;
extern	BOOL	ClearUndoOnSave;
extern	BOOL	NewEditorInsert;

/* File save options */

extern	EOL_TYPE Default_eol_type;
extern	BOOL	 Autodetect_eol;
extern	BOOL	 ReadEOF;
extern	BOOL	 WriteEOF;

/* Print options */

extern	BOOL	PrintLineNumbers;
extern	BOOL	PrintOnNextPage;
extern	BOOL	PrintOnNextLine;
extern	BOOL	PrintFileName;
extern	BOOL	PrintFilePath;
extern	BOOL	PrintPageNumbers;

extern	LOGFONT	PrintLogFont;
extern	int	PrintFontSize;
extern	RECT	PrintMargins;

/* Search options */

extern	BOOL	search_insert_current;
extern	BOOL	replace_insert_current;
extern	BOOL	search_case;
extern	BOOL	search_advanced;
extern	BOOL	search_prefix;
extern	BOOL	search_suffix;
extern	BOOL	search_fromcursor;
extern	BOOL	search_back;

extern	BOOL	msearch_subdir;
extern	BOOL	msearch_in_opened;
extern	BOOL	msearch_in_project;
extern	BOOL	msearch_in_files;

extern	char msearch_path [STRLEN];
extern	char msearch_mask [300];

/* Run options */

extern	char	RunArguments [STRLEN];
extern	char	ExeFile	     [STRLEN];
extern	char	RunStartDir  [STRLEN];
extern	int	RunMode;
extern	BOOL	SameConsole;

/* Windows options */

extern	BOOL	DockMessages;
extern	BOOL	DockFList;
extern	BOOL	DockSearchResults;

/* New project options */

extern	char	std_red_file [];
extern	char	std_template_file [];

/* ------------------------------------------------------------------ */

extern	void	Read_open_filter (void);
extern	COLORREF Read_color (char * name, BOOL solid);
extern	void	Write_color (COLORREF c, char * name);
BOOL	Read_bool_color_option (char * name, BOOL dfl);
void	Write_bool_color_option (char * name, BOOL val);

extern	void	Read_options (void);
extern	BOOL    Configure_print (void);
extern	BOOL    Configure_general (void);
extern  BOOL    Configure_new_project (void);
extern	void    Configure_run (void);
extern  void    Configure_editor (void);
extern	void    Configure_languages (void);
extern  void    Configure_colors (void);
extern  BOOL    Register_config (void);

extern	void	Read_newproj_ini_file (char * dirstring, char * defmainmod);
extern	void	Read_run_options (void);

extern	void	Set_editor_params (HWND edit);
extern	void	Write_compiler_font (void);

extern	BOOL	choose_font (HFONT *font, int * size, char * title, BOOL fixed);

void	Write_search_settings (void);
void	Write_search_history (void);
void	Read_search_settings (void);
