
extern	void	Proj_new (int file);
extern	void	Proj_open (void);
extern	void	Proj_open_by_history (int num);
extern	void	Proj_open_by_name (char * filename);
extern	BOOL	Proj_close (void);
extern	BOOL	Is_proj_opened (void);
extern	void	Proj_edit (void);
extern	char  * Proj_name_from_history (int i);

extern	void	Proj_new_style (void);
extern	void	Proj_save_as_style (void);
extern	void	Proj_apply_style (void);
extern	void	Proj_edit_style (void);
extern	void	Proj_delete_style (void);

extern	void	Proj_enable_menu (void);

extern	void	Proj_run (void);
extern	void	Proj_debug (void);
extern	void	Proj_stop (void);

extern	void	Proj_apply_style (void);
extern	void	Proj_edit_style (void);
extern	void	Proj_delete_style (void);

extern	void	Read_proj_history  (char * IniFile);
extern	void	Write_proj_history (char * IniFile);
extern	void	Add_project_history (HMENU menu);
extern	char  * Last_project (void);
extern	void	Proj_set_vars (char * s);

extern	void	OpenOptions (void);
BOOL	Test_special_title (int file, char * title, BOOL lng);
