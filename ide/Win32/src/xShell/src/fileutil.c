#include "util.h"
#include "fileutil.h"
#include "res.h"
#include <dlgs.h>

/* ---------------- General file name function ------ */

char * name_only (char * name)
{
	char * p;
	for (p = name + strlen (name); p > name; --p)
		if (p[-1] == '\\' || p[-1]==':')
			return p;
	return name;
}

char * vol_only (char * name)
{
	int i;
	if (name [0] == '\\')	{
		if (name [1] != '\\') return NULL;
		for (i = 2; name [i] && name [i] != '\\'; i++);
		if (!name [i]) return FALSE;
		++i;
		while (name [i] && name [i] != '\\') ++i;
		name [i] = '\\';
		name [i+1] = 0;
		return name;
	}
	if (name [1] == ':')	{
		name [2] = '\\';
		name [3] = 0;
		return name;
	}
	return NULL;
}

char * dir_only (char * name)
{
	int i;
	for (i = strlen (name)-1; i >= 0; i--)
		if (name [i] == '\\' || name [i] == '/')	{
			name [i] = 0;
			return name;
		}
		if (name [i] == ':')	{
			name [i+1] = 0;
			strcat (name, ".");
			return name;
		}
	name [0] = 0;
	return name;
}

char * 	ext_only (char * s)
{
	char *p;
	p = 0;
	for (s = name_only (s); *s; s++)
		if (*s == '.') p=s;
	if (!p) return s;
	return p+1;
}

void	change_ext (char * s, char * d, char * ext)
{
	char * n, *p;
	if (d != s) strcpy (d, s);
	p = 0;
	for (n = name_only (d); *n; n++)
		if (*n == '.') p=n;
	if (!ext || !ext [0])	{
		if (p) *p = 0;
		return;
	}
	if (!p)	{
		*n='.';
		p=n;
	}
	strcpy (p+1, ext);
}


void	add_ext (char * s, char * d, char * ext)
{
	char * n;
	if (d != s)
		strcpy (d, s);
	for (n = name_only (d); *n; n++)
		if (*n == '.') return;
	strcat (d, ".");
	strcat (d, ext);
}

BOOL	is_full_dir (char * p)
{
	if (p[0] && p[1]==':')
		return p[2] == '\\';
	return p[0] == '\\';
}

void	add_dir (char * dir, char * file, char * res)
{
	if (is_full_dir (file))	{
		strcpy (res, file);
		return;
	}
	strcpy (res, dir);
	dir_only (res);
	strcat (res, "\\");
	if (file [0] && file [1] == ':') file += 2;
	strcat (res, file);
}


BOOL	wildcard_name (char * s)
{
	return ! s[0] || strchr (s, '?') || strchr (s, '*') ||
		 s[strlen (s)-1] == '\\' || s[strlen (s)-1] == ':';
}

BOOL	match_suffix (char * p, char * suffixes)
{
	int i, j, k;
	char c;
	if (! suffixes [0]) return FALSE;
	if (suffixes [0] == '*') return TRUE;

	i = 0;
	for (;;)	{
		for (j = i; suffixes [j] && suffixes [j]!=';'; j++);
		c = suffixes [j];
		suffixes [j] = 0;
		k = stricmp (suffixes+i, p);
		suffixes [j] = c;
		if (!k)	return TRUE;
		if (!c) break;
		i = j+1;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------- */

long	prepare_cmd_line (char * s, char * d, char * exename)
{
	int i,j,q;
	char b [STRLEN];
	char * filepart;

	for (i = 0; s[i] && isspace(s[i]); i++);

	q = s[i]=='"';

	if (q) {
		i++;
		for (j = 0; s[i] && s[i] != '"'; i++,j++)
			d[j] = s[i];
	}
	else {
		for (j = 0; s [i] && !isspace (s [i]); i++,j++)
			d [j] = s [i];
	}
	d[j] = 0;
	if (!SearchPath (NULL, d, ".exe", sizeof (b), b, &filepart))
		return GetLastError ();

	if (exename) strcpy (exename, b);
	if (q) {
		strcpy(d,"\"");
		strcat(d,b);
	}
	else
		strcpy(d,b);

	strcat (d, s+i);
	return 0;
}

BOOL	long_filenames (char * name)
{
	BOOL b;
	DWORD len, flags;
	char d [STRLEN];
	strcpy (d, name);
	vol_only (d);
	b = GetVolumeInformation (d, 0, 0, 0, &len, &flags, 0, 0);
	if (!b) return FALSE;
	return len >= 128;
}

char all_filter[] = "All files (*.*)\0*.*\0";

BOOL	Browse_file_name (BOOL save, char * title, char * startname,
			  char * filter, char * result, int * filterpos, char * startdir)
{
	OPENFILENAME of;
	BOOL b;
	char SaveDir [STRLEN];
	char dir [STRLEN];

	lock_help ++;
	GetCurrentDirectory (sizeof (SaveDir), SaveDir);
	if (startdir)
		SetCurrentDirectory (startdir);
	strcpy (dir, startname);
	dir_only (dir);
	if (dir [0])
		SetCurrentDirectory (startdir);
	if (!filter) filter = all_filter;
	if (result != startname) strcpy (result, startname);
	memset (&of, 0, sizeof (OPENFILENAME));
	of.lStructSize = sizeof (OPENFILENAME);
	of.hwndOwner = GetActiveWindow ();
	of.lpstrFilter = filter;
	of.nFilterIndex = * filterpos;
	of.lpstrFile = result;
	of.nMaxFile = _MAX_PATH;
	of.lpstrTitle = title;
	of.Flags = OFN_HIDEREADONLY;
	if (save)
		b = GetSaveFileName (&of);
	else
		b = GetOpenFileName (&of);
	*filterpos = of.nFilterIndex;
	if (startdir)
		GetCurrentDirectory (STRLEN, startdir);
	SetCurrentDirectory (SaveDir);
	lock_help --;
	return b;
}

INT APIENTRY BrowseDirectoryHook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_PAINT)
		SetDlgItemText (hwnd, edt1, "jopa");
	return 0;
}

BOOL	Browse_for_directory (char * title, char * startdir, char * result)
{
	OPENFILENAME of;
	BOOL b;
	char buf [STRLEN];
	DWORD err;

	buf[0] = 0;
	memset (&of, 0, sizeof (OPENFILENAME));
	of.lStructSize = sizeof (OPENFILENAME);
	of.hInstance = MyInstance;
	of.hwndOwner = GetActiveWindow ();
	of.lpstrFilter = all_filter;
	of.lpstrDefExt = "";
	of.nFilterIndex = 0;
	of.lpstrFile = buf;
	of.nMaxFile = STRLEN;
	of.lpstrInitialDir = startdir;
	of.lpstrTitle = title;
	of.Flags = OFN_HIDEREADONLY|OFN_ENABLEHOOK|OFN_ENABLETEMPLATE|OFN_NOCHANGEDIR|OFN_LONGNAMES;
	of.lpTemplateName = MAKEINTRESOURCE (DIRECTORY_BROWSE_DIALOG);
	of.lpfnHook = BrowseDirectoryHook;
	b = GetOpenFileName (&of);
	err = CommDlgExtendedError ();
	if (!b) return FALSE;
	dir_only (buf);
	strcpy (result, buf);
	return TRUE;
}

BOOL	Get_file_time (char * fname, FILETIME * time)
{
	BOOL b;
	HANDLE h;
	memset (time, 0, sizeof (FILETIME));
	h = CreateFile (fname, 0, FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (!h) return FALSE;
	b = GetFileTime (h, 0, 0, time);
	CloseHandle (h);
	return b;
}
