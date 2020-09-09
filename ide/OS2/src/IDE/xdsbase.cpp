/*
*
*  File: XDSBASE.CPP
*
*  XDS Shell main file
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
#include "winmake\winmake.h"


/*>>> G l o b a l s
*/
HAB               hAB;
HMQ               hMsgQ;
HSWITCH           hSwitch;
BOOL              fConfigON     = FALSE;
PMDIFRAME         pMDI=0;
HWND              hMDI0Client;
CLIENTINFO        clSrcList (CLIENT_SRCLIST);
CLIENTINFO        clMsgList (CLIENT_MSGLIST);
ERROFFICE         errOffice;
SRCLIST           srcList;
GROUPOFFICE       grOffice;
PVARTABLE         pAutoVarTable;
PVARTABLE         pUserVarTable;
PTOOLBOX          pToolbox;
MACROS            Macros;
char              szPrjFile    [CCHMAXPATH] = "";
char              szHomePath   [CCHMAXPATH] = "";
char              szCurIOPath  [CCHMAXPATH] = "";
char              szCurPath    [CCHMAXPATH] = ".";
STATUSINFO        statusInfo;
HISTORY           hstWPlaces (FALSE, HSTWPDEEP);
HISTORY           hstFiles   (FALSE, HSTFILESDEEP);
HISTORY           hstProjects(FALSE, HSTPRJDEEP);
HISTORY           hstFind    (TRUE,  HSTFINDDEEP);
HISTORY           hstFindChTo(TRUE,  HSTFINDDEEP);
PAINTEROFFICE     ptOffice;
char              chArgKey   = 0; // command line: key
PSZ               pszArgLine = 0; // command line: malloc()ed argument
BOOL              fFindCaseSensitive = FALSE;
BOOL              fFindWholeWord     = FALSE;
HWND              hZast              = 0;

/*>>> Function prototypes
*/
MRESULT EXPENTRY MainMsgHook   (HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY wpMsgList     (HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY wpSrcList     (HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY wpListsFrame  (HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY dpFindDlg     (HWND,ULONG,MPARAM,MPARAM);
void    EnableMenues           (ULONG emFlags); // EM_*
void    TuneFileMenu           ();
void    TuneOptMenu            ();
void    TuneWPMenu             ();
void    TunePrjMenu            ();
void    TuneToolsMenu          ();
void    ClickFileHist          (USHORT usCmd);
void    ClickWPHist            (USHORT usCmd);
void    ClickProjHist          (USHORT usCmd);
void    Print                  ();
MRESULT WMEdCharHandler        (HWND hWnd, MPARAM m1, MPARAM m2);
void    ListToClipb            (HWND hList);
void    Zastavka               ();
void    RegisterOurREXX        ();


int main(int argc, char **argv)
{
  try
  {
    SWCNTRL     swctl;
    QMSG        qMsg;
    PID         pid;
    ULONG       cpage,cpsize;

    if (! (hAB   = WinInitialize(0)))          return 1;
    if (! (hMsgQ = WinCreateMsgQueue(hAB,32))) return 1;

    DosGetCp(sizeof(cpage), &cpage, &cpsize);
    WinSetCp(hMsgQ, (USHORT)cpage);

    QueryCurrentPath(szCurPath);
    pAutoVarTable = new VARTABLE;
    pUserVarTable = new VARTABLE;
    pToolbox      = new TOOLBOX;
    RexxMon_Init();

    {/// --- Parse arguments
      PSZ pszALin = 0;
      if (argc>1 && (argv[1][0] == '-' || argv[1][0] == '/'))
      {
        chArgKey = sf_toupper(argv[1][1]);
        if (argv[1][2])  pszALin = argv[1]+2;  // -K<line>
        else if (argc>2) pszALin = argv[2];    // -K <line>
      }
      else if (argc>1) pszALin = argv[1];
      if (pszALin && (pszArgLine=(PSZ)malloc(strlen(pszALin)+1)))
        strcpy(pszArgLine,pszALin);
      // Now: chArgKey=key or 0; pszArgLine=line or 0
    }

    // Get our home path first
    {
      PTIB ptib;
      PPIB ppib;
      PCH  pch;
      DosGetInfoBlocks(&ptib,&ppib);
      DosQueryModuleName(ppib->pib_hmte,sizeof(szHomePath),szHomePath);
      if ((pch=strrchr(szHomePath,'\\')) || (pch=strrchr(szHomePath,'/')) ) *pch = '\0';
    }
    // Register some window classes
    WinRegisterClass(hAB,WC_QUERYACCEL,(PFNWP)wpQueryAccel,CS_SIZEREDRAW,12L);

    { /*>>>   C r e a t e   M D I   w i n d o w <<<*/
      SWP   swp;
      ULONG fcdata = FCF_NOBYTEALIGN | FCF_TITLEBAR   | FCF_SYSMENU | FCF_MINBUTTON |
                     FCF_MAXBUTTON   | FCF_SIZEBORDER | FCF_ICON    | FCF_MENU      |
                     FCF_ACCELTABLE  | FCF_SHELLPOSITION;
      WinQueryWindowPos(HWND_DESKTOP,&swp);
      pMDI = new MDIFRAME(&hMDI0Client, hAB, ICO_MDICLIENT, WS_VISIBLE, &fcdata, "XDS 2.31", 0,
                          RES_MAIN, 0,swp.cy/4,swp.cx,swp.cy*3/4);
      if (!hMDI0Client) return 1;

      pMDI->SetMsgHook(&MainMsgHook);
    }

    memset               (&swctl,0,sizeof(swctl));
    WinQueryWindowProcess(hMDI0Client, &pid, NULL);
    strcpy               (swctl.szSwtitle,TASK_NAME);
    swctl.hwnd          = WinQueryWindow(hMDI0Client,QW_PARENT); // MDIFrame
    swctl.idProcess     = pid;
    swctl.uchVisibility = SWL_VISIBLE;
    swctl.fbJump        = SWL_JUMPABLE;
    swctl.bProgType     = PROG_PM;
    hSwitch             = WinCreateSwitchEntry(hAB, &swctl);

    WinPostMsg(hMDI0Client,MM_START,0,0);

    while (1)
    {
      while(WinGetMsg(hAB,&qMsg,0,0,0))
      {
        WinDispatchMsg( hAB,&qMsg);
      }
      if (WinSendMsg(hMDI0Client,MM_KILLME,0,0))
        break;
      else
        WinCancelShutdown(hMsgQ,FALSE);
    }

    HelpDestroy();
    delete pMDI;
    WinDestroyMsgQueue(hMsgQ);
    WinTerminate(hAB);
    fConfigON = FALSE;
    WinRemoveSwitchEntry(hSwitch);
    exit(0);
  }
  catch (char *psz)
  {
    Beep(); Beep();
    char sz[300] = "Uncatched exception: ";
    strcat(sz,psz);
    WinMessageBox(HWND_DESKTOP,0,sz,"Fatal error",0,MB_ERROR|MB_OK|MB_MOVEABLE);
    return 5;
  }
}





/*-------------------------------------------------------------------------------------------*/
/*                                                                                           */
/*                  >>>>    M a i n   m e s s a g e   h o o k   <<<<                         */
/*                                                                                           */
/*-------------------------------------------------------------------------------------------*/

MRESULT EXPENTRY MainMsgHook(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  switch(ulMsg)
  {
  case MM_START:
    Zastavka();
    try
    {
      {
        HEXPR hExpr;
        LONG  l;
        PSZ   sz = "j*a";
        RCThunk_Compile(sz,&hExpr,&l);
        l = 0;
        hExpr = 0;
      }

      char   *psz;
      HWND    hFr,hCl;
      WinUpdateWindow(hMDI0Client);
      pAutoVarTable->AssignVar("Proj",        ""); // Full project file name (or "")
      pAutoVarTable->AssignVar("ProjName",    ""); // Project name w/o path & .ext (or "")
      pAutoVarTable->AssignVar("ProjPath",    ""); // Project file path (w/o last '\') (or "")
      pAutoVarTable->AssignVar("File",        ""); // Full current file name (or "" if none or when 'nonamed')
      pAutoVarTable->AssignVar("FileName",    ""); // Current file name w/o path & .ext (or "")
      pAutoVarTable->AssignVar("FileExt",     ""); // Current file extention (or "")
      pAutoVarTable->AssignVar("FilePath",    ""); // Current file path (w/0 last '\') (or "")
      pAutoVarTable->AssignVar("ActiveFrame", "");
      //--- Start to prepare color matrix
      FontClrInit();
      HelpInit();
      ptOffice.Init(hMDI0Client);

      //--- Register our REXX handlers
      RegisterOurREXX();

      //--- Init  TED  class
      if (psz = TedInit(hAB, ".", 0, PTR_MOVE,PTR_COPY)) throw psz;

      //--- Create TOOLBAR and STATUS LINE
      if (psz = BrCreate()) throw psz;
      SWP  swp;
      WinQueryWindowPos(pMDI->QueryWindow(MDIQW_DESK),&swp);

      //--- Create Message list window
      hFr = pMDI->CreateChild( MDIBIND_LEFTBOTTOM, WC_LISTBOX, "", WS_VISIBLE|LS_HORZSCROLL|LS_NOADJUSTPOS,
                                         FCF_TITLEBAR|FCF_SIZEBORDER|FCF_NOBYTEALIGN|FCF_HIDEMAX|FCF_SYSMENU,
                                         0,0,swp.cx*8/10,swp.cy*3/10, 0,0,0);
      if (hCl = WinWindowFromID(hFr,FID_CLIENT))
      {
        wpMsgList   (hCl, MM_START, MPARAM(ULONG(WinSubclassWindow(hCl,(PFNWP)wpMsgList))),   0L);
        wpListsFrame(hFr, MM_START, MPARAM(ULONG(WinSubclassWindow(hFr,(PFNWP)wpListsFrame))),0L);
        clMsgList.hWnd   = hCl;
        clMsgList.hFrame = hFr;
        WinSetWindowPos(hFr,HWND_TOP,0,0,0,0,SWP_SHOW|SWP_ACTIVATE|SWP_ZORDER);
      }
      else throw "Can't create message list window";
      if (hZast) WinSetFocus(HWND_DESKTOP,hZast);

      //--- Create Source files list window
      hFr = pMDI->CreateChild( MDIBIND_LEFTTOP, WC_LISTBOX, "", WS_VISIBLE|LS_HORZSCROLL|LS_NOADJUSTPOS,
                                         FCF_TITLEBAR|FCF_SIZEBORDER|FCF_NOBYTEALIGN|FCF_HIDEMAX|FCF_SYSMENU,
                                         0,swp.cy*2/10,swp.cx*3/10,swp.cy-swp.cy*2/10, 0,0,0);
      if (hCl = WinWindowFromID(hFr,FID_CLIENT))
      {
        wpSrcList   (hCl, MM_START, MPARAM(ULONG(WinSubclassWindow(hCl,(PFNWP)wpSrcList))),   0L);
        wpListsFrame(hFr, MM_START, MPARAM(ULONG(WinSubclassWindow(hFr,(PFNWP)wpListsFrame))),0L);
        clSrcList.hWnd   = hCl;
        clSrcList.hFrame = hFr;
        WinSetWindowPos(hFr,HWND_TOP,0,0,0,0,SWP_SHOW|SWP_ACTIVATE|SWP_ZORDER);
      }
      else throw "Can't create source files list window";
      if (hZast) WinSetFocus(HWND_DESKTOP,hZast);

      SetAllCaptions();

      if (chArgKey=='I')
      {
        int    rc = PfCreateProfile(pszArgLine, TRUE, HWND_DESKTOP) ? 0 : 2;
        fConfigON = FALSE;
        delete pMDI;
        HelpDestroy();
        WinDestroyMsgQueue(hMsgQ);
        WinTerminate(hAB);
        WinRemoveSwitchEntry(hSwitch);
        exit(rc);
      }
      else if (chArgKey=='P')
      {
        if (!PfOpenDefProfile(pszArgLine, hWnd)) throw "Can't open (or create) the profile specifyed.";
      }
      else if (!PfOpenDefProfile(0, hWnd))       throw "Can't open (or create) profile.";
      if (!chArgKey && pszArgLine) PfOpenWPlace(pszArgLine);
      else                         PfOpenWPlace(0);
    }
    catch (char* pszErr)
    {
      MessageBox(hWnd,0,MB_ERROR|MB_OK,pszErr,"\nApplication terminated.");
      exit(2);
    }
    if (hZast) WinSetFocus(HWND_DESKTOP,hZast);
    fConfigON=TRUE;
    EnableMenues(EM_ASKALL);
    break;
  case MM_KILLME:
    return (MRESULT)SaveConfig();
  case MM_CHECKFTIMES:
    grOffice.CheckFileTimes(HWND(m1));
    break;
  case MM_STARTPOPUPWIN:
    if (m1) ToolStartPopup (m1);
    else    ToolRecallPopup();
    break;
  case WM_COMMAND:
  {
    USHORT usCmd = SHORT1FROMMP(m1);
    if (usCmd>=IDM_FILEHIST0     && usCmd<IDM_FILEHIST0+HSTFILESDEEP)   {ClickFileHist(usCmd);     return 0;}
    if (usCmd>=IDM_WPHIST0       && usCmd<IDM_WPHIST0+HSTWPDEEP)        {ClickWPHist(usCmd);       return 0;}
    if (usCmd>=TOOL_MIN_ID       && usCmd<TOOL_MIN_ID+MAXTOOLS)         {pToolbox->Execute(usCmd); return 0;}
    if (usCmd>=IDM_PROJHIST0     && usCmd<IDM_PROJHIST0+HSTPRJDEEP)     {ClickProjHist(usCmd);     return 0;}
    if (usCmd>=IDM_HELP_INFFILE0 && usCmd<IDM_HELP_INFFILE0+MAXINFFILES){HelpClickMenu(usCmd);     return 0;}
    if (usCmd>=IDM_POP_GOBM0     && usCmd<IDM_POP_GOBM0+MAXBOOKMARKS)
    {
      PFILEED pFE = grOffice.QueryActiveFEd();
      if (pFE) pFE->GoBM(usCmd);
      return 0;
    }
    if (usCmd>=IDM_POP_SETBM0 && usCmd<IDM_POP_SETBM0+MAXBOOKMARKS)
    {
      PFILEED pFE = grOffice.QueryActiveFEd();
      if (pFE) pFE->SetBM(usCmd);
      return 0;
    }
    switch(usCmd)
    {
    case IDM_FILENEW:
      RxExecute("FileOpen('','IN_AUTO_WINDOW');");
      break;
    case IDM_FILELOAD:
      RxExecute("FileLoadDlg('FAST');");
      break;
    case IDM_FILERELOAD:
      RxExecute("FileReload();");
      break;
    case IDM_FILESAVE:
      RxExecute("FileSave();");
      break;
    case IDM_FILESAVEAS:
      RxExecute("FileSaveAs();");
      break;
    case IDM_FILESAVEALL:
      RxExecute("FileSaveAll();");
      break;
    case IDM_FILEPRINT:
      Print();
      break;
    case IDM_FILEAUTOSAVE:
      grOffice.ChangeAutosaveState();
      break;
    case IDM_FILEMAXNEW:
      grOffice.ChangeMaxNewState();
      break;
    case IDM_FILEBAK:
      grOffice.ChangeMakeBakState();
      break;
    case IDM_FILE1AUTOWIN:
      grOffice.Change1AutoWinState();
      break;
    case IDM_EDITUNDO   :    RxExecute("EdUndo();");                        break;
    case IDM_EDITREDO   :    RxExecute("EdRedo();");                        break;
    case IDM_EDITCUT    :    RxExecute("EdCopy(); EdDelete('SELECTION');"); break;
    case IDM_EDITCOPY   :    RxExecute("EdCopy('UNSELECT')");               break;
    case IDM_EDITPASTE  :    RxExecute("EdPaste();");                       break;
    case IDM_EDITSELALL :    RxExecute("Edmode('SELECTION','STREAM','NOTIFY');"
                                       "EdMove(-100000,-100000);"
                                       "EdMove(0,100000,'SELECT')"
                                       "EdEnd('SELECT');");                break;
    case IDM_EDITCLEAR  :    RxExecute("EdDelete('SELECTION');");          break;
    case IDM_EDITFIND:
      RxExecute("FindDlg();");
      break;
    case IDM_EDITFINDNEXT:
    case IDM_EDITFINDPREVIOUS:
    {
      char szOpt[10] = "LS";
      if (usCmd==IDM_EDITFINDPREVIOUS || WinGetKeyState(HWND_DESKTOP,VK_SHIFT) & 0x8000)
                              strcat(szOpt,"B");
      char szCmd[30];
      sprintf(szCmd,"EdFind(\"\", \"%s\");", szOpt);
      if (!hstFind.Enum()) {Beep(); break; } // Nothing to find
      if (!RxExecute(szCmd))
      {
        char sz[FINDLINELEN+100];
        sprintf(sz,"Can't find: '%s'",hstFind.Enum());
        WinMessageBox(HWND_DESKTOP,hMDI0Client,sz, strchr(szOpt,'B') ? "Find previous" : "Find next",0,MB_OK|MB_MOVEABLE);
      }
      break;
    }
    case IDM_EDITFILEFIND:    RxExecute("FindDlg('FILE');");             break;
    case IDM_EDITPREVERR :    RxExecute("ErrGoto('NEXT_IN_FILE');");     break;
    case IDM_EDITNEXTERR :    RxExecute("ErrGoto('PREVIOUS_IN_FILE');"); break;
    case IDM_EDITGOTOLINE:
      RxExecute("EdGoto(GetLine('Goto line','Line number:',8)-1, 0);");    break;
    case IDM_OPTSETTINGS:
      RxExecute("OptionsDlg('SETTINGS');");
      break;
    case IDM_WPNEW:
      WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpNewWPDlg,NULLHANDLE,IDD_NEWWPDLG,0);
      break;
    case IDM_OPTASS:
      RxExecute("OptionsDlg('ASSOCIATIONS');");
      break;
    case IDM_OPTMACROS:
      Macros.MacroDlg();
      break;
    case IDM_OPTRECALLMACROED:
      Macros.RecallMacroEd(FALSE);
      break;
    case IDM_OPTVARTABLE:
      EditVarTable(hMDI0Client);
      break;
    case IDM_WPLIST:
      WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpWPListDlg,NULLHANDLE,IDD_WPLISTDLG,0);
      break;
    case IDM_WPPROPERTIES:
      WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpWPPropDlg,NULLHANDLE,IDD_WPPROPDLG,0);
      break;
    case IDM_WPSAVEAS:
      RxExecute("WPSaveAs();");
      break;
    case IDM_WPTEMPLATES:
      WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpWPTplDlg,NULLHANDLE,IDD_WPTPLDLG,0);
      break;
    case IDM_TOOLCONFIGURE:
      pToolbox->EditTools();
      break;
    case IDM_PROJNEW:
      NewProject();
      break;
    case IDM_PROJOPEN:
      OpenProject(NULL,hWnd);
      break;
    case IDM_PROJCLOSE:
      OpenProject("",hWnd);
      break;
    case IDM_PROJMODIFY:
      if (szPrjFile[0])
        PrjEdit(hMDI0Client,szPrjFile,0);
      break;
    case IDM_PROJEDTEXT:
      EdProjectFile();
      break;
    case IDM_WINREPOPUP:
      ToolRecallPopup();
      break;
    case IDM_WINREXXMON:
      if (RexxMon_Toggle(1)) RexxMon_Activate();
      break;
    case IDM_WINCLOSEALL:
    {
      if (MBID_YES == MessageBox(hMDI0Client,"Close all windows",MB_YESNO|MB_ICONQUESTION,"Close all windows?"))
        if (grOffice.CloseWindow(0, hMDI0Client))
        {
          WinShowWindow(clMsgList.hFrame,FALSE);
          WinShowWindow(clSrcList.hFrame,FALSE);
          WinShowWindow(clSrcList.hFrame,FALSE);
          ToolHidePopup();
          RexxMon_Toggle(0);
        }
      break;
    }
    case IDM_WINCASCADE:
    {
      if (!grOffice.EnumEd() ||
          MBID_YES != MessageBox(hMDI0Client,"Cascade",MB_YESNO|MB_ICONQUESTION,"Cascade editor windows?"))
        break;
      SWP   swp;
      HWND  hEd;
      int   nSpill=0;
      LONG  x0, y0, cx, cy;
      WinQueryWindowPos(pMDI->QueryWindow(MDIQW_DESK),&swp);
      for (hEd=grOffice.EnumEd(); hEd; hEd=grOffice.NextEnumEd())
      {
        HWND hFr = WinQueryWindow(hEd,QW_PARENT);
        cx       = swp.cx * 3 / 4;
        cy       = swp.cy * 3 / 4;
        x0       = 0 + swp.cx*nSpill/40;
        y0       = swp.cy - cy - swp.cy*nSpill/40;
        nSpill   = (nSpill+1)%11;
        WinSetWindowPos(hFr,0, 0, 0, 0, 0,SWP_RESTORE);
        WinSetWindowPos(hFr,0,x0,y0,cx,cy,SWP_SIZE|SWP_MOVE|SWP_SHOW);
      }
      break;
    }
    case IDM_WINMAXALL:
    {
      if (!grOffice.EnumEd() ||
          MBID_YES != MessageBox(hMDI0Client,"Maximize",MB_YESNO|MB_ICONQUESTION,"Maximize all editor windows?"))
        break;
      HWND hEd;
      for (hEd=grOffice.EnumEd(); hEd; hEd=grOffice.NextEnumEd())
      {
        HWND hFr = WinQueryWindow(hEd,QW_PARENT);
        WinSetWindowPos(hFr,0, 0, 0, 0, 0,SWP_MAXIMIZE);
      }
      break;
    }
    case IDM_HELPINDEX:
      HelpIndex();
      break;
    case IDM_HELPGENERAL:
      HelpGeneral();
      break;
    case IDM_HELPUSINGHELP:
      HelpUsingHelp();
      break;
    case IDM_HELPKEYS:
      HelpKeys();
      break;
    case IDM_HELPPRODUCTINFO:
      HelpProductInfo();
      break;
    }
    break; // WM_COMMAND
  }
  case WM_CONTROL:
  {
    SHORT s2m1 = SHORT2FROMMP(m1);
    switch(s2m1)
    {
      case TLBN_COMMENT:
      case TN_COMMENT:
        BrDisplayComment(m2 ? (PSZ)m2 : "",4);
        break;
      case TN_DESTROYED:
        // Меню передернулись из grOffice.CloseWindow
        // если редакторов больше не осталось
        SetAllCaptions();
        break;
      case TN_KILLFOCUS:
      {
        pAutoVarTable->AssignVar("ActiveFrame", "");
        break;
      }
      case TN_LETMEDIE:
        grOffice.CloseWindow(HWND(m2),hMDI0Client);
        break;
      case TN_SELPRESENTS:
        EnableMenues(EM_ED | (m2 ? EM_SEL : EM_SEL_OFF));
        break;
      case TN_SETFOCUS:
      {
        grOffice.EdActivated (HWND(m2)); // $(File[Name]) тут установятся
        WinPostMsg(hWnd,MM_CHECKFTIMES,m2,0);
        EnableMenues(EM_ED);
        pToolbox->ReenableAll();
        break;
      }
      case TN_TEXTCHANGED:
        errOffice.TextChanged(PTCD(m2));
        grOffice.TextChanged(PTCD(m2));
        break;
      case TN_CHANGESTATE:
        switch (int(m2))
        {
          case 1:  statusInfo.stmode = STMODE_CHANGED;   break;
          case 2:  statusInfo.stmode = STMODE_READONLY;  break;
          default: statusInfo.stmode = 0;
        }
        break;
      case TN_POSCHANGED:
        statusInfo.lLine = PTPR(m2)->lLine+1;
        statusInfo.lCol  = PTPR(m2)->lPos+1;
        BrForceUpdate();
        break;
      case TN_PALETTECHANGED:
      {
        CLP_PALETTETYPE aa_rgb;
        WinSendMsg(HWND(m2),TM_QUERYCOLORS,MPARAM(aa_rgb),0);
        pMDI->WBarSetColors(WinQueryWindow(HWND(m2),QW_PARENT),
                            aa_rgb[CLP_DEFAULT][CLP_FOREGROUND],
                            aa_rgb[CLP_DEFAULT][CLP_BACKGROUND]);
        break;
      }
      case TN_UNAVAILABLE:
        EnableMenues(EM_ED | (m2 ? EM_UNDO : EM_UNDO_OFF));
        break;
    }
    break;
  }
  case WM_HELP:
    HelpGeneral();
    break;
  case WM_CHAR:
    return WMEdCharHandler(hWnd,m1,m2);
  case WM_INITMENU:
    switch SHORT1FROMMP(m1)
    {
      case IDM_FILE:      TuneFileMenu (); break;
      case IDM_OPTIONS:   TuneOptMenu  (); break;
      case IDM_WORKPLACE: TuneWPMenu   (); break;
      case IDM_PROJECT:   TunePrjMenu  (); break;
      case IDM_TOOLS:     TuneToolsMenu(); break;
    }
    break;
  default: return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

void SetAllCaptions()
{
  static BOOL fFirstCall = TRUE;
  int         nItem = 0;;
  if (fFirstCall)
    pMDI->SetMenuMode(FALSE); // manual window menu control
  fFirstCall = FALSE;

  pMDI->SetMenuItem (clSrcList.hFrame, "Source files\tAlt+\\",  nItem++, 0);
  WinSetWindowText  (clSrcList.hFrame, "Source files"); // 2 times need
  WinSetWindowText  (clSrcList.hFrame, "Source files"); //   (PM bug?)
  pMDI->SetMenuItem (clMsgList.hFrame, "Message list\tAlt+~", nItem++, 0);
  WinSetWindowText  (clMsgList.hFrame, "Message list");
  {
    char sz[WPNAMELEN+100] = "XDS 2.31 ";
    if (szWorkplace[0]) sprintf(sz+strlen(sz),"- %s",szWorkplace);
    if (szPrjFile[0])   sprintf(sz+strlen(sz),": %s", szPrjFile);
    WinSetWindowText(pMDI->QueryWindow(MDIQW_MAINFRAME),sz);
  }

  nItem = grOffice.SetAllCaptions(nItem);
}

void enable_menues(ULONG aIt[], BOOL fEnable)
{
  static HWND hMenu = 0;
  if (!hMenu) hMenu = WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU);
  for (int i=0; aIt[i]; i++)
  {
    WinEnableMenuItem(hMenu, aIt[i], fEnable);
    pToolbar->EnableButton(  aIt[i], fEnable);
  }
}

void EnableMenues(ULONG emFlags)
{
  static BOOL fEdON   = -1;
  static BOOL fSelON  = -1;
  static BOOL fUnON   = -1;
  static BOOL fPrjON  = -1;
  BOOL        fEd=0, fSel=0, fUn=0, fPrj=0;

  if (emFlags & EM_ASKALL)
  {
    emFlags = EM_ED | EM_UNDO | EM_SEL;
    PFILEED pFE=grOffice.QueryActiveFEd();
    if (!pFE)                                       emFlags |= EM_ED_OFF;
    else
    {
      TPR tpr1, tpr2;
      WinSendMsg(pFE->hWnd,TM_QUERYSEL,MPARAM(&tpr1),MPARAM(&tpr2));
      if (tpr1==tpr2)                               emFlags |= EM_SEL_OFF;
      if (!WinSendMsg(pFE->hWnd,TM_QUERYUNDO,0,0))  emFlags |= EM_UNDO_OFF;
    }
  }
  fEd  = ( emFlags & EM_ED  ) ? !(emFlags & EM_ED_OFFF  ) : fEdON;
  fSel = ((emFlags & EM_SEL ) ? !(emFlags & EM_SEL_OFFF ) : fSelON) && fEd;
  fUn  = ((emFlags & EM_UNDO) ? !(emFlags & EM_UNDO_OFFF) : fUnON ) && fEd;
  fPrj = (!!szPrjFile[0]);

  if (fSel!=fSelON)
  {
    ULONG aIt[] = { IDM_EDITCUT, IDM_EDITCOPY, IDM_EDITCLEAR, 0};
    enable_menues(aIt, fSelON=fSel);
  }
  if (fUn!=fUnON)
  {
    ULONG aIt[] = { IDM_EDITUNDO, 0 };
    enable_menues(aIt,fUnON=fUn);
  }
  if (fEd!=fEdON)
  {
    static ULONG aIt[] =
    { IDM_FILERELOAD,   IDM_FILESAVE,         IDM_FILESAVEAS,   IDM_FILEPRINT,
      IDM_EDITREDO,     IDM_EDITPASTE,        IDM_EDITSELALL,   IDM_EDITFIND,
      IDM_EDITFINDNEXT, IDM_EDITFINDPREVIOUS, IDM_EDITGOTOLINE, IDM_OPTSETTINGS, 0};
    enable_menues(aIt,fEdON=fEd);
  }
  if (fPrj!=fPrjON)
  {
    static ULONG aIt[] = { IDM_PROJCLOSE, IDM_PROJMODIFY, IDM_PROJEDTEXT, 0};
    enable_menues(aIt,fPrjON=fPrj);
  }
}


ULONG IOErMsg(APIRET err, HWND hWnd, char *szCapt, char *szErr, char *szFile, ULONG MBF)
{
  char    *psz   = szErr;
  switch((ULONG)err)
  {
  case 1:       psz = "Invalid funstion number";                                 break;
  case 2:       psz = "File not found";                                          break;
  case 3:       psz = "Path not found";                                          break;
  case 4:       psz = "Too many opened files (no handles left)";                 break;
  case 5:       psz = "Access denied";                                           break;
  case 6:       psz = "Invalid handle";                                          break;
  case 8:       psz = "Insufficient memory";                                     break;
  case 10:      psz = "Invalid environment";                                     break;
  case 11:      psz = "Invalid format";                                          break;
  case 12:      psz = "Invalid access";                                          break;
  case 13:      psz = "Invalid data";                                            break;
  case 19:      psz = "Disk is write protected";                                 break;
  case 26:      psz = "Uniknown media type";                                     break;
  case 29:      psz = "Write fault";                                             break;
  case 32:      psz = "Sharing violation";                                       break;
  case 33:      psz = "Lock violation";                                          break;
  case 36:      psz = "Sharing buffer overflov";                                 break;
  case 82:      psz = "Cannot make directory entry";                             break;
  case 84:      psz = "Too many pipes";                                          break;
  case 87:      psz = "Invalid parameter";                                       break;
  case 89:      psz = "No process slots available";                              break;
  case 95:      psz = "Interrupted system call";                                 break;
  case 99:      psz = "Device in use";                                           break;
  case 108:     psz = "Drive locked by another process";                         break;
  case 109:     psz = "Broken pipe";                                             break;
  case 110:     psz = "Open/create failed due to explicit fail command";         break;
  case 112:     psz = "No enough space on the disk";                             break;
  case 114:     psz = "Invalid target handle";                                   break;
  case 127:     psz = "Procedure address not found";                             break;
  case 182:     psz = "Invalid ordinal";                                         break;
  case 190:     psz = "Invalid module type";                                     break;
  case 191:     psz = "Invalid EXE signature";                                   break;
  case 192:     psz = "EXE marked invalid";                                      break;
  case 195:     psz = "Invalid minimum allocation size";                         break;
  case 196:     psz = "Dynamic link from invalid privilege level";               break;
  case 206:     psz = "File name or extention is greater than 8.3 characters";   break;
  case 231:     psz = "Pipe is busy";                                            break;
  }

  char szErrMsg[CCHMAXPATH+90];
  if (psz != szErr) sprintf(szErrMsg, "Error %u: %s", int(err), psz);
  else              strcpy(szErrMsg, psz);
  if (szFile && szFile[0])
  {
    strcat(szErrMsg,"\n(File ");
    strcat(szErrMsg,szFile);
    strcat(szErrMsg,")");
  }
  return MessageBox(hWnd, szCapt, MB_ERROR|MBF, szErrMsg);
}

ULONG MessageBox (HWND hOwner, PSZ pszCaption, ULONG flStyle, PSZ pszText1, PSZ pszText2, PSZ pszText3, PSZ pszText4)
{
  char sz[1000];
  strcpy(sz,pszText1);
  if (pszText2) strcat(sz,pszText2);
  if (pszText3) strcat(sz,pszText3);
  if (pszText4) strcat(sz,pszText4);
  return  WinMessageBox(HWND_DESKTOP,hOwner,sz,pszCaption,0,flStyle|MB_MOVEABLE);
}

MRESULT EXPENTRY wpMsgList(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PFNWP pWindowProc;

  switch (ulMsg)
  {
    case MM_START:
      pWindowProc = PFNWP(ULONG(m1));
      return 0;
    case WM_CHAR:
      if (!(SHORT1FROMMP(m1) & (KC_ALT|KC_CTRL|KC_SHIFT)) && SHORT2FROMMP(m2)==VK_NEWLINE)
      { // Она зачем-то, отработав, отдает этот enter овнеру, так что приходится делать ручками:
        int nIt = (int)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        if (nIt!=LIT_NONE)
          errOffice.GoError(nIt, TRUE);
        return 0;
      }
      break;
    case WM_CONTROL:
    {
      SHORT s2m1 = SHORT2FROMMP(m1);
      if (s2m1==LN_SELECT || s2m1==LN_ENTER)
        errOffice.GoError( (int)WinSendMsg(hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0),
                           s2m1==LN_ENTER || WinQueryFocus(HWND_DESKTOP)!=hWnd);
        if (s2m1==LN_SELECT && WinQueryFocus(HWND_DESKTOP)==hWnd)
          WinSetWindowPos(WinQueryWindow(hWnd,QW_PARENT),HWND_TOP,0,0,0,0,SWP_ZORDER);
      break;
    }
    case WM_FOCUSCHANGE:
      grOffice.EdActivated(0);
      pAutoVarTable->AssignVar("ActiveFrame", "MessageList");
      EnableMenues(EM_ASKALL);
      pToolbox->ReenableAll();
      break;
    case WM_COMMAND:
    {
      SHORT s1m1 = SHORT1FROMMP(m1);
      switch(s1m1){
      case IDM_POP_FONT: SetPPFont(hWnd);   break;
      case IDM_POP_COPY: ListToClipb(hWnd); break;
      case IDM_POP_DEFAULT:
      case IDM_POP_UNSORTED:
      {
        ERROFFICE::SORT_MODE sNew = (s1m1==IDM_POP_DEFAULT ? ERROFFICE::USUAL_ORDER : ERROFFICE::SENT_ORDER);
        if (sNew != errOffice.QuerySortMode())
        {
          errOffice.SetSortMode(sNew);
          errOffice.FillList();
        }
      }
      }
      return 0;
    }
    case WM_BUTTON2UP:
      if (WinQueryFocus(HWND_DESKTOP)==hWnd)
      {
        LONG aChk[2] = {0, 0};
        aChk     [0] = (ERROFFICE::USUAL_ORDER == errOffice.QuerySortMode())
                        ? IDM_POP_DEFAULT : IDM_POP_UNSORTED;
        PlayMenu(hWnd, IDM_POPUP_MSGLIST, aChk, 0, 0);
        return 0;
      }
      break;
    case WM_SYSCOMMAND:
      if (SHORT1FROMMP(m1)==SC_CLOSE)
        return WinSendMsg(WinQueryWindow(hWnd,QW_PARENT),WM_SYSCOMMAND,MPFROM2SHORT(SC_HIDE,SHORT2FROMMP(m1)),m2);
      break;
  }
  return (*pWindowProc)(hWnd,ulMsg,m1,m2);
}


MRESULT EXPENTRY wpSrcList(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PFNWP pWindowProc;

  switch (ulMsg){
    case MM_START:
      pWindowProc = PFNWP(ULONG(m1));
      return 0;
    case WM_FOCUSCHANGE:
      grOffice.EdActivated(0);
      pAutoVarTable->AssignVar("ActiveFrame", "SourceList");
      EnableMenues(EM_ASKALL);
      pToolbox->ReenableAll();
      break;
    case WM_COMMAND:
    {
      SHORT s1m1 = SHORT1FROMMP(m1);
      switch(s1m1){
        case IDM_POP_FONT:        SetPPFont(hWnd);                                 break;
        case IDM_POP_COPY:        ListToClipb(hWnd);                               break;
        case IDM_POP_FULLNAMES:   srcList.SetFullNames(!srcList.QueryFullNames()); break;
        case IDM_POP_SELFILES:    RxExecute("SelFiles('SELECT');");                break;
        case IDM_POP_UNSELFILES:  RxExecute("SelFiles('UNSELECT');");              break;
        case IDM_POP_INVSELFILES: RxExecute("SelFiles('INVERT');");                break;
        case IDM_POP_FILEFIND:    RxExecute("FindDlg('FILE');");                   break;
        case IDM_POP_FILEINVSEL:  srcList.ToggleCurItemSel();                      break;
        default:
          if (s1m1>=IDM_POP_AGROUP && s1m1<=IDM_POP_9GROUP)
          {
            int       nIt = (int)WinSendMsg(hWnd,LM_QUERYSELECTION, MPARAM(LIT_CURSOR),0);
            PLINELIST pll = (PLINELIST)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nIt), 0);
            if (!pll || !grOffice.OpenEditor(s1m1==IDM_POP_AGROUP ? IN_AUTO_WINDOW : (s1m1-IDM_POP_AGROUP),
                                             pll->text+2, hMDI0Client, 0))
              break;
            HWND hEd = grOffice.KnownFilename(pll->text+2);
            if (!hEd)
            {
              MessageBox(hMDI0Client,"Not fatal error",MB_ERROR|MB_OK,"An internal error occured (Location: ERROFFICE::wpSrcList)");
              break;
            }
            // hEd==this editor
            grOffice.GotoEd(hEd, FALSE);
            break;
          }
      }
      return 0;
    }
    case WM_CONTROL:
      if (SHORT2FROMMP(m1) == LN_ENTER)
      {
        int       nIt = (int)WinSendMsg(hWnd,LM_QUERYSELECTION, MPARAM(LIT_CURSOR),0);
        PLINELIST pll = (PLINELIST)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nIt),       0);
        if (!pll) break;
        HWND hEd = grOffice.KnownFilename(pll->text+2);
        if (!hEd && !grOffice.OpenEditor(IN_AUTO_WINDOW, pll->text+2, hMDI0Client, 0)) break;
        if (!(hEd = grOffice.KnownFilename(pll->text+2)))
        {
          MessageBox(hMDI0Client,"Not fatal error",MB_ERROR|MB_OK,"An internal error occured (Location: ERROFFICE::wpSrcList(2))");
          break;
        }
        // hEd==this editor
        grOffice.GotoEd(hEd, FALSE);
        return 0;
      }
      break;
    case WM_BUTTON2UP:
      if (WinQueryFocus(HWND_DESKTOP)==hWnd)
      {
        (*pWindowProc)(hWnd,WM_BUTTON1DOWN,m1,m2);
        LONG aChk[2] = {0, 0};
        LONG aDsb[]  = {IDM_POP_FILEINVSEL, IDM_POP_GROUPS, 0};
        int       nIt = (int)WinSendMsg(hWnd,LM_QUERYSELECTION, MPARAM(LIT_CURSOR),0);
        PLINELIST pll = (PLINELIST)WinSendMsg(hWnd,LM_QUERYITEMHANDLE,MPARAM(nIt),       0);
        if (pll) aDsb[0] = 0;
        if (srcList.QueryFullNames()) aChk[0] = IDM_POP_FULLNAMES;
        PlayMenu(hWnd, IDM_POPUP_SRCLIST, aChk, aDsb, 0);
        return 0;
      }
      break;
    case WM_CHAR:
    {
      USHORT usvk = SHORT2FROMMP(m2);
      USHORT fsf  = SHORT1FROMMP(m1);
      if (!(fsf & (KC_ALT|KC_CTRL|KC_SHIFT|KC_KEYUP)) && usvk==VK_NEWLINE)
        return WinSendMsg(hWnd,WM_CONTROL,MPFROM2SHORT(0,LN_ENTER),0);
      else if (!(fsf & (KC_ALT|KC_CTRL|KC_SHIFT|KC_KEYUP)) && usvk==VK_INSERT)
      {
        int nIt = (int)WinSendMsg(hWnd,LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
        int nC  = (int)WinSendMsg(hWnd,LM_QUERYITEMCOUNT, 0, 0);
        srcList.ToggleCurItemSel();
        if (nIt+1<nC)
          WinSendMsg(hWnd,LM_SELECTITEM,MPARAM(nIt+1),MPARAM(TRUE));
        break;
      }
      else if (fsf & (KC_ALT|KC_CTRL|KC_SHIFT) ||
          (usvk!=VK_UP && usvk!=VK_DOWN && usvk!=VK_PAGEUP && usvk!=VK_PAGEDOWN))
        return WinSendMsg(WinQueryWindow(hWnd,QW_OWNER),WM_CHAR,m1,m2);
      break;
    }
    case WM_SYSCOMMAND:
      if (SHORT1FROMMP(m1)==SC_CLOSE)
        return WinSendMsg(WinQueryWindow(hWnd,QW_PARENT),WM_SYSCOMMAND,MPFROM2SHORT(SC_HIDE,SHORT2FROMMP(m1)),m2);
      break;
  }
  return (*pWindowProc)(hWnd,ulMsg,m1,m2);
}

MRESULT EXPENTRY wpListsFrame(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PFNWP pWindowProc;

  switch (ulMsg){
  case MM_START:
  {
    pWindowProc      = PFNWP(ULONG(m1));
    return 0;
  }
  case WM_CHAR:
    return WMEdCharHandler(hWnd, m1, m2);
  default: return (*pWindowProc)(hWnd,ulMsg,m1,m2);
  }
}



/*>>> Translate key-description string into ACCEL definition
>
>   PSZ   *ppsz   (in/out) - pointer to pointer to source string, out: poistion after the parsed part of the string
>   ACCEL *pAccel (out)    - pointer to ACCEL structure
>   returns: TRUE - success, FALSE - error occured
>
>   NOTE: String syntax is like the next:
>         Ctrl+Shift+'e'  //
>         Alt+Space       //
>         Alx+0x45        // 0x45 is a scancode
>
*/
BOOL StrToAcc(PSZ *ppsz, ACCEL *pAccel)
{
  static SymTable stVKeys;
  static SymTable stFKeys;
  static BOOL     fFirstCall = TRUE;
  PSZ            &sz         = *ppsz;
  char           *pch;
  char            szBuf[30];
  LONG            l;

  if (fFirstCall)
  {
    fFirstCall = FALSE;
    stFKeys.AddName("Alt",       AF_ALT);
    stFKeys.AddName("Ctrl",      AF_CONTROL);
    stFKeys.AddName("Control",   AF_CONTROL);
    stFKeys.AddName("Ctl",       AF_CONTROL);
    stFKeys.AddName("Shift",     AF_SHIFT);
    stFKeys.AddName("Sh",        AF_SHIFT);

    stVKeys.AddName("Backspace", VK_BACKSPACE);
    stVKeys.AddName("BackSpace", VK_BACKSPACE);
    stVKeys.AddName("BkSp",      VK_BACKSPACE);
    stVKeys.AddName("Tab",       VK_TAB);
    stVKeys.AddName("BackTab",   VK_BACKTAB);
    stVKeys.AddName("BkTab",     VK_BACKTAB);
    stVKeys.AddName("Enter",     VK_NEWLINE);
    stVKeys.AddName("Esc",       VK_ESC);
    stVKeys.AddName("Escape",    VK_ESC);
    stVKeys.AddName("Space",     VK_SPACE);
    stVKeys.AddName("PgUp",      VK_PAGEUP);
    stVKeys.AddName("PageUp",    VK_PAGEUP);
    stVKeys.AddName("PgDn",      VK_PAGEDOWN);
    stVKeys.AddName("PageDown",  VK_PAGEDOWN);
    stVKeys.AddName("End",       VK_END);
    stVKeys.AddName("Home",      VK_HOME);
    stVKeys.AddName("Left",      VK_LEFT);
    stVKeys.AddName("Up",        VK_UP);
    stVKeys.AddName("Right",     VK_RIGHT);
    stVKeys.AddName("Down",      VK_DOWN);
    stVKeys.AddName("Ins",       VK_INSERT);
    stVKeys.AddName("Insert",    VK_INSERT);
    stVKeys.AddName("Del",       VK_DELETE);
    stVKeys.AddName("Delete",    VK_DELETE);
    stVKeys.AddName("F1",        VK_F1);
    stVKeys.AddName("F2",        VK_F2);
    stVKeys.AddName("F3",        VK_F3);
    stVKeys.AddName("F4",        VK_F4);
    stVKeys.AddName("F5",        VK_F5);
    stVKeys.AddName("F6",        VK_F6);
    stVKeys.AddName("F7",        VK_F7);
    stVKeys.AddName("F8",        VK_F8);
    stVKeys.AddName("F9",        VK_F9);
    stVKeys.AddName("F10",       VK_F10);
    stVKeys.AddName("F11",       VK_F11);
    stVKeys.AddName("F12",       VK_F12);
    stVKeys.AddName("F13",       VK_F13);
    stVKeys.AddName("F14",       VK_F14);
    stVKeys.AddName("F15",       VK_F15);
    stVKeys.AddName("F16",       VK_F16);
    stVKeys.AddName("F17",       VK_F17);
    stVKeys.AddName("F18",       VK_F18);
    stVKeys.AddName("F19",       VK_F19);
    stVKeys.AddName("F20",       VK_F20);
    stVKeys.AddName("F21",       VK_F21);
    stVKeys.AddName("F22",       VK_F22);
    stVKeys.AddName("F23",       VK_F23);
    stVKeys.AddName("F24",       VK_F24);
  }

  sz = sf_skipspaces(sz);
  memset(pAccel,0,sizeof(*pAccel));
  while(1)
  {
    if (sz [0]=='\'' && strlen(sz)>=3)
    { //--- '<char>'
      pAccel->fs  |= AF_CHAR;                                   // <<--- AF_CHAR
      pAccel->key  = sz[1];
      if (sz[2]!='\'') return FALSE;
      sz+=3;
      return TRUE;
    }
    else if (sf_isdigit(sz[0]))
    { //--- <scancode>
      pAccel->fs  |= AF_SCANCODE;                               // <<--- AF_SCANCODE
      pAccel->key  = strtol(sz,&sz,0);
      return TRUE;
    }
    else if (!strncmp(sz,"Gray",4))
    { //--- Gray...
      char       *pchNext = sf_skipspaces(sz+4);
      char        ch      = *pchNext;
      char       *pch;
      int         scode;
      static PSZ  szGrays   = "/+-*.0123456789";
      static int  aSCodes[] = {0x5c,0x4e,0x4a,0x37,0x53,0x52,0x4f,0x50,0x51,0x4b,0x4c,0x4d,0x47,0x48,0x49};
      if      (ch && (pch=strchr(szGrays,ch))) scode = aSCodes[pch-szGrays],  sz = pchNext + 1;
      else if (!strncmp(pchNext,"Enter",5))    scode = 0x5a                ,  sz = pchNext + 5;
      else                                     return FALSE;
      pAccel->fs  |= AF_SCANCODE;                               // <<--- AF_SCANCODE (Gray)
      pAccel->key  = scode;
      return TRUE;
    }
    else
    { //--- <Ident> + ... OR <Ident>
      int n;
      if (!sf_isalpha(sz[0]))                    return FALSE;  // <<--- eггoг
      for (pch=sz;sf_isalpha(*pch)||sf_isdigit(*pch);pch++)
      {
        n = pch-sz;
        if (n+2==sizeof(szBuf))                  return FALSE;  // <<--- eггoг
        szBuf[n] = *pch;
      }
      szBuf[pch-sz] = '\0';
      pch = sf_skipspaces(pch);
      if (*pch=='+')
      { //--- <Ident> + ...
        if ((l=stFKeys.SearchName(szBuf)) == -1) return FALSE;  // <<--- eггoг
        sz           = pch+1;
        pAccel->fs  |= l;
      }
      else
      { //--- <Ident>
        if ((l=stVKeys.SearchName(szBuf)) == -1) return FALSE;  // <<--- eггoг
        sz = pch;
        pAccel->fs  |= AF_VIRTUALKEY;                           // <<--- AF_VIRTUALKEY
        pAccel->key  = l;
        return TRUE;
      }
    }
  }
}

CPARAMSTR::CPARAMSTR () {memset(this,0,sizeof(*this)); cb=sizeof(this);}

MRESULT EXPENTRY dpEntryFDlg(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
//  CPARAMSTR fields:
//    psz1 (in)     - window caption
//    psz2 (in)     - line over the entry field
//    psz3 (in/out) - entry filed contents
//    l1            - size of psz3 buffer
//    l3            - help panel identifyer (or 0)
//    Returns:      - TRUE when Ok.
//
//
  static HWND        hEF;
  static PCPARAMSTR  pcps;

  switch (ulMsg){
  case WM_INITDLG:
    CenterChildWindow(hWnd);
    hEF  = WinWindowFromID(hWnd,IDD_EFEF);
    pcps = (PCPARAMSTR)m2;
    WinSetWindowText (hWnd,pcps->psz1);
    WinSetDlgItemText(hWnd,IDD_STLINE,pcps->psz2);
    WinSendMsg       (hEF,EM_SETTEXTLIMIT,MPARAM(pcps->l1),0);
    WinSetWindowText (hEF,pcps->psz3);
    WinSendMsg(hEF,EM_SETSEL,MPFROM2SHORT(0,10000),0);
    if (!pcps->l3) WinDestroyWindow(WinWindowFromID(hWnd,IDD_PBHELP));
    return 0;
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
  {
    USHORT usCmd = SHORT1FROMMP(m1);
    switch(usCmd)
    {
    case DID_OK:
      WinQueryWindowText(hEF,pcps->l1,pcps->psz3);
    case DID_CANCEL:
      WinDismissDlg(hWnd,(usCmd==DID_OK));
      break;
    case IDD_PBHELP:
      if (pcps->l3) DisplayHelpPanel(pcps->l3);
      break;
    }
    break;
  }
  case WM_CHAR:
    if(pcps->l3 && SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(pcps->l3);
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}



MRESULT EXPENTRY dpSelDirDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
//  CPARAMSTR fields:
//    char  psz1[CCHMAXPATH] (in/out) - directory (in: "" => current)
//    PSZ   psz2             (in)     - Window caption (or 0 => "Select directory")
//    LONG  l3               (in)     - help panel or 0
//    Returns:                        - TRUE when Ok.
//
{
  static HWND        hEF;
  static HWND        hLB;
  static PSZ         pszDir;
  static PCPARAMSTR  pcps;

  switch(ulMsg){
  case WM_INITDLG:
  {
    CenterChildWindow(hWnd);
    pcps   = (PCPARAMSTR)m2;
    pszDir = pcps->psz1;
    hEF    = WinWindowFromID(hWnd,IDD_EFSELDIR);
    hLB    = WinWindowFromID(hWnd,IDD_LBSELDIR);
    if   (!pszDir[0]) QueryCurrentPath(pszDir);
    FName2Full(pszDir,pszDir,FALSE);
    if (strlen(pszDir)==2) strcat(pszDir,"\\");
    WinSendMsg(hWnd,WM_USER,0,0); // Goto pszDir
    if (!pcps->l3) WinDestroyWindow(WinWindowFromID(hWnd,IDD_PBHELP));
    break;
  }
  case WM_CONTROL:
  {
    int nItem;
    if (HWND(m2)!=hLB ||
       !(SHORT2FROMMP(m1)==LN_SELECT || SHORT2FROMMP(m1)==LN_ENTER) ||
       LIT_NONE==(nItem=(int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0)))
      goto defret;
    //--- LN_SELECT or LN_ENTER ---//
    char sz   [CCHMAXPATH];
    char szRes[CCHMAXPATH];
    strcpy(szRes,pszDir);
    WinSendMsg(hLB,LM_QUERYITEMTEXT,MPFROM2SHORT(nItem,100),MPARAM(sz));
    if (!strcmp(sz,".."))
    {
      char * pch = strrchr(szRes,'\\');
      if (pch)
        if (pch-szRes>2)  pch[0] = 0;
        else              pch[1] = 0;
    }
    else if (sz[0]=='[' && !strcmp(sz+2,":\\]"))
    {
      strcpy(szRes," :\\");
      szRes[0] = sz[1];
    }
    else // Name
    {
      if (strlen(szRes)>3) strcat(szRes,"\\");
      strcat(szRes,sz);
    }
    if (SHORT2FROMMP(m1)!=LN_ENTER)
    {
      WinSetWindowText(hEF,szRes);
      WinSendMsg(hEF,EM_SETSEL,MPFROM2SHORT(1000,10000),0);
      goto defret;
    }
    //--- LN_ENTER ---//
    strcpy(pszDir,szRes);
    WinSendMsg(hWnd,WM_USER,0,0);
    goto defret;
  }
  case WM_USER: // Goto pszDir
  {
    //--- Re-read hLB
    HDIR          hdirFindHandle = HDIR_SYSTEM;
    FILEFINDBUF3  FindBuffer     = {0};
    ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
    ULONG         ulFindCount    = 1;
    APIRET        rc             = 0;
    ULONG         i, ul, ulDrvs;
    char          sz[CCHMAXPATH];

    if (!FName2Full(pszDir,pszDir,FALSE))
    {
      WinSetWindowText(hEF,pszDir);
      WinSendMsg      (hEF,EM_SETSEL,MPFROM2SHORT(1000,1000),0);
      WinMessageBox(HWND_DESKTOP,hWnd,"Error in the directory name.",
                    0,0,MB_ERROR|MB_OK|MB_MOVEABLE);
      break;
    }
    else strcpy(sz,pszDir);
    WinSetWindowText(hEF,pszDir); WinSendMsg(hEF,EM_SETSEL,MPFROM2SHORT(1000,1000),0);

    WinSendMsg(hLB,LM_DELETEALL,0,0);
    if (strlen(pszDir)>3) strcat(sz,"\\*.*");
    else                 strcat(sz,  "*.*");

    DosError(FERR_DISABLEHARDERR);

    if (!DosFindFirst(sz, &hdirFindHandle,
                      MUST_HAVE_DIRECTORY | FILE_DIRECTORY | FILE_READONLY |
                      FILE_HIDDEN         | FILE_SYSTEM    | FILE_ARCHIVED ,
                      &FindBuffer, ulResultBufLen, &ulFindCount, FIL_STANDARD))
    {
      while (!rc)
      {
        if (strcmp(FindBuffer.achName,".") && strcmp(FindBuffer.achName,".."))
          WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),MPARAM(FindBuffer.achName));
        ulFindCount = 1;
        rc = DosFindNext(hdirFindHandle, &FindBuffer, ulResultBufLen, &ulFindCount);
      }
      DosFindClose(hdirFindHandle);
    }
    else WinMessageBox(HWND_DESKTOP,hWnd,"The directory is not valid.",
                       0,0,MB_ERROR|MB_OK|MB_MOVEABLE);

    DosError(FERR_ENABLEHARDERR);

    if (strlen(pszDir)>3) // No root dir
      WinSendMsg(hLB,LM_INSERTITEM,MPARAM(0),MPARAM(".."));
    DosQueryCurrentDisk(&i,&ulDrvs);
    for (i=0, ul=0x00000001; i <= 'Z'-'A'; ul<<=1, i++)
      if (ul&ulDrvs)
      {
        char sz[] = "[A:\\]";
        sz[1] += i;
        WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(sz));
      }
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
    case DID_OK:
    {
      WinQueryWindowText(hEF,CCHMAXPATH,pszDir);
      FName2Full(pszDir,pszDir,FALSE);
      WinDismissDlg(hWnd,TRUE);
      break;
    }
    case DID_CANCEL:
      pszDir = 0;
      WinDismissDlg(hWnd,0);
      break;
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_SELDIRDLG);
      break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1){
      DisplayHelpPanel(PANEL_SELDIRDLG);
      break;
    }
  default:
  defret:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


void menu4pll(USHORT idm, LONG nIt, LONG nItCount, PHISTORY phst)
{
  LONG      l;
  MENUITEM  mi;
  HWND      hmMain = WinWindowFromID(WinQueryWindow(hMDI0Client,QW_PARENT),FID_MENU);
  if (!WinSendMsg(hmMain,MM_QUERYITEM,MPFROM2SHORT(idm,TRUE),(MPARAM)&mi)) return;
  HWND      hm     = mi.hwndSubMenu;
  if (!hm) return;
  PSZ       psz    = phst->Enum();
  memset(&mi,0,sizeof(mi));
  for(l = nIt+nItCount-1; l>=nIt; l--)
    WinSendMsg(hm,MM_DELETEITEM,MPFROM2SHORT(l,TRUE),0);
  WinSendMsg(hm,MM_DELETEITEM,MPFROM2SHORT(IDM_LASTSEPARATOR,0),0);
  if (!psz) return;

  mi.iPosition   = MIT_END;
  mi.afStyle     = MIS_SEPARATOR;
  mi.id          = IDM_LASTSEPARATOR;
  WinSendMsg(hm,MM_INSERTITEM,MPARAM(&mi),MPARAM(""));
  for (l=nIt; psz && l<nIt+nItCount; psz=phst->NextEnum(), l++)
  {
    mi.afStyle = MIS_TEXT;
    mi.id      = l;
    WinSendMsg(hm,MM_INSERTITEM,MPARAM(&mi),MPARAM(psz));
  }
}
void TuneFileMenu (){ menu4pll(IDM_FILE,      IDM_FILEHIST0, HSTFILESDEEP, &hstFiles); }
void TuneWPMenu   (){ menu4pll(IDM_WORKPLACE, IDM_WPHIST0,   HSTWPDEEP,    &hstWPlaces); }
void TunePrjMenu  (){ menu4pll(IDM_PROJECT,   IDM_PROJHIST0, HSTPRJDEEP,   &hstProjects); }
void TuneOptMenu  (){ WinEnableMenuItem(WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU),
                                        IDM_OPTRECALLMACROED, Macros.RecallMacroEd(TRUE));}
void TuneToolsMenu(){ pToolbox->TuneMenu(); }

void ClickFileHist(USHORT usCmd)
{
  MENUITEM  mi;
  HWND      hmMain = WinWindowFromID(WinQueryWindow(hMDI0Client,QW_PARENT),FID_MENU);
  if (!WinSendMsg(hmMain,MM_QUERYITEM,MPFROM2SHORT(IDM_FILE,TRUE),(MPARAM)&mi)) return;
  HWND      hm     = mi.hwndSubMenu;
  if (!hm) return;
  char     sz[CCHMAXPATH] = "";
  WinSendMsg(hm,MM_QUERYITEMTEXT,MPFROM2SHORT(usCmd,sizeof(sz)),MPARAM(sz));
  if (!sz[0]) return;
  HWND     hEd = grOffice.KnownFilename (sz);
  if (hEd) grOffice.GotoEd(hEd, FALSE);
  else     grOffice.OpenEditor(IN_AUTO_WINDOW, sz, hMDI0Client, 0);
}

void ClickWPHist(USHORT usCmd)
{
  MENUITEM  mi;
  HWND      hmMain = WinWindowFromID(WinQueryWindow(hMDI0Client,QW_PARENT),FID_MENU);
  if (!WinSendMsg(hmMain,MM_QUERYITEM,MPFROM2SHORT(IDM_WORKPLACE,TRUE),(MPARAM)&mi)) return;
  HWND      hm     = mi.hwndSubMenu;
  if (!hm) return;
  char     szWP[WPNAMELEN] = "";
  WinSendMsg(hm,MM_QUERYITEMTEXT,MPFROM2SHORT(usCmd,sizeof(szWP)),MPARAM(szWP));
  if (!szWP[0]) return;
  if (SaveConfig()) PfOpenWPlace(szWP);
}

void ClickProjHist(USHORT usCmd)
{
  MENUITEM  mi;
  HWND      hmMain = WinWindowFromID(WinQueryWindow(hMDI0Client,QW_PARENT),FID_MENU);
  if (!WinSendMsg(hmMain,MM_QUERYITEM,MPFROM2SHORT(IDM_PROJECT,TRUE),(MPARAM)&mi)) return;
  HWND      hm     = mi.hwndSubMenu;
  if (hm)
  {
    char     sz_prj[CCHMAXPATH] = "";
    WinSendMsg(hm,MM_QUERYITEMTEXT,MPFROM2SHORT(usCmd,sizeof(sz_prj)),MPARAM(sz_prj));
    if (sz_prj[0]) OpenProject(sz_prj,hMDI0Client);
  }
}

void Print()
{
  PFILEED      pFE    = grOffice.QueryActiveFEd();
  if (!pFE)    return;
  int          nSel   = (int)WinSendMsg(pFE->hWnd,TM_GETSELTEXT,0,0); // selection text length OR 0
  PSZ          pszBuf = 0;
  HFILE        hf;
  ULONG        ulAction,ul;

  if (MBID_YES != MessageBox(hMDI0Client, "Print", MB_ICONQUESTION|MB_MOVEABLE|MB_YESNO,
                             nSel ? "Do you want to print the text selected?" :
                                    "Do you want to print the text?"))
    return;
  if (DosOpen("prn",&hf,&ulAction,0,FILE_NORMAL,FILE_OPEN,
              OPEN_ACCESS_WRITEONLY|OPEN_SHARE_DENYNONE,(PEAOP2)0))
  {
    MessageBox(hMDI0Client, "Print error", MB_ERROR|MB_MOVEABLE|MB_OK,"Can not print a text.");
    return;
  }
  if (nSel)
  {
    if (pszBuf = (PSZ)malloc(nSel+1))
    {
      WinSendMsg(pFE->hWnd,TM_GETSELTEXT,MPARAM(pszBuf),MPARAM(nSel+1));
      pszBuf[nSel] = 0;
    }
  }
  else
    pszBuf = (PSZ)WinSendMsg(pFE->hWnd,TM_EXPORT,0,0);
  if (!pszBuf)
  {
    MessageBox(hMDI0Client, "Print error", MB_ERROR|MB_MOVEABLE|MB_OK,"Out of memory error.");
    return;
  }
  DosWrite(hf,PBYTE(pszBuf),strlen(pszBuf),&ul);
  DosClose(hf);
  free(pszBuf);
}


MRESULT WMEdCharHandler(HWND hWnd, MPARAM m1, MPARAM m2)
{

  QMSG qmsg;
  qmsg.hwnd      = hWnd;
  qmsg.msg       = WM_CHAR;
  qmsg.mp1       = m1;
  qmsg.mp2       = MPFROM2SHORT(sf_toupper(SHORT1FROMMP(m2)),SHORT2FROMMP(m2));
  qmsg.time      = 100;
  qmsg.ptl.x     = 1;
  qmsg.ptl.y     = 1;
  qmsg.reserved  = 0;

  // Eleminate KC_KEYUP:
  if (SHORT1FROMMP(m1) & KC_KEYUP) return 0;
  // Try macros:
  if (Macros.PlayMacro(&qmsg))  return 0;
  // Try tools:
  if (pToolbox->PlayTool(&qmsg)) return 0;
  else
  {
    if (SHORT2FROMMP(m2)==VK_SPACE)  m2 = MPFROM2SHORT(' ',0);
    char ch = CHAR1FROMMP(m2);
    if (ch && SHORT1FROMMP(m2) && !(SHORT1FROMMP(m1) & (KC_KEYUP|KC_ALT|KC_CTRL)))
    {
      static char sz[30];
      if (ch>=0x20)
      {
        if (ch!='\'') sprintf(sz,"EdType('%c');",   ch);
        else          sprintf(sz,"EdType(\"%c\");", ch);
        RxExecute(sz);
      }
      else Beep();
    }
  }

  return 0;
}

BOOL SaveConfig()
{
  if (!fConfigON) return TRUE;
  HWND          hMsgWnd = WinQueryActiveWindow(hMDI0Client);
  if (!hMsgWnd) hMsgWnd = hMDI0Client;
  return (!grOffice.SaveAll(TRUE,hMsgWnd) && PfStoreWPlace(0));
}

HISTORY::HISTORY(BOOL fCharSet, LONG lMaxDeep)
{
  memset(this,0,sizeof(*this));
  HISTORY::fCharSet = fCharSet;
  HISTORY::lMaxDeep = lMaxDeep;
}
HISTORY::~HISTORY()  {sf_freelist(pllHist);}
PLINELIST *HISTORY::find(PSZ szItem)
{
  BOOL      fCS  = fCharSet;
  for (PLINELIST *ppll=&pllHist; *ppll; ppll=&((*ppll)->next))
    if (fCS) {if (!strcmp    (szItem,(*ppll)->text)) break; }
    else     {if (!sf_stricmp(szItem,(*ppll)->text)) break; }
  return ppll;
}
BOOL HISTORY::Append(PSZ szItem)
{
  if (!szItem   || !fConfigON) return FALSE;
  if (!fCharSet && !szItem[0]) return FALSE;
  PLINELIST  pllNew = sf_applist(0,szItem);
  PLINELIST *ppll   = find(szItem);
  if (*ppll)
  {
    PLINELIST pll = *ppll;
    *ppll         = pll->next;
    pll->next     = 0;
    sf_freelist(pll);
  }
  pllNew->next = pllHist;
  pllHist      = pllNew;
  if (lMaxDeep>0)
  {
    PLINELIST pll;
    int       i;
    for(pll=pllHist, i=lMaxDeep; pll && i>0; pll=pll->next,i--);
    if (pll && pll->next)
    {
      sf_freelist(pll->next);
      pll->next = 0;
    }
  }
  return TRUE;
}
BOOL HISTORY::Delete(PSZ szItem)
{
  if (!fConfigON) return TRUE;
  PLINELIST *ppll = find(szItem);
  if (!*ppll) return FALSE;
  PLINELIST pll = *ppll;
  *ppll         = pll->next;
  pll->next     = 0;
  sf_freelist(pll);
}
PSZ  HISTORY::Enum()
{
  pllEnum = pllHist;
  if (pllEnum) return pllEnum->text;
  else         return 0;
}
PSZ  HISTORY::NextEnum()
{
  if (pllEnum) pllEnum=pllEnum->next;
  return (pllEnum) ? pllEnum->text : 0;
}
void HISTORY::Clear()
{
  sf_freelist(pllHist); pllHist=0; pllEnum=0;
}
void HISTORY::WProfile(LONG slot, PSZ szName)
{
  char sz[100];
  PfWriteLineList (slot, szName, pllHist);
  sprintf(sz,"%s_deep", szName);
  PfWriteLong     (slot, sz,     lMaxDeep);
  sprintf(sz,"%s_chset",szName);
  PfWriteLong     (slot, sz,     fCharSet);
}
void HISTORY::RProfile(LONG slot, PSZ szName)
{
  Clear();
  char sz[100];
  LONG l;
  pllHist = PfReadLineList(slot, szName);
  sprintf(sz,"%s_deep", szName); if (PfReadLong (slot, sz, &l, 15)) lMaxDeep = l;
  sprintf(sz,"%s_chset",szName); if (PfReadLong (slot, sz, &l,  0)) fCharSet = l;
}

void find_setFS(PFINDSTRUCT p, HWND hWnd)
{
  WinQueryWindowText(WinWindowFromID(hWnd,IDD_CBFIND),sizeof(p->szFind),p->szFind);
  p->fCaseSensitive  = WinQueryButtonCheckstate(hWnd, IDD_CBCASESENSITIVE);
  p->fWholeWord      = WinQueryButtonCheckstate(hWnd, IDD_CBWHOLEWORD);
  p->fGotoFound      = TRUE;
  p->fShowDownScreen = TRUE;
  p->tprPos.Set(-1,-1);
}

MRESULT EXPENTRY dpFindDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static LONG    x0, y0             = -1000;
  static HWND    hCBFind;
  static HWND    hCBChangeTo;
  static PFILEED pFE;

  switch(ulMsg)
  {
    case WM_INITDLG:
    {
      if (!(pFE=grOffice.QueryActiveFEd()))
      {
        WinDismissDlg(hWnd,0);
        break;
      }
      {
        SWP swp; WinQueryWindowPos(HWND_DESKTOP,&swp);
        if (x0>-100 && y0>-50 && x0<swp.cx-100 && y0<swp.cx-100)
          WinSetWindowPos(hWnd,0,x0,y0,0,0,SWP_MOVE);
      }
      hCBFind     = WinWindowFromID(hWnd,IDD_CBFIND);
      hCBChangeTo = WinWindowFromID(hWnd,IDD_CBCHANGETO);
      WinSendMsg(hCBFind,     EM_SETTEXTLIMIT, MPARAM(FINDLINELEN-1), 0);
      WinSendMsg(hCBChangeTo, EM_SETTEXTLIMIT, MPARAM(FINDLINELEN-1), 0);
      { // Initial selection?
        char sz[FINDLINELEN];
        if (WinSendMsg(pFE->hWnd,TM_GETSELTEXT,MPARAM(sz),MPARAM(sizeof(sz))))
        {
          char *pch = strchr(sz,CR); if (pch) *pch = 0;
                pch = strchr(sz,LF); if (pch) *pch = 0;
          if (sz[0])  hstFind.Append(sz);
        }
      }
      WinCheckButton(hWnd,IDD_CBCASESENSITIVE, fFindCaseSensitive);
      WinCheckButton(hWnd,IDD_CBWHOLEWORD,     fFindWholeWord);
      WinSendMsg(hWnd, WM_USER+1,  MPARAM(TRUE),       MPARAM(TRUE));
      WinSendMsg(hWnd, WM_CONTROL, MPARAM(IDD_CBFIND), 0);
      break;
    }
    case WM_CONTROL:  // only SHORT1FROMMP(m1) alloved
      if (SHORT1FROMMP(m1)==IDD_CBFIND          || SHORT1FROMMP(m1)==IDD_CBCHANGETO ||
          SHORT1FROMMP(m1)==IDD_CBCASESENSITIVE || SHORT1FROMMP(m1)==IDD_CBWHOLEWORD )
      {
        BOOL    fFind   = WinQueryWindowTextLength(hCBFind);
        BOOL    fFound  = (BOOL)WinSendMsg(hWnd,WM_USER,0,0);
        PFILEED pFE     = grOffice.QueryActiveFEd();
        BOOL    fWrite  = pFE ? !WinSendMsg(pFE->hWnd,TM_QUERYREADONLY,0,0) : TRUE;
        WinEnableControl(hWnd, IDD_PBFIND,       fFind);
        WinEnableControl(hWnd, IDD_PBFINDGLOBAL, fFind);
        WinEnableControl(hWnd, IDD_PBFINDBACK,   fFind);
        WinEnableControl(hWnd, IDD_PBCHANGE,     fFound & fWrite);
        WinEnableControl(hWnd, IDD_PBCHFIND,     fFound & fWrite);
        WinEnableControl(hWnd, IDD_PBCHALL,      fFound & fWrite);
      }
      break;
    case WM_USER: // returns fFound state
    {
      FINDSTRUCT fs;
      find_setFS(&fs,hWnd);
      fs.fAction   = FA_MATCHSEL;
      BOOL fFind   = !!fs.szFind[0];
      return MRESULT(fFind && (BOOL)WinSendMsg(pFE->hWnd,TM_FIND,MPARAM(&fs),0));
    }
    case WM_USER+1: // [Re-]fill lists (BOOL m1 - hCBFind, BOOL m2 - hCBChangeTo)
    {
      PSZ psz;
      if (m1)
      {
        WinSendMsg(hCBFind,LM_DELETEALL,0,0);
        for (psz=hstFind.Enum(); psz; psz=hstFind.NextEnum())
          WinSendMsg(hCBFind,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(psz));
        WinSendMsg(hCBFind,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
      }
      if (m2)
      {
        WinSendMsg(hCBChangeTo,LM_DELETEALL,0,0);
        for (psz=hstFindChTo.Enum(); psz; psz=hstFindChTo.NextEnum())
          WinSendMsg(hCBChangeTo,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(psz));
        WinSendMsg(hCBChangeTo,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
      }
      break;
    }
    case WM_CLOSE:   m1 = MPARAM(DID_CANCEL);
    case WM_COMMAND:
    {
      SHORT s1m1 = SHORT1FROMMP(m1);
      switch(s1m1)
      {
        case IDD_PBFIND:
        case IDD_PBFINDGLOBAL:
        case IDD_PBFINDBACK:
        {
          FINDSTRUCT fs;
          char       szOpt[10] = "";
          BOOL       fOk = FALSE;
          find_setFS(&fs,hWnd);
          if (fs.szFind[0])
          {
            hstFind.Append(fs.szFind);
            if (SHORT1FROMMP(m2)!=CMDSRC_OTHER) // NOT 'Change all' mode
              WinSendMsg(hWnd,WM_USER+1,MPARAM(1),0); // Re-fill hCBFind list
            if      (s1m1==IDD_PBFINDGLOBAL)  strcat(szOpt,"G");
            else if (s1m1==IDD_PBFINDBACK)    strcat(szOpt,"B");
            if (WinSendMsg(hWnd,WM_USER,0,0)) strcat(szOpt,"S");
            if (fs.fWholeWord)                strcat(szOpt,"W");
            if (fs.fCaseSensitive)            strcat(szOpt,"C");
            { // EdFind call:
              char szCmd [FINDLINELEN*2+30];
              char szPatt[FINDLINELEN*2];
              strcpy(szPatt,fs.szFind);
              RxDupQuotes(szPatt);
              sprintf(szCmd,"EdFind(\"%s\",\"%s\");",szPatt,szOpt);
              fOk = RxExecute(szCmd);
            }
            if (SHORT1FROMMP(m2)!=CMDSRC_OTHER) // NOT 'Change all' mode
            {
              WinSendMsg(hWnd,WM_CONTROL,MPARAM(IDD_CBFIND),0);
              if (!fOk)
              {
                char sz[FINDLINELEN+100];
                sprintf(sz,"Can't find: '%s'",fs.szFind);
                WinMessageBox(HWND_DESKTOP,hWnd,sz,"Find",0,MB_OK|MB_MOVEABLE);
              }
            }
          }
          WinSetFocus(HWND_DESKTOP,hCBFind);
          return MRESULT(fOk);
        }
        case IDD_PBCHANGE:
        {
          BOOL fFound  = (BOOL)WinSendMsg(hWnd,WM_USER,0,0);
          if (!fFound) break;
          char sz[FINDLINELEN*2] = "";
          WinQueryWindowText(hCBChangeTo,sizeof(sz),sz);
          hstFindChTo.Append(sz);
          RxDupQuotes(sz);
          char szEx[FINDLINELEN*2+20];
          sprintf(szEx,"EdPaste(\"%s\");",sz);
          BOOL fOk = RxExecute(szEx);
          if (SHORT1FROMMP(m2)!=CMDSRC_OTHER) // NOT 'Change all' mode
            WinSendMsg(hWnd,WM_CONTROL,MPARAM(IDD_CBFIND),0);
          return (MRESULT)fOk;
        }
        case IDD_PBCHFIND:
          if (WinSendMsg(hWnd,WM_COMMAND,MPARAM(IDD_PBCHANGE),m2))
            return WinSendMsg(hWnd,WM_COMMAND,MPARAM(IDD_PBFIND),m2);
          break;
        case IDD_PBCHALL:
        {
          static HWND hEd = 0;
          if (SHORT1FROMMP(m2)!=CMDSRC_OTHER) // 1st call
          {
            {
              PFILEED pFE = grOffice.QueryActiveFEd();
              hEd         = pFE ? pFE->hWnd : 0;
              if (!hEd) return 0;
            }
            char sz[FINDLINELEN*2] = "";
            WinQueryWindowText(hCBChangeTo,sizeof(sz),sz);
            hstFindChTo.Append(sz);
            WinSendMsg(hWnd,WM_USER+1,MPARAM(1),MPARAM(1)); // Re-fill hCB* lists
            WinSendMsg(hEd,TM_LOCKUPDATE,MPARAM(TRUE),0);
          }
          if (WinSendMsg(hWnd,WM_COMMAND,MPARAM(IDD_PBCHFIND),MPFROM2SHORT(CMDSRC_OTHER,0)))
            WinPostMsg(hWnd,WM_COMMAND,MPARAM(IDD_PBCHALL),MPFROM2SHORT(CMDSRC_OTHER,0));
          else
          {
            WinSendMsg(hWnd,WM_CONTROL,MPARAM(IDD_CBFIND),0);
            WinSendMsg(hEd,TM_LOCKUPDATE,MPARAM(FALSE),0);
          }
          break;
        }
        case DID_CANCEL:
        {
          SWP swp;
          WinQueryWindowPos(hWnd,&swp);
          x0 = swp.x;
          y0 = swp.y;
          fFindCaseSensitive  = WinQueryButtonCheckstate(hWnd, IDD_CBCASESENSITIVE);
          fFindWholeWord      = WinQueryButtonCheckstate(hWnd, IDD_CBWHOLEWORD);
          WinDismissDlg(hWnd,0);
          break;
        case IDD_PBHELP:
          DisplayHelpPanel(PANEL_FINDDLG);
          break;
        }

      }
      WinSetFocus(HWND_DESKTOP,hCBFind);
      break;
    }
    case WM_CHAR:
      if (SHORT2FROMMP(m2)==VK_F1)
      {
        DisplayHelpPanel(PANEL_FINDDLG);
        break;
      }
    default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

/*======================================================================================================*/
/*                                                                                                      */
/*  E R R O R   L I S T ( S )                                                                           */
/*                                                                                                      */
/*======================================================================================================*/

ERROFFICE::ERROFFICE()
{
  memset(this,0,sizeof(*this));
  sMode    = USUAL_ORDER;
  l1stLine = 1;
  l1stPos  = 1;
}
ERROFFICE::~ERROFFICE()
{
  for (PFILEERRLIST p1, p=pFEList; p; p=p1)
  {
    p1 = p->next;
    delete p;
  }
  pFEList = 0;
}

void ERROFFICE::ClearAll()
{
  WinSendMsg(clMsgList.hWnd,LM_DELETEALL,0,0);
  for (PFILEERRLIST p1, p=pFEList; p; p=p1)
  {
    p1 = p->next;
    delete p;
  }
  pFEList     = 0;
  ulSentCount = 0;
}
void ERROFFICE::SetSortMode(ERROFFICE::SORT_MODE sMode)
{
  ERROFFICE::sMode = sMode;
}
void ERROFFICE::DoneUpdating()
{
  FillList();
}
void ERROFFICE::AddError(PSZ szText, ULONG ulPos, ULONG ulLine, PSZ pszFile, ULONG ulErrClass, ULONG ulErrNum)
{
  char         szFN[CCHMAXPATH] = "";
  PFILEERRLIST pFE;
  PERRSTR      pES,*ppES;
  //--- Создадим pES - новую ERRSTR
  if (!(pES = new ERRSTR(szText))) return;
  pES->tprErr     . Set(max(ulLine-l1stLine,0),max(ulPos-l1stPos,0));
  pES->ulErrNum   = ulErrNum;
  pES->ulErrClass = ulErrClass;
  //--- Найдем или создадим нужный список pFE
  if (pszFile && !FName2Full(szFN,pszFile,FALSE)) strcpy(szFN,pszFile);
  for (pFE = pFEList; pFE; pFE=pFE->next)
    if (!sf_stricmp(pFE->szFilename,szFN)) break;
  if (!pFE)
  {
    if (!(pFE = new FILEERRLIST(szFN))) {delete pES; return;}
    pFE->next = pFEList;
    pFEList   = pFE;
  }
  //--- Вставим *pES в список pFE сохранив порядок по убыванию позиции
  pES->pFEL = pFE;
  for (ppES = &pFE->pErrList; *ppES; ppES = &(*ppES)->next)
    if ((*ppES)->tprErr < pES->tprErr) break;
  if ( *ppES &&
      (*ppES)->tprErr         == pES->tprErr &&
      (*ppES)->ulErrNum       == pES->ulErrNum &&
      (*ppES)->ulErrClass     == pES->ulErrClass &&
      !strcmp((*ppES)->pszError, pES->pszError)
     )
  {// Это дубликат. Случается...
    delete pES;
    return;
  }
  pES->ulSentOrder = ulSentCount++;
  pES->next        = *ppES;
  *ppES            = pES;
}
BOOL ERROFFICE::GoError(ULONG ulLboxItem, BOOL fActivateEd)
{
  PERRSTR       pES = (PERRSTR)WinSendMsg(clMsgList.hWnd,LM_QUERYITEMHANDLE,MPARAM(ulLboxItem),0);
  if (!pES)                return FALSE;
  PFILEERRLIST  pFE = pES->pFEL;
  if (!pFE->szFilename[0]) return TRUE;
  HWND          hEd = grOffice.KnownFilename(pFE->szFilename);
  if (!hEd && !grOffice.OpenEditor(IN_AUTO_WINDOW, pFE->szFilename, hMDI0Client, 0)) return FALSE;
  if (!(hEd = grOffice.KnownFilename(pFE->szFilename)))
  {
    MessageBox(hMDI0Client,"Not fatal error",MB_ERROR|MB_OK,"An internal error occured (Location: ERROFFICE::GoError)");
    return FALSE;
  }
  // hEd==this editor
  TPR tprAnc   = pES->tprErr;
  if (WinQueryFocus(HWND_DESKTOP)==clMsgList.hWnd && !fActivateEd)
    tprAnc.lPos+= 10;
  if (strcmp(pAutoVarTable->QueryVar("ActiveFrame"),"Editor"))
    WinSendMsg(hEd,TM_SETSELSHOWUP,MPARAM(&pES->tprErr),MPARAM(&tprAnc));
  else
    WinSendMsg(hEd,TM_SETSEL,      MPARAM(&pES->tprErr),MPARAM(&tprAnc));
  grOffice.GotoEd(hEd, !fActivateEd);
  BrDisplayComment(pES->pszError,30);
  return TRUE;
}
BOOL ERROFFICE::GoError(GO_ERROR goError)
{
  BOOL fPrev;
  if ((fPrev = goError==PREVIOUS_IN_LIST) || goError==NEXT_IN_LIST)
  {
    int nIt  = (int)WinSendMsg(clMsgList.hWnd,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    int nCnt = (int)WinSendMsg(clMsgList.hWnd,LM_QUERYITEMCOUNT,0,0);
    if      (nIt==LIT_NONE)        nIt=0;
    else if (fPrev && nIt>0)       nIt--;
    else if (!fPrev && nIt+1<nCnt) nIt++;

    if (nIt<nCnt)
    {
      WinSendMsg(clMsgList.hWnd,LM_SELECTITEM,MPARAM(nIt),MPARAM(FALSE));
      if (WinSendMsg(clMsgList.hWnd,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE)))
        return TRUE;
    }
  }
  else if ((fPrev = goError==PREVIOUS_IN_FILE) || goError==NEXT_IN_FILE)
  {
    while(1)
    {
      PFILEED      pFEd;
      PFILEERRLIST pFEr;
      PERRSTR      pES,pES_LT=0,pES_GT=0;
      PSZ          pszF;
      TPR          tpr;
      if (!(pFEd = grOffice.QueryActiveFEd()) ||
          !(pszF = grOffice.QueryEdFile(pFEd->hWnd)) ||
          ! pszF[0])
        break;
      for (pFEr = pFEList; pFEr; pFEr=pFEr->next)
        if (!sf_stricmp(pFEr->szFilename,pszF)) break;
      if (!pFEr) break;
      // pFEr - лист ошибок нашего файла
      WinSendMsg(pFEd->hWnd,TM_QUERYSEL,MPARAM(&tpr),0);
      for (pES=pFEr->pErrList; pES; pES=pES->next)
      {
        if (pES->tprErr<tpr) {pES_LT=pES; break;}
        if (pES->tprErr>tpr)  pES_GT = pES;
      }
      if (!(pES = (fPrev) ? pES_LT : pES_GT)) break;
      WinSendMsg(clMsgList.hWnd,LM_SELECTITEM,MPARAM(pES->lListItem),MPARAM(FALSE));
      if (!WinSendMsg(clMsgList.hWnd,LM_SELECTITEM,MPARAM(pES->lListItem),MPARAM(TRUE)))
        break;
      return TRUE;
    }
    Beep();
  }
  return FALSE;
}

int _Optlink cmperrs(const void * pv1, const void * pv2)
// For the 'qsort' call
{
  static ERROFFICE::SORT_MODE sMode = ERROFFICE::USUAL_ORDER;
  if (!pv1)
  {
    sMode = ERROFFICE::SORT_MODE((ULONG)pv2);
    return 0;
  }
  int     cmp = 0;
  PERRSTR pe1 = *(PERRSTR*)pv1;
  PERRSTR pe2 = *(PERRSTR*)pv2;
  if (sMode==ERROFFICE::SENT_ORDER)
    cmp = pe1->ulSentOrder-pe2->ulSentOrder;
  else
  {
    // --- 1 key - key(MSG_ERR) << other MSG_*
    if      (pe1->ulErrClass == 'E' && pe2->ulErrClass != 'E') cmp=-1;
    else if (pe1->ulErrClass != 'E' && pe2->ulErrClass == 'E') cmp= 1;
    // --- 2 key - filenames (w/o name - to the bottom)
    else if (cmp=sf_stricmp(pe1->pFEL->szFilename,pe2->pFEL->szFilename))
    {
      if (cmp<0 && !pe1->pFEL->szFilename[0]) cmp =  1; // W/O files
      if (cmp>0 && !pe2->pFEL->szFilename[0]) cmp = -1; //   - to the bottom
    }
    // --- 3 key - text position
    else if (pe1->tprErr<pe2->tprErr) cmp = -1;
    else if (pe1->tprErr>pe2->tprErr) cmp =  1;
    else cmp =0;
  }
  return cmp;
}

void ERROFFICE::FillList()
{
  int          nErrs,i;
  PERRSTR     *apErrs;
  PERRSTR      pES;
  PFILEERRLIST pFE;
  PSZ          psz;
  WinSendMsg(clMsgList.hWnd,LM_DELETEALL,0,0);
  //--- Заведем массив apErrs[0..nErrs-1] с указателями на все ошибки
  for (nErrs=0, pFE=pFEList; pFE; pFE=pFE->next)
    for (pES=pFE->pErrList; pES; pES=pES->next)
    {
      nErrs++;
      pES->lListItem = LIT_NONE;
    }
  if (!nErrs) return;
  if (!(apErrs = (PERRSTR*)malloc(nErrs * sizeof(PERRSTR)))) return;
  for (nErrs=0, pFE=pFEList; pFE; pFE=pFE->next)
    for (pES=pFE->pErrList; pES; pES=pES->next)
      apErrs[nErrs++] = pES;
  //--- Сортируем его
  cmperrs(NULL, PVOID(sMode));
  qsort(apErrs, nErrs, sizeof(PERRSTR), cmperrs);
  //--- И запллняем лист (handles == pointers to ERRSTRs)
  for (i=0; i<nErrs && (pES=apErrs[i]); i++)
  {
    if (!(psz = (PSZ)malloc(CCHMAXPATH + strlen(pES->pszError) + 50))) continue;
    strcpy(psz,pES->pFEL->pszShortName);
    if (pES->ulErrClass==MSG_WARNING || pES->ulErrClass==MSG_ERROR)
    {
      sprintf(psz+strlen(psz), " %05lu:%03u", pES->tprErr.lLine+1, pES->tprErr.lPos+1);
      if (pES->ulErrClass==MSG_ERROR)
        strcat(psz," Error");
      else
        strcat(psz," Warning");
      if (pES->ulErrNum) sprintf(psz+strlen(psz)," %u:",pES->ulErrNum);
      else               strcat (psz, ":");
    }
    strcat(psz," ");
    strcat(psz,pES->pszError);
    int nIt = (int)WinSendMsg(clMsgList.hWnd, LM_INSERTITEM,    MPARAM(LIT_END), MPARAM(psz));
                   WinSendMsg(clMsgList.hWnd, LM_SETITEMHANDLE, MPARAM(nIt),     MPARAM(pES));
    pES->lListItem = nIt;
    free(psz);
  }
  free(apErrs);
}

void ERROFFICE::TextChanged(PTCD ptcd)
{
  PSZ          pszFile;
  PFILEERRLIST pFE;
  PERRSTR      pES;
  if (!ptcd || !(pszFile = grOffice.QueryEdFile(ptcd->hEd)) || !pszFile[0])
    return;
  for (pFE = pFEList; pFE; pFE=pFE->next)
    if (!sf_stricmp(pszFile,pFE->szFilename)) break;
  if (pFE)
    for (pES=pFE->pErrList; pES; pES=pES->next)
      pES->tprErr.UpdatePos(ptcd);
}

FILEERRLIST::FILEERRLIST(PSZ szFilename)
{
  memset(this,0,sizeof(*this));
  if (szFilename) strcpy(FILEERRLIST::szFilename,szFilename);
  pszShortName = sf_fname2short(FILEERRLIST::szFilename);
}
FILEERRLIST::~FILEERRLIST()
{
  for (PERRSTR p1,p=pErrList; p; p=p1)
  {
    p1 = p->next;
    delete p;
  }
  pErrList = 0;
}
ERRSTR::ERRSTR(PSZ pszErr)
{
  memset(this,0,sizeof(*this));
  lListItem = LIT_NONE;
  PSZ pszE  = pszErr ? pszErr : "";
  pszError  = (PSZ)malloc(strlen(pszE)+1);
  char ch;
  for (int i=0; ch=pszError[i]=pszE[i]; i++)
    if (ch=='\r' || ch=='\n' || ch=='\t')
      pszError[i]=' ';
}
ERRSTR::~ERRSTR()
{
  free(pszError);
  pszError = 0;
}


/////////////////////// S o u r s e   f i l e s   l i s t : //////////


void SRCLIST::SetFullNames(BOOL fFullNames)
{
  if (fFullNames!=SRCLIST::fFullNames)
  {
    SRCLIST::fFullNames = fFullNames;
    fill_lbox();
  }
}
PSZ SRCLIST::Enum(int nWhat,BOOL f_private)
// nWhat = 0/1/2: all/sel/manual
{
  if (!f_private) // start scan
    nItEnum = 0-1;
  while(TRUE)
  {
    nItEnum++;
    PLINELIST pll = (PLINELIST)WinSendMsg(clSrcList.hWnd,LM_QUERYITEMHANDLE,MPARAM(nItEnum),0);
    if (!pll) return 0;
    if ( !nWhat                         ||
          nWhat==1 && pll->text[1]=='+' ||
          nWhat==2 && pll->text[0]=='M')
      return pll->text+2;
  }
}
void SRCLIST::AddFile(PSZ pszFile, BOOL fTagItManual)
{
  if (pszFile && pszFile[0])
  {
    char szItem[CCHMAXPATH+2];
    szItem[0] = fTagItManual ? 'M' : 'C';
    szItem[1] ='-';
    if (!FName2Full(szItem+2,pszFile,FALSE))
      strncpy(szItem+2,pszFile,CCHMAXPATH);
    szItem[sizeof(szItem)-1] = 0;
    pllSrcFiles = sf_applist(pllSrcFiles,szItem);
  }
}
void SRCLIST::ShowAppended()
{
  fill_lbox();
}
void SRCLIST::Select(char chMode, PSZ pszPattern)
{
  int       i,j;
  PLINELIST pll;
  HWND      hLB    = clSrcList.hWnd;
  int       nItems = (int)WinSendMsg(hLB,LM_QUERYITEMCOUNT,0,0);
  if (chMode == '*')
  {
    for (i=0; i<nItems; i++)
      if (pll = (PLINELIST)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(i),0))
        pll->text[1] ^= ('+' ^ '-');
  }
  else
  {
    HEXPR    hExpr;
    {
      LONG     rc;
      PSZ      psz;
      if (!(psz = OpenExprVar(pszPattern))) return;
      RCThunk_Compile(sf_strtoupper(psz), &hExpr, &rc);
      free(psz);
      if (!hExpr) return;
    }
    for (i=0; i<nItems; i++)
      if (pll = (PLINELIST)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(i),0))
      {
        char sz[CCHMAXPATH];
        for(j=0; sz[j]=sf_toupper(pll->text[2+j]); j++);
        if (RCThunk_Match(hExpr, fFullNames ? sz : sf_fname2short(sz)))
          pll->text[1] = chMode;
      }
    RCThunk_FreeExpr(hExpr);
  }
  show_selstates();
}
void SRCLIST::ToggleCurItemSel()
{
  HWND      hLB = clSrcList.hWnd;
  int       nIt = (int)WinSendMsg(hLB,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
  PLINELIST pll = (PLINELIST)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
  if (pll)
  {
    pll->text[1] ^= ('+' ^ '-');
    show_selstates(nIt);
  }
}
void SRCLIST::WProfile(LONG slot, PSZ szName)
{
  char sz[100];
  sprintf(sz,"%s_mode", szName);
  PfWriteLong  (slot, sz,     fFullNames);
  sprintf(sz,"%s_PPFont", szName);
  PfWritePPFont(slot, sz, clSrcList.hWnd);
  PfWriteLineList (slot, szName, pllSrcFiles);
}
void SRCLIST::RProfile(LONG slot, PSZ szName)
{
  char sz[100];
  sprintf(sz,"%s_PPFont", szName);
  PfApplyPPFont  (slot,sz,clSrcList.hWnd);
  sprintf(sz,"%s_mode",   szName);
  PfReadLong     (slot,sz,(PLONG)&fFullNames, TRUE);
  clear_spec(0,TRUE);
  pllSrcFiles = PfReadLineList(slot, szName);
  /*+++ Старое представление (без +/-) -> новое: */
  {
    PLINELIST pllNew = 0;
    for (PLINELIST pll=pllSrcFiles; pll; pll=pll->next)
    {
      if (strlen(pll->text)<2) continue;
      char sz[CCHMAXPATH+2];
      if (pll->text[1]=='+' || pll->text[1]=='-')
        strcpy(sz,pll->text);
      else
      {
        sz[0] = pll->text[0];
        sz[1] = '-';
        strcpy(sz+2,pll->text+1);
      }
      pllNew = sf_applist(pllNew,sz);
    }
    sf_freelist(pllSrcFiles);
    pllSrcFiles = pllNew;
  }
  ShowAppended();
}
void SRCLIST::fill_lbox()
{
  HWND hLB = clSrcList.hWnd;
  WinSendMsg(hLB,LM_DELETEALL,0,0);
  for (PLINELIST pll=pllSrcFiles; pll; pll=pll->next)
  {
    char sz[CCHMAXPATH+2];
    sprintf(sz,"  %s", fFullNames ? pll->text+2 : sf_fname2short(pll->text+2));
    if (sz[2])
    {
      int nIt = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),MPARAM(sz));
      WinSendMsg(hLB,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(pll));
    }
  }
  show_selstates();
}
void SRCLIST::show_selstates(int nIt)
{
  HWND      hLB = clSrcList.hWnd;
  PLINELIST pll;
  int       nItHi = nIt;
  if (nIt==LIT_NONE)
  {
    nIt   = 0;
    nItHi = (int)WinSendMsg(hLB,LM_QUERYITEMCOUNT,0,0)-1;
  }
  for (; nIt<=nItHi; nIt++)
    if (pll = (PLINELIST)WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(nIt),0))
    {
      char sz[CCHMAXPATH+2];
      sprintf(sz,"%c %s", pll->text[1]=='+' ? '' : ' ', fFullNames ? pll->text+2 : sf_fname2short(pll->text+2));
      WinSendMsg(hLB,LM_SETITEMTEXT,MPARAM(nIt),MPARAM(sz));
    }
}
void SRCLIST::clear_spec(char chWhat, BOOL fNoShow)
{
  if (chWhat)
  {
    for (PLINELIST *ppll=&pllSrcFiles; *ppll;)
    {
      if ((*ppll)->text[0]==chWhat) sf_cutlist (ppll);
      else                          ppll=&(*ppll)->next;
    }
  }
  else
  {
    sf_freelist(pllSrcFiles);
    pllSrcFiles = 0;
  }
  if (!fNoShow) fill_lbox();
}
SRCLIST::SRCLIST()
{
  memset(this,0,sizeof(*this));
  nItEnum=LIT_NONE;
}


void ListToClipb(HWND hList)
{
  ULONG  len,ul;
  char  *pch;
  int    nItems = (int)WinSendMsg(hList,LM_QUERYITEMCOUNT,0,0);
  int    i;

  if (!nItems) return;
  for(len=0,i=0; i<nItems; i++)
    len += (ULONG)WinSendMsg(hList,LM_QUERYITEMTEXTLENGTH,MPARAM(i),0);
  len += i*2+1;

  DosAllocSharedMem(PPVOID(&pch),NULL,len,PAG_COMMIT|OBJ_GIVEABLE|PAG_READ|PAG_WRITE);
  if (!pch) return;

  for (i=0,ul=0; i<nItems && ul<len-3; i++)
  {
    ul += (ULONG) WinSendMsg(hList,LM_QUERYITEMTEXT,MPFROM2SHORT(i,len-ul-2),MPARAM(pch+ul));
    strcpy(pch+ul,"\r\n");
    ul += 2;
  }

  if (!WinOpenClipbrd(hAB))
  {
    DosFreeMem((PVOID)pch);
    return;
  }
  WinSetClipbrdData(hAB,(ULONG)pch,CF_TEXT,CFI_POINTER);
  WinCloseClipbrd(hAB);
  DosFreeMem((PVOID)pch);
}

//
void PlayMenu(HWND hWnd, ULONG ulRCMenu, LONG alChecked[], LONG alDisabled[], LONG lPosItem, LONG xPos, LONG yPos)
/*
> (HWND)  hWnd       - owner
> (ULONG) ulRCMenu   - ресурс с описанием меню
> (PLONG) alChecked  - массив идентификаторов разделов которые следует зачекать,
>                      признак конца - 0; Можно передать NULL
> (PLONG) alDisabled - массив идентификаторов разделов которые следует задизэйблить,
>                      признак конца - 0; Можно передать NULL
> (LONG)  lPosItem   - ид-р начального раздела (или 0)
> (LONG)  xPos,yPos  - позиция на экране (-1000,-1000 - use mouse pos.), можно не писать
>
>
*/
{
  int  i;
  if (xPos==-1000 && yPos==-1000)
  {
    POINTL ptl;
    WinQueryPointerPos(HWND_DESKTOP,&ptl);
    xPos = ptl.x;
    yPos = ptl.y;
  }
  HWND hMenu   = WinLoadMenu(HWND_DESKTOP, (HMODULE) NULL, ulRCMenu);
  ULONG fs     = PU_HCONSTRAIN | PU_VCONSTRAIN | PU_KEYBOARD | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2;
  for (i=0; alChecked  && alChecked [i]; i++) WinCheckMenuItem (hMenu,alChecked[i], TRUE);
  for (i=0; alDisabled && alDisabled[i]; i++) WinEnableMenuItem(hMenu,alDisabled[i],FALSE);
  if (lPosItem)                                         fs |= PU_POSITIONONITEM;
  if (WinGetKeyState(HWND_DESKTOP,VK_BUTTON1) & 0x8000) fs |= PU_MOUSEBUTTON1DOWN;
  if (WinGetKeyState(HWND_DESKTOP,VK_BUTTON2) & 0x8000) fs |= PU_MOUSEBUTTON2DOWN;
  WinPopupMenu(HWND_DESKTOP, hWnd, hMenu, xPos,yPos,lPosItem, fs);
  WinSetPresParam(hMenu,PP_FONTNAMESIZE,strlen("10.System Proportional")+1,"10.System Proportional");
  //WinDestroyWindow(hMenu);
}

void SetPPFont(HWND hWnd, PSZ pszPPCopy)
{
  FONTDLG      fontDlg;
  HPS          hps = WinGetPS(hWnd);
  CHAR         szFamily[CCHMAXPATH] = "";
  FONTMETRICS  fontMetrics;
  if (pszPPCopy) pszPPCopy[0] = 0;
  GpiQueryFontMetrics  (hps,sizeof(FONTMETRICS), &fontMetrics);
  strcpy(szFamily, fontMetrics.szFamilyname);

  memset(&fontDlg, 0, sizeof(FONTDLG));
  fontDlg.hpsScreen      = hps;
  fontDlg.cbSize         = sizeof(FONTDLG);
  fontDlg.pszFamilyname  = szFamily;
  fontDlg.usFamilyBufLen = sizeof(szFamily);
  fontDlg.fxPointSize    = MAKEFIXED(8,0);
  fontDlg.fl             = FNTS_CENTER;
  fontDlg.flType         = (LONG) fontMetrics.fsType;
  fontDlg.clrFore        = CLR_BLACK;
  fontDlg.clrBack        = CLR_WHITE;
  fontDlg.sNominalPointSize = fontMetrics.sNominalPointSize;
  fontDlg.usWeight          = fontMetrics.usWeightClass;
  fontDlg.usWidth           = fontMetrics.usWidthClass;

  WinFontDlg(HWND_DESKTOP,hWnd,&fontDlg);
  if(fontDlg.lReturn==DID_OK)
  {
    char szPPFont[FACESIZE+10];
    sprintf(szPPFont,"%u.%s",(int)(fontDlg.sNominalPointSize/10), fontDlg.fAttrs.szFacename);
    WinSetPresParam(hWnd,PP_FONTNAMESIZE,strlen(szPPFont)+1,szPPFont);
    if (pszPPCopy) strcpy(pszPPCopy,szPPFont);
  }
  WinReleasePS(hps);
}

void Beep()
{
  static BOOL fIni  = FALSE;
  static BOOL fBeep = TRUE;
  if (!fIni)
  {
    fBeep = PrfQueryProfileInt(HINI_USERPROFILE,"PM_ControlPanel","Beep",1);
    fIni = TRUE;
  }
  if (fBeep) DosBeep(2000,60);
}

/*==========================> Z a s t a v k a <=========================================================*/

MRESULT EXPENTRY wpZastWin(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static LONG  nTimeLeft;
  static HWND  hbm;
  switch(ulMsg){
  case WM_CREATE:
    WinDefWindowProc(hWnd,ulMsg,m1,m2);
    hbm       = 0;
    nTimeLeft = PrfQueryProfileInt(HINI_USERPROFILE,"PM_ControlPanel","LogoDisplayTime",5000);
    if (!nTimeLeft)
      return MRESULT(TRUE); // Terminate it
    nTimeLeft = max (1, nTimeLeft/100); // By 100 msec
    WinStartTimer(hAB,hWnd,1,100);
    break;
  case WM_USER:
    hbm = HWND(m1);
    WinInvalidateRect(hWnd,0,0);
    break;
  case WM_PAINT:
  {
    HPS    hps = WinBeginPaint(hWnd,0,0);
    POINTL ptl = {0,0};
    if (hbm) WinDrawBitmap(hps,hbm,NULL,&ptl,0,0,DBM_NORMAL|DBM_IMAGEATTRS);
    WinEndPaint(hps);
    break;
  }
  case WM_FOCUSCHANGE:
    if(USHORT(ULONG(m2)) || !fConfigON) break;
    nTimeLeft = 1;
    m1 = MPARAM(1);
  case WM_TIMER:
   if (SHORT1FROMMP(m1)!=1 || --nTimeLeft) break;
    m1 = m2 = MPARAM(0);
  case WM_CHAR:
    if (!(SHORT1FROMMP(m1)&KC_KEYUP))
    {
      if (SHORT1FROMMP(m2)=='p') nTimeLeft += 20; // ++2 sec.
      else
      {
        WinDestroyWindow(hWnd);
        hZast = 0;
      }
    }
    break;
  default:
    return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

void Zastavka()
{
  BITMAPINFOHEADER bmpData;
  HPS              hps = WinGetPS(HWND_DESKTOP);
  HBITMAP          hbm = GpiLoadBitmap(hps, NULLHANDLE, IDB_LOGOBMP, 0, 0);
  WinReleasePS(hps);
  if (!hbm)
    return;
  GpiQueryBitmapParameters(hbm,&bmpData);

  WinRegisterClass(hAB,"XDSZASTCLASS",(PFNWP)wpZastWin, CS_CLIPCHILDREN,0L);
  hZast = WinCreateWindow(HWND_DESKTOP,"XDSZASTCLASS",0,WS_CLIPSIBLINGS|WS_SYNCPAINT,
                               0,0,1,1,hMDI0Client,HWND_TOP,0,0,NULL);
  if (!hZast) return;

  WinSetWindowPos  (hZast,0,0,0,bmpData.cx,bmpData.cy,SWP_SIZE);
  CenterChildWindow(hZast);
  WinSendMsg       (hZast,WM_USER,MPARAM(hbm),0);
  WinShowWindow    (hZast,TRUE);
  WinInvalidateRect(hZast,0,0);
}
/*======================================================================================================*/
/*                                                                                                      */
/*  R E X X   h a n d l e r s                                                                           */
/*                                                                                                      */
/*======================================================================================================*/
static void rx_set_retstr(PRXSTRING retstr,PSZ sz)
{
  strcpy(retstr->strptr,sz);
  retstr->strlength = strlen(sz);
}
static void rx_set_retval(PRXSTRING retstr,int i)
{
  sprintf(retstr->strptr,"%d",i);
  retstr->strlength = strlen(retstr->strptr);
}
static void rx_set_retpos(PRXSTRING retstr,int line, int pos)
{
  sprintf(retstr->strptr,"%d,%d",line,pos);
  retstr->strlength = strlen(retstr->strptr);
}


/*>>> EdBookmark(SET | GO)    : "";              - play menu
>     EdBookmark(SET | GO ,n) : "line,pos" | "";
>     EdBookmark(QUERY    ,n) : "line,pos" | "";
>
*/

LONG   EXPENTRY RhEdBookmark(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  PSZ        pszFr;
  PFILEED    pFE;
  PTPR       ptpr = 0;
  ULONG      ulBM = 0;
  rx_set_retstr(retstr,"");
  if ((pFE   = grOffice.QueryActiveFEd()) &&
      (pszFr = pAutoVarTable->QueryVar("ActiveFrame")) &&
      !strcmp(pszFr,"Editor"))
  {
    if (argc>1 && !strcmp(argv[0].strptr,"QUERY"))
    {
      ulBM = strtol(argv[1].strptr,0,0);
      ptpr = pFE->QueryBM(ulBM+IDM_POP_GOBM0);
    }
    else
    {
      BOOL    fSet;
      if      (argc>0 && !strcmp(argv[0].strptr,"SET")) fSet = TRUE;
      else if (argc>0 && !strcmp(argv[0].strptr,"GO" )) fSet = FALSE;
      else                                              return RxERROR;
      if (argc>1)
      {
        ulBM = strtol(argv[1].strptr,0,0);
        ptpr = fSet ? pFE->SetBM(ulBM+IDM_POP_GOBM0) : pFE->GoBM(ulBM+IDM_POP_GOBM0);
      }
      else
      {
        SWP swp;
        WinQueryWindowPos(pMDI->QueryWindow(MDIQW_MAINFRAME),&swp);
        PlayMenu(hMDI0Client, fSet ? IDM_POPUP_SETBM : IDM_POPUP_GOBM, 0, pFE->QDisabledBMs(), 0,
                 swp.x,swp.y);
      }
    }
  }
  if (ptpr) rx_set_retpos(retstr, ptpr->lLine, ptpr->lPos);
  return ulBM < MAXBOOKMARKS ? 0 : RxERROR;
}


/* ErrGoto( PREVIOUS_IN_LIST | NEXT_IN_LIST |  PREVIOUS_IN_FILE | NEXT_IN_FILE );
*/
LONG   EXPENTRY RhErrGoto(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  ERROFFICE::GO_ERROR ge;
  BOOL                fe = FALSE;
  if (argc!=1)
    fe = TRUE;
  else
  {
    if      (!strcmp(argv[0].strptr,"PREVIOUS_IN_LIST")) ge = ERROFFICE::PREVIOUS_IN_LIST;
    else if (!strcmp(argv[0].strptr,"NEXT_IN_LIST"    )) ge = ERROFFICE::NEXT_IN_LIST;
    else if (!strcmp(argv[0].strptr,"PREVIOUS_IN_FILE")) ge = ERROFFICE::PREVIOUS_IN_FILE;
    else if (!strcmp(argv[0].strptr,"NEXT_IN_FILE"    )) ge = ERROFFICE::NEXT_IN_FILE;
    else                                                 fe = TRUE;
  }
  return (!fe && errOffice.GoError(ge)) ? 0 : RxERROR;
}

// FileLoadDlg([FAST]);
LONG   EXPENTRY RhFileLoadDlg(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  if (argc>1 || (argc==1 && strcmp(argv[0].strptr,"FAST")))
    return RxERROR;
  try
  {
    if (argc) FastLoadDlg();
    else      FileLoadDlg();
  }
  catch (char *psz)
  {
    MessageBox(hMDI0Client,"Load file(s)",MB_ERROR|MB_OK,psz);
    return RxERROR;
  }
  return 0;
}


/*>>> FileOpen(Filename [,nGroup] )
>
>  Filename - filename to open or "" to create new buffer
>  nGroup   - ( < MAXFGROUPS) - group to use, else the curent assumed,
>             if no nGroup specifyed - open in the current window
>             "IN_AUTO_WINDOW" - open in old/new auto window (depends on
>                                the current settings)
*/

LONG   EXPENTRY RhFileOpen(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  rx_set_retstr(retstr,"");
  try
  {
    char         *pch;
    int           nGrp;
    BOOL          fOver   = FALSE;
    HWND          hMsgWnd = WinQueryActiveWindow(hMDI0Client);
    if (!hMsgWnd) hMsgWnd = hMDI0Client;

    if      (argc==1) fOver = TRUE;
    else if (argc==2) nGrp  = strtol(argv[1].strptr,&pch,0);
    else              return RxERROR;

    if      (!strcmp(argv[1].strptr,"IN_AUTO_WINDOW")) nGrp = IN_AUTO_WINDOW;
    else if (nGrp<0 || nGrp>=MAXFGROUPS)               nGrp = grOffice.QueryActiveGroup();

    if (!fOver)
    {
      TEDWINPARAMS twp(pMDI->QueryWindow(MDIQW_DESK));
      if (!grOffice.OpenEditor(nGrp,argv[0].strptr,hMsgWnd,&twp))  return 0;
    }
    else
    {
      PFILEED pFE;
      if (!(pFE = grOffice.QueryActiveFEd()) ||
          !grOffice.OpenEditor(nGrp,argv[0].strptr,hMsgWnd,0,pFE)) return 0;
    }
    rx_set_retstr(retstr,"ok");
    return 0;
  }
  catch (char *psz)
  {
    MessageBox(hMDI0Client,"Open editor",MB_ERROR|MB_OK,psz);
    return RxERROR;
  }
  return 0;
}

// FileReload();
LONG   EXPENTRY RhFileReload(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  if (grOffice.Reload(0,hMDI0Client))
    MessageBox(hMDI0Client,"Reload file",MB_INFORMATION|MB_OK,"File is reloaded.");
  return 0;
}


/*>>> FileSave(): "ok" | "";
>
*/

LONG   EXPENTRY RhFileSave(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  rx_set_retstr(retstr,"");
  HWND          hMsgWnd = WinQueryActiveWindow(hMDI0Client);
  if (!hMsgWnd) hMsgWnd = hMDI0Client;
  if (argc) return RxERROR;
  if (grOffice.SaveOneFile (0,FALSE,hMsgWnd))
    rx_set_retstr(retstr,"ok");
  return 0;
}


/*>>> FileSaveAll([PROMPT]): "ok" : "";
>
*/

LONG   EXPENTRY RhFileSaveAll(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  rx_set_retstr(retstr,"");
  BOOL          fPrompt = FALSE;
  HWND          hMsgWnd = WinQueryActiveWindow(hMDI0Client);
  if (!hMsgWnd) hMsgWnd = hMDI0Client;

  if (argc>1 || argc==1 && !(fPrompt = !sf_stricmp(argv[0].strptr,"PROMPT")))
    return RxERROR;
  if (grOffice.SaveAll(fPrompt,hMsgWnd))
    rx_set_retstr(retstr,"ok");
  return 0;
}


/*>>> FileSaveAs([<filename>]): "ok" | "";
>
*/

LONG   EXPENTRY RhFileSaveAs(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  rx_set_retstr(retstr,"");
  HWND          hMsgWnd = WinQueryActiveWindow(hMDI0Client);
  if (!hMsgWnd) hMsgWnd = hMDI0Client;
  char          szFull[CCHMAXPATH] = "";
  if      (argc>1)
    return RxERROR;
  else if (argc==1 && (strlen(argv[0].strptr)>=CCHMAXPATH || !FName2Full(szFull,argv[0].strptr,FALSE)))
    return RxERROR;

  if (grOffice.SaveOneFile (0,TRUE,hMsgWnd,szFull[0] ? szFull : 0))
    rx_set_retstr(retstr,"ok");
  return 0;
}



/*>>> FindDlg([FILE]) // FILE - absent yet
>
*/

LONG   EXPENTRY RhFindDlg(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  BOOL fFile = (argc==1 && !strcmp(argv[0].strptr,"FILE"));
  if (!fFile && argc)               return RxERROR;
  if (fFile) WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpFileFindDlg,NULLHANDLE,IDD_FILEFINDDLG,0);
  else
  {
    if (!(grOffice.QueryActiveFEd())) return RxERROR;
    WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpFindDlg,NULLHANDLE,IDD_FINDDLG,0);
  }
  return 0;
}


/*>>> GetLine( <Window caption>, <Text near EF>, <EF length>, [<EF initial text>] )
>
> When no <EF initial text> specifyed, the last EF's contents assumed
*/
LONG   EXPENTRY RhGetLine(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  static  PSZ  pszLast = NULL;
  CPARAMSTR    cps;
  PSZ          psz;
  int          lLen;
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  if  (argc<3 || argc>4) return RxERROR;
  lLen      = strtol(argv[2].strptr,0,0);
  if (lLen<1 || lLen>10000 || !(psz=(PSZ)malloc(lLen+1))) return RxERROR;
  psz[0]    = 0;
  cps.psz1  = argv[0].strptr;
  cps.psz2  = argv[1].strptr;
  cps.psz3  = psz;
  cps.l1    = lLen;
  if      (argc == 4) strncpy(psz,argv[3].strptr,lLen);
  else if (pszLast)   strncpy(psz,pszLast,       lLen);
  free(pszLast);
  pszLast   = psz;
  psz[lLen] = 0;
  if (WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
  {
    if ((lLen=strlen(psz)) > 255 &&  DosAllocMem((PPVOID)&retstr->strptr, lLen+1, PAG_COMMIT | PAG_WRITE))
      return RxERROR;
    strcpy(retstr->strptr,psz);
    retstr->strlength = lLen;
    return 0;
  }
  return RxERROR;
}


/* GotoLine( [line] );
* /
LONG   EXPENTRY RhGotoLine(
  PSZ        name,                     // function name
  LONG       argc,                     // count of arguments
  PRXSTRING  argv,                     // argument RXSTRINGs
  PSZ        queue,                    // current Rexx queue
  PRXSTRING  retstr )                  // returned string value
{
  retstr->strlength  = 0;
  retstr->strptr[0] = 0;
  static char  sz[30] = "";
  PFILEED      pFEd;
  CPARAMSTR    cps;
  {
    PSZ psz = pAutoVarTable->QueryVar("ActiveFrame");
    if (!psz || strcmp(psz,"Editor") || !(pFEd = grOffice.QueryActiveFEd()))
      return RxERROR;
  }
  cps.psz1 = "Goto line";
  cps.psz2 = "Line number:";
  cps.psz3 = sz;
  cps.l1   = sizeof(sz);
  cps.l3   = 0;
  if (WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
  {
    TPR   tpr(0,0);
    char *pch = sz;
    if (tpr.lLine = strtol(sz,&pch,0)) tpr.lLine--;
    if (pch!=sz && !*sf_skipspaces(pch))
    {
      WinSendMsg(pFEd->hWnd,TM_SETSEL,MPARAM(&tpr),MPARAM(&tpr));
      return 0;
    }
    else sz[0] = 0;
    Beep();
  }
  return RxERROR;
}
*/


/*>>> HelpContext([topic [,extension]])
>
*/
LONG   EXPENTRY RhHelpContext(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  PSZ  pszTopic = 0;
  PSZ  pszExt   = 0;
  BOOL fFreeMe  = FALSE;
  if (argc>0) pszTopic = argv[0].strptr;
  if (argc>1) pszExt   = argv[1].strptr;
  if (argc>2) return RxERROR;
  PFILEED pFE = grOffice.QueryActiveFEd();
  if (!pszTopic)
  {
    if (pFE) pszTopic = (PSZ)WinSendMsg(pFE->hWnd,TM_GETCURWORD1,MPARAM(TRUE),0);
    if (!pszTopic) return RxERROR;
    fFreeMe = TRUE;
  }
  if (!pszExt && pFE->pszFilename && pFE->pszFilename[0])
    if (pszExt = strrchr(pFE->pszFilename,'.')) pszExt++;
  if (!pszExt) pszExt = "";
  DisplayContextHelp(pszTopic, pszExt);
  if (fFreeMe) free(pszTopic);
  return 0;
}

/*>>> MacroRecord('START' | 'START_DONE' | 'DONE' | 'TERMINATE' [,reason] | 'SHOWMONITOR')
> реально работает только при простом прямом вызове (тогда дергается Rh2MacroRecord)
*/
LONG   EXPENTRY RhMacroRecord(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  return 0;
}


/*>>> OptionsDlg(SETTINGS | ASSOCIATIONS)
>
*/

LONG   EXPENTRY RhOptionsDlg(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  LONG rc = 0;
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  if (argc==1)
  {
    if      (!strcmp(argv[0].strptr,"SETTINGS"))     AssOptDlg(TRUE);
    else if (!strcmp(argv[0].strptr,"ASSOCIATIONS")) AssOptDlg(FALSE);
    else    rc = RxERROR;
  }
  else rc = RxERROR;
  return rc;
}


/*>>> SelFiles(<key>[=<val>])
> <key> = SELECT | UNSELECT | INVERT
> <val> = reg. expr - имена файлов; если не задано - выдается диалог
>         INVERT значение <val> игнорирует
>
*/

LONG   EXPENTRY RhSelFiles(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  char          chMode;   // '+' or '-' or '*'
  static char   szPattern[CCHMAXPATH];
  PSZ           pszArg  = argv[0].strptr;
  HWND          hMsgWnd = WinQueryActiveWindow(hMDI0Client);
  if (!hMsgWnd) hMsgWnd = hMDI0Client;

  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  if      (argc!=1 || strcmp(pAutoVarTable->QueryVar("ActiveFrame"),"SourceList"))
    return RxERROR;
  if      (!strncmp(pszArg,"SELECT",  6)) {chMode = '+'; pszArg += 6; }
  else if (!strncmp(pszArg,"UNSELECT",8)) {chMode = '-'; pszArg += 8; }
  else if (!strncmp(pszArg,"INVERT",  6)) {chMode = '*'; pszArg += 6; }
  else                                    return RxERROR;
  if      (pszArg[0]=='=') pszArg++;
  else if (chMode!='*')
  {
    CPARAMSTR cps;
    cps.psz1 = (chMode=='+') ? "Select the files" : "Unselect the files";
    cps.psz2 = "Filename template:";
    cps.psz3 = szPattern;
    cps.l1   = sizeof(szPattern);
    if (!WinDlgBox(HWND_DESKTOP,hMsgWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
      return RxERROR;
    if (!*sf_skipspaces(szPattern)) pszArg = "*";
    else                            pszArg = szPattern;
  }
  srcList.Select(chMode,pszArg);
  return 0;
}

/*>>> VarQuery(VARNAME): string;
> Вернет содержимое $(VARNAME) или пустую строку.
>
*/
LONG   EXPENTRY RhVarQuery(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  rx_set_retstr(retstr,"");
  if (argc!=1) return RxERROR;
  PSZ    psz;
  int    lLen;
  if ((psz = pUserVarTable->QueryVar(argv[0].strptr)) || (psz = pAutoVarTable->QueryVar(argv[0].strptr)))
  {
    if ((lLen=strlen(psz)) > 255 &&  DosAllocMem((PPVOID)&retstr->strptr, lLen+1, PAG_COMMIT | PAG_WRITE))
      return RxERROR;
    strcpy(retstr->strptr,psz);
    retstr->strlength = lLen;
  }
  return 0;
}


/*>>> VarSet(VARNAME [, VALUE])
>
> if (VARNAME from *pAutoVarTable)
>   error;
> else
>   if (VALUE specifyed) place it into the *pUserVarTable;
>   else                 try to remove this variable from the *pUserVarTable;
> success;
*/

LONG   EXPENTRY RhVarSet(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  rx_set_retstr(retstr,"");
  if (argc<1 || argc>2 || pAutoVarTable->QueryVarIdx(argv[0].strptr)>=0 ) return RxERROR;
  if (argc==2)  pUserVarTable->AssignVar(argv[0].strptr,argv[1].strptr);
  else          pUserVarTable->ForgetVar(argv[0].strptr);
  return 0;
}


/* WinActivate(MESSAGE_LIST | SOURCE_LIST | POPUP_WINDOW | MACRO_EDITOR | EDITOR, <E_arg>| FILE, <filename>): "ok" : "";
>
> Переход в окно.
> <E_arg> ::=  <nGroup> | NEXT_IN_GROUP | PREV_IN_GROUP | NEXT_ON_DISPLAY | PREV_ON_DISPLAY
>
> Note: "" может вернуться только при отсутствии такого файла или при пустой nGroup,
>       во всех остальных случаях, вернется "ok" (или случится RxERROR)
*/
LONG   EXPENTRY RhWinActivate(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  rx_set_retstr(retstr,"ok");
  if (argc==1)
  {
    HWND    hwGo = 0;
    if      (!strcmp(argv[0].strptr,"MESSAGE_LIST")) hwGo = clMsgList.hFrame;
    else if (!strcmp(argv[0].strptr,"SOURCE_LIST"))  hwGo = clSrcList.hFrame;
    else if (!strcmp(argv[0].strptr,"POPUP_WINDOW")) WinPostMsg(hMDI0Client,MM_STARTPOPUPWIN,0,0);
    else if (!strcmp(argv[0].strptr,"MACRO_EDITOR")) Macros.RecallMacroEd(FALSE);
    else return RxERROR;
    if (hwGo) WinSetWindowPos(hwGo,0,0,0,0,0,SWP_SHOW|SWP_ACTIVATE);
  }
  else if (argc==2 && !strcmp(argv[0].strptr,"EDITOR"))
  {
    HWND hFr0 = pMDI->QueryWindow(MDIQW_UPPERFRAME);
    if      (!strcmp(argv[1].strptr,"NEXT_IN_GROUP"))
      grOffice.GoNextEditor(FALSE);
    else if (!strcmp(argv[1].strptr,"PREV_IN_GROUP"))
      grOffice.GoNextEditor(TRUE);
    else if (!strcmp(argv[1].strptr,"NEXT_ON_DISPLAY"))
    {
      HWND hFr = pMDI->QueryWindow(MDIQW_BOTTOMVISFRAME);
      if (hFr && hFr!=hFr0) WinSetActiveWindow(HWND_DESKTOP,hFr);
    }
    else if (!strcmp(argv[1].strptr,"PREV_ON_DISPLAY"))
    {
      HWND hFr  = pMDI->QueryWindow(MDIQW_NEXTVISFRAME);
      if (hFr && hFr!=hFr0)
      {
        WinSetActiveWindow(HWND_DESKTOP,hFr);
        WinSetWindowPos(hFr0,HWND_BOTTOM,0,0,0,0,SWP_ZORDER);
      }
    }
    else // <nGroup>
    {
      ULONG nGrp = strtol(argv[1].strptr,0,0);
      if (!sf_isdigit(argv[1].strptr[0]) || nGrp>=MAXFGROUPS) return RxERROR;
      if (!grOffice.GotoGroup(nGrp,FALSE))                    rx_set_retstr(retstr,"");
    }
  }
  else if (argc==2 && !strcmp(argv[0].strptr,"FILE"))
  {
    char sz[CCHMAXPATH];
    HWND hEd;
    if (strlen(argv[0].strptr)<CCHMAXPATH   &&
        FName2Full(sz,argv[0].strptr,FALSE) &&
        (hEd = grOffice.KnownFilename(argv[0].strptr)))
      grOffice.GotoEd(hEd, FALSE);
    else
      rx_set_retstr(retstr,"");
  }
  else return RxERROR;
  return 0;
}


/*>>> WPSaveAs([<WPName>])
>
> NOTE: It doesn't saves the changed files
*/
LONG   EXPENTRY RhWPSaveAs(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  CPARAMSTR cps;
  char       sz_path[CCHMAXPATH] = "";
  cps.psz3 = sz_path;
  cps.l1   = CCHMAXPATH;
  cps.l3   = 0; /*++ help id */
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  try
  {
    HWND          hMsgWnd = WinQueryActiveWindow(hMDI0Client);
    if (!hMsgWnd) hMsgWnd = hMDI0Client;
    PSZ           pszWP   = 0;

    if (argc>1)  return RxERROR;
    if (argc==1) pszWP = argv[0].strptr;

    while (!pszWP)
    {
      cps.psz1 = "Save workplace as";
      cps.psz2 = "Name:";
      if (!WinDlgBox(HWND_DESKTOP,hMsgWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
        return RxERROR;
      if (cps.psz3[0]) pszWP = cps.psz3;
      else             Beep();
    }

    if (!PfStoreWPlace(pszWP)) return RxERROR;
  }
  catch (char *psz)
  {
    MessageBox(hMDI0Client,"SaveAs workplace",MB_ERROR|MB_OK,psz);
    return RxERROR;
  }
  return 0;
}

/*>>> WPOpen(WPName)
>
> Note: It saves all the changes and the previously opened workplace (if it exists)
*/
LONG   EXPENTRY RhWPOpen(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  try
  {
    PSZ           pszWP   = 0;
    if (argc!=1)  return RxERROR;
    pszWP = argv[0].strptr;

    if (!SaveConfig() || !PfOpenWPlace(pszWP))
      return RxERROR;

  }
  catch (char *psz)
  {
    MessageBox(hMDI0Client,"Open workplace",MB_ERROR|MB_OK,psz);
    return RxERROR;
  }
  return 0;
}

void RegisterOurREXX()
{
  RxRegister("EdBookmark",       (PFN)RhEdBookmark,FALSE);
  RxRegister("ErrGoto",          (PFN)RhErrGoto,FALSE);
  RxRegister("FileLoadDlg",      (PFN)RhFileLoadDlg,FALSE);
  RxRegister("FileOpen",         (PFN)RhFileOpen,FALSE);
  RxRegister("FileReload",       (PFN)RhFileReload,FALSE);
  RxRegister("FileSave",         (PFN)RhFileSave,FALSE);
  RxRegister("FileSaveAll",      (PFN)RhFileSaveAll,FALSE);
  RxRegister("FileSaveAs",       (PFN)RhFileSaveAs,FALSE);
  RxRegister("FindDlg",          (PFN)RhFindDlg,FALSE);
  RxRegister("GetLine",          (PFN)RhGetLine,FALSE);
//RxRegister("GotoLine",         (PFN)RhGotoLine,FALSE);
  RxRegister("HelpContext",      (PFN)RhHelpContext,FALSE);
  RxRegister("MacroRecord",      (PFN)RhMacroRecord,TRUE);
  RxRegister("OptionsDlg",       (PFN)RhOptionsDlg,FALSE);
  RxRegister("SelFiles",         (PFN)RhSelFiles,FALSE);
  RxRegister("VarQuery",         (PFN)RhVarQuery,FALSE);
  RxRegister("VarSet",           (PFN)RhVarSet,FALSE);
  RxRegister("WinActivate",      (PFN)RhWinActivate,FALSE);
  RxRegister("WPSaveAs",         (PFN)RhWPSaveAs,FALSE);
  RxRegister("WPOpen",           (PFN)RhWPOpen,FALSE);
}


