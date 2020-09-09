
void	Set_name (char * name, char * value);
void	Make_line (char * s, char * d, int len);
void	Make_line_general (char * s, char * d, int len, char * (* Finder) (char * name));
BOOL	Copy_and_expand_file (char * src, char * dst);

void	File_set_vars (char * s);
