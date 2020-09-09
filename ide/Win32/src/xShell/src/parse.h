#include "options.h"

typedef struct _ParsedProject * ParsedProject;

extern	ParsedProject EditProj;

ParsedProject Parse_project	(int file, BOOL add_errors, int * errcnt);
void	Clear_project_parsing	(ParsedProject proj);
void	Set_proj_to_edit	(ParsedProject proj, ParsedProject opt);
BOOL	Is_complex		(ParsedProject proj);
int	Save_parsed_project	(ParsedProject proj);
BOOL	Save_edited_project	(void);
void	Enum_lookups		(ParsedProject p, void (* EnumLookupProc) (char * s));
void	Enum_proj_options	(ParsedProject proj, BOOL (* enumproc) (char * line));
void	Enum_user_options	(BOOL (* enumproc) (char * line));
void	Enum_user_equations	(BOOL (* enumproc) (char * line));
char *	Get_equation		(ParsedProject proj, char * name);
BOOL	Get_option		(ParsedProject proj, char * name);
BOOL	Test_option_presence	(ParsedProject proj, char * name);
BOOL	Set_user_option		(ParsedProject proj, char * name, BOOL val);
BOOL	Set_user_equation	(ParsedProject proj, char * name, char * val);
BOOL	Delete_option		(ParsedProject proj, char * name);

void	Enum_proj_modules	(BOOL (* enumproc) (char * line));
BOOL	Add_proj_module		(ParsedProject proj, char * file);
BOOL	Edit_proj_module	(ParsedProject proj, int i, char * file);
BOOL	Delete_proj_module	(ParsedProject proj, int i);
BOOL	Replace_start_file	(int projfile, char * startfile);

void	Enum_proj_packages	(BOOL (* enumproc) (char * dir, char * mask));
char *	Get_proj_package_file	(ParsedProject proj, int i);
char *	Get_proj_package_mask	(ParsedProject proj, int i);
BOOL	Add_proj_package	(ParsedProject proj, char * file, char * mask);
BOOL	Edit_proj_package_file	(ParsedProject proj, int i, char * file);
BOOL	Edit_proj_package_mask	(ParsedProject proj, int i, char * mask);
BOOL	Delete_proj_package	(ParsedProject proj, int i);

BOOL	Apply_style		(ParsedProject proj, ParsedProject sty);

void	FillOptVal (Option * opt);
void	StoreOptVal (Option * opt);
