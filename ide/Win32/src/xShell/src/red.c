#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "red.h"

typedef unsigned char uchar;

#define _str 0
#define _any 1  /* "?"  */
#define _set 2  /* "[]" */
#define _bra 3  /* "{}" */
#define _seq 4  /* "*"  */
#define _and 5  /* "&"  */
#define _or  6  /* "|"  */
#define _not 7  /* "^"  */
#define _par 8  /* "()" */

typedef unsigned long (* char_set)[8];
#ifdef IN
#undef IN
#endif
#ifdef INCL
#undef INCL
#endif

#define IN(c,set) ((1ul << ((c)%32)) & (*(set))[c/32])
#define INCL(c,set) (*(set))[(c)/32] |= 1ul << ((c)%32)

typedef struct _Expr {
	struct _Expr * next;
	short          kind;
	struct _Expr * left;
	uchar	     * pat;
	char_set       leg;
}
	Expr;

static	uchar * expr;
static	int pos;
static	int error;

static void re (Expr **);

static  void app (Expr ** reg, Expr * r)
{
	Expr * t = 0;
	if (*reg==0) {
		*reg = r;
		return;
	}
	t = *reg;
	while (t->next!=0) t = t->next;
	if (t->kind==_seq && r->kind==_seq) error = 1;
	else t->next = r;
}

static  Expr * new_expr (short kind)
{
	Expr * n;
	n = Malloc (sizeof (Expr));
	n->kind   = kind;
	n->next   = 0;
	n->left   = 0;
	n->pat    = 0;
	n->leg    = 0;
	return n;
}

static  Expr * app_new (Expr ** reg, short kind)
{
	Expr * n = new_expr (kind);
	app (reg, n);
	return n;
}

static  uchar  char_code (uchar c, int * j)
{
	int n;
	c -='0';
	for (n = 0; n<=1; n++) {
		if ('0'<=expr[*j] && expr[*j]<='7') {
			c = c*8 + expr[*j] - '0';
			++*j;
		} else {
			error = 1;
			return c;
		}
	}
	return c;
}

static  uchar  esc (int * j)
{
	uchar c;
	++*j;
	c = expr[*j];
	++*j;
	if ('0'<=c && c<='7')
		return char_code (c, j);
	else
		return c;
	return 0;
}

static  void  incl (uchar from, Expr * n, uchar * range, uchar ch)
{
	int i;
	if (!*range) {
		INCL (ch, n->leg);
		return;
	}
	if (from>ch) {
		error = 1;
		return;
	}
	for (i = from; i <= ch; i++)
			INCL (i, n->leg);
	*range = 0;
}

static  void  fill_set (Expr * n)
{
	uchar from;
	uchar range;
	short j;
	uchar ch, q, inv;

	if (error) return;
	n -> leg = Malloc (sizeof (unsigned long) * 8);
	for (j = 0; j<=7; j++)
		(*n->leg)[j] = 0;
	range = 0;
	from = '\0';
	ch = '\0';
	if (expr[pos]=='[')
		q = ']';
	else {
		q = '}';
		n->kind = _bra;
	}
	++pos;
	inv = expr[pos]=='^';
	if (inv) ++pos;
	if (expr[pos]==q) {
		error = 1;
		return;
	}
	while (expr[pos]!=q && expr[pos]!='\0' && !error) {
		if (expr[pos]=='\\' && expr [pos+1]) {
			ch = esc (&pos);
			incl (from, n, &range, ch);
		} else if (expr[pos]=='-'&& expr[pos+1]!=q) {
			from = ch;
			range = 1;
			++pos;
		} else {
			ch = expr[pos];
			incl (from, n, &range, ch);
			++pos;
		}
	}
	if (error) return;
	if (expr[pos]!=q || range)
		error = 1;
	else {
		++pos;
		if (!inv) return;
		for (j = 0; j<=7; j++)
			(*n->leg)[j] = ~(*n->leg)[j];
	}
}

static int scan (Expr * n)
{
	uchar ch;
	int j, c;
	j = pos;
	c = 0;
	while (1) {
		ch = expr[j];
		if (ch=='\0' || ch=='[' || ch=='*' || ch=='?' || ch=='{' || ch==')'
		    || ch=='&' || ch=='|' || ch=='^' || ch=='$')
			break;
		if (ch=='\\' && expr[j+1]!='\0') {
			ch = esc (&j);
			if (n->pat)
				n->pat[c] = ch;
			++c;
		}
		else	{
			if (n->pat)
				n->pat[c] = expr[j];
			j++;
			c++;
		}
	}
	if (n->pat)	{
		pos = j;
		n->pat [c] = 0;
	}
	return c;
}

static  void  fill_str (Expr * n)
{
	if (error) return;
	n->pat = Malloc (scan (n) + 1);
	scan (n);
}

static  void simple (Expr ** reg)
{
	Expr * n;

	if (error) return;
	if (!expr[pos])	{
		error = 1;
		return;
	}
	for (;;) {
		if (error) return;
		switch (expr[pos])	{
		case '\0':
		case ')':
		case '(':
		case '|':
		case '&':
		case '^':	return;
		case '*':	n = app_new (reg, _seq); ++pos; break;
		case '?':	n = app_new (reg, _any); ++pos; break;
		case '{':
		case '[':	n = app_new (reg, _set); fill_set (n); break;
		default:	
				n = app_new (reg, _str);
				fill_str (n);
		}
	}
}

static  void factor (Expr ** reg)
{
	*reg = 0;
	if (error) return;
	if (!expr[pos])	{
		error = 1;
		return;
	}
	if (expr[pos]=='(') {
		++pos;
		*reg = new_expr (_par);
		if (error) return;
		re (&(*reg)->next);
		if (error) return;
		if (expr[pos]==')')
			++pos;
		else error = 1;
	} else if (expr[pos]=='^') {
		++pos;
		*reg = new_expr (_not);
		if (error) return;
		factor (&(*reg)->next);
	} else
		simple (reg);
}

static  void term (Expr ** reg)
{
	Expr * t;

	*reg = 0;
	if (error) return;
	if (!expr[pos])	{
		error = 1;
		return;
	}
	factor (reg);
	if (expr[pos]=='&' && !error) {
		t = new_expr (_and);
		++pos;
		t->left = *reg;
		*reg = t;
		term (&t->next);
	}
}

static  void re (Expr ** reg)
{
	Expr * t;

	*reg = 0;
	if (error) return;
	if (!expr [pos])	{
		error = 1;
		return;
	}
	term (reg);
	if (expr [pos]=='|' && !error) {
		t = new_expr (_or);
		++pos;
		t->left = *reg;
		*reg = t;
		re (&t->next);
	}
}

/** result <= 0 -- error in position ABS (result);
    result >  0 -- done.
*/

int	compile_expr (unsigned char * _expr, Expr ** reg)
{
	expr = _expr;
	pos  = 0;
	*reg = 0;
	error  = 0;
	re (reg);
	if (error || expr [pos])
		return -pos;
	else
		return pos;
}

void	FreeExpr (Expr * reg)
{
	Expr * p;
	while (reg)	{
		p = reg -> next;
		if (reg -> pat) free (reg -> pat);
		if (reg -> leg) free (reg -> leg);
		if (reg -> left) free (reg -> left);
		free (reg);
		reg = p;
	}
}

static  int  bra_seq_end (Expr ** reg)
{
	while (*reg && ((*reg)->kind == _bra || (*reg) -> kind == _seq))
		*reg = (*reg)->next;
	return *reg==0;
}

int  Match (Expr * reg, uchar *s)
{
	uchar * p;
	if (reg==0) return *s=='\0';
	if (!*s)
		return reg==0 || (reg->kind == _bra || reg -> kind == _seq) && reg->next==0;
	if (reg->kind==_any)
		return Match (reg->next, s+1);
	else if (reg->kind==_seq) {
		while (*s) {
			if (Match (reg->next, s))
				return 1;
			++s;
		}
		return bra_seq_end (&reg);
	}
	else if (reg->kind==_set) {
		if (! IN (tolower (*s), reg->leg)) return 0;
		return Match (reg->next, s+1);
	}
	else if (reg->kind==_bra) {
		while (*s) {
			if (Match (reg->next, s))
				return 1;
			if (! IN (*s, reg->leg)) return 0;
			++s;
		}
		return bra_seq_end (&reg);
	}
	else if (reg->kind==_str) {
		for (p = reg->pat; *p; p++) {
			if (tolower (*s) != tolower (*p)) return 0;
			++s;
		}
		return Match (reg->next, s);
	}
	else if (reg->kind==_and)
		return Match (reg->left, s) && Match (reg->next, s);
	else if (reg->kind==_or) {
		return Match (reg->left, s) || Match (reg->next, s);
	}
	else if (reg->kind==_not) {
		if (Match (reg->next, s))
			return 0;
		else {
			while (*s) ++s;
			return 1;
		}
	}
	else if (reg->kind==_par)
		return Match (reg->next, s);
	return 0;
}

/*=========================================================================*/

typedef	struct	_DirList	{
		char * dir;
		struct _DirList * next;
	}
		DirList;

typedef struct _Redir {
		Expr * expr;
		DirList * list;
		struct _Redir * next;
	}
		Redir;

void	Add_redir (Redir ** p, Redir * q)
{
	if (!q) return;
	while (*p) p = & (*p) -> next;
	*p = q;
}

void	add_dir_to_list (Redir * p, DirList * d)
{
	DirList * c;
	c = p -> list;
	if (!c)
		p -> list = d;
	else	{
		while (c -> next) c = c -> next;
		c -> next = d;
	}
	d -> next = NULL;
}

static char * skipspaces (char * p)
{
	while (isspace (*p)) ++p;
	return p;
}

static void lowerline (char * s)
{
	for (;*s;s++) *s = tolower (*s);
}

Redir *	ParseRedLine (char * p)
{
	char * s, *t, *u;
	Expr * e;
	Redir * r;
	DirList * d;
	p = skipspaces (p);
	if (!*p) return 0;
	s = p;
	while (*s && *s != '=') ++s;
	if (!*s) return 0;
	if (s == p) return 0;
	for (t = s-1; isspace (*t); --t);
	t[1] = 0;
	lowerline (p);
	if (compile_expr (p, &e)<0) return 0;
	r = Malloc (sizeof (Redir));
	r->expr = e;
	r->next = 0;
	r->list = 0;
	s = skipspaces (s+1);
	while (1)	{
		if (!*s) return r;
		t=s;
		while (*t && *t!=';') ++t;
		for (u = t-1; isspace (*u); --u);
		d = Malloc (sizeof (DirList));
		d -> next = 0;
		d -> dir = Malloc (u-s+2);
		strncpy (d -> dir, s, u-s+1);
		d->dir [u-s+1] = 0;
		lowerline (d->dir);
		add_dir_to_list (r, d);
		if (!*t) return r;
		s = skipspaces (t+1);
	}		
}

void	FreeRedir (Redir * r)
{
	Redir * n;
	DirList * dn;
	while (r)	{
		n = r -> next;
		while (r -> list)	{
			dn = r -> list -> next;
			free (r -> list -> dir);
			free (r -> list);
			r -> list = dn;
		}
		FreeExpr (r -> expr);
		free (r);
		r = n;
	}
}

BOOL	find_in_red_list (char * name, char * res, Redir * red)
{
	FILE * f;
	DirList * dir;

	for (dir = red -> list; dir; dir = dir -> next)	{
		strcpy (res, dir -> dir);
		strcat (res, "\\");
		strcat (res, name);
		f = fopen (res, "r");
		if (f)	{
			fclose (f);
			return TRUE;
		}
	}
	return FALSE;
}

int	Search_with_red (char * name, char * res, Redir * red)
{
	if (name [0] == '\\' || name [1] == ':')	{
		strcpy (res, name);
		return 1;
	}
	while (red)	{
		if (Match (red -> expr, name) && find_in_red_list (name, res, red))
			return 1;
		red = red -> next;
	}
	strcpy (res, name);
	return 0;
}

int	Create_with_red (char * name, char * res, Redir * red)
{
	if (Search_with_red (name, res, red))
		return 1;

	while (red)	{
		if (Match (red -> expr, res) && red->list)	{
			strcpy (res, red->list->dir);
			strcat (res, "\\");
			strcat (res, name);
			break;
		}
		red = red -> next;
	}
	strcpy (res, name);
	return 0;
}

