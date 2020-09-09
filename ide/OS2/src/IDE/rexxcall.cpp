/*
*
*  File: REXXCALL.CPP (Rx)
*
*  XDS Shell: Calling REXX interpreter, macros
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define  INCL_WIN
#define  INCL_GPI
#define  INCL_DOS
#include <os2.h>
#define  INCL_REXXSAA
#include "xdsbase.h"

struct IMMEDIATE
{
  PFN     pfn;
};
typedef IMMEDIATE *PIMMEDIATE;

/*>>> ------ G L O B A L S
*/
SymTable     stImmediate;
BOOL         fMacroRecord  = FALSE;
BOOL         fRexxMonReady = FALSE;


BOOL RxRegister(PSZ pszName, PFN pfn, BOOL fImmediate)
{
  if (RexxRegisterFunctionExe(pszName,pfn)) return FALSE;
  if (fImmediate)
  {
    PIMMEDIATE pim = new IMMEDIATE;
    if (!strcmp(pszName,"MacroRecord")) pim->pfn = PFN(Rh2MacroRecord);
    else                                pim->pfn = pfn;
    stImmediate.AddName(pszName,LONG(pim));
  }
  return TRUE;
}

BOOL pim_execute(PSZ pszCmd, char *pchE, PIMMEDIATE pim)
// pim->pfn's arguments are: PSZ name, LONG argc, PRXSTRING argv, PSZ queue, PRXSTRING retstr
{
  char         szName   [32];
  LONG         argc     = 0;
  RXSTRING     argv     [32];
  RXSTRING     rsResult;
  char         achRetBuf[256];
  char         *pchB;
  char         ch;
  BOOL         rc = FALSE;

  try
  {
    memcpy(szName,pszCmd,pchE-pszCmd); szName[pchE-pszCmd] = '\0';
    MAKERXSTRING(rsResult,achRetBuf,sizeof(achRetBuf)-1);
    // Parse the arguments
    pchE = sf_skipspaces(pchE);
    if (*(pchE++) != '(') return FALSE;
    while(1)
    {
      pchE = sf_skipspaces(pchE);
      if (*pchE == ')') {pchE++; break;}
      else
      {
        ch = (pchB = pchE)[0];
        if (ch=='\'' || ch=='\"')
        {
          for(pchE++; *pchE && *pchE!=ch; pchE++);
          pchE++;
        }
        else
           while ((ch=*++pchE) && !sf_isspace(ch) && !strchr("),;\'\"", ch));
        {// Create the REXXSTRING argv[argc++]
          char *pchStr = (char*)malloc(pchE-pchB+1);
          if (!pchStr) throw "Out of memory error";
          if (*pchB=='\'' || *pchB=='\"') memcpy(pchStr,pchB+1,pchE-pchB-2), pchStr[pchE-pchB-2] = '\0';
          else                            memcpy(pchStr,pchB,  pchE-pchB  ), pchStr[pchE-pchB  ] = '\0';
          MAKERXSTRING(argv[argc],pchStr,strlen(pchStr));
          argc++;
          if (argc==31) throw "Too many arguments";
        }
        pchE = sf_skipspaces(pchE);
        if (*pchE == ')') break;
        if (*pchE != ',') throw "Syntax error in arguments";
        pchE++;
      }
    }
  }
  catch (char *psz)
  {
    for (int i=0; i<argc; i++) free(argv[i].strptr);
    return FALSE;
  }

  pchE = sf_skipspaces(pchE);
  {// Call it
    typedef LONG (EXPENTRY *PRXXHAND) (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
    rc = !PRXXHAND(pim->pfn)(szName,argc,argv,"",&rsResult);
    for (int i=0; i<argc; i++) free(argv[i].strptr);
  }
  return rc;
}

BOOL rx_execute(PSZ pszCmd, PLONG plRes, PSZ *ppszRes)
{
  INT          rc;
  RXSTRING     arsInstore[2];
  RXSTRING     rsResult;
  SHORT        shRes;
  char         *pch;
  static char  achRetBuf[257];
  static char *pszBuf = 0;

  free(pszBuf); pszBuf =0;
  PSZ pszExeCmd = OpenExprVar(sf_skipspaces(pszCmd));
  if (!pszExeCmd) return FALSE;
  pszExeCmd = sf_skipspaces(pszExeCmd);

  // Immediate?

  if (sf_isalpha(pszExeCmd[0]))
  {
    char *pchE = pszExeCmd+1;
    while(sf_isalpha(*pchE) || sf_isdigit(*pchE)) pchE++;
    if (pchE-pszExeCmd < 30)
    {
      char sz[32];
      memcpy(sz,pszExeCmd,pchE-pszExeCmd);
      sz[pchE-pszExeCmd]='\0';
      LONG l = stImmediate.SearchName(sz);
      // Immediate && pchE line is looking like "(...)  ;  ",0
      if (l!=-1 && *pchE=='(' && !strchr(pchE+1,'(') && (pch=strchr(pchE,')'))
          && *(pch=sf_skipspaces(pch+1))==';' && !*sf_skipspaces(pch+1))
      {
        PIMMEDIATE pim = (PIMMEDIATE) l;
        rc = pim_execute(pszExeCmd, pchE, pim);
        free(pszExeCmd);
        return rc;
      }
    }
  }

  MAKERXSTRING(arsInstore[0],pszExeCmd,strlen(pszExeCmd));
  MAKERXSTRING(arsInstore[1],"",       0);

  MAKERXSTRING(rsResult,achRetBuf,sizeof(achRetBuf)-1);

  RXSYSEXIT exit_list[2];
  exit_list[0].sysexit_name = RXEXIT_NAME;
  exit_list[0].sysexit_code = RXSIO;
  exit_list[1].sysexit_code = RXENDLST;

  rc = RexxStart
           (0,          // argc
            0,          // args
            "xds.exe",
            arsInstore,
            0,          // EnvName
            RXFUNCTION,
            exit_list,
            &shRes,
            &rsResult);
  free(pszExeCmd);
  if (rc) return FALSE;

  pch = RXSTRPTR(rsResult);

  if (!pch)
    pch = "";
  else if ( pch != achRetBuf)  // I'd like to add the '0' char...
  {
    pszBuf = (PSZ)malloc(RXSTRLEN(rsResult)+1);
    memcpy(pszBuf,pch,RXSTRLEN(rsResult));
    pszBuf[RXSTRLEN(rsResult)] = '\0';
    DosFreeMem(RXSTRPTR(rsResult));
    pch = pszBuf;
  }
  else
    pch[RXSTRLEN(rsResult)] = '\0';

  if(ppszRes) *ppszRes = pch;
  if(plRes)   *plRes   = strtol(pch,&pch,0); // LONG needs (instead SHORT shRes)

  return TRUE;
}

BOOL RxExecute(PSZ pszCmd, PLONG plRes, PSZ *ppszRes, BOOL fTryImmediate)
{
  if (rx_execute(pszCmd,plRes,ppszRes))
  {
    Macros.AddToMacro(pszCmd);
    return TRUE;
  }
  return FALSE;
}


int RxParseLine(PSZ pszLine, PSZ *pArgv[], LONG *pArgl[], PSZ szDelimiters)
{
  const  int  MAXARGS = 200;
  static PSZ  Argv[MAXARGS];
  static LONG Argl[MAXARGS];
  static PSZ  pszBuf  = 0;
  int         i       = 0;
  char       *pch,*pch1;

  *pArgv = &Argv[0];
  *pArgl = &Argl[0];

  free(pszBuf);
  pch = pszBuf = (PSZ)malloc(strlen(pszLine)+1);
  strcpy(pszBuf,pszLine);

  while(1)
  {
    pch     += strspn(pch,szDelimiters);  // skip delimiters
    if (*pch) return i;
    Argv[i]  = pch;
    pch     += strcspn(pch,szDelimiters); // skip token
    *(pch++) = '\0';
    Argl[i]  = strtol(Argv[i],&pch1,0);
    i++;
  }
}

void RxDupQuotes(PSZ sz)
// Duplicates all the quotes ('"') in sz
// (assumes that sizeof(sz) is enought!)
{
  int len = strlen(sz);
  for (int i=0; i<len; i++)
  {
    if (sz[i]=='"')
    {
      memmove(sz+i+1,sz+i,len-i+1);
      i++; len++;
    }
  }
}


/*========================================================================================================*/
/*=========================                                                         ======================*/
/*=======================            - - M A C R O S E S  ! ! ! - -                   ====================*/
/*=========================                                                         ======================*/
/*========================================================================================================*/

/*>>> MacroRecord('START' | 'START_DONE' | 'DONE' | 'TERMINATE' [,reason] | 'SHOWMONITOR')
> работает только при простом прямом вызове (из rexx-а дергается RhMacroRecord == stub)
*/
LONG   EXPENTRY Rh2MacroRecord(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  BOOL          fOk = FALSE;
  if (argc<1 || argc>2) return RxERROR;
  if      (!sf_stricmp(argv[0].strptr,"START_DONE"))
    fOk = Macros.hMacroMon ? Macros.DoneMacro() : Macros.StartRecord();
  else if (!sf_stricmp(argv[0].strptr,"START"))
    fOk = Macros.StartRecord();
  else if (!sf_stricmp(argv[0].strptr,"DONE"))
    fOk = Macros.DoneMacro();
  else if (!sf_stricmp(argv[0].strptr,"TERMINATE"))
    fOk = Macros.TerminateMacro(argc>1 ? argv[1].strptr : 0);
  return fOk ? 0 : RxERROR;
}

MACRO::MACRO (){memset(this,0,sizeof(*this));}
MACRO::~MACRO(){free(pszScript);free(pszComment);free(pszFile);}
void MACRO::Copy  (MACRO *pmSrc)
{
  free(pszScript);
  free(pszComment);
  free(pszFile);
  memset(this,0,sizeof(*this));
  if (pmSrc->pszScript)  pszScript  = sf_mallocstr(pmSrc->pszScript);
  if (pmSrc->pszComment) pszComment = sf_mallocstr(pmSrc->pszComment);
  if (pmSrc->pszFile)    pszFile    = sf_mallocstr(pmSrc->pszFile);
  rAccel                            = pmSrc->rAccel;
  nLastEdId                         = pmSrc->nLastEdId;
}
void MACRO::WProfile(LONG lSlot, PSZ szName)
{ // Пишем szKey,0, Script,0, Comment,0, File,0
  MPARAM  mmp1, mmp2;
  Acc2Mp(&rAccel,&mmp1,&mmp2);
  PSZ     szKey       = Mp2Txt(&mmp1, &mmp2);
  int     nKeyLen     = strlen(szKey      ? szKey      : "");
  int     nScriptLen  = strlen(pszScript  ? pszScript  : "");
  int     nCommentLen = strlen(pszComment ? pszComment : "");
  int     nFileLen    = strlen(pszFile    ? pszFile    : "");
  PSZ     pBuf        = (PSZ)malloc(nKeyLen + nScriptLen + nCommentLen + nFileLen + 4);
  if (pBuf)
  {
    int   skip;
    strcpy(pBuf,      szKey      ? szKey     :  ""); skip  = nKeyLen     + 1;
    strcpy(pBuf+skip, pszScript  ? pszScript :  ""); skip += nScriptLen  + 1;
    strcpy(pBuf+skip, pszComment ? pszComment : ""); skip += nCommentLen + 1;
    strcpy(pBuf+skip, pszFile    ? pszFile :    ""); skip += nFileLen    + 1;
    PfWriteItem(lSlot, szName, pBuf, skip);
  }
}

BOOL MACRO::RProfile(LONG lSlot, PSZ szName)
{
  free(pszScript);
  free(pszComment);
  free(pszFile);
  memset(this,0,sizeof(*this));
  int  nBufLen = PfQueryItemSize(lSlot, szName);
  if (nBufLen<sizeof(rAccel)+3) return FALSE;
  PSZ  pBuf    = (PSZ)malloc(nBufLen);
  PSZ  psz;
  if (!pBuf || nBufLen!=PfReadItem(lSlot, szName, pBuf, nBufLen, 0, 0))
  {
    free(pBuf);
    return FALSE;
  }
  psz = pBuf;
  if (!pBuf[0] || !StrToAcc(&psz, &rAccel))
    memset(&rAccel,0,sizeof(rAccel));
  if (!(psz=(PSZ)memchr(pBuf,'\0',nBufLen)))
  {
    free(pBuf);
    return FALSE;
  }
  int skip = psz - pBuf + 1;
  if (pBuf[skip]) pszScript  = sf_mallocstr(pBuf+skip); skip += strlen(pBuf+skip)+1;
  if (pBuf[skip]) pszComment = sf_mallocstr(pBuf+skip); skip += strlen(pBuf+skip)+1;
  if (pBuf[skip]) pszFile    = sf_mallocstr(pBuf+skip); skip += strlen(pBuf+skip)+1;
  free(pBuf);
  return TRUE;
}


int MACROS::nLastEdId;  // used to recall macro editor.

MACROS::MACROS(){memset(this,0,sizeof(*this)); nLastEdId=1;}
MACROS::~MACROS()
{
  if (hMacroMon) WinDestroyWindow(hMacroMon);
  free_list(pGlobalList);
  free_list(pLocalList);
}
BOOL MACROS::fMacroPlayingNow = FALSE;
BOOL MACROS::StartRecord()
{
  static CPARAMSTR  cps;
  static TID        tid;
  if (hMacroMon) return FALSE;
  cps.l1      = (LONG)this;
  hMacroMon   = WinLoadDlg(HWND_DESKTOP,NULLHANDLE,(PFNWP)dpMacroMonDlg,0,IDD_MACROMONDLG,&cps);
  WinProcessDlg(hMacroMon);
}

BOOL MACROS::DoneMacro()
{
  if (hMacroMon) WinSendMsg(hMacroMon,WM_USER+1,0,0);
  else           return FALSE;
  return TRUE;
}

BOOL MACROS::TerminateMacro (PSZ szReason)
{
  if (hMacroMon)
  {
    WinSendDlgItemMsg(hMacroMon,DID_CANCEL,BM_CLICK,0,0);
    if (szReason)
    {
      PSZ psz = (PSZ)malloc(strlen(szReason)+100);
      if (psz)
      {
        sprintf(psz,"Macro recording terminated. Reason: %s",szReason);
        MessageBox(hMDI0Client,"Macro record",MB_ERROR|MB_OK,psz);
      }
      free(psz);
    }
    return TRUE;
  }
  return FALSE;
}

void MACROS::AddToMacro(PSZ szCmd)
{
  if (hMacroMon && szCmd && szCmd[0])
  {
    PSZ sz = szCmd;
    if (!strchr(szCmd,LF))
    {
      if (!(sz = (PSZ)malloc(strlen(szCmd)+3))) return;
      sprintf(sz,"%s\r\n",szCmd);
    }
    WinSendMsg(hMacroMon, WM_USER, MPARAM(sz),0);
    if (sz!=szCmd) free(sz);
  }
}

    static BOOL match_file(PSZ *ppszFile, PSZ pszFileTpl)
    {
      PSZ pszTpl = pszFileTpl ? sf_skipspaces(pszFileTpl) : "";
      if (!*pszTpl) return TRUE;
      if (!*ppszFile)
      {
        PFILEED pFE = grOffice.QueryActiveFEd();
        if (pFE) *ppszFile = grOffice.QueryEdFile(pFE->hWnd);
      }
      if (!*ppszFile) *ppszFile = "";

      char  szFN[CCHMAXPATH];
      strcpy(szFN,*ppszFile);
      PSZ  psz    = OpenExprVar(pszTpl);
      if (!psz) return TRUE;;
      BOOL fMatch = RCThunk_FastMatch(sf_strtoupper(psz),sf_strtoupper(szFN));
      free(psz);
      return fMatch;
    }
    BOOL match_accel(PACCEL pAcc1, PACCEL pAcc2)
    {
      return (pAcc1->key==pAcc2->key && pAcc1->fs==pAcc2->fs && pAcc1->fs);
    }
BOOL MACROS::PlayMacro(PQMSG pqmsg)
{
  PMACRO   pm;
  ACCEL    rAccel;
  PSZ      pszFile    = 0;
  BOOL     fLocalWalk = TRUE;
  MPARAM   m1 = pqmsg->mp1;
  MPARAM   m2 = pqmsg->mp2;
  char     szKey[30] = "<none>";
  PSZ      sz        = Mp2Txt(&m1,&m2);
  #if DBGLOG
    if (sz) strcpy(szKey,sz);
    fprintf(stderr, "PlayMacro: m1=%08x m2=%08x, %s (now=%u)\n",pqmsg->mp1,pqmsg->mp2, szKey,fMacroPlayingNow);
  #endif
  Mp2Acc (m1,m2,&rAccel);
  if (!rAccel.fs || fMacroPlayingNow) return FALSE;
  pm                  = pLocalList;
  fMacroPlayingNow    = TRUE;
  while(1)
  {
    for (; pm; pm=pm->next)
    {
      if (match_accel(&pm->rAccel, &rAccel) && match_file(&pszFile,pm->pszFile))
      {
        RxExecute(pm->pszScript ? pm->pszScript : "");
        fMacroPlayingNow = FALSE;
        #if DBGLOG
          fprintf(stderr, "End macro playing: %s\n", szKey);
        #endif
        return TRUE;
      }
    }
    if (fLocalWalk)
    {
      pm         = pGlobalList;
      fLocalWalk = FALSE;
    }
    else
    {
      fMacroPlayingNow   = FALSE;
      #if DBGLOG
        fprintf(stderr, "No macro for: %s\n", szKey);
      #endif
      return FALSE;
    }
  }
}
BOOL MACROS::MacroDlg()
{
  static CPARAMSTR  cps;
  cps.l1      = (LONG)this;
  return WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpMacroDlg,NULLHANDLE,IDD_MACRODLG,&cps);
}
BOOL MACROS::RecallMacroEd(BOOL fTestEnabled)
{
  static CPARAMSTR  cps;
  BOOL              fOk = FALSE;
  PMACRO            pm;
  for (pm=pLocalList;  pm; pm=pm->next)
    if (pm->nLastEdId==nLastEdId)
      {fOk=TRUE; break;}
  if (!fOk)
    for (pm=pGlobalList; pm; pm=pm->next)
      if (pm->nLastEdId==nLastEdId)
        {fOk=TRUE; break;}
  if (fTestEnabled) return fOk;
  if (fOk)
  {
    cps.l1 = (LONG)pm;
    WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpEdMacroDlg,NULLHANDLE,IDD_EDMACRODLG,&cps);
  }
  else
    Beep();
}
void MACROS::WProfile(LONG lSlot, PSZ szName, BOOL fGlobal)
{
  PMACRO *ppList = fGlobal ? &pGlobalList : &pLocalList;
  PMACRO  pm;
  char    sz[100];
  LONG    lTotal;
  sprintf(sz,"%s_total",szName);
  PfReadLong(lSlot, sz, &lTotal, 0);
  while(--lTotal >= 0)
  {
    sprintf(sz,"%s%u",szName,lTotal);
    PfDeleteItem(lSlot, sz);
  }
  for (pm = *ppList,lTotal=0; pm; pm=pm->next,lTotal++)
  {
    sprintf(sz,"%s%u",szName,lTotal);
    pm->WProfile(lSlot,sz);
  }
  sprintf(sz,"%s_total",szName);
  PfWriteLong(lSlot, sz, lTotal);
}
void MACROS::RProfile(LONG lSlot, PSZ szName, BOOL fGlobal)
{
  PMACRO *ppList = fGlobal ? &pGlobalList : &pLocalList;
  char    sz[100];
  LONG    lTotal;
  sprintf(sz,"%s_total",szName);
  PfReadLong(lSlot, sz, &lTotal, 0);
  free_list(*ppList);
  *ppList = 0;
  while(--lTotal >= 0)
  {
    PMACRO pmNew = new MACRO;
    sprintf(sz,"%s%u",szName,lTotal);
    if (pmNew->RProfile(lSlot,sz))
    {
      pmNew->next = *ppList;
      *ppList     = pmNew;
    }
    else
      delete pmNew;
  }
}
    ins_mac_item(HWND hList, PMACRO pm, int nIt)
    {
      MPARAM     mmp1, mmp2;
      Acc2Mp(&pm->rAccel,&mmp1,&mmp2);
      PSZ  szKey     = Mp2Txt(&mmp1, &mmp2);
      BOOL fFile     = pm->pszFile && strcmp(pm->pszFile,"*") && strcmp(pm->pszFile,"*.*");
      PSZ  pszStr    = sf_mallocstr(pm->pszComment);
      BOOL fBrackets = pszStr && *sf_skipspaces(pszStr) && (fFile || szKey);
      if (fBrackets) pszStr = sf_mallocconcat(pszStr," (");
      if (szKey)     pszStr = sf_mallocconcat(pszStr,szKey);
      if (fFile)
      {
        if (szKey) pszStr = sf_mallocconcat(pszStr, ", ");
        pszStr            = sf_mallocconcat(pszStr, "Files: ");
        pszStr            = sf_mallocconcat(pszStr, pm->pszFile);
      }
      if (fBrackets) pszStr = sf_mallocconcat(pszStr,")");
      if (!pszStr || !*sf_skipspaces(pszStr))
      {
        free(pszStr);
        pszStr = sf_mallocstr("<No comment, no key, no file>");
      }
      nIt = (int)WinSendMsg(hList, LM_INSERTITEM,    MPARAM(nIt), MPARAM(pszStr));
      WinSendMsg           (hList, LM_SETITEMHANDLE, MPARAM(nIt), MPARAM(pm));
      free(pszStr);
    }
BOOL MACROS::list_event (PMACRO pmGlobal, PMACRO pmLocal, HWND hGlobal, HWND hLocal, BOOL fGlobal, LISTEV listev)
{
  // enum LISTEV { INIT, MOVE_UP, MOVE_DOWN, LIST2LIST, EDIT, DUPLICATE, REMOVE, NEW, SELCHANGED };
  // pmGlobal & pmLocal used for INIT event only
  PMACRO pmList = fGlobal ? pmGlobal : pmLocal;
  HWND   hList  = fGlobal ? hGlobal  : hLocal;
  int    nIt    = (int)   WinSendMsg(hList,LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
  PMACRO pm     = (PMACRO)WinSendMsg(hList,LM_QUERYITEMHANDLE,MPARAM(nIt),      0);
  switch(listev)
  {
    case INIT:
    {
      WinSendMsg(hList,LM_DELETEALL,0,0);
      for(pm=pmList,nIt=0; pm; pm=pm->next,nIt++)
        ins_mac_item(hList,pm,nIt);
      WinSendMsg(hList,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
      list_event(0, 0, hGlobal, hLocal, TRUE,  MACROS::SELCHANGED);
      list_event(0, 0, hGlobal, hLocal, FALSE, MACROS::SELCHANGED);
      return TRUE;
    }
    case MOVE_UP:
    case MOVE_DOWN:
    {
      int  nTotal = (int)WinSendMsg(hList,LM_QUERYITEMCOUNT,0,0);
      BOOL fUp    = listev==MOVE_UP;
      if (fUp && nIt<1 || !fUp && nIt+1>=nTotal) break;
      int  h      = (int)WinSendMsg(hList,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
      int  len    = (int)WinSendMsg(hList,LM_QUERYITEMTEXTLENGTH,MPARAM(nIt), 0) + 1;
      PSZ  psz    = (PSZ)malloc(len);
      if (!psz) break;
      WinSendMsg(hList,LM_QUERYITEMTEXT,  MPFROM2SHORT(nIt,len),MPARAM(psz));
      WinSendMsg(hList,LM_DELETEITEM,MPARAM(nIt),0);
      nIt = (int)WinSendMsg(hList,LM_INSERTITEM,MPARAM(nIt + (fUp ? -1 : 1)),MPARAM(psz));
      WinSendMsg(hList,LM_SETITEMHANDLE, MPARAM(nIt), MPARAM(h));
      WinSendMsg(hList,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
      return TRUE;
    }
    case LIST2LIST:
    {
      if (!pm) break;
      PMACRO pmNew = new MACRO;
      pmNew->Copy(pm);
      HWND hTarg  = fGlobal ? hLocal  : hGlobal;
      ins_mac_item(hTarg, pmNew, 0);
      WinSendMsg(hTarg,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
      if (!fGlobal)
      {
        WinSendMsg(hList,LM_DELETEITEM,MPARAM(nIt),MPARAM(TRUE));
        WinSendMsg(hList,LM_SELECTITEM,MPARAM(nIt ? nIt-1 : 0),MPARAM(TRUE));
      }
      return TRUE;
    }
    case EDIT:
    {
      static CPARAMSTR  cps;
      if (!(cps.l1 = (LONG)pm)) break;
      if (WinDlgBox(HWND_DESKTOP,hList,(PFNWP)dpEdMacroDlg,NULLHANDLE,IDD_EDMACRODLG,&cps))
      {
        WinSendMsg(hList,LM_DELETEITEM,MPARAM(nIt),0);
        ins_mac_item(hList, pm, nIt);
        WinSendMsg(hList,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
      }
      return TRUE;
    }
    case NEW:
    {
      static CPARAMSTR  cps;
      PMACRO pmNew = new MACRO;
      if (!(cps.l1 = (LONG)pmNew)) break;
      if (WinDlgBox(HWND_DESKTOP,hList,(PFNWP)dpEdMacroDlg,NULLHANDLE,IDD_EDMACRODLG,&cps))
      {
        ins_mac_item(hList, pmNew, nIt);
        WinSendMsg(hList,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
      }
      else
        delete pmNew;
      return TRUE;
    }
    case DUPLICATE:
    {
      if (!pm) break;
      PMACRO pmNew = new MACRO;
      pmNew->Copy(pm);
      ins_mac_item(hList, pmNew, nIt);
      WinSendMsg(hList,LM_SELECTITEM,MPARAM(nIt+1),MPARAM(TRUE));
      return TRUE;
    }
    case REMOVE:
    {
      int rc = fGlobal ?
        MessageBox(hList,"WARNING!",    MB_ICONQUESTION|MB_OKCANCEL,"You are going to remove GLOBAL macro. If you will do it, all the workplaces will never can use this macro!")
       :MessageBox(hList,"Remove macro",MB_ICONQUESTION|MB_OKCANCEL,"You are going to remove local macro.");
      if (rc==MBID_OK)
      {
        delete pm;
        WinSendMsg(hList,LM_DELETEITEM,MPARAM(nIt),0);
        if (!WinSendMsg(hList,LM_SELECTITEM,MPARAM(nIt?nIt-1:0),MPARAM(TRUE)))
          list_event (pmGlobal, pmLocal, hGlobal, hLocal, fGlobal, SELCHANGED);
      }
      return TRUE;
    }
    case SELCHANGED:
    {
      int  nTotal = (int)WinSendMsg(hList,LM_QUERYITEMCOUNT,0,0);
      HWND hDlg = WinQueryWindow(hList,QW_PARENT);
      BOOL fSel = nIt!=LIT_NONE;
      WinEnableControl(hDlg,fGlobal ? IDD_PBMACCP2LOC    : IDD_PBMACMV2GLOB, fSel);
      WinEnableControl(hDlg,fGlobal ? IDD_PBMACGLOBED    : IDD_PBMACLOCED,   fSel);
      WinEnableControl(hDlg,fGlobal ? IDD_PBMACGLOBREM   : IDD_PBMACLOCREM,  fSel);
      WinEnableControl(hDlg,fGlobal ? IDD_PBMACGLOBDUP   : IDD_PBMACLOCDUP,  fSel);
      WinEnableControl(hDlg,fGlobal ? IDD_PBMACGLOBUP    : IDD_PBMACLOCUP,   fSel && nIt>0       );
      WinEnableControl(hDlg,fGlobal ? IDD_PBMACGLOBDOWN  : IDD_PBMACLOCDOWN, fSel && nIt+1<nTotal);
      return TRUE;
    }
  }
  return FALSE;
}
PMACRO MACROS::dup_list(PMACRO pmList)
{
  PMACRO pmRet = 0;
  PMACRO pmRTail;
  for (PMACRO pmSrc=pmList; pmSrc; pmSrc=pmSrc->next)
  {
    PMACRO pmNew = new MACRO;
    pmNew->Copy(pmSrc);
    if (pmRet) pmRTail->next = pmNew;
    else       pmRet         = pmNew;
    pmRTail = pmNew;
  }
  return pmRet;
}
void MACROS::free_list(PMACRO pmList)
{
  for (PMACRO pm; pmList; pmList = pm)
  {
    pm = pmList->next;
    delete pmList;
  }
}
MRESULT EXPENTRY dpMacroDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  static HWND      hGlobal;
  static HWND      hLocal;
  static PMACROS   pThis;
  static int       nLastEdId0;

  switch (ulMsg)
  {
    case WM_INITDLG:
    {
      {
        PCPARAMSTR pcps = (PCPARAMSTR)m2;
        pThis           = PMACROS(pcps->l1);
      }
      hGlobal = WinWindowFromID(hWnd,IDD_LBMACGLOB);
      hLocal  = WinWindowFromID(hWnd,IDD_LBMACLOC);
      PMACRO pmGlobal = pThis->dup_list(pThis->pGlobalList);
      PMACRO pmLocal  = pThis->dup_list(pThis->pLocalList);
      nLastEdId0      = MACROS::nLastEdId;
      pThis->list_event(pmGlobal, pmLocal, hGlobal, hLocal, TRUE,  MACROS::INIT);
      pThis->list_event(pmGlobal, pmLocal, hGlobal, hLocal, FALSE, MACROS::INIT);
      // pmGlobal & pmLocal будут дезинтегрированы и освобождены посредством листбоксовых хэндлов
      break;
    }
    case WM_CONTROL:
      if (SHORT2FROMMP(m1)==LN_SELECT)
        pThis->list_event(0, 0, hGlobal, hLocal, SHORT1FROMMP(m1)==IDD_LBMACGLOB, MACROS::SELCHANGED);
      break;
    case WM_USER: // Build back the (m1 ? Global : Local) list
      {
        HWND    hList  = m1 ? hGlobal             : hLocal;
        PMACRO *ppm    = m1 ? &pThis->pGlobalList : &pThis->pLocalList;
        int     nTotal = (int)WinSendMsg(hList,LM_QUERYITEMCOUNT,0,0);
        pThis->free_list(*ppm); *ppm = 0;
        for (int nIt = 0; nIt<nTotal; nIt++)
        {
          PMACRO pm = (PMACRO)WinSendMsg(hList,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
          if (!pm) break;
          *ppm     = pm;
          pm->next = 0;
          ppm      = &pm->next;
        }
      }
      break;
    case WM_USER+1: // free (m1 ? Global : Local) list
      {
        HWND hList  = m1 ? hGlobal             : hLocal;
        int  nTotal = (int)WinSendMsg(hList,LM_QUERYITEMCOUNT,0,0);
        for (int nIt = 0; nIt<nTotal; nIt++)
          delete (PMACRO)WinSendMsg(hList,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
        WinSendMsg(hList,LM_DELETEALL,0,0);
      }
      break;
    case WM_CLOSE:
      m1 = MPARAM(DID_CANCEL);
    case WM_COMMAND:
    {
      int s1m1 = SHORT1FROMMP(m1);
      switch(s1m1)
      {
        case DID_OK:
          WinSendMsg(hWnd,WM_USER,MPARAM(TRUE), 0);
          WinSendMsg(hWnd,WM_USER,MPARAM(FALSE),0);
          WinDismissDlg(hWnd,1);
          break;
        case DID_CANCEL:
          WinSendMsg(hWnd,WM_USER+1,MPARAM(TRUE), 0);
          WinSendMsg(hWnd,WM_USER+1,MPARAM(FALSE),0);
          MACROS::nLastEdId = nLastEdId0;
          WinDismissDlg(hWnd,0);
          break;
        case IDD_PBMACGLOBUP:
        case IDD_PBMACLOCUP:
          pThis->list_event(0, 0, hGlobal, hLocal, s1m1==IDD_PBMACGLOBUP, MACROS::MOVE_UP);
          break;
        case IDD_PBMACGLOBDOWN:
        case IDD_PBMACLOCDOWN:
          pThis->list_event(0, 0, hGlobal, hLocal, s1m1==IDD_PBMACGLOBDOWN, MACROS::MOVE_DOWN);
          break;
        case IDD_PBMACCP2LOC:
        case IDD_PBMACMV2GLOB:
          pThis->list_event(0, 0, hGlobal, hLocal, s1m1==IDD_PBMACCP2LOC, MACROS::LIST2LIST);
          break;
        case IDD_PBMACGLOBED:
        case IDD_PBMACLOCED:
          pThis->list_event(0, 0, hGlobal, hLocal, s1m1==IDD_PBMACGLOBED, MACROS::EDIT);
          break;
        case IDD_PBMACGLOBREM:
        case IDD_PBMACLOCREM:
          pThis->list_event(0, 0, hGlobal, hLocal, s1m1==IDD_PBMACGLOBREM, MACROS::REMOVE);
          break;
        case IDD_PBMACGLOBDUP:
        case IDD_PBMACLOCDUP:
          pThis->list_event(0, 0, hGlobal, hLocal, s1m1==IDD_PBMACGLOBDUP, MACROS::DUPLICATE);
          break;
        case IDD_PBMACGLOBNEW:
        case IDD_PBMACLOCNEW:
          pThis->list_event(0, 0, hGlobal, hLocal, s1m1==IDD_PBMACGLOBNEW, MACROS::NEW);
          break;
        case IDD_PBHELP:
          DisplayHelpPanel(PANEL_MACRODLG);
          break;
      }
      break;
    }
    case WM_CHAR:
      if(SHORT2FROMMP(m2)==VK_F1)
      {
        DisplayHelpPanel(PANEL_MACRODLG);
        break;
      }
    default:
      return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}
MRESULT EXPENTRY dpEdMacroDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PMACRO    pMacro;
  static HWND      hEFComment;
  static HWND      hEFFile;
  static HWND      hQAKey;
  static HWND      hMLE;

  switch (ulMsg)
  {
    case WM_INITDLG:
      {
        PCPARAMSTR pcps = (PCPARAMSTR)m2;
        pMacro = PMACRO(pcps->l1);
      }
      // Mark this macro to recall editor
      pMacro->nLastEdId = ++MACROS::nLastEdId;
      { // Crearte WC_QUERYACCEL window:
        SWP  swp;
        HWND hST = WinWindowFromID(hWnd,IDD_STEDMKEY);
        WinQueryWindowPos(hST,&swp);
        hQAKey   = WinCreateWindow(hWnd, WC_QUERYACCEL, "", WS_VISIBLE,
                                        swp.x, swp.y, swp.cx, swp.cy,hWnd,
                                        HWND_TOP, IDD_STMMONKEY, 0, 0);
        MPARAM mmp1,mmp2;
        Acc2Mp(&pMacro->rAccel,&mmp1,&mmp2);
        Mp2Txt(&mmp1, &mmp2);
        WinSendMsg(hQAKey,WM_USER+1,mmp1,mmp2);
      }
      hMLE       = WinWindowFromID(hWnd,IDD_MLEEDMSCRIPT);
      hEFComment = WinWindowFromID(hWnd,IDD_EFEDMCOMMENT);
      hEFFile    = WinWindowFromID(hWnd,IDD_EFEDMFILE);
      WinSetWindowText(hMLE,       pMacro->pszScript  ? pMacro->pszScript  : "");
      WinSetWindowText(hEFComment, pMacro->pszComment ? pMacro->pszComment : "");
      WinSetWindowText(hEFFile,    pMacro->pszFile    ? pMacro->pszFile    : "");
      break;
    case WM_CLOSE:
      m1 = MPARAM(DID_CANCEL);
    case WM_COMMAND:
      switch(SHORT1FROMMP(m1))
      {
        case IDD_PBEDMKEY:
        {
          char      sz[32];
          CPARAMSTR cps;
          {
            MPARAM m1     = (MPARAM)WinQueryWindowULong(hQAKey,QWL_USER+0);
            MPARAM m2     = (MPARAM)WinQueryWindowULong(hQAKey,QWL_USER+4);
            PSZ    psz    = Mp2Txt(&m1,&m2);
            strcpy(sz,psz ? psz : "");
          }
          cps.psz1 = "Edit macro";
          cps.psz2 = "Key for this macro:";
          cps.psz3 = sz;
          cps.l1   = sizeof(sz);
          if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
          {
            PSZ    psz = sz;
            ACCEL  rAccel;
            if (!StrToAcc(&psz, &rAccel))
              MessageBox(hWnd,"Key definition",MB_ERROR|MB_OK,"Wrong key definition.");
            else
            {
              MPARAM mmp1,mmp2;
              Acc2Mp(&rAccel,&mmp1,&mmp2);
              Mp2Txt(&mmp1, &mmp2);
              WinSendMsg(hQAKey,WM_USER+1,mmp1,mmp2);
            }
          }
          break;
        }
        case DID_OK:
        {
          int    n;
          MPARAM m1     = (MPARAM)WinQueryWindowULong(hQAKey,QWL_USER+0);
          MPARAM m2     = (MPARAM)WinQueryWindowULong(hQAKey,QWL_USER+4);
          Mp2Txt(&m1,&m2);
          Mp2Acc(m1,m2,&pMacro->rAccel);
          free(pMacro->pszScript);  pMacro->pszScript  = 0;
          free(pMacro->pszComment); pMacro->pszComment = 0;
          free(pMacro->pszFile);    pMacro->pszFile    = 0;
          n = WinQueryWindowTextLength(hMLE);
          if (n && (pMacro->pszScript  = (PSZ)malloc(n+1))) WinQueryWindowText(hMLE,       n+1, pMacro->pszScript);
          n = WinQueryWindowTextLength(hEFComment);
          if (n && (pMacro->pszComment = (PSZ)malloc(n+1))) WinQueryWindowText(hEFComment, n+1, pMacro->pszComment);
          n = WinQueryWindowTextLength(hEFFile);
          if (n && (pMacro->pszFile    = (PSZ)malloc(n+1))) WinQueryWindowText(hEFFile,    n+1, pMacro->pszFile   );
          WinDismissDlg(hWnd,1);
          break;
        }
        case DID_CANCEL:
          WinDismissDlg(hWnd,0);
          break;
        case IDD_PBHELP:
          DisplayHelpPanel(PANEL_EDMACRODLG);
          break;
      }
      break;
    case WM_CHAR:
      if(SHORT2FROMMP(m2)==VK_F1)
      {
        DisplayHelpPanel(PANEL_EDMACRODLG);
        break;
      }
    default:
      return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

#define TYPEBUFLEN    200 //
#define EDMOVECMD     0x1 // EdMove(dx,dy[,SELECT])
#define EDMOVESELCMD  0x2 //             [,SELECT]
#define EDTYPECMD     0x4 // EdType(text)
#define EDJUNKCMD     0x8 // MacroStartRecord - skip it
#define SGN(n)  (n<0 ? -1 : n>0 ? 1 : 0)

int parse_cmd(PSZ pszSrc, PSZ *ppszTypeBuf, int *pdx, int *pdy)
{
  int   dx, dy;
  PSZ   psz,pchE,pch;
  PSZ  &pszBuf = *ppszTypeBuf;
  psz = sf_skipspaces(pszSrc);
  if (!sf_strnicmp(psz,"MacroStartRecord",16))
  {
    psz = sf_skipspaces(psz+16);
    if (*psz!='(')                return 0;
    psz = sf_skipspaces(psz+1);
    if (*psz != ')')              return 0;
    psz = sf_skipspaces(psz+1);
    if (*psz==';') psz = sf_skipspaces(psz+1);
    if (*psz)                     return 0;
    return EDJUNKCMD;
  }
  if (!sf_strnicmp(psz,"EdType",6))
  {
    psz = sf_skipspaces(psz+6);
    if (*psz!='(')                return 0;   // EdType(
    psz = sf_skipspaces(psz+1);
    char ch = *psz;
    if (ch!='"' && ch != '\'')    return 0;   // EdType("
    psz++;
    if (!(pchE = strchr(psz,ch))) return 0;   // EdType("..."
    pch = sf_skipspaces(pchE+1);
    if (*pch != ')')              return 0;   // EdType("...")
    pch = sf_skipspaces(pch+1);
    if (*pch==';') pch = sf_skipspaces(pch+1);
    if (*pch)                     return 0;   // EdType("...") [;] [CR LF] EOL
    if (memchr(psz,'"',pchE-psz)) return 0;   // '..."...' - oops
    // Ok Ob
    int nLen = pszBuf ? strlen(pszBuf) : 0;
    if (!(pszBuf = (PSZ)realloc(pszBuf,nLen+pchE-psz+1))) throw "Out of memory error.";
    strncpy(pszBuf+nLen,psz,pchE-psz);
    pszBuf[nLen+(pchE-psz)] = 0;
    return EDTYPECMD;
  }
  if (!sf_strnicmp(psz,"EdMove",6))
  {
    int rc = EDMOVECMD;
    psz    = sf_skipspaces(psz+6);
    if (*psz!='(')                return 0;   // EdMove(
    dx = strtol(psz+1,&pchE,0);
    if (pchE==psz+1 || *(psz = sf_skipspaces(pchE))!=',') return 0; // EdMove(<dx>,
    dy = strtol(psz+1,&pchE,0);
    if (pchE==psz+1)                                      return 0; // EdMove(<dx>,<dy>
    pchE = sf_skipspaces(pchE);
    if (*pchE == ',')
    {
      pchE = sf_skipspaces(pchE+1);
      if      (!strncmp(pchE,"SELECT",6))                                      pchE += 6;
      else if (!strncmp(pchE,"'SELECT'",8) || !strncmp(pchE,"\"SELECT\"",8))   pchE += 8;
      else                                                                     return 0;
      pchE = sf_skipspaces(pchE);
      rc |= EDMOVESELCMD;
    }
    if (*pchE != ')')             return 0;   // EdMove("...")
    pchE = sf_skipspaces(pchE+1);
    if (*pchE==';') pchE = sf_skipspaces(pchE+1);
    if (*pchE)                    return 0;   // EdMove("...") [;] [CR LF] EOL
    // Ok Ob
    *pdx = dx;
    *pdy = dy;
    return rc;
  }
  return 0;
}

MRESULT EXPENTRY dpMacroMonDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
/* Дополнительные сообщения:
/  WM_USER    (PSZ m1) - добавить текст к скрипту
/  WM_USER+1  ()       - готово. Если не все поля заполнены - запросить, иначе - OK.
/  WM_USER+2  (m1,m2)  - вывод в MLE:    m1 - ipt (-1 = tail), m2 - psz, returns ipt хвоста текста
/  WM_USER+3  (m1,m2)  - вывод в script: m1 - nPos(-1 = tail), m2 - psz
/  WM_USER+10 ()       - internal
*/
  static PMACROS pThis;
  static PSZ       pszScript  = 0;      // Тело макроса
  static int       nScriptTail;         // Поз. конца текста в скрипте
  static int       nScriptBeforeLast;   // Тут был последний вывод EdType или EdMove в скрипте
  static IPT       iptTail;             // Конец текста в MLE
  static IPT       iptBeforeLast;       // ОтсюдаТут был последний вывод в MLE
  static int       xMove,yMove;
  static int       nLastMode  = 0;
  static PSZ       pszTypeBuf = 0;
  static BOOL      fDetailsON = FALSE;
  static HWND      hEFComment;
  static HWND      hQAKey;
  static HWND      hEFFile;
  static HWND      hMLE;
  static BOOL      fSwitchBack;

  switch (ulMsg)
  {
    case WM_INITDLG:
      {
        PCPARAMSTR pcps = (PCPARAMSTR)m2;
        pThis = PMACROS(pcps->l1);
      }
      { // Crearte WC_QUERYACCEL window:
        SWP  swp;
        HWND hST = WinWindowFromID(hWnd,IDD_STMMONKEY);
        WinQueryWindowPos(hST,&swp);
        hQAKey   = WinCreateWindow(hWnd, WC_QUERYACCEL, "", WS_VISIBLE,
                                        swp.x, swp.y, swp.cx, swp.cy,hWnd,
                                        HWND_TOP, IDD_STMMONKEY, 0, 0);
      }
      { // Init all...
        hEFComment = WinWindowFromID(hWnd,IDD_EFMMONCOMMENT);
        hEFFile    = WinWindowFromID(hWnd,IDD_EFMMONFILE);
        free(pszScript);     pszScript  = 0;
        free(pszTypeBuf);    pszTypeBuf = 0;
        nScriptTail = nScriptBeforeLast = 0;
        iptTail     = iptBeforeLast     = 0;
        nLastMode   = 0;
        fSwitchBack = TRUE;
        WinSetWindowText(hEFFile, "*");
      }
      hMLE           = WinWindowFromID(hWnd,IDD_MLEMMONSCRIPT);
      WinSendMsg(hWnd,WM_USER+10,0,0); // Show/hide details
      WinSetFocus(HWND_DESKTOP,hQAKey);
      pThis->fMacroPlayingNow = FALSE; // We're not a critical macro, but it was still TRUE...
      return MRESULT(TRUE);
    case WM_CONTROL:
      if (SHORT2FROMMP(m1)==EN_CHANGE && HWND(m2)==hQAKey && fSwitchBack && WinQueryWindowULong(hQAKey,QWL_USER+0))
      {
        WinDefWindowProc(hMDI0Client,WM_BUTTON1DOWN,MPARAM(0),MPFROM2SHORT(HT_NORMAL,KC_NONE));
        fSwitchBack = FALSE;
      }
      break;
    case WM_USER: // m1 = text to append. EdMove & EdType are processed in a special way...
      try
      {
        static int   dxMove, dyMove;
        static int   dxOld,  dyOld;
        int          dx,     dy;
        int          nMode = parse_cmd((PSZ)m1, &pszTypeBuf,&dx,&dy);
        if (nMode & EDTYPECMD)
        {
          int nBufLen = strlen(pszTypeBuf);
          if (nBufLen>=TYPEBUFLEN && nLastMode==nMode)
          { // Разорвем слияние EdType
            nLastMode  = 0;
            free(pszTypeBuf); pszTypeBuf = 0;
            parse_cmd((PSZ)m1, &pszTypeBuf,&dx,&dy);
            nBufLen    = strlen(pszTypeBuf);
          }
          if (nLastMode == nMode)
          { // _Слияние_ EdType(s) вместо предыдущего
            PSZ sz;
            if (!(sz = (PSZ)malloc(nBufLen + 50))) throw "Out of memory error.";
            sprintf(sz,"EdType('%s');\r\n",pszTypeBuf);
            iptTail     = (IPT)WinSendMsg   (hWnd,WM_USER+2,MPARAM(iptBeforeLast),    MPARAM(sz));
            nScriptTail = (int)dpMacroMonDlg(hWnd,WM_USER+3,MPARAM(nScriptBeforeLast),MPARAM(sz)); // try{} => call
            free(sz);
            break;
          }
          // else EdType - обработается как простая новая команда
        }
        else
          free(pszTypeBuf), pszTypeBuf = 0;
        if(nMode & EDMOVECMD)
        {
          if (SGN(dxOld) != SGN(dx) || SGN(dyOld) != SGN(dy))  nLastMode = 0;
          if (nLastMode == nMode)
          {
            static char sz[100];
            dxMove       += dx;
            dyMove       += dy;
            sprintf(sz,"EdMove(%d,%d%s);\r\n", dxMove, dyMove,nMode & EDMOVESELCMD ? ",SELECT" : "");
            iptTail     = (IPT)WinSendMsg   (hWnd,WM_USER+2,MPARAM(iptBeforeLast),    MPARAM(sz));
            nScriptTail = (int)dpMacroMonDlg(hWnd,WM_USER+3,MPARAM(nScriptBeforeLast),MPARAM(sz)); // try{} => call
            break;
          }
          else
            dxMove =  dx, dyMove  = dy;
          dxOld = dx; dyOld = dy;
        }
        // Текст в хвост MLE
        nLastMode = nMode;
        {
          nScriptBeforeLast = nScriptTail;
          iptBeforeLast     = iptTail;
          if (nMode != EDJUNKCMD)
          {
            iptTail     = (IPT)WinSendMsg   (hWnd,WM_USER+2,MPARAM(-1), MPARAM((PSZ)m1));
            nScriptTail = (int)dpMacroMonDlg(hWnd,WM_USER+3,MPARAM(-1), MPARAM((PSZ)m1)); // try{} => call
          }
        }
      }
      catch (char *psz)
      {
        pThis->TerminateMacro(psz);
      }
      break;
    case WM_USER+1: //  готово. Если не все поля заполнены - запросить, иначе - OK.
      return WinSendMsg(hWnd,WM_COMMAND,MPARAM(DID_OK),0);
    case WM_USER+2: //  - вывод в MLE: m1 - ipt (-1 = tail), m2 - psz, returns ipt хвоста текста
    {
      IPT iptHi   = (IPT)WinSendMsg(hMLE, MLM_QUERYTEXTLENGTH, 0,0);
      IPT iptTarg = (IPT(m1)==-1) ? iptHi : IPT(m1);
      WinSendMsg(hMLE, MLM_SETSEL, MPARAM(iptTarg), MPARAM(iptHi));
      WinSendMsg(hMLE, MLM_INSERT, MPARAM(m2),      0);
      return MRESULT(WinSendMsg(hMLE,MLM_QUERYSEL,MPARAM(MLFQS_CURSORSEL),0));
    }
    case WM_USER+3: // (m1,m2)  - вывод в script: m1 - nPos(-1 = tail), m2 - psz; returns script length
    {
      int nSLen   = pszScript ? strlen(pszScript) : 0;
      int nTLen   = strlen(PSZ(m2));
      int nPosUse = (int(m1)==-1) ? nSLen : int(m1);
      if (!(pszScript = (PSZ)realloc(pszScript,nPosUse + nTLen + 1)))
        return 0;
      strcpy(pszScript+nPosUse,PSZ(m2));
      return MRESULT(nPosUse + nTLen);
    }
    case WM_USER+10:
    {
      SWP swp,swpDT;
      WinQueryWindowPos(hMLE,&swp);
      int cx = fDetailsON ? swp.x+swp.cx+5 : swp.x;
      WinQueryWindowPos(hWnd,&swp);
      WinQueryWindowPos(HWND_DESKTOP,&swpDT);
      swp.x = swpDT.cx-cx;
      swp.y = 0;
      WinSetWindowPos(hWnd,0,swp.x,swp.y,cx,swp.cy,SWP_MOVE|SWP_SIZE);
      WinSetDlgItemText(hWnd,IDD_PBMMONDETAILS,fDetailsON ? "No ~details" : "~Details >>");
      break;
    }
    case WM_CLOSE:
      m1 = MPARAM(DID_CANCEL);
    case WM_COMMAND:
      switch(SHORT1FROMMP(m1))
      {
        case DID_OK:
          try
          {
            PMACRO pm;
            int    n;
            ACCEL  rAccel = {0,0,0};
            MPARAM m1     = (MPARAM)WinQueryWindowULong(hQAKey,QWL_USER+0);
            MPARAM m2     = (MPARAM)WinQueryWindowULong(hQAKey,QWL_USER+4);
            Mp2Txt(&m1,&m2);
            Mp2Acc(m1,m2,&rAccel);
            {// Duplicated?
              for (pm=pThis->pLocalList; pm; pm=pm->next)
                if (match_accel(&pm->rAccel,&rAccel)) break;
              if (pm)
              {
                int rc = MessageBox(hWnd,"Macro definition (local)",MB_ICONQUESTION|MB_YESNOCANCEL,
                         "This key is used in another macro(s). Forget that macro(s)?");
                WinSetFocus(HWND_DESKTOP,hWnd);
                if (rc==MBID_YES)
                {
                  for (PMACRO *ppm=&pThis->pLocalList; *ppm;)
                  {
                    if (match_accel(&(*ppm)->rAccel,&rAccel))
                    {
                      pm   = *ppm;
                      *ppm = pm->next;
                      delete pm;
                    }
                    else
                      ppm = &(*ppm)->next;
                  }
                }
                else if (rc!=MBID_NO) break;
              }
            }
            // Create new MACRO
            if (!(pm = new MACRO)) throw "out of memory error.";
            pm->rAccel         = rAccel;
            pm->pszScript      = sf_mallocstr(pszScript);
            n = WinQueryWindowTextLength(hEFComment);
            if (pm->pszComment = (PSZ)malloc(n+1)) WinQueryWindowText(hEFComment, n+1, pm->pszComment);
            n = WinQueryWindowTextLength(hEFFile);
            if (pm->pszFile    = (PSZ)malloc(n+1)) WinQueryWindowText(hEFFile,    n+1, pm->pszFile   );
            pm->next           = pThis->pLocalList;
            pThis->pLocalList  = pm;
            WinDismissDlg(hWnd,1);
            pThis->hMacroMon = 0;
            WinSetActiveWindow(HWND_DESKTOP,pMDI->QueryWindow(MDIQW_MAINFRAME));
          }
          catch (char *psz)
          {
            pThis->TerminateMacro(psz);
            WinSetActiveWindow(HWND_DESKTOP,pMDI->QueryWindow(MDIQW_MAINFRAME));
          }
          break; // DID_OK
        case DID_CANCEL:
          WinDismissDlg(hWnd,0);
          pThis->hMacroMon = 0;
          WinSetActiveWindow(HWND_DESKTOP,pMDI->QueryWindow(MDIQW_MAINFRAME));
          break;
        case IDD_PBMMONDETAILS:
          fDetailsON = !fDetailsON;
          WinSendMsg(hWnd,WM_USER+10,0,0);
          break;
        case IDD_PBHELP:
          DisplayHelpPanel(PANEL_MACROMONDLG);
          break;
      }
      break;
    case WM_CHAR:
      if(SHORT2FROMMP(m2)==VK_F1)
      {
        DisplayHelpPanel(PANEL_MACROMONDLG);
        break;
      }
    default:
      return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

///////////////////////////////// REXXMON /////////////////////////////////////

#define MM_SUBCLASS (WM_USER+0)
struct  RXMONSTR
{
  HWND   hMonFrame;
  HWND   hMonLB;
         RXMONSTR()  {memset(this,0,sizeof(this));}
};
static RXMONSTR rms;



MRESULT EXPENTRY fwpRexxMon (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  // REXX monitor frame subclass function
  static PFNWP     pWindowProc;
  switch(ulMsg)
  {
    case MM_SUBCLASS:
      pWindowProc = PFNWP(ULONG(m1));
      return 0;
    case WM_SYSCOMMAND:
      if (SHORT1FROMMP(m1)==SC_CLOSE)
        return WinSendMsg(hWnd,WM_SYSCOMMAND,MPARAM(SC_HIDE), MPFROM2SHORT(CMDSRC_OTHER,0));
  }
  return ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
}

rx_app_string(PSZ pszIn)
{
  //--- pszIn ::= <string> {[CR] LF <string>}
  while(pszIn && *pszIn)
  {
    PSZ pszE = strchr(pszIn,LF);
    if (pszE)
    {
      *pszE = 0;
      if (pszE>pszIn && pszE[-1]==CR) pszE[-1] = 0;
    }
    int nIt = (int)WinSendMsg(rms.hMonLB,LM_QUERYITEMCOUNT,0,0);
    WinSendMsg(rms.hMonLB,LM_INSERTITEM,MPARAM(nIt),MPARAM(pszIn));
    if (nIt >= REXXMONLINES)
    {
      WinSendMsg(rms.hMonLB,LM_DELETEITEM,MPARAM(0),0);
      nIt--;
    }
    WinSendMsg(rms.hMonLB,LM_SELECTITEM,MPARAM(nIt),0);

    pszIn = pszE ? pszE+1 : 0;
  }
}


LONG EXPENTRY rx_exit_handler(LONG  ExitNumber,
                              LONG  Subfunction,
                              PEXIT ParamBlock)
{
  //--- REXX exit handler used to capture execution output
  if (ExitNumber==RXSIO && (Subfunction==RXSIOSAY || Subfunction==RXSIOTRC))
  {
    rx_app_string(PRXSTRING(ParamBlock)->strptr);
    return RXEXIT_HANDLED;
  }
  else
    return RXEXIT_NOT_HANDLED;
}


/*------------------------------------------------------------
> REXX monitor initialisation
>
>*/
void RexxMon_Init()
{
  static BOOL f1stcall = TRUE;
  if (!f1stcall) return;
  f1stcall = FALSE;
  int rc = RexxRegisterExitExe(RXEXIT_NAME,(PFN)rx_exit_handler,0);
  if (!rc || rc==RXEXIT_DUP) fRexxMonReady = TRUE;

  // Create window
  {
    HWND        hFr;
    FRAMECDATA  fcdata;
    fcdata.cb            = sizeof(fcdata);
    fcdata.flCreateFlags = FCF_TITLEBAR|FCF_SIZEBORDER|FCF_NOBYTEALIGN|FCF_MINBUTTON|FCF_MAXBUTTON|
                           FCF_HIDEBUTTON|FCF_SYSMENU;
    HPOINTER hptrIco     = (HPOINTER)WinLoadPointer(HWND_DESKTOP,0,ICO_REXXMON);

    rms.hMonFrame = hFr  = WinCreateWindow(HWND_DESKTOP, WC_FRAME, "REXX execution monitor", 0, 0,0,1,1,
                                           hMDI0Client, HWND_BOTTOM, 0, &fcdata, NULL);
    rms.hMonLB           = WinCreateWindow(hFr, WC_LISTBOX, 0, WS_VISIBLE|LS_HORZSCROLL|LS_NOADJUSTPOS, 0,0,1,1,
                                           hFr, HWND_TOP, FID_CLIENT, 0, 0);
    if (!rms.hMonLB) { WinDestroyWindow(hFr); rms.hMonFrame = 0; return;}

    WinSendMsg(hFr,WM_SETICON,MPARAM(ULONG(hptrIco)),0);
    int cxs = WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN);
    int cys = WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN);
    WinSetWindowPos(hFr, 0, 0,       0,       cxs/3-3, cys/3-3, SWP_SIZE);
    WinSetWindowPos(hFr, 0, cxs/3*2, cys/3*2, 0,       0,       SWP_MOVE);
    fwpRexxMon(hFr, MM_SUBCLASS, MPARAM(ULONG(WinSubclassWindow(hFr,(PFNWP)fwpRexxMon))),0L);
  }
}


/*------------------------------------------------------------
> Toggle REXX monitor show state
>   int nState (in):  0/1/another = hide/show/toggle state
>   Returns        :  show state
>*/
BOOL RexxMon_Toggle(int nState)
{
  if (rms.hMonFrame)
  {
    LONG ws = WinQueryWindowULong(rms.hMonFrame, QWL_STYLE);
    if (nState && nState!=1) nState = !(ws & WS_VISIBLE);
    if (nState && (ws & WS_MINIMIZED)) WinSetWindowPos(rms.hMonFrame, 0,0,0,0,0, SWP_RESTORE);
    WinShowWindow(rms.hMonFrame, nState);
  }
  return nState;
}


/*------------------------------------------------------------
> Activates the monitor
>
>
>*/
void RexxMon_Activate()
{
  if (rms.hMonFrame)
  {
    LONG ws = WinQueryWindowULong(rms.hMonFrame, QWL_STYLE);
    if (!(ws & WS_VISIBLE)) WinShowWindow(rms.hMonFrame, TRUE);
    if (ws & WS_MINIMIZED)  WinSetWindowPos(rms.hMonFrame, 0,0,0,0,0, SWP_RESTORE);
    WinSetFocus(HWND_DESKTOP,rms.hMonLB);
  }
}


/*------------------------------------------------------------
> White REXX conitor configuration (to the 0 slot)
>   PSZ pszKey (in)
>*/
void RexxMon_WProfile(PSZ pszKey)
{
  if (rms.hMonFrame)
  {
    char        sz [32];
    SWP         swp;
    LONG        ws = WinQueryWindowULong(rms.hMonFrame, QWL_STYLE);
    WinQueryWindowPos(rms.hMonFrame, &swp);
    sprintf     (sz,"%s_swp",pszKey);
    PfWriteItem (0, sz, &swp, sizeof (swp));
    sprintf     (sz,"%s_ws",pszKey);
    PfWriteLong (0, sz, ws);
  }
}


/*------------------------------------------------------------
> Read REXX conitor configuration (from the 0 slot)
>   PSZ pszKey (in)
>*/
void RexxMon_RProfile(PSZ pszKey)
{
  HWND hFr;
  if (hFr=rms.hMonFrame)
  {
    char        sz [32];
    SWP         swp;
    LONG        ws = WinQueryWindowULong(hFr, QWL_STYLE);
    WinQueryWindowPos(hFr,&swp);
    sprintf     (sz,"%s_swp",pszKey);
    PfReadItem  (0, sz, &swp, sizeof (swp), 0,0);
    sprintf     (sz,"%s_ws",pszKey);
    PfReadLong  (0, sz, (PLONG)&ws, ws);
    WinSetWindowPos(hFr, 0, 0,0,0,0, SWP_RESTORE);
    WinSetWindowPos(hFr, 0, swp.x, swp.y,swp.cx, swp.cy, SWP_MOVE|SWP_SIZE);
    if      (ws & WS_MINIMIZED) WinSetWindowPos(hFr, 0, 0,0,0,0, SWP_MINIMIZE);
    else if (ws & WS_MAXIMIZED) WinSetWindowPos(hFr, 0, 0,0,0,0, SWP_MAXIMIZE);
    WinShowWindow(hFr,ws & WS_VISIBLE);
  }
}


