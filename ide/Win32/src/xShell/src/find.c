#include <windows.h>
#include <stdlib.h>
#include <assert.h>

#include "find.h"
#include "tbuf.h"
#include "tbuf_impl.h"

/* ----------------- Utilities ---------------------------------------- */

char * find_ichar (char * s, unsigned char c, int len)
{
	c = TOLOWER (c);

	for (; len--; s++)
		if (TOLOWER(*s) == c) return s;
	return NULL;
}

int	imemcmp (char * p, char * q, int len)
{
	for (; len--; p++, q++)
		if (TOLOWER (*p) != TOLOWER (*q))
			return 1;
	return 0;
}

char * find_char (char * s, char c, int len)
{
	for (; len--; s++)
		if (*s == c) return s;
	return NULL;
}

char * find_str (char * s, int len, char * p, int plen)
{
	char * r;
	len -= plen;
	if (len < 0) return NULL;
	if (len == 0)
		return memcmp (s, p, plen) ? NULL : s;
	r = s + len + 1;
	while (s < r)	{
		s = find_char (s, p[0], r-s);
		if (!s) return NULL;
		if (! memcmp (s, p, plen))
			return s;
		++ s;
	}
	return NULL;
}

char * find_istr (char * s, int len, char * p, int plen)
{
	char * r;
	len -= plen;
	if (len < 0) return NULL;
	if (len == 0)
		return imemcmp (s, p, plen) ? NULL : s;
	r = s + len + 1;
	while (s < r)	{
		s = find_ichar (s, p[0], r-s);
		if (!s) return NULL;
		if (! imemcmp (s, p, plen))
			return s;
		++ s;
	}
	return NULL;
}

char * find_last_str (char * s, int len, char * p, int plen)
{
	char * r, * q;
	q = NULL;
	for (;;)	{
		r = find_str (s, len, p, plen);
		if (!r) break;
		q = r;
		len -= r-s+1;
		s = r+1;
	}
	return q;
}

char * find_last_istr (char * s, int len, char * p, int plen)
{
	char * r, * q;
	q = NULL;
	for (;;)	{
		r = find_istr (s, len, p, plen);
		if (!r) break;
		q = r;
		len -= r-s+1;
		s = r+1;
	}
	return q;
}


BOOL	check_affix (TBUF * buf, int x1, int y1, int x2, int y2, BOOL prefix, BOOL suffix)
{
	if (prefix && x1 && IS_ALNUM (buf->lines [y1].s[x1-1]))
		return FALSE;
	if (suffix && x2 < buf->lines [y2].len && 
	    IS_ALNUM (buf->lines [y1].s[x2]))
		return FALSE;
	return TRUE;
}

/* ----------------- Pattern compiling for advanced search ------------ */

typedef enum	{
			LITERAL_STRING,
			WHITE_SPACE,
			DIGIT,
			ALPHA,
			NUMBER,
			IDENT,
			ANY_CHAR
		}
			TOKEN_TYPE;

typedef struct	{
			TOKEN_TYPE type;
			unsigned char * s;
			int len;
		}
			TOKEN;

typedef struct	{
			TOKEN * t;
			int n;
			int len;
			BOOL exact;
			BOOL left_cr;
			BOOL right_cr;
		}
			LINE;

typedef struct	_SEARCH_PATTERN {
			LINE * l;
			int n;
			char * line;
			TOKEN * tokens;
		}
			SEARCH_PATTERN;

void count_line_len (LINE * l)
{
	int i, len;
	len = 0;

	l->len = 0;
	l->exact = 1;
	for (i = 0; i < l->n; i++)
		switch (l->t[i].type)	{
		case LITERAL_STRING : l->len += l->t[i].len; break;
		case NUMBER:
		case IDENT:
		case WHITE_SPACE: l->exact = 0;
		case DIGIT:
		case ALPHA:
		case ANY_CHAR:	l->len += 1;
		}
}


SEARCH_PATTERN * compile_pattern (char * s)
{
	SEARCH_PATTERN * p;
	int len;
	char * d;
	LINE * l;
	TOKEN * t;
	int i;

	len = strlen (s);
	p = malloc (sizeof (SEARCH_PATTERN));
	p->line = malloc (len+1);

	p->tokens = malloc (len * sizeof (TOKEN));
	p->l = malloc (((len+1)/2) * sizeof (LINE));
	p->n = 0;
	l = p->l;
	l->n = 0;
	t = l->t = p->tokens;
	d = p->line;
	while (*s)	{
		t->s = d;
		while (*s)
			if (*s != '\\')
				*d++ = *s++;
			else if (s[1] == 't')	{
				*d++ = '\t';
				s += 2;
			} else if (s[1] == '\\')	{
				*d++ = '\\';
				s += 2;
			} else
				break;
		if (d > t->s)	{
			t->len = d-t->s;
			*d++ = 0;
			t->type = LITERAL_STRING;
			t++;
			continue;
		}
		if (!*s)
			break;
		switch (s[1])	{
		case 'n':
			l->n = t-l->t;
			l++;
			l->t = t;
			s += 2;
			continue;
		case 'w':
			t->type = WHITE_SPACE;
			break;
		case '#':
			t->type = DIGIT;
			break;
		case '$':
			t->type = ALPHA;
			break;
		case 'N':
			t->type = NUMBER;
			break;
		case 'I':
			t->type = IDENT;
			break;
		case '?':
			t->type = ANY_CHAR;
			break;
                default:
			s++;
			continue;
		}			
		t++;
		s += 2;
	}
	l->n = t-l->t;
	l++;
	p->n = l-p->l;
	for (i = 0; i < p->n; i++)
		p->l[i].left_cr = p->l[i].right_cr = 1;
	if (p->n)	{
		p->l[p->n-1].right_cr = 0;
		p->l[0].left_cr = 0;
	}
	if (p->n >= 2 && p->l[p->n-1].n == 0)
		-- p->n;
	if (p->n >= 2 && p->l[0].n == 0)	{
		memmove (p->l, p->l+1, (p->n-1) * sizeof (LINE));
		-- p->n;
	}
	for (i = 0; i < p->n; i++)
		count_line_len (&p->l [i]);
		
	return p;
}

void free_pattern (SEARCH_PATTERN * p)
{
	free (p -> l);
	free (p -> line);
	free (p -> tokens);
	free (p);
}

#if 0

void dump_pattern (SEARCH_PATTERN * p)
{
	int i, j;

	printf ("Pattern: %d lines\n", p->n);
	for (i = 0; i < p->n; i++)	{
		printf ("line %d: (%d) %c%c. ", i, p->l[i].n,
			p->l[i].left_cr ? 'L':' ',
			p->l[i].right_cr ? 'R':' ');
		for (j = 0; j < p->l[i].n; j++)	{
			TOKEN * t = &p->l[i].t[j];
			switch (t->type)	{
			case LITERAL_STRING : printf ("<%s>", t->s); break;
			case WHITE_SPACE: printf ("\\w"); break;
			case DIGIT:	printf ("\\#"); break;
			case ALPHA:	printf ("\\$"); break;
			case NUMBER:	printf ("\\N"); break;
			case IDENT:	printf ("\\I"); break;
			case ANY_CHAR:	printf ("\\?"); break;
			default: printf ("@%d", t->type); break;
			}
			printf (" ");
		}
		printf ("\n");
	}
}
#endif

/* ----------------------- pattern search ----------------------------------- */

int	imemcmp (char * p, char * q, int len);

/* Returns: > 0 - matches; result is matching string len
              0 - not matches;
*/

int	line_prefix_matches (char * s, int len, LINE * line, BOOL ignore_case)
{
	int i, len0;

	len0 = len;

	for (i = 0; i < line->n; i++)	{
		if (! len)
			return 0;

		switch (line->t[i].type)	{
		case LITERAL_STRING:
			len -= line->t[i].len;
			if (len < 0) return FALSE;
			if (ignore_case)	{
				if (imemcmp (s, line->t[i].s, line->t[i].len))
					return FALSE;
			} else	{
				if (memcmp (s, line->t[i].s, line->t[i].len))
					return FALSE;
			}
			s += line->t[i].len;
			break;

		case WHITE_SPACE:
			if (!IS_WHITE (*s))
				return FALSE;
			do {
				++ s;
				-- len;
			} while (len && IS_WHITE (*s));
			break;

		case DIGIT:
			if (!IS_DIGIT (*s))
				return FALSE;
			++ s;
			-- len;
			break;
				
		case ALPHA:
			if (! IS_ALPHA (*s))
				return FALSE;
			++ s;
			-- len;
			break;
				
		case NUMBER:
			if (! IS_DIGIT (*s))
				return FALSE;
			do {
				++ s;
				-- len;
			} while (len && IS_DIGIT (*s));
			break;
			
		case IDENT:
			if (! IS_ALPHA (*s))
				return FALSE;
			do {
				++ s;
				-- len;
			} while (len && IS_ALNUM (*s));
			break;
		case ANY_CHAR:
			++ s;
			-- len;
			break;
		}
	}

	if (line -> right_cr && len)
		return 0;

	return len0-len;
}

int	find_suspicious_prefix (char * s, int len, LINE * line, BOOL ignore_case)
{
	char * p;
	int i;

	if (len < line->len) return -1;

	switch (line->t[0].type)	{
	case LITERAL_STRING:
		if (ignore_case)
			p = find_istr (s, len, line->t[0].s, line->t[0].len);
		else
			p = find_str  (s, len, line->t[0].s, line->t[0].len);
		if (!p) return -1;
		return p-s;

	case WHITE_SPACE:
		for (i = 0; i < len; i++)
			if (IS_WHITE (s[i]))
				return i;
		return -1;

	case DIGIT:
	case NUMBER:
		for (i = 0; i < len; i++)
			if (IS_DIGIT (s[i]))
				return i;
		return -1;
			
	case ALPHA:
	case IDENT:
		for (i = 0; i < len; i++)
			if (IS_ALPHA (s[i]))
				return i;
		return -1;
	case ANY_CHAR:
		return 0;
	}
	return -1;
}

int	skip_first_token (char * s, int len, LINE * line)
{
	int i;

	switch (line->t[0].type)	{
	case WHITE_SPACE:
		for (i = 0; i < len; i++)
			if (!IS_WHITE (s[i]))
				break;
		return i;

	case NUMBER:
		for (i = 0; i < len; i++)
			if (!IS_DIGIT (s[i]))
				break;
		return i;
			
	case IDENT:
		for (i = 0; i < len; i++)
			if (!IS_ALNUM (s[i]))
				break;
		return i;
	}
	return 0;
}

void	shift_point (TBUF * buf, int * x0, int * y0, SEARCH_PATTERN * pat)
{
	int i;
	i = skip_first_token (buf->lines [*y0].s+*x0, buf->lines [*y0].len-*x0, &pat->l[0]);
	if (i == 0) i = 1;
	* x0 += i;
}

int	line_matches (char * s, int len, LINE * line, BOOL ignore_case, BOOL back, int * findlen)
{
	int m, i, j;

	if (line -> len > len)
		return -1;

	if (line->n == 0)	{
		* findlen = 0;
		if (line -> left_cr && line -> right_cr)
			return len == 0 ? 0 : -1;
		if (line -> left_cr)
			return 0;
		if (line -> right_cr)
			return len;
		return 0;
	}
	
	if (line -> left_cr)	{
		if (line->right_cr && line->exact && line->len != len)
			return -1;
		m = line_prefix_matches (s, len, line, ignore_case);
		if (m > 0)	{
			* findlen = m;
			return 0;
		}
		return -1;
	}

	if (line -> right_cr && line->exact)	{
		m = line_prefix_matches (s+len-line->len, line->len, line, ignore_case);
		if (m > 0)	{
			* findlen = m;
			return len-line->len;
		}
		return -1;
	}

	if (back)
		for (i = len - line->len; i >= 0; i--)	{
			m = line_prefix_matches (s+i, len-i, line, ignore_case);
			if (m > 0)	{
				* findlen = m;
				return i;
			}
		}
	else
		for (i = 0; i <= len - line->len; ++i)	{
			j = find_suspicious_prefix (s+i, len-i, line, ignore_case);
			if (j < 0) return -1;
			i += j;
			m = line_prefix_matches (s+i, len-i, line, ignore_case);
			if (m > 0)	{
				* findlen = m;
				return i;
			}
		}
	return -1;
}

/* applicable to LINE with !left_cr && !right_cr */

BOOL	single_line_pattern_search (TBUF * buf, int x0, int y0, LINE * line, int *x1, int *y1, int *x2, int *y2, BOOL ignorecase, BOOL back)
{
	int i;
	int findlen;

	if (! back)	{
		if (x0 >= buf -> lines [y0].len)	{
			y0 ++;
			x0 = 0;
		}
		if (x0 && line -> left_cr)	{
			y0++;
			x0 = 0;
		}
		for (;y0 < buf->nlines; y0++)	{
			i = line_matches (buf->lines [y0].s+x0, buf->lines [y0].len-x0, line, ignorecase, back, &findlen);
			if (i >= 0)	{
				*x1 = i+x0;
				*y1 = *y2 = y0;
				*x2 = i+x0+findlen;
				return TRUE;
			}
			x0 = 0;
		}
	} else	{
		if (x0 > buf -> lines [y0].len)
			x0 = buf -> lines [y0].len;
		if (x0 < buf->lines [y0].len && line->right_cr)	{
			y0--;
			if (y0 < 0) return FALSE;
			x0 = buf->lines [y0].len;
		}
		for (;;)	{
			i = line_matches (buf->lines [y0].s, x0, line, ignorecase, back, &findlen);
			if (i >= 0)	{
				*x1 = i;
				*y1 = *y2 = y0;
				*x2 = i+findlen;
				return TRUE;
			}
			--y0;
			if (y0 < 0) break;
			x0 = buf->lines [y0].len;
		}
	}
	return FALSE;
}

BOOL	pattern_matches (TBUF * buf, int x0, int y0, SEARCH_PATTERN * p, int *x1, int *y1, int *x2, int *y2, BOOL ignorecase, BOOL back)
{
	int x, xx, i;
	int findlen;
	
	x = line_matches (buf->lines [y0].s+x0,
			  buf->lines [y0].len-x0,
			  &p->l[0],
			  ignorecase, back, &findlen);
	if (x < 0)
		return FALSE;

	for (i = 1; i < p->n; i++)	{
		xx = line_matches (buf->lines [y0+i].s,
				   buf->lines [y0+i].len,
				   &p->l[i],
				   ignorecase, back, &findlen);
		if (xx < 0)
			return FALSE;
	}
	*y1 = y0;
	*y2 = y0 + p->n-1;
	*x1 = x;
	*x2 = xx + findlen;
	return TRUE;
}

BOOL	pattern_search (TBUF * buf, int x0, int y0, SEARCH_PATTERN * pattern, int *x1, int *y1, int *x2, int *y2, BOOL ignorecase, BOOL back, BOOL move)
{
	if (pattern->n == 1)
		return single_line_pattern_search (buf, x0, y0, &pattern->l [0], x1, y1, x2, y2, ignorecase, back);
	
	if (! back)	{
		if (x0 >= buf -> lines [y0].len)	{
			y0 ++;
			x0 = 0;
		}
		if (x0 && pattern->l[0].left_cr)	{
			y0 ++;
			x0 = 0;
		}
		for (;y0 <= buf->nlines-pattern->n; y0++)	{
			if (pattern_matches (buf, x0, y0, pattern, x1, y1, x2, y2, ignorecase, back))
				return TRUE;
			x0 = 0;
		}
	} else	{
		if (x0 > buf->lines[y0].len)
			x0 = buf->lines[y0].len;
		if (x0 != buf->lines[y0].len && pattern->l[pattern->n-1].right_cr)	{
			y0 --;
			if (y0 < 0) return FALSE;
			x0 = buf->lines[y0].len;
		}
		while (y0 >= pattern->n-1)	{
			if (pattern_matches (buf, x0, y0-pattern->n+1, pattern, x1, y1, x2, y2, ignorecase, back))
				return TRUE;
			--y0;
			if (y0 < 0) break;
			x0 = buf->lines [y0].len;
		}
	}
	return FALSE;
}

/* ----------------------- Normal search ------------------------------------ */

BOOL	tbuf_search (TBUF * buf, int x0, int y0, char * string, int *x1, int *y1, int *x2, int *y2, BOOL ignorecase, BOOL back)
{
	char * p;
	int len = strlen (string);

	if (!back)	{
		if (x0 >= buf -> lines [y0].len)	{
			x0 = 0;
			y0 ++;
		}
		for (;y0 < buf->nlines; y0++)	{
			if (ignorecase)
				p = find_istr (buf->lines [y0].s+x0, buf->lines [y0].len-x0,
						string, len);
			else
				p = find_str (buf->lines [y0].s+x0, buf->lines [y0].len-x0,
						string, len);
			if (p)	{
				*y1 = *y2 = y0;
				*x1 = p-buf->lines [y0].s;
				*x2 = *x1 + len;
				return TRUE;
			}
			x0 = 0;
		}
	} else {
		if (x0 >= buf -> lines [y0].len)
			x0 = buf -> lines [y0].len;

		for (;;)	{
			if (ignorecase)
				p = find_last_istr (buf->lines [y0].s, x0,
						string, len);
			else
				p = find_last_str (buf->lines [y0].s, x0,
						string, len);
			if (p)	{
				*y1 = *y2 = y0;
				*x1 = p-buf->lines [y0].s;
				*x2 = *x1 + len;
				return TRUE;
			}
			--y0;
			if (y0 < 0) break;
			x0 = buf->lines [y0].len;
		}
	}
	return FALSE;
}

BOOL	tbuf_word_pattern_search (TBUF * buf, int x0, int y0, SEARCH_PATTERN * pat, int *x1, int *y1, int *x2, int *y2, BOOL ignorecase, BOOL back, BOOL prefix, BOOL suffix, BOOL move)
{
	if (move && !back)
		shift_point (buf, &x0, &y0, pat);
	return pattern_search (buf, x0, y0, pat, x1, y1, x2, y2, ignorecase, back, move);
}

BOOL	tbuf_word_search (TBUF * buf, int x0, int y0, char * string, int *x1, int *y1, int *x2, int *y2, BOOL ignorecase, BOOL back, BOOL prefix, BOOL suffix, BOOL move)
{
	if (move && !back)
		x0 ++;

	if (! IS_ALPHA (string[0])) prefix = FALSE;
	if (! IS_ALPHA (string[strlen (string)-1])) suffix = FALSE;
	if (! suffix && ! prefix)
		return tbuf_search (buf, x0, y0, string, x1, y1, x2, y2, ignorecase, back);
	
	for (;;)	{
		if (! tbuf_search (buf, x0, y0, string, x1, y1, x2, y2, ignorecase, back))
			return FALSE;
		if (check_affix (buf, *x1, *y1, *x2, *y2, prefix, suffix))
			return TRUE;

		if (!back)
			if (*x2 < buf->lines [*y2].len)	{
				y0 = *y2;
				x0 = *x2 + 1;
			} else if (*y2 < buf->nlines-1)	{
				y0 = *y2 + 1;
				x0 = 0;
			} else
				return FALSE;
		else
			if (*x1)	{
				y0 = *y1;
				x0 = *x1 - 1;
			} else if (*y1)	{
				y0 = *y1 - 1;
				x0 = buf->lines [y0].len;
			} else
				return FALSE;
	}
	return TRUE;
}

BOOL	tbuf_file_word_search (char * buf, int len,
			       char * string, BOOL ignorecase, BOOL prefix, BOOL suffix)
{
	int plen = strlen (string);
	char * p, *p0;
	if (!plen) return TRUE;

	p0 = buf;
	for (;;)	{
		p = ignorecase ? find_istr (p0, len, string, plen) :
				 find_str  (p0, len, string, plen);
		if (!p) return FALSE;
		if ((!prefix || p==buf || !IS_ALPHA (p[-1])) &&
		    (!suffix || len<=plen || !IS_ALPHA (p[plen])))
			return TRUE;
		len -= p-p0+1;
		p0 = p+1;
	}
	return FALSE;
}
