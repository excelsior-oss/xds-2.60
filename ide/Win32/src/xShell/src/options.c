#include "util.h"
#include "fileutil.h"
#include "options.h"
#include "shell.h"
#include "var.h"

Node * NodeList = NULL;
Node * LastNode = NULL;
Option * LastOpt;

Option * make_option (void)
{
	Option * opt;

	opt = Malloc (sizeof (Option));
	if (!opt) return NULL;
	opt -> name [0] = 0;
	opt -> comment [0] = 0;
	opt -> defval = opt -> curval = 0;
	opt -> defstring = opt -> curstring = NULL;
	opt -> nalts = 0;
	opt -> modified = 0;
	opt -> reversed = 0;
	opt -> new_define = 0;
	return opt;
}

Option * read_name (char ** s)
{
	char * p, c;
	Option * opt;

	++*s;
	skip_spaces (s);
	for (p = *s; isalnum (*p); p ++);
	if (p == *s) return NULL;
	opt = make_option ();
	if (!opt) return NULL;
	c = *p;
	*p = 0;
	scpy (opt -> name, *s);
	*p = c;
	*s = p;
	return opt;
}

char * read_string (char ** s)
{
	char * p, *str;

        skip_spaces (s);
	if (**s != '"') return NULL;
	++*s;
	for (p = *s; *p != '"' && *p; p ++);
	if (!*p) return NULL;
	*p = 0;
	str = Malloc (strlen (*s)+1);
	if (!str) return NULL;
	strcpy (str, *s);
	*s = p+1;
	return str;
}

void	read_comment (Option * opt, char * s)
{
	skip_spaces (&s);
	if (*s == ';')	{
		++s;
		skip_spaces (&s);
		scpy (opt -> comment, s);
		for (s = opt -> comment; *s; s ++)
			if (*s == '\\')	{
				*s = '\r';
				if (s[1]) s[1] = '\n';
			}
	} else
		scpy (opt -> comment, opt -> name);
}

Node * make_node (char * s, int level)
{
	Node * node;
	node = Malloc (sizeof (Node));
	node -> expanded = 0;
	if (*s == '^')	{
		node -> expanded = 1;
                ++s;
		skip_spaces (&s);
	}
	scpy (node -> string, s);
	node -> level = level;
	node -> optlist = NULL;
	node -> next = NULL;
	LastOpt = NULL;
	if (LastNode)
		LastNode -> next = node;
	else
		NodeList = node;
	LastNode = node;
	return node;
}

Option * LastGroup = NULL;

void	add_option (Option * opt)
{
	if (LastOpt)
		LastOpt -> next = opt;
	else	{
		if (! LastNode)	{
			LastNode = make_node ("", 0);
			if (! LastNode) return;
		}
		LastNode -> optlist = opt;
	}
	LastOpt = opt;
	opt -> curval = -1;
	opt -> curstring = NULL;
	opt -> next = NULL;
	opt -> present = 0;
	if (LastGroup) ++ LastGroup -> nalts;
}

void	scan_options (char * s)
{
	int level;
	Option * opt;
	char * def;
	BOOL reversed, newdef;
	int i;

	skip_spaces (&s);
	if (!*s) return;
	level = 0;
	switch (*s)	{
	case '\'': return;
	case '!':
		++s;
		i = 0;
		while (*s && !isspace (*s) && *s != ';')	{
			switch (toupper (*s))	{
			case '2': i |= STATIC_BIG; break;
			case 'B': i |= STATIC_BOLD; break;
			case 'I': i |= STATIC_ITALIC; break;
			case 'Z': i |= STATIC_CENTER; break;
			case 'R': i |= STATIC_RIGHT; break;
			}
			s++;
		}
		opt = make_option ();
		read_comment (opt, s);
		opt -> defval = i;
		opt -> type = Static;
		add_option (opt);
		break;
	case '[':
		++s;
		skip_spaces (&s);
		opt = make_option ();
		opt -> nalts = 1;
		if (*s >= '1' && *s <= '9')	{
			opt -> nalts = atoi (s);
			while (isdigit (*s)) ++s;
			skip_spaces (&s);
			if (*s != ']') {free (opt); return; }
		}
		skip_spaces (&s);
		if (*s == ']')	{
			++s;
			skip_spaces (&s);
			LastGroup = 0;
		} else	{
			LastGroup = opt;
			opt -> nalts = -1;
		}
		switch (*s)	{
		case '+': opt -> defval = GROUP_RAISED; ++s; break;
		case '-': opt -> defval = GROUP_INSET; ++s; break;
		case '0': opt -> defval = GROUP_NULL; ++s; break;
		default : opt -> defval = GROUP_BORDER; break;
		}
		opt -> type = Group;
		read_comment (opt, s);
		add_option (opt);
		break;
	case ']':
		LastGroup = 0;
		break;
	case '#':
	case '$':
	case '%':
		i = *s;
		newdef = s[1]==':';
		if (newdef) s++;
		if (!LastNode) return;
		opt = read_name (&s);
		skip_spaces (&s);
		if (*s != '=')	{ free (opt); return; }
		++s;
		def = read_string (&s);
		if (!def) { free (opt); return; }
		opt -> type = Equation;
		opt -> defstring = def;
		opt -> defval = i;
		opt -> new_define = newdef;
		read_comment (opt, s);
		add_option (opt);
		break;
	case '+':
	case '-':
		i = *s == '+';
		newdef = s[1]==':';
		if (newdef) s++;
		reversed = s[1]=='~';
		if (reversed) s++;
		opt = read_name (&s);
		opt -> type = Boolean;
		opt -> defval = i;
		read_comment (opt, s);
		opt -> reversed = reversed;
		opt -> new_define = newdef;
		add_option (opt);
		break;
	case '(':
		newdef = s[1]==':';
		if (newdef) s++;
		opt = read_name (&s);
		skip_spaces (&s);
		if (*s != ':') { free (opt); return; }
		opt -> nalts = opt -> defval = 0;
		do	{
			++s;
			if (*s == '!')	{
				opt -> defval = opt -> nalts;
				++s;
			}
			def = read_string (&s);
			if (!def)	{
				while (--opt -> nalts >= 0)
					free (opt -> values [opt -> nalts]);
				free (opt);
			}
			if (opt -> nalts < 10)
				opt -> values [opt -> nalts++] = def;
			skip_spaces (&s);
		} while (*s == ',');
		if (*s == ')') ++s;
		opt -> type = Enumeration;
		opt -> new_define = newdef;
		read_comment (opt, s);
		add_option (opt);
		break;
	case '@':
		++s;
		skip_spaces (&s);
		i = 0;
		if (!stricmp (s, "user-options"))
			i = SPECIAL_USEROPTIONS;
		else if (!stricmp (s, "user-equations"))
			i = SPECIAL_USEREQUATIONS;
		else if (!stricmp (s, "modules"))
			i = SPECIAL_MODULELIST;
		else if (!stricmp (s, "packages"))
			i = SPECIAL_PACKAGES;
		if (!i) break;
		opt = make_option ();
		opt -> type = Special;
		opt -> defval = i;
		add_option (opt);
		break;
	case '>':
		while (*s == '>') s ++, level ++;
		skip_spaces (&s);
	default:
		if (LastNode && level - LastNode -> level > 1)
			level = LastNode -> level + 1;
		make_node (s, level);
	}
}

void	clear_options (void)
{
	Node * n, * n1;
	Option * o, * o1;
	for (n = NodeList; n;)	{
		for (o = n -> optlist; o;)	{
			o1 = o -> next;
			free (o);
			o = o1;
		}
		n1 = n -> next;
		free (n);
		n = n1;
	}
	NodeList = LastNode = 0;
	LastOpt = 0;
}

void	signal_opt_err (char * string, char * fname)
{
	Option * opt;
	opt = make_option ();
	if (!opt) return;
	sprintf (opt -> comment, "%s option definition file", string);
	opt -> defval = STATIC_CENTER;
	opt -> type = Static;
	add_option (opt);
	opt = make_option ();
	if (!opt) return;
	scpy (opt -> comment, fname);
	opt -> defval = STATIC_CENTER | STATIC_BOLD;
	opt -> type = Static;
	add_option (opt);
}

void	read_options (void)
{
	FILE * f;
	char fname [_MAX_PATH * 2+3], fname2 [_MAX_PATH * 2+3];
	char buf [255];
	char * p;

	clear_options ();
	NodeList = NULL;
	LastNode = NULL;

	Make_line (XDSMAIN, fname, sizeof (fname));
	dir_only (fname);
	strcat (fname, "\\");
	strcat (fname, OptFileName);
	f = fopen (fname, "r");
	if (!f)	{
		strcpy (fname2, HomeDir);
		strcat (fname2, "\\");
		strcat (fname2, OptFileName);
		f = fopen (fname2, "r");
		if (!f)	{
			signal_opt_err ("Can't open", fname);
			return;
		}
	}
	while (fgets (buf, sizeof (buf), f))	{
		buf [sizeof (buf) - 1] = '\n';
		for (p = buf; *p != '\n'; p++);
		*p = 0;
		scan_options (buf);
	}
	if (ferror (f))	{
		clear_options ();
		signal_opt_err ("Error reading", fname);
	}
	fclose (f);
}

char * set_string_val (Option * opt, char * s)
{
	if (opt -> curstring)
		free (opt -> curstring);
	opt -> curstring = NULL;
	if (!s) return NULL;
	opt -> curstring = Malloc (strlen (s)+1);
	if (! opt -> curstring) return NULL;
	strcpy (opt -> curstring, s);
	return opt -> curstring;
}
