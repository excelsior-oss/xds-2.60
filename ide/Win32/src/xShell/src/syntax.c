#include "util.h"
#include "shell.h"
#include "lang.h"
#include "syntax.h"
#include "config.h"
#include "fileutil.h"

#define LANG_SECTION "languages"

/* ---------------------------------------------------------------- */

int	 n_langs = 0;
DWORD	last_token = 0;

/* ---------------------------------------------------------------- */

void	Configure_lang_driver (HWND hwnd, char * name, char * dll)
{
	HINSTANCE h;
	LANG_INIT_PROC init;
	LANG_BLOCK * block;
	char s [2000];

	h = LoadLibrary (dll);
	if (!h)	{
		MsgBox ("Configure driver", MB_OK | MB_ICONASTERISK,
			"Can't load language driver %s", dll);
		return;
	}
	init = (LANG_INIT_PROC) GetProcAddress (h, "LangInit");
	if (!init)	{
		MsgBox ("Configure driver", MB_OK | MB_ICONASTERISK,
			"Specified dll (%s) is not correct language driver", dll);
		FreeLibrary (h);
		return;
	}
	block = init ();
	if (!block)	{
		MsgBox ("Configure driver", MB_OK | MB_ICONASTERISK,
			"Specified language driver %s refused to initialize", dll);
		FreeLibrary (h);
		return;
	}
	if (block->version != CUR_LANG_VERSION)	{
		MsgBox ("Configure driver", MB_OK | MB_ICONASTERISK,
			"Specified language driver %s uses obsolete version of interface\n"
			"and can not be used with present version of IDE", dll);
		FreeLibrary (h);
		return;
	}
	if (!block -> configure)	{
		MsgBox ("Configure driver", MB_OK | MB_ICONASTERISK,
			"This language driver doesn't require\n"
			"(nor does it provide) any configuration");
		FreeLibrary (h);
		return;
	}
	sprintf (s, "%s-language", name);
	block->configure (h, hwnd, s, IniFile);
	FreeLibrary (h);
}

/* ---------------------------------------------------------------- */

BOOL	load_lang_driver (int i)
{
	LANG_INIT_PROC init;
	LANG_BLOCK * block;
	HINSTANCE h;
	int size;
	char s [1000];
	void * env;

	if (i <= 0 || i >= n_langs) return FALSE;
	if (langs [i].failed) return FALSE;

	if (!langs [i].dll) return FALSE;
	h = LoadLibrary (langs [i].dll);
	if (!h) return FALSE;
	init = (LANG_INIT_PROC) GetProcAddress (h, "LangInit");
	if (!init)	{
		FreeLibrary (h);
		langs [i].failed = 1;
		return FALSE;
	}
	block = init ();
	if (!block || block->version != CUR_LANG_VERSION)	{
		FreeLibrary (h);
		langs [i].failed = 1;
		return FALSE;
	}
	langs [i].env = NULL;
	if (block->env_size)	{
		env = malloc (block->env_size);
		if (!env)	{
			FreeLibrary (h);
			langs [i].failed = 1;
			return FALSE;
		}
		langs [i].env = env;
	}

	langs [i].handle = h;
	size = block->size;
	if (size >= sizeof (LANG_BLOCK)) size = sizeof (LANG_BLOCK);
	memset (&langs [i].lang_block, 0, sizeof (LANG_BLOCK));
	memcpy (&langs [i].lang_block, block, size);
	if (langs [i].env && block->read_env)	{
		sprintf (s, "%s-language", langs [i].name);
		block->read_env (h, langs [i].env, s, IniFile);
	}		
	return TRUE;
}

void	free_drivers (void)
{
	int i;

	for (i = 1; i < n_langs; i++)
		if (langs [i].handle)	{
			if (langs [i].lang_block.free_env && langs [i].env)
				langs [i].lang_block.free_env (langs [i].env);
			free (langs [i].env);
			langs [i].env = NULL;
			FreeLibrary (langs [i].handle);
			langs [i].handle = 0;
			memset (&langs [i].lang_block, 0, sizeof (LANG_BLOCK));
		}
}

void	load_drivers (void)
{
	int i;

	for (i = 1; i < n_langs; i++)
		load_lang_driver (i);
}

/* ---------------------------------------------------------------- */

void	color_name (char * s, char * lang, char * element, char * name)
{
	sprintf (s, "edit%s%s%s%s-%s",
		lang ? "-" : "", lang ? lang : "", 
		element ? "-" : "", element ? element:"", name);
}

COLORREF read_lang_color (char * lang, char * element, char * name)
{
	char s [200];
	color_name (s, lang, element, name);
	return Read_color (s, TRUE);
}

void	read_one_lang_colors (int i)
{
	char s [200];
	int j;
	char * lang;

	lang = i ? langs [i].name : NULL;

	color_name (s, lang, NULL, "disable-coloring");
	langs [i].highlight_disabled = Read_bool_color_option (s, FALSE);
	
	for (j = 0; j <= langs [i].lang_block.nclasses; j++)	{
		char * n = j ? langs [i].lang_block.class_keys[j-1]: NULL;
		langs [i].norm [j].fg = read_lang_color (lang, n, "foreground");
		langs [i].norm [j].bk = read_lang_color (lang, n, "background");
		langs [i].sel  [j].fg = read_lang_color (lang, n, "select-text");
		langs [i].sel  [j].bk = read_lang_color (lang, n, "selection");
		langs [i].high [j].fg = read_lang_color (lang, n, "highlight-text");
		langs [i].high [j].bk = read_lang_color (lang, n, "highlight");
	}
}

void	read_lang_colors ()
{
	int i;

	for (i = 0; i < n_langs; i++)
		read_one_lang_colors (i);
}

void	write_lang_color (COLORREF color, char * lang, char * element, char * name)
{
	char s [200];
	color_name (s, lang, element, name);
	Write_color (color, s);
}

void	write_one_lang_colors (int i)
{
	char s [200];
	int j;
	char * lang;

	lang = i ? langs [i].name : NULL;

	color_name (s, lang, NULL, "disable-coloring");
	Write_bool_color_option (s, langs [i].highlight_disabled);

	for (j = 0; j <= langs [i].lang_block.nclasses; j++)	{
		char * n = j ? langs [i].lang_block.class_keys[j-1]: NULL;
		write_lang_color (langs [i].norm [j].fg, lang, n, "foreground");
		write_lang_color (langs [i].norm [j].bk, lang, n, "background");
		write_lang_color (langs [i].sel  [j].fg, lang, n, "select-text");
		write_lang_color (langs [i].sel  [j].bk, lang, n, "selection");
		write_lang_color (langs [i].high [j].fg, lang, n, "highlight-text");
		write_lang_color (langs [i].high [j].bk, lang, n, "highlight");
	}
}

void	Write_lang_colors (void)
{
	int i;

	for (i = 0; i < n_langs; i++)
		write_one_lang_colors (i);
}

/* ---------------------------------------------------------------- */

void	write_lang (int i)
{
	char s [300];
	sprintf (s, "%s-driver", langs[i].name);
	WritePrivateProfileString (LANG_SECTION, s, langs [i].dll? langs [i].dll : "", IniFile);
	sprintf (s, "%s-extensions", langs[i].name);
	WritePrivateProfileString (LANG_SECTION, s, langs [i].ext? langs [i].ext : "", IniFile);
}

void	write_lang_ini_file (void)
{
	int i;
	char s [20];

	WritePrivateProfileSection (LANG_SECTION, "\0", IniFile);
	for (i = 1; i < n_langs; i++)	{
		wsprintf (s, "lang%d", i);
		WritePrivateProfileString (LANG_SECTION, s, langs [i].name, IniFile);
	}

	for (i = 1; i < n_langs; i++)
		write_lang (i);
}

BOOL	read_lang (int i)
{
	char s [300];
	char d [300];
	wsprintf (s, "lang%d", i);
	GetPrivateProfileString (LANG_SECTION, s, "", d, sizeof (d), IniFile);
	if (!d[0]) return 0;
	langs [i].name = alloc_copy (d);
	if (!langs [i].name) return FALSE;

	sprintf (s, "%s-driver", langs[i].name);
	GetPrivateProfileString (LANG_SECTION, s, "", d, sizeof (d), IniFile);
	langs [i].dll = alloc_copy (d);
	
	sprintf (s, "%s-extensions", langs[i].name);
	GetPrivateProfileString (LANG_SECTION, s, "", d, sizeof (d), IniFile);
	langs [i].ext = alloc_copy (d);

	langs [i].token = ++ last_token;
	return TRUE;
}

/* ---------------------------------------------------------------- */
char * def_class_names [] = {"default"};

char def_sample_text [] = "This is the sample of a text\n"
			  "that is not recognized as belonging\n"
			  "to any programming language.\n"
			  "\n"
			  "Any coloring and formatting applied to\n"
			  "this text becomes default\n"
			  "for all edit windows";

char * CALLBACK get_default_sample_text (HINSTANCE h, void * env)
{
	return def_sample_text;
}

void	init_def_lang (void)
{
	langs [0].name = "default";
	langs [0].lang_block.nclasses = 0;
	langs [0].lang_block.env_size = 0;
	langs [0].lang_block.class_display_names = def_class_names;
	langs [0].lang_block.get_sample_text = get_default_sample_text;
}

void	Read_languages (void)
{
	memset (langs, 0, sizeof (langs));
	init_def_lang ();
	for (n_langs = 1; read_lang (n_langs); n_langs++);
	load_drivers ();
	read_lang_colors ();
}

int	Get_lang_def_data (LANG_CFG_DATA * d)
{
	int i;

	for (i = 1; i < n_langs; i++)	{
		d[i-1].name = alloc_copy (langs [i].name);
		d[i-1].ext  = alloc_copy (langs [i].ext);
		d[i-1].dll  = alloc_copy (langs [i].dll);
		if (!d[i-1].name || !d[i-1].ext || !d[i-1].dll)
			return -1;
	}
	return n_langs-1;
}

void	Set_lang_def_data (LANG_CFG_DATA * d, int n)
{
	int i;
	free_drivers ();
	for (i = 1; i < n_langs; i++)	{
		free (langs[i].name);
		free (langs[i].ext);
		free (langs[i].dll);
	}
	memset (langs+1, 0, sizeof (langs)-sizeof (langs[0]));
	for (i = 0; i < n; i++)	{
		langs [i+1].name = d[i].name;
		langs [i+1].ext  = d[i].ext;
		langs [i+1].dll  = d[i].dll;
		d[i].name = d[i].ext = d[i].dll = NULL;
		langs [i+1].token = ++ last_token;
	}
	n_langs = n+1;
	write_lang_ini_file ();
	load_drivers ();
	read_lang_colors ();
}

int	find_ext (char * filename)
{
	int i;
	
	if (!filename || !filename[0])
		return -1;
	filename = ext_only (filename);
	for (i = 1; i < n_langs; i++)
		if (match_suffix (filename, langs [i].ext))
			return i;
	return 0;
}

int	find_token (DWORD token)
{
	int i;
	if (!token) return 0;
	token &= 0x7FFFFFFF;
	for (i = 1; i < n_langs; i++)
		if (langs [i].token == token)
			return i;
	return 0;
}

/* ---------------------------------------------------------------- */

DWORD	get_lang_token (char * filename)
{
	int i = find_ext (filename);
	return langs [i].token;
}

LANG_BLOCK * Get_lang_block (DWORD lang_token)
{
	int i = find_token (lang_token);
	return &langs [i].lang_block;
}

void *	Get_lang_env (DWORD lang_token)
{
	int i = find_token (lang_token);
	return langs [i].env;
}

void	fill_color (OUTMODE * d, OUTMODE * s)
{
	if (s->fg != DEFAULT_COLOR) d->fg = s->fg;
	if (s->bk != DEFAULT_COLOR) d->bk = s->bk;
}

void	fill_norm_colors (int i, OUTMODE * norm_modes, OUTMODE * sel_modes, OUTMODE * high_modes)
{
	OUTMODE n, h, s;
	LOGFONT logf;
	int j;
	
	n.font = s.font = h.font = EditFont;

	GetObject (EditFont, sizeof (LOGFONT), &logf);
	n.oem_font = h.oem_font = s.oem_font = logf.lfCharSet == OEM_CHARSET;

	n.fg = AbsColor (langs [0].norm [0].fg, AbsSysColor (WndForeColor, COLOR_WINDOWTEXT));
	n.bk = AbsColor (langs [0].norm [0].bk, AbsSysColor (WndBackColor, COLOR_WINDOW));
	
	s.fg = AbsSysColor (langs [0].sel [0].fg, COLOR_HIGHLIGHTTEXT);
	s.bk = AbsSysColor (langs [0].sel [0].bk, COLOR_HIGHLIGHT);

	h.fg = AbsColor (langs [0].high [0].fg, n.fg);
	h.bk = AbsColor (langs [0].high [0].bk, light_color (s.bk));

	if (! langs [0].highlight_disabled && ! langs [i].highlight_disabled)	{
		fill_color (&n, &langs [i].norm[0]);
		fill_color (&s, &langs [i].sel [0]);
		fill_color (&h, &langs [i].high[0]);
	}

	for (j = 0; j < NMODES; j++)	{
		norm_modes [j] = n;
		sel_modes  [j] = s;
		high_modes [j] = h;
	}

	if (langs [0].highlight_disabled || langs [i].highlight_disabled)
		return;

	for (j = 1; j <= langs [i].lang_block.nclasses; j++)	{
		fill_color (&norm_modes [j], &langs [i].norm [j]);
		fill_color (&sel_modes  [j], &langs [i].sel  [j]);
		fill_color (&high_modes [j], &langs [i].high [j]);
	}
}

void	fill_temp_colors (int i, OUTMODE * norm_modes, OUTMODE * sel_modes, OUTMODE * high_modes)
{
	OUTMODE n, h, s;
	LOGFONT logf;
	int j;
	
	n.font = s.font = h.font = temp_EditFont;

	GetObject (EditFont, sizeof (LOGFONT), &logf);
	n.oem_font = h.oem_font = s.oem_font = logf.lfCharSet == OEM_CHARSET;

	n.fg = AbsColor (langs [0].temp_norm [0].fg, AbsSysColor (temp_WndForeColor, COLOR_WINDOWTEXT));
	n.bk = AbsColor (langs [0].temp_norm [0].bk, AbsSysColor (temp_WndBackColor, COLOR_WINDOW));
	
	s.fg = AbsSysColor (langs [0].temp_sel [0].fg, COLOR_HIGHLIGHTTEXT);
	s.bk = AbsSysColor (langs [0].temp_sel [0].bk, COLOR_HIGHLIGHT);

	h.fg = AbsColor (langs [0].temp_high [0].fg, n.fg);
	h.bk = AbsColor (langs [0].temp_high [0].bk, light_color (s.bk));
	
	if (! langs [0].temp_highlight_disabled && ! langs [i].temp_highlight_disabled)	{
		fill_color (&n, &langs [i].temp_norm[0]);
		fill_color (&s, &langs [i].temp_sel [0]);
		fill_color (&h, &langs [i].temp_high[0]);
	}

	for (j = 0; j < NMODES; j++)	{
		norm_modes [j] = n;
		sel_modes  [j] = s;
		high_modes [j] = h;
	}

	if (langs [0].temp_highlight_disabled || langs [i].temp_highlight_disabled)
		return;

	for (j = 1; j <= langs [i].lang_block.nclasses; j++)	{
		fill_color (&norm_modes [j], &langs [i].temp_norm [j]);
		fill_color (&sel_modes  [j], &langs [i].temp_sel  [j]);
		fill_color (&high_modes [j], &langs [i].temp_high [j]);
	}
}

void	fill_edit_colors (DWORD lang_token, OUTMODE * norm_modes, OUTMODE * sel_modes, OUTMODE * high_modes)
{
	int i = find_token (lang_token);
	if (lang_token & 0x80000000)
		fill_temp_colors (i, norm_modes, sel_modes, high_modes);
	else
		fill_norm_colors (i, norm_modes, sel_modes, high_modes);
}
