#include "util.h"
#include "lang.h"
#include "resource.h"
#include <stdio.h>

#define is_alpha(c) (c>='A' && c<='Z' || c>='a' && c<='z' || c == '_')
#define is_digit(c) (c>='0' && c <= '9')
#define is_hex(c) (is_digit(c) || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f')
#define is_normal(c) normal_syms[(unsigned char)c]

#define LEVEL_BITS  0x7FFFFFFF
#define TROUBLE_BIT 0x80000000
#define LEVEL(x)   (int)((x) & LEVEL_BITS)
#define TROUBLE(x) ((x) & TROUBLE_BIT)

char normal_syms [256];

typedef struct	{
		char * s;
		int len;
	}
		KWD;

typedef struct	{
	BOOL	ext_comments;
	char	kwd_file_name [2000];
	char *	kwd_buf;
	int	kwd_counts [256];
	KWD *	kwd_index [256];
	KWD *	kwds;
	BOOL	kwd_table_loaded;
	}
		M2_ENV_BLOCK;

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
	if (res != dir) strcpy (res, dir);
	dir_only (res);
	strcat (res, "\\");
	if (file [0] && file [1] == ':') file += 2;
	strcat (res, file);
}

BOOL	load_kwd_table (HINSTANCE h, M2_ENV_BLOCK * env)
{
	int i, j, c, kwd_count;
	long len;
	char dir [2000], name [3000];
	FILE * f;

	GetModuleFileName (h, dir, sizeof (dir));
	add_dir (dir, env->kwd_file_name, name);
	
	f = fopen (name, "rb");
	if (!f) return FALSE;
	if (fseek (f, 0, SEEK_END))	{
		fclose (f);
		return FALSE;
	}
	len = ftell (f);
	fseek (f, 0, SEEK_SET);
	env->kwd_buf = malloc (len+2);
	if (!env->kwd_buf)	{
		fclose (f);
		return FALSE;
	}
	i = (int) fread (env->kwd_buf+1, 1, len, f);
	if (i < len)	{
		fclose (f);
		free (env->kwd_buf);
		return FALSE;
	}
	fclose (f);
	env->kwd_buf [len] = env->kwd_buf [0] = 0;
	len += 2;
	for (i = 0; i < len; i++)
		if (env->kwd_buf [i] <= ' ') env->kwd_buf [i] = 0;
	
	memset (env->kwd_counts, 0, sizeof (env->kwd_counts));
	memset (env->kwd_index,  0, sizeof (env->kwd_index));
	kwd_count = 0;
	for (i = 0; i < len-1; i++)
		if (!env->kwd_buf [i] && env->kwd_buf [i+1])	{
			++ env->kwd_counts [(unsigned char) env->kwd_buf [i+1]];
			kwd_count ++;
		}
	env->kwds = malloc (kwd_count * sizeof (KWD));
	if (!env->kwds)	{
		free (env->kwd_buf);
		return FALSE;
	}
	memset (env->kwds, 0, kwd_count * sizeof (KWD));
	j = 0;
	for (i = 0; i < 256; i++)	{
		env->kwd_index [i] = env->kwd_counts [i] ? env->kwds + j : NULL;
		j += env->kwd_counts [i];
		env->kwd_counts [i] = 0;
	}
	for (i = 0; i < len-1; i++)
		if (!env->kwd_buf [i] && env->kwd_buf [i+1])	{
			c = (unsigned char) env->kwd_buf [i+1];
			env->kwd_index [c] [env->kwd_counts [c]].s = env->kwd_buf+i+1;
			env->kwd_index [c] [env->kwd_counts [c]].len = strlen (env->kwd_buf+i+1);
			++ env->kwd_counts [c];
		}
	return TRUE;
}

void	m2_read_env (HINSTANCE h, void * env, char * name, char * IniFile)
{
	char s [100];
	M2_ENV_BLOCK * e = (M2_ENV_BLOCK *) env;
	memset (e, 0, sizeof (M2_ENV_BLOCK));

	GetPrivateProfileString (name, "keywords", "m2.kwd", e->kwd_file_name, sizeof (e->kwd_file_name), IniFile);
	GetPrivateProfileString (name, "ada-comments", "on", s, sizeof (s), IniFile);
	e->ext_comments = !stricmp (s, "on");
}

void	m2_write_env (M2_ENV_BLOCK * e, char * name, char * IniFile)
{
	WritePrivateProfileString (name, "keywords", e->kwd_file_name, IniFile);
	WritePrivateProfileString (name, "ada-comments", e->ext_comments? "on" : "off", IniFile);
}

void	CALLBACK m2_load_env (HINSTANCE h, void * env, char * name, char * IniFile)
{
	M2_ENV_BLOCK * e = (M2_ENV_BLOCK *) env;
	m2_read_env (h, env, name, IniFile);
	e->kwd_table_loaded = load_kwd_table (h, e);
}

void	m2_free_env (void * env)
{
	M2_ENV_BLOCK * e = (M2_ENV_BLOCK *) env;
	if (!e) return;
	if (e->kwd_table_loaded)	{
		free (e->kwds);
		free (e->kwd_buf);
	}
}

int	calc_comment_level (BOOL ext_comments, int level, char * p, int len, BOOL * was_negative)
{
	int i;

	* was_negative = 0;
	for (i = 0; i < len-1; i++)
		if (p[i]=='(' && p[i+1]=='*')	{
			++ level;
			i++;
		} else if (p[i]=='*' && p[i+1]==')')	{
			i++;
			if (level > 0)
				--level;
			else
				* was_negative = 1;
		} else if (ext_comments && p[i]=='-' && p[i+1]=='-')	{
			if (!level) return 0;
			i++;
		}
	return level;
}

void	CALLBACK m2_comment_counter (void * env, PARSER_BLOCK * b, int start, int fin)
{
	int y, len, level, old_level, delta;
	char * p;
	BOOL was_neg;
	M2_ENV_BLOCK * e = (M2_ENV_BLOCK*) env;

	level = start ? LEVEL (b->data [start-1]) : 0;
	for (y = start; y <= fin; y++)	{
		if (!b->line_supply (b->buf, y, &p, &len))
			return;
		level = b->data [y] = calc_comment_level (e->ext_comments, level, p, len, &was_neg);
		if (was_neg)
			b->data [y] |= TROUBLE_BIT;
	}
	if (fin < b->nlines-1)	{
		y = fin + 1;
		while (y < b->nlines && b->line_supply (b->buf, y, &p, &len))	{
			old_level = LEVEL (b->data [y]);
			b->data [y] = level = calc_comment_level (e->ext_comments, LEVEL (b->data [y-1]), p, len, &was_neg);
			if (was_neg)
				b->data [y] |= TROUBLE_BIT;
			if (level == old_level)
				break;
			delta = level - old_level;
			for (y = y+1; y < b->nlines; y++)
				if (TROUBLE (b->data [y]) || LEVEL (b->data [y]) + delta < 0)
					break;
				else
					b->data [y] += delta;
		}
		b->redraw_lines (b->buf, start, b->nlines-1);
	}
}

#define ELEM_NORMAL 0
#define ELEM_KEYWORD 1
#define ELEM_IDENT 2
#define ELEM_NUMBER 3
#define ELEM_STRING 4
#define ELEM_COMMENT 5
#define ELEM_INVALID 6

#define M2_NELEMS 6

char * m2_keys [] = {
			"keyword",
			"ident",
			"number",
			"string",
			"comment",
			"error"
		};

char * m2_elements [] = {
			"Keywords",
			"Identifiers",
			"Numeric constants",
			"String literals",
			"Comments",
			"Invalid sequences"
		};

BOOL	test_keyword (M2_ENV_BLOCK * e, char * s, int len)
{
	int i, c;

	if (!e->kwd_table_loaded) return FALSE;
	c = (unsigned char) s[0];
	for (i = 0; i < e->kwd_counts [c]; i++)
		if (e->kwd_index [c][i].len == len && !strncmp (s, e->kwd_index [c][i].s, len))
			return TRUE;
	return FALSE;
}

int	parse_word (M2_ENV_BLOCK * e, char * s, char * d, int len, int displen, int i)
{
	int i0;
	char c;

	i0 = i ++;
	while (i<len && (is_alpha (s[i]) || is_digit (s[i]))) ++i;
	if (test_keyword (e, s+i0, i-i0))
		c = ELEM_KEYWORD;
	else
		c = ELEM_IDENT;
	while (i0 < i && i0 < displen)
		d [i0++] = c;
	return i;
}

BOOL	only_octals (char * s, int n)
{
	while (n--)	{
		if (*s < '0' || *s >= '8')
			return FALSE;
		++s;
	}
	return TRUE;
}

BOOL	only_hex (char * s, int n)
{
	while (n--)	{
		if (! is_hex (*s))
			return FALSE;
		++s;
	}
	return TRUE;
}

int	parse_number (char * s, char * d, int displen, int i)
{
	int i0;
	char c;
	BOOL were_letters;
	
	were_letters = FALSE;
	i0 = i;
	for (; i < displen; i++)
		if (is_digit (s[i])) continue;
		else if (is_alpha (s[i]))
			were_letters = TRUE;
		else
			break;

	if (were_letters)	{
		c = ELEM_INVALID;
		if (s[i-1]=='C')	{
			if (only_octals (s+i0, i-i0-1))
				c = ELEM_STRING;
		} else if (s[i-1]=='B')	{
			if (only_octals (s+i0, i-i0-1))
				c = ELEM_NUMBER;
		} else if (s[i-1]=='x' || s[i-1]=='X')	{
			if (only_hex (s+i0, i-i0-1))
				c = ELEM_STRING;
		} else if (s[i-1]=='h' || s[i-1]=='H')	{
			if (only_hex (s+i0, i-i0-1))
				c = ELEM_NUMBER;
		}
		memset (d+i0, c, i-i0);
		return i;
	}
	memset (d+i0, ELEM_NUMBER, i-i0);	
	if (i >= displen)
		return i;
	if (s[i]!='.') return i;
	if (i+1 < displen && s[i+1] == '.') return i;
	d[i++] = ELEM_NUMBER;
	while (i < displen && is_digit (s[i]))
		d[i++] = ELEM_NUMBER;
	if (i >= displen || s[i]!='E') return i;
	d[i++] = ELEM_NUMBER;
	if (i < displen && (s[i]=='+' || s[i]=='-'))
		d[i++] = ELEM_NUMBER;
	while (i < displen && is_digit (i))
		d[i++] = ELEM_NUMBER;
	return i;
}

int	skip_comment (M2_ENV_BLOCK * e, char * s, char * d, int len, int displen, int nest)
{
	int i = 0;

	while (i<displen && nest)
		if (s[i] == '(' && i < len-1 && s[i+1] == '*')	{
			d[i++] = ELEM_COMMENT;
			d[i++] = ELEM_COMMENT;
			++ nest;
		} else if (s[i] == '*' && i < len-1 && s[i+1] == ')')	{
			d[i++] = ELEM_COMMENT;
			d[i++] = ELEM_COMMENT;
			-- nest;
		} else if (s[i] == '-' && e->ext_comments && i < len-1 && s[i+1] == '-')	{
			while (i < displen) d[i++] = ELEM_COMMENT;
		} else
			d[i++] = ELEM_COMMENT;
	return i;
}

int	parse_comment (char * s, char * d, int displen, int i)
{
	int nest;
	
	d[i++] = ELEM_COMMENT;
	if (i < displen) d[i++] = ELEM_COMMENT;
	nest = 1;
	while (i<displen && nest)
		if (s[i] == '(' && i<displen-1 && s[i+1] == '*')	{
			d[i++] = ELEM_COMMENT;
			d[i++] = ELEM_COMMENT;
			++ nest;
		} else if (s[i] == '*' && i<displen-1 && s[i+1] == ')')	{
			d[i++] = ELEM_COMMENT;
			d[i++] = ELEM_COMMENT;
			-- nest;
		} else
			d[i++] = ELEM_COMMENT;
	return i;
}

int	parse_string (char * s, char * d, int len, int displen, int i)
{
	char c = s[i];
	int i0 = i++;
	while (i<len && s[i]!=c) ++i;
	c = i<len ? ELEM_STRING : ELEM_INVALID;
	if (i<len) ++i;
	if (i > displen) i = displen;
	memset (d+i0, c, i-i0);
	return i;
}

int	CALLBACK m2_parser (void * env, char * s, char * d, int len, int displen, DWORD data0, DWORD data)
{
	int i;

	M2_ENV_BLOCK * e = (M2_ENV_BLOCK *) env;
	
	i = skip_comment (e, s, d, len, displen, LEVEL (data0));

	while (i < displen)	{
		if (is_alpha(s[i]))
			i = parse_word (e, s, d, len, displen, i);
		else if (is_digit (s[i]))
			i = parse_number (s, d, displen, i);
		else if (s[i] == '\'' || s[i] == '"')
			i = parse_string (s, d, len, displen, i);
		else if (s[i] == '(' && i < len-1 && s[i+1] == '*')
			i = parse_comment (s, d, displen, i);
		else if (s[i] == '-' && e->ext_comments && i < len-1 && s[i+1] == '-')
			while (i < displen)
				d[i++] = ELEM_COMMENT;
		else if (is_normal (s[i]))
			d[i++] = ELEM_NORMAL;
		else
			d[i++] = ELEM_INVALID;
	}
	return i;
}

#define ALL_NORMAL_SYMS ":,.=;()[]{}|+-*/&<>#^@"

void init_normal_syms (void)
{
	char * s;
	memset (normal_syms, 0, sizeof (normal_syms));
	for (s = ALL_NORMAL_SYMS; *s; s++)
		normal_syms [(unsigned char)*s] = 1;
}

char m2_sample_text [] =
	"(* This function calculates a factorial *)\n"
	"PROCEDURE factorial (n:INTEGER):INTEGER;\n"
	"VAR f:INTEGER;\n"
	"BEGIN\n"
	"    f := 1;\n"
	"    FOR i:=1 TO n DO\n"
	"        f := f*i;\n"
	"    END;\n"
	"    RETURN f;\n"
	"END factorial;"
	;

char * CALLBACK get_sample_text (HINSTANCE h, void * env)
{
	return m2_sample_text;
}

/* ---------------------------------------------------------------- */

HINSTANCE MyInstance;
M2_ENV_BLOCK cfg_env;
static char kwd_file_filter[] =	"Keyword files (*.kwd)\0*.kwd\0"
				"All files (*.*)\0*.*\0";

BOOL	Browse_file_name (HWND hwnd, char * title, char * name)
{
	OPENFILENAME of;
	BOOL b;

	memset (&of, 0, sizeof (OPENFILENAME));
	of.lStructSize = sizeof (OPENFILENAME);
	of.hwndOwner = hwnd;
	of.lpstrFilter = kwd_file_filter;
	of.lpstrFile = name;
	of.nMaxFile = _MAX_PATH;
	of.lpstrTitle = title;
	of.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	b = GetOpenFileName (&of);
	return b;
}

BOOL CALLBACK ConfigureDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char s [3000];
	switch (msg)	{
	case WM_INITDIALOG:
		SetDlgItemText (hwnd, IDC_FILENAME, cfg_env.kwd_file_name);
		CheckDlgButton (hwnd, IDC_EXTCOMMENT, cfg_env.ext_comments);
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			GetDlgItemText (hwnd, IDC_FILENAME, cfg_env.kwd_file_name, sizeof (cfg_env.kwd_file_name));
			cfg_env.ext_comments = IsDlgButtonChecked (hwnd, IDC_EXTCOMMENT);
			EndDialog (hwnd, 1);
			break;
		case IDCANCEL:
			EndDialog (hwnd, 0);
			break;
		case IDC_BROWSE:
			GetDlgItemText (hwnd, IDC_FILENAME, cfg_env.kwd_file_name, sizeof (cfg_env.kwd_file_name));
			GetModuleFileName (MyInstance, s, sizeof (s));
			add_dir (s, cfg_env.kwd_file_name, s);
			if (Browse_file_name (hwnd, "Select file with keywords list", s))
				SetDlgItemText (hwnd, IDC_FILENAME, s);
			break;
		}
		return 1;
	}
	return 0;
}

void CALLBACK configure (HINSTANCE h, HWND parent, char * name, char * IniFile)
{
	MyInstance = h;
	m2_read_env (h, &cfg_env, name, IniFile);
	if (DialogBox (h, MAKEINTRESOURCE (IDD_CONFIG), parent, ConfigureDialogProc))
		m2_write_env (&cfg_env, name, IniFile);
}

/* ---------------------------------------------------------------- */

LANG_BLOCK m2_lang_block =
	 {
		sizeof (LANG_BLOCK),
		1,
		M2_NELEMS,
		sizeof (M2_ENV_BLOCK),
		m2_keys,
		m2_elements,
		m2_comment_counter,
		m2_parser,
		m2_load_env,
		NULL,
		configure,
		get_sample_text
	};

LANG_BLOCK * CALLBACK LangInit (void)
{
	init_normal_syms ();
	return &m2_lang_block;
}

BOOL WINAPI DllMain (HINSTANCE hinstance, DWORD Reason, LPVOID Reserved)
{
    return TRUE;
}
