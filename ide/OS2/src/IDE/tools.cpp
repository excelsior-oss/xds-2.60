/*
*
*  File: TOOLS.CPP
*
*  XDS Shell - tools realisation
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

#include "Filters.cpp"

MRESULT EXPENTRY dpToolCfgDlg    (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY dpToolChBtnDlg  (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY dpToolWizDlg    (HWND, ULONG, MPARAM, MPARAM);
void             RegisterToolREXX();

FILTER Filter;

//================================================================================//
//                                                                                //
//                      -----  T O O L  realisation  ----------                   //
//                                                                                //
//================================================================================//

TOOL::TOOL ()
{
  memset(this,0,sizeof(*this));
  cb = sizeof(*this);
}

TOOL::~TOOL()
{
  free_all();
}

BOOL TOOL::IsEnabled()
{
  if (!pszToolScript)                   return FALSE; // <--- F
  if (fRunWhenProject && !szPrjFile[0]) return FALSE; // <--- F
  //--- Соответствие имени:
  if (!szMatchFiles[0]) return TRUE;                  // <--- T
  PSZ      psz;
  char     szFN[CCHMAXPATH] = "";
  if (psz=OpenExprVar("$(File)")) {strcpy(szFN,psz); free(psz);}
  if (!(psz=OpenExprVar(szMatchFiles))) return FALSE; // <--- F
  BOOL fMatch = RCThunk_FastMatch(sf_strtoupper(psz),sf_strtoupper(szFN));
  free(psz);
  return fMatch;                                      // <--- ?
}


BOOL TOOL::Execute(HWND hMsgWnd)
{
  if (IsEnabled())
  {
    PSZ pszE;
    if (pszE=Filter.Set((pszFilter && pszFilter[0]=='+') ? pszFilter+1 : 0))
    {
      int rc = MessageBox(hMsgWnd,"Set filter(s)",MB_ERROR|MB_YESNO,pszE,"\nExecute tool without filter(s)?");
      free(pszE);
      if (rc!=MBID_YES) return FALSE;
      Filter.Set(0);
    }
    BOOL    rc = RxExecute(pszToolScript);
    // Restote tasklist item:
    SWCNTRL swctl;
    if (!WinQuerySwitchEntry(hSwitch,&swctl))
    {
      strcpy(swctl.szSwtitle,TASK_NAME);
      swctl.bProgType = PROG_PM;
      WinChangeSwitchEntry(hSwitch, &swctl);
    }
    return rc;
  }
  else return FALSE;
}

void TOOL::Copy(TOOL *pSrc)
{
  strcpy(szName,pSrc->szName);
  strcpy(szMatchFiles,pSrc->szMatchFiles);
  free(pszToolScript);
  free(pszFilter);
  pszToolScript   = pSrc->pszToolScript ? strdup(pSrc->pszToolScript) : 0;
  pszFilter       = pSrc->pszFilter     ? strdup(pSrc->pszFilter)     : 0;
  fRunWhenProject = pSrc->fRunWhenProject;
}

BOOL TOOL::WProfile(LONG slot, PSZ szKey)
{
  // Пишем: <структуру>+<строку тула,0>+<строку фильтра,0>,
  // если строки тула не должно быть - сотрем ключ
  BOOL fRC = TRUE;
  if (!pszToolScript) PfDeleteItem(slot, szKey);
  else
  {
    LONG  lStrLen = strlen(pszToolScript);
    LONG  lLen    = sizeof(*this) + lStrLen+1 + strlen(pszFilter?pszFilter:"")+1;
    PTOOL ptBuf   = (PTOOL)malloc(lLen);
    if (!ptBuf) return FALSE;
    memcpy(ptBuf,this,sizeof(*this));
    strcpy(PSZ(ptBuf)+sizeof(*this),           pszToolScript);
    strcpy(PSZ(ptBuf)+sizeof(*this)+lStrLen+1, pszFilter?pszFilter:"");
    fRC   = PfWriteItem(slot, szKey, ptBuf, lLen);
    free(ptBuf);
  }
  return fRC;
}

BOOL  TOOL::RProfile(LONG slot, PSZ szKey)
{
  // В файле: <структура>+<строка тула,0>+<строка фильтра,0>,
  free_all();
  LONG  lLen  = PfQueryItemSize(slot, szKey);  if (!lLen || lLen<sizeof(*this)) return FALSE;
  PTOOL ptBuf = (PTOOL)malloc(lLen+2);         if (!ptBuf)                      return FALSE;
  memset(ptBuf,0,lLen+2);
  if (lLen!=PfReadItem(slot, szKey, ptBuf, lLen, 0, 0))
  {
    free(ptBuf);
    return FALSE;
  }
  PSZ pszTS     = PSZ(ptBuf) + sizeof(*this);
  PSZ pszF      = PSZ(ptBuf) + sizeof(*this) + strlen(PSZ(ptBuf)+sizeof(*this)) + 1;

  memcpy(this,ptBuf,sizeof(*this));
  cb            = sizeof(*this);
  pszToolScript = *pszTS ? strdup(pszTS) : 0;
  pszFilter     = *pszF  ? strdup(pszF)  : 0;
  return TRUE;
}

void TOOL::free_all()
{
  free(pszToolScript);
  free(pszFilter);
  cb              = sizeof(*this);
  szName[0]       = 0;
  pszToolScript   = 0;
  pszFilter       = 0;
  fRunWhenProject = FALSE;
  szMatchFiles[0] = 0;
}


//================================================================================//
//                                                                                //
//           -----  T O O L C A T E G O R Y  realisation  ----------              //
//                                                                                //
//================================================================================//

TOOLCATEGORY::TOOLCATEGORY ()
{
  memset(this,0,sizeof(*this));
  cb = sizeof(*this);
}

TOOLCATEGORY::~TOOLCATEGORY ()
{
  free_all();
}

BOOL TOOLCATEGORY::IsEnabled ()
{
  for (PTOOL p = pToolList; p; p=p->next)
    if (p->IsEnabled()) return TRUE;
  return FALSE;
}

BOOL TOOLCATEGORY::Execute (HWND hMsgWnd)
{
  for (PTOOL p = pToolList; p; p=p->next)
    if (p->IsEnabled()) return p->Execute(hMsgWnd);
  return FALSE;
}

void TOOLCATEGORY::Apply (TOOLCATEGORY *pSrc)
{
  // Если у нас нет имени - копируем в себя pSrc, иначе - для всех тулзов из pSrc добавляем их
  // в себя, если дублируется имя тула - заменяем на последний встретившийся
  PTOOL pFrom, *pp;
  if (!szName[0])
  {
    free_all();
    memcpy(this,pSrc,sizeof(*this));
    cb        = sizeof(*this);
    pToolList = 0;
  }
  for (pFrom=pSrc->pToolList; pFrom; pFrom=pFrom->next)
  {
    for (pp = &pToolList; *pp; pp = &((*pp)->next))
      if (!sf_stricmp((*pp)->szName, pFrom->szName))
        break;
    if (!*pp) *pp = new TOOL;
    (*pp)->Copy(pFrom);
  }
}

BOOL TOOLCATEGORY::WProfile (LONG slot, PSZ szKey)
{
  // Пишем структуру, в поле pToolList - количество элементов списка,
  // тулы в списке имеют имена szKey+"_N_", где N - счетчик (0...)
  PTOOL p;
  int   i;

  for (i=0, p=pToolList; p; p=p->next) i++;
  p         = pToolList;
  pToolList = PTOOL(i);
  PfWriteItem(slot, szKey, this, sizeof(*this));
  pToolList = p;
  for (i=0, p=pToolList; p; p=p->next, i++)
  {
    char  szToolKey[100];
    sprintf(szToolKey,"%s_%u_",szKey,i);
    p->WProfile(slot,szToolKey);
  }
  return TRUE;
}

BOOL TOOLCATEGORY::RProfile (LONG slot, PSZ szKey)
{
  // Читаем структуру, в поле pToolList будет количество элементов списка,
  // тулы в списке имеют имена szKey+"_N_", где N - счетчик (0...)
  int   i, cnt;
  PTOOL pnew, plast = 0;

  free_all();
  if (sizeof(*this) != PfReadItem(slot, szKey, this, sizeof(*this), 0, 0))
  {
    free_all();
    return FALSE;
  }
  cnt       = int(pToolList);
  pToolList = 0;
  for (i=0; i<cnt; i++)
  {
    char  szToolKey[100];
    sprintf(szToolKey,"%s_%u_",szKey,i);
    pnew = new TOOL;
    if (!pnew->RProfile(slot,szToolKey))
    {
      delete pnew;
      return FALSE;
    }
    if (plast) plast->next = pnew;
    else       pToolList   = pnew;
    plast = pnew;
  }
  return TRUE;
}

void TOOLCATEGORY::free_all()
{
  for (PTOOL p1, p=pToolList; p; p = p1)
  {
    p1 = p->next;
    delete p;
  }
  cb             = sizeof(*this);
  szName    [0]  = 0;
  szComment [0]  = 0;
  szMenuItem[0]  = 0;
  memset(&rAccel,0,sizeof(rAccel));
  usBmp          = 0;
  pToolList      = 0;
  nLastEdTool    = 0;
}



//================================================================================//
//                                                                                //
//                    -------->  T O O L B O X  <----------                       //
//                                                                                //
//================================================================================//


TOOLBOX::TOOLBOX()
{
  static BOOL f1st = TRUE;
  if (f1st)
  {
    RegisterToolREXX();
    f1st = FALSE;
  }
  memset(this,0,sizeof(*this));
  for (int i=0; i<MAXTOOLS; i++)
    apToolCategories[i] = new TOOLCATEGORY;
}

TOOLBOX::~TOOLBOX()
{
  for (int i=0; i<MAXTOOLS; i++)
    delete apToolCategories[i];
}

BOOL TOOLBOX::Execute(LONG lCtrlId)
{
  ULONG nTool = lCtrlId-TOOL_MIN_ID;
  if (nTool>=MAXTOOLS)
  {
    Beep();
    return FALSE;
  }
  if ( apToolCategories[nTool]->szName[0] &&
       apToolCategories[nTool]->IsEnabled() &&
       !grOffice.SaveAll(TRUE,hMDI0Client) )
  {
    apToolCategories[nTool]->Execute(hMDI0Client);
    grOffice.CheckFileTimes(0);
  }
  return TRUE;
}
BOOL TOOLBOX::PlayTool(PQMSG pqmsg)
{
  ACCEL    rAccel;
  MPARAM   m1 = pqmsg->mp1;
  MPARAM   m2 = pqmsg->mp2;
#if DBGLOG
  {
    PSZ sz = Mp2Txt(&m1,&m2);
    fprintf(stderr, "PlayTool: m1=%08x m2=%08x, %s\n",pqmsg->mp1,pqmsg->mp2, sz);
  }
#else
  Mp2Txt(&m1,&m2);
#endif
  Mp2Acc (m1,m2,&rAccel);
  if (!rAccel.fs) return FALSE;
  for (int nTool=0; nTool<MAXTOOLS; nTool++)
    if (apToolCategories[nTool]->szName && match_accel(&apToolCategories[nTool]->rAccel, &rAccel))
      return Execute(nTool+TOOL_MIN_ID);
  return FALSE;
}
void TOOLBOX::EditTools()
{
  if (WinDlgBox(HWND_DESKTOP,hMDI0Client,(PFNWP)dpToolCfgDlg,NULLHANDLE,IDD_TOOLCFGDLG,0))
    reFill_controls(FALSE);
}

void TOOLBOX::ReenableAll()
{
  int i;
  for (i=0; i<MAXTOOLS; i++)
  {
    if (!apToolCategories[i]->szName) continue;
    BOOL fEnabled = apToolCategories[i]->IsEnabled();
    pToolbar->EnableButton(TOOL_MIN_ID+i, fEnabled);
    if (fEnabled)
      pToolbar->SetButtonComment(TOOL_MIN_ID+i, apToolCategories[i]->szComment);
  }
}

void TOOLBOX::TuneMenu()
{
  reFill_controls(TRUE);
}

void  TOOLBOX::Copy(TOOLBOX *ptb)
{
  for (int i=0; i<MAXTOOLS; i++)
  {
    apToolCategories[i]->free_all();
    apToolCategories[i]->Apply(ptb->apToolCategories[i]);
  }
}

void TOOLBOX::StorePopupPPFont(char szPPFont[FACESIZE+10])
{
  strcpy(TOOLBOX::szPPFont,szPPFont);
}

void TOOLBOX::free_all()
{
  for (int i=0; i<MAXTOOLS; i++)
    apToolCategories[i]->free_all();
  reFill_controls(FALSE);
}

BOOL TOOLBOX::RProfile(LONG slot, PSZ pszKey)
// Вернет FALSE если их станет слишком много
{
  char         szKey[20];
  int          i, nKey, nTarg;
  BOOL         fRC   = TRUE;
  TOOLCATEGORY tc;
  sprintf   (szKey,"%s_PPFont",pszKey);
  PfReadItem(slot, szKey, szPPFont, sizeof(szPPFont), "", 0);
  for(nKey=0; nKey<MAXTOOLS; nKey++)
  {
    sprintf(szKey,"%s%u_",pszKey,nKey);
    if (!PfQueryItemSize (slot,szKey)) break;
    if (!tc.RProfile(slot,szKey))      continue;
    if (!tc.szName[0])                 continue;
    for (nTarg=-1, i=0; i<MAXTOOLS; i++)  // nTarg := номер категории
    {
      if (!sf_stricmp(tc.szName, apToolCategories[i]->szName))
      {
        nTarg = i;
        break;
      }
      else if (nTarg<0 && !apToolCategories[i]->szName[0]) nTarg = i;
    }
    if (nTarg<0) // нет свободных категорий - переполнение
    {
      fRC = FALSE;
      break;
    }
    apToolCategories[nTarg]->Apply(&tc);
  }
  reFill_controls(FALSE);
  return fRC;
}



void  TOOLBOX::WProfile(LONG slot, PSZ pszKey)
{
  char szKey[20];
  sprintf    (szKey,"%s_PPFont",pszKey);
  PfWriteItem(slot, szKey, szPPFont, strlen(szPPFont)+1);
  for(int i=0; i<MAXTOOLS; i++)
  {
    sprintf(szKey,"%s%u_",pszKey,i);
    apToolCategories[i]->WProfile(slot,szKey);
  }
}

void TOOLBOX::del_controls(BOOL fMenu)
{
  static HWND hmTools = 0;
  if (!hmTools)
  {
    MENUITEM mi;
    HWND     hmMain = WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU);
    if (!WinSendMsg(hmMain,MM_QUERYITEM,MPFROM2SHORT(IDM_TOOLS,TRUE),(MPARAM)&mi)) return;
    if (!(hmTools = mi.hwndSubMenu)) return;
  }
  ULONG aulCmd[MAXTOOLS];
  memset(aulCmd,0,sizeof(aulCmd));
  for (int i=0; i<MAXTOOLS; i++)
  {
    if (fMenu) WinSendMsg(hmTools,MM_DELETEITEM,MPFROM2SHORT(i+TOOL_MIN_ID,TRUE),0);
    aulCmd[i] = i+TOOL_MIN_ID;
  }
  if (fMenu) WinSendMsg(hmTools,MM_DELETEITEM,MPFROM2SHORT(IDM_TOOL_SEPARATOR,TRUE),0);
  else       pToolbar->RemoveMultButton(aulCmd,MAXTOOLS);
}

void TOOLBOX::reFill_controls(BOOL fMenu)
{
  static HWND     hmTools;
  static MENUITEM mi;
  if (!hmTools)
  {
    HWND hmMain = WinWindowFromID(pMDI->QueryWindow(MDIQW_MAINFRAME),FID_MENU);
    if (!WinSendMsg(hmMain,MM_QUERYITEM,MPFROM2SHORT(IDM_TOOLS,TRUE),(MPARAM)&mi)) return;
    if (!(hmTools = mi.hwndSubMenu)) return;
  }
  del_controls(fMenu);

  SymTable        st;
  int             i;
  PSZ             pszItem;
  BOOL            fEnabled;
  TOOLBAR::NEWBTN aBtns[MAXTOOLS];
  int             nBtns=0;
  memset(aBtns+1,0,sizeof(aBtns)-sizeof(aBtns[0]));
  memset(&mi,0,sizeof(mi));
  mi.iPosition   = MIT_END;
  mi.afStyle     = MIS_SEPARATOR;
  mi.id          = IDM_TOOL_SEPARATOR;
  for (i=0; i<MAXTOOLS; i++)
  {
    if (!apToolCategories[i]->szName[0])    continue;
    fEnabled = apToolCategories[i]->IsEnabled();
    if (fMenu)
    {
      if (!apToolCategories[i]->szMenuItem[0] || !(pszItem = OpenExprVar(apToolCategories[i]->szMenuItem))) continue;
      if (mi.afStyle == MIS_SEPARATOR) WinSendMsg(hmTools,MM_INSERTITEM,MPARAM(&mi),MPARAM(""));
      mi.afStyle     = MIS_TEXT;
      mi.id          = TOOL_MIN_ID+i;
      mi.afAttribute = fEnabled ? 0 : MIA_DISABLED;
      WinSendMsg(hmTools,MM_INSERTITEM,MPARAM(&mi),MPARAM(pszItem));
      free(pszItem);
    }
    else
    {
      if (!apToolCategories[i]->usBmp) continue;
      aBtns[nBtns].hOwner     = hMDI0Client;
      aBtns[nBtns].ulCmd      = TOOL_MIN_ID+i;
      aBtns[nBtns].ulBmp      = apToolCategories[i]->usBmp;
      aBtns[nBtns].rgbBack    = 0x0;
      aBtns[nBtns].fEnabled   = fEnabled;
      aBtns[nBtns].pszComment = fEnabled ? apToolCategories[i]->szComment : "";
      nBtns++;
    }
  }
  if (!fMenu) pToolbar->InsertMultButton (aBtns, nBtns, 0);
}

PSZ Mp2Txt(PMPARAM pm1, PMPARAM pm2)
// In:  WM_CHARовские mparamы
// Out: текст с описанием кнопки и канонизированные mparamы ИЛИ 0 и нулевые mparamы
// WM_CHAR's mparams -> key description (0 when can not translate)
{
  static char   aszKeys[][15] =
  {
    "Break",      "Backspace",  "Tab",        "BackTab",
    "Enter",      "",           "",           "",
    "AltGraf",    "Pause",      "",           "Escape",
    "Space",      "PageUp",     "PageDown",   "End",
    "Home",       "Left",       "Up",         "Right",
    "Down",       "PrintScreen","Insert",     "Delete",
    "Scrollock",  "NumLock",    "",           "SysReq",
    "F1",  "F2",  "F3",  "F4",  "F5",  "F6",  "F7",
    "F8",  "F9",  "F10", "F11", "F12"
  };
  static char sz [40];
  sz[0]          = 0;
  USHORT fsf     = SHORT1FROMMP(*pm1);
  UCHAR  chch    = CHAR1FROMMP (*pm2);
  USHORT usvk    = SHORT2FROMMP(*pm2);
  BOOL   fDone   = FALSE;
  if (!*pm1 && !*pm2 || (fsf & KC_KEYUP)) return 0;
  sprintf (sz,"%s%s%s", (fsf & KC_ALT  ) ? "Alt+"  :"",
                        (fsf & KC_CTRL ) ? "Ctrl+" :"",
                        (fsf & KC_SHIFT) ? "Shift+":"" );
  static int  aGrays[] = {0x5c,0x4e,0x4a,0x37,0x53,0x52,0x4f,0x50,0x51,0x4b,0x4c,0x4d,0x47,0x48,0x49,0x5a,0};
  static PSZ  szGrays  = "/+-*.0123456789";
  if (fsf & KC_SCANCODE)
  {
    UCHAR scan = CHAR4FROMMP(*pm1);
    for (int i=0; aGrays[i];i++)
      if (aGrays[i]==scan)
      {
        if (i<strlen(szGrays))  sprintf(sz+strlen(sz),"Gray%c",szGrays[i]);
        else                    strcpy (sz+strlen(sz),"GrayEnter");
        fsf  &= ~(KC_CHAR|KC_VIRTUALKEY);
        *pm1  = MPFROM2SHORT(fsf,SHORT2FROMMP(*pm1)); // Только scancode
        fDone = TRUE;
        break;
      }
  }
  if (fsf & KC_VIRTUALKEY && !fDone && usvk>=VK_BREAK && usvk<=VK_F12 && aszKeys[usvk-VK_BREAK][0])
  {
    strcat(sz, aszKeys[usvk-VK_BREAK]);
    fDone = TRUE;
  }
  if (!fDone && chch)
  {
    chch = sf_toupper(chch);
    *pm2 = MPARAM(ULONG(*pm1) & 0xffffff00 | chch);
    sprintf(sz+strlen(sz),"'%c'",chch);
    fsf  |= KC_CHAR;
    *pm1  = MPFROM2SHORT(fsf,SHORT2FROMMP(*pm1)); // (KC_CHAR на ctrl не приходит)
    fDone = TRUE;
  }
  if (!fDone)
  {
    sz[0] = 0;
    *pm1  = *pm2 = 0;
    return 0;
  }
  return sz;
}
MRESULT EXPENTRY wpQueryAccel(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  // WindowULong (output):
  //  QWL_USER+0: m1  // 0 -  No accel.
  //  QWL_USER+4: m2  // 0 -      defined
  //
  // WM_USER   (BOOL m1) - enable EF window
  // WM_USER+1 (m1,m2)   == WM_CHAR w/o executing controls
  // Notification: sends WM_CONTRL (EN_CHANGE) to its owner

  HWND hEF = (HWND)WinQueryWindowULong(hWnd,QWL_USER+8);

  switch(ulMsg)
  {
  case WM_CREATE:
  {
    WinDefWindowProc(hWnd,ulMsg,m1,m2);
    hEF = WinCreateWindow(hWnd, WC_ENTRYFIELD, "", ES_LEFT|ES_MARGIN|ES_READONLY|WS_VISIBLE,
                          0,0,5,5, hWnd, HWND_TOP, 0,0,NULL);
    WinSetWindowULong(hWnd,QWL_USER+8,ULONG(hEF));
    WinSetWindowULong(hWnd,QWL_USER+0,0L);
    WinSetWindowULong(hWnd,QWL_USER+4,0L);
  }
  case WM_SIZE:
  {
    SWP  swp;
    WinQueryWindowPos(hWnd,&swp);
    WinSetWindowPos(hEF,HWND_TOP,4,3,max(swp.cx-7l,0),max(swp.cy-7l,0),SWP_SIZE|SWP_MOVE|SWP_ZORDER);
    break;
  }
  case WM_CHAR:
     if(SHORT2FROMMP(m2)==VK_ENTER   ||
        SHORT2FROMMP(m2)==VK_NEWLINE ||
        SHORT2FROMMP(m2)==VK_ESC     ||
        SHORT1FROMMP(m1)&KC_KEYUP )
      return WinSendMsg(WinQueryWindow(hWnd,QW_OWNER),ulMsg,m1,m2);
  case WM_USER+1:
  {
    PSZ szKey = Mp2Txt(&m1, &m2);
    WinSetWindowText(hEF,szKey ? szKey : "");
    WinSetWindowULong(hWnd,QWL_USER+0, ULONG(m1));
    WinSetWindowULong(hWnd,QWL_USER+4, ULONG(m2));
    {// Notify
      HWND hOwner = WinQueryWindow(hWnd,QW_OWNER);
      if (hOwner)
      {
        WinSendMsg(hOwner,WM_CONTROL,MPFROM2SHORT(WinQueryWindowUShort(hWnd,QWS_ID),EN_CHANGE),MPARAM(hWnd));
        if (ulMsg==WM_CHAR && szKey) // Pass focus
          WinSendMsg(hOwner,WM_CHAR,m1,MPFROM2SHORT(SHORT1FROMMP(m2),VK_TAB));
      }
    }
    break;
  }
  case WM_USER:
    WinEnableWindow(hEF,!!m1);
    break;
//  case WM_PAINT:
//    WinEndPaint(WinBeginPaint(hWnd,0,0));
//    WinInvalidateRect(hEF,0,0);
//    break;
  default:
    return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}
void Acc2Mp(const ACCEL *pAcc, PMPARAM pm1, PMPARAM pm2)
{
  USHORT usFs  = pAcc->fs;
  USHORT usKey = pAcc->key;
  USHORT fsf   = ((usFs & AF_SHIFT)?KC_SHIFT:0) | ((usFs & AF_ALT)?KC_ALT:0) | ((usFs & AF_CONTROL)?KC_CTRL:0);
  UCHAR  scan  = 0;
  USHORT usch  = 0;
  USHORT usvk  = 0;
  if (usFs & AF_CHAR)
  {
    fsf |= KC_CHAR;
    usch = usKey;
  }
  else if (usFs & AF_VIRTUALKEY)
  {
    fsf |= KC_VIRTUALKEY;
    usvk = usKey;
  }
  else if (usFs & AF_SCANCODE)
  {
    fsf |= KC_SCANCODE;
    scan = usKey;
  }
  *pm1 = MPFROM2SHORT(fsf, USHORT(scan)<<8 | 0x0001);
  *pm2 = MPFROM2SHORT(usch,usvk);
}
void Mp2Acc(MPARAM m1, MPARAM m2, PACCEL pAcc)
{
  memset(pAcc,0,sizeof(ACCEL));
  USHORT fsf  = SHORT1FROMMP(m1);
  if (!m1 && !m2 || fsf & KC_KEYUP) return;
  USHORT usFs  = ((fsf & KC_SHIFT)?AF_SHIFT:0) | ((fsf & KC_ALT)?AF_ALT:0) | ((fsf & KC_CTRL)?AF_CONTROL:0);
  USHORT usKey = 0;
  if (fsf & KC_SCANCODE)
  {
    // Grays: scancodes only
    static int  aGrays[] = {0x5c,0x4e,0x4a,0x37,0x53,0x52,0x4f,0x50,0x51,0x4b,0x4c,0x4d,0x47,0x48,0x49,0x5a,0};
    UCHAR       scan     = CHAR4FROMMP(m1);
    for (int i=0; aGrays[i]; i++)
      if (aGrays[i]==scan)
      {
        fsf  &= ~(KC_CHAR|KC_VIRTUALKEY);
        m2    = 0;
        break;
      }
  }
  if (CHAR1FROMMP(m2) && ((fsf&(KC_CTRL|KC_VIRTUALKEY))==KC_CTRL)) fsf |= KC_CHAR;
  if (fsf & KC_VIRTUALKEY)
  {
    usFs |= AF_VIRTUALKEY;
    usKey = CHAR3FROMMP(m2);
  }
  else if (fsf & KC_CHAR)
  {
    usFs |= AF_CHAR;
    usKey = CHAR1FROMMP(m2);
  }
  else if (fsf & KC_SCANCODE)
  {
    usFs |= AF_SCANCODE;
    usKey = CHAR4FROMMP(m1);
  }
  pAcc->fs  = usFs;
  pAcc->key = usKey;
}

MRESULT EXPENTRY dpToolCfgDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  #define MM_TCONF_STOREFIELDS    WM_USER+0
  #define MM_TCONF_DISABLEFIELDS  WM_USER+1
  #define MM_TCONF_SHOWCATFIELDS  WM_USER+2
  #define MM_TCONF_SHOWTOOLFIELDS WM_USER+3
  #define MM_TCONF_GETCURTOOL     WM_USER+4 // return **TOOL or 0
  static HWND     hLBTCats;
  static HWND     hEFTCatMenu;
  static HWND     hQATCatHotKey;
  static HWND     hEFTCatComment;

  static HWND     hLBTools;
  static HWND     hEFToolFNameRQ;
  static HWND     hMLEToolScript;
  static BOOL     fLockWmControl;
  static BOOL     fWM_CharFlag = FALSE;
  static PTOOLBAR pTB;
  static PTOOLBOX ptbEd;
  static int      nSelCat  = 0;
  static int      nSelTool;
  switch(ulMsg){
  case WM_INITDLG:
  {
    CenterChildWindow(hWnd);
    hLBTCats       = WinWindowFromID(hWnd,IDD_LBTCATS);
    hEFTCatMenu    = WinWindowFromID(hWnd,IDD_EFTCATMENU);
    hEFTCatComment = WinWindowFromID(hWnd,IDD_EFTCATCOMMENT);
    hLBTools       = WinWindowFromID(hWnd,IDD_LBTOOLS);
    hEFToolFNameRQ = WinWindowFromID(hWnd,IDD_EFTOOLFNAMERQ);
    hMLEToolScript = WinWindowFromID(hWnd,IDD_MLETOOLSCRIPT);
    WinSendMsg(hMLEToolScript,MLM_FORMAT,MPARAM(MLFIE_NOTRANS),0);
    fLockWmControl = FALSE;
    fWM_CharFlag   = FALSE;
    nSelTool       = LIT_NONE;

    {// Create hotkey entry window:
      SWP  swp;
      HWND hST      = WinWindowFromID(hWnd,IDD_STEFTCATHOTKEY);
      WinQueryWindowPos(hST,&swp);
      hQATCatHotKey = WinCreateWindow(hWnd, WC_QUERYACCEL, "", WS_VISIBLE,
                                      swp.x, swp.y, swp.cx, swp.cy,hWnd,
                                      HWND_TOP, IDD_STEFTCATHOTKEY, 0, 0);
    }

    {// Create toolbar:
      BOOL fRet;
      HWND hST = WinWindowFromID(hWnd,IDD_STBTNSHOW);
      pTB = new TOOLBAR(&fRet, hAB, hST, hWnd, X_TBBUTTON_SIZE, Y_TBBUTTON_SIZE, 0);
      if (!fRet) {WinDismissDlg(hWnd,0); break;}
      pTB->SetAlignment(TOOLBAR::TBA_CENTER);
    }

    ptbEd = new TOOLBOX;
    ptbEd->Copy(pToolbox);
    for (int i=0; i<MAXTOOLS; i++)
    {
      if (ptbEd->apToolCategories[i]->szName[0])
        WinSendMsg(hLBTCats,LM_INSERTITEM,MPARAM(i),MPARAM(ptbEd->apToolCategories[i]->szName));
      else break;
    }

    {// Initial selection:
      int nSel = nSelCat;
      WinSendMsg(hWnd,MM_TCONF_DISABLEFIELDS,MPARAM(TRUE),0);
      if ((nSel==LIT_NONE) || !WinSendMsg(hLBTCats,LM_SELECTITEM,MPARAM(nSel),MPARAM(TRUE)))
        WinSendMsg(hLBTCats,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
    }
    break;
  }
  case WM_CONTROL:
    if (fLockWmControl) break;
    if (m1==MPFROM2SHORT(IDD_LBTCATS,LN_SELECT) || m1==MPFROM2SHORT(IDD_LBTCATS,LN_SETFOCUS))
    {
      WinSendMsg(hWnd,MM_TCONF_STOREFIELDS,0,0); // Сохранить установки если они есть
      int i  = (int)WinSendMsg(hLBTCats,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      if (i<MAXTOOLS && nSelCat!=i)
      {
        nSelCat  = i;
        nSelTool = LIT_NONE;
        WinSendMsg(hWnd,MM_TCONF_SHOWCATFIELDS,0,0);
        // Re-fill tool list:
        WinSendMsg(hLBTools,LM_DELETEALL,0,0);
        PTOOLCATEGORY ptc = (ULONG(nSelCat)<MAXTOOLS) ? ptbEd->apToolCategories[nSelCat] : NULL;
        if (!ptc || !ptc->pToolList)
          WinSendMsg(hWnd,MM_TCONF_DISABLEFIELDS,MPARAM(FALSE),0);
        else
        {
          for (PTOOL pt=ptc->pToolList; pt; pt=pt->next)
            WinSendMsg(hLBTools,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(pt->szName));
        }
        if (!WinSendMsg(hLBTools,LM_SELECTITEM,MPARAM(ptc->nLastEdTool),MPARAM(TRUE)))
        {
          ptc->nLastEdTool = 0;
          WinSendMsg(hLBTools,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
        }
      }
    }
    else if (m1==MPFROM2SHORT(IDD_LBTOOLS,LN_SELECT))
    {
      WinSendMsg(hWnd,MM_TCONF_STOREFIELDS,0,0); // Сохранить установки если они есть
      nSelTool = (int)WinSendMsg(hLBTools,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
      if (ULONG(nSelCat)<MAXTOOLS && WinSendMsg(hWnd,MM_TCONF_GETCURTOOL,0,0)) // Exists?
        ptbEd->apToolCategories[nSelCat]->nLastEdTool = nSelTool;
      else
        nSelTool = LIT_NONE;
      WinSendMsg(hWnd,MM_TCONF_SHOWTOOLFIELDS,0,0);
    }
    break;
  case MM_TCONF_STOREFIELDS:  // Сохранить установки
  {
    if (ULONG(nSelCat)>=MAXTOOLS) break;
    // Tool category:
    PTOOLCATEGORY ptc = ptbEd->apToolCategories[nSelCat];
    WinQueryWindowText(hEFTCatMenu,    sizeof(ptc->szMenuItem), ptc->szMenuItem);
    {
      ptc->rAccel.fs = 0;
      MPARAM m1 = (MPARAM)WinQueryWindowULong(hQATCatHotKey,QWL_USER+0);
      MPARAM m2 = (MPARAM)WinQueryWindowULong(hQATCatHotKey,QWL_USER+4);
      if (m1||m2) Mp2Acc(m1,m2,&ptc->rAccel);
    }
    WinQueryWindowText(hEFTCatComment, sizeof(ptc->szComment),  ptc->szComment);

    // Tool:
    PTOOL *ppt = (PTOOL*)WinSendMsg(hWnd,MM_TCONF_GETCURTOOL,0,0);
    if (!ppt) break;
    PTOOL  pt  = *ppt;
    pt->fRunWhenProject = WinQueryButtonCheckstate(hWnd,IDD_CBTOOLPRJRQ);
    WinQueryWindowText(hEFToolFNameRQ, sizeof(pt->szMatchFiles), pt->szMatchFiles);
    {
      // Get script:
      LONG l   = WinQueryWindowTextLength(hMLEToolScript);
      PSZ  psz = (PSZ)malloc(l+1);
      if (psz)
      {
        WinQueryWindowText(hMLEToolScript,l+1,psz);
        free(pt->pszToolScript);
        pt->pszToolScript = psz;
      }
    }
    break;
  }
  case MM_TCONF_DISABLEFIELDS:
  // m1 = FALSE - only the tool fields;
  //      TRUE  - both the category and the tool fields
  {
    static USHORT ausCat[]  = {IDD_PBMVTCATUP,   IDD_PBMVTCATDOWN, IDD_EFTCATMENU,  IDD_EFTCATCOMMENT,
                               IDD_PBTCATDUP,    IDD_PBTCATREN,    IDD_PBTCATREM,   IDD_PBBTNCHANGE,
                               IDD_LBTOOLS,      0};
    static USHORT ausTool[] = {IDD_PBMVTOOLUP,    IDD_PBMVTOOLDOWN, IDD_EFTOOLFNAMERQ, IDD_CBTOOLPRJRQ,
                               IDD_PBTOOLDUP,     IDD_PBTOOLREN,    IDD_PBTOOLREM,     IDD_PBTOOLWIZ,
                               IDD_MLETOOLSCRIPT, 0};
    int   i;

    if (m1)
    {
      ULONG ulCmd = IDD_PBBTNCHANGE;
      pTB->RemoveMultButton(&ulCmd,1);
      for(i=0; ausCat[i]; i++)
        WinEnableControl(hWnd,ausCat[i],FALSE);
        WinSetWindowText(hEFTCatMenu,   "");
        WinSendMsg(hQATCatHotKey,WM_USER,MPARAM(FALSE),0);
        WinSendMsg(hQATCatHotKey,WM_CHAR, 0, 0);
        WinSetWindowText(hEFTCatComment,"");
        nSelCat = LIT_NONE;
    }

    for(i=0; ausTool[i]; i++)
      WinEnableControl(hWnd,ausTool[i],FALSE);
    WinSetWindowText(hEFToolFNameRQ,"");
    WinSetWindowText(hMLEToolScript,"");
    break;
  }
  case MM_TCONF_SHOWCATFIELDS:
  // Show nSelCat category fields: if it is valid - changes "Tool category" box only,
  //                               else - free and disable all the dialog fields
  {
    PTOOLCATEGORY ptc = (ULONG(nSelCat)<MAXTOOLS) ? ptbEd->apToolCategories[nSelCat] : NULL;
    if (!ptc)
      WinSendMsg(hWnd,MM_TCONF_DISABLEFIELDS,MPARAM(TRUE),0);
    else
    {
      // Fill tool category fields:
      static USHORT ausCat[]  = {IDD_PBMVTCATUP,   IDD_PBMVTCATDOWN, IDD_EFTCATMENU,  IDD_EFTCATCOMMENT,
                                 IDD_PBTCATDUP,    IDD_PBTCATREN,    IDD_PBTCATREM,   IDD_PBBTNCHANGE,
                                 IDD_LBTOOLS,      0};
      for(int i=0; ausCat[i]; i++)
        WinEnableControl(hWnd,ausCat[i],TRUE);
      WinSendMsg(hQATCatHotKey,WM_USER,MPARAM(TRUE),0);
      BOOL fSpace = !ptbEd->apToolCategories[MAXTOOLS-1]->szName[0];
      WinEnableControl(hWnd,IDD_PBTOOLDUP, fSpace);
      WinEnableControl(hWnd,IDD_PBTOOLADD, fSpace);
      WinEnableControl(hWnd,IDD_PBMVTCATUP,   !!nSelCat);
      WinEnableControl(hWnd,IDD_PBMVTCATDOWN, nSelCat+1 < (int)WinSendMsg(hLBTCats,LM_QUERYITEMCOUNT,0,0));
      WinSetWindowText(hEFTCatMenu,    ptc->szMenuItem);
      {
        MPARAM m1,m2;
        Acc2Mp(&ptc->rAccel,&m1,&m2);
        WinSendMsg(hQATCatHotKey, WM_USER+1, m1, m2);
      }
      WinSetWindowText(hEFTCatComment, ptc->szComment);
      {// Button:
        ULONG ulCmd = IDD_PBBTNCHANGE;
        pTB->RemoveMultButton(&ulCmd,1);

        if (ptc->usBmp)
        {
          TOOLBAR::NEWBTN nbtn;
          memset(&nbtn,0,sizeof(nbtn));
          nbtn.hOwner     = hWnd;
          nbtn.ulCmd      = IDD_PBBTNCHANGE;
          nbtn.ulBmp      = ptc->usBmp;
          nbtn.rgbBack    = 0x0;
          nbtn.fEnabled   = TRUE;
          nbtn.pszComment = "Change the button.";
          pTB->InsertMultButton(&nbtn,1,0);
        }
      }
    }
    break;
  }
  case MM_TCONF_SHOWTOOLFIELDS:
  // Show nSelCat->nSelTool tool fields: if it is valid - changes "Tool in the category" box only,
  //                                     else - free and disable tool fields
  {
    PTOOLCATEGORY ptc  = (ULONG(nSelCat)<MAXTOOLS) ? ptbEd->apToolCategories[nSelCat] : NULL;
    PTOOL         pt   = NULL;
    int           i;

    if (ptc)
      for (i=0,pt=ptc->pToolList;   pt && i!=nSelTool;   i++,pt=pt->next);

    if (!pt)
      WinSendMsg(hWnd,MM_TCONF_DISABLEFIELDS,MPARAM(FALSE),0);
    else
    {
      // Fill tool fields:
      static USHORT ausTool[] = {IDD_PBMVTOOLUP,    IDD_PBMVTOOLDOWN, IDD_EFTOOLFNAMERQ, IDD_CBTOOLPRJRQ,
                                 IDD_PBTOOLDUP,     IDD_PBTOOLREN,    IDD_PBTOOLREM,     IDD_PBTOOLWIZ,
                                 IDD_MLETOOLSCRIPT, 0};
      for(int i=0; ausTool[i]; i++)
        WinEnableControl(hWnd,ausTool[i],TRUE);

      WinCheckButton(hWnd,IDD_CBTOOLPRJRQ, !!pt->fRunWhenProject);
      WinSetWindowText(hEFToolFNameRQ,pt->szMatchFiles);
      WinSetWindowText(hMLEToolScript,pt->pszToolScript ? pt->pszToolScript : "");
      WinEnableControl(hWnd,IDD_PBMVTOOLUP,   !!nSelTool);
      WinEnableControl(hWnd,IDD_PBMVTOOLDOWN, nSelTool+1 < (int)WinSendMsg(hLBTools,LM_QUERYITEMCOUNT,0,0));
    }
    break;
  }
  case MM_TCONF_GETCURTOOL: // return **TOOL or 0
  {
    if (ULONG(nSelCat)>=MAXTOOLS) return 0;
    PTOOL *ppt = &ptbEd->apToolCategories[nSelCat]->pToolList;
    for (int i=nSelTool; i && *ppt; i--, ppt=&((*ppt)->next));
    return MRESULT(*ppt ? ppt : 0);
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
  {
    WinSendMsg(hWnd,MM_TCONF_STOREFIELDS,0,0);
    SHORT s1m1 = SHORT1FROMMP(m1);
    switch(s1m1){
    //--- Tool category commands:
    case IDD_PBMVTCATUP:
    case IDD_PBMVTCATDOWN:
    {
      ULONG nIt1 = nSelCat + ((s1m1==IDD_PBMVTCATUP) ? -1 : 0); // swap (nIt1, nIt1+1)
      if (nSelCat==LIT_NONE || nIt1+1>=(int)WinSendMsg(hLBTCats,LM_QUERYITEMCOUNT,0,0))
        break;
      fLockWmControl = TRUE;
      WinSendMsg(hLBTCats, LM_DELETEITEM, MPARAM(nIt1), 0);
      PTOOLCATEGORY ptc                = ptbEd->apToolCategories[nIt1];
      ptbEd->apToolCategories[nIt1]    = ptbEd->apToolCategories[nIt1+1];
      ptbEd->apToolCategories[nIt1+1]  = ptc;
      WinSendMsg(hLBTCats,LM_INSERTITEM,MPARAM(nIt1+1),MPARAM(ptc->szName));
      WinSendMsg(hLBTCats,LM_SELECTITEM,MPARAM(nIt1+((s1m1==IDD_PBMVTCATUP) ? 0 : 1)),MPARAM(TRUE));
      fLockWmControl = FALSE;
      nSelCat = LIT_NONE;
      WinSendMsg(hWnd,WM_CONTROL, MPFROM2SHORT(IDD_LBTCATS,LN_SELECT),0);
      break;
    }
    case IDD_PBTCATADD:
    case IDD_PBTCATREN:
    {
      char          szName[TOOLNAMELEN] = "";
      CPARAMSTR     cps;
      BOOL          fRen  = (s1m1==IDD_PBTCATREN);
      ULONG         nTarg = nSelCat;

      if (fRen)
      {
        if (nTarg >= MAXTOOLS)
          {Beep(); break;}
        strcpy(szName, ptbEd->apToolCategories[nTarg]->szName);
        cps.psz1 = "Rename tool category";
      }
      else
      {
        if (ptbEd->apToolCategories[MAXTOOLS-1]->szName[0])
          {Beep(); break;}
        cps.psz1 = "Add tool category";
      }
      cps.psz2  = "Name:";
      cps.psz3  = szName;
      cps.l1    = sizeof(szName);
      if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
      {
        PSZ psz = sf_skipspaces(szName);
        if (!*psz)
        {
          MessageBox(hWnd, fRen ? "Rename tool category" : "Add tool category", MB_ERROR|MB_OK,
                     "Error: You must specify the tool category name.");
          return 0;
        }
        for (int i=0; i<MAXTOOLS; i++)
          if (!sf_stricmp(psz,ptbEd->apToolCategories[i]->szName))
          {
            if (MBID_OK != MessageBox(hWnd, fRen ? "Rename tool category" : "Add tool category", MB_WARNING|MB_OKCANCEL,
                                      "Note: The name is belonged to another tool category."))
              return 0;
            else
              break; //for
          }
        // Ok. Rename or Add it.
        if (fRen)
        {
          strcpy(ptbEd->apToolCategories[nTarg]->szName, psz);
          WinSendMsg(hLBTCats,LM_SETITEMTEXT,MPARAM(nTarg),MPARAM(psz));
        }
        else
        {
          PTOOLCATEGORY ptc = ptbEd->apToolCategories[MAXTOOLS-1];
          if (++nTarg >= MAXTOOLS) nTarg = 0;
          memmove(ptbEd->apToolCategories + nTarg + 1,
                  ptbEd->apToolCategories + nTarg,
                  (MAXTOOLS-nTarg-1) * sizeof(ptbEd->apToolCategories[0]));
          ptbEd->apToolCategories[nTarg] = ptc;
          ptc->free_all();
          strcpy(ptc->szName,psz);
          WinSendMsg(hLBTCats,LM_INSERTITEM,MPARAM(nTarg),MPARAM(ptc->szName));
          WinSendMsg(hLBTCats,LM_SELECTITEM,MPARAM(nTarg),MPARAM(TRUE));
        }
      }
      break;
    }
    case IDD_PBTCATDUP:
    {
      PTOOLCATEGORY ptc    = ptbEd->apToolCategories[MAXTOOLS-1];
      if (ULONG(nSelCat+1) >= MAXTOOLS || ptc->szName[0]) {Beep(); break;}
      PTOOLCATEGORY ptcSrc = ptbEd->apToolCategories[nSelCat];

      memmove(ptbEd->apToolCategories + nSelCat + 1,
              ptbEd->apToolCategories + nSelCat,
              (MAXTOOLS-1-nSelCat) * sizeof(ptbEd->apToolCategories[0]));


      ptc->free_all();
      ptc->Apply(ptcSrc);
      strcpy(ptc->szName,ptcSrc->szName);
      strcpy(ptc->szComment,ptcSrc->szComment);
      ptbEd->apToolCategories[nSelCat+1] = ptc;

      WinSendMsg(hLBTCats,LM_INSERTITEM,MPARAM(nSelCat+1),MPARAM(ptc->szName));
      WinSendMsg(hLBTCats,LM_SELECTITEM,MPARAM(nSelCat+1),MPARAM(TRUE));
      break;
    }
    case IDD_PBTCATREM:
    {
      PTOOLCATEGORY ptc;
      char          szMsg[TOOLNAMELEN+200];
      if (ULONG(nSelCat)>=MAXTOOLS || !(ptc = ptbEd->apToolCategories[nSelCat])->szName[0] )
        {Beep(); break;}

      if (ptc->pToolList)
        sprintf(szMsg, "Tool category '%s' is not empty! Do you really want to remove it?",ptc->szName);
      else
        sprintf(szMsg, "Tool category '%s' contains no tools. Remove it?",ptc->szName);

      if (MBID_YES == MessageBox(hWnd, "Remove tool category", MB_YESNO | (ptc->pToolList ? MB_WARNING : MB_QUERY), szMsg))
      {
        PTOOLCATEGORY ptc = ptbEd->apToolCategories[nSelCat];
        ptc->free_all();
        memmove(ptbEd->apToolCategories + nSelCat,
                ptbEd->apToolCategories + nSelCat+1,
                (MAXTOOLS-nSelCat-1) * sizeof(ptbEd->apToolCategories[0]));
        ptbEd->apToolCategories[MAXTOOLS-1] = ptc;
        WinSendMsg(hLBTCats,LM_DELETEITEM,MPARAM(nSelCat),0);
        int nSel = nSelCat;
        nSelCat  = LIT_NONE;
        if (!WinSendMsg(hLBTCats,LM_SELECTITEM,MPARAM(nSel),MPARAM(TRUE)))
          if (nSel<1 || !WinSendMsg(hLBTCats,LM_SELECTITEM,MPARAM(nSel-1),MPARAM(TRUE)))
            WinSendMsg(hWnd,MM_TCONF_DISABLEFIELDS,MPARAM(TRUE),0);
      }
      break;
    }
    case IDD_PBBTNCHANGE:
      if (ULONG(nSelCat)<MAXTOOLS && WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpToolChBtnDlg,NULLHANDLE,
                                               IDD_TOOLCHBTNDLG,ptbEd->apToolCategories[nSelCat]))
        nSelCat = LIT_NONE;
        WinSendMsg(hWnd,WM_CONTROL, MPFROM2SHORT(IDD_LBTCATS,LN_SELECT),0);
      break;

    //--- Tool commands:
    case IDD_PBMVTOOLUP:
    case IDD_PBMVTOOLDOWN:
    {
      BOOL           fUp  = (s1m1==IDD_PBMVTOOLUP);
      PTOOL          pt1, *ppt;
      PTOOLCATEGORY  ptc  = (ULONG(nSelCat)<MAXTOOLS ? ptbEd->apToolCategories[nSelCat] : 0);
      int            nIt  = nSelTool - (fUp?1:0);
      int            i;
      if (!ptc || nIt<0 || nIt+1>=(int)WinSendMsg(hLBTools,LM_QUERYITEMCOUNT,0,0)) break;
      for (i=0,ppt=&(ptc->pToolList); *ppt && i!=nIt; ppt = &((*ppt)->next),i++);
      if (!*ppt || !(pt1=(*ppt)->next)) break;

      (*ppt)->next = pt1->next; // swap *ppt & (pt1==(*ppt)->next)
      pt1->next    = *ppt;      //
      *ppt         = pt1;       //

      nSelTool = LIT_NONE;
      WinSendMsg(hLBTools, LM_DELETEITEM, MPARAM(nIt), 0);
      WinSendMsg(hLBTools, LM_DELETEITEM, MPARAM(nIt), 0);
      WinSendMsg(hLBTools,LM_INSERTITEM,MPARAM(nIt),  MPARAM((*ppt)->szName));
      WinSendMsg(hLBTools,LM_INSERTITEM,MPARAM(nIt+1),MPARAM((*ppt)->next->szName));
      if (!WinSendMsg(hLBTools,LM_SELECTITEM,MPARAM(nIt+(fUp?0:1)),MPARAM(TRUE)))
        WinSendMsg(hWnd, MM_TCONF_DISABLEFIELDS, MPARAM(FALSE), 0);
      break;
    }
    case IDD_PBTOOLADD:
    case IDD_PBTOOLREN:
    {
      if (ULONG(nSelCat)>=MAXTOOLS) return 0;
      PTOOLCATEGORY ptc                 = ptbEd->apToolCategories[nSelCat];
      PTOOL        *ppt                 = (PTOOL*)WinSendMsg(hWnd,MM_TCONF_GETCURTOOL,0,0);
      BOOL          fRen                = (s1m1==IDD_PBTOOLREN);
      char          szName[TOOLNAMELEN] = "";
      CPARAMSTR     cps;

      if (!ppt)
        if (fRen) {Beep(); break;}
        else      ppt = &(ptc->pToolList);

      cps.psz1  = fRen ? "Rename tool" : "Add tool";
      cps.psz2  = "Name:";
      cps.psz3  = szName;
      cps.l1    = sizeof(szName);
      if (fRen) strcpy(szName,(*ppt)->szName);
      if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
      {
        PSZ psz = sf_skipspaces(szName);
        if (!*psz)
        {
          MessageBox(hWnd, fRen ? "Rename tool" : "Add tool", MB_ERROR|MB_OK,
                     "Error: You must specify the tool name.");
          return 0;
        }
        for (PTOOL pt1=ptbEd->apToolCategories[nSelCat]->pToolList; pt1; pt1=pt1->next)
          if (!sf_stricmp(psz,pt1->szName))
          {
            if (MBID_OK != MessageBox(hWnd, fRen ? "Rename tool" : "Add tool", MB_WARNING|MB_OKCANCEL,
                                      "Note: The name is belonged to another tool."))
              return 0;
            else
              break; //for
          }
        // Ok. Rename or Add this tool.
        if (fRen)
        {
          strcpy((*ppt)->szName, psz);
          WinSendMsg(hLBTools,LM_SETITEMTEXT,MPARAM(nSelTool),MPARAM(psz));
        }
        else
        {
          PTOOL ptNew = new TOOL;
          int   nIt   = (nSelTool==LIT_NONE) ? 0 : nSelTool+1;
          strcpy(ptNew->szName,psz);
          if (*ppt)
          {
            ptNew->next  = (*ppt)->next;
            (*ppt)->next = ptNew;
          }
          else
          {
            *ppt        = ptNew;
            ptNew->next = 0;
          }
          nSelTool = LIT_NONE;
          WinSendMsg(hLBTools,LM_INSERTITEM,MPARAM(nIt),MPARAM(ptNew->szName));
          WinSendMsg(hLBTools,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
        }
      }
      break;
    }
    case IDD_PBTOOLDUP:
    {
      PTOOL   pt, ptNew;
      PTOOL  *ppt = (PTOOL*)WinSendMsg(hWnd,MM_TCONF_GETCURTOOL,0,0);
      int     nIt = nSelTool+1;

      if (!ppt) {Beep(); break;}
      pt = *ppt;

      ptNew       = new TOOL;
      ptNew->Copy(pt);
      ptNew->next = pt->next;
      pt->next    = ptNew;
      nSelTool    = LIT_NONE;
      WinSendMsg(hLBTools,LM_INSERTITEM,MPARAM(nIt),MPARAM(ptNew->szName));
      WinSendMsg(hLBTools,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
      break;
    }
    case IDD_PBTOOLREM:
    {
      PTOOL  *ppt = (PTOOL*)WinSendMsg(hWnd,MM_TCONF_GETCURTOOL,0,0);
      char    szMsg[TOOLNAMELEN+200];

      if (!ppt) {Beep(); break;}

      sprintf(szMsg,"Do you really want to remove tool '%s'?",(*ppt)->szName);
      if (MBID_YES == MessageBox(hWnd, "Remove tool", MB_YESNO | MB_WARNING, szMsg))
      {
        int   nIt = nSelTool;
        PTOOL pt  = *ppt;
        *ppt      = pt->next;
        delete pt;
        WinSendMsg(hLBTools,LM_DELETEITEM,MPARAM(nIt),0);
        nSelTool = LIT_NONE;
        if (!WinSendMsg(hLBTools,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE)))
          if (!nIt || !WinSendMsg(hLBTools,LM_SELECTITEM,MPARAM(nIt-1),MPARAM(TRUE)))
            WinSendMsg(hWnd,MM_TCONF_DISABLEFIELDS,MPARAM(FALSE),0);
      }
      break;
    }
    case IDD_PBTOOLWIZ:
    {
      PTOOL *ppt = (PTOOL*)WinSendMsg(hWnd,MM_TCONF_GETCURTOOL,0,0);
      if (ppt && WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpToolWizDlg,NULLHANDLE,IDD_TOOLWIZDLG,*ppt))
      {
        nSelTool = LIT_NONE;
        WinSendMsg(hWnd,WM_CONTROL, MPFROM2SHORT(IDD_LBTOOLS,LN_SELECT),0);
      }
      break;
    }
    case DID_OK:
      pToolbox->Copy(ptbEd);
    case DID_CANCEL:
      delete ptbEd;
      delete pTB;
      WinDismissDlg(hWnd,s1m1==DID_OK);
      break;
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_TOOLCFGDLG);
      break;
    }
    break;
  }
  case WM_CHAR:
  {
    USHORT fsf = SHORT1FROMMP(m1);
    if (!(fsf & KC_KEYUP))
    {
      if(SHORT2FROMMP(m2)==VK_F1)
      {
        DisplayHelpPanel(PANEL_TOOLCFGDLG);
        break;
      }
      /*
      // Ctrl+Shift+'D', Ctrl+Shift+'F' - create DEFAULT value for this tool
      else if (sf_toupper(CHAR1FROMMP(m2)) == (fWM_CharFlag ? 'F' : 'D') &&
              (fsf & (KC_SHIFT|KC_CTRL)) == (KC_SHIFT|KC_CTRL))
      {
        if (!(fWM_CharFlag = !fWM_CharFlag) && ULONG(nSelItem)<MAXTOOLS)
        {
          PTOOL pt  = ptbEd->apTools[nSelItem];
          delete (pt->ptDefValue);
          (pt->ptDefValue = new TOOL)->Copy(pt);
          nSelItem  = LIT_NONE;
          WinSendMsg(hWnd,WM_CONTROL, MPFROM2SHORT(IDD_LBTOOLS,LN_SELECT),0);
        }
        break;
      }
      else
        fWM_CharFlag = FALSE;
      */
    }
  }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

MRESULT EXPENTRY dpToolChBtnDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PTOOLCATEGORY ptc;
  static PTOOLBAR pTB;
  static int      nMax;

  switch(ulMsg){
  case WM_INITDLG:
  {
    SWP  swp;
    BOOL fRet;
    HWND hST = WinWindowFromID(hWnd,IDD_STBTNSHOW);
    ptc      = PTOOLCATEGORY(m2);
    WinQueryWindowPos(hST,&swp);
    pTB = new TOOLBAR(&fRet, hAB, hST, hWnd, X_TBBUTTON_SIZE, Y_TBBUTTON_SIZE, 0);
    if (!fRet) {WinDismissDlg(hWnd,0); break;}
    pTB->SetAlignment(TOOLBAR::TBA_FILL);
    TOOLBAR::NEWBTN nbtn;
    nMax=0;
    while(1)
    {
      memset(&nbtn,0,sizeof(nbtn));
      nbtn.hOwner     = hWnd;
      nbtn.ulCmd      = TOOL_MIN_ID+nMax;
      nbtn.ulBmp      = TOOL_MIN_ID+nMax;
      nbtn.rgbBack    = 0x0;
      nbtn.fEnabled   = TRUE;
      nbtn.pszComment = "Set this button";
      if (!pTB->InsertMultButton(&nbtn,1,0)) break;
      nMax++;
    }
    if (!nMax)
      WinSetDlgItemText(hWnd,IDD_STBTNSIGN,"Sorry, there is no any button resources found (or a error occured).");
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
  {
    SHORT s1m1 = SHORT1FROMMP(m1);
    switch(s1m1)
    {
    case DID_CANCEL:
      WinDismissDlg(hWnd,0);
      break;
    case IDD_PBNOBTN:
      ptc->usBmp = 0;
      WinDismissDlg(hWnd,TRUE);
      break;
    case IDD_PBHELP:
      DisplayHelpPanel(PANEL_TOOLCHBTNDLG);
      break;
    case WM_CHAR:
      if(SHORT2FROMMP(m2)==VK_F1)
      {
        DisplayHelpPanel(PANEL_TOOLCHBTNDLG);
        break;
      }
    default:
      if (s1m1>=TOOL_MIN_ID && s1m1<TOOL_MIN_ID+nMax)
      {
        ptc->usBmp = s1m1;
        WinDismissDlg(hWnd,TRUE);
      }
      break;
    }
    break;
  }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


///// W I Z A R D * * * *

struct WIZSTR
{
  BOOL fPopup;
  BOOL fKeepSession;
  char szPopTitle[200];
  char szExecute [CCHMAXPATH*2];
  PSZ  pszToolTail;
};

char *get_pars(char *pch, PSZ szWhat)
{
  LONG l = strlen(szWhat);
  pch    = sf_skipspaces(pch);
  return strncmp(pch,szWhat,l) ? 0 : pch+l;
}

BOOL parse_wiz(HWND hWnd, PTOOL pTool, WIZSTR *pWS)
{
  try
  {
    PSZ   pszParse   = 0;
    memset(pWS,0,sizeof(*pWS));
    strcpy(pWS->szPopTitle,"My Tool");
    pWS->fKeepSession = TRUE;
    char *pch,*pchB,*pchE;
    if (!pTool->pszToolScript || !*(pch=sf_skipspaces(pTool->pszToolScript)))
      return TRUE;
    {// pszParse := Первая строка скрипта - только в ней будем искать команду
      int l = strcspn(pTool->pszToolScript,"\n\r");
      if (!(pszParse = (PSZ)malloc(l+1))) return FALSE;
      strncpy(pszParse,pTool->pszToolScript,l);
      pszParse[l] = 0;
    }
    //--- #Execute(
    if (!(pch=get_pars(pszParse,"Execute(")))
    {
      int i = (int)WinMessageBox(HWND_DESKTOP, hWnd,
                                 "Wizard can't parse this tool script ('Execute' expected) "
                                 "so it will insert an execution command before it.",
                                 NULL, 0, MB_ERROR|MB_MOVEABLE|MB_OKCANCEL);
      if (i!=MBID_OK) return FALSE;
      if (pWS->pszToolTail = (PSZ)malloc(strlen(pTool->pszToolScript)+3))
      {
        strcpy(pWS->pszToolTail+0, "\r\n");
        strcpy(pWS->pszToolTail+2, pTool->pszToolScript);
      }
      return TRUE;
    }

    //--- Execute( #"
    if (!(pch=get_pars(pch,"\"")))
      throw "Tool script contains an error in the 'Execute' command (quote expected). Wizard can not process it.";

    //--- Execute( "#<file cmd_line>        // могут быть двойные ""
    pchB=pch;
    do
    {
      if (!(pchE = strchr(pch,'\"')))
        throw "Tool script contains an error in the 'Execute' command (text is not terminated). Wizard can not process it.";
    }while(pchE[1]=='\"' && (pch=pchE+2));
    if (pchE-pchB+2 >= sizeof(pWS->szExecute))
      throw "Tool script contains an error in the 'Execute' command (text is too long). Wizard can not process it.";
    strncpy(pWS->szExecute,pchB,pchE-pchB);
    pWS->szExecute[pchE-pchB] = 0;
    pch=pchE+1;

    //--- Execute( "<file cmd_line>" #,
    if (!(pch=get_pars(pch,",")))
      throw "Tool script contains an error in the 'Execute' command (comma expected). Wizard can not process it.";

    if (pchB=get_pars(pch,"\"POPUP="))
    {
      pWS->fPopup = TRUE;
      pch=pchB;
      do
      {
        if (!(pchE = strchr(pch,'\"')))
          throw "Tool script contains an error in the 'Execute' command (text is not terminated). Wizard can not process it.";
      }while(pchE[1]=='\"' && (pch=pchE+2));
      int l = min(pchE-pchB,sizeof(pWS->szPopTitle));
      strncpy(pWS->szPopTitle,pchB,l);
      pWS->szPopTitle[l] = 0;
      pch = pchE+1;
    }
    else if (pchB=get_pars(pch,"\"SEPARATE="))
    {
      pch = pchB;
      pWS->fKeepSession = TRUE;
      do
      {
        if ((pchB=get_pars(pch,"/N")) || (pchB=get_pars(pch,"/n")))
        {
          pWS->fKeepSession = FALSE;
          pch = pchB;
        }
        else if ((pchB=get_pars(pch,"/F")) || (pchB=get_pars(pch,"/f")))
          pch = pchB;
      }while (pchB);
      if (!(pch=get_pars(pch,"\"")))
        throw "Tool script contains an error in the 'Execute' command (text is not terminated). Wizard can not process it.";
    }
    else
      throw "Tool script contains an error in the 'Execute' command (wrong 2nd argument). Wizard can not process it.";

    //--- Final );
    if (!(pch=get_pars(pch,")")))
      throw "Tool script contains an error in the 'Execute' command (')' expected). Wizard can not process it.";
    if (pchB=get_pars(pch,";")) pch = pchB;

    LONG l_tail = strlen(pTool->pszToolScript) - (pch-pszParse);
    if (pWS->pszToolTail = (PSZ)malloc(l_tail+1))
      strcpy(pWS->pszToolTail, pTool->pszToolScript + (pch-pszParse));
    return TRUE;
  }
  catch (char *sz)
  {
    WinMessageBox(HWND_DESKTOP,hWnd,sz,NULL,0,MB_ERROR|MB_MOVEABLE|MB_OK);
    return FALSE;
  }
}

MRESULT EXPENTRY dpToolWizDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static  PTOOL    pTool;
  static  HWND     hEFWTitle;
  static  HWND     hCBWExecute;
  static  WIZSTR   wizstr;
  #define EXE_PROJ "<Project>"

  switch(ulMsg){
  case WM_INITDLG:
  {
    memset(&wizstr,0,sizeof(wizstr));
    pTool       = (PTOOL)m2;
    if (!parse_wiz(WinQueryWindow(hWnd,QW_OWNER),pTool,&wizstr))
    {
      WinDismissDlg(hWnd,0);
      break;
    }
    ULONG  ulRB = wizstr.fPopup ? IDD_RBWPOPUP : IDD_RBWSEPARATE;
    hEFWTitle   = WinWindowFromID(hWnd,IDD_EFWTITLE);
    hCBWExecute = WinWindowFromID(hWnd,IDD_CBWEXECUTE);
    WinSetDlgItemText(hWnd,IDD_STWTOOLNAME,pTool->szName);
    WinSetWindowText (hEFWTitle,  wizstr.szPopTitle);
    WinSendMsg(hCBWExecute,LM_INSERTITEM,MPARAM(LIT_END),MPARAM(EXE_PROJ));
    if (wizstr.szExecute[0])
    {
      int nIt = (int)WinSendMsg(hCBWExecute,LM_INSERTITEM,MPARAM(0),MPARAM(wizstr.szExecute));
      WinSendMsg(hCBWExecute,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
    }
    WinCheckButton   (hWnd,ulRB,TRUE);
    WinCheckButton   (hWnd,IDD_CBWKEEPSESSION,wizstr.fKeepSession);
    WinSendMsg(hWnd,WM_CONTROL,MPARAM(ulRB),0); // Сам не приходит
    break;
  }
  case WM_CONTROL:
  {
    if (SHORT1FROMMP(m1)==IDD_RBWSEPARATE || SHORT1FROMMP(m1)==IDD_RBWPOPUP)
    {
      BOOL fPop = WinQueryButtonCheckstate(hWnd,IDD_RBWPOPUP);
      WinEnableWindow(hEFWTitle,fPop);
      WinEnableControl(hWnd,IDD_CBWUSEFILTER,   fPop);
      WinEnableControl(hWnd,IDD_PBWEDFILTER,    fPop);
      WinEnableControl(hWnd,IDD_CBWKEEPSESSION,!fPop);
      WinCheckButton  (hWnd,IDD_CBWUSEFILTER,  pTool->pszFilter && pTool->pszFilter[0]=='+');
    }
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
  {
    SHORT s1m1 = SHORT1FROMMP(m1);
    switch(s1m1)
    {
      case IDD_PBWBROWSE:
      {
        char szF[CCHMAXPATH];
        WinQueryWindowText(hCBWExecute,sizeof(szF),szF);
        if (!*sf_skipspaces(szF) || !strcmp(szF,EXE_PROJ))
          strcpy(szF,"*.exe");
        if (Browse(szF,szF,hWnd,"Execute file",FALSE))
          WinSetWindowText(hCBWExecute,szF);
        break;
      }
      case IDD_PBWEDFILTER:
      {
        MRESULT EXPENTRY dpFilterDlg(HWND, ULONG, MPARAM, MPARAM);
        WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpFilterDlg,NULLHANDLE,IDD_FILTERDLG,pTool);
        WinSendMsg(hWnd,WM_CONTROL,MPARAM(IDD_RBWPOPUP),0); // check IDD_CBWUSEFILTER
        break;
      }
      case DID_CANCEL:
        free(wizstr.pszToolTail);
        wizstr.pszToolTail = 0;
        WinDismissDlg(hWnd,0);
        break;
      case DID_OK:
      {
        char sz   [CCHMAXPATH*2];
        char szCmd[CCHMAXPATH*2+100];
        WinQueryWindowText(hCBWExecute,sizeof(sz)/2,sz);
        RxDupQuotes(sz);
        BOOL fPrj = !*sf_skipspaces(sz) || !strcmp(sz,EXE_PROJ);
        sprintf(szCmd,"Execute(\"%s\"", fPrj ? "$(ProjName).exe" : sz);
        if (WinQueryButtonCheckstate(hWnd,IDD_RBWPOPUP))
        {
          WinQueryWindowText(hEFWTitle,sizeof(sz)/2,sz);
          RxDupQuotes(sz);
          sprintf(szCmd+strlen(szCmd),",\"POPUP=%s\"",sz);
          //sprintf(szCmd+strlen(szCmd),",\"FILTER=???\"");
        }
        else
        {
          sprintf(szCmd+strlen(szCmd),",\"SEPARATE=%s /F\"",
            WinQueryButtonCheckstate(hWnd,IDD_CBWKEEPSESSION) ? "" : "/N");
        }
        strcat(szCmd,");");
        PSZ psz_tail = wizstr.pszToolTail;
        if (!psz_tail) psz_tail = "";
        PSZ psz = (PSZ)malloc(strlen(szCmd)+strlen(psz_tail)+1);
        if (psz)
        {
          strcpy(psz,szCmd);
          strcat(psz,psz_tail);
          free(pTool->pszToolScript);
          pTool->pszToolScript = psz;
        }
        if (pTool->pszFilter && pTool->pszFilter[0])
          pTool->pszFilter[0] = WinQueryButtonCheckstate(hWnd,IDD_CBWUSEFILTER) ? '+' : '-';
        if (fPrj) pTool->fRunWhenProject = TRUE;
        free(wizstr.pszToolTail);
        wizstr.pszToolTail = 0;
        WinDismissDlg(hWnd,TRUE);
        break;
      }
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_TOOLWIZDLG);
        break;
    }
    break;
  }
  case WM_DESTROY:
    free(wizstr.pszToolTail);
    wizstr.pszToolTail = 0;
    break;
  case WM_CHAR:
    if(SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_TOOLWIZDLG);
      break;
    }
  default:
    return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


MRESULT EXPENTRY dpFilterDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
#define SAMPLE_FILTER \
   ";; Error message example:\n"                                           \
   ";; E:\\TEMP\\zz.cpp(3:1) : informational EDC3100: \"i\" is set but not used in function \"main()\".\n" \
   "Filter       = \"*$1\\({0-9}$2:{0-9}$3\\) : {a-z}$4 {A-Z}{0-9}: *$5\"\n" \
   "File         = \"$1\"\n"   \
   "Line         = \"$2\"\n"   \
   "Position     = \"$3\"\n"   \
   "Message      = \"$5\"\n"   \
   "MessageKind  = \"$4\" \"error\" \"informational|warning\" \"---\" \"---\"  ; ( ERROR WARNING NOTICE MSG_SEVERE )\n"

  static  PTOOL    pTool;
  static  HWND     hMle;

  switch(ulMsg){
    case WM_INITDLG:
    {
      pTool  = (PTOOL)m2;
      hMle   = WinWindowFromID(hWnd,IDD_MLEFILTER);
      WinSendMsg(hMle,MLM_SETWRAP,MPARAM(FALSE),0);
      if (pTool->pszFilter)
        WinSetWindowText(hMle,pTool->pszFilter+1);
      WinSendMsg(hMle,MLM_SETSEL,MPARAM(0),MPARAM(0));
      break;
    }
    case WM_COMMAND:
    {
      SHORT s1m1 = SHORT1FROMMP(m1);
      switch(s1m1)
      {
        case IDD_PBINSFILTER:
          WinSendMsg(hMle,MLM_INSERT,MPARAM(SAMPLE_FILTER),0);
          break;
        case DID_OK:
        {
          int l   = WinQueryWindowTextLength(hMle)+1;
          PSZ psz = (PSZ)malloc(l+1);
          psz[0] = '+';
          if (psz)
          {
            WinQueryWindowText(hMle,l,psz+1);
            psz[l] = 0;
          }
          free(pTool->pszFilter);
          pTool->pszFilter = psz;
        }
        case DID_CANCEL:
          WinDismissDlg(hWnd,0);
          break;
        case IDD_PBHELP:
          DisplayHelpPanel(PANEL_TOOLEDFILTERDLG);
          break;
      }
      break;
    }
    case WM_CHAR:
      if(SHORT2FROMMP(m2)==VK_F1)
      {
        DisplayHelpPanel(PANEL_TOOLEDFILTERDLG);
        break;
      }
    default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}



char *add_env_var(PSZ szName,PSZ szValue)
// returns: malloc()ed envinronment variables = current variables + the var. specifyed
// (0 when error)
{
  PPIB   ppib;
  PTIB   ptib;
  DosGetInfoBlocks(&ptib,&ppib);
  char  *pch, *pchEnv = ppib->pib_pchenv ? ppib->pib_pchenv : "";
  for(pch = pchEnv; *pch; pch += strlen(pch)+1);
  int len = pch-pchEnv;
  if (!(pch = (char*)malloc(len+strlen(szName)+strlen(szValue)+4))) return NULL;
  memcpy(pch,pchEnv,len);
  sprintf(pch+len,"%s=%s%c",szName,szValue,'\0');
  return pch;
}

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//                ======== Pop-up window management ==========                     //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

// Запуск Popup диалога
typedef struct POPSTR
{
  POPSTR  (PSZ pszExe, PSZ pszExeArg, PSZ pszPopCapt, PSZ pszFilter)
    {
      cb = sizeof(*this);
      this->pszExe     = sf_mallocstr(pszExe     ? pszExe     : "");
      this->pszExeArg  = sf_mallocstr(pszExeArg  ? pszExeArg  : "");
      this->pszPopCapt = sf_mallocstr(pszPopCapt ? pszPopCapt : "");
      this->pszFilter  = sf_mallocstr(pszFilter  ? pszFilter  : "");
    }
 ~POPSTR()
    {
      free(pszExe);
      free(pszExeArg);
      free(pszPopCapt);
      free(pszFilter);
      memset(this,0,sizeof(*this));
    }
  int     cb;
  PSZ     pszExe;
  PSZ     pszExeArg;
  PSZ     pszPopCapt;
  PSZ     pszFilter;
} *PPOPSTR;

typedef enum{
  FLASH_LIGHT,
  FLASH_OFF,
  FLASH_DONE,
  FLASH_ERROR,
  FLASH_START_FLASH,
  FLASH_KILL_FLASH
} FLASHSTATE;


HWND hPopup       = 0;     // Не 0, если диалог на экране
BOOL fPopBusy     = FALSE; // Если есть окно, то оно что-то еще делает


UINT        nErrors   = 0;           // Total errors
UINT        nWarnings = 0;           // Total warnings
char        szLastPopCapt[100]="";   // Last popup window caption
char        szLastCaption[100]="";   // Last popup caption line
IPT         iptBeg;                  // MLE 1-st wisible pos (to recall popup)
IPT         iptCur;                  // MLE cursor pos       (to recall popup)
PCOMMONMEM  pcm = 0;                 // pointer to the shared structure COMMONMEM
PID         pid = 0;                 // Process id
IPT         ipt;                     // MLE insertion point
LONG        lProgress;               // В диапазоне 0..lProgressLimit
LONG        lProgressLimit;          // Граница прогресса :)
PSZ         pszOutStream = 0;        // Накопитель строки (для фильтра)

FLASHSTATE  flashState = FLASH_OFF;
static  void      Flash  (HWND hFlash, FLASHSTATE fs);
static  BOOL      ShowOut(HWND hDlg, HFILE hfFrom);
MRESULT EXPENTRY  wpProgress(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY  wpMle     (HWND, ULONG, MPARAM, MPARAM);
#define  HF_STDOUT 1      /* Standard output handle */
#define  HF_STDERR 2      /* Standard error output handle */

MRESULT EXPENTRY dpPopupDlg(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
// Returns 0 - Ok;
//         1 - errors
//         2 - errors & goto message list
{
  static RESULTCODES  rcodes;
  static HWND         hFlash;
  static HWND         hProgress;
  static HWND         hMle;

  static BOOL        fShowProgress;                              // Устанавливается, если прогресс менялся
  static HPIPE       hNPipe = 0;                                 // По трубе разговариваем
  static PPOPSTR     pPopStr;                                    //
  static SHORT       nGotoMsg;                                   // Код возврата диалога (0/1/2)
  static char        sz[CCHMAXPATH+100];                         // TEMP:  EQU $C0 ;)
  static int         i;                                          // TEMP1: EQU $C2 ;)

  switch(ulMsg){
  case WM_INITDLG:
  {
    HFILE  hfOutSave,hfOutNew;
    HFILE  hfErrSave,hfErrNew;
    APIRET err;

    hPopup   = hWnd;
    fPopBusy = TRUE;
    CenterChildWindow(hWnd);
    pPopStr  = (PPOPSTR)m2;

    free(pszOutStream);
    pszOutStream          =  0;
    lProgressLimit        =  10;
    lProgress             =  0;
    fShowProgress         =  TRUE;
    ipt                   =  0;
    iptBeg                =  0;
    iptCur                =  0;
    nGotoMsg              =  0;
    nErrors = nWarnings   =  0;
    szLastCaption[0]      =  0;
    hFlash                = WinWindowFromID(hWnd,IDD_STFLASH);
    hMle                  = WinWindowFromID(hWnd,IDD_MLEOUTPUT);
    wpMle     (hMle,      MM_START, MPARAM(ULONG(WinSubclassWindow(hMle,     (PFNWP)wpMle     ))), 0L);
    hProgress             = WinWindowFromID(hWnd,IDD_STPROGRESS);
    wpProgress(hProgress, MM_START, MPARAM(ULONG(WinSubclassWindow(hProgress,(PFNWP)wpProgress))), 0L);
    {
      PSZ pszPP = pToolbox->QueryPopupPPFont();
      if (pszPP[0])
        WinSetPresParam(hMle,PP_FONTNAMESIZE,strlen(pszPP)+1,pszPP);
    }
    errOffice.ClearAll   ();                       // Clear message list
    errOffice.SetSortMode(ERROFFICE::USUAL_ORDER); // Default sort mode

    { //--- Window caption
      char *pch = OpenExprVar(pPopStr->pszPopCapt ? pPopStr->pszPopCapt : "Popup window");
      WinSetWindowText(hWnd,pch);
      strncpy(szLastPopCapt,pch,sizeof(szLastPopCapt));
      szLastPopCapt[sizeof(szLastPopCapt)-1] = '\0';
      free(pch);
    }

    if (DosAllocSharedMem((PPVOID)&pcm,COMMONMEMNAME,sizeof(*pcm),
                          PAG_COMMIT|PAG_READ|PAG_WRITE))
    {
      WinMessageBox(HWND_DESKTOP,hWnd,(PSZ)"Can't allocate shared memory.",
                    NULL,0,MB_ERROR|MB_OK|MB_MOVEABLE |MB_SYSTEMMODAL);
      WinDismissDlg(hWnd,0);
      break;
    }
    memset((PVOID)pcm,0,sizeof(*pcm));

    //--- Open named pipe for the client & riderect stdout & stderr into it
    if (hNPipe) DosClose(hNPipe);
    hNPipe = 0;
    if (err = DosCreateNPipe("\\PIPE\\XDStmp.out",&hNPipe,NP_NOINHERIT|NP_ACCESS_INBOUND,
                             NP_NOWAIT|NP_TYPE_BYTE|NP_READMODE_BYTE|0x1,0x4000,0,0))
      IOErMsg(err, hWnd, "Error", "Can't open pipe", "\\PIPE\\XDStmp.out", MB_OK |MB_SYSTEMMODAL);

    if (!err)
    {
      DosConnectNPipe(hNPipe);

      HPIPE  hNPipe2 = 0; //-- Тут же открываем второй (входной) конец созданной трубы
      ULONG  ulAction;
      err = DosOpen("\\PIPE\\XDStmp.out",&hNPipe2,&ulAction,0L,FILE_NORMAL,FILE_OPEN,
                    OPEN_ACCESS_WRITEONLY|OPEN_SHARE_DENYNONE,(PEAOP2)0L);
                          //-- И загоняем в него stdout и stderr
      hfOutSave = -1;
      hfErrSave = -1;
      if (!err) err = (BOOL)DosDupHandle(HF_STDOUT, &hfOutSave); /* Saves stdout handle      */
      if (!err) err = (BOOL)DosDupHandle(HF_STDERR, &hfErrSave); /* Saves stderr handle      */
      hfOutNew  = HF_STDOUT;
      hfErrNew  = HF_STDERR;
      if (!err) err = (BOOL)DosDupHandle(hNPipe2, &hfOutNew);    /* Duplicates stdout handle */
      if (!err) err = (BOOL)DosDupHandle(hNPipe2, &hfErrNew);    /* Duplicates stderr handle */
      if (err)
        IOErMsg(err, hWnd, "Error", "'DosDupHandle' call error", 0, MB_OK |MB_SYSTEMMODAL);
      DosClose(hNPipe2);  //-- Сам этот конец больше не нужен
    }
    BOOL fErrStdHandles = err;

    if (!err) err = DosConnectNPipe(hNPipe); // Не ясно для чего это второй раз... Но так - работает.

    if (!err)
    {
      char    szLoadError[CCHMAXPATH];
      PSZ     psz_arg      = (PSZ)malloc(strlen(pPopStr->pszExe) + strlen(pPopStr->pszExeArg) + 3);
      sprintf(psz_arg,     "%s%c%s%c", pPopStr->pszExe, '\0', pPopStr->pszExeArg, '\0');
      PSZ     psz_env      = add_env_var("__XDS_SHELL__",pPopStr->pszExe);
      err = (BOOL)DosExecPgm(szLoadError,sizeof(szLoadError),EXEC_ASYNCRESULT,psz_arg,psz_env,&rcodes,pPopStr->pszExe);
      free(psz_env);
      free(psz_arg);
      if (err)
        IOErMsg(err, hWnd, "Error", "Can't execute the program", pPopStr->pszExe, MB_OK);
    }

    if (!fErrStdHandles)
    {
      DosDupHandle(hfErrSave, &hfErrNew);        /* Brings stderr back  */
      DosDupHandle(hfOutSave, &hfOutNew);        /* Brings stdout back  */
      DosClose(hfErrSave);
      DosClose(hfOutSave);
    }

    if (err)
    {
      DosFreeMem(PVOID(pcm));
      pcm = 0;
      WinDismissDlg(hWnd,0);
      break;
    }

    pid = (PID)rcodes.codeTerminate; // It is our process...
    rcodes.codeTerminate = 0;        // No result code indicator

    flashState = FLASH_LIGHT;
    Flash (hFlash,FLASH_START_FLASH);
    WinEnableControl(hWnd,IDD_PBSTOP,TRUE);
    WinSendDlgItemMsg(hWnd,IDD_PBSTOP,BM_SETDEFAULT,MPARAM(TRUE),0);
    WinSendMsg(hMle,MLM_SETTEXTLIMIT,MPARAM(65535),0);
    WinSendMsg(hMle,MLM_FORMAT,MPARAM(MLFIE_NOTRANS),0);
    WinSendMsg(hMle,MLM_SETWRAP,MPARAM(TRUE),0);
    WinStartTimer(hAB,hWnd,1L,60);
    WinStartTimer(hAB,hWnd,2L,300); // Специально для мигалки
    break;
  }
  case WM_COMMAND:
    switch (SHORT1FROMMP(m1))
    {
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_POPUPWINDLG);
        break;
      case IDD_PBMSGLIST:
        nGotoMsg = 2;
      case DID_CANCEL:    // 'ESC' accelerator
      case IDD_PBCLOSE:
        WinPostMsg(hWnd,WM_CLOSE,0,0);
      case IDD_PBSTOP:
        WinSendDlgItemMsg(hWnd,IDD_PBSTOP,BM_SETDEFAULT,MPARAM(FALSE),0);
        WinEnableControl(hWnd,IDD_PBSTOP,FALSE);
        HWND hbtn = WinWindowFromID(hWnd, nErrors ? IDD_PBMSGLIST : IDD_PBCLOSE);
        WinSendMsg(hbtn,BM_SETDEFAULT,MPARAM(TRUE),0);
        WinSetFocus(HWND_DESKTOP,hbtn);
        if (SHORT1FROMMP(m1)==IDD_PBSTOP)
        {
          WinSetDlgItemText(hWnd,IDD_TXTCAPTION,"Stop");
          strcpy(szLastCaption,"Stop");
        }
        if (flashState != FLASH_ERROR)
          Flash(hFlash, (flashState = FLASH_DONE));
        if (pid){
          DosKillProcess(0,pid);
          pid = 0;
        }
        fPopBusy = FALSE;
        break;
    }
    break;
  case WM_CLOSE:
  {
    if (pid)    DosKillProcess(0,pid);
    if (pcm)    DosFreeMem((PVOID)pcm);
    if (hNPipe) DosClose(hNPipe);
    pcm    = 0;
    pid    = 0;
    hNPipe = 0;
    Flash (hFlash,FLASH_KILL_FLASH);
    WinStopTimer(hAB,hWnd,1L);
    WinStopTimer(hAB,hWnd,2L);
    iptBeg = (IPT)WinSendMsg(hMle,MLM_QUERYFIRSTCHAR,0,0);
    iptCur = (IPT)WinSendMsg(hMle,MLM_QUERYSEL,MPARAM(MLFQS_CURSORSEL),0);
    {//--- Save MLE text to recall popup window
      char szFTmp[CCHMAXPATH];
      strcpy(szFTmp,szIniPath);
      strcat(szFTmp,"\\");
      strcat(szFTmp,POPUP_TMP_FILE);
      DosDelete(szFTmp);
      ULONG   fsize = WinQueryWindowTextLength(hMle) + 1;
      char   *pch   = (char*)malloc(fsize);
      if (fsize>1 && pch)
      {
        HFILE     hf;
        ULONG     ul;
        WinQueryWindowText(hMle,fsize,pch);
        if (!DosOpen(szFTmp,&hf,&ul,0L,FILE_NORMAL,FILE_CREATE,
                      OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE,(PEAOP2)0L))
        {
          DosWrite(hf,pch,fsize-1,&ul);
          DosClose(hf);
        }
      }
      free(pch);
    }
    WinDismissDlg(hWnd,nGotoMsg);
    break;
  }
  case WM_TIMER:
  {
    if (SHORT1FROMMP(m1) == 2 && flashState != FLASH_ERROR && flashState!=FLASH_DONE)
    {
      static BOOL f;
      flashState = (f^=TRUE) ? FLASH_LIGHT : FLASH_OFF;
      Flash(hFlash,flashState);
      break;
    }

    { // Очень Хитрый Ускоритель
      AVAILDATA avdat;
      BYTE      buf[1];
      ULONG     cb,state;
      BOOL      fPost = FALSE;
      while (!DosPeekNPipe(hNPipe,&buf,sizeof(buf),&cb,&avdat,&state) && avdat.cbpipe)
      {
        fPost |= ShowOut(hWnd,hNPipe);
        DosSleep(0);
      }
      if (fPost) WinPostMsg(hWnd,WM_TIMER,MPARAM(1L),0);
    }
    ULONG   ulChg;
    if (!(ulChg=pcm->ulChanged))
    {
      RESULTCODES res;
      PID         pid0;
      APIRET      rc;
      rc = DosWaitChild(DCWA_PROCESSTREE,DCWW_NOWAIT,&res,&pid0,pid);
      if ((!rc || rc==128) && !rcodes.codeTerminate)
      {
        rcodes.codeResult    = res.codeResult;
        rcodes.codeTerminate = 1; // codeResult set indicator
      }

      if (rc == 0   ||    // NO_ERROR
          rc == 13  ||    // ERROR_INVALID_DATA
          rc == 128 ||    // ERROR_WAIT_NO_CHILDREN
          rc == 184 ||    // ERROR_NO_CHILD_PROCESS
          rc == 303 )     // ERROR_INVALID_PROCID
        ulChg |= CHG_ENDMAKE;
      else
         break;
    }

    if (ulChg & CHG_SENDERROR)
    {
      if (pcm->chErrClass) WinSendMsg(hWnd,MM_ADDTOOLMSG,MPARAM(pcm->chErrClass),0);
      errOffice.AddError(pcm->szBody,pcm->lPos,pcm->lLine,pcm->szFilename,pcm->chErrClass,pcm->ulErrNum);
      pcm->ulChanged &= ~CHG_SENDERROR;
      WinPostMsg(hWnd,WM_TIMER,MPARAM(1L),0);
    }

    if (ulChg & CHG_SENDCAPTION){
      WinSetDlgItemText(hWnd,IDD_TXTCAPTION,pcm->szCaption);
      strcpy(szLastCaption,pcm->szCaption);
      pcm->ulChanged &= ~CHG_SENDCAPTION;
    }

    if (ulChg & CHG_SENDCOMMENT){
      WinSetDlgItemText(hWnd,IDD_TXTCOMMENT,pcm->szComment);
      pcm->ulChanged &= ~CHG_SENDCOMMENT;
    }
    if (ulChg & CHG_SENDSTARTJOB){
      WinSetDlgItemText(hWnd,IDD_TXTPROGRESS,pcm->szProgressComment);
      lProgressLimit  = pcm->lProgressLimit;
      lProgress       = 0;
      fShowProgress   = TRUE;
      pcm->ulChanged &= ~CHG_SENDSTARTJOB;
    }

    if (ulChg & CHG_SENDPROGRESS){
      lProgress     = pcm->lProgress;
      fShowProgress = TRUE;
      pcm->ulChanged &= ~CHG_SENDPROGRESS;
    }

    if (ulChg & CHG_ENDMAKE){
      pid = 0;
      WinSendDlgItemMsg(hWnd,IDD_PBSTOP,BM_SETDEFAULT,MPARAM(FALSE),0);
      WinEnableControl(hWnd,IDD_PBSTOP,FALSE);
      HWND hbtn = WinWindowFromID(hWnd, nErrors ? IDD_PBMSGLIST : IDD_PBCLOSE);
      WinSendMsg(hbtn,BM_SETDEFAULT,MPARAM(TRUE),0);
      WinSetFocus(HWND_DESKTOP,hbtn);
      pcm->ulChanged &= ~CHG_ENDMAKE;
      ShowOut(hWnd,hNPipe);
      pcm->ulChanged &= ~CHG_ENDMAKE;
      WinStopTimer(hAB,hWnd,1L);
      WinStopTimer(hAB,hWnd,2L);
      {
        RESULTCODES res;
        PID         pid0;
        APIRET      rc;
        if (!rcodes.codeTerminate)
        {
          for(int i=0; i<20; i++) // Waiting for result code for 2 seconds
          {
            rc = DosWaitChild(DCWA_PROCESSTREE,DCWW_NOWAIT,&res,&pid0,pid);
            if (!rc || rc==128)
            {
              rcodes.codeResult    = res.codeResult;
              rcodes.codeTerminate = 1; // codeResult set indicator
              break;
            }
            DosSleep(100);
          }
        }
      }
      flashState = FLASH_DONE;
      if (!rcodes.codeTerminate || rcodes.codeResult)
      {
        if (rcodes.codeTerminate) sprintf(szLastCaption,"Error (exit code = %u)",rcodes.codeResult);
        else                      strcpy (szLastCaption,"Error (can't get exit code)");
        flashState = FLASH_ERROR;
      }
      else strcpy (szLastCaption,"Complete");
      WinSetDlgItemText(hWnd,IDD_TXTCAPTION,szLastCaption);
      Flash(hFlash,flashState);
      fPopBusy = FALSE;
    }

    if (ulChg & CHG_STARTFILES){
      pcm->ulChanged &= ~CHG_STARTFILES;
      srcList.ClearComp();
    }
    if (ulChg & CHG_APPENDFILE){
      srcList.AddFile(pcm->szFile,FALSE);
      pcm->ulChanged &= ~CHG_APPENDFILE;
    }
    if (ulChg & CHG_ENDFILES){
      srcList.ShowAppended();
      pcm->ulChanged &= ~CHG_ENDFILES;
    }
    if (ulChg & CHG_SORT_OFF)
    {
      errOffice.SetSortMode(ERROFFICE::SENT_ORDER);
      pcm->ulChanged &= ~CHG_SORT_OFF;
    }

    if (fShowProgress)
      WinInvalidateRect(hProgress,0,0);
    fShowProgress = FALSE;
    break;
  }
  case MM_ADDTOOLMSG: // CHAR1FROMMP(m1) == MSG_* constant
  {
    char ch=CHAR1FROMMP(m1);
    if (ch==MSG_ERROR || ch == MSG_SEVERE)
    {
      nErrors++;
      if (!nGotoMsg){
        WinEnableControl(hWnd,IDD_PBMSGLIST,TRUE);
        nGotoMsg = 1;
      }
      sprintf(sz,"Errors: %u",nErrors);
      WinSetDlgItemText(hWnd,IDD_TXTERRORS,sz);
    }
    else if (ch == MSG_WARNING)
    {
      nWarnings++;
      if (!nGotoMsg){
        WinEnableControl(hWnd,IDD_PBMSGLIST,TRUE);
        nGotoMsg = 1;
      }
      sprintf(sz,"Warnings: %u",nWarnings);
      WinSetDlgItemText(hWnd,IDD_TXTWARNINGS,sz);
    }
    break;
  }
  case WM_PAINT:
    WinDefDlgProc(hWnd,ulMsg,m1,m2);
    fShowProgress = FALSE;
    break;
  case WM_DESTROY:
    if (pid)    DosKillProcess(0,pid);
    if (pcm)    DosFreeMem((PVOID)pcm);
    if (hNPipe) DosClose(hNPipe);
    pcm      = 0;
    pid      = 0;
    hNPipe   = 0;
    hPopup   = 0;
    fPopBusy = FALSE;
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_POPUPWINDLG);
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

static void Flash(HWND hFlash, FLASHSTATE fs)
{
  static HBITMAP hbmFLight = 0;
  static HBITMAP hbmFOff   = 0;
  static HBITMAP hbmFDone  = 0;
  static HBITMAP hbmFError = 0;
  static HPS     hpsFlash  = 0;

  if (fs==FLASH_START_FLASH)
  {
    hpsFlash  = WinGetPS(hFlash);
    hbmFLight = GpiLoadBitmap(hpsFlash, NULLHANDLE, IDB_FLASH_ON, 0, 0);
    hbmFOff   = GpiLoadBitmap(hpsFlash, NULLHANDLE, IDB_FLASH_OF, 0, 0);
    hbmFDone  = GpiLoadBitmap(hpsFlash, NULLHANDLE, IDB_FLASH_OK, 0, 0);
    hbmFError = GpiLoadBitmap(hpsFlash, NULLHANDLE, IDB_FLASH_ER, 0, 0);
  }
  else if (fs==FLASH_KILL_FLASH)
  {
    GpiDeleteBitmap(hbmFError);
    GpiDeleteBitmap(hbmFDone);
    GpiDeleteBitmap(hbmFOff);
    GpiDeleteBitmap(hbmFLight);
    WinReleasePS(hpsFlash);
  }
  else
  {
    HBITMAP hbm = 0;
    switch(fs)
    {
      case FLASH_LIGHT: hbm = hbmFLight; break;
      case FLASH_OFF:   hbm = hbmFOff;   break;
      case FLASH_DONE:  hbm = hbmFDone;  break;
      case FLASH_ERROR: hbm = hbmFError; break;
    }
    if (hbm)
      WinSendMsg(hFlash,SM_SETHANDLE,MPARAM(hbm),0);
  }
}


MRESULT EXPENTRY wpProgress(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PFNWP pWindowProc=0;

  if (ulMsg==MM_START) pWindowProc = PFNWP(ULONG(m1));
  if (!pWindowProc)    return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  if (ulMsg==WM_PAINT)
  {
    HPS    hps  = WinBeginPaint(hWnd,0,0);
    POINTL ptl  = {0,0};
    RECTL  rclWin;
    WinQueryWindowRect(hWnd,&rclWin);
    GpiSetCurrentPosition(hps,&ptl);
    GpiSetColor(hps,CLR_DARKGRAY);
    ptl.y = rclWin.yTop;   GpiLine(hps,&ptl);
    ptl.x = rclWin.xRight; GpiLine(hps,&ptl);
    GpiSetColor(hps,CLR_WHITE);
    ptl.y = 0;             GpiLine(hps,&ptl);
    ptl.x = 0;             GpiLine(hps,&ptl);
    RECTL rcl  = {1,1,0,rclWin.yTop};
    rcl.xRight = 1 + (rclWin.xRight-2) * lProgress / max(lProgressLimit,1);
    rcl.xRight = min(rcl.xRight,rclWin.xRight-1);
    WinFillRect(hps,&rcl,CLR_BLUE);
    rcl.xLeft  = rcl.xRight;
    rcl.xRight = rclWin.xRight-1;
    WinFillRect(hps,&rcl,CLR_PALEGRAY);
    WinEndPaint(hps);
  }
  return (*pWindowProc)(hWnd,ulMsg,m1,m2);
}


MRESULT EXPENTRY wpMle(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PFNWP pWindowProc=0;

  if (ulMsg==MM_START) pWindowProc = PFNWP(ULONG(m1));
  if (!pWindowProc)    return WinDefWindowProc(hWnd,ulMsg,m1,m2);
  switch(ulMsg)
  {
    case WM_BUTTON2UP:
      if (WinQueryFocus(HWND_DESKTOP)!=hWnd)
        WinSetFocus(HWND_DESKTOP,hWnd);
      PlayMenu(hWnd, IDM_POPUP_POPUPWIN, 0, 0, 0);
      return 0;
    case WM_COMMAND:
    {
      switch(SHORT1FROMMP(m1))
      {
        case IDM_POP_FONT:
        {
          char szPPFont[FACESIZE+10] = "";
          SetPPFont(hWnd,szPPFont);
          if (szPPFont[0]) pToolbox->StorePopupPPFont(szPPFont);
          return 0;
        }
        case IDM_POP_COPY:
        {
          ULONG l = WinQueryWindowTextLength(hWnd) + 1;
          PSZ   psz;
          if (l==1 || DosAllocSharedMem(PPVOID(&psz),NULL,l,PAG_COMMIT|OBJ_GIVEABLE|PAG_READ|PAG_WRITE))
            return 0;
          WinQueryWindowText(hWnd,l,psz);
          if (WinOpenClipbrd(hAB))
          {
            WinSetClipbrdData(hAB,(ULONG)psz,CF_TEXT,CFI_POINTER);
            WinCloseClipbrd(hAB);
          }
          DosFreeMem((PVOID)psz);
          return 0;
        }
      }
      break;
    }
  }
  return (*pWindowProc)(hWnd,ulMsg,m1,m2);
}

static BOOL ShowOut(HWND hDlg, HFILE hfFrom)
{
  CHAR       achBuf[2048];
  char      *pch;
  ULONG      cb;
  HWND       hMle = WinWindowFromID(hDlg,IDD_MLEOUTPUT);
  APIRET     err;
  BOOL       fRC  = FALSE;
  while(1)
  {
    err = DosRead(hfFrom,achBuf,sizeof(achBuf-1),&cb); //----------sizeof(achBuf  _-1)_ ????????!!!!!!!!!! --*FSA
    if (err || !cb) return fRC;
    achBuf[cb] = 0;
    for (pch=achBuf; *pch; pch++) if (*pch==CR) *pch = ' ';
    WinSendMsg(hMle,MLM_SETIMPORTEXPORT,MPARAM(achBuf),MPARAM(sizeof(achBuf)));
    WinSendMsg(hMle,MLM_IMPORT,MPARAM(&ipt),MPARAM(strlen(achBuf)) );
    WinSendMsg(hMle,MLM_SETSEL,MPARAM(ipt),MPARAM(ipt));
    fRC = TRUE;
    {// filter:
      PSZ pszIn = achBuf;
      PSZ psz;
      while(psz = strchr(pszIn,LF))
      {
        *psz = 0;
        if (psz-pszIn > 2)
        {
          pszOutStream = sf_mallocconcat(pszOutStream,pszIn);
          char ch = Filter.Test(pszOutStream);
          if  (ch)  WinSendMsg(hDlg,MM_ADDTOOLMSG,MPARAM(ch),0);
          free(pszOutStream);
          pszOutStream = 0;
        }
        pszIn = psz+1;
      }
      pszOutStream = sf_mallocconcat(pszOutStream,pszIn);
    }
    DosSleep(0);
  }
}


//                           ------               ------
//                          ---- Recall popup window ----
//                           ------               ------


MRESULT EXPENTRY dpToolRePopup(HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static HWND hFlash;
  static HWND hMle;
  switch(ulMsg)
  {
  case WM_INITDLG:
  {
    fPopBusy = FALSE;
    hPopup   = hWnd;
    HFILE     hf;
    char      szFTmp[CCHMAXPATH];
    ULONG     ul,ulAction,fsize;
    char      sz[50];
    hFlash = WinWindowFromID(hWnd,IDD_STFLASH);
    hMle   = WinWindowFromID(hWnd,IDD_MLEOUTPUT);
    wpMle (hMle,MM_START, MPARAM(ULONG(WinSubclassWindow(hMle,(PFNWP)wpMle))), 0L);
    WinSetWindowText (hWnd, szLastPopCapt);
    WinSetDlgItemText(hWnd, IDD_TXTCAPTION, szLastCaption);
    sprintf(sz,"Errors: %u",nErrors);
    WinSetDlgItemText(hWnd, IDD_TXTERRORS,  sz);
    sprintf(sz,"Warnings: %u",nWarnings);
    WinSetDlgItemText(hWnd, IDD_TXTWARNINGS,sz);
    Flash (hFlash,FLASH_START_FLASH);
    Flash (hFlash,flashState);
    strcpy(szFTmp,szIniPath);
    strcat(szFTmp,"\\");
    strcat(szFTmp,POPUP_TMP_FILE);
    {
      PSZ pszPP = pToolbox->QueryPopupPPFont();
      if (pszPP[0])
        WinSetPresParam(hMle,PP_FONTNAMESIZE,strlen(pszPP)+1,pszPP);
    }

    if (DosOpen(szFTmp,&hf,&ulAction,0L,FILE_NORMAL,FILE_OPEN,
                      OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE,
                      (PEAOP2)0L))
      break;
    DosSetFilePtr(hf,0L,FILE_END,&fsize);
    fsize = min(fsize,65534);
    char *pch = (char*)malloc(fsize+1);
    if (pch)
    {
      DosSetFilePtr(hf,0L,FILE_BEGIN,&ul);
      DosRead(hf,pch,fsize,&ul);
      pch[ul] = '\0';
      WinSendMsg      (hMle,MLM_FORMAT,MPARAM(MLFIE_NOTRANS),0);
      WinSendMsg      (hMle,MLM_SETWRAP,MPARAM(TRUE),0);
      WinSetWindowText(hMle,pch);
      WinSendMsg      (hMle,MLM_SETSEL,MPARAM(iptCur),MPARAM(iptCur));
      WinSendMsg      (hMle,MLM_SETFIRSTCHAR,MPARAM(iptBeg),0);
      free(pch);
    }
    DosClose(hf);
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(hWnd,IDD_PBCLOSE));
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
    switch(SHORT1FROMMP(m1))
    {
    case IDD_PBCLOSE:
    case DID_CANCEL:
      iptBeg = (IPT)WinSendMsg(hMle,MLM_QUERYFIRSTCHAR,0,0);
      iptCur = (IPT)WinSendMsg(hMle,MLM_QUERYSEL,MPARAM(MLFQS_CURSORSEL),0);
      Flash (hFlash,FLASH_KILL_FLASH);
      WinDismissDlg(hWnd,1);
      break;
    case IDD_PBHELP:
      /*++ DisplayHelpPanel(PANEL_DLG_POPUPDLG);*/
      break;
    }
    break;
  case WM_DESTROY:
    hPopup   = 0;
    break;
  case WM_CHAR:
    if (SHORT2FROMMP(m2)==VK_F1)
    {
      /*++ DisplayHelpPanel(PANEL_DLG_POPUPDLG);*/
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}

void ToolRecallPopup()
// Всплывет/создастся немодальное popup окно, во втором случае - не вернется.
{
  if   (hPopup)  WinSetFocus(HWND_DESKTOP,hPopup);
  else WinDlgBox(HWND_DESKTOP, NULL, (PFNWP)dpToolRePopup, NULLHANDLE, IDD_POPUPDLG, 0);
}

void ToolHidePopup()
// Если popup окно не в работе, то оно спрячется.
{
  if(hPopup && !fPopBusy)  WinDestroyWindow(hPopup);
}

void ToolStartPopup (MPARAM(m1))
// Запуск тула в немодальном popup окне, не вернется до окончания.
{
  if (m1) // в m1 постился указатель не POPSTR
  {
    if (hPopup)
    {
      if (fPopBusy)
      {
         WinMessageBox(HWND_DESKTOP,hMDI0Client,"You must terminate the process currently executing"
                       " in popup window before using the window to start a new process.",
                       "REXX: 'Execute' call",0,MB_ERROR|MB_OK|MB_MOVEABLE);
         delete PPOPSTR(m1);
      }
      else
      {
        WinDismissDlg(hPopup,0);
        WinDestroyWindow(hPopup);
        hPopup = 0;
      }
    }
    int rc = (int)WinDlgBox(HWND_DESKTOP, NULL, (PFNWP)dpPopupDlg,
                        NULLHANDLE, IDD_POPUPDLG, PVOID(m1));
    errOffice.DoneUpdating();
    if (rc==2)
      WinSetFocus(HWND_DESKTOP,clMsgList.hWnd);
    delete PPOPSTR(m1);
  }
}



/////////////////////////// З а п у с к   т у л о в ///////////////////////////////////////////
/////////////////////////// З а п у с к   т у л о в ///////////////////////////////////////////
/////////////////////////// З а п у с к   т у л о в ///////////////////////////////////////////

/*>>> Execute (<filename>, <...>)
>
> <filename> - имя файла для выполнения, может быть дополнено
>              пробелом и аргументами ($() разрешены)
>
> <...>
>   Для Popup окна:
>     "POPUP=<title line>" [, "FILTER=<filter>"]
>       <title line> - строка для заголовка окна ($() разрешены)
>       <filter>     - имя фильтра
>   Для отдельной сессии:
>     "SEPARATE=<mode>"
>       <mode>       - флаги для start-а (f.ex. "/K /F")
*/
LONG   EXPENTRY RhExecute(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  PSZ  pszExe     = 0;
  PSZ  pszExeArg  = 0;
  BOOL fCmdFile   = FALSE;
  BOOL fPopup;
  PSZ  pszPopCapt = 0;
  PSZ  szFilter   = 0;
  PSZ  szStartF   = "";
  PSZ  pszErr     = 0;
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  {
    // This fucked IBM's CSET doesn't generates try{} correctly here!
    // So we must fuck it up using 'pszErr' var. Gr-r-r!!!
    //
    if (argc<2) {pszErr = "Error in the argument list."; goto fucked_ibm;}
    //--- Установим запускаемый файл и аргументы:
    {
      PSZ   psz;
      BOOL  f_ext = FALSE;
      int   l = strcspn(argv[0].strptr," ");
      if (!(pszExe = (PSZ)malloc(l+1))) { pszErr="Out of memory error.";  goto fucked_ibm;}
      strncpy(pszExe,argv[0].strptr,l); pszExe[l] = '\0';      // pszExe == argument 1 [до пробела]
      if (!pszExe[0]) {pszErr="Argument 1: file name expected."; goto fucked_ibm;}
      if (psz = strrchr(pszExe,'.'))
      {
        fCmdFile = !sf_stricmp(psz,".cmd");                    // fCmdFile = *.CMD
        f_ext    = fCmdFile || !sf_stricmp(psz,".exe");        // f_ext    = *.cmd || *.exe
      }
      if (!f_ext)                                              // добавим ".EXE"?
      {
        if (!(psz = (PSZ)malloc(strlen(pszExe)+5))) {pszErr="Out of memory error."; goto fucked_ibm;}
        sprintf(psz,"%s.exe",pszExe);
        free(pszExe);
        pszExe = psz;
      }
      //--- И найдем полное имя к существующему файлу
      {
        int l_exe = strlen(pszExe)*2;
        if (!(psz = (PSZ)malloc(max(l_exe,CCHMAXPATH)))) {pszErr="Out of memory error."; goto fucked_ibm;}

        BOOL fFound = FName2Full(psz,pszExe,TRUE);
        if (!fFound && !strchr(pszExe,'\\') && !strchr(pszExe,':'))
          if (!(fFound = FindRedirFile(psz,pszExe)))
          {
            PCSZ  PathValue                = "";        /* PATH environment variable    */
            char  SearchResult[CCHMAXPATH] = "";        /* Result of PATH search        */

            if (!DosScanEnv("PATH",&PathValue) &&
                !DosSearchPath(SEARCH_CUR_DIRECTORY | SEARCH_IGNORENETERRS,
                               PathValue,pszExe, (PBYTE)SearchResult, sizeof(SearchResult)) )
            {
              strcpy(psz,SearchResult);
              fFound = TRUE;
            }
          }
        if (!fFound)
        {
          char sz[500];
          sprintf(sz,"Can't find executable file '%s'.",pszExe);
          free(psz); free(pszExe);
          pszErr=sz;
          goto fucked_ibm;
        }
        else
        {
          free(pszExe);
          pszExe = psz;
        }
      }
      if (!(pszExeArg=strchr(argv[0].strptr,' '))) pszExeArg = "";
    }
    //--- pszExe    == malloc()ed full filename
    //--- pszExeArg == argument line (may be "")

    //--- Определяем тип сессии
    {
      const pop_len = 6; // strlen("POPUP=");
      const flt_len = 7; // strlen("FILTER=");
      const spr_len = 9; // strlen("SEPARATE=");
      if (!strncmp(argv[1].strptr,"POPUP=",pop_len))
      {
        fPopup     = TRUE;
        pszPopCapt = argv[1].strptr + pop_len;
        if (argc>=3)
        {
          if (!strncmp(argv[2].strptr,"FILTER=",flt_len))
            szFilter = argv[2].strptr + flt_len;
          else
            {pszErr="Argument 2: 'FILTER=' expected."; goto fucked_ibm;}
        }
      }
      else if (!strncmp(argv[1].strptr,"SEPARATE=",spr_len))
      {
        fPopup   = FALSE;
        szStartF = argv[1].strptr + spr_len; // Флаги для start-а
      }
      else {pszErr="Argument 1: 'POPUP=' expected."; goto fucked_ibm;}
    }

    if (fPopup) // ------ Run in pop-up window --------- //
    {
      if (fCmdFile)
        WinMessageBox(HWND_DESKTOP,hMDI0Client,"Sorry, I can't execute '*.cmd' files in the popup window.",
                      "REXX: 'Execute' call",0,MB_ERROR|MB_OK|MB_MOVEABLE);
      else
      {
        //--- POPUP окно запускается через посылку сообщения, чтобы в вернуться из rexx-а прямо сейчас
        PPOPSTR pPS = new POPSTR(pszExe, pszExeArg, pszPopCapt, szFilter);
        WinPostMsg(hMDI0Client,MM_STARTPOPUPWIN,MPARAM(pPS),0); //--- Там стартует немодальное окно.
      }
      free(pszExe);
      return 0;
    }
    else // ------ Run in a separate session --------- //
    {
      RESULTCODES rcodes = {0};
      char        szLoadError[100];
      APIRET      err;
      PSZ         pszCmd = (PSZ)malloc(strlen(pszExe) + strlen(pszExeArg) + 100);
      if (!pszCmd) {pszErr="Out of memory error"; goto fucked_ibm;}
      sprintf (pszCmd,"cmd.exe%c /C \"start %s %s %s \"%c", '\0', szStartF, pszExe, pszExeArg, '\0');

      free(pszExe);

      // -- Now: pszCmd - arguments (in the DosExecPgm form) for cmd.exe
      PSZ     psz_env      = add_env_var("__XDS_SHELL__",pszExe);
      if (err=DosExecPgm(szLoadError,sizeof(szLoadError),EXEC_ASYNC,pszCmd,psz_env,&rcodes,"cmd.exe"))
        IOErMsg(err, hMDI0Client, "REXX: 'Execute' call", "Can't execute the program", "cmd.exe", MB_OK);
      free(psz_env);
      free(pszCmd);
    }
  }
  if (pszErr)
  { // FUCK IBM & it's try/catch code generation!!
  fucked_ibm:
    WinMessageBox(HWND_DESKTOP,hMDI0Client,pszErr,"REXX: 'Execute' call",0,MB_ERROR|MB_OK|MB_MOVEABLE);
    return RxERROR;
  }
  return 0;
}


void RegisterToolREXX()
{
  RxRegister("Execute",          (PFN)RhExecute,FALSE);
}


