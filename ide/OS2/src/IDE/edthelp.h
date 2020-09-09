/*
*
*  File: edthelp.h
*
*  XDS IDE help
*
*  (c) 1997 by *FSA & NHA Inc.
*
*/

extern HWND hwndHelpInstance;

#define MAXINFFILES           16

class HELPMENU
{
public:
  HELPMENU();
 ~HELPMENU();
  //// Append:
  // <szInfList> ::= <infitem>[;<szInfList>]
  // <infitem>   ::= FULLNAME.INF | DIRECTORY | NAME.INF | NAME | ""//
  // FULLNAME.INF - добавляем этот файл
  // DIRECTORY    - все *.inf файлы из нее
  // NAME.INF     - ищем этот файл по BOOKSHELF
  // NAME         - 1) добавим ".inf" и попробуем как NAME.INF
  //                2)если есть env. variable с таким именем и ее вид *.inf{+*.inf} -
  //                  в качестве NAME.INF попробуем первый *.inf
  void Append           (PSZ szInfList);
  void Clear            ();
  void WProfile         (LONG slot, PSZ szName, BOOL fMirror);
  void RProfile         (LONG slot, PSZ szName);
private:
  PSZ  apszInfs [MAXINFFILES];
  BOOL bookshelf_search (PSZ szInf);
  BOOL add_inf_fullname (PSZ szFName, int nFile);
};

void  HelpInit           ();
void  HelpDestroy        ();
void  HelpGeneral        ();
void  HelpUsingHelp      ();
void  HelpKeys           ();
void  HelpIndex          ();
void  HelpTutorial       ();
void  HelpProductInfo    ();
void  HelpClickMenu      (int nCmd);
void  HelpWProfile       (LONG slot, PSZ szName, BOOL fMirror);
void  HelpRProfile       (LONG slot, PSZ szName);
void  DisplayHelpPanel   (ULONG idPanel);
void  DisplayContextHelp (PSZ szTopic, PSZ szExt);



