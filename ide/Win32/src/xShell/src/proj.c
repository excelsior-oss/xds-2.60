#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "res.h"
#include "edit.h"
#include "find.h"
#include "proj.h"
#include "tools.h"
#include "message.h"
#include "options.h"
#include "optedit.h"
#include "projred.h"
#include "parse.h"
#include "fcache.h"
#include "flist.h"
#include "open.h"
#include "make.h"
#include "config.h"
#include "var.h"

int	ProjFile = -1;
int	OptFile = -1;
BOOL	ProjWasOpened = FALSE;

int	proj_filter_index = 1;
int	templ_filter_index = 1;
int	style_filter_index = 1;

#define PROJ_HISTORY_LEN 5
char * proj_history [PROJ_HISTORY_LEN+1] = {0};

#define STYLE_SHEETS_NUM 20
char * style_sheets [STYLE_SHEETS_NUM][2] = {0};

#define STYLE_SHEETS_NAME "style sheets"

static char open_proj_filter[] =	"XDS project files (*.prj)\0*.prj\0"
					"All files (*.*)\0*.*\0";

static char write_style_filter[] =	"XDS style sheets (*.sty)\0*.sty\0"
					"XDS project files (*.prj)\0*.prj\0"
					"All files (*.*)\0*.*\0";

/* ---------------------------------------------------------------------- */

void	Proj_set_vars (char * s)
{
	char d [STRLEN], ff [STRLEN];
	char * name_portion;

	if (!s[0])	{
		Set_name ("project", "");
		Set_name ("projname", "");
		Set_name ("projext", "");
		Set_name ("projwholename", "");
	} else	{
		if (!GetFullPathName (s, sizeof (ff), ff, &name_portion))
			strcpy (ff, s);
		add_ext (ff, ff, "prj");
		Set_name ("project", ff);
		Set_name ("projwholename", name_only (ff));
		change_ext (name_only (ff), d, 0);
		Set_name ("projname", d);
		Set_name ("projext", ext_only (ff));
		strcpy (ProjDir, ff);
		dir_only (ProjDir);
		strcpy (ProjIni, ff);
		change_ext (ProjIni, ProjIni, "ini");
	}
	Set_name ("projdir", ProjDir);
	Set_name ("projini", ProjIni);
	Create_tools_menu ();
}

void	Proj_set (void)
{
	if (ProjFile <= 0)	{
		Proj_set_vars ("");
		Proj_enable_menu ();
		Set_title ("");
		Read_run_options ();
		Read_search_settings ();
		return;
	}
	
	Proj_set_vars (filedir [ProjFile]);
	Proj_enable_menu ();
	Set_title (filedir [ProjFile]);
	Insert_history (proj_history, PROJ_HISTORY_LEN, filedir [ProjFile]);
	SetCurrentDirectory (ProjDir);
	Read_run_options ();
	Read_search_settings  ();
}

BOOL	close_project (void)
{
	int r;
	if (RunProcess)	{
		r = MsgBox ("Closing project", MB_YESNOCANCEL | MB_ICONEXCLAMATION,
			    "User process still running. Do you want to terminate it ?");
		if (r == IDCANCEL) return FALSE;
		if (r == IDYES)	RunStop ();
	}

	if (! CloseAll ()) return FALSE;
	Write_search_history ();
	if (ProjFile == -1) return TRUE;
	ProjFile = -1;
	Set_title (NULL);
	Clear_messages ();
	Proj_enable_menu ();
	Proj_set_vars ("");
	ProjWasOpened = FALSE;
	return TRUE;
}

void	Proj_open_by_name (char * filename)
{
	char f [_MAX_PATH+1];
	LPSTR name_portion;
	int file;

	file = new_filedir (filename);
	if (!Test_file_exists (filename))	{
		if (MsgBox ("Error", MB_YESNO|MB_ICONSTOP,
			"Can't find project file %s\n"
			"Do you want to create new project?", filedir [file])
		    == IDYES
		)
			Proj_new (file);
		return;
	}
	if (!Test_file_readable (filename))	{
		MsgBox ("Error", MB_OK|MB_ICONSTOP,
			"Can't open project file %s.\n%s",
			filedir [file], ErrText ());
		return;
	}
	
	if (! close_project ())	{
		return;
	}
	ProjFile = file;
	GetFullPathName (filename, sizeof (f), f, &name_portion);
	*name_portion = 0;
	if (name_portion != f && name_portion[-1]=='\\')
		name_portion[-1] = 0;
	strcpy (ProjDir, f);
	SetCurrentDirectory (ProjDir);
	Proj_set ();
	Read_windows (ProjIni);
	Read_file_list (ProjIni);
	ProjWasOpened = TRUE;
}

void	Open_null_project (void)
{
	strcpy (ProjDir, IniFile);
	dir_only (ProjDir);
	strcpy (ProjIni, ProjDir);
	strcat (ProjIni, "\\noproj.ini");
	SetCurrentDirectory (ProjDir);
	Proj_set ();
	Read_windows (ProjIni);
	Read_file_list (ProjIni);
}

BOOL	Proj_close (void)
{
	if (!close_project ()) return FALSE;
	Open_null_project ();
	return TRUE;
}

BOOL	Test_special_title (int file, char * title, BOOL lng)
{
	if (file == ProjFile)	{
		if (lng)
			sprintf (title, "Project text: %s", filedir [file]);
		else
			strcpy (title, "Project");
		return TRUE;
	}
	if (file == OptFile)	{
		if (lng)
			sprintf (title, "XDS config file: %s", filedir [file]);
		else
			strcpy (title, "XDS config file");
		return TRUE;
	}
	return FALSE;
}

void	Proj_edit (void)
{
	if (ProjFile == -1) return;
	Open_by_name (filedir [ProjFile], OPEN_EXISTING_BUFFER);
}

void	Options_edit (WINDOWPLACEMENT * pl)
{
}

void	Proj_run (void)
{
	HWND hwnd = (HWND) SendMessage (FrameWindow, WM_MDIGETACTIVE, 0, 0);
	if (! Tool_make (FALSE)) return;
	if (err_count || severe_count) return;
	SendMessage (FrameWindow, WM_MDIACTIVATE, (WPARAM) hwnd, 0);
	RunRun ();
}

void	Proj_debug (void)
{
	HWND hwnd = (HWND) SendMessage (FrameWindow, WM_MDIGETACTIVE, 0, 0);
	if (! Tool_make (FALSE)) return;
	if (err_count || severe_count) return;
	SendMessage (FrameWindow, WM_MDIACTIVATE, (WPARAM) hwnd, 0);
	RunDebug ();
}

void	Proj_stop (void)
{
	RunStop ();
}

BOOL	Is_proj_opened (void)
{
	return ProjFile >= 0;
}

/* ------------------------ options editing -------------------- */

ParsedProject test_project (int file, char * header, char * name)
{
	int cnt;
	ParsedProject proj;

	Clear_messages ();
	proj = Parse_project (file, TRUE, &cnt);
	if (! proj)	{
		if (cnt < 0)	{
			MsgBox (header, MB_OK | MB_ICONSTOP,
				"Error reading %s %s:\n%s",
				name, filedir [file], ErrText ());
			return NULL;
		}
		MsgBox (header, MB_OK | MB_ICONEXCLAMATION,
			"Parsing %s %s:\n"
			"Syntax errors detected: %d",
			name, filedir [file], cnt);
		Show_messages ();
		First_message ();
		return NULL;
	}
	return proj;
}

void	edit_proj_options (int projfile, char * header, char * name, char * boxname)
{
	int r, ecnt, cfgfile;
	ParsedProject proj, cfg;

	proj = test_project (projfile, header, name);
	if (! proj)
		return;

	cfg = NULL;
	cfgfile = Redirect_find_cfg_file ();
	if (cfgfile >= 0)
		cfg = Parse_project (cfgfile, FALSE, &ecnt);

	Set_proj_to_edit (proj, cfg);
	if (Is_complex (proj))	{
		r = MsgBox ("Attention", MB_OKCANCEL | MB_ICONEXCLAMATION,
			"The %s has complicated syntax that can't be "
			"entirely parsed by Options Editor.\n"
			"You can still edit it but it is unsafe.\n"
			"Do you want to proceed editing ?", name);
		if (r == IDCANCEL)	{
			Clear_project_parsing (proj);
			Clear_project_parsing (cfg);
			return;
		}
	}

	Show_option_box (boxname);
	Clear_project_parsing (proj);
	Clear_project_parsing (cfg);
}

void	OpenOptions (void)
{
	edit_proj_options (ProjFile, "Project Errors", "project file", "Modifying project: ");
}

void	Proj_enable_menu (void)
{
	BOOL styles = style_sheets [0][0] != 0;
	BOOL state  = ProjFile != -1;
	Enable_item (IDM_PROJCLOSE,   state);
	Enable_item (IDM_PROJOPTIONS, state);
	Enable_item (IDM_PROJEDIT,    state);
	Enable_item (IDM_FILELIST,    state);
	Enable_item (IDM_STOP,	      RunProcess != 0);
	Enable_item (IDM_VIEWUSERS,   RunProcess != 0 || SameConsole && ConsoleHandle);
	Enable_item (IDM_SHOWMESS,    state);
	Enable_item (IDM_HIDEMESS,    state);
	Enable_item (IDM_NEXTMESS,    state);
	Enable_item (IDM_PREVMESS,    state);
	Enable_item (IDM_PROJSAVESTYLE, state);
	Enable_item (IDM_APPLYSTYLE, state && styles);
	Enable_item (IDM_EDITSTYLE, styles);
	Enable_item (IDM_DELETESTYLE, styles);
	Msg_proj_enable (state);
		Tools_enable ();
	UpdateWindow (ToolBar);
}

void	Proj_open (void)
{
	char	filename [_MAX_PATH+1];

	filename [0] = 0;
	if (!Browse_file_name (FALSE, "Select project file name",
			       filename, open_proj_filter, filename, &proj_filter_index, NULL))
		return;
	Proj_open_by_name (filename);
}

void	Proj_open_by_history (int num)
{
	if (num < PROJ_HISTORY_LEN && proj_history [num])
		Proj_open_by_name (proj_history [num]);
}

char * Proj_name_from_history (int i)
{
	if (i < PROJ_HISTORY_LEN && proj_history [i])
		return proj_history [i];
	return 0;
}


void	Add_project_history (HMENU menu)
{
	Add_history_to_menu (menu, proj_history, PROJ_HISTORY_LEN,
			     IDM_PROJLAST, IDM_FIRST_PROJOPEN);
}

void	Write_proj_history (char * IniFile)
{
	Write_history (proj_history, PROJ_HISTORY_LEN, "project history", IniFile);
	WritePrivateProfileString ("general", "project-opened", ProjWasOpened ? "on":"off", IniFile);
}

void	Read_proj_history (char * IniFile)
{
	char s [20];
	GetPrivateProfileString ("general", "project-opened", "off", s, sizeof (s)-1, IniFile);
	ProjWasOpened = !stricmp (s, "on");
	Read_history  (proj_history, PROJ_HISTORY_LEN, _MAX_PATH*2,
			"project history", IniFile);
	Read_section  (style_sheets, STYLE_SHEETS_NUM,
			STYLE_SHEETS_NAME, IniFile);
}

char * Last_project (void)
{
	return ProjWasOpened ? proj_history [0] : "";
}

/* ----------------------------- new project creation ---------------------- */

#define MAXDIRS 10

static  char * dir_string, * def_start_mod;

char *	dirlist [MAXDIRS];
int	ndirs;
BOOL	createdirs = TRUE;
BOOL	createred  = TRUE;
BOOL	createmainmod = TRUE;

int	newproj_file;
int	template_file = 0;
int	start_file;

char	start_file_name [MAX_PATH];

int	parse_semicolon (char * s, char ** p, int maxlen)
{
	char * q;
	int i = 0;
	while (i < maxlen)	{
		while (*s && isspace (*s)) ++s;
		if (!*s) return i;

		if (*s == ';') {
			++s;
			continue;
		}

		p [i++] = s;
		while (*s && *s != ';') ++ s;
		if (!*s) return i;
		for (q = s-1; isspace (*q); --q);
		q[1] = 0;
		++s;
	}
	return i;
}

void	assign_newproj_vars (HWND hwnd)
{
	char s [STRLEN];
	GetDlgItemText (hwnd, NEWPROJ_PROJECT,  s, sizeof (s));
	if (wildcard_name (s))
		Proj_set_vars (".\\NewProject.prj");
	else
		Proj_set_vars (s);
}

BOOL CALLBACK NewProjProc   (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char f [_MAX_PATH+1], ff [_MAX_PATH];
	char s [STRLEN];

	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		SetDlgItemText (hwnd, NEWPROJ_STARTFILE, def_start_mod);
		EnableWindow   (GetDlgItem (hwnd, NEWPROJ_STDMODULE), def_start_mod [0] != 0);
		CheckDlgButton (hwnd, NEWPROJ_CREATEDIR, ndirs && createdirs);
		CheckDlgButton (hwnd, NEWPROJ_CREATERED, createred);
		CheckDlgButton (hwnd, NEWPROJ_CREATEMAINMOD, createmainmod);
		if (! ndirs)
			EnableWindow (GetDlgItem (hwnd, NEWPROJ_CREATEDIR), FALSE);
		if (! std_template_file[0])
			EnableWindow (GetDlgItem (hwnd, NEWPROJ_STDTEMPL), FALSE);
		Make_line (std_template_file, s, sizeof (s));
		SetDlgItemText (hwnd, NEWPROJ_TEMPLATE,
				template_file ? filedir [template_file]:
				s [0] ? s : "");

		if (newproj_file)	{
			SetDlgItemText (hwnd, NEWPROJ_PROJECT, filedir [newproj_file]);
			SendDlgItemMessage (hwnd, NEWPROJ_PROJECT, EM_SETSEL, 0, strlen (filedir [newproj_file]));
		} else	{
			strcpy (f, ProjDir);
			if (strlen (f) == 0 || f [strlen (f) - 1] != '\\')
				strcat (f, "\\");
			if (f [strlen (f) - 1] == '\\')
				strcat (f, "*.prj");
			SetDlgItemText (hwnd, NEWPROJ_PROJECT, f);
			SendDlgItemMessage (hwnd, NEWPROJ_PROJECT, EM_SETSEL, strlen (f)-5, strlen (f)-4);
		}
		SetFocus (GetDlgItem (hwnd, NEWPROJ_PROJECT));
		return 0;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case NEWPROJ_PROJECT:
			if (NOTIFY_CODE == EN_CHANGE)
				assign_newproj_vars (hwnd);
			break;
		case IDOK:
			createdirs = IsDlgButtonChecked (hwnd, NEWPROJ_CREATEDIR);
			createred  = IsDlgButtonChecked (hwnd, NEWPROJ_CREATERED);
			createmainmod = IsDlgButtonChecked (hwnd, NEWPROJ_CREATEMAINMOD);
			GetDlgItemText (hwnd, NEWPROJ_PROJECT,  f, _MAX_PATH);
			if (!f[0])	{
				MsgBox ("Error", MB_ICONEXCLAMATION|MB_OK, "Project name mustn't be left blank");
				SetFocus (GetDlgItem (hwnd, NEWPROJ_PROJECT));
				break;
			}
			if (wildcard_name (f))
				goto Do_browse_project_name;
			add_ext (f, ff, "prj");
			newproj_file = new_filedir (ff);

			GetDlgItemText (hwnd, NEWPROJ_TEMPLATE, f, _MAX_PATH);
			template_file = 0;
			Make_line (f, ff, sizeof (ff));
			if (ff [0])	{
				if (wildcard_name (ff))
					goto Do_browse_template_name;
				template_file = new_filedir (ff);
			}
			GetDlgItemText (hwnd, NEWPROJ_STARTFILE, start_file_name,
					sizeof (start_file_name));
			Make_line (start_file_name, ff, sizeof (ff));
			if (wildcard_name (ff))
				goto Do_browse_start_file_name;

			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case NEWPROJ_PROJBROWSE:
	Do_browse_project_name:
			GetDlgItemText (hwnd, NEWPROJ_PROJECT,  f, _MAX_PATH);
			if (!Browse_file_name (FALSE, "Select name for a new project",
					  f, open_proj_filter, f, &proj_filter_index, NULL))
				break;
			SetDlgItemText (hwnd, NEWPROJ_PROJECT,  f);
			assign_newproj_vars (hwnd);
			break;
		case NEWPROJ_TEMPLBROWSE:
	Do_browse_template_name:
			GetDlgItemText (hwnd, NEWPROJ_TEMPLATE,  f, _MAX_PATH);
			Make_line (f, ff, sizeof (ff));
			if (Browse_file_name (FALSE, "Select template file",
						  ff, open_proj_filter, ff, &templ_filter_index, NULL))
				SetDlgItemText (hwnd, NEWPROJ_TEMPLATE, ff);
			break;
		case NEWPROJ_FILEBROWSE:
	Do_browse_start_file_name:
			GetDlgItemText (hwnd, NEWPROJ_STARTFILE,  f, _MAX_PATH);
			assign_newproj_vars (hwnd);
			Make_line (f, ff, sizeof (ff));
			if (!Browse_file_name (FALSE, "Select main module",
					  ff, open_filter, ff, &edit_filter_index, NULL))
				break;
			SetDlgItemText (hwnd, NEWPROJ_STARTFILE,  ff);
			break;
		case NEWPROJ_STDTEMPL:
			if (std_template_file [0])	{
				assign_newproj_vars (hwnd);
				Make_line (std_template_file, s, sizeof (s));
				SetDlgItemText (hwnd, NEWPROJ_TEMPLATE, s);
			}
			break;
		case NEWPROJ_STDMODULE:
			SetDlgItemText (hwnd, NEWPROJ_STARTFILE, def_start_mod);
			break;
		case NEWPROJ_CONFIGURE:
			if (Configure_new_project ())	{
				Read_newproj_ini_file (dir_string, def_start_mod);
				ndirs = parse_semicolon (dir_string, dirlist, MAXDIRS);
				EnableWindow (GetDlgItem (hwnd, NEWPROJ_CREATEDIR), ndirs != 0);
				EnableWindow (GetDlgItem (hwnd, NEWPROJ_STDTEMPL), std_template_file [0] != 0);
				EnableWindow (GetDlgItem (hwnd, NEWPROJ_STDMODULE), def_start_mod [0] != 0);
			}
			break;
		case IDHELP:
			return Dialog_help (NEWPROJ_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (NEWPROJ_DIALOG, lparam);
	}
	return 0;
}

void	copy_red_file (void)
{
	char s [STRLEN], f [STRLEN];
	int r;

	strcpy (s, ProjDir);
	strcat (s, "\\");
	strcat (s, RedFileName);

	if (std_red_file[0])      {
		Make_line (std_red_file, f, STRLEN);
read_redfile_again:
		if (! Copy_and_expand_file (f, s))	{
			if (ErrCode == ERROR_CANCELLED) return;
			r = MsgBox ("Error", MB_RETRYCANCEL|MB_ICONSTOP,
				"Can't open redirection file %s:\n%s", f, ErrText ());
			if (r == IDCANCEL) return;
			goto read_redfile_again;
		}
	}
}

BOOL	try_proj_new (int file)
{
	int r;
	char dirs_str [STRLEN];
	char def_start_mod_string [STRLEN];
	char d[STRLEN], smodule [STRLEN];
	BOOL template_used = FALSE;

	newproj_file = file;
	start_file = 0;
	dir_string = dirs_str;
	def_start_mod = def_start_mod_string;
	Read_newproj_ini_file (dir_string, def_start_mod);
	ndirs = parse_semicolon (dirs_str, dirlist, MAXDIRS);
	SetCurrentDirectory (ProjDir);

	if (!Dialog (NEWPROJ_DIALOG, NewProjProc))
		return FALSE;

	SetCurrentDirectory (ProjDir);

	if (!Proj_close ()) return FALSE;

	Proj_set_vars (filedir [newproj_file]);
	SetCurrentDirectory (ProjDir);
	if (createdirs && ! Create_dirs (ndirs, dirlist))
		return FALSE;

	Make_line (start_file_name, smodule, sizeof (smodule));
	Set_name ("mainmodule", smodule);

	if (template_file)      {
read_template_again:
		if (! Copy_and_expand_file (filedir [template_file], filedir [newproj_file]))	{
			if (ErrCode == ERROR_CANCELLED) return FALSE;
			r = MsgBox ("Error", MB_RETRYCANCEL|MB_ICONSTOP,
				"Can't open template file %s:\n%s",
				filedir [template_file], ErrText ());
			if (r == IDCANCEL) return FALSE;
			goto read_template_again;
		} else
			template_used = TRUE;
	} else	{
		if (!Put_file_start ()) return FALSE;
		if (!Put_file_fin (newproj_file, FALSE)) return FALSE;
	}

	if (createred && std_red_file[0])
		copy_red_file ();

	ProjFile = newproj_file;

	if (! template_used)
		if (smodule[0] && ! Replace_start_file (newproj_file, smodule))
			MsgBox ("Error", MB_OK | MB_ICONHAND, "Error inserting start file name %s\n"
			"Into project file %s", smodule, filedir [newproj_file]);

	Clear_file_list ();
	Proj_set ();
	Set_name ("mainmodule", "");

	if (smodule[0])	{
		Redirect_create_file (smodule, d);
		Add_file_list (new_filedir (d));
		Update_file_list ();
		if (createmainmod)
			Open_by_name (smodule, OPEN_EXISTING_BUFFER);
	} else
		Proj_edit ();
	return TRUE;
}

void	Proj_new (int file)
{
	BOOL b;

	b = try_proj_new (file);
	if (!b)
		Proj_set ();
}

/* ----------------------------- Style sheets operations ------------------- */

#define STYLE_SUFFIX ".STY"

void	make_filename (int i, char * buf)
{
	if (style_sheets [i][1])
		strcpy (buf, style_sheets [i][1]);
	else	{
		strcpy (buf, style_sheets [i][0]);
		strcat (buf, STYLE_SUFFIX);
	}
}

void	make_full_style_name (char * src, char * dst)
{
	add_dir (IniFile, src, dst);
}

BOOL wr_style_error;

BOOL wr_enum_proc (char * line)
{
	wr_style_error = wr_style_error || Put_file (line, strlen (line));
	return !wr_style_error;
}

BOOL	write_style_sheet (int file, char * name, char * filename, BOOL prompt)
{
	char s [2048];
	ParsedProject proj;
	int errcnt, sheetfile;

	if (file)	{
		proj = Parse_project (file, FALSE, &errcnt);
		if (!proj)
			return FALSE;
	}
	sheetfile = new_filedir (name);
	if (! Put_file_start ()) return FALSE;
	sprintf (s, "%% Style sheet: %s\n\n", name);
	Put_file (s, strlen (s));
	wr_style_error = FALSE;
	if (file) Enum_proj_options (proj, wr_enum_proc);
	if (file) Clear_project_parsing (proj);
	if (wr_style_error) return FALSE;
	return Put_file_fin (sheetfile, TRUE);
}

BOOL filename_edited = FALSE;

int save_style_project;
char save_style_name [_MAX_PATH*2];
int  style_selected = -1;

BOOL CALLBACK SaveStyleProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	LRESULT sel;

	char name [_MAX_PATH-4], fname [_MAX_PATH];

	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		SendDlgItemMessage (hwnd, SAVESTYLE_NAMES, CB_LIMITTEXT, sizeof (name)-1, 0);
		SendDlgItemMessage (hwnd, SAVESTYLE_FILE, CB_LIMITTEXT,  sizeof (fname)-1, 0);
		EnableWindow (GetDlgItem (hwnd, IDOK), 0);
		for (i = 0; i < STYLE_SHEETS_NUM && style_sheets [i][0]; i ++)
			SendDlgItemMessage (hwnd, SAVESTYLE_NAMES, CB_ADDSTRING, 0,
			     (LPARAM)(LPSTR) style_sheets [i][0]);
		filename_edited = FALSE;
		EnableWindow (GetDlgItem (hwnd, SAVESTYLE_FILE), 0);
		EnableWindow (GetDlgItem (hwnd, SAVESTYLE_BROWSE), 0);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case SAVESTYLE_FILE:
			if (NOTIFY_CODE == EN_CHANGE)
				filename_edited = TRUE;
			return 1;
		case SAVESTYLE_NAMES:
			switch (NOTIFY_CODE)	{
			case CBN_SELCHANGE:
				sel = SendDlgItemMessage (hwnd, SAVESTYLE_NAMES, CB_GETCURSEL, 0,0);
				EnableWindow (GetDlgItem (hwnd, IDOK), sel != CB_ERR);
				EnableWindow (GetDlgItem (hwnd, SAVESTYLE_FILE), sel != CB_ERR);
				EnableWindow (GetDlgItem (hwnd, SAVESTYLE_BROWSE), sel != CB_ERR);
				if (sel != CB_ERR)	{
					make_filename (sel, fname);
					SetDlgItemText (hwnd, SAVESTYLE_FILE, fname);
					filename_edited = FALSE;
				}
				break;
			case CBN_EDITCHANGE:
				GetDlgItemText (hwnd, SAVESTYLE_NAMES, name, sizeof (name)-1);
				EnableWindow (GetDlgItem (hwnd, IDOK), name [0]!=0);
				EnableWindow (GetDlgItem (hwnd, SAVESTYLE_FILE), name [0]!=0);
				EnableWindow (GetDlgItem (hwnd, SAVESTYLE_BROWSE), name [0]!=0);
				if (filename_edited) return 1;
				if (!name[0])
					fname [0] = 0;
				else	{
					strcpy (fname, name);
					strcat (fname, STYLE_SUFFIX);
				}
				SetDlgItemText (hwnd, SAVESTYLE_FILE, fname);
				filename_edited = FALSE;
				return 1;
			}
			return 1;
		case IDOK:
			GetDlgItemText (hwnd, SAVESTYLE_NAMES, name, sizeof (name)-1);
			GetDlgItemText (hwnd, SAVESTYLE_FILE,  fname, sizeof (fname)-1);
			name [sizeof (name)-1] = fname [sizeof (fname)-1] = 0;
			if (!name[0])	{
				MsgBox ("Error", MB_ICONEXCLAMATION|MB_OK,
					"You should specify some style sheet name");
				SetFocus (GetDlgItem (hwnd, SAVESTYLE_NAMES));
				break;
			}
			if (!fname[0])	{
				strcpy (fname, name);
				strcat (fname, STYLE_SUFFIX);
				SetDlgItemText (hwnd, SAVESTYLE_FILE, fname);
			}
			make_full_style_name (fname, save_style_name);
			if (write_style_sheet (save_style_project, name, save_style_name, TRUE))	{
				if (Add_section_elem (style_sheets, STYLE_SHEETS_NUM,
				    name, fname, STYLE_SHEETS_NAME, IniFile))	{
					i = SendDlgItemMessage (hwnd, SAVESTYLE_NAMES, CB_ADDSTRING, 0,
							    (LPARAM)(LPSTR) name);
					if (i >= 0 && i < STYLE_SHEETS_NUM)	{
						SendDlgItemMessage (hwnd, SAVESTYLE_NAMES, CB_SETCURSEL, i, 0);
						style_selected = i;
					}
				EndDialog (hwnd, 1);
				}
			}
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case SAVESTYLE_BROWSE:
			GetDlgItemText (hwnd, SAVESTYLE_FILE, fname, _MAX_PATH);
			Browse_file_name (TRUE, "Select name for style sheet file",
					  fname, write_style_filter, fname, &style_filter_index, NULL);
			SetDlgItemText (hwnd, SAVESTYLE_FILE,  fname);
			filename_edited = TRUE;
			break;
		case IDHELP:
			return Dialog_help (SAVESTYLE_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (SAVESTYLE_DIALOG, lparam);
	}
	return 0;
}

void	Proj_save_as_style (void)
{
	if (ProjFile == -1) return;
	save_style_project = ProjFile;
	Dialog (SAVESTYLE_DIALOG, SaveStyleProc);
	Proj_enable_menu ();
}

void	edit_style (char * fname)
{
	char lfname [_MAX_PATH*2];
	int file;
	make_full_style_name (fname, lfname);
	file = new_filedir (lfname);
	edit_proj_options (file, "Style sheet errors", "style sheet file", "Style sheet: ");
}

void	Proj_new_style (void)
{
	save_style_project = 0;
	if (Dialog (SAVESTYLE_DIALOG, SaveStyleProc))
		edit_style (save_style_name);
	Proj_enable_menu ();
}

int dlgnum;

BOOL CALLBACK StyleDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	LRESULT sel;

	switch (msg)	{
	case WM_INITDIALOG:
		SetWindowText (hwnd, dlgnum == STYLE_DIALOG ? "Apply style sheet":
				dlgnum == DELETESTYLE_DIALOG ? "Delete style sheet":
				"Edit style sheet");
		Center_window (hwnd);
		EnableWindow (GetDlgItem (hwnd, IDOK), 0);
		for (i = 0; i < STYLE_SHEETS_NUM && style_sheets [i][0]; i ++)
			SendDlgItemMessage (hwnd, STYLE_LISTBOX, LB_ADDSTRING, 0,
			     (LPARAM)(LPSTR) style_sheets [i][0]);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case STYLE_LISTBOX:
			switch (NOTIFY_CODE)	{
			case LBN_SELCHANGE:
				EnableWindow (GetDlgItem (hwnd, IDOK), 1);
				break;
			case LBN_DBLCLK:
				SendMessage (hwnd, WM_COMMAND, IDOK, 0);
				break;
			}
			return 1;
		case IDOK:
			sel = SendDlgItemMessage (hwnd, STYLE_LISTBOX, LB_GETCURSEL, 0, 0);
			if (sel == LB_ERR) return 1;
			style_selected = sel;
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (dlgnum);
		}
		return 1;
	case WM_HELP:
		return Control_help (dlgnum, lparam);
	}
	return 0;
}

void	Proj_apply_style (void)
{
	char fname [_MAX_PATH];
	ParsedProject proj, sty;

	if (ProjFile == -1) return;
	dlgnum = STYLE_DIALOG;
	if (!Dialog (STYLE_DIALOG, StyleDlgProc))
		return;
	if (style_selected == -1) return;
	make_filename (style_selected, fname);
	proj = test_project (ProjFile, "Project Errors", "project file");
	if (!proj) return;
	sty  = test_project (new_filedir (fname), "Style sheet errors", "style sheet file");
	if (!sty)	{
		Clear_project_parsing (proj);		
		return;
	}
	if (!Apply_style (proj, sty))
		MsgBox ("Error", MB_OK|MB_ICONSTOP, "Error applying style sheet %s: out of memory",
				 style_sheets [style_selected][0]);
	else	{
		if (!Save_parsed_project (proj))	{
			MsgBox ("Saving project file", MB_OK,
				"Error saving project file %s after applying style sheet:\n%s",
				filedir [ProjFile], ErrText ());
		}
	}
	Clear_project_parsing (proj);
	Clear_project_parsing (sty);
}

void	Proj_edit_style (void)
{
	char fname [_MAX_PATH];
	dlgnum = EDITSTYLE_DIALOG;
	if (!Dialog (STYLE_DIALOG, StyleDlgProc))
		return;
	if (style_selected == -1) return;
	make_filename (style_selected, fname);
	edit_style (fname);
}

void	Proj_delete_style (void)
{
	int i;
	char fname [_MAX_PATH], lfname [_MAX_PATH*2];
	dlgnum = DELETESTYLE_DIALOG;
	if (!Dialog (STYLE_DIALOG, StyleDlgProc))
		return;
	if (style_selected == -1) return;
	make_filename (style_selected, fname);
	make_full_style_name (fname, lfname);

	i = MsgBox ("Delete style", MB_YESNOCANCEL | MB_ICONQUESTION,
		    "Do you want to delete file %s ?", lfname);
	if (i == IDCANCEL) return;
	if (i == IDYES)	{
		if (!DeleteFile (lfname))	{
			i = MsgBox ("Error", MB_YESNO | MB_ICONEXCLAMATION,
				    "Can't delete file %s\n"
				    "Do you want to delete reference?",
				    lfname);
			if (i == IDNO) return;
		}
	}
	Delete_section_item (style_sheets, STYLE_SHEETS_NUM,
			     style_selected, STYLE_SHEETS_NAME, IniFile);
	Proj_enable_menu ();
}


