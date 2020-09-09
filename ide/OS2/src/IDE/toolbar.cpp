/*
*
*  File: TOOLBAR.cpp
*
*  Multy Document Interface definition file
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/
#define  INCL_WIN
#define  INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "TOOLBAR.H"

#define MM_SUBCLASS WM_USER+0
#define TBCLASSNAME "TBClassName"
#define WID_TOOLBAR 2000

MRESULT EXPENTRY fwpTBWindow(HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY fwpButton  (HWND,ULONG,MPARAM,MPARAM);

BOOL TOOLBAR::fInitialized = FALSE;

/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  Create the class instance                                                                               */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
/*
>   PBOOL   pRet(in/out)    - success indicator will be stored
>   HAB     hAB (in)        - ancor block
>   HWND    hParent (in)    - window to become the toolbar's parent (all the hParent size will be occupied)
>   HWND    hOwner (in)     - window to become the toolbar's owner
>   ULONG   dx (in)         - x-size of the buttons
>   ULONG   dy (in)         - y-size of the buttons
>   HMODULE hmResource (in) - resource identity containg the bitmaps (or 0 to use application .exe file)
*/
TOOLBAR::TOOLBAR (PBOOL pRet, HAB hAB, HWND hParent, HWND hOwner, ULONG dx, ULONG dy, HMODULE hmResource)
{
  *pRet = FALSE;
  memset(this,0,sizeof(*this));
  TOOLBAR::hAB        = hAB;
  TOOLBAR::hParent    = hParent;
  TOOLBAR::hOwner     = hOwner;
  TOOLBAR::dx         = dx;
  TOOLBAR::dy         = dy;
  TOOLBAR::hmResource = hmResource;
  TOOLBAR::xBorder    = 3;
  TOOLBAR::yBorder    = 3;

  if (!fInitialized)
    if (!WinRegisterClass(hAB,TBCLASSNAME,(PFNWP)fwpTBWindow,CS_SIZEREDRAW|CS_CLIPCHILDREN,4L)) return;
  fInitialized = TRUE;

  hTB = WinCreateWindow(hParent, TBCLASSNAME, 0, WS_VISIBLE, 0,0,1,1, hOwner, HWND_TOP, WID_TOOLBAR, 0, 0);
  if (!hTB)
    return;
  else
  {
    WinSetWindowULong(hTB,QWL_USER,ULONG(this));
    AdjustTBSize();
    { // I don't know how to prevent painting of the focus marker on the 1st
      // created button (it appears when WinSetPresParam is called - PM bug?),
      // so I'll create this button at first
      BTNCDATA   btnCData = {sizeof(BTNCDATA),0,0,0};
      HWND       hBtnBug = WinCreateWindow(hTB,WC_BUTTON, "Bugger", BS_PUSHBUTTON | BS_NOPOINTERFOCUS,
                                     -10, -10, 1, 1, 0, HWND_TOP, 0, &btnCData, NULL);
      ULONG ul = 0xa03030;
      WinSetPresParam(hBtnBug,PP_BACKGROUNDCOLOR,4,&ul);
    }
    *pRet = TRUE;
  }
}

TOOLBAR::~TOOLBAR()
{
  PBTN pbt,qbt;
  for (pbt=pBtn; pbt;)
  {
    qbt = pbt;
    pbt = pbt->next;
    delete qbt;
  }

//  if (hTB) WinDestroyWindow(hTB);
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  Add buttons to the toolbar                                                                              */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
/*
>  PNEWBTN pNewBtn (in/out)    - Array of NEWBTN structures
>                                hWnd fields will be set for the successfully created buttons,
>                                other fields must be set
>  USHORT  usCount (in)        - Number of elements in the array (0 or more)
>  ULONG   ulInsertBefore (in) - Command value associated with the button to insert new buttons before
>                                or 0 to append them at the end
>
>  Returns: TRUE if all the buttons was successfully created
*/
BOOL TOOLBAR::InsertMultButton (PNEWBTN aNewBtn, USHORT usCount, ULONG ulInsertBefore)
{
  int        i;

  BTNCDATA   btnCData = {sizeof(BTNCDATA),0,0,0};
  PNEWBTN    pnb;
  PBTN       pbtIns   = pBtn;
  HPS        hps      = WinGetScreenPS(HWND_DESKTOP);
  BOOL       fErrs    = FALSE;
  HWND       hBtn;

  while (pbtIns && pbtIns->next && (!ulInsertBefore || pbtIns->next->ulCmd!=ulInsertBefore))
    pbtIns = pbtIns->next;
  // now: pbtIns = 0 (no list) OR new buttons must be inserted after *pbtIns PBT structure

  for (i=0; i<usCount; i++)
  {
    pnb         = &aNewBtn[i];
    pnb->hWnd   = 0;
    hBtn        = 0;
    if (pnb->ulBmp)
    {
      if
      (
        !(btnCData.hImage = GpiLoadBitmap(hps, hmResource, pnb->ulBmp, 0, 0))
      ||
        !(hBtn            = WinCreateWindow(hTB,WC_BUTTON, "Button",
                                            BS_PUSHBUTTON | BS_BITMAP | BS_NOPOINTERFOCUS | WS_VISIBLE | WS_DISABLED,
                                            0-dx, 0, dx, dy, pnb->hOwner, HWND_TOP,
                                            pnb->ulCmd, &btnCData, NULL))
      )
      {
        fErrs = TRUE;
        continue;
      }
      pnb->hWnd = hBtn;
      if (pnb->rgbBack != 0x00000001) WinSetPresParam(hBtn,PP_BACKGROUNDCOLOR,4,&pnb->rgbBack);
      WinEnableWindow(hBtn,pnb->fEnabled);
      fwpButton(hBtn,MM_SUBCLASS,MPARAM(ULONG(WinSubclassWindow(hBtn,(PFNWP)fwpButton))),0L);
    }

    { // Insert it into the list
      PBTN pbt  = new BTN;
      int len   = pnb->pszComment ? strlen(pnb->pszComment) : 0;
      pbt->hWnd     = pnb->hWnd;
      pbt->ulCmd    = pnb->ulCmd;
      pbt->ulBmp    = pnb->ulBmp;
      pbt->x  = 0-dx;
      pbt->y  = 0;
      pbt->cx = pbt->hWnd ? dx : dx/2; // Button or
      pbt->cy = pbt->hWnd ? dy : dy/2; //   separator sizes
      if (len)
      {
        pbt->pszComment = (PSZ)malloc(len+1);
        strcpy(pbt->pszComment,pnb->pszComment);
      }
      if (pbtIns)
      {
        pbt ->next   = pbtIns->next;
        pbtIns->next = pbt;
      }
      else
        pBtn = pbt;
      pbtIns = pbt;
      if (hBtn) WinSetWindowULong(hBtn,QWL_USER,ULONG(pbt));
    }
  }
  WinReleasePS(hps);
  AlignAll();
  return !fErrs;
}



/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  Remove some buttons from the toolbar                                                                    */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
/*
>  PULONG aulCmd (in) - array of the command values associated with the buttons or the separators to be removed
>  USHORT usCount(in) - number of elements in the array
>
>  NOTE: button CAN'T be removed by destroying it's window
>        (using the handle returned by AddButton call)
*/
void TOOLBAR::RemoveMultButton (PULONG aulCmd, USHORT usCount)
{
  BOOL     fDel = FALSE;
  PBTN    *ppbt = &pBtn;
  PBTN     pbt;
  int      idx;
  BOOL     fDelMe;
  while(*ppbt)
  {
    idx    = usCount;
    fDelMe = FALSE;
    pbt    = *ppbt;
    while(idx-- > 0)
      if(pbt->ulCmd == aulCmd[idx])
      {
        fDelMe = fDel = TRUE;
        break;
      }
    if (fDelMe)
    {
      *ppbt = pbt->next;
      if (pbt->hWnd) WinDestroyWindow(pbt->hWnd);
      delete pbt;
    }
    else
      ppbt = &((*ppbt)->next);
  }
  if (fDel) AlignAll();
}


/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  Enable button                                                                                           */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
//
void TOOLBAR::EnableButton(ULONG ulCmd, BOOL fEnable)
{
  for (PBTN p = pBtn; p; p=p->next)
    if (p->ulCmd==ulCmd)
      WinEnableWindow(p->hWnd,fEnable);
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  Set new comment to button                                                                               */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
//
void TOOLBAR::SetButtonComment(ULONG ulCmd, PSZ pszComment)
{
  for (PBTN p = pBtn; p; p=p->next)
    if (p->ulCmd==ulCmd)
    {
      free(p->pszComment);
      PSZ psz = pszComment ? pszComment : "";
      p->pszComment = (PSZ)malloc(strlen(psz)+1);
      strcpy(p->pszComment,psz);
    }
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  Adjust toolbar window size                                                                              */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
//
void TOOLBAR::AdjustTBSize()
{
  SWP swp;
  WinQueryWindowPos(hParent,&swp);
  WinSetWindowPos(hTB,0,0,0,swp.cx,swp.cy,SWP_MOVE|SWP_SIZE);
  AlignAll();
}



/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  Query the optimal toolbar window sizes to place all the controls                                        */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
/*
>  PRECTL prcl (in/out) - rectangle with the required sizes (xLeft and yBottom will be 0)
*/
void TOOLBAR::QueryRequiredRect (PRECTL prcl)
{
  prcl->xLeft  = prcl->yBottom = 0;
  prcl->xRight = xRequired;
  prcl->yTop   = yRequired;
}


/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  Toolbar window procedure                                                                                */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
//
MRESULT EXPENTRY fwpTBWindow (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  PTOOLBAR ptb = (PTOOLBAR)WinQueryWindowULong(hWnd,QWL_USER);
  switch (ulMsg){
  case WM_SIZE:
    WinInvalidateRect(hWnd,0,0);
    break;
  case WM_PAINT:
  {
    RECTL rclInv,rclWin;
    HPS   hps = WinBeginPaint(hWnd,0,&rclInv);
    WinQueryWindowRect(hWnd,&rclWin);
    if (ptb)
    {
      POINTL ptl;
      SWP    swp;
      WinQueryWindowPos (hWnd,&swp);
      LONG cx = swp.cx-1;
      LONG cy = swp.cy-1;
      WinFillRect(hps, &rclInv, CLR_WHITE);
      ptl.x = 0; ptl.y = 0;
      GpiSetCurrentPosition(hps,&ptl);
      GpiSetColor(hps,CLR_PALEGRAY);
      ptl.y += cy;      GpiLine(hps,&ptl);
      ptl.x += (cx-1);  GpiLine(hps,&ptl);
      ptl.y -= (cy-1);  GpiLine(hps,&ptl);
      ptl.x -= (cx-3);  GpiLine(hps,&ptl);
      ptl.x = cx; ptl.y = cy;
      GpiSetCurrentPosition(hps,&ptl);
      GpiSetColor(hps,CLR_DARKGRAY);
      ptl.y -= cy;      GpiLine(hps,&ptl);
      ptl.x -= (cx-1);  GpiLine(hps,&ptl);
      ptl.y += (cy-1);  GpiLine(hps,&ptl);
      ptl.x += (cx-3);  GpiLine(hps,&ptl);
    }
    WinEndPaint(hps);
    break;
  }
  case WM_DESTROY:
    if (ptb) ptb->hTB = 0;
    break;
  default: return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  Button window procedure                                                                                 */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/
//
MRESULT EXPENTRY fwpButton (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static   PFNWP   pWindowProc;
  static   HWND    hCommentButton = 0;
  TOOLBAR::PBTN    pbt            = (TOOLBAR::PBTN)WinQueryWindowULong(hWnd,QWL_USER);

  switch (ulMsg){
  case MM_SUBCLASS:
    pWindowProc = PFNWP(ULONG(m1));
    return 0;
  case WM_MOUSEMOVE:
  {
    HWND hOwn = WinQueryWindow(hWnd,QW_OWNER);
    if (hOwn && hCommentButton!=hWnd && SHORT1FROMMP(m2)==HT_NORMAL && pbt && pbt->pszComment)
    {
      if (hCommentButton)
        WinSendMsg(hOwn,WM_CONTROL,MPFROM2SHORT(pbt->ulCmd,TLBN_COMMENT),0);
      hCommentButton = hWnd;
      WinSendMsg(hOwn,WM_CONTROL,MPFROM2SHORT(pbt->ulCmd,TLBN_COMMENT),pbt->pszComment);
      WinStartTimer(WinQueryAnchorBlock(hWnd),hWnd,13,100);
    }
    break;
  }
  case WM_TIMER:
  {
    if (m1 != MPARAM(13)) break;
    HWND hOwn = WinQueryWindow(hWnd,QW_OWNER);
    if (hOwn && hCommentButton == hWnd)
    {
      POINTL ptl;
      WinQueryPointerPos(HWND_DESKTOP,&ptl);
      if (hWnd == WinWindowFromPoint(HWND_DESKTOP,&ptl,TRUE))
        return 0;
      else
      {
        hCommentButton = 0;
        if (hOwn) WinSendMsg(hOwn,WM_CONTROL,MPFROM2SHORT(pbt->ulCmd,TLBN_COMMENT),0);
      }
    }
    WinStopTimer(WinQueryAnchorBlock(hWnd),hWnd,1);
    return 0;
  }
  case BM_SETCHECK:
  case BM_SETDEFAULT:
    return 0;
  }
  return ((MRESULT)(*pWindowProc)(hWnd,ulMsg,m1,m2));
}



/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                                          */
/*  M i x                                                                                                   */
/*                                                                                                          */
/*----------------------------------------------------------------------------------------------------------*/

TOOLBAR::BTN::BTN()
{
  memset(this,0,sizeof(*this));
}
TOOLBAR::BTN::~BTN()
{
  free(pszComment);
  pszComment = 0;
}

void TOOLBAR::AlignAll()
// Alignes all the controls by the method depends on tbAlignment mode
// and sets xRequired & yRequired values - minimal window sizes needs to
// make all the controls inside (for the TBA_FILL mode this sizes
// depends on the current hTB window width)
{
  SWP      swp;
  WinQueryWindowPos(hTB,&swp);
  swp.cx = max(swp.cx-xBorder*2,0);
  PBTN  pbt       = pBtn;
  LONG  yLineTop  = swp.cy-1-yBorder;
  xRequired       = 0;
  yRequired       = yBorder*2;
  while(pbt) // Align line ptb...
  {
    if (tbAlignment==TBA_CENTER)
    {
      pbt->x = (swp.cx-pbt->cx)/2+2;
      pbt->y = (swp.cy-pbt->cy)/2;
      if (pbt->hWnd)
        WinSetWindowPos(pbt->hWnd,0, pbt->x, pbt->y, 0,0,SWP_MOVE|SWP_NOREDRAW);
      pbt = pbt->next;
    }
    else
    {
      int   nLineLen    = 1;
      LONG  xLineWidth  = pbt->cx;
      LONG  yLineHeight = pbt->cy;
      PBTN  qbt         = pbt->next; // next line 1st elem.
      while(qbt && (tbAlignment!=TBA_VERTICAL &&
                    (tbAlignment==TBA_HORIZONTAL || xLineWidth+qbt->cx <= swp.cx))
           )
      {
        yLineHeight  = max(yLineHeight,qbt->cy);
        xLineWidth  += qbt->cx;
        qbt          = qbt->next;
        nLineLen++;
      }
      yRequired  += yLineHeight;
      xRequired  = max(xRequired, xLineWidth+2*xBorder);
      xLineWidth = xBorder;
      while(nLineLen--)
      {
        pbt->x = xLineWidth;
        pbt->y = yLineTop - pbt->cy + (yLineHeight - pbt->cy)/2 + 1;
        if (pbt->hWnd)
          WinSetWindowPos(pbt->hWnd,0, pbt->x, pbt->y, 0,0,SWP_MOVE|SWP_NOREDRAW);
        xLineWidth += pbt->cx;
        pbt = pbt->next;
      }
      yLineTop  -= yLineHeight;
    }
  }
  WinInvalidateRect(hTB,0,TRUE);
}


