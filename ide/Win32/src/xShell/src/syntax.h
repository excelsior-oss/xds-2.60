#include "lang.h"


typedef struct	{
			HFONT font;
			COLORREF fg;
			COLORREF bk;
			BOOL	 oem_font;
		}
			OUTMODE;

#define NMODES 20

typedef struct	{
			char * name;
			char * ext;
			char * dll;
			HINSTANCE handle;
			DWORD	token;
			BOOL	failed;
			void *	env;
			LANG_BLOCK lang_block;
			BOOL	highlight_disabled;
			BOOL	temp_highlight_disabled;
			OUTMODE norm [NMODES];
			OUTMODE sel  [NMODES];
			OUTMODE high [NMODES];
			OUTMODE temp_norm [NMODES];
			OUTMODE temp_sel  [NMODES];
			OUTMODE temp_high [NMODES];
	}
		LANG_DEF;

#define MAX_LANG_DEFS 20

LANG_DEF langs [MAX_LANG_DEFS];
int	 n_langs;

#define TEMP_TOKEN(t) (t|0x80000000)

typedef struct	{
			char * name;
			char * ext;
			char * dll;
	}
		LANG_CFG_DATA;

#define MAX_LANG_DEFS 20

extern	void	Read_languages (void);
extern	DWORD	get_lang_token (char * file_name);
extern	LANG_BLOCK * Get_lang_block (DWORD lang_token);
extern	void *	Get_lang_env (DWORD lang_token);
extern	void	fill_edit_colors (DWORD lang_token, OUTMODE * norm_modes, OUTMODE * sel_modes, OUTMODE * high_modes);
extern	int	Get_lang_def_data (LANG_CFG_DATA * d);
extern	void	Set_lang_def_data (LANG_CFG_DATA * d, int n);
extern	void	Write_lang_colors (void);
extern	void	Configure_lang_driver (HWND hwnd, char * name, char * dll);