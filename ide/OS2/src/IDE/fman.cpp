/*
*
*  File: FMAN.CPP
*
*  XDS Shell: file management
*
*  (c) 1996,1997 by *FSA & NHA Inc.
*
*/

#define  INCL_WIN
#define  INCL_GPI
#define  INCL_DOS
#include <os2.h>
#include <REXXSAA.H>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "XDSBASE.H"
#include "sf_lib.h"




//=============================== Advanced file load dialog ================================================//




MRESULT     EXPENTRY dpFileDlg (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2);
int         save_prompt        (PFILEED pFE, HWND hMsgWnd);
BUFSCONFIG *pBufsConfig = new  BUFSCONFIG(TRUE);
BUFSCONFIG *pSrcConfig  = new  BUFSCONFIG(FALSE);
BUFSCONFIG *pbcCurrent  = pBufsConfig;

void FileLoadDlg()
{
  if (!pBufsConfig || !pBufsConfig->AutoBuild())  return;
  if (!pSrcConfig  || !pSrcConfig->AutoBuild() )  return;
  BOOL fOk;
  do
  {
    fOk = TRUE;
    if (WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpFileDlg,NULLHANDLE,IDD_FILEDLG,0))
      fOk = pBufsConfig->Apply(hMDI0Client);
    pSrcConfig->Apply(hMDI0Client);
  } while(!fOk);
}

/*
/
/  Definition of our internal class FILEDIR
/
*/
enum FDSORT{
  FDSORT_NAME,   // Files sorting
  FDSORT_EXT     // mode
};
typedef class FILEDIR
{
public:
  LHANDLE  AppFile        (PFILEFINDBUF3 pfb3);     // Append this file;  Returns it's handle or 0 when error
  void     Sort           (FDSORT fds);             // Sort items
  LHANDLE  QueryFirst     ();                       // Returns handle of the 1st file (or 0)
  LHANDLE  QueryNext      ();                       // Returns handle of the next file (or 0)
                                                    //NOTE: In the next methods wrong handle value
                                                    //      caused a runtime error!
  PSZ      QueryName      (LHANDLE h, BOOL fShort); //   Query [short] file name
  ULONG    QueryAttrs     (LHANDLE h);              //   Query attrFile value
  ULONG    QuerySel       (LHANDLE h);              //   Query selected state
  ULONG    QuerySize      (LHANDLE h);              //   Query file size
  ULONG    QueryNamePos   (LHANDLE h);              //   Query filename positon
  void     SetSel         (LHANDLE h,BOOL fSel);    //   Set new selected state
  void     SetNamePos     (LHANDLE h, ULONG l);     //   Set new filename position
  BOOL     FastFind       (char ch);                // Try with ch; if ch=0 - clear data; rc: TRUE if it was find mode before call
  BOOL     FastFindNext   (BOOL fUp);               // Try to find next

  FILEDIR  ()       {memset(this,0,sizeof(*this));}
 ~FILEDIR  ()       {freelist();}
  typedef  struct   FILEREC;
  typedef  FILEREC *PFILEREC;

private:
  typedef struct FILEREC
  {
    PFILEREC  next;
    ULONG     attrFile;
    ULONG     cbFile;
    BOOL      fSel;
    PSZ       pszName;
    ULONG     ulNamePos;   // short 1 - xLeft, 2 - xRight
    FILEREC  () {memset(this,0,sizeof(*this));}
   ~FILEREC  () {free(pszName);}
  };

  PFILEREC pfList;         // The list
  PFILEREC pfListTop;      // Last list element (to optimize access)
  PFILEREC pfLastQueried;  // QueryFirst/Next() data
  char     szFastFind[CCHMAXPATH];
  HWND     hFFDisplay;
  BOOL     fDispLocked;
  BOOL     cmp_fnames    (PFILEREC pfr1, PFILEREC pfr2, BOOL fExt);
  void     freelist      ();
};
typedef FILEDIR *PFILEDIR;


/*--- Globals:
*/
PFILEDIR   pfdDirectory    = new FILEDIR;
HBITMAP    hbmpFile        = 0;
HBITMAP    hbmpFNew        = 0;
HBITMAP    hbmpGroup       = 0;
HBITMAP    hbmpGOpen       = 0;
ULONG      ulDirItHeight   = 1;
ULONG      ulGrpItHeight   = 1;
ULONG      ulBMWidth       = 1;
ULONG      ulBMHeight      = 1;
HPOINTER   hptrDropGItem   = 0;
HPOINTER   hptrNoDropGItem = 0;
HPOINTER   hptrDropFile    = 0;
HPOINTER   hptrNoDropFile  = 0;
HPOINTER   hptrDropFiles   = 0;
HPOINTER   hptrNoDropFiles = 0;
HWND       hCBPath         = 0;
HWND       hLBDir          = 0;
HWND       hLBGrp          = 0;
HWND       hSTPath         = 0;
char       szShownDir [CCHMAXPATH] = ".\\";
char       szShownPath[CCHMAXPATH] = ".";
//--------- Dialog settings: we'll write it to the profile
char   szGrpPPFont[FACESIZE+10] = "8.Helv";
char   szDirPPFont[FACESIZE+10] = "11.System VIO";
FDSORT fdSort                   = FDSORT_EXT;
BOOL   fShowFullNames           = TRUE;

/*--- Function prtotypes
*/
BOOL    ReReadDir          (PSZ pszPatt);
void    FillDirList        (HWND hLBDir, ULONG ulDirShowMode, PSZ szSelItem="");
void    SetStPathText      (HWND hSTPath,PSZ pszDir);
void    FillGroupList      (HWND hLBGrp, LHANDLE hSel);
LONG    LBPos2Item         (HWND hLB, ULONG ulItemHeight, LONG yPos);
void    Win2WinCoord       (HWND hwSource, HWND hwTarget, PPOINTL pptl);
void    set_lists_heights  ();
MRESULT EXPENTRY wpGrpList (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2);
MRESULT EXPENTRY wpDirList (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2);
BOOL    StartDrag          (HWND hwndSource, PMYDRAGITEM pmdi, HPOINTER hptrDrop, HPOINTER hptrNoDrop);


// Messages are used only for the hLBGrp & hLBDir windows
//
#define  LLM_DRAWITEM          (WM_USER+103)
#define  LLM_LOCKUPDATE        (WM_USER+104)
#define  LLM_SETDIRSHOWMODE    (WM_USER+105)
#define  LLM_CONTROL           (WM_USER+106)
//
#define  LDN_DIRUP             3000 // WM_CONTROL's parameter (by directory list)

MRESULT EXPENTRY dpFileDlg   (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  switch(ulMsg){
  case WM_INITDLG:
  {
    CenterChildWindow(hWnd);
    hCBPath   = WinWindowFromID(hWnd,IDD_CBPATH);
    hLBDir    = WinWindowFromID(hWnd,IDD_LBDIRFILES);
    hLBGrp    = WinWindowFromID(hWnd,IDD_LBFILESLOADED);
    hSTPath   = WinWindowFromID(hWnd,IDD_STPATHNAME);
    WinSendMsg(hCBPath,EM_SETTEXTLIMIT,MPARAM(CCHMAXPATH),0);
    if (!hptrDropGItem)   hptrDropGItem   = WinLoadPointer(HWND_DESKTOP,0,PTR_DROPGITEM);
    if (!hptrNoDropGItem) hptrNoDropGItem = WinLoadPointer(HWND_DESKTOP,0,PTR_NODROPGITEM);
    if (!hptrDropFile)    hptrDropFile    = WinLoadPointer(HWND_DESKTOP,0,PTR_DROPFILE);
    if (!hptrNoDropFile)  hptrNoDropFile  = WinLoadPointer(HWND_DESKTOP,0,PTR_NODROPFILE);
    if (!hptrDropFiles)   hptrDropFiles   = WinLoadPointer(HWND_DESKTOP,0,PTR_DROPFILES);
    if (!hptrNoDropFiles) hptrNoDropFiles = WinLoadPointer(HWND_DESKTOP,0,PTR_NODROPFILES);
    wpGrpList (hLBGrp,   MM_START,
              MPARAM(ULONG(WinSubclassWindow(hLBGrp,   (PFNWP)wpGrpList ))),0L);
    wpDirList (hLBDir,   MM_START,
              MPARAM(ULONG(WinSubclassWindow(hLBDir,   (PFNWP)wpDirList ))),0L);
    WinSetPresParam(hLBDir,PP_FONTNAMESIZE,strlen(szDirPPFont)+1,szDirPPFont);
    WinSetPresParam(hLBGrp,PP_FONTNAMESIZE,strlen(szGrpPPFont)+1,szGrpPPFont);
    if (!hbmpFile)
    {
      HPS hps   = WinGetPS(HWND_DESKTOP);
      hbmpFile  = GpiLoadBitmap(hps, NULLHANDLE,     BMP_FILE2LST, 0, 0);
      hbmpFNew  = GpiLoadBitmap(hps, NULLHANDLE,     BMP_FNEW2LST, 0, 0);
      hbmpGroup = GpiLoadBitmap(hps, NULLHANDLE,     BMP_GRP2LST,  0, 0);
      hbmpGOpen = GpiLoadBitmap(hps, NULLHANDLE,     BMP_GOP2LST,  0, 0);
    }
    ulGrpItHeight = 1;
    ulDirItHeight = 1;
    set_lists_heights();
    FillGroupList(hLBGrp,0);
    // Directory settings
    if (szCurIOPath[0]) strcpy(szShownDir,szCurIOPath);
    else                QueryCurrentPath(szShownDir);
    FName2Full(szShownDir,szShownDir,FALSE);
    strcpy(szShownPath,szShownDir);
    strcat (szShownDir,"\\");
    if (!ReReadDir(szShownDir))
      SetStPathText(hSTPath,szShownPath);
    FillDirList(hLBDir,0L);
    // Fill hCBPath
    {
      DosError(FERR_DISABLEHARDERR);
      ULONG ulCur;
      ULONG ulMap;
      ULONG lSel = 0;
      DosQueryCurrentDisk(&ulCur,&ulMap);
      for (char ch='A'; ch<='Z'; ch++,ulMap>>=1)
      {
        if (!(ulMap & 0x1)) continue;
        char   sz[5];
        sprintf(sz,"%c:",ch);
        int nIt=(int)WinSendMsg(hCBPath,LM_INSERTITEM,   MPARAM(LIT_END),MPARAM(sz));
        if (sf_toupper(szShownDir[0]==ch)) lSel=nIt;
      }
      WinSendMsg(hCBPath,LM_SELECTITEM,MPARAM(lSel),MPARAM(TRUE));
      DosError(FERR_ENABLEHARDERR);
    }
    WinSetFocus(HWND_DESKTOP,hLBGrp);
    return MRESULT(TRUE);
  }
  case WM_MEASUREITEM:
    return MRESULT( SHORT1FROMMP(m1)==IDD_LBDIRFILES ? ulDirItHeight : ulGrpItHeight);
  case WM_DRAWITEM:
    if (SHORT1FROMMP(m1)==IDD_LBFILESLOADED)
      return WinSendMsg(hLBGrp,LLM_DRAWITEM,m1,m2);
    else if (SHORT1FROMMP(m1)==IDD_LBDIRFILES)
      return WinSendMsg(hLBDir,LLM_DRAWITEM,m1,m2);
    else goto defret;
  case WM_CONTROL:
  {
    // Group list: let him process it...
    if (HWND(m2)==hLBGrp)
      return WinSendMsg(hLBGrp,LLM_CONTROL,m1,m2);
    // Enter a pattern
    if (HWND(m2)==hCBPath && SHORT2FROMMP(m1)==CBN_ENTER)
    {
      char sz[CCHMAXPATH];
      WinQueryWindowText(hCBPath,sizeof(sz),sz);
      char *pszPatt = sf_skipspaces(sz);
      if (!pszPatt[0]) break;

      if (!ReReadDir(pszPatt))
      {
        FillDirList(hLBDir,0L);
        SetStPathText(hSTPath,szShownPath);
        if (WinQueryFocus(HWND_DESKTOP)!=hCBPath)
          WinSetFocus(HWND_DESKTOP,hLBDir);
      }
      else
      {
        char szErr[CCHMAXPATH+100];
        sprintf(szErr,"Can not retrive path: '%s'.",pszPatt);
        MessageBox(hWnd,"Retrive path",MB_ERROR|MB_OK,szErr);
        WinSetFocus(HWND_DESKTOP,hCBPath);
      }
      break;
    }
    // Directory
    if (HWND(m2)==hLBDir)
    {
      if (SHORT2FROMMP(m1)==LN_SELECT || SHORT2FROMMP(m1)==LN_ENTER)
        pfdDirectory->FastFind(0);
      int  nSrcIt = (int)WinSendMsg(hLBDir,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      BOOL fDirUp = SHORT2FROMMP(m1)==LDN_DIRUP;
      if ((SHORT2FROMMP(m1)==LN_ENTER || fDirUp) && nSrcIt!=LIT_NONE)
      {
        //--- LN_ENTER or LDN_DIRUP in the file list ---//
        char szNewDir[CCHMAXPATH];
        strcpy(szNewDir,szShownDir);
        if (fDirUp) nSrcIt=0;
        LHANDLE h = (LHANDLE)WinSendMsg(hLBDir,LM_QUERYITEMHANDLE,MPARAM(nSrcIt),0);
        if (!h || !(pfdDirectory->QueryAttrs(h)&FILE_DIRECTORY)) break;

        char  sz   [CCHMAXPATH];
        char  szSel[CCHMAXPATH] = "";
        strcpy(sz,    pfdDirectory->QueryName(h,TRUE));
        if (!strcmp(sz,".."))
        {
          char *pch = strrchr(szNewDir,'\\');
          if (pch && pch-szNewDir>2) pch[0] = 0; // e:\zzz\zu[\]
          if (pch = strrchr(szNewDir,'\\'))
          {
            strcpy(szSel,pch+1);
            pch[1] = 0;                          // e:\zzz\[z]u
          }
        }
        else
        {
          if (fDirUp) break;
          strcat(szNewDir,sz);
          strcat(szNewDir,"\\");
        }

        if (!ReReadDir(szNewDir))
          SetStPathText(hSTPath,szShownPath);
        FillDirList(hLBDir,0L,szSel);
      }
    }
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
    case IDD_PBPATHGO:
      WinSendMsg(hWnd,WM_CONTROL,MPFROM2SHORT(0,CBN_ENTER),MPARAM(hCBPath));
      break;
    case IDD_PBFILEEDITORS:
    case IDD_PBFILESRCLIST:
      pbcCurrent = (SHORT1FROMMP(m1)==IDD_PBFILEEDITORS) ? pBufsConfig : pSrcConfig;
      FillGroupList(hLBGrp,0);
      break;
    case DID_OK:
      WinDismissDlg(hWnd,TRUE);
      break;
    case DID_CANCEL:
      WinDismissDlg(hWnd,FALSE);
      break;
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_FILEDLG);
      break;
    }
    return 0;
  case WM_CHAR:
  {
    USHORT fsflags = SHORT1FROMMP(m1);
    LONG    nIt    = (LONG)   WinSendMsg(hLBDir,LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
    LHANDLE h      = (LHANDLE)WinSendMsg(hLBDir,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
    if (fsflags & KC_KEYUP) goto defret;
    switch(SHORT2FROMMP(m2))
    {
    case VK_F1:
      if (!(fsflags&KC_ALT))
      {
        DisplayHelpPanel(PANEL_FILEDLG);
        break;
      }
    case VK_F2:
      if (fsflags&KC_ALT)
      {
        WinSetFocus(HWND_DESKTOP,hCBPath);
        WinSendMsg(hCBPath,CBM_SHOWLIST,MPARAM(TRUE),0);
        break;
      }
      break;
    case VK_F3:
    case VK_F4:
      if (fsflags&KC_CTRL && h)
      {
        char    szSel[CCHMAXPATH] = "";
        strcpy(szSel, pfdDirectory->QueryName(h,TRUE));
        pfdDirectory->Sort((SHORT2FROMMP(m2)==VK_F3) ? FDSORT_NAME : FDSORT_EXT);
        FillDirList(hLBDir,0L,szSel);
      }
      break;
    case VK_INSERT:
    {
      if (h)
      {
        pfdDirectory->SetSel(h,!pfdDirectory->QuerySel(h));
        char sz[CCHMAXPATH+100]; // Redraw the item:
        WinSendMsg(hLBDir,LM_QUERYITEMTEXT,MPFROM2SHORT(nIt,sizeof(sz)),MPARAM(sz));
        WinSendMsg(hLBDir,LM_SETITEMTEXT,  MPARAM(nIt),                 MPARAM(sz));
        WinSendMsg(hLBDir,WM_CHAR,         MPFROM2SHORT(KC_VIRTUALKEY,0),MPFROM2SHORT(0,VK_DOWN));
      }
      break;
    }
    default: goto defret;
    }
  }
  default:
  defret:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

void set_lists_heights()
{
  ULONG             uldir;
  ULONG             ulgrp;
  BITMAPINFOHEADER  bmpData;
  FONTMETRICS       fm;
  HPS               hps;
  hps = WinGetPS(hLBDir);
    GpiQueryFontMetrics(hps,sizeof(fm),&fm);
    uldir = fm.lMaxBaselineExt+fm.lExternalLeading;
  WinReleasePS(hps);

  hps = WinGetPS(hLBGrp);
    GpiQueryFontMetrics(hps,sizeof(fm),&fm);
    ulgrp = fm.lMaxBaselineExt+fm.lExternalLeading;
  WinReleasePS(hps);

  if (GpiQueryBitmapParameters(hbmpFile,&bmpData))
  {
    ulBMWidth  = bmpData.cx;
    ulBMHeight = bmpData.cy;
  }
  ulgrp = max (ulgrp,ulBMHeight+1);
  if (ulGrpItHeight!=ulgrp) { ulGrpItHeight = ulgrp; WinSendMsg(hLBGrp,LM_SETITEMHEIGHT,MPARAM(ulGrpItHeight),0); }
  if (ulDirItHeight!=uldir) { ulDirItHeight = uldir; WinSendMsg(hLBDir,LM_SETITEMHEIGHT,MPARAM(ulDirItHeight),0); }
}

//--- Отображение содержимого pfdDirectory в окне
// hLBDir        - лист
// ulDirShowMode - пока только сортировка
// szSelItem     - при наличии такой строки - курсор ставить на нее
void FillDirList(HWND hLBDir, ULONG ulDirShowMode, PSZ szSelItem)
{
  LONG    lSel   = 0;
  LHANDLE h      = pfdDirectory->QueryFirst();
  char    sz[CCHMAXPATH+15];
  WinSendMsg(hLBDir,LM_DELETEALL,0,0);
  if (!h)
  {
    WinSendMsg(hLBDir,LM_INSERTITEM,MPARAM(LIT_END),MPARAM("<Empty>"));
    return;
  }
  WinSendMsg(hLBDir,LLM_LOCKUPDATE,MPARAM(TRUE),0);
  do
  {
    PSZ  psz = pfdDirectory->QueryName(h,TRUE);
    if (!psz) continue;
    if (pfdDirectory->QueryAttrs(h) & FILE_DIRECTORY)
      if (strcmp(psz,"..")) strcpy(sz,"SUBDIR");
      else                  strcpy(sz,"UP-DIR");
    else
      sprintf(sz,"% 8u",pfdDirectory->QuerySize(h));
    sprintf(sz+strlen(sz),"│%s",psz);
    LONG nIt = (LONG)WinSendMsg(hLBDir, LM_INSERTITEM,    MPARAM(LIT_END), MPARAM(sz));
                     WinSendMsg(hLBDir, LM_SETITEMHANDLE, MPARAM(nIt),     MPARAM(h));
    if (szSelItem[0] && !sf_stricmp(szSelItem,psz)) lSel = nIt;
  } while(h=pfdDirectory->QueryNext());
  WinSendMsg(hLBDir, LM_SELECTITEM,  MPARAM(lSel),  MPARAM(TRUE));
  WinSendMsg(hLBDir, LLM_LOCKUPDATE, MPARAM(FALSE), 0);
}


//--- Чтение каталога в pfdDirectory
// pszDir - путь (шаблон)
//
BOOL ReReadDir(PSZ pszPatt)
{
  HDIR          hdirFindHandle = HDIR_SYSTEM;
  FILEFINDBUF3  FindBuffer     = {0};
  ULONG         ulFindCount    = 1;
  APIRET        rc             = 0;
  APIRET        err            = 0;
  char          sz_dir    [CCHMAXPATH];         // The directory ("c:\zzz");
  char          sz_patt   [CCHMAXPATH] = "*.*"; // Pattern       ("*.zzz", may be "")
  HEXPR         hPatt          = 0;
  char         *pch;
  int           i;

  if (!FName2Full(sz_dir,pszPatt,FALSE,szShownPath))
  {
    strcpy(sz_dir,pszPatt);
    for (i=0; sz_dir[i]; i++) if (sz_dir[i]=='/') sz_dir[i] = '\\';
  }

  {//--- Отъедим от sz_dir pattern, уберем хвостовой '\'
    if (pch = strrchr(sz_dir,'\\'))
    {
      if      (!pch[1]) // Хвостовой '\'?
        *pch=0;
      else if (strpbrk(pch+1,"*?")) // Шаблон в хвосте?
      {
        strcpy(sz_patt,pch+1);
        *pch=0;
      }
    }
  }
  pch        = strncmp(sz_dir,"\\\\",2) ? sz_dir : sz_dir+2;
  BOOL fRoot = !strchr(pch,'\\');

  //--- Компиляем шаблон


  if (strcmp(sz_patt,"*.*"))
  {
    char sz[CCHMAXPATH];
    LONG res;
    strcpy(sz,sz_patt);
    RCThunk_Compile(sf_strtoupper(sz), &hPatt, &res);
  }

  char sz_dos[CCHMAXPATH];
  sprintf(sz_dos,"%s\\*.*",sz_dir);
  PFILEDIR pfd = new FILEDIR;
  DosError(FERR_DISABLEHARDERR);
  if (!(err=DosFindFirst(sz_dos, &hdirFindHandle,
                    FILE_DIRECTORY|FILE_READONLY|FILE_HIDDEN|FILE_SYSTEM|FILE_ARCHIVED ,
                    &FindBuffer, sizeof(FILEFINDBUF3), &ulFindCount, FIL_STANDARD)))
  {
    for (   ;
         !rc;
         ulFindCount = 1,
         rc = DosFindNext(hdirFindHandle, &FindBuffer, sizeof(FILEFINDBUF3), &ulFindCount))
    {
      char sz[CCHMAXPATH];
      strcpy(sz,FindBuffer.achName);
      if (!strcmp(sz,"."))           continue;
      if (!strcmp(sz,"..") && fRoot) continue;
      if (FindBuffer.attrFile & FILE_DIRECTORY || RCThunk_Match (hPatt, sf_strtoupper(sz)))
      {
        sprintf(sz, "%s\\%s", sz_dir, FindBuffer.achName);
        strcpy (FindBuffer.achName,sz);
        pfd->AppFile(&FindBuffer);
      }
    }
    DosFindClose(hdirFindHandle);
  }
  DosError(FERR_ENABLEHARDERR);
  RCThunk_FreeExpr(hPatt);
  if (err)
  {
    delete pfd;
    return (BOOL)err;
  }
  delete pfdDirectory;
  pfdDirectory = pfd;
  pfdDirectory->Sort(fdSort);
  strcpy(szCurIOPath,sz_dir);
  strcpy(szShownPath,sz_dir);
  sprintf(szShownDir, "%s\\", sz_dir);
  if (sz_patt[0] && strcmp(sz_patt,"*.*")) sprintf(szShownPath+strlen(szShownPath),"\\%s",sz_patt);
  return FALSE;
}

void SetStPathText(HWND hSTPath, PSZ pszDir)
{
  const int MAXLEN = 32;
  char sz[MAXLEN+5];
  LONG l0  = strlen(pszDir);
  if (l0>MAXLEN) sprintf(sz,"...%s",pszDir+l0-MAXLEN);
  else           strcpy (sz,pszDir);
  WinSetWindowText(hSTPath,sz);
}


//-------------------- GroupList box -------------------------------


BOOL ExpandGroup(HWND hLBGrp, int nIt)
{
  // Open/close the nIt group
  LHANDLE hG = (LHANDLE)WinSendMsg(hLBGrp,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
  if (hG && pbcCurrent->QueryItemType(hG)==BFITEM_GROUP)
  {
    pbcCurrent->CloseGroup(hG);
    FillGroupList(hLBGrp,hG);
    return TRUE;
  }
  return FALSE;
}


void FillGroupList(HWND hLBGrp, LHANDLE hSel)
{
  LHANDLE     h;
  int         nIt     = 0;
  int         nSel    = 0;
  int         nExists = (int)WinSendMsg(hLBGrp,LM_QUERYITEMCOUNT,0,0);

  hSel = pbcCurrent->QVisibleParent(hSel);

  WinSendMsg(hLBGrp,LLM_LOCKUPDATE,MPARAM(TRUE),0);
  // We don't use LM_DELETEALL to prevent dirty flashes
  for (h=pbcCurrent->StartEnum(); h; h=pbcCurrent->NextEnum())
  {
    PSZ psz = pbcCurrent->QueryItemName(h,!fShowFullNames);
    if (nIt<nExists)
    {
      WinSendMsg(hLBGrp,LM_SETITEMTEXT,  MPARAM(nIt),MPARAM(psz));
      WinSendMsg(hLBGrp,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(h));
    }
    else
    {
      nIt = (int)WinSendMsg(hLBGrp,LM_INSERTITEM,   MPARAM(LIT_END),MPARAM(psz));
                 WinSendMsg(hLBGrp,LM_SETITEMHANDLE,MPARAM(nIt),    MPARAM(h));
    }
    if (h==hSel) nSel = nIt;
    nIt++;
  }
  while(nExists>nIt) WinSendMsg(hLBGrp,LM_DELETEITEM,MPARAM(--nExists),0);
  WinSendMsg(hLBGrp,LM_SELECTITEM, MPARAM(nSel), MPARAM(TRUE));
  WinSendMsg(hLBGrp,LLM_LOCKUPDATE,MPARAM(FALSE),0);
}


MRESULT EXPENTRY wpGrpList (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PFNWP pWindowProc;
  static BOOL  fDrawLocked = FALSE;

  switch(ulMsg)
  {
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
  case MM_START:
    pWindowProc = PFNWP(ULONG(m1));
    return 0;
  case LLM_LOCKUPDATE:
    if (!(fDrawLocked = BOOL(m1)))
      WinInvalidateRect(hWnd,0,0);
    return 0;
  case LLM_CONTROL:
  {
    int nIt = (int)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    if (SHORT2FROMMP(m1)==LN_ENTER && nIt!=LIT_NONE)
      if (!ExpandGroup(hWnd,nIt))
      {
        LHANDLE h = (LHANDLE) WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
        if (h && pbcCurrent->MarkFileActive(h))
          WinPostMsg(WinQueryWindow(hWnd,QW_PARENT),WM_COMMAND,MPARAM(DID_OK),0);
      }
    return 0;
  }
  case LLM_DRAWITEM:
  {
    POWNERITEM pow     = POWNERITEM(m2);
    int        nIt     = pow -> idItem;
    HPS        hps     = pow->hps;
    RECTL      rc      = pow->rclItem; rc.yTop--;
    LONG       clrFore = CLR_BLACK;
    LONG       clrBack = CLR_WHITE;
    LHANDLE    h       = LHANDLE(pow->hItem);
    HBITMAP    bmp     = 0;
    BOOL       fShf    = TRUE;
    POINTL     aptl[TXTBOX_COUNT];
    POINTL     ptl;
    if (!h) break;
    if (nIt==LIT_NONE || fDrawLocked) return MRESULT(TRUE);

    if (pbcCurrent->QueryItemType(h)==BFITEM_GROUP)
    {
      bmp  = pbcCurrent->QueryClosed(h) ? hbmpGroup : hbmpGOpen;
      fShf = FALSE;
    }
    else
      bmp  = pbcCurrent->QueryItemUL(h) ? hbmpFile  : hbmpFNew;

    WinFillRect(hps,&rc,clrBack);

    // Draw bitmap
    ptl.x = rc.xLeft+1 + (fShf ? ulBMWidth : 0);
    ptl.y = (rc.yTop-rc.yBottom-ulBMHeight)/2;
    ptl.y = rc.yBottom+max(ptl.y,0);
    WinDrawBitmap(hps,bmp,NULL,&ptl,clrFore,clrBack,DBM_NORMAL);
    rc.xLeft = ptl.x + ulBMWidth + 1;

    if (pow->fsState){
      clrFore = CLR_WHITE;
      clrBack = CLR_DARKBLUE;
    }

    PSZ psz = pbcCurrent->QueryItemName(h,!fShowFullNames);
    if (GpiQueryTextBox(hps, strlen(psz), psz, TXTBOX_COUNT,aptl))
      rc.xRight = min(rc.xRight, rc.xLeft+aptl[TXTBOX_TOPRIGHT].x+4);

    GpiSetBackMix(hps,BM_OVERPAINT);
    WinDrawText(hps,-1,psz,&rc,clrFore, clrBack,DT_LEFT | DT_VCENTER);
    pbcCurrent->SetItemNamePos( h, ULONG (MPFROM2SHORT(ptl.x, rc.xRight) ));
    pow->fsState = pow->fsStateOld = 0;
    return MRESULT(TRUE);
  }
  case WM_BUTTON2DOWN:
  {
    LONG       nIt       =  LBPos2Item(hWnd, ulGrpItHeight, SHORT2FROMMP(m1));
    LHANDLE    h         = (LHANDLE) WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
    BOOL       fGrp      = pBufsConfig == pbcCurrent;
    MYDRAGITEM mdi;
    WinSendMsg(hWnd, LM_SELECTITEM, MPARAM(h ? nIt : LIT_NONE), MPARAM(TRUE));
    if (!h || pbcCurrent->QueryItemType(h)==BFITEM_GROUP || !fGrp)  break;
    memset(&mdi,0,sizeof(mdi));
    mdi.mdt   = MYDRAG_GROUPITEM;
    mdi.ul    = nIt;
    StartDrag(hWnd,&mdi,hptrDropGItem,hptrNoDropGItem);
    break;
  }
  case WM_BUTTON2UP:
  {
    if (WinQueryFocus(HWND_DESKTOP)!=hWnd) WinSetFocus(HWND_DESKTOP,hWnd);
    LONG     nIt       =  LBPos2Item(hWnd, ulGrpItHeight, SHORT2FROMMP(m1));
    LHANDLE  h         = (LHANDLE) WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
    BOOL     fGrp      = pBufsConfig == pbcCurrent;
    LONG     alDis[3]  = {0,0,0};
    LONG     alChk[2]  = {0,0};
    int      idx       = 0;
    WinSendMsg(hWnd, LM_SELECTITEM, MPARAM(h ? nIt : LIT_NONE), MPARAM(TRUE));
    if (!h || pbcCurrent->QueryItemType(h)==BFITEM_GROUP) alDis[idx++] = IDM_POP_CLOSEWIN;
    if (!nIt && fGrp)                                     alDis[idx++] = IDM_POP_NEWFILE;
    if (fShowFullNames) alChk[0] = IDM_POP_FULLNAMES;
    PlayMenu(hWnd, fGrp ? IDM_POPUP_GRPLIST : IDM_POPUP_GRPLIST1, alChk, alDis, 0);
    return 0;
  }
  case MDM_DRAGOVER:
  {
    PMYDRAGITEM pmdi = (PMYDRAGITEM)m1;
    int         nIt  = LBPos2Item(hWnd, ulGrpItHeight, SHORT2FROMMP(m2));
    return MRESULT(nIt!=LIT_NONE && (pmdi->mdt==MYDRAG_GROUPITEM || pmdi->mdt==MYDRAG_FILES));
  }
  case MDM_DRAGDROP:
  {
    PMYDRAGITEM pmdi    = (PMYDRAGITEM)m1;
    LONG        nItTarg = LBPos2Item(hWnd, ulGrpItHeight, SHORT2FROMMP(m2));
    LHANDLE     hTarg   = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nItTarg), 0);
    if (hTarg && pmdi->mdt==MYDRAG_GROUPITEM)
    {
      LHANDLE hSrc  = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(pmdi->ul),0);
      if (!hSrc)                             break;
      if (!pbcCurrent->MoveItem(hSrc,hTarg)) break;
      FillGroupList(hWnd,hSrc);
      return MRESULT(TRUE);
    }
    else if(pmdi->mdt==MYDRAG_FILES)
    {
      LHANDLE hNew = hTarg;
      for (PSZ p=pmdi->pszMalloced; p && *p; p += strlen(p)+1)
        if (!(hNew = pbcCurrent->AddItemFile(hNew,p))) break;
      free(pmdi->pszMalloced);
      pmdi->pszMalloced=0;
      FillGroupList(hWnd,hNew ? hNew : hTarg);
      WinSendMsg(hLBDir,WM_USER+1,0,0); // Unselect all
      return MRESULT(TRUE);
    }
    break;
  }
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
      case IDM_POP_FONT:
      {
        SetPPFont(hWnd,szGrpPPFont);
        set_lists_heights();
        break;
      }
      case IDM_POP_CLOSEWIN:
      {
        int     nIt = (int)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        LHANDLE h   = (LHANDLE)WinSendMsg(hLBGrp,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
        if (h && pbcCurrent->QueryItemUL(h))
        {
          char szQ[CCHMAXPATH+100];
          sprintf(szQ,"Close editor window '%s'?",pbcCurrent->QueryItemName(h,FALSE));
          if (MBID_YES != WinMessageBox(HWND_DESKTOP,hWnd,szQ,"Close window",0,MB_ICONQUESTION|MB_YESNO|MB_MOVEABLE))
            break;
        }
        if (h = pbcCurrent->DeleteItem(h))
          FillGroupList(hLBGrp, h);
        break;
      }
      case IDM_POP_NEWFILE:
      {
        if (pBufsConfig == pbcCurrent)
        {
          int     nItTarg = (int)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
          LHANDLE hTarg   = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nItTarg), 0);
          if (pbcCurrent->AddItemNoname(hTarg,-1,0))
              FillGroupList(hWnd,hTarg);
        }
        else
        {
          char      sz[CCHMAXPATH] = "";
          CPARAMSTR cps;
          BOOL      fOk = FALSE;
          LHANDLE   h;
          cps.psz1 =  "Add source file";
          cps.psz2 = "Name:";
          cps.psz3 = sz;
          cps.l1   = sizeof(sz);
          if (!WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
            break;
          if (!strpbrk(sz,"\\/:")) fOk = FindRedirFile(sz,sz);
          if (!fOk)
          {
            FName2Full(sz,sz,FALSE);
            fOk = FName2Full(sz,sz,TRUE);
          }
          if (!fOk)
            fOk = (MBID_YES==MessageBox(hWnd,"Add source file",MB_ICONQUESTION|MB_YESNO,"File '",sz,
                                        "' not found. Add it anyway?"));
          if (fOk && (h = pbcCurrent->AddItemFile(0,sz)))
            FillGroupList(hWnd,h);
        }
        break;
      }
      case IDM_POP_FULLNAMES:
      {
        fShowFullNames = !fShowFullNames;
        int     nIt = (int)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        LHANDLE h   = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nIt), 0);
        FillGroupList(hWnd,h);
        break;
      }
    }
    return 0;
  case WM_CHAR:
    if (!(SHORT1FROMMP(m1)&KC_KEYUP) && SHORT2FROMMP(m2)==VK_RIGHT)
    {
      WinSetFocus(HWND_DESKTOP,WinWindowFromID(WinQueryWindow(hWnd,QW_PARENT),IDD_LBDIRFILES));
      return 0;
    }
    else if (!(SHORT1FROMMP(m1)&KC_KEYUP) && SHORT2FROMMP(m2)==VK_DELETE)
    {
      WinSendMsg(hWnd,WM_COMMAND,MPFROM2SHORT(IDM_POP_CLOSEWIN,0),0);
      return 0;
    }
    break;
  } // switch
  return ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
}



//--------------------- D i r e c t o r y   l i s t ------------------------

MRESULT EXPENTRY wpDirList (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PFNWP pWindowProc;
  static ULONG ulDirShowMode = 0L; // Not used yet
  static LONG  xWire         = -1;
  static BOOL  fDrawLocked   = FALSE;

  switch(ulMsg)
  {
  case MM_START:
    pWindowProc = PFNWP(ULONG(m1));
    return 0;
  case LLM_SETDIRSHOWMODE:
    if (ULONG(m1) != ulDirShowMode)
    {
      ulDirShowMode = ULONG(m1);
      xWire         = -1;
      WinInvalidateRect(hWnd,0,0);
    }
    break;
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
    if (fDrawLocked) return MRESULT(TRUE);
    POWNERITEM pow       = POWNERITEM(m2);
    HPS        hps       = pow->hps;
    RECTL      rc        = pow->rclItem;
    LHANDLE    h         = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(pow -> idItem),0);
    if (!h) break;
    BOOL       fDir      = (pfdDirectory->QueryAttrs(h) & FILE_DIRECTORY);
    ULONG      clrB      = (pow->fsState) ? CLR_DARKCYAN : CLR_DARKBLUE;
    ULONG      clrF      = fDir           ? CLR_WHITE    : (pow->fsState) ? CLR_BLACK : CLR_CYAN;
    if (pfdDirectory->QuerySel(h)) clrF = CLR_YELLOW;
    WinFillRect(hps,&rc,clrB);
    GpiSetColor(hps,clrF);
    rc.xLeft +=2;
    rc.xRight --;
    if(xWire<0)
    {
      POINTL aptl[TXTBOX_COUNT];
      if (GpiQueryTextBox(hps, 9, "SUBDIR ", TXTBOX_COUNT,aptl))
        xWire = rc.xRight - (aptl[TXTBOX_TOPRIGHT].x+2);
        if (xWire<0) xWire = 0;
    }
    {
      char       sz   [CCHMAXPATH];
      POINTL     aptl [TXTBOX_COUNT];
      PSZ        psz;
      ULONG      dt        = DT_RIGHT | DT_VCENTER;
      RECTL      rclTxt    = rc;
      rclTxt.xRight        = rclTxt.xLeft+xWire;

      if(!(psz = pfdDirectory->QueryName(h,TRUE))) break;

      if (GpiQueryTextBox(hps, strlen(psz), psz, TXTBOX_COUNT,aptl))
      {
        ULONG xR = min(rclTxt.xRight, rclTxt.xLeft+aptl[TXTBOX_TOPRIGHT].x+4);
        pfdDirectory->SetNamePos( h, ULONG (MPFROM2SHORT(0, xR) ));
      }

      GpiSetBackMix(hps,BM_OVERPAINT);
      WinDrawText(hps,-1,psz,&rclTxt,clrF,clrB, DT_LEFT | DT_VCENTER);

      if (pfdDirectory->QueryAttrs(h) & FILE_DIRECTORY)
      {
        if (strcmp(psz,"..")) strcpy(sz,"SUBDIR");
        else                  strcpy(sz,"UP-DIR");
        dt = DT_CENTER | DT_VCENTER;
      }
      else  sprintf(sz,"%u",pfdDirectory->QuerySize(h));
      rclTxt.xLeft  = rclTxt.xLeft + xWire;
      rclTxt.xRight = rc.xRight;
      WinDrawText(hps,-1,sz,&rclTxt,clrF,clrB, dt);

      POINTL ptl = {rclTxt.xLeft-1,rclTxt.yTop};
      GpiSetCurrentPosition(hps,&ptl);
      ptl.y = rclTxt.yBottom; GpiLine(hps,&ptl);

    }
    pow->fsState = pow->fsStateOld = 0;
    return MRESULT(TRUE);
  }
  case WM_BUTTON1DOWN:
  {
    USHORT  fsflags = SHORT2FROMMP(m2);
    LONG    nIt     = LBPos2Item(hWnd, ulDirItHeight, SHORT2FROMMP(m1));
    LONG    nIt0    = (LONG)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    if (nIt==LIT_NONE || !(fsflags & (KC_SHIFT|KC_CTRL))) break;
    BOOL    fCtrl   = fsflags & KC_CTRL;
    if (fCtrl) nIt0 = nIt;
    BOOL    fQShow  = abs(nIt-nIt0) < 5;
    for (int i=min(nIt0,nIt); i<=max(nIt0,nIt); i++)
    {
      LHANDLE h = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(i),0);
      if (h)
      {
        pfdDirectory->SetSel(h,fCtrl ? !pfdDirectory->QuerySel(h) : TRUE);
        if (fQShow)
        {
          char sz[CCHMAXPATH+100]; // Redraw the item:
          WinSendMsg(hLBDir,LM_QUERYITEMTEXT,MPFROM2SHORT(i,sizeof(sz)),MPARAM(sz));
          WinSendMsg(hLBDir,LM_SETITEMTEXT,  MPARAM(i),                 MPARAM(sz));
        }
      }
    }
    if (!fQShow) WinInvalidateRect(hWnd,0,0);
    break;
  }
  case WM_USER+1: // Unselect all
  {
    BOOL fOff = FALSE;
    for (LHANDLE hEnum = pfdDirectory->QueryFirst(); hEnum; hEnum = pfdDirectory->QueryNext())
      if (pfdDirectory->QuerySel(hEnum) && !(pfdDirectory->QueryAttrs(hEnum) & FILE_DIRECTORY))
      {
        fOff = TRUE;
        pfdDirectory->SetSel(hEnum,FALSE);
      }
    if (fOff) WinInvalidateRect(hWnd,0,0);
    break;
  }
  case WM_USER:
  // In:  m1    - current item; m2 - указатель на MUDRAGITEM для заполнения
  // Out: 0/1/2 - failed/m1 item/selected files
  {
    PSZ               psz  = 0;
    LONG              lsz  = 0;
    LHANDLE           h    = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,m1,0);
    PMYDRAGITEM       pmdi = (PMYDRAGITEM)m2;
    if (!h) return 0;
    {
      LHANDLE hEnum;
      for (hEnum = pfdDirectory->QueryFirst(); hEnum; hEnum = pfdDirectory->QueryNext())
        if (pfdDirectory->QuerySel(hEnum) && !(pfdDirectory->QueryAttrs(hEnum) & FILE_DIRECTORY))
        {
          PSZ pszN = pfdDirectory->QueryName(hEnum, FALSE);
          if (!pszN) continue;
          LONG lsz1 = lsz + strlen(pszN)+1;
          if (!(psz = (PSZ)realloc(psz,lsz1+1))) return 0;
          strcpy(psz+lsz,pszN);
          psz[lsz1] = 0;
          lsz       = lsz1;
        }
    }
    if (!lsz)
    {
      PSZ  pszN = pfdDirectory->QueryName(h,FALSE);
      LONG l;
      if (!pszN ||
           (pfdDirectory->QueryAttrs(h) & FILE_DIRECTORY) ||
          (l=strlen(pszN), !(psz = (PSZ)malloc(l+2)))) return 0;
      strcpy(psz,pszN);
      psz[l+1] = 0;
    }
    memset(pmdi,0,sizeof(*pmdi));
    pmdi->mdt          = MYDRAG_FILES;
    pmdi->ul           = ULONG(m1);
    pmdi->pszMalloced  = psz;
    return MRESULT(lsz ? 2 : 1);
  }
  case WM_BUTTON2DOWN:
  {
    LONG       nIt =  LBPos2Item(hWnd, ulDirItHeight, SHORT2FROMMP(m1));
    if        (nIt == LIT_NONE) break;
    LHANDLE    h = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
    if        (!h)              break;
    MYDRAGITEM mdi;
    int        rc = (int)WinSendMsg(hWnd,WM_USER,MPARAM(nIt),MPARAM(&mdi));
    if (rc==1)
    {
      WinSendMsg(hWnd, LM_SELECTITEM, MPARAM(nIt), MPARAM(TRUE));
      StartDrag(hWnd,&mdi,hptrDropFile,hptrNoDropFile);
    }
    else if (rc==2)
      StartDrag(hWnd,&mdi,hptrDropFiles,hptrNoDropFiles);
    break;
  }
  case WM_BUTTON2UP:
  {
    if (WinQueryFocus(HWND_DESKTOP)!=hWnd) WinSetFocus(HWND_DESKTOP,hWnd);
    LONG aChk[2] = {0, 0};
    aChk[0]      = (fdSort==FDSORT_EXT) ? IDM_POP_TYPE : IDM_POP_NAME;
    PlayMenu(hWnd, IDM_POPUP_FILELIST, aChk, 0, 0);
    return 0;
  }
  case MDM_DRAGOVER:
  {
    PMYDRAGITEM pmdi = (PMYDRAGITEM)m1;
    if (pmdi->mdt!=MYDRAG_GROUPITEM)            return MRESULT(FALSE);
    LHANDLE     hSrc = (LHANDLE)WinSendMsg(hLBGrp,LM_QUERYITEMHANDLE,MPARAM(pmdi->ul),0);
    if (!hSrc || !pbcCurrent->QueryItemUL(hSrc))  return MRESULT(FALSE);
    return MRESULT(TRUE);
  }
  case MDM_DRAGDROP:
  {
    PMYDRAGITEM pmdi = (PMYDRAGITEM)m1;
    if (pmdi->mdt!=MYDRAG_GROUPITEM)     return MRESULT(FALSE);
    LHANDLE     hSrc = (LHANDLE)WinSendMsg(hLBGrp,LM_QUERYITEMHANDLE,MPARAM(pmdi->ul),0);
    PFILEED     pFE  = hSrc ? (PFILEED)pbcCurrent->QueryItemUL(hSrc) : 0;
    if (!hSrc || !pFE)                   return MRESULT(FALSE);
    if (pFE->SaveFile(hWnd, TRUE, szShownDir))
    {
      pbcCurrent->SetItemName(hSrc,pFE->pszFilename);
      FillGroupList(hLBGrp,hSrc);
      return MRESULT(TRUE);
    }
    return MRESULT(FALSE);
  }
  case WM_FOCUSCHANGE:
      pfdDirectory->FastFind(0);
      break;
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
      case IDM_POP_FONT:
      {
        SetPPFont(hWnd,szDirPPFont);
        set_lists_heights();
        break;
      }
      case IDM_POP_NAME:
      case IDM_POP_TYPE:
      {
        LONG    nIt =  (LONG)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        LHANDLE h   = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
        char    szSel[CCHMAXPATH] = "";
        if (h) strcpy(szSel, pfdDirectory->QueryName(h,TRUE));
        pfdDirectory->Sort((SHORT1FROMMP(m1)==IDM_POP_NAME) ? FDSORT_NAME : FDSORT_EXT);
        FillDirList(hLBDir,0L,szSel);
        break;
      }
      case IDM_POP_UNSELALL:
      {
        for (int i=(int)WinSendMsg(hWnd,LM_QUERYITEMCOUNT,0,0)-1; i>=0; i--)
        {
          LHANDLE h   = (LHANDLE)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(i),0);
          pfdDirectory->SetSel(h,FALSE);
        }
        WinInvalidateRect(hWnd,0,0);
        break;
      }
    }
    return 0;
  case WM_CHAR:
  {
    USHORT fsflags = SHORT1FROMMP(m1);
    CHAR   ch      = SHORT1FROMMP(m2);
    CHAR   vk      = SHORT2FROMMP(m2);
    if (fsflags & KC_KEYUP) break;
    if ((vk==VK_UP || vk==VK_DOWN) &&
        pfdDirectory->FastFindNext(vk==VK_UP))
      return 0;
    if ((fsflags & (KC_CTRL|KC_ALT|KC_CHAR|KC_VIRTUALKEY))==KC_CHAR)
    {
      pfdDirectory->FastFind(ch);
      break;
    }
    else if (vk!=VK_SHIFT)
    {
      BOOL f = pfdDirectory->FastFind(0);
      if (f && vk==VK_ESC) return 0; // don't close dialog
    }
    if (SHORT1FROMMP(m2)>='0' && SHORT1FROMMP(m2)<='9' && fsflags & KC_ALT)
    {// Try to load file(s) to this group
      LHANDLE    hGrp, hSel=0;              // Хэндл раздела листа перед которым будем вставлять файл(ы)
      int        ng = SHORT1FROMMP(m2)-'0'; // Искомая группа
      for(hGrp=pbcCurrent->StartEnum(); hGrp; hGrp=pbcCurrent->NextEnum(FALSE))
        if (pbcCurrent->QueryItemType(hGrp)==BFITEM_GROUP && ng==pbcCurrent->QueryItemNum(hGrp)) break;
      if(!hGrp) {Beep(); break;}            // Группа не найдена

      MYDRAGITEM mdi;
      LONG       nIt = (LONG)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      if (nIt==LIT_NONE || !WinSendMsg(hWnd,WM_USER,MPARAM(nIt),MPARAM(&mdi))) break;
      for (PSZ p=mdi.pszMalloced; p && *p; p += strlen(p)+1)
        if (!(hSel=pbcCurrent->AddItemFile(hGrp,p))) break;
      FillGroupList(hLBGrp,hSel);
      WinSendMsg(hWnd,WM_USER+1,0,0);
      break;
    }
    else if (fsflags & KC_CTRL &&
        (vk==VK_PAGEUP || vk==VK_PAGEDOWN))
    {
      WinSendMsg(WinQueryWindow(hWnd,QW_OWNER),WM_CONTROL,
                 MPFROM2SHORT(WinQueryWindowUShort(hWnd,QWS_ID),LDN_DIRUP),
                 MPARAM(hWnd));
      return 0;
    }
    else if (vk==VK_LEFT)
    {
      WinSetFocus(HWND_DESKTOP,WinWindowFromID(WinQueryWindow(hWnd,QW_PARENT),IDD_LBFILESLOADED));
      return 0;
    }
    break;
  } // WM_CHAR
  } // switch
  return ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
}

LONG LBPos2Item(HWND hLB, ULONG ulItemHeight, LONG yPos)
{
  SWP  swp;
  WinQueryWindowPos(hLB,&swp);
  if (!ulItemHeight) return LIT_NONE;
  LONG lin = (swp.cy-yPos-3)/ulItemHeight + (LONG)WinSendMsg(hLB,LM_QUERYTOPINDEX,0,0);
  if (lin<0 || lin>=(LONG)WinSendMsg(hLB,LM_QUERYITEMCOUNT,0,0))
    return LIT_NONE;
  return lin;
}

void Win2WinCoord(HWND hwSource, HWND hwTarget, PPOINTL pptl)
// Converts pptl related to hwSource window to hwTarget's window coordinates
{
  SWP    swp;
  HWND   wnd, wnd0;
  LONG   dx=0;
  LONG   dy=0;
  for (wnd=hwSource,wnd0=0;   wnd && wnd0!=wnd;   wnd=WinQueryWindow( (wnd0=wnd), QW_PARENT))
  {
    WinQueryWindowPos(wnd,&swp);
    dx  += swp.x;
    dy  += swp.y;
  }
  for (wnd=hwTarget,wnd0=0;   wnd && wnd0!=wnd;   wnd=WinQueryWindow( (wnd0=wnd), QW_PARENT))
  {
    WinQueryWindowPos(wnd,&swp);
    dx -= swp.x;
    dy -= swp.y;
  }
  pptl->x += dx;
  pptl->y += dy;
}



//=============================== Fast file load dialog ================================================//

void FastLoadDlg()
{
  MRESULT EXPENTRY dpFastLoadDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2);
  if (!WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpFastLoadDlg,NULLHANDLE,IDD_FASTLOADDLG,0))
    FileLoadDlg();
}

MRESULT EXPENTRY dpFastLoadDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  int    fast_open(HWND hWnd, PSZ szFile, PSZ szDir, ULONG ulArea);
  static HWND hCBFile;
  static HWND hCBWin;
  static char szDir[CCHMAXPATH];
  static char aszWins[][32] =
  {
    "<Current window>",     // This
    "New in current group",  //   order
    "New in 'Auto' group",   //
    "New in 1 group",        //       ...!
    "New in 2 group",        //
    "New in 3 group",
    "New in 4 group",
    "New in 5 group",
    "New in 6 group",
    "New in 7 group",
    "New in 8 group",
    "New in 9 group",
    ""
  };

  switch(ulMsg){
  case WM_INITDLG:
  {
    PSZ   psz;
    int   i;
    hCBFile = WinWindowFromID(hWnd,IDD_CBFILELOAD);
    hCBWin  = WinWindowFromID(hWnd,IDD_CBWINLOAD);
    WinSendMsg(hCBFile,EM_SETTEXTLIMIT,MPARAM(CCHMAXPATH-1),0);
    for (psz=hstFiles.Enum(); psz; psz=hstFiles.NextEnum())
      WinSendMsg(hCBFile,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(psz));
    for (i=0; aszWins[i][0]; i++)
      WinSendMsg(hCBWin,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(aszWins[i]));
    i = grOffice.QueryActiveFEd() ? 1 : 3;
    WinSendMsg(hCBWin,LM_SELECTITEM,MPARAM(i),MPARAM(TRUE));
    strcpy(szDir,szCurIOPath);
    WinSendMsg(hWnd,WM_USER,0,0);
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch(SHORT1FROMMP(m1))
    {
      case DID_CANCEL:
        WinDismissDlg(hWnd,1);
        break;
      case DID_OK:
      {
        char sz[CCHMAXPATH];
        WinQueryWindowText(hCBFile,sizeof(sz),sz);
        if (sz[0] && !strchr(sz,'*') && !strchr(sz,'?'))
        {
          ULONG ulArea = (int)WinSendMsg(hCBWin,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
          int   rc     = fast_open(hWnd,sz,szDir,ulArea);
          if      (rc==2) WinDismissDlg(hWnd,1); // File opened
          else if (rc==1)
          { // Dir. changed
            WinSendMsg(hWnd,WM_USER,0,0);
            WinSetWindowText(hCBFile,"");
            WinSetFocus(HWND_DESKTOP,hCBFile);
          }
          WinSetFocus(HWND_DESKTOP,hCBFile);
          break;
        }
        // no 'break'
      }
      case IDD_PBBROWSE:
      {
        char szF[CCHMAXPATH];
        WinQueryWindowText(hCBFile,sizeof(szF),szF);
        FName2Full(szF,szF,FALSE,szDir);
        if (Browse(szF,szF,hWnd,"Load",FALSE))
          WinSetWindowText(hCBFile,szF);
        WinSetFocus(HWND_DESKTOP,hCBFile);
        break;
      }
      case IDD_PBADVLOAD:
        WinDismissDlg(hWnd,0);
        break;
      case IDD_PBRESETPATH:
        strcpy(szCurIOPath,szCurPath);
        strcpy(szDir,      szCurPath);
        WinSendMsg(hWnd,WM_USER,0,0);
        break;
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_FASTLOADDLG);
        break;
    } // switch(SHORT1FROMMP(m1))
    break;
  case WM_DESTROY:
    WinUpdateWindow(hMDI0Client);
    break;
  case WM_USER: // Show new path
  {
    char sz[CCHMAXPATH+2];
    sprintf(sz,"%s\\",szDir);
    WinSetDlgItemText(hWnd,IDD_EFFILEPATH,sz);
    break;
  }
  case WM_CHAR:
    {
      USHORT fsflags = SHORT1FROMMP(m1);
      CHAR   ch      = SHORT1FROMMP(m2);
      if (!(fsflags & KC_KEYUP))
      {
        if (SHORT2FROMMP(m2)==VK_F1)
        {
          DisplayHelpPanel(PANEL_FASTLOADDLG);
          return 0;
        }
        else if (SHORT2FROMMP(m2)==VK_F3)
        {
          WinPostMsg(hWnd,WM_COMMAND,MPARAM(IDD_PBADVLOAD),0);
          return 0;
        }
        else if (ch>='0' && ch<='9' && fsflags & KC_ALT)
        {
          WinSendMsg(hCBWin,LM_SELECTITEM,MPARAM(2+ch-'0'),MPARAM(TRUE));
          WinPostMsg(hWnd,WM_COMMAND,MPARAM(DID_OK),0);
          return 0;
        }
      }
      // no break here
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

int fast_open(HWND hWnd, PSZ szFile, PSZ szDir, ULONG ulArea)
//
// hWnd   (in)     - use as a parent
// szFile (in)     - filename (entry field contents)
// szDir  (in/out) - current directory
// ulArea (in)     - area to open in
//
// Return: 0/1/2 == error / directory changed / file opened
//
// Note: szDir - текущий каталог (обычно - szCurIOPath). Если szFile задает не файл, а путь -
//       обновляется szDir и возвращается FALSE, диалог продолжается

{
  try
  {
    char szFull[CCHMAXPATH] = "";
    // Find file:
    if (!strchr(szFile,':') && !strchr(szFile,'\\') && !strchr(szFile,'/'))
      FindRedirFile(szFull,szFile);
    else
      if (!FName2Full(szFull, szFile, TRUE, szDir)) szFull[0] = '\0';
    if (szFull[0])
    { // Found. Open it now.
      PFILEED pFE = grOffice.QueryActiveFEd();
      if (!ulArea     && !pFE)
        throw "There is no CURRENT editor window to load";
      if (ulArea == 1 && !pFE)
        throw "There is no CURRENT group to load";
      ULONG nGroup = (ulArea>1) ? ulArea-2 : pFE->pFGroup->nGroup;
      if (nGroup >= MAXFGROUPS) throw "Non fatal internal error occured while opening file ";
      return grOffice.OpenEditor(nGroup, szFull, hWnd, 0, ulArea ? 0 : pFE) ? 2 : 0;
    }
    // Not found. Is it a directory?
    else
    {
      int         rc;
      FILESTATUS3 fs3;
      if (!FName2Full(szFull, szFile, FALSE, szDir)) throw "Error in the file name ";
      if (rc = (int)DosQueryPathInfo(szFull,FIL_STANDARD,&fs3,sizeof(fs3)))
        IOErMsg(rc, hWnd, "Load file", "Can't open file or path", szFull, MB_OK);
      else if (fs3.attrFile & FILE_DIRECTORY)
      {
        strcpy(szDir,szFull);
        return 1;
      }
      else throw "Can not open file ";
    }
    return 0;
  }
  catch (char *sz)
  {
    if (sz[0])
    {
      char szE[CCHMAXPATH+100];
      sprintf(szE,"%s: '%s'.",sz,szFile);
      WinMessageBox(HWND_DESKTOP,hWnd,szE,"Load file",0,MB_INFORMATION|MB_OK|MB_MOVEABLE);
    }
    return 0;
  }
}





/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                 D R A G G I N G   realisation                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*--- Drag globals
*/
MYDRAGITEM myDragItem;     // Dragging now
HWND       hDragger = 0;   // Our internal window
HWND       hwndSource;     // Initiator
HPOINTER   hptrSaved;      // Saved pointer
BOOL       fDropPtr;       // Dragging pointer DROP now?
HPOINTER   hptrDrop;       // Dragging pointer DROP
HPOINTER   hptrNoDrop;     // Dragging pointer NODROP

/*--- Drag prptotypes
*/
BOOL             PointOurWin(HWND hWnd, MPARAM m1, PHWND phwnd, PPOINTL pptl);
MRESULT EXPENTRY wpDragger(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2);


//--- StartDrag
//
// It starts dragging operation and returns immediatly.
//
// HWND        hwndSource (in) - window to notify when operation terminated (or HWND_DESKTOP)
// PMYDRAGITEM pmdi (in)       - the item been dragged (StartDrag creates its own copy of it)
// HPOINTER    hptrDrop (in)   - pointer to show when dropping possible
// HPOINTER    hptrNoDrop (in) - pointer to show when dropping impossible
//
// Return value (BOOL): TRUE when error occured.
//
// Remarks:
//
// MDM_DRAGOVER(PMYDRAGITEM pmdi, MPARAM xy) - sended (like WM_MOUSMOVE) to the window under
//      the pointer. When the window returns TRUE the hptrDrop pointer is showed.
//      PMYDRAGITEM  pmdi
//      MPARAM       xy    - pointer coordinates (related to this window)
//
// MDM_DRAGDROP(PMYDRAGITEM, MPARAM xy) - sended to the pointed window when the dropping occured.
//      The window returns TRUE when the drop is success.
//      PMYDRAGITEM  pmdi
//      MPARAM       xy    - pointer coordinates (related to this window)
//
// MDM_DRAGEND (BOOL fOk, HWND hTarg) - sends to the hSource window when the drag ends.
//      BOOL         fOk   - success state (returned on the MDM_DRAGDROP message)
//      HWND         hTarg - window the MDM_DRAGDROP message was sent to (or 0)
//

BOOL StartDrag(HWND hwndSource, PMYDRAGITEM pmdi, HPOINTER hptrDrop, HPOINTER hptrNoDrop)
{
  static BOOL fFirstCall = TRUE;
  if (fFirstCall)
  {
    if (!WinRegisterClass(hAB,"MYDRAGGER",(PFNWP)wpDragger,0,0L)) return TRUE;
    fFirstCall = FALSE;
  }
  if ( ::hDragger) WinDestroyWindow(hDragger);
  ::hDragger    = WinCreateWindow(hwndSource,"MYDRAGGER",0,0, 0,0,1,1, 0,HWND_TOP,0,0,0);
  if (!::hDragger) return TRUE;
  ::myDragItem = *pmdi;
  ::hwndSource = hwndSource;
  ::hptrSaved  = WinQueryPointer(HWND_DESKTOP);
  ::fDropPtr   = FALSE;
  ::hptrDrop   = hptrDrop;
  ::hptrNoDrop = hptrNoDrop;
}


BOOL PointOurWin(HWND hWnd, MPARAM m1, PHWND phwnd, PPOINTL pptl)
// m1 - coordinates in hWnd
// If it points to our window - it will be set in phwnd,
//                              it's coordinates in pptl,
//                              returns TRUE
// else returns FALSE
{
  POINTL ptl  = {SHORT1FROMMP(m1),SHORT2FROMMP(m1)};
  Win2WinCoord(hWnd,HWND_DESKTOP,&ptl);
  HWND   hwnd = WinWindowFromPoint(HWND_DESKTOP,&ptl,TRUE);
  PID    p1,p2;
  TID    t1,t2;
  if (hwnd                                &&
      WinQueryWindowProcess(hWnd,&p1,&t1) &&
      WinQueryWindowProcess(hwnd,&p2,&t2) &&
      p1 == p2)
  {// Our process.
    Win2WinCoord(HWND_DESKTOP,hwnd,&ptl);
    *pptl = ptl;
    *phwnd = hwnd;
    return TRUE;
  }
  return FALSE;
}


MRESULT EXPENTRY wpDragger(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static int nDragStart;
  switch(ulMsg)
  {
    case WM_CREATE:
      nDragStart = 3;
      WinDefWindowProc(hWnd,ulMsg,m1,m2);
      if (!WinSetCapture(HWND_DESKTOP,hWnd)) return MRESULT(TRUE); // Oops!
      if (!(0x8000&WinGetKeyState(HWND_DESKTOP,VK_BUTTON2)))
      {
        WinSetCapture(HWND_DESKTOP,0);
        return MRESULT(TRUE);                                      // Oops!
      }
      break;
    case WM_MOUSEMOVE:
    {
      POINTL ptl;
      HWND   hwPtl;
      BOOL   fDropPtr = FALSE;
      if (PointOurWin(hWnd,m1,&hwPtl,&ptl))
      {
        MPARAM xy  = MPFROM2SHORT(ptl.x,ptl.y);
        fDropPtr   = (BOOL)WinSendMsg(hwPtl,MDM_DRAGOVER,MPARAM(&myDragItem),xy);
      }
      if (!::fDropPtr != !fDropPtr || nDragStart)
      {
        if (nDragStart) nDragStart--;
        ::fDropPtr = fDropPtr;
        WinSetPointer(HWND_DESKTOP, (fDropPtr ? hptrDrop : hptrNoDrop));
      }
      break;
    }
    case WM_BUTTON2UP:
    {
      POINTL ptl;
      HWND   hwPtl    = 0;
      BOOL   fDropped = FALSE;
      if (PointOurWin(hWnd,m1,&hwPtl,&ptl))
      {
        MPARAM xy = MPFROM2SHORT(ptl.x,ptl.y);
        if (!nDragStart)
          fDropped  = (BOOL)WinSendMsg(hwPtl,MDM_DRAGDROP,MPARAM(&myDragItem),xy);
        else
          WinSendMsg(hwPtl,WM_BUTTON2UP,xy,m2);
      }
      WinSendMsg(::hwndSource,MDM_DRAGEND,MPARAM(fDropped),MPARAM(hwPtl));
      WinSendMsg(hWnd,WM_USER,0,0);
      break;
    }
    case WM_CHAR:
      WinSendMsg(::hwndSource,MDM_DRAGEND,MPARAM(FALSE),MPARAM(0));
    case WM_USER: // Cancel drag
      WinSetCapture(HWND_DESKTOP,0);
      WinSetPointer(HWND_DESKTOP,hptrSaved);
      free(myDragItem.pszMalloced);
      myDragItem.pszMalloced = 0;
      WinDestroyWindow(hWnd);
      break;
    case WM_CLOSE:
      WinSendMsg(hWnd,WM_USER,0,0);
    default:
      return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

/*-------------------------------------------------------------------------*/
/*             Useful functions                                            */
/*-------------------------------------------------------------------------*/

void CenterChildWindow(HWND hWnd)
{
  SWP swpWnd, swpParent;
  WinQueryWindowPos(hWnd,&swpWnd);
  WinQueryWindowPos(WinQueryWindow(hWnd,QW_PARENT),&swpParent);
  LONG xNew = swpParent.cx/2 - swpWnd.cx/2;
  LONG yNew = swpParent.cy/2 - swpWnd.cy/2;
  WinSetWindowPos(hWnd,0,xNew,yNew,0,0,SWP_MOVE);
}

BOOL CreateNewDir (PSZ pszTarg, PSZ pszSrc)
{
  int       rc,i;
  char      sz    [CCHMAXPATH];
  char      szRes [CCHMAXPATH];

  if (!FName2Full(szRes,pszSrc,FALSE)) return FALSE;
  for(i=strlen(szRes)-1; i>=0 && (szRes[i]==' '|| szRes[i]=='\\'); i--) szRes[i]=0;

  // Let szRes = "X:\dir1\dir2\...\dir<dirs> THEN
  // FOR level=0 TO dirs DO
  //   CreateDir(X:\dir1\...\dir<level>)
  // END;

  if (pszTarg) strcpy(pszTarg,szRes);
  strcat(szRes,"\\");

  for (i=0; szRes[i]; i++)
  {
    sz[i] = szRes[i];
    if (sz[i]=='\\' && i>2)
    {
      sz[i] = '\0';
      rc = (int)DosCreateDir(sz,0);
      if (rc && rc!=5 /* ERROR_ACCESS_DENIED */) return FALSE;
      sz[i] = '\\';
    }
  }
  return TRUE;
}

void QueryCurrentPath(char *szTarg)
{
  // Returns path w/o last '\'
  ULONG ul,ul1;
  DosQueryCurrentDisk(&ul,&ul1);
  sprintf(szTarg,"%c:\\",'A'-1+ul);
  ul = CCHMAXPATH-3;
  DosQueryCurrentDir(0,szTarg+3,&ul);
  if (!szTarg[3]) szTarg[2] = '\0'; // Home dir w/o '\' (like "e:")
}



/*-------------------------------------------------------------------------*/
/*                                                                         */
/*             B U F C O N F I G   class realisation                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/

BOOL BUFSCONFIG::afGClosed[MAXFGROUPS];

BUFSCONFIG::BUFSCONFIG (BOOL fGroups) {memset(this,0,sizeof(*this)); this->fGroups = fGroups;}

BUFSCONFIG::BUFINFO::BUFINFO()
{
  memset(this,0,sizeof(*this));
}
BUFSCONFIG::BUFINFO::~BUFINFO()
{
  free(pszName);
  free(pszShortName);
}

BUFSCONFIG::DELITEM::DELITEM(DELITEM **ppdi, PFILEED pFE)
{
  while(*ppdi) ppdi = &(*ppdi)->next;
  *ppdi  = this;
  next   = 0;
  pFEDel = pFE;
}

BUFSCONFIG::PBUFINFO* BUFSCONFIG::seek(LHANDLE h)
{
  PBUFINFO* ppbi = &pbList;
  while(*ppbi && *ppbi!=(PBUFINFO)h) ppbi = &((*ppbi)->next);
  return ppbi;
}

LHANDLE BUFSCONFIG::AddItemNoname(LHANDLE hAfter, LONG lNonNum, PSZ pszName)
{
  PSZ    psz,pszS;
  int    len      = (pszName ? strlen(pszName)+1 : 15);
  if (lNonNum<0)
    lNonNum = GROUPOFFICE::GenerateNonNum();
  if (!(psz=(PSZ)malloc(len)) || !(pszS=(PSZ)malloc(len)))
  {
    free(psz);
    return 0;
  }
  if (!pszName) sprintf(psz,"Noname%u",lNonNum);
  else          strcpy (psz,pszName);
  strcpy (pszS,psz);
  PBUFINFO pbi      = new BUFINFO;
  if (!pbi) return 0;
  pbi->bfitemt      = BFITEM_NONAME;
  pbi->nNum         = lNonNum;
  pbi->pszName      = psz;
  pbi->pszShortName = pszS;
  PBUFINFO *ppbi = seek(hAfter);
  if (*ppbi)
  {
    pbi->next       = (*ppbi)->next;
    (*ppbi)->next   = pbi;
  }
  else
    *ppbi           = pbi;
  return LHANDLE(pbi);
}
LHANDLE BUFSCONFIG::AddItemGroup (ULONG nGrp)
{
  PSZ    psz,pszS;
  if (!(psz=(PSZ)malloc(15)) || !(pszS=(PSZ)malloc(15)))
  {
    free(psz);
    return 0;
  }
  if (!nGrp) strcpy (psz,fGroups ? "Auto group:" : "Source files:");
  else       sprintf(psz,  "%u:",nGrp);
  strcpy (pszS, psz);
  PBUFINFO pbi      = new BUFINFO;
  if (!pbi) return 0;
  pbi->bfitemt      = BFITEM_GROUP;
  pbi->nNum         = nGrp;
  pbi->pszName      = psz;
  pbi->pszShortName = pszS;
  if (pbi->nNum < MAXFGROUPS)
    pbi->fGroupClosed = afGClosed[pbi->nNum];
  PBUFINFO *ppbi = seek(0);
  *ppbi     = pbi;
  return LHANDLE(pbi);
}
LHANDLE BUFSCONFIG::AddItemFile  (LHANDLE hAfter, PSZ szName)
// Для sourcelist - упорядочивает по алфавиту.
{
  PBUFINFO *ppbi;
  if (fGroups)
    ppbi = seek(hAfter);
  else
  {
    for (ppbi = &pbList; *ppbi; ppbi = &((*ppbi)->next))
      if ((*ppbi)->bfitemt != BFITEM_GROUP)
      {
        int cmp = sf_stricmp((*ppbi)->pszName,szName);
        if (!cmp)  return LHANDLE(*ppbi);
        if (cmp>0) break;
      }
  }
  PSZ    psz,pszS;
  if (!(psz=(PSZ)malloc(strlen(szName)+1))) return 0;
  strcpy(psz,szName);
  {
    char *pch = sf_fname2short(psz);
    if (!(pszS=(PSZ)malloc(strlen(pch)+1))) {free(psz); return 0;}
    strcpy(pszS,pch);
  }
  PBUFINFO pbi      = new BUFINFO;
  if (!pbi) return 0;
  pbi->bfitemt      = BFITEM_FILE;
  pbi->pszName      = psz;
  pbi->pszShortName = pszS;
  if (*ppbi)
  {
    pbi->next       = (*ppbi)->next;
    (*ppbi)->next   = pbi;
  }
  else
    *ppbi           = pbi;
  return LHANDLE(pbi);
}
BOOL BUFSCONFIG::MoveItem(LHANDLE h, LHANDLE hAfter)
{
  PBUFINFO *ppbiSrc  = seek(h);
  PBUFINFO *ppbiTarg = seek(hAfter);
  PBUFINFO  pbiH     = PBUFINFO(h);
  PBUFINFO  pbiT     = *ppbiTarg;
  if (!h || !*ppbiSrc || !*ppbiTarg || (hAfter == h)) return TRUE;
  *ppbiSrc           = pbiH->next;
  pbiH->next         = pbiT->next;
  pbiT->next         = pbiH;
  return TRUE;
}
LHANDLE BUFSCONFIG::DeleteItem(LHANDLE h)
{
  PBUFINFO  pbRet = 0;
  PBUFINFO *ppbi  = seek(h);
  PBUFINFO  pbi   = *ppbi;
  if (!pbi || pbi!=(PBUFINFO)h || pbi->bfitemt==BFITEM_GROUP)
    return (LHANDLE)pbRet;
  pbRet = *ppbi = pbi->next;
  new DELITEM(&pDelItems,PFILEED(pbi->ulUser));
  delete pbi;
  if (!pbRet) // get a last h
    for (pbRet=pbList; pbRet->next; pbRet=pbRet->next);
  return (LHANDLE)pbRet;
}
BFITEMT BUFSCONFIG::QueryItemType(LHANDLE h)
{
  PBUFINFO *ppbi = seek(h);
  PBUFINFO  pbi  = *ppbi;
  if (!pbi || pbi!=(PBUFINFO)h) return BFITEM_WRONG;
  return pbi->bfitemt;
}
PSZ BUFSCONFIG::QueryItemName(LHANDLE h, BOOL fShort)
{
  PBUFINFO *ppbi = seek(h);
  PBUFINFO  pbi  = *ppbi;
  if (!pbi || pbi!=(PBUFINFO)h) return "";
  return fShort ? pbi->pszShortName : pbi->pszName;
}
ULONG BUFSCONFIG::QueryItemNamePos(LHANDLE h)
{
  PBUFINFO *ppbi = seek(h);
  PBUFINFO  pbi  = *ppbi;
  return (!pbi || pbi!=(PBUFINFO)h) ? 0 : pbi->ulNamePos;
}
int BUFSCONFIG::QueryItemNum (LHANDLE h)
{
  PBUFINFO *ppbi = seek(h);
  PBUFINFO  pbi  = *ppbi;
  if (!pbi || pbi!=(PBUFINFO)h) return -1;
  return pbi->nNum;
}
LHANDLE BUFSCONFIG::QVisibleParent(LHANDLE h)
{
  PBUFINFO pbiGrp   = 0;
  BOOL     fHidden  = FALSE;
  for (PBUFINFO pbi = pbList; pbi; pbi=pbi->next)
  {
    if (pbi == (PBUFINFO)h) return fHidden ? LHANDLE(pbiGrp) : h;
    if (pbi->bfitemt==BFITEM_GROUP)
    {
      pbiGrp  = pbi;
      fHidden = pbi->fGroupClosed;
    }
  }
  return 0;
}
void    BUFSCONFIG::SetItemUL    (LHANDLE h, ULONG ul)
{
  PBUFINFO *ppbi = seek(h);
  PBUFINFO  pbi  = *ppbi;
  if (!pbi || pbi!=(PBUFINFO)h) return;
  pbi->ulUser = ul;
}
void    BUFSCONFIG::SetItemName  (LHANDLE h, PSZ pszName)
{
  PBUFINFO *ppbi = seek(h);
  PBUFINFO  pbi  = *ppbi;
  if (!pbi || pbi!=(PBUFINFO)h || (pbi->bfitemt!=BFITEM_FILE && pbi->bfitemt!=BFITEM_NONAME)
      || !pszName)
    return;
  PSZ    psz,pszS;
  if (!(psz=(PSZ)malloc(strlen(pszName)+1))) return;
  strcpy(psz,pszName);
  {
    char *pch = sf_fname2short(psz);
    if (!(pszS=(PSZ)malloc(strlen(pch)+1))) {free(psz); return;}
    strcpy(pszS,pch);
  }
  free(pbi->pszName);      pbi->pszName      = psz;
  free(pbi->pszShortName); pbi->pszShortName = pszS;
}
void    BUFSCONFIG::SetItemNamePos  (LHANDLE h, ULONG ul)
{
  PBUFINFO *ppbi = seek(h);
  PBUFINFO  pbi  = *ppbi;
  if (pbi && pbi==(PBUFINFO)h)  pbi->ulNamePos = ul;
}
ULONG   BUFSCONFIG::QueryItemUL  (LHANDLE h)
{
  PBUFINFO *ppbi = seek(h);
  PBUFINFO  pbi  = *ppbi;
  if (!pbi || pbi!=(PBUFINFO)h) return 0;
  return pbi->ulUser;
}
BOOL BUFSCONFIG::CloseGroup(LHANDLE h)
{
  BOOL fClosed1 = 0;
  if (h)
  {
    fClosed1 = PBUFINFO(h)->fGroupClosed = !(PBUFINFO(h)->fGroupClosed);
    ULONG ng = PBUFINFO(h)->nNum;
    if (ng<MAXFGROUPS) afGClosed[ng] = fClosed1;
  }
  return fClosed1;
}
BOOL BUFSCONFIG::QueryClosed(LHANDLE h)
{
  return (h) ? PBUFINFO(h)->fGroupClosed : FALSE;
}
BOOL BUFSCONFIG::MarkFileActive(LHANDLE h)
{
  PBUFINFO *ppbi = seek(h);
  PBUFINFO  pbi  = *ppbi;
  if (!pbi || pbi!=(PBUFINFO)h || (pbi->bfitemt!=BFITEM_FILE && pbi->bfitemt!=BFITEM_NONAME))
    return FALSE;
  hActivateMe = h;
  return TRUE;
}
LHANDLE BUFSCONFIG::StartEnum()
{
  return (LHANDLE)(pbiEnum = pbList);
}
LHANDLE BUFSCONFIG::NextEnum(BOOL fPeekClosed)
{
  PBUFINFO pbi = pbiEnum;
  if (pbi && pbi->bfitemt==BFITEM_GROUP && pbi->fGroupClosed && !fPeekClosed)
    while((pbi=pbi->next) && pbi->bfitemt!=BFITEM_GROUP);
  else
    pbi=pbi->next;
  return (LHANDLE)(pbiEnum=pbi);
}
BOOL BUFSCONFIG::AutoBuild()
{
  LHANDLE lh;
  Free();
  if (fGroups)
  {
    for (int ng=0; ng<MAXFGROUPS; ng++)
    {
      AddItemGroup (ng);
      for (PFILEED pFE = grOffice.aFG[ng].pFEdList; pFE; pFE=pFE->next)
      {
        if (pFE->pszFilename)
          SetItemUL(lh = AddItemFile  (0, pFE->pszFilename), ULONG(pFE));
        else
          SetItemUL(lh = AddItemNoname(0, pFE->nNonNum, 0) , ULONG(pFE));
      }
    }
  }
  else
  {
    AddItemGroup (0);
    for (PSZ psz = srcList.Enum(2); psz; psz = srcList.NextEnum(2))
      AddItemFile  (0, psz);
  }
  return TRUE;
}
void BUFSCONFIG::bite(PFILEED pFE)
{ // Bite pFE out from its FILEGROUP's list
  PFILEED pfe = pFE->pFGroup->pFEdList;
  if (pfe==pFE) pFE->pFGroup->pFEdList = pFE->next;
  else
  {
    while(pfe->next && pfe->next!=pFE) pfe=pfe->next;
    if (pfe) pfe->next = pFE->next;
  }
}
BOOL BUFSCONFIG::Apply(HWND hMsgWnd)
{
  int        ng;
  LHANDLE    h;
  PFILEED    pFE;
  PFILEED    pFEActivate = 0;
  PFILEGROUP pFG;
  PFILEED   *ppFE;
  if (!fGroups) // Source files list
  {
    srcList.ClearManual(TRUE);
    for (LHANDLE h = StartEnum(); h; h=NextEnum(TRUE))
      if (QueryItemType(h)==BFITEM_FILE)
        srcList.AddFile(QueryItemName(h,FALSE),TRUE);
    srcList.ShowAppended();
    return TRUE;
  }
  // Try to close all the deleted editors first
  {
    PDELITEM pdi = pDelItems;
    while(pdi)
    {
      if (pdi->pFEDel)
      {
        if (!pdi->pFEDel->SaveChanges(hMsgWnd)) return FALSE;
        pdi->pFEDel->Destroy();
      }
      pdi = pdi->next;
    }
  }
  for (h=StartEnum(), ppFE=0; h; h = NextEnum(TRUE))
  {
    BFITEMT typ = QueryItemType(h);
    if (typ==BFITEM_GROUP)
    {
      ng   = QueryItemNum(h);
      ng   = min(ng,MAXFGROUPS-1);
      ppFE = &(grOffice.aFG[ng].pFEdList);
      continue;
    }
    if (!ppFE) continue; // Cockroash?

    pFE = (PFILEED)QueryItemUL(h);
    if (pFE)
    { // The item exists. Move it to the current location.
      if ((pFG=pFE->pFGroup)->pFEdActive==pFE && pFG!=&(grOffice.aFG[ng]))
        pFG->pFEdActive = 0;
      bite(pFE);
      pFE->pFGroup = &(grOffice.aFG[ng]);
      pFE->next    = *ppFE;
      *ppFE        = pFE;
      ppFE         = &(pFE->next);
    }
    else
    { // Create new item
      PTEDWINPARAMS pTWP = 0;
      PSZ pszFile = (typ==BFITEM_NONAME) ? NULL : QueryItemName(h,FALSE);
      pFE         = grOffice.aFG[ng].GOpenEditor (pszFile, hMsgWnd, pTWP, 0, QueryItemNum(h));
      if (!pFE)
      {
        ::SetAllCaptions();
        grOffice.ReorderWBar();
        return FALSE; // An error occured.
      }
      bite(pFE);
      pFE->next    = *ppFE;
      *ppFE        = pFE;
      ppFE         = &(pFE->next);
    }
    if (h==hActivateMe) pFEActivate = pFE;
  }
  ::SetAllCaptions();
  grOffice.ReorderWBar();
  if (pFEActivate)
    grOffice.GotoEd(pFEActivate->hWnd,FALSE);
  return TRUE;
}

void BUFSCONFIG::Free()
{
  for (PBUFINFO pbi, pbi0=pbList; pbi=pbi0;)
  {
    pbi0=pbi->next;
    delete pbi;
  }
  for (PDELITEM pdi, pdi0=pDelItems; pdi=pdi0;)
  {
    pdi0=pdi->next;
    delete pdi;
  }
  BOOL  f = fGroups;
  memset(this,0,sizeof(*this));
  fGroups = f;
}



/*-------------------------------------------------------------------------*/
/*                                                                         */
/*               F I L E D I R   class realisation                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/

void FILEDIR::freelist()
{
  for (PFILEREC pfr=pfList,pfr1; pfr; pfr=pfr1)
  {
    pfr1=pfr->next;
    delete pfr;
  }
  pfList = pfListTop = pfLastQueried = NULL;
}
LHANDLE FILEDIR::AppFile    (PFILEFINDBUF3 pfb3)
{
  PFILEREC pfr = new FILEREC;
  if (!pfr) return NULLHANDLE;
  if (!(pfr->pszName = (char*)malloc(strlen(pfb3->achName)+1)))
  {
    delete pfr;
    return NULLHANDLE;
  }
  strcpy(pfr->pszName,pfb3->achName);
  pfr->attrFile   = pfb3->attrFile;
  pfr->cbFile     = pfb3->cbFile;
  pfr->ulNamePos  = 0;
  if (!pfList) pfListTop = pfList = pfr;
  else         pfListTop = (pfListTop->next = pfr);
  return LHANDLE(pfr);
}
LHANDLE FILEDIR::QueryFirst()
{
  pfLastQueried = pfList;
  return LHANDLE(pfLastQueried);
}
LHANDLE FILEDIR::QueryNext()
{
  if (pfLastQueried) pfLastQueried = pfLastQueried->next;
  return LHANDLE(pfLastQueried);
}
PSZ FILEDIR::QueryName  (LHANDLE h, BOOL fShort)
{
  if (!h) return "";
  return fShort ? sf_fname2short(PFILEREC(h)->pszName) : PFILEREC(h)->pszName;
}
ULONG FILEDIR::QueryAttrs (LHANDLE h)
{
    return h ? PFILEREC(h)->attrFile : 0;
}
ULONG FILEDIR::QuerySel (LHANDLE h)
{
    return h ? PFILEREC(h)->fSel : 0;
}
ULONG   FILEDIR::QuerySize  (LHANDLE h)
{
    return h ? PFILEREC(h)->cbFile : 0;
}
ULONG FILEDIR::QueryNamePos(LHANDLE h)
{
    return h ? PFILEREC(h)->ulNamePos : 0;
}
void FILEDIR::SetSel (LHANDLE h, BOOL fSel)
{
  if (h && !(PFILEREC(h)->attrFile & FILE_DIRECTORY))
    PFILEREC(h)->fSel = fSel;
}
void FILEDIR::SetNamePos(LHANDLE h, ULONG l)
{
  if (h) PFILEREC(h)->ulNamePos = l;
}
BOOL FILEDIR::FastFind(char ch)
{
  int  lLen = strlen(szFastFind);
  BOOL rc   = !!lLen;
  if (!ch)
  {
    if (!fDispLocked)
    {
      szFastFind[0] = 0;
      if (hFFDisplay) WinDestroyWindow(hFFDisplay);
      hFFDisplay    = 0;
    }
    return rc;
  }
  if (lLen>=sizeof(szFastFind)-1) {Beep();return rc;}
  szFastFind[lLen]   = ch;
  szFastFind[lLen+1] = 0;
  {
    int nWalk = 1;
    int nIt   = (int)WinSendMsg(hLBDir,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    do
    {
      for (LHANDLE h; h=(LHANDLE)WinSendMsg(hLBDir,LM_QUERYITEMHANDLE,MPARAM(nIt),0); nIt++)
      {
        PSZ szIt = QueryName(h, TRUE);
        if (szIt && !sf_strnicmp(szIt,szFastFind,lLen+1))
        {
          fDispLocked = TRUE;
          WinSendMsg(hLBDir,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
          fDispLocked = FALSE;
          if (!hFFDisplay)
          {
            SWP   swp;
            HWND  hDlg  = WinQueryWindow(hLBDir,QW_PARENT);
            ULONG ul;
            WinQueryWindowPos(hLBDir,&swp);
            hFFDisplay = WinCreateWindow(hDlg,WC_STATIC,"",SS_TEXT | DT_LEFT | DT_VCENTER | WS_VISIBLE, 0,0,1,1, hDlg,HWND_TOP,0,0,0);
            WinSetPresParam(hFFDisplay,PP_FONTNAMESIZE,strlen(szDirPPFont)+1,szDirPPFont);
            ul = CLR_BLACK;  WinSetPresParam(hFFDisplay,PP_FOREGROUNDCOLORINDEX,sizeof(ULONG),&ul);
            ul = CLR_YELLOW; WinSetPresParam(hFFDisplay,PP_BACKGROUNDCOLORINDEX,sizeof(ULONG), &ul);
            HPS  hps   = WinGetPS(hFFDisplay);
            int  dy    = 14;
            POINTL aptl[TXTBOX_COUNT];
            if (GpiQueryTextBox(hps, 2, "Wg", TXTBOX_COUNT,aptl)) dy = aptl[TXTBOX_TOPRIGHT].y + 4;
            WinReleasePS(hps);
            WinSetWindowPos(hFFDisplay,HWND_TOP,swp.x+swp.cx/3,swp.y+swp.cy,swp.cx*2/3,dy,SWP_MOVE|SWP_SIZE|SWP_ZORDER);
          }
          WinSetWindowText(hFFDisplay,szFastFind);
          return rc;
        }
      }
      nIt=0;
    }while(nWalk++ == 1);
  }
  Beep();
  szFastFind[lLen] = 0;
  return rc;
}
BOOL FILEDIR::FastFindNext(BOOL fUp)
// Returns TRUE when fasf find mode ON
{
  LONG lLen  = strlen(szFastFind);
  if   (!lLen) return FALSE;
  int  nStep = fUp ? -1 : 1;
  int  nIt   = (int)WinSendMsg(hLBDir,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0) + nStep;
  for (LHANDLE h; h=(LHANDLE)WinSendMsg(hLBDir,LM_QUERYITEMHANDLE,MPARAM(nIt),0); nIt+=nStep)
  {
    PSZ szIt = QueryName(h, TRUE);
    if (szIt && !sf_strnicmp(szIt,szFastFind,lLen))
    {
      fDispLocked = TRUE;
      WinSendMsg(hLBDir,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
      fDispLocked = FALSE;
      return TRUE;
    }
  }
  return TRUE;
}
BOOL FILEDIR::cmp_fnames(PFILEREC pfr1, PFILEREC pfr2, BOOL fExt) // returns *pfr1 > *pfr2
{
  PSZ  pszCmp1  = 0;
  PSZ  pszCmp2  = 0;
  PSZ  psz1     = pfr1->pszName;
  PSZ  psz2     = pfr2->pszName;
  PSZ  pszName1 = sf_fname2short(psz1);
  PSZ  pszName2 = sf_fname2short(psz2);
  BOOL fDir1    = (pfr1->attrFile & FILE_DIRECTORY);
  BOOL fDir2    = (pfr2->attrFile & FILE_DIRECTORY);

  if (fDir1 != fDir2) return fDir2;
  else if (fDir1)
  {
    if (!strcmp(pszName1,"..")) return FALSE;
    if (!strcmp(pszName2,"..")) return TRUE;
  }
  if (fExt)
  {
    PSZ  psz;
    int  cmp = 0;
    if (psz = strrchr(pszName1,'.')) pszCmp1 = psz+1;
    if (psz = strrchr(pszName2,'.')) pszCmp2 = psz+1;
    if (!pszCmp1 != !pszCmp2) return !pszCmp2;
    if (pszCmp1 && pszCmp2) cmp = sf_stricmp(pszCmp1, pszCmp2);
    if (cmp) return cmp>0;
  }
  return sf_stricmp(pszName1,pszName2) > 0;
}
void FILEDIR::Sort(FDSORT fds)
{
  int       total,i;
  BOOL      fOk,fSwap=FALSE;;
  PFILEREC  pfr;
  for (total=0,pfr=pfList; pfr; total++,pfr=pfr->next);
  if (total<2) return;
  PFILEREC *apfr = (PFILEREC*)malloc(sizeof(PFILEDIR)*total);
  if (!apfr) return;
  for (pfr=pfList,total=0; pfr; total++,pfr=pfr->next) apfr[total]=pfr;
  // Sort it now:
  do
  {
    fOk = TRUE;
    for (i=total-2; i>=0; i--)
    {
      switch(fds){
        case FDSORT_NAME: fSwap = cmp_fnames(apfr[i],apfr[i+1],FALSE); break;
        case FDSORT_EXT:  fSwap = cmp_fnames(apfr[i],apfr[i+1],TRUE ); break;
      }
      if (fSwap)
      {
        fOk       = FALSE;
        pfr       = apfr[i];
        apfr[i]   = apfr[i+1];
        apfr[i+1] = pfr;
      }
    }
  } while(!fOk);
  // Re-build the list
  pfList = apfr[0];
  for (i=0; i<total; i++)
    apfr[i]->next = (i<total-1) ? apfr[i+1] : 0;
  pfListTop     = apfr[total-1];
  pfLastQueried = 0;
  fdSort        = fds;
}

/*-------------------------------------------------------------------------*/
//
// FILEGROUP class - represents one editors group. It can open a new editor
//

// pszFilename   - file name to open OR NULL - open Noname file;
// hMsgWnd       - window to use as a message boxe's owner or NULL to don't
//                 display any messages;
// pTWP          - preferable TED window's parameters (or 0 to use default)
// pFEIn         - fileed to open the new file inside (or 0 to create new)
// phEDUseShared - (out) 0 or PHWND to place hED with this file (when user forbade to
//                 open shared buffer. RC==0).
// returns PFILEED or NULL when error.
//
PFILEED FILEGROUP::GOpenEditor(PSZ pszFilenam0, HWND hMsgWnd, PTEDWINPARAMS pTWP,
                               PFILEED pFEIn, int nNonNum, HWND *phEDUseShared)
{
  int         rc;
  char       *pFileBuf   = NULL;
  BOOL        fReadOnly  = FALSE;
  HWND        hShareWith = NULLHANDLE;
  ULONG       fsize;
  static      PSZ           pszFilename = 0;
  static      PTEDWINPARAMS pTWP1       = 0;
  {// Provide TEDWINPARAMS
    if (pTWP1) delete pTWP1;
    pTWP1           = 0;
    if (!pTWP) pTWP = pTWP1 = new TEDWINPARAMS(pMDI->QueryWindow(MDIQW_DESK));
    if (!pTWP) return 0;
  }

  if (pFEIn && !pFEIn->SaveChanges(hMsgWnd)) return 0;
  if (pszFilenam0 && !pszFilenam0[0]) pszFilenam0 = NULL;

  free(pszFilename); pszFilename = 0;
  if (pszFilenam0)
  {
    pszFilename = (PSZ)malloc(CCHMAXPATH);
    if (!FName2Full(pszFilename,pszFilenam0,FALSE)) strcpy(pszFilename,pszFilenam0);
    // Self-shared?:
    if (pFEIn && !sf_stricmp(pFEIn->pszFilename,pszFilename)) return pFEIn;
  }

  if (pszFilename)
  {
    hShareWith = grOffice.KnownFilename(pszFilename);
    if (hShareWith)
    {
      if (hMsgWnd && MBID_YES!=MessageBox(hMsgWnd,"Load file",MB_ICONQUESTION|MB_YESNO,"File '",
          pszFilename,"' is now opened in another editor window(s). Do you want to open it in a shared mode?"))
      {
        if (phEDUseShared) *phEDUseShared = hShareWith;
        return 0;
      }
    }
    else
    {
      /*** Read file ***/
      HFILE hf;
      ULONG ul;
      DosError(FERR_DISABLEHARDERR);
      if (rc = (int)DosOpen(pszFilename,&hf,&ul,0,FILE_NORMAL,FILE_OPEN,
                        OPEN_FLAGS_NO_CACHE |OPEN_FLAGS_SEQUENTIAL| // Fastest mode?
                        OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE, (PEAOP2)0))
      {
        if (hMsgWnd) IOErMsg(rc, hMsgWnd, "Load file", "Can't open file", pszFilename, MB_OK);
        DosError(FERR_ENABLEHARDERR);
        return NULL;
      }
      {
        FILESTATUS3 fs3;
        memset(&fs3,0,sizeof(fs3));
        DosQueryFileInfo(hf,FIL_STANDARD,&fs3,sizeof(fs3));
        fReadOnly = !!(fs3.attrFile & FILE_READONLY);
      }
      DosSetFilePtr(hf,0L,FILE_END,  &fsize);
      DosSetFilePtr(hf,0L,FILE_BEGIN,&ul);
      if (!(pFileBuf = (char*)malloc(fsize+1)))
      {
        if (hMsgWnd) MessageBox(hMsgWnd,"Load file",MB_ERROR|MB_OK,"No enouth memory error");
        DosClose(hf);
        DosError(FERR_ENABLEHARDERR);
        return NULL;
      }
      if (rc = (int)DosRead(hf,(PVOID)pFileBuf,fsize,&ul))
      {
        if (hMsgWnd) IOErMsg(rc, hMsgWnd, "Load file", "File read error", pszFilename, MB_OK);
        DosError(FERR_ENABLEHARDERR);
        free(pFileBuf);
        return NULL;
      }
      pFileBuf[ul] = 0;
      DosClose(hf);
      DosError(FERR_ENABLEHARDERR);
    }
  }

  // Now: 1. pFileBuf   = buffer  &&  hShareWith = 0  => Edit this file
  //      2. hShareWith = hEd     &&  pFileBuf   = 0  => Share buffer
  //      3. hShareWith = 0       &&  pFileBuf   = 0  => Open 'Noname' editor
  //
  //      pFEIn = pFEd to use or 0 to create new
  //      fReadOnly may be TRUE when pFileBuf is set

  BOOL fNewWindow = !pFEIn;

  if (fNewWindow)
  {
    // >>>--- It's time to create new editor

    TEDCTLDATA             tcdata;
    tcdata.cb            = sizeof(tcdata);
    tcdata.pfCH          = (PFNCHARHANDLER)WMEdCharHandler;
    tcdata.hNotifyWindow = hMDI0Client;
    tcdata.hSharedEd     = hShareWith;
    HWND hEd;
    HWND hEdFrame = pMDI->CreateChild( MDIBIND_LEFTTOP, TEDCLASS, "", WS_VISIBLE,
                                       FCF_TITLEBAR|FCF_SIZEBORDER|FCF_HIDEMAX|
                                       FCF_SYSMENU|FCF_NOBYTEALIGN|FCF_VERTSCROLL,
                                       pTWP->x0,pTWP->y0,pTWP->cx,pTWP->cy, 0,&tcdata,0);
    if (!hEdFrame || !(hEd = WinWindowFromID(hEdFrame,FID_CLIENT)))
    {
      if (hMsgWnd) MessageBox(hMsgWnd,"Load file",MB_ERROR|MB_OK,"Can't create editor window. Possible "
                              "environment error or no enouth memory error.");
      free(pFileBuf);
      return NULL;
    }
    HWND hTBar    = WinWindowFromID(hEdFrame,FID_TITLEBAR);
    MRESULT EXPENTRY wpTitleBars(HWND,ULONG,MPARAM,MPARAM);
    wpTitleBars(hTBar, MM_START, MPARAM(ULONG(WinSubclassWindow(hTBar,(PFNWP)wpTitleBars))),0L);
    WinSendMsg(hEd,TM_SETEDFRAME,MPARAM(hEdFrame),0);
    if (pTWP->fMax) WinSetWindowPos(hEdFrame,0,0,0,0,0,SWP_MAXIMIZE);

    //--- Set hEd's text
    if (pFileBuf)
    {
      WinSendMsg(hEd,TM_SETTEXT,MPARAM(ULONG(pFileBuf)),MPARAM(strlen(pFileBuf)));
      WinSendMsg(hEd,TM_SETREADONLY,MPARAM(fReadOnly),0);
    }
    WinSendMsg(hEd,TM_SETCHANGED,MPARAM(FALSE),0);

    //
    // Create FILEED structure
    //

    pFEIn = new FILEED(this);
    if (!pFEIn)
    {
      WinDestroyWindow(hEdFrame);
      throw "No enouth memory error (Location: FILEGROUP::OpenFile)";
    }
    pMDI->SetChildHandle(hEdFrame,ULONG(pFEIn));
    pFEIn->hWnd   = hEd;
    pFEIn->hFrame = hEdFrame;
    if (!pFEdList) pFEdList = pFEIn;
    else
    {
      for(PFILEED pFE1 = pFEdList; pFE1->next; pFE1=pFE1->next);
      pFE1->next = pFEIn;
    }
  }
  else // Use the old window (pFEIn)
  {
    WinSendMsg(pFEIn->hWnd,TM_CLEARALL,0,0); // UnShare & clear
    pFEIn->pFInfo->Release();
    pFEIn->pFInfo = new FILEINFO;
    if (hShareWith)
    {
      if (!WinSendMsg(pFEIn->hWnd,TM_SETSHARE,MPARAM(hShareWith),0))
      {
        pFEIn->Destroy();
        free(pFileBuf);
        return 0;
      }
    }
    else if (pFileBuf)
    {
      WinSendMsg(pFEIn->hWnd,TM_SETTEXT,MPARAM(ULONG(pFileBuf)),MPARAM(strlen(pFileBuf)));
      WinSendMsg(pFEIn->hWnd,TM_SETREADONLY,MPARAM(fReadOnly),0);
    }
    free(pFEIn->pszFilename);
    pFEIn     ->pszFilename = 0;
  }

  if (hShareWith) // Share the fileinfo
  {
    PFILEED pFEShare = GROUPOFFICE::hEd2pFE(hShareWith);
    if (pFEShare)
    {
      pFEIn->pFInfo->Release();
      pFEIn->pFInfo = pFEShare->pFInfo;
      pFEIn->pFInfo->Capture();
    }
  }

  if (pFileBuf || hShareWith)
  {
    pFEIn->pszFilename = (PSZ)malloc(strlen(pszFilename)+1);
    strcpy(pFEIn->pszFilename,pszFilename);
    if (!hShareWith) pFEIn->pFInfo->SetInfo(pszFilename);
  }
  else
    pFEIn->nNonNum = nNonNum;
  pFEIn->NameSelf();
  { // Apply associations to the window:
    PASSTR pAss = ptOffice.QueryAss(pFEIn->pszFilename);
    EDCONF ec;
    WinSendMsg(pFEIn->hWnd,TM_QUERYEDCONF,MPARAM(&ec),0);
    if (fNewWindow)
    {
      memcpy(&ec.edPalette, &pAss->aaRGB,  sizeof(ec.edPalette));
      memcpy(&ec.fAttrs,    &pAss->fAttrs, sizeof(FATTRS));
      ec.fHilite        = pAss->fHilite;
      ec.fHiliteCurLine = pAss->fHiliteCurLine;
    }
    strcpy(ec.szPainterId, pAss->szPainterId);
    ec.lTabSize       = pAss->ulTabWidth;
    ec.fSmartTab      = pAss->fSmartTabs;
    ec.fAutoidentMode = pAss->fAutoident;
    ec.fTabCharsMode  = pAss->fUseTabChars;
    WinSendMsg(pFEIn->hWnd,TM_SETEDCONF,MPARAM(&ec),0);
  }
  // Re-set focus to set active group (via TED's WM_CONTROL message)
  if (!pTWP->fHidden)
    WinSetWindowPos(pFEIn->hFrame,HWND_TOP,0,0,0,0,SWP_SHOW|SWP_ACTIVATE|SWP_ZORDER);
  free(pFileBuf);
  return pFEIn;
}

void FILEGROUP::DestroyAll()
// Note: the group number is losed
{
  PFILEED pFE,pFE1;
  for(pFE=pFEdList; pFE1=pFE, pFE; pFE=pFE1->next)
    delete pFE;
  memset(this,0,sizeof(*this));
}

FILEGROUP::~FILEGROUP()
{
  DestroyAll();
}

MRESULT EXPENTRY wpTitleBars(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PFNWP pWindowProc;

  switch (ulMsg)
  {
    case MM_START:
      pWindowProc = PFNWP(ULONG(m1));
      return 0;
    case WM_COMMAND:
    {
      SHORT s1m1 = SHORT1FROMMP(m1);
      switch(s1m1){
        case IDM_POP_FULLNAMES:
        {
          FILEED::fShortName = !FILEED::fShortName;
          SetAllCaptions();
          return 0;
        }
      }
      break;
    }
    case WM_BUTTON2UP:
      if (WinSendMsg(hWnd,TBM_QUERYHILITE,0,0))
      {
        LONG aChk[2] = {IDM_POP_FULLNAMES, 0};
        if (FILEED::fShortName) aChk [0] = 0;
        PlayMenu(hWnd, IDM_POPUP_EDTITLEBAR, aChk, 0, 0);
        return 0;
      }
      break;
  }
  return (*pWindowProc)(hWnd,ulMsg,m1,m2);
}


//-----------------------------------------------------------------------------//

FILEINFO::FILEINFO()
{
  memset(this,0,sizeof(*this));
  for (int i=0; i<MAXBOOKMARKS; i++)
    atprBM[i].Set(-1,-1);
  nCapt = 1;
}
void FILEINFO::SetInfo (PSZ szPath)
{
  FILESTATUS3 fs30;
  fSet = FALSE;
  if (!DosQueryPathInfo(szPath,FIL_STANDARD,&fs30,sizeof(fs30)))
  {
    fs3  = fs30;
    fSet = TRUE;
  }
}
int FILEINFO::CheckInfo (PSZ szPath)
// Returns: 0 - Ok ob; 1 - updated; 2 - removed
{
  FILESTATUS3 fs30;
  APIRET      err;
  int         ret = 0;
  if (err = DosQueryPathInfo(szPath,FIL_STANDARD,&fs30,sizeof(fs30)))
  {
    if(fSet && (err==3 || err==2 /* Path or file not found */))
    {
      fSet=FALSE;
      ret = 2;          // <--- Removed.
    }
  }
  else
  {
    ret = ( fSet &&
             memcmp(&fs3.ftimeLastWrite, &fs30.ftimeLastWrite,2) ||
             memcmp(&fs3.fdateLastWrite, &fs30.fdateLastWrite,2) ||
             memcmp(&fs3.ftimeCreation,  &fs30.ftimeCreation, 2) ||
             memcmp(&fs3.fdateCreation,  &fs30.fdateCreation, 2)
             /*(fs3.ftimeLastWrite.twosecs != fs30.ftimeLastWrite.twosecs) ||
             (fs3.ftimeLastWrite.minutes != fs30.ftimeLastWrite.minutes) ||
             (fs3.ftimeLastWrite.hours   != fs30.ftimeLastWrite.hours)   ||
             (fs3.ftimeCreation. twosecs != fs30.ftimeCreation. twosecs) ||
             (fs3.ftimeCreation. minutes != fs30.ftimeCreation. minutes) ||
             (fs3.ftimeCreation. hours   != fs30.ftimeCreation. hours)   ||
             (fs3.fdateLastWrite.day     != fs30.fdateLastWrite.day)     ||
             (fs3.fdateLastWrite.month   != fs30.fdateLastWrite.month)   ||
             (fs3.fdateLastWrite.year    != fs30.fdateLastWrite.year)    ||
             (fs3.fdateCreation. day     != fs30.fdateCreation. day)     ||
             (fs3.fdateCreation. month   != fs30.fdateCreation. month)   ||
             (fs3.fdateCreation. year    != fs30.fdateCreation. year)    ||
             (fs3.cbFile                 != fs30.cbFile)  */
          ) ? 1 : ret;
    #if DBGLOG
    if (ret==1)
    {
      fprintf(stderr,"File updated: %s\n",szPath);
      fprintf(stderr,"ftimeLastWrite old=%04x,new=%04x\n", (int)*(PUSHORT)&fs3.ftimeLastWrite, (int)*(PUSHORT)&fs30.ftimeLastWrite);
      fprintf(stderr,"fdateLastWrite old=%04x,new=%04x\n", (int)*(PUSHORT)&fs3.fdateLastWrite, (int)*(PUSHORT)&fs30.fdateLastWrite);
      fprintf(stderr,"ftimeCreation  old=%04x,new=%04x\n", (int)*(PUSHORT)&fs3.ftimeCreation,  (int)*(PUSHORT)&fs30.ftimeCreation );
      fprintf(stderr,"fdateCreation  old=%04x,new=%04x\n", (int)*(PUSHORT)&fs3.fdateCreation,  (int)*(PUSHORT)&fs30.fdateCreation );

      /*fprintf(stderr,"fdateLastWrite(old): %016x; fdateLastWrite(new): %016x",
                     (int)*(PUSHORT)&fs3.fdateLastWrite, (int)*(PUSHORT)&fs30.fdateLastWrite);
      fprintf(stderr,"Old info (time creation): 2secs=%08u, min=%08u, hrs=%08u, day=%08u, month=%08u, year=%08u\n",
       (int)fs3.ftimeCreation.twosecs,   (int)fs3.ftimeCreation.minutes,   (int)fs3.ftimeCreation.hours,
       (int)fs3.fdateCreation.day,fs3,   (int)fs3.fdateCreation.month,     (int)fs3.fdateCreation.year  );
      fprintf(stderr,"New info (time creation): 2secs=%08u, min=%08u, hrs=%08u, day=%08u, month=%08u, year=%08u\n",
       (int)fs30.ftimeCreation.twosecs,  (int)fs30.ftimeCreation.minutes,  (int)fs30.ftimeCreation.hours,
       (int)fs30.fdateCreation.day,fs3,  (int)fs30.fdateCreation.month,    (int)fs30.fdateCreation.year  );
      fprintf(stderr,"Old info (last write):    2secs=%08u, min=%08u, hrs=%08u, day=%08u, month=%08u, year=%08u\n",
       (int)fs3.ftimeLastWrite.twosecs,  (int)fs3.ftimeLastWrite.minutes,  (int)fs3.ftimeLastWrite.hours,
       (int)fs3.fdateLastWrite.month,    (int)fs3.fdateLastWrite.year, (int)fs3.fdateLastWrite.day );
      fprintf(stderr,"New info (last write):    2secs=%08u, min=%08u, hrs=%08u, day=%08u, month=%08u, year=%08u\n\n",
       (int)fs30.ftimeLastWrite.twosecs, (int)fs30.ftimeLastWrite.minutes, (int)fs30.ftimeLastWrite.hours,
       (int)fs30.fdateLastWrite.day,fs3, (int)fs30.fdateLastWrite.month,   (int)fs30.fdateLastWrite.year  );*/
    }
    #endif
    fs3  = fs30;
    fSet = TRUE;
  }
  return ret;
}
void FILEINFO::TextChanged(PTCD ptcd)
{
  for (int i=0; i<MAXBOOKMARKS; i++)
    if (atprBM[i].lLine>=0) atprBM[i].UpdatePos(ptcd);
}

/*-------------------------------------------------------------------------*/
//
// FILEED class - all the editor windows has a corresponded FILEED structure.
// Lists of this structures are hanged on a FILEGROUP structures grouping
//   editors as it is need.
// Pointer to FILEED (equ CLASSINFO) structure writes into the MDI client
//   window's handle.
//

BOOL FILEED::fShortName  = TRUE;

FILEED::FILEED(PFILEGROUP pFG):
  CLIENTINFO(CLIENT_EDITOR)
{
  next        = 0;
  pFGroup     = pFG;
  pszFilename = 0;
  pFInfo      = new FILEINFO;
}

// Caused window destruction and group windows reordering
//
FILEED::~FILEED()
{
  if (pszFilename) hstFiles.Append(pszFilename);
  WinDestroyWindow(hFrame);
  free(pszFilename);
  // Strike us out from the list
  PFILEED    pFE = pFGroup->pFEdList;
  if (pFE==this) pFGroup->pFEdList = next;
  else
  {
    while(pFE && pFE->next!=this) pFE = pFE->next;
    if (pFE) pFE->next = next;
  }
  // Was we the active in our group?
  PFILEGROUP pFG;
  if ((pFG=pFE->pFGroup)->pFEdActive == pFE)
    pFG->pFEdActive = 0;
  pFInfo->Release();
}

//
// If the buffer was changed: Save/Discard/Cancel
// Returns FALSE if cancelled or if it was write error
//
BOOL FILEED::SaveChanges(HWND hMsgWnd)
{
  if (!WinSendMsg(hWnd,TM_QUERYCHANGED,0,0)) return TRUE;
  char  szQQ [CCHMAXPATH+100];
  char  szNam[CCHMAXPATH+20];
  char *pch;
  LONG  rc = MBID_YES;
  QueryName(szNam,sizeof(szNam));
  pch = strchr(szNam,':');
  if (pch) pch = sf_skipspaces(pch+1);
  else     pch = szNam;
  sprintf(szQQ,"File '%s' is changed. Save it?",pch);
  if (!GROUPOFFICE::fAutosave)
    rc = WinMessageBox(HWND_DESKTOP,hMsgWnd,szQQ,"Save changes",0,MB_ICONQUESTION|MB_YESNOCANCEL|MB_MOVEABLE);
  if      (rc==MBID_NO)  return TRUE;
  else if (rc==MBID_YES) return SaveFile(hMsgWnd, FALSE, 0);
  else                   return FALSE;
}

void FILEED::Destroy()
{
  delete this;
}

BOOL FILEED::SaveFile(HWND hMsgWnd, BOOL fSaveAs, PSZ pszDestDir, PSZ pszNewName)
//
// hMsgWnd    - window to use as a parent for messages (Can not be 0)
// fSaveAs    - Save as mode. For 'Noname' assumed TRUE in both cases
// pszDestDir - If it is SaveAs mode and pszDestDir is not 0 - it uses as a
//              destination directory in the SaveAs prompt. (with last '\')
//
{
  MRESULT EXPENTRY dpSaveAs(HWND,ULONG,MPARAM,MPARAM); // Only for the SaveFile method
  PSZ              pszDestName = fSaveAs ? 0 : pszFilename; // This PSZ we'll use when saving the file
  APIRET           err;
  HFILE            hf = 0;
  ULONG            ulAction;
  char             sz_oldname[CCHMAXPATH];
  char             sz_pathuse[CCHMAXPATH];
  CPARAMSTR        cps;
                   cps.psz1 = sz_oldname; // Old name
                   cps.psz2 = sz_pathuse; // Path to use
  if (pszFilename)  strcpy (cps.psz1,sf_fname2short(pszFilename));
  else              sprintf(cps.psz1,"Noname%u",nNonNum);
  if (pszDestDir)
  {
    strcpy(cps.psz2,pszDestDir);
    if (pszFilename) strcat(cps.psz2,sf_fname2short(pszFilename));
  }
  else
  {
    if (pszFilename) strcpy (cps.psz2,pszFilename);
    else             sprintf(cps.psz2,"%s\\",szCurIOPath);
  }

  //--- Forbid to SaveAS shared file:
  if (fSaveAs && pszFilename && WinSendMsg(hWnd,TM_QUERYSHARED,MPARAM(1),0))
  {
    char    szQQ[CCHMAXPATH+100];
    sprintf(szQQ,"The file '%s' is shared and may be saved only using this name. Save it now?",pszFilename);
    if (MBID_YES!=WinMessageBox(HWND_DESKTOP,hMsgWnd,szQQ,"Save as",0,MB_MOVEABLE|MB_ICONQUESTION|MB_YESNO))
      return FALSE;
    else
      return SaveFile(hMsgWnd, FALSE, pszDestDir); // Note: recursive call!
  }
  DosError(FERR_DISABLEHARDERR);
  if (!pszDestName)
  {
  try_again:
    fSaveAs = TRUE;
    if      (pszNewName) strcpy(cps.psz2,pszNewName), pszNewName = 0;
    else if (!WinDlgBox(HWND_DESKTOP,hMsgWnd,(PFNWP)dpSaveAs,NULLHANDLE,IDD_SAVEASDLG,&cps))
    {
      DosError(FERR_ENABLEHARDERR);
      return FALSE;
    }
    else
      pszDestName = cps.psz2;
    if ((!pszFilename || sf_stricmp(pszFilename,pszDestName)) &&
        grOffice.KnownFilename(pszDestName))
    {
      WinMessageBox(HWND_DESKTOP,hMsgWnd,(PSZ)"Can't overwrite currently opened file.",
                    (PSZ)"Save as",0,MB_MOVEABLE|MB_ERROR|MB_OK);
      goto try_again;
    }
    err = DosOpen(pszDestName,&hf,&ulAction,0,FILE_NORMAL,FILE_OPEN,
                  OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYWRITE,(PEAOP2)0L);
    if (!err)
    {
      char szQQ[CCHMAXPATH+40];
      sprintf(szQQ,"File %s is already exists. Overwrite?",pszDestName);
      if (MBID_YES != WinMessageBox(HWND_DESKTOP,hMsgWnd,(PSZ)szQQ,(PSZ)"Save as",0,
                                    MB_QUERY|MB_YESNO|MB_MOVEABLE))
      {
        DosClose(hf);
        goto try_again;
      }
    }
    else if (err != 2 && err != 110)
    {
      IOErMsg(err, hMsgWnd, "Save as", "Can't open or create file", pszDestName, MB_OK);
      goto try_again;
    }
  }
  if (!hf)
  {
    char szBakFile[CCHMAXPATH];
    strcpy(szBakFile,pszDestName);
    // Сделаем '.bak' расширение
    PSZ  pszFN,pszDot;
    BOOL fBakMaked = FALSE;
    (pszFN=strrchr(szBakFile,'\\')) || (pszFN=strrchr(szBakFile,'/')) ||(pszFN=szBakFile);
    (pszDot=strrchr(pszFN,'.')) || (pszDot=pszFN+strlen(pszFN));
    strcpy(pszDot,".BAK");
    if (GROUPOFFICE::fMakeBak) fBakMaked = !DosMove(pszDestName,szBakFile);
    err = DosOpen(pszDestName,&hf,&ulAction,0,FILE_NORMAL,
              OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
              OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYWRITE,(PEAOP2)0L);
    if (err)
    {
      if (fBakMaked) DosMove(szBakFile,pszDestName); // Попытаемся сокрыть содеянное...
      IOErMsg(err, hMsgWnd, "Save", "Can't open or create file", pszDestName, MB_OK);
      if (fSaveAs) goto try_again;
      else         { DosError(FERR_ENABLEHARDERR); return FALSE;}
    }
  }
  PSZ pszBuf = (PSZ)WinSendMsg(hWnd,TM_EXPORT,0,0);
  if (!pszBuf)
  {
    WinMessageBox(HWND_DESKTOP,hMsgWnd,(PSZ)"Out of memory error (Location: FILEED::SaveFile)",
                  0,0,MB_MOVEABLE|MB_ERROR|MB_OK);
    DosError(FERR_ENABLEHARDERR);
    return FALSE;
  }
  DosSetFileSize(hf,0);
  LONG  lLen = strlen(pszBuf);
  ULONG ul;
  err = DosWrite(hf,PBYTE(pszBuf),lLen,&ul);
  free(pszBuf);
  if (err)
    IOErMsg(err, hMsgWnd, "Save", "Write file error", pszDestName, MB_OK);
  else
  {
    WinSendMsg(hWnd,TM_SETCHANGED,MPARAM(FALSE),0);
    if((!pszFilename || sf_stricmp(pszFilename,pszDestName)))
    { //--- The file name is changed, but it was not shared & does not becomes shared
      PSZ psz = (PSZ)malloc(strlen(pszDestName)+1); // New (* pszDest can points into the pszFilename *)
      strcpy(psz,pszDestName);
      free(pszFilename);
      pszFilename = psz;
      pFInfo->Release();
      pFInfo = new FILEINFO;
      SetAllCaptions();
    }
  }
  DosClose(hf);
  pFInfo->SetInfo(pszFilename);
  DosError(FERR_ENABLEHARDERR);
  return !err;
}


void FILEED::NameSelf()
{
  char  szName[CCHMAXPATH+100];
  // Winbar:
  if (pFGroup->nGroup) sprintf(szName, "%u: ", pFGroup->nGroup);
  else                 strcpy (szName, "A: ");
  if (pszFilename)
  {
    strcat(szName, sf_fname2short(pszFilename));
    if (WinSendMsg(hWnd,TM_QUERYSHARED,MPARAM(1),0)) strcat(szName," (Shared)");
  }
  else sprintf(szName+strlen(szName),"Noname%u",nNonNum);
  pMDI->WBarSetText      (hFrame,szName);
  // Window title:
  if (pFGroup->nGroup) sprintf(szName, "%u: ", pFGroup->nGroup);
  else                 strcpy (szName, "Auto window: ");
  if (pszFilename)
  {
    strcat(szName, fShortName ? sf_fname2short(pszFilename) : pszFilename);
    if (WinSendMsg(hWnd,TM_QUERYSHARED,MPARAM(1),0)) strcat(szName," (Shared)");
  }
  else
  sprintf(szName+strlen(szName),"Noname%u",nNonNum);
  WinSetWindowText(hFrame,szName);
}

void FILEED::QueryName(PCH pBuf, LONG lLen)
{
  NameSelf();
  WinQueryWindowText(hFrame,lLen,pBuf);
}
BOOL FILEED::CheckUpdated(HWND hMsgWnd)
// Ret: FALSE - editor contents has been updated (or losed)
{
  int  chk;
  BOOL ret = TRUE;
  if (pszFilename && (chk = pFInfo->CheckInfo(pszFilename)))
  {
    char sz[CCHMAXPATH+100];
    if (chk==2) // Removed
    {
      sprintf(sz,"Another application has removed '%s'.",pszFilename);
      WinMessageBox(HWND_DESKTOP,hMsgWnd,sz,"Warning!",0,MB_WARNING|MB_OK|MB_MOVEABLE);
    }
    else       // Updated
    {
      if (!WinSendMsg(hWnd,TM_QUERYCHANGED,MPARAM(TRUE),0)) grOffice.Reload(this,hMsgWnd), ret = FALSE;
      else
      {
        sprintf (sz,"Another application has modifyed '%s'. Do you want to reload it?",pszFilename);
        if ( MBID_YES == WinMessageBox(HWND_DESKTOP,hMsgWnd,sz,"Warning!",0,MB_WARNING|MB_YESNO|MB_MOVEABLE)
             && grOffice.Reload(this,hMsgWnd))
          ret = FALSE;
      }
    }
  }
  return ret;
}

PLONG FILEED::QDisabledBMs()
{
  static LONG al[MAXBOOKMARKS+1];
  int         i,j=0;
  memset(al,0,sizeof(al));
  for(i=0; i<MAXBOOKMARKS; i++)
    if (pFInfo->atprBM[i].lLine<0)
      al[j++] = i+IDM_POP_GOBM0;
  return al;
}
PTPR FILEED::GoBM(ULONG usCmd)
{
  ULONG ul = usCmd-IDM_POP_GOBM0;
  if ((ul < MAXBOOKMARKS) && (pFInfo->atprBM[ul].lLine>=0))
  {
    WinSendMsg(hWnd,TM_SETSEL,  MPARAM(&pFInfo->atprBM[ul]),MPARAM(&pFInfo->atprBM[ul]));
    WinSendMsg(hWnd,TM_QUERYSEL,MPARAM(&pFInfo->atprBM[ul]),0);
    return &pFInfo->atprBM[ul];
  }
  return 0;
}
PTPR FILEED::SetBM(ULONG usCmd)
{
  ULONG ul = usCmd-IDM_POP_SETBM0;
  if (ul < MAXBOOKMARKS)
  {
    WinSendMsg(hWnd,TM_QUERYSEL,MPARAM(&pFInfo->atprBM[ul]),0);
    return &pFInfo->atprBM[ul];
  }
  return 0;
}
PTPR FILEED::QueryBM(ULONG usCmd)
{
  ULONG ul = usCmd-IDM_POP_SETBM0;
  return ((ul < MAXBOOKMARKS) && (pFInfo->atprBM[ul].lLine>=0)) ? &pFInfo->atprBM[ul] : 0;
}

MRESULT EXPENTRY dpSaveAs(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static HWND        hEF;
  static PCPARAMSTR  pcps;

  switch (ulMsg){
  case WM_INITDLG:
    CenterChildWindow(hWnd);
    hEF = WinWindowFromID(hWnd,IDD_EFSAVEAS);
    pcps = (PCPARAMSTR)m2;
    {
      char sz[CCHMAXPATH+100];
      sprintf(sz,"Save file %s as:",pcps->psz1);  // old name
      WinSetDlgItemText(hWnd,IDD_STSAVEAS,sz);
    }
    WinSetWindowText(hEF,pcps->psz2);             // Path to use (in/out)
    WinSetActiveWindow(HWND_DESKTOP,hEF);
    break;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
  {
    switch (SHORT1FROMMP(m1))
    {
      case DID_CANCEL:
        WinDismissDlg(hWnd,0);
        break;
      case DID_OK:
      {
        char sz[CCHMAXPATH];
        WinQueryWindowText(hEF,sizeof(sz),sz);
        FName2Full(sz,sz,FALSE);
        strcpy(pcps->psz2,sz);
        WinDismissDlg(hWnd,TRUE);
        break;
      }
      case IDD_PBBROWSE:
      {
        char szF[CCHMAXPATH];
        WinQueryWindowText(hEF,sizeof(szF),szF);
        if (Browse(szF,szF,hWnd,"Save as",TRUE))
        {
          WinSetWindowText(hEF,szF);
          WinSetActiveWindow(HWND_DESKTOP,hEF);
        }
        break;
      }
      case IDD_PBRESETPATH:
      {
        char sz [CCHMAXPATH];
        char sz1[CCHMAXPATH*2];
        WinQueryWindowText(hEF,sizeof(sz),sz);
        PSZ  psz = sf_fname2short(sz);
        sprintf(sz1,"%s\\%s",szCurPath,psz);
        sz1[CCHMAXPATH-1] = 0;
        WinSetWindowText(hEF,sz1);
        WinSetActiveWindow(HWND_DESKTOP,hEF);
        break;
      }
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_SAVEASDLG);
        break;
    }
    break;
  }
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_FINDDLG);
      break;
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


//------------- GROUPOFFICE -------------------

int   GROUPOFFICE::nNonamCount = 0;
BOOL  GROUPOFFICE::fAutosave   = FALSE;
BOOL  GROUPOFFICE::fMakeBak    = FALSE;
BOOL  GROUPOFFICE::f1AutoWin   = TRUE;

GROUPOFFICE::GROUPOFFICE()
{
  for (int i=0; i<MAXFGROUPS; i++)
    aFG[i].nGroup = i;
  nActiveGroup = 0;
  pfeEnum      = 0;
}
GROUPOFFICE::~GROUPOFFICE()
{
  // Does nothing yet.
}

/* Open new file in the group
>
>  ULONG         nGroup (in)      - Group to open editor in (may be IN_AUTO_WINDOW - to open in/new [the]
>                                   auto window depends on f1AutoWin state)
>  PSZ           pszFilename (in) - File to open or 0 to create 'Noname' window
>  HWND          nMsgWin (in)     - Window to use as a parent to show a messages. If 0 - show nothing.
>  PTEDWINPARAMS pTWP (in)        - TED creation data. If 0 - use default
>  PFILEED       pFEIn   (in,def) - If not 0 - try to open in its window
>  int           nNonNum (in,def) - When a 'Noname' file needs denotes it's number (If <0 - autogenerate)
>  Return value (BOOL)            - Success indicator
*/
BOOL GROUPOFFICE::OpenEditor(ULONG nGroup, PSZ pszFilename, HWND hMsgWnd, PTEDWINPARAMS pTWP,
                             PFILEED pFEIn, int nNonNum)
{
  if (nGroup == IN_AUTO_WINDOW && !pFEIn)
  {
    nGroup = 0;
    pFEIn  = f1AutoWin ? aFG[0].pFEdList : 0;
  }
  if (nGroup >= MAXFGROUPS) return FALSE;

  BOOL    fMyTWP       = !pTWP;
  HWND    hEDUseShared = 0;
  if ((!pszFilename || !pszFilename[0]) && nNonNum<0) nNonNum = GenerateNonNum();
  if (fMyTWP) pTWP = new TEDWINPARAMS(pMDI->QueryWindow(MDIQW_DESK));
  if (aFG[nGroup].GOpenEditor(pszFilename,hMsgWnd,pTWP,pFEIn,nNonNum, &hEDUseShared))
  {
    ::SetAllCaptions();
    ReorderWBar();
  }
  if      (fMyTWP)        delete pTWP;
  else if (hEDUseShared)  GotoEd(hEDUseShared,FALSE);
  else                    return FALSE;
  return TRUE;
}


/* CloseWindow
>
>  HWND hEd (in)       - Window to close (0 - close all)
>  HWND nMsgWnd (in)   - Window to use as a parent to show a messages. If 0 - close immediatly
>  Return value (BOOL) - Success indicator
*/
BOOL GROUPOFFICE::CloseWindow(HWND hEd, HWND hMsgWnd)
{
  PFILEED pFE;
  if (hEd)
  {
    if (pFE=hEd2pFE(hEd))
    {
      int rc = save_prompt(pFE,hMsgWnd);
      if (rc!=MBID_YES && rc!=MBID_NO)                        return FALSE; // Cancelled
      if (rc==MBID_YES && !SaveOneFile (hEd, FALSE, hMsgWnd)) return FALSE; // Can't save
      pFE->Destroy();
      {// Если окон не осталось - переэнейблим тулзы
        for (int ng=0; ng<MAXFGROUPS; ng++) if (aFG[ng].pFEdList) break;
        if (ng==MAXFGROUPS)
        {
          EnableMenues(EM_ASKALL);
          pToolbox->ReenableAll();
        }
      }
      ReorderWBar();
      return TRUE;
    }
  }
  else // Close all
  {
    int ng;
    for(ng=0; ng<MAXFGROUPS; ng++)
      for (pFE=aFG[ng].pFEdList; pFE; pFE=pFE->next)
      {
        int rc = save_prompt(pFE,hMsgWnd);
        if (rc!=MBID_YES && rc!=MBID_NO)                        return FALSE; // Cancelled
        if (rc==MBID_YES && !SaveOneFile (hEd, FALSE, hMsgWnd)) return FALSE; // Can't save
      }
    for(ng=0; ng<MAXFGROUPS; ng++)
      for (pFE=aFG[ng].pFEdList; pFE; pFE=pFE->next)
        pFE->Destroy();
    EnableMenues(EM_ASKALL);
    pToolbox->ReenableAll();
    return TRUE;
  }
  return FALSE;
}


/* SaveOneFile
>
>  HWND hEd            - hEd to save or 0 to save the active
>  BOOL fSaveAs        - 'Save as' mode (for 'Noname - anyway)
>  HWND hMsgWnd        - Window to use as a parent to show a messages
>  Return value        - success state
*/
BOOL GROUPOFFICE::SaveOneFile (HWND hEd, BOOL fSaveAs, HWND hMsgWnd, PSZ szNewName)
{
  PFILEED  pFE = (hEd) ? hEd2pFE(hEd) : aFG[nActiveGroup].pFEdActive;
  if (!pFE) return !fSaveAs;
  return pFE->SaveFile(hMsgWnd, fSaveAs, 0, szNewName);
}


/* SaveAll (save all the changed buffers)
>
>  BOOL fPrompt        - Prompt before save
>  HWND nMsgWin        - Window to use as a parent to show a messages
>  Return value        - hEd was not succesfully saved OR 0 - Ok.
*/
HWND GROUPOFFICE::SaveAll (BOOL fPrompt, HWND hMsgWnd)
{
  MRESULT EXPENTRY dpSavCh(HWND,ULONG,MPARAM,MPARAM);
  int              ng;
  PFILEED          pFE;
  int              nMode = IDD_PBSAVEALL;
  HWND             hRet  = 0;
  // Is anybody changed here?
  {
    BOOL fChg = FALSE;
    for(ng=0; !fChg && ng<MAXFGROUPS; ng++)
    {
      for (pFE=aFG[ng].pFEdList; pFE; pFE=pFE->next)
        if (fChg=(BOOL)WinSendMsg(pFE->hWnd,TM_QUERYCHANGED,0,0))
        {
          hRet = pFE->hWnd;
          break;
        }
    }
    if (!fChg) return hRet; // Nothing to save
  }

  if (fPrompt && !fAutosave)
    nMode = (int)WinDlgBox(HWND_DESKTOP,hMsgWnd,(PFNWP)dpSavCh,NULLHANDLE,IDD_SAVCHDLG,0);
  if (nMode==IDD_PBDONTSAVE)                       return NULLHANDLE;  // Save nothing, Ok.
  if (nMode!=IDD_PBSAVEALL && nMode!=IDD_PBPROMPT) return hRet;        // Cancelled

  // Save...
  {
    BOOL fChg = FALSE;
    for(ng=0; !fChg && ng<MAXFGROUPS; ng++)
      for (pFE=aFG[ng].pFEdList; pFE; pFE=pFE->next)
      {
        if (!WinSendMsg(pFE->hWnd,TM_QUERYCHANGED,0,0)) continue;
        if (nMode==IDD_PBPROMPT)
        {
          int rc = save_prompt(pFE,hMsgWnd);
          if (rc==MBID_NO)  continue;                                   // Skip
          if (rc!=MBID_YES) return pFE->hWnd;                           // Cancel
        }
        if (!SaveOneFile (pFE->hWnd, FALSE, hMsgWnd)) return pFE->hWnd; // Oops!
      }
  }
  return NULLHANDLE;
}

/* Reload the file
>
>  PFILEED pFE     - FILEED to reload or 0 to reload the active
>  HWND    hMsgWnd - Window to use as a parent to show a messages
>  Return value    - TRUE - file has been reloaded
*/
BOOL GROUPOFFICE::Reload(PFILEED pFE, HWND hMsgWnd)
{
  if (!pFE && !(pFE=QueryActiveFEd())) return FALSE;
  if (!pFE->pszFilename)               return FALSE;
  if (WinSendMsg(pFE->hWnd,TM_QUERYCHANGED,0,0))
  {
    char sz[CCHMAXPATH+150];
    sprintf(sz,"File '%s' has been changed. Do you want to reload it without saving changes?",pFE->pszFilename);
    if (MBID_YES!=WinMessageBox(HWND_DESKTOP,hMsgWnd,sz,"Reload file",0,MB_MOVEABLE|MB_ICONQUESTION|MB_YESNO))
      return FALSE;
  }

  // Get the new file contents
  HFILE  hf;
  ULONG  ul,fsize;
  APIRET rc;
  BOOL   fReadOnly = FALSE;
  char   *pFileBuf=0;
  DosError(FERR_DISABLEHARDERR);
  if (rc = DosOpen(pFE->pszFilename,&hf,&ul,0,FILE_NORMAL,FILE_OPEN,
                   OPEN_FLAGS_NO_CACHE |OPEN_FLAGS_SEQUENTIAL| // Fastest mode?
                   OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE, (PEAOP2)0))
  {
    IOErMsg(rc, hMsgWnd, "Reload file", "Can't open file", pFE->pszFilename, MB_OK);
    DosError(FERR_ENABLEHARDERR);
    return FALSE;
  }
  DosSetFilePtr(hf,0L,FILE_END,  &fsize);
  DosSetFilePtr(hf,0L,FILE_BEGIN,&ul);
  if (!(pFileBuf = (char*)malloc(fsize+1)))
  {
    MessageBox(hMsgWnd,"Reload file",MB_ERROR|MB_OK,"No enouth memory error");
    DosClose(hf);
    DosError(FERR_ENABLEHARDERR);
    return FALSE;
  }
  if (rc = DosRead(hf,(PVOID)pFileBuf,fsize,&ul))
  {
    IOErMsg(rc, hMsgWnd, "Reload file", "File read error", pFE->pszFilename, MB_OK);
    DosError(FERR_ENABLEHARDERR);
    free(pFileBuf);
    return FALSE;
  }
  pFileBuf[ul] = 0;
  {
    FILESTATUS3 fs3;
    DosQueryFileInfo(hf,FIL_STANDARD,&fs3,sizeof(fs3));
    fReadOnly = !!(fs3.attrFile & FILE_READONLY);
  }
  DosClose(hf);
  DosError(FERR_ENABLEHARDERR);

  // Set new text instead the old
  TPR tpr1,tpr2;
  WinSendMsg(pFE->hWnd,TM_QUERYSEL,   MPARAM(&tpr1),           MPARAM(&tpr2));
  WinSendMsg(pFE->hWnd,TM_SETTEXT,    MPARAM(ULONG(pFileBuf)), MPARAM(strlen(pFileBuf)));
  WinSendMsg(pFE->hWnd,TM_SETSEL,     MPARAM(&tpr1),           MPARAM(&tpr2));
  WinSendMsg(pFE->hWnd,TM_SETREADONLY,MPARAM(fReadOnly),       0);
  free (pFileBuf);
  return TRUE;
}

/* KnownFilename
>
>  Returns one of the editors contained the szFile file. If no one - returns 0
*/
HWND GROUPOFFICE::KnownFilename (PSZ szFile)
{
  for (int n=0; n<MAXFGROUPS; n++)
    for (PFILEED pFE=aFG[n].pFEdList; pFE; pFE=pFE->next)
      if (pFE->pszFilename && !sf_stricmp(pFE->pszFilename, szFile))
        return pFE->hWnd;
  return NULL;

}


/* AttachPainter
>
>  HWND   hEd          - the editor to associate painter with
>  PSZ    pszPainterId - painter identificator (or 0 to associate no painter)
>  BOOL   fMsgON       - Enable displaying possible error messages
>  Returns             - TRUE when Ok.
*/
BOOL GROUPOFFICE::AssociatePainter(HWND hEd, PSZ pszPainterId, BOOL fMsgON)
{
  return !!WinSendMsg(hEd,TM_SETPAINTER,MPARAM(pszPainterId),MPARAM(fMsgON));
}

/* QueryPainter
>
>  HWND   hEd          - the editor wich painter we are interested in
>  Returns             - the painter associated or 0
>
*/
PPAINTER GROUPOFFICE::QueryPainter(HWND hEd)
{
  PFILEED pFE = hEd2pFE(hEd);
  return  pFE ? pFE->pFInfo->pPainter : 0;
}

/* GotoEd
>
>  HWND hEd (in)       - Editor to go into
>  BOOL fShowOnly (in) - Denotes that the window doesn't recives the focus immediatly
*/
void GROUPOFFICE::GotoEd (HWND hEd, BOOL fShowOnly)
{
  WinSetWindowPos(WinQueryWindow(hEd,QW_PARENT),HWND_TOP,0,0,0,0,
                  SWP_SHOW|SWP_ZORDER|(fShowOnly ? 0 : SWP_ACTIVATE));
}

/* GotoGroup
>
>  ULONG nGroup (in)   - The group to activate
>  BOOL fShowOnly (in) - Denotes that the window doesn't recives the focus immediatly
>  Return value (BOOL) - FALSE if no windows in the group exists
*/
BOOL GROUPOFFICE::GotoGroup (ULONG nGroup, BOOL fShowOnly)
{
  if (nGroup<MAXFGROUPS)
  {
    PFILEED   pfe = aFG[nGroup].pFEdActive;
    if (!pfe) pfe = aFG[nGroup].pFEdList;
    if (pfe)
    {
      GotoEd(pfe->hWnd,fShowOnly);
      return TRUE;
    }
    else Beep();
  }
  return FALSE;
}

void GROUPOFFICE::GoNextGroup (BOOL fPrevious)
{
  ULONG ng;
  do    ng = (nActiveGroup + (fPrevious ? MAXFGROUPS-1 : 1))%MAXFGROUPS;
  while (!aFG[ng].pFEdList && ng!=nActiveGroup);
  if (ng!=nActiveGroup)
  {
    GotoGroup(ng,FALSE);
  }
}
void GROUPOFFICE::GoNextEditor(BOOL fPrevious)
{
  PFILEED pFE    = aFG[nActiveGroup].pFEdList;
  PFILEED pFEAct = aFG[nActiveGroup].pFEdActive;
  if (!pFE) return;
  if (fPrevious)
    if (pFE == pFEAct) while(pFE->next)                      pFE=pFE->next;
    else               while(pFE->next && pFE->next!=pFEAct) pFE=pFE->next;
  else
    if (!pFEAct || !(pFE = pFEAct->next)) pFE = aFG[nActiveGroup].pFEdList;
  GotoEd(pFE->hWnd,FALSE);
}

LONG GROUPOFFICE::SetAllCaptions(LONG n1stIt)
{
  char szIt[CCHMAXPATH+10];
  for (int ng=0; ng<MAXFGROUPS; ng++)
    for (PFILEED pFE=aFG[ng].pFEdList; pFE; pFE=pFE->next)
    {
      pFE->QueryName(szIt,sizeof(szIt));
      if (strlen(szIt))
        pMDI->SetMenuItem (pFE->hFrame, szIt, n1stIt++, 0);
    }
  return n1stIt;
}

/* EdActivated
>
> The editors owner must notify us when a editor window becomes active.
> hEd=0 means that no active editor now
*/
void GROUPOFFICE::EdActivated (HWND hEd)
{
  if (!hEd)
    aFG[nActiveGroup].pFEdActive = 0;
  else
  {
    PFILEED pFE = hEd2pFE(hEd);
    if (pFE)
    {
      nActiveGroup = pFE->pFGroup->nGroup;
      aFG[nActiveGroup].pFEdActive = pFE;
      pAutoVarTable->AssignVar("ActiveFrame","Editor");
      if (pFE->pszFilename)
      {
        char  sz[CCHMAXPATH];
        char *pch;
        strcpy(sz,sf_fname2short(pFE->pszFilename));
        if (pch=strrchr(sz,'.')) *pch = 0;

        pAutoVarTable->AssignVar("FileName",   sz);
        pAutoVarTable->AssignVar("File",       pFE->pszFilename);
        pAutoVarTable->AssignVar("FilePath",   FName2Path(sz,pFE->pszFilename) ? sz : "");
        pAutoVarTable->AssignVar("FileExt",    FName2Ext (sz,pFE->pszFilename) ? sz : "");
      }
      else
      {
        pAutoVarTable->AssignVar("File",    "");
        pAutoVarTable->AssignVar("FileName","");
        pAutoVarTable->AssignVar("FileExt", "");
        pAutoVarTable->AssignVar("FilePath","");

      }
      pToolbox->ReenableAll();
    }
  }
  BrDisplayComment("",0);
}
void GROUPOFFICE::CheckFileTimes(HWND hEd)
{
  PFILEED pFE;
  if (hEd) { if (pFE=hEd2pFE(hEd)) pFE->CheckUpdated(hMDI0Client);}
  else
  {
    for (int ng=0; ng<MAXFGROUPS; ng++)
      for (pFE=aFG[ng].pFEdList; pFE; pFE=pFE->next)
        pFE->CheckUpdated(hMDI0Client);
  }
}
void GROUPOFFICE::ChangeMaxNewState(int nState)
{
  if (nState && nState!=1) nState = !TEDWINPARAMS::fMaxNew;
  TEDWINPARAMS::fMaxNew = nState;
  WinSendMsg(WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU), MM_SETITEMATTR,
             MPFROM2SHORT(IDM_FILEMAXNEW,TRUE), MPFROM2SHORT(MIA_CHECKED,nState?MIA_CHECKED:0));
}
void GROUPOFFICE::ChangeAutosaveState(int nState)
{
  if (nState && nState!=1) nState = !fAutosave;
  fAutosave = nState;
  WinSendMsg(WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU), MM_SETITEMATTR,
             MPFROM2SHORT(IDM_FILEAUTOSAVE,TRUE), MPFROM2SHORT(MIA_CHECKED,nState?MIA_CHECKED:0));
}
void GROUPOFFICE::ChangeMakeBakState(int nState)
{
  if (nState && nState!=1) nState = !fMakeBak;
  fMakeBak = nState;
  WinSendMsg(WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU), MM_SETITEMATTR,
             MPFROM2SHORT(IDM_FILEBAK,TRUE), MPFROM2SHORT(MIA_CHECKED,nState?MIA_CHECKED:0));
}
void GROUPOFFICE::Change1AutoWinState(int nState)
{
  if (nState && nState!=1) nState = !f1AutoWin;
  f1AutoWin = nState;
  WinSendMsg(WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU), MM_SETITEMATTR,
             MPFROM2SHORT(IDM_FILE1AUTOWIN,TRUE), MPFROM2SHORT(MIA_CHECKED,nState?MIA_CHECKED:0));
}

PSZ  GROUPOFFICE::QueryEdFile(HWND hEd)
{
  PFILEED pFE = hEd2pFE(hEd);
  return pFE ? pFE->pszFilename : 0;
}


HWND GROUPOFFICE::EnumEd()
{
  int     ng;
  for (ng=0; ng<MAXFGROUPS; ng++)
    if (pfeEnum = aFG[ng].pFEdList)
      return pfeEnum->hWnd;
  return 0;
}
HWND GROUPOFFICE::NextEnumEd()
{
  if (!pfeEnum) return 0;
  if (pfeEnum->next)
  {
    pfeEnum = pfeEnum->next;
    return pfeEnum->hWnd;
  }
  else
    for(int ng1 = pfeEnum->pFGroup->nGroup+1; ng1<MAXFGROUPS; ng1++)
      if (pfeEnum = aFG[ng1].pFEdList)
        return pfeEnum->hWnd;
  pfeEnum = 0;
  return 0;
}
void GROUPOFFICE::TextChanged(PTCD ptcd)
{
  PFILEED pFE = hEd2pFE(ptcd->hEd);
  if (pFE)
    pFE->pFInfo->TextChanged(ptcd);
}

/* Read profile
>
> LONG slot    - slot number to read from
> HWND nMsgWnd - Window to use as a parent to show a messages
> Return value - success state
> NOTE: all the current configuration & buffers will be LOSED!
>
*/
BOOL GROUPOFFICE::RProfile (LONG slot)
{
  BOOL     r_prf_ed(LONG slot, LONG nEd, PSZ szKey, PVOID pBuf, LONG cbBuf);
  LONG     ng, nEd;
  LONG     nTotal = 0;
  PFILEED  apFEAct[MAXFGROUPS];
  HWND     hActEd;
  HWND     hLastEd;
  LONG     nActGr;
  memset(apFEAct,0,sizeof(apFEAct));
  for (ng=0; ng<MAXFGROUPS; ng++)
  {
    aFG[ng].DestroyAll();
    aFG[ng].nGroup = ng;
  }

  {
    LONG m,s,b,a;
    PfReadLong(slot, "nTotalEds",     &nTotal, 0);
    PfReadLong(slot, "nActiveGroup",  &nActGr, 0);
    PfReadLong(slot, "fMaxNew",       &m,      0);
    PfReadLong(slot, "fAutosave",     &s,      0);
    PfReadLong(slot, "fMakeBak",      &b,      0);
    PfReadLong(slot, "f1AutoWin",     &a,      0);
    PfReadLong(slot, "fEdShortNames", (PLONG)&FILEED::fShortName,FALSE);
    ChangeMaxNewState  (!!m);
    ChangeAutosaveState(!!s);
    ChangeMakeBakState (!!b);
    Change1AutoWinState(!!a);
  }
  nNonamCount = 0;

  // Turn the winbar OFF
  WBCONFIGDATA wbcd;
  pMDI->WBarQueryConfig(&wbcd);
  ULONG ulWBV = wbcd.ulWBV;
  wbcd.ulWBV  = WBV_HIDDEN;
  pMDI->WBarSetConfig(&wbcd);
  WinUpdateWindow(hMDI0Client);
  for (nEd=0; nEd<nTotal; nEd++)
  {
    BOOL     fOk;
    EDCONF   rEdConf;
    CHAR     szFile[CCHMAXPATH];
    BOOL     fActive;
    PFILEED  pFE;
    fOk =  r_prf_ed(slot, nEd, "rEdConf", &rEdConf,          sizeof(rEdConf))
        && r_prf_ed(slot, nEd, "nGroup",  &ng,               sizeof(ng))
        && r_prf_ed(slot, nEd, "fActive", &fActive,          sizeof(fActive));
    if (fOk)
    {
      char sz[100];
      sprintf(sz,"e%u_szFile",nEd);
      fOk = PfReadRelFName(slot, sz, szFile, "", 0);
    }
    if (!fOk || ng>=MAXFGROUPS) continue;
    {
      TEDWINPARAMS twp(pMDI->QueryWindow(MDIQW_DESK));
      twp.fHidden = TRUE;
      if (!(pFE = aFG[ng].GOpenEditor(szFile, 0, &twp, 0, 0))) continue;
    }
    WinSendMsg(pFE->hWnd,TM_SETEDCONF,MPARAM(&rEdConf),0);
    hLastEd = pFE->hWnd;
    if (fActive || !apFEAct[ng])
    {
      apFEAct[ng] = pFE;
      if (ng==nActGr) hActEd = hLastEd;
    }
  }
  // In addition: restore Message & Source lists positions
  BOOL fMsg = TRUE;
  while(1)
  {
    LONG    al[5]; // x,y,dx,dy,ws
    if (sizeof(al) == PfReadItem (slot,fMsg ? "aMsgWPos" : "aSrcWPos", al, sizeof(al), 0,0))
    {
      HWND    h      =  fMsg ? clMsgList.hFrame : clSrcList.hFrame;
      WinSetWindowPos(h,0, al[0],al[1],al[2],al[3], SWP_SIZE|SWP_RESTORE);
      WinSetWindowPos(h,0, al[0],al[1],al[2],al[3], SWP_MOVE);
      if (al[4]&WS_MAXIMIZED)
        WinSetWindowPos(h,0, 0,0,0,0, SWP_MAXIMIZE);
    }
    if (fMsg) fMsg = FALSE;
    else      break;
  }
  // Restore winbar
  wbcd.ulWBV  = ulWBV;
  pMDI->WBarSetConfig(&wbcd);

  for (ng=0; ng<MAXFGROUPS; ng++)
  {
    for (PFILEED pFE=aFG[ng].pFEdList; pFE; pFE=pFE->next)
      WinShowWindow(pFE->hFrame,TRUE);
    // Restore the group focus window:
    aFG[ng].pFEdActive = apFEAct[ng];
  }
  //
  ::SetAllCaptions();
  if (hActEd)       GotoEd (hActEd,  FALSE); // The focus window
  else if (hLastEd) GotoEd (hLastEd, FALSE); //    is set back now.

  {
    WBCONFIGDATA wbcd;
    if (sizeof(wbcd)!=PfReadItem (slot,"WinBarCfg",&wbcd,sizeof(wbcd),0,0))
    {
      wbcd.ulWBV = WBV_MULTILINE | WBV_SHOWCOLOR;
      strcpy(wbcd.szPPFont,"8.Helv");
    }
    pMDI->WBarSetConfig(&wbcd);
  }
  ReorderWBar();
  return TRUE;
}
BOOL r_prf_ed(LONG slot, LONG nEd, PSZ szKey, PVOID pBuf, LONG cbBuf)
{
  char sz[100];
  sprintf(sz,"e%u_%s",nEd,szKey);
  return !!PfReadItem(slot,sz,pBuf,cbBuf,0,0);
}

/* Write to profile
>
> LONG slot      - slot number to write into
> BOOL fContents - if FALSE - only the free slot contents wrote
> HWND nMsgWnd   - Window to use as a parent to show a messages
> Return value   - success state
>
*/
/*
: Special fields:
:   nTotalEds  - total editors opened (w/o 'Noname's - it'll be losed)
:   e#_<name>  - the #-number editor's field
:
*/
BOOL GROUPOFFICE::WProfile (LONG slot, BOOL fContents)
{
  void     w_prf_ed(LONG slot, LONG nEd, PSZ szKey, PVOID pBuf, LONG cbBuf);
  EDCONF   rEdConf;
  LONG     ng;
  PFILEED  pFE;
  LONG     nEd = 0;
  LONG     nTotal0, nTotal1;
  if (fContents)
  {
    // Turn the winbar off
    WBCONFIGDATA wbcd;
    pMDI->WBarQueryConfig(&wbcd);
    ULONG ulWBV = wbcd.ulWBV;
    wbcd.ulWBV  = WBV_HIDDEN;
    pMDI->WBarSetConfig(&wbcd);
    for (ng=0; ng<MAXFGROUPS; ng++)
      for (pFE=aFG[ng].pFEdList; pFE; pFE=pFE->next)
      {
        if (!pFE->pszFilename) continue;
        BOOL fActive = aFG[ng].pFEdActive==pFE;
        WinSendMsg(pFE->hWnd,TM_QUERYEDCONF,MPARAM(&rEdConf),0);
        w_prf_ed(slot, nEd, "rEdConf", &rEdConf,          sizeof(rEdConf));
        w_prf_ed(slot, nEd, "nGroup",  &ng,               sizeof(ng));
        w_prf_ed(slot, nEd, "fActive", &fActive,          sizeof(fActive));
        {
          char sz[100];
          sprintf(sz,"e%u_szFile",nEd);
          PfWriteRelFName(slot, sz, pFE->pszFilename);
        }
        nEd++;
      }
    {
      // In addition: save Message & Source lists positions
      BOOL fMsg = TRUE;
      while(1)
      {
        LONG    al[5]; // x,y,dx,dy,ws
        HWND    h = fMsg ? clMsgList.hFrame : clSrcList.hFrame;
        memset(al,0,sizeof(al));
        al[4]   = WinQueryWindowULong(h,QWL_STYLE);
        if (al[4] & (WS_MAXIMIZED|WS_MINIMIZED))
        {
          al[0] = WinQueryWindowUShort(h,QWS_XRESTORE);
          al[1] = WinQueryWindowUShort(h,QWS_YRESTORE);
          al[2] = WinQueryWindowUShort(h,QWS_CXRESTORE);
          al[3] = WinQueryWindowUShort(h,QWS_CYRESTORE);
        }
        else
        {
          SWP swp;
          WinQueryWindowPos(h,&swp);
          al[0] =  swp.x;
          al[1] =  swp.y;
          al[2] =  swp.cx;
          al[3] =  swp.cy;
        }
        PfWriteItem (slot,fMsg ? "aMsgWPos" : "aSrcWPos", al, sizeof(al));
        if (fMsg) fMsg = FALSE;
        else      break;
      }
    }
    // Restore winbar
    wbcd.ulWBV  = ulWBV;
    pMDI->WBarSetConfig(&wbcd);
  }
  nTotal1 = nEd;
  if (PfReadLong(slot, "nTotalEds", &nTotal0, 0))
  { // Вытираем остальные лежавшие на этом слоте редакторы
    for (;nEd<nTotal0;nEd++)
    {
      w_prf_ed(slot, nEd, "rEdConf", 0,0);
      w_prf_ed(slot, nEd, "szFile",  0,0);
      w_prf_ed(slot, nEd, "nGroup",  0,0);
      w_prf_ed(slot, nEd, "fActive", 0,0);
    }
  }
  PfWriteLong(slot, "nTotalEds",    nTotal1);
  PfWriteLong(slot, "nActiveGroup", fContents ? nActiveGroup : 0);
  PfWriteLong(slot, "fMaxNew",      TEDWINPARAMS::fMaxNew);
  PfWriteLong(slot, "fAutosave",    fAutosave);
  PfWriteLong(slot, "fMakeBak",     fMakeBak);
  PfWriteLong(slot, "f1AutoWin",    f1AutoWin);
  PfWriteLong(slot, "fEdShortNames",FILEED::fShortName);
  {
    WBCONFIGDATA wbcd;
    pMDI->WBarQueryConfig(&wbcd);
    PfWriteItem (slot,"WinBarCfg",&wbcd,sizeof(wbcd));
  }
  return TRUE;
}
void w_prf_ed(LONG slot, LONG nEd, PSZ szKey, PVOID pBuf, LONG cbBuf)
{
  char sz[100];
  sprintf(sz,"e%u_%s",nEd,szKey);
  if (pBuf) PfWriteItem (slot,sz,pBuf,cbBuf);
  else      PfDeleteItem(slot,sz);
}


PFILEED GROUPOFFICE::hEd2pFE(HWND hEd)
{
  return (PFILEED)pMDI->QueryChildHandle(WinQueryWindow(hEd,QW_PARENT));
}

void GROUPOFFICE::ReorderWBar()
{
  int   nAlloc = 0;
  int   nOrder = 0;
  PHWND pH     = 0;
  for (int ng=1; ng!=-1; ng = (ng==MAXFGROUPS-1) ? 0 : (ng ? ng+1 : -1)) // {1, 2, .., MAXFGROUPS-1, 0}
    for(PFILEED pFE = aFG[ng].pFEdList; pFE; pFE=pFE->next)
    {
      if (nAlloc<=nOrder)
      {
        nAlloc   = nAlloc ? nAlloc*2 : 8;
        if (!(pH = (PHWND)realloc(pH,sizeof(HWND)*nAlloc))) return;
      }
      pH[nOrder++] = pFE->hFrame;
    }
  if (nOrder) pMDI->WBarSetOrder(pH,nOrder);
  free(pH);
}

int GROUPOFFICE::q_frame_ng(HWND hFrame)
{
  HWND     hClient = hFrame  ? WinWindowFromID(hFrame,FID_CLIENT) : 0;
  PFILEED  pFE     = hClient ? hEd2pFE(hClient)                   : 0;
  int      ng      = MAXFGROUPS;
  if (pFE)
  {
    for (ng=0; ng<MAXFGROUPS; ng++)
    {
      PFILEED pFE1;
      for (pFE1=aFG[ng].pFEdList; pFE1; pFE1=pFE1->next)
        if (pFE1==pFE) break;
      if (pFE1) break;
    }
  }
  return ng<MAXFGROUPS ? ng : -1;
}

BOOL GROUPOFFICE::mv2group(HWND hFrame, int ngTarg)
{
  int ngSrc = q_frame_ng(hFrame);
  if (ngSrc<0 || ngTarg<0 || ngTarg>=MAXFGROUPS || ngTarg==ngSrc) return ngSrc>=0;
  // Move it!
  PFILEED *ppFETarg, *ppFESrc, pFESrc;
  HWND     hClient = WinWindowFromID(hFrame,FID_CLIENT);
  for (ppFETarg = &aFG[ngTarg].pFEdList; *ppFETarg; ppFETarg = &(*ppFETarg)->next);
  for ( ppFESrc = &aFG[ngSrc].pFEdList;
       *ppFESrc && (*ppFESrc)->hWnd != hClient;
        ppFESrc = &(*ppFESrc)->next);
  if (!(pFESrc = *ppFESrc)) return FALSE;
  // Cut off
  *ppFESrc        = pFESrc->next;
  if (aFG[ngSrc].pFEdActive==pFESrc) aFG[ngSrc].pFEdActive = *ppFESrc;
  // Paste
  pFESrc->next    = *ppFETarg;
  *ppFETarg       = pFESrc;
  pFESrc->pFGroup = aFG+ngTarg;
  // Re-tune all
  ::SetAllCaptions();
  ReorderWBar();
  GotoEd(hClient,FALSE);
  return TRUE;
}

BOOL curved_call(HWND hFrame, int nGroup)
// Дергается из примочки к винбару: мувит фрейм в группу,
// если группа неверна - возвращает мувэйбльность фрейма
{
  return grOffice.mv2group(hFrame,nGroup);
}

MRESULT EXPENTRY dpSavCh(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  switch(ulMsg){
  case WM_INITDLG:
    CenterChildWindow(hWnd);
    WinFocusChange(HWND_DESKTOP,WinWindowFromID(hWnd,IDD_PBSAVEALL),0);
    return MRESULT(TRUE);
  case WM_COMMAND:
    if      (SHORT1FROMMP(m1)==IDD_PBHELP)
      DisplayHelpPanel(PANEL_SAVECHDLG);
    else if (SHORT1FROMMP(m2)==CMDSRC_PUSHBUTTON)
      WinDismissDlg(hWnd,SHORT1FROMMP(m1));
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1){
      DisplayHelpPanel(PANEL_SAVECHDLG);
      break;
    }
    switch (SHORT1FROMMP(m2)){
    case 'a':
    case 'A':
      WinDismissDlg(hWnd,IDD_PBSAVEALL);
      return 0;
    case 'p':
    case 'P':
      WinDismissDlg(hWnd,IDD_PBPROMPT);
      return 0;
    case 'd':
    case 'D':
      WinDismissDlg(hWnd,IDD_PBDONTSAVE);
      return 0;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

int save_prompt(PFILEED pFE, HWND hMsgWnd)
{
  if (!hMsgWnd || !WinSendMsg(pFE->hWnd,TM_QUERYCHANGED,0,0)) return MBID_NO;
  if (GROUPOFFICE::fAutosave)                                 return MBID_YES;
  char szQ1[CCHMAXPATH+100];
  char szQQ[CCHMAXPATH+200];
  pFE->QueryName(szQ1,sizeof(szQ1));
  sprintf(szQQ,"Text in the window '%s' is changed. Save it?",szQ1);
  return WinMessageBox(HWND_DESKTOP,hMsgWnd,szQQ,"Save changes",0,MB_MOVEABLE|MB_ICONQUESTION|MB_YESNOCANCEL);
}

int  TEDWINPARAMS::nSpill  = 0;
BOOL TEDWINPARAMS::fMaxNew = FALSE;
     TEDWINPARAMS::TEDWINPARAMS(HWND hDesk)
     {
       SWP swp;
       WinQueryWindowPos(hDesk,&swp);
       cx      = swp.cx * 3 / 4;
       cy      = swp.cy * 3 / 4;
       x0      = 0 + swp.cx*nSpill/40;
       y0      = swp.cy - cy - swp.cy*nSpill/40;
       fMax    = fMaxNew;
       fMin    = FALSE;
       fHidden = FALSE;
       nSpill  = (nSpill+1)%11;
     }

/*=====================================================================================*/
/*                                                                                     */
/*        ------------->>> F i l e   F i n d  <<<------------                          */
/*                                                                                     */
/*=====================================================================================*/

void APIENTRY filefind_thread(ULONG);

// Make global our thread's i/o data

#define     FF_FOUND       1
#define     FF_OPENED      2

TID         tid;
HWND        hThrDlg;                       // In: dialog window
LINELIST   *pllThrSrcFiles = 0;            // In: source filenames list
char        szThrFoundName [CCHMAXPATH];   // In/Out: ""-empty now / the file found
ULONG       ulThrFF;                       // Out FF_*
BOOL        fThrKill       = FALSE;
LINELIST   *pllFilesFound  = 0;
FINDSTRUCT  fsFF;

void kill_thread()
{
  if (!tid) return;
  fThrKill = TRUE;
  DosWaitThread(&tid,DCWW_WAIT);
  fThrKill = FALSE;
  tid = 0;
}

#define FFM_FILLHIST   WM_USER+10
#define FFM_STORECTRLS WM_USER+11
#define FFM_TERMINATE  WM_USER+12
#define FFM_LISTSNAP   WM_USER+13

MRESULT EXPENTRY dpFileFindDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  const  ULONG     ERROR_THREAD_NOT_TERMINATED = 294;
  static BOOL      fSelFiles                   = FALSE;
  static HWND      hLB;
  static HWND      hCB;
  static int       nLastSel = LIT_NONE;
  static int       nLastTop = LIT_NONE;

  switch(ulMsg){

  case WM_INITDLG:
  {
    CenterChildWindow(hWnd);
    tid = 0;
    hLB = WinWindowFromID(hWnd,IDD_LBFFOUND);
    hCB = WinWindowFromID(hWnd,IDD_CBFFIND);
    WinSendMsg(hCB,     EM_SETTEXTLIMIT, MPARAM(FINDLINELEN-1), 0);
    { // Add editor's selection to history?
      PFILEED pFE;
      if (pFE = grOffice.QueryActiveFEd())
      {
        char sz[FINDLINELEN];
        if (WinSendMsg(pFE->hWnd,TM_GETSELTEXT,MPARAM(sz),MPARAM(sizeof(sz))))
        {
          char *pch = strchr(sz,CR); if (pch) *pch = 0;
                pch = strchr(sz,LF); if (pch) *pch = 0;
          if (sz[0])
          {
            hstFind.Append(sz);
            strcpy(ed_szLastPattern,sz);
          }
        }
      }
    }
    WinCheckButton(hWnd,IDD_CBFFCASE,    fFindCaseSensitive);
    WinCheckButton(hWnd,IDD_CBFFWHOLE,   fFindWholeWord);
    WinCheckButton(hWnd,IDD_CBFFSELFILES,fSelFiles);
    for (LINELIST *pll=pllFilesFound; pll; pll=pll->next)
      WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(pll->text));
    WinSendMsg(hLB,LM_SELECTITEM, MPARAM(nLastSel==LIT_NONE ? 0 : nLastSel),MPARAM(TRUE));
    if (nLastTop != LIT_NONE) WinSendMsg(hLB,LM_SETTOPINDEX,MPARAM(nLastTop),0);
    WinSendMsg(hWnd, FFM_FILLHIST,0,0);
    if (pllFilesFound) WinSetFocus(HWND_DESKTOP,hLB);
    else               WinSetFocus(HWND_DESKTOP,hCB);
    return MRESULT(TRUE);
  }

  case FFM_FILLHIST:
  {
    WinSendMsg(hCB,LM_DELETEALL,0,0);
    for (PSZ psz=hstFind.Enum(); psz; psz=hstFind.NextEnum())
      WinSendMsg(hCB,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(psz));
    WinSendMsg(hCB,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
    break;
  }

  case FFM_STORECTRLS:
    memset(&fsFF,0,sizeof(fsFF));
    fsFF.fAction       = FA_FINDGLOBAL;
    fFindCaseSensitive = fsFF.fCaseSensitive = WinQueryButtonCheckstate(hWnd,IDD_CBFFCASE);
    fFindWholeWord     = fsFF.fWholeWord     = WinQueryButtonCheckstate(hWnd,IDD_CBFFWHOLE);
    fSelFiles          = WinQueryButtonCheckstate(hWnd,IDD_CBFFSELFILES);
    WinQueryWindowText(hCB,sizeof(fsFF.szFind),fsFF.szFind);
    if (fsFF.szFind[0])
    {
      hstFind.Append(fsFF.szFind);
      strcpy(ed_szLastPattern,fsFF.szFind);
    }
    break;

  case FFM_TERMINATE: // Terminate find
    WinStopTimer(hAB,hWnd,1);
    kill_thread();
    sf_freelist(pllThrSrcFiles);
    pllThrSrcFiles = 0;
    WinSendDlgItemMsg(hWnd, DID_CANCEL,BM_SETDEFAULT, MPARAM(FALSE), 0);
    WinSendDlgItemMsg(hWnd, DID_OK,    BM_SETDEFAULT, MPARAM(TRUE),  0);
    WinEnableControl (hWnd, DID_OK,TRUE);
    break;

  case FFM_LISTSNAP: // Store list contents
  {
    sf_freelist(pllFilesFound);
    pllFilesFound = 0;
    int nCnt      = (int)WinSendMsg(hLB,LM_QUERYITEMCOUNT,0,0);
    for (SHORT i=0; i<nCnt; i++)
    {
      char sz[CCHMAXPATH];
      if (WinSendMsg(hLB,LM_QUERYITEMTEXT,MPFROM2SHORT(i,sizeof(sz)),MPARAM(sz)))
        pllFilesFound = sf_applist(pllFilesFound, sz);
    }
    nLastTop = (int)WinSendMsg(hLB,LM_QUERYTOPINDEX, 0,                0);
    nLastSel = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    break;
  }

  case WM_TIMER:
    if (szThrFoundName[0])
    {
      if (ulThrFF==FF_OPENED)
      {
        DosSuspendThread(tid);
        HWND hEd = grOffice.KnownFilename(szThrFoundName);
        if (hEd && WinSendMsg(hEd,TM_FIND,MPARAM(&fsFF),0))
          ulThrFF = FF_FOUND;
        DosResumeThread(tid);
      }
      if (ulThrFF==FF_FOUND)
        WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(szThrFoundName));
      szThrFoundName[0] = '\0';
    }
    if (!tid || ERROR_THREAD_NOT_TERMINATED!=DosWaitThread(&tid,DCWW_NOWAIT))
    {
      WinSendMsg(hWnd,FFM_TERMINATE,0,0);
      WinSetFocus(HWND_DESKTOP, WinSendMsg(hLB,LM_SELECTITEM,MPARAM(0),
                                MPARAM(TRUE)) ? hLB : hCB);
    }
    break;
  case WM_CONTROL:
    if (m1 == MPFROM2SHORT(IDD_LBFFOUND,LN_ENTER))
    {
      WinSendMsg(hWnd,FFM_TERMINATE,0,0);
      int nItem = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      if (nItem==LIT_NONE) break;
      char sz[CCHMAXPATH];
      WinSendMsg(hLB,LM_QUERYITEMTEXT,MPFROM2SHORT(nItem,sizeof(sz)),MPARAM(sz));
      HWND hEd = grOffice.KnownFilename(sz);
      if (!hEd)
      {
        if (!grOffice.OpenEditor(IN_AUTO_WINDOW, sz, hMDI0Client, 0) || !(hEd = grOffice.KnownFilename(sz)))
        {
          char szEr[CCHMAXPATH+50] = "Can't open file: ";
          strcat(szEr,sz);
          WinMessageBox(HWND_DESKTOP,hWnd,szEr,NULL,0,MB_ERROR|MB_OK|MB_MOVEABLE);
          break;
        }
      }
      WinSendMsg(hWnd,FFM_STORECTRLS,0,0);
      WinSendMsg(hWnd,FFM_LISTSNAP,0,0);
      fsFF.fGotoFound = TRUE;
      WinSendMsg(hEd,TM_FIND,MPARAM(&fsFF),0);
      grOffice.GotoEd(hEd, FALSE);
      WinDismissDlg(hWnd,TRUE);
    }
    break;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch(SHORT1FROMMP(m1)){
    case DID_CANCEL:
      if (!tid)
      {
        WinSendMsg(hWnd,FFM_STORECTRLS,0,0);
        WinSendMsg(hWnd,FFM_LISTSNAP,0,0);
        WinDismissDlg(hWnd,0);
      }
      else DosKillThread(tid);
      break;
    case DID_OK: // 'Find'
    {
      PSZ psz;
      WinSendMsg(hWnd, FFM_STORECTRLS, 0, 0);
      WinSendMsg(hWnd, FFM_FILLHIST,0,0);

      sf_freelist(pllThrSrcFiles);
      pllThrSrcFiles = 0;
      for (psz=srcList.Enum(fSelFiles ? 1 : 0); psz; psz=srcList.NextEnum(fSelFiles ? 1 : 0))
        pllThrSrcFiles = sf_applist(pllThrSrcFiles,psz);
      if (!pllThrSrcFiles || !fsFF.szFind[0]) {Beep(); break;}
      hThrDlg = hWnd; // To post me WM_TIMER immediatly
      WinEnableControl(hWnd,DID_OK,FALSE);
      WinSendDlgItemMsg(hWnd, DID_OK,    BM_SETDEFAULT, MPARAM(FALSE), 0);
      WinSendDlgItemMsg(hWnd, DID_CANCEL,BM_SETDEFAULT, MPARAM(TRUE),  0);
      WinSendMsg(hLB,  LM_DELETEALL, 0, 0);
      DosCreateThread(&tid,&filefind_thread,0,CREATE_READY|STACK_SPARSE,8192L);
      WinStartTimer(hAB,hWnd,1,200);
      break;
    }
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_FILEFINDDLG);
      break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1){
      DisplayHelpPanel(PANEL_FILEFINDDLG);
      break;
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

////////--- File found thread ---////////////////
//

BOOL ffound(char *szFile)
{
  char  szFull[CCHMAXPATH];

  if (!FName2Full(szFull,szFile,FALSE)) return FALSE;
  if (grOffice.KnownFilename(szFull))
  {
    ulThrFF = FF_OPENED; // Scan it in the primary thread
    strcpy(szThrFoundName,szFull);
    return TRUE;
  }

  //-- The file find --
  const ULONG FFDBLSIZE = 512;
  const ULONG FFBUFSIZE = 65536; // FFBUFSIZE >> DBLBUFSIZE > pattern length
  HFILE hf;
  ULONG ul;
  BOOL  fWasWord = TRUE;
  BOOL  fFound;
  ULONG ulSeekBeg, ulTailSize;
  char *pchBuf,   *pchFind;

  char  szFind[sizeof(fsFF.szFind)];
  strcpy(szFind, fsFF.szFind);
  int   nFindLen = strlen(szFind);


  if (DosOpen(szFull,&hf,&ul,0,FILE_NORMAL,FILE_OPEN,
              OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE,(PEAOP2)0)) return FALSE;

  if (!(pchBuf = (char*)malloc(FFBUFSIZE+1))) {DosClose(hf); return FALSE;}

  for (ulSeekBeg=0, ulTailSize=FFBUFSIZE, fFound=FALSE;
       ulTailSize==FFBUFSIZE && !fFound;
       ulSeekBeg += FFBUFSIZE-FFDBLSIZE )
  {
    DosSetFilePtr(hf,ulSeekBeg,FILE_BEGIN,&ul);
    if (ul!=ulSeekBeg || DosRead(hf,pchBuf,FFBUFSIZE,&ulTailSize)) ulTailSize = 0;
    pchBuf[ulTailSize] = '\0';

    if (!ulTailSize) {DosClose(hf); free(pchBuf); return FALSE;}

    //-- Scan pchBuf[0..ulTailSize-1]  Corresponded :
    //   file seek is [ulSeekBeg.. ],  fWasWord == wordbreak before[ .. ]

    pchFind = pchBuf;
    while(1)
    {
      if (fsFF.fCaseSensitive)
        pchFind = strstr(pchFind+1, szFind);
      else
      {
        char *pchE = pchBuf+ulTailSize;
        char  ch   = sf_toupper(szFind[0]);
        while((++pchFind)<pchE &&
              !(sf_toupper(*pchFind)==ch && !sf_strnicmp(pchFind,szFind,nFindLen)) );
        if (pchFind==pchE) pchFind=0;
      }
      // pchFind = the word found OR 0
      if (pchFind && fsFF.fWholeWord)
      {
        if (pchFind==pchBuf && !fWasWord)                    continue;
        if (pchFind>pchBuf && !sf_iswordbreak(*(pchFind-1))) continue;
        if (!sf_iswordbreak(pchFind[nFindLen]))              continue;
      }
      break;
    } // while(1)

    fFound = !!pchFind;
    if (!pchFind) pchFind = pchBuf+FFBUFSIZE-FFDBLSIZE;
    fWasWord  = (pchFind==pchBuf) || sf_iswordbreak(*(pchFind-1));
  }
  DosClose(hf);
  free(pchBuf);
  if (fFound)
  {
    ulThrFF = FF_FOUND;
    strcpy(szThrFoundName,szFull);
  }
  return fFound;
}


void APIENTRY filefind_thread(ULONG ul)
{
  LINELIST *pll = pllThrSrcFiles;

  while(pll && !fThrKill)
  {
    while(szThrFoundName[0])
    {
      if (fThrKill) DosExit(EXIT_THREAD,0);
      DosSleep(0);
    }
    if (ffound(pll->text)) WinPostMsg(hThrDlg,WM_TIMER,0,0);
    pll = pll->next;
  }
  DosExit(EXIT_THREAD,0);
}


