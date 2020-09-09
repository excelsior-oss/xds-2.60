/*
*
*  File: TOOLS.H
*
*  XDS Shell - tools definitions
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#ifndef __TOOLS__
#define __TOOLS__

#define TOOLNAMELEN        32
#define MAXTOOLS           50
#define IDM_TOOL_SEPARATOR (TOOL_MIN_ID + MAXTOOLS + 1)
#define POPUP_TMP_FILE     "XD$$HELL.TMP"

void    ToolRecallPopup();           // Всплывет/создастся немодальное popup окно, во втором случае - не вернется.
void    ToolHidePopup();             // Если popup окно не в работе, то оно спрячется.
void    ToolStartPopup (MPARAM(m1)); // Запуск тула в немодальном popup окне, не вернется до окончания.

struct TOOL
{
  int           cb;                         // Для передачи диалогам
  TOOL         *ptDefValue;                 // Не 0 если это был тул из тулсета
  char          szName      [TOOLNAMELEN];  // Наше имя
  char          szComment   [46];           // Текст 'сопли'
  PSZ           pszToolScript;              // malloc()ed текст собссно ; 0 - тула нету
  PSZ           pszFilter;                  // malloc()ed фильтр (или 0); [0] = +/- (use/not use)
  char          szMenuItem  [50];           // "" - в меню не ставится
  BOOL          fRunWhenProject;            // Фильтр: нужен известный файл проекта
  char          szMatchFiles[50];           // Фильтр: шаблон имени текущего файла
  char          szHotKey    [20];           // Что-то типа "Alt+'C'" (--- не используется!)
  ACCEL         rAccel;                     // Accel для hot key (или .fs будет 0)
  USHORT        usBmp;                      // Идентификатор ресурса битмапа или 0

                TOOL        ();                       //
               ~TOOL        ();                       //
  BOOL          IsEnabled   ();                       // Enabled?
  BOOL          Execute     (HWND hMsgWnd);           // Запуск (вернет FALSE при ошибке)
  void          Copy        (TOOL *pSrc);             // Взять значение
  BOOL          RProfile    (LONG slot, PSZ szKey);
  BOOL          WProfile    (LONG slot, PSZ szKey, BOOL fMkNewDefs = FALSE);
  void          free_all    ();
private:
  BOOL          w_prf       (LONG slot, PSZ szKey, BOOL fMkNewDefs = FALSE);
  BOOL          r_prf       (LONG slot, PSZ szKey, BOOL *pfQDef);
};
typedef TOOL *PTOOL;


class TOOLBOX
{
public:
               TOOLBOX();
              ~TOOLBOX();
  BOOL         Execute          (LONG lCtrlId);
  BOOL         PlayTool         (PQMSG pqmsg);
  void         EditTools        ();
  void         ReenableAll      ();
  void         TuneMenu         ();
  void         Copy             (TOOLBOX *ptb);
  void         StorePopupPPFont (char szPPFont[FACESIZE+10]);
  PSZ          QueryPopupPPFont () {return szPPFont;}
  BOOL         RProfile         (LONG slot, PSZ pszKey, BOOL fKeepOld); // FALSE если слишком много тулзов
  void         WProfile         (LONG slot, PSZ pszKey, BOOL fMkNewDefs = FALSE);
private:
  PTOOL        apTools[MAXTOOLS];             // Структуры тулов
  char         szPPFont[FACESIZE+10];         // Фонт popup окна (типа "8.Helv" или "")

  HACCEL       make_hAccel       ();
  void         del_controls      (BOOL fMenu);
  void         reFill_controls   (BOOL fMenu);
  LONG         find_enabled      (PSZ pszName); // Номер активного тула с таким именем (или -1)
  friend       MRESULT EXPENTRY  dpToolCfgDlg(HWND,ULONG,MPARAM,MPARAM);
};
typedef TOOLBOX *PTOOLBOX;

#if 0
  Тулам сопоставляются идентификаторы контролов TOOL_ID_MIN+<номер тула>,
  Реально вызванный тул ищется путим взятия имени кликнутого и поиска среди
  множества тулов с таким именем первого активного
#endif

#define WC_QUERYACCEL "WC_QAccelWindow"
MRESULT EXPENTRY wpQueryAccel (HWND, ULONG, MPARAM, MPARAM);
void             Acc2Mp       (const ACCEL*, PMPARAM, PMPARAM);
void             Mp2Acc       (MPARAM, MPARAM, PACCEL);
PSZ              Mp2Txt       (PMPARAM pm1, PMPARAM pm2);


#endif /* ifndef __TOOLS__ */

