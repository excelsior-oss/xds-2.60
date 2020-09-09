#include "util.h"
#include "shell.h"
#include "tbuf.h"
#include "mdi.h"
#include "syntax.h"
#include "config.h"
#include "classes.h"
#include "toolbar.h"
#include "var.h"
#include "edit.h"
#include "tbuf.h"
#include "pooredit.h"
#include "flist.h"
#include "message.h"
#include "tools.h"
#include "make.h"
#include "res.h"
#include "xdsres.h"

/* General options */

BOOL	OpenLastProject = TRUE;
BOOL	ShowLogo = TRUE;
BOOL	FlatToolBar = TRUE;

/* Colors used */

COLORREF ShellColor;

COLORREF WndForeColor, WndBackColor;

COLORREF ListForeColor, ListBackColor;
COLORREF MsgForeColor, MsgBackColor;

/* Edit versions of colors */

COLORREF temp_ShellColor;

COLORREF temp_WndForeColor, temp_WndBackColor;

COLORREF temp_ListForeColor, temp_ListBackColor;
COLORREF temp_MsgForeColor, temp_MsgBackColor;

/* Fonts used */

HFONT	 EditFont, MsgFont, ListFont;
HFONT	 CompileFont;
HFONT	 temp_EditFont;
int	 EditFontSize, MsgFontSize, ListFontSize, CompileFontSize;

/* Editor options */

int	TabSize;
BOOL	HardTabs;
BOOL	PrevLineTabs;
BOOL	AutoIndent;
BOOL	AutoSave;
BOOL	BakFiles;
BOOL	ClearUndoOnSave;
BOOL	OemConvert;
BOOL	BSJoinsLines;
BOOL	DelJoinsLines;
BOOL	CrSplitsLine;
BOOL	TabTypes;
BOOL	NewEditorInsert;
BOOL	ShowLeftMargin;
BOOL	ShowRightMargin;
BOOL	ShowTabs;
BOOL	ShowBookmarks;
int	PastePosition;
int	TypeWhenBlock;
int	PasteWhenBlock;
int	RightMargin;

/* File save options */

EOL_TYPE Default_eol_type = EOL_CRLF;
BOOL	 Autodetect_eol = TRUE;
BOOL	 ReadEOF = TRUE;
BOOL	 WriteEOF = FALSE;

/* Print options */

BOOL	PrintLineNumbers;
BOOL	PrintOnNextPage;
BOOL	PrintOnNextLine;
BOOL	PrintFileName = 1;
BOOL	PrintFilePath = 1;
BOOL	PrintPageNumbers = 1;

LOGFONT	PrintLogFont;
int	PrintFontSize;
RECT	PrintMargins;

/* Search options */

BOOL	search_insert_current = FALSE;
BOOL	replace_insert_current = FALSE;
BOOL	search_case = FALSE;
BOOL	search_advanced = FALSE;

BOOL	search_prefix = FALSE;
BOOL	search_suffix = FALSE;
BOOL	search_fromcursor = TRUE;
BOOL	search_back = FALSE;

BOOL	msearch_subdir = FALSE;
BOOL	msearch_in_opened = TRUE;
BOOL	msearch_in_project = FALSE;
BOOL	msearch_in_files   = FALSE;

char msearch_path [STRLEN];
char msearch_mask [300];

/* Run options */

char	RunArguments [STRLEN];
char	ExeFile	     [STRLEN];
char	RunStartDir  [STRLEN];
int	RunMode;
BOOL	SameConsole;

/* Windows options */

BOOL	DockMessages = FALSE;
BOOL	DockFList = FALSE;
BOOL	DockSearchResults = FALSE;

/* New project options */

char	std_red_file [STRLEN];
char	std_template_file [STRLEN];

/* ------------------------------------------------------------------ */

#define GENERAL_SECTION  "general"
#define NEWPROJ_SECTION  "new-project"
#define EDITOR_SECTION   "editor"
#define COLOR_SECTION    "colors"
#define FONT_SECTION     "fonts"
#define PRINT_SECTION    "print"
#define OPEN_FILTER_SECTION	"open-filter"
#define RUN_SECTION "run-options"

#define SEARCH_SECTION   "search"
#define MSEARCH_SECTION   "file-search"
#define REPLACE_SECTION  "replace"


static char find_red_filter[] =		"Redirection files (*.red)\0*.red\0"
					"all files (*.*)\0*.*\0";

static char find_template_filter[] =	"Project files (*.prj)\0*.prj\0"
					"all files (*.*)\0*.*\0";

static char find_exe_filter[]	 =	"Executable files (*.exe)\0*.exe\0"
					"all files (*.*)\0*.*\0";

static char find_dll_filter[]	 =	"Library files (*.dll)\0*.dll\0"
					"all files (*.*)\0*.*\0";


#define FILTER_SIZE 100

char * open_filter;

/* ------------------------------------------------------------------------------------------- */

BOOL	read_bool_option (char * section, char * name, BOOL def)
{
	char s [20];
	GetPrivateProfileString (section, name, def?"on":"off", s, sizeof (s)-1, IniFile);
	return !stricmp (s, "on");
}

void	write_bool_option (char * section, char * name, BOOL value)
{
	WritePrivateProfileString (section, name, value ? "on": "off", IniFile);
}

void	write_int_option (char * section, char * name, long value)
{
	char s [10];

	wsprintf (s, "%ld", value);
	WritePrivateProfileString (section, name, s, IniFile);
}

long	read_int_option (char * section, char * name, long def)
{
	char s [20], d[20];

	wsprintf (d, "%ld", def);
	GetPrivateProfileString (section, name, d, s, sizeof (s)-1, IniFile);
	return isdigit (s[0]) ? atoi (s) : def;
}

BOOL	proj_read_bool_option (char * section, char * name, BOOL def)
{
	char s [20];
	GetPrivateProfileString (section, name, def?"on":"off", s, sizeof (s)-1, ProjIni);
	return !stricmp (s, "on");
}

void	proj_write_bool_option (char * section, char * name, BOOL value)
{
	WritePrivateProfileString (section, name, value ? "on": "off", ProjIni);
}

/* ------------------------------------------------------------------------------------------- */

void	read_general_options (void)
{
	OpenLastProject = read_bool_option (GENERAL_SECTION, "open-last-project", TRUE);
	ShowLogo	= read_bool_option (GENERAL_SECTION, "show-logo",	  TRUE);
	FlatToolBar	= read_bool_option (GENERAL_SECTION, "flat-toolbar",	  TRUE);
}

void	write_general_options (void)
{
	write_bool_option (GENERAL_SECTION, "open-last-project", OpenLastProject);
	write_bool_option (GENERAL_SECTION, "show-logo", ShowLogo);
	write_bool_option (GENERAL_SECTION, "flat-toolbar", FlatToolBar);
}

/* ------------------------------------------------------------------------------------------- */

void	get_filter_from_resource (void)
{
	int i, j, pos, len;
	char s [10000];

	len = 100;
	for (i = 0; i < FILTER_SIZE; i++) {
		if (!LoadString (ResInstance, IDSTR_OPEN_FILTER + i, s, sizeof (s)))
			break;
		for (j = 0; s [j] && s [j] != '='; j++);
		if (s [j] == '=')
			len += strlen (s+j+1) + 2 * j + 10;
		else
			len += 2 * strlen (s) + 10;
	}
	open_filter = Malloc (len);
	pos = 0;
	for (i = 0; i < FILTER_SIZE; i++) {
		if (!LoadString (ResInstance, IDSTR_OPEN_FILTER + i, s, sizeof (s)))
			break;
		for (j = 0; s [j] && s [j] != '='; j++);
		if (s [j] == '=') {
			s [j] = 0;
			pos += sprintf (open_filter + pos, "%s (%s)%c%s%c", s+j+1, s, 0, s, 0);
		} else
			pos += sprintf (open_filter + pos, "%s%c%s%c", s, 0, s, 0);
	}
	sprintf (open_filter + pos, "All files (*.*)%c*.*%c", 0, 0);
}

void	Read_open_filter (void)
{
	char * section [FILTER_SIZE][2];
	int i, len, pos;

	Read_section (section, sizeof (section) / sizeof (section [0]), OPEN_FILTER_SECTION, IniFile);
	if (!section [0][0]) {
		get_filter_from_resource ();
		return;
	}
	len = 100;
	for (i = 0; i < FILTER_SIZE && section [i][0]; i++)
		if (section [i][1])
			len += strlen (section [i][1]) + 2 * strlen (section [i][0]) + 10;
		else
			len += 2 * strlen (section [i][0]) + 10;
		
	open_filter = Malloc (len);
	pos = 0;
	for (i = 0; i < FILTER_SIZE && section [i][0]; i++)
		if (section [i][1])
			pos += sprintf (open_filter + pos, "%s (%s)%c%s%c", section [i][1], section [i][0], 0, section [i][0], 0);
		else
			pos += sprintf (open_filter + pos, "%s%c%s%c", section [i][0], 0, section [i][0], 0);
	sprintf (open_filter + pos, "All files (*.*)%c*.*%c", 0, 0);
}

/* ------------------------------------------------------------------------------------------- */

void	Read_newproj_ini_file (char * dirstring, char * defmainmod)
{
	read_ini_file (NEWPROJ_SECTION, "directories", IDSTR_DIRLIST, dirstring, STRLEN, IniFile);
	read_ini_file (NEWPROJ_SECTION, "default-main-module", IDSTR_DEFMAINMOD, defmainmod, STRLEN, IniFile);
	read_ini_file (NEWPROJ_SECTION, "template", IDSTR_TEMPLATE, std_template_file, sizeof (std_template_file), IniFile);
	read_ini_file (NEWPROJ_SECTION, "redfile", IDSTR_STDREDFILE, std_red_file, sizeof (std_red_file), IniFile);
}

void	write_newproj_ini_file (char * dirstring, char * defmainmod)
{
	WritePrivateProfileString (NEWPROJ_SECTION, "directories", dirstring, IniFile);
	WritePrivateProfileString (NEWPROJ_SECTION, "default-main-module", defmainmod, IniFile);
	WritePrivateProfileString (NEWPROJ_SECTION, "template",	std_template_file, IniFile);
	WritePrivateProfileString (NEWPROJ_SECTION, "redfile", std_red_file, IniFile);
}

/* ------------------------------------------------------------------------------------------- */

void	write_edit_options (void)
{
	char s [10];

	wsprintf (s, "%d", TabSize);
	write_bool_option (EDITOR_SECTION, "hard-tabs",		HardTabs);
	write_bool_option (EDITOR_SECTION, "flexible-tabs",	PrevLineTabs);
	write_int_option  (EDITOR_SECTION, "tab-size",		TabSize);
	write_bool_option (EDITOR_SECTION, "auto-indent",	AutoIndent);
	write_bool_option (EDITOR_SECTION, "auto-save",		AutoSave);
	write_bool_option (EDITOR_SECTION, "bak-files",		BakFiles);
	write_bool_option (EDITOR_SECTION, "clear-undo-on-save",ClearUndoOnSave);
	write_bool_option (EDITOR_SECTION, "oem-convert",	OemConvert);
	write_bool_option (EDITOR_SECTION, "autodetect-eol",	Autodetect_eol);
	WritePrivateProfileString (EDITOR_SECTION, "eol-style",
					Default_eol_type == EOL_CR ? "cr" :
					Default_eol_type == EOL_LF ? "lf" : "crlf",
					IniFile);
	write_bool_option (EDITOR_SECTION, "write-eof",		WriteEOF);
	write_bool_option (EDITOR_SECTION, "recognize-eof",	ReadEOF);
	write_bool_option (EDITOR_SECTION, "bs-joins-lines",	BSJoinsLines);
	write_bool_option (EDITOR_SECTION, "del-joins-lines",	DelJoinsLines);
	write_bool_option (EDITOR_SECTION, "cr-splits-line",	CrSplitsLine);
	write_bool_option (EDITOR_SECTION, "tab-types",		TabTypes);
	write_bool_option (EDITOR_SECTION, "new-editor-insert", NewEditorInsert);
	write_bool_option (EDITOR_SECTION, "show-tabs",		ShowTabs);
	write_bool_option (EDITOR_SECTION, "show-bookmarks",	ShowBookmarks);
	write_bool_option (EDITOR_SECTION, "show-left-margin",	ShowLeftMargin);
	write_bool_option (EDITOR_SECTION, "show-right-margin", ShowRightMargin);
	WritePrivateProfileString (EDITOR_SECTION, "cursor-after-paste", PastePosition > 0 ? "after":
									 PastePosition < 0 ? "before" :
									 "selection", IniFile);
	WritePrivateProfileString (EDITOR_SECTION, "typing-when-block-selected", TypeWhenBlock > 0 ? "after":
									 TypeWhenBlock < 0 ? "before" :
									 "replace", IniFile);
	WritePrivateProfileString (EDITOR_SECTION, "paste-when-block-selected", PasteWhenBlock > 0 ? "after":
									 PasteWhenBlock < 0 ? "before" :
									 "replace", IniFile);
	write_int_option (EDITOR_SECTION, "right-margin-pos",	RightMargin);
}

void	read_edit_options (void)
{
	char s [100];

	TabSize		= read_int_option (EDITOR_SECTION, "tab-size", 8);
	if (TabSize < 1)  TabSize = 8;
	if (TabSize > 20) TabSize = 20;
	HardTabs	= read_bool_option (EDITOR_SECTION, "hard-tabs",	 TRUE);
	PrevLineTabs	= read_bool_option (EDITOR_SECTION, "flexible-tabs",	 FALSE);
	AutoIndent	= read_bool_option (EDITOR_SECTION, "auto-indent",	 TRUE);
	AutoSave	= read_bool_option (EDITOR_SECTION, "auto-save",	 FALSE);
	BakFiles	= read_bool_option (EDITOR_SECTION, "bak-files",	 TRUE);
	ClearUndoOnSave	= read_bool_option (EDITOR_SECTION, "clear-undi-on-save", TRUE);
	OemConvert	= read_bool_option (EDITOR_SECTION, "oem-convert",	 FALSE);
	Autodetect_eol	= read_bool_option (EDITOR_SECTION, "autodetect-eol",	 TRUE);
	GetPrivateProfileString (EDITOR_SECTION, "eol-style", "crlf", s, sizeof (s)-1, IniFile);
	Default_eol_type = !stricmp (s, "cr") ? EOL_CR:
			   !stricmp (s, "lf") ? EOL_LF: EOL_CRLF;
	WriteEOF = read_bool_option (EDITOR_SECTION, "write-eof",	 FALSE);
	ReadEOF	 = read_bool_option (EDITOR_SECTION, "recognize-eof",	 TRUE);
	BSJoinsLines	= read_bool_option (EDITOR_SECTION, "bs-joins-lines",	 TRUE);
	DelJoinsLines	= read_bool_option (EDITOR_SECTION, "del-joins-lines",	 TRUE);
	CrSplitsLine	= read_bool_option (EDITOR_SECTION, "cr-splits-line",	 TRUE);
	TabTypes	= read_bool_option (EDITOR_SECTION, "tab-types",	 TRUE);
	NewEditorInsert	= read_bool_option (EDITOR_SECTION, "ned-editor-insert", TRUE);
	ShowTabs	= read_bool_option (EDITOR_SECTION, "show-tabs",	 FALSE);
	ShowBookmarks	= read_bool_option (EDITOR_SECTION, "show-bookmarks",	 TRUE);
	ShowLeftMargin	= read_bool_option (EDITOR_SECTION, "show-left-margin",  TRUE);
	ShowRightMargin	= read_bool_option (EDITOR_SECTION, "show-right-margin", TRUE);
	GetPrivateProfileString (EDITOR_SECTION, "cursor-after-paste", "after", s, sizeof (s)-1, IniFile);
	PastePosition	= !strcmp (s, "before") ? -1 :
			  !strcmp (s, "after") ? 1 : 0;
	GetPrivateProfileString (EDITOR_SECTION, "typing-when-selected", "replace", s, sizeof (s)-1, IniFile);
	TypeWhenBlock	= !strcmp (s, "before") ? -1 :
			  !strcmp (s, "after") ? 1 : 0;
	GetPrivateProfileString (EDITOR_SECTION, "paste-when-selected", "replace", s, sizeof (s)-1, IniFile);
	PasteWhenBlock	= !strcmp (s, "before") ? -1 :
			  !strcmp (s, "after") ? 1 : 0;
	RightMargin	= read_int_option (EDITOR_SECTION, "right-margin-pos", 80);
	if (TabSize < 1)  RightMargin = 0;
	if (!RightMargin) ShowRightMargin = FALSE;
}

/* ------------------------------------------------------------------------------------------- */

void	Write_search_settings (void)
{
	proj_write_bool_option (SEARCH_SECTION,  "insert-current", search_insert_current);
	proj_write_bool_option (SEARCH_SECTION,  "advanced", search_advanced);
	proj_write_bool_option (SEARCH_SECTION,  "case-sensitive", search_case);
	proj_write_bool_option (SEARCH_SECTION,  "prefix", search_prefix);
	proj_write_bool_option (SEARCH_SECTION,  "suffix", search_suffix);
	proj_write_bool_option (SEARCH_SECTION,  "from-cursor", search_fromcursor);
	proj_write_bool_option (SEARCH_SECTION,  "back", search_back);

	proj_write_bool_option (REPLACE_SECTION, "insert-current", replace_insert_current);

	proj_write_bool_option (MSEARCH_SECTION, "subdirectories", msearch_subdir);
	proj_write_bool_option (MSEARCH_SECTION, "in-opened-files", msearch_in_opened);
	proj_write_bool_option (MSEARCH_SECTION, "in-project-files", msearch_in_project);
	proj_write_bool_option (MSEARCH_SECTION, "in-disk-files", msearch_in_files);

	WritePrivateProfileString (MSEARCH_SECTION, "file-mask", msearch_mask, ProjIni);
	WritePrivateProfileString (MSEARCH_SECTION, "directory", msearch_path, ProjIni);
}

void	Write_search_history (void)
{
	Write_history (search_history, SEARCH_HISTORY_LEN, "search history", ProjIni);
}

void	Read_search_settings (void)
{
	search_insert_current  = proj_read_bool_option (SEARCH_SECTION, "insert-current", FALSE);
	search_advanced        = proj_read_bool_option (SEARCH_SECTION, "advanced", FALSE);
	search_case	       = proj_read_bool_option (SEARCH_SECTION, "case-sensitive", FALSE);
	search_prefix	       = proj_read_bool_option (SEARCH_SECTION,  "prefix", FALSE);
	search_suffix          = proj_read_bool_option (SEARCH_SECTION,  "suffix", FALSE);
	search_fromcursor      = proj_read_bool_option (SEARCH_SECTION,  "from-cursor", TRUE);
	search_back            = proj_read_bool_option (SEARCH_SECTION,  "back", FALSE);

	replace_insert_current = proj_read_bool_option (REPLACE_SECTION, "insert-current", FALSE);
	Read_history (search_history, SEARCH_HISTORY_LEN, 200, "search history", ProjIni);

	search_insert_current  = proj_read_bool_option (SEARCH_SECTION, "insert-current", FALSE);

	msearch_subdir         = proj_read_bool_option (MSEARCH_SECTION, "subdirectories", FALSE);
	msearch_in_opened      = proj_read_bool_option (MSEARCH_SECTION, "in-opened-files", TRUE);
	msearch_in_project     = proj_read_bool_option (MSEARCH_SECTION, "in-project-files", FALSE);
	msearch_in_files       = proj_read_bool_option (MSEARCH_SECTION, "in-disk-files", FALSE);

	if (msearch_in_opened)
		msearch_in_project = msearch_in_files = FALSE;
	if (msearch_in_project)
		msearch_in_files = FALSE;

	GetPrivateProfileString (MSEARCH_SECTION, "file-mask", "*", msearch_mask, sizeof (msearch_mask), ProjIni);
	GetPrivateProfileString (MSEARCH_SECTION, "directory", ".", msearch_path, sizeof (msearch_path), ProjIni);
}

/* ------------------------------------------------------------------------------------------- */
void	write_print_options (void)
{
	write_int_option (PRINT_SECTION, "left-margin",   PrintMargins.left);
	write_int_option (PRINT_SECTION, "right-margin",  PrintMargins.right);
	write_int_option (PRINT_SECTION, "top-margin",    PrintMargins.top);
	write_int_option (PRINT_SECTION, "bottom-margin", PrintMargins.bottom);
	write_bool_option (PRINT_SECTION, "line-numbers", PrintLineNumbers);
	write_bool_option (PRINT_SECTION, "file-name", PrintFileName);
	write_bool_option (PRINT_SECTION, "file-path", PrintFilePath);
	write_bool_option (PRINT_SECTION, "page-numbers", PrintPageNumbers);
	WritePrivateProfileString (PRINT_SECTION, "long-lines",
		PrintOnNextPage ? "next-page" : 
		PrintOnNextLine ? "next-page" : "truncated",
		IniFile);
}

void	read_print_options (void)
{
	char s [100];
	PrintMargins.left   = read_int_option (PRINT_SECTION, "left-margin",   0);
	PrintMargins.right  = read_int_option (PRINT_SECTION, "right-margin",  0);
	PrintMargins.top    = read_int_option (PRINT_SECTION, "top-margin",    0);
	PrintMargins.bottom = read_int_option (PRINT_SECTION, "bottom-margin", 0);

	PrintLineNumbers    = read_bool_option (PRINT_SECTION, "line-numbers", 1);
	PrintFileName       = read_bool_option (PRINT_SECTION, "file-name", 1);
	PrintFilePath       = read_bool_option (PRINT_SECTION, "file-path", 0);
	PrintPageNumbers    = read_bool_option (PRINT_SECTION, "page-numbers", 1);
	GetPrivateProfileString (PRINT_SECTION, "long-lines", "next-line", s, sizeof (s), IniFile);
	PrintOnNextPage = PrintOnNextLine = FALSE;
	if (!stricmp (s, "truncated"));
	else if (!stricmp (s, "next-page"))
		PrintOnNextPage = TRUE;
	else
		PrintOnNextLine = TRUE;
}

/* ------------------------------------------------------------------------------------------- */

COLORREF Read_color (char * name, BOOL solid)
{
	char s [100];
	HDC dc;
	COLORREF c;

	GetPrivateProfileString (COLOR_SECTION, name, "default", s, sizeof (s)-1, IniFile);
	if (!stricmp (s, "default"))
		return DEFAULT_COLOR;
	if (s[0])
		sscanf (s, "%lX", &c);
	if (solid && c != (COLORREF) -1)	{
		dc = GetDC (0);
		c = GetNearestColor (dc, c);
		ReleaseDC (0, dc);
	}
	return c;
}

BOOL	Read_bool_color_option (char * name, BOOL dfl)
{
	return read_bool_option (COLOR_SECTION, name, dfl);
}

void	read_colors (void)
{
	ShellColor    =	Read_color ("workspace",	    FALSE);

	WndForeColor =	Read_color ("windows-foreground",   TRUE);
	WndBackColor =	Read_color ("windows-background",   TRUE);
	
	MsgForeColor  =	Read_color ("messages-foreground",  TRUE);
	MsgBackColor  = Read_color ("messages-background",  TRUE);
	
	ListForeColor =	Read_color ("file-list-foreground", TRUE);
	ListBackColor =	Read_color ("file-list-background", TRUE);
}

void	Write_color (COLORREF c, char * name)
{
	char s [20];
	if (c == DEFAULT_COLOR)
		strcpy (s, "default");
	else
		wsprintf (s, "%lX", c);
	WritePrivateProfileString (COLOR_SECTION, name, s, IniFile);
}

void	Write_bool_color_option (char * name, BOOL val)
{
	write_bool_option (COLOR_SECTION, name, val);
}

void	write_log_font (char * name, LOGFONT * lf, int size)
{
	char s [100];
	char t [100];

	WritePrivateProfileString (FONT_SECTION, name, lf->lfFaceName, IniFile);

	sprintf (s, "%s-bold", name);
	sprintf (t, "%ld", lf->lfWeight >= FW_BOLD);
	WritePrivateProfileString (FONT_SECTION, s, t, IniFile);

	sprintf (s, "%s-size", name);
	sprintf (t, "%ld", size ? size : lf->lfHeight < 0 ? lf->lfHeight : 0);
	WritePrivateProfileString (FONT_SECTION, s, t, IniFile);

	sprintf (s, "%s-italic", name);
	sprintf (t, "%d", lf->lfItalic!=0);
	WritePrivateProfileString (FONT_SECTION, s, t, IniFile);

	sprintf (s, "%s-quality", name);
	sprintf (t, "%d,%d,%d", (int) lf->lfOutPrecision, (int) lf->lfClipPrecision, (int) lf->lfQuality);
	WritePrivateProfileString (FONT_SECTION, s, t, IniFile);

	sprintf (s, "%s-pitch", name);
	sprintf (t, "%d", (int) lf->lfPitchAndFamily);
	WritePrivateProfileString (FONT_SECTION, s, t, IniFile);

	sprintf (s, "%s-charset", name);
	sprintf (t, "%d", (int) lf->lfCharSet);
	WritePrivateProfileString (FONT_SECTION, s, t, IniFile);
}

void	write_font (char * name, HFONT font, int size)
{
	LOGFONT lf;
	GetObject (font, sizeof (LOGFONT), &lf);
	write_log_font (name, &lf, size);
}

void	read_log_font (char * name, LOGFONT * lf, int * size)
{
	char s [100];
	char t [100];
	int out, clip, quality, sz;
	HDC dc;

	GetPrivateProfileString (FONT_SECTION, name, "", t, sizeof (t)-1, IniFile);
	if (t[0])	{
		strncpy ((char*)lf->lfFaceName, t, sizeof (lf->lfFaceName));
		lf->lfFaceName [sizeof (lf->lfFaceName)-1] = 0;
	}

	sprintf (s, "%s-size", name);
	GetPrivateProfileString (FONT_SECTION, s, "0", t, sizeof (t)-1, IniFile);
	sz = atoi (t);
	if (sz < 0)
		lf->lfHeight = sz;
	else
		*size = sz;
	if (*size)	{
		dc = GetDC (0);
		lf->lfHeight = -MulDiv (*size, GetDeviceCaps (dc, LOGPIXELSY), 720);
		ReleaseDC (0, dc);
	}

	sprintf (s, "%s-bold", name);
	GetPrivateProfileString (FONT_SECTION, s, "", t, sizeof (t)-1, IniFile);
	lf->lfWeight = t[0] == '1' ? FW_BOLD: FW_NORMAL;

	sprintf (s, "%s-italic", name);
	GetPrivateProfileString (FONT_SECTION, s, "0", t, sizeof (t)-1, IniFile);
	lf->lfItalic = (BYTE) (t[0]=='1');

	sprintf (s, "%s-quality", name);
	GetPrivateProfileString (FONT_SECTION, s, "0,0,1", t, sizeof (t)-1, IniFile);
	clip = out = 0; quality = 1;
	sscanf (t, "%d,%d,%d", &out, &clip, &quality);
	lf->lfOutPrecision = out;
	lf->lfClipPrecision = clip;
	lf->lfQuality = quality;

	sprintf (s, "%s-pitch", name);
	GetPrivateProfileString (FONT_SECTION, s, "0", t, sizeof (t)-1, IniFile);
	lf->lfPitchAndFamily = atoi (t);

	sprintf (s, "%s-charset", name);
	GetPrivateProfileString (FONT_SECTION, s, "0", t, sizeof (t)-1, IniFile);
	lf->lfCharSet = atoi (t);

	lf->lfWidth = 0;
}

HFONT   read_screen_font (char * name, HFONT font, int * size)
{
	HFONT f;
	LOGFONT lf;

	GetObject (font, sizeof (LOGFONT), &lf);
	read_log_font (name, &lf, size);
	f = CreateFontIndirect (&lf);
	return f ? f : font;
}

void	write_colors (void)
{
	Write_color (ShellColor,    "workspace");

	Write_color (WndForeColor, "windows-foreground");
	Write_color (WndBackColor, "windows-background");
	
	Write_color (MsgForeColor,  "messages-foreground");
	Write_color (MsgBackColor,  "messages-background");
	
	Write_color (ListForeColor, "file-list-foreground");
	Write_color (ListBackColor, "file-list-background");
	
	write_font ("editor",         EditFont, EditFontSize);
	write_font ("file-list",      ListFont, ListFontSize);
	write_font ("message-window", MsgFont, MsgFontSize);
}

void	Write_compiler_font (void)
{
	write_font ("compile-window", CompileFont, CompileFontSize);
}

void	write_print_font (void)
{
	write_log_font ("print", &PrintLogFont, PrintFontSize);
}

void	read_fonts (void)
{
	LOGFONT lf;

	EditFontSize = ListFontSize = MsgFontSize = CompileFontSize = 0;

	EditFont    = read_screen_font ("editor",         GetStockObject (SYSTEM_FIXED_FONT),	&EditFontSize);
	ListFont    = read_screen_font ("file-list",      GetStockObject (SYSTEM_FONT),		&ListFontSize);
	MsgFont     = read_screen_font ("message-window", GetStockObject (SYSTEM_FONT),		&MsgFontSize);
	CompileFont = read_screen_font ("compile-window", GetStockObject (SYSTEM_FIXED_FONT),	&CompileFontSize);

	GetObject (EditFont, sizeof (lf), &lf);
	memset (&PrintLogFont, 0, sizeof (PrintLogFont));
	PrintLogFont.lfCharSet = lf.lfCharSet;
	PrintLogFont.lfOutPrecision = OUT_DEVICE_PRECIS;
	PrintLogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	PrintLogFont.lfQuality = PROOF_QUALITY;
	PrintLogFont.lfQuality = PROOF_QUALITY;
	PrintLogFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	strcpy (PrintLogFont.lfFaceName, "Courier New");
	PrintFontSize = 100;
	read_log_font ("print", &PrintLogFont, &PrintFontSize);
}

/* ------------------------------------------------------------------------------------------- */

void	Read_run_options (void)
{
	char s [200];
	BOOL same;
	GetPrivateProfileString (RUN_SECTION, "run-arguments", "", RunArguments, STRLEN, ProjIni);
	read_ini_file (RUN_SECTION, "exe-file",  ProjFile >= 0 ? IDSTR_EXEFILE : IDSTR_EXEFILE_NOPROJ,  ExeFile, STRLEN, ProjIni);
	read_ini_file (RUN_SECTION, "start-dir", ProjFile >= 0 ? IDSTR_RUNDIR : IDSTR_RUNDIR_NOPROJ, RunStartDir, STRLEN, ProjIni);
	GetPrivateProfileString (RUN_SECTION, "run-mode", "", s, sizeof (s), ProjIni);
	RunMode = !stricmp (s, "maximized") ? SW_MAXIMIZE : SW_NORMAL;
	GetPrivateProfileString (RUN_SECTION, "one-console", "on", s, sizeof (s), ProjIni);
	same = !stricmp (s, "on");
	if (SameConsole != same && !same) Reset_console ();
	SameConsole = same;
	Set_name ("arguments", RunArguments);
	Set_name ("exefile",   ExeFile);
	Set_name ("rundir",    RunStartDir);
}

void	write_run_options (void)
{
	char * s;
	WritePrivateProfileString (RUN_SECTION, "run-arguments", RunArguments, ProjIni);
	WritePrivateProfileString (RUN_SECTION, "exe-file",  ExeFile,  ProjIni);
	WritePrivateProfileString (RUN_SECTION, "start-dir", RunStartDir, ProjIni);
	s = RunMode == SW_MAXIMIZE ? "maximized": "normal";
	WritePrivateProfileString (RUN_SECTION, "run-mode",  s, ProjIni);
	s = SameConsole ? "on": "off";
	WritePrivateProfileString (RUN_SECTION, "one-console",  s, ProjIni);
}
/* ------------------------------------------------------------------------------------------- */

HDC	default_print_dc (void)
{
	PRINTDLG dlg;

	memset (&dlg, 0, sizeof (dlg));
	dlg.lStructSize = sizeof (dlg);
	dlg.hwndOwner = 0;
	dlg.Flags = PD_RETURNDC | PD_RETURNDEFAULT;
	if (!PrintDlg (&dlg))
		return NULL;
	return dlg.hDC;
}

void	Page_setup (HWND hwnd)
{
	PAGESETUPDLG p;
	HDC dc;
	int phys_x, phys_y, log_x, log_y, offset_x, offset_y, res_x, res_y;

	dc = default_print_dc ();
	if (! dc)	{
		MsgBox ("Print Setup Error", MB_OK|MB_ICONSTOP,
				"No default printer is selected.\n"
				"Configure default printer in Control Panel");
		return;
	}

	res_x =	GetDeviceCaps (dc, LOGPIXELSX),
	res_y =	GetDeviceCaps (dc, LOGPIXELSY),
	log_x =	GetDeviceCaps (dc, HORZRES),
	log_y =	GetDeviceCaps (dc, VERTRES),
	phys_x = GetDeviceCaps (dc, PHYSICALWIDTH),
	phys_y = GetDeviceCaps (dc, PHYSICALHEIGHT),
	offset_x = GetDeviceCaps (dc, PHYSICALOFFSETX),
	offset_y = GetDeviceCaps (dc, PHYSICALOFFSETY);
	DeleteDC (dc);

	memset (&p, 0, sizeof (p));
	p.lStructSize = sizeof (p);
	p.hwndOwner = hwnd; 
	p.Flags = PSD_INHUNDREDTHSOFMILLIMETERS | PSD_MARGINS | PSD_MINMARGINS;
	p.rtMinMargin.left   = offset_x * 2540 / res_x;
	p.rtMinMargin.top    = offset_y * 2540 / res_y;
	p.rtMinMargin.right  = (phys_x - log_x - offset_x) * 2540 / res_x;
	p.rtMinMargin.bottom = (phys_y - log_y - offset_y) * 2540 / res_y;
	if (p.rtMinMargin.right < 0)  p.rtMinMargin.right = 0;
	if (p.rtMinMargin.bottom < 0) p.rtMinMargin.bottom = 0;
	p.rtMargin.left   = max (PrintMargins.left,   p.rtMinMargin.left);
	p.rtMargin.top    = max (PrintMargins.top,    p.rtMinMargin.top);
	p.rtMargin.right  = max (PrintMargins.right,  p.rtMinMargin.right);
	p.rtMargin.bottom = max (PrintMargins.bottom, p.rtMinMargin.bottom);
	
	if (PageSetupDlg (&p))
		PrintMargins = p.rtMargin;
}

void	Choose_print_font (HWND hwnd)
{
	CHOOSEFONT cf;
	LOGFONT lf;
	BOOL b;
	HDC dc;

	dc = default_print_dc ();
	memset (&cf, 0, sizeof (CHOOSEFONT));
	lf = PrintLogFont;
	cf.lStructSize = sizeof (CHOOSEFONT);
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lf;
	cf.hDC = dc;
	cf.Flags = CF_INITTOLOGFONTSTRUCT;
	cf.Flags |= CF_FIXEDPITCHONLY;
	cf.Flags |= CF_PRINTERFONTS;
	b = ChooseFont (&cf);
	DeleteDC (dc);
	if (!b) return;
	PrintLogFont = lf;
	PrintFontSize = cf.iPointSize;
}

BOOL CALLBACK CfgPrintDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		CheckDlgButton (hwnd, CFGPRINT_TRUNCATE, !PrintOnNextPage && !PrintOnNextLine);
		CheckDlgButton (hwnd, CFGPRINT_NEXTLINE, PrintOnNextLine);
		CheckDlgButton (hwnd, CFGPRINT_NEXTPAGE, PrintOnNextPage);
		CheckDlgButton (hwnd, CFGPRINT_FILENAME, PrintFileName);
		CheckDlgButton (hwnd, CFGPRINT_FILEPATH, PrintFilePath);
		CheckDlgButton (hwnd, CFGPRINT_PAGENUM, PrintPageNumbers);
		CheckDlgButton (hwnd, CFGPRINT_LINENUM, PrintLineNumbers);
		EnableWindow (GetDlgItem (hwnd, CFGPRINT_FILEPATH), PrintFileName);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			PrintOnNextLine = PrintOnNextPage = 0;
			if (IsDlgButtonChecked (hwnd, CFGPRINT_NEXTLINE))
				PrintOnNextLine = TRUE;
			else if (IsDlgButtonChecked (hwnd, CFGPRINT_NEXTPAGE))
				PrintOnNextPage = TRUE;
			PrintFileName = IsDlgButtonChecked (hwnd, CFGPRINT_FILENAME);
			PrintFilePath = IsDlgButtonChecked (hwnd, CFGPRINT_FILEPATH);
			PrintPageNumbers = IsDlgButtonChecked (hwnd, CFGPRINT_PAGENUM);
			PrintLineNumbers = IsDlgButtonChecked (hwnd, CFGPRINT_LINENUM);
			EndDialog (hwnd, 1);
			return 1;
		case CFGPRINT_FILENAME:
			EnableWindow (GetDlgItem (hwnd, CFGPRINT_FILEPATH), IsDlgButtonChecked (hwnd, CFGPRINT_FILENAME));
			return 1;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			return 1;
		case CFGPRINT_FONT:
			Choose_print_font (hwnd);
			return 1;
		case CFGPRINT_PAGE:
			Page_setup (hwnd);
			return 1;
		case IDHELP:
			return Dialog_help (CFGPRINT_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (CFGPRINT_DIALOG, lparam);
	}
	return 0;
}

BOOL	Configure_print (void)
{
	if (!Dialog (CFGPRINT_DIALOG, CfgPrintDialogProc))
		return FALSE;
	write_print_font ();
	write_print_options ();
	return TRUE;
}

/* ------------------------------------------------------------------------------------------- */

BOOL CALLBACK CfgGeneralDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		CheckDlgButton (hwnd, CFGGENERAL_SHOWLOGO, ShowLogo);
		CheckDlgButton (hwnd, CFGGENERAL_OPENLAST, OpenLastProject);
		CheckDlgButton (hwnd, CFGGENERAL_FLATTOOLBAR, FlatToolBar);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			ShowLogo	= IsDlgButtonChecked (hwnd, CFGGENERAL_SHOWLOGO);
			OpenLastProject = IsDlgButtonChecked (hwnd, CFGGENERAL_OPENLAST);
			FlatToolBar	= IsDlgButtonChecked (hwnd, CFGGENERAL_FLATTOOLBAR);
			write_general_options ();
			Set_flat_toolbar (ToolBar, FlatToolBar);
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (CFGGENERAL_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (CFGGENERAL_DIALOG, lparam);
	}
	return 0;
}

BOOL	Configure_general (void)
{
	return Dialog (CFGGENERAL_DIALOG, CfgGeneralDialogProc);
}

static char * directories;
static char * def_main_mod;

BOOL CALLBACK CfgProjDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		SetDlgItemText (hwnd, CFGPROJ_DIRECTORIES, directories);
		SetDlgItemText (hwnd, CFGPROJ_TEMPLATE, std_template_file);
		SetDlgItemText (hwnd, CFGPROJ_REDFILE, std_red_file);
		SetDlgItemText (hwnd, CFGPROJ_DEFMAINMOD,  def_main_mod);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			GetDlgItemText (hwnd, CFGPROJ_DIRECTORIES, directories, STRLEN);
			GetDlgItemText (hwnd, CFGPROJ_DEFMAINMOD,   def_main_mod,   STRLEN);
			GetDlgItemText (hwnd, CFGPROJ_TEMPLATE, std_template_file, sizeof (std_template_file));
			GetDlgItemText (hwnd, CFGPROJ_REDFILE,  std_red_file, sizeof (std_red_file));
			write_newproj_ini_file (directories, def_main_mod);
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case CFGPROJ_BROWSETEMPLATE:
			browse_string (hwnd, CFGPROJ_TEMPLATE, find_template_filter,
				       "Select default template for new project", FALSE, TRUE, FALSE);
			break;
		case CFGPROJ_BROWSEREDFILE:
			browse_string (hwnd, CFGPROJ_REDFILE, find_red_filter,
				     "Select default redirection file", FALSE, TRUE, FALSE);
			break;
		case IDHELP:
			return Dialog_help (CFGPROJ_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (CFGPROJ_DIALOG, lparam);
	}
	return 0;
}

BOOL	Configure_new_project (void)
{
	char dir_str [STRLEN];
	char def_main_mod_str [STRLEN];

	directories = dir_str;
	def_main_mod = def_main_mod_str;

	Read_newproj_ini_file (directories, def_main_mod);

	return Dialog (CFGPROJ_DIALOG, CfgProjDialogProc);
}

BOOL CALLBACK CfgRunDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	BOOL same;
	char s [STRLEN];
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);

		GetDlgItemText (hwnd, CFGRUN_TITLE, s, sizeof (s));
		strcat (s, ProjIni);
		SetDlgItemText (hwnd, CFGRUN_TITLE, s);
		SetDlgItemText (hwnd, CFGRUN_EXE, ExeFile);
		SetDlgItemText (hwnd, CFGRUN_STARTDIR, RunStartDir);
		SetDlgItemText (hwnd, CFGRUN_ARGS, RunArguments);
		CheckDlgButton (hwnd, RunMode == SW_MAXIMIZE ? CFGRUN_MAXIMIZE:
				      CFGRUN_NORMAL,
				TRUE);
		CheckDlgButton (hwnd, CFGRUN_SAMECONSOLE, SameConsole);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			GetDlgItemText (hwnd, CFGRUN_EXE, s, STRLEN);
			if (!s[0])	{
				MsgBox ("Error", MB_OK | MB_ICONSTOP, "Empty exe file name not allowed");
				SetDlgItemText (hwnd, CFGRUN_EXE, ExeFile);
				SetFocus (GetDlgItem (hwnd, CFGRUN_EXE));
				break;
			}
			strcpy (ExeFile, s);
			GetDlgItemText (hwnd, CFGRUN_STARTDIR, s, STRLEN);
			if (!s[0])
				strcpy (RunStartDir, "$(projdir)");
			else
				strcpy (RunStartDir, s);
			GetDlgItemText (hwnd, CFGRUN_ARGS, RunArguments, sizeof (RunArguments));
			RunMode = IsDlgButtonChecked (hwnd, CFGRUN_MAXIMIZE) ? SW_MAXIMIZE:
				  SW_NORMAL;
			same = IsDlgButtonChecked (hwnd, CFGRUN_SAMECONSOLE);
			if (same != SameConsole && ! same) Reset_console ();
			SameConsole = same;
			write_run_options ();
			EndDialog (hwnd, 1);
			break;
		case CFGRUN_BROWSEEXE:
			browse_string (hwnd, CFGRUN_EXE, find_exe_filter,
				     "Select executable file", FALSE, TRUE, FALSE);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (CFGRUN_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (CFGRUN_DIALOG, lparam);
	}
	return 0;
}

void	Configure_run (void)
{
	if (!ProjIni [0]) return;
	Read_run_options ();
	Dialog (CFGRUN_DIALOG, CfgRunDialogProc);
}

/* ------------------------------------------------------------------------------------------------ */

int cfgedit_active_page = 0;

BOOL CALLBACK CfgEditTabDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char s [STRLEN];
	int size;

	switch (msg)	{
	case WM_INITDIALOG:
		wsprintf (s, "%d", TabSize);
		SetDlgItemText (hwnd, CFGEDIT_TAB_SIZE, s);
		CheckDlgButton (hwnd, HardTabs     ? CFGEDIT_TAB_HARD:
				      PrevLineTabs ? CFGEDIT_TAB_FLEXIBLE:
						     CFGEDIT_TAB_SPACES,
				TRUE);
		CheckDlgButton (hwnd, CFGEDIT_TAB_AUTOINDENT, AutoIndent);
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_KILLACTIVE:
			GetDlgItemText (hwnd, CFGEDIT_TAB_SIZE, s, STRLEN);
			if (!s[0])	{
				MsgBox ("Error", MB_OK | MB_ICONSTOP, "You should specify tab size");
				SetFocus (GetDlgItem (hwnd, CFGEDIT_TAB_SIZE));
				SetWindowLong (hwnd, DWL_MSGRESULT, 1);
				return 1;
			}
			size = atoi (s);
			if (size <= 0 || size >= 20)	{
				MsgBox ("Error", MB_OK | MB_ICONSTOP, "Tab size should fit in range 1 to 20");
				SetFocus (GetDlgItem (hwnd, CFGEDIT_TAB_SIZE));
				SetWindowLong (hwnd, DWL_MSGRESULT, 1);
				return 1;
			}
		case PSN_APPLY:
			GetDlgItemText (hwnd, CFGEDIT_TAB_SIZE, s, STRLEN);
			TabSize = atoi (s);
			HardTabs = IsDlgButtonChecked (hwnd, CFGEDIT_TAB_HARD);
			PrevLineTabs = IsDlgButtonChecked (hwnd, CFGEDIT_TAB_FLEXIBLE);
			AutoIndent = IsDlgButtonChecked (hwnd, CFGEDIT_TAB_AUTOINDENT);
			Refresh_edits ();
			return 1;
		case PSN_SETACTIVE:
			cfgedit_active_page = CFGEDIT_TAB_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGEDIT_TAB_DIALOG);
		}
		return 0;
	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED || NOTIFY_CODE == EN_CHANGE)
			PropSheet_Changed (GetParent (hwnd), hwnd);
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGEDIT_DIALOG, lparam);
	}
	return 0;
}

BOOL CALLBACK CfgEditFeaturesDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		CheckDlgButton (hwnd, CFGEDIT_FEATURES_BSJOINS, BSJoinsLines);
		CheckDlgButton (hwnd, CFGEDIT_FEATURES_DEL_JOINS, DelJoinsLines);
		CheckDlgButton (hwnd, CFGEDIT_FEATURES_CRSPLITS, CrSplitsLine);
		CheckDlgButton (hwnd, CFGEDIT_FEATURES_TABMOVES, !TabTypes);
		CheckDlgButton (hwnd, CFGEDIT_FEATURES_INSERT, NewEditorInsert);
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_APPLY:
			BSJoinsLines   = IsDlgButtonChecked (hwnd, CFGEDIT_FEATURES_BSJOINS);
			DelJoinsLines  = IsDlgButtonChecked (hwnd, CFGEDIT_FEATURES_DEL_JOINS);
			CrSplitsLine   = IsDlgButtonChecked (hwnd, CFGEDIT_FEATURES_CRSPLITS);
			TabTypes       =!IsDlgButtonChecked (hwnd, CFGEDIT_FEATURES_TABMOVES);
			NewEditorInsert= IsDlgButtonChecked (hwnd, CFGEDIT_FEATURES_INSERT);
			Refresh_edits ();
			return 1;
		case PSN_SETACTIVE:
			cfgedit_active_page = CFGEDIT_FEATURES_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGEDIT_FEATURES_DIALOG);
		}
		return 0;
	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED)
			PropSheet_Changed (GetParent (hwnd), hwnd);
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGEDIT_FEATURES_DIALOG, lparam);
	}
	return 0;
}

BOOL CALLBACK CfgEditBlockDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		CheckDlgButton (hwnd, CFGEDIT_BLOCK_PASTE_BEFORE, PastePosition < 0);
		CheckDlgButton (hwnd, CFGEDIT_BLOCK_PASTE_AFTER, PastePosition > 0);
		CheckDlgButton (hwnd, CFGEDIT_BLOCK_PASTE_SELECT, PastePosition == 0);
		CheckDlgButton (hwnd, CFGEDIT_BLOCK_TYPE_BEFORE, TypeWhenBlock < 0);
		CheckDlgButton (hwnd, CFGEDIT_BLOCK_TYPE_AFTER, TypeWhenBlock > 0);
		CheckDlgButton (hwnd, CFGEDIT_BLOCK_TYPE_REPLACE, TypeWhenBlock == 0);
		CheckDlgButton (hwnd, CFGEDIT_BLOCK_PASTE_INSERTS_BEFORE, PasteWhenBlock < 0);
		CheckDlgButton (hwnd, CFGEDIT_BLOCK_PASTE_INSERTS_AFTER, PasteWhenBlock > 0);
		CheckDlgButton (hwnd, CFGEDIT_BLOCK_PASTE_REPLACES, PasteWhenBlock == 0);
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_APPLY:
			PastePosition = IsDlgButtonChecked (hwnd, CFGEDIT_BLOCK_PASTE_BEFORE) ? -1:
					IsDlgButtonChecked (hwnd, CFGEDIT_BLOCK_PASTE_AFTER) ? 1 : 0;
			TypeWhenBlock = IsDlgButtonChecked (hwnd, CFGEDIT_BLOCK_TYPE_BEFORE) ? -1:
					IsDlgButtonChecked (hwnd, CFGEDIT_BLOCK_TYPE_AFTER) ? 1 : 0;
			PasteWhenBlock = IsDlgButtonChecked (hwnd, CFGEDIT_BLOCK_PASTE_INSERTS_BEFORE) ? -1:
					 IsDlgButtonChecked (hwnd, CFGEDIT_BLOCK_PASTE_INSERTS_AFTER) ? 1 : 0;
			Refresh_edits ();
			return 1;
		case PSN_SETACTIVE:
			cfgedit_active_page = CFGEDIT_BLOCK_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGEDIT_BLOCK_DIALOG);
		}
		return 0;
	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED)
			PropSheet_Changed (GetParent (hwnd), hwnd);
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGEDIT_BLOCK_DIALOG, lparam);
	}
	return 0;
}

BOOL CALLBACK CfgEditShowDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char s [STRLEN];
	int size;

	switch (msg)	{
	case WM_INITDIALOG:
		wsprintf (s, "%d", RightMargin);
		SetDlgItemText (hwnd, CFGEDIT_SHOW_RIGHTMARGINVAL, s);
		CheckDlgButton (hwnd, CFGEDIT_SHOW_LEFTMARGIN, ShowLeftMargin);
		CheckDlgButton (hwnd, CFGEDIT_SHOW_RIGHTMARGIN, ShowRightMargin);
		CheckDlgButton (hwnd, CFGEDIT_SHOW_TABS, ShowTabs);
		CheckDlgButton (hwnd, CFGEDIT_SHOW_BOOKMARKS, ShowBookmarks);
		EnableWindow (GetDlgItem (hwnd, CFGEDIT_SHOW_RIGHTMARGINVAL),
				IsDlgButtonChecked (hwnd, CFGEDIT_SHOW_RIGHTMARGIN));
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_KILLACTIVE:
			if (!IsDlgButtonChecked (hwnd, CFGEDIT_SHOW_RIGHTMARGIN)) return 1;
			GetDlgItemText (hwnd, CFGEDIT_SHOW_RIGHTMARGINVAL, s, STRLEN);
			if (!s[0])	{
				MsgBox ("Error", MB_OK | MB_ICONSTOP, "You should specify right margin");
				SetFocus (GetDlgItem (hwnd, CFGEDIT_SHOW_RIGHTMARGINVAL));
				SetWindowLong (hwnd, DWL_MSGRESULT, 1);
				return 1;
			}
			size = atoi (s);
			if (size <= 0)	{
				MsgBox ("Error", MB_OK | MB_ICONSTOP, "Right margin must be positive");
				SetFocus (GetDlgItem (hwnd, CFGEDIT_SHOW_RIGHTMARGINVAL));
				SetWindowLong (hwnd, DWL_MSGRESULT, 1);
				return 1;
			}
		case PSN_APPLY:
			GetDlgItemText (hwnd, CFGEDIT_SHOW_RIGHTMARGINVAL, s, STRLEN);
			RightMargin = atoi (s);
			ShowRightMargin = IsDlgButtonChecked (hwnd, CFGEDIT_SHOW_RIGHTMARGIN);
			ShowLeftMargin  = IsDlgButtonChecked (hwnd, CFGEDIT_SHOW_LEFTMARGIN);
			ShowTabs = IsDlgButtonChecked (hwnd, CFGEDIT_SHOW_TABS);
			ShowBookmarks = IsDlgButtonChecked (hwnd, CFGEDIT_SHOW_BOOKMARKS);
			Refresh_edits ();
			return 1;
		case PSN_SETACTIVE:
			cfgedit_active_page = CFGEDIT_SHOW_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGEDIT_SHOW_DIALOG);
		}
		return 0;
	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED || NOTIFY_CODE == EN_CHANGE)	{
			PropSheet_Changed (GetParent (hwnd), hwnd);
			EnableWindow (GetDlgItem (hwnd, CFGEDIT_SHOW_RIGHTMARGINVAL),
					IsDlgButtonChecked (hwnd, CFGEDIT_SHOW_RIGHTMARGIN));
		}
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGEDIT_SHOW_DIALOG, lparam);
	}
	return 0;
}

BOOL CALLBACK CfgEditEncodingDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		CheckDlgButton (hwnd, CFGEDIT_ENCODING_OEMCONVERT, OemConvert);
		CheckDlgButton (hwnd, CFGEDIT_ENCODING_AUTODETECT, Autodetect_eol);
		if (Default_eol_type == EOL_CR)
			CheckDlgButton (hwnd, CFGEDIT_ENCODING_CR, 1);
		else if (Default_eol_type == EOL_LF)
			CheckDlgButton (hwnd, CFGEDIT_ENCODING_LF, 1);
		else
			CheckDlgButton (hwnd, CFGEDIT_ENCODING_CRLF, 1);
		CheckDlgButton (hwnd, CFGEDIT_ENCODING_READEOF, ReadEOF);
		CheckDlgButton (hwnd, CFGEDIT_ENCODING_WRITEEOF, WriteEOF);
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_APPLY:
			OemConvert = IsDlgButtonChecked (hwnd, CFGEDIT_ENCODING_OEMCONVERT);
			Default_eol_type = IsDlgButtonChecked (hwnd, CFGEDIT_ENCODING_CR) ? EOL_CR:
					   IsDlgButtonChecked (hwnd, CFGEDIT_ENCODING_LF) ? EOL_LF: EOL_CRLF;
			Autodetect_eol = IsDlgButtonChecked (hwnd, CFGEDIT_ENCODING_AUTODETECT);
			ReadEOF = IsDlgButtonChecked (hwnd, CFGEDIT_ENCODING_READEOF);
			WriteEOF = IsDlgButtonChecked (hwnd, CFGEDIT_ENCODING_WRITEEOF);
			Refresh_edits ();
			return 1;
		case PSN_SETACTIVE:
			cfgedit_active_page = CFGEDIT_ENCODING_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGEDIT_ENCODING_DIALOG);
		}
		return 0;
	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED)
			PropSheet_Changed (GetParent (hwnd), hwnd);
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGEDIT_DIALOG, lparam);
	}
	return 0;
}

BOOL CALLBACK CfgEditFilesDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		CheckDlgButton (hwnd, CFGEDIT_FILES_AUTOSAVE, AutoSave);
		CheckDlgButton (hwnd, CFGEDIT_FILES_BAK, BakFiles);
		CheckDlgButton (hwnd, CFGEDIT_FILES_CLEARUNDO, ClearUndoOnSave);
		return 1;
	case WM_NOTIFY:
		switch (((NMHDR*) lparam)->code)	{
		case PSN_APPLY:
			AutoSave   = IsDlgButtonChecked (hwnd, CFGEDIT_FILES_AUTOSAVE);
			BakFiles   = IsDlgButtonChecked (hwnd, CFGEDIT_FILES_BAK);
			ClearUndoOnSave = IsDlgButtonChecked (hwnd, CFGEDIT_FILES_CLEARUNDO);
			Refresh_edits ();
			return PSNRET_NOERROR;
		case PSN_SETACTIVE:
			cfgedit_active_page = CFGEDIT_FILES_DIALOG;
			return 1;
		case PSN_HELP:
			return Dialog_help (CFGEDIT_FILES_DIALOG);
		}
		return 0;
	case WM_COMMAND:
		if (NOTIFY_CODE == BN_CLICKED)
			PropSheet_Changed (GetParent (hwnd), hwnd);
		return 1;
	case WM_HELP:
		return Page_Control_help (hwnd, CFGEDIT_DIALOG, lparam);
	}
	return 0;
}

int	cfgedit_n_active_page = 0;

void	Configure_editor (void)
{
	PROPSHEETHEADER psh;
	PROPSHEETPAGE pages [6];
	int i;

	memset (pages, 0, sizeof (pages));
	pages [0].dwSize = sizeof (pages [0]);
	pages [0].dwFlags = PSP_HASHELP;
	pages [0].hInstance = MyInstance;
	pages [5] = pages [4] = pages [3] = pages [2] = pages [1] = pages [0];

	pages [0].pfnDlgProc = CfgEditTabDialogProc;
	pages [1].pfnDlgProc = CfgEditFeaturesDialogProc;
	pages [2].pfnDlgProc = CfgEditBlockDialogProc;
	pages [3].pfnDlgProc = CfgEditShowDialogProc;
	pages [4].pfnDlgProc = CfgEditEncodingDialogProc;
	pages [5].pfnDlgProc = CfgEditFilesDialogProc;
	pages [0].pszTemplate = MAKEINTRESOURCE (CFGEDIT_TAB_DIALOG);
	pages [1].pszTemplate = MAKEINTRESOURCE (CFGEDIT_FEATURES_DIALOG);
	pages [2].pszTemplate = MAKEINTRESOURCE (CFGEDIT_BLOCK_DIALOG);
	pages [3].pszTemplate = MAKEINTRESOURCE (CFGEDIT_SHOW_DIALOG);
	pages [4].pszTemplate = MAKEINTRESOURCE (CFGEDIT_ENCODING_DIALOG);
	pages [5].pszTemplate = MAKEINTRESOURCE (CFGEDIT_FILES_DIALOG);

	memset (&psh, 0, sizeof (psh));
	psh.dwSize = sizeof (psh); 
	psh.dwFlags = PSH_PROPSHEETPAGE;
	psh.hwndParent = FrameWindow;
	psh.hInstance = MyInstance;
	psh.pszIcon = 0;
	psh.pszCaption = "Configure Editor";
	psh.nPages = 6;
        psh.pStartPage = 0;
	psh.nStartPage = cfgedit_n_active_page;
	psh.ppsp = pages;
	
	PropertyDialog (&psh);
	for (i = 0; i < 6; i ++)
		if (pages [i].pszTemplate == MAKEINTRESOURCE (cfgedit_active_page))
			cfgedit_n_active_page = i;
	write_edit_options ();
}

void	Set_editor_params (HWND edit)
{
	Editor_set_param (edit, BS_JOINS_LINES,	BSJoinsLines);
	Editor_set_param (edit, DEL_JOINS_LINES,DelJoinsLines);
	Editor_set_param (edit, CR_SPLITS_LINE,	CrSplitsLine);
	Editor_set_param (edit, TAB_TYPES,	TabTypes);
	Editor_set_param (edit, TAB_AS_SPACES,	!HardTabs);
	Editor_set_param (edit, SMART_TABS,	PrevLineTabs);
	Editor_set_param (edit, CR_INDENTS,	AutoIndent);

	Editor_set_param (edit, EDITOR_FONT,		(DWORD) EditFont);
	Editor_set_param (edit, EDITOR_SHOW_LEFT_MARGIN, (DWORD) ShowLeftMargin);
	Editor_set_param (edit, EDITOR_SHOW_RIGHT_MARGIN, (DWORD) ShowRightMargin);
	Editor_set_param (edit, EDITOR_RIGHT_MARGIN,	(DWORD) RightMargin);
	Editor_set_param (edit, EDITOR_SHOW_TABS,	(DWORD) ShowTabs);
	Editor_set_param (edit, EDITOR_SHOW_BOOKMARKS,	(DWORD) ShowBookmarks);

	Editor_set_param (edit, EDITOR_PASTE_POS,	(DWORD) (long) PastePosition);
	Editor_set_param (edit, EDITOR_BLOCK_TYPE,	(DWORD) (long) TypeWhenBlock);
	Editor_set_param (edit, EDITOR_BLOCK_PASTE,	(DWORD) (long) PasteWhenBlock);
	Editor_set_param (edit, EDITOR_TAB_SIZE,	(DWORD)TabSize);
	InvalidateRect (edit, 0, 0);
}

/* ------------------------------------------------------------------------------------------------ */

static	LANG_CFG_DATA tmp_langs [MAX_LANG_DEFS];
static	int tmp_nlangs;
int	curlang;

void	store_cur_lang (HWND hwnd)
{
	char s [300];
	
	if (curlang >= 0)	{
		GetDlgItemText (hwnd, CFGLANG_DRIVER, s, sizeof (s));
		if (strcmp (s, tmp_langs [curlang].dll ? tmp_langs [curlang].dll : ""))	{
			free (tmp_langs [curlang].dll);
			tmp_langs[curlang].dll = alloc_copy (s);
		}
		GetDlgItemText (hwnd, CFGLANG_EXTENSIONS, s, sizeof (s));
		if (strcmp (s, tmp_langs [curlang].ext ? tmp_langs [curlang].ext : ""))	{
			free (tmp_langs [curlang].ext);
			tmp_langs[curlang].ext = alloc_copy (s);
		}
	}
}

void	set_cur_lang (HWND hwnd, int i)
{
	char s [300];
	
	if (i >= tmp_nlangs) i = -1;
	
	if (curlang != i)
		store_cur_lang (hwnd);
	curlang = i;
	if (i < 0)	{
		SetDlgItemText (hwnd, CFGLANG_DRIVERTITLE, "Language driver");
		SetDlgItemText (hwnd, CFGLANG_EXTENSIONS_TITLE, "Extensions");
		SetDlgItemText (hwnd, CFGLANG_DRIVER, "");
		SetDlgItemText (hwnd, CFGLANG_EXTENSIONS, "");
	} else	{
		sprintf (s, "Language driver for %s", tmp_langs [i].name);
		SetDlgItemText (hwnd, CFGLANG_DRIVERTITLE, s);
		sprintf (s, "Extensions of %s source files", tmp_langs [i].name);
		SetDlgItemText (hwnd, CFGLANG_EXTENSIONS_TITLE, s);
		SetDlgItemText (hwnd, CFGLANG_DRIVER, tmp_langs [i].dll ? tmp_langs [i].dll : "");
		SetDlgItemText (hwnd, CFGLANG_EXTENSIONS, tmp_langs [i].ext ? tmp_langs [i].ext : "");
	}

	EnableWindow (GetDlgItem (hwnd, CFGLANG_DRIVER), i>=0);
	EnableWindow (GetDlgItem (hwnd, CFGLANG_EXTENSIONS), i>=0);
	EnableWindow (GetDlgItem (hwnd, CFGLANG_BROWSE), i>=0);
	EnableWindow (GetDlgItem (hwnd, CFGLANG_REMOVE), i>=0);
}

BOOL CALLBACK CfgLangDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	char s [300];
	char dll [2000];
	char * p;

	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		for (i = 0; i < tmp_nlangs; i++)
			SendDlgItemMessage (hwnd, CFGLANG_LIST, LB_ADDSTRING, 0, (LPARAM) tmp_langs [i].name);
		curlang = tmp_nlangs ? 0 : -1;
		SendDlgItemMessage (hwnd, CFGLANG_LIST, LB_SETCURSEL, 0, 0);
		set_cur_lang (hwnd, 0);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			store_cur_lang (hwnd);
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (CFGLANG_DIALOG);
		case CFGLANG_LIST:
			if (NOTIFY_CODE == LBN_SELCHANGE)	{
				i = (int) SendDlgItemMessage (hwnd, CFGLANG_LIST, LB_GETCURSEL, 0, 0);
				set_cur_lang (hwnd, i);
			}
			return 1;
		case CFGLANG_ADD:
			if (tmp_nlangs == MAX_LANG_DEFS)
				return 1;
			if (!Ask_text ("Enter language name", "New language", s, sizeof (s), 0))
				return 1;
			if (!s) return 1;
			p = alloc_copy (s);
			if (!p) return 1;
			tmp_langs [tmp_nlangs].name = p;
			tmp_langs [tmp_nlangs].dll = NULL;
			tmp_langs [tmp_nlangs].ext = NULL;
			++ tmp_nlangs;
			SendDlgItemMessage (hwnd, CFGLANG_LIST, LB_ADDSTRING, 0, (LPARAM) p);
			SendDlgItemMessage (hwnd, CFGLANG_LIST, LB_SETCURSEL, tmp_nlangs-1, 0);
			set_cur_lang (hwnd, tmp_nlangs-1);
			return 1;
		case CFGLANG_REMOVE:
			if (curlang < 0) return 1;
			free (tmp_langs [curlang].name);
			free (tmp_langs [curlang].dll);
			free (tmp_langs [curlang].ext);
			memmove (&tmp_langs [curlang], &tmp_langs [curlang+1],
				sizeof (LANG_CFG_DATA) * (tmp_nlangs-curlang-1));
			--tmp_nlangs;
			SendDlgItemMessage (hwnd, CFGLANG_LIST, LB_DELETESTRING, curlang, 0);
			if (curlang >= tmp_nlangs) --curlang;
			set_cur_lang (hwnd, curlang);
			SendDlgItemMessage (hwnd, CFGLANG_LIST, LB_SETCURSEL, curlang, 0);
			return 1;
		case CFGLANG_BROWSE:
			browse_string (hwnd, CFGLANG_DRIVER, find_dll_filter,
				     "Select language driver", FALSE, FALSE, FALSE);
			return 1;
		case CFGLANG_CONFIGURE:
			if (curlang < 0) return 1;
			GetDlgItemText (hwnd, CFGLANG_DRIVER, dll, sizeof (dll));
			if (dll[0])
				Configure_lang_driver (hwnd, tmp_langs [curlang].name, dll);
			return 1;
		}
		return 1;
	case WM_HELP:
		return Control_help (CFGLANG_DIALOG, lparam);
	}
	return 0;
}

void	Configure_languages (void)
{
	int i;
	tmp_nlangs = Get_lang_def_data (tmp_langs);
	if (tmp_nlangs < 0) return;
	if (Dialog (CFGLANG_DIALOG, CfgLangDialogProc))	{
		Set_lang_def_data (tmp_langs, tmp_nlangs);
		Refresh_edit_tokens ();
	}
	for (i = 0; i < tmp_nlangs; i++)	{
		free (tmp_langs[i].name);
		free (tmp_langs[i].ext);
		free (tmp_langs[i].dll);
	}
}

/* ------------------------------------------------------------------------------------------------ */

HWND	sample_list, sample_msg, sample, sample_list_wnd, sample_msg_wnd;
HBRUSH	msg_brush, list_brush, shell_brush;
HFONT	temp_ListFont, temp_MsgFont;
int	edit_font_size, list_font_size, msg_font_size;
BOOL	edit_colors_changed;

TBUF * sample_bufs [MAX_LANG_DEFS];
HWND   sample_edits [MAX_LANG_DEFS];
HWND   sample_wnds  [MAX_LANG_DEFS];
int	cur_elems [MAX_LANG_DEFS];
int	cur_lang, cur_elem;
int     nsamples;
HWND	active_sample_child;
char *  FontDlgTitle;
int	edit_color_page;
int	messages_color_page;
int	list_color_page;
HWND	ColorDialog;
HWND	color_childs [5];
int	TabPos = 0;


void	change_tab_page (void);
void	set_edit_cur_lang (HWND hwnd, int i);

void	activate_sample_child (HWND hwnd)
{
	int i, page;
	
	if (active_sample_child == hwnd) return;
	BringWindowToTop (hwnd);
	if (active_sample_child)
		FlashWindow (active_sample_child, FALSE);
	FlashWindow (hwnd, TRUE);
	active_sample_child = hwnd;
	page = -1;
	if (hwnd == sample_msg_wnd)
		page = messages_color_page;
	else if (hwnd == sample_list_wnd)
		page = list_color_page;
	else
		for (i = 0; i < nsamples; i++)
			if (sample_wnds [i] == hwnd)	{
				page = edit_color_page;
				set_edit_cur_lang (color_childs [page], i);
				break;
			}
	if (page >= 0 && page != (int) SendDlgItemMessage (ColorDialog, COLOR_TAB, TCM_GETCURSEL, 0, 0))	{
		SendDlgItemMessage (ColorDialog, COLOR_TAB, TCM_SETCURSEL, page, 0);
		change_tab_page ();
	}
}

UINT CALLBACK FontTitleHook (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	IGNORE_PARAM (wparam);
	IGNORE_PARAM (lparam);
	IGNORE_PARAM (hwnd);

	if (msg == WM_INITDIALOG)
		SetWindowText (hwnd, FontDlgTitle);
	return 0;
}

BOOL	choose_font (HFONT *font, int * size, char * title, BOOL fixed)
{
	CHOOSEFONT cf;
	LOGFONT lf;
	HFONT f;
	BOOL b;

	memset (&cf, 0, sizeof (CHOOSEFONT));
	GetObject (*font, sizeof (LOGFONT), &lf);
	cf.lStructSize = sizeof (CHOOSEFONT);
	cf.hwndOwner = FrameWindow;
	cf.lpLogFont = &lf;
	cf.lpfnHook = FontTitleHook;
	FontDlgTitle = title;
	cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_ENABLEHOOK;
	if (fixed) cf.Flags |= CF_FIXEDPITCHONLY;
	lock_help ++;
	b = ChooseFont (&cf);
	lock_help --;
	if (!b) return FALSE;
	f = CreateFontIndirect (&lf);
	if (!f) return FALSE;
	*font = f;
	*size = cf.iPointSize;
	return TRUE;
}

void	config_e_handler (int code, EDIT_NOTIFY * data, DWORD param)
{
	char parsing [2000];
	char * p;
	int x, y, n, len, displen, parselen, elem;
	TBUF * buf;
	if (code != EDIT_NOTIFY_CURSOR && code != EDIT_NOTIFY_FOCUS)
		return;
	if (!data->has_focus)
		return;
	n = (int) param;
	if (!langs [n].lang_block.line_parser) return;
	buf = sample_bufs [n];
	if (!tbuf_get_line (buf, data->y1, &p, &len))
		return;
	x = data->x1;
	y = data->y1;
	if (x >= len)
		return;
	displen = x + 1;
	if (displen >= len) displen = len;
	if (displen >= sizeof (parsing)-10) displen = sizeof (parsing)-10;
	parselen = langs [n].lang_block.line_parser (langs [n].env, p, parsing, len, displen,
		   y ? tbuf_get_parsing_data (buf)[y-1] : 0, tbuf_get_parsing_data (buf)[y]);
	if (x && (p[x] == ' ' || p[x]=='\t') && p[x-1] != ' ' && p[x-1] != '\t') --x;
	if (x >= parselen)
		elem = 0;
	else
		elem = (unsigned char) parsing [x];
	SetWindowText (ColorDialog, elem==0?"Generic":langs [n].lang_block.class_display_names [elem-1]);
	cur_elems [n] = elem;
	set_edit_cur_lang (color_childs [edit_color_page], n);
}

char SampleText [] =
	"You either didn't specify language driver\n"
	"for %s language or specified incorrect one\n"
	"that can't be loaded or that doesn't provide\n"
	"sample text in this language.\n"
	"The IDE has to use this text instead\n";

void	create_edit_samples (void)
{
	int i;
	char * p;
	TBUF * buf;
	HWND wnd, edit;
	RECT rc;
	int x, y, w, h;
	char s [1000];

       	GetClientRect (sample, &rc);
	x = rc.right * 2/10;
	y = 0;
	w = rc.right - x - 40;
	h = rc.bottom - 50;

	nsamples = n_langs;
	memset (sample_bufs,  0, sizeof (sample_bufs));
	memset (sample_edits, 0, sizeof (sample_edits));
	memset (sample_wnds,  0, sizeof (sample_wnds));
	for (i = nsamples-1; i >= 0; i --)	{
		buf = create_tbuf (TabSize);
		if (!buf) return;
		p = NULL;
		if (langs [i].lang_block.get_sample_text)
			p = langs [i].lang_block.get_sample_text (langs [i].handle,
								  langs [i].env);
		if (!p)	{
			sprintf (s, SampleText, langs [i].name);
			p = s;
		}
		if (!tbuf_set_text (buf, p, strlen (p), TRUE, FALSE))	{
			free_tbuf (buf);
			return;
		}
		if (i)
			sprintf (s, "%s text sample", langs [i].name);
		else
			strcpy (s, "Generic edit window");
		wnd = CreateWindow ("SampleWindow", s,
				     WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW|
				     WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				     x, y, w, h,
				     sample,
				     NULL, MyInstance, NULL);
		x += 20; y += 20;
		if (!wnd)	{
			free_tbuf (buf);
			return;
		}
		SendMessage (wnd, WM_SETICON, 0, (LPARAM) EditSmIcon);
		edit = Editor_create (buf, wnd);
		if (!edit)	{
			DestroyWindow (wnd);
			free_tbuf (buf);
			return;
		}
		Editor_set_insert (edit, NewEditorInsert);
		Editor_set_param (edit, EDITOR_FONT, (DWORD) temp_EditFont);
		SetWindowLong (wnd, GWL_USERDATA, (DWORD) edit);
		Editor_set_lang_token (edit, TEMP_TOKEN (langs [i].token));
		BringWindowToTop (wnd);
		sample_bufs  [i] = buf;
		sample_wnds  [i] = wnd;
		sample_edits [i] = edit;
		Editor_set_notify_handler (edit, config_e_handler, (DWORD) i);
	}
}

void	invalidate_edits (void)
{
	int i;

	for (i = 0; i < nsamples; i++)
		InvalidateRect (sample_edits [i], 0, 0);
	edit_colors_changed = TRUE;
}

void	set_edits_font (void)
{
	int i;

	for (i = 0; i < nsamples; i++)
		Editor_set_param (sample_edits [i], EDITOR_FONT, (DWORD) temp_EditFont);
}

void	create_sample (char * cname, char * title, UINT style,
		       int x, int y, int cx, int cy,
		       HWND * wnd, HWND * inner)
{
	RECT rc;
	*wnd = CreateWindow ("SampleWindow", title,
			     WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW|
			     WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				x, y, cx, cy,
				sample,	NULL, MyInstance, NULL
			);
	GetClientRect (*wnd, &rc);
	*inner = CreateWindow (cname, "",
			       WS_CHILD | WS_VISIBLE | style,
			       0, 0, rc.right, rc.bottom,
			       *wnd, NULL, MyInstance, NULL);
	SetWindowLong (*wnd, GWL_USERDATA, (DWORD) *inner);
}

void	apply_colors (void)
{
	BOOL wnd_color_changed;
	BOOL need_write = 0;
	HFONT f;
	int i;

	wnd_color_changed = temp_WndForeColor != WndForeColor || temp_WndBackColor != WndBackColor;
	
	WndForeColor = temp_WndForeColor;
	WndBackColor = temp_WndBackColor;

	if (wnd_color_changed || temp_EditFont != EditFont || edit_font_size != EditFontSize || edit_colors_changed)	{
		edit_colors_changed = FALSE;
		for (i = 0; i < n_langs; i++)	{
			memcpy (langs [i].norm, langs [i].temp_norm, sizeof (langs [i].norm));
			memcpy (langs [i].sel,  langs [i].temp_sel,  sizeof (langs [i].sel));
			memcpy (langs [i].high, langs [i].temp_high, sizeof (langs [i].high));
			langs [i].highlight_disabled = langs [i].temp_highlight_disabled;
		}

		need_write = 1;
		f = EditFont;
		EditFont = temp_EditFont;
		EditFontSize = edit_font_size;
		Refresh_edits ();
		if (f != EditFont) DeleteObject (f);
	}
	if (wnd_color_changed || temp_ListFont != ListFont || list_font_size != ListFontSize ||
	    temp_ListForeColor != ListForeColor || temp_ListBackColor != ListBackColor)	{
		need_write = 1;
		ListForeColor = temp_ListForeColor;
		ListBackColor = temp_ListBackColor;
		f = ListFont;
		ListFont = temp_ListFont;
		ListFontSize = list_font_size;
		Refresh_file_list ();
		if (f != ListFont)
			DeleteObject (f);
	}
	if (wnd_color_changed || temp_MsgFont != MsgFont || msg_font_size != MsgFontSize ||
	    temp_MsgForeColor != MsgForeColor || temp_MsgBackColor != MsgBackColor)	{
		need_write = 1;
		MsgForeColor = temp_MsgForeColor;
		MsgBackColor = temp_MsgBackColor;
		f = MsgFont;
		MsgFont = temp_MsgFont;
		MsgFontSize = msg_font_size;
		Refresh_message_window ();
		if (f != MsgFont)
			DeleteObject (f);
	}
	if (temp_ShellColor != ShellColor)	{
		need_write = 1;
		ShellColor = temp_ShellColor;
		InvalidateRect ((HWND) SendMessage (FrameWindow, WM_MYMDI_GETCLIENT, 0, 0), 0, 0);
	}

	if (need_write)	{
		write_colors ();
		Write_lang_colors ();
	}
}

BOOL CALLBACK ColorWorkspaceDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		Colorbox_fill (hwnd, COLOR_WORKSPACE_BACK, temp_ShellColor);
		return 1;
	case WM_MEASUREITEM:
		return Colorbox_measure (hwnd, lparam);
	case WM_DRAWITEM:
		return Colorbox_draw_item (lparam);
	case MSG_GET_DEFAULT_COLOR:
		SetWindowLong (hwnd, DWL_MSGRESULT, GetSysColor (COLOR_APPWORKSPACE));
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case COLOR_WORKSPACE_BACK:
			if (!Colorbox_notify (hwnd, wparam, lparam, &temp_ShellColor))
				return FALSE;
			DeleteObject (shell_brush);
			shell_brush = CreateSolidBrush (AbsSysColor (temp_ShellColor, COLOR_APPWORKSPACE));
			InvalidateRect (sample, 0, 1);
			return 1;
		}
		return 1;
	}
	return 0;
}

BOOL CALLBACK ColorWindowsDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	COLORREF c;
	switch (msg)	{
	case WM_INITDIALOG:
		Colorbox_fill (hwnd, COLOR_WINDOWS_BACK, temp_WndBackColor);
		Colorbox_fill (hwnd, COLOR_WINDOWS_FORE, temp_WndForeColor);
		return 1;
	case WM_MEASUREITEM:
		return Colorbox_measure (hwnd, lparam);
	case WM_DRAWITEM:
		return Colorbox_draw_item (lparam);
	case MSG_GET_DEFAULT_COLOR:
		if (wparam == COLOR_WINDOWS_BACK)
			c = GetSysColor (COLOR_WINDOW);
		else
			c = GetSysColor (COLOR_WINDOWTEXT);
		SetWindowLong (hwnd, DWL_MSGRESULT, (LPARAM) c);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case COLOR_WINDOWS_BACK:
			if (!Colorbox_notify (hwnd, wparam, lparam, &temp_WndBackColor))
				return FALSE;
			DeleteObject (msg_brush);
			msg_brush = CreateSolidBrush (AbsColor (temp_MsgBackColor, AbsSysColor (temp_MsgBackColor, COLOR_WINDOW)));
			DeleteObject (list_brush);
			list_brush = CreateSolidBrush (AbsColor (temp_ListBackColor, AbsSysColor (temp_ListBackColor, COLOR_WINDOW)));
			InvalidateRect (sample_msg, 0, TRUE);
			InvalidateRect (sample_list, 0, TRUE);
			invalidate_edits ();
			return 1;
		case COLOR_WINDOWS_FORE:
			if (!Colorbox_notify (hwnd, wparam, lparam, &temp_WndForeColor))
				return FALSE;
			InvalidateRect (sample_msg, 0, TRUE);
			InvalidateRect (sample_list, 0, TRUE);
			invalidate_edits ();
			return 1;
		}
		return 1;
	}
	return 0;
}

void	fill_lang_list (HWND hwnd)
{
	int i;
	for (i = 0; i < n_langs; i++)
		SendMessage (hwnd, CB_ADDSTRING, 0, (LPARAM) langs[i].name);
	SendMessage (hwnd, CB_SETCURSEL, cur_lang, 0);
}

void	update_color_boxes (HWND hwnd, int i, int j)
{
	CheckDlgButton  (hwnd, COLOR_EDIT_DISABLE, langs [cur_lang].temp_highlight_disabled);
	
	Colorbox_update (hwnd, COLOR_EDIT_BACK, langs [cur_lang].temp_norm [cur_elem].bk);
	Colorbox_update (hwnd, COLOR_EDIT_FORE, langs [cur_lang].temp_norm [cur_elem].fg);
	Colorbox_update (hwnd, COLOR_EDIT_SELBACK, langs [cur_lang].temp_sel [cur_elem].bk);
	Colorbox_update (hwnd, COLOR_EDIT_SELFORE, langs [cur_lang].temp_sel [cur_elem].fg);
	Colorbox_update (hwnd, COLOR_EDIT_HIGHBACK, langs [cur_lang].temp_high [cur_elem].bk);
	Colorbox_update (hwnd, COLOR_EDIT_HIGHFORE, langs [cur_lang].temp_high [cur_elem].fg);
}

void	enable_edit_color_boxes (HWND hwnd)
{
	BOOL enable_lang_select = !langs [0].temp_highlight_disabled;
	BOOL enable_elem_select = !langs [0].temp_highlight_disabled &&
				  cur_lang && !langs [cur_lang].temp_highlight_disabled;
	
	EnableWindow (GetDlgItem (hwnd, COLOR_EDIT_LANG), enable_lang_select);
	EnableWindow (GetDlgItem (hwnd, COLOR_EDIT_ELEM), enable_elem_select);

	if (! enable_lang_select)
		SendDlgItemMessage (hwnd, COLOR_EDIT_LANG, CB_SETCURSEL, 0, 0);
	if (! enable_elem_select)
		SendDlgItemMessage (hwnd, COLOR_EDIT_ELEM, CB_SETCURSEL, 0, 0);
}

void	set_edit_cur_lang (HWND hwnd, int i)
{
	int j;
	char s [500];

	activate_sample_child (sample_wnds [i]);

	if (langs [0].temp_highlight_disabled)
		i = 0;
	
	cur_lang = i;
	cur_elem = cur_elems [i];
	if (langs [i].temp_highlight_disabled)
		cur_elem = 0;

	strcpy (s, "Disable coloring");
	if (i)	{
		strcat (s, " for ");
		strcat (s, langs [i].name);
	}
	SetDlgItemText (hwnd, COLOR_EDIT_DISABLE, s);
	SendDlgItemMessage (hwnd, COLOR_EDIT_LANG, CB_SETCURSEL, cur_lang, 0);
	SendDlgItemMessage (hwnd, COLOR_EDIT_ELEM, CB_RESETCONTENT, 0, 0);
	SendDlgItemMessage (hwnd, COLOR_EDIT_ELEM, CB_ADDSTRING, 0, (LPARAM) "Generic text");
	for (j = 0; j < langs [i].lang_block.nclasses; j++)
		SendDlgItemMessage (hwnd, COLOR_EDIT_ELEM, CB_ADDSTRING, 0, (LPARAM) langs [i].lang_block.class_display_names[j]);
	SendDlgItemMessage (hwnd, COLOR_EDIT_ELEM, CB_SETCURSEL, cur_elem, 0);
	enable_edit_color_boxes (hwnd);
	update_color_boxes (hwnd, cur_lang, cur_elem);
}

BOOL CALLBACK ColorEditDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	COLORREF color;
	
	switch (msg)	{
	case WM_INITDIALOG:
		fill_lang_list (GetDlgItem (hwnd, COLOR_EDIT_LANG));
		Colorbox_fill (hwnd, COLOR_EDIT_BACK, langs [cur_lang].temp_norm [cur_elem].bk);
		Colorbox_fill (hwnd, COLOR_EDIT_FORE, langs [cur_lang].temp_norm [cur_elem].fg);
		Colorbox_fill (hwnd, COLOR_EDIT_SELBACK, langs [cur_lang].temp_sel [cur_elem].bk);
		Colorbox_fill (hwnd, COLOR_EDIT_SELFORE, langs [cur_lang].temp_sel [cur_elem].fg);
		Colorbox_fill (hwnd, COLOR_EDIT_HIGHBACK, langs [cur_lang].temp_high [cur_elem].bk);
		Colorbox_fill (hwnd, COLOR_EDIT_HIGHFORE, langs [cur_lang].temp_high [cur_elem].fg);
		set_edit_cur_lang (hwnd, cur_lang);
		return 1;
	case WM_MEASUREITEM:
		return Colorbox_measure (hwnd, lparam);
	case WM_DRAWITEM:
		return Colorbox_draw_item (lparam);
	case MSG_GET_DEFAULT_COLOR:
		switch (wparam)	{
		case COLOR_EDIT_BACK:
			color = AbsSysColor (temp_WndBackColor, COLOR_WINDOW);
			if (cur_lang)
				color = AbsColor (langs [0].temp_norm [0].bk, color);
			if (cur_elem)
				color = AbsColor (langs [cur_lang].temp_norm [0].bk, color);
			break;
		case COLOR_EDIT_FORE:
			color = AbsSysColor (temp_WndForeColor, COLOR_WINDOWTEXT);
			if (cur_lang)
				color = AbsColor (langs [0].temp_norm [0].fg, color);
			if (cur_elem)
				color = AbsColor (langs [cur_lang].temp_norm [0].fg, color);
			break;
		case COLOR_EDIT_SELBACK:
			color = GetSysColor (COLOR_HIGHLIGHT);
			if (cur_lang)
				color = AbsColor (langs [0].temp_sel [0].bk, color);
			if (cur_elem)
				color = AbsColor (langs [cur_lang].temp_sel [0].bk, color);
			break;
		case COLOR_EDIT_SELFORE:
			color = GetSysColor (COLOR_HIGHLIGHTTEXT);
			if (cur_lang)
				color = AbsColor (langs [0].temp_sel [0].fg, color);
			if (cur_elem)
				color = AbsColor (langs [cur_lang].temp_sel [0].fg, color);
			break;
		case COLOR_EDIT_HIGHFORE:
			color = Colorbox_color (hwnd, COLOR_EDIT_FORE);
			break;
		case COLOR_EDIT_HIGHBACK:
			color = light_color (Colorbox_color (hwnd, COLOR_EDIT_BACK));
			break;
		}
		SetWindowLong (hwnd, DWL_MSGRESULT, color);
		return 1;
	case WM_SHOWWINDOW:
		if (wparam)
			activate_sample_child (sample_wnds [cur_lang]);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case COLOR_EDIT_LANG:
			if (NOTIFY_CODE != CBN_CLOSEUP) return 0;
			i = (int) SendDlgItemMessage (hwnd, COLOR_EDIT_LANG, CB_GETCURSEL, 0, 0);
			set_edit_cur_lang (hwnd, i);
			update_color_boxes (hwnd, cur_lang, cur_elem);
			return 1;
		case COLOR_EDIT_ELEM:
			if (NOTIFY_CODE != CBN_CLOSEUP) return 0;
			cur_elem = (int) SendDlgItemMessage (hwnd, COLOR_EDIT_ELEM, CB_GETCURSEL, 0, 0);
			cur_elems [cur_lang] = cur_elem;
			update_color_boxes (hwnd, cur_lang, cur_elem);
			return 1;
		case COLOR_EDIT_FORE:
			if (!Colorbox_notify (hwnd, wparam, lparam, &langs [cur_lang].temp_norm [cur_elem].fg))
				return FALSE;
			InvalidateRect (GetDlgItem (hwnd, COLOR_EDIT_HIGHFORE), 0, 0);
			invalidate_edits ();
			return 1;
		case COLOR_EDIT_BACK:
			if (!Colorbox_notify (hwnd, wparam, lparam, &langs [cur_lang].temp_norm [cur_elem].bk))
				return FALSE;
			InvalidateRect (GetDlgItem (hwnd, COLOR_EDIT_HIGHBACK), 0, 0);
			invalidate_edits ();
			return 1;
		case COLOR_EDIT_SELFORE:
			if (!Colorbox_notify (hwnd, wparam, lparam, &langs [cur_lang].temp_sel [cur_elem].fg))
				return FALSE;
			invalidate_edits ();
			return 1;
		case COLOR_EDIT_SELBACK:
			if (!Colorbox_notify (hwnd, wparam, lparam, &langs [cur_lang].temp_sel [cur_elem].bk))
				return FALSE;
			invalidate_edits ();
			return 1;
		case COLOR_EDIT_HIGHFORE:
			if (!Colorbox_notify (hwnd, wparam, lparam, &langs [cur_lang].temp_high [cur_elem].fg))
				return FALSE;
			invalidate_edits ();
			return 1;
		case COLOR_EDIT_HIGHBACK:
			if (!Colorbox_notify (hwnd, wparam, lparam, &langs [cur_lang].temp_high [cur_elem].bk))
				return FALSE;
			invalidate_edits ();
			return 1;
		case COLOR_EDIT_FONT:
			if (choose_font (&temp_EditFont, &edit_font_size, "Select editor font", TRUE))
				set_edits_font ();
			break;
		case COLOR_EDIT_DISABLE:
			langs [cur_lang].temp_highlight_disabled = IsDlgButtonChecked (hwnd, COLOR_EDIT_DISABLE);
			set_edit_cur_lang (hwnd, cur_lang);
			invalidate_edits ();
			break;
		}
		return 1;
	}
	return 0;
}

BOOL CALLBACK ColorMessagesDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	COLORREF c;
	switch (msg)	{
	case WM_INITDIALOG:
		Colorbox_fill (hwnd, COLOR_MESSAGES_BACK, temp_MsgBackColor);
		Colorbox_fill (hwnd, COLOR_MESSAGES_FORE, temp_MsgForeColor);
		return 1;
	case WM_MEASUREITEM:
		return Colorbox_measure (hwnd, lparam);
	case WM_DRAWITEM:
		return Colorbox_draw_item (lparam);
	case MSG_GET_DEFAULT_COLOR:
		if (wparam == COLOR_MESSAGES_BACK)
			c = AbsSysColor (temp_WndBackColor, COLOR_WINDOW);
		else
			c = AbsSysColor (temp_WndForeColor, COLOR_WINDOWTEXT);
		SetWindowLong (hwnd, DWL_MSGRESULT, (LPARAM) c);
		return 1;
	case WM_SHOWWINDOW:
		if (wparam)
			activate_sample_child (sample_msg_wnd);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case COLOR_MESSAGES_BACK:
			if (!Colorbox_notify (hwnd, wparam, lparam, &temp_MsgBackColor))
				return FALSE;
			DeleteObject (msg_brush);
			msg_brush = CreateSolidBrush (AbsColor (temp_MsgBackColor, AbsSysColor (temp_MsgBackColor, COLOR_WINDOW)));
			InvalidateRect (sample_msg, 0, TRUE);
			return 1;
		case COLOR_MESSAGES_FORE:
			if (!Colorbox_notify (hwnd, wparam, lparam, &temp_MsgForeColor))
				return FALSE;
			InvalidateRect (sample_msg, 0, TRUE);
			return 1;
		case COLOR_MESSAGES_FONT:
			choose_font (&temp_MsgFont, &msg_font_size, "Select font for Messages window", FALSE);
			SendMessage (sample_msg, WM_SETFONT, (WPARAM) temp_MsgFont, 0);
			InvalidateRect (sample_msg, 0, 1);
		}
		return 1;
	}
	return 0;
}

BOOL CALLBACK ColorFlistDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	COLORREF c;
	switch (msg)	{
	case WM_INITDIALOG:
		Colorbox_fill (hwnd, COLOR_FLIST_BACK, temp_ListBackColor);
		Colorbox_fill (hwnd, COLOR_FLIST_FORE, temp_ListForeColor);
		return 1;
	case WM_MEASUREITEM:
		return Colorbox_measure (hwnd, lparam);
	case WM_DRAWITEM:
		return Colorbox_draw_item (lparam);
	case MSG_GET_DEFAULT_COLOR:
		if (wparam == COLOR_FLIST_BACK)
			c = AbsSysColor (temp_ListBackColor, COLOR_WINDOW);
		else
			c = AbsSysColor (temp_ListForeColor, COLOR_WINDOWTEXT);
		SetWindowLong (hwnd, DWL_MSGRESULT, (LPARAM) c);
		return 1;
	case WM_SHOWWINDOW:
		if (wparam)
			activate_sample_child (sample_list_wnd);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case COLOR_FLIST_BACK:
			if (!Colorbox_notify (hwnd, wparam, lparam, &temp_ListBackColor))
				return FALSE;
			DeleteObject (list_brush);
			list_brush = CreateSolidBrush (AbsColor (temp_ListBackColor, AbsSysColor (temp_ListBackColor, COLOR_WINDOW)));
			InvalidateRect (sample_list, 0, TRUE);
			return 1;
		case COLOR_FLIST_FORE:
			if (!Colorbox_notify (hwnd, wparam, lparam, &temp_ListForeColor))
				return FALSE;
			InvalidateRect (sample_msg, 0, TRUE);
			return 1;
		case COLOR_FLIST_FONT:
			choose_font (&temp_ListFont, &list_font_size, "Select font form Files List", FALSE);
			SendMessage (sample_list, WM_SETFONT, (WPARAM) temp_ListFont, 0);
			InvalidateRect (sample_list, 0, 1);
			break;
		}
		return 1;
	}
	return 0;
}

HWND	child_dialog (int id, DLGPROC proc, RECT * rc)
{
	HWND hwnd = CreateDialog (MyInstance, MAKEINTRESOURCE (id), ColorDialog, proc);
	ShowWindow (hwnd, SW_HIDE);
	SetWindowPos (hwnd, 0, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_NOZORDER);
	return hwnd;
}

void	change_tab_page (void)
{
	int i;
	TabPos = TabCtrl_GetCurSel (GetDlgItem (ColorDialog, COLOR_TAB));
	ShowWindow (color_childs[TabPos], SW_SHOWNORMAL);
	for (i = 0; i < 5; i++)
		if (i != TabPos)
			ShowWindow (color_childs[i], SW_HIDE);
}

int	color_child_ids [5] = {
		COLOR_WORKSPACE_DIALOG,
		COLOR_WINDOWS_DIALOG,
		COLOR_EDIT_DIALOG,
		COLOR_MESSAGES_DIALOG,
		COLOR_FLIST_DIALOG
	};

DLGPROC color_child_procedures [5] = {
		ColorWorkspaceDlgProc,
		ColorWindowsDlgProc,
		ColorEditDlgProc,
		ColorMessagesDlgProc,
		ColorFlistDlgProc
	};

void	fill_list_box_by_resource (HWND box, int resource)
{
	char s [1000];
	int i;

	for (i = 0; i < 100; i++) {
		if (!LoadString (ResInstance, resource + i, s, sizeof (s)))
			break;
		SendMessage (box, LB_ADDSTRING, 0, (LPARAM) s);
	}
}

BOOL CALLBACK ColorDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HWND sp;
	RECT rc;
	NMHDR * nm;
	HWND tab;
	TC_ITEM tc;
	int i;

	switch (msg)	{
	case WM_INITDIALOG:
		ColorDialog = hwnd;
		Center_window (hwnd);
		tab = GetDlgItem (hwnd, COLOR_TAB);
		memset (&tc, 0, sizeof (tc));
		tc.mask = TCIF_TEXT;
		tc.pszText = "Workspace";
		TabCtrl_InsertItem (tab, 0, &tc);
		tc.pszText = "Windows";
		TabCtrl_InsertItem (tab, 1, &tc);
		tc.pszText = "Editor";
		TabCtrl_InsertItem (tab, 2, &tc);
		tc.pszText = "Messages";
		TabCtrl_InsertItem (tab, 3, &tc);
		tc.pszText = "File list";
		TabCtrl_InsertItem (tab, 4, &tc);

		Get_child_pos (tab, &rc);
		TabCtrl_AdjustRect (tab, 0, &rc);
		for (i = 0; i < 5; i++)
			color_childs [i] = child_dialog (color_child_ids [i],
							 color_child_procedures [i],
							 &rc);

		edit_color_page = 2;
		messages_color_page = 3;
		list_color_page = 4;

		TabCtrl_SetCurSel (tab, TabPos);
		ShowWindow (color_childs[TabPos], SW_SHOWNORMAL);

		sp = GetDlgItem (hwnd, COLOR_SAMPLE);
		Get_child_pos (sp, &rc);

		sample = CreateWindow ("STATIC", "",
			     WS_CHILD | WS_VISIBLE | SS_LEFT |
			     WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			     rc.left+2, rc.top+2, rc.right-rc.left-4, rc.bottom-rc.top-4,
			     hwnd, NULL, MyInstance, NULL
			);
		BringWindowToTop (sample);

		GetClientRect (sample, &rc);

		active_sample_child = 0;

		create_sample ("LISTBOX", "Project files List",
			       WS_VSCROLL |
			       LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT,
			       5, 10, rc.right/2, rc.bottom/2,
			       &sample_list_wnd, &sample_list);

		create_sample ("LISTBOX", "Messages",
			       WS_VSCROLL |
			       LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT,
			       20, rc.bottom*2/3-20, rc.right-10, rc.bottom-rc.bottom*2/3+20,
			       &sample_msg_wnd, &sample_msg);

		create_edit_samples ();

		SendMessage (sample_list_wnd, WM_SETICON, 0, (LPARAM) FListSmIcon);
		SendMessage (sample_msg_wnd,  WM_SETICON, 0, (LPARAM) MessageSmIcon);

		fill_list_box_by_resource (sample_msg,  IDSTR_SAMPLE_MESSAGES);
		fill_list_box_by_resource (sample_list, IDSTR_SAMPLE_FILES);
/*		
		SendMessage (sample_msg, LB_ADDSTRING, 0, (LPARAM) "Error in line 153 of crocodile.mod: ; expected");
		SendMessage (sample_msg, LB_ADDSTRING, 0, (LPARAM) "Warning in line 82 of turtle.mod: code has no effect");
		SendMessage (sample_msg, LB_ADDSTRING, 0, (LPARAM) "Warning in line 82 of parrot.ob2: variable declared but never used");

		SendMessage (sample_list, LB_ADDSTRING, 0, (LPARAM) "c:\\project\\src\\crocodile.mod");
		SendMessage (sample_list, LB_ADDSTRING, 0, (LPARAM) "c:\\project\\src\\parrot.ob2");
		SendMessage (sample_list, LB_ADDSTRING, 0, (LPARAM) "c:\\project\\src\\turtle.mod");
*/
		SendMessage (sample_list, WM_SETFONT, (WPARAM) temp_ListFont, 0);
		SendMessage (sample_msg,  WM_SETFONT, (WPARAM) temp_MsgFont, 0);

		SendMessage (sample_list, LB_SETCURSEL, 1, 0);
		SendMessage (sample_msg,  LB_SETCURSEL, 0, 0);
		return 1;

	case WM_CTLCOLORSTATIC:
		if ((HWND) lparam == sample)
			return (BOOL) shell_brush;
		break;
	case WM_NOTIFY:
		if (wparam == COLOR_TAB)	{
			nm = (NMHDR*) lparam;
			if (nm->code == TCN_SELCHANGE)	{
				change_tab_page ();
				return 1;
			}
		}
		return 0;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case COLOR_APPLY:
			apply_colors ();
			break;
		case IDHELP:
			i = TabCtrl_GetCurSel (GetDlgItem (ColorDialog, COLOR_TAB));
			return Dialog_help (color_child_ids [i]);
		}
		return 1;
	case WM_HELP:
		return Control_help (COLOR_DIALOG, lparam);
	}
	return 0;
}


void	Configure_colors (void)
{
	int i;

	for (i = 0; i < n_langs; i++)	{
		memcpy (langs [i].temp_norm, langs [i].norm, sizeof (langs [i].norm));
		memcpy (langs [i].temp_sel,  langs [i].sel,  sizeof (langs [i].sel));
		memcpy (langs [i].temp_high, langs [i].high, sizeof (langs [i].high));
		langs [i].temp_highlight_disabled = langs [i].highlight_disabled;
		cur_elems [i] = 0;
	}
	cur_lang = 0;

	temp_ShellColor = ShellColor;

	temp_WndBackColor = WndBackColor;
	temp_WndForeColor = WndForeColor;
	
	temp_MsgForeColor  = MsgForeColor;
	temp_MsgBackColor  = MsgBackColor;

	temp_ListForeColor = ListForeColor;
	temp_ListBackColor = ListBackColor;

	temp_EditFont = EditFont;
	temp_ListFont = ListFont;
	temp_MsgFont  = MsgFont;
	edit_font_size = EditFontSize;
	list_font_size = ListFontSize;
	msg_font_size  = MsgFontSize;

	list_brush = CreateSolidBrush (AbsColor (temp_ListBackColor, AbsSysColor (temp_ListBackColor, COLOR_WINDOW)));
	msg_brush  = CreateSolidBrush (AbsColor (temp_MsgBackColor, AbsSysColor (temp_MsgBackColor, COLOR_WINDOW)));
	shell_brush = CreateSolidBrush (AbsSysColor (temp_ShellColor, COLOR_APPWORKSPACE));

	edit_colors_changed = FALSE;

	if (Dialog (COLOR_DIALOG, ColorDialogProc))
		apply_colors ();

	DeleteObject (list_brush);
	DeleteObject (msg_brush);
	DeleteObject (shell_brush);
}

LRESULT CALLBACK SampleWndProc (HWND hwnd, UINT msg,
				 WPARAM wparam, LPARAM lparam)
{
	int i, id;
	HDC dc;
	HMENU menu;
	HWND child = (HWND) GetWindowLong (hwnd, GWL_USERDATA);
	
	switch (msg)	{
	case WM_CREATE:
		menu = GetSystemMenu (hwnd, FALSE);
		for (i = GetMenuItemCount (menu)-1; i>=0; i--)	{
			id = GetMenuItemID (menu, i);
			if (id != SC_MOVE && id != SC_SIZE)
				DeleteMenu (menu, i, MF_BYPOSITION);
		}
		break;
	case WM_CLOSE:
		return 0;
	case WM_SIZE:
		MoveWindow (child, 0, 0, LOWORD (lparam), HIWORD (lparam), 1);
		break;
	case WM_PARENTNOTIFY:
		if (LOWORD (wparam) != WM_LBUTTONDOWN &&
		    LOWORD (wparam) != WM_RBUTTONDOWN)
			break;
	case WM_LBUTTONDOWN:
	case WM_NCLBUTTONDOWN:
	case WM_SETFOCUS:
		activate_sample_child (hwnd);
		SetFocus (child);
		break;
	case WM_CTLCOLORLISTBOX:
		dc = (HDC) wparam;
		if ((HWND) lparam == sample_msg)	{
			SetBkColor   (dc, AbsColor (temp_WndBackColor, AbsSysColor (temp_MsgBackColor, COLOR_WINDOW)));
			SetTextColor (dc, AbsColor (temp_WndForeColor, AbsSysColor (temp_MsgForeColor, COLOR_WINDOWTEXT)));
			return (LRESULT) msg_brush;
		} else if ((HWND) lparam == sample_list)	{
			SetBkColor   (dc, AbsColor (temp_WndBackColor, AbsSysColor (temp_ListBackColor, COLOR_WINDOW)));
			SetTextColor (dc, AbsColor (temp_WndForeColor, AbsSysColor (temp_ListForeColor, COLOR_WINDOWTEXT)));
			return (LRESULT) list_brush;
		}
		break;
	case WM_COMMAND:
		if (CONTROL_HWND == child &&
		    NOTIFY_CODE == LBN_SETFOCUS)
			activate_sample_child (hwnd);
		break;
	}
	return DefWindowProc (hwnd, msg, wparam, lparam);
}

BOOL	Register_config (void)
{
	WNDCLASSEX wc;

	wc.cbSize 	 = sizeof (WNDCLASSEX);
	wc.style	 = 0;
	wc.lpfnWndProc   = (LPVOID) SampleWndProc;
	wc.hInstance     = MyInstance;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hIcon 	 = 0;
	wc.hCursor 	 = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "SampleWindow";
	wc.lpszMenuName  = NULL;
	wc.hIconSm 	 = 0;
	if (!RegisterClassEx (&wc)) return FALSE;

	return TRUE;
}

void	Read_options (void)
{
	read_general_options ();
	read_edit_options ();
	read_colors ();
        read_fonts ();
	read_print_options ();
}

