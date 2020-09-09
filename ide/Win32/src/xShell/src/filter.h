#include "winmake.h"

void	Read_filters (void);
void	Clear_filters (void);
void	Configure_filters (void);
BOOL	Edit_filter (int i);
int	New_filter (void);

BOOL	read_tool_filter (char * name);
BOOL	compile_tool_filter (void);
void	delete_tool_filter (void);
BOOL	line_matches_filter (char * src, char ** body, int * bodylen, char ** filename, int * fnamelen,
			     int * line, int * col, ERRCLASS * errclass);

int	Filter_fill_list (HWND list, BOOL combo, BOOL addnone);
int	Filter_search_name (char * name);
char *	Filter_get_name (int i);