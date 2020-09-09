/* 
	Project:	XDS IDE
	File:		macro.c 
	Purpose:	data structures and utility procedures to control set of maroes in the editor
*/

#include "macro.h"
#include <windows.h>
#include "util.h"
#include "var.h"
#include "fileutil.h"
#include "shell.h"
#include "syntax.h"  // for MAX_LANG_DEFS constant and n_langs

#define		MaxMacro			1000
#define		MacroSectionName	"macro"
#define		ToggleOnOpt			"toggleon"

#define		CR	'\015'
#define		LF	'\012'
#define		TAB	'\010'

EditMacroSet macro_sets [MAX_LANG_DEFS];
int n_macro = 0;

EditMacro empty_edit_macroes [] = {
	{ 0, 0, 0 }
};


extern	EditMacroSet * get_language_macro(long token)
{
	int i;
	for ( i=0; i<n_langs; i++ )
		if ( macro_sets[i].lang_token == token )
			return &macro_sets[i];
	return macro_sets; // provided that 0th element is a default set
} // get_language_macro

typedef struct {
	int key;
	char * name;
} KeyName;

KeyName key_names [] = {
	{ VK_CANCEL, "CANCEL" },
    { VK_BACK, "BACK" },
	{ VK_TAB, "TAB" },
	{ VK_CLEAR, "CLEAR" },
	{ VK_RETURN, "RETURN" },
	{ VK_PAUSE, "PAUSE" },
	{ VK_CAPITAL, "CAPITAL" },
	{ VK_ESCAPE, "ESCAPE" },
	{ VK_SPACE, "SPACE" },
	{ VK_PRIOR, "PRIOR" },
	{ VK_NEXT, "NEXT" },
	{ VK_END, "END" },
	{ VK_HOME, "HOME" },
	{ VK_LEFT, "LEFT" },
	{ VK_UP, "UP" },
	{ VK_RIGHT, "RIGHT" },
	{ VK_DOWN, "DOWN" },
	{ VK_SELECT, "SELECT" },
	{ VK_PRINT, "PRINT" },
	{ VK_EXECUTE, "EXECUTE" },
	{ VK_SNAPSHOT, "SNAPSHOT" },
    { VK_INSERT, "INSERT" },
	{ VK_DELETE, "DELETE" },
	{ VK_HELP, "HELP" },
	{ VK_LWIN, "LWIN" },
	{ VK_RWIN, "RWIN" },
	{ VK_APPS, "APPS" },
	{ VK_NUMPAD0, "NUMPAD0" },
	{ VK_NUMPAD1, "NUMPAD1" },
	{ VK_NUMPAD2, "NUMPAD2" },
	{ VK_NUMPAD3, "NUMPAD3" },
	{ VK_NUMPAD4, "NUMPAD4" },
	{ VK_NUMPAD5, "NUMPAD5" },
	{ VK_NUMPAD6, "NUMPAD6" },
	{ VK_NUMPAD7, "NUMPAD7" },
	{ VK_NUMPAD8, "NUMPAD8" },
	{ VK_NUMPAD9, "NUMPAD9" },
	{ VK_MULTIPLY, "MULTIPLY" },
    { VK_ADD, "ADD" },
	{ VK_SEPARATOR, "SEPARATOR" },
	{ VK_SUBTRACT, "SUBTRACT" },
    { VK_DECIMAL, "DECIMAL" },
	{ VK_DIVIDE, "DIVIDE" },
	{ VK_F1, "F1" },
	{ VK_F2, "F2" },
	{ VK_F3, "F3" },
	{ VK_F4, "F4" },
	{ VK_F5, "F5" },
	{ VK_F6, "F6" },
	{ VK_F7, "F7" },
	{ VK_F8, "F8" },
	{ VK_F9, "F9" },
	{ VK_F10, "F10" },
	{ VK_F11, "F11" },
	{ VK_F12, "F12" },
	{ VK_F13, "F13" },
	{ VK_F14, "F14" },
	{ VK_F15, "F15" },
	{ VK_F16, "F16" },
	{ VK_F17, "F17" },
	{ VK_F18, "F18" },
	{ VK_F19, "F19" },
	{ VK_F20, "F20" },
	{ VK_F21, "F21" },
	{ VK_F22, "F22" },
	{ VK_F23, "F23" },
	{ VK_F24, "F24" },
	{ VK_NUMLOCK, "NUMLOCK" },
	{ VK_SCROLL, "SCROLL" }
};

char *	LookupMacro (EditMacroSet * set, int key, int mod)
{
	int i;

	if ( !key ) return 0;

	for (i = 0; set->body[i].action; i++)
		if (set->body[i].key == key && set->body[i].mod == mod)
			return set->body[i].action;
	return 0;
}

int	scan_key(char * str, int * key, int * mod)
{
	int i,j;
	char sep, name[32];
	SHORT	scan;

	*key = 0;
	*mod = FVIRTKEY;

	i = 0;
	while ( str[i] ) {
		if ( str[i] == '\"' || str[i] == '\'' ) {
			sep = str[i++];
			if ( str[i]==0 || str[i+1] != sep ) return 0;
			scan = VkKeyScan(str[i]);
			*key = (char)scan;
			if ( *key == 0xff ) { /* VkKeyCode returned error */
				*key = str[i];
			}
			else {
				*key = scan & 0xff; /* low-order byte */
				if ( scan & 0x100 ) *mod |= FSHIFT;
				if ( scan & 0x200 ) *mod |= FCONTROL;
				if ( scan & 0x400 ) *mod |= FSHIFT;
			}
			i++; i++;
		}
		else if ( str[i] == ' ' || str[i] == ',' ) {
			i++;
		}
		else {
			j = 0;
			while ( str[i] && str[i] != ',' && str[i] != ' ' ) {
				if (j<31) 
					name[j++] = str[i];
				i++;
				name[j] = 0;
			}
			for ( j=0; j<sizeof(key_names)/sizeof(KeyName); j++ ) {
				if ( !strcmp(key_names[i].name,name) ) {
					*key = key_names[i].key;
					break;
				}
			}
			if ( !strcmp("SHIFT",name) ) *mod |= FSHIFT;
			if ( !strcmp("CONTROL",name) ) *mod |= FCONTROL;
			if ( !strcmp("ALT",name) ) *mod |= FALT;
		}
	}
	return 1;
} // scan_key

void read_file (char *name, char ** buf, long *sz)
// Opens the file that is located in the same folder with the XDS.exe but with given name
// reads all data from the file to memore and returns pointer to the buffer.
// please don't forget to free the buffer
{
	char fname [_MAX_PATH * 2+3]; //много-много, кое-где по 2000...
	FILE * f;

	*buf = NULL;

	if ( (name[0] == '\\') || (name[1] == ':') ) {
		fname[0] = 0;
		if ( strlen(name) < sizeof(fname) ) // buffer overrun protection
			strcpy(fname,name);
	} 
	else {
		// prepare full file name
		Make_line (XDSMAIN, fname, sizeof (fname));
		dir_only (fname);
		strcat (fname, "\\");
		strcat (fname, name);
	}
	f = fopen (fname, "r");

	if ( !f ) return;

	// find a size of the file
	if ( fseek ( f, 0, SEEK_END) ) {
		fclose(f);
		return;
	}
	*sz = ftell(f);
	if ( *sz<0 ) {
		fclose(f);
		return;
	}
	if ( fseek ( f, 0, SEEK_SET) ) {
		fclose(f);
		return;
	}

	*buf = malloc((*sz)+1);
	if ( !(*buf) ) {
		fclose(f);
		return;
	}
	fread((*buf),1,*sz,f);
	fclose(f);
	(*buf)[*sz]=0;
} //read_file

int	unpack_section (char * section [][2], int maxlen, char * buf, long sz)
{
	long p;
	int i;

	for ( i=0; i<maxlen; i++ ) {
		section[i][0] = section[i][1] = 0;
	}

	p = 0;
	i = 0;
	while ( p<sz ) {
		while ( (p<sz) && (buf[p] <= ' ') ) p++; // skip empty lines and leading whitespaces
		if ( p>=sz ) return i;

		section[i][0] = buf + p;
		while ( (p<sz) && (buf[p] != '=') && (buf[p] >= ' ') ) p++;  // equ name may end with '=' or endofline mark
		if ( buf[p] == '=' ) {
			buf[p++] = 0;
			while ( (p<sz) && ( (buf[p] == ' ') || (buf[p] == TAB) ) ) p++; // skip whitespaces
			section[i][1] = buf + p;
			while ( p<sz ) {
				if ( buf[p] == TAB )
					buf[p] = ' ';
				else if ( buf[p] == LF ) {
					if ( p>0 && buf[p-1] == '\\' ) { // single LF separator && line ends with backslash
						buf[p  ] = ' ';
						buf[p-1] = ' ';
					} 
					else if ( p>1 && buf[p-1] == CR && buf[p-2] == '\\' ) { // CR+LF separator && line ends with backslash
						buf[p  ] = ' ';
						buf[p-1] = ' ';
						buf[p-2] = ' ';
					}
					else { // this is the real end of equation body - mark its end and exit
						if ( p>0 && buf[p-1]==CR ) buf[p-1]=0; // if CRLF separator then CR must be discarded
						buf[p++]=0;
						break;
					}
				}
				p++;
			}
		} 
		else {
			buf[p++]=0;
			section[i][1]=0;
		}
		i++;

	}
	return i;
} // unpack_section

void read_macro_file(char * fname, EditMacro ** body, int * togle)
{
	char * buf;
	long sz;
	char * buffer[MaxMacro][2];
	int pairs, i, j;
	EditMacro * mbuf;

	*body = NULL;
	*togle = 0;

	if ( !fname || !fname[0] )
		return;

	read_file(fname, &buf, &sz);
	if (!buf) return;

	pairs = unpack_section(buffer,MaxMacro,buf,sz);
	if (pairs) {

		for (i=0,j=0; i<pairs; i++) 
			if ( strcmp(ToggleOnOpt,buffer[i][0]) ) 
				j++;
		if (!j) return;

		mbuf = malloc( (j+1) * sizeof(EditMacro) );
		for (i=0; i<j; i++) {
			mbuf[i].key = 0;
			mbuf[i].mod = 0;
			mbuf[i].action = 0;
		}

		j=0;
		for ( i=0; i<pairs; i++ ) {
			if ( !strcmp(ToggleOnOpt,buffer[i][0]) ) {
				*togle = 1;
			}
			else {
				if ( ( buffer[i][1] ) && (scan_key( buffer[i][0], &mbuf[j].key, &mbuf[j].mod) ) ) {
					mbuf[j].action = malloc( strlen(buffer[i][1])+1 );
					strcpy( mbuf[j].action, buffer[i][1] );
					j++;
				}
			}
		}
		*body = mbuf;
	}

	free(buf);
} //read_macro_file

void init_def_macro_set() 
{
	char d [300];

	GetPrivateProfileString (MacroSectionName, "default", "xds.mdf", d, sizeof (d), IniFile);
	read_macro_file( d, &macro_sets[0].body, &macro_sets[0].togle );
	if ( !macro_sets[0].body ) {
		macro_sets[0].body = empty_edit_macroes;
	}
	macro_sets[n_macro].lang_token = 0;
	n_macro++;

} //init_def_macro_set

void Read_macroes()
{
	int i;
	char d [300];

	memset(macro_sets,sizeof(macro_sets),0);
	n_macro = 0;

	init_def_macro_set();

	for ( i=1; i<n_langs; i++ ) {
		GetPrivateProfileString (MacroSectionName, langs[i].name, "", d, sizeof (d), IniFile);
		read_macro_file( d, &macro_sets[n_macro].body, &macro_sets[n_macro].togle );
		if ( macro_sets[n_macro].body ) {
			macro_sets[n_macro].lang_token = langs[i].token;
			n_macro++;
		}
	}

} // Read_macroes