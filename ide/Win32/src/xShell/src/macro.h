
typedef struct {
	int	key;		// key+mod define keyboard event that selects this macro
	int	mod;
	char * action;	// macro string
} EditMacro;

typedef struct {
	long lang_token;
	int togle;			
	EditMacro * body;
} EditMacroSet;

extern	EditMacroSet * get_language_macro(long token);  // returns macroset corresponding to a given language 
														// token have same meaning as in syntax.h

extern	char *	LookupMacro (EditMacroSet * set, int key, int mod); // returns a macro definition string or NULL if none
extern	void Read_macroes();

