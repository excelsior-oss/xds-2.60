///////////////////// File: edthelp.cpp //////////////////////////////////
#define  INCL_WIN
#define  INCL_DOS
#define  INCL_WINHELP

#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "xdsbase.h"

/*
 *  Global variables
 */
HWND     hwndHelpInstance = 0;
PSZ      apszInfFiles [MAXINFFILES];
HELPMENU hmHelpMenu; // class instance

/********************************************************/

void  HelpInit()
{
  LONG EXPENTRY   RhHelpAppendMenu (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  LONG EXPENTRY   RhHelpClearMenu  (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  LONG EXPENTRY   RhHelpLoad       (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  HELPINIT        hini;
  char            szPath[CCHMAXPATH];
  static BOOL     f1st = TRUE;

  memset(&hini,0,sizeof(hini));
  hini.cb                       = sizeof(HELPINIT);
  hini.phtHelpTable             = (PHELPTABLE)MAKELONG(SHELL_HELP_TABLE, 0xFFFF);
  hini.pszHelpWindowTitle       = "XDS help";
  hini.fShowPanelId             = CMIC_HIDE_PANEL_ID;
  strcpy(szPath,szHomePath);
  strcat(szPath,"\\xds.hlp");
  hini.pszHelpLibraryName       = szPath;

  if(hwndHelpInstance) WinDestroyHelpInstance(hwndHelpInstance);
  hwndHelpInstance = WinCreateHelpInstance(hAB, &hini);

  BOOL fOk = hwndHelpInstance && !hini.ulReturnCode &&
             WinAssociateHelpInstance(hwndHelpInstance, pMDI->QueryWindow(MDIQW_MAINFRAME));
  if (!fOk && hwndHelpInstance)
  {
    WinDestroyHelpInstance(hwndHelpInstance);
    hwndHelpInstance = 0;
  }
  if (!fOk)
  {
    char sz[100];
    strcpy(sz,"Failed to load help manager.");
    if (hini.ulReturnCode) sprintf( sz+strlen(sz), " Error code = %x", hini.ulReturnCode);
    MessageBox(hMDI0Client, "Help initialization", MB_ERROR|MB_OK|MB_MOVEABLE,sz);
  }
  if (f1st)
  {
    memset(apszInfFiles,0,sizeof(apszInfFiles));
    RxRegister("HelpAppendMenu", (PFN)RhHelpAppendMenu,FALSE);
    RxRegister("HelpClearMenu",  (PFN)RhHelpClearMenu, FALSE);
    RxRegister("HelpLoad",       (PFN)RhHelpLoad,      FALSE);
    f1st = FALSE;
  }
}   /* End of HelpInit */

/*>>> HelpAppendMenu(inflist [,CLEAROLD]):"ok" | "";
>
*/
LONG   EXPENTRY RhHelpAppendMenu(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  strcpy(retstr->strptr,"ok");
  retstr->strlength = 2;
  if (argc<1 || argc>2) return RxERROR;
  if (argc==2)
    if (!sf_stricmp(argv[1].strptr,"CLEAROLD"))  hmHelpMenu.Clear();
    else                                         return RxERROR;
  hmHelpMenu.Append(argv[0].strptr);
  return 0;
}

/*>>> HelpClearMenu()
>
*/
LONG   EXPENTRY RhHelpClearMenu(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  hmHelpMenu.Clear();
  return 0;
}


/*>>> HelpLoad('LOAD' | 'UNLOAD')
>
*/
LONG   EXPENTRY RhHelpLoad(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  HelpDestroy();
  if (argc>=1 && !strcmp(argv[0].strptr,"LOAD"))
  {
    HelpInit();
  }
  return 0;
}


void  HelpDestroy()
{
  if(hwndHelpInstance)  WinDestroyHelpInstance(hwndHelpInstance);
  hwndHelpInstance = 0;
}


void  HelpGeneral()
{
    /* this just displays the system extended help panel */
   if(hwndHelpInstance)
      if(NULL != WinSendMsg(hwndHelpInstance, HM_EXT_HELP,
                            (MPARAM)NULL, (MPARAM)NULL))
         MessageBox(hMDI0Client, "Help error", MB_ERROR|MB_OK|MB_MOVEABLE,"Failed to display help panel");
}


void  HelpUsingHelp()
{
   /* this just displays the system help for help panel */
   if(hwndHelpInstance)
      if(NULL != WinSendMsg(hwndHelpInstance, HM_DISPLAY_HELP,
                            (MPARAM)NULL, (MPARAM)NULL))
         MessageBox(hMDI0Client, "Help error", MB_ERROR|MB_OK|MB_MOVEABLE,"Failed to display help panel");
}


void  HelpKeys()
{
   /* this just displays the system keys help panel */
   if(hwndHelpInstance)
/*
      if(NULL != WinSendMsg(hwndHelpInstance, HM_KEYS_HELP,
                            (MPARAM)NULL, (MPARAM)NULL))
         MessageBox(hMDI0Client, "Help error", MB_ERROR|MB_OK|MB_MOVEABLE,"Failed to display help panel");
*/
      DisplayHelpPanel(PANEL_HELPKEYS);
}


void HelpIndex()
{
   /* this just displays the system help index panel */
   if(hwndHelpInstance)
      if(NULL != WinSendMsg(hwndHelpInstance, HM_HELP_INDEX,
                             (MPARAM)NULL, (MPARAM)NULL))
         MessageBox(hMDI0Client, "Help error", MB_ERROR|MB_OK|MB_MOVEABLE,"Failed to display help panel");
}


void  HelpTutorial()
{
   MessageBox(hMDI0Client, "Help error", MB_ERROR|MB_OK|MB_MOVEABLE,"Sorry. No tutorial is now.");
}


    MRESULT EXPENTRY dpAbout(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
    {
      switch(ulMsg){
      case WM_INITDLG:
      {
        HWND hBmp = WinWindowFromID(hWnd,IDD_ABOUTBMP);
        SWP  swp;
        CenterChildWindow(hWnd);
        WinQueryWindowPos(hBmp,&swp);
        //--- Center the bitmap related to it's current position
        swp.x = swp.x - swp.cx/2;
        swp.y = swp.y - swp.cy/2;
        WinSetWindowPos(hBmp,0,swp.x,swp.y,0,0,SWP_MOVE);
        WinSetActiveWindow(HWND_DESKTOP,hWnd);
        return MRESULT(TRUE);
      }
      case WM_COMMAND:
        if (SHORT1FROMMP(m1)==DID_OK){
          WinDismissDlg(hWnd,TRUE);
          break;
        }
      default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
      }
      return 0;
    }
void  HelpProductInfo()
{
   /* display the Product Information dialog. */
   WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpAbout,NULLHANDLE,IDD_ABOUT,0);
}

void  HelpClickMenu(int nCmd)
{
  char          szArg       [CCHMAXPATH+100];
  char          szLoadError [CCHMAXPATH];
  RESULTCODES   rcodes      = {0};
  nCmd -= IDM_HELP_INFFILE0;
  if (nCmd<0 || nCmd>=MAXINFFILES || !apszInfFiles[nCmd]) return;
  sprintf (szArg,"view.exe%c \"%s\"%c", '\0', apszInfFiles[nCmd], '\0');
  if (DosExecPgm(szLoadError,sizeof(szLoadError),EXEC_ASYNC,szArg,0,&rcodes,"view.exe"))
    MessageBox(hMDI0Client, "Help error", MB_ERROR|MB_OK|MB_MOVEABLE,"Can't execute:\n  view ", apszInfFiles[nCmd]);
}

void  HelpWProfile(LONG slot, PSZ szName, BOOL fMirror) {hmHelpMenu.WProfile(slot,szName,fMirror);}
void  HelpRProfile(LONG slot, PSZ szName)               {hmHelpMenu.RProfile(slot,szName);}

void DisplayHelpPanel(ULONG idPanel)
{
   if(hwndHelpInstance)
      if(NULL != WinSendMsg(hwndHelpInstance,
                            HM_DISPLAY_HELP,
                            MPFROMLONG(idPanel),
                            MPFROMSHORT(HM_RESOURCEID)))
         MessageBox(hMDI0Client, "Help error", MB_ERROR|MB_OK|MB_MOVEABLE,"Failed to display help panel");
}   /* End of DisplayHelpPanel   */




//--------------------- DispalyContextHelp... ----------------------//

  static PSZ skipsp(PSZ pch)
  {// в отличие от sf_skipspaces не скипает CR LF
    while(*pch==' ' || *pch=='\t') pch++;
    return pch;
  }
  static BOOL cmp_tpl(PSZ pszWord, int nWordLen, PSZ pszTpl)
  {
    int nTplLen = strlen(pszTpl);
    if (!nTplLen) return FALSE;
    BOOL fStar = (pszTpl[nTplLen-1]=='*');
    if (fStar ? (nTplLen--, nWordLen<nTplLen) : (nWordLen!=nTplLen)) return FALSE;
    return (!nTplLen || !sf_strnicmp(pszWord,pszTpl,nTplLen));
  }
  #define TOKEN_EOL          0  // all the EOL(s) skipped
  #define TOKEN_EOF          1  //
  #define TOKEN_LBRACKET     2  // (
  #define TOKEN_RBRACKET     2  // )
  #define TOKEN_COMMA        2  // ,
  #define TOKEN_COLON        2  // :
  #define TOKEN_WORD         3  // unrecognized chars up to delimiter
  static PSZ get_token(PSZ *pszSrc,PSZ pchTop, PULONG pulTokType)
  {// TOKEN_WORD: returns its text, no free() needs!
    static PSZ pszMalloc = 0;
    PSZ        psz1, psz = skipsp(*pszSrc);
    CHAR       ch        = *psz;
    free(pszMalloc);
    pszMalloc = 0;
    // Comment(s)?
  skip_comment:
    if (ch=='/' && psz[1]=='*')
    {
      PSZ psz1 = psz+2;
      while(1)
      {
        if (!(psz1 = (PSZ)memchr(psz1,'*',pchTop-psz1)))
        {
          *pszSrc     = pchTop;
          *pulTokType = TOKEN_EOF;
          return 0;
        }
        else if(*(++psz1)=='/')
        {
          psz = skipsp(psz1+1);
          ch = *psz;
          goto skip_comment;
        }
      }
    }
    else if (ch=='/' && psz[1]=='/')
    {
      if (psz1 = strpbrk(psz,"\r\n"))
      {
        *pszSrc     = sf_skipspaces(psz1);
        *pulTokType = TOKEN_EOL;
        return 0;
      }
      else
      {
        *pszSrc     = strchr(psz,'\0');
        *pulTokType = TOKEN_EOF;
        return 0;
      }
    }
    // Analyse:
    if (!ch)
    {
      *pszSrc     = psz;
      *pulTokType = TOKEN_EOF;
    }
    else if (ch==CR || ch==LF)
    {
      *pszSrc     = sf_skipspaces(psz);
      *pulTokType = TOKEN_EOL;
    }
    else if (ch == '(')
    {
      *pszSrc = skipsp(psz+1);
      *pulTokType = TOKEN_LBRACKET;
    }
    else if (ch == ')')
    {
      *pszSrc = skipsp(psz+1);
      *pulTokType = TOKEN_RBRACKET;
    }
    else if (ch == ',')
    {
      *pszSrc = skipsp(psz+1);
      *pulTokType = TOKEN_COMMA;
    }
    else if (ch == ':')
    {
      *pszSrc = skipsp(psz+1);
      *pulTokType = TOKEN_COLON;
    }
    else
    {
      PSZ pchE;
      (pchE = strpbrk(psz,"\r\n,: ")) || (pchE = pchTop);
      *pszSrc   = skipsp(pchE);
      if (!(pszMalloc = (PSZ)malloc(pchE-psz+1)))
        *pulTokType = TOKEN_EOF;
      else
      {
        strncpy(pszMalloc,psz,pchE-psz);
        pszMalloc[psz,pchE-psz] = 0;
        *pulTokType = TOKEN_WORD;
        return pszMalloc;
      }
    }
    return 0;
  }
  static PSZ skip2EOL(PSZ pszSrc)
  {
    PSZ psz;
    (psz = strpbrk(pszSrc,"\r\n")) || (psz = strchr(pszSrc,'\0'));
    psz = sf_skipspaces(psz);
    return psz;
  }
static BOOL ndx_scan_file(PSZ pFileBuf, PSZ szTopic, PSZ szExt)
{ // Оно скипает все что непонятно, если есть ошибки - может непонять больше чем следует :)
  PSZ   pszTok;
  ULONG ulTok;
  PSZ   pchTop = pFileBuf+strlen(pFileBuf);
  int   nTopic = strlen(szTopic);
  if (!nTopic) return FALSE;
  while(pszTok = get_token(&pFileBuf,pchTop,&ulTok), ulTok != TOKEN_EOF)
  {
    if (ulTok==TOKEN_EOL ) continue;
    if (ulTok==TOKEN_WORD)
    {
      if (!sf_stricmp(pszTok,"EXTENSIONS"))
      {
        BOOL fExtOk = TRUE;
        int  nExt   = strlen(szExt);
        pszTok      = get_token(&pFileBuf,pchTop,&ulTok);
        if (ulTok == TOKEN_COLON)
        {
          while(pszTok = get_token(&pFileBuf,pchTop,&ulTok), ulTok == TOKEN_WORD)
            if (fExtOk = (!nExt || cmp_tpl(szExt,nExt,pszTok))) break;
          if (!fExtOk) return FALSE;
        }
        if (!(pFileBuf = strpbrk(pFileBuf,"\r\n"))) return FALSE;
      }
    }
    else if (ulTok==TOKEN_LBRACKET)
    { //(@zzz*, view manual.inf ~)CRLF
      pszTok      = get_token(&pFileBuf,pchTop,&ulTok);
      //(zzz*@, view manual.inf ~)CRLF
      if (ulTok==TOKEN_WORD)
      {
        int nTokLen = strlen(pszTok);
        if (nTokLen &&
            (pszTok[nTokLen-1]=='*') ? (nTokLen--, nTopic>=nTokLen) : (nTopic==nTokLen))
        {
          if (!sf_strnicmp(szTopic,pszTok,nTokLen))
          {
            pszTok = get_token(&pFileBuf,pchTop,&ulTok);
            //(zzz*, @view manual.inf ~)CRLF
            if (ulTok==TOKEN_COMMA)
            {
              PSZ pchE;
              {
                int nested = 1;
                for (pchE = pFileBuf; *pchE && *pchE!=CR && *pchE!=LF; pchE++)
                  if      (*pchE=='(')              nested++;
                  else if (*pchE==')' && !--nested) break;
              }
              if (*pchE==')')
              {
                //Now: pFileBuf->  "view manual.inf ~)"  <-pchE
                PSZ pch1;
                int i,nlen = pchE-pFileBuf+1;
                for (pch1=pFileBuf; pch1<pchE; pch1++)
                  if (*pch1=='~') nlen+=nTopic;
                PSZ pszCmd = (PSZ)malloc(nlen);
                if (!pszCmd) return FALSE;
                for (i=0,pch1=pFileBuf; pch1<pchE; pch1++)
                  if (*pch1=='~')
                  {
                    strcpy(pszCmd+i,szTopic);
                    i += nTopic;
                  }
                  else
                    pszCmd[i++] = *pch1;
                pszCmd[i] = 0;
                // Now: pszCmd == malloc()ed "view manual.inf topic"
                char          szArg       [CCHMAXPATH+100];
                char          szLoadError [CCHMAXPATH];
                RESULTCODES   rcodes      = {0};
                sprintf (szArg,"cmd.exe%c/C \"%s\"%c", '\0', pszCmd, '\0');
                if (DosExecPgm(szLoadError,sizeof(szLoadError),EXEC_ASYNC,szArg,0,&rcodes,"cmd.exe"))
                  MessageBox(hMDI0Client, "Help error", MB_ERROR|MB_OK|MB_MOVEABLE,"Can't execute:\n ", pszCmd);
                free(pszCmd);
                return TRUE;
              }
            }
          }
        }
      }
      if (!(pFileBuf = strpbrk(pFileBuf,")\r\n"))) return FALSE;
      pFileBuf++;
    } // (ulTok==TOKEN_LBRACKET)
  } // while(...)
  return FALSE;
}
BOOL ndx_1file(PSZ szFull, PSZ szTopic, PSZ szExt)
{
  HFILE hf;
  ULONG ul,fsize;
  PSZ   pFileBuf = 0;
  BOOL  fOk      = FALSE;
  DosError(FERR_DISABLEHARDERR);
  if (!DosOpen(szFull,&hf,&ul,0,FILE_NORMAL,FILE_OPEN,
              OPEN_FLAGS_NO_CACHE |OPEN_FLAGS_SEQUENTIAL|
              OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE, (PEAOP2)0))
  {
    DosSetFilePtr(hf,0L,FILE_END,  &fsize);
    DosSetFilePtr(hf,0L,FILE_BEGIN,&ul);
    if ((pFileBuf = (PSZ)malloc(fsize+1)) && !DosRead(hf,(PVOID)pFileBuf,fsize,&ul))
    {
      pFileBuf[ul] = 0;
      fOk          = ndx_scan_file(pFileBuf, szTopic, szExt);
    }
    DosClose(hf);
    free(pFileBuf);
  }
  DosError(FERR_ENABLEHARDERR);
  return fOk;
}
BOOL ndx_help(PSZ szFName, PSZ szTopic, PSZ szExt)
{
  // Имя с путями - смотрим в нем,
  // иначе ищем szFName в .;EPMPATH;DPATH;PATH
  char  szFull[CCHMAXPATH];
  PSZ   pszEnvVar = "";
  if (strpbrk(szFName,"\\/:"))
    return (FName2Full(szFull,szFName,FALSE) && ndx_1file(szFull,szTopic, szExt));

  if (!DosScanEnv("EPMPATH",(PCSZ*)&pszEnvVar) &&
      !DosSearchPath(SEARCH_IGNORENETERRS|SEARCH_CUR_DIRECTORY,
       pszEnvVar,szFName,szFull,sizeof(szFull)))
    return ndx_1file(szFull,szTopic, szExt);

  if (!DosScanEnv("DPATH",(PCSZ*)&pszEnvVar) &&
      !DosSearchPath(SEARCH_IGNORENETERRS,pszEnvVar,szFName,szFull,sizeof(szFull)))
    return ndx_1file(szFull,szTopic, szExt);

  if (!DosScanEnv("PATH",(PCSZ*)&pszEnvVar) &&
      !DosSearchPath(SEARCH_IGNORENETERRS,pszEnvVar,szFName,szFull,sizeof(szFull)))
    return ndx_1file(szFull,szTopic, szExt);

  return FALSE;
}

void DisplayContextHelp(PSZ szTopic, PSZ szExt)
{
  char szNdx[CCHMAXPATH];
  PSZ  pszHelpndx = "";
  sprintf(szNdx,"%s\\xds.ndx",szHomePath);
  if (ndx_help(szNdx,szTopic, szExt)) return;
  if (!DosScanEnv("HELPNDX",(PCSZ*)&pszHelpndx))
  {
    PSZ pchE, pch=sf_skipspaces(pszHelpndx);
    while(*pch)
    {
      (pchE = strpbrk(pch,"+ \t")) || (pchE = strchr(pch,'\0'));
      strncpy(szNdx,pch,pchE-pch);
      szNdx[pchE-pch] = 0;
      pch = sf_skipspaces(pchE);
      if (*pch=='+') pch = sf_skipspaces(pch+1);
      if (!szNdx[0]) continue;
      if (ndx_help(szNdx,szTopic,szExt)) return;
    }
  }
  MessageBox(hMDI0Client, "Context help", MB_ERROR|MB_OK|MB_MOVEABLE,"Can not find context help for: ", szTopic);
}

HELPMENU:: HELPMENU(){ memset(this,0,sizeof(*this)); }
HELPMENU::~HELPMENU(){ Clear();                      }

void HELPMENU::Append (PSZ szInfList)
{
//
// <szInfList> ::= <infitem>[;<szInfList>]
// <infitem>   ::= FULLNAME.INF | DIRECTORY | NAME.INF | NAME | ""//
// FULLNAME.INF - добавляем этот файл
// DIRECTORY    - все *.inf файлы из нее
// NAME.INF     - ищем этот файл по BOOKSHELF
// NAME         - 1) добавим ".inf" и попробуем как NAME.INF
//                2)если есть env. variable с таким именем и ее вид *.inf{+*.inf} -
//                  в качестве NAME.INF попробуем первый *.inf
  if (!pMDI) return;
  int           i;
  PSZ           pch;
  char          sz    [CCHMAXPATH];
  int           nFile;
  PSZ           pchList = szInfList;
  HWND          hMenu   = WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU);

  for(nFile=0; nFile<MAXINFFILES && apszInfFiles[nFile]; nFile++);
  if (!szInfList || !hMenu) return;

  while(nFile < MAXINFFILES)
  {
    pchList = sf_skipspaces(pchList);
    if (!*pchList) break; ///--- End of list
    if (!(pch = strpbrk(pchList,"; \t"))) pch = strchr(pchList,'\0');
    strncpy(sz,pchList,pch-pchList);
    sz[pch-pchList] = '\0';
    pchList = sf_skipspaces(pch); if (*pchList==';') pchList++;
    if (!sz[0]) continue;
    // Now: pchList -> следующий элемент списка
    //      sz      =  <infitem>

    if (strpbrk(sz,"\\/:")) // FULLANAME or DIRECTORY
    {
      if (strchr(sz,'.'))   // FULLANAME
      {
        if (add_inf_fullname(sz,nFile)) nFile++;
      }
      else               // DIRECTORY
      {
        HDIR          hdirFindHandle = HDIR_SYSTEM;
        FILEFINDBUF3  FindBuffer     = {0};
        ULONG         ulFindCount    = 1;
        APIRET        rc             = 0;
        char          szPath[CCHMAXPATH];
        char          szFull[CCHMAXPATH];
        if (!FName2Full(szPath,sz,FALSE)) continue;
        strcat(szPath,"\\");
        strcat(sz,"\\*.inf");
        if (!DosFindFirst(sz, &hdirFindHandle, FILE_ARCHIVED | FILE_READONLY,
                          &FindBuffer, sizeof(FILEFINDBUF3), &ulFindCount, FIL_STANDARD))
        {
          while (!rc)
          {
            strcpy(szFull,szPath);
            strcat(szFull,FindBuffer.achName);
            if (add_inf_fullname(sz,nFile)) nFile++;
            ulFindCount = 1;
            rc = DosFindNext(hdirFindHandle, &FindBuffer, sizeof(FILEFINDBUF3), &ulFindCount);
          }
          DosFindClose(hdirFindHandle);
        }
      }
    }
    else // NAME.INF or NAME
    {
      if ((pch=strrchr(sz,'.')) && !sf_stricmp(pch,".INF")) // NAME.INF
      {
        if (bookshelf_search(sz) && add_inf_fullname(sz,nFile)) nFile++;
      }
      else
      // NAME
      {
        // 1) добавим ".inf" и попробуем как NAME.INF
        i = strlen(sz);
        strcat(sz,".inf");
        if (bookshelf_search(sz))
        {
          if (add_inf_fullname(sz,nFile)) nFile++;
          continue;
        }
        sz[i] = '\0';
        // 2) если есть env. variable с таким именем и ее вид *.inf{+*.inf} -
        //    в качестве NAME.INF попробуем первый *.inf
        if (!DosScanEnv(sz,(PCSZ*)&pch) && pch)
        {
          strncpy(sz,sf_skipspaces(pch),sizeof(sz));
          sz[sizeof(sz)-1] = 0;
          if (pch=strpbrk(sz,"+ \t")) *pch = 0;
          if ((pch=strchr(sz,'.')) && !sf_stricmp(pch,".INF") && bookshelf_search(sz))
          {
            if (add_inf_fullname(sz,nFile)) nFile++;
            continue;
          }
        }
      }
    }
  } // while(nFile < MAXINFFILES)
}
void HELPMENU::Clear()
{
  HWND hMenu = WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU);
  for (int i=0; i<MAXINFFILES; i++)
  {
    free(apszInfFiles[i]); apszInfFiles[i] = 0;
    WinSendMsg(hMenu,MM_REMOVEITEM,MPFROM2SHORT(IDM_HELP_INFFILE0+i,TRUE),0);
  }
  WinSendMsg(hMenu,MM_REMOVEITEM,MPFROM2SHORT(IDM_HELPSEPARATOR,TRUE),0);
}
void HELPMENU::WProfile(LONG slot, PSZ szName, BOOL fMirror)
{
  PLINELIST pllInfs = 0;
  for (int i=0; i<MAXINFFILES; i++)
   if (apszInfFiles[i])
     pllInfs = sf_applist(pllInfs,fMirror ? sf_fname2short(apszInfFiles[i]) : apszInfFiles[i]);
  PfWriteLineList (slot, szName, pllInfs);
  sf_freelist(pllInfs);
}
void HELPMENU::RProfile(LONG slot, PSZ szName)
{
  PSZ       pszBuf,psz;
  int       nSize   = 1;
  PLINELIST pllInfs = PfReadLineList(slot, szName);
  Clear();
  for (PLINELIST pll = pllInfs; pll; pll=pll->next)
    nSize += strlen(pll->text)+1;
  if (nSize>1 && (psz = pszBuf = (PSZ)malloc(nSize)))
  {
    for (PLINELIST pll = pllInfs; pll; pll=pll->next)
    {
      strcpy(psz,pll->text);
      psz += strlen(psz);
      *(psz++) = ';';
      *psz     = 0;
    }
    Append(pszBuf);
    free(pszBuf);
  }
  sf_freelist(pllInfs);
}


BOOL HELPMENU::add_inf_fullname(PSZ szFName, int nFile)
// добавить в хэлповое меню этот файл
{
  char          szFull[CCHMAXPATH];
  MENUITEM      mi;
  memset (&mi,0,sizeof(mi));
  HWND          hMenu          = WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU);
  WinSendMsg(hMenu,MM_QUERYITEM,MPFROM2SHORT(IDM_HELP,TRUE),MPARAM(&mi));
  HWND          hPullDown      = mi.hwndSubMenu;

  char          szItem[CCHMAXPATH];
  BOOL          fOk            = FALSE;
  HFILE         hf;
  ULONG         ulAction,ul;
  if (!FName2Full(szFull,szFName,FALSE)) return FALSE;
  if (!DosOpen(szFull,&hf,&ulAction,0,FILE_NORMAL,FILE_OPEN,
               OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE, (PEAOP2)0))
  {
    char achBuf[0xa0];
    DosRead(hf,achBuf,sizeof(achBuf),&ul);
    DosClose(hf);
    if (ul==sizeof(achBuf) && !strncmp(achBuf,"HS",2))
    {
      achBuf[0x6b + 48] = '\0';
      if (strlen(achBuf+0x6b)) strcpy(szItem, achBuf+0x6b);
      else                     strcpy(szItem, sf_fname2short(szFull));
      fOk = TRUE;
    }
  }
  if (!fOk || !(apszInfFiles[nFile] = (PSZ)malloc(strlen(szFull)+1))) return FALSE;
  strcpy(apszInfFiles[nFile],szFull);

  if (!nFile)
  {
    mi.hwndSubMenu  = NULL;
    mi.iPosition    = MIT_END;
    mi.id           = IDM_HELPSEPARATOR;
    mi.afStyle      = MIS_SEPARATOR;
    WinSendMsg(hPullDown,MM_INSERTITEM,MPARAM(&mi),MPARAM(""));
  }
  mi.hwndSubMenu  = NULL;
  mi.iPosition    = MIT_END;
  mi.id           = IDM_HELP_INFFILE0+nFile;
  mi.afStyle      = MIS_TEXT;
  WinSendMsg(hPullDown,MM_INSERTITEM,MPARAM(&mi),MPARAM(szItem));
  return TRUE;
}

BOOL HELPMENU::bookshelf_search(PSZ szInf)
{
  PSZ      pszBookshelf               = "";
  char     szSearchResult[CCHMAXPATH] = "";
  if (!DosScanEnv("BOOKSHELF",(PCSZ*)&pszBookshelf) &&
      !DosSearchPath(SEARCH_IGNORENETERRS, pszBookshelf,szInf,szSearchResult,sizeof(szSearchResult)))
  {
    strcpy(szInf,szSearchResult);
    return TRUE;
  }
  else
   return FALSE;
}


