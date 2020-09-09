
extern	void	Init_tools (void);
extern	void	Configure_tools (void);
extern	void	Tools_enable (void);
extern	BOOL	Tool_make (BOOL wait);
extern	char *	Tool_get_comment (int i);
extern	void	Create_tools_toolbar (void);
extern	void	Create_tools_menu (void);
extern	BOOL	Call_tool (int i);
extern	void	Update_tools_for_file (char * s);
extern	void	browse_string (HWND hwnd, int id, char * filter,
		       char * title, BOOL save_spaces, BOOL test_prefixes, BOOL test_xc);
