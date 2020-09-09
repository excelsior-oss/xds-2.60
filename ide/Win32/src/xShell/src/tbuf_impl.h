#ifndef _TBUF_IMPL_H
#define _TBUF_IMPL_H

#include "tbuf.h"
#include "undo.h"

typedef struct	_EdRegister	{
		void * ed;
		long code;
		EDIT_CALLBACKS p;
		struct _EdRegister * next;
	}
		EdRegister;

typedef struct	_bookmark {
		long	code;
		BOOL	keep;
		int	x;
		int	y;
		struct _bookmark * next;
	}
		BOOKMARK;

typedef struct	_destr	{
		void * data;
		void	(* fun) (void *);
		struct _destr * next;
	}
		DESTRUCTOR;

typedef	struct	{
		char *	s;
		int	len;		/* actual length in characters */
		int	a_len;		/* allocated size */
		int	p_len;		/* physical size (tabs expanded */
		unsigned flags;
	}
		TLINE;

#define NOT_ALLOCATED 0x0001

#define TBUF_MAGIC 'tbuf'
#define TEXT_ALLOC_DELTA 1024
#define LINE_ALLOC_DELTA 16

struct	_TBUF {
		ULONG	magic;
		TLINE *	lines;
		DWORD *	parse_info;
		int	nlines;
		int	a_nlines;
		int	max_line_length;
		int	longest_line;
		int	tab_size;
		long	cur_edit;
		int	ref_count;
		int	inv_start;
		int	inv_end;
		EOL_TYPE eol_type;
    BOOL  oem;
		BOOL	cut_spaces;
		BOOL	dirty;
		BOOL	undo_available;
		BOOL	no_line_notifications;
		EdRegister * edlist;
		BOOKMARK * bookmarks;
		UNDO_BUFFER * undo_buffer;
		UNDO_BUFFER * undo;
		DESTRUCTOR * destr_list;
	};

extern	BOOL	alpha   [256];
extern	BOOL	numeric [256];
extern	BOOL	alnum   [256];
extern	BOOL	hexnum  [256];
extern	unsigned char	tolower_table [256];
extern	unsigned char	toupper_table [256];
extern	unsigned char	toothercase_table [256];

#define IS_ALPHA(c) (alpha [(unsigned char) (c)])
#define IS_ALNUM(c) (alnum [(unsigned char) (c)])
#define IS_DIGIT(c) (numeric [(unsigned char) (c)])
#define IS_HEX(c)   (hexnum  [(unsigned char) (c)])
#define IS_WHITE(c) ((c)==' ' || (c)=='\t')
#define TOLOWER(c)    (tolower_table [(unsigned char) (c)])
#define TOUPPER(c)    (toupper_table [(unsigned char) (c)])

#endif /* _TBUF_IMPL_H */
