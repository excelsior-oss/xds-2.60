#include "util.h"
#include "shell.h"
#include "parse.h"
#include "winmake.h"
#include "message.h"
#include "options.h"
#include "tbuf.h"
#include "fcache.h"

typedef enum	{
		COMMENT,
		EXCLAMATION,
		MODULE,
		PACKAGE,
		EQUATION,
		OPTION,
		BAD_LINE
	}
		LType;

typedef	struct	textline {
			LType type;
			char * l;
			char * key;
			char * value;
			int    new_define;
			struct textline * next;
		}
			TextLine;

typedef	struct _ParsedProject {
		int file;
		TextLine * line_list;
		TextLine * end_point;
		int parsed;
		int modified;
		int complex;
		FILETIME time;
	}
		* ParsedProject;

char ONVALUE  [] = "+";
char OFFVALUE [] = "-";

ParsedProject EditProj = NULL;
ParsedProject EditOpt  = NULL;

char * copyval (char * v)
{
	if (v == ONVALUE || v==OFFVALUE)
		return v;
	return alloc_copy (v);
}

void	freeval (char * v)
{
	if (v != ONVALUE && v != OFFVALUE)
		free (v);
}

void	Clear_project_parsing (ParsedProject proj)
{
	TextLine * p, * q;
	if (!proj) return;
	for (p = proj -> line_list; p;)	{
		q = p -> next;
		free (p -> key);
		freeval (p -> value);
		free (p -> l);
		free (p);
		p = q;
	}
	free (proj);
}

void	del_proj_line (ParsedProject proj, TextLine * l)
{
	TextLine **last;

	last = & proj->line_list;
	while (*last && *last != l) last = &(*last)->next;
	free (l -> l);
	free (l -> key);
	freeval (l -> value);
	if (*last == l)
		*last = l -> next;
	if (l == proj->end_point) proj->end_point = l->next;
	proj -> modified = TRUE;
}

TextLine * add_proj_line (ParsedProject proj, char * s, int len)
{
	TextLine * line, **last;

	last = & proj->line_list;
	while (*last && *last != proj->end_point) last = &(*last)->next;
	line = Malloc (sizeof (TextLine));
	if (!line)
		return NULL;
	line -> next = *last;
	line -> l = line -> key = line -> value = NULL;
	line -> type = COMMENT;
	*last = line;
	line -> l = Malloc (len+1);
	if (!line -> l)
		return NULL;
	memmove (line -> l, s, len);
	line->l [len] = 0;
	return line;
}

ParsedProject make_linelist (TBUF * buf, int file)
{
	TextLine **last;
	char * p;
	int i, len, nlines;
	ParsedProject proj;

	proj = Malloc (sizeof (struct _ParsedProject));
	if (!proj)	{
		ErrCode = ERROR_NOT_ENOUGH_MEMORY;
		return NULL;
	}

	proj -> line_list = NULL;
	proj -> end_point = NULL;
	last = & proj -> line_list;
	proj -> file = file;
	proj -> parsed = 0;
	proj -> modified = 0;
	proj -> complex = 0;

	nlines = tbuf_get_nlines (buf);
	for (i = 0; i < nlines; i++)	{
		if (! tbuf_get_line (buf, i, &p, &len))
			break;
		if (!add_proj_line (proj, p, len))	{
			Clear_project_parsing (proj);
			ErrCode = ERROR_NOT_ENOUGH_MEMORY;
			return NULL;
		}
	}
	return proj;
}

char * skip_name (char * s)
{
	while (isalpha (*s) || isdigit (*s) || *s == '_') ++ s;
	return s;
}

#define PROJ_ERR_NAME_EXPECTED 1
#define PROJ_ERR_EQU_EXPECTED 2
#define PROJ_ERR_PLUS_EXPECTED 3
#define PROJ_ERR_BAD_SYMBOL 4

char * converr (int l)
{
	switch (l)	{
	case PROJ_ERR_NAME_EXPECTED : return "Name expected";
	case PROJ_ERR_EQU_EXPECTED  : return "'=' expected";
	case PROJ_ERR_PLUS_EXPECTED : return "'+' or '-' expected";
	case PROJ_ERR_BAD_SYMBOL    : return "Bad symbol";
	}
	return "error";
}

int	parse_error_x;

int	parse_minus (TextLine * line, char * p)
{
	skip_spaces (&p);
	if (!*p) return 0;
	switch (*p)	{
	case '=':
	case '!':
		line->new_define = *p == '!';
		++p;
		skip_spaces (&p);
		line->type = EQUATION;
		line->value = alloc_copy (p);
		if (!line->value)
			return -1;
		return 0;
	case ':':
		++p;
		skip_spaces (&p);
		switch (*p)	{
		case '=':	line->new_define = TRUE;
				++p;
				skip_spaces (&p);
				line->type = EQUATION;
				line->value = alloc_copy (p);
				if (!line->value)
					return -1;
				line->type = EQUATION;
				return 0;
		case '-':
		case '+':	line->type = OPTION;
				line->value = *p == '+'? ONVALUE : OFFVALUE;
				line->new_define = TRUE;
				return 0;
		default:
				line->type = COMMENT;
				parse_error_x = p - line->l;
				return PROJ_ERR_PLUS_EXPECTED;
		}
	case '+':
	case '-':
		line->type = OPTION;
		line->value = *p == '+'? ONVALUE : OFFVALUE;
		return 0;
	default:
		line->type = COMMENT;
		parse_error_x = p - line->l;
		return PROJ_ERR_PLUS_EXPECTED;
	}
}

int	parse_line (TextLine * line)
{
	char * p, *t;
	char * s = line->l;

	skip_spaces (&s);
	line -> type = COMMENT;
	line -> new_define = 0;
	parse_error_x = 0;
	switch (*s)	{
		case '!':
			++s;
			skip_spaces (&s);
			p = skip_name (s);
			if (p-s == 6 && !strncmp (s, "module", 6))
				line -> type = MODULE;
			else if (p-s == 7 && !strncmp (s, "package", 7))	{
				line -> type = PACKAGE;
				skip_spaces (&p);
				s = p;
				while (*p && !isspace (*p)) ++ p;
				line -> key = Malloc (p-s+1);
				memcpy (line->key, s, p-s);
				line->key [p-s] = 0;
				skip_spaces (&p);
				s = p;
				while (*p && !isspace (*p)) ++ p;
				line -> value = Malloc (p-s+1);
				memcpy (line->value, s, p-s);
				line->value [p-s] = 0;
			}
			else
				line -> type = EXCLAMATION;
			break;
		case '\0':
		case '%':
			break;
		case '#':
			++s;
			skip_spaces (&s);
			p = skip_name (s);
			if (s == p)	{
				parse_error_x = s - line -> l;
				return PROJ_ERR_NAME_EXPECTED;
			}
			t = p;
			skip_spaces (&t);
			if (*t != '=' && *t != '!')	{
				parse_error_x = t - line -> l;
				return PROJ_ERR_EQU_EXPECTED;
			}
			line->new_define = *t == '!';
			++t;
			skip_spaces (&t);
			line -> key = Malloc (p-s+1);
			if (! line -> key)
				return -1;
			memmove (line -> key, s, p-s);
			line -> key [p-s] = '\0';
			line -> value = alloc_copy (t);
			if (!line -> value)
				return -1;
			line -> type = EQUATION;
			break;
		case ':':
			++s;
			skip_spaces (&s);
			p = skip_name (s);
			if (p == s)	{
				parse_error_x = p - line -> l;
				return PROJ_ERR_NAME_EXPECTED;
			}
			line -> key = Malloc (p-s+1);
			if (!line -> key)
				return -1;
			memmove (line -> key, s, p-s);
			line -> key [p-s] = 0;
			skip_spaces (&p);
			if (*p != '+' && *p != '-')	{
				parse_error_x = p-line->l;
				return PROJ_ERR_PLUS_EXPECTED;
			}
			line -> value = *p == '+' ? ONVALUE : OFFVALUE;
			line -> type = OPTION;
			line -> new_define = 1;
			break;
		case '+':
		case '-':
			p = s+1;
			skip_spaces (&p);
			t = skip_name (p);
			if (p == t)	{
				parse_error_x = p - line -> l;
				return PROJ_ERR_NAME_EXPECTED;
			}
			line -> key = Malloc (t-p+1);
			if (!line -> key)
				return -1;
			memmove (line -> key, p, t-p);
			line -> key [t-p] = 0;
			line -> value = *s == '+' ? ONVALUE : OFFVALUE;
			line -> type = OPTION;
			if (*s=='-')
				return parse_minus (line, t);
			break;
		default:
			line -> type = BAD_LINE;
			parse_error_x = s - line -> l;
			return PROJ_ERR_BAD_SYMBOL;
	}
	return 0;
}

ParsedProject Parse_project (int file, BOOL add_errors, int * errcnt)
{
	int v, y;
	int cnt;
	TextLine * l;
        ParsedProject proj;
	TBUF * buf;
	
	cnt = 0;
	* errcnt = -1;

	buf = Find_or_read_file (file);
	if (!buf)
		return NULL;

	proj = make_linelist (buf, file);
	tbuf_release (buf);
	if (!proj)
		return NULL;

	proj -> end_point = 0;
	proj -> complex = 0;
	y = 0;
	for (l = proj->line_list; l; l = l->next)	{
		v = parse_line (l);
		if (v < 0)	{
			Clear_project_parsing (proj);
			ErrCode = ERROR_NOT_ENOUGH_MEMORY;
			return NULL;
		}
		if (v)	{
			if (add_errors)
				Add_error (converr (v), 0, parse_error_x, y, file, MSG_ERROR);
			++  cnt;
		}
		y ++;
	}
	proj -> end_point = 0;
	for (l = proj -> line_list; l; l = l->next)	{
		if ((l -> type == MODULE || l->type == PACKAGE || l->type == EXCLAMATION) &&
		    ! proj->end_point)
			proj -> end_point = l;
		if (l -> type == EXCLAMATION)	{
			proj -> complex = 1;
			break;
		}
	}

	* errcnt = cnt;
	if (cnt && add_errors)	{
		Clear_project_parsing (proj);
		return NULL;
	} else
		return proj;
}

TextLine * find_proj_option (ParsedProject proj, char * name)
{
	TextLine * l;
	if (! proj) return FALSE;

	for (l = proj -> line_list; l && l != proj -> end_point; l = l -> next)
		if (l -> key && !stricmp (name, l -> key))
			return l;
	return NULL;
}

/* ------- why so ? ------------ */


void	SetValue (Option * opt, char * value)
{
	int i;
	opt -> present = TRUE;
	switch (opt -> type)	{
	case Boolean:
		opt -> curval = (value [0] == '+') ^ opt -> reversed; break;
	case Equation:
		set_string_val (opt, value); break;
	case Enumeration:
		for (i = 0; i < opt -> nalts; i ++)
			if (! stricmp (opt -> values [i], value))	{
				opt -> curval = i;
				return;
			}
		opt -> curval = opt -> defval;
		opt -> present = FALSE;
	}
}

void	SetDefValue (Option * opt, char * value)
{
	int i;
	opt -> present = TRUE;
	switch (opt -> type)	{
	case Boolean:
		opt -> defval = (value [0] == '+') ^ opt -> reversed; break;
	case Equation:
		free (opt -> defstring);
		opt -> defstring = NULL;
		opt -> defstring = alloc_copy (value);
		break;
	case Enumeration:
		for (i = 0; i < opt -> nalts; i ++)
			if (! stricmp (opt -> values [i], value))	{
				opt -> defval = i;
				return;
			}
	}
}


void	FillOptVal (Option * opt)
{
	TextLine * l;

	l = find_proj_option (EditOpt, opt -> name);
	if (l) SetDefValue (opt, l -> value);
	opt -> curval = opt -> defval;
	set_string_val (opt, opt -> defstring);
	opt -> present = 0;
	l = find_proj_option (EditProj, opt -> name);
	if (l) SetValue (opt, l -> value);
}

void	StoreOptVal (Option * opt)
{
	char s [512];
	char * p, * q, * val;
	TextLine * l;
	if (!opt -> present)	{
		l = find_proj_option (EditProj, opt -> name);
		if (l)
			del_proj_line (EditProj, l);
		return;
	}
	EditProj -> modified = TRUE;
	switch (opt -> type)	{
	case Boolean:
		val = (opt -> curval ^ opt -> reversed) ? ONVALUE : OFFVALUE;
		break;
	case Equation:
		val = opt -> curstring;
		break;
	case Enumeration:
		val = opt -> values [opt -> curval];
		break;
	default: return;
	}
	l = find_proj_option (EditProj, opt -> name);
	if (l)	{
		strcpy (s, l->l);
		p = s;
		skip_spaces (&p);
		*p = 0;
		switch (opt -> type)	{
		case Boolean:
			l -> value = val;
			wsprintf (p, "-%s%s", l->key, val);
			p = alloc_copy (s);
			if (!p) return;
			l -> l = p;
			break;
		case Equation:
		case Enumeration:
			wsprintf (p, "-%s = %s", l -> key, val);
			p = alloc_copy (s);
			if (!p) return;
			q = alloc_copy (val);
			if (!q)	{
				free (p);
				return;
			}
			free (l -> l);
			l -> l = p;
			freeval (l -> value);
			l -> value = q;
		}
	} else	{
		switch (opt -> type)	{
		case Boolean:
			wsprintf (s, "-%s%s%s", opt -> name,
				  opt->new_define ? ":":"", val);
			break;
		case Equation:
		case Enumeration:
			wsprintf (s, "-%s %c %s", opt -> name,
				  opt->new_define ? '!':'=', val);
			break;
		}
		l = add_proj_line (EditProj, s, strlen (s));
		if (!l) return;
		parse_line (l);
	}
}

void	Add_edit_proj_name (char * s)
{
	strcat (s, filedir [EditProj->file]);
}

void	Set_proj_to_edit (ParsedProject proj, ParsedProject opt)
{
	EditProj = proj;
	EditOpt  = opt;
}

BOOL	Is_complex (ParsedProject proj)
{
	return proj -> complex;
}

BOOL	Save_parsed_project (ParsedProject proj)
{
	TextLine * l;

	if (!Put_file_start ()) return FALSE;
	for (l = proj -> line_list; l; l = l -> next)
		if (!Put_file (l->l, strlen (l->l)))
			return FALSE;
	return Put_file_fin (proj->file, FALSE);
}

void	proj_save_error (ParsedProject p)
{
	MsgBox ("Write error", MB_OK, "Error writing project file %s:\n%s", 
		filedir [p->file], ErrText ());
}

BOOL	Save_edited_project (void)
{
	if (Save_parsed_project (EditProj)) return TRUE;
	proj_save_error (EditProj);
	return FALSE;
}

void	Enum_lookups (ParsedProject p, void (* EnumLookupProc) (char * s))
{
	TextLine * l;
	for (l = p -> line_list; l; l = l -> next)
		if (l -> type == EQUATION && !stricmp (l->key, "lookup") && l->value)
			EnumLookupProc (l->value);
}

char * Get_equation (ParsedProject proj, char * name)
{
	TextLine * l = find_proj_option (proj, name);
	return l ? l->value : NULL;
}

BOOL	Get_option (ParsedProject proj, char * name)
{
	TextLine * l = find_proj_option (proj, name);
	return l && l->value==ONVALUE;
}

void	Enum_proj_modules (BOOL (* enumproc) (char * line))
{
	TextLine * t;
	char * s;
	if (!EditProj) return;
	for (t = EditProj->line_list; t; t = t -> next)
		if (t->type == MODULE)	{
			s = t->l;
			++s;
			skip_spaces (&s);
			s = skip_name (s);
			skip_spaces (&s);
			if (! enumproc (s)) return;
		}
}

TextLine * find_proj_module (ParsedProject proj, int i)
{
	TextLine * t;
	for (t = proj->line_list; t; t = t -> next)
		if (t->type == MODULE && ! i--)
			return t;
	return NULL;
}

BOOL	Delete_proj_module (ParsedProject proj, int i)
{
	TextLine * t = find_proj_module (proj, i);
	if (!t) return FALSE;

	del_proj_line (proj, t);
	return TRUE;
}

BOOL	Edit_proj_module (ParsedProject proj, int i, char * file)
{
	char s[STRLEN], *p;
	TextLine * t = find_proj_module (proj, i);
	if (!t) return FALSE;

	strcpy (s, "!module ");
	strcat (s, file);
	p = alloc_copy (s);
	if (!p) return FALSE;

	free (t->l);
	t->l = p;
	return TRUE;
}

BOOL	Add_proj_module (ParsedProject proj, char * file)
{
	TextLine *r, *q, *t;
	char s [STRLEN], *p;

	strcpy (s, "!module ");
	strcat (s, file);
	p = alloc_copy (s);
	if (!p)	{
		ErrCode = ERROR_NOT_ENOUGH_MEMORY;
	 	return FALSE;
	}
	r = Malloc (sizeof (TextLine));
	if (!r)	{
		free (p);
		ErrCode = ERROR_NOT_ENOUGH_MEMORY;
		return FALSE;
	}
	r -> type = MODULE;
	r -> l = p;
	r -> key = 0;
	r -> value = 0;
	r -> new_define = 0;

	proj -> modified = TRUE;

	q = NULL;
	for (t = proj->line_list; t; t = t->next)
		if (t -> type == MODULE)
			q = t;
	if (q)	{
		r -> next = q -> next;
		q -> next = r;
		return TRUE;
	}

	q = proj->end_point;

	if (proj->line_list == q)	{
		proj->line_list = r;
		r -> next = q;
		proj->end_point = r;
	} else	{
		r -> next = q;
		for (t = proj->line_list; t; t = t->next)
			if (t -> next == q)	{
				t -> next = r;
				r -> next = q;
				proj->end_point = r;
				break;
			}
	}
	return TRUE;
}

BOOL	find_and_insert_file (ParsedProject proj, char * file)
{
	if (find_proj_module (proj, 0))
		return Edit_proj_module (proj, 0, file);
	else
		return Add_proj_module (proj, file);
}

BOOL	Replace_start_file (int projfile, char * startfile)
{
	ParsedProject proj;
	int errcnt;
	BOOL b;

	proj = Parse_project (projfile, FALSE, &errcnt);
	if (!proj)
		return FALSE;
	b = find_and_insert_file (proj, startfile);

	if (b)	{
		if (!Save_parsed_project (proj))	{
			proj_save_error (proj);
			b = FALSE;
		}
	}

	Clear_project_parsing (proj);
	return b;
}

void	Enum_proj_packages (BOOL (* enumproc) (char * dir, char * mask))
{
	TextLine * t;
	if (!EditProj) return;
	for (t = EditProj->line_list; t; t = t -> next)
		if (t->type == PACKAGE)	{
			if (! enumproc (t->value, t->key)) return;
		}
}

TextLine * find_proj_package (ParsedProject proj, int i)
{
	TextLine * t;
	for (t = proj->line_list; t; t = t -> next)
		if (t->type == PACKAGE && ! i--)
			return t;
	return NULL;
}

BOOL	update_package_line (TextLine * t, char * file, char * mask)
{
	char s[STRLEN], *p;

	strcpy (s, "!package ");
	strcat (s, mask);
	strcat (s, " ");
	strcat (s, file);
	p = alloc_copy (s);
	if (!p) return FALSE;
	free (t->l);
	t->l = p;
	return TRUE;
}

BOOL	Edit_proj_package_file (ParsedProject proj, int i, char * file)
{
	char *v;
	TextLine * t = find_proj_package (proj, i);
	if (!t) return FALSE;

	v = alloc_copy (file);
	if (! update_package_line (t, file, t->key))
		return FALSE;
	if (!v) return FALSE;
	free (t->value);
	t->value = v;
	return TRUE;
}

BOOL	Edit_proj_package_mask (ParsedProject proj, int i, char * mask)
{
	char *k;
	TextLine * t = find_proj_package (proj, i);
	if (!t) return FALSE;

	k = alloc_copy (mask);
	if (!k) return FALSE;
	if (! update_package_line (t, t->value, mask))
		return FALSE;
	free (t->key);
	t->key = k;
	return TRUE;
}

char *	Get_proj_package_file (ParsedProject proj, int i)
{
	TextLine * t = find_proj_package (proj, i);
	if (!t) return NULL;
	return t->value;
}

char *	Get_proj_package_mask (ParsedProject proj, int i)
{
	TextLine * t = find_proj_package (proj, i);
	if (!t) return NULL;
	return t->key;
}

BOOL	Add_proj_package (ParsedProject proj, char * file, char * mask)
{
	char *p, *k, *v, s[STRLEN];
	TextLine *r, *q, *t;
	strcpy (s, "!package ");
	strcat (s, mask);
	strcat (s, " ");
	strcat (s, file);
	p = alloc_copy (s);
	k = alloc_copy (mask);
	v = alloc_copy (file);
	r = Malloc (sizeof (TextLine));
	if (!p || !k || !v || !r)	{
		ErrCode = ERROR_NOT_ENOUGH_MEMORY;
	 	return FALSE;
	}
	r -> type = PACKAGE;
	r -> l = p;
	r -> key = k;
	r -> value = v;
	r -> new_define = 0;

	proj -> modified = TRUE;

	q = NULL;
	for (t = proj->line_list; t; t = t->next)
		if (t -> type == PACKAGE)
			q = t;
	if (q)	{
		r -> next = q -> next;
		q -> next = r;
		return TRUE;
	}

	q = proj->end_point;

	if (proj->line_list == q)	{
		proj->line_list = r;
		r -> next = q;
		proj->end_point = r;
	} else	{
		r -> next = q;
		for (t = proj->line_list; t; t = t->next)
			if (t -> next == q)	{
				t -> next = r;
				r -> next = q;
				proj->end_point = r;
				break;
			}
	}
	return TRUE;
}

BOOL	Delete_proj_package (ParsedProject proj, int i)
{
	TextLine * t = find_proj_package (proj, i);
	if (!t) return FALSE;

	del_proj_line (proj, t);
	return TRUE;
}

void	Enum_proj_options (ParsedProject proj, BOOL (* enumproc) (char * line))
{
	TextLine * t;

	for (t = proj->line_list; t && t != proj->end_point; t = t -> next)
		if (t->type == EQUATION || t->type == OPTION)
			if (! enumproc (t->l))
				return;
}

void	Enum_user_options (BOOL (* enumproc) (char * line))
{
	TextLine * t;
	if (!EditProj) return;
	for (t = EditProj->line_list; t && t != EditProj->end_point; t = t -> next)
		if (t->type == OPTION && t->new_define)
			if (! enumproc (t->key))
				return;
}

void	Enum_user_equations (BOOL (* enumproc) (char * line))
{
	TextLine * t;

	if (!EditProj) return;

	for (t = EditProj->line_list; t && t != EditProj->end_point; t = t -> next)
		if (t->type == EQUATION && t->new_define)
			if (! enumproc (t->key))
				return;
}

BOOL	Test_option_presence (ParsedProject proj, char * name)
{
	return find_proj_option (proj, name) != NULL;
}

BOOL	Set_user_option (ParsedProject proj, char * name, BOOL val)
{
	TextLine * l;
	char * v, *p;
	char s [STRLEN];
	if (!proj) return FALSE;
	v = val? ONVALUE : OFFVALUE;
	l = find_proj_option (proj, name);
	proj->modified = TRUE;

	if (l)	{
		strcpy (s, l->l);
		p = s;
		skip_spaces (&p);
		*p = 0;
		wsprintf (p, "-%s:%s", l->key, v);
		p = alloc_copy (s);
		if (!p) return FALSE;
		l -> l = p;
		l -> value = v;
		return FALSE;
	} else	{
		wsprintf (s, "-%s:%s", name, v);
		l = add_proj_line (proj, s, strlen (s));
		if (!l) return FALSE;
		parse_line (l);
		return TRUE;
	}
}

BOOL	Set_user_equation (ParsedProject proj, char * name, char * val)
{
	char s [512];
	char * p, * q;
	TextLine * l;

	l = find_proj_option (proj, name);
	if (l)	{
		strcpy (s, l->l);
		p = s;
		skip_spaces (&p);
		*p = 0;
		wsprintf (p, "-%s := %s", l -> key, val);
		p = alloc_copy (s);
		if (!p) return FALSE;
		q = alloc_copy (val);
		if (!q)	{
			free (p);
			return FALSE;
		}
		free (l -> l);
		l -> l = p;
		freeval (l -> value);
		l -> value = q;
		return TRUE;
	} else	{
		wsprintf (s, "-%s := %s", name, val);
		l = add_proj_line (proj, s, strlen (s));
		if (!l) return FALSE;
		parse_line (l);
		return TRUE;
	}
}

BOOL	Delete_option (ParsedProject proj, char * name)
{
	TextLine * t;
	
	if (!proj) return FALSE;
	t = find_proj_option (proj, name);
	if (!t) return FALSE;
	del_proj_line (proj, t);
	return TRUE;
}


BOOL	Apply_style (ParsedProject proj, ParsedProject sty)
{
	TextLine * l, *p;
	char * s, *v;

	for (l = sty->line_list; l && l != sty->end_point; l = l->next)
		if (l->type == OPTION || l->type == EQUATION)	{
			p = find_proj_option (proj, l->key);
			if (p)	{
				s = alloc_copy (l->l);
				if (!s) return FALSE;
				v = copyval (l->value);
				if (!v)	{
					free (s);
					return FALSE;
				}
				free (p->l);
				p->l=s;
				freeval (p->value);
				p->value = v;
			} else	{
				p = add_proj_line (proj, l->l, strlen (l->l));
				if (!p) return FALSE;
				if (parse_line (p))
					return FALSE;
			}
		}
	return TRUE;
}
