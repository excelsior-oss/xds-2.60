/*
*
*  File: ted.h
*
*  Text editor window class
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

//
// Note: TEDCLASS window don't uses it's QWL_USER window ULONG.
//
//

#ifndef __TED__
#define __TED__

  #include "hltdll.h"

  #define  TED_CFGFILENAME        "xdsedit.cfg"      // TED configuration file
  #define  TEDCLASS               "TEDWindowClass"   // TED window class name
  #define  fHiliteInit            TRUE               // Syntax highlighting initial state
  #define  fHiliteCurLineInit     FALSE              // Current line highlighting initial state

  #define  UNMAXDEEP              1000               // Maximal number of actions can be undone (>=2)
  #define  UNMAXSTORE             100000             // Total size of texts can be undone (after deletions)
  #define  MAXSHARED              30                 // Maximal shared windows possible



  /*>>> Module initialisation: must be called before use
  >
  > HAB     hAB          (in) - Ancor block
  > PSZ     szHomePath   (in) - Path to read configuration files
  > HMODULE hmResource   (in) - Resource module handle (or 0 to use application's .exe file)
  > ULONG   ulMovePtrId  (in) - Mouse pointer for mouse move operation resource id.
  > ULONG   ulCopyPtrId  (in) - Mouse pointer for mouse copy operation resource id.
  >
  > Returns: error message or 0
  */
  char *TedInit(HAB hAB, PSZ szHomePath, HMODULE hmResource, ULONG ulMovePtrId, ULONG ulCopyPtrId);



  /*>>> Function to be called when WM_CHAR message is not processed by the editor
  */
  typedef  MRESULT (*PFNCHARHANDLER)(HWND hWnd, MPARAM m1, MPARAM m2);

  /*>>> Control data structure must be passed when TEDCLASS window is created
  */
  struct TEDCTLDATA
  {
    SHORT          cb;
    PFNCHARHANDLER pfCH;
    HWND           hNotifyWindow; // Window to send notification messages
    HWND           hSharedEd;     // TED window to share buffer with (or 0)
  };
  typedef TEDCTLDATA *PTEDCTLDATA;

  typedef struct
  {
    LONG            x0,y0,cx,cy;
    RECTL           rclWin;
    BOOL            fMaximized;
    BOOL            fMinimized;
    LONG            xLeftScrShift;
    LONG            lFirstScrLine;
    LONG            lLine;
    LONG            lPos;
    BOOL            fHilite;
    BOOL            fHiliteCurLine;
    LONG            lTabSize;
    BOOL            fSmartTab;
    BOOL            fTabCharsMode;
    BOOL            fAutoidentMode;
    ULONG           ulBlockMode;
    BOOL            fInsertMode;
    LONG            lReserved1;
    LONG            lReserved2;
    char            szPainterId[PAINTIDLEN];
    FATTRS          fAttrs;
    CLP_PALETTETYPE edPalette;
  } EDCONF, *PEDCONF;



  /*>>> TED messages (in WM_USER+30000..WM_USER+30999 range)
  */
  #define TM_CLEAR                (WM_USER+30000)
  #define TM_CLEARALL             (WM_USER+30001)
  #define TM_COPY                 (WM_USER+30002)
  #define TM_CUT                  (WM_USER+30003)
  #define TM_EXPORT               (WM_USER+30004)
  #define TM_FIND                 (WM_USER+30005)
  #define TM_GETCURWORD           (WM_USER+30006)
  #define TM_GETCURWORD1          (WM_USER+30007)
  #define TM_GETSELTEXT           (WM_USER+30008)
  #define TM_GOBRACKET            (WM_USER+30009)
  #define TM_LOCKUPDATE           (WM_USER+30010)
  #define TM_PASTE                (WM_USER+30011)
  #define TM_QUERYCHANGED         (WM_USER+30012)
  #define TM_QUERYCOLORS          (WM_USER+30020)
  #define TM_QUERYEDCONF          (WM_USER+30021)
  #define TM_QUERYHILITE          (WM_USER+30022)
  #define TM_QUERYMACROS          (WM_USER+30023)
  #define TM_QUERYPAINTER         (WM_USER+30024)
  #define TM_QUERYREADONLY        (WM_USER+30025)
  #define TM_QUERYSEL             (WM_USER+30026)
  #define TM_QUERYSHARED          (WM_USER+30027)
  #define TM_QUERYTEXTLENGTH      (WM_USER+30028)
  #define TM_QUERYUNDO            (WM_USER+30029)
  #define TM_QUERYBUFSTR          (WM_USER+30030)
  #define TM_REDO                 (WM_USER+30031)
  #define TM_RESETUNDO            (WM_USER+30040)
  #define TM_SEARCH               (WM_USER+30041)
  #define TM_SELCURWORD           (WM_USER+30042)
  #define TM_SETCASE              (WM_USER+30043)
  #define TM_SETCHANGED           (WM_USER+30044)
  #define TM_SETCOLORS            (WM_USER+30045)
  #define TM_SETEDCONF            (WM_USER+30046)
  #define TM_SETEDFRAME           (WM_USER+30047)
  #define TM_SETFONT              (WM_USER+30048)
  #define TM_SETHILITE            (WM_USER+30049)
  #define TM_SETMACROS            (WM_USER+30050)
  #define TM_SETPAINTER           (WM_USER+30051)
  #define TM_SETREADONLY          (WM_USER+30052)
  #define TM_SETSEL               (WM_USER+30060)
  #define TM_SETSELSHOWUP         (WM_USER+30061)
  #define TM_SETSHARE             (WM_USER+30062)
  #define TM_SETTABSTOP           (WM_USER+30063)
  #define TM_SETTEXT              (WM_USER+30064)
  #define TM_UNDO                 (WM_USER+30065)

  #define TN_COMMENT              2000             // Set comment (PSZ m2)
  #define TN_CHANGESTATE          2001             // To display: m2 = 0/1/2 = NoChanged/Changed/ReadOnly
  #define TN_DESTROYED            2002             // TED (hEd==m2) destroyed
  #define TN_KILLFOCUS            2003             // TED (hEd==m2) loses the focus
  #define TN_LETMEDIE             2004             // TED (hEd==m2) wants to die; rc = TRUE => Ok.
  #define TN_SETFOCUS             2005             // TED (hEd==m2) recives the focus
  #define TN_TEXTCHANGED          2006             // Text is changed (m2=PTCD)
  #define TN_PALETTECHANGED       2007             // Palette is changed (m2=hEd)
  #define TN_POSCHANGED           2008             // Cursor position is changed (m2=PTPR)
  #define TN_SELPRESENTS          2009             // m2 is TRUE when selection presents
  #define TN_UNAVAILABLE          2030             // m2 is TRUE when UNDO is available


  /*>>> Text change description structure
  */
  struct TCD
  {
    enum         TCA {TCA_UNINITED, TCA_INSSTREAM, TCA_INSCOL, TCA_DELSTREAM,
                      TCA_DELCOL,   TCA_VMOVESEL,  TCA_MAXLINECHECK};
                 TCD();
    HWND         hEd;         // Our hEd
    TCA          tca;         // TCA_* constant (Text Change Action)
    LONG         lLine0;      // STREAM: old line;                     COL: upper line           VMOVESEL: Was 1-st sel. line
    LONG         lLine1;      // STREAM: line the old line becomed to; COL: last  line           VMOVESEL: Was last sel. line
    LONG         lPos0;       // STREAM: old pos;                      COL: left  pos
    LONG         lPos1;       // STREAM: pos the old pos becomed to;   COL: pos after the right
    LONG         lVJump;      //                                                                 VMOVESEL: VJump value
    LONG         lMaxLineNum; // All:    lMaxLineNum after the change
    void         Clear     ()
                           {tca=TCA_UNINITED;}
    void         InsStream (LONG lLin0, LONG lLin1, LONG lPs0, LONG lPs1, LONG lMxLineNum)
                           {tca=TCA_INSSTREAM; lLine0=lLin0; lLine1=lLin1; lPos0=lPs0; lPos1=lPs1; lMaxLineNum=lMxLineNum;}
    void         DelStream (LONG lLin0, LONG lLin1, LONG lPs0, LONG lPs1, LONG lMxLineNum)
                           {tca=TCA_DELSTREAM; lLine0=lLin0; lLine1=lLin1; lPos0=lPs0; lPos1=lPs1; lMaxLineNum=lMxLineNum;}
    void         InsCol    (LONG lLin0, LONG lLin1, LONG lPs0, LONG lPs1, LONG lMxLineNum)
                           {tca=TCA_INSCOL;    lLine0=lLin0; lLine1=lLin1; lPos0=lPs0; lPos1=lPs1; lMaxLineNum=lMxLineNum;}
    void         DelCol    (LONG lLin0, LONG lLin1, LONG lPs0, LONG lPs1, LONG lMxLineNum)
                           {tca=TCA_DELCOL;    lLine0=lLin0; lLine1=lLin1; lPos0=lPs0; lPos1=lPs1; lMaxLineNum=lMxLineNum;}
    void         VMoveSel  (LONG lLin0, LONG lLin1, LONG lVJmp)
                           {tca=TCA_VMOVESEL;  lLine0=lLin0; lLine1=lLin1; lVJump = lVJmp;}
    void         MaxLin    (LONG lMxLineNum)
                           {tca=TCA_MAXLINECHECK; lMaxLineNum=lMxLineNum;}
  };
  typedef TCD *PTCD;

  /*>>> Text position reference
  */
  struct TPR
  {
    LONG         lLine;
    LONG         lPos;
                 TPR        ()              {lLine = -1;}
                 TPR        (LONG l,LONG p) {lLine = l; lPos = p;}
    inline TPR   operator-  (TPR &r)        {return TPR(lLine-r.lLine,lPos-r.lPos);}
    inline TPR   operator+  (TPR &r)        {return TPR(lLine+r.lLine,lPos+r.lPos);}
    inline TPR   operator-= (TPR &r)        {lLine-=r.lLine; lPos-=r.lPos; return *this;}
    inline TPR   operator+= (TPR &r)        {lLine+=r.lLine; lPos+=r.lPos; return *this;}
    inline BOOL  operator== (TPR &r)        {return lLine==r.lLine && lPos==r.lPos;}
    inline BOOL  operator!= (TPR &r)        {return (!(*this == r));}
    inline BOOL  operator<  (TPR &r)        {return lLine<r.lLine ||
                                            (lLine==r.lLine && lPos<r.lPos);}
    inline BOOL  operator>  (TPR &r)        {return lLine>r.lLine ||
                                            (lLine==r.lLine && lPos>r.lPos);}
    inline BOOL  operator<= (TPR &r)        {return lLine<r.lLine ||
                                            (lLine==r.lLine && lPos<=r.lPos);}
    inline BOOL  operator>= (TPR &r)        {return lLine>r.lLine ||
                                            (lLine==r.lLine && lPos>=r.lPos);}
    inline void  Set        (LONG l, LONG p){lLine = l; lPos = p;}
    TPR         &UpdatePos  (PTCD ptcd);
  };
  typedef TPR *PTPR;


  /*>>> CHSTRUCT is used to recalculate TPR after text insertion/deletion
  */
  struct CHSTRUCT
  {
    TPR tprOld;
    TPR tprNew;
    CHSTRUCT () {tprOld.Set(0,0); tprNew.Set(0,0);}
    inline Change (TPR &tpr) // Refresh tpr
    {
      if (tprOld<tprNew && tpr>=tprOld)
      {
        if (tpr.lLine==tprOld.lLine)
        {
          tpr.lPos  += tprNew.lPos - tprOld.lPos;
          tpr.lLine  = tprNew.lLine;
        }
        else  tpr.lLine += tprNew.lLine-tprOld.lLine;
      }
      else if (tprOld>tprNew && tpr>tprNew)
      {
        if (tpr<=tprOld)             tpr = tprNew;
        else if (tpr.lLine==tprOld.lLine)
        {
          tpr.lPos  += tprNew.lPos - tprOld.lPos;
          tpr.lLine  = tprNew.lLine;
        }
        else  tpr.lLine += tprNew.lLine-tprOld.lLine;
      }
    }
  };

  //--- FINDSTRUCT:
  //
  #define FINDLINELEN   100
  enum    FINDACTION    { FA_FINDNEXT,     // Find from [tprPos...] -> EOF]
                          FA_FINDBACK,     // Find from BOF <- [tprPos...]
                          FA_FINDGLOBAL,   // Find from [0..] -> EOF
                          FA_MATCHSEL      // Matches current selection & the pattern
                        };
  typedef struct
  {
    char            szFind[FINDLINELEN+1];  // In: Pattern
    FINDACTION      fAction;                // In: Action
    BOOL            fWholeWord;             // In: find whole words
    BOOL            fCaseSensitive;         // In: match case
    BOOL            fGotoFound;             // In: when no FA_MATCHSEL: place selection on the result
    BOOL            fShowDownScreen;        // In: fGotoFound: try shift to the window's
    TPR             tprPos;                 // In: see fAction,  Out: tprPos[0..lLenFound[ = the word found
                                            // (In: if lLine<0 - the current cursor position assumed)
    LONG            lLenFound;              // In: <none>,       Out: tprPos[0..lLenFound[ = the word found
  } FINDSTRUCT, *PFINDSTRUCT;

extern char ed_szLastPattern[FINDLINELEN];  // Gr-r-r

#endif /* #ifndef __TED__ */

