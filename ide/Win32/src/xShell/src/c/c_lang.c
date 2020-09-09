#include "util.h"
#include "lang.h"

#define C_ELEM_NORMAL 0
#define C_ELEM_NUMBER 1
#define C_ELEM_KEYWORD 2
#define C_ELEM_IDENT 3

#define C_NELEMS 3

char * c_keys [] = {
			"number",
			"keyword",
			"ident"
		};

char * c_elements [] = {
			"Numbers",
			"Keywords",
			"Identifiers"
		};

BOOL	test_c_keyword (char * s, int len)
{
	if (!strncmp (s, "if", len)) return 1;
	if (!strncmp (s, "void", len)) return 1;
	if (!strncmp (s, "while", len)) return 1;
	if (!strncmp (s, "return", len)) return 1;
	if (!strncmp (s, "for", len)) return 1;
	if (!strncmp (s, "char", len)) return 1;
	if (!strncmp (s, "int", len)) return 1;
	return 0;
}

int	CALLBACK cparser (void * env, char * s, char * d, int len, int displen, DWORD data0, DWORD data)
{
	int i, i0, c;
	i = 0;
	while (i < len)	{
		if (isalpha (s[i]))	{
			i0 = i ++;
			while (isalpha (s[i])) ++i;
			if (test_c_keyword (s+i0, i-i0))
				c = C_ELEM_KEYWORD;
			else
				c = C_ELEM_IDENT;
			while (i0 < i)
				d [i0++] = c;
		} else if (isdigit (s[i]))	{
			d[i++] = C_ELEM_NUMBER;
			while (isdigit (i))
				d[i++] = C_ELEM_NUMBER;
		} else
			d[i++] = C_ELEM_NORMAL;
	}
	return i;
}

char c_sample_text[] =
	"static void swap (\n"
	"\tchar *a,\n"
	"\tchar *b,\n"
	"\tunsigned width\n"
	"\t)\n"
	"{\n"
	"\tchar tmp;\n"
	"\n"
	"\tif ( a != b )\n"
	"\t\t/* Do the swap one character at a time to avoid potential alignment\n"
	"\t\t   problems. */\n"
	"\t\twhile ( width-- ) {\n"
	"\t\t\ttmp = *a;\n"
	"\t\t\t*a++ = *b;\n"
	"\t\t\t*b++ = tmp;\n"
	"\t\t}\n"
	"}\n"
	;

char * CALLBACK get_sample_text (HINSTANCE h, void * env)
{
	return c_sample_text;
}

LANG_BLOCK c_lang_block =
	 {
		sizeof (LANG_BLOCK),
		1,
		C_NELEMS,
		0,
		c_keys,
		c_elements,
		NULL,
		cparser,
		NULL,
		NULL,
		NULL,
		get_sample_text
	};

LANG_BLOCK * c_init (void)
{
	return &c_lang_block;
}

LANG_BLOCK * CALLBACK LangInit (void)
{
	return &c_lang_block;
}

BOOL WINAPI DllMain (HINSTANCE hinstance, DWORD Reason, LPVOID Reserved)
{
    return TRUE;
}

