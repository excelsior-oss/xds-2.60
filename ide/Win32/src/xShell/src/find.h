#ifndef _FIND_H
#define _FIND_H

#include "tbuf.h"

typedef struct _SEARCH_PATTERN SEARCH_PATTERN;

BOOL	tbuf_search		(TBUF * buf, int x0, int y0,
				 char * string,
				 int *x1, int *y1, int *x2, int *y2,
				 BOOL ignorecase, BOOL back);
BOOL	tbuf_word_search	(TBUF * buf, int x0, int y0,
				 char * string,
				 int *x1, int *y1, int *x2, int *y2,
				 BOOL ignorecase, BOOL back, BOOL prefix, BOOL suffix, BOOL move);
BOOL	tbuf_file_word_search (char * buf, int len,
			       char * string, BOOL ignorecase, BOOL prefix, BOOL suffix);

BOOL	tbuf_word_pattern_search (TBUF * buf, int x0, int y0, SEARCH_PATTERN * pat,
				  int *x1, int *y1, int *x2, int *y2,
				  BOOL ignorecase, BOOL back, BOOL prefix, BOOL suffix, BOOL move);

SEARCH_PATTERN * compile_pattern (char * s);
void free_pattern (SEARCH_PATTERN * p);

BOOL	pattern_search (TBUF * buf, int x0, int y0, SEARCH_PATTERN * pattern, int *x1, int *y1, int *x2, int *y2, BOOL ignorecase, BOOL back, BOOL move);

#endif /* _FIND_H */
