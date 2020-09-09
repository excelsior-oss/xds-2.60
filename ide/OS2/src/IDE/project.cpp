/*
*
*  File: PROJECT.CPP
*
*  XDS Shell - project: open & edit features
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#define  INCL_WIN
#define  INCL_GPI
#define  INCL_DOS
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "XDSBASE.H"
#define  __WINMAKER
#include "WINMAKE\WINMAKE.H"

MRESULT EXPENTRY dpNewPrjDlg(HWND, ULONG, MPARAM, MPARAM);
BOOL             create_project(PSZ pszFile, PSZ pszTplFile, HWND hMsgWnd);

BOOL NewProject()
{
  return (BOOL)WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpNewPrjDlg,NULLHANDLE,IDD_NEWPRJDLG,0);
}

BOOL OpenProject(PSZ pszFile, HWND hMsgWnd)
{
  char  szF[CCHMAXPATH];
  char *pch;
  if (!pszFile)
  {
    if (!Browse(szF,"*.*",hMDI0Client,"Open project",FALSE)) return FALSE;
    else                                              pszFile = szF;
  }
  if (pszFile[0])
  {
    if (!FName2Full(szF,pszFile,FALSE) && szF!=pszFile)
      strcpy(szF,pszFile);
  }
  else
    szF[0] = 0;
  if (!sf_stricmp(szPrjFile,szF))
    return FALSE;
  {
    FILESTATUS3 fs3;
    if (szF[0] && DosQueryPathInfo(szF,FIL_STANDARD,&fs3,sizeof(fs3)))
    {
      if (hMsgWnd)
        MessageBox(hMsgWnd,"Open project",MB_ERROR|MB_OK,"Can not find project file '", szF, "'.");
      return FALSE;
    }
  }
  strcpy(szPrjFile, szF);

  //--- variable assignment:
  strcpy(szF,sf_fname2short(szPrjFile));
  if (pch=strrchr(szF,'.')) *pch=0;
  pAutoVarTable->AssignVar("Proj",     szPrjFile);
  pAutoVarTable->AssignVar("ProjName", szF);
  pAutoVarTable->AssignVar("ProjPath", FName2Path(szF, szPrjFile) ? szF : "");
  SetAllCaptions();
  EnableMenues(EM_ASKALL);
  pToolbox->ReenableAll();
  hstProjects.Append(szPrjFile);
  return TRUE;
}

BOOL EdProjectFile ()
{
  if (szPrjFile[0] && grOffice.OpenEditor(IN_AUTO_WINDOW, szPrjFile, hMDI0Client, 0))
  {
    HWND hEd = grOffice.KnownFilename(szPrjFile);
    if (hEd)
    {
      grOffice.GotoEd(hEd, FALSE);
      return TRUE;
    }
  }
  return FALSE;
}

MRESULT EXPENTRY dpNewPrjDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static HWND hEF;
  static HWND hCB;
  switch(ulMsg){
  case WM_INITDLG:
    CenterChildWindow(hWnd);
    hEF = WinWindowFromID(hWnd,IDD_EFNEWPRJNAME);
    hCB = WinWindowFromID(hWnd,IDD_CBNEWPRJTPLS);
    WinSendMsg(hEF,EM_SETTEXTLIMIT,MPARAM(CCHMAXPATH-1),0);
    WinSetWindowText(hCB,"Feature under construction... :-(");
    break;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch(SHORT1FROMMP(m1))
    {
      case IDD_PBNEWPRJBROWSE:
      {
        char szF[CCHMAXPATH];
        WinQueryWindowText(hEF,sizeof(szF),szF);
        if (!szF[0]) strcpy(szF,"*.*");
        if (Browse(szF,szF,hWnd,"New project",FALSE))
          WinSetWindowText(hEF,szF);
        break;
      }
      case DID_CANCEL:
        WinDismissDlg(hWnd,0);
        break;
      case DID_OK:
      {
        char szF[CCHMAXPATH];
        WinQueryWindowText(hEF,sizeof(szF),szF);
        if (!*sf_skipspaces(szF))
        {
          Beep();
          WinSetFocus(HWND_DESKTOP,hEF);
          break;
        }
        FName2Full(szF,szF,FALSE);
        if (create_project(szF,0,hWnd) && OpenProject(szF,hWnd))
          WinDismissDlg(hWnd,1);
        break;
      }
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_PRJNEWDLG);
        break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_PRJNEWDLG);
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


BOOL create_project(PSZ pszFile, PSZ pszTplFile, HWND hMsgWnd)
{
  HFILE       hf;
  ULONG       ulAction;
  APIRET      err;
  FILESTATUS3 fs3;
  if (!DosQueryPathInfo(pszFile,FIL_STANDARD,&fs3,sizeof(fs3)) &&
      (!hMsgWnd || MBID_YES!=MessageBox(hMsgWnd,"New project",MB_ICONQUESTION|MB_YESNO,"File '", pszFile,
                                            "' already exists. Do you want to owerwrite it?")))
    return FALSE;
  if (err = DosOpen(pszFile, &hf, &ulAction, 0, FILE_NORMAL,
                    OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                    OPEN_ACCESS_READWRITE     | OPEN_SHARE_DENYWRITE,(PEAOP2)0L))
  {
    if (hMsgWnd) IOErMsg(err, hMsgWnd, "Craete project file", "Can't create the file", pszFile, MB_OK);
    return FALSE;
  }
  /*++++ ApplyProjectTemplate(hf,...); */
  DosClose(hf);
  return TRUE;
}

/******************  Редактирование проекта 'в картинках'  ********************************************************/
#define STATIC_LARGE     0x0001
#define STATIC_BOLD      0x0002
#define STATIC_ITALIC    0x0004
#define STATIC_CENTER    0x0008
#define STATIC_RIGHT     0x0010
#define GROUP_BEGIN      0x0001
#define GROUP_END        0x0002
#define GROUP_STYLE_1    0x0004
#define GROUP_STYLE_2    0x0008
#define GROUP_STYLE_3    0x0010
#define GROUP_CLOSEALL   0x8000
#define ENTRY_TEXT       0x0001
#define ENTRY_DECIMAL    0x0002
#define ENTRY_HEX        0x0004
#define BOOL_ON          0x0001
#define BOOL_SHOWINVERSE 0x0002
#define LLM_DRAWITEM     (WM_USER+103)
#define LLM_LOCKUPDATE   (WM_USER+104)

static  PSZ  sz_alloc(PSZ pszLine, int nLen);
typedef enum {COMMENTARY,EQUATION,USEREQUATION,XDSOPTION,USEROPTION,MODULE,LOOKUP} PRJ_LINE_TYPE;

struct  PRJLINE                 // Parsed project line
// !!! NOTE: try/catch(PSZ pszErr) block required !!!
{
  PRJ_LINE_TYPE    type;        // Line contents type
  char            *pszText;     // malloc()ed  Text of the line
  char            *pszKeyName;  // malloc()ed  Equ: <eqname>,    Opt: <optname>, Mod: <>,       Lookup: <Filename template>
  char            *pszKeyValue; // malloc()ed  Equ: <rght part>, Opt: <"+"/"-">, Mod: <{Name}>, Lookup: <Path(s)>
  char            *pszComment;  // malloc()ed  [EOL] comment of the line (with '%')
  BOOL             fChanged;
  PRJLINE         *next;        // List link

  PRJLINE              ();
 ~PRJLINE              ();
  void SetText         (PSZ psz, int nLen = 0);
  void SetKeyName      (PSZ psz, int nLen = 0);
  void SetKeyValue     (PSZ psz, int nLen = 0);
  void SetComment      (PSZ psz, int nLen = 0);
};
typedef PRJLINE *PPRJLINE;


struct PRJSTRUCT  // Project
{
  BOOL  Parse        (HWND hMsgWnd, PSZ szPrjFile);
  BOOL  BuildBack    (HWND hMsgWnd, PSZ szPrjFile);
  BOOL  IsChanged    ();
  BOOL  IsAnyModules ();
  void  FreeAll      ();
  PRJSTRUCT          () {memset(this,0,sizeof(*this));}
 ~PRJSTRUCT          () {FreeAll();}
  PPRJLINE pPrjLines;
};
typedef PRJSTRUCT *PPRJSTRUCT;

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////----------------------- PRJLINE realisation -------------------//////////////////
PRJLINE::PRJLINE()
{
  memset(this,0,sizeof(*this));
}
PRJLINE::~PRJLINE()
{
  free(pszText);
  free(pszKeyName);
  free(pszKeyValue);
  free(pszComment);
}
void PRJLINE::SetText(PSZ psz, int nLen)
{
  free(pszText);
  pszText = 0;
  pszText = sz_alloc(psz,nLen);
  fChanged=TRUE;
}
void PRJLINE::SetKeyName(PSZ psz, int nLen)
{
  free(pszKeyName);
  pszKeyName = 0;
  pszKeyName = sz_alloc(psz,nLen);
  fChanged=TRUE;
}
void PRJLINE::SetKeyValue(PSZ psz, int nLen)
{
  free(pszKeyValue);
  pszKeyValue = 0;
  pszKeyValue = sz_alloc(psz,nLen);
  fChanged=TRUE;
}
void PRJLINE::SetComment(PSZ psz, int nLen)
{
  free(pszComment);
  pszComment = 0;
  pszComment = sz_alloc(psz,nLen);
  fChanged=TRUE;
}

/////////////////------------- PRJSTRUCT realisation ----------////////////////

static char * skipname (char * s)
{
  while (sf_isalpha (*s) || sf_isdigit (*s) || *s == '_') ++ s;
  return s;
}
static void skipvalue (PSZ *pp, PPRJLINE pPrjLine)
{
  char ch = **pp;
  PSZ  pchE;
  if (ch=='"' || ch=='\'')
  {
    pchE = strchr(*pp+1,ch);
    if (!pchE) throw "Text is not terminated";
    pPrjLine->SetKeyValue(*pp+1,pchE-*pp-1);
    pchE++;
  }
  else
  {
    pchE = *pp;
    if (ch!='%')
      while((ch=*pchE) && !sf_isspace(ch)) pchE++;
    pPrjLine->SetKeyValue(*pp,pchE-*pp);
  }
  *pp = sf_skipspaces(pchE);
}
static void skipcomment(PSZ psz, PPRJLINE pPrjLine)
{
  if      (*psz=='%') pPrjLine->SetComment(psz);
  else if (*psz)      throw "'%' (comment) expected";
}

static PSZ sz_alloc(PSZ pszLine, int nLen)
{
  PSZ psz;
  if (!pszLine) pszLine = "";
  if (!nLen) nLen = strlen(pszLine);
  if (psz = (PSZ)malloc(nLen+1))
  {
    strncpy(psz,pszLine,nLen);
    psz[nLen] = 0;;
  }
  else throw "Out of memory error (Location: sz_alloc)"; // This text!
  return psz;
}

BOOL PRJSTRUCT::Parse(HWND hMsgWnd, PSZ szPrjFile)
{
  PSZ       pszFileBuf;
  PPRJLINE  pPrjLine, pPrjLast = 0, pEmptyTail = 0;;
  HWND      hEd;
  FreeAll();
  if (hEd = grOffice.KnownFilename(szPrjFile))
  {
    if (!(pszFileBuf = (PSZ)WinSendMsg(hEd,TM_EXPORT,0,0)))
      MessageBox(hMsgWnd,"Read project file",MB_ERROR|MB_OK,"No enouth memory error (or an internal error)");
  }
  else
  {
    APIRET  rc;
    HFILE   hf;
    ULONG   ul,fsize;
    DosError(FERR_DISABLEHARDERR);
    if (rc = (int)DosOpen(szPrjFile,&hf,&ul,0,FILE_NORMAL,FILE_OPEN,
                      OPEN_FLAGS_NO_CACHE |OPEN_FLAGS_SEQUENTIAL|
                      OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE, (PEAOP2)0))
    {
      DosError(FERR_ENABLEHARDERR);
      IOErMsg(rc, hMsgWnd, "Read project file", "Can't open file", szPrjFile, MB_OK);
      return FALSE;
    }
    DosSetFilePtr(hf,0L,FILE_END,  &fsize);
    DosSetFilePtr(hf,0L,FILE_BEGIN,&ul);
    if (!(pszFileBuf = (char*)malloc(fsize+1)))
    {
      DosClose(hf);
      DosError(FERR_ENABLEHARDERR);
      MessageBox(hMsgWnd,"Read project file",MB_ERROR|MB_OK,"No enouth memory error");
      return FALSE;
    }
    if (rc = (int)DosRead(hf,(PVOID)pszFileBuf,fsize,&ul))
    {
      DosError(FERR_ENABLEHARDERR);
      free(pszFileBuf);
      IOErMsg(rc, hMsgWnd, "Read project file", "File read error", szPrjFile, MB_OK);
      return FALSE;
    }
    pszFileBuf[ul] = 0;
    DosClose(hf);
    DosError(FERR_ENABLEHARDERR);
  }

  // pszFileBuf == z-terminated project text
  int    parse_error_x;
  int    parse_error_line = 0;
  int    nBugsFound       = 0;
  PSZ    pszSrcLine, pszNxtLine;
  BOOL   fModuleSection   = FALSE;
  for (pszNxtLine=pszFileBuf; *(pszSrcLine = pszNxtLine); parse_error_line++)
  {
    {
      char *pchE  = pszSrcLine+strcspn(pszSrcLine,"\n\r");
      pszNxtLine  = pchE+1;
      if      (!*pchE)                         pszNxtLine--;
      else if (pchE[0]=='\r' && pchE[1]=='\n') pszNxtLine++;
      *pchE = 0;
    }
    pPrjLine = new PRJLINE;
    if (pPrjLast) pPrjLast->next = pPrjLine;
    else          pPrjLines      = pPrjLine;
    try
    {
      // Тут заполняется структура pPrjLine
      // для строки проекта pszSrcLine;
      // При ошибке: throw с текстом ошибки, parse_error_x = позиция в строке
      //
      parse_error_x    = 0;
      pPrjLine -> SetText(pszSrcLine);
      char * p, *t;
      char * s         = sf_skipspaces (pszSrcLine);
      pPrjLine -> type = COMMENTARY;
      if (!*s) {if (!pEmptyTail) pEmptyTail = pPrjLast;}
      else     pEmptyTail = 0;
      switch (*s)
      {
        case '\0':
          break;
        case '%':
          pPrjLine -> SetComment(s);
          break;
        case '!':     // !module {<module_name> } [%comment]
          parse_error_x = s - pszSrcLine + 1;
          if (sf_strnicmp(s,"!module",7)) throw "'!module' expected";
          s = sf_skipspaces(s+7);
          p = s;
          while(1)
          {
            t = skipname(p);
            if (t==s)
            {
              parse_error_x = p - pszSrcLine;
              throw "Module name expected";
            }
            else if (t==p)
              break;
            p = sf_skipspaces(t);
          }
          pPrjLine -> SetKeyValue(s, t-s);
          pPrjLine -> type = MODULE;
          parse_error_x    = p - pszSrcLine;
          skipcomment(p,pPrjLine);
          fModuleSection = TRUE;
          break;
        case ':':  // ':' <name> '+' | '-' |  '=' <value>
          if (fModuleSection) throw "'!module' expected";
          s                = sf_skipspaces (s+1);
          p                = skipname (s);
          parse_error_x    = p - pszSrcLine;
          if (p == s) throw "User option name expected";
          pPrjLine -> SetKeyName(p, p-s);
          p                = sf_skipspaces (p);
          if (*p=='+' || *p=='-')
          {
            pPrjLine -> type = USEROPTION;
            pPrjLine -> SetKeyValue((*p == '+') ? "+" : "-");
          }
          else if (*p=='=')
          {  // ':' <name> '=' <value>
            p = sf_skipspaces(p+1);
            pPrjLine -> type = USEREQUATION;
            parse_error_x    = p - pszSrcLine;
            skipvalue(&p,pPrjLine);
          }
          else
          {
            parse_error_x = p-pszSrcLine;
            throw "'+', '-' or '=' expected.";
          }
          p             = sf_skipspaces(p);
          parse_error_x = p - pszSrcLine;
          skipcomment(p,pPrjLine);
          break;
        case '-':
        case '+':
        {
          if (fModuleSection) throw "'!module' expected";
          BOOL fOptOnly = (*s == '+');
          p             = sf_skipspaces (s+1);
          t             = skipname (p);
          parse_error_x = p - pszSrcLine;
          if (p == t) throw "Name expected";
          if (!fOptOnly && !sf_strnicmp(p,"lookup",6))
          {
            pPrjLine -> type = LOOKUP;
            p                = sf_skipspaces(t);
            parse_error_x    = p - pszSrcLine;
            if (*p != '=')               throw "'=' expected";
            p                = sf_skipspaces(p+1);
            for (t=p; *t && *t!=' ' && *t != '\t' && *t!='='; t++);
            parse_error_x    = p - pszSrcLine;
            if (p==t)                    throw "Bad symbol";
            pPrjLine -> SetKeyName(p, t-p); /* Filename templates */
            p                = sf_skipspaces(t);
            parse_error_x    = p - pszSrcLine;
            if (*p != '=')               throw "'=' expected";
            t                = sf_skipspaces(p+1);
            for (p=t; *p && *p!=' '; p++);
            parse_error_x    = p - pszSrcLine;
            if (p==t)                    throw "Path list expected";
            pPrjLine -> SetKeyValue(t, p-t); /* Path(s) */
            break;
          }
          pPrjLine -> SetKeyName(p, t-p);
          pPrjLine -> SetKeyValue((*s == '+') ? "+" : "-");
          pPrjLine -> type = XDSOPTION;
          t = sf_skipspaces(t);
          // if (+/- <name>) t -> [%comment]
          if (!fOptOnly)
          { // '-' <name> (( ':=' | '=' <value>) | ( [':'] '+' / '-')) [%comment]
            if (!strncmp(t,":=",2) || *t=='=')
            {
              t++;
              pPrjLine -> type = EQUATION;
              if (t[-1]==':')
              {
                t++;
                pPrjLine -> type = USEREQUATION;
              }
              t             = sf_skipspaces(t);
              parse_error_x = t - pszSrcLine;
              skipvalue(&t,pPrjLine);
              // Now: t -> [%comment]
            }
            else
            {
              if (*t==':')
              {
                pPrjLine -> type = USEROPTION;
                t = sf_skipspaces(t+1);
              }
              parse_error_x = t - pszSrcLine;
              if (*t=='+' || *t=='-')
              {
                pPrjLine -> SetKeyValue((*t == '+') ? "+" : "-");
                t = sf_skipspaces(t+1);
                // Now: t -> [%comment]
              }
              else if (pPrjLine->type == USEROPTION)
                throw "'+' or '-' expected";
            }
          }
          parse_error_x = t - pszSrcLine;
          skipcomment(t,pPrjLine);
          break;
        }
        default:
          if (fModuleSection) throw "'!module' expected";
          parse_error_x = s - pszSrcLine;
          throw "Bad symbol";
      }
    }
    catch (char *psz)
    {
      if (!nBugsFound)
        errOffice.ClearAll();
      errOffice.AddError(psz, parse_error_x+errOffice.l1stPos, parse_error_line+errOffice.l1stLine, szPrjFile, MSG_ERROR, 0);
      nBugsFound++;
      if (!strncmp(psz,"Out of memory error",19)) break;
    }
    pPrjLine->fChanged = FALSE;
    pPrjLast           = pPrjLine;
  }// for
  free(pszFileBuf);
  if (nBugsFound)
  {
    errOffice.DoneUpdating();
    char szEr[200];
    sprintf(szEr,"Parsing project file: %u error(s) found.",nBugsFound);
    MessageBox(hMsgWnd,"Read project file",MB_ERROR|MB_OK,szEr);
    FreeAll();
    WinSetFocus(HWND_DESKTOP,clMsgList.hWnd);
    return FALSE;
  }
  if (pEmptyTail)
  {
    PPRJLINE p1,p = pEmptyTail->next;
    pEmptyTail->next = 0;
    for (;p;p=p1)
    {
      p1 = p->next;
      delete p;
    }
  }
  return TRUE;
}

static void app_txt(char **ppBuf, int *pnUsed, int *pnAlloc, PSZ psz)
{
  int l = psz ? strlen(psz) : 0;
  while(*pnUsed + l + 1 >= *pnAlloc) *pnAlloc = *pnAlloc ? *pnAlloc*2 : 64;
  if (!(*ppBuf = (char*)realloc(*ppBuf,*pnAlloc)))
  {
    free(*ppBuf);
    *ppBuf = 0;
    *pnUsed = *pnAlloc = 0;
    throw "Out of memory error.";
  }
  strcpy(*ppBuf + *pnUsed, psz ? psz : "");
  *pnUsed += l;
}
static void app_quote(char **ppBuf, int *pnUsed, int *pnAlloc, PSZ psz)
{
  char ch;
  if (!psz || !(ch=psz[0]) || strchr(psz,'"'))      return; // can not be quoted or empty
  if (sf_isspace(ch) && !*sf_skipspaces(psz))       return; // empty
  if (!sf_isspace(ch) && ch!='%')                           // (Start from space or '%' - must be quoted)
    for (;;psz++)
    {
      if (!*psz)                                    return; // no balnks in the value
      if (sf_isspace(*psz))
        if (!*sf_skipspaces(psz))                   return; // no balnks _inside_ value
        else break;
    }
  app_txt(ppBuf, pnUsed, pnAlloc, "\"");
}
static void app_comment(char **ppBuf, int *pnUsed, int *pnAlloc, PPRJLINE p, int nSkipLen)
{
  if (p->pszComment && p->pszComment[0])
  {
    int   nLineLen = *pnUsed - nSkipLen;
    int   nTab     = nLineLen < 31 ? 32-nLineLen : 1;
    PSZ   psz      = (PSZ)malloc(strlen(p->pszComment)+nTab+1);
    if (psz)
    {
      memset(psz,' ',nTab);
      strcpy(psz+nTab,p->pszComment);
      app_txt(ppBuf, pnUsed, pnAlloc, psz);
      free(psz);
    }
  }
}

BOOL PRJSTRUCT::BuildBack(HWND hMsgWnd, PSZ szPrjFile)
{
  int   nAlloc   = 0;
  int   nUsed    = 0;
  PSZ   pBuf     = 0;
  int   nSkipLen = 0;
  try
  {
    for (PPRJLINE p = pPrjLines; p; p=p->next)
    {
      if (!p->fChanged || p->type==COMMENTARY)
        app_txt(&pBuf,&nUsed,&nAlloc,p->pszText);
      else
      {
        switch(p->type)
        {
          case EQUATION:
          case USEREQUATION:
            app_txt    (&pBuf, &nUsed, &nAlloc, "-");
            app_txt    (&pBuf, &nUsed, &nAlloc, p->pszKeyName);
            app_txt    (&pBuf, &nUsed, &nAlloc, p->type==EQUATION ? " = " : " := ");
            app_quote  (&pBuf, &nUsed, &nAlloc, p->pszKeyValue);
            app_txt    (&pBuf, &nUsed, &nAlloc, p->pszKeyValue);
            app_quote  (&pBuf, &nUsed, &nAlloc, p->pszKeyValue);
            app_comment(&pBuf, &nUsed, &nAlloc, p,nSkipLen);
            break;
          case XDSOPTION:
          case USEROPTION:
            app_txt    (&pBuf, &nUsed, &nAlloc, "-");
            app_txt    (&pBuf, &nUsed, &nAlloc, p->pszKeyName);
            if (p->type==USEROPTION)
              app_txt  (&pBuf, &nUsed, &nAlloc, ":");
            app_txt    (&pBuf, &nUsed, &nAlloc, p->pszKeyValue);
            app_comment(&pBuf, &nUsed, &nAlloc, p,nSkipLen);
            break;
          case MODULE:
            app_txt    (&pBuf, &nUsed, &nAlloc, "!module ");
            app_txt    (&pBuf, &nUsed, &nAlloc, p->pszKeyValue);
            app_comment(&pBuf, &nUsed, &nAlloc, p,nSkipLen);
            break;
          case LOOKUP:
            app_txt    (&pBuf, &nUsed, &nAlloc, "-lookup = ");
            app_txt    (&pBuf, &nUsed, &nAlloc, p->pszKeyName);
            app_txt    (&pBuf, &nUsed, &nAlloc, " = ");
            app_txt    (&pBuf, &nUsed, &nAlloc, p->pszKeyValue);
            app_comment(&pBuf, &nUsed, &nAlloc, p,nSkipLen);
            break;
          default:
            throw "Internal error (Location: PRJSTRUCT::BuildBack).";
        }
      }
      app_txt(&pBuf,&nUsed,&nAlloc,"\r\n");
      nSkipLen = nUsed;
    }
  }
  catch (char *psz)
  {
    MessageBox(hMsgWnd,"Update project file",MB_ERROR|MB_OK,psz,"Project file is not updated");
    free(pBuf);
    return FALSE;
  }
  if (!pBuf) {pBuf = (PSZ)malloc(3); strcpy(pBuf,"\r\n");}

  //--- Now: pBuf contains malloc()ed z-terminated text of the updated project file
  HWND hEd;
  if (hEd = grOffice.KnownFilename(szPrjFile))
  {
    TPR tpr(0,0);
    WinSendMsg(hEd,TM_SETTEXT,    MPARAM(pBuf), MPARAM(strlen(pBuf)));
    WinSendMsg(hEd,TM_SETSEL,     MPARAM(&tpr), MPARAM(&tpr));
    WinSendMsg(hEd,TM_SETCHANGED, MPARAM(TRUE), 0);
  }
  else
  {
    DosError(FERR_DISABLEHARDERR);
    HFILE   hf;
    ULONG   ul,ulAction;
    char    szBakFile[CCHMAXPATH];
    strcpy(szBakFile,szPrjFile);
    // Сделаем '.bak' расширение
    PSZ  pszFN,pszDot;
    BOOL fBakMaked = FALSE;
    (pszFN=strrchr(szBakFile,'\\')) || (pszFN=strrchr(szBakFile,'/')) ||(pszFN=szBakFile);
    (pszDot=strrchr(pszFN,'.')) || (pszDot=pszFN+strlen(pszFN));
    strcpy(pszDot,".BAK");
    if (GROUPOFFICE::fMakeBak) fBakMaked = !DosMove(szPrjFile,szBakFile);
    APIRET err = DosOpen(szPrjFile,&hf,&ulAction,0,FILE_NORMAL,
                         OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
                         OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYWRITE,(PEAOP2)0L);
    if (err)
    {
      if (fBakMaked) DosMove(szBakFile,szPrjFile); // Попытаемся сокрыть содеянное...
      IOErMsg(err, hMsgWnd, "Update project", "Can't open or create file", szPrjFile, MB_OK);
      DosError(FERR_ENABLEHARDERR);
      return FALSE;
    }
    DosSetFileSize(hf,0);
    LONG  lLen = strlen(pBuf);
    if (err = DosWrite(hf,PBYTE(pBuf),lLen,&ul))
      IOErMsg(err, hMsgWnd, "Update project", "Write file error", szPrjFile, MB_OK);
    DosClose(hf);
    DosError(FERR_ENABLEHARDERR);
    return !err;
  }
  return TRUE;
}

BOOL PRJSTRUCT::IsChanged()
{
  for (PPRJLINE p=pPrjLines; p; p=p->next)
    if (p->fChanged) return TRUE;
  return FALSE;
}

BOOL PRJSTRUCT::IsAnyModules()
{
  for (PPRJLINE p=pPrjLines; p; p=p->next)
    if (p->type==MODULE && p->pszKeyValue && *sf_skipspaces(p->pszKeyValue))
      return TRUE;
  return FALSE;
}
void PRJSTRUCT::FreeAll()
{
  for (PPRJLINE p1,p=pPrjLines; p; p=p1)
  {
    p1 = p->next;
    delete p;
  }
  pPrjLines = 0;
}
/////////////////------------- End of PRJLINES & PRJSTRUCT realisations ----------////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

  #define  MAXENUM 20
  typedef  enum    {OPT_EQUATION, OPT_BOOLEAN, OPT_ENUMERATION, OPT_STATIC, OPT_GROUP} OPT_TYPE;
  struct   OPTION      //  !!! NOTE: throw is possible inside !!!
  {
    OPT_TYPE           type;
    char               szName [30];         //  имя в проекте (переменной, например);
    PSZ                pszComment;          //  malloc()ed комментарий (после ';');
    PSZ                pszTip;              //  malloc()ed текст tip-а
    int                nAlts;               //  кол-во альтернатив в Enumeration (используется и в Group)
    int                nVal;                //  <OPT_BOOLEAN> : BOOL_* ;  <OPT_ENUMERATION> : # альт-вы ; <OPT_EQUATION> : ENTRY_*
                                            //                            <OPT_GROUP>       : GROUP_*   ; <OPT_STATIC>   : STATIC_*
    BOOL               fPresent;            //  не/ставить в проект
    int                fModifyed;           //  ясно
    char              *pszString;           //  <OPT_EQUATION>    : ук-ль на текст
    char              *apszEnums [MAXENUM]; //  <OPT_ENUMERATION> : массив malloc()ed текстов
    OPTION            *next;
    OPTION             ();
   ~OPTION             ();
    void SetComment    (PSZ psz, int nLen=0);
    void SetTip        (PSZ psz, int nLen=0);
    void SetString     (PSZ psz, int nLen=0);
    void AddEnumValue  (PSZ psz, int nLen=0);
    void AskControl    (HWND hWnd, HWND hCBPresent);
    void SetControl    (HWND hWnd, HWND hCBPresent);
    void ReadName      (PSZ *ppsz);   //
    void ReadComment   (PSZ  psz);    // Там может
    void ReadString    (PSZ *ppsz);   //   приключиться throw
    void ReadEnumValue (PSZ *ppsz);   //
    void MakeTip       (PSZ pszTipVal);//
  };
  typedef OPTION *POPTION;

  struct NODE
  {
    char               szNodeName[80];   //  имя узла (f.ex. "Compiler options")
    int                nLevel;           //  уровень вложенности в листе
    BOOL               fExpanded;        //  открыт ли его подуровень
    POPTION            pOptList;         //  список его опций
    NODE              *next;             //  след. Node
    NODE               () {memset(this,0,sizeof(*this));}
  };
  typedef NODE *PNODE;

  struct CONTROL
  {
    CONTROL() {memset(this,0,sizeof(*this));}
    HWND     hCtrl;
    HWND     hCBPresent;
    HWND     hStatic;
    POPTION  pOption;
    CONTROL *next;
  };
  typedef CONTROL *PCONTROL;

class NODELIST
{
public:
  NODELIST                ();
 ~NODELIST                ();
  BOOL     Initialize     (HWND hMsgWnd, PSZ szCfgFile); // Чтем options.cfg файл
  void     BindToDialog   (HWND hDlg, HWND hLBox, HWND hDisp);
  void     FillLBox       (PNODE pStartFrom = 0);
  void     LBSelect       ();                            // Selection (may be) changed
  void     LBEnter        ();                            // LBox enter action
  void     WMControl      (MPARAM m1,MPARAM m2);         // WM_CONTROL - может установить hCBPresent?
  void     AskControlsNow ();                            // Read the controls states NOW
  void     ForgetControls ();                            // Диалог сам покиляет наши контролы
  void     Prj2Options    (PPRJSTRUCT pPrj);             // Выставить значения имеющихся в проекте опциий
  void     Options2Prj    (PPRJSTRUCT pPrj);             // Значения из опций -> в проект
  void     WMPaint        (BOOL fErase=FALSE);           // Рисует groupboxы
  void     GenComment     (BOOL fReset, HWND hComment);  // Проверяет мышу на попадание в контрол, выдает tip
  friend   struct OPTION;
  friend   struct NODE;
private:
  PNODE    pNodes;
  PCONTROL pControls;
  HWND     hDlg;
  HWND     hLBox;
  HWND     hDisp;
  int      cyFont;
  int      cxFont;
  PNODE    pnSelected;
  RECTL    aGroups  [30];
  ULONG    aGrStyles[30];
  int      nGroups;
  BOOL     fConfigOk;

  void     del_controls   (BOOL fKillWindows);
  void     new_controls   (PNODE pNode);
  int      align_rect     (RECTL *pRcl, int yTop, int xLeftBound, int xRightBound, BOOL fRight);
  void     mk_group       (PLONG pyTop, PLONG pxLeftBound, PLONG pxRightBound, ULONG ulGrStyle);
  POPTION  find_opt       (PPRJLINE   pPrjLine);
  void     free_all       ();
  #define  create_ctrl(wclass, sz, ulStyle, rcl, ctlID)                                          \
                          WinCreateWindow(hDlg, wclass, sz,                                      \
                                          ulStyle|WS_VISIBLE,                                    \
                                          rcl.xLeft, rcl.yBottom,  rcl.xRight-rcl.xLeft,         \
                                          rcl.yTop-rcl.yBottom, hDlg, HWND_TOP, ctlID,NULL,NULL)
};
typedef NODELIST *PNODELIST;

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////----------------------- OPTION realisation -------------------//////////////////
OPTION::OPTION()
{
  memset(this,0,sizeof(*this));
}
OPTION::~OPTION()
{
  free(pszComment);
  free(pszString);
  for (int i=0; i < MAXENUM; i++)
    free(apszEnums[i]);
  memset(this,0,sizeof(*this));
}
void OPTION::SetComment(PSZ psz, int nLen)
{
  free(pszComment);
  pszComment = 0;
  pszComment = sz_alloc(psz,nLen);
}
void OPTION::SetTip(PSZ psz, int nLen)
{
  free(pszTip);
  pszTip = 0;
  pszTip = sz_alloc(psz,nLen);
}
void OPTION::SetString(PSZ psz, int nLen)
{
  free(pszString);
  pszString = 0;
  pszString = sz_alloc(psz,nLen);
}
void OPTION::AddEnumValue(PSZ psz, int nLen)
{
  for (int i=0; i<MAXENUM; i++)
    if (!apszEnums[i])
    {
      apszEnums[i] = sz_alloc(psz,nLen);
      return;
    }
}
void OPTION::AskControl(HWND hWnd, HWND hCBPresent)
{
  switch(type)
  {
    case OPT_EQUATION:
      if (WinSendMsg(hWnd,EM_QUERYCHANGED,0,0))
      {
        int nLen = WinQueryWindowTextLength(hWnd)+1;
        PSZ psz  = (PSZ)malloc(nLen);
        if (psz)
        {
          WinQueryWindowText(hWnd,nLen,psz);
          free(pszString);
          pszString = psz;
          fModifyed = TRUE;
        }
      }
      if (!WinSendMsg(hCBPresent,BM_QUERYCHECK,0,0) != !fPresent)
      {
        fPresent = ! fPresent;
        fModifyed = TRUE;
      }
      break;
    case OPT_BOOLEAN:
    {
      int  i     = (int)WinSendMsg(hWnd,BM_QUERYCHECK,0,0);
      BOOL fPres = (i!=2);
      int  nVal1 = nVal & BOOL_SHOWINVERSE;
      if (nVal1) i = !i;
      if (i) nVal1 |= BOOL_ON;
      if (nVal1 != nVal || fPres != fPresent)
      {
        nVal      = nVal1;
        fPresent  = fPres;
        fModifyed = TRUE;
      }
      break;
    }
    case OPT_ENUMERATION:
    {
      ULONG nIt = (ULONG)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      if (nIt!=nVal && nIt<MAXENUM && apszEnums[nIt])
      {
        nVal      = nIt;
        fModifyed = TRUE;
      }
      if (!WinSendMsg(hCBPresent,BM_QUERYCHECK,0,0) != !fPresent)
      {
        fPresent = ! fPresent;
        fModifyed = TRUE;
      }
      break;
    }
  }
}
void OPTION::SetControl(HWND hWnd, HWND hCBPresent)
{
  switch(type)
  {
    case OPT_EQUATION:
      WinSetWindowText(hWnd,pszString ? pszString : "");
      WinSendMsg(hWnd,EM_QUERYCHANGED,0,0); // To clear this state
      WinSendMsg(hCBPresent,BM_SETCHECK,MPARAM(!!fPresent),0);
      break;
    case OPT_BOOLEAN:
    {
      WinSetWindowText(hWnd, pszComment ? pszComment : szName);
      int n = 2;
      if (fPresent)
      {
        n = !!(nVal & BOOL_ON);
        if (nVal & BOOL_SHOWINVERSE) n = !n;
      }
      WinSendMsg(hWnd,BM_SETCHECK,MPARAM(n), 0);
      break;
    }
    case OPT_ENUMERATION:
    {
      WinSendMsg(hWnd,LM_DELETEALL,0,0);
      for (int i=0; i<MAXENUM; i++)
        if (apszEnums[i]) WinSendMsg(hWnd,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(apszEnums[i]));
      WinSendMsg(hWnd,LM_SELECTITEM,MPARAM(nVal),MPARAM(TRUE));
      WinSendMsg(hCBPresent,BM_SETCHECK,MPARAM(!!fPresent),0);
      break;
    }
  }
}
void OPTION::ReadName(PSZ *ppsz)
{
  PSZ s,p;
  s = sf_skipspaces (*ppsz);
  for (p = s; sf_isalnum (*p) && p-s+1<sizeof(szName); p ++);
  if (p == s) throw "name expected";
  strncpy(szName,*ppsz,p-s);
  szName[p-s] = 0;
  *ppsz = p;
}
void OPTION::ReadComment(PSZ psz)
{
  PSZ s;
  s = sf_skipspaces (psz);
  SetComment(*s == ';' ? sf_skipspaces(s+1) : "");
}
void OPTION::ReadString(PSZ *ppsz)
{
  PSZ p, s = sf_skipspaces(*ppsz);
  if (*s!='"') throw "'\"' expected";
  for (p=++s; *p && *p!='"'; p++);
  SetString(p-s ? s : "", p-s);
  *ppsz = p + (*p ? 1 : 0);
}
void OPTION::ReadEnumValue(PSZ *ppsz)
{
  PSZ p, s = sf_skipspaces(*ppsz);
  if (*s!='"') throw "'\"' expected";
  for (p=++s; *p && *p!='"'; p++);
  AddEnumValue(p-s ? s : "", p-s);
  *ppsz = p + (*p ? 1 : 0);
}
void OPTION::MakeTip(PSZ pszTipVal)
{
  SetTip("");
  if (!pszComment || !pszComment[0]) return;
  PSZ psz = (PSZ)malloc(strlen(pszComment) + strlen(pszTipVal ? pszTipVal : "") + 30);
  if (psz)
  {
    strcpy(psz,pszComment);
    if (pszTipVal && pszTipVal[0])
      sprintf(psz+strlen(psz)," (Default: %s)",pszTipVal);
    SetTip(psz);
    free(psz);
  }
}


//------------------------- N O D E L I S T --------------------//

NODELIST::NODELIST()
{
  memset(this,0,sizeof(*this));
}
NODELIST::~NODELIST()
{
  free_all();
  memset(this,0,sizeof(*this));
}

BOOL NODELIST::Initialize(HWND hMsgWnd, PSZ szCfgFile)
{
  PSZ       pszFileBuf;
  free_all();
  {
    APIRET  rc;
    HFILE   hf;
    ULONG   ul,fsize;
    DosError(FERR_DISABLEHARDERR);
    if (rc = (int)DosOpen(szCfgFile,&hf,&ul,0,FILE_NORMAL,FILE_OPEN,
                      OPEN_FLAGS_NO_CACHE |OPEN_FLAGS_SEQUENTIAL|
                      OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE, (PEAOP2)0))
    {
      DosError(FERR_ENABLEHARDERR);
      IOErMsg(rc, hMsgWnd, "Read project configuration file", "Can't open file", szCfgFile, MB_OK);
      return FALSE;
    }
    DosSetFilePtr(hf,0L,FILE_END,  &fsize);
    DosSetFilePtr(hf,0L,FILE_BEGIN,&ul);
    if (!(pszFileBuf = (char*)malloc(fsize+1)))
    {
      DosClose(hf);
      DosError(FERR_ENABLEHARDERR);
      MessageBox(hMsgWnd,"Read project configuration file",MB_ERROR|MB_OK,"No enouth memory error");
      return FALSE;
    }
    if (rc = (int)DosRead(hf,(PVOID)pszFileBuf,fsize,&ul))
    {
      DosError(FERR_ENABLEHARDERR);
      free(pszFileBuf);
      IOErMsg(rc, hMsgWnd, "Read project configuration file", "File read error", szPrjFile, MB_OK);
      return FALSE;
    }
    pszFileBuf[ul] = 0;
    DosClose(hf);
    DosError(FERR_ENABLEHARDERR);
  }

  // pszFileBuf == z-terminated *.cfg text
  int      parse_error_x;
  int      parse_error_line = 0;
  int      nBugsFound       = 0;
  PSZ      pszSrcLine, pszNxtLine, s;
  int      i;
  PNODE    pLastNode        = 0;
  POPTION  pOption;
  POPTION *ppNewOption      = 0;
  for (pszNxtLine=pszFileBuf; *(pszSrcLine = pszNxtLine); parse_error_line++)
  {
    {
      char *pchE  = pszSrcLine+strcspn(pszSrcLine,"\n\r");
      pszNxtLine  = pchE+1;
      if      (!*pchE)                         pszNxtLine--;
      else if (pchE[0]=='\r' && pchE[1]=='\n') pszNxtLine++;
      *pchE = 0;
    }
    try // parse pszSrcLine
    {
      pOption       = 0;
      parse_error_x = 0;
      s             = sf_skipspaces (pszSrcLine);
      if (!*s) continue;
      switch (*s)
      {
        case '\'': continue;
        case '!' : // [Font] ; Text to draw
          if (!ppNewOption) throw "New topic expected";
          ++s;
          i = 0;
          while (*s && !sf_isspace (*s) && *s != ';')
          {
            switch (sf_toupper (*s)){
            case '2': i |= STATIC_LARGE;  break;
            case 'B': i |= STATIC_BOLD;   break;
            case 'I': i |= STATIC_ITALIC; break;
            case 'Z': i |= STATIC_CENTER; break;
            case 'R': i |= STATIC_RIGHT;  break;
            }
            s++;
          }
          pOption            = new OPTION;
          pOption -> ReadComment(s);
          pOption -> nVal    = i;
          pOption -> type    = OPT_STATIC;
          *ppNewOption       = pOption;
          ppNewOption        = &pOption->next;
          break;
        case '[':
          if (!ppNewOption) throw "New topic expected";
          pOption = new OPTION;
          s       = sf_skipspaces (s+1);
          pOption -> nVal |= GROUP_BEGIN;
          // Skip this fucked bugs
          if (*s>='1' && *s<='9')
          {
            strtol(s,&s,0);
            s = sf_skipspaces(s);
          }
          if (*s == ']') s = sf_skipspaces (s+1);
          switch (*s) {
          case '+': pOption -> nVal |= GROUP_STYLE_1; ++s; break;
          case '-': pOption -> nVal |= GROUP_STYLE_2; ++s; break;
          case '0': pOption -> nVal |= GROUP_STYLE_3; ++s; break;
          }
          pOption -> type = OPT_GROUP;
          pOption -> ReadComment(s);
          *ppNewOption    = pOption;
          ppNewOption     = &pOption->next;
          break;
        case ']':
          if (!ppNewOption) throw "New topic expected";
          pOption         = new OPTION;
          pOption -> type = OPT_GROUP;
          pOption -> nVal = GROUP_END;
          *ppNewOption    = pOption;
          ppNewOption     = &pOption->next;
          break;
        case '#': // Entry field : Text
        case '$': //               Hex
        case '%': //               Decimal
          if (!ppNewOption) throw "New topic expected";
          pOption       = new OPTION;
          pOption->nVal = pOption->nVal = *s=='#' ? ENTRY_TEXT : (*s=='$' ? ENTRY_HEX : ENTRY_DECIMAL);
          s++;
          parse_error_x = s-pszSrcLine;
          pOption -> ReadName(&s);
          s             = sf_skipspaces(s);
          parse_error_x = s-pszSrcLine;
          if (*s != '=')  throw "'=' expected";
          ++s;
          parse_error_x = s-pszSrcLine;
          pOption->ReadString(&s);
          pOption -> type     = OPT_EQUATION;
          pOption -> fPresent = TRUE;
          pOption -> ReadComment(s);
          pOption -> MakeTip    (pOption->pszString);
          *ppNewOption        = pOption;
          ppNewOption         = &pOption->next;
          break;
        case '+':
        case '-':
        {
          if (!ppNewOption) throw "New topic expected";
          pOption = new OPTION;
          if (s[0] == '+') pOption->nVal |= BOOL_ON;
          if (s[1] == '~') pOption->nVal |= BOOL_SHOWINVERSE, s++;
          s++;
          parse_error_x       = s-pszSrcLine;
          pOption -> ReadName(&s);
          pOption -> type     = OPT_BOOLEAN;
          pOption -> fPresent = TRUE;
          pOption -> ReadComment(s);
          if (!pOption->pszComment) pOption -> SetComment(pOption->szName);
          pOption -> MakeTip    ( (!(pOption->nVal&BOOL_ON) == !(pOption->nVal&BOOL_SHOWINVERSE)) ? "OFF" : "ON");
          *ppNewOption        = pOption;
          ppNewOption         = &pOption->next;
          break;
        }
        case '(':
        {
          BOOL fDefKnown = FALSE;
          s++;
          if (!ppNewOption) throw "New topic expected";
          pOption       = new OPTION;
          parse_error_x = s-pszSrcLine;
          pOption->ReadName(&s);
          s = sf_skipspaces (s);
          if (*s != ':') throw "':' expected";
          do  {
            ++s;
            if (*s == '!') pOption -> nVal = pOption -> nAlts, s++, fDefKnown = TRUE;
            parse_error_x = s-pszSrcLine;
            pOption->ReadEnumValue(&s);
            s = sf_skipspaces (s);
          } while (*s == ',');
          if (*s == ')') s++;
          pOption -> type     = OPT_ENUMERATION;
          pOption -> fPresent = TRUE;
          pOption -> ReadComment(s);
          pOption -> MakeTip    (fDefKnown ? pOption->apszEnums[pOption->nVal] : 0);
          *ppNewOption        = pOption;
          ppNewOption         = &pOption->next;
          break;
        }
        default: // New node
        {
          PNODE pNode = new NODE;
          i = 0;
          while (*s == '>') s++, i++;
          s = sf_skipspaces (s);
          while (*s == '^'){ s++; s=sf_skipspaces(s);} // Skip it now
          if (pLastNode)
          {
            if (i - pLastNode->nLevel > 1) i = pLastNode->nLevel+1;
            pLastNode->next = pNode;
          }
          else
          {
            i = 0;
            pNodes = pNode;
          }
          pLastNode = pNode;
          strncpy (pNode -> szNodeName, s, sizeof(pNode->szNodeName)-1);
          pNode -> szNodeName [sizeof(pNode->szNodeName)-1] = 0;
          pNode -> fExpanded = TRUE;
          pNode -> nLevel    = i;
          ppNewOption = &pNode->pOptList;
        }
      }
    }
    catch (char *psz)
    {
      delete pOption;
      if (!nBugsFound)
        errOffice.ClearAll();
      errOffice.AddError(psz, parse_error_x+errOffice.l1stPos, parse_error_line+errOffice.l1stLine, szCfgFile, MSG_ERROR, 0);
      nBugsFound++;
      if (!strncmp(psz,"Out of memory error",19)) break;
    }
  }// for
  free(pszFileBuf);
  if (nBugsFound)
  {
    errOffice.DoneUpdating();
    char szEr[200];
    sprintf(szEr,"Parsing %s file: %u error(s) found.",szCfgFile,nBugsFound);
    MessageBox(hMsgWnd,"Read project configuration file",MB_ERROR|MB_OK,szEr);
    free_all();
    WinSetFocus(HWND_DESKTOP,clMsgList.hWnd);
    return FALSE;
  }
  return TRUE;
}

void NODELIST::BindToDialog(HWND hDlg, HWND hLBox, HWND hDisp)
{
  NODELIST::hDlg  = hDlg;
  NODELIST::hLBox = hLBox;
  NODELIST::hDisp = hDisp;
  pnSelected      = 0;
  {
    FONTMETRICS fm;
    HPS         hps = WinGetPS(hDlg);
    GpiQueryFontMetrics(hps,sizeof(FONTMETRICS),&fm);
    cyFont = (fm.lMaxBaselineExt+fm.lExternalLeading+1)*1.2+2;
    cxFont = fm.lEmInc;
    WinReleasePS(hps);
  }
}
void NODELIST::FillLBox(PNODE pStartFrom)
{
  BOOL fFillNow   = !pStartFrom;
  int  nIt        = -1;
  int  aShrinkLevel [100];
  int  nShrinkMax = -1;
  if (fFillNow) WinSendMsg(hLBox,LM_DELETEALL,0,0);
  WinSendMsg(hLBox,LLM_LOCKUPDATE,MPARAM(TRUE),0);
  for (PNODE pNode = pNodes; pNode; pNode = pNode->next)
  {
    if (nShrinkMax>=0)
    {
      if (pNode->nLevel >  aShrinkLevel[nShrinkMax]) continue;
      if (pNode->nLevel <= aShrinkLevel[nShrinkMax]) nShrinkMax--;
    }
    nIt++;
    if (!pNode->fExpanded && (nShrinkMax+1)<(sizeof(aShrinkLevel)/sizeof(aShrinkLevel)[0]))
      aShrinkLevel[++nShrinkMax] = pNode->nLevel;
    if (!fFillNow)
    {
      if (pStartFrom != pNode) continue;
      else
      {
        for (int nMax = (int)WinSendMsg(hLBox,LM_QUERYITEMCOUNT,0,0) - 1; nMax >= nIt; nMax--)
           WinSendMsg(hLBox,LM_DELETEITEM,MPARAM(nMax),0);
        fFillNow = TRUE;
      }
    }
    WinSendMsg(hLBox,
               LM_SETITEMHANDLE,
               MPARAM( WinSendMsg(hLBox,LM_INSERTITEM,MPARAM(nIt),pNode->szNodeName) ),
               MPARAM(pNode));
  }
  WinSendMsg(hLBox,LLM_LOCKUPDATE,MPARAM(FALSE),0); // Unlock update
}
void NODELIST::LBSelect()
{
  int   nIt   = (int)  WinSendMsg(hLBox,LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
  PNODE pNode = (PNODE)WinSendMsg(hLBox,LM_QUERYITEMHANDLE,MPARAM(nIt),      0);
  if (pNode && pnSelected!=pNode)
  {
    fConfigOk = FALSE;
    AskControlsNow();
    new_controls(pNode);
    WMPaint();
    fConfigOk = TRUE;
  }
}
void NODELIST::LBEnter()
{
  int   nIt   = (int)  WinSendMsg(hLBox,LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
  PNODE pNode = (PNODE)WinSendMsg(hLBox,LM_QUERYITEMHANDLE,MPARAM(nIt),      0);
  if (pNode  &&  pNode->next  &&  pNode->nLevel < pNode->next->nLevel)
  {
    pNode->fExpanded = !pNode->fExpanded;
    FillLBox(pNode);
    WinSendMsg(hLBox,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
  }
}
void NODELIST::WMControl(MPARAM m1,MPARAM m2)
{
  if (fConfigOk)
    for (PCONTROL pCtrl=pControls; pCtrl; pCtrl=pCtrl->next)
      if (pCtrl->hCtrl == HWND(m2)
          && (pCtrl->pOption->type==OPT_EQUATION || pCtrl->pOption->type==OPT_ENUMERATION)
          && SHORT2FROMMP(m1)==EN_CHANGE
         )
      {
        WinSendMsg(pCtrl->hCBPresent,BM_SETCHECK,MPARAM(TRUE),0);
        return;
      }
}
void NODELIST::AskControlsNow ()
{
  for (PCONTROL pCtrl=pControls; pCtrl; pCtrl=pCtrl->next)
    if (pCtrl->pOption) pCtrl->pOption->AskControl(pCtrl->hCtrl,pCtrl->hCBPresent);
}
void NODELIST::ForgetControls()
{
  del_controls(FALSE);
}
void NODELIST::Prj2Options(PPRJSTRUCT pPrj)
{
  PNODE     pNode;
  POPTION   pOpt;
  for (pNode=pNodes; pNode; pNode=pNode->next)
    for (pOpt=pNode->pOptList; pOpt; pOpt=pOpt->next)
      pOpt->fPresent = FALSE;
  for (PPRJLINE pPrjLine = pPrj->pPrjLines; pPrjLine; pPrjLine=pPrjLine->next)
    if (pOpt = find_opt(pPrjLine))
    {
      if (pOpt->type==OPT_BOOLEAN)
      {
        pOpt->nVal &= ~BOOL_ON;
        if (pPrjLine->pszKeyValue[0]=='+') pOpt->nVal |= BOOL_ON;
        pOpt->fPresent = TRUE;
      }
      else if (pOpt->type==OPT_ENUMERATION)
      {
        for (int i=0; i<MAXENUM; i++)
          if (pOpt->apszEnums[i] && !sf_stricmp(pPrjLine->pszKeyValue, pOpt->apszEnums[i]))
          {
            pOpt->nVal     = i;
            pOpt->fPresent = TRUE;
          }
      }
      else if (pOpt->type==OPT_EQUATION)
      {
        pOpt->SetString(pPrjLine->pszKeyValue);
        pOpt->fPresent = TRUE;
      }
    }
}
  static void new_prj_comment(PPRJLINE pNew, POPTION pOpt)
  {
    if (pOpt->pszComment && pOpt->pszComment[0])
    {
      PSZ psz = (PSZ)malloc(strlen(pOpt->pszComment) + 3);
      if (psz)
      {
        sprintf(psz,"%% %s",pOpt->pszComment);
        pNew->SetComment(psz);
        free(psz);
      }
    }
  }
void NODELIST::Options2Prj(PPRJSTRUCT pPrj)
{
  PPRJLINE *ppEqPlace  = 0; // Место для добавления EQUATION(s) в проект
  PPRJLINE *ppOptPlace = 0; // Место для добавления XDSOPTION(s) в проект
  PPRJLINE *ppTopPlace = 0; // Место для добавления строк перед !module(s)
  PPRJLINE *pp;
  PNODE    pNode;
  POPTION  pOpt;
  PPRJLINE pPrjLine;
  for (pp = &pPrj->pPrjLines; pPrjLine = *pp; )
  {
    if (pPrjLine->type == MODULE)
    {
      if (!ppEqPlace ) ppEqPlace  = pp;
      if (!ppOptPlace) ppOptPlace = pp;
      if (!ppTopPlace) ppTopPlace = pp;
    }
    if ((pOpt=find_opt(pPrjLine)) && pOpt->fModifyed && !pOpt->fPresent)
    {
      if (!ppOptPlace  && pPrjLine->type == XDSOPTION) ppOptPlace  = pp;
      if (!ppEqPlace   && pPrjLine->type == EQUATION ) ppEqPlace   = pp;
      if (!ppTopPlace  && !(pPrjLine->next)          ) ppTopPlace  = pp;
      *pp = pPrjLine->next;
      delete pPrjLine;
      continue;
    }
    if      (pPrjLine->type == XDSOPTION) ppOptPlace = &pPrjLine->next;
    else if (pPrjLine->type == EQUATION)  ppEqPlace  = &pPrjLine->next;

    if (pOpt && pOpt->fModifyed)
    {
      // Change project value...
      if (pOpt->type==OPT_BOOLEAN)
      {
        pPrjLine->SetKeyValue((pOpt->nVal & BOOL_ON) ? "+" : "-");
        pOpt->fModifyed = FALSE;
      }
      else if (pOpt->type==OPT_ENUMERATION || pOpt->type==OPT_EQUATION)
      {
        pPrjLine->SetKeyValue((pOpt->type==OPT_EQUATION) ? pOpt->pszString : pOpt->apszEnums[pOpt->nVal]);
        pOpt->fModifyed = FALSE;
      }
    }
    else
      if (!*(pp = &pPrjLine->next) && !ppTopPlace) ppTopPlace = pp;
  }
  if (!ppTopPlace) ppTopPlace = &pPrj->pPrjLines;
  if (!ppEqPlace)  ppEqPlace  = ppTopPlace;
  if (!ppOptPlace) ppOptPlace = ppTopPlace;
  // Add a new options to the project
  OPT_TYPE a_opt_types[] = {OPT_BOOLEAN, OPT_ENUMERATION, OPT_EQUATION};
  for (int n_opt_type=0; n_opt_type < sizeof(a_opt_types)/sizeof(a_opt_types[0]); n_opt_type++)
  {
    OPT_TYPE cur_opt_type = a_opt_types[n_opt_type];
    for (pNode = pNodes; pNode; pNode = pNode->next)
      for (pOpt = pNode->pOptList; pOpt; pOpt = pOpt->next)
        if (pOpt->fModifyed && pOpt->fPresent)
        {
          if (pOpt->type != cur_opt_type) continue;
          PPRJLINE pNew;
          if (pOpt->type==OPT_BOOLEAN)
          {
            pNew        = new PRJLINE;
            pNew->type  = XDSOPTION;
            pNew->SetText("%--- Oops! XDS project generation error (Opt)");
            pNew->SetKeyName(pOpt->szName);
            pNew->SetKeyValue((pOpt->nVal & BOOL_ON) ? "+" : "-");
            new_prj_comment(pNew,pOpt);
            pNew->next  = *ppOptPlace;
            *ppOptPlace = pNew;
            ppOptPlace  = &pNew->next;
            pOpt->fModifyed = FALSE;
          }
          else if (pOpt->type==OPT_ENUMERATION || pOpt->type==OPT_EQUATION)
          {
            pNew       = new PRJLINE;
            pNew->type = EQUATION;
            pNew->SetText("%--- Oops! XDS project generation error (Equ)");
            pNew->SetKeyName(pOpt->szName);
            pNew->SetKeyValue((pOpt->type==OPT_ENUMERATION) ? pOpt->apszEnums[pOpt->nVal] : pOpt->pszString);
            new_prj_comment(pNew,pOpt);
            pNew->next = *ppEqPlace;
            *ppEqPlace = pNew;
            ppEqPlace  = &pNew->next;
            pOpt->fModifyed = FALSE;
          }
        }
  }
}
void NODELIST::WMPaint(BOOL fErase)
{
  HPS hps = WinGetPS(hDlg);
  for (int ng = 0; ng<nGroups; ng++)
    if (aGroups[ng].yBottom <= aGroups[ng].yTop)
    {
      ULONG ulStyle = aGrStyles[ng];
      ULONG clr1 = CLR_WHITE;
      ULONG clr2 = CLR_DARKGRAY;
      if (ulStyle & (GROUP_STYLE_1|GROUP_STYLE_3)) clr1 = CLR_DARKGRAY, clr2 = CLR_WHITE;
      if (fErase) clr1 = clr2 = CLR_PALEGRAY;
      if (ulStyle & (GROUP_STYLE_2|GROUP_STYLE_3))
      {
        POINTL ptl;
        ptl.x = aGroups[ng].xLeft;
        ptl.y = aGroups[ng].yBottom;
        GpiSetCurrentPosition(hps,&ptl);
        GpiSetColor(hps,clr1);
        ptl.y = aGroups[ng].yTop;    GpiLine(hps,&ptl);
        ptl.x = aGroups[ng].xRight;  GpiLine(hps,&ptl);
        GpiSetColor(hps,clr2);
        ptl.y = aGroups[ng].yBottom;  GpiLine(hps,&ptl);
        ptl.x = aGroups[ng].xLeft;    GpiLine(hps,&ptl);
      }
      else
        BrDrawNiceBorder(hps,
                         aGroups[ng].xLeft,
                         aGroups[ng].yBottom,
                         aGroups[ng].xRight - aGroups[ng].xLeft,
                         aGroups[ng].yTop   - aGroups[ng].yBottom,
                         clr1, clr2);
    }
  WinReleasePS(hps);
}
void NODELIST::GenComment(BOOL fReset, HWND hComment)
{
  static HWND hCtrlNow = 0;
  HWND        hCtrl    = 0;
  POINTL      ptl;
  SWP         swp;
  POPTION     pOption;
  if (fReset)
  {
    hCtrlNow = 0;
    WinSetWindowText(hComment,"");
  }
  else
  {
    WinQueryPointerPos(HWND_DESKTOP,&ptl);
    Win2WinCoord      (HWND_DESKTOP, hDlg, &ptl);
    for (PCONTROL pCtrl=pControls; pCtrl; pCtrl=pCtrl->next)
      if ((pOption=pCtrl->pOption) && (pOption->type==OPT_EQUATION || pOption->type==OPT_BOOLEAN ||
                                     pOption->type==OPT_ENUMERATION))
      {
        WinQueryWindowPos(pCtrl->hCtrl,&swp);
        if (pOption->type==OPT_ENUMERATION)
        {
          swp.y  += cxFont*6; // See new_controls()
          swp.cy -= cxFont*6; // See new_controls()
        }
        if (ptl.x >= swp.x && ptl.x <= swp.x+swp.cx &&
            ptl.y >= swp.y && ptl.y <= swp.y+swp.cy &&
            pOption->pszTip)
        {
          hCtrl = pCtrl->hCtrl;
          break;
        }
      }
    if (hCtrl!=hCtrlNow)
    {
      hCtrlNow = hCtrl;
      if (hCtrl) WinSetWindowText(hComment,pOption->pszTip);
      else       WinSetWindowText(hComment,"");
    }
  }
}
void NODELIST::del_controls (BOOL fKillWindows)
{
  for (PCONTROL pCtrl1, pCtrl=pControls; pCtrl; pCtrl=pCtrl1)
  {
    if (fKillWindows)
    {
      if (pCtrl->hCtrl)      WinDestroyWindow(pCtrl->hCtrl);
      if (pCtrl->hCBPresent) WinDestroyWindow(pCtrl->hCBPresent);
      if (pCtrl->hStatic)    WinDestroyWindow(pCtrl->hStatic);
    }
    pCtrl1 = pCtrl->next;
    delete pCtrl;
  }
  pControls  = 0;
  pnSelected = 0;
  WMPaint(TRUE);
  memset(aGroups,0,sizeof(aGroups));
  nGroups    = 0;
}
  void NODELIST::mk_group(PLONG pyTop, PLONG pxLeftBound, PLONG pxRightBound, ULONG ulGrStyle)
  {
    int ng = nGroups;
    if (ulGrStyle & GROUP_BEGIN)
    {
      if (ng >= sizeof(aGroups)/sizeof(aGroups[0])) return;
      aGroups  [ng].xLeft   = *pxLeftBound;
      aGroups  [ng].xRight  = *pxRightBound;
      aGroups  [ng].yTop    = *pyTop;
      aGroups  [ng].yBottom = *pyTop + ((ulGrStyle&GROUP_END) ? -2 : 1);
      aGrStyles[ng]         = ulGrStyle;
      *pyTop        -= cyFont/2;
      *pxLeftBound  += cxFont;
      *pxRightBound -= cxFont;
      nGroups++;
    }
    else if (ulGrStyle & (GROUP_END|GROUP_CLOSEALL))
      do
      {
        while(ng>=0 && (aGroups[ng].yBottom <= aGroups[ng].yTop)) ng--;
        if (ng<0) return;
        aGroups[ng].yBottom = *pyTop;
        *pyTop             -= cyFont/2;
        *pxLeftBound       -= cxFont;
        *pxRightBound      += cxFont;
      } while(ulGrStyle & GROUP_CLOSEALL);
  }

  void  text_size (HWND hWnd, PSZ sz, PRECTL pRcl, int incX = 0, int incY = 0)
  {
    HPS    hps = WinGetPS(hWnd);
    POINTL aptl[TXTBOX_COUNT];
    if (!sz)    sz = "";
    GpiQueryTextBox(hps,strlen(sz),sz,TXTBOX_COUNT,aptl);
    pRcl -> xLeft  = pRcl -> yBottom = 0;
    pRcl -> xRight = aptl[TXTBOX_TOPRIGHT].x - aptl[TXTBOX_TOPLEFT].x    + incX;
    pRcl -> yTop   = aptl[TXTBOX_TOPLEFT].y  - aptl[TXTBOX_BOTTOMLEFT].y + incY;
    WinReleasePS(hps);
  }
  int NODELIST::align_rect (RECTL *pRcl, int yTop, int xLeftBound, int xRightBound, BOOL fRight)
  {
    LONG dx       = pRcl->xRight - pRcl->xLeft;
    LONG dy       = pRcl->yTop   - pRcl->yBottom;
    pRcl->yTop    = yTop;
    yTop         -= dy;
    pRcl->yBottom = yTop;
    if (fRight)  {                              // Move to right side
      pRcl->xRight  = xRightBound;
      pRcl->xLeft   = pRcl->xRight - dx;
    } else  {                                  // Move to left side
      pRcl->xLeft   = xLeftBound;
      pRcl->xRight  = pRcl->xLeft + dx;
    }
    pRcl->xLeft   = max(pRcl->xLeft,  xLeftBound);
    pRcl->xRight  = min(pRcl->xRight, xRightBound);
    return yTop;
  }


void NODELIST::new_controls (PNODE pNode)
{
  RECTL     rc;
  SWP       swp;
  WinQueryWindowPos(hDisp,&swp);
  LONG      yTop        = swp.y + swp.cy - cyFont;     // Верхушка очередного контрола
  LONG      xLeftBound  = swp.x + cxFont;              // Левая и правая границы
  LONG      xRightBound = swp.x + swp.cx - cxFont;     //   свободного пространства
  int       nTotal      = 0;                           // Всего создано контролов
  PCONTROL  pCtrlTop    = 0;                           // Верхушка списка pControls
  PCONTROL  pNew        = 0;                           // Сюда будем создавать контрол(ы)
  if (pNode == pnSelected || !pNode) return;
  del_controls(TRUE);
  pnSelected = pNode;
  for (POPTION pOption = pNode->pOptList; pOption; pOption = pOption->next)
  {
    pNew = 0;
    switch (pOption->type){
    case OPT_EQUATION:
    {
      MRESULT EXPENTRY wpHexEFProc     (HWND, ULONG, MPARAM, MPARAM);
      MRESULT EXPENTRY wpDecimalEFProc (HWND, ULONG, MPARAM, MPARAM);
      // Begin group
      //mk_group(&yTop,&xLeftBound,&xRightBound,GROUP_BEGIN);
      int   yTop1;
      RECTL rcEF;
      RECTL rcPres;
      RECTL rcStatic;
      text_size(hDlg,pOption->pszComment,&rcStatic);
      text_size(hDlg,"WWW Present",      &rcPres);
      rcEF            = rcPres;
      rcEF.yTop      += 2;
      rcEF.xRight     = 1000;
      yTop1 = align_rect (&rcPres,   yTop, xLeftBound, xRightBound,           TRUE ) - cyFont/2;
      yTop  = align_rect (&rcEF,     yTop, xLeftBound, rcPres.xLeft-cxFont*2, FALSE) - cyFont/2;
      yTop  = min(yTop,yTop1);
      yTop  = align_rect (&rcStatic, yTop, xLeftBound, xRightBound,           FALSE) - cyFont/2;
      // End group:
      ///mk_group(&yTop,&xLeftBound,&xRightBound,GROUP_END);
      pNew          = new CONTROL;
      // EF:
      pNew->hCtrl   = create_ctrl(WC_ENTRYFIELD, "", WS_TABSTOP | ES_AUTOSCROLL | ES_MARGIN, rcEF,  IDD_BASE_PRJMOD+nTotal++);
      if      (pOption->nVal & ENTRY_HEX)
        wpHexEFProc    (pNew->hCtrl, MM_START, MPARAM(ULONG(WinSubclassWindow(pNew->hCtrl,(PFNWP)wpHexEFProc    ))),0L);
      else if (pOption->nVal & ENTRY_DECIMAL)
        wpDecimalEFProc(pNew->hCtrl, MM_START, MPARAM(ULONG(WinSubclassWindow(pNew->hCtrl,(PFNWP)wpDecimalEFProc))),0L);
      // CB "Present"
      pNew->hCBPresent = create_ctrl(WC_BUTTON, "Present", WS_TABSTOP | BS_AUTOCHECKBOX, rcPres, IDD_BASE_PRJMOD+nTotal++);
      // Comment static:
      pNew->hStatic    = create_ctrl(WC_STATIC, (pOption->pszComment ? pOption->pszComment : pOption->szName),
                                     SS_TEXT | DT_WORDBREAK, rcStatic, 0);
      break;
    }
    case OPT_BOOLEAN:
      pNew        = new CONTROL;
      text_size(hDlg,pOption->pszComment,&rc,cxFont*3,2);
      yTop        = align_rect (&rc, yTop, xLeftBound, xRightBound, FALSE) - cyFont/2;
      pNew->hCtrl = create_ctrl(WC_BUTTON, "", WS_TABSTOP | BS_AUTO3STATE, rc,  IDD_BASE_PRJMOD+nTotal++);
      break;
    case OPT_ENUMERATION:
      // Begin group
      //mk_group(&yTop,&xLeftBound,&xRightBound,GROUP_BEGIN);
      int   yTop1;
      RECTL rcCB;
      RECTL rcPres;
      RECTL rcStatic;
      text_size(hDlg,pOption->pszComment,&rcStatic);
      text_size(hDlg,"WWW Present",      &rcPres);
      rcCB            = rcPres;
      rcCB.yTop      += 6;
      rcCB.xRight     = 1000;
      yTop1 = align_rect (&rcPres,   yTop, xLeftBound, xRightBound,           TRUE ) - cyFont/2;
      yTop  = align_rect (&rcCB,     yTop, xLeftBound, rcPres.xLeft-cxFont*2, FALSE) - cyFont/2;
      yTop  = min(yTop,yTop1);
      yTop  = align_rect (&rcStatic, yTop, xLeftBound, xRightBound,           FALSE) - cyFont/2;
      rcPres.yBottom -= 3;
      rcCB.yBottom   -= cxFont*6; // See GenComment()
      // End group:
      //mk_group(&yTop,&xLeftBound,&xRightBound,GROUP_END);
      pNew             = new CONTROL;
      // Combobox:
      pNew->hCtrl      = create_ctrl(WC_COMBOBOX, "", WS_TABSTOP | CBS_DROPDOWNLIST, rcCB,  IDD_BASE_PRJMOD+nTotal++);
      // CB "Present"
      pNew->hCBPresent = create_ctrl(WC_BUTTON, "Present", WS_TABSTOP | BS_AUTOCHECKBOX, rcPres, IDD_BASE_PRJMOD+nTotal++);
      // Comment static:
      pNew->hStatic    = create_ctrl(WC_STATIC, (pOption->pszComment ? pOption->pszComment : pOption->szName),
                                     SS_TEXT | DT_WORDBREAK, rcStatic, 0);
      break;
    case OPT_STATIC:
      if (!pOption->pszComment || !pOption->pszComment[0])
        yTop -= cyFont;
      else
      {
        char  szFontUse[50] = "";
        ULONG ulForePP      = 0x00000000; // Forecolor
        ULONG fl            = SS_TEXT | DT_VCENTER;
        strcpy(szFontUse, (pOption->nVal & STATIC_LARGE) ? "14.Helv" : "8.Helv");
        if (pOption->nVal & STATIC_ITALIC) strcat(szFontUse," Italic");
        if (pOption->nVal & STATIC_BOLD)
        {
          if (pOption->nVal & STATIC_LARGE) strcat(szFontUse," Bold");
          else                              ulForePP = 0x00c00000;
        }
        if (pOption->nVal & STATIC_CENTER) fl |= DT_CENTER;
        else                               fl |= (pOption->nVal & STATIC_RIGHT) ? DT_RIGHT : DT_LEFT;
        pNew = new CONTROL;
        memset(&rc,0,sizeof(rc));
        pNew->hStatic = create_ctrl(WC_STATIC, (pOption->pszComment ? pOption->pszComment : ""),
                                     fl | SS_TEXT, rc, 0);
        WinSetPresParam(pNew->hStatic,PP_FOREGROUNDCOLOR,sizeof(ulForePP),PVOID(&ulForePP));
        if (szFontUse[0])
          WinSetPresParam(pNew->hStatic,PP_FONTNAMESIZE,strlen(szFontUse)+1,PVOID(szFontUse));
        text_size(pNew->hStatic, (pOption->pszComment ? pOption->pszComment : ""), &rc, 10000);
        yTop     = align_rect (&rc, yTop, xLeftBound, xRightBound, FALSE);
        WinSetWindowPos(pNew->hStatic,0, rc.xLeft, rc.yBottom, rc.xRight-rc.xLeft,
                        rc.yTop-rc.yBottom,SWP_SIZE|SWP_MOVE|SWP_SHOW);
        if (pOption->next && pOption->next->type!=OPT_STATIC) yTop -= cyFont*2/3;
      }
      break;
    case OPT_GROUP:
      mk_group(&yTop,&xLeftBound,&xRightBound,pOption->nVal);
      break;
    }
    if (pNew)
    {
      pOption->SetControl(pNew->hCtrl,pNew->hCBPresent);
      pNew->pOption                = pOption;
      if (pCtrlTop) pCtrlTop->next = pNew;
      else          pControls      = pNew;
      pCtrlTop = pNew;
    }
  } // for(..pOption..)
  mk_group(&yTop,&xLeftBound,&xRightBound,GROUP_CLOSEALL);
}
POPTION NODELIST::find_opt(PPRJLINE pPrjLine)
{
  if (pPrjLine->type==EQUATION || pPrjLine->type==XDSOPTION)
    for (PNODE pNode=pNodes; pNode; pNode=pNode->next)
      for (POPTION pOpt=pNode->pOptList; pOpt; pOpt=pOpt->next)
        if (!sf_stricmp(pPrjLine->pszKeyName, pOpt->szName) &&
            ((pPrjLine->type==XDSOPTION && pOpt->type==OPT_BOOLEAN) ||
             (pPrjLine->type==EQUATION && (pOpt->type==OPT_ENUMERATION || pOpt->type==OPT_EQUATION))))
          return pOpt;
  return 0;
}
void NODELIST::free_all()
{
  for (PNODE pNode1,pNode=pNodes; pNode; pNode=pNode1)
  {
    for (POPTION pOpt1,pOpt=pNode->pOptList; pOpt; pOpt=pOpt1)
    {
      pOpt1 = pOpt->next;
      delete pOpt;
    }
    pNode1 = pNode->next;
    delete pNode;
  }
  pNodes = 0;
  del_controls(FALSE);
}


MRESULT EXPENTRY wpHexEFProc (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
//
{
  static PFNWP pWindowProc;
  char ch;

  if (ulMsg == WM_CHAR && SHORT1FROMMP(m1) & KC_CHAR && SHORT2FROMMP(m2)!=VK_BACKSPACE){
    ch = sf_toupper(char(SHORT1FROMMP(m2)));
    *PCHAR(&m2) = ch; // To upper it
    if (!(ch>='0' && ch<='9') && !(ch>='A' && ch<='F'))
      return 0;
  }
  else if (ulMsg == MM_START){
    pWindowProc = (PFNWP)m1;
    return 0;
  }
//  if (ulMsg == WM_MOUSEMOVE)
//    DispMouse(hWnd,m1,m2);
  return (*pWindowProc)(hWnd, ulMsg, m1, m2);
}
MRESULT EXPENTRY wpDecimalEFProc (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
//
{
  static PFNWP pWindowProc;

  if (ulMsg == WM_CHAR && SHORT1FROMMP(m1) & KC_CHAR && SHORT2FROMMP(m2) != VK_BACKSPACE &&
        !(SHORT1FROMMP(m2)>='0' && SHORT1FROMMP(m2) <= '9'))
    return 0;
  else if (ulMsg == MM_START){
    pWindowProc = (PFNWP)m1;
    return 0;
  }
//  if (ulMsg == WM_MOUSEMOVE)
//    DispMouse(hWnd,m1,m2);
  return (*pWindowProc)(hWnd, ulMsg, m1, m2);
}

/////////////////------------- End of NODELIST realisations ----------////////////////
//////////////////////////////////////////////////////////////////////////////////////


// Globals:
//
PRJSTRUCT     PrjStruct;
HBITMAP       hbmpOpOpt = 0;
HBITMAP       hbmpClOpt = 0;
LONG          xOptBMap  = 1;
LONG          yOptBMap  = 1;
LONG          yItHeight = 1;
PRJ_LINE_TYPE ulUsrDlgType;

//// Собственно редактирование проекта 'в картинках'
//
// hMsgWnd   - родительское окно для диалога(ов)
// szPrjFile - полное имя файла проекта
// szPrtFile - полное имя *.prt файла = темплита диалога or 0
// Returns   - TRUE если файл проекта изменился
//
BOOL PrjEdit(HWND hMsgWnd, PSZ szPrjFile, PSZ szPrtFile)
{
  char sz[CCHMAXPATH];
  if (!szPrtFile)
  {
    BOOL fOk = FALSE;
    PSZ  psz = pUserVarTable->QueryVar("XDSCompiler");
    if (psz &&  *(psz=sf_skipspaces(psz)))
    {
      strcpy(sz,psz);
      PSZ pszExt = strrchr(sz,'.');
      if (pszExt && !strpbrk(pszExt,"\\/:"))
      {
        strcpy(pszExt,".prt");
        fOk = TRUE;
      }
    }
    if (!fOk)
    {
      MessageBox(hMsgWnd,"Modify project",MB_ERROR|MB_OK,"Can not open project templite (.prt) file. For more info see help for 'XDSCompiler' variable.");
      return FALSE;
    }
    szPrtFile = sz;
  }
  MRESULT      EXPENTRY dpPrjEdDialog (HWND, ULONG, MPARAM, MPARAM);
  CPARAMSTR    cps;
  cps.psz1     = szPrtFile;
  BOOL     fOk = PrjStruct.Parse(hMsgWnd, szPrjFile);
  if (fOk) fOk = (BOOL)WinDlgBox(HWND_DESKTOP,hMsgWnd,(PFNWP)dpPrjEdDialog,NULLHANDLE,IDD_PRJMODDLG,&cps);
  BOOL     fCh = PrjStruct.IsChanged();
  if (fOk) fOk = PrjStruct.BuildBack(hMsgWnd, szPrjFile);
  PrjStruct.FreeAll();
  return fCh && fOk;
}


MRESULT EXPENTRY dpPrjEdDialog (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  MRESULT EXPENTRY  wpOptLBox  (HWND, ULONG, MPARAM, MPARAM);
  MRESULT EXPENTRY  dpUsrModDlg(HWND, ULONG, MPARAM, MPARAM);
  MRESULT EXPENTRY  dpUsrOptDlg(HWND, ULONG, MPARAM, MPARAM);
  static  HWND      hLB;
  static  HWND      hDisp;
  static  HWND      hComment;
  static  int       nLBLastSel = 0;
  static  PNODELIST pNodeList  = 0;
  switch(ulMsg)
  {
  case WM_INITDLG:
    hLB      = WinWindowFromID(hWnd,IDD_LBPRJMOD);
    hDisp    = WinWindowFromID(hWnd,IDD_STPRJMODDISP);
    hComment = WinWindowFromID(hWnd,IDD_STPRJMODCOMMENT);
    wpOptLBox(hLB, MM_START, MPARAM(ULONG(WinSubclassWindow(hLB,(PFNWP)wpOptLBox))),0L);
    if (!pNodeList) pNodeList = new NODELIST;
    if (!pNodeList->Initialize(hMDI0Client,PCPARAMSTR(m2)->psz1))
    {
      WinDismissDlg(hWnd,FALSE);
      break;
    }
    if (!hbmpOpOpt)
    {
      HPS hps   = WinGetPS(HWND_DESKTOP);
      hbmpOpOpt = GpiLoadBitmap(hps, NULLHANDLE, BMP_OPOPTION, 0, 0);
      hbmpClOpt = GpiLoadBitmap(hps, NULLHANDLE, BMP_CLOPTION, 0, 0);
    }
    {
      FONTMETRICS       fm;
      BITMAPINFOHEADER  bmpData;
      HPS               hps = WinGetPS(hWnd);
      GpiQueryFontMetrics(hps,sizeof(FONTMETRICS),&fm);
      yItHeight = (fm.lMaxBaselineExt+fm.lExternalLeading+1) + 1;
      WinReleasePS(hps);
      if (GpiQueryBitmapParameters(hbmpOpOpt,&bmpData))
      {
        xOptBMap = bmpData.cx;
        yOptBMap = bmpData.cy;
      }
      yItHeight = max (yItHeight,yOptBMap+1);
    }
    WinSendMsg(hLB,LM_SETITEMHEIGHT,MPARAM(yItHeight),0);
    pNodeList->BindToDialog(hWnd,hLB,hDisp);
    pNodeList->Prj2Options(&PrjStruct);
    pNodeList->FillLBox();
    if (!WinSendMsg(hLB,LM_SELECTITEM,MPARAM(nLBLastSel),MPARAM(TRUE)))
      WinSendMsg(hLB,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
    break;
  case WM_MEASUREITEM:
    return MRESULT(yItHeight);
  case WM_DRAWITEM:
    return WinSendMsg(hLB,LLM_DRAWITEM,m1,m2);
  case WM_FOCUSCHANGE:
    if (SHORT1FROMMP(m2))
      WinStartTimer(hAB,hWnd,1,200);
    else
    {
      WinStopTimer(hAB,hWnd,1);
      pNodeList->GenComment(TRUE,hComment);
    }
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  case WM_TIMER:
    pNodeList->GenComment(FALSE,hComment);
    break;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch(SHORT1FROMMP(m1))
    {
      case IDD_PBUSROPT:
      case IDD_PBUSREQ:
      case IDD_PBMODULES:
      case IDD_PBLOOKUPS:
        if (SHORT1FROMMP(m1)==IDD_PBMODULES)
        {
          ulUsrDlgType = MODULE;
          WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpUsrModDlg,NULLHANDLE,IDD_USROPTDLG,0);
        }
        else
        {
          if      (SHORT1FROMMP(m1)==IDD_PBUSROPT)  ulUsrDlgType = USEROPTION;
          else if (SHORT1FROMMP(m1)==IDD_PBUSREQ)   ulUsrDlgType = USEREQUATION;
          else                                      ulUsrDlgType = LOOKUP;
          WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpUsrOptDlg,NULLHANDLE,IDD_USROPTDLG,0);
        }
        break;
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_PRJMODDLG);
        break;
      case DID_CANCEL:
        pNodeList->ForgetControls();
        WinDismissDlg(hWnd,FALSE);
        break;;
      case DID_OK:
        if (!PrjStruct.IsAnyModules())
          if (MBID_YES != MessageBox(hWnd,"Edit project",MB_ICONHAND|MB_YESNO,"The project is not complete "
                                     "(no modules specifyed). Do you want to add modules late?"))
            break;
        pNodeList->AskControlsNow();
        pNodeList->Options2Prj(&PrjStruct);
        pNodeList->ForgetControls();
        WinDismissDlg(hWnd,TRUE);
        break;
    }
    break;
  case WM_CONTROL:
    if      (m1==MPFROM2SHORT(IDD_LBPRJMOD,LN_SELECT)) pNodeList->LBSelect();
    else if (m1==MPFROM2SHORT(IDD_LBPRJMOD,LN_ENTER))  pNodeList->LBEnter();
    else                                               pNodeList->WMControl(m1,m2);
    break;
  case WM_PAINT:
     WinDefDlgProc(hWnd,ulMsg,m1,m2);
     pNodeList->WMPaint();
     break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_PRJMODDLG);
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

MRESULT EXPENTRY wpOptLBox(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PFNWP pWindowProc;
  static BOOL  fDrawLocked = FALSE;

  switch(ulMsg)
  {
  case MM_START:
    pWindowProc = PFNWP(ULONG(m1));
    return 0;
  case WM_PAINT:
    if (fDrawLocked)
    {
      WinEndPaint(WinBeginPaint(hWnd,0,0));
      return 0;
    }
    break;
  case WM_DRAWITEM:
    if (fDrawLocked) return MRESULT(TRUE);
    break;
  case LLM_LOCKUPDATE:
    if (!(fDrawLocked = BOOL(m1)))
      WinInvalidateRect(hWnd,0,0);
    return 0;
  case LLM_DRAWITEM:
  {
    POWNERITEM pow     = POWNERITEM(m2);
    int        nIt     = pow -> idItem;
    HPS        hps     = pow->hps;
    RECTL      rc      = pow->rclItem; rc.yTop--;
    LONG       clrFore = CLR_BLACK;
    LONG       clrBack = CLR_WHITE;
    PNODE      pNode   = PNODE(pow->hItem);
    HBITMAP    bmp     = 0;
    POINTL     ptl;
    if (!pNode) break;
    if (nIt==LIT_NONE || fDrawLocked) return MRESULT(TRUE);

    WinFillRect(hps,&rc,clrBack);

    if (pNode->next  &&  pNode->next->nLevel > pNode->nLevel)
      bmp  = pNode->fExpanded ? hbmpOpOpt : hbmpClOpt;

    if (pow->fsState){
      clrFore = CLR_WHITE;
      clrBack = CLR_BLACK;
    }
    // Draw bitmap
    ptl.x = rc.xLeft = rc.xLeft+1 + pNode->nLevel*xOptBMap;
    ptl.y = (rc.yTop-rc.yBottom-yOptBMap)/2;
    ptl.y = rc.yBottom + max(ptl.y,0);
    if (bmp)
    {
      WinDrawBitmap(hps,bmp,NULL,&ptl,clrFore,clrBack,DBM_NORMAL);
      rc.xLeft += xOptBMap + 1;
    }
    WinFillRect(hps,&rc,clrBack);

    GpiSetBackMix(hps,BM_OVERPAINT);
    WinDrawText(hps,-1,pNode->szNodeName,&rc,clrFore, clrBack,DT_LEFT | DT_VCENTER);
    pow->fsState = pow->fsStateOld = 0;
    return MRESULT(TRUE);
  }
  } // switch
  return ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
}

struct USRLISTITEM
{
  PSZ         pszLeftPart;  // !module(s) are placed here
  PSZ         pszRightPart; // !modul's comment - here
  BOOL        fOptON;
  PPRJLINE    pPrjLine;
  BOOL        fChanged;
  USRLISTITEM () {memset(this,0,sizeof(*this));}
 ~USRLISTITEM () {free(pszLeftPart);free(pszRightPart);}
};
typedef USRLISTITEM *PUSRLISTITEM;
struct USRDELLIST
{
  PPRJLINE    pPrjLine;
  USRDELLIST *next;
  USRDELLIST  () {memset(this,0,sizeof(*this));}
};
typedef USRDELLIST *PUSRDELLIST;

static PSZ mk_usr_item(PUSRLISTITEM pUsrIt)
{ // malloc()ed text for the listbox
  int   nAlloc   = 0;
  int   nUsed    = 0;
  PSZ   pBuf     = 0;
  try
  {
    switch (ulUsrDlgType)
    {
      case USEROPTION:
        app_txt(&pBuf,&nUsed,&nAlloc,"-");
        app_txt(&pBuf,&nUsed,&nAlloc,pUsrIt->pszLeftPart);
        app_txt(&pBuf,&nUsed,&nAlloc,pUsrIt->fOptON ? "+" : "-");
        break;
      case USEREQUATION:
        app_txt(&pBuf,&nUsed,&nAlloc,"-");
        app_txt(&pBuf,&nUsed,&nAlloc,pUsrIt->pszLeftPart);
        app_txt(&pBuf,&nUsed,&nAlloc," = ");
        app_txt(&pBuf,&nUsed,&nAlloc,pUsrIt->pszRightPart);
        break;
      case LOOKUP:
        app_txt(&pBuf,&nUsed,&nAlloc,"-lookup = ");
        app_txt(&pBuf,&nUsed,&nAlloc,pUsrIt->pszLeftPart);
        app_txt(&pBuf,&nUsed,&nAlloc," = ");
        app_txt(&pBuf,&nUsed,&nAlloc,pUsrIt->pszRightPart);
        break;
      case MODULE:
        app_txt(&pBuf,&nUsed,&nAlloc,"!module ");
        app_txt(&pBuf,&nUsed,&nAlloc,pUsrIt->pszLeftPart);
        break;
    }
    return pBuf;
  }
  catch (char *psz)
  {
    free(pBuf);
    return 0;
  }
}
static void fill_usropt_list(HWND hLB)
{// Filling the listbox
  PUSRLISTITEM pNew;
  PSZ          pszIt;
  int          nIt;
  WinSendMsg(hLB,LM_DELETEALL,0,0);
  for (PPRJLINE pPrjLine = PrjStruct.pPrjLines; pPrjLine; pPrjLine = pPrjLine->next)
  if (pPrjLine->type==ulUsrDlgType)
  {
    pNew = new USRLISTITEM;
    pNew->pPrjLine = pPrjLine;
    // Left part:
    if (pPrjLine->pszKeyName)
    {
      if (pNew->pszLeftPart = (PSZ)malloc(strlen(pPrjLine->pszKeyName)+1))
        strcpy(pNew->pszLeftPart, pPrjLine->pszKeyName);
    }
    // Right part:
    if (pPrjLine->pszKeyValue)
    {
      if (ulUsrDlgType == USEROPTION)
          pNew->fOptON = (pPrjLine->pszKeyValue[0] == '+');
      else  // LOOKUP or USEREQUATION
      {
        if (pNew->pszRightPart = (PSZ)malloc(strlen(pPrjLine->pszKeyValue)+1))
          strcpy(pNew->pszRightPart, pPrjLine->pszKeyValue);
      }
    }
    // Insert into the hLB
    if (pszIt = mk_usr_item(pNew))
    {
      nIt   = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),MPARAM(pszIt));
      WinSendMsg(hLB,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(pNew));
      free(pszIt);
    }
  }
}
static void apply_usropt_list(HWND hLB, PUSRDELLIST pUsrDelList)
{// Apply changes to the project
  PPRJLINE *ppInsTo = 0;
  for (; pUsrDelList; pUsrDelList=pUsrDelList->next)
  {// Delete all we need
    for (PPRJLINE *pp = &PrjStruct.pPrjLines; *pp; )
    {
      PPRJLINE p = *pp;
      if (p == pUsrDelList->pPrjLine)
      {
        *pp     = p->next;
        ppInsTo = pp;
        delete p;
      }
      else pp = &(*pp)->next;
    }
  }
  {// set ppInsTo - 2nd walk
    for (PPRJLINE p = PrjStruct.pPrjLines; p; p = p->next)
      if (p->type==ulUsrDlgType || (!ppInsTo && (!p->next || p->next->type==MODULE))) ppInsTo = &p->next;
    if (!ppInsTo) ppInsTo = &PrjStruct.pPrjLines;
  }
  int nTop = (int)WinSendMsg(hLB,LM_QUERYITEMCOUNT,0,0);
  for (int nIt=0; nIt<nTop; nIt++)
  {
    PPRJLINE     pPrjLine;
    PUSRLISTITEM pIt = (PUSRLISTITEM)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
    if (pIt && pIt->fChanged && ((pPrjLine=pIt->pPrjLine) || (pPrjLine = new PRJLINE)))
    {
      pPrjLine->type     = ulUsrDlgType;
      pPrjLine->SetKeyName (pIt->pszLeftPart);
      pPrjLine->SetKeyValue((ulUsrDlgType==USEROPTION) ? (pIt->fOptON ? "+" : "-") : pIt->pszRightPart);
      pPrjLine->fChanged = TRUE;
      pIt->fChanged      = FALSE;
      if (!pIt->pPrjLine)
      {
        pPrjLine->next   = *ppInsTo;
        *ppInsTo         = pPrjLine;
        ppInsTo          = &pPrjLine->next;
        pIt->pPrjLine    = pPrjLine;
      }
    }
  }
}

// User options / user equations / lookups  dialog
MRESULT EXPENTRY dpUsrOptDlg (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static  HWND        hLB;
  static  PUSRDELLIST pUsrDelList = 0;
  switch(ulMsg)
  {
  case WM_INITDLG:
  {
    pUsrDelList = 0;
    hLB         = WinWindowFromID(hWnd,IDD_LBUSROPTS);
    PSZ psz                               = "User options";
    PSZ psz1                              = "Toggle";
    if (ulUsrDlgType == USEREQUATION) psz = "User equations",  psz1 = "Edit value...";
    if (ulUsrDlgType == LOOKUP)       psz = "Project lookups", psz1 = "Edit paths...";
    WinSetWindowText (hWnd,psz);
    WinSetDlgItemText(hWnd,IDD_PBUSROPTED,psz1);
    fill_usropt_list(hLB);
    WinSendMsg(hLB,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch(SHORT1FROMMP(m1))
    {
      case IDD_PBUSROPTNEW:
      {
        char sz[1000] = "";
        CPARAMSTR cps;
        cps.psz1                                   = "New user option";
        if (ulUsrDlgType == USEREQUATION) cps.psz1 = "New user equation";
        if (ulUsrDlgType == LOOKUP)       cps.psz1 = "New lookup statement";
        cps.psz2                                   = "Option name:";
        if (ulUsrDlgType == USEREQUATION) cps.psz2 = "Equation name:";
        if (ulUsrDlgType == LOOKUP)       cps.psz2 = "Lookup file template(s):";
        cps.psz3 = sz;
        cps.l1   = sizeof(sz);
        cps.l3   = 0;
        if (!WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
          break;
        PUSRLISTITEM pNew;
        PSZ          pszIt;
        int          nIt;
        if ((pNew = new USRLISTITEM) && (pNew->pszLeftPart = (PSZ)malloc(strlen(sz)+1)))
        {
          strcpy(pNew->pszLeftPart, sz);
          pNew->fChanged = TRUE;
          if (pszIt = mk_usr_item(pNew))
          {
            nIt   = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),MPARAM(pszIt));
            WinSendMsg(hLB,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(pNew));
            WinSendMsg(hLB,LM_SELECTITEM,   MPARAM(nIt),MPARAM(TRUE));
            free(pszIt);
            WinSendDlgItemMsg(hWnd,IDD_PBUSROPTED,BM_CLICK,0,0);
          }
        }
        break;
      }
      case IDD_PBUSROPTED:
      {
        int          nIt = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        PUSRLISTITEM pIt = (PUSRLISTITEM)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
        PSZ          pszIt;
        if (!pIt) break;
        if (ulUsrDlgType == USEROPTION)
          pIt->fOptON = !pIt->fOptON;
        else
        {
          CPARAMSTR cps;
          char      sz[1000] = "";
          if (pIt->pszRightPart) strcpy(sz,pIt->pszRightPart);
          cps.psz1 = (ulUsrDlgType == LOOKUP) ? "Edit lookup pathlist" : "Edit user equation value";
          cps.psz2 = (PSZ)malloc(strlen(pIt->pszRightPart ? pIt->pszRightPart : "") + 30);
          if (!cps.psz2) break;
          sprintf(cps.psz2, (ulUsrDlgType == LOOKUP) ? "Pathlist for '%s':" : "Value for '%s':", pIt->pszLeftPart ? pIt->pszLeftPart : "???");
          cps.psz3 = sz;
          cps.l1   = sizeof(sz);
          cps.l3   = 0;
          BOOL fOk = (BOOL)WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps);
          free(cps.psz2);
          if (!fOk) break;
          free(pIt->pszRightPart);
          if (pIt->pszRightPart = (PSZ)malloc(strlen(sz)+1))
            strcpy(pIt->pszRightPart,sz);
        }
        if (pszIt = mk_usr_item(pIt))
        {
          WinSendMsg(hLB,LM_SETITEMTEXT,MPARAM(nIt),MPARAM(pszIt));
          free(pszIt);
        }
        pIt->fChanged = TRUE;
        break;
      }
      case IDD_PBUSROPTREM:
      {
        int          nIt = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        PUSRLISTITEM pIt = (PUSRLISTITEM)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
        if (pIt)
        {
          delete pIt;
          WinSendMsg(hLB,LM_DELETEITEM,MPARAM(nIt),0);
          WinEnableControl(hWnd,IDD_PBUSROPTREM,FALSE);
          WinEnableControl(hWnd,IDD_PBUSROPTED, FALSE);
          WinSendMsg(hLB,LM_SELECTITEM,MPARAM(max(nIt,1)-1),MPARAM(TRUE));
          PUSRDELLIST pNew;
          if (pIt->pPrjLine && (pNew = new USRDELLIST))
          {
            pNew->next     = pUsrDelList;
            pNew->pPrjLine = pIt->pPrjLine;
            pUsrDelList    = pNew;
          }
        }
        break;
      }
      case IDD_PBHELP:
        switch(ulUsrDlgType)
        {
          case USEREQUATION: DisplayHelpPanel(PANEL_USREQDLG);     break;
          case LOOKUP:       DisplayHelpPanel(PANEL_USRLOOKUPDLG); break;
          default:           DisplayHelpPanel(PANEL_USROPTDLG);
        }
        break;
      case DID_OK:
        apply_usropt_list(hLB,pUsrDelList);
      case DID_CANCEL:
      {
        int nTop = (int)WinSendMsg(hLB,LM_QUERYITEMCOUNT,0,0);
        for (int nIt=0; nIt<nTop; nIt++)
        {
          PUSRLISTITEM pIt = (PUSRLISTITEM)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
          if (pIt) delete pIt;
        }
        for (PUSRDELLIST p1,p=pUsrDelList; p; p=p1)
        {
          p1 = p->next;
          delete p1;
        }
        WinDismissDlg(hWnd,FALSE);
        break;
      }
    }
    break;
  case WM_CONTROL:
    if      (m1==MPFROM2SHORT(IDD_LBUSROPTS,LN_ENTER))
      WinSendDlgItemMsg(hWnd,IDD_PBUSROPTED,BM_CLICK,0,0);
    else if (m1==MPFROM2SHORT(IDD_LBUSROPTS,LN_SELECT))
    {
      WinEnableControl(hWnd,IDD_PBUSROPTREM,TRUE);
      WinEnableControl(hWnd,IDD_PBUSROPTED, TRUE);
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      switch(ulUsrDlgType)
      {
        case USEREQUATION: DisplayHelpPanel(PANEL_USREQDLG);     break;
        case LOOKUP:       DisplayHelpPanel(PANEL_USRLOOKUPDLG); break;
        default:           DisplayHelpPanel(PANEL_USROPTDLG);
      }
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


// 'Modules' dialog
MRESULT EXPENTRY dpUsrModDlg (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static  HWND    hLB;
  static  BOOL    fChanged;
  switch(ulMsg)
  {
  case WM_INITDLG:
  {
    fChanged = FALSE;
    hLB = WinWindowFromID(hWnd,IDD_LBUSROPTS);
    WinSetWindowText (hWnd,"Project modules");
    WinSetDlgItemText(hWnd,IDD_PBUSROPTED,"Edit name...");
    for (PPRJLINE pPrjLine = PrjStruct.pPrjLines; pPrjLine; pPrjLine = pPrjLine->next)
      if (pPrjLine->type==MODULE)
      {
        PSZ          psz,pszE,pszIt;
        PUSRLISTITEM pNew;
        for (psz = pPrjLine->pszKeyValue; psz && *(psz=sf_skipspaces(psz)); psz = pszE)
        {
          pszE = skipname(psz);
          if (pszE==psz) break;
          if ((pNew = new USRLISTITEM) && (pNew->pszLeftPart = (PSZ)malloc(pszE-psz+1)))
          {
            strncpy(pNew->pszLeftPart, psz, pszE-psz);
            pNew->pszLeftPart[pszE-psz] = 0;
            if (pPrjLine->pszComment && (pNew->pszRightPart = (PSZ)malloc(strlen(pPrjLine->pszComment)+1)))
              strcpy(pNew->pszRightPart, pPrjLine->pszComment);
            if (pszIt = mk_usr_item(pNew))
            {
              int nIt = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),MPARAM(pszIt));
              WinSendMsg(hLB,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(pNew));
              free(pszIt);
            }
          }
        }
      }
    WinSendMsg(hLB,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch(SHORT1FROMMP(m1))
    {
      case IDD_PBUSROPTNEW:
      {
        PUSRLISTITEM pNew;
        PSZ          pszIt;
        int          nIt;
        if ((pNew = new USRLISTITEM) && (pNew->pszLeftPart = (PSZ)malloc(1)))
        {
          pNew->pszLeftPart[0] = 0;
          if (pszIt = mk_usr_item(pNew))
          {
            nIt   = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),MPARAM(pszIt));
            WinSendMsg(hLB,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(pNew));
            WinSendMsg(hLB,LM_SELECTITEM,   MPARAM(nIt),MPARAM(TRUE));
            free(pszIt);
            WinSendDlgItemMsg(hWnd,IDD_PBUSROPTED,BM_CLICK,0,0);
          }
        }
        fChanged = TRUE;
        break;
      }
      case IDD_PBUSROPTED:
      {
        int          nIt = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        PUSRLISTITEM pIt = (PUSRLISTITEM)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
        CPARAMSTR    cps;
        PSZ          pszIt;
        if (!pIt) break;
        char      sz[1000] = "";
        if (pIt->pszLeftPart) strcpy(sz,pIt->pszLeftPart);
        cps.psz1 = "Edit module name";
        cps.psz2 = "Module name:";
        cps.psz3 = sz;
        cps.l1   = sizeof(sz);
        cps.l3   = 0;
        if (!WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
          break;
        free(pIt->pszLeftPart);
        if (pIt->pszLeftPart = (PSZ)malloc(strlen(sz)+1))
          strcpy(pIt->pszLeftPart,sz);
        if (pszIt = mk_usr_item(pIt))
        {
          WinSendMsg(hLB,LM_SETITEMTEXT,MPARAM(nIt),MPARAM(pszIt));
          free(pszIt);
        }
        fChanged = TRUE;
        break;
      }
      case IDD_PBUSROPTREM:
      {
        int          nIt = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        PUSRLISTITEM pIt = (PUSRLISTITEM)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
        if (pIt)
        {
          delete pIt;
          WinSendMsg(hLB,LM_DELETEITEM,MPARAM(nIt),0);
          WinEnableControl(hWnd,IDD_PBUSROPTREM,FALSE);
          WinEnableControl(hWnd,IDD_PBUSROPTED, FALSE);
          WinSendMsg(hLB,LM_SELECTITEM,MPARAM(max(nIt,1)-1),MPARAM(TRUE));
        }
        fChanged = TRUE;
        break;
      }
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_MODULESDLG);
        break;
      case DID_OK:
      if (fChanged)
      {
        PPRJLINE *pp;
        for (pp = &PrjStruct.pPrjLines; *pp; )
        { // delete all the '!module' statements
          PPRJLINE p = *pp;
          if (p->type == MODULE)
          {
            *pp     = p->next;
            delete p;
          }
          else pp = &(*pp)->next;
        }
        int nTop = (int)WinSendMsg(hLB,LM_QUERYITEMCOUNT,0,0);
        for (int nIt=0; nIt<nTop; nIt++)
        {
          PUSRLISTITEM pIt = (PUSRLISTITEM)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
          if (pIt && pIt->pszLeftPart && *sf_skipspaces(pIt->pszLeftPart))
          {
            PPRJLINE pNew  = new PRJLINE;
            pNew->type     = MODULE;
            pNew->SetKeyValue(pIt->pszLeftPart);
            pNew->SetComment (pIt->pszRightPart);
            pNew->fChanged = TRUE;
            pNew->next     = *pp;
            *pp            = pNew;
            pp             = &pNew->next;
          }
          delete pIt;
        }
      }
      case DID_CANCEL:
        WinDismissDlg(hWnd,FALSE);
        break;;
    }
    break;
  case WM_CONTROL:
    if (m1==MPFROM2SHORT(IDD_LBUSROPTS,LN_SELECT))
    {
      WinEnableControl(hWnd,IDD_PBUSROPTREM,TRUE);
      WinEnableControl(hWnd,IDD_PBUSROPTED, TRUE);
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_MODULESDLG);
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

