#ifndef OPTIONS_H
#define OPTIONS_H

typedef	enum {Equation, Boolean, Enumeration, Static, Group, Special} OptType;

#define MAXENUM 10

typedef	struct _Option	{
			OptType type;
			char name [30];
			char comment [80];
			int nalts;
			int defval;
			int curval;
			int present;
			int modified;
			int reversed;
			int new_define;
			char * defstring;
			char * curstring;
			char * values [MAXENUM];
			struct _Option * next;
		}
			Option;

typedef	struct _Node	{
			char string [80];
			int level;
			int expanded;
			Option * optlist;
			struct _Node * next;
		}
			Node;

extern	Node * NodeList;

char *  set_string_val (Option * opt, char * s);

/* values for "defval" field for Statics: */

#define STATIC_BIG 1
#define STATIC_BOLD 2
#define STATIC_ITALIC 4
#define STATIC_CENTER 8
#define STATIC_RIGHT 16

/* values for "defval" field for Groups: */

#define GROUP_RAISED 1
#define GROUP_INSET  -1
#define GROUP_BORDER 0
#define GROUP_NULL   -2

#define SPECIAL_USEROPTIONS 1
#define SPECIAL_USEREQUATIONS 2
#define SPECIAL_MODULELIST 3
#define SPECIAL_PACKAGES 4

void	clear_options (void);
void	read_options (void);

#endif