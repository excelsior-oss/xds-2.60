/*
*
*  File: XDSBASE.H
*
*  XDS Shell main definitions
*
*  (c) 1996,1997 by *FSA & NHA Inc.
*
*/

#ifndef __XDSBASE__
#define __XDSBASE__
#define  _DLL_USER_

#include <REXXSAA.H>
#include "MDI.H"
#include "TOOLBAR.H"
#include "REXXCALL.H"
#include "TED.H"
#include "TOOLS.H"
#include "PROJECT.H"
#include "REDIR.H"
#include "BARS.H"
#include "FMAN.H"
#include "FONTCLR.H"
#include "PROFILE.H"
#include "HLTDLL.H"
#include "edthelp.h"
#include "edthelp2.h"
#include "sf_lib.h"

#define  HSTWPDEEP    15
#define  HSTFILESDEEP 15
#define  HSTPRJDEEP   15
#define  HSTFINDDEEP  15
#define  TASK_NAME    "XDS v2.14"
#define  DBGLOG 1  // 0/1 no/print log (to stderr)

typedef class    FILEED;
typedef FILEED *PFILEED;

struct CPARAMSTR
{
  ULONG   cb;
  PSZ     psz1, psz2, psz3;
  LONG    l1,   l2,   l3;
  CPARAMSTR ();
};
typedef CPARAMSTR *PCPARAMSTR;

class HISTORY
{
public:
   HISTORY(BOOL fCharSet,LONG lMaxDeep);
  ~HISTORY();
   BOOL Append(PSZ szItem);
   BOOL Delete(PSZ szItem);
   PSZ  Enum();
   PSZ  NextEnum();
   void Clear();
   void WProfile(LONG slot, PSZ szName);
   void RProfile(LONG slot, PSZ szName);
private:
   PLINELIST  pllHist;
   PLINELIST  pllEnum;
   LONG       lMaxDeep;
   BOOL       fCharSet;
   PLINELIST *find(PSZ szItem);
};
typedef HISTORY *PHISTORY;


struct  ERRSTR;
typedef ERRSTR *PERRSTR;
struct FILEERRLIST
{
  FILEERRLIST   (PSZ szFilename);
 ~FILEERRLIST   ();
  FILEERRLIST   *next;
  char           szFilename[CCHMAXPATH];
  PSZ            pszShortName; // указывает в szFilename
  PERRSTR        pErrList;
};
typedef FILEERRLIST *PFILEERRLIST;

struct ERRSTR
{
  PFILEERRLIST  pFEL;
  ERRSTR       *next;
  LONG          lListItem;
  TPR           tprErr;     // TPR (начало текста = 0,0)
  ULONG         ulErrNum;
  ULONG         ulErrClass;
  ULONG         ulSentOrder;
  PSZ           pszError;
   ERRSTR      (PSZ pszErr);
  ~ERRSTR      ();
};

class ERROFFICE
{
public:
  enum      SORT_MODE    {USUAL_ORDER, SENT_ORDER};
  enum      GO_ERROR     {PREVIOUS_IN_LIST, NEXT_IN_LIST, PREVIOUS_IN_FILE, NEXT_IN_FILE};
            ERROFFICE    ();
           ~ERROFFICE    ();
  void      ClearAll     ();
  void      SetSortMode  (SORT_MODE sMode);
  SORT_MODE QuerySortMode(){return sMode;}
  void      DoneUpdating ();
  void      AddError     (PSZ szText, ULONG ulPos, ULONG ulLine, PSZ pszFile, ULONG ulErrClass, ULONG ulErrNum);
  BOOL      GoError      (ULONG    ulLboxItem, BOOL fActivateEd);
  BOOL      GoError      (GO_ERROR goError);
  void      FillList     ();
  void      TextChanged  (PTCD ptcd);
  LONG      l1stLine;    // Позиция первого символа по мнению компилятора
  LONG      l1stPos;     // (обычно 1;1) В самих ошибках хранится в нормальном виде (от 0;0).
  ULONG     ulSentCount; // Возрастает для каждой новой пришедшей ошибки
private:
     // Ошибки хранятся в упорядоченных по УБЫВАНИЮ позиции в файле списках ERRSTR (свой список для
     // каждого файла.) Эти списки висят на списке FILEERRLIST. В нем:
     //   имена файлов не дублируются;
     //   может быть одно вхождение с пустым именем файла;
     //   нет узлов с пустым списком ошибок.
  PFILEERRLIST    pFEList;
  SORT_MODE       sMode;
};

class SRCLIST
{
// Имена в списке есть двух типов - 'manual'=полученные из списка установленного юзером
//        и 'comp' - полученные от компилятора (добавлен 1 символ перед строкой = 'M'/'C')
//
public:
  SRCLIST();
  void      SetFullNames   (BOOL fFullNames);              // Показывать ли полные имена
  BOOL      QueryFullNames () {return fFullNames;}
  PSZ       Enum           (int nWhat, BOOL f_private=FALSE);              // nWhat = 0/1/2: all/sel/manual
  PSZ       NextEnum       (int nWhat) {return Enum(nWhat,TRUE);}          // --//--
  void      ClearManual    (BOOL fNoShow=FALSE) {clear_spec('M',fNoShow);} // Очистить m-имена
  void      ClearComp      (BOOL fNoShow=FALSE) {clear_spec('C',fNoShow);} // Очистить c-имена
  void      AddFile        (PSZ pszFile, BOOL fTagItManual = FALSE);
  void      ShowAppended   ();                                             // Показать изменения
  void      Select         (char chMode, PSZ pszPattern);
  void      ToggleCurItemSel();
  void      WProfile(LONG slot, PSZ szName);
  void      RProfile(LONG slot, PSZ szName);
private:
  void      fill_lbox();
  void      show_selstates(int nIt=LIT_NONE);
  void      clear_spec(char chWhat, BOOL fNoShow);
  PLINELIST pllSrcFiles;
  BOOL      fFullNames;
  int       nItEnum;
  // List item handles == PLINELIST, text: 'M'|'C' '+'|'-' <full filename> ,
  //                     M/C - manual/compiler's item
  //                     +/- - selected/unselected state
};

enum    TB_POSITION { TB_UP,TB_DOWN,TB_LEFT,TB_RIGHT,TB_NONE };
#define TB_TOTALPOS   5

/* --------- Global variables -------------- */
extern HAB                    hAB;
extern HMQ                    hMsgQ;
extern HSWITCH                hSwitch;
extern BOOL                   fConfigON;
extern PMDIFRAME              pMDI;
extern HWND                   hMDI0Client;
extern CLIENTINFO             clSrcList;
extern CLIENTINFO             clMsgList;
extern ERROFFICE              errOffice;
extern SRCLIST                srcList;
extern GROUPOFFICE            grOffice;
extern PVARTABLE              pAutoVarTable;
extern PVARTABLE              pUserVarTable;
extern PTOOLBOX               pToolbox;
extern MACROS                 Macros;
extern STATUSINFO             statusInfo;
extern char                   szPrjFile    [CCHMAXPATH];
extern char                   szHomePath   [CCHMAXPATH];
extern char                   szCurIOPath  [CCHMAXPATH];
extern char                   szCurPath    [CCHMAXPATH];
extern HISTORY                hstWPlaces;
extern HISTORY                hstFiles;
extern HISTORY                hstProjects;
extern HISTORY                hstFind;
extern HISTORY                hstFindChTo;
extern PAINTEROFFICE          ptOffice;
extern BOOL                   fFindCaseSensitive;
extern BOOL                   fFindWholeWord;


#define EM_SEL           0x00000001 // Selection ON
#define EM_SEL_OFF       0x00000003 //           OFF
#define EM_SEL_OFFF      0x00000002 //
#define EM_UNDO          0x00000004 // Undo ON
#define EM_UNDO_OFF      0x0000000c //      OFF
#define EM_UNDO_OFFF     0x00000008 //
#define EM_ED            0x00000010 // Current editor ON
#define EM_ED_OFF        0x00000030 //                OFF
#define EM_ED_OFFF       0x00000020 //
#define EM_ASKALL        0x80000000 // Ask all the states directly

/* --------- Exported functions -------------- */

void       SetAllCaptions     ();
void       EnableMenues       (ULONG emFlags);
MRESULT    WMEdCharHandler    (HWND hWnd, MPARAM m1, MPARAM m2);
ULONG      IOErMsg            (APIRET err, HWND hWnd, char *szCapt, char *szErr, char *szFile, ULONG MBF);
ULONG      MessageBox         (HWND hOwner,   PSZ pszCaption, ULONG flStyle,
                               PSZ pszText1,  PSZ pszText2=0, PSZ pszText3=0,
                               PSZ pszText4=0);
BOOL       SaveConfig         ();
BOOL       StrToAcc           (PSZ *ppsz, ACCEL *pAccel);
void       PlayMenu           (HWND hWnd, ULONG ulRCMenu, LONG alChecked[], LONG alDisabled[],
                               LONG lPosItem, LONG xPos=-1000, LONG yPos=-1000);
void       SetPPFont          (HWND hWnd, PSZ pszPPCopy = 0);
void       Beep               ();
//--- Entryfield dialog:
//
//  CPARAMSTR fields:
//    psz1 (in)     - window caption
//    psz2 (in)     - line over the entry field
//    psz3 (in/out) - entry filed contents
//    l1            - size of psz3 buffer
//    l3            - help panel identifyer (or 0)
//    Returns:      - TRUE when Ok.
//
MRESULT EXPENTRY dpEntryFDlg(HWND hWnd,ULONG ulMsg, MPARAM m1, MPARAM m2);

//--- Select directory dialog
//
//  CPARAMSTR fields:
//    char  psz1[CCHMAXPATH] (in/out) - directory (in: "" => current)
//    PSZ   psz2             (in)     - Window caption (or 0 => "Select directory")
//    LONG  l3               (in)     - help panel or 0
//    Returns:                        - TRUE when Ok.
//
MRESULT EXPENTRY dpSelDirDlg(HWND hWnd,ULONG ulMsg,MPARAM m1,MPARAM m2);


/* --------- Our internal messages -------------- */

#define MM_START              (WM_USER+1000)
#define MM_KILLME             (WM_USER+1001)
#define MM_CHECKFTIMES        (WM_USER+1002)
#define MM_ADDTOOLMSG         (WM_USER+1003)
#define MM_STARTPOPUPWIN      (WM_USER+1004)

#define ICO_MDICLIENT         1200
#define ICO_REXXMON           1201

/* --------- Resources ids & commands -------------- */

//--- Images -----
#define IDB_LOGOBMP           999
// Standard toolbar
#define IDB_TBBNEW            1000
#define IDB_TBBOPEN           1001
#define IDB_TBBSAVE           1002
#define IDB_TBBPRINT          1003
#define IDB_TBBUNDO           1004
#define IDB_TBBCUT            1005
#define IDB_TBBCOPY           1006
#define IDB_TBBPASTE          1007
#define IDB_TBBFIND           1008
#define IDB_TBBFINDNEXT       1009
#define IDB_TBBSETTINGS       1010
// Toolbar tools
#if 0
  #define IDB_TBBCOMPILE        1100
  #define IDB_TBBMAKE           1101
  #define IDB_TBBBUILD          1102
  #define IDB_TBBRUN            1103
  #define IDB_TBBDBG            1104
  #define IDB_TBBTOOL0          1105
  #define IDB_TBBTOOL1          1106
  #define IDB_TBBTOOL2          1107
  #define IDB_TBBTOOL3          1108
  #define IDB_TBBTOOL4          1109
  #define IDB_TBBTOOL5          1110
  #define IDB_TBBTOOL6          1111
  #define IDB_TBBTOOL7          1112
  #define IDB_TBBTOOL8          1113
  #define IDB_TBBTOOL9          1114
#endif
// Pointers
#define PTR_MOVE              1300
#define PTR_COPY              1301
#define PTR_DROPGITEM         1302
#define PTR_NODROPGITEM       1303
#define PTR_DROPFILE          1304
#define PTR_NODROPFILE        1305
#define PTR_DROPFILES         1306
#define PTR_NODROPFILES       1307
// Bitmaps
#define BMP_FILE2LST          1400
#define BMP_FNEW2LST          1401
#define BMP_GRP2LST           1402
#define BMP_GOP2LST           1403
#define BMP_RGB2LST           1404
#define BMP_COLORS            1410
#define BMP_OPOPTION          1415
#define BMP_CLOPTION          1416
#define IDB_FLASH_ON          1420
#define IDB_FLASH_OF          1421
#define IDB_FLASH_OK          1422
#define IDB_FLASH_ER          1423


#define RES_MAIN              10

#define ACC_PREVEDITOR        12
#define ACC_NEXTEDITOR        13
#define ACC_ZPREVEDITOR       14
#define ACC_ZNEXTEDITOR       15
#define ACC_LISTNEXTERR       16
#define ACC_LISTPREVERR       17

#define IDM_LASTSEPARATOR     99

#define IDM_FILE              100
#define IDM_FILENEW           101
#define IDM_FILELOAD          102
#define IDM_FILERELOAD        103
#define IDM_FILESAVE          104
#define IDM_FILESAVEAS        105
#define IDM_FILESAVEALL       106
#define IDM_FILEPRINT         107
#define IDM_FILEAUTOSAVE      108
#define IDM_FILEMAXNEW        109
#define IDM_FILEBAK           110
#define IDM_FILE1AUTOWIN      111
#define IDM_FILEHIST0         120 //...IDM_FILEHIST0+HISTDEEP-1

#define IDM_EDIT              300
#define IDM_EDITUNDO          301
#define IDM_EDITREDO          302
#define IDM_EDITCUT           303
#define IDM_EDITCOPY          304
#define IDM_EDITPASTE         305
#define IDM_EDITSELALL        306
#define IDM_EDITCLEAR         307
#define IDM_EDITFIND          308
#define IDM_EDITFINDNEXT      309
#define IDM_EDITFINDPREVIOUS  310
#define IDM_EDITFILEFIND      311
#define IDM_EDITPREVERR       312
#define IDM_EDITNEXTERR       313
#define IDM_EDITGOTOLINE      314

#define IDM_OPTIONS           400
#define IDM_OPTSETTINGS       401
#define IDM_OPTASS            402
#define IDM_OPTMACROS         403
#define IDM_OPTRECALLMACROED  404
#define IDM_OPTVARTABLE       405

#define IDM_WORKPLACE         500
#define IDM_WPNEW             501
#define IDM_WPLIST            502
#define IDM_WPSAVEAS          503
#define IDM_WPTEMPLATES       504
#define IDM_WPPROPERTIES      506
#define IDM_WPHIST0           507 //... IDM_WPHIST0+WPHISTDEEP-1

#define IDM_TOOLS             600
#define IDM_TOOLCONFIGURE     601
// Команды вызова тулов будут
//          TOOL_MIN_ID+<номер тула (*одного из перегруженных*)>

#define IDM_PROJECT           700
#define IDM_PROJNEW           701
#define IDM_PROJOPEN          702
#define IDM_PROJCLOSE         703
#define IDM_PROJMODIFY        704
#define IDM_PROJEDTEXT        705
#define IDM_PROJHIST0         706 //... IDM_PROJHIST+PROJHISTDEEP-1

// IDM_WINDOW menu defined in MDI.H
#define IDM_WINSRCLIST        800
#define IDM_WINMSGLIST        801
#define IDM_WINREPOPUP        802
#define IDM_WINREXXMON        803
#define IDM_WINEDFIRST        810
#define IDM_WINEDAUTO         (IDM_WINEDFIRST+AUTOWINDOW)
#define IDM_WINEDLAST         (IDM_WINEDFIRST+MAXEDWINDOWS-1)

#define IDM_HELP              900
#define IDM_HELPINDEX         901
#define IDM_HELPGENERAL       902
#define IDM_HELPUSINGHELP     903
#define IDM_HELPKEYS          904
#define IDM_HELPPRODUCTINFO   905
#define IDM_HELPSEPARATOR     906
#define IDM_HELP_INFFILE0     907 //...IDM_HELPINFFILE0+MAXINFFILES-1 (see edthelp.h)

// Items to insert into frame system menu:
#define IDM_SYSMY             2000
#define IDM_SYSMY1            2001
#define IDM_SYSMY2            2002

#define IDM_POPUP_MSGLIST     2050
#define IDM_POPUP_SRCLIST     2051
#define IDM_POPUP_POPUPWIN    2052
#define IDM_POPUP_SETBM       2053
#define IDM_POPUP_GOBM        2054
#define IDM_POPUP_EDTITLEBAR  2055
#define IDM_POPUP_FILELIST    2056
#define IDM_POPUP_GRPLIST     2057
#define IDM_POPUP_GRPLIST1    2058

#define IDM_POP_FONT          2100
#define IDM_POP_SORT          2101
#define IDM_POP_DEFAULT       2102
#define IDM_POP_UNSORTED      2103
#define IDM_POP_NAME          2104
#define IDM_POP_TYPE          2105
#define IDM_POP_FULLNAMES     2106
#define IDM_POP_COPY          2108
#define MAXBOOKMARKS                   10
#define IDM_POP_SETBM0        2110 //..2119
#define IDM_POP_GOBM0         2120 //..2129
#define IDM_POP_SELFILES      2130
#define IDM_POP_UNSELFILES    2131
#define IDM_POP_INVSELFILES   2132
#define IDM_POP_FILEFIND      2133
#define IDM_POP_FILEINVSEL    2134
#define IDM_POP_UNSELALL      2135
#define IDM_POP_NEWFILE       2136
#define IDM_POP_CLOSEWIN      2137
#define IDM_POP_GROUPS        2138
#define IDM_POP_AGROUP        2139
#define IDM_POP_1GROUP        2140
#define IDM_POP_2GROUP        2141
#define IDM_POP_3GROUP        2142
#define IDM_POP_4GROUP        2143
#define IDM_POP_5GROUP        2144
#define IDM_POP_6GROUP        2145
#define IDM_POP_7GROUP        2146
#define IDM_POP_8GROUP        2147
#define IDM_POP_9GROUP        2148

//                            2400..2499 reserved for MDI


#define IDM_TBTOOL0           3000 // ... 3000+MAXTOOLS-1 (toolbar tool buttons)

//--- File open dialog
#define IDD_FILEDLG           6001
#define IDD_LBFILESLOADED     6002
#define IDD_LBDIRFILES        6003
#define IDD_STPATHNAME        6004
#define IDD_CBPATH            6005
#define IDD_PBPATHGO          6006
#define IDD_PBFILEEDITORS     6007
#define IDD_PBFILESRCLIST     6008
#define IDD_PBHELP            6009

//--- Load dialog
#define IDD_FASTLOADDLG       6050
#define IDD_EFFILEPATH        6051
#define IDD_CBFILELOAD        6052
#define IDD_CBWINLOAD         6053
#define IDD_PBBROWSE          6054
#define IDD_PBADVLOAD         6055
#define IDD_PBRESETPATH       6056

//--- SaveAs dialog
#define IDD_SAVEASDLG         6101
#define IDD_EFSAVEAS          6102
#define IDD_STSAVEAS          6103

//--- Save Changes (Y/N..) dialog
#define IDD_SAVCHDLG          6201
#define IDD_PBSAVEALL         6202
#define IDD_PBPROMPT          6203
#define IDD_PBDONTSAVE        6204

//--- Entryfield dialog
#define IDD_ENTRYFDLG         6301
#define IDD_STLINE            6302
#define IDD_EFEF              6303

//--- Select directory dialog
#define IDD_SELDIRDLG         6400
#define IDD_EFSELDIR          6401
#define IDD_LBSELDIR          6402

//--- Workplace list dialog
#define IDD_WPLISTDLG         6500
#define IDD_LBWPLIST          6501
#define IDD_MLEWPCOMMENT      6502
#define IDD_PBWPREMOVE        6503

//--- Workplace apply dailog
#define IDD_APPWPDLG          6510
#define IDD_EFAWPNAME         6511
#define IDD_MLEAWPCOMMENT     6512
#define IDD_CBAWPTOOLS        6513
#define IDD_RBAWPTOOLSADD     6514
#define IDD_RBAWPTOOLSREM     6515
#define IDD_CBAWPASS          6516

//--- Workplace properties dialog
#define IDD_WPPROPDLG         6520
#define IDD_EFWPPNAME         6521
#define IDD_MLEWPPCOMMENT     6522
#define IDD_PBWPPCHDIR        6523
#define IDD_STWPPDIR          6524
#define IDD_CBWPPMIRROR       6525
#define IDD_EFWPPMIRROR       6526
#define IDD_PBWPPMIRRBROWSE   6527

//--- Workplace templates dialog
#define IDD_WPTPLDLG          6550
#define IDD_LBWPTPLS          6551
#define IDD_MLEWPTPL          6552
#define IDD_PBWPTPLAPPLY      6553
#define IDD_PBWPTPLREM        6554
#define IDD_PBWPTPLSAVE       6555
#define IDD_PBWPTPLADV        6556
#define IDD_EFWPTPLNAME       6557

//--- Workplace templates / advanced settings dialog
#define IDD_WPTPLADVDLG       6568
#define IDD_MLEWPADV          6569

//--- New workplace dialog
#define IDD_NEWWPDLG          6570
#define IDD_LBNWPTPL          6571
#define IDD_MLENWPCOMMENT     6572
#define IDD_EFNWPNAME         6573
#define IDD_EFNWPDIR          6574
#define IDD_PBNWPCHDIR        6575
#define IDD_RBNWPMIRROR       6576
#define IDD_RBNWPTPL          6577
#define IDD_EFNWPMIRROR       6578
#define IDD_PBNWPBROWSE       6579

//--- Associations/options dialog
#define IDD_ASSOPTDLG         6600
#define IDD_STASSDESCRIPTION  6601
#define IDD_CBASSWITH         6602
#define IDD_PBEDASSLIST       6603
#define IDD_RBGLOBASS         6604
#define IDD_RBLOCASS          6605
#define IDD_CBHILITEMODE      6606
#define IDD_SPBTABWIDTH       6607
#define IDD_CBSMARTTABS       6608
#define IDD_CBAUTOIDENT       6609
#define IDD_CBPASTEREPSEL     6610
#define IDD_CBUSETABCHARS     6611
#define IDD_CBHLTLINE         6612
#define IDD_CBHLTSYNTAX       6613
#define IDD_CBCLRTOALL        6614
#define IDD_STFONTSHOW        6615
#define IDD_PBCHFONT          6616
#define IDD_PBCHFONT2ALL      6617
#define IDD_BMCLRSEL          6618
#define IDD_BMCLR0            6619
#define IDD_BMCLR1            6620
#define IDD_BMCLR2            6621
#define IDD_BMCLR3            6622
#define IDD_BMCLR4            6623
#define IDD_BMCLR5            6624
#define IDD_BMCLR6            6625
#define IDD_BMCLR7            6626
#define IDD_BMCLR8            6627
#define BMCLRTOTAL            8

//--- Editir colors dialog
#define IDD_EDITCLRDLG        6700
#define IDD_STCLREXAMPLE      6701
#define IDD_LBCLRITEMS        6702
#define IDD_CBCLRUNDERLINE    6703
#define IDD_VSCOLORS          6704

//--- Make color dialog
#define IDD_MAKECLRDLG        6800
#define IDD_STCLRMATRIX       6801
#define IDD_STCLRSCALE        6802
#define IDD_SPBRVAL           6803
#define IDD_SPBGVAL           6804
#define IDD_SPBBVAL           6805

//--- Edit associations list dialog
#define IDD_EDASSLISTDLG      6900
#define IDD_LBLASSLIST        6901
#define IDD_PBLASSMVUP        6902
#define IDD_PBLASSMVDOWN      6903
#define IDD_PBLASSNEW         6904
#define IDD_PBLASSEDIT        6905
#define IDD_PBLASSDUP         6906
#define IDD_PBLASSREM         6907
#define IDD_LBGASSLIST        6908
#define IDD_PBGASSMVUP        6909
#define IDD_PBGASSMVDOWN      6910
#define IDD_PBGASSNEW         6911
#define IDD_PBGASSEDIT        6912
#define IDD_PBGASSDUP         6913
#define IDD_PBGASSREM         6914
#define IDD_PBVARS            6915
#define IDD_PBASSCPYLG        6916
#define IDD_PBASSCPYGL        6917
#define IDD_PBASSMVLG         6918
#define IDD_PBASSMVGL         6919

//--- Find dialog
#define IDD_FINDDLG           7000
#define IDD_CBFIND            7001
#define IDD_CBCHANGETO        7002
#define IDD_PBFIND            7003
#define IDD_PBFINDGLOBAL      7004
#define IDD_PBFINDBACK        7005
#define IDD_PBCHANGE          7007
#define IDD_PBCHFIND          7008
#define IDD_PBCHALL           7009
#define IDD_PUSHBUTTON8       7010
#define IDD_CBCASESENSITIVE   7011
#define IDD_CBWHOLEWORD       7012

//--- File find dialog
#define IDD_FILEFINDDLG       7050
#define IDD_LBFFOUND          7051
#define IDD_CBFFIND           7052
#define IDD_CBFFCASE          7053
#define IDD_CBFFWHOLE         7054
#define IDD_CBFFSELFILES      7055


//--- Edit variables dialog
#define IDD_EDVARDLG          7100
#define IDD_LBVARS            7101
#define IDD_PBVARNEW          7102
#define IDD_PBVAREDNAME       7103
#define IDD_PBVAREDVALUE      7104
#define IDD_PBVARREM          7105

//--- Configure tools dialog


#define IDD_TOOLCFGDLG        7200
#define IDD_STBTNSHOW         7201
#define IDD_STEFTCATHOTKEY    7202
#define IDD_LBTCATS           7203
#define IDD_PBMVTCATUP        7204
#define IDD_PBMVTCATDOWN      7205
#define IDD_EFTCATMENU        7206
#define IDD_EFTCATCOMMENT     7207
#define IDD_PBTCATADD         7208
#define IDD_PBTCATREN         7209
#define IDD_PBTCATDUP         7210
#define IDD_PBTCATREM         7211
#define IDD_PBBTNCHANGE       7212
#define IDD_LBTOOLS           7213
#define IDD_PBMVTOOLUP        7214
#define IDD_PBMVTOOLDOWN      7215
#define IDD_PBTOOLADD         7216
#define IDD_PBTOOLREN         7217
#define IDD_PBTOOLDUP         7218
#define IDD_PBTOOLREM         7219
#define IDD_PBTOOLWIZ         7220
#define IDD_CBTOOLPRJRQ       7221
#define IDD_EFTOOLFNAMERQ     7222
#define IDD_MLETOOLSCRIPT     7223

//--- Tools: change button dialog
#define IDD_TOOLCHBTNDLG      7300
#define IDD_STBTNSIGN         7301
#define IDD_PBNOBTN           7302

//--- Tools: wizard dialog
#define IDD_TOOLWIZDLG        7500
#define IDD_STWTOOLNAME       7501
#define IDD_CBWEXECUTE        7503
#define IDD_PBWBROWSE         7504
#define IDD_RBWSEPARATE       7505
#define IDD_RBWPOPUP          7506
#define IDD_CBWKEEPSESSION    7507
#define IDD_EFWTITLE          7508
#define IDD_CBWUSEFILTER      7509
#define IDD_PBWEDFILTER       7510

//--- Tools: filter dialog
#define IDD_FILTERDLG         7550
#define IDD_MLEFILTER         7551
#define IDD_PBINSFILTER       7552

//--- Popup dialog:
#define IDD_POPUPDLG          7600
#define IDD_TXTCAPTION        7601
#define IDD_TXTPROGRESS       7602
#define IDD_TXTCOMMENT        7603
#define IDD_TXTERRORS         7604
#define IDD_TXTWARNINGS       7605
#define IDD_MLEOUTPUT         7606
#define IDD_PBSTOP            7607
#define IDD_PBCLOSE           7608
#define IDD_PBMSGLIST         7609
#define IDD_STPROGRESS        7610
#define IDD_STFLASH           7611

//--- New projecn dialog:
#define IDD_NEWPRJDLG         7700
#define IDD_EFNEWPRJNAME      7701
#define IDD_PBNEWPRJBROWSE    7702
#define IDD_CBNEWPRJTPLS      7703

//--- Modify project dialog
#define IDD_PRJMODDLG         7800
#define IDD_STPRJMODDISP      7801
#define IDD_STPRJMODCOMMENT   7802
#define IDD_LBPRJMOD          7803
#define IDD_PBUSROPT          7804
#define IDD_PBUSREQ           7805
#define IDD_PBMODULES         7806
#define IDD_PBLOOKUPS         7807
#define IDD_BASE_PRJMOD       7810 //..7899
//--- User options dialog
#define IDD_USROPTDLG         7851
#define IDD_LBUSROPTS         7852
#define IDD_PBUSROPTNEW       7853
#define IDD_PBUSROPTREM       7854
#define IDD_PBUSROPTED        7855

//--- About dialog
#define IDD_ABOUT             7900
#define IDD_ABOUTBMP          7901

//--- Macro monitor dialog
#define IDD_MACROMONDLG       8000
#define IDD_MLEMMONSCRIPT     8001
#define IDD_EFMMONCOMMENT     8002
#define IDD_STMMONKEY         8003
#define IDD_EFMMONFILE        8004
#define IDD_PBMMONDETAILS     8005

//--- Macro dialog
#define IDD_MACRODLG          8100
#define IDD_LBMACGLOB         8101
#define IDD_PBMACGLOBUP       8102
#define IDD_PBMACGLOBDOWN     8103
#define IDD_PBMACCP2LOC       8104
#define IDD_PBMACGLOBED       8105
#define IDD_PBMACGLOBNEW      8106
#define IDD_PBMACGLOBDUP      8107
#define IDD_PBMACGLOBREM      8108
#define IDD_LBMACLOC          8109
#define IDD_PBMACLOCUP        8110
#define IDD_PBMACLOCDOWN      8111
#define IDD_PBMACMV2GLOB      8112
#define IDD_PBMACLOCED        8113
#define IDD_PBMACLOCNEW       8114
#define IDD_PBMACLOCDUP       8115
#define IDD_PBMACLOCREM       8116

//--- Edit macro dialog
#define IDD_EDMACRODLG        8200
#define IDD_EFEDMCOMMENT      8201
#define IDD_STEDMKEY          8202
#define IDD_PBEDMKEY          8203
#define IDD_EFEDMFILE         8204
#define IDD_MLEEDMSCRIPT      8205


//--- Dialog definition file (*.dlg):
//
//  Reserved values:          9500
//                            ....
//                            9599
//

//--- MDI dialog(s):
//
//  Reserved values:          9600
//                            ....
//                            9699
//

//--- TOOLS:
//
//  Reserved values:
#define TOOL_MIN_ID           10000
// RC не понимает выражений Ж:-(
#define IDB_TOOL0             10000
#define IDB_TOOL1             10001
#define IDB_TOOL2             10002
#define IDB_TOOL3             10003
#define IDB_TOOL4             10004
#define IDB_TOOL5             10005
#define IDB_TOOL6             10006
#define IDB_TOOL7             10007
#define IDB_TOOL8             10008
#define IDB_TOOL9             10009
#define IDB_TOOL10            10010
#define IDB_TOOL11            10011
#define IDB_TOOL12            10012
#define IDB_TOOL13            10013
#define IDB_TOOL14            10014

//                            ....
//                            10099
//

#endif /* ifndef __XDSBASE__ */

