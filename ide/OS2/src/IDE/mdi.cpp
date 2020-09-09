/*
*
*  File: MDI.CPP
*
*  Multy Document Interface implementation file
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#define  INCL_WIN
#define  INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <sf_lib.h>
#include "MDI.H"
#include "edthelp2.h" // use: PANEL_MDILISTDLG
#include "edthelp.h"  // use: DisplayHelpPanel();

/////// MDIFRAME class realisation ////////////////

//--- Messages
#define MM_SUBCLASS       WM_USER+0
#define MM_CREATE_WINBAR  WM_USER+1

#define CLIENT0CLASS "MDIClient0Class"
#define DESKCLASS    "MDIDeskClass"
#define WBOWNERCLASS "MDIWBOwnerClass"

struct  CLIENTINFO
{
  PMDIFRAME              pmdi;
  PSZ                    pszWBText;
  MDIFRAME::PCLIENTLIST  pcl;
};
typedef CLIENTINFO *PCLIENTINFO; // <- Client frames window ULONG

static const ULONG aMDI_SC[7] = {SC_CLOSE,SC_RESTORE,SC_MOVE,SC_SIZE,
                                 SC_MAXIMIZE,SC_HIDE,SC_SYSMENU};

MRESULT EXPENTRY fwpBars     (HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY fwpFrames   (HWND,ULONG,MPARAM,MPARAM);


/***************************************************/
/*                                                 */
/*  WINBAR class definition                        */
/*                                                 */
/***************************************************/


#define WB_BMP_ED        1
#define WB_BMP_MSGLIST   2
#define WB_BMP_SRCLIST   3

#define WB_RGB_FORE      0x000000
#define WB_RGB_BACK      0x808080

// Winbar's WM_CONTROLs:
#define WBN_CLICKED         4000           // (s1m1 not used); m2 = hFrame required
#define WBN_RESIZEME        4001           // (s1m1 not used); m2 = the height required

struct WBNEWWINDOW
{
  HWND  hFrame;
  PSZ   pszText;    // or 0
  ULONG rgbFore;
  ULONG rgbBack;
  ULONG ulWB_Bmp;  // WB_BMP_*
};

typedef WBNEWWINDOW *PWBNEWWINDOW;

class          WINBAR;
typedef struct WBBUTTON
{
  WBBUTTON(HWND hParent, WINBAR *pWBar, BOOL fShowColors, USHORT usCmd, PWBNEWWINDOW pWBWin);
 ~WBBUTTON();
  void    set_pp_font    (PSZ szPPFont);
  void    set_show_colors(BOOL fShow);
  HWND    hBtn;     // The button
  HWND    hFrame;   // Corresponded frame
  ULONG   rgbFore;  // Col-
  ULONG   rgbBack;  //   ors
  BOOL    fShowColors;
  PSZ     pszText;  // malloc()ed text
  BOOL    fFocusON; // Active frame indicator
  HBITMAP hBmp;     // Bitmap (or 0)
  WINBAR *pWBar;
} *PWBBUTTON;

typedef class WINBAR
{
public:
  WINBAR(HWND hParent, HWND hOwner, PMDIFRAME pMDI);
 ~WINBAR();
  void  ClearAll         ();
  void  AddButton        (PWBNEWWINDOW pWBWin);
  void  DeleteButton     (HWND hFrame);
  void  SetViewMode      (ULONG ulWBV);
  void  WBSetPPFont      (PSZ szPPFont);
  void  PopMenu          (HWND hFrame,BOOL fNoTile);
  void  QueryConfig      (PWBCONFIGDATA pWBCfg);
  void  SetConfig        (PWBCONFIGDATA pWBCfg);
  void  SetOrder         (PHWND aFrames, int nFrames);
  void  SetNewText       (HWND hFrame,PSZ szText);
  void  SetNewColors     (HWND hFrame,ULONG rgbFore, ULONG rgbBack);
  void  ShowFocus        (HWND hFrame,BOOL fFocus);
  void  AdjustParentPos  ();
  void  ReAlignAll       (BOOL fShow = FALSE);
private:
  ULONG           cxBtn;
  ULONG           cyBtn;
  ULONG           ulWBV;                  // WBV_* (WB view mode)
  PMDIFRAME       pMDI;                   // Our MDI
  HWND            hParent;                // Our parent
  HWND            hOwner;                 // Our owner
  HWND            hWBar;
  PWBBUTTON       apWBtns[MDI_MAXFRAMES]; // Array of buttons
  int             nBtns;                  // Total buttons
  POINTL          ptlText;                // Text drawing position
  POINTL          ptlBmp;                 // Bitmap drawing position
  BOOL            fResizeInProgress;      // TRUE when we've just asked to resize us
  HWND            hPopFrame;              // hFrame while popup menu processed
  BOOL            fReAlignMe;             // ReAlignAllNow() on WM_TIMER?
  BOOL            fSelfSort;              // Enable winbar sorting (see SetOrder)
  static HBITMAP  hbmEd;                  //
  static HBITMAP  hbmMsgList;             //
  static HBITMAP  hbmSrcList;             //
  friend MRESULT  EXPENTRY fwpWBarBack(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2);
  friend MRESULT  EXPENTRY fwpWBBtn   (HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2);
  friend class    WBBUTTON;
// Note(s):
//  Button QWL_USERs are pointers to it's new()ed WBWINDOW structures
//  Destructor does not caused buttons destroying. When application is been
//   terminated it vill be better to allow PM to destroy them.
} *PWINBAR;



/*******************************************************************************************************/
/*                                                                                                     */
/*  MDIFrame, MDI0Client and MDIDesk windows                                                           */
/*                                                                                                     */
/*******************************************************************************************************/

BOOL MDIFRAME::fCalledBefore = FALSE;
BOOL MDIFRAME::hAB           = 0;

MDIFRAME::MDIFRAME (
                    PHWND    phMDI0Client,         // Created MDI0Client window handle or 0 when error
                    HAB      hAB,
                    ULONG    ulClientIconId,       // Default client icon ident (or 0)
                    // WinCreateStdWindow params:
                    ULONG    flStyle,              // frame style
                    PULONG   pflCreateFlags,       // frame create flags
                    PSZ      pszTitle,             // frame title
                    HMODULE  hmResource,           // resource identifoer or 0
                    ULONG    ulId,                 // frame (and the related resources) identifier
                    LONG x0, LONG y0,              // desktop-relative
                    LONG cx, LONG cy               //   coordinates
                    )
{
  cb = sizeof(*this);
  *phMDI0Client = 0;
  memset(this,0,sizeof(*this));
  rgbDesk                  = 0x00c0b5e0;
  pfnwpMsgHook             = &WinDefWindowProc;
  MDIFRAME::ulClientIconId = ulClientIconId;
  fAutoMenu                = TRUE;
  this->hAB                = hAB;
  if (!fCalledBefore)
  {
    if (!WinRegisterClass(hAB,CLIENT0CLASS, (PFNWP)fwp0Client, CS_SIZEREDRAW | CS_CLIPCHILDREN,4L))
      return;
    if (!WinRegisterClass(hAB,DESKCLASS,    (PFNWP)fwpDesk,    CS_SIZEREDRAW | CS_CLIPCHILDREN,4L))
      return;
    if (!WinRegisterClass(hAB,WBOWNERCLASS, (PFNWP)fwpWBOwner, CS_SIZEREDRAW | CS_CLIPCHILDREN,4L))
      return;
  }
  fCalledBefore = TRUE;

  hMDIFrame = WinCreateStdWindow( HWND_DESKTOP,
                                  flStyle,
                                  pflCreateFlags,
                                  CLIENT0CLASS,
                                  pszTitle,
                                  WS_VISIBLE,
                                  hmResource,
                                  ulId,
                                  (PHWND)&h0Client);
  if (!hMDIFrame || !h0Client) return;
  hDesk     = WinCreateWindow   ( h0Client,
                                  DESKCLASS,
                                  0,
                                  WS_VISIBLE,
                                  0, 0,
                                  200, 100,
                                  h0Client,
                                  HWND_TOP,
                                  0,
                                  0,
                                  NULL);
  if (!hDesk)
  {
    WinDestroyWindow(hMDIFrame);
    hMDIFrame = h0Client = 0;
    return;
  }
  WinSetWindowULong(hMDIFrame,QWL_USER,ULONG(this));
  WinSetWindowULong(h0Client, QWL_USER,ULONG(this));
  WinSetWindowULong(hDesk,    QWL_USER,ULONG(this));
  WinSetWindowPos(hMDIFrame,HWND_TOP,x0,y0,cx,cy,SWP_SIZE|SWP_MOVE| ((flStyle & WS_VISIBLE) ? SWP_SHOW : 0));
  fwpMDIFrame (hMDIFrame, MM_SUBCLASS, MPARAM(ULONG(WinSubclassWindow(hMDIFrame,(PFNWP)fwpMDIFrame))),0L);
  WinPostMsg(hMDIFrame,MM_CREATE_WINBAR,0,0);
  *phMDI0Client = h0Client;
}

MDIFRAME::~MDIFRAME ()
{
  pclClients = 0;  // Нефиг там больше делать,
  pclBars    = 0;  // иначе мы при выходе падаем
  if (WinQueryWindowPtr(hMDIFrame,0)) // Still exsists?
    WinDestroyWindow(hMDIFrame);
}


/*******************************************************************************************************/
/*                                                                                                     */
/*  Client windows (frames and bars)                                                                   */
/*                                                                                                     */
/*******************************************************************************************************/

HWND MDIFRAME::CreateChild (
                            MDIBIND  usBind,               // Window coordinates binding type
                            // WinCreateWindow params:
                            PSZ      pszClass,             // Bar: class,       Frame: client class
                            PSZ      szName,               //                   Frame: frame caption
                            ULONG    flStyle,              // Bar: style,       Frame: client style
                            ULONG    flCreateFlags,        //                   Frame: frame FCF_* flags
                            LONG x0, LONG y0,
                            LONG cx, LONG cy,
                            HWND     hwndOwner,
                            PVOID    pCtlData,             // Bar: pCtlData,    Frame: pCtlData for the client
                            PVOID    pPresParams           // Bar: pPresParams, Frame: pPresParams for the client
                            )
{
  PCLIENTLIST   pcl;
  PCLIENTINFO   pci;
  BOOL          fBar = usBind >= MDIBIND_LEFTBAR;
  HWND          hWnd;
  HWND          hClient;

  if (fBar)
    hWnd = WinCreateWindow (h0Client, pszClass, szName, flStyle, 0,0,1,1,
                            hwndOwner, HWND_TOP, 0, pCtlData, pPresParams);
  else
  {
    {// Restriction: up to MDI_MAXFRAMES frames possible
      int nCl=0;
      for (PCLIENTLIST pcl=pclClients; pcl; pcl=pcl->next) nCl++;
      if (nCl>=MDI_MAXFRAMES) return 0;
    }
    FRAMECDATA  fcdata;
    fcdata.cb            = sizeof(fcdata);
    fcdata.flCreateFlags = flCreateFlags;
    hWnd = WinCreateWindow(hDesk, WC_FRAME,szName, 0, 0,0,1,1,
                           h0Client, HWND_TOP, 0, &fcdata, NULL);
    if (hWnd)
    {
      hClient = WinCreateWindow (hWnd, pszClass, 0, flStyle, 0,0,1,1,
                                 hwndOwner ? hwndOwner : hWnd, HWND_TOP, FID_CLIENT, pCtlData, pPresParams);
      if (!hClient) { WinDestroyWindow(hWnd); hWnd = 0; }
    }
  }
  if (!hWnd) return 0;


  for(pcl = (fBar ? pclBars : pclClients); pcl && pcl->next; pcl = pcl->next);
  if (pcl)
    {pcl->next = new CLIENTLIST; pcl = pcl->next; }
  else
    {pcl       = new CLIENTLIST; fBar ? (pclBars=pcl) : (pclClients=pcl); }
  pcl->hWnd   = hWnd;
  pcl->usBind = usBind;
  pcl->cx     = cx;
  pcl->cy     = cy;
  pcl->x0     = x0;
  pcl->y0     = y0;

  pci            = new CLIENTINFO;
  pci->pmdi      = this;
  pci->pszWBText = 0;
  pci->pcl       = pcl;
  WinSetWindowULong(hWnd,QWL_USER,ULONG(pci));

  if (fBar)
  {
    pcl->pwpOriginal = (PFNWP)WinSubclassWindow(hWnd,(PFNWP)fwpBars);
    ArrangeChildren();
  }
  else  // Frame
  {
    if (!hptrClientIcon)
      hptrClientIcon = (HPOINTER)WinLoadPointer(HWND_DESKTOP,hmResource,ulClientIconId);
    WinSendMsg(hWnd,WM_SETICON,MPARAM(ULONG(hptrClientIcon)),0);
    WinSetWindowPos(hWnd,0,x0,y0,0, 0, SWP_MOVE);
    WinSetWindowPos(hWnd,0,0, 0 ,cx,cy,SWP_SIZE);
    fwpFrames(hWnd, MM_SUBCLASS, MPARAM(ULONG(WinSubclassWindow(hWnd,(PFNWP)fwpFrames))),0L);
  }
  return hWnd;
}


MRESULT EXPENTRY fwpMDIFrame (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  static PFNWP     pWindowProc;
  PMDIFRAME        p = 0;
  if (ulMsg!=WM_CREATE) p = (PMDIFRAME)WinQueryWindowULong(hWnd,QWL_USER);

  if (ulMsg == MM_SUBCLASS)
  {
    pWindowProc = PFNWP(ULONG(m1));
    p->wpMDIFrame(hWnd,MM_SUBCLASS,m1,m2);
    return 0;
  }
  else if (p && ulMsg==MM_CREATE_WINBAR)
  {//--- Create winbar window:
    if (p->hWBOwner = p->CreateChild (MDIBIND_BOTTOMBAR, WBOWNERCLASS, 0, WS_VISIBLE, 0, 0,0,0,0, 0,0,0))
    {
      p->pWinBar  = new WINBAR(p->hWBOwner, p->hWBOwner, p);
      WinSendMsg(p->hWBOwner,WM_SIZE,0,0);
    }
    return 0;
  }
  if (p) return  p->wpMDIFrame(hWnd,ulMsg,m1,m2);
  else   return  ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
}
MRESULT EXPENTRY fwp0Client (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  PMDIFRAME p = 0;
  if (ulMsg!=WM_CREATE) p = (PMDIFRAME)WinQueryWindowULong(hWnd,QWL_USER);
  if (p) return p->wp0Client(hWnd,ulMsg,m1,m2);
  else   return WinDefWindowProc(hWnd,ulMsg,m1,m2);
}
MRESULT EXPENTRY fwpDesk (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  PMDIFRAME p = 0;
  if (ulMsg!=WM_CREATE) p = (PMDIFRAME)WinQueryWindowULong(hWnd,QWL_USER);
  if (ulMsg==WM_PAINT)
    ulMsg = WM_PAINT;
  if (p) return p->wpDesk(hWnd,ulMsg,m1,m2);
  else   return WinDefWindowProc(hWnd,ulMsg,m1,m2);
}
MRESULT EXPENTRY fwpWBOwner (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  PMDIFRAME p = (PMDIFRAME)WinQueryWindowULong(WinQueryWindow(hWnd,QW_PARENT), // MDI0Client
                                               QWL_USER);                      // pMDI
  switch(ulMsg)
  {
    case WM_SIZE: // can't use m1,m2
    {
      if (p && p->pWinBar) p->pWinBar->AdjustParentPos();
      break;
    }
    case WM_CONTROL:
      if (p)
      {
        if      (SHORT2FROMMP(m1)==WBN_RESIZEME)
        {
          p->SetBarWidth(hWnd, LONG(m2));
        }
        else if (SHORT2FROMMP(m1)==WBN_CLICKED)
          WinSetWindowPos(HWND(m2),HWND_TOP,0,0,0,0,SWP_SHOW|SWP_ZORDER|SWP_ACTIVATE);
      }
      break;
  }
  return WinDefWindowProc(hWnd,ulMsg,m1,m2);
}

MRESULT MDIFRAME::wpMDIFrame (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  static PFNWP     pWindowProc;

  switch(ulMsg)
  {
  case MM_SUBCLASS:
    pWindowProc = PFNWP(ULONG(m1));
    return 0;
  case WM_SYSCOMMAND:
  {
    SHORT s1m1 = SHORT1FROMMP(m1);
    if (s1m1==SC_MINIMIZE)
    {
      fMainMinimized = TRUE;
      WinBroadcastMsg(hWnd,WM_CONTROL,MPFROM2SHORT(0,MDIN_MINIMIZE),MPARAM(TRUE), BMSG_SEND|BMSG_DESCENDANTS);
    }
    else if (s1m1==SC_MAXIMIZE || s1m1==SC_RESTORE)
    {
      MRESULT rc = ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
      if (fMainMinimized)
      {
        WinBroadcastMsg(hWnd,WM_CONTROL,MPFROM2SHORT(0,MDIN_MINIMIZE),MPARAM(FALSE), BMSG_SEND|BMSG_DESCENDANTS);
        fMainMinimized = FALSE;
      }
      return rc;
    }
    break;
  }
  }
  return ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
}

MRESULT MDIFRAME::wp0Client (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  switch(ulMsg)
  {
  case WM_SIZE:
    ArrangeChildren();
    break;
  case WM_PAINT:
  {
    RECTL rcl;
    HPS   hps = WinBeginPaint(hWnd,NULLHANDLE,&rcl);
    GpiCreateLogColorTable(hps,0,LCOLF_RGB,0,0,0);
    WinFillRect(hps, &rcl, rgbDesk);
    WinEndPaint(hps);
    break;
  }
  case WM_COMMAND:
  {
    if (fMainMinimized) return 0;
    int         i;
    PCLIENTLIST pcl;
    SHORT       s1m1 = SHORT1FROMMP(m1);
    // --- Client windows accelerators (set with SetMenuItem)
    {
      for (pcl=pclClients; pcl; pcl=pcl->next)
        if (pcl->nAccel==s1m1) break; /*for*/
      if (pcl)
      {
        WinSetActiveWindow(HWND_DESKTOP,pcl->hWnd);
        return 0;
      }
    }
    // --- Window menu:
    if (s1m1>=IDM_WINFIRST && s1m1<=IDM_WINLAST)
    {
      for (i=s1m1-(IDM_WINFIRST),pcl=pclClients; pcl && i>0; pcl=pcl->next,i--);
      if (pcl)
      {
        WinShowWindow(pcl->hWnd,TRUE);
        WinSetActiveWindow(HWND_DESKTOP,pcl->hWnd);
        return 0;
      }
    }
    if (s1m1==IDM_WINMORE)
    {
      WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpMDIListDlg,NULLHANDLE,IDD_MDILISTDLG,this);
      return 0;
    }
    // --- Client's CTRL+F# accelerators - change it with WM_SYSCOMMAND message
    //     and send to the corresponded client's frame
    if (s1m1 >= MDICL_CLOSE && s1m1 <= MDICL_SYSMENU)
    {
      if (SHORT1FROMMP(m2)!=CMDSRC_ACCELERATOR) break;
      HWND hTarg = WinQueryFocus(HWND_DESKTOP);
      while (hTarg) // --- Looking for the nearest frame
      {
        char sz[30];
        WinQueryClassName(hTarg,sizeof(sz),sz);
        if (!strcmp(sz,"#1" /* WC_FRAME */)) break;
        hTarg = WinQueryWindow(hTarg,QW_PARENT);
      }
      if (!hTarg || WinQueryWindow(hTarg,QW_PARENT)!=hDesk) break; // It's not our client
      /// --- Send it to the MDI client found as a SYSCOMMAND msg
      MPARAM mp = (MPARAM)aMDI_SC[s1m1-MDICL_CLOSE]; // Used to prevent CSet codegeneration bug
      WinSendMsg(hTarg, WM_SYSCOMMAND, mp, MPFROM2SHORT(CMDSRC_OTHER,FALSE));
      return 0;
    }
    goto defret;
  }
  case WM_INITMENU:
    if (m1 == MPARAM(IDM_WINDOW)) fill_menu();
  defret:
  default: return (*pfnwpMsgHook)(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


MRESULT MDIFRAME::wpDesk    (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  switch(ulMsg)
  {
  case WM_PAINT:
  {
    RECTL rcl;
    HPS   hps = WinBeginPaint(hWnd,NULLHANDLE,&rcl);
    GpiCreateLogColorTable(hps,0,LCOLF_RGB,0,0,0);
    WinFillRect(hps, &rcl, rgbDesk);
    WinEndPaint(hps);
    break;
  }
  default: return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

void MDIFRAME::SetMsgHook(PFNWP pfnwpHook) { pfnwpMsgHook = pfnwpHook; }



//------------ B a r s   subclass function -------------
//
MRESULT EXPENTRY fwpBars (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  PCLIENTINFO                pci = (PCLIENTINFO)WinQueryWindowULong(hWnd,QWL_USER);

  if (pci)
  {
    MDIFRAME::PCLIENTLIST pcl = pci->pcl;
    switch (ulMsg){
      case WM_DESTROY:
      {
        WinSetWindowULong(hWnd,QWL_USER,0);
        if (pci->pmdi->pclClients || pci->pmdi->pclBars) // MDI still exists?
        { // Exclide from the list & re-arrange all
          MDIFRAME::PCLIENTLIST pcl0 = pci->pmdi->pclBars;
          if (pcl == pcl0) pci->pmdi->pclBars = pcl->next;
          else
            for (; pcl0; pcl0 = pcl0->next)
              if (pcl0->next == pcl)
              {
                pcl0->next = pcl->next;
                break;
              }
          pci->pmdi->ArrangeChildren();
        }
        delete pcl;
        delete pci;
        break;
      }
    }
    if (pci->pcl->pwpOriginal)
      return ((MRESULT)(*pci->pcl->pwpOriginal)(hWnd,ulMsg,m1,m2));
  }
  return WinDefWindowProc(hWnd,ulMsg,m1,m2);
}

//------------ Client frame subclass function -------------
//
MRESULT EXPENTRY fwpFrames (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  static PFNWP               pWindowProc;
  PCLIENTINFO                pci = (PCLIENTINFO)WinQueryWindowULong(hWnd,QWL_USER);

  if (ulMsg == MM_SUBCLASS)
  {
    pWindowProc = PFNWP(ULONG(m1));
    HWND hwndSysMenu = WinWindowFromID(hWnd, FID_SYSMENU);
    WinSetMenuItemText(hwndSysMenu, SC_RESTORE,   "~Restore  \tCtrl+F5");
    WinSetMenuItemText(hwndSysMenu, SC_MOVE,      "~Move     \tCtrl+F7");
    WinSetMenuItemText(hwndSysMenu, SC_SIZE,      "~Size     \tCtrl+F8");
    WinSetMenuItemText(hwndSysMenu, SC_MINIMIZE,  "Mi~nimize \tCtrl+F9");
    WinSetMenuItemText(hwndSysMenu, SC_MAXIMIZE,  "Ma~ximize \tCtrl+F10");
    WinSetMenuItemText(hwndSysMenu, SC_HIDE,      "~Hide     \tCtrl+F11");
    WinSetMenuItemText(hwndSysMenu, SC_CLOSE,     "~Close    \tCtrl+F4");

    // Place button into the winbar
    PMDIFRAME p = (PMDIFRAME)WinQueryWindowULong(WinQueryWindow(hWnd,QW_PARENT), // MDI0Client
                                                 QWL_USER);                      // pMDI
    if (p && p->pWinBar)
    {
      char sz[CCHMAXPATH+30] = "";
      WinQueryWindowText(hWnd,sizeof(sz),sz);
      WBNEWWINDOW nw;
      nw.hFrame   = hWnd;
      nw.pszText  = sz;
      nw.rgbFore  = 0x00000000;
      nw.rgbBack  = 0x00c0c0c0;
      nw.ulWB_Bmp = WB_BMP_ED;
      p->pWinBar->AddButton(&nw);
    }
    return 0;
  }
  else if (ulMsg==WM_SETWINDOWPARAMS && pWindowProc)
  {
    if      (m1) (*pWindowProc)(hWnd,ulMsg,m1,m2);
    else if (m2 && pci)
    {
      free(pci->pszWBText);
      pci->pszWBText = 0;
      if (PSZ(m2)[0] && (pci->pszWBText = (PSZ)malloc(strlen(PSZ(m2))+1)))
        strcpy(pci->pszWBText,PSZ(m2));
    }
    else return 0;
    if (pci && (!m1 || ULONG(m1) & (WPM_TEXT|WPM_CCHTEXT)) )
    {
      char sz[CCHMAXPATH+30] = "";
      WinQueryWindowText(hWnd,sizeof(sz),sz);
      PMDIFRAME p = (PMDIFRAME)WinQueryWindowULong(WinQueryWindow(hWnd,QW_PARENT), // MDI0Client
                                                   QWL_USER);                      // pMDI
      if (p && p->pWinBar && pci)
        p->pWinBar->SetNewText(hWnd, pci->pszWBText ? pci->pszWBText : sz);
    }
    return 0;
  }

  if (pci)
  {
    MDIFRAME::PCLIENTLIST pcl = pci->pcl;
    switch (ulMsg){
      case WM_MOVE:
      case WM_SIZE:
      {
        SWP swp;
        WinQueryWindowPos(hWnd,&swp);
        if (!(swp.fl & (SWP_MAXIMIZE | SWP_MINIMIZE)) && !(pcl->fMaxim || pcl->fMinim))
        {
          pcl->x0 = swp.x;  pcl->y0 = swp.y;
          pcl->cx = swp.cx; pcl->cy = swp.cy;
        }
        break;
      }
      case WM_ACTIVATE:
      {
        PMDIFRAME p = (PMDIFRAME)WinQueryWindowULong(WinQueryWindow(hWnd,QW_PARENT), // MDI0Client
                                                   QWL_USER);                      // pMDI
        if (p && p->pWinBar)
          p->pWinBar->ShowFocus(hWnd, !!m1);
        if (m1)
        { //--- Allow user to see our titlebar anyway
          SWP swp,swpP;
          WinQueryWindowPos(hWnd,&swp);
          WinQueryWindowPos(WinQueryWindow(hWnd,QW_PARENT),&swpP);
          LONG x=swp.x, y=swp.y;
          if (!(swp.fl&SWP_MAXIMIZE) && swpP.cx>45 && swpP.cy>10)
          {
            if (swp.x > swpP.cx-20)        x = swpP.cx/2;
            if (swp.x+swp.cx < 20)         x = 0;
            if (swp.y+swp.cy < 20)         y = swpP.cy/2-swp.cy;
            if (swp.y+swp.cy > swpP.cy+15) y = swpP.cy-swp.cy;
          }
          if (x!=swp.x || y!=swp.y)
            WinSetWindowPos(hWnd,0,x,y,0,0,SWP_MOVE);
        }
        break;
      }
      case WM_TRACKFRAME:
      {
        SWP swp;
        WinQueryWindowPos(hWnd,&swp);
        if (swp.fl & SWP_MAXIMIZE || pcl->fMaxim)
        {
          WinSetActiveWindow(HWND_DESKTOP,hWnd);
          return 0;
        }
        break;
      }
      case WM_ADJUSTFRAMEPOS:
      {
        ULONG fl = PSWP(m1)->fl;
        if (fl & (SWP_RESTORE | SWP_MINIMIZE))
        {
          pcl->fMaxim = FALSE;
          if (!(pcl->fMinim = (fl & SWP_MINIMIZE)))
          {
            PSWP(m1)->x = pcl->x0;
            PSWP(m1)->y = pcl->y0;
          }
        }
        else if (fl & SWP_MAXIMIZE)
        {
          pcl->fMaxim = TRUE;
          pcl->fMinim = FALSE;
        }
        break;
      }
      case WM_SYSCOMMAND:
        if (SHORT1FROMMP(m2)==CMDSRC_ACCELERATOR)  // Pass to the main system menu
          return WinSendMsg( pci->pmdi->hMDIFrame,ulMsg,m1,m2);
        else if (SHORT1FROMMP(m1)==SC_CLOSE)
          return WinSendMsg( WinWindowFromID(hWnd,FID_CLIENT),ulMsg,m1,m2);
        else if (SHORT1FROMMP(m1)==SC_HIDE)
        {
          WinSetWindowPos(hWnd,0,0,0,0,0,SWP_DEACTIVATE);
          WinShowWindow(hWnd,FALSE);
          return 0;
        }
        break;
      case WM_INITMENU:
        if (m1 == MPARAM(SC_SYSMENU))
        {
          MRESULT rc =((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
          if (pcl->fMaxim)
          {
            WinEnableMenuItem(HWND(m2),SC_SIZE,FALSE);
            WinEnableMenuItem(HWND(m2),SC_MOVE,FALSE);
          }
          return rc;
        }
        break;
      case WM_DESTROY:
      {
        WinSetWindowULong(hWnd,QWL_USER,0);

        MDIFRAME::PCLIENTLIST pcl0 = pci->pmdi->pclClients;
        if (pci->pmdi->pclClients || pci->pmdi->pclBars) // MDI still exists?
        { // Exclide from the list
          if (pcl == pcl0) pci->pmdi->pclClients = pcl->next;
          else
            for (; pcl0; pcl0 = pcl0->next)
              if (pcl0->next == pcl)
              {
                pcl0->next = pcl->next;
                break;
              }
          pci->pmdi->pWinBar->DeleteButton(hWnd);
        }
        delete pcl;
        delete pci;
        break;
      }
    }
  }
  return ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
}






/*******************************************************************************************************/
/*                                                                                                     */
/*  Window management interface                                                                        */
/*                                                                                                     */
/*******************************************************************************************************/


BOOL MDIFRAME::SetBarWidth(HWND hBar, LONG lNewWidth)
{
  PCLIENTLIST pcl = pclBars;
  while(pcl && pcl->hWnd!=hBar) pcl = pcl->next;
  if (!pcl) return FALSE;
  if (pcl->usBind == MDIBIND_LEFTBAR || pcl->usBind == MDIBIND_RIGHTBAR)
    pcl->cx = lNewWidth;
  else
    pcl->cy = lNewWidth;
  ArrangeChildren();
  return TRUE;
}

HWND MDIFRAME::QueryWindow(MDIQW mdiqw)
{
  if (mdiqw==MDIQW_DESK)      return hDesk;
  if (mdiqw==MDIQW_MAINFRAME) return hMDIFrame;

  HWND  hRet  = 0;
  int   nFr   = EnumFrames(0,0);
  if (!nFr)   return hRet;
  PHWND ahWnd = (PHWND)malloc(nFr*sizeof(PHWND));
  if (!ahWnd) return hRet;
  EnumFrames(ahWnd,nFr);
  switch (mdiqw)
  {
    case MDIQW_UPPERFRAME:    hRet = ahWnd[0];                           break;
    case MDIQW_NEXTFRAME:     hRet = (nFr>1 ? ahWnd[1]     : ahWnd[0]);  break;
    case MDIQW_BOTTOMFRAME:   hRet = ahWnd[nFr-1];                       break;
    case MDIQW_NEXTVISFRAME:
    {
      for (int i=(nFr>1?1:0); i<nFr; i++)
        if (WinQueryWindowULong(ahWnd[i],QWL_STYLE)&WS_VISIBLE)
        {
          hRet=ahWnd[i];
          break;
        }
      break;
    }
    case MDIQW_BOTTOMVISFRAME:
    {
      for (int i=nFr-1; i>=0; i--)
        if (WinQueryWindowULong(ahWnd[i],QWL_STYLE)&WS_VISIBLE)
        {
          hRet=ahWnd[i];
          break;
        }
      break;
    }
  }
  free(ahWnd);
  return hRet;
}

BOOL MDIFRAME::SetChildHandle(HWND hChild, ULONG ulHandle)
{
  PCLIENTLIST pcl = hFr2pcl(hChild);
  if (!pcl)   pcl = hBar2pcl(hChild);
  if (pcl)
  {
    pcl->ulHandle = ulHandle;
    return TRUE;
  }
  return FALSE;
}

ULONG MDIFRAME::QueryChildHandle(HWND hChild)
{
  PCLIENTLIST pcl = hFr2pcl(hChild);
  if (!pcl)   pcl = hBar2pcl(hChild);
  if (pcl) return pcl->ulHandle;
  else     return 0;
}

int MDIFRAME::EnumFrames(PHWND ahWnd, int n)
{
  HWND   hwndNext;
  HENUM  henum;
  int    i=0;

  if (!ahWnd)
  {
    PCLIENTLIST pcl = pclClients;
    for (; pcl; pcl=pcl->next,i++);
    return i;
  }

  henum = WinBeginEnumWindows(hDesk);
  for (;i<n;i++)
    if (hwndNext = WinGetNextWindow(henum)) ahWnd[i] = hwndNext;
    else                                    break;
  WinEndEnumWindows (henum);
  return i;
}

BOOL MDIFRAME::QueryFramePos(HWND hFrame, PMDICLPOS pcp)
{
  SWP              swp;
  PCLIENTLIST      pcl = hFr2pcl(hFrame);
  if (!pcl || !pcp) return FALSE;
  pcp->Reserved0 = 0;
  pcp->x0        = pcl->x0;
  pcp->y0        = pcl->y0;
  pcp->cx        = pcl->cx;
  pcp->cy        = pcl->cy;
  pcp->fMaxim    = pcl->fMaxim;
  pcp->fMinim    = pcl->fMinim;
  pcp->fVisible  = !!(WS_VISIBLE & WinQueryWindowULong(hFrame,QWL_STYLE));
  pcp->usBind    = pcl->usBind;
  WinQueryWindowPos(hDesk,&swp);
  pcp->cxDesk    = swp.cx;
  pcp->cyDesk    = swp.cy;
  return TRUE;
}

BOOL MDIFRAME::SetFramePos(HWND hFrame, PMDICLPOS pcp)
{
  PCLIENTLIST pcl  = hFr2pcl(hFrame);
  if (!pcl || !pcp) return FALSE;
  BOOL        fVis = pcp->fVisible;
  pcl->x0     = pcp->x0;
  pcl->y0     = pcp->y0;
  pcl->cx     = pcp->cx;
  pcl->cy     = pcp->cy;
  pcl->fMaxim = pcp->fMaxim;
  pcl->fMinim = pcp->fMinim;
  pcl->usBind = pcp->usBind;
  WinShowWindow(hFrame,fVis);

  { // Account desk size changes
    SWP swp;
    WinQueryWindowPos(hDesk,&swp);
    LONG dcx = swp.cx-pcp->cxDesk;
    LONG dcy = swp.cy-pcp->cyDesk;
    LONG dx=0, dy=0;
    switch (pcl->usBind)
    {
      case MDIBIND_LEFTTOP:
        dy = dcy;
      break;
      case MDIBIND_RIGHTTOP:
        dy = dcy;
        dx = dcx;
      break;
      case MDIBIND_RIGHTBOTTOM:
        dx = dcx;
      break;
    }
    pcl->x0 += dx;
    pcl->y0 += dy;
    if (pcl->fMaxim)
    { // Re-maximize it (the styles fucking used to prevent any window flashing)
      WinSetWindowPos(hFrame,0,0,0,0,0,SWP_MINIMIZE | (fVis ? SWP_HIDE : 0));
      WinSetWindowPos(hFrame,0,0,0,0,0,SWP_MAXIMIZE | (fVis ? SWP_SHOW : 0));
    }
    else if (pcl->fMinim)
    {
      WinSetWindowPos(hFrame,0,0,0,0,0,SWP_MINIMIZE);
      WinSetWindowUShort(hFrame,QWS_XRESTORE,  pcl->x0);
      WinSetWindowUShort(hFrame,QWS_YRESTORE,  pcl->y0);
      WinSetWindowUShort(hFrame,QWS_CXRESTORE, pcl->cx);
      WinSetWindowUShort(hFrame,QWS_CYRESTORE, pcl->cy);
    }
    else
      WinSetWindowPos(hFrame,0,pcl->x0,pcl->y0,0,0,SWP_MOVE);
  }
  return TRUE;
}

void MDIFRAME::WBarSetText(HWND hFrame, PSZ szText)
{
  WinSendMsg(hFrame,WM_SETWINDOWPARAMS,0,MPARAM(szText?szText:"")); // Why not?
}
void MDIFRAME::WBarSetColors(HWND hFrame, ULONG rgbFore, ULONG rgbBack)
{
  if (pWinBar) pWinBar->SetNewColors(hFrame,rgbFore,rgbBack);
}
void MDIFRAME::WBarQueryConfig(PWBCONFIGDATA pWBCfg)
{
  if (pWinBar) pWinBar->QueryConfig(pWBCfg);
}
void MDIFRAME::WBarSetConfig(PWBCONFIGDATA pWBCfg)
{
  if (pWinBar) pWinBar->SetConfig(pWBCfg);
}
void MDIFRAME::WBarSetOrder(PHWND aFrames, int nFrames)
{
  if (pWinBar) pWinBar->SetOrder(aFrames, nFrames);
}
/*******************************************************************************************************/
/*                                                                                                     */
/*  Internal class kitchen                                                                             */
/*                                                                                                     */
/*******************************************************************************************************/

MDIFRAME::CLIENTLIST::CLIENTLIST()
{
  memset(this,0,sizeof(*this));
}

MDIFRAME::PCLIENTLIST MDIFRAME::hFr2pcl(HWND hFrame)
{
  for (PCLIENTLIST pcl = pclClients; pcl; pcl=pcl->next)
    if (pcl->hWnd == hFrame) return pcl;
  return 0;
}

MDIFRAME::PCLIENTLIST MDIFRAME::hBar2pcl(HWND hBar)
{
  for (PCLIENTLIST pcl = pclBars; pcl; pcl=pcl->next)
    if (pcl->hWnd == hBar) return pcl;
  return 0;
}


/*******************************************************************************************************/
/*                                                                                                     */
/*  Window placement                                                                                   */
/*                                                                                                     */
/*******************************************************************************************************/

void MDIFRAME::ArrangeChildren()
{
  SWP         swp,swp0;
  PCLIENTLIST pcl;
  ULONG       ul;
  WinQueryWindowPos(h0Client,&swp);
  swp.x = swp.y = 0;
  for (pcl = pclBars; pcl; pcl=pcl->next)
  {
    switch (pcl->usBind)
    {
      case MDIBIND_LEFTBAR:
        ul      = min(swp.cx,pcl->cx);
        pcl->x0 = swp.x;
        pcl->y0 = swp.y;
        pcl->cy = swp.cy;
        swp.x  += ul;
        swp.cx -= ul;
      break;
      case MDIBIND_RIGHTBAR:
        ul      = min(swp.cx,pcl->cx);
        pcl->x0 = swp.x+swp.cx-ul;
        pcl->y0 = swp.y;
        pcl->cy = swp.cy;
        swp.cx -= ul;
      break;
      case MDIBIND_TOPBAR:
        ul      = min(swp.cy,pcl->cy);
        pcl->x0 = swp.x;
        pcl->y0 = swp.y+swp.cy-ul;
        pcl->cx = swp.cx;
        swp.cy -= ul;
      break;
      case MDIBIND_BOTTOMBAR:
        ul      = min(swp.cy,pcl->cy);
        pcl->x0 = swp.x;
        pcl->y0 = swp.y;
        pcl->cx = swp.cx;
        swp.y  += ul;
        swp.cy -= ul;
      break;
    }
    WinSetWindowPos(pcl->hWnd, 0, pcl->x0, pcl->y0,  pcl->cx, pcl->cy, SWP_MOVE|SWP_SIZE);
  }

  //--- hDesk size is changed ?
  WinQueryWindowPos(hDesk,&swp0);
  if (swp0.x != swp.x || swp0.y != swp.y || swp0.cx != swp.cx || swp0.cy != swp.cy)
  {
    WinSetWindowPos(hDesk,0,swp.x,swp.y,swp.cx,swp.cy,SWP_MOVE|SWP_SIZE);
    LONG dcx = swp.cx-swp0.cx;
    LONG dcy = swp.cy-swp0.cy;
    LONG dx,dy;
    HWND hWnd;

    for (pcl = pclClients; pcl; pcl=pcl->next)
    {
      dx = dy = 0;
      switch (pcl->usBind)
      {
        case MDIBIND_LEFTTOP:
          dy = dcy;
        break;
        case MDIBIND_RIGHTTOP:
          dy = dcy;
          dx = dcx;
        break;
        case MDIBIND_RIGHTBOTTOM:
          dx = dcx;
        break;
      }
      pcl->x0 += dx;
      pcl->y0 += dy;
      hWnd = pcl->hWnd;
      if (pcl->fMaxim)
      { // Re-maximize it (the styles fucking used to prevent any window flashing)
        ULONG f = (WinQueryWindowULong(hWnd,QWL_STYLE) & WS_VISIBLE);
        WinSetWindowPos(hWnd,0,0,0,0,0,SWP_MINIMIZE | (f ? SWP_HIDE : 0));
        WinSetWindowPos(hWnd,0,0,0,0,0,SWP_MAXIMIZE | (f ? SWP_SHOW : 0));
      }
      else if ((dx || dy) && !pcl->fMinim)
        WinSetWindowPos(hWnd,0,pcl->x0,pcl->y0,0,0,SWP_MOVE);
    }
  }
}



/*******************************************************************************************************/
/*                                                                                                     */
/*                   M e n u                                                                           */
/*                                                                                                     */
/*******************************************************************************************************/

BOOL MDIFRAME::SetMenuItem (HWND hFrame, PSZ pszItem, int nItem, int nAccel)
{
  PCLIENTLIST pcl = hFr2pcl(hFrame);
  if (fAutoMenu || !pcl) return FALSE;

  free (pcl->pszItem);
  pcl->nItem  = nItem;
  pcl->nAccel = nAccel;
  if (pszItem && (pcl->pszItem=(PSZ)malloc(strlen(pszItem)+1)))
    strcpy(pcl->pszItem,pszItem);
  return TRUE;
}

void MDIFRAME::fill_menu()
{
  int           i,j;
  MENUITEM      mi;
  HWND          hMenu = WinWindowFromID(hMDIFrame,FID_MENU);
  HWND          hPullDown;
  LONG          nTotalWins = 0;
  WinSendMsg(hMenu,MM_QUERYITEM,MPFROM2SHORT(IDM_WINDOW,TRUE),(MPARAM) &mi);
  if (!(hPullDown = mi.hwndSubMenu)) return;
  for (i=IDM_WINFIRST;i<=IDM_WINLAST;i++) WinSendMsg(hPullDown,MM_DELETEITEM,MPFROM2SHORT(i,0),0);
  WinSendMsg(hPullDown,MM_DELETEITEM,MPFROM2SHORT(IDM_WINMORE,0),0);
  mi.hwndSubMenu = NULL;
  mi.afStyle     = MIS_TEXT;
  mi.hItem       = 0;

  PCLIENTLIST   pcl;
  PCLIENTLIST   apcl[MDI_MAXFRAMES];
  USHORT        aus [MDI_MAXFRAMES];
  char          sz[300];
  memset(apcl,0,sizeof(apcl));
  for (pcl=pclClients,i=0; pcl; pcl= pcl->next,i++)
  {
    nTotalWins++;
    if (fAutoMenu)
    {
      apcl[i] = pcl;
      aus [i] = i;
    }
    else
    {
      int nIt   = min(pcl->nItem,(MDI_MAXFRAMES-1));
      if (apcl[nIt]) nTotalWins--; // Buggy?
      apcl[nIt] = pcl;
      aus [nIt] = i;
    }
  }
  for (i=j=0; i<MDI_MAXFRAMES && j<10; i++)
  {
    if (pcl=apcl[i])
    {
      if (fAutoMenu)
        WinQueryWindowText(pcl->hWnd,sizeof(sz),sz);
      else if(pcl->pszItem)
      {
        strncpy(sz,pcl->pszItem,sizeof(sz));
        sz[sizeof(sz)-1] = 0;
      }
      else
        continue;
      mi.afAttribute = 0;
      mi.iPosition = j++;
      mi.id        = IDM_WINFIRST+aus[i]; // aus[i] == number in the pclClients list
      if (WinSendMsg(WinWindowFromID(pcl->hWnd,FID_TITLEBAR),TBM_QUERYHILITE,0,0))
        mi.afAttribute |= MIA_CHECKED;
      WinSendMsg(hPullDown,MM_INSERTITEM,MPARAM(&mi),MPARAM(sz));
    }
  }
  if (j<nTotalWins)
  {
      mi.afAttribute = 0;
      mi.iPosition   = j;
      mi.id          = IDM_WINMORE;
      WinSendMsg(hPullDown,MM_INSERTITEM,MPARAM(&mi),MPARAM("~More windows..."));
  }
}


//
// When Ok it posts the command message with IDM_WINFIRST... command
// to the it's owner
//
MRESULT EXPENTRY dpMDIListDlg(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static    HWND        hLB;
  static    PMDIFRAME   pmdi;

  switch (ulMsg){
  case WM_INITDLG:
  {
    int                     i,j;
    MDIFRAME::PCLIENTLIST   pcl;
    MDIFRAME::PCLIENTLIST   apcl[MDI_MAXFRAMES];
    USHORT                  aus [MDI_MAXFRAMES];
    char                    sz[300];
    int                     nItSel = 0;
    pmdi = (PMDIFRAME)m2;
    hLB  = WinWindowFromID(hWnd,IDD_MDILB);
    memset(apcl,0,sizeof(apcl));
    for (pcl=pmdi->pclClients,i=0; pcl; pcl= pcl->next,i++)
    {
      if (pmdi->fAutoMenu)
      {
        apcl[i] = pcl;
        aus [i] = i;
      }
      else
      {
        int nIt   = min(pcl->nItem,(MDI_MAXFRAMES-1));
        apcl[nIt] = pcl;
        aus [nIt] = i;
      }
    }
    for (i=j=0; i<MDI_MAXFRAMES; i++)
    {
      if (pcl=apcl[i])
      {
        if (pmdi->fAutoMenu)
          WinQueryWindowText(pcl->hWnd,sizeof(sz),sz);
        else if(pcl->pszItem)
        {
          strncpy(sz,pcl->pszItem,sizeof(sz));
          sz[sizeof(sz)-1] = 0;
          char *pch;
          while(pch=strchr(sz,'~')) memmove(pch,pch+1,strlen(pch)+1);
          if (pch=strchr(sz,'\t'))  *pch = '\0';
        }
        else
          continue;
        if (sz[0])
        {
          int nIt = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(LIT_END),sz);
          WinSendMsg(hLB,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(IDM_WINFIRST+aus[i]));
          if (WinSendMsg(WinWindowFromID(pcl->hWnd,FID_TITLEBAR),TBM_QUERYHILITE,0,0))
            nItSel = nIt;
        }
      }
    }
    WinSendMsg(hLB,LM_SELECTITEM,MPARAM(nItSel),MPARAM(TRUE));
    return 0;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_CONTROL:
    if (HWND(m2)==hLB && SHORT2FROMMP(m1)==LN_ENTER)
      m1 = MPARAM(DID_OK);
    else
      break;
  case WM_COMMAND:
  {
    USHORT usCmd = SHORT1FROMMP(m1);
    switch(usCmd)
    {
    case DID_OK:
    {
      int i = (int) WinSendMsg(hLB,LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
      i     = (int) WinSendMsg(hLB,LM_QUERYITEMHANDLE,MPARAM(i),        0);
      WinPostMsg(WinQueryWindow(hWnd,QW_OWNER),WM_COMMAND,MPARAM(i),0);
    } // No break here
    case DID_CANCEL:
      WinDismissDlg(hWnd,0);
      break;
    case IDD_MDIPBHELP:
      DisplayHelpPanel(PANEL_MDILISTDLG);
      break;
    }
    break;
  }
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_MDILISTDLG);
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


/*======================================================================================================*/
/*=============================/                            \===========================================*/
/*=========================   ->>> W I N B A R    A R E A <<<-   =======================================*/
/*=============================\                            /===========================================*/
/*======================================================================================================*/


//--- Single button(s):


MRESULT EXPENTRY fwpWBBtn(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static  PFNWP   pWindowProc;
  PWBBUTTON       pWb;
  if (ulMsg!=WM_CREATE) pWb = (PWBBUTTON)WinQueryWindowULong(hWnd,QWL_USER);

  if (ulMsg == MM_SUBCLASS)
  {
    pWindowProc = PFNWP(ULONG(m1));
    return 0;
  }
  else if (pWb && ulMsg==WM_BUTTON2UP)
  {
    HWND hTopFrame = pWb->pWBar->pMDI->QueryWindow(MDIQW_UPPERFRAME);
    pWb->pWBar->PopMenu(pWb->hFrame, hTopFrame==pWb->hFrame);
    return 0;
  }
  return ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
}

WBBUTTON::WBBUTTON(HWND hParent, PWINBAR pWBar, BOOL fShowColors, USHORT usCmd, PWBNEWWINDOW pWBWin)
{
  memset(this,0,sizeof(*this));
  hFrame            = pWBWin->hFrame;
  rgbFore           = pWBWin->rgbFore;
  rgbBack           = pWBWin->rgbBack;
  this->pWBar       = pWBar;
  this->fShowColors = fShowColors;
  switch(pWBWin->ulWB_Bmp){
  case WB_BMP_ED:       hBmp = WINBAR::hbmEd;      break;
  case WB_BMP_SRCLIST:  hBmp = WINBAR::hbmSrcList; break;
  case WB_BMP_MSGLIST:  hBmp = WINBAR::hbmMsgList; break;
  }
  if (!(pszText = (PSZ)malloc(strlen(pWBWin->pszText)+1))) return;
  strcpy(pszText,pWBWin->pszText);
  // Create the button
  BTNCDATA   btnCData = {sizeof(BTNCDATA),0,0,0};
  hBtn                = WinCreateWindow(hParent,WC_BUTTON, "Button",
                                        BS_PUSHBUTTON | BS_USERBUTTON | BS_NOPOINTERFOCUS | WS_VISIBLE,
                                        0,0,1,1, hParent, HWND_TOP, usCmd, &btnCData, NULL);
  WinSetWindowULong(hBtn,QWL_USER,ULONG(this));
  fwpWBBtn(hBtn, MM_SUBCLASS, MPARAM(ULONG(WinSubclassWindow(hBtn,(PFNWP)fwpWBBtn))),0L);
}
WBBUTTON::~WBBUTTON()
{
  WinDestroyWindow(hBtn);
  free(pszText);
  memset(this,0,sizeof(*this));
}
void WBBUTTON::set_pp_font(PSZ szPPFont)
{
  if (!hBtn) return;
  WinSetPresParam(hBtn,PP_FONTNAMESIZE,strlen(szPPFont)+1,szPPFont);
}
void WBBUTTON::set_show_colors(BOOL fShow)
{
  if (!hBtn) return;
  fShowColors = fShow;
  WinInvalidateRect(hBtn,0,0);
}


//---  WINBAR

MRESULT EXPENTRY fwpWBarBack(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  PWINBAR pWBar = (PWINBAR)WinQueryWindowULong(hWnd,QWL_USER);
  switch(ulMsg)
  {
    case WM_TIMER:
    {
      if (pWBar && pWBar->fReAlignMe)
      {
        pWBar->fReAlignMe = FALSE;
        pWBar->ReAlignAll(TRUE);
      }
      break;
    }
    case WM_PAINT:
    {
      RECTL rcl;
      HPS   hps    = WinBeginPaint(hWnd,0,&rcl);
      WinFillRect(hps,&rcl,CLR_PALEGRAY);
      WinEndPaint(hps);
      break;
    }
    case WM_SIZE:
    {
      if (pWBar && !pWBar->fResizeInProgress)
        WinPostMsg(hWnd,WM_USER,0,0);
      break;
    }
    case WM_USER:
    {
      if (pWBar && !pWBar->fResizeInProgress)
        pWBar->ReAlignAll();
      break;
    }
    case WM_PRESPARAMCHANGED:
    {
      if (m1==MPARAM(PP_FONTNAMESIZE) && pWBar)
        pWBar->ReAlignAll();
      break;
    }
    case WM_COMMAND:
    {
      HWND     hPopFrame = (pWBar ? pWBar->hPopFrame : 0);
      SHORT    s1m1      = SHORT1FROMMP(m1);
      if (s1m1 >= IDM_POP_MVAGROUP && s1m1 <= IDM_POP_MV9GROUP) // Вообще-то это к MDI не отностится... :-(
      {
        BOOL curved_call(HWND hFrame, int nGroup);
        // мувит фрейм в группу, если группа неверна - возвращает мувэйбльность фрейма
        curved_call(hPopFrame,s1m1-IDM_POP_MVAGROUP);
      }
      else
        switch (s1m1)
        {
          case IDM_POP_WBBCLOSE:
            if (hPopFrame)
              WinPostMsg(hPopFrame,WM_SYSCOMMAND,MPARAM(SC_CLOSE),   MPFROM2SHORT(CMDSRC_OTHER,0));
            break;
          case IDM_POP_WBBRESTORE:
            if (hPopFrame)
              WinSetWindowPos(hPopFrame,HWND_TOP,0,0,0,0,SWP_RESTORE|SWP_ZORDER|SWP_SHOW|SWP_ACTIVATE);
            break;
          case IDM_POP_WBBMAXIMIZE:
            if (hPopFrame)
              WinSetWindowPos(hPopFrame,HWND_TOP,0,0,0,0,SWP_MAXIMIZE|SWP_ZORDER|SWP_SHOW|SWP_ACTIVATE);
            break;
          case IDM_POP_WBBHIDE:
            if (hPopFrame)
              WinPostMsg(hPopFrame,WM_SYSCOMMAND,MPARAM(SC_HIDE), MPFROM2SHORT(CMDSRC_OTHER,0));
            break;
          case IDM_POP_WBBVTILE:
          case IDM_POP_WBBHTILE:
          {
            HWND hTopFrame = 0;
            if (pWBar) hTopFrame = pWBar->pMDI->QueryWindow(MDIQW_UPPERFRAME);
            if (!hPopFrame || !hTopFrame || hPopFrame==hTopFrame) break;
            SWP swp;
            WinQueryWindowPos(WinQueryWindow(hPopFrame,QW_PARENT),&swp);
            WinSetWindowPos(hTopFrame, 0,0,0,0,0, SWP_RESTORE);
            WinSetWindowPos(hPopFrame, 0,0,0,0,0, SWP_RESTORE);
            if (s1m1==IDM_POP_WBBVTILE)
            {
              WinSetWindowPos(hPopFrame, HWND_TOP, swp.cx/2, 0, swp.cx/2, swp.cy, SWP_MOVE|SWP_SIZE|SWP_SHOW|SWP_ZORDER);
              WinSetWindowPos(hTopFrame, HWND_TOP, 0,        0, swp.cx/2, swp.cy, SWP_MOVE|SWP_SIZE|SWP_SHOW|SWP_ZORDER|SWP_ACTIVATE);
            }
            else
            {
              WinSetWindowPos(hPopFrame, HWND_TOP, 0, swp.cy/2, swp.cx, swp.cy/2, SWP_MOVE|SWP_SIZE|SWP_SHOW|SWP_ZORDER);
              WinSetWindowPos(hTopFrame, HWND_TOP, 0, 0,        swp.cx, swp.cy/2, SWP_MOVE|SWP_SIZE|SWP_SHOW|SWP_ZORDER|SWP_ACTIVATE);
            }
            break;
          }
          case IDM_POP_WBFONT:
          {
            void SetPPFont(HWND hWnd, PSZ pszPPCopy=0);
            SetPPFont(pWBar->hWBar);
            break;
          }
          case IDM_POP_WBCOLORS:
          case IDM_POP_WBWARP:
          {
            WBCONFIGDATA wbcd;
            pWBar->QueryConfig(&wbcd);
            wbcd.ulWBV ^= (s1m1==IDM_POP_WBCOLORS ? WBV_SHOWCOLOR : WBV_MULTILINE);
            pWBar->SetConfig(&wbcd);
            break;
          }
          default:
          {
            PWBBUTTON pWb;
            HWND      hBt = WinWindowFromID(hWnd,s1m1);
            if (hBt && (pWb = (PWBBUTTON)WinQueryWindowULong(hBt,QWL_USER)))
              return WinSendMsg(WinQueryWindow(hWnd,QW_OWNER),WM_CONTROL,MPFROM2SHORT(0,WBN_CLICKED),MPARAM(pWb->hFrame));
          }
        } //switch (s1m1)
      break;
    }
    case WM_CONTROL:
    {
      if (SHORT2FROMMP(m1)==BN_PAINT) // Redraw button
      {
        POINTL       ptl;
        RECTL        rcl;
        PUSERBUTTON  pUb     = (PUSERBUTTON)m2;                                    if (!pUb) return 0;
        PWBBUTTON    pWb     = (PWBBUTTON)WinQueryWindowULong(pUb->hwnd,QWL_USER); if (!pWb) return 0;
        HPS          hps     = pUb->hps;
        RECTL        rclBtn;   WinQueryWindowRect(pUb->hwnd,&rclBtn);
        ULONG        clrFG   = CLR_BLACK;
        ULONG        clrBG   = CLR_PALEGRAY;
        BOOL         fPushed = (pUb->fsState  == BDS_HILITED || pWb->fFocusON);
        int          dx      = (fPushed) ? 1 : 0;
        int          dy      = -dx;
        if (pWb->fShowColors || pWb->fFocusON)
        {
          if (pWb->fShowColors)
          {
            clrFG = pWb->rgbFore;
            clrBG = pWb->rgbBack;
          }
          else
          {
            clrFG = 0x000000;
            clrBG = 0xf0f0f0;
          }
          GpiCreateLogColorTable(hps,LCOL_PURECOLOR,LCOLF_RGB,0,0,0);
        }
        GpiSetBackMix(hps,BM_OVERPAINT);
        WinFillRect(hps,&rclBtn,clrBG);
        // Bitmap:
        ptl    = pWb->pWBar->ptlBmp;
        ptl.x += dx;
        ptl.y += dy;
        WinDrawBitmap(hps,pWb->hBmp,NULL,&ptl,clrFG,clrBG,DBM_NORMAL);
        // Text:
        rcl.xLeft   = pWb->pWBar->ptlText.x + dx;
        rcl.xRight  = rclBtn.xRight;
        rcl.yBottom = pWb->pWBar->ptlText.y + dy;
        rcl.yTop    = rclBtn.yTop;
        WinDrawText(hps, -1, pWb->pszText, &rcl, clrFG, clrBG, DT_LEFT|DT_BOTTOM);
        rclBtn.xRight--; rclBtn.yTop--;
        // Borders:
        GpiCreateLogColorTable(hps,LCOL_RESET,LCOLF_INDRGB,0,0,0);
        ptl.x = 0; ptl.y = 0;
        GpiSetColor(hps,CLR_DARKGRAY);
        GpiSetCurrentPosition(hps,&ptl);
        ptl.y = rclBtn.yTop;   GpiLine(hps,&ptl);
        ptl.x = rclBtn.xRight; GpiLine(hps,&ptl);
        ptl.y = 0;             GpiLine(hps,&ptl);
        ptl.x = 0;             GpiLine(hps,&ptl);
        GpiSetColor(hps,CLR_PALEGRAY);
                                                                GpiLine(hps,&ptl);
        ptl.y = rclBtn.yTop;   GpiSetCurrentPosition(hps,&ptl); GpiLine(hps,&ptl);
        ptl.x = rclBtn.xRight; GpiSetCurrentPosition(hps,&ptl); GpiLine(hps,&ptl);
        ptl.y = 0;             GpiSetCurrentPosition(hps,&ptl); GpiLine(hps,&ptl);

        GpiSetColor(hps,fPushed ? CLR_WHITE : CLR_DARKGRAY);
        ptl.x = rclBtn.xRight-1;
        ptl.y = rclBtn.yTop  -1;
        GpiSetCurrentPosition(hps,&ptl);
        ptl.x = 1;               GpiLine(hps,&ptl);
        ptl.y = 1;               GpiLine(hps,&ptl);
        GpiSetColor(hps,fPushed ? CLR_DARKGRAY : CLR_WHITE);
        ptl.y = rclBtn.yTop-1;   GpiLine(hps,&ptl);
        ptl.x = rclBtn.xRight-1; GpiLine(hps,&ptl);
      }
      break;
    }
    case WM_BUTTON2UP:
    {
      PWINBAR pWBar = (PWINBAR)WinQueryWindowULong(hWnd,QWL_USER);
      if (pWBar) pWBar->PopMenu(0,TRUE);
      break;
    }
    default: return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


HBITMAP WINBAR::hbmEd      = 0;
HBITMAP WINBAR::hbmMsgList = 0;
HBITMAP WINBAR::hbmSrcList = 0;

WINBAR::WINBAR(HWND hParent, HWND hOwner, PMDIFRAME pMDI)
{
  static BOOL f1stCall = TRUE;
  if (f1stCall)
    WinRegisterClass(MDIFRAME::hAB,"WC_WINBAR_BACK", (PFNWP)fwpWBarBack, CS_SIZEREDRAW | CS_CLIPCHILDREN, 4L);
  f1stCall = FALSE;

  memset(this,0,sizeof(*this));
  this->pMDI    = pMDI;
  this->hParent = hParent;
  this->hOwner  = hOwner;
  ulWBV         = WBV_MULTILINE;
  fSelfSort     = TRUE;
  fResizeInProgress = TRUE;
  hWBar = WinCreateWindow(hParent, "WC_WINBAR_BACK", 0, WS_VISIBLE, 0,0,1,1, hOwner, HWND_TOP, 0,0,NULL);
  WinSetWindowULong (hWBar,QWL_USER,ULONG(this));
  WinStartTimer(pMDI->hAB,hWBar,1,500);
  fResizeInProgress = FALSE;
  WBSetPPFont("8.Helv"); // We'll be resized & realigned inside
}
WINBAR::~WINBAR()
{
  // We'll not destroy the contents. Just forget all.
  memset(this,0,sizeof(*this));
}
void  WINBAR::ClearAll()
{
  for (int i=0; i<nBtns; i++)
  {
    if (apWBtns[i])
    {
      delete (PWBBUTTON)WinQueryWindowULong(apWBtns[i]->hBtn,QWL_USER);
      WinDestroyWindow(apWBtns[i]->hBtn);
      apWBtns[i] = 0;
    }
  }
  nBtns = 0;
}
void  WINBAR::AddButton(PWBNEWWINDOW pWBWin)
{
  if (nBtns<MDI_MAXFRAMES)
  {
    PWBBUTTON  pWb  = new WBBUTTON(hWBar, this, ulWBV & WBV_SHOWCOLOR, nBtns, pWBWin);
    int        i;
    PSZ        pszN;
    if ((pszN = sf_skipspaces(pWb->pszText)) != pWb->pszText)
    {
      memmove(pWb->pszText,pszN,strlen(pszN)+1);
      pszN = pWb->pszText;
    }
    i = nBtns;
    if (pszN[0] && fSelfSort)
    {
      for (i=0; i<nBtns; i++)
      {
        PSZ pszCmp = apWBtns[i]->pszText;
        if ( sf_isdigit(pszCmp[0]) && !sf_isdigit(pszN[0])) break;
        if (!sf_isdigit(pszCmp[0]) &&  sf_isdigit(pszN[0])) continue;
        if (sf_stricmp(pszCmp,pszN) > 0)                    break;
      }
    }
    memmove(apWBtns+i+1,apWBtns+i,(nBtns-i)*sizeof(apWBtns[0]));
    nBtns++;
    apWBtns[i] = pWb;
    ReAlignAll();
  }
}
void WINBAR::DeleteButton(HWND hFrame)
{
  BOOL fOk = FALSE;
  for (int i=0; i<nBtns; i++)
  {
    PWBBUTTON pWb = apWBtns[i];
    if (pWb->hFrame == hFrame)
    {
      fOk = TRUE;
      delete pWb;
      memmove(apWBtns+i,apWBtns+i+1,(nBtns-i-1)*sizeof(apWBtns[0]));
      nBtns--;
    }
  }
  if (fOk) ReAlignAll();
}
void  WINBAR::SetViewMode(ULONG ulWBV)
{
  this->ulWBV = ulWBV;
  /*+++ */
}
void  WINBAR::WBSetPPFont(PSZ szPPFont)
{
  int  l = strlen(szPPFont)+1;
  if   (l>1) WinSetPresParam(hWBar,PP_FONTNAMESIZE,l,szPPFont);
  else       ReAlignAll();
}
void  WINBAR::PopMenu(HWND hFrame, BOOL fNoTile)
{
  void PlayMenu(HWND hWnd, ULONG ulRCMenu, LONG alChecked[], LONG alDisabled[], LONG lPosItem, LONG xPos=-1000, LONG yPos=-1000);
  BOOL curved_call(HWND hFrame, int nGroup); // мувит фрейм в группу, если группа неверна - возвращает мувэйбльность фрейма
  LONG aChk[3]  = {0,0,0};
  LONG aDsb[]   = {IDM_POP_WBBVTILE,    IDM_POP_WBBHTILE, IDM_POP_WBBCLOSE, IDM_POP_WBBRESTORE,
                   IDM_POP_WBBMAXIMIZE, IDM_POP_WBBHIDE,  IDM_POP_MVGROUPS, 0};
  if (ulWBV & WBV_SHOWCOLOR) aChk[0]               = IDM_POP_WBCOLORS;
  if (ulWBV & WBV_MULTILINE) aChk[aChk[0] ? 1 : 0] = IDM_POP_WBWARP;
  if (hFrame)
  {
    aDsb[0] = 0;
    int idx = 0;
    SWP swp;
    WinQueryWindowPos(hFrame,&swp);
    if (!(swp.fl & (SWP_MAXIMIZE|SWP_HIDE))) aDsb[idx++] = IDM_POP_WBBRESTORE;
    if (swp.fl & SWP_MAXIMIZE)               aDsb[idx++] = IDM_POP_WBBMAXIMIZE;
    if (swp.fl & SWP_HIDE)                   aDsb[idx++] = IDM_POP_WBBHIDE;
    if (fNoTile)
    {                                        aDsb[idx++] = IDM_POP_WBBVTILE;
                                             aDsb[idx++] = IDM_POP_WBBHTILE;
    }
    if (!curved_call(hFrame,-1))             aDsb[idx++] = IDM_POP_MVGROUPS;
    aDsb[idx] = 0;
  }
  hPopFrame = hFrame;
  PlayMenu(hWBar, IDM_POPUP_WINBAR, aChk, aDsb, 0);
}
void  WINBAR::QueryConfig(PWBCONFIGDATA pWBCfg)
{
  ULONG  ulAttrFound;
  WinQueryPresParam(hWBar,PP_FONTNAMESIZE,0,&ulAttrFound,sizeof(pWBCfg->szPPFont),pWBCfg->szPPFont,QPF_NOINHERIT);
  if(PP_FONTNAMESIZE != ulAttrFound) pWBCfg->szPPFont[0] = 0;
  pWBCfg->ulWBV = this->ulWBV;
}
void  WINBAR::SetNewText(HWND hFrame,PSZ szText)
{
  PWBBUTTON pWb;
  for (int i=0; i<nBtns; i++)
  {
    if ((pWb=apWBtns[i]) && pWb->hFrame==hFrame)
    {
      PSZ pszTxt = sf_skipspaces(szText);
      PSZ pszN   = (PSZ)malloc(strlen(pszTxt)+1);
      if (pszN)
      {
        strcpy(pszN,pszTxt);
        free(pWb->pszText);
        pWb->pszText = pszN;
        {// New place in alphabetical order:
          memmove(apWBtns+i,apWBtns+i+1,(nBtns-i-1)*sizeof(apWBtns[0]));
          nBtns--;
          i=nBtns;
          if (pszN[0] && fSelfSort)
            for (i=0; i<nBtns; i++)
            {
              PSZ pszCmp = apWBtns[i]->pszText;
              if (!sf_isdigit(pszCmp[0]) &&  sf_isdigit(pszN[0])) break;
              if ( sf_isdigit(pszCmp[0]) && !sf_isdigit(pszN[0])) continue;
              if (sf_stricmp(pszCmp,pszN) > 0)                    break;
            }
          memmove(apWBtns+i+1,apWBtns+i,(nBtns-i)*sizeof(apWBtns[0]));
          nBtns++;
          apWBtns[i] = pWb;
          ReAlignAll();
        }
      }
    }
  }
}
void WINBAR::SetNewColors(HWND hFrame,ULONG rgbFore, ULONG rgbBack)
{
  PWBBUTTON pWb;
  for (int i=0; i<nBtns; i++)
  {
    if ((pWb=apWBtns[i]) && pWb->hFrame==hFrame)
    {
      pWb->rgbFore = rgbFore;
      pWb->rgbBack = rgbBack;
      if (pWb->fShowColors)
        WinInvalidateRect(pWb->hBtn,0,0);
    }
  }
}

void WINBAR::ShowFocus(HWND hFrame, BOOL fFocus)
{
  PWBBUTTON pWb;
  for (int i=0; i<nBtns; i++)
  {
    if ((pWb=apWBtns[i]) && pWb->hFrame==hFrame)
    {
      if (pWb->fFocusON != !!fFocus)
      {
        pWb->fFocusON = !!fFocus;
        WinInvalidateRect(pWb->hBtn,0,0);
      }
    }
  }
}

void  WINBAR::SetConfig(PWBCONFIGDATA pWBCfg)
{
  ulWBV = pWBCfg->ulWBV;
  for (int i=0; i<nBtns; i++)
    apWBtns[i]->set_show_colors(ulWBV & WBV_SHOWCOLOR);
  WBSetPPFont(pWBCfg->szPPFont);
}
void  WINBAR::SetOrder(PHWND aFrames, int nFrames)
{
  if (!nFrames)
  {
    fSelfSort = TRUE;
    return;
  }
  fSelfSort   = FALSE;
  BOOL fMoved = FALSE;
  int  nB, n;
  for (nB = 0; nB<nFrames && nB<nBtns; nB++)
  {
    if (apWBtns[nB]->hFrame != aFrames[nB])
      for (n=nBtns-1; n>=0; n--)
      {
        if (apWBtns[n]->hFrame == aFrames[nB])
        {
          PWBBUTTON pwb = apWBtns[nB];
          apWBtns[nB]   = apWBtns[n];
          apWBtns[n]    = pwb;
          fMoved        = TRUE;
          break;
        }
      }
  }
  if (fMoved)
    ReAlignAll();
}
void  WINBAR::AdjustParentPos()
{
  SWP swp;
  if (WinQueryWindowPos(hParent,&swp))
    WinSetWindowPos(hWBar,0,0,0,swp.cx,swp.cy,SWP_MOVE|SWP_SIZE);
}
void  WINBAR::ReAlignAll(BOOL fShow)
{
  int  nBtnInLine = 1;
  LONG xStep      = 1;
  {// Optimize window size
    SWP               swp;
    HPS               hps = WinGetPS(hWBar);
    FONTMETRICS       fm;
    BITMAPINFOHEADER  bmpData;
    GpiQueryFontMetrics(hps,sizeof(fm),&fm);
    LONG              lBMWidth   = 1;
    LONG              lBMHeight  = 1;
    LONG              lTxtWidth  = 80;
    LONG              lTxtHeight = fm.lMaxBaselineExt+fm.lExternalLeading;
    SWP               swpBar;
    {// Examine text sizes
      POINTL aptl[TXTBOX_COUNT];
      if (GpiQueryTextBox(hps, 12, "Example.Name", TXTBOX_COUNT,aptl))
        lTxtWidth = aptl[TXTBOX_TOPRIGHT].x;
    }
    WinReleasePS(hps);
    if (GpiQueryBitmapParameters(hbmEd,&bmpData))
    {
      lBMWidth  = bmpData.cx;
      lBMHeight = bmpData.cy;
    }
    cyBtn       = max (lTxtHeight, lBMHeight)+4;
    cxBtn       = lBMWidth + lTxtWidth + 10;
    ptlBmp.x    = 4;
    ptlText.x   = 4 + 2 + lBMWidth;
    ptlBmp.y    = (cyBtn - lBMHeight )/2;
    ptlText.y   = (cyBtn - lTxtHeight)/2;
    WinQueryWindowPos(hWBar,&swpBar);
    nBtnInLine  = max(swpBar.cx-5,0)/(cxBtn+xStep); if (nBtnInLine<1) nBtnInLine = 1;
    cxBtn       = max(1,(swpBar.cx-5)/nBtnInLine-xStep);
    int nLines  = (ulWBV & WBV_MULTILINE) ? (nBtns+nBtnInLine-1)/nBtnInLine :
                  ((ulWBV & WBV_HIDDEN) ? 0 : 1);
    int hghBar  = nLines ? nLines*(cyBtn+1)+2 : 0;
    WinQueryWindowPos(hWBar,&swp);
    if (swp.cy!=hghBar)
    {
      fResizeInProgress = TRUE;
      WinSendMsg(hOwner,WM_CONTROL,MPFROM2SHORT(0,WBN_RESIZEME),MPARAM(hghBar));
      fResizeInProgress = FALSE;
    }

    if (!(ulWBV & WBV_MULTILINE) && nBtnInLine<nBtns)
    {
      cxBtn      = max(1,(swpBar.cx-2)/nBtns);
      xStep      = 0;
      nBtnInLine = nBtns;
    }
  }
  if (fShow)
  {
    SWP   swpBar;
    WinQueryWindowPos(hWBar,&swpBar);
    LONG  xPos    = 1 + 2*xStep;
    LONG  yPos    = swpBar.cy - 1 - cyBtn;
    for (int cbtn = nBtnInLine, nCurBtn = 0; nCurBtn<nBtns & yPos>0; nCurBtn++)
    {
      HWND hBtn = apWBtns[nCurBtn]->hBtn;
      SWP  swp;
      WinQueryWindowPos(hBtn,&swp);
      if (swp.x  != xPos  || swp.y  != yPos ) WinSetWindowPos(hBtn,0,xPos,yPos,0,0,SWP_MOVE);
      if (swp.cx != cxBtn || swp.cy != cyBtn) WinSetWindowPos(hBtn,0,0,0,cxBtn,cyBtn,SWP_SIZE);
      xPos += cxBtn+xStep;
      if (!--cbtn)
      {
        cbtn = nBtnInLine;
        xPos  = 3;
        yPos -= cyBtn+1;
      }
    }
    WinInvalidateRect(hWBar,0,TRUE);
  }
  else
    fReAlignMe = TRUE;
}


