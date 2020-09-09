#ifndef LANG_H
#define LANG_H

typedef struct _parser_buffer * PARSER_BUFFER;

typedef struct	{
			PARSER_BUFFER	buf;
			int	nlines;
			DWORD * data;
			BOOL (* line_supply) (PARSER_BUFFER buf, int line, char ** s, int * len);
			void (* redraw_lines) (PARSER_BUFFER buf, int start, int fin);
	}
		PARSER_BLOCK;

#define CUR_LANG_VERSION 1

typedef struct	{
		int size;
		int version;
		int nclasses;
		int env_size;
		char ** class_keys;
		char ** class_display_names;
		void (CALLBACK * module_parser) (void * env, PARSER_BLOCK * b, int start, int fin);
		int  (CALLBACK * line_parser) (void * env, char * s, char * d, int len, int displen, DWORD data0, DWORD data);
		void (CALLBACK * read_env) (HINSTANCE h, void * env, char * name, char * IniFile);
		void (CALLBACK * free_env) (void * env);
		void (CALLBACK * configure) (HINSTANCE h, HWND parent, char * name, char * IniFile);
		char * (CALLBACK * get_sample_text) (HINSTANCE h, void * env);
	}
		LANG_BLOCK;

typedef LANG_BLOCK * (CALLBACK * LANG_INIT_PROC) (void);

#endif
