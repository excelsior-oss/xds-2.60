
char *	name_only (char * name);
char *	vol_only (char * name);
char *	dir_only (char * name);
char * 	ext_only (char * s);
void	change_ext (char * s, char * d, char * ext);
void	add_ext (char * s, char * d, char * ext);
BOOL	is_full_dir (char * p);
void	add_dir (char * dir, char * file, char * res);
BOOL	wildcard_name (char * s);
BOOL	match_suffix (char * p, char * suffixes);

long	prepare_cmd_line (char * s, char * d, char * exename);

BOOL	long_filenames (char * name);
BOOL	Browse_file_name (BOOL save, char * title, char * startname,
			  char * filter, char * result, int * filterpos, char * startdir);
BOOL	Browse_for_directory (char * title, char * startdir, char * result);
BOOL	Get_file_time (char * fname, FILETIME * time);
