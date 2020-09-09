#include "util.h"
#include "lang.h"
#include "resource.h"
#include <stdio.h>

#define is_alpha(c) (c>='A' && c<='Z' || c>='a' && c<='z' || c == '_')
#define is_digit(c) (c>='0' && c <= '9')
#define is_uphex(c) (c >= 'A' && c <= 'F')
#define is_lohex(c) (c >= 'a' && c <= 'f')
#define is_hex(c) (is_digit(c) || is_uphex (c) || is_lohex (c))
#define is_normal(c) normal_syms[(unsigned char)c]
#define hexdig(c) (is_digit(c)?(c-'0') : is_uphex(c)?(c-'A'+10) : is_lohex(c)?(c-'a'+10) : 0)

#define NO_COMMENT 0
#define COMMENT 1
#define JAVADOC 2

#define LEVEL_BITS 0x7FFFFFFF
#define LEVEL(x) ((x)&LEVEL_BITS)
#define SPECIAL_BIT 0x80000000

char normal_syms [256];

typedef struct	{
		char * s;
		int len;
	}
		KWD;

typedef struct	{
	char	kwd_file_name [2000];
	char *	kwd_buf;
	int	kwd_counts [256];
	KWD *	kwd_index [256];
	KWD *	kwds;
	BOOL	kwd_table_loaded;
	}
		JAVA_ENV_BLOCK;

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

BOOL	load_kwd_table (HINSTANCE h, JAVA_ENV_BLOCK * env)
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

void	read_env (HINSTANCE h, void * env, char * name, char * IniFile)
{
	JAVA_ENV_BLOCK * e = (JAVA_ENV_BLOCK *) env;
	memset (e, 0, sizeof (JAVA_ENV_BLOCK));

	GetPrivateProfileString (name, "keywords", "java.kwd", e->kwd_file_name, sizeof (e->kwd_file_name), IniFile);
}

void	write_env (JAVA_ENV_BLOCK * e, char * name, char * IniFile)
{
	WritePrivateProfileString (name, "keywords", e->kwd_file_name, IniFile);
}

void	CALLBACK load_env (HINSTANCE h, void * env, char * name, char * IniFile)
{
	JAVA_ENV_BLOCK * e = (JAVA_ENV_BLOCK *) env;
	read_env (h, env, name, IniFile);
	e->kwd_table_loaded = load_kwd_table (h, e);
}

void	free_env (void * env)
{
	JAVA_ENV_BLOCK * e = (JAVA_ENV_BLOCK *) env;
	if (!e) return;
	if (e->kwd_table_loaded)	{
		free (e->kwds);
		free (e->kwd_buf);
	}
}

int	skip_string (char * p, int i, int len)
{
	char c = p [i++];
	while (i < len) {
		if (*p == c)
			return i+1;
		else if (*p == '\\')
			i += 2;
		else
			i ++;
	}
	return i;
}

int	contains_specials (char * p, int len)
{
	int i = 0;

	for (;;) {
		while (i < len-5 && p [i] != '\\') ++ i;
		if (i >= len-5) return 0;
		++i;
		if (p[i] == 'u') return 1;
	}
}

int get_sym (unsigned char * p, int len, int * i)
{
	int res, j, cnt;
	
	if (*i >= len) return -1;
	if (p [*i] != '\\') return p [(*i)++];
	cnt = 0;
	for (j = *i; j >= 0 && p [j] == '\\'; j--) ++cnt;
	++*i;
	if (! (cnt&1)) return '\\';
	if (*i >= len-5 || p [*i] != 'u') return '\\';
	while (*i < len-5 && p [*i] == 'u') ++*i;
	if (! is_hex (p [*i]) || ! is_hex (p [*i+1]) || ! is_hex (p [*i+2]) || ! is_hex (p [*i+3]))
		return '\\';
	res = hexdig (p [*i]);
	res = hexdig (p [*i+1]) + (res << 4);
	res = hexdig (p [*i+2]) + (res << 4);
	res = hexdig (p [*i+3]) + (res << 4);
	(*i) += 4;
	return res;
}

int	skip_string_special (char * p, int i, int len)
{
	int c;
	do {
		c = get_sym (p, len, &i);
		if (c < 0) return i;
		if (c == '\\') ++i;
	} while (c != '"');
	return i;
}

int	calc_comment_level_special (int level, char * p, int len)
{
	int i, i0, c;

	i = 0;
	for (;;) {
		if (level) {
			do {
				c = get_sym (p, len, &i);
				if (c < 0) return level;
			} while (c != '*');
			do {
				c = get_sym (p, len, &i);
				if (c < 0) return level;
			} while (c == '*');
			if (c == '/')
				level = 0;
		} else	{
			do {
				c = get_sym (p, len, &i);
				if (c < 0) return level;
			} while (c != '/' && p [i] != '"');
			if (c =='"') {
				i = skip_string_special (p, i, len);
				continue;
			}
			c = get_sym (p, len, &i);
			if (c < 0) return level;
			if (c == '"') {
				i = skip_string_special (p, i, len);
				continue;
			}
			if (c == '/')
				return 0;
			if (c != '*')
				continue;
			c = get_sym (p, len, &i);
			if (c < 0)
				return COMMENT;
			if (c == '*') {
				i0 = i;
				c = get_sym (p, len, &i);
				if (c < 0)
					return JAVADOC;
				if (c == '/')
					continue;
				i = i0;
				level = JAVADOC;
			} else
				level = COMMENT;
		}
	}		
	return level;
}

int	calc_comment_level (int level, char * p, int len)
{
	int i;

	i = 0;
	while (i < len-1)	{
		if (level) {
			while (i < len-1 && ! (p [i] == '*' && p[i+1] == '/'))
				++i;
			if (i == len-1) return level;
			level = 0;
			i += 2;
		} else	{
			while (i < len-1 && p [i] != '/' && p [i] != '"' && p [i] != '\'')
				++i;
			if (i == len-1) return level;
			if (p [i] =='"' || p [i] == '\'') {
				i = skip_string (p, i, len);
				continue;
			}

			++i;
			if (p [i] == '*') {
				++i;
				if (i < len && p [i] == '*' && ! (i < len-1 && p[i+1] == '/')) {
					level = JAVADOC;
					++i;
				} else
					level = COMMENT;
			} else if (p [i] == '/')
				return 0;
		}
	}		
	return level;
}

void	CALLBACK comment_counter (void * env, PARSER_BLOCK * b, int start, int fin)
{
	int y, len, level, old_level;
	char * p;
	JAVA_ENV_BLOCK * e = (JAVA_ENV_BLOCK*) env;

	level = start ? LEVEL (b->data [start-1]) : 0;
	for (y = start; y <= fin; y++)	{
		if (!b->line_supply (b->buf, y, &p, &len))
			return;
		if (contains_specials (p, len)) {
			level = b->data [y] = calc_comment_level_special (level, p, len);
			b->data [y] = level | SPECIAL_BIT;
		} else
			level = b->data [y] = calc_comment_level (level, p, len);
	}
	for (y = fin+1; y < b->nlines && b->line_supply (b->buf, y, &p, &len); ++y)	{
		old_level = LEVEL (b->data [y]);
		b->data [y] = level = calc_comment_level (LEVEL (b->data [y-1]), p, len);
		if (level == old_level)
			break;
		b->data [y] = (b -> data [y] & ~LEVEL_BITS) | level;
	}
	b->redraw_lines (b->buf, start, b->nlines-1);
}

#define ELEM_NORMAL 0
#define ELEM_KEYWORD 1
#define ELEM_IDENT 2
#define ELEM_NUMBER 3
#define ELEM_STRING 4
#define ELEM_COMMENT 5
#define ELEM_JAVADOC 6
#define ELEM_INVALID 7

#define JAVA_NELEMS 7

char * keys [] = {
			"keyword",
			"ident",
			"number",
			"string",
			"comment",
			"javadoc",
			"error"
		};

char * elements [] = {
			"Keywords",
			"Identifiers",
			"Numeric constants",
			"String literals",
			"Comments",
			"Javadoc comments",
			"Invalid sequences"
		};

BOOL	test_keyword (JAVA_ENV_BLOCK * e, char * s, int len)
{
	int i, c;

	if (!e->kwd_table_loaded) return FALSE;
	c = (unsigned char) s[0];
	for (i = 0; i < e->kwd_counts [c]; i++)
		if (e->kwd_index [c][i].len == len && !strncmp (s, e->kwd_index [c][i].s, len))
			return TRUE;
	return FALSE;
}

int	parse_word (JAVA_ENV_BLOCK * e, char * s, char * d, int len, int displen, int i)
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

char filler;

int	scan_number (char * s, int len)
{
	int i = 0;
	
	if (s[0] == '0') {
		if (len >= 3 && (s[1] == 'x' || s[1] == 'X')) {
			i += 2;
			while (i < len && is_hex (s [i])) ++ i;
			if (i == 2) {
				filler = ELEM_INVALID;
				return 1;
			}
			if (i < len && is_alpha (s [i]))
				filler = ELEM_INVALID;
			return i;
		}
		if (len >= 2 && isdigit (s [1])) {
			i += 2;
			while (i < len && isdigit (s [i])) ++ i;
			if (!only_octals (s, i))
				filler = ELEM_INVALID;
			else if (i < len && is_alpha (s [i]))
				filler = ELEM_INVALID;
			return i;
		}
	}

	for (; i < len; i++)
		if (!is_digit (s[i])) break;

	if (i >= len)
		return i;
	if (s [i] == '.') {
		++i;
		while (is_digit (s[i])) ++i;
	}
	if (i >= len)
		return i;
	if (! is_alpha (s [i]))
		return i;
	if (s [i] != 'e' && s [i] != 'E' && s [i] != 'd' && s [i] != 'D') {
		filler = ELEM_INVALID;
		return i;
	}
	++i;
	if (i >= len) {
		filler = ELEM_INVALID;
		return i;
	}
	if (s[i]=='+' || s[i]=='-')
		++i;
	if (i >= len) {
		filler = ELEM_INVALID;
		return i;
	}
	if (! is_digit (s[i])) {
		filler = ELEM_INVALID;
		return i;
	}
	while (i < len && is_digit (i)) ++ i;
	return i;
}

int	parse_number (char * s, char * d, int displen, int i)
{
	int j;
	
	if (i >= displen) return i;
	filler = ELEM_NUMBER;
	j = scan_number (s+i, displen-i);
	memset (d+i, filler, j);
	return i+j;
}

int	skip_comment (JAVA_ENV_BLOCK * e, char * s, char * d, int len, int displen, int level)
{
	int i = 0;
	char elem = level == COMMENT ? ELEM_COMMENT : ELEM_JAVADOC;

	while (i<displen)
		if (s[i] == '*' && i < len-1 && s[i+1] == '/')	{
			d[i++] = elem;
			d[i++] = elem;
			return i;
		} else
			d[i++] = elem;
	return i;
}

int	parse_comment (char * s, char * d, int displen, int i)
{
	char elem;
	
	// s [i] == '/'; s [i+1] == '*'.

	if (i < displen-2 && s [i+2] == '*' && !(i < displen-3 && s [i+3]=='/'))	{
		elem = ELEM_JAVADOC;
		d[i++] = elem;
		d[i++] = elem;
		d[i++] = elem;
	} else {
		elem = ELEM_COMMENT;
		d [i++] = elem;
		if (i < displen) d[i++] = elem;
	}
	while (i<displen)
		if (s[i] == '*' && i<displen-1 && s[i+1] == '/')	{
			d[i++] = elem;
			d[i++] = elem;
			return i;
		} else
			d[i++] = elem;
	return i;
}

int	parse_string (char * s, char * d, int len, int displen, int i)
{
	char c = s[i];
	int i0 = i++;
	while (i<len && s[i]!=c)
		if (s [i] == '\\')
			i += 2;
		else
			++i;
	c = i<len ? ELEM_STRING : ELEM_INVALID;
	if (i<len) ++i;
	if (i > displen) i = displen;
	memset (d+i0, c, i-i0);
	return i;
}

int	CALLBACK normalized_parser (void * env, char * s, char * d, int len, int displen, DWORD data)
{
	int i;

	JAVA_ENV_BLOCK * e = (JAVA_ENV_BLOCK *) env;

	if (LEVEL (data))
		i = skip_comment (e, s, d, len, displen, LEVEL (data));
	else
		i = 0;

	while (i < displen)	{
		if (is_alpha(s[i]))
			i = parse_word (e, s, d, len, displen, i);
		else if (is_digit (s[i]))
			i = parse_number (s, d, displen, i);
		else if (s[i] == '\'' || s[i] == '"')
			i = parse_string (s, d, len, displen, i);
		else if (s[i] == '/' && i < len-1 && s[i+1] == '*')
			i = parse_comment (s, d, displen, i);
		else if (s[i] == '/' && i < len-1 && s[i+1] == '/')
			while (i < displen)
				d[i++] = ELEM_COMMENT;
		else if (is_normal (s[i]))
			d[i++] = ELEM_NORMAL;
		else
			d[i++] = ELEM_INVALID;
	}
	return i;
}

int	decode (char * s, char * d, int len, int buflen)
{
	int i = 0;
	int j = 0;
	int c;
	while (j < buflen && i < len) {
		c = get_sym (s, len, &i);
		if (c < 0) break;
		if (c >= 0x100) c = 0xFF;
		d [j++] = (char) c;
	}
	return j;
}

int	CALLBACK parser (void * env, char * s, char * d, int len, int displen, DWORD data0, DWORD data)
{
	int i, normlen, parslen, dlen, j, j0, c;
	char srcbuf [2000];
	char dstbuf [2000];

	if (! (data & SPECIAL_BIT))
		return normalized_parser (env, s, d, len, displen, data0);

	normlen = decode (s, srcbuf, len, sizeof (srcbuf));
	dlen = min (displen, normlen);
	parslen = normalized_parser (env, srcbuf, dstbuf, normlen, dlen, data0);
	i = 0;
	j = 0;
	while (i < parslen && j < displen) {
		j0 = j;
		c = get_sym (s, len, &j);
		if (c < 0) break;
		while (j0 < j && j0 < displen) d [j0++] = dstbuf [i];
		i++;
	}
	return j;
}

#define ALL_NORMAL_SYMS "!&()*+,-./:;<=>?[]^{|}~ \t"

void init_normal_syms (void)
{
	char * s;
	memset (normal_syms, 0, sizeof (normal_syms));
	for (s = ALL_NORMAL_SYMS; *s; s++)
		normal_syms [(unsigned char)*s] = 1;
}

char sample_text [] =
	"/** This class represents a point on a plane\n"
	" */\n"
	"public class Point\n"
	"{\n"
	"    /* these variables are not accessible from outside */\n"
	"    private int x, y;\n"
	"\n"
	"    public Point (int x, int y)\n"
	"    {\n"
	"        this.x = x;\n"
	"        this.y = y;\n"
	"    }\n"
        "\n"
	"    public String toString ()\n"
	"    {\n"
	"        return \"(\" + x + \", \" + y + \")\";\n"
	"    }\n"
	"}\n"
	;

char * CALLBACK get_sample_text (HINSTANCE h, void * env)
{
	return sample_text;
}

/* ---------------------------------------------------------------- */

HINSTANCE MyInstance;

JAVA_ENV_BLOCK cfg_env;

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
		return 1;
	case WM_COMMAND:
		switch (CONTROL_ID)	{
		case IDOK:
			GetDlgItemText (hwnd, IDC_FILENAME, cfg_env.kwd_file_name, sizeof (cfg_env.kwd_file_name));
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
	read_env (h, &cfg_env, name, IniFile);
	if (DialogBox (h, MAKEINTRESOURCE (IDD_CONFIG), parent, ConfigureDialogProc))
		write_env (&cfg_env, name, IniFile);
}

/* ---------------------------------------------------------------- */

LANG_BLOCK lang_block =
	 {
		sizeof (LANG_BLOCK),
		1,
		JAVA_NELEMS,
		sizeof (JAVA_ENV_BLOCK),
		keys,
		elements,
		comment_counter,
		parser,
		load_env,
		NULL,
		configure,
		get_sample_text
	};

LANG_BLOCK * CALLBACK LangInit (void)
{
	init_normal_syms ();
	return &lang_block;
}

BOOL WINAPI DllMain (HINSTANCE hinstance, DWORD Reason, LPVOID Reserved)
{
    return TRUE;
}
