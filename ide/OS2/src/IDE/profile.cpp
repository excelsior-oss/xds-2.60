/*
*
*  File: PROFILE.CPP
*
*  XDS Shell: access to profile(s)
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
#define  INCL_REXXSAA
#include "XDSBASE.H"

HINI              hfPrf = 0;
char              szWorkplace[WPNAMELEN]  = "";
BOOL              fUseMirror              = FALSE;
char              szWPMirror [CCHMAXPATH] = "";
char              szIniPath  [CCHMAXPATH] = "";
PSZ               pszWPComment            = 0;
WPTPLADVANCEDINIT wpTplAdvancedInit; // Object used to advance new WP initialization (using WP template)

BOOL      pf_storeWP(LONG slot, BOOL fContents, PSZ szWP, ULONG ulPrfIO);
BOOL      pf_readWP (LONG slot, ULONG ulPrfIO);



/* Create the profile
>
> pszPFPath     - path to create the profile in (if 0 - query the directory)
> fSetAsDefault - set this profile as the default
> hMsgWnd       - window to use as a parent for a messages or 0 to query nothing
> szIniDir      - 0 or the directory to use as initial for the profile creating dialog
> NOTE: when pszPFPath specifyed this function will newer display a dialogs and
>       does not needs any windows more then message boxes. This allows to use
>       it with the '/I' key to install us from the command prompt.
*/
BOOL PfCreateProfile(PSZ pszPFPath, BOOL fSetAsDefault, HWND hMsgWnd, PSZ szIniDir)
{
  HINI      hiniOld = hfPrf;
  char      szPath[CCHMAXPATH];
  char      szPF  [CCHMAXPATH];
  CPARAMSTR cps;
  char      sz_path[CCHMAXPATH] = "";

  if (!pszPFPath)
  {
    if (!hMsgWnd) return FALSE;
    cps.psz1 = sz_path;
    if (szIniDir) strcpy(cps.psz1,szIniDir);
    cps.l3   = 0; /*++ help id */
    if (!szIniDir)
      if (!WinDlgBox(HWND_DESKTOP,hMsgWnd,(PFNWP)dpSelDirDlg,NULLHANDLE,IDD_SELDIRDLG,&cps))
        return FALSE;
    pszPFPath = cps.psz1;
  }

  if (!FName2Full(szPath,pszPFPath,FALSE))
  {
    char szQQ[CCHMAXPATH+100];
    sprintf(szQQ,"Can't create profile using path: '%'.",pszPFPath);
    if (hMsgWnd)
      WinMessageBox(HWND_DESKTOP,hMsgWnd,szQQ,"Create profile",0,MB_ERROR|MB_OK|MB_MOVEABLE);
    return FALSE;
  }
  sprintf(szPF,"%s\\%s",szPath,INIFILENAME);

  CreateNewDir(0, szPath);
  hfPrf = PrfOpenProfile(hAB,szPF);
  if (!hfPrf)
  {
    char szQQ[CCHMAXPATH+100];
    sprintf(szQQ,"Can't create profile: %",szPF);
    if (hMsgWnd)
      WinMessageBox(HWND_DESKTOP,hMsgWnd,szQQ,"Create profile",0,MB_ERROR|MB_OK|MB_MOVEABLE);
    hfPrf = hiniOld;
    return FALSE;
  }
  { // Exists?
    LONG prfId = PROFIDENT+1;
    PfReadLong(0,"lProfIdent",&prfId,PROFIDENT+1);
    if (prfId==PROFIDENT)
    {
      char szQQ[CCHMAXPATH+200];
      sprintf(szQQ,"The Environment found an existent profile '%s'. Do you want to overwrite it?",szPF);
      if (hMsgWnd && MBID_YES!=WinMessageBox(HWND_DESKTOP,hMsgWnd,szQQ,"Create profile",
                     0,MB_ERROR|MB_YESNO|MB_MOVEABLE))
      {
        PrfCloseProfile(hfPrf);
        hfPrf=0;  // Nothing to write
      }
    }
  }
  if (hfPrf)
  {
    PrfWriteProfileData(hfPrf,"XDS",0,0,0); // delete all
    PfWriteLong(0,"lProfIdent",PROFIDENT);
    BOOL f    = fConfigON;
    fConfigON = TRUE;
    PfStoreWPlace("MyWorkplace");
    fConfigON = f;
    {
      char         szScriptFile[CCHMAXPATH];
      RXSTRING     rsArg;
      SHORT        shRes;
      char         szResult[256];
      RXSTRING     rsResult;
      sprintf(szScriptFile,"%s\\" INISCRIPTFILENAME, szHomePath);
      MAKERXSTRING(rsArg,szPF,strlen(szPF));
      MAKERXSTRING(rsResult,szResult,sizeof(szResult)-1);
      if (RexxStart
               (1,          // argc
                &rsArg,     // args
                szScriptFile,
                0,
                0,          // EnvName
                RXCOMMAND,
                NULL,
                &shRes,
                &rsResult))
      {
        MessageBox(hMsgWnd, "Initialisation is not complete", MB_ERROR|MB_OK|MB_MOVEABLE,
          "Can not execute:\n", szScriptFile, "\nNow you can run IDE but it is not prperly configured.");
      }
    }
    PrfCloseProfile(hfPrf);
  }
  if (fSetAsDefault)
    PrfWriteProfileData(HINI_USERPROFILE,"XDS","XD$_INI_PATH",szPath,strlen(szPath)+1);
  hfPrf = hiniOld;
  return TRUE;
}

/* Open the default profile
>
>  PSZ  pszPath (in)  - Profile path (to use as default) or 0 to find the default
>                       using the installation information was wrote before
>  HWND hMsgWnd (in)  - Window to use as a parent for a messages or 0 to query nothing
>
>  Returns TRUE when success;
*/
BOOL PfOpenDefProfile(PSZ pszPath,HWND hMsgWnd)
{
  FILESTATUS3 fs3;
  LONG        prfId                  = PROFIDENT+1;
  char        szIniPath  [CCHMAXPATH] = "";
  char        szini_file [CCHMAXPATH] = "";
try_again:
  if (!pszPath)
  { // Trying to create the default profile:
    ULONG ul = sizeof(szIniPath);
    if (!PrfQueryProfileData(HINI_USERPROFILE,"XDS","XD$_INI_PATH",szIniPath,&ul) || !ul)
    {
      if (   hMsgWnd
          && MBID_YES==WinMessageBox(HWND_DESKTOP,hMsgWnd,"The envinronment profile path is not set. "
             "Do you want to set in now?","Install",0,MB_ERROR|MB_YESNO|MB_MOVEABLE)
          && PfCreateProfile(0, TRUE, hMsgWnd)
          )
        goto try_again;
     return FALSE;
    }
  }
  else strcpy(szIniPath,pszPath);
  FName2Full(szIniPath,szIniPath,0);
  sprintf(szini_file,"%s\\" INIFILENAME, szIniPath);
  if (hfPrf) PrfCloseProfile(hfPrf);
  hfPrf = 0;
  if (!DosQueryPathInfo(szini_file,FIL_STANDARD,&fs3,sizeof(fs3))) // exists? (to prevent creating new)
    hfPrf = PrfOpenProfile(hAB,szini_file);
  if (hfPrf) PfReadLong(0,"lProfIdent",&prfId,0);
  if (prfId!=PROFIDENT)
  {
    if (hfPrf) PrfCloseProfile(hfPrf);
    hfPrf = 0;
  }
  if (!hfPrf)
  {
    char szQQ[CCHMAXPATH+200];
    sprintf(szQQ,"The envinronment profile path '%s' can't be retrived. Create new profile?",szini_file);
    if (   hMsgWnd
        && MBID_YES==WinMessageBox(HWND_DESKTOP,hMsgWnd,szQQ,"Init",0,MB_ERROR|MB_YESNO|MB_MOVEABLE)
        && PfCreateProfile(0, FALSE, hMsgWnd, szIniPath)
        )
      goto try_again;
  }
  if (hfPrf) strcpy(::szIniPath,szIniPath);
  return !!hfPrf;
}

BOOL PfOpenWPlace(PSZ pszWPName)
//
// pszWPName - name to open (may be new or 0 to open the last active)
// The current configuration must be stored before.
// Returns: success. (Does anybody needs it?..)
//
{
  char szWP[WPNAMELEN];
  ULONG l;
  if (pszWPName)
    strcpy(szWP,pszWPName);
  else if (!PrfQueryProfileData(hfPrf,"XDS","szWPDefault",szWP,&(l = sizeof(szWP))))
    return FALSE;
  {// Turn the winbar off
    WBCONFIGDATA wbcd;
    wbcd.ulWBV = WBV_HIDDEN;
    strcpy(wbcd.szPPFont,"8.Helv");
    pMDI->WBarSetConfig(&wbcd);
  }
  LONG slot   = PfSearchSlot(szWP,FALSE);
  BOOL fNewWP = slot<0;
  slot        = abs(slot);
  if (fNewWP)
    pf_storeWP(slot,FALSE,szWP,PRF_IO_NORMAL); // Создадим пустой и сразу же будем читаться из него
  strcpy(szWorkplace,szWP);
  PrfWriteProfileData(hfPrf,"XDS","szWPDefault",szWorkplace,strlen(szWorkplace)+1);
  PfReadItem (slot, "szWPMirror", szWPMirror, sizeof(szWPMirror), "", 0);
  PfReadLong (slot, "fUseMirror", (PLONG)&fUseMirror, FALSE);
  HINI hiniMirr = 0;
  BOOL rc;

  // Тек. директорию надо поставить до попытки использования mirror-а из нее
  PfReadItem (slot, "szCurPath",     szCurPath,     sizeof(szCurPath),     "",  0);
  DosSetCurrentDir (szCurPath);
  DosSetDefaultDisk(sf_toupper(szCurPath[0])-'A'+1);
  QueryCurrentPath(szCurPath);
  char szMrr[CCHMAXPATH];
  if (fUseMirror && FName2Full(szMrr,szWPMirror,TRUE) && (hiniMirr = PrfOpenProfile(hAB,szMrr)))
  {
    HINI hini0  = hfPrf;
    rc          = pf_readWP (slot, PRF_IO_MIRRORBASE);
    hfPrf       = hiniMirr;
    rc         |= pf_readWP (1, PRF_IO_MIRROR);
    PrfCloseProfile(hfPrf);
    hfPrf       = hini0;
  }
  else
  {
    rc         = pf_readWP (slot, PRF_IO_NORMAL);
    fUseMirror = FALSE;
  }
  return rc;
}

BOOL pf_readWP (LONG slot, ULONG ulPrfIO)
{
  if (ulPrfIO & (PRF_IO_NORMAL|PRF_IO_MIRROR))
  {// NORMAL or MIRRORBASE or MIRROR
    if (!(ulPrfIO & PRF_IO_MIRROR))
    { //>>> NORMAL or MIRRORBASE (station-depended)
      extern LINELIST *pllFilesFound;
      sf_freelist(pllFilesFound);
      pllFilesFound = 0;
      pAutoVarTable->AssignVar("Proj",        "");
      pAutoVarTable->AssignVar("ProjName",    "");
      pAutoVarTable->AssignVar("ProjPath",    "");
      pAutoVarTable->AssignVar("File",        "");
      pAutoVarTable->AssignVar("FileName",    "");
      pAutoVarTable->AssignVar("FileExt",     "");
      pAutoVarTable->AssignVar("FilePath",    "");
      pAutoVarTable->AssignVar("ActiveFrame", "");

      PfReadItem (slot, "szCurPath",     szCurPath,     sizeof(szCurPath),     "",  0);
      PfReadItem (slot, "szCurIOPath",   szCurIOPath,   sizeof(szCurIOPath),   "C:",0);
      DosSetCurrentDir (szCurPath);
      DosSetDefaultDisk(sf_toupper(szCurPath[0])-'A'+1);
      QueryCurrentPath(szCurPath);
      wpTplAdvancedInit.RProfile(slot,"WPTplAdvScript");
    }
    if ((ulPrfIO & PRF_IO_MIRRORBASE) != PRF_IO_MIRRORBASE)
    { //>>> NORMAL or MIRROR
      char sz_prj[CCHMAXPATH]="";
      szPrjFile[0] = 0;
      PfReadRelFName(slot, "szPrjFile", sz_prj, "",  0);
      OpenProject(sz_prj,0);

      HWND hMainFrame = pMDI->QueryWindow(MDIQW_MAINFRAME);
      LONG al[5]; //x, y, cx, cy, fMaximize
      if (sizeof(al)==PfReadItem(slot,"alFramePos",al,sizeof(al),0,0))
      {
        WinSetWindowPos(hMainFrame, 0, 0,     0,     0,     0,     SWP_RESTORE);
        WinSetWindowPos(hMainFrame, 0, al[0], al[1], al[2], al[3], SWP_MOVE|SWP_SIZE);
        if (al[4]) WinSetWindowPos(hMainFrame, 0, 0,0,0,0, SWP_MAXIMIZE);
      }
      Macros. RProfile(slot,"LMacro",FALSE);
      grOffice. RProfile(slot);
      srcList.  RProfile(slot,"SrcList");
      HelpRProfile(slot,"HelpMenuInfs");
    }
    if (!(ulPrfIO & PRF_IO_MIRROR))
    { //>>> NORMAL or MIRRORBASE (station-depended)
      hstFiles.   RProfile(slot,"FileHist");
      hstProjects.RProfile(slot,"ProjHist");
      Macros.   RProfile(0,   "GMacro", TRUE);
      RexxMon_RProfile  ("RexxMon");
    }
  }

  if (ulPrfIO & PRF_IO_NORMAL)
  {
    hstWPlaces.RProfile(0,   "WPHist");
    PrfWriteProfileData(hfPrf,"XDS","szWPDefault",szWorkplace,strlen(szWorkplace)+1);
  }
  if ((ulPrfIO & PRF_IO_MIRRORBASE)!=PRF_IO_MIRRORBASE)
  {
    pUserVarTable-> RProfile(slot,"UserVarTable", !!(ulPrfIO&PRF_IO_TEMPLATE));
    if (ulPrfIO & (PRF_IO_TPL_TOOLS | PRF_IO_NORMAL | PRF_IO_MIRROR))
    {
      if (ulPrfIO & (PRF_IO_NORMAL | PRF_IO_MIRROR| PRF_IO_TPL_RMTLS))
        pToolbox->free_all();
      if(!pToolbox-> RProfile(slot,"ToolBox"))
        MessageBox(hMDI0Client,"Read tools",MB_ERROR|MB_OK,"Can not set so many tools.");
    }
    FontClrRProfile(slot,"ClrSet");
    PfApplyPPFont  (slot,"PPFontMsg",clMsgList.hWnd);
    SetAllCaptions();
    LONG nWPComment;
    free(pszWPComment); pszWPComment = 0;
    PfReadLong(slot,"nWPComment",&nWPComment,0);
    if (nWPComment>1 && (pszWPComment=(PSZ)malloc(nWPComment)))
      PfReadItem(slot,"szWPComment",pszWPComment,nWPComment,"<No comment>",0);
  }
  if (ulPrfIO & (PRF_IO_TPL_ASS | PRF_IO_NORMAL | PRF_IO_MIRROR))
    ptOffice.RProfile(slot, !(ulPrfIO & PRF_IO_NORMAL));
  grOffice.ReorderWBar();
  return TRUE;
}

BOOL PfStoreWPlace(PSZ szWPName, LONG slot)
// Returns: success
{
  if (szWPName && szWPName!=szWorkplace) strcpy(szWorkplace,szWPName);
  if (!szWorkplace[0]) return TRUE; // Succesfully losed...
  hstWPlaces.Append(szWorkplace);
  if (slot<0) slot = PfSearchSlot(szWorkplace,FALSE);
  slot          = abs(slot);
  HINI hiniMirr = 0;
  BOOL rc       = pf_storeWP(slot,TRUE,szWorkplace,PRF_IO_NORMAL);
  if (fUseMirror &&  (hiniMirr = PrfOpenProfile(hAB,szWPMirror)))
  {
    HINI hini0  = hfPrf;
    hfPrf       = hiniMirr;
    rc         |= pf_storeWP(1,TRUE,szWorkplace,PRF_IO_MIRROR);
    PrfCloseProfile(hfPrf);
    hfPrf       = hini0;
  }
  SetAllCaptions();
  return rc;
}


BOOL pf_storeWP(LONG slot, BOOL fContents, PSZ szWP, ULONG ulPrfIO)
//
// Writes the slot info.
//   slot      - slot # to write
//   fContents - if FALSE - only the free slot contents
//   szWP      - WP name for this slot
//
{
  if (ulPrfIO & (PRF_IO_NORMAL | PRF_IO_MIRROR))
  { //--- Store Main Frame position
    HWND hMainFrame = pMDI->QueryWindow(MDIQW_MAINFRAME);
    LONG al[5]; //x, y, cx, cy, fMaximize
    SWP  swp;
    WinQueryWindowPos(hMainFrame, &swp);
    if (swp.fl & SWP_MINIMIZE)
    {
      WinSetWindowPos  (hMainFrame,0,0,0,0,0,SWP_RESTORE);
      WinQueryWindowPos(hMainFrame, &swp);
    }
    if (al[4] = (swp.fl & SWP_MAXIMIZE))
    {
      al[0] = WinQueryWindowUShort(hMainFrame,QWS_XRESTORE);
      al[1] = WinQueryWindowUShort(hMainFrame,QWS_YRESTORE);
      al[2] = WinQueryWindowUShort(hMainFrame,QWS_CXRESTORE);
      al[3] = WinQueryWindowUShort(hMainFrame,QWS_CYRESTORE);
    }
    else
    {
      al[0] = swp.x;   al[1] = swp.y;
      al[2] = swp.cx;  al[3] = swp.cy;
    }
    PfWriteItem (slot, "alFramePos", al, sizeof(al));
    //---
    PfWriteItem  (slot, "szCurPath",     szCurPath,     strlen(szCurPath)+1);
    PfWriteItem  (slot, "szCurIOPath",   szCurIOPath,   strlen(szCurIOPath)+1);
    PfWriteItem  (slot, "szPrjFile",     szPrjFile,     strlen(szPrjFile)+1);
    PfWritePPFont(slot, "PPFontMsg",     clMsgList.hWnd);
    if (fContents)
    {
      hstWPlaces.     WProfile(0,   "WPHist");
      hstFiles.       WProfile(slot,"FileHist");
      hstProjects.    WProfile(slot,"ProjHist");
      srcList.        WProfile(slot,"SrcList");
    }
    else
    {
      PfDeleteItem(slot,"FileHist");
      PfDeleteItem(slot,"ProjHist");
      PfDeleteItem(slot,"SrcList");
    }
    HelpWProfile(slot,"HelpMenuInfs", ulPrfIO & PRF_IO_MIRROR);
  }

  if (fContents)
  {
    pUserVarTable-> WProfile(slot,"UserVarTable");
    pToolbox     -> WProfile(slot,"ToolBox"/*,!!(ulPrfIO&PRF_IO_TEMPLATE) - was fMkNewDefs*/);
    ptOffice.WProfile(slot);
    Macros.WProfile(slot,"LMacro",FALSE);
  }
  else
  {
    VARTABLE      vtFree;
    TOOLBOX       tBox;
    PAINTEROFFICE ptFree;
    vtFree.WProfile(slot,"UserVarTable");
    tBox.WProfile  (slot,"ToolBox");
    ptFree.WProfile(slot);
  }

  FontClrWProfile  (slot, "ClrSet");
  PfWriteItem      (slot, "szWorkplace",   szWP,          strlen(szWP)+1);
  grOffice.WProfile(slot, fContents && !(ulPrfIO & PRF_IO_TEMPLATE));

  if (pszWPComment && pszWPComment[0])
  {
    LONG nWPComment = strlen(pszWPComment) + 1;
    PfWriteItem(slot,"szWPComment",pszWPComment,nWPComment);
    PfWriteLong(slot,"nWPComment", nWPComment);
  }
  else
    PfWriteLong(slot,"nWPComment", 0);
  PfWriteLong (slot,"fFreeSlot",FALSE);
  if (ulPrfIO & PRF_IO_NORMAL) // NORMAL or MIRRORBASE
  {
    PrfWriteProfileData(hfPrf,"XDS","szWPDefault",szWP,strlen(szWP)+1);
    PSZ psz = fContents ? szWPMirror : "";
    PfWriteItem(slot, "szWPMirror", psz, strlen(psz)+1);
    PfWriteLong(slot, "fUseMirror", fUseMirror && fContents);
    Macros.WProfile(0,"GMacro",TRUE);
    RexxMon_WProfile("RexxMon");
  }
  if (ulPrfIO & (PRF_IO_NORMAL | PRF_IO_TEMPLATE)) // NORMAL or MIRRORBASE or TEMPLATE
    wpTplAdvancedInit.WProfile(slot,"WPTplAdvScript");
  return TRUE;
}



/* Search the slot
>
>  PSZ  pszName (in) - what to search: 0,"" - free slot, else the name
>  BOOL fPrj (in)    - TRUE  - means that the name is a project
>                      FALSE - means that the name is a workplace
>  Returns           - positive valie - the slot found
>                      negative value - -free slot number (pszName==0  OR  not found)
*/
LONG PfSearchSlot(PSZ pszName, BOOL fPrj)
{
  LONG            lSlots,slot,l;
  LONG            freeslot = 0;
  char            sz  [CCHMAXPATH];
  char            szIn[CCHMAXPATH];

  if (pszName && !pszName[0]) pszName = 0;
  if (pszName)
  {
    strcpy(szIn,pszName);
    if (fPrj) FName2Full(szIn,szIn,FALSE);
  }

  PfReadLong(0,"lSlots",&lSlots,1);
  for (slot = 1; slot<lSlots; slot++)
  {
    PfReadLong(slot,"fFreeSlot",&l,TRUE);
    if (l) freeslot = slot;
    else if (pszName)
    {
      if (fPrj) PfReadItem(slot,"szPrjFile",   sz,sizeof(sz),"",0);
      else      PfReadItem(slot,"szWorkplace", sz,sizeof(sz),"",0);
      if (!sf_stricmp(sz,szIn))
        return slot;
    }
  }
  if (!freeslot)
  {
    freeslot = lSlots;
    PfWriteLong(0,"lSlots",++lSlots);
  }
  return -freeslot;
}


/////////////////// S l o t  I / O /////////////////////////////


BOOL PfWriteItem (LONG slot, PSZ pszName, PVOID pBuf, LONG cbBuf)
{
  char sz[50];
  sprintf(sz,"%s_%u",pszName,slot);
  return PrfWriteProfileData(hfPrf,"XDS",sz,pBuf,cbBuf);
}


BOOL PfDeleteItem (LONG slot, PSZ pszName)
{
  char sz[50];
  sprintf(sz,"%s_%u",pszName,slot);
  return PrfWriteProfileData(hfPrf,"XDS",sz,NULL,0);
}

LONG PfReadItem (LONG slot, PSZ pszName, PVOID pBuf,  LONG cbBuf, PVOID pDefVal, int cbDef)
// returns the real length; pDefVal may be NULL
//                          else if  cbDef == 0 then pDefVal assumed a PSZ
{
  char   sz[50];
  ULONG  l = cbBuf;
  sprintf(sz,"%s_%u",pszName,slot);
  if (!PrfQueryProfileData(hfPrf,"XDS",sz,pBuf,&l))
  {
    l = 0;
    if (pDefVal)
      if (cbDef) memcpy(pBuf,pDefVal,min(cbBuf,cbDef));
      else       strcpy((char*)pBuf,(char*)pDefVal);
  }
  else if (l==cbBuf && !PrfQueryProfileSize(hfPrf,"XDS",sz,&l)) // Set the _real_ size
    l=cbBuf;
  return l;
}


BOOL PfWriteLong (LONG slot, PSZ pszName, LONG  l)
{
  return PfWriteItem (slot, pszName, &l, sizeof(l));
}

BOOL PfReadLong (LONG slot, PSZ pszName, LONG *pLong, LONG lDefVal)
{
  if (!PfReadItem(slot,pszName,pLong,sizeof(LONG),0,0))
  {
    *pLong = lDefVal;
    return FALSE;
  }
  return TRUE;
}

void PfWriteLineList (LONG slot, PSZ pszName, LINELIST *pllWrite)
{
  // Writes it as N,"text1",...,"textN"
  // N is a LONG, text(s) are z-terminated lines
  LINELIST *pll;
  char     *pchBuf;
  LONG      lSize = 0;
  LONG      lN    = 0;
  char      sz[50];
  sprintf   (sz,"%s_%u",pszName,slot);
  for (pll=pllWrite; pll; pll=pll->next,lN++)
    lSize += strlen(pll->text)+1;
  if (pchBuf=(char*)malloc(lSize+sizeof(lN)))
  {
    *PLONG(pchBuf) = lN;
    char *pchTarg  = pchBuf+sizeof(lN);
    for (pll=pllWrite; pll; pll=pll->next)
    {
      strcpy(pchTarg,pll->text);
      pchTarg += strlen(pchTarg)+1;
    }
    PrfWriteProfileData(hfPrf,"XDS",sz,pchBuf,pchTarg-pchBuf);
    free(pchBuf);
  }
  else PrfWriteProfileData(hfPrf,"XDS",sz,NULL,0);
}

LINELIST *PfReadLineList (LONG slot, PSZ pszName)
{
  char      sz[50];
  char     *pchBuf = 0;
  ULONG     ulSize = 0;
  LINELIST *pll    = 0;
  sprintf(sz,"%s_%u",pszName,slot);
  if (   !PrfQueryProfileSize(hfPrf,"XDS",sz,&ulSize)
      || !ulSize
      || !(pchBuf=(char*)malloc(ulSize+1))
      || !PrfQueryProfileData(hfPrf,"XDS",sz,pchBuf,&ulSize)
     )
  {
    free(pchBuf);
    return NULL;
  }
  pchBuf[ulSize] = '\0';
  LONG lN = *(PLONG)pchBuf;
  for (PSZ psz=pchBuf+sizeof(lN); psz<pchBuf+ulSize && lN>0; psz += strlen(psz)+1, lN--)
    pll = sf_applist(pll,psz);
  free(pchBuf);
  return pll;
}

void PfWritePPFont(LONG slot, PSZ pszName, HWND hWnd)
{
  ULONG ulAttrFound;
  char  szPPFont[FACESIZE+10] = "";

  WinQueryPresParam(hWnd,PP_FONTNAMESIZE,0,&ulAttrFound,
                    sizeof(szPPFont),szPPFont,QPF_NOINHERIT);
  if(PP_FONTNAMESIZE != ulAttrFound) szPPFont[0] = 0;
  PfWriteItem (slot, pszName, szPPFont, strlen(szPPFont)+1);
}
void PfApplyPPFont(LONG slot, PSZ pszName, HWND hWnd)
{
  char  szPPFont[FACESIZE+10] = "";
  if (PfReadItem(slot,pszName,szPPFont,sizeof(szPPFont),"",0) <= 2)
    strcpy(szPPFont,"10.System Proportional");
  WinSetPresParam(hWnd,PP_FONTNAMESIZE,strlen(szPPFont)+1,szPPFont);
}

BOOL PfWriteRelFName (LONG slot, PSZ pszName, PSZ szFName)
// Пишет строку szFName, если ее начало совпадает с текущей
// директорией (szCurPath), заменяет его на символ ":"
// Предполагается, что длина строки не больше CCHMAXPATH
{
  char sz[CCHMAXPATH];
  PSZ  psz = sz;
  LONG l   = strlen(szCurPath);
  strcpy(sz,szFName);
  if (!sf_strnicmp(sz,szCurPath,l))
  {
    sz[l] = ':';  // instead '\' after szCurPath's text
    psz   = sz+l;
  }
  return PfWriteItem(slot, pszName, psz,strlen(psz)+1);
}

LONG PfReadRelFName(LONG slot, PSZ pszName, char szTarg[CCHMAXPATH], PVOID pDefVal, int cbDef)
// Работает аналогично PfReadItem, но если считаная строка начинается с ":" - считает ее хвост относительным
// именем файла и преобразует в полное с учетом текущей директории (szCurPath)
// Предполагается, что буфер достаточно велик!
{
  char sz[CCHMAXPATH] = "";
  LONG rc             = PfReadItem (slot, pszName, sz, sizeof(sz), pDefVal, cbDef);
  if (rc && sz[0]==':')
  {
    if (!FName2Full(szTarg, sz+1, FALSE, szCurPath)) strcpy(szTarg,sz+1); //oops
  }
  else strcpy(szTarg,sz);
  return strlen(szTarg);
}

LONG PfQueryItemSize(LONG slot, PSZ pszName)
{
  ULONG     ulSize = 0;
  char      sz[50];
  sprintf(sz,"%s_%u",pszName,slot);
  if (!PrfQueryProfileSize(hfPrf,"XDS",sz,&ulSize)) return 0;
  return ulSize;
}

/*******************************************************************************/
/*                                                                             */
/*  U s e r  i n t e r f a c e                                                 */
/*                                                                             */
/*******************************************************************************/
MRESULT EXPENTRY dpNewWPDlg(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static HWND        hEFMirror;
  static HWND        hLBTpl;
  static HWND        hMLEComment;
  static HWND        hEFWPName;
  static HWND        hEFDir;
  static int         nCurIt;
  static LHANDLE     hCurH;
  static WPTPLOFFICE wptplOffice;
  static BOOL        fUsrLocked = FALSE;
  static LONG        rgbEF;
  static LONG        rgbGray;

  switch(ulMsg)
  {
  case WM_INITDLG:
    fUsrLocked  = TRUE;
    hEFMirror   = WinWindowFromID(hWnd,IDD_EFNWPMIRROR);
    hLBTpl      = WinWindowFromID(hWnd,IDD_LBNWPTPL);
    hMLEComment = WinWindowFromID(hWnd,IDD_MLENWPCOMMENT);
    hEFWPName   = WinWindowFromID(hWnd,IDD_EFNWPNAME);
    hEFDir      = WinWindowFromID(hWnd,IDD_EFNWPDIR);
    nCurIt      = LIT_NONE;
    hCurH       = 0;
    rgbEF       = WinQuerySysColor(HWND_DESKTOP,SYSCLR_ENTRYFIELD,0);
    rgbGray     = WinQuerySysColor(HWND_DESKTOP,SYSCLR_DIALOGBACKGROUND,0);
    WinSendMsg (hMLEComment, MLM_SETBACKCOLOR, MPARAM(rgbGray), 0);
    WinSendMsg (hEFMirror,EM_SETTEXTLIMIT,MPARAM(CCHMAXPATH),0);
    WinSendMsg (hEFWPName,EM_SETTEXTLIMIT,MPARAM(WPNAMELEN),0);
    WinSendMsg (hEFDir,   EM_SETTEXTLIMIT,MPARAM(CCHMAXPATH),0);
    WinSetWindowText(hEFDir, szCurPath);
    wptplOffice.ReBuild();
    wptplOffice.ShowList(hLBTpl);
    WinSendMsg(hLBTpl,LM_INSERTITEM,MPARAM(0), MPARAM("<None>"));
    WinSendMsg(hLBTpl,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
    WinCheckButton(hWnd,IDD_RBNWPTPL,TRUE);
    fUsrLocked  = FALSE;
    WinSendMsg(hWnd,WM_USER,0,0);
    break;
  case WM_USER:
    if (!fUsrLocked)
    {
      // Re-enable all, fill MLE
      nCurIt = (int)WinSendMsg(hLBTpl,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      hCurH  = nCurIt == LIT_NONE ? 0 : (LHANDLE)WinSendMsg(hLBTpl,LM_QUERYITEMHANDLE,MPARAM(nCurIt),0);
      if (hCurH) wptplOffice.ShowComment(hCurH,hMLEComment);
      else       WinSetWindowText(hMLEComment,"Create free workplace.");
      BOOL fTpl = WinQueryButtonCheckstate(hWnd,IDD_RBNWPTPL);
      WinSetPresParam (hLBTpl,                PP_BACKGROUNDCOLOR, sizeof(ULONG), fTpl ? &rgbEF   : &rgbGray);
      WinSetPresParam (hEFWPName,             PP_BACKGROUNDCOLOR, sizeof(ULONG), fTpl ? &rgbEF   : &rgbGray);
      WinEnableWindow (hEFWPName,             fTpl);
      WinEnableWindow (hLBTpl,                fTpl);
      WinEnableControl(hWnd, IDD_PBNWPCHDIR,  fTpl);

      WinSetPresParam (hEFMirror,             PP_BACKGROUNDCOLOR, sizeof(ULONG), fTpl ? &rgbGray : &rgbEF);
      WinEnableWindow (hEFMirror,             !fTpl);
      WinEnableControl(hWnd, IDD_PBNWPBROWSE, !fTpl);
    }
    break;
  case WM_CONTROL:
    if (m1==MPFROM2SHORT(IDD_LBNWPTPL,LN_ENTER) || m1==MPFROM2SHORT(IDD_LBNWPTPL,LN_SELECT))
    {
      int nIt = (int)WinSendMsg(hLBTpl,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      if (nIt==nCurIt) break;
      wptplOffice.RefreshComment(hCurH,hMLEComment);
      WinSendMsg(hWnd,WM_USER,0,0);
    }
    else if (SHORT1FROMMP(m1)==IDD_RBNWPMIRROR || SHORT1FROMMP(m1)==IDD_RBNWPTPL)
      WinSendMsg(hWnd,WM_USER,0,0);
    break;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
    case IDD_PBNWPBROWSE:
    {
      char sz[CCHMAXPATH] = "";
      FName2Full(sz,"*.mrr",FALSE,szCurPath);
      if (Browse(sz, sz, hWnd, "Workplace mirror", FALSE))
      {
        FName2Full(sz,sz,FALSE);
        WinSetWindowText(hEFMirror,sz);
      }
      break;
    }
    case IDD_PBNWPCHDIR:
    {
      CPARAMSTR cps;
      char      sz[CCHMAXPATH];
      WinQueryWindowText(hEFDir,sizeof(sz),sz);
      cps.l3   = 0; /*++ help id */
      cps.psz1 = sz;
      if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpSelDirDlg,NULLHANDLE,IDD_SELDIRDLG,&cps) && cps.psz1)
        WinSetWindowText(hEFDir,sz);
      break;
    }
    case DID_OK:
    {
      char szNWP[WPNAMELEN] = "";
      HINI hiniMirr         = 0;
      HINI hini0            = hfPrf;
      BOOL fTpl             = WinQueryButtonCheckstate(hWnd,IDD_RBNWPTPL);
      char szMirr[CCHMAXPATH];
      WinQueryWindowText(hEFMirror,sizeof(szMirr),szMirr);
      FName2Full(szMirr,szMirr,0);
      try
      {
        if (fTpl)
          WinQueryWindowText(hEFWPName,sizeof(szNWP),szNWP);
        else
        { // Откроем зеркало и считаем из него имя воркплэйса
          if (!(hiniMirr = PrfOpenProfile(hAB,szMirr)))
            throw "Can not open workplace mirror file.";
          hfPrf = hiniMirr;
          if (!PfReadItem(1,"szWorkplace", szNWP, sizeof(szNWP),"",0) || !szNWP[0])
            throw "The workplace mirror file specifyed is corrupt (or it is not a mirror file).";
          hfPrf = hini0;
        }
        //  Проверим имя воркплэйса
        PSZ pszNWP = sf_skipspaces(szNWP); // Далее - только с ним
        if (!*pszNWP) throw "You must specify the new workplace name.";
        LONG lSlot    = PfSearchSlot(pszNWP,FALSE);
        BOOL fGetBase = FALSE; // Надо ли будет базу mirror-а подчитать со слота lSlot?
        if (lSlot>=0)
        {
          if (fTpl) throw "This workplace name is belonged to another workplace.";
          else
          {
            if (MBID_YES == MessageBox(hWnd,"New workplace",MB_ERROR|MB_YESNO,"The workplace name in the mirror file is '",pszNWP,
            "'. This name is belonged to another workplace. Do you want to apply the mirror over this workplace? Some of"
            " workplace settings woll be losed!"))
              fGetBase = TRUE;
            else
              throw "Can not create new workplace with this name.";
          }
        }
        // Поехали все крушить:
        //--- Free all
        if (!SaveConfig() || !grOffice.CloseWindow(0,hWnd)) break;
        fUseMirror       = FALSE;
        szWPMirror   [0] = 0;
        szPrjFile    [0] = 0;
        srcList.ClearManual();
        srcList.ClearComp();
        hstFiles.Clear();
        hstProjects.Clear();
        pUserVarTable->ForgetAll();
        delete pToolbox;
        pToolbox         = new TOOLBOX;
        pToolbox->TuneMenu();
        ptOffice.SetNewAssList(FALSE, 0, FALSE);
        free(pszWPComment);
        pszWPComment     = 0;
        pAutoVarTable->AssignVar("Proj",        "");
        pAutoVarTable->AssignVar("ProjName",    "");
        pAutoVarTable->AssignVar("ProjPath",    "");
        pAutoVarTable->AssignVar("File",        "");
        pAutoVarTable->AssignVar("FileName",    "");
        pAutoVarTable->AssignVar("FileExt",     "");
        pAutoVarTable->AssignVar("FilePath",    "");
        pAutoVarTable->AssignVar("ActiveFrame", "");
        RxExecute("HelpClearMenu();");
        WinEnableMenuItem(WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU),
                          IDM_OPTRECALLMACROED, FALSE);
        //--- Apply anyth.?
        if (fTpl)
        {
          char sz[CCHMAXPATH];
          WinQueryWindowText(hEFDir,sizeof(sz),sz);
          DosSetCurrentDir (sz);
          DosSetDefaultDisk(sf_toupper(sz[0])-'A'+1);
          QueryCurrentPath(szCurPath);
          strcpy(szCurIOPath, szCurPath);
          if(hCurH) wptplOffice.ApplyTpl(hCurH,hWnd,TRUE);
        }
        else
        {
          char sz[CCHMAXPATH];
          if (fGetBase) pf_readWP(lSlot, PRF_IO_MIRRORBASE);
          // Тек. директория := дир. mirror-а
          {
            strcpy(sz,szMirr);
            PSZ psz;
            if (psz = strrchr(sz,'\\')) *psz = 0;
            DosSetCurrentDir (sz);
            DosSetDefaultDisk(sf_toupper(sz[0])-'A'+1);
            QueryCurrentPath(szCurPath);
          }
          hfPrf = hiniMirr;
          pf_readWP(1, PRF_IO_MIRROR);
          hfPrf = hini0;
          strcpy(szWPMirror,sf_fname2short(szMirr));
          fUseMirror = FName2Full(sz,szWPMirror,TRUE);
        }
        // Set new workplace name
        strcpy(szWorkplace,pszNWP);
      }
      catch (char *psz)
      {
        hfPrf = hini0;
        MessageBox(hWnd,"New workplace",MB_ERROR|MB_OK,psz);
        PrfCloseProfile(hiniMirr);
        break;
      }
      SetAllCaptions();
      PfStoreWPlace(0);
      WinDismissDlg(hWnd,1);
      break;
    }
    case DID_CANCEL:
      WinDismissDlg(hWnd,0);
      break;
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_NEWWPDLG);
      break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_NEWWPDLG);
      break;
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}




MRESULT EXPENTRY dpWPListDlg(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
// Returns: success
  static HWND       hLB;
  static HWND       hMLE;
  static int        nCurItem;
  static BOOL       fCurrentWP = FALSE;
  char              sz [CCHMAXPATH+WPNAMELEN+10];
  ULONG             slot;
  int               i,j;

  switch(ulMsg)
  {
  case WM_INITDLG:
  {
    LONG            lSlots;
    char            sz1[CCHMAXPATH];

    CenterChildWindow(hWnd);
    hLB      = WinWindowFromID(hWnd,IDD_LBWPLIST);
    hMLE     = WinWindowFromID(hWnd,IDD_MLEWPCOMMENT);

    PfReadLong(0,"lSlots",&lSlots,2);
    for (slot = lSlots-1; slot>=1; slot--)
    {
      LONG l;
      PfReadLong(slot,"fFreeSlot",&l,1l);
      if (l) continue;
      PfReadItem(slot,"szWorkplace",sz,WPNAMELEN,"???",0);
      if (!(i = strlen(sz))) continue;
      PfReadItem(slot,"szPrjFile",sz1,sizeof(sz1),"",0);
      if (sz1[0])
      {
        strcat(sz," :   ");
        strncat(sz,sz1,CCHMAXPATH);
      }
      j = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),MPARAM(sz));
      WinSendMsg(hLB,LM_SETITEMHANDLE,MPARAM(j),MPFROM2SHORT(SHORT(slot),SHORT(i)));
    }
    nCurItem   = LIT_NONE;
    fCurrentWP = FALSE;
    WinSendMsg(hLB,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
    break;
  }
  case WM_USER: // QQ: save the comment?
  {
    if (!WinSendMsg(hMLE,MLM_QUERYCHANGED,0,0) || nCurItem==LIT_NONE) break;
    {
      char szz[100];
      WinSendMsg(hLB,LM_QUERYITEMTEXT,MPFROM2SHORT(nCurItem,sizeof(szz)),MPARAM(szz));
      szz [ SHORT2FROMMR(WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nCurItem),0)) ] = '\0';
      sprintf(sz,"The comment for the '%s' workplace is changed. Do you want to save it?",szz);
    }
    if (MBID_YES != WinMessageBox(HWND_DESKTOP, hWnd, sz, "Workplace list",0, MB_QUERY|MB_YESNO|MB_MOVEABLE))
      break;
    // save the comment
    slot              = SHORT1FROMMR(WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nCurItem),0));
    LONG   n_comment  = WinQueryWindowTextLength(hMLE) + 1;
    char  *sz_comment = (char*)malloc(n_comment);
    if (sz_comment)
    {
      WinQueryWindowText(hMLE,n_comment,sz_comment);
      PfWriteItem(slot,"szWPComment",sz_comment,n_comment);
      PfWriteLong(slot,"nWPComment", n_comment);
      if (slot==PfSearchSlot(szWorkplace,FALSE))
      {
        free(pszWPComment);
        pszWPComment = sz_comment;
      }
      else
        free(sz_comment);
    }
    break;
  }
  case WM_CONTROL:
    if (m1==MPFROM2SHORT(IDD_LBWPLIST,LN_SELECT))
    {
      WinSendMsg(hWnd,WM_USER,0,0);
      nCurItem = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      WinSetWindowText(hMLE,"<No comment>");
      if (nCurItem != LIT_NONE)
      {
        WinSendMsg(hLB,LM_QUERYITEMTEXT,MPFROM2SHORT(nCurItem,100),MPARAM(sz));
        sz[ SHORT2FROMMR(WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nCurItem),0)) ] = '\0';

        fCurrentWP = !sf_stricmp(sz,szWorkplace);
        WinEnableControl(hWnd, IDD_PBWPREMOVE, !fCurrentWP);
        WinEnableControl(hWnd, DID_OK,         !fCurrentWP);

        slot = SHORT1FROMMR(WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nCurItem),0));
        LONG n_comment;
        PfReadLong(slot,"nWPComment",&n_comment,0);
        if (n_comment>1)
        {
          char *sz_comment = (char*)malloc(n_comment);
          if (sz_comment)
          {
            PfReadItem(slot,"szWPComment",sz_comment,n_comment,"<No comment>",0);
            WinSetWindowText(hMLE,sz_comment);
            free(sz_comment);
          }
        }
      }
      WinSendMsg(hMLE,MLM_SETCHANGED,MPARAM(FALSE),0);
    }
    else if (m1==MPFROM2SHORT(IDD_LBWPLIST,LN_ENTER))
      return WinSendMsg(hWnd,WM_COMMAND,MPARAM(DID_OK),0);
    break;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
    case IDD_PBWPREMOVE:
      {
        int nItem = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        if (nItem==LIT_NONE) break;
        WinSendMsg(hLB,LM_QUERYITEMTEXT,MPFROM2SHORT(nItem,100),MPARAM(sz));
        sz [ SHORT2FROMMR(WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nItem),0)) ] = '\0';

        if(!sf_stricmp(sz,szWorkplace)) {Beep(); break;}

        {
          char szQQ[WPNAMELEN+100];
          sprintf(szQQ,"Do you want to remove all the information about the '%s' workplace?",sz);
          if ( MBID_YES==WinMessageBox(HWND_DESKTOP,hWnd,szQQ,"Remove workplace",0,MB_ICONHAND|MB_YESNO|MB_MOVEABLE)
              && (i = PfSearchSlot(sz,FALSE)) >0 )
          {
            PfWriteLong(i,"fFreeSlot",TRUE);
            hstWPlaces.Delete  (sz);
            hstWPlaces.WProfile(0,"WPHist");
            i = (int)WinSendMsg(hLB,LM_DELETEITEM, MPARAM(nItem),0);
            WinSendMsg(hLB,LM_SELECTITEM,MPARAM( min(i-1, nItem)),MPARAM(TRUE));
          }
        }
      }
      break;
    case DID_CANCEL:
      WinSendMsg(hWnd,WM_USER,0,0);
      WinDismissDlg(hWnd,0);
      break;
    case DID_OK:
    {
      if (fCurrentWP) return 0;
      i = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      if (i==LIT_NONE) break;
      WinSendMsg(hLB,LM_QUERYITEMTEXT,MPFROM2SHORT(i,100),MPARAM(sz));
      sz [ SHORT2FROMMR(WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(i),0)) ] = '\0';
      char szz[WPNAMELEN+50];
      sprintf(szz,"WPOpen('%s');",sz);
      WinDismissDlg(hWnd,RxExecute(szz));
      break;
    }
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_WPLISTDLG);
      break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1){
      DisplayHelpPanel(PANEL_WPLISTDLG);
      break;
    }
  default:
   return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


MRESULT EXPENTRY dpWPPropDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static HWND hEF;
  static HWND hMLE;
  static HWND hEFMirr;
  static LONG slot;
  static char szDir[CCHMAXPATH];

  switch(ulMsg)
  {
  case WM_INITDLG:
    CenterChildWindow(hWnd);
    strcpy(szDir,szCurPath);
    slot    = PfSearchSlot(szWorkplace, FALSE);
    hEF     = WinWindowFromID(hWnd,IDD_EFWPPNAME);
    hMLE    = WinWindowFromID(hWnd,IDD_MLEWPPCOMMENT);
    hEFMirr = WinWindowFromID(hWnd,IDD_EFWPPMIRROR);
    WinSendMsg       (hEF,     EM_SETTEXTLIMIT,MPARAM(WPNAMELEN-1),0);
    WinSetWindowText (hEF,     szWorkplace);
    WinSendMsg       (hEFMirr, EM_SETTEXTLIMIT,MPARAM(sizeof(szWPMirror)-1),0);
    WinSetWindowText (hEFMirr, szWPMirror);
    WinCheckButton(hWnd,IDD_CBWPPMIRROR,fUseMirror);
    WinSetDlgItemText(hWnd,IDD_STWPPDIR,szDir);
    if (pszWPComment) WinSetWindowText(hMLE,pszWPComment);
    else              WinSetWindowText(hMLE,"<No comment>");
    WinSendMsg(hMLE,MLM_SETCHANGED,MPARAM(FALSE),0);
    break;
  case WM_CONTROL:
    if (m1==MPFROM2SHORT(IDD_EFWPPMIRROR,EN_CHANGE))
    {
      char sz[CCHMAXPATH];
      WinQueryWindowText(hEFMirr,sizeof(sz),sz);
      WinCheckButton(hWnd,IDD_CBWPPMIRROR,!!*sf_skipspaces(sz));
    }
    break;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
    case IDD_PBWPPCHDIR:
    {
      CPARAMSTR cps;
      cps.l3 = 0; /*++ help id */
      char sz_path[CCHMAXPATH];
      cps.psz1 = sz_path;
      strcpy(cps.psz1,szDir);
      if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpSelDirDlg,NULLHANDLE,IDD_SELDIRDLG,&cps) && cps.psz1)
      {
        strcpy(szDir,cps.psz1);
        WinSetDlgItemText(hWnd,IDD_STWPPDIR,szDir);
      }
      break;
    }
    case IDD_PBWPPMIRRBROWSE:
    {
      char sz[CCHMAXPATH] = "";
      FName2Full(sz,szWPMirror[0] ? szWPMirror : "*.mrr",FALSE,szCurPath);
      if (Browse(sz, sz, hWnd, "Workplace mirror", FALSE))
      {
        LONG l   = strlen(szCurPath);
        FName2Full(sz,sz,FALSE);
        if (strlen(sz) < l+2 || sf_strnicmp(sz,szCurPath,l) || sz[l]!='\\' || strpbrk(sz+l+1,"\\/:"))
        {
          MessageBox(hWnd,"Workplace mirror",MB_ERROR|MB_OK,"Can not use this file as a workplace mirror:"
                     " mirror file must be located in the workplace current directory. "
                     "(Current directory now is ",szCurPath,").");
        }
        else WinSetWindowText(hEFMirr,sz+l+1);
      }
      break;
    }
    case DID_OK:
    {
      DosSetCurrentDir (szDir);
      DosSetDefaultDisk(sf_toupper(szDir[0])-'A'+1);
      QueryCurrentPath(szCurPath);
      strcpy(szCurIOPath, szCurPath);

      char sz[WPNAMELEN];
      {// sz := new wp name
        WinQueryWindowText(hEF,sizeof(sz),sz);
        memmove(sz,sf_skipspaces(sz),strlen(sf_skipspaces(sz))+1);
        WinFocusChange(HWND_DESKTOP,hEF,TRUE);
        if (!strlen(sz)) {Beep(); break;}
        LONG slot1 = PfSearchSlot(sz, FALSE);
        if (slot1>=0 && slot1 != slot)
        {
          WinMessageBox(HWND_DESKTOP,hWnd,"You cat't use this name: it is belonged to an another workplace.",
                        0,0,MB_ERROR|MB_OK|MB_MOVEABLE);
          break;
        }
      }

      {//mirror
        char sz[CCHMAXPATH] = "";
        WinQueryWindowText(hEFMirr,sizeof(sz),sz);
        PSZ  psz = sf_skipspaces(sz);
        if (*psz)
        {
          if (!strchr(psz,'.') && strlen(sz)+5<sizeof(sz)) strcat(psz,".mrr");
          if (strpbrk(psz,"\\/:"))
          {
            MessageBox(hWnd,"Workplace mirror",MB_ERROR|MB_OK,"Can not use '",psz,"'as a workplace mirror file"
                       "because of it is not a valid name name wothout path.");
            break;
          }
          else strcpy(szWPMirror,psz);
        }
        fUseMirror = WinQueryButtonCheckstate(hWnd,IDD_CBWPPMIRROR) && szWPMirror[0];
      }

      hstWPlaces.Delete(szWorkplace);
      strcpy(szWorkplace,sz);
      if (WinSendMsg(hMLE,MLM_QUERYCHANGED,0,0))
      {
        LONG l = WinQueryWindowTextLength(hMLE)+1;
        free(pszWPComment);
        if (pszWPComment = (PSZ)malloc(l))
          WinQueryWindowText(hMLE,l,pszWPComment);
      }
      PfStoreWPlace(szWorkplace, abs(slot));
      WinDismissDlg(hWnd,1);
      break;
    }
    case DID_CANCEL:
      WinDismissDlg(hWnd,0);
      break;
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_WPPROPDLG);
      break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_WPPROPDLG);
      break;
    }
  default:
   return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

MRESULT EXPENTRY dpWPTplDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static HWND        hLB;
  static HWND        hMLE;
  static HWND        hEF;
  static int         nCurIt;
  static LHANDLE     hCurH;
  static WPTPLOFFICE wptplOffice;

  switch(ulMsg)
  {
  case WM_INITDLG:
    CenterChildWindow(hWnd);
    hLB    = WinWindowFromID(hWnd,IDD_LBWPTPLS);
    hMLE   = WinWindowFromID(hWnd,IDD_MLEWPTPL);
    hEF    = WinWindowFromID(hWnd,IDD_EFWPTPLNAME);
    nCurIt = LIT_NONE;
    hCurH  = 0;
    WinSendMsg(hEF,EM_SETTEXTLIMIT,MPARAM(WPNAMELEN-1),0);
    wptplOffice.ReBuild();
    wptplOffice.ShowList(hLB);
    WinSendMsg(hWnd,WM_USER,0,0);
    break;
  case WM_USER:
  { // Re-enable all, fill hMLE
    nCurIt = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    hCurH  = nCurIt == LIT_NONE? 0 : (LHANDLE)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nCurIt),0);
    wptplOffice.ShowComment(hCurH,hMLE);
    WinEnableControl(hWnd, IDD_PBWPTPLAPPLY, !!hCurH);
    WinEnableControl(hWnd, IDD_PBWPTPLREM,   !wptplOffice.IsReadOnly(hCurH));
    break;
  }
  case WM_CONTROL:
    if (m1==MPFROM2SHORT(IDD_LBWPTPLS,LN_ENTER) || m1==MPFROM2SHORT(IDD_LBWPTPLS,LN_SELECT))
    {
      int nIt = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      if (nIt==nCurIt) break;
      wptplOffice.RefreshComment(hCurH,hMLE);
      WinSendMsg(hWnd,WM_USER,0,0);
    }
    break;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
      case IDD_PBWPTPLAPPLY:
        wptplOffice.ApplyTpl(hCurH,hWnd);
        break;
      case IDD_PBWPTPLREM:
        if (wptplOffice.RemoveTpl(hCurH,hWnd))
        {
          nCurIt = LIT_NONE;
          hCurH  = 0;
          wptplOffice.ReBuild();
          wptplOffice.ShowList(hLB);
          WinSendMsg(hWnd,WM_USER,0,0);
        }
        break;
      case IDD_PBWPTPLADV:
        wpTplAdvancedInit.ModifyDlg(hWnd);
        break;
      case IDD_PBWPTPLSAVE:
      {
        char szName[WPNAMELEN];
        WinQueryWindowText(hEF,sizeof(szName),szName);
        PSZ  psz = sf_skipspaces(szName);
        if (!*psz)
        {
          Beep();
          WinSetFocus(HWND_DESKTOP,hEF);
        }
        else
        {
          wptplOffice.RefreshComment(hCurH,hMLE);
          wptplOffice.StoreTpl(psz,hWnd);
          nCurIt = LIT_NONE;
          hCurH  = 0;
          wptplOffice.ReBuild();
          wptplOffice.ShowList(hLB);
          WinSendMsg(hWnd,WM_USER,0,0);
        }
        break;
      }
      case DID_CANCEL:
        wptplOffice.RefreshComment(hCurH,hMLE);
        WinDismissDlg(hWnd,0);
        break;
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_WPTPLDLG);
        break;
    }
    break; // WM_COMMAND
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_WPTPLDLG);
      break;
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

WPTPLOFFICE::WPTPLOFFICE        () {memset(this,0,sizeof(*this));}
WPTPLOFFICE::~WPTPLOFFICE       () {free_all();memset(this,0,sizeof(*this));}
WPTPLOFFICE::ONEWPTPL::ONEWPTPL () {memset(this,0,sizeof(*this));}
WPTPLOFFICE::ONEWPTPL::~ONEWPTPL() {free(pszComment); memset(this,0,sizeof(*this));}

void WPTPLOFFICE::ReBuild() // Поиметь инфо обо всех доступных темплитах
{
  free_all();
  // В os2.ini/xds ключи XDS_WPT_* - пути к *.ini файлам с системными темплитами (slots 1..)
  while(TRUE)
  {
    ULONG bufsize;
    if (!PrfQueryProfileSize(HINI_USERPROFILE,"XDS",0,&bufsize)) break;
    PSZ   pszBuf  = (char*)malloc(bufsize+1);
    if (!pszBuf || !PrfQueryProfileData(HINI_USERPROFILE,"XDS",0,pszBuf,&bufsize)) { free(pszBuf); break; }
    pszBuf[bufsize] = '\0';
    for (char *pch=pszBuf; pch<pszBuf+bufsize; pch = strchr(pch,'\0')+1)
    {
      if (strncmp(pch,"XDS_WPT_",8)) continue;
      char    sz[CCHMAXPATH];
      ULONG   cb = sizeof(sz);
      if (PrfQueryProfileData(HINI_USERPROFILE,"XDS",pch,sz,&cb))
        app_ini_file(sz,TRUE);
    }
    free(pszBuf);
    break;
  }
  // В szIniPath файл WPTPL_FILE содержит пользовательские темплиты (slots 1..)
  {
    char sz[CCHMAXPATH];
    if (FName2Full(sz,WPTPL_FILE,TRUE,szIniPath))
      app_ini_file(sz,FALSE);
  }
}
void WPTPLOFFICE::ShowList(HWND hLB) // Заполнить лист
{
  int nSel = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
  int nTop = (int)WinSendMsg(hLB,LM_QUERYTOPINDEX,0,0);
  int nMax = -1;
  WinSendMsg(hLB,LM_DELETEALL,0,0);
  for(PONEWPTPL p=poneList; p; p=p->next)
  {
    int nIt = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),MPARAM(p->szName));
    WinSendMsg(hLB,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(p));
    nMax++;
  }
  if (nTop!=LIT_NONE && nMax>=0) WinSendMsg(hLB,LM_SETTOPINDEX,MPARAM(min(nTop,nMax)),0);
  if (nSel!=LIT_NONE && nMax>=0) WinSendMsg(hLB,LM_SELECTITEM,MPARAM(min(nSel,nMax)),MPARAM(TRUE));
  else                           WinSendMsg(hLB,LM_SELECTITEM,MPARAM(0),             MPARAM(TRUE));
}
void WPTPLOFFICE::ShowComment(LHANDLE h, HWND hMLE) // Показать комментарий (h - хэндл в листе)
{
  PONEWPTPL p=seek(h);
  PSZ psz = p ? p->pszComment : 0;
  if (psz)    WinSetWindowText(hMLE,psz);
  else if (h) WinSetWindowText(hMLE,"<No comment>");
  else        WinSetWindowText(hMLE,"");
  WinSendMsg(hMLE,MLM_SETCHANGED, MPARAM(FALSE),0);
  WinSendMsg(hMLE,MLM_SETREADONLY,MPARAM(!p || p->fReadOnly),0);
}
void WPTPLOFFICE::RefreshComment(LHANDLE h, HWND hMLE) // Обновить, если надо, комментарий (h - хэндл в листе)
{
  PONEWPTPL p=seek(h);
  if (!p || p->fReadOnly) return;
  if ( WinSendMsg(hMLE,MLM_QUERYCHANGED,0,0) &&
       MBID_YES == WinMessageBox(HWND_DESKTOP, WinQueryWindow(hMLE,QW_PARENT),
                   "The comment is changed. Save changes?", "Workplace list",0, MB_QUERY|MB_YESNO|MB_MOVEABLE)
     )
  {
    LONG l = WinQueryWindowTextLength(hMLE)+1;
    free(p->pszComment);
    if (p->pszComment = (PSZ)malloc(l))
    {
      WinQueryWindowText(hMLE,l,p->pszComment);
      HINI hini0 = hfPrf;
      if (hfPrf = PrfOpenProfile(hAB,p->szIniFile))
      {
        PfWriteItem(p->lSlot,"szWPComment",p->pszComment,l);
        PfWriteLong(p->lSlot,"nWPComment", l);
        PrfCloseProfile(hfPrf);
      }
      hfPrf = hini0;
    }
  }
  WinSendMsg(hMLE,MLM_SETCHANGED,MPARAM(FALSE),0);
}
void WPTPLOFFICE::StoreTpl(PSZ szName, HWND hMsgWnd) // Создать новый темплит
{
  char      szF[CCHMAXPATH] = WPTPL_FILE;
  HINI      hini0           = hfPrf;
  PONEWPTPL p;

  FName2Full(szF,WPTPL_FILE,FALSE,szIniPath);
  szName = sf_skipspaces(szName);
  if (!szName[0] || strlen(szName)>WPNAMELEN)
  {
    MessageBox(hMsgWnd,"Save workplace template",MB_ERROR|MB_OK,"Can not save template: wrong name spacifyed.");
    return;
  }
  for (p=poneList; p && sf_stricmp(szName,p->szName); p=p->next);
  if (p)
  {
    if (p->fReadOnly)
    {
      MessageBox(hMsgWnd,"Save workplace template",MB_ERROR|MB_OK,"Can not overwrite workplace '",szName,"'.");
      return;
    }
    else if (MBID_YES!=MessageBox(hMsgWnd,"Save workplace template",MB_ICONQUESTION|MB_YESNO,"Name '",szName,"' is belonged to another template. Do you want to overwrite it?"))
      return;
  }

  /// WARNING!! hfPrf is changed:
  if (!(hfPrf = PrfOpenProfile(hAB,szF)))
  {
    hfPrf = hini0;
    MessageBox(hMsgWnd,"Save workplace template",MB_ERROR|MB_OK,"Can not open or create file '",szF, "'. Template is not saved.");
    return;
  }
  LONG slot = PfSearchSlot(szName, FALSE);
  slot      = abs(slot);
  pf_storeWP (slot, TRUE, szName, PRF_IO_TEMPLATE);
  hfPrf = hini0;
}
BOOL WPTPLOFFICE::RemoveTpl(LHANDLE h, HWND hMsgWnd) // Удалить темплит. Вернет fOk.
{
  PONEWPTPL p;
  HINI      hini0 = hfPrf;
  if (!(p=seek(h)) || p->fReadOnly) return FALSE;
  if (MBID_YES != MessageBox(hMsgWnd,"Remove workplace template",MB_ICONQUESTION|MB_YESNO,"Do you really want to permanentely remove '",p->szName,"' template?"))
    return FALSE;
  /// WARNING!! hfPrf is changed:
  if (!(hfPrf = PrfOpenProfile(hAB,p->szIniFile)))
  {
    hfPrf = hini0;
    MessageBox(hMsgWnd,"Remove workplace template",MB_ERROR|MB_OK,"Can not open file '",p->szIniFile, "'. The template is not removed.");
    return FALSE;
  }
  PfWriteLong(p->lSlot,"fFreeSlot",TRUE);
  hfPrf = hini0;
}
void WPTPLOFFICE::ApplyTpl(LHANDLE h, HWND hMsgWnd, BOOL fNewWP) // Применить темплит (h - хэндл в листе)
{
  MRESULT EXPENTRY dpAppWpDlg(HWND,ULONG,MPARAM,MPARAM);
  PONEWPTPL p;
  HINI      hini0 = hfPrf;
  ULONG     fl    = 0x7; // bits = 0..0ATR (Associations, Tools (Remove old tools))
  if (!(p=seek(h))) return;
  if (!fNewWP)
  {
    CPARAMSTR cps;
    fl = 0;
    cps.psz1 = p->szName;
    cps.psz2 = p->pszComment ? p->pszComment : "<No comment>";
    if (!(fl = (ULONG)WinDlgBox(HWND_DESKTOP,hMsgWnd,(PFNWP)dpAppWpDlg,NULLHANDLE,IDD_APPWPDLG,&cps)))
      return;
  }
  /// WARNING!! hfPrf is changed:
  if (!(hfPrf = PrfOpenProfile(hAB,p->szIniFile)))
  {
    hfPrf = hini0;
    MessageBox(hMsgWnd,"Apply workplace template",MB_ERROR|MB_OK,"Can not open file '",p->szIniFile,
               fNewWP ? ". Free workplace created." : "'. Settings are not changed.");
    return;
  }
  ULONG flPrf = PRF_IO_TEMPLATE;
  if (fl & 0x4) flPrf |= PRF_IO_TPL_ASS;
  if (fl & 0x2) flPrf |= PRF_IO_TPL_TOOLS;
  if (fl & 0x1) flPrf |= PRF_IO_TPL_RMTLS;
  pf_readWP (p->lSlot, flPrf);
  wpTplAdvancedInit.RProfile(p->lSlot,"WPTplAdvScript");
  wpTplAdvancedInit.ApplyNow(hMsgWnd); /*+++ Неприятность: от старых advanced settings ничего не остается... 8-( */
  hfPrf = hini0;
  if (!fNewWP && flPrf & PRF_IO_TPL_ASS &&
      MBID_YES==MessageBox(hMsgWnd,"Apply workplace template",MB_ICONQUESTION|MB_YESNO,"Files associations are changed. Do you want to apply the new associations naw to all the opened editors?"))
  {
    for (HWND hEd0 = grOffice.EnumEd(); hEd0; hEd0=grOffice.NextEnumEd())
    {
      PSZ pszFName = grOffice.QueryEdFile(hEd0);
      if (!pszFName) pszFName = "";
      PASSTR pAss0 = ptOffice.QueryAss(pszFName);
      ptOffice.ApplyAss(hEd0,pAss0);
    }
  }
  if (!fNewWP)
    MessageBox(hMsgWnd,"Apply workplace template",MB_INFORMATION|MB_OK,"Done.");
}
BOOL WPTPLOFFICE::IsReadOnly(LHANDLE h) // TRUE если h - read olny или h = 0
{
  PONEWPTPL p = seek(h);
  return !p || p->fReadOnly;
}
void WPTPLOFFICE::app_ini_file(PSZ szFile, BOOL fReadOnly)
{
  PONEWPTPL pNew;
  PONEWPTPL *ppNew;
  HINI      hini0 = hfPrf;
  LONG      slot, lSlots,l;
  char      szF[CCHMAXPATH];
  for (ppNew=&poneList; *ppNew; ppNew = &(*ppNew)->next);
  if  (!FName2Full(szF,szFile,TRUE)) return;

  /// WARNING!! hfPrf is changed:
  if (!(hfPrf = PrfOpenProfile(hAB,szF))) {hfPrf=hini0; return;}

  for (PfReadLong(0,"lSlots",&lSlots,1), slot = 1;  slot<lSlots;  slot++)
  {
    char szWP[WPNAMELEN+1];
    PSZ  pszC = 0;
    PfReadLong(slot,"fFreeSlot",&l,TRUE);
    if (l) continue;
    PfReadItem(slot,"szWorkplace", szWP,sizeof(szWP),"",0);
    PfReadLong(slot,"nWPComment",&l,0);
    if (l>1 && (pszC = (PSZ)malloc(l))) PfReadItem(slot,"szWPComment",pszC,l,"<No comment>",0);
    if (!(pNew = new ONEWPTPL)) {free(pszC); continue;}
    strcpy(pNew->szName,   szWP);
    strcpy(pNew->szIniFile,szF);
    pNew->pszComment = pszC;
    pNew->lSlot      = slot;
    pNew->fReadOnly  = fReadOnly;
    *ppNew           = pNew;
    ppNew            = &pNew->next;
  }
  hfPrf = hini0;
}

//--- class WPTPLADVANCEDINIT
//
// Это некий абстрактный класс для advanced инициализации нового WP при создании его из
// темплита. Пока что просто игает rexx script.
// Хранится не только в темплитах, но и в WPs (и базах зеркал), чтобы можно было из них
// создавать темплиты.
//
WPTPLADVANCEDINIT:: WPTPLADVANCEDINIT() {memset(this,0,sizeof(*this));}
WPTPLADVANCEDINIT::~WPTPLADVANCEDINIT() {free(pszRexxScript);pszRexxScript=0;}
void WPTPLADVANCEDINIT::ApplyNow(HWND hMsgWnd)
{
  if (pszRexxScript && *sf_skipspaces(pszRexxScript))
    RxExecute(pszRexxScript);
}
void WPTPLADVANCEDINIT::ModifyDlg(HWND hMsgWnd)
{
  CPARAMSTR cps;
  cps.l1 = (LONG)this;
  WinDlgBox(HWND_DESKTOP,hMsgWnd,(PFNWP)dpWpTplAdvDlg,NULLHANDLE,IDD_WPTPLADVDLG,&cps);
}
void WPTPLADVANCEDINIT::WProfile(LONG slot, PSZ pszName)
{
  int   l   = (pszRexxScript ? strlen(pszRexxScript) : -1) + 1;
  char  sz[100];
  sprintf     (sz,"%s_len",pszName);
  PfWriteLong (slot, sz, l);
  PfWriteItem (slot, pszName, (pszRexxScript ? pszRexxScript : ""), l);
}
void WPTPLADVANCEDINIT::RProfile(LONG slot, PSZ pszName)
{
  char  sz[100];
  LONG  l = 0;;
  free(pszRexxScript);
  pszRexxScript = 0;
  sprintf(sz,"%s_len", pszName);
  if (PfReadLong (slot, sz, &l, 0) && l)
  {
    if (pszRexxScript = (PSZ)malloc(l))
    {
      PfReadItem(slot,pszName,pszRexxScript,l,"",0);
      pszRexxScript[l-1] = 0;
    }
  }
}
MRESULT EXPENTRY dpWpTplAdvDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  static HWND               hMLE;
  static PWPTPLADVANCEDINIT pAdv;
  switch(ulMsg)
  {
  case WM_INITDLG:
  {
    PCPARAMSTR pcps = (PCPARAMSTR)m2;
    pAdv            = (PWPTPLADVANCEDINIT)pcps->l1;
    hMLE            = WinWindowFromID(hWnd,IDD_MLEWPADV);
    if (pAdv->pszRexxScript)
      WinSetWindowText(hMLE,pAdv->pszRexxScript);
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
      case DID_OK:
      {
        free(pAdv->pszRexxScript);
        pAdv->pszRexxScript = 0;
        LONG l = WinQueryWindowTextLength(hMLE)+1;
        if (pAdv->pszRexxScript = (PSZ)malloc(l))
          WinQueryWindowText(hMLE,l,pAdv->pszRexxScript);
      } // no break here
      case DID_CANCEL:
        WinDismissDlg(hWnd,0);
        break;
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_WPTPLADVDLG);
        break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_WPTPLADVDLG);
      break;
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


MRESULT EXPENTRY dpAppWpDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static HWND        hLB;
  static HWND        hMLE;
  static HWND        hEF;
  static int         nCurIt;
  static LHANDLE     hCurH;
  static WPTPLOFFICE wptplOffice;

  switch(ulMsg)
  {
  case WM_INITDLG:
  {
    PCPARAMSTR pcps = (PCPARAMSTR)m2;
    WinSetDlgItemText(hWnd,IDD_EFAWPNAME,     pcps->psz1);
    WinSetDlgItemText(hWnd,IDD_MLEAWPCOMMENT, pcps->psz2 ? pcps->psz2 : "<No comment>");
    WinCheckButton(hWnd,IDD_CBAWPTOOLS,   TRUE);
    WinCheckButton(hWnd,IDD_RBAWPTOOLSADD,TRUE);
    WinCheckButton(hWnd,IDD_CBAWPASS,     TRUE);
    break;
  }
  case WM_CONTROL:
    if (SHORT1FROMMP(m1)==IDD_CBAWPTOOLS)
    {
      BOOL f = WinQueryButtonCheckstate(hWnd,IDD_CBAWPTOOLS);
      WinEnableControl(hWnd,IDD_RBAWPTOOLSADD,f);
      WinEnableControl(hWnd,IDD_RBAWPTOOLSREM,f);
    }
    break;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
      case DID_OK:
      {
        ULONG rc = 0x0; // bits = 0..0ATR (Associations, Tools (Remove old tools))
        if (WinQueryButtonCheckstate(hWnd,IDD_CBAWPTOOLS))    rc |= 0x2;
        if (WinQueryButtonCheckstate(hWnd,IDD_RBAWPTOOLSREM)) rc |= 0x1;
        if (WinQueryButtonCheckstate(hWnd,IDD_CBAWPASS))      rc |= 0x4;
        if (!rc) MessageBox(hWnd,"Apply workplace template",MB_INFORMATION|MB_OK,"Nothing to do.");
        WinDismissDlg(hWnd,rc);
        break;
      }
      case DID_CANCEL:
        WinDismissDlg(hWnd,0);
        break;
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_APPWPDLG);
        break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_APPWPDLG);
      break;
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

