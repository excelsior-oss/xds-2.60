/*
*
*  File: FONTCLR.CPP
*
*  XDS Shell: font & color selection dialog
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
#include <math.h>
#include "XDSBASE.H"

/*---- Call the module startup code ----*/
class INITCLASS
{
  public: INITCLASS();
};
INITCLASS initclass;
/*--------------------------------------*/

struct ASSOPTDLGSTR  // Structure passed to ASSOPTDLG window
{
  ULONG   cb;
  PASSTR  pEdAss;      // In:  Для редактора - ASSTR, для списков ассоциаций - NULL
  PASSTR  pGAssList;   // I/O: Для НЕ редактора - копии списков ассоциаций для
  PASSTR  pLAssList;   // O/O:   редактирования. По 'Ok' ими заменятся старые списки.
  BOOL    fClrToAll;   // Out: Для редактора - применить цвета ко всем окнам
  BOOL    fFntToAll;   // Out: Для редактора - применить фонт ко всем окнам
};

//--- Globals:
//
#define          MAXDEFCOLORS BMCLRTOTAL
#define          RAINBOWLEN   64
CLP_PALETTETYPE  aClrSet    [MAXDEFCOLORS];
CLP_PALETTETYPE  aClrSetIni [MAXDEFCOLORS];
ULONG            aRainbow   [RAINBOWLEN];
ULONG            aRainbowIni[RAINBOWLEN];

//--- Function prototypes:
//
MRESULT EXPENTRY dpAssOptDlg (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2);
MRESULT EXPENTRY dpEditClrDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2);
MRESULT EXPENTRY dpMakeClrDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2);
MRESULT EXPENTRY dpEdAssList (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2);


/*================ Profile the module internal settings ===========*/
//
struct PRF_STR
{
  CLP_PALETTETYPE aClrSet [MAXDEFCOLORS];
  ULONG           aRainbow[RAINBOWLEN];
};
//
void FontClrRProfile(LONG lSlot, PSZ szItem)
{
  PRF_STR prfs;
  if (sizeof(prfs) != PfReadItem(lSlot,szItem,&prfs,sizeof(prfs),0,0))
  {
    memcpy(aClrSet, aClrSetIni, sizeof(aClrSet));
    memcpy(aRainbow,aRainbowIni,sizeof(aRainbow));
  }
  else
  {
    memcpy(aClrSet, prfs.aClrSet, sizeof(aClrSet));
    memcpy(aRainbow,prfs.aRainbow,sizeof(aRainbow));
  }
}
//
void FontClrWProfile(LONG lSlot, PSZ szItem)
{
  PRF_STR prfs;
  memcpy(prfs.aClrSet, aClrSet, sizeof(aClrSet));
  memcpy(prfs.aRainbow,aRainbow,sizeof(aRainbow));
  PfWriteItem(lSlot,szItem,&prfs,sizeof(prfs));
}


/*================ Module startup code ===========*/
//
#define RGB(r,g,b) ((r&0xff)<<16 | (g&0xff)<<8 | b&0xff)
ULONG rnd(ULONG ul)
{
  static float m = 3.7346252;
  m = (3.14159265*m*m + 2.71828183*m + 0.8362514384) * 1000;
  m -= int(m);
  return ULONG(float(ul)*m);
}
ULONG inicolor(ULONG rgb, BOOL fContrast)
{
  ULONG r,g,b,r1,g1,b1;
  ULONG d = fContrast ? 255 : 32;
  r = (rgb&0xff0000)>>16;
  g = (rgb&0xff00)>>8;
  b = rgb&0xff;
  attempt:
  r1 = r ^ rnd(d);
  g1 = g ^ rnd(d);
  b1 = b ^ rnd(d);
  if      (fContrast  && (abs(r-r1)<100 || abs(g-g1)<100 || abs(b-b1)<100)) goto attempt;
  else if (!fContrast &&  abs(r-r1)<10  && abs(g-g1)<10  && abs(b-b1)<10)   goto attempt;
  return RGB(r1,g1,b1);
}
void ini1(ULONG rgbF, ULONG rgbB, CLP_PALETTETYPE pplt)
{
  for (int i=0; i<CLP_PALETTELEN; i++)
  {
    pplt[i][CLP_FOREGROUND] = inicolor(rgbB,TRUE);
    pplt[i][CLP_BACKGROUND] = rgbB;
  }
  pplt[CLP_CURLINE]  [CLP_BACKGROUND] = inicolor(rgbB,FALSE);
  pplt[CLP_SELECTION][CLP_BACKGROUND] = rgbF;
  pplt[CLP_SELECTION][CLP_FOREGROUND] = rgbB;
  pplt[CLP_DEFAULT]  [CLP_BACKGROUND] = rgbB;
  pplt[CLP_DEFAULT]  [CLP_FOREGROUND] = rgbF;
}
INITCLASS::INITCLASS()
{
  int i;
  #if !(MAXDEFCOLORS-8)
    ULONG fgbg[8][2] =  // <---
    {
      {RGB(000,000,000), RGB(247,247,247)},   {RGB(000,000,164), RGB(247,235,207)},
      {RGB(000,226,000), RGB(000,000,000)},   {RGB(195,195,195), RGB(000,000,000)},
      {RGB(255,255,255), RGB(000,000,200)},   {RGB(255,255,255), RGB(174,000,000)},
      {RGB(255,255,255), RGB(128,128,128)},   {RGB(000,000,128), RGB(255,255,255)}
    };
  #elif
    #error >>>> Initialisation of 'fgbg' failed
  #endif
  for (i=0; i < sizeof(fgbg)/sizeof(fgbg[0]); i++)
    ini1(fgbg[i][0], fgbg[i][1], aClrSetIni[i]);
  memcpy(aClrSet,aClrSetIni,sizeof(aClrSet));
  //----- Init the rainbow
  {
    for(i=0; i<RAINBOWLEN; i++) aRainbowIni[i] = 0x00ffffff;
    HPS hps = WinGetPS(HWND_DESKTOP);
      for (i=0; i<16; i++) aRainbowIni[i] = GpiQueryRGBColor(hps,0,i);
    WinReleasePS(hps);
    memcpy(aRainbow,aRainbowIni,sizeof(aRainbow));
  }
}


/*===========================================================*/
PASSTR dup_asslist(ASSTR *pList)
{
  PASSTR pN,pL=0,pRet=0;
  for (; pList; pList=pList->next)
  {
    if (!(pN = new ASSTR)) continue;
    pN->Assign(pList);
    if (pL) pL->next = pN;
    else    pRet     = pN;
    pL               = pN;
  }
  return pRet;
}

void AssOptDlg(BOOL fCurEdit)
{
  ASSOPTDLGSTR aoStr;
  EDCONF       ec;
  HWND         hEd  = 0;
  PFILEED      pFE  = 0;
  memset(&aoStr,0,sizeof(aoStr));
  aoStr.cb          = sizeof(aoStr);

  if (fCurEdit)
  {
    PASSTR pAss = 0;
    pFE=grOffice.QueryActiveFEd();
    if (!pFE)
    {
      WinMessageBox(HWND_DESKTOP,hMDI0Client,"Can't set editor options because no active editor exists.",
                    "Options",0,MB_ERROR|MB_OK|MB_MOVEABLE);
      return;
    }
    if (!(pAss = new ASSTR)) return;
    pAss->SetFiles("<Current editor>");
    // Fill pAss in accordance with pFE settings
    hEd = pFE->hWnd;
    memset(&ec,0,sizeof(ec));
    WinSendMsg(hEd,TM_QUERYEDCONF,MPARAM(&ec),0);
    memcpy(&pAss->aaRGB,     &ec.edPalette, sizeof(pAss->aaRGB));
    memcpy(&pAss->fAttrs,    &ec.fAttrs,    sizeof(FATTRS));
    strcpy(pAss->szPainterId, ec.szPainterId);
    pAss->fHilite        = ec.fHilite;
    pAss->fHiliteCurLine = ec.fHiliteCurLine;
    pAss->ulTabWidth     = ec.lTabSize;
    pAss->fSmartTabs     = ec.fSmartTab;
    pAss->fAutoident     = ec.fAutoidentMode;
    pAss->fPasteRepSel   = TRUE;
    pAss->fUseTabChars   = ec.fTabCharsMode;
    aoStr.pEdAss = pAss;
  }
  else
  {
    aoStr.pGAssList = dup_asslist(ptOffice.QueryAssList(TRUE));
    aoStr.pLAssList = dup_asslist(ptOffice.QueryAssList(FALSE));
  }


  if (WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpAssOptDlg,NULLHANDLE,IDD_ASSOPTDLG,&aoStr))
  {
    if (fCurEdit) // Apply changes
    {
      PASSTR pAss = aoStr.pEdAss;
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
      if (aoStr.fFntToAll || aoStr.fClrToAll)
      {
        for (HWND hEd0 = grOffice.EnumEd(); hEd0; hEd0=grOffice.NextEnumEd())
          if (hEd0!=hEd)
          {
            WinSendMsg(hEd0,TM_QUERYEDCONF,MPARAM(&ec),0);
            if (aoStr.fFntToAll)
              memcpy(&ec.fAttrs, &pAss->fAttrs,   sizeof(FATTRS));
            if (aoStr.fClrToAll)
            {
              memcpy(&ec.edPalette, &pAss->aaRGB, sizeof(ec.edPalette));
              ec.fHilite        = pAss->fHilite;
              ec.fHiliteCurLine = pAss->fHiliteCurLine;
            }
            WinSendMsg(hEd0,TM_SETEDCONF,MPARAM(&ec),0);
          }
      }
    }
    else // New associations
    {
      BOOL fApp = (grOffice.EnumEd() && MBID_YES==WinMessageBox(HWND_DESKTOP,hMDI0Client,
                                        "Apply the associated settings to all the opeded windows?",
                                        "Associations",0,MB_ICONQUESTION|MB_YESNO|MB_MOVEABLE));
      ptOffice.SetNewAssList(FALSE, aoStr.pLAssList, FALSE);
      ptOffice.SetNewAssList(TRUE,  aoStr.pGAssList, fApp);
      aoStr.pGAssList = aoStr.pLAssList = NULL;
    }
  }
  { // Free the associations list(s)
    PASSTR p,p1;
    for (p=aoStr.pGAssList; p; p=p1){ p1 = p->next; delete p; }
    for (p=aoStr.pLAssList; p; p=p1){ p1 = p->next; delete p; }
    delete aoStr.pEdAss;
  }
}


MRESULT EXPENTRY wp_fontshow(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  static PFNWP  pWindowProc;
  static FATTRS fAttrs;

  switch (ulMsg){
  case MM_START:
  {
    pWindowProc      = PFNWP(ULONG(m1));
    return 0;
  }
  case WM_USER: // m1==&fAttrs
  {
    fAttrs = *(PFATTRS)m1;
    WinInvalidateRect(hWnd,0,0);
    return 0;
  }
  case WM_PAINT:
  {
    HPS hps = WinBeginPaint(hWnd,0,0);
    RECTL rcl;
    GpiCreateLogFont(hps,NULL,001L,&fAttrs);
    GpiSetCharSet   (hps,001L);
    WinQueryWindowRect(hWnd,&rcl);
    WinDrawText(hps,-1,fAttrs.szFacename,&rcl,CLR_BLACK,CLR_PALEGRAY,DT_CENTER|DT_VCENTER|DT_ERASERECT);
    WinEndPaint(hps);
    return 0;
  }
  }
  return (*pWindowProc)(hWnd,ulMsg,m1,m2);
}

MRESULT EXPENTRY wp_bmclr(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  static PFNWP  pWindowProc;
  static HWND   hBM       = 0;

  switch (ulMsg){
  case MM_START:
  {
    pWindowProc = PFNWP(ULONG(m1));
    return 0;
  }
  case WM_PAINT:
  {

    if (!hBM)
    {
      HPS hps = WinGetPS(HWND_DESKTOP);
      hBM     = GpiLoadBitmap(hps, NULLHANDLE, BMP_COLORS,0,0);
    }
    static ULONG aulEmpty[2] = {0x00808080,0x00a0a0a0};
    RECTL        rcl;
    POINTL       ptl         = {0,0};
    HPS          hps         = WinBeginPaint(hWnd,0,0);
    PULONG       pul         = (PULONG)WinQueryWindowULong(hWnd,QWL_USER);
    if (!pul)    pul         = aulEmpty;
    WinQueryWindowRect(hWnd,&rcl);
    GpiCreateLogColorTable(hps,LCOL_PURECOLOR,LCOLF_RGB,0,0,0);
    WinFillRect(hps,&rcl,pul[CLP_BACKGROUND]&CLP_COLORMASK);
    WinDrawBitmap(hps, hBM, 0, &ptl, pul[CLP_FOREGROUND]&CLP_COLORMASK, pul[CLP_BACKGROUND]&CLP_COLORMASK, DBM_NORMAL);
    GpiSetColor(hps,0x808080);
    GpiSetCurrentPosition(hps,&ptl);
    ptl.y = rcl.yTop;   GpiLine(hps,&ptl);
    ptl.x = rcl.xRight; GpiLine(hps,&ptl);
    ptl.y = 0;          GpiLine(hps,&ptl);
    ptl.x = 0;          GpiLine(hps,&ptl);
    WinEndPaint(hps);
    return 0;
  }
  case WM_BUTTON1DOWN:
    WinPostMsg(WinQueryWindow(hWnd,QW_PARENT),WM_COMMAND,
               MPARAM(WinQueryWindowUShort(hWnd,QWS_ID)),MPARAM(FALSE));
    break;
  case WM_BUTTON1DBLCLK:
    WinPostMsg(WinQueryWindow(hWnd,QW_PARENT),WM_COMMAND,
               MPARAM(WinQueryWindowUShort(hWnd,QWS_ID)),MPARAM(TRUE));
    break;
  }
  return (*pWindowProc)(hWnd,ulMsg,m1,m2);
}



////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//  Редактирование опций / ассоциаций                                             //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

// Глобальные цвета и painter для общения
// с редактором цветов (dblclick on a bitmap)
//
CLP_PALETTETYPE aaEdRGB;
PPAINTER        pClrPainter = 0;

MRESULT EXPENTRY dpAssOptDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  static ASSOPTDLGSTR *paoStr;
  static HWND          hBMClrSel;
  static HWND          hCBAssWith;
  static HWND          hCBHltMode;
  static HWND          hSTFontShow;
  static BOOL          fGlobal = TRUE;
  static PASSTR        pAss;      // Текущий pAss или 0

  switch(ulMsg){
  case WM_INITDLG:
  {
    hSTFontShow    = WinWindowFromID(hWnd,IDD_STFONTSHOW);
    hCBAssWith     = WinWindowFromID(hWnd,IDD_CBASSWITH);
    hCBHltMode     = WinWindowFromID(hWnd,IDD_CBHILITEMODE);
    paoStr         = (ASSOPTDLGSTR*)m2;
    pAss           = 0;
    WinSendDlgItemMsg(hWnd,IDD_SPBTABWIDTH,SPBM_SETLIMITS,MPARAM(24),MPARAM(1));
    wp_fontshow(hSTFontShow, MM_START, MPARAM(ULONG(WinSubclassWindow(hSTFontShow,(PFNWP)wp_fontshow))),0L);
    { // Subclass the bitmaps
      HWND h = WinWindowFromID(hWnd,IDD_BMCLRSEL);
      wp_bmclr         (h, MM_START,MPARAM(ULONG(WinSubclassWindow(h,(PFNWP)wp_bmclr))),0L);
      for (int n=0; n<BMCLRTOTAL; n++)
      {
        h = WinWindowFromID(hWnd,n+IDD_BMCLR0);
        wp_bmclr (h, MM_START, MPARAM(ULONG(WinSubclassWindow(h,(PFNWP)wp_bmclr))),0L);
        WinSetWindowULong(h, QWL_USER, ULONG(aClrSet[n][CLP_DEFAULT]));
      }
    }
    //--- Fill hilite modes list:
    for (PPAINTER ppt = ptOffice.GetPainterList(); ppt; ppt = ppt->next)
    {
      int nIt = (int)WinSendMsg(hCBHltMode,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),
                                                     MPARAM(ppt->pszDescription));
      WinSendMsg(hCBHltMode,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(ppt));
    }
    WinSendMsg (hCBHltMode,LM_INSERTITEM,MPARAM(0),MPARAM("<Text>"));
    if (paoStr->pEdAss)
    {
      WinSetWindowText(hWnd,"Editor options");
      WinDestroyWindow(WinWindowFromID(hWnd,IDD_PBEDASSLIST));
      WinDestroyWindow(WinWindowFromID(hWnd,IDD_RBGLOBASS));
      WinDestroyWindow(WinWindowFromID(hWnd,IDD_RBLOCASS));
      WinSendMsg(hCBAssWith,LM_INSERTITEM,MPARAM(0),"<Current editor>");
      WinSendMsg(hCBAssWith,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
      WinEnableControl(hWnd,IDD_CBASSWITH,FALSE);
      WinSetActiveWindow(HWND_DESKTOP,hCBHltMode);
      // Сделаем из кнопки - checkbox 'Apply to all'
      HWND h      = WinWindowFromID(hWnd,IDD_PBCHFONT2ALL);
      WinSetWindowULong(h,QWL_STYLE,WinQueryWindowULong(h,QWL_STYLE) & ~BS_PUSHBUTTON | BS_AUTOCHECKBOX);
      WinSetWindowText (h,"Apply to all");
    }
    else
    {
      WinSetWindowText(hWnd,"Associations");
      WinDestroyWindow  (WinWindowFromID(hWnd,IDD_CBCLRTOALL));
      WinSendDlgItemMsg (hWnd,fGlobal ? IDD_RBGLOBASS : IDD_RBLOCASS, BM_CLICK,0,0);
      // Тут^ пришел WM_CONTROL и заполнился IDD_CBASSWITH list, в нем
      // выбрался элемент и по очередному WM_CONTROL установились цвета etc.
      WinSetActiveWindow(HWND_DESKTOP,hCBAssWith);
    }
    return MRESULT(TRUE); // Focus is turned Ok.
  }
  case WM_CONTROL:
  {
    if (m1==MPFROM2SHORT(IDD_RBGLOBASS,BN_CLICKED) || m1==MPFROM2SHORT(IDD_RBLOCASS,BN_CLICKED))
    { // ********
      // *******  RADIOBUTTON :  установить глобальный или локальный список ассоциаций
      // ********
      fGlobal = WinQueryButtonCheckstate(hWnd, IDD_RBGLOBASS);
      WinSendMsg(hWnd,WM_USER+1,0,0); // Сохраняем установки
      WinSendMsg(hCBAssWith,LM_DELETEALL,0,0);
      WinSetWindowText(hCBAssWith,"");
      for(PASSTR p = fGlobal ? paoStr->pGAssList : paoStr->pLAssList; p; p=p->next)
      {
        int nIt = (int)WinSendMsg(hCBAssWith,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(p->QueryFiles()));
        WinSendMsg(hCBAssWith,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(p));
      }
      BOOL fSel = (BOOL)WinSendMsg(hCBAssWith,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
      // Если нет ассоциации - задизэйблить все подряд
      WinEnableControl(hWnd, IDD_CBHILITEMODE, fSel);
      WinEnableControl(hWnd, IDD_SPBTABWIDTH,  fSel);
      WinEnableControl(hWnd, IDD_SPBTABWIDTH,  fSel);
      WinEnableControl(hWnd, IDD_CBSMARTTABS,  fSel);
      WinEnableControl(hWnd, IDD_CBAUTOIDENT,  fSel);
      WinEnableControl(hWnd, IDD_CBPASTEREPSEL,fSel);
      WinEnableControl(hWnd, IDD_CBUSETABCHARS,fSel);
      WinEnableControl(hWnd, IDD_CBHLTLINE,    fSel);
      WinEnableControl(hWnd, IDD_CBHLTSYNTAX,  fSel);
      WinEnableControl(hWnd, IDD_PBCHFONT,     fSel);
      WinEnableControl(hWnd, IDD_BMCLRSEL,     fSel);
      break;
    }
    if (m1==MPFROM2SHORT(IDD_CBASSWITH,CBN_LBSELECT))
    {
      WinSendMsg(hWnd,WM_USER+1,0,0); // Сохраняем установки
      {
        int nIt    = (int)WinSendMsg(hCBAssWith,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
        if (!(pAss = (PASSTR)WinSendMsg(hCBAssWith,LM_QUERYITEMHANDLE,MPARAM(nIt),0)))
          pAss = paoStr->pEdAss;
        if (!pAss) break; //Bug
      }
      //--- Set new controls state: ---//
      WinCheckButton(hWnd,IDD_CBSMARTTABS,  pAss->fSmartTabs);
      WinCheckButton(hWnd,IDD_CBAUTOIDENT,  pAss->fAutoident);
      WinCheckButton(hWnd,IDD_CBPASTEREPSEL,pAss->fPasteRepSel);
      WinCheckButton(hWnd,IDD_CBUSETABCHARS,pAss->fUseTabChars);
      WinCheckButton(hWnd,IDD_CBHLTSYNTAX,  pAss->fHilite);
      WinCheckButton(hWnd,IDD_CBHLTLINE,    pAss->fHiliteCurLine);
      WinSendDlgItemMsg(hWnd, IDD_SPBTABWIDTH, SPBM_SETCURRENTVALUE, MPARAM(pAss->ulTabWidth), 0);
      WinSendMsg       (hSTFontShow,           WM_USER,              MPARAM(&pAss->fAttrs),    0);
      {
        //--- Color show bitmap:
        HWND h = WinWindowFromID(hWnd,IDD_BMCLRSEL);
        WinSetWindowULong(h, QWL_USER, ULONG(pAss->aaRGB[CLP_DEFAULT]));
        WinInvalidateRect(h,0,0);
      }
      {
        //--- Show hilite mode:
        int nSel    = 0; // 'Text mode'
        for (int nIt=(int)WinSendMsg(hCBHltMode,LM_QUERYITEMCOUNT,0,0)-1; nIt>0; nIt--)
        {
          PPAINTER ppt = (PPAINTER)WinSendMsg(hCBHltMode,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
          if (ppt && !strcmp(ppt->szNameId,pAss->szPainterId))
          {
            nSel=nIt;
            break;
          }
        }
        WinSendMsg(hCBHltMode,LM_SELECTITEM,MPARAM(nSel),MPARAM(TRUE));
      }
    }
    break;
  }
  case WM_USER:  // Заинвалидить цветовые битмапы
  {
    WinInvalidateRect(WinWindowFromID(hWnd,IDD_BMCLRSEL),0,0);
    if (m1)
    {
      for (int n=0; n<BMCLRTOTAL; n++)
        WinInvalidateRect(WinWindowFromID(hWnd,n+IDD_BMCLR0),0,0);
    }
    break;
  }
  case WM_USER+1: // Сохранить установки
  {
    if (!pAss) break;
    pAss->fSmartTabs     = WinQueryButtonCheckstate(hWnd,IDD_CBSMARTTABS);
    pAss->fAutoident     = WinQueryButtonCheckstate(hWnd,IDD_CBAUTOIDENT);
    pAss->fPasteRepSel   = WinQueryButtonCheckstate(hWnd,IDD_CBPASTEREPSEL);
    pAss->fUseTabChars   = WinQueryButtonCheckstate(hWnd,IDD_CBUSETABCHARS);
    pAss->fHilite        = WinQueryButtonCheckstate(hWnd,IDD_CBHLTSYNTAX);
    pAss->fHiliteCurLine = WinQueryButtonCheckstate(hWnd,IDD_CBHLTLINE);
    { // Hilite mode cbox:
      int       nIt = (int)     WinSendMsg(hCBHltMode,LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
      PPAINTER  pp  = (PPAINTER)WinSendMsg(hCBHltMode,LM_QUERYITEMHANDLE,MPARAM(nIt),      0);
      if (pp) strcpy(pAss->szPainterId,pp->szNameId);
      else    pAss->szPainterId[0] = '\0';
    }
    WinSendDlgItemMsg(hWnd,IDD_SPBTABWIDTH,SPBM_QUERYVALUE,MPARAM(&pAss->ulTabWidth),
                      MPFROM2SHORT(0,SPBQ_UPDATEIFVALID));
    break;
  }
  case WM_CLOSE:
    m1 = MPFROM2SHORT(DID_CANCEL,0);
  case WM_COMMAND:
  {
    SHORT s1m1    = SHORT1FROMMP(m1);
    BOOL  fBmpSel = s1m1==IDD_BMCLRSEL;
    if (pAss && (s1m1>=IDD_BMCLR0 && s1m1<IDD_BMCLR0+BMCLRTOTAL || fBmpSel))
    {
      if (!m2) // Set color
      {
        if (!fBmpSel)
        {
          memcpy(pAss->aaRGB,aClrSet[s1m1-IDD_BMCLR0],sizeof(pAss->aaRGB));
          WinSendMsg(hWnd,WM_USER,MPARAM(FALSE),0);
        }
      }
      else     // Edit color (fBmpSel - the selected colors)
      {
        // Use the global aaEdRGB & pClrPainter:
        {
          if (fBmpSel) memcpy(aaEdRGB, pAss->aaRGB,              sizeof(aaEdRGB));
          else         memcpy(aaEdRGB, aClrSet[s1m1-IDD_BMCLR0], sizeof(aaEdRGB));
          int       nIt = (int)WinSendMsg(hCBHltMode,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
          PPAINTER  pp  = (PPAINTER)WinSendMsg(hCBHltMode,LM_QUERYITEMHANDLE,MPARAM(nIt),0);
          pClrPainter   = pp;
        }

        if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEditClrDlg,NULLHANDLE,IDD_EDITCLRDLG,0))
        {
          if (!fBmpSel) memcpy(aClrSet[s1m1-IDD_BMCLR0], aaEdRGB, sizeof(aaEdRGB));
          memcpy              (pAss->aaRGB,              aaEdRGB, sizeof(aaEdRGB));
          WinSendMsg(hWnd,WM_USER,MPARAM(TRUE),0);
        }
      }
    }
    else switch(s1m1)
    {
    case IDD_PBEDASSLIST:
    {
      if (paoStr->pEdAss) break;
      WinSendMsg(hWnd,WM_USER+1,0,0);   // Сохранить установки
      if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEdAssList,NULLHANDLE,IDD_EDASSLISTDLG,paoStr))
        // Перезаполним лист ассоциаций:
        WinSendMsg(hWnd,WM_CONTROL, MPFROM2SHORT(IDD_RBGLOBASS,BN_CLICKED),0);
      break;
    }
    case IDD_PBCHFONT2ALL:
      if (!pAss || MBID_YES!=MessageBox(hWnd,"Edit associations",MB_ICONQUESTION|MB_YESNO,
          "Do you want to change font for all the associations in the list?"))
        break;
    case IDD_PBCHFONT:
    {
      if (!pAss) break;
      FONTDLG      fontDlg;
      HPS          hps = WinGetScreenPS(hWnd);
      CHAR         szFamily[CCHMAXPATH] = "";
      FONTMETRICS  fontMetrics;

      GpiQueryFontMetrics  (hps,sizeof(FONTMETRICS), &fontMetrics);
      strcpy(szFamily, fontMetrics.szFamilyname);

      memset(&fontDlg, 0, sizeof(FONTDLG));
      memcpy(&fontDlg.fAttrs,&pAss->fAttrs,sizeof(FATTRS));
      fontDlg.hpsScreen      = hps;
      fontDlg.cbSize         = sizeof(FONTDLG);
      fontDlg.pszFamilyname  = szFamily;
      fontDlg.usFamilyBufLen = sizeof(szFamily);
      fontDlg.fxPointSize    = MAKEFIXED(8,0);
      fontDlg.fl             = FNTS_CENTER|FNTS_INITFROMFATTRS|FNTS_BITMAPONLY;
      fontDlg.flType         = (LONG) fontMetrics.fsType;
      fontDlg.clrFore        = CLR_BLACK;
      fontDlg.clrBack        = CLR_WHITE;
      fontDlg.sNominalPointSize = fontMetrics.sNominalPointSize;
      fontDlg.usWeight          = fontMetrics.usWeightClass;
      fontDlg.usWidth           = fontMetrics.usWidthClass;

      WinFontDlg(HWND_DESKTOP,hWnd,&fontDlg);
      WinReleasePS(hps);
      if(fontDlg.lReturn==DID_OK)
      {
        memcpy(&pAss->fAttrs,&fontDlg.fAttrs,sizeof(FATTRS));
        if (s1m1==IDD_PBCHFONT2ALL)
        {
          for(PASSTR p = fGlobal ? paoStr->pGAssList : paoStr->pLAssList; p; p=p->next)
            memcpy(&p->fAttrs,&fontDlg.fAttrs,sizeof(FATTRS));
        }
        WinSendMsg(hSTFontShow,WM_USER,MPARAM(&pAss->fAttrs),0);
      }
      break;
    }
    case DID_OK:
    {
      WinSendMsg(hWnd,WM_USER+1,0,0); // Сохранить установки
      if (paoStr->pEdAss)
      {
        paoStr->fClrToAll = WinQueryButtonCheckstate(hWnd,IDD_CBCLRTOALL);
        paoStr->fFntToAll = WinQueryButtonCheckstate(hWnd,IDD_PBCHFONT2ALL);
      }
      WinDismissDlg(hWnd,TRUE);
      break;
    }
    case DID_CANCEL:
      WinDismissDlg(hWnd,FALSE);
      break;
    case IDD_PBHELP:
      DisplayHelpPanel(paoStr->pEdAss ? PANEL_EDOPTDLG : PANEL_ASSDLG);
      break;
    }
    break;
  }
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(paoStr->pEdAss ? PANEL_EDOPTDLG : PANEL_ASSDLG);
      break;
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}



void fill_cllist(CLP_PRENTRYINFO *ppList, HWND hLBClrItems, PPAINTER pPainter)
{
  CLP_PRENTRYINFO p,p1;
  int i;
  for (p=*ppList; p; p=p1){p1=p->next; delete p;}
  *ppList = 0;

  char  asz[][50] = {"Normal",
                        "Normal (background)",
                          "Selection",
                            "Selection (background)",
                              "Current line (background)",""};
  ULONG aul[]     = {CLP_DEFAULT,
                        CLP_DEFAULT,
                          CLP_SELECTION,
                            CLP_SELECTION,
                              CLP_CURLINE};
  BOOL af[]       = {FALSE,
                        TRUE,
                          FALSE,
                            TRUE,
                              TRUE};

  for (i=0; asz[i][0]; i++)
  {
    p1 = new CLP_RENTRYINFO;
    if (!p1) break;
    strcpy(p1->szName,asz[i]);
    p1->idx         = aul[i];
    p1->fBackground = af [i];
    p1->next        = 0;
    if (!*ppList) *ppList = p1;
    else          p->next = p1;
    p = p1;
  }
  if (pPainter)
  {
    CLP_RENTRYINFO rec;
    for (i=0; (*pPainter->PClQueryEntryInfo)(i,&rec); i++)
    {
      rec.next =0;
      p1       = new CLP_RENTRYINFO;
      *p1      = rec;
      p->next  = p1;
      p        = p1;
    }
  }
  WinSendMsg(hLBClrItems,LM_DELETEALL,0,0);
  for (p=*ppList; p; p=p->next)
  {
    int nIt = (int)WinSendMsg(hLBClrItems,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(p->szName));
    WinSendMsg(hLBClrItems,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(p));
  }
  WinSendMsg(hLBClrItems,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
}

MRESULT WMStubCharHandler(HWND hWnd, MPARAM m1, MPARAM m2){ return 0;}

MRESULT EXPENTRY dpEditClrDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
// Edits global aaEdRGB (using global pointer pClrPainter)
// Color palette contains in the aRainbow array
  #define VSROW(order)      (order/8+1)
  #define VSCOLUMN(order)   (order%8+1)
  #define RC2ORDER(row,col) (col-1 + (row-1)*8)
{
  static HWND            hEdExample;
  static HWND            hVSColors;
  static HWND            hLBClrItems;
  static CLP_PRENTRYINFO pClList = 0;
  static BOOL            fVSLocked;

  switch(ulMsg){
  case WM_INITDLG:
  {
    int i;
    fVSLocked     = TRUE;
    hVSColors     = WinWindowFromID(hWnd,IDD_VSCOLORS);
    hLBClrItems   = WinWindowFromID(hWnd,IDD_LBCLRITEMS);
    for (i=0; i<RAINBOWLEN; i++)
    {
      WinSendMsg(hVSColors,VM_SETITEM,    MPFROM2SHORT(VSROW(i),VSCOLUMN(i)),MPARAM(aRainbow[i]));
      WinSendMsg(hVSColors,VM_SETITEMATTR,MPFROM2SHORT(VSROW(i),VSCOLUMN(i)),MPFROM2SHORT(VIA_OWNERDRAW,1));
    }
    fill_cllist(&pClList,hLBClrItems,pClrPainter);
    {// Create editor instance
      SWP  swp;
      HWND hST   = WinWindowFromID(hWnd,IDD_STCLREXAMPLE);
      WinQueryWindowPos(hST,&swp);
      TEDCTLDATA tcd;
      tcd.cb            = sizeof(tcd);
      tcd.pfCH          = (PFNCHARHANDLER)WMStubCharHandler;
      tcd.hNotifyWindow = hWnd;
      tcd.hSharedEd     = 0;
      hEdExample = WinCreateWindow(hST, TEDCLASS, "", WS_VISIBLE, 0,0,swp.cx,swp.cy, hWnd, HWND_TOP,0,&tcd,0);
      {// Editor initialisation:
        PASSTR pAssDef = ptOffice.QueryAss("");
        WinSendMsg(hEdExample,TM_SETFONT, MPARAM(&pAssDef->fAttrs), 0);
        PSZ pszTxt =  "\n"
                      "Example text\n"
                      "%Selection% here.\n";
        if (pClrPainter) pszTxt = (*pClrPainter->PClQueryExText)();
        TPR  tprC(0,0);
        TPR  tprA(0,0);
        TPR  tpr (0,0);
        BOOL f   = FALSE;
        PSZ  psz = (PSZ)malloc(strlen(pszTxt)+1);
        strcpy(psz,pszTxt);
        for (char *pch = psz; *pch; pch++)
        {
          if      (*pch=='\n')
            {tpr.lLine++; tpr.lPos=0; continue;}
          else if (*pch=='%')
          {
            if (!f) {tprA = tpr; tprA.lPos++;}
            else    tprC = tpr;
            f    = TRUE;
            *pch = ' ';
          }
          tpr.lPos++;
        }
        WinSendMsg(hEdExample,TM_SETTEXT,MPARAM(ULONG(psz)),MPARAM(strlen(psz)));
        EDCONF ec;
        memset(&ec,0,sizeof(ec));
        WinSendMsg(hEdExample,TM_QUERYEDCONF,MPARAM(&ec),0);
        ec.xLeftScrShift = ec.lFirstScrLine  = 0;
        ec.fHilite       = ec.fHiliteCurLine = TRUE;
        WinSendMsg(hEdExample,TM_SETEDCONF,MPARAM(&ec),0);
        WinSendMsg(hEdExample,TM_SETSEL,MPARAM(&tprC),MPARAM(&tprA));
        WinSendMsg(hEdExample,TM_SETPAINTER,MPARAM(pClrPainter ? pClrPainter->szNameId : 0),MPARAM(TRUE));
        WinSendMsg(hEdExample,TM_SETCOLORS, MPARAM(aaEdRGB),0);
      }
    }
    WinSendMsg(hVSColors,VM_SELECTITEM,MPFROM2SHORT(VSROW(0),VSCOLUMN(0)),0);
    WinPostMsg(hWnd,WM_USER,0,0); // Какие-то тут баги с фокусом...
    break;
  }
  case WM_USER:
    WinSetActiveWindow(HWND_DESKTOP,hLBClrItems);
    break;
  case WM_CONTROL:
  {
    ULONG           ul = (ULONG)          WinSendMsg(hLBClrItems,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    CLP_PRENTRYINFO p  = (CLP_PRENTRYINFO)WinSendMsg(hLBClrItems,LM_QUERYITEMHANDLE,MPARAM(ul),0);

    if (SHORT1FROMMP(m1)==IDD_LBCLRITEMS && (SHORT2FROMMP(m1)==LN_SELECT||SHORT2FROMMP(m1)==LN_ENTER))
    {
      WinCheckButton  (hWnd, IDD_CBCLRUNDERLINE, p && !p->fBackground && (aaEdRGB[p->idx][CLP_FOREGROUND] & CLP_UNDERLINETAG));
      WinEnableControl(hWnd, IDD_CBCLRUNDERLINE, p && !p->fBackground);
      break;
    }
    else if (SHORT1FROMMP(m1)==IDD_CBCLRUNDERLINE)
    {
      PULONG pulClr = &aaEdRGB[p->idx][CLP_FOREGROUND];
      ULONG  ulTag  = WinQueryButtonCheckstate(hWnd,IDD_CBCLRUNDERLINE) ? CLP_UNDERLINETAG : 0;
      if ( (*pulClr ^ ulTag) & CLP_UNDERLINETAG )
      {
        *pulClr ^= CLP_UNDERLINETAG;
        WinSendMsg(hEdExample,TM_SETCOLORS, MPARAM(aaEdRGB),0);
      }
      break;
    }
    else if (SHORT1FROMMP(m1)==IDD_VSCOLORS)
    {
      switch(SHORT2FROMMP(m1))
      {
      case VN_ENTER:
      {
        CPARAMSTR cps;
        int       nOrder = RC2ORDER(SHORT1FROMMP(m2), SHORT2FROMMP(m2));
        if (nOrder<16) {Beep(); break;}
        cps.l1 = (LONG)WinSendMsg(hVSColors,VM_QUERYITEM,m2,0);
        if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpMakeClrDlg,NULLHANDLE,IDD_MAKECLRDLG,&cps))
        {
          aRainbow[nOrder] = cps.l1;
          WinSendMsg(hVSColors,VM_SETITEM,m2,MPARAM(cps.l1));
        }
        // no 'break'
      }
      case VN_SETFOCUS:
        fVSLocked = FALSE;
      case VN_SELECT:
        if (!fVSLocked)
        {
          ULONG ulVS = (ULONG)WinSendMsg(hVSColors,  VM_QUERYSELECTEDITEM,0,0);
          if (!ulVS || !p) return 0;
          ULONG rgb = (ULONG)WinSendMsg(hVSColors,VM_QUERYITEM,MPARAM(ulVS),0);
          aaEdRGB[p->idx][p->fBackground ? CLP_BACKGROUND : CLP_FOREGROUND] = rgb & CLP_COLORMASK;
          if (WinQueryButtonCheckstate(hWnd,IDD_CBCLRUNDERLINE))
            aaEdRGB[p->idx][CLP_FOREGROUND] |= CLP_UNDERLINETAG;
          if (p->idx==CLP_DEFAULT && p->fBackground)
          {
            for (int i=0; i<CLP_PALETTELEN; i++)
              if (i!=CLP_CURLINE && i!=CLP_SELECTION)
                aaEdRGB[i][CLP_BACKGROUND] = rgb;
          }
          WinSendMsg(hEdExample,TM_SETCOLORS, MPARAM(aaEdRGB),0);
          break;
        }
      }
    }
    break;
  }
  case WM_DRAWITEM:
  {
    POWNERITEM pow = (POWNERITEM)m2;
    if (pow->idItem != VDA_ITEM && pow->idItem != VDA_ITEMBACKGROUND)
      return MRESULT(FALSE);
    ULONG rgb = (ULONG)WinSendMsg(hVSColors,VM_QUERYITEM,MPARAM(pow->hItem),0);
    GpiSetColor(pow->hps,CLR_DARKGRAY);
    POINTL ptl = {pow->rclItem.xLeft,pow->rclItem.yBottom};
    GpiSetCurrentPosition(pow->hps,&ptl);
    ptl.y = pow->rclItem.yTop-1;    GpiLine(pow->hps,&ptl);
    ptl.x = pow->rclItem.xRight-1;  GpiLine(pow->hps,&ptl);
    ptl.y = pow->rclItem.yBottom;   GpiLine(pow->hps,&ptl);
    ptl.x = pow->rclItem.xLeft;     GpiLine(pow->hps,&ptl);
    pow->rclItem.xLeft  ++;  pow->rclItem.yBottom ++;
    pow->rclItem.xRight --;  pow->rclItem.yTop    --;
    GpiCreateLogColorTable(pow->hps,LCOL_PURECOLOR,LCOLF_RGB,0,0,0);
    WinFillRect(pow->hps,&pow->rclItem,rgb);
    return MRESULT(TRUE);
  }
  case WM_CLOSE:
    m1 = MPFROM2SHORT(DID_CANCEL,0);
  case WM_COMMAND:
    switch(SHORT1FROMMP(m1)){
    case DID_OK:
      WinDismissDlg(hWnd,TRUE);
      break;
    case DID_CANCEL:
      WinDismissDlg(hWnd,FALSE);
      break;
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_EDITCLRDLG);
      break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_EDITCLRDLG);
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

#define RFROMUL(ul) ((ul & 0xff0000)>>16)
#define GFROMUL(ul) ((ul & 0x00ff00)>>8)
#define BFROMUL(ul) ( ul & 0x0000ff)
#define R2UL(r,ul)  (ul = ul&0x0000ffff | ((ULONG(r)&0xff)<<16))
#define G2UL(g,ul)  (ul = ul&0x00ff00ff | ((ULONG(g)&0xff)<<8))
#define B2UL(b,ul)  (ul = ul&0x00ffff00 | ((ULONG(b)&0xff)))


MRESULT EXPENTRY wpClrMatrix (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  //  Notifycation:
  //     posts to the parent window WM_USER (m1 == TRUE, m2 == ulRGB);
  static PFNWP pWindowProc;
  static BOOL  fLockMove;

  switch(ulMsg){
  case MM_START:
    pWindowProc = (PFNWP)m1;
    fLockMove   = TRUE;
    return 0;
  case WM_MOUSEMOVE:
    if (fLockMove) break;
    if (!(0x8000 & WinGetKeyState(HWND_DESKTOP,VK_BUTTON1))) break;
  case WM_BUTTON1DOWN:
  {
    POINTL ptl = {SHORT1FROMMP(m1),SHORT2FROMMP(m1)};
    HPS    hps = WinGetPS(hWnd);
    GpiCreateLogColorTable(hps,LCOL_PURECOLOR,LCOLF_RGB,0,0,0);
    LONG   clr = GpiQueryPel(hps,&ptl);
    if (clr>=0)
      WinPostMsg(WinQueryWindow(hWnd,QW_PARENT),WM_USER,MPARAM(TRUE),MPARAM(clr));
    WinReleasePS(hps);
    fLockMove = FALSE;
    break;
  }
  }
  return (*pWindowProc)(hWnd, ulMsg, m1, m2);
}

MRESULT EXPENTRY wpClrScale (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  /*
  /  Message processing:
  /  WM_USER (m1==ulRGB): Set this color and notify parent window
  /  Mouse messages:      Adjust color   and notify parent window
  /  Notifycation:
  /     sends to the parent window WM_USER (m1 == FALSE, m2 == ulRGB);
  */
  static PFNWP pWindowProc;
  static BOOL  fLockMove;
  static int   ulRMax;
  static int   ulGMax;
  static int   ulBMax;
  static int   ulR;
  static int   ulG;
  static int   ulB;
  static int   yPos;
  static int   ulWinHeight;
  static int   ulWinWidth;

  switch(ulMsg){
  case MM_START:
  {
    RECTL rcl;
    WinQueryWindowRect(hWnd,&rcl);
    pWindowProc   = (PFNWP)m1;
    fLockMove     = TRUE;
    ulWinHeight   = rcl.yTop ? rcl.yTop : 1;
    ulWinWidth    = rcl.xRight;
    return 0;
  }
  case WM_MOUSEMOVE:
    if (fLockMove) break;
    if (!(0x8000 & WinGetKeyState(HWND_DESKTOP,VK_BUTTON1))) break;
  case WM_BUTTON1DOWN:
  {
    yPos        = (SHORT2FROMMP(m1) & 0x8000) ? 0 : min(SHORT2FROMMP(m1),ulWinHeight);
    fLockMove   = FALSE;
    ulR         = ulRMax * yPos / ulWinHeight;
    ulG         = ulGMax * yPos / ulWinHeight;
    ulB         = ulBMax * yPos / ulWinHeight;
    WinInvalidateRect(hWnd,0,0);
    WinSendMsg(WinQueryWindow(hWnd,QW_PARENT),WM_USER,MPARAM(FALSE),
               MPARAM(((ULONG(ulR)&0xff)<<16)+((ULONG(ulG)&0xff)<<8)+(ULONG(ulB)&0xff)));
    break;
  }
  case WM_PAINT:
  {
    HPS    hps = WinBeginPaint(hWnd,0,0);
    ULONG  rgb,rgbFr;
    POINTL p0,p1;
    p0.x = 0;
    p1.x = ulWinWidth;
    GpiCreateLogColorTable(hps,0,LCOLF_RGB,0,0,0);
    for (int i=0; i<=ulWinHeight; i++)
    {
      rgb = (((ULONG(ulRMax)&0xff) * i / ulWinHeight ) << 16) +
            (((ULONG(ulGMax)&0xff) * i / ulWinHeight ) << 8 ) +
            (( ULONG(ulBMax)&0xff) * i / ulWinHeight );
      p0.y = p1.y = i;
      GpiSetCurrentPosition(hps,&p0);
      GpiSetColor          (hps,rgb);
      GpiLine              (hps,&p1);
    }
    // Draw thumb box
    int cy = ulWinWidth*2/3;
    int y  = max(yPos,cy/2); y = min(y,ulWinHeight-cy/2);
    p0.y   = y-cy/2;
    p1.y   = y+cy/2;
    rgb    = ((ULONG(ulR)&0xff) << 16) + ((ULONG(ulG)&0xff) << 8 ) + (ULONG(ulB)&0xff);
    if ((ulR|ulG|ulB) < 0x80) rgbFr = 0xffffff;
    else                      rgbFr = 0;
    GpiCreateLogColorTable(hps,LCOL_PURECOLOR,LCOLF_RGB,0,0,0);
    GpiSetCurrentPosition (hps,&p0);
    GpiSetColor           (hps,rgb);
    GpiBox                (hps,DRO_FILL,&p1,0,0);
    GpiSetColor           (hps,rgbFr);
    GpiBox                (hps,DRO_OUTLINE,&p1,0,0);
    WinEndPaint(hps);
    return 0;
  }
  case WM_USER:
  {
    ulR = RFROMUL(ULONG(m1));
    ulG = GFROMUL(ULONG(m1));
    ulB = BFROMUL(ULONG(m1));
    if (!(ulR|ulG|ulB)) // Grayscale
    {
      ulRMax = ulGMax = ulBMax = 0xff;
      yPos   = 0;
    }
    else
    {
      int clrMax = max(max(ulR,ulG),ulB);
      float mul  = float(0xff)/float(clrMax);
      ulRMax     = int(0.999 + float(ulR)*mul); ulRMax = min(ulRMax,0xff);
      ulGMax     = int(0.999 + float(ulG)*mul); ulGMax = min(ulGMax,0xff);
      ulBMax     = int(0.999 + float(ulB)*mul); ulBMax = min(ulBMax,0xff);
      int hiMax  = max(max(ulRMax,ulGMax),ulBMax);
      yPos       = int(float(clrMax)/float(hiMax)*float(ulWinHeight));
    }
    WinSendMsg(WinQueryWindow(hWnd,QW_PARENT),WM_USER,MPARAM(FALSE),
               MPARAM(((ULONG(ulR)&0xff)<<16)+((ULONG(ulG)&0xff)<<8)+(ULONG(ulB)&0xff)));
    WinInvalidateRect(hWnd,0,0);
    return 0;
  }
  }
  return (*pWindowProc)(hWnd, ulMsg, m1, m2);
}
MRESULT EXPENTRY dpMakeClrDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
{
  MRESULT EXPENTRY   wpDecimalEFProc (HWND, ULONG, MPARAM, MPARAM);
  static PCPARAMSTR  pcps;
  static HWND        hStClrMatrix;
  static HWND        hStClrScale;
  static HWND        hSPBR;
  static HWND        hSPBG;
  static HWND        hSPBB;
  static ULONG       ulRGB;
  extern TID         tidClrMatrix;
  extern HBITMAP     hbmClrMatrix;

  switch (ulMsg){
  case WM_INITDLG:
  {
    pcps  = (PCPARAMSTR)m2;
    hStClrMatrix = WinWindowFromID(hWnd,IDD_STCLRMATRIX);
    hStClrScale  = WinWindowFromID(hWnd,IDD_STCLRSCALE);
    hSPBR        = WinWindowFromID(hWnd,IDD_SPBRVAL);
    hSPBG        = WinWindowFromID(hWnd,IDD_SPBGVAL);
    hSPBB        = WinWindowFromID(hWnd,IDD_SPBBVAL);
    {
      if (tidClrMatrix)
        DosWaitThread(&tidClrMatrix,DCWW_WAIT);
      BITMAPINFOHEADER  bh;
      if (GpiQueryBitmapParameters(hbmClrMatrix,&bh))
      {
        SWP  swp;
        WinSendMsg(hStClrMatrix,SM_SETHANDLE,MPARAM(hbmClrMatrix),0);
        WinQueryWindowPos(hStClrMatrix,&swp);
        //--- Center the bitmap related to it's current position
        swp.x = swp.x - bh.cx/2;
        swp.y = swp.y - bh.cy/2;
        WinSetWindowPos(hStClrMatrix,0,swp.x,swp.y,bh.cx,bh.cy,SWP_MOVE|SWP_SIZE);
      }
    }
    WinSendMsg(hSPBR,SPBM_SETLIMITS,MPARAM(255),MPARAM(0));
    WinSendMsg(hSPBG,SPBM_SETLIMITS,MPARAM(255),MPARAM(0));
    WinSendMsg(hSPBB,SPBM_SETLIMITS,MPARAM(255),MPARAM(0));
    wpClrScale (hStClrScale, MM_START,MPARAM(ULONG(WinSubclassWindow(hStClrScale, (PFNWP)wpClrScale ))),0L);
    wpClrMatrix(hStClrMatrix,MM_START,MPARAM(ULONG(WinSubclassWindow(hStClrMatrix,(PFNWP)wpClrMatrix))),0L);
    WinSendMsg (hStClrScale, WM_USER, MPARAM(pcps->l1),0);
    return 0;
  }
  case WM_USER:
  // m1 == TRUE  (message from hStClrMatrix) - send message to hStClrScale
  // m1 == FALSE (message from hStClrScale)  - set this RGB value
  {
    if (m1)
      WinSendMsg(hStClrScale,WM_USER,m2,0);
    else
    {
      ulRGB     = ULONG(m2)&0x00ffffff;
      WinSendMsg(hSPBR,SPBM_SETCURRENTVALUE,MPARAM(RFROMUL(ulRGB)),0);
      WinSendMsg(hSPBG,SPBM_SETCURRENTVALUE,MPARAM(GFROMUL(ulRGB)),0);
      WinSendMsg(hSPBB,SPBM_SETCURRENTVALUE,MPARAM(BFROMUL(ulRGB)),0);
    }
    break;
  }
  case WM_CONTROL:
  {

    if ((SHORT1FROMMP(m1)==IDD_SPBRVAL ||
         SHORT1FROMMP(m1)==IDD_SPBGVAL ||
         SHORT1FROMMP(m1)==IDD_SPBBVAL)   &&
        (SHORT2FROMMP(m1)==SPBN_UPARROW   ||
         SHORT2FROMMP(m1)==SPBN_DOWNARROW ||
         SHORT2FROMMP(m1)==SPBN_ENDSPIN ))
    {
      int r=0,g=0,b=0;
      WinSendMsg(hSPBR,SPBM_QUERYVALUE,MPARAM(&r),MPFROM2SHORT(0,SPBQ_DONOTUPDATE));
      WinSendMsg(hSPBG,SPBM_QUERYVALUE,MPARAM(&g),MPFROM2SHORT(0,SPBQ_DONOTUPDATE));
      WinSendMsg(hSPBB,SPBM_QUERYVALUE,MPARAM(&b),MPFROM2SHORT(0,SPBQ_DONOTUPDATE));
      R2UL(r,ulRGB);
      G2UL(g,ulRGB);
      B2UL(b,ulRGB);
      WinSendMsg(hStClrScale,WM_USER,MPARAM(ulRGB),0);
    }
    break;
  }
  case WM_CLOSE: m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch SHORT1FROMMP(m1){
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_MAKECLRDLG);
      break;
    case DID_OK:
      pcps->l1 = ulRGB;
    case DID_CANCEL:
      WinDismissDlg(hWnd,SHORT1FROMMP(m1)==DID_OK);
      break;
    }
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_MAKECLRDLG);
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


PSZ mk_itemtext(HWND hLB, int nIt)
// Returns malloc()ed item text. If nIt==LIT_NONE assumed selection
{
  if (nIt==LIT_NONE) nIt = (int)WinSendMsg(hLB,LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
  if (nIt==LIT_NONE) return 0;
  int                len = (int)WinSendMsg(hLB,LM_QUERYITEMTEXTLENGTH,MPARAM(nIt), 0) + 1;
  PSZ                psz = (PSZ)malloc(len);
  if (psz)
    WinSendMsg(hLB,LM_QUERYITEMTEXT,MPFROM2SHORT(nIt,len),MPARAM(psz));
  return psz;
}

PASSTR mk_asslist(HWND hLB)
{
  PASSTR  p, pRet=0, pE=0;
  PSZ     psz;
  int     nIC = (int)WinSendMsg(hLB, LM_QUERYITEMCOUNT, 0, 0);
  for (int nIt =0; nIt<nIC; nIt++)
  {
    if (!(psz = mk_itemtext(hLB,nIt))) continue;
    p = (PASSTR)WinSendMsg(hLB, LM_QUERYITEMHANDLE, MPARAM(nIt), 0);
    if (p)
    {
      PASSTR p1 = new ASSTR;
      p1->Assign(p);
      p = p1;
    }
    else
    {
      p           = new ASSTR(TRUE);
      p->fDefault = FALSE;
    }
    p->SetFiles(psz);
    free(psz);
    if (pE) pE->next = p;
    else    pRet     = p;
    pE               = p;
  }
  return pRet;
}

MRESULT EXPENTRY dpEdAssList (HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2)
// Редактор списков ассоциаций из (ASSOPDLGSTR*)m2. При cancel-е
// ничего не меняет и возвращает FALSE.
//
{
  static ASSOPTDLGSTR *paoStr;
  static HWND          hLBGAss;
  static HWND          hLBLAss;

  switch(ulMsg){
  case WM_INITDLG:
  {
    // Заполним листы. Хендлы указывают на элементы исходных списков,
    // новые неинициализированные хендлы будут нулевые. При Ok мы построим новые
    // списки, копируя указанное содержимое из старых или, для строк с нулевыми
    // хэндлами, создавая дефолтные ассоциации. После этого старые списки
    // можно будет уничтожить.
    paoStr      = (ASSOPTDLGSTR*)m2;
    hLBGAss     = WinWindowFromID(hWnd,IDD_LBGASSLIST);
    hLBLAss     = WinWindowFromID(hWnd,IDD_LBLASSLIST);
    PASSTR p;
    for (p = paoStr->pGAssList; p; p=p->next)
    {
      int nIt = (int)WinSendMsg(hLBGAss,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(p->QueryFiles()));
      WinSendMsg(hLBGAss,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(p));
    }
    for (p = paoStr->pLAssList; p; p=p->next)
    {
      int nIt = (int)WinSendMsg(hLBLAss,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(p->QueryFiles()));
      WinSendMsg(hLBLAss,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(p));
    }
    WinSendMsg(hWnd,WM_USER,0,0);
    break;
  }
  case WM_USER: // Re-enable buttons
  {
    int    nIt,g;
    BOOL   f;
    //--- Local selection:
    nIt = (int)WinSendMsg(hLBLAss,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    f   = (nIt!=LIT_NONE);
    WinEnableControl(hWnd, IDD_PBLASSEDIT, f);
    WinEnableControl(hWnd, IDD_PBLASSDUP,  f);
    WinEnableControl(hWnd, IDD_PBLASSREM,  f);
    WinEnableControl(hWnd, IDD_PBASSCPYLG, f);
    WinEnableControl(hWnd, IDD_PBASSMVLG,  f);
    //--- Global selection:
    nIt   = (int)WinSendMsg(hLBGAss,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    if (f = (nIt!=LIT_NONE))
    {
      PASSTR p = (PASSTR)WinSendMsg(hLBGAss, LM_QUERYITEMHANDLE, MPARAM(nIt), 0);
      f        = (!p || !p->fDefault);
    }
    WinEnableControl(hWnd, IDD_PBGASSDUP,  f);
    WinEnableControl(hWnd, IDD_PBASSCPYGL, f);
    WinEnableControl(hWnd, IDD_PBGASSEDIT, f);
    WinEnableControl(hWnd, IDD_PBGASSREM,  f);
    WinEnableControl(hWnd, IDD_PBASSMVGL,  f);

    case IDD_PBLASSMVUP:
    case IDD_PBLASSMVDOWN:
    case IDD_PBGASSMVUP:
    case IDD_PBGASSMVDOWN:
    //--- Enable 'Move UP/DOWN' keys:
    for (g=0; g<=1; g++)
    {
      HWND hLB = (g ? hLBGAss : hLBLAss);
      int  nIC = (int)WinSendMsg(hLB, LM_QUERYITEMCOUNT,  0,                 0);
      nIt      = (int)WinSendMsg(hLB, LM_QUERYSELECTION,  MPARAM(LIT_FIRST), 0);
      WinEnableControl(hWnd, g ? IDD_PBGASSMVUP   : IDD_PBLASSMVUP,   (nIt!=LIT_NONE && nIt>0    ));
      WinEnableControl(hWnd, g ? IDD_PBGASSMVDOWN : IDD_PBLASSMVDOWN, (nIt!=LIT_NONE && nIt+1<nIC));
    }
    break;
  }
  case WM_CONTROL:
  {
    SHORT s1m1 = SHORT1FROMMP(m1);
    if ((s1m1==IDD_LBLASSLIST || s1m1==IDD_LBGASSLIST) && (SHORT2FROMMP(m1)==LN_SELECT))
      WinSendMsg(hWnd,WM_USER,0,0);
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
  {
    SHORT s1m1 = SHORT1FROMMP(m1);
    HWND  hLB  = hLBLAss;
    switch(s1m1)
    {
      case IDD_PBGASSNEW: hLB  = hLBGAss;
      case IDD_PBLASSNEW:
      {
        char      sz[CCHMAXPATH] = "";
        CPARAMSTR cps;
        cps.psz1 = "New association:";
        cps.psz2 = "Filename template:";
        cps.psz3 = sz;
        cps.l1   = sizeof(sz);
        cps.l3   = 0;
        if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
        {
          int nIt = (int)WinSendMsg(hLB, LM_QUERYSELECTION,  MPARAM(LIT_FIRST), 0);
          if (nIt=LIT_NONE) nIt=0;
          nIt = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(nIt),MPARAM(sz));
          WinSendMsg(hLB, LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
        }
        break;
      }
      case IDD_PBGASSEDIT: hLB  = hLBGAss;
      case IDD_PBLASSEDIT:
      {
        char sz[CCHMAXPATH] = "*";
        int nIt = (int)WinSendMsg(hLB, LM_QUERYSELECTION,  MPARAM(LIT_FIRST), 0);
        if (nIt==LIT_NONE)    break;

        PASSTR p = (PASSTR)WinSendMsg(hLB, LM_QUERYITEMHANDLE, MPARAM(nIt),       0);
        if (p && p->fDefault) break;

        WinSendMsg(hLB,LM_QUERYITEMTEXT,MPFROM2SHORT(nIt,sizeof(sz)),MPARAM(sz));

        CPARAMSTR cps;
        cps.psz1 = "Edit association:";
        cps.psz2 = "Filename template:";
        cps.psz3 = sz;
        cps.l1   = sizeof(sz);
        cps.l3   = 0;
        if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
          WinSendMsg(hLB,LM_SETITEMTEXT,MPARAM(nIt),MPARAM(sz));
        break;
      }
      case IDD_PBGASSDUP: hLB  = hLBGAss;
      case IDD_PBLASSDUP:
      {
        int nIt = (int)WinSendMsg(hLB, LM_QUERYSELECTION,  MPARAM(LIT_FIRST), 0);
        if (nIt==LIT_NONE)    break;

        PASSTR p = (PASSTR)WinSendMsg(hLB, LM_QUERYITEMHANDLE, MPARAM(nIt),       0);
        if (p && p->fDefault) break;

        PSZ pszDup = mk_itemtext(hLB,nIt);
        if (!pszDup)break;

        nIt = (int)WinSendMsg(hLB,LM_INSERTITEM,MPARAM(nIt),MPARAM(pszDup));
        WinSendMsg(hLB, LM_SETITEMHANDLE, MPARAM(nIt), MPARAM(p));
        free(pszDup);
        WinSendMsg(hLB, LM_SELECTITEM,    MPARAM(nIt), MPARAM(TRUE));
        break;
      }
      case IDD_PBGASSREM: hLB  = hLBGAss;
      case IDD_PBLASSREM:
      {
        int nIt = (int)WinSendMsg(hLB, LM_QUERYSELECTION,  MPARAM(LIT_FIRST), 0);
        if (nIt==LIT_NONE)    break;

        PASSTR p = (PASSTR)WinSendMsg(hLB, LM_QUERYITEMHANDLE, MPARAM(nIt), 0);
        if (p && p->fDefault) break;

        WinSendMsg(hLB, LM_DELETEITEM,MPARAM(nIt),0);
        WinSendMsg(hLB, LM_SELECTITEM,MPARAM(nIt ? nIt-1 : 0),MPARAM(TRUE));
        WinSendMsg(hWnd,WM_USER,0,0);
        break;
      }
      case IDD_PBLASSMVUP:
      case IDD_PBLASSMVDOWN:
      case IDD_PBGASSMVUP:
      case IDD_PBGASSMVDOWN:
      {
        HWND   hList = (s1m1==IDD_PBGASSMVUP || s1m1==IDD_PBGASSMVDOWN) ? hLBGAss : hLBLAss;
        BOOL   fUp   = (s1m1==IDD_PBGASSMVUP || s1m1==IDD_PBLASSMVUP);
        int    nIt = (int)   WinSendMsg(hList, LM_QUERYSELECTION,  MPARAM(LIT_FIRST), 0);
        PASSTR p   = (PASSTR)WinSendMsg(hList, LM_QUERYITEMHANDLE, MPARAM(nIt),       0);
        int    nIC = (int)   WinSendMsg(hList, LM_QUERYITEMCOUNT,  0,                 0);
        if ((fUp && !nIt) || (!fUp && nIt+1>=nIC)) break;
        PSZ    psz;
        if (!( psz = mk_itemtext(hList,nIt))) break;
        WinSendMsg(hList,LM_DELETEITEM,MPARAM(nIt),0);
        nIt = (fUp) ? nIt-1 : nIt+1;
        nIt = (int)WinSendMsg(hList,LM_INSERTITEM,MPARAM(nIt),MPARAM(psz));
        WinSendMsg(hList, LM_SETITEMHANDLE, MPARAM(nIt), MPARAM(p));
        WinSendMsg(hList, LM_SELECTITEM,    MPARAM(nIt), MPARAM(TRUE));
        free(psz);
        break;
      }
      case IDD_PBASSCPYLG:
      case IDD_PBASSCPYGL:
      case IDD_PBASSMVLG:
      case IDD_PBASSMVGL:
      {
        PSZ    psz;
        HWND   hSrc    = (s1m1==IDD_PBASSCPYGL || s1m1==IDD_PBASSMVGL) ? hLBGAss : hLBLAss;
        HWND   hTarg   = (hSrc==hLBLAss)                               ? hLBGAss : hLBLAss;
        BOOL   fMv     = (s1m1==IDD_PBASSMVGL || s1m1==IDD_PBASSMVLG);
        int    nItSrc  = (int)   WinSendMsg(hSrc, LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
        int    nItTarg = (int)   WinSendMsg(hTarg,LM_QUERYSELECTION, MPARAM(LIT_FIRST),0);
        if    (nItSrc  == LIT_NONE) break;
        if    (nItTarg == LIT_NONE) nItTarg = LIT_END;
        PASSTR p       = (PASSTR)WinSendMsg(hSrc, LM_QUERYITEMHANDLE,MPARAM(nItSrc),   0);
        if ((p && p->fDefault) || !(psz = mk_itemtext(hSrc,nItSrc))) break;
        nItTarg = (int)WinSendMsg(hTarg, LM_INSERTITEM,    MPARAM(nItTarg), MPARAM(psz));
                       WinSendMsg(hTarg, LM_SETITEMHANDLE, MPARAM(nItTarg), MPARAM(p));
        free(psz);
        if (fMv) WinSendMsg(hSrc, LM_DELETEITEM, MPARAM(nItSrc),0);
        WinSendMsg(hTarg, LM_SELECTITEM, MPARAM(nItTarg),MPARAM(TRUE)); // Re-enable inside
        WinSetFocus(HWND_DESKTOP,hTarg);
        break;
      }
      case IDD_PBVARS:
        EditVarTable(hWnd);
        break;
      case DID_OK:
      {
        PASSTR p,p1;
        // Строим оба списка по хэндлам из листов, при нулевых хэндлах ставим
        // дефолтные ассоциации.
        PASSTR pLList = mk_asslist(hLBLAss);
        PASSTR pGList = mk_asslist(hLBGAss);
        // На всякий случай проверим наличие дефолтного элемента в pGList-е
        {
          for (p=pGList; p; p=p->next) if (p->fDefault) break;
          if (!p) // BUG! Воткнем его туда - может удастся выжить...
          {
            p           = new ASSTR(TRUE);
            p->fDefault = TRUE;
            p->next     = pGList;
            pGList      = p;
          }
        }
        // Замещаем старые списки новыми
        for (p=paoStr->pGAssList; p; p=p1) {p1=p->next; delete p;}
        for (p=paoStr->pLAssList; p; p=p1) {p1=p->next; delete p;}
        paoStr->pGAssList = pGList;
        paoStr->pLAssList = pLList;
        WinDismissDlg(hWnd,TRUE);
        break;
      }
      case DID_CANCEL:
      {
        WinDismissDlg(hWnd,FALSE);
        break;
      }
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_EDASSLISTDLG);
        break;
    }
    break;
  } // case WM_COMMAND
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_EDASSLISTDLG);
      break;
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


/////-- Prepare color matrix thread:

TID     tidClrMatrix = 0;
HBITMAP hbmClrMatrix = 0;

void FontClrInit()
{
  void APIENTRY MakeClrMatrixThread(ULONG ul);
  DosCreateThread(&tidClrMatrix,&MakeClrMatrixThread,0,CREATE_READY|STACK_SPARSE,8192L);
}



void APIENTRY MakeClrMatrixThread(ULONG ul)
{
  int bmp_cx = WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN)/4;
  int bmp_cy = WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN)/4;

  { //--- create hbmClrMatrix:
    HPS         hps = WinGetPS(HWND_DESKTOP);
    BITMAPINFOHEADER2 bmih;
    memset(&bmih, 0, sizeof(bmih));
    bmih.cbFix     = sizeof(bmih);
    bmih.cx        = bmp_cx;
    bmih.cy        = bmp_cy;
    bmih.cPlanes   = 1;
    bmih.cBitCount = 24;
    hbmClrMatrix   = GpiCreateBitmap(hps, &bmih, 0, 0, 0);
    WinReleasePS(hps);
  }

  HPS hpsBmp; //--- Create it:
  {
    struct
    {
      PSZ   address;
      PSZ   driver_name;
      PSZ   driver_data;
      PSZ   data_type;
      PSZ   comment;
      PSZ   proc_name;
      PSZ   proc_params;
      PSZ   spl_params;
      PSZ   network_params;
    }       dcdatablk = {0L, "DISPLAY", 0L, 0L, 0L, 0L, 0L, 0L, 0L};
    SIZEL   szl;
    szl.cx = bmp_cx;
    szl.cy = bmp_cy;
    HDC hdc = DevOpenDC(hAB, OD_MEMORY, (PSZ)"*", 8L, (PDEVOPENDATA)&dcdatablk,(HDC)0L);
    hpsBmp  = GpiCreatePS(hAB, hdc, &szl, GPIA_ASSOC | PU_PELS);
    GpiSetBitmap(hpsBmp,hbmClrMatrix);
  }

  //--- Paint all:
  GpiCreateLogColorTable(hpsBmp,LCOL_PURECOLOR,LCOLF_RGB,0,0,0);
  POINTL  ptl;
  double  r,g,b;
  int     nSlices = 3;
  double  xmax    = sqrt(nSlices*nSlices+1);
  double  xs      = xmax/nSlices;
  for (ptl.x=0; ptl.x<bmp_cx; ptl.x++)
    for (ptl.y=0; ptl.y<bmp_cy; ptl.y++)
    {
      double x  = double(ptl.x)/double(bmp_cx)*xmax;
      double y  = double(ptl.y)/bmp_cy;
      int    ns = int(x/xs);
      double x1 = x-xs*ns;
      double g1 = x1/xs;
      r         = (ns&1) ? 1-g1 : g1;
      g         = x/xmax;
      b         = y;
      ULONG rgb = (int(255.0*r)<<16) + (int(255.0*g)<<8) + int(255.0*b);
      GpiSetColor(hpsBmp,rgb);
      GpiSetCurrentPosition(hpsBmp,&ptl);
      GpiSetPel(hpsBmp,&ptl);
    }
  tidClrMatrix = 0;
  DosExit(EXIT_THREAD,0);
}



