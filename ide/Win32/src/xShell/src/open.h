
extern	char	EditCurDir [];
extern  int	edit_filter_index;

void	Insert_into_file_history (int file);
void	Read_file_history (char * IniFile);
void	Add_file_history (HMENU menu);
void	Write_file_history (char * IniFile);
void	Write_open_history (char * IniFile);

void	Read_open_history (char * IniFile);

HWND	Open_by_name	(char * name, int how);
void	Open_by_history (int num);
void	Open		(void);
void	Open_null_project (void);

void	MoreFiles (void);

char *	File_name_from_history (int i);
int	Ask_save_file_name (int file);