/*
*
*  File: bars.cpp
*
*  XDS Shell toolbar and status line management
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#define  INCL_WIN
#define  INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "XDSBASE.H"

#define  TOOLBARBACKCLASS     "TBBackClass"
#define  STATUSBARCLASS       "StatusBarClass"
#define  STATUSDISPCLASS      "StatusDispClass"

/*>>> Globals
*/
HWND           hTBBack;  // Toolbar
HWND           hWBBack;  // Winbar
HWND           hStatusBar;
PTOOLBAR       pToolbar       = 0;
PTOOLBAR       pWinBar        = 0;

/*>>> Function prototypes
*/
MRESULT EXPENTRY wpTBBack      (HWND,ULONG,MPARAM,MPARAM);  // MDI bar under the toolbar window
MRESULT EXPENTRY wpStatusBar   (HWND,ULONG,MPARAM,MPARAM);  // Status bar contains
MRESULT EXPENTRY wpStatusDisp  (HWND,ULONG,MPARAM,MPARAM);  //   two status displays


PSZ BrCreate()
{
  static BOOL   fInited = FALSE;
  BOOL          fOk;
  RECTL         rcl;
  if (!fInited)
  {
    if (!WinRegisterClass(hAB,TOOLBARBACKCLASS, (PFNWP)wpTBBack,     CS_SIZEREDRAW,4L) ||
        !WinRegisterClass(hAB,STATUSBARCLASS,   (PFNWP)wpStatusBar,  CS_SIZEREDRAW,4L) ||
        !WinRegisterClass(hAB,STATUSDISPCLASS,  (PFNWP)wpStatusDisp, CS_SIZEREDRAW,4L))
    return "Can't registrate toolbar's window classes";
  }
  fInited = TRUE;

  //--- Create TOOLBAR
  hTBBack  = pMDI->CreateChild(MDIBIND_TOPBAR, TOOLBARBACKCLASS, 0, WS_VISIBLE|WS_CLIPSIBLINGS, 0, 0,0,1,1, hMDI0Client,0,0);
  pToolbar = new TOOLBAR(&fOk, hAB, hTBBack, hMDI0Client, X_TBBUTTON_SIZE, Y_TBBUTTON_SIZE, 0);
  if (!hTBBack || !fOk)
    return "Can't initialize toolbar class";
  TOOLBAR::NEWBTN anb [] =
  {
    0, hMDI0Client, IDM_FILENEW,       IDB_TBBNEW,       0x0, TRUE, "Edit new file",
    0, hMDI0Client, IDM_FILELOAD,      IDB_TBBOPEN,      0x0, TRUE, "Load file...",
    0, hMDI0Client, IDM_FILESAVE,      IDB_TBBSAVE,      0x0, TRUE, "Save current edit file",
    0, hMDI0Client, IDM_FILEPRINT,     IDB_TBBPRINT,     0x0, TRUE, "Print file/selection",
    0, hMDI0Client, 0,                 0,                0x0, TRUE, " ",
    0, hMDI0Client, IDM_EDITUNDO,      IDB_TBBUNDO,      0x0, TRUE, "Undo last edit action",
    0, hMDI0Client, 0,                 0,                0x0, TRUE, " ",
    0, hMDI0Client, IDM_EDITCUT,       IDB_TBBCUT,       0x0, TRUE, "Cut selection to the clipboard",
    0, hMDI0Client, IDM_EDITCOPY,      IDB_TBBCOPY,      0x0, TRUE, "Copy selection to the clipboard",
    0, hMDI0Client, IDM_EDITPASTE,     IDB_TBBPASTE,     0x0, TRUE, "Paste text from the clipboard",
    0, hMDI0Client, 0,                 0,                0x0, TRUE, " ",
    0, hMDI0Client, IDM_EDITFIND,      IDB_TBBFIND,      0x0, TRUE, "Find / Replace (Shift - File find)...",
    0, hMDI0Client, IDM_EDITFINDNEXT,  IDB_TBBFINDNEXT,  0x0, TRUE, "Find next (Shift - previous)",
    0, hMDI0Client, 0,                 0,                0x0, TRUE, " ",
    0, hMDI0Client, IDM_OPTSETTINGS,   IDB_TBBSETTINGS,  0x0, TRUE, "Settings...",
    0, hMDI0Client, 0,                 0,                0x0, TRUE, " ",
  };
  pToolbar->InsertMultButton (anb, sizeof(anb)/sizeof(anb[0]), 0);
  pToolbar->QueryRequiredRect(&rcl);
  pMDI->SetBarWidth(hTBBack,rcl.yTop);

  /*>>>   C r e a t e   S T A T U S   L I N E  <<<*/
  hStatusBar = pMDI->CreateChild(MDIBIND_BOTTOMBAR, STATUSBARCLASS, 0, WS_VISIBLE|WS_CLIPSIBLINGS,
                                 0, 0,0,1,Y_STATUSBAR_SIZE, hMDI0Client,0,0);
  if (!hStatusBar) return "Can't create status line window";
  return 0;
}

void BrDisplayComment(PSZ szComment,LONG lSec)
{
  WinSendMsg(hStatusBar,STM_DISPLAYTEXT,MPARAM(szComment),MPARAM(lSec));
}

void BrForceUpdate()
{
  WinSendMsg(hStatusBar,WM_TIMER,MPARAM(2),0);
}

void BrDrawNiceBorder(HPS hps, LONG x, LONG y, LONG cx, LONG cy, ULONG clrLight, ULONG clrDark)
// Draw 2-pixel width border
{
  POINTL ptl;
  if (cx<=0 || cy<=0) return;
  ptl.x = x; ptl.y = y+1;
  GpiSetCurrentPosition(hps,&ptl);
  GpiSetColor(hps,clrLight);
  ptl.y += (cy-2);  GpiLine(hps,&ptl);
  ptl.x += (cx-1);  GpiLine(hps,&ptl);
  ptl.x--;  ptl.y--;
  GpiSetCurrentPosition(hps,&ptl);
  ptl.y -= (cy-3);  GpiLine(hps,&ptl);
  ptl.x -= (cx-3);  GpiLine(hps,&ptl);
  ptl.x = x; ptl.y = y;
  GpiSetCurrentPosition(hps,&ptl);
  GpiSetColor(hps,clrDark);
  ptl.x += (cx-1);  GpiLine(hps,&ptl);
  ptl.y += (cy-2);  GpiLine(hps,&ptl);
  ptl.x = x+1; ptl.y = y+2;
  GpiSetCurrentPosition(hps,&ptl);
  ptl.y += (cy-4);  GpiLine(hps,&ptl);
  ptl.x += (cx-4);  GpiLine(hps,&ptl);
}

void BrEnableButton(ULONG ulId, BOOL fEnable)
{
}

MRESULT EXPENTRY wpStatusDisp(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
//
// Responds on STM_DISPLAYTEXT (PSZ m1) by displaying the text pointed by m1 parameter
//
// Responds on STM_SETCOLORS (ULONG m1,m2) by setting fore & back CLR_* values (m1 & m2)
//
{
  struct DSPWINSTR
  {
    HPS    hps;
    PSZ    pszText;
    RECTL  rclWin;
    ULONG  clrFore;
    ULONG  clrBack;
  };
  typedef DSPWINSTR *PDSPWINSTR;

  PDSPWINSTR pds = (PDSPWINSTR)WinQueryWindowULong(hWnd,QWL_USER);

  switch(ulMsg)
  {
  case WM_CREATE:
  {
    FATTRS    fAttrs;
    WinDefWindowProc(hWnd,ulMsg,m1,m2);
    pds = (PDSPWINSTR)malloc(sizeof(DSPWINSTR));
    memset(pds,0,sizeof(pds));
    pds->clrFore = CLR_BLACK;
    pds->clrBack = CLR_PALEGRAY;
    WinSetWindowULong(hWnd,QWL_USER,ULONG(pds));
    pds->hps = WinGetPS(hWnd);
    memset(&fAttrs,0,sizeof(fAttrs));
    fAttrs.usRecordLength    = sizeof(fAttrs);
    strcpy(fAttrs.szFacename,"System VIO");
    fAttrs.lMatch            = 67;
    fAttrs.lMaxBaselineExt   = 10;
    fAttrs.lAveCharWidth     = 6;
    fAttrs.fsFontUse         = 0;//FATTR_FONTUSE_NOMIX;
    GpiCreateLogFont(pds->hps,NULL,1,&fAttrs);
    GpiSetCharSet   (pds->hps,1);
    WinQueryWindowRect(hWnd,&pds->rclWin);
    break;
  }
  case WM_SIZE:
  {
    if (pds) WinQueryWindowRect(hWnd,&pds->rclWin);
    break;
  }
  case WM_PAINT:
  {
    RECTL rcl;
    HPS         hpsPaint = WinBeginPaint(hWnd,0,&rcl);
    if (pds)
    {
      WinFillRect(hpsPaint,&rcl,pds->clrBack);
      if (pds) BrDrawNiceBorder(hpsPaint, 0, 0, pds->rclWin.xRight, pds->rclWin.yTop, CLR_WHITE, CLR_DARKGRAY);
    }
    WinEndPaint(hpsPaint);
    WinSendMsg(hWnd,STM_DISPLAYTEXT,MPARAM(0),0);
    break;
  }
  case STM_DISPLAYTEXT:
  { /* m1 = pszText or 0 to re-display the old*/
    /* m2 = time (in seconds) or 0            */

    if (!pds) break;

    if (m1)
    {
      free(pds->pszText);
      pds->pszText = (PSZ)malloc(strlen((PSZ)m1)+1);
      strcpy(pds->pszText,(PSZ)m1);
    }
    RECTL rcl;
    rcl.xLeft   = 6;
    rcl.yBottom = 2;
    rcl.yTop    = max(pds->rclWin.yTop   - 2, 2);
    rcl.xRight  = max(pds->rclWin.xRight - 2, 2);
    char *pch     = pds->pszText;
    if (!pch) pch = "";
    WinDrawText(pds->hps,-1,pch,&rcl, pds->clrFore, pds->clrBack, DT_LEFT|DT_VCENTER|DT_ERASERECT);
    break;
  }
  case STM_SETCOLORS:
    /*>>> m1 = CLR_* forecolor, m2 = CLR_* backcolor */
    if(pds)
    {
      pds->clrFore = ULONG(m1);
      pds->clrBack = ULONG(m2);
    }
    break;
  default: return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

MRESULT EXPENTRY wpStatusBar(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
//
// Responds on STM_DISPLAYTEXT(PSZ m1, LONG m2) by displaying the text pointed by
//   m1 in the comment display during m2 seconds (infinity when m2==0)
// The position display refreshes automatically (using timer)
//
{
  static HWND       hDspPos       = 0;
  static HWND       hDspComment   = 0;
  static STATUSINFO sti;
  static int        xStatusWidth  = 10;
  static int        lDispTimeout  = 0;
  switch(ulMsg)
  {
  case WM_CREATE:
  {
    WinDefWindowProc(hWnd,ulMsg,m1,m2);
    memset(&sti,0,sizeof(sti));
    sti.lLine = -1;
    { /*>>> Determine the display window size */
      HPS hps = WinGetPS(hWnd);
      FATTRS fAttrs;
      memset(&fAttrs,0,sizeof(fAttrs));
      fAttrs.usRecordLength    = sizeof(fAttrs);
      strcpy(fAttrs.szFacename,"System VIO");
      fAttrs.lMatch            = 67;
      fAttrs.lMaxBaselineExt   = 10;
      fAttrs.lAveCharWidth     = 6;
      fAttrs.fsFontUse         = FATTR_FONTUSE_NOMIX;
      GpiCreateLogFont(hps,NULL,1,&fAttrs);
      GpiSetCharSet(hps,1);
      // Look at the font sizes :
      POINTL ptl         = {0,0};
      char   sz   []     = "Line 00000 Col 0000 (Read only) ";
      POINTL aptl [100];
      GpiQueryCharStringPosAt(hps,&ptl,0,strlen(sz),sz,0,aptl);
      xStatusWidth      = aptl[strlen(sz)-1].x + 14;
      WinReleasePS(hps);
    }
    SWP swp;
    WinQueryWindowPos(hWnd,&swp);
    hDspPos     = WinCreateWindow(hWnd, STATUSDISPCLASS, 0, WS_VISIBLE|WS_CLIPSIBLINGS, 0,            0,
                                  swp.cy, xStatusWidth,        hMDI0Client, HWND_TOP, 0,0,0);
    hDspComment = WinCreateWindow(hWnd, STATUSDISPCLASS, 0, WS_VISIBLE|WS_CLIPSIBLINGS, xStatusWidth, 0,
                                  swp.cy, swp.cx-xStatusWidth, hMDI0Client, HWND_TOP, 0,0,0);
    WinSendMsg(hDspPos,    STM_SETCOLORS,MPARAM(CLR_BLACK),  MPARAM(CLR_PALEGRAY));
    WinSendMsg(hDspComment,STM_SETCOLORS,MPARAM(CLR_DARKRED),MPARAM(CLR_PALEGRAY));
    WinStartTimer(hAB,hWnd,1,100); // EQU 0.1 sec
    break;
  }
  case WM_SIZE:
  {
    WinSetWindowPos(hDspPos,     0, 0,             0, min(SHORT1FROMMP(m2),xStatusWidth),      SHORT2FROMMP(m2), SWP_SIZE);
    WinSetWindowPos(hDspComment, 0, xStatusWidth-2,0, max(0, SHORT1FROMMP(m2)-xStatusWidth+2), SHORT2FROMMP(m2), SWP_MOVE|SWP_SIZE);
    break;
  }
  case STM_DISPLAYTEXT:
    /* m1 = pszComment, m2 = time*/
    lDispTimeout = 10*(LONG)m2;
    WinSendMsg(hDspComment,ulMsg,m1,0);
    break;
  case WM_TIMER:
  {
    if (memcmp(&sti,&statusInfo,sizeof(sti)))
    {
      sti = statusInfo;
      char sz[50] = "Line       Col      ";
      if (sti.lLine > 0 && sti.lCol > 0)
        sprintf(sz,"Line %05u Col %04u ",sti.lLine,sti.lCol);
      if      (sti.stmode == STMODE_CHANGED)  strcat(sz,"(Changed)");
      else if (sti.stmode == STMODE_READONLY) strcat(sz,"(Read only)");
      WinSendMsg(hDspPos,STM_DISPLAYTEXT,MPARAM(sz),0);
    }
    if (m1==MPARAM(1) && lDispTimeout && --lDispTimeout==0)
      WinSendMsg(hDspComment,STM_DISPLAYTEXT,MPARAM(""),0);
    break;
  }
  default: return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

MRESULT EXPENTRY wpTBBack(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  if (ulMsg == WM_SIZE && pToolbar) pToolbar->AdjustTBSize();
  return WinDefWindowProc(hWnd,ulMsg,m1,m2);
}



