#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "fcache.h"
#include "projred.h"
#include "parse.h"
#include "red.h"
#include "var.h"

Redir * CurRedir = NULL;
char * red_dir;

int	at_homedir (char * name)
{
	char s [STRLEN];
	strcpy (s, HomeDir);
	strcat (s, "\\");
	strcat (s, name);
	return new_filedir (s);
}

int	at_xdsdir (char * name)
{
	char s [STRLEN];

	Make_line (XDSMAIN, s, sizeof (s));
	dir_only (s);
	strcat (s, "\\");
	strcat (s, name);
	return new_filedir (s);
}

char * FindEnv (char * s)
{
	if (!strcmp (s, "!"))
		return red_dir;
	else
		return getenv (s);
}

Redir * load_red_text (TBUF * buf, int file)
{
	char fname [STRLEN];
	char s [STRLEN];
	char t [STRLEN];
	int i, j, sp, len, nlines;
	char * p;
	Redir * redir;

	strcpy (fname, filedir [file]);
	dir_only (fname);
	red_dir = fname;

	redir = NULL;
	sp = 0;
	nlines = tbuf_get_nlines (buf);
	for (i = 0; i < nlines; i ++)	{
		if (!tbuf_get_line (buf, i, &p, &len)) break;
		for (j = len-1; j >= 0 && (p [j] == ' ' || p [j] == '\t'); j--);
		memcpy (s + sp, p, j+1);
		sp += j+1;
		if (s [sp-1] == '\\')
			-- sp;
		else	{
			s [sp] = 0;
			sp = 0;
			Make_line_general (s, t, sizeof (t), FindEnv);
			Add_redir (&redir, ParseRedLine (t));
		}
	}
	return redir;
}

Redir * read_red_file (char * name, BOOL searchhome)
{
	char fname [STRLEN];
	TBUF * buf;
	Redir * r;
	int file;

	strcpy (fname, ProjDir);
	if (ProjDir [0])
		strcat (fname, "\\");
	strcat (fname, name);
	file = new_filedir (fname);
	if (!Test_file_exists (filedir [file]))	{
		file = at_xdsdir (name);
		if (!Test_file_exists (filedir [file]))	{
			file = at_homedir (name);
			if (!Test_file_exists (filedir [file]))
				return NULL;
		}
	}
	
	buf = Find_or_read_file (file);
	if (! buf)
		return NULL;
	r = load_red_text (buf, file);
	tbuf_release (buf);
	return r;
}

int	find_xc_cfg (Redir * red)
{
	char s [STRLEN];
	int file;

	Search_with_red (CfgFileName, s, red);
	file = new_filedir (s);
	if (!Test_file_exists (filedir [file]))	{
		file = at_xdsdir (CfgFileName);
		if (! Test_file_exists (filedir [file]))
			return -1;
	}
	return file;
}

ParsedProject ParsedCfg, ParsedProj;

char * FindOptions (char * name)
{
	char * s;
	if (!strcmp (name, "!"))
		return red_dir;
	if (ParsedProj && (s = Get_equation (ParsedProj, name)))
		return s;
	if (ParsedCfg && (s = Get_equation (ParsedCfg, name)))
		return s;
	return getenv (name);
}

Redir * ProjRedir;

void	addredir (char * s)
{
	char t [STRLEN];
	Make_line_general (s, t, sizeof (t), FindOptions);
	Add_redir (&ProjRedir, ParseRedLine (t));
}

void	create_redirection (void)
{
	int file;
	int errcnt;
	char fname [STRLEN];

	CurRedir = read_red_file (RedFileName, FALSE);
	ParsedProj = ParsedCfg = NULL;
	file = find_xc_cfg (CurRedir);
	if (file > 0)	{
		ParsedCfg = Parse_project (file, FALSE, &errcnt);
		if (ParsedCfg)	{
			ProjRedir = NULL;
			strcpy (fname, filedir [file]);
			dir_only (fname);
			red_dir = fname;
			Enum_lookups (ParsedCfg, addredir);
			Add_redir (&ProjRedir, CurRedir);
			CurRedir = ProjRedir;
		}
	}
	if (ProjFile > 0)	{
		ParsedProj = Parse_project (ProjFile, FALSE, &errcnt);
		if (ParsedProj)	{
			ProjRedir = NULL;
			red_dir = ProjDir;
			Enum_lookups (ParsedProj, addredir);
			Add_redir (&ProjRedir, CurRedir);
			CurRedir = ProjRedir;
		}
	}
	Clear_project_parsing (ParsedCfg);
	Clear_project_parsing (ParsedProj);
	Add_redir (&CurRedir, read_red_file (XRedFileName, TRUE));
}

int	Redirect_find_cfg_file (void)
{
	int file;

	Redir * redir = read_red_file (RedFileName, FALSE);
	file = find_xc_cfg (redir);
	FreeRedir (redir);
	return file;
}

int	Redirect_search_file (char * name, char * res)
{
	if (is_full_dir (name))	{
		strcpy (res, name);
		return 0;
	}
	create_redirection ();
	Search_with_red (name, res, CurRedir);
	FreeRedir (CurRedir);
	CurRedir = NULL;
	return 0;
}

int	Redirect_create_file (char * name, char * res)
{
	if (is_full_dir (name))	{
		strcpy (res, name);
		return 0;
	}
	create_redirection ();
	Create_with_red (name, res, CurRedir);
	FreeRedir (CurRedir);
	CurRedir = NULL;
	return 0;
}
