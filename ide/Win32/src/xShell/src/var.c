#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "var.h"
#include "tbuf.h"
#include "fcache.h"
#include <string.h>

/* ---------------------- Shell variables processing ------------------- */

/* following vars are now defined:
   xdsmain  - main Compiler Executable
   xdsname  - its name without .exe
   xdsdir   - its directory
   file     - full file name in current edit window
   filedir  - directory part of file
   filename - name part of file
   fileext  - extension part of file
   project  - full project file name
   projdir  - project directory
   projname - project name without path and extension
   projext  - project file extension
   projini  - full name of ini file corresponding out proj file
   homedir  - directory where xds shell resides
   startdir - directory from where it was started (current dir at startup)
   inifile  - shell ini file name
   argumenmts - command line arguments for Run
   exefile    - full name of exe file to run
   rundir     - dirrectory to be set as current when running user exe file
*/


#define NNAMES 100
#define NVALLEN 1000

int    nnames;
char * names [NNAMES];
char   nvalues [NNAMES][NVALLEN];

int	find_name (char * name)
{
	int i;
	for (i = 0; i < nnames; i ++)
		if (! stricmp (names [i], name)) return i;
	return -1;
}

char * find_name_val (char * name)
{
	int i = find_name (name);
	return i >= 0 ? nvalues [i] : NULL;
}

void	Set_name (char * name, char * value)
{
	int i = find_name (name);
	if (i<0)	{
		if (nnames == NNAMES) return;
		i = nnames ++;
		names [i] = name;
	}
	strncpy (nvalues [i], value, NVALLEN);
	nvalues [i][NVALLEN-1]=0;
}

int	expand_line_general (char * s, char * d, int len, char * (* Finder) (char * name))
{
	int i;
	int cnt = 0;
	char * p, * v;
	char * x = NULL;
	int xlen = 0;
	for (i = 0; *s && i < len-1;)
		if (*s != '$')
			d[i++]=*s++;
		else if (s [1] == '!')	{
			v = Finder ("!");
			if (v)	{
				while (*v && i < len-1)
					d [i++] = *v++;
				s += 2;
			} else
				d [i++] = *s++;
		} else if (s[1]!='(')
			d [i++] = *s++;
		else	{
			p = s+2;
			for (s++; *s && !isspace (*s) && *s != ')'; ++ s);
			if (*s == ')')	{
				cnt ++;
				if (xlen < s-p+1)	{
					xlen = s-p+1;
					x = realloc (x, xlen);
				}
				memcpy (x, p, s-p);
				x [s-p] = 0;
				v = Finder (x);
				s++;
				if (v)
					while (*v && i < len-1)
						d [i++] = *v++;
				else
					for (p -= 2; p < s && i < len-1;)
						d [i++] = *p++;
			} else	{
				d[i++] = '$';
				s = p-1;
			}
		}
	d [i] = 0;
	if (x)	{
		free (x);
	}
	return cnt;
}

int	expand_line (char * s, char * d, int len)
{
	return expand_line_general (s, d, len, find_name_val);
}

void	Make_line_general (char * s, char * d, int len, char * (* Finder) (char * name))
{
	char buf1 [3000], buf2 [3000];
	char * q = buf1;
	int i, cnt;

	for (i = 0; i < 5; i++)	{
		cnt = expand_line_general (s, q, 3000, Finder);
		if (q == buf2)	{
			q = buf1;
			s = buf2;
		} else	{
			q = buf2;
			s = buf1;
		}
		if (!cnt) break;
	}
	expand_line (s, d, len);
}

void	Make_line (char * s, char * d, int len)
{
	Make_line_general (s, d, len, find_name_val);
}

/* provided neither file is open in IDE */

BOOL	Copy_and_expand_file (char * src, char * dst)
{
	char s [2048], d [2048];
	char * p;
	int i, len;
	TBUF * b;

	b = Find_or_read_file (new_filedir (src));
	if (!b) return FALSE;
	if (! Put_file_start ())	{
		tbuf_release (b);
		return FALSE;
	}

	for (i = 0; tbuf_get_line (b, i, &p, &len); i++)	{
		memcpy (s, p, len);
		s [len] = 0;
		Make_line (s, d, sizeof (d)-1);
		if (!Put_file (d, strlen (d)))	{
			tbuf_release (b);
			return FALSE;
		}
	}
	tbuf_release (b);
	return Put_file_fin (new_filedir (dst), FALSE);
}

void	File_set_vars (char * s)
{
	char d [2000];

	Set_name ("file", s);
	if (!s [0])	{
		Set_name ("filename", "");
		Set_name ("fileext",  "");
		Set_name ("filedir",  "");
		Set_name ("filewholename", "");
	} else	{
		Set_name ("filewholename", name_only (s));
		change_ext (name_only (s), d, 0);
		Set_name ("filename", d);
		Set_name ("fileext", ext_only (s));
		strcpy (d, s);
		dir_only (d);
		Set_name ("filedir", d);
	}
}
