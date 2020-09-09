#include "util.h"
#include "fileutil.h"
#include "edit.h"
#include "open.h"
#include "shell.h"
#include "projred.h"
#include "config.h"
#include "res.h"
#include <cderr.h>

#define FILE_HISTORY_LEN 100
#define FILE_MENU_HISTORY_LEN 5
char *	file_history [FILE_HISTORY_LEN] = {0};

#define OPEN_HISTORY_LEN 100
char *	open_history [OPEN_HISTORY_LEN] = {0};

int	FileHistorySelected;

char filename [_MAX_PATH+1];
/*
char open_filter[] =	"Modula-2 source files (*.mod;*.def)\0*.mod;*.def\0"
			"Oberon-2 source files (*.ob2)\0*.ob2\0"
			"XDS project files (*.prj)\0*.prj\0"
			"XDS style sheets (*.sty)\0*.sty\0"
			"C/C++ source files (*.c;*.cpp;*.h;*.hpp)\0*.c;*.cpp;*.h;*.hpp\0"
			"Assembly language files (*.asm)\0*.asm\0"
			"Resource scripts (*.rc)\0*.rc\0"
			"All files (*.*)\0*.*\0";
*/
int	edit_filter_index = 1;
char	EditCurDir [STRLEN];

BOOL CALLBACK MoreFilesDlgProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	int i;
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		for (i = 0; i < FILE_HISTORY_LEN && file_history [i]; i ++)
			SendDlgItemMessage (hwnd, MOREFILES_LBOX, LB_ADDSTRING, 0,
				     (LPARAM)(LPSTR) file_history [i]);
		SendDlgItemMessage (hwnd, MOREFILES_LBOX, LB_SETCURSEL, 0, 0);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case MOREFILES_LBOX:
			if (NOTIFY_CODE != LBN_DBLCLK) break;
		case IDOK:
			FileHistorySelected = SendDlgItemMessage (hwnd, MOREFILES_LBOX, LB_GETCURSEL, 0, 0);
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDHELP:
			return Dialog_help (MOREFILES_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (MOREFILES_DIALOG, lparam);
		return 1;
	}
	return 0;
}

void	MoreFiles (void)
{
	FileHistorySelected = -1;
	if (!Dialog (MOREFILES_DIALOG, MoreFilesDlgProc))
		return;
	if (FileHistorySelected < 0) return;
	Open_by_history (FileHistorySelected);
}

BOOL CALLBACK OpenFileProc   (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)	{
	case WM_INITDIALOG:
		Center_window (hwnd);
		Fill_history_box (GetDlgItem (hwnd, OPENFILE_COMBOBOX), open_history, OPEN_HISTORY_LEN);
		SendDlgItemMessage (hwnd, OPENFILE_COMBOBOX, CB_SETCURSEL, 0, 0);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			GetDlgItemText (hwnd, OPENFILE_COMBOBOX, filename, _MAX_PATH);
			Insert_history (open_history, OPEN_HISTORY_LEN, filename);
			if (wildcard_name (filename))	{
				if (Browse_file_name (FALSE, NULL, filename, open_filter, filename, &edit_filter_index, EditCurDir))
					EndDialog (hwnd, 1);
				else
					break;
			}
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case OPENFILE_BROWSE:
			GetDlgItemText (hwnd, OPENFILE_COMBOBOX, filename, _MAX_PATH);
			if (Browse_file_name (FALSE, NULL, filename, open_filter, filename, &edit_filter_index, EditCurDir))
				EndDialog (hwnd, 1);
			break;
		case OPENFILE_FILELIST:
			FileHistorySelected = -1;
			if (!Dialog (MOREFILES_DIALOG, MoreFilesDlgProc))
				break;
			if (FileHistorySelected < 0) break;
			SetDlgItemText (hwnd, OPENFILE_COMBOBOX, file_history [FileHistorySelected]);
			break;
		case IDHELP:
			return Dialog_help (OPENFILE_DIALOG);
		}
		return 1;
	case WM_HELP:
		return Control_help (OPENFILE_DIALOG, lparam);
		return 1;
	}
	return 0;
}

BOOL	ask_file_name (BOOL save)
{
	return Dialog (OPENFILE_DIALOG, OpenFileProc);
}

void	Add_file_history (HMENU menu)
{
	if (Add_history_to_menu (menu, file_history, FILE_MENU_HISTORY_LEN,
			     IDM_FILELAST, IDM_FIRST_FILEOPEN))
		AppendMenu (menu, MF_ENABLED|MF_STRING, IDM_MOREFILES, "More Files...");
}

void	Insert_into_file_history (int file)
{
	Insert_history (file_history, FILE_HISTORY_LEN, filedir [file]);
}

char *	File_name_from_history (int i)
{
	if (i >= FILE_HISTORY_LEN) return 0;
	return file_history [i];
}

void	Write_file_history (char * IniFile)
{
	Write_history (file_history, FILE_HISTORY_LEN, "file history", IniFile);
}

void	Read_file_history (char * IniFile)
{
	Read_history  (file_history, FILE_HISTORY_LEN, _MAX_PATH*2,
			"file history", IniFile);
}

void	Write_open_history (char * IniFile)
{
	Write_history (open_history, OPEN_HISTORY_LEN, "open history", IniFile);
}

void	Read_open_history (char * IniFile)
{
	Read_history  (open_history, OPEN_HISTORY_LEN, _MAX_PATH*2,
			"open history", IniFile);
}

HWND	Open_by_name (char * name, int how)
{
	HWND	ewnd;
	char fullname [STRLEN];
	int file;
	
	Redirect_search_file (name, fullname);
	file = new_filedir (fullname);

	ewnd = Edit_read_text (file, name, how, TRUE);

	if (ewnd)	{
		Insert_into_file_history (file);
		Edit_show (ewnd);
	}
	return ewnd;
}

void	Open (void)
{
	if (ask_file_name (FALSE))
		Open_by_name (filename, ASK_SHARE_EXISTING_BUFFER);
}

void	Open_by_history (int num)
{
	char * s = File_name_from_history (num);
	if (s)
		Open_by_name (s, ASK_SHARE_EXISTING_BUFFER);
}

int	Ask_save_file_name (int file)
{
	DWORD err;
	if (!Browse_file_name (TRUE, "", file == NOFILE ? "" : filedir [file],
				open_filter, filename, &edit_filter_index, EditCurDir))
	{
		err = CommDlgExtendedError ();
		if (err != FNERR_INVALIDFILENAME)
			return NOFILE;
		if (!Browse_file_name (TRUE, "", "", open_filter, filename, &edit_filter_index, EditCurDir))
			return NOFILE;
	}
	return new_filedir (filename);
}
