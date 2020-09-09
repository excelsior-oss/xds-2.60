#include "tbuf.h"

typedef	struct	{
		HANDLE fhandle;
		HANDLE fmap;
		char * ptr;
		unsigned long len;
	}
		FILEMAP;

BOOL	Load_file (char * name, FILEMAP * map, BOOL copy);
void	Unload_file (FILEMAP * map);
BOOL	Test_file_exists (char * name);
BOOL	Test_file_readable (char * name);
BOOL    Test_overwrite (char * name, BOOL prompt);

BOOL	Create_dirs (int ndirs, char ** dirlist);

TBUF *	Read_file_to_buf (int file, BOOL copy, unsigned long * len);
BOOL	Write_file_from_buf (TBUF * buf, int file, BOOL prompt_overwrite);

TBUF *	Find_or_read_file	(int file);
int	Put_file_start		(void);
int	Put_file		(char * buf, int len);
int	Put_file_fin		(int file, BOOL test_overwrite);
