/*
*
*  File: REXXCALL.H (Rx)
*
*  XDS Shell: Calling REXX interpreter, macros
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#ifndef __REXXCALL__
#define __REXXCALL__

#define RxERROR      40 // Handlers returns it when error occured
#define RXEXIT_NAME  "TraceStream"
#define REXXMONLINES 200

/*>>> Registrate new REXX function
>
>  PSZ  pszName     - function name
>  PFN  pfn         - handler address
>  BOOL fImmediate  - if TRUE, RxExecute will run it w/o calling REXX.
>                     In this case: only constant arguments alloved,
>                     the function can't return value
>  returns          - TRUE - success, FALSE - error occured.
*/
BOOL RxRegister(PSZ pszName, PFN pfn, BOOL fImmediate);


/*>>> Execute REXX program
>
>  PSZ   pszCmd (in)     - the source text to execute. When it's first word is a 'immediate' function name
>                          and fTryImmediate parameter is set TRUE, tryes to call this function w/o REXX
>                          call. Only the simple function call with constant parameters is alloved in this case.
>  PLONG plRes  (in/out) - 0 or pointer to LONG value to store result (if it is a number)
>  PSZ  *pszRes (in/out) - 0 or pointer to PSZ variable to place pointer to the returned string. No deallocation
>                          needs for this string.
>  returns               - TRUE - success, FALSE - error occured.
*/
BOOL RxExecute(PSZ pszCmd, PLONG plRes=0, PSZ *ppszRes=0, BOOL fTryImmediate = FALSE);


/*>>> Append currently defined macro
>
>  PSZ pszCmd - Appends this line to the macro definition (if the macro is been recording now).
>
*/
void RxAddToMacro(PSZ pszCmd);


/*>>> Abnormal macro recording termination
>
>  PSZ pszErrMsg - Message to display (or 0 to say nothing).
>
*/
void RxAbortMacroRecord(PSZ pszErrMsg);

/*>>> Parse line
>
>  PSZ    pszLine (in)     - line to parse
>  PSZ   *pArgv   (in/out) - returns address of the string pointers array. This strings are tokens found in the pszLine
>  LONG  *pArgl   (in/out) - returns address of array of pArgv(s) values (0 for tokens that are not numbers)
>  pszDelimiters  (in)     - symbols used to separate tokens in pszLine
>
>  returns                 - number of tokens found
>
>  NOTE: 1. The maximal argument's number is limited. (200 - default)
>        2. This function results exists only until next call will be occured!
*/
int RxParseLine(PSZ pszLine, PSZ *pArgv[], LONG *pArgl[], PSZ szDelimiters = " (),\t");


/*>>> Duplicate quotes in the string
>
> PSZ  sz (in/out) - string
>
> Duplicates all the quotes ('"') in the string.
> NOTE: The string must be enougth size!
>
*/
void RxDupQuotes(PSZ sz);

BOOL match_accel(PACCEL pAcc1, PACCEL pAcc2); // Useful function

struct MACRO
{
  MACRO    ();
 ~MACRO    ();
  void     Copy     (MACRO *pmSrc);           // *this := *pmSrc
  void     WProfile (LONG lSlot, PSZ szName);
  BOOL     RProfile (LONG lSlot, PSZ szName);
  PSZ      pszScript;      // malloc()ed macro body
  PSZ      pszComment;     // malloc()ed comment
  PSZ      pszFile;        // malloc()ed required filename template
  ACCEL    rAccel;         // key
  int      nLastEdId;      // used to recall macro editor window
  MACRO   *next;
};
typedef MACRO *PMACRO;

class MACROS
{
public:
  MACROS            ();
 ~MACROS            ();
  BOOL StartRecord    ();             // Начать запись
  BOOL DoneMacro      ();             // Готово.
  BOOL TerminateMacro (PSZ szReason); // Терминировать запись макроса. Если szReson - выдать это сообщение.
  void AddToMacro     (PSZ szCmd);    // Добавить к макросу
  BOOL PlayMacro      (PQMSG pqmsg);
  BOOL MacroDlg       ();
  BOOL RecallMacroEd  (BOOL fTestEnabled);
  void WProfile       (LONG lSlot, PSZ szName, BOOL fGlobal);
  void RProfile       (LONG lSlot, PSZ szName, BOOL fGlobal);
private:
  // Поддержка диалога макросов
  enum   LISTEV { INIT, MOVE_UP, MOVE_DOWN, LIST2LIST, EDIT, DUPLICATE, REMOVE, NEW, SELCHANGED };
  BOOL   list_event (PMACRO pmGlobal, PMACRO pmLocal, HWND hGlobal, HWND hLocal, BOOL fGlobal, LISTEV listev);
  PMACRO dup_list   (PMACRO pmList);
  void   free_list  (PMACRO pmList);

  static BOOL fMacroPlayingNow;
  PMACRO pLocalList;
  PMACRO pGlobalList;
  HWND   hMacroMon;
  static int nLastEdId;  // used to recall macro editor.

  friend MRESULT EXPENTRY dpMacroMonDlg  (HWND,ULONG,MPARAM,MPARAM);
  friend MRESULT EXPENTRY dpMacroDlg     (HWND,ULONG,MPARAM,MPARAM);
  friend MRESULT EXPENTRY dpEdMacroDlg   (HWND,ULONG,MPARAM,MPARAM);
  friend LONG    EXPENTRY Rh2MacroRecord (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);

};
typedef MACROS *PMACROS;

void RexxMon_Init    ();
BOOL RexxMon_Toggle  (int nState = -1); // 1/0 == SHOW/HIDE, else - toggle : fShow;
void RexxMon_Activate();
void RexxMon_WProfile(PSZ pszKey); // global vision: 0 slot used.
void RexxMon_RProfile(PSZ pszKey); // global vision: 0 slot used.

#endif /* ifndef __REXXCALL__ */


