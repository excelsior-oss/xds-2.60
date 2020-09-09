/*
*
*  File: HLTDLL.CPP
*
*  XDS: realisation of the methods used to allow access to
*       text hilighters contained in a DLLs
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#define  INCL_WIN
#define  INCL_DOS
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "xdsbase.h"

PAINTER::PAINTER()
{
  memset(this,0,sizeof(*this));
}
PAINTER::~PAINTER()
{
  if (hmDll) DosFreeModule(hmDll);
  free(pszDescription);
}


PAINTEROFFICE::PAINTEROFFICE()
{
  memset (this, 0,sizeof(*this));
  pGlobalAssList = new ASSTR(TRUE); // 'Default' association forever...
  pLocalAssList  = 0;
}
PAINTEROFFICE::~PAINTEROFFICE()
{
  PASSTR pass, pass1;
  for (PPAINTER pp1,pp=pPainterList; pp; pp=pp1) { pp1   = pp->next;   delete pp;   }
  for (pass=pGlobalAssList; pass; pass=pass1)    { pass1 = pass->next; delete pass; }
  for (pass=pLocalAssList; pass; pass=pass1)     { pass1 = pass->next; delete pass; }
}
void PAINTEROFFICE::Init(HWND hMsgWnd)
{
  // Прицепляем все dll-ки вида <szHomePath>\..\DLL\hlt*.dll
  // или, если есть ключ HINI_USERPROFILE/XDS/HLT_DLL_PATH, то <ключ>\hlt*.dll
  char          szTpl[CCHMAXPATH];
  HDIR          hdirFindHandle = HDIR_SYSTEM;
  FILEFINDBUF3  FindBuffer     = {0};
  ULONG         ulFindCount    = 1;
  APIRET        rc             = 0;
  char          szPath[CCHMAXPATH] = "";
  ULONG         ul             = sizeof(szPath);
  if (PrfQueryProfileData(HINI_USERPROFILE,"XDS","HLT_DLL_PATH",szPath,&ul) && ul && szPath[0])
    strcat(szPath,"\\");
  else
    sprintf(szPath,"%s\\..\\DLL\\",szHomePath);
  if (!FName2Full(szPath,szPath,FALSE)) return;
  sprintf(szTpl,"%shlt*.dll",szPath);
  if (!DosFindFirst(szTpl, &hdirFindHandle, FILE_ARCHIVED | FILE_READONLY,
                    &FindBuffer, sizeof(FILEFINDBUF3), &ulFindCount, FIL_STANDARD))
  {
    while (!rc)
    {
      char   szFull[CCHMAXPATH];
      strcpy(szFull,szPath);
      strcat(szFull,FindBuffer.achName);
      try
      {// Try to append this DLL
         HMODULE hmDll = 0;
         {
           char szErBuf[100];
           if (DosLoadModule(szErBuf,sizeof(szErBuf),szFull,&hmDll) || !hmDll)
             throw "Can not load this module.";
         }
         PPAINTER pPainter = new(PAINTER);
         if (
             DosQueryProcAddr(hmDll,1, "ClInit",           (PFN*)&pPainter->PClInit)            ||
             DosQueryProcAddr(hmDll,2, "ClQueryMap",       (PFN*)&pPainter->PClQueryMap)        ||
             DosQueryProcAddr(hmDll,3, "ClQueryExText",    (PFN*)&pPainter->PClQueryExText)     ||
             DosQueryProcAddr(hmDll,4, "ClQueryEntryInfo", (PFN*)&pPainter->PClQueryEntryInfo)  ||
             DosQueryProcAddr(hmDll,5, "ClLineLevel",      (PFN*)&pPainter->PClLineLevel)       ||
             DosQueryProcAddr(hmDll,6, "ClClrLine",        (PFN*)&pPainter->PClClrLine)         ||
             DosQueryProcAddr(hmDll,10,"ClDescription",    (PFN*)&pPainter->PClDescription))
         {
           DosFreeModule(hmDll);
           delete pPainter;
           throw "Can not get all the entryes in the module.";
         }
         {
           PSZ pszId  = (*pPainter->PClDescription)(0);
           PSZ pszDsc = (*pPainter->PClDescription)(1);
           if (!pszId || !pszDsc)
           {
             DosFreeModule(hmDll);
             delete pPainter;
             throw "Can not get module description.";
           }
           strcpy(pPainter->szNameId, pszId);
           pPainter->pszDescription = sf_mallocstr(pszDsc);
           pPainter->hmDll          = hmDll;
         }
         // Call the DLL's initialisation route
         {
           PSZ psz;
           char szPth[CCHMAXPATH];
           strcpy(szPth,szPath);
           if ((psz = strrchr(szPth,'\\')) && !psz[1]) *psz = 0;
           if (psz=(*pPainter->PClInit)(szPth)) // ret: malloc()ed text or 0
           {
             char szE[CCHMAXPATH+500];
             sprintf(szE,"Can not attach DLL: %s.\nReason: %s",szFull,psz);
             WinMessageBox(HWND_DESKTOP,hMsgWnd,szE,"Not fatal error",0,MB_ERROR|MB_OK|MB_MOVEABLE);
             free(psz);
             delete pPainter;
             throw "";
           }
         }
         if (!pPainterList) pPainterList = pPainter;
         else
         {
           PPAINTER pp;
           for (pp=pPainterList; pp->next; pp=pp->next);
           pp->next = pPainter;
         }
      }
      catch (char *sz)
      {
        if (sz[0])
        {
          char szE[CCHMAXPATH+300];
          sprintf(szE,"Can not attach DLL: %s.\nReason: %s",szFull,sz);
          WinMessageBox(HWND_DESKTOP,hMsgWnd,szE,"Not fatal error",0,MB_ERROR|MB_OK|MB_MOVEABLE);
        }
      }
      ulFindCount = 1;
      rc = DosFindNext(hdirFindHandle, &FindBuffer, sizeof(FILEFINDBUF3), &ulFindCount);
    }
    DosFindClose(hdirFindHandle);
  }
}

PPAINTER PAINTEROFFICE::LoadPainter(PSZ szNameId, HWND hMsgWnd)
{
  for (PPAINTER pp=pPainterList; pp; pp=pp->next)
    if (!strcmp(pp->szNameId,szNameId)) return pp;
  return 0;
}

PASSTR PAINTEROFFICE::QueryAss(PSZ szFilename)
{
  return *find_ass(szFilename);
}

void   PAINTEROFFICE::ApplyAss (HWND hEd, PASSTR pAss)
{
  EDCONF ec;
  WinSendMsg(hEd,TM_QUERYEDCONF,MPARAM(&ec),0);
  memcpy(&ec.edPalette, &pAss->aaRGB,  sizeof(ec.edPalette));
  memcpy(&ec.fAttrs,    &pAss->fAttrs, sizeof(FATTRS));
  strcpy(ec.szPainterId, pAss->szPainterId);
  ec.fHilite        = pAss->fHilite;
  ec.fHiliteCurLine = pAss->fHiliteCurLine;
  ec.lTabSize       = pAss->ulTabWidth;
  ec.fSmartTab      = pAss->fSmartTabs;
  ec.fAutoidentMode = pAss->fAutoident;
  ec.fTabCharsMode  = pAss->fUseTabChars;
  WinSendMsg(hEd,TM_SETEDCONF,MPARAM(&ec),0);
}
void   PAINTEROFFICE::SetNewAssList(BOOL fGlobal, PASSTR pAssList1, BOOL fApply)
{
  PASSTR *ppList = fGlobal ? &pGlobalAssList : &pLocalAssList;
  for (PASSTR p1,p=*ppList; p; p=p1)
  {
    p1=p->next;
    delete p;
  }
  *ppList = pAssList1;
  if (fApply)
  {
    for (HWND hEd0 = grOffice.EnumEd(); hEd0; hEd0=grOffice.NextEnumEd())
    {
      PSZ pszFName = grOffice.QueryEdFile(hEd0);
      if (!pszFName) pszFName = "";
      PASSTR pAss0 = ptOffice.QueryAss(pszFName);
      ApplyAss(hEd0,pAss0);
    }
  }
}

/*+++ not used?
  void   PAINTEROFFICE::ForgetAsses()
  {
    for (PASSTR pass1, pass=pAssList; pass; pass=pass1)
    {
      pass1 = pass->next;
      delete pass;
    }
    pAssList = 0;
  }
*/

struct ASSPRFSTR
{
  ULONG  cb;
  ASSTR  ass;
  char   szFiles[1];
};
typedef ASSPRFSTR *PASSPRFSTR;


PASSTR r_prf_asslist(LONG slot, BOOL fDefNeeds)
{
  PASSTR    pRet = 0;
  PASSTR    pE   = 0;
  LONG      lAssTotal;
  LONG      lAssCount;
  char      szKey[30];
  PfReadLong(slot, "nAsses", &lAssTotal, 0);

  for(lAssCount=0; lAssCount<lAssTotal; lAssCount++)
  {
    LONG         cb;
    PASSPRFSTR   p;
    PASSTR       pNew;
    sprintf(szKey,"cbAss_%u", lAssCount);
    if (!PfReadLong(slot, szKey, &cb, 0) || cb<sizeof(ASSPRFSTR)) continue;
    if (!(p = (PASSPRFSTR)malloc(cb)))                            continue;
    sprintf(szKey,"Ass_%u", lAssCount);
    if (cb != PfReadItem(slot, szKey, p, cb, 0, 0))    { free(p); continue; }
    pNew = new ASSTR;
    memcpy(pNew,&p->ass,sizeof(*pNew));
    pNew->next     = 0;
    pNew->pszFiles = 0;
    pNew->SetFiles  (p->szFiles);
    free(p);
    if (pNew->fDefault) fDefNeeds = FALSE;

    if (pE) pE->next = pNew;
    else    pRet     = pNew;
    pE               = pNew;
  }

  if(fDefNeeds)
  {
    PASSTR pN = new ASSTR(TRUE);
    pN->fDefault = TRUE;
    pRet ? (pE->next = pN) : (pRet=pN);
  }
  return pRet;
}

void w_prf_asslist(LONG slot, PASSTR pAss)
{
  LONG      lAssCount;
  char      szKey[30];
  //--- Remove old asses:
  PfReadLong(slot, "nAsses", &lAssCount, 0);
  while (--lAssCount >= 0)
  {
    sprintf(szKey,"Ass_%u", lAssCount);
    PfDeleteItem(slot, szKey);
  }

  //--- Write new:
  lAssCount = 0;
  {
    PASSPRFSTR p;
    for( ; pAss; pAss=pAss->next)
    {
      LONG cb = sizeof(ASSPRFSTR) + (pAss->QueryFiles() ? strlen(pAss->QueryFiles()) : 0);
      if (!(p = (PASSPRFSTR)malloc(cb))) continue;
      p->cb              = cb;
      memcpy(&p->ass,    pAss, sizeof(p->ass));
      strcpy(p->szFiles, pAss->QueryFiles() ? pAss->QueryFiles() : "");
      sprintf(szKey,"Ass_%u", lAssCount);
      PfWriteItem(slot, szKey, p, cb);
      sprintf(szKey,"cbAss_%u", lAssCount);
      PfWriteLong(slot, szKey, cb);
      free(p);
      lAssCount++;
    }
    PfWriteLong(slot, "nAsses", lAssCount);
  }
}

void PAINTEROFFICE::RProfile(LONG slot, BOOL fLocalOnly)
{
  PASSTR p,p1,*pp;
  for (p=pLocalAssList;  p; p=p1) {p1=p->next; delete p;}
  pLocalAssList  = r_prf_asslist(slot, FALSE);
  if (!fLocalOnly)
  {
    for (p=pGlobalAssList; p; p=p1) {p1=p->next; delete p;}
    pGlobalAssList = r_prf_asslist(0,    TRUE);
  }
  for (pp=&pGlobalAssList; *pp; pp=&((*pp)->next)) if((*pp)->fDefault) return;
  *pp = new ASSTR(TRUE); /* 'Default' должен быть всегда */
}
void PAINTEROFFICE::WProfile(LONG slot)
{
  w_prf_asslist(slot,pLocalAssList);
  w_prf_asslist(0,   pGlobalAssList);
}


PASSTR *PAINTEROFFICE::find_ass(PSZ szFilename)
{
  static ASSTR     emergencyAss(TRUE);              // Вернется при ошибке
  static PASSTR    pEmergencyAss = & emergencyAss;  // поиска :-(

  int              flen;
  char             szFN[CCHMAXPATH] = "";
  if (szFilename) strcpy(szFN,szFilename);
  for(flen=0; szFN[flen]; flen++) szFN[flen] = sf_toupper(szFN[flen]);
  PASSTR          *pp;
  pp              = &pLocalAssList;
  BOOL fLocalWalk = TRUE;
  while(1)
  {
    for (; *pp; pp = &((*pp)->next))
    {
      if ((*pp)->fDefault) break;
      if ((*pp)->QueryFiles())
      {
        PSZ      psz      = OpenExprVar((*pp)->QueryFiles());
        if (!psz) continue;
        BOOL     fMatch   = RCThunk_FastMatch(sf_strtoupper(psz),szFN);
        free(psz);
        if (fMatch) break;
      }
    }
    if (*pp) return pp;      // <--- Ok.
    if (fLocalWalk)
      pp = &pGlobalAssList;
    else
      return &pEmergencyAss; // <--- Bug.
  }
}


void ASSTR::Assign(ASSTR *pAss)
{
  *this    = *pAss;
  pszFiles = 0;
  next     = 0;
  SetFiles(pAss->pszFiles);
}
void ASSTR::SetFiles(PSZ sz)
{
  PSZ        psz;
  if (psz  = (PSZ)malloc(strlen(sz)+1))
  {
    free(pszFiles);
    strcpy(psz,sz);
    pszFiles = psz;
  }
}
ASSTR::ASSTR(BOOL fDef)
{
  memset(this,0,sizeof(*this));
  if (fDef)
  {
    int i;
    for (i=0; i<CLP_PALETTELEN; i++)
    {
      aaRGB[i][CLP_BACKGROUND]           = 0xf8f8f8;
      aaRGB[i][CLP_FOREGROUND]           = 0x000000;
    }
    aaRGB[CLP_CURLINE]  [CLP_BACKGROUND] = 0xf0f0f0;
    aaRGB[CLP_SELECTION][CLP_BACKGROUND] = 0x404040;
    aaRGB[CLP_SELECTION][CLP_FOREGROUND] = 0xf0f0f0;

    strcpy(fAttrs.szFacename,"Courier");
    fAttrs.usRecordLength    = sizeof(fAttrs);
    fAttrs.lMatch            = 10;
    fAttrs.lMaxBaselineExt   = 16;
    fAttrs.lAveCharWidth     = 9;
    fAttrs.fsFontUse         = 2;

    fHiliteCurLine           = TRUE;
    ulTabWidth               = 8;
    fSmartTabs               = TRUE;
    fAutoident               = TRUE;
    fDefault                 = TRUE;
    SetFiles("*");
  }
}
ASSTR::~ASSTR()
{
  free(pszFiles);
  pszFiles = 0;
}


