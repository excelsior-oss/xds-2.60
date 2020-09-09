/*
*
*  File: ted.cpp
*
*  Text editor window class
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define  _DLL_USER_
#include "ted.h"
#include "sf_lib.h"
#include <REXXSAA.H>
#include "REXXCALL.H"

/* --- Internal messages --- */
//
#define MM_SUBSTART             (WM_USER+30500)

#define TID_MOUSESCROLL 10
#define MOUSE_COPY_NOW  0x0001
#define MOUSE_MOVE_NOW  0x0002

#define TCN_FOREGROUND  0
#define TCN_BACKGROUND  1

#define QWL_TEDTHIS     QWL_USER+4

#define DEL_CHAR      0x00000001
#define DEL_CHARLEFT  0x00000002
#define DEL_LINE      0x00000004
#define DEL_LINELEFT  0x00000008
#define DEL_LINERIGHT 0x00000010
#define DEL_SELECTION 0x00000020
#define DEL_WORD      0x00000040
#define DEL_WORDLEFT  0x00000080
#define DEL_WORDRIGHT 0x00000100

#define MOVSEL_UP       1
#define MOVSEL_DOWN     2
#define MOVSEL_LEFT     3
#define MOVSEL_RIGHT    4
#define MOVSEL_PAGEUP   5
#define MOVSEL_PAGEDOWN 6

#define BLOCK_STREAM  1 //
#define BLOCK_COLUMN  2 // This order!
#define BLOCK_LINES   3 //
#define BLOCK_COLUMNOVER 4 // Only for InsertText

extern CLP_PALETTETYPE aaRGBInit;
       CLP_PALETTETYPE aaRGBInit;

#ifndef Beep
  #define Beep() DosBeep(1200,40)
#endif

#define ISUNDONABLE(len) (len<=UNMAXSTORE/2)

/*---------- U N D O  d e f i n i t i o n s -----------*/
//
enum   UNOP
{
  UNOP_TYPESTR,
  UNOP_INSERT,
  UNOP_INSOVER,
  UNOP_DELETE,
  UNOP_MOVSEL,
  UNOP_DUPSEL,
  UNOP_MOVEMENT
};
struct UNOPTYPESTR
{
  LONG   lLine;           // Position
  LONG   lPos;            //    before the type
  LONG   lTypLen;         // Typed characters after this pos.
  char   chWas;           // Overwritten char  (pszWas = 0) | Both = 0
  PSZ    pszWas;          // Overwritten block (chWas  = 0) |    => insertion mode
  LONG   lWas;            // pszWas's allocated size
};
struct UNOPINSERT
{
  LONG   lLine;           // Position before
  LONG   lPos;            //    the insertion
  LONG   lEndLine;        // Position after
  LONG   lEndPos;         //    the insertion
  ULONG  ulBlockMode;     // Insertion mode
  LONG   lMaxLineNum0;    // For BLOCK_COLUMN mode
};
struct UNOPINSOVER
{
  LONG   lLine;           // Position before
  LONG   lPos;            //    the insertion
  LONG   lEndLine;        // Position after
  LONG   lEndPos;         //    the insertion
  LONG   lMaxLineNum0;    //
  PSZ    pszWas;          // Overwritten text
  LONG   lWas;            // pszWas's allocated size
};
struct UNOPDELETE
{
  LONG   lLine;           // Position before
  LONG   lPos;            //    the deletion
  LONG   lLine1;          // Deletion size
  LONG   lPos1;           //    (for redo)
  ULONG  ulBlockMode;     // Deletion mode
  PSZ    pszWas;          // Deletion
  LONG   lWas;            // pszWas's allocated size
};
struct UNOPMOVSEL
{
  LONG   lLineMin;        // Minimal moved line (before operation)
  LONG   lLineMax;        // Maximal moved line (before operation)
  LONG   lVJump;          // Vertical movement   (lHJump = 0)
  LONG   lHJump;          // Horizintal movement (lVJump = 0)
  LONG   l1stCol;         // For 'move over' mode: 1-st moved position
  LONG   lAfterCol;       // For 'move over' mode: pos. after moved (or -1 when mode is not 'move over')
};
struct UNOPDUPSEL
{
  LONG   lLineMin;        // LINES to delete
  LONG   lLineMax;        //    (for redo: it must be 1st copy of two)
};
struct UNOPMOVEMENT
{
};
struct UNDOSTR
{
  UNOP   unOperation;     // UNOP_* value
  LONG   lCurLine;        // Cursor
  LONG   lCurPos;         //    before
  LONG   lAncLine;        // Anchor
  LONG   lAncPos;         //    before
  union _un
  {
    UNOPTYPESTR   unoptypestr;
    UNOPINSERT    unopinsert;
    UNOPINSOVER   unopinsover;
    UNOPDELETE    unopdelete;
    UNOPMOVSEL    unopmovsel;
    UNOPDUPSEL    unopdupsel;
    UNOPMOVEMENT  unopmovement;
  } un;
};
typedef UNDOSTR *PUNDOSTR;

/*---------- R E D O   d e f i n i t i o n s -----------*/
//
enum   REOP
{
  REOP_TYPESTR,
  REOP_INSERT,
  REOP_INSOVER,
  REOP_DELETE,
  REOP_MOVSEL,
  REOP_DUPSEL,
  REOP_MOVEMENT
};
struct REOPTYPESTR
{
  LONG   lLine;           // Position before
  LONG   lPos;            //    the type
  PSZ    pszTxt;          // Text to type
  BOOL   fIns;            // Insert mode to re-type
};
struct REOPINSERT
{
  LONG   lLine;           // Position before
  LONG   lPos;            //    the insertion
  PSZ    pszTxt;          // Insertion
  ULONG  ulBlockMode;     // Insertion mode
};
struct REOPINSOVER
{
  LONG   lLine;           // Position before
  LONG   lPos;            //    the insertion
  PSZ    pszTxt;          // Column to insert over
};
struct REOPDELETE
{
  LONG   lLineOne;        // The
  LONG   lPosOne;         //   deletion
  LONG   lLineTwo;        //      sizes
  LONG   lPosTwo;         //        ...
  ULONG  ulBlockMode;     // Deletion mode
};
struct REOPMOVSEL
{
  LONG   lMin;            // Min. line to move
  LONG   lMax;            // Max. line to move
  LONG   lVJump;          // Vert. jump
  LONG   lHJump;          // Hor. jump
  LONG   l1stCol;         // For 'move over' mode: 1-st moved position
  LONG   lAfterCol;       // For 'move over' mode: pos. after moved (or -1 when mode is not 'move over')
};
struct REOPDUPSEL
{
  LONG   lLineMin;        // Lines to
  LONG   lLineMax;        //    duplicate
  BOOL   fUp;             // It was fUp duplication
};
struct REOPMOVEMENT
{
};
struct REDOSTR
{
  REDOSTR *previous;
  REOP     reOperation;   // REOP_* value
  LONG     lCurLine;      // Cursor
  LONG     lCurPos;       //    to set
  LONG     lAncLine;      // Anchor
  LONG     lAncPos;       //    to set
  union _un
  {
    REOPTYPESTR   reoptypestr;
    REOPINSERT    reopinsert;
    REOPINSOVER   reopinsover;
    REOPDELETE    reopdelete;
    REOPMOVSEL    reopmovsel;
    REOPDUPSEL    reopdupsel;
    REOPMOVEMENT  reopmovement;
  } un;
};
typedef REDOSTR *PREDOSTR;


/*>>> ===============  B U F F E R   class definition =====================
>
>
*/

#pragma pack(1)
struct LINESTR // sizeof(LINESTR) must be at least 2 bytes
{
  CLC_COMLEVELTYPE cl;
  char             tags;
  char             sz[1];
};
typedef LINESTR *PLINESTR;
#pragma pack()

class   TED;
typedef TED *PTED;

struct BUFSTR
{
  LONG      nUsers;              // Editors connected (1..MAXSHARED)
  TED      *aptUsers[MAXSHARED]; // Pointers to it's TEDs
  BOOL      fBufferChanged;      // Is buffer changed?
  BOOL      fBufferWasChanged;   // Was buffer changed (before fBufferChanged := FALSE)?
  BOOL      fReadOnly;           // ReadOnly mode
  BOOL      fLockWinUpdate;      // Lock window(s) update
  LONG      lAlloc;              // Allocated lines (in the apl)
  LONG      lMaxLineNum;         // Lines used (in the apl)
  LONG      lKnownCLevLine;      // Known line with the comlevel (-1 - don't knov)
  LONG      lKnownCLevLevel;     // This known comlevel
  PLINESTR *aPLines;             // LINESTR array
  // Undo+redo
  UNDOSTR   aUnStr[UNMAXDEEP];   // Undo
  int       nUnTop;              //    buf-
  int       nUnBottom;           //       fer
  ULONG     ulUnFreeStorage;     // ...
  PREDOSTR  pRedoTop;            // Redo list
  void      UnPush          (PUNDOSTR pus, PTPR ptprCur0, PTPR ptprAnc0);
  void      RePush          (PREDOSTR pre, PTPR ptprCur0, PTPR ptprAnc0);
  BOOL      UnDrop          ();
  BOOL      ReDrop          ();
  void      PurgeUndo       () {while(!UnDrop());}
  void      PurgeRedo       () {while(!ReDrop());}
  void      Purgen          () {PurgeRedo(); PurgeUndo();}
  //
  BUFSTR                 (PTED pted);   // Initiates buffer to be ready for use, pted is it's user
  ~BUFSTR                ();            // Deallocates all
  BOOL      Connect      (PTED pted);   // Add a new buffer's user
  void      Disconnect   (PTED pted);   // Remove one user (self-destruction occured when no users retained)
  PLINESTR *LimitLines   (LONG lLines); // Reallocates space for aPLines if it is need (usualy does nothing)
  void      ClearBuf     ();            // Clears all the contents
};
typedef BUFSTR          *PBUFSTR;
#define CURLINESZ        (pBuf->aPLines[tprCursor.lLine]->sz)

/*>>> ==================  T E D  class definition ========================
>
>   Pointer to this class instance is placed in the QWL_TEDTHIS window word
>     of the TED windows.
>
*/
class TED
{
  TED (HWND hEd, PFNCHARHANDLER pfCH, HWND hNotifyWindow, HWND hSharedEd);
  ~TED();


  /* Window */
  HWND            hEd;                    // TED window
  HWND            hEdFrame;               // TED's frame (may be 0)
  HWND            hNotifyWindow;          // Window to send notification messages
  HWND            hVScroll;               // V-scrollbar (may be 0)
  HPS             hpsEd;                  // hED's HPS
  PFNCHARHANDLER  pfnCH;                  // WM_CHAR handler
  CLP_PALETTETYPE aaRGB;                  // RGB color palette
  /* Sizes snd coordinates (use WM_SIZE to change them) */
  LONG            xWindowHeight;          // hEd's
  LONG            xWindowWidth;           // hEd's
  LONG            xLeftScrShift;          // Current horisontal shift (>=0)
  LONG            xHorScrollZone;         // Size of the left and right scroll zones
  LONG            xHorScrollStep;         // Step for horizontal scrolling jump
  LONG            lVertScrollZone;        // Upper and downer scroll zones (in Lines!)
  // Font characteristics:
  FATTRS          fAttrs;                 // Our font
  LONG            xMaxCharWidth;          // Widest character width
  LONG            xLineHeight;            // One line height
  LONG            xMaxDescender;          // Use it to rise up text drawing position
  LONG            xCurPosPix;             // Optimal horisontal position to place cursor
  LONG            xCurPosPixDraw;         // Real cursor position (for proport. fonts != xCurPosPix)
  LONG            lFirstScrLine;          // First visible line
  LONG            alCharWidths[256];      // Width of the each char in the selected font
  BOOL            fHilite;                // Highlight state ON/OFF
  BOOL            fHiliteCurLine;         // Highlight current line ON/OFF
  /* Colors */
  CLM_ELEMTYPE   *aclMap;                 // Pointer to DLL's map or to aMyMap array
  CLM_MAPTYPE     aMyMap;                 // my map used when no painters attached
  PPAINTER        pPainter;               // Attached painter or 0
  char            szPainterId[PAINTIDLEN];// Painter id. name or ""
  /* Text */
  TPR             tprAnchor;              // Anchor position
  TPR             tprCursor;              // Cursor position
  TCD             tcdStruct;              // Text change description structure
  ULONG           ulBlockMode;            // BLOCK_* value
  /* Move selection by mouse */
  BOOL            fMouseMoverNow;         // If mouse drag operation ON
  HPOINTER        hptrOld;                // Saved mouse pointer handle
  /* Buffer */
  PBUFSTR         pBuf;                   // Shared buffer
  /* Common data */
  static HAB      hAB;                   // Ancor block
  static LONG     lTabSize;              // Tab size
  static BOOL     fSmartTab;             // Smarttab mode
  static BOOL     fTabCharsMode;         // Use '\t' characters (not supported yet)
  static BOOL     fAutoidentMode;        // Autoident ON/OFF
  static BOOL     fInsertMode;           // Insert/overwrite text mode
  static HPOINTER hptrMove;              // Mouse move pointer
  static HPOINTER hptrCopy;              // Mouse copy pointer
  static BOOL     fScrollLock;           // Scroll lock state

  /* message processing support */
  MRESULT       TMClear          (MPARAM,MPARAM);  // Clear selection, returns TRUE if error
  MRESULT       TMClearAll       (MPARAM,MPARAM);  // Clear all
//MRESULT       TMCopy           (MPARAM,MPARAM);  // Copy sel to the clip.
//MRESULT       TMCut            (MPARAM,MPARAM);  // Cut sel to the clip.
  MRESULT       TMExport         (MPARAM,MPARAM);  // Returns: addr of the malloc()ed 0-terminated contents
  MRESULT       TMFind           (MPARAM,MPARAM);  // (PFINDSTRUCT m1) : fSuccess
  MRESULT       TMGetCurWord     (MPARAM,MPARAM);  // Returns: malloc()ed word or 0
  MRESULT       TMGetCurWord1    (MPARAM,MPARAM);  // Returns: malloc()ed word or 0; BOOL m1 - see on the left
  MRESULT       TMGetSelText     (MPARAM,MPARAM);  // (m1=pszBuf,m2=cb OR 0,0); Returns: strlen OR 0
  MRESULT       TMGoBracket      (MPARAM,MPARAM);  // Go to the pair bracket. Returns fOk.
  MRESULT       TMLockUpdate     (MPARAM,MPARAM);  // BOOL m1: lock/unlock (buffer scope area)
//MRESULT       TMPaste          (MPARAM,MPARAM);  // Insert clip.
  MRESULT       TMQueryChanged   (MPARAM,MPARAM);  // Returns: TRUE - buffer is (m1) ? (WAS changed) : (changed)
  MRESULT       TMQueryColors    (MPARAM,MPARAM);  // (CLP_PALETTE *m1);
  MRESULT       TMQueryEdConf    (MPARAM,MPARAM);  // Fill EDCONF (PEDCONF* m1);
//MRESULT       TMQueryHilite    (MPARAM,MPARAM);  // Returns: BOOL - highlight state;
//MRESULT       TMQueryMacros    (MPARAM,MPARAM);  // Returns pointer to MACROLIST;
  MRESULT       TMQueryPainter   (MPARAM,MPARAM);  // Returns PPAINTER (or 0)
  MRESULT       TMQueryReadOnly  (MPARAM,MPARAM)   {return MRESULT (pBuf ? pBuf->fReadOnly : TRUE);}
  MRESULT       TMQuerySel       (MPARAM,MPARAM);  // (PTPR m1 (cursor), PTPR m2 (anchor(or0))
//MRESULT       TMQueryTextLength(MPARAM,MPARAM);  // Returns text size
  MRESULT       TMQueryUndo      (MPARAM,MPARAM);  // Returns: TRUE if UNDO available
//MRESULT       TMRedo           (MPARAM,MPARAM);  // UnUndo
//MRESULT       TMResetUndo      (MPARAM,MPARAM);  // Reset undo buffer
//MRESULT       TMSearch         (MPARAM,MPARAM);  // Search opeartion(SEARCHSTRUCT* m1):fFound;
  MRESULT       TMSelCurWord     (MPARAM,MPARAM);  // Select the word; Returns TRUE when Ok
  MRESULT       TMSetCase        (MPARAM,MPARAM);  // Set word case : m1 0/1 == same/upper
  MRESULT       TMSetChanged     (MPARAM,MPARAM);  // Set new fBufferChanged state (BOOL m1);
  MRESULT       TMSetColors      (MPARAM,MPARAM);  // (CLP_PALETTETYPE m1);
  MRESULT       TMSetEdConf      (MPARAM,MPARAM);  // (EDCONF *m1);
  MRESULT       TMSetEdFrame     (MPARAM,MPARAM);  // (HWND m1);
  MRESULT       TMSetFont        (MPARAM,MPARAM);  // (PFATTRS m1, m2 - private use, must be 0);
  MRESULT       TMSetHilite      (MPARAM,MPARAM);  // (BOOL m1 = hlt syntax, m2 = cur. line);
//MRESULT       TMSetMacros      (MPARAM,MPARAM);  // Set new macros list (MACROLIST *m1);
  MRESULT       TMSetPainter     (MPARAM,MPARAM);  // Set new painter (m1 = szPainterId, m2 = msg ON/OFF)
  MRESULT       TMSetReadOnly    (MPARAM,MPARAM);  // Set readonly mode (BOOL m1)
  MRESULT       TMSetSel         (MPARAM,MPARAM);  // (PTPR m1, PTPR m2); - Cursor/Anchor pos
  MRESULT       TMSetSelShowUp   (MPARAM,MPARAM);  // .EQ. TMSetSel, tryes to move it to the top of the window
  MRESULT       TMSetShare       (MPARAM,MPARAM);  // (HWND m1): fOk m1 - hEd to share buffer with
//MRESULT       TMSetTabStop     (MPARAM,MPARAM);  // Set new TAB size (lTab m1);
  MRESULT       TMSetText        (MPARAM,MPARAM);  // Set editor text (Adr m1,Len m2):fErr;
  MRESULT       TMQueryShared    (MPARAM,MPARAM);  // Returns shared hEd #m1 or 0 if m1 is too match
//MRESULT       TMUndo           (MPARAM,MPARAM);  // Undo
//
//MRESULT       WMButton1Dblclk  (MPARAM,MPARAM);
  MRESULT       WMButton1Down    (MPARAM,MPARAM);
  MRESULT       WMButton1Up      (MPARAM,MPARAM);
  MRESULT       WMChar           (MPARAM,MPARAM);
  MRESULT       WMDestroy        (MPARAM,MPARAM);
  MRESULT       WMFocusChange    (MPARAM,MPARAM);
//MRESULT       WMMinMaxFrame    (MPARAM,MPARAM);
  MRESULT       WMMouseMove      (MPARAM,MPARAM);
  MRESULT       WMPaint          (MPARAM,MPARAM);
//MRESULT       WMSetFocus       (MPARAM,MPARAM);
  MRESULT       WMSize           (MPARAM,MPARAM);
  MRESULT       WMSysCommand     (MPARAM,MPARAM);
  MRESULT       WMTimer          (MPARAM,MPARAM);
  MRESULT       WMVScroll        (MPARAM,MPARAM);

  void          GotoCoord       (LONG lLine, LONG lPos, BOOL fUndo = FALSE, BOOL fShowOnly = FALSE);
  void          HideSel         (BOOL fNoNotify = FALSE);
  void          InvSel          (BOOL fCurOnly = FALSE);
  void          RestartCursor   ();
  void          SetVScroll      ();             // Set V-scrollbar position
  void          DrawNumLine     (LONG lLine);
  void          InvalidateLines (LONG l1, LONG l2, BOOL fCurOnly = FALSE);
  void          inv_lines       (LONG l1, LONG l2);
  BOOL          UpdateComLevels (LONG l1, LONG l2);
  void          ExportChState   (BOOL fAnyway = FALSE);
  void          TextChanged     ();
  void          UpdateTPRs      (PTCD ptcd, BOOL fActive);
  PLINESTR      cut_eolspc      (LONG lLine=-1); // Cuts current line's EOL spaces
  char         *GetWord         (char *pchLine, LONG lPos, PLONG plLen, BOOL fCheckSpaces);
  char         *AllocBlock      (BOOL fSharedMem, ULONG ulBlockMode = 0, PTPR ptpr1=0, PTPR ptpr2=0);
  BOOL          IsSelectedPos   (PTPR ptpr);
  TPR           Graf2Pos        (ULONG xy);
  void          TerminateMouseMover();                                                            // Terminate mouse drag operation
  MRESULT       InsertText      (char* pch, LONG lLen, BOOL fUndo, ULONG ulBlockMode=0);          //
  MRESULT       TypeString      (PSZ psz, BOOL fInsertMode=-1);
  MRESULT       DeleteAction    (ULONG ulDelMode);                                                // returns error state
  BOOL          DeleteBlock     (TPR tprOne, TPR tprTwo, ULONG ulBlockMode = 0, BOOL fUndo=TRUE); // returns error state
  LONG          MoveSel         (ULONG ulMSMode, LONG dy=0);                                      // dy!=0 || MOVSEL_* constant
  BOOL          move_sel        (LONG lLineMin, LONG lLineMax, LONG lVJump, LONG lHJump, BOOL fKeepHPos=0,
                                 LONG l1stCol=0, LONG lAfterCol=-1, PLONG pdy = 0);
  BOOL          DupSel          (BOOL fUp);                                                       // returns: error state
  ULONG         MessageBox      (ULONG ulFlags, char *szCaption, char *sz1, char *sz2=0, char *sz3=0);
//
  ULONG         ulUnMode;       // 0 - normal, 1 - UNDO is in progress now
  ULONG         ulReMode;       // 0 - normal, 1 - REDO is in progress now

  BOOL          UnTypeStr       (PTPR ptprCur0, PTPR ptprAnc0, LONG lTypLen, char chWas, PSZ pszWas);
  BOOL          UnInsert        (PTPR ptprCur0, PTPR ptprAnc0, LONG lLine, LONG lPos, LONG lEndLine,
                                 LONG lEndPos,  ULONG ulBlockMode, LONG lMaxLineNum0);
  BOOL          UnInsOver       (PTPR ptprCur0, PTPR ptprAnc0, LONG lLine, LONG lPos, LONG lEndLine,
                                 LONG lEndPos,  PSZ  pszWas, LONG lMaxLineNum0);
  BOOL          UnDelete        (PTPR ptprCur0, PTPR ptprAnc0, LONG lLine, LONG lPos, LONG lLine1, LONG lPos1, ULONG ulBlockMode, PSZ pszWas);
  BOOL          UnMovSel        (PTPR ptprCur0, PTPR ptprAnc0, LONG lLineMin, LONG lLineMax, LONG lVJump, LONG lHJump, LONG l1stCol, LONG lAfterCol);
  BOOL          UnDupSel        (PTPR ptprCur0, PTPR ptprAnc0, LONG lLineMin, LONG lLineMax);
  BOOL          UnMovement      (PTPR ptprCur0, PTPR ptprAnc0);

  BOOL          DoUnTypeStr     (UNOPTYPESTR  *pun);
  BOOL          DoReTypeStr     (REOPTYPESTR  *pre);
  BOOL          DoUnInsert      (UNOPINSERT   *pun);
  BOOL          DoReInsert      (REOPINSERT   *pre);
  BOOL          DoUnInsOver     (UNOPINSOVER  *pun);
  BOOL          DoReInsOver     (REOPINSOVER  *pre);
  BOOL          DoUnDelete      (UNOPDELETE   *pun);
  BOOL          DoReDelete      (REOPDELETE   *pre);
  BOOL          DoUnMovSel      (UNOPMOVSEL   *pun);
  BOOL          DoReMovSel      (REOPMOVSEL   *pre);
  BOOL          DoUnDupSel      (UNOPDUPSEL   *pun);
  BOOL          DoReDupSel      (REOPDUPSEL   *pre);
  BOOL          DoUnMovement    (UNOPMOVEMENT *pun);
  BOOL          DoReMovement    (REOPMOVEMENT *pre);
  BOOL          DoUndo          ();
  BOOL          DoRedo          ();
  void          UnNotify        (int nVal = -1);
  void          UnPush          (PUNDOSTR pus, PTPR ptprCur0, PTPR ptprAnc0) {pBuf->UnPush(pus,ptprCur0, ptprAnc0); UnNotify();}
  void          RePush          (PREDOSTR pre, PTPR ptprCur0, PTPR ptprAnc0) {pBuf->RePush(pre,ptprCur0, ptprAnc0);}
  BOOL          UnDrop          ()             {BOOL fRC = pBuf->UnDrop   (); UnNotify(); return fRC;}
  BOOL          ReDrop          ()             {return     pBuf->ReDrop   ();   }
  void          PurgeUndo       ()             {           pBuf->PurgeUndo(); UnNotify(FALSE); }
  void          PurgeRedo       ()             {           pBuf->PurgeRedo();   }
  void          Purgen          ()             {           pBuf->Purgen   ();   }

  friend  PSZ              TedInit (HAB hAB, PSZ szHomePath, HMODULE hmResource, ULONG ulMovePtrId, ULONG ulCopyPtrId);
  friend  MRESULT EXPENTRY fwpTed  (HWND,ULONG,MPARAM,MPARAM);

  friend  LONG    EXPENTRY EdCopy           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdDelete         (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdDupSel         (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdEnd            (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdEnter          (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdFindBracket    (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdGoto           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdFindWord       (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdFind           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdHome           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdMode           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdMouseBlock     (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdMove           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdMoveSel        (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdPage           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdPaste          (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdQuery          (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdRedo           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdSelWord        (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdWordCase       (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdTab            (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdType           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
  friend  LONG    EXPENTRY EdUndo           (PSZ,LONG,PRXSTRING,PSZ,PRXSTRING);
};

LONG     TED::lTabSize        = 8;
BOOL     TED::fSmartTab       = TRUE;
BOOL     TED::fTabCharsMode   = FALSE;
BOOL     TED::fInsertMode     = TRUE;
BOOL     TED::fAutoidentMode  = TRUE;
HAB      TED::hAB             = 0;
HPOINTER TED::hptrMove        = 0;
HPOINTER TED::hptrCopy        = 0;
HPOINTER TED::fScrollLock     = FALSE;



/*>>> - - - - - - - - - - - - - G L O B A L S : - - - - - - - - - - - -
*/
PTED   pCmdTed = 0;
ULONG  xyMouse;
char   ed_szLastPattern[FINDLINELEN] = "";

/*>>> - - - - - - - F U N C T I O N   P R O T O T Y P E S : - - - - - -
*/
void      RegisterTedCommands();
PLINESTR  AllocLine(LONG lLen);
void      FreeLine(PLINESTR pl);
PLINESTR  ReallocLine(PLINESTR pl, LONG lNewLen);
PLINESTR  MkLine(CLC_COMLEVELTYPE cl, char tags, char* pch, LONG cb);


/*>>> - - - - - - - SOME USEFUL FUNCTIONS & CLASSES : - - - - - -
*/
PLINESTR  AllocLine(LONG lLen)
{
  PLINESTR pl = (PLINESTR)malloc(lLen+sizeof(LINESTR));
  if (!pl) throw "Out of memory error (Location: AllocLine)";
  pl->cl = 0x7f;
  return pl;
}
void  FreeLine(PLINESTR pl)
{
  free(pl);
}
PLINESTR  ReallocLine(PLINESTR pl, LONG lNewLen)
{
  PLINESTR pl1;
  if (!(pl1 = (PLINESTR)realloc(pl,sizeof(LINESTR)+lNewLen)))
    throw "Out of memory error (Location: RellocLine)";
  pl1->sz[lNewLen] = '\0';
  return pl1;
}
PLINESTR  MkLine(CLC_COMLEVELTYPE cl, char tags, char* pch, LONG cb)
{
  PLINESTR pl   = AllocLine(cb);
  if (!pl) return 0;
  pl->cl     = cl;
  pl->tags   = tags;
  strncpy(pl->sz,pch,cb);
  pl->sz[cb] = '\0';
  return pl;
}


TCD::TCD() {memset(this, 0, sizeof(*this)); Clear();}

TPR& TPR::UpdatePos(PTCD p)
{
  switch(p->tca)
  {
    case TCD::TCA_UNINITED:
      return *this;
    case TCD::TCA_INSSTREAM:
      if (lLine>p->lLine0  ||  lLine==p->lLine0 && lPos>=p->lPos0)
      {
        if (lLine==p->lLine0) lPos += p->lPos1  - p->lPos0;
        lLine                      += p->lLine1 - p->lLine0;
      }
      break;
    case TCD::TCA_DELSTREAM:
      if (lLine>p->lLine0  ||  lLine==p->lLine0 && lPos>=p->lPos0)
      {
        if (lLine==p->lLine0) lPos -= p->lPos0  - p->lPos1;
        lLine                      -= p->lLine0 - p->lLine1;
      }
      else if (lLine>p->lLine1  ||  lLine==p->lLine1 && lPos>=p->lPos1)
      {
        lLine = p->lLine1;
        lPos  = p->lPos1;
      }
      break;
    case TCD::TCA_INSCOL:
      if (lLine >= p->lLine0 && lLine <= p->lLine1 && lPos >= p->lPos0)
        lPos += p->lPos1 - p->lPos0;
      break;
    case TCD::TCA_DELCOL:
      if (lLine >= p->lLine0 && lLine <= p->lLine1)
      {
        if      (lPos >= p->lPos1) lPos -= p->lPos1 - p->lPos0;
        else if (lPos >= p->lPos0) lPos  = p->lPos0;
      }
      break;
    case TCD::TCA_VMOVESEL:
      if     (lLine >= p->lLine0 && lLine <= p->lLine1)
        lLine += p->lVJump;
      else if( p->lVJump<0 && (lLine>=p->lLine0 + p->lVJump && lLine<p->lLine0))
        lLine -= p->lLine1 - p->lLine0 + 1;
      else if( p->lVJump>0 && (lLine<=p->lLine1 + p->lVJump && lLine>p->lLine1))
        lLine += p->lLine1 - p->lLine0 + 1;
      break;
  }
  if (lLine>p->lMaxLineNum) lLine = p->lMaxLineNum;
  return *this;
}

BUFSTR::BUFSTR(PTED pted)
{
  memset(this,0,sizeof(*this));
  nUsers            = 1;
  aptUsers[0]       = pted;
  fBufferChanged    = FALSE;
  fBufferWasChanged = FALSE;
  fReadOnly         = FALSE;
  lAlloc            = 512;
  lKnownCLevLine    = -1;
  aPLines           = (PLINESTR*)malloc(sizeof(PLINESTR)*lAlloc);
  if (!aPLines)
    throw "Out of memory error (Location: BUFSTR:1)";
  if (!(aPLines[0] = MkLine(0,0,"",0)))
    throw "Out of memory error (Location: BUFSTR:2)";
  lMaxLineNum       = 0;
  nUnTop            = 0;
  nUnBottom         = 0;
  ulUnFreeStorage   = UNMAXSTORE;
  pRedoTop          = NULL;
}
BUFSTR::~BUFSTR()
{
  if (aPLines)
    for(LONG l=0; l<=lMaxLineNum; l++) free(aPLines[l]);
  free(aPLines);
  Purgen();
}
BOOL BUFSTR::Connect(PTED pted)
{
  if (nUsers<MAXSHARED)
  {
    aptUsers[nUsers++] = pted;
    return TRUE;
  }
  return FALSE;
}
void BUFSTR::Disconnect(PTED pted)
{
  for (int i = nUsers-1; i>=0; i--)
    if (aptUsers[i]==pted)
    {
      memmove(&aptUsers[i], &aptUsers[i+1], sizeof(aptUsers[0])*(nUsers-i-1));
      if (!(--nUsers))
        delete this;
      return;
    }
}
PLINESTR *BUFSTR::LimitLines(LONG lLines)
{
  lLines+=10;
  if (lAlloc > lLines && lAlloc < lLines*2) return aPLines;

  LONG lAlloc1 = 512;
  while(lAlloc1 < lLines) lAlloc1 *= 2;
  PLINESTR *apl = (PLINESTR*)realloc(aPLines,sizeof(PLINESTR)*lAlloc1);
  if (!apl) throw "Out of memory error (Location: LimitLines)";
  aPLines = apl;
  lAlloc  = lAlloc1;
  return apl;

}
void BUFSTR::ClearBuf()
{
  if (aPLines)
    for(LONG l=0; l<=lMaxLineNum; l++) free(aPLines[l]);
  LimitLines(512);
  lMaxLineNum       = 0;
  aPLines[0]        = MkLine(0,0,"",0);
  lKnownCLevLine    = -1;
  fBufferChanged    = 0;
  fBufferWasChanged = 0;
}
void BUFSTR::UnPush(PUNDOSTR pus, PTPR ptprCur0, PTPR ptprAnc0)
{
  pus->lCurLine  = ptprCur0->lLine;
  pus->lCurPos   = ptprCur0->lPos;
  pus->lAncLine  = ptprAnc0->lLine;
  pus->lAncPos   = ptprAnc0->lPos;
  int nTarg      = (nUnTop+1)%UNMAXDEEP;
  if (nTarg == nUnBottom) UnDrop();
  nUnTop         = nTarg;
  aUnStr[nUnTop] = *pus;
}


void BUFSTR::RePush(PREDOSTR pre, PTPR ptprCur0, PTPR ptprAnc0)
{
  pre->lCurLine  = ptprCur0->lLine;
  pre->lCurPos   = ptprCur0->lPos;
  pre->lAncLine  = ptprAnc0->lLine;
  pre->lAncPos   = ptprAnc0->lPos;
  PREDOSTR pre1  = (PREDOSTR)malloc(sizeof(REDOSTR));
  if (!pre1) throw "Out of memory error (Location: RePush)";
  memcpy(pre1,pre,sizeof(REDOSTR));
  pre1->previous = pRedoTop;
  pRedoTop       = pre1;
}

BOOL BUFSTR::UnDrop()
{
  if (nUnTop==nUnBottom) return TRUE;
  nUnBottom = (nUnBottom+1)%UNMAXDEEP;
  switch (aUnStr[nUnBottom].unOperation)
  {
  case UNOP_TYPESTR:
  {
    LONG len = aUnStr[nUnBottom].un.unoptypestr.lWas;
    if (len && aUnStr[nUnBottom].un.unoptypestr.pszWas)
    {
      ulUnFreeStorage += len;
      free(aUnStr[nUnBottom].un.unoptypestr.pszWas);
    }
    break;
  }
  case  UNOP_DELETE:
    ulUnFreeStorage += aUnStr[nUnBottom].un.unopdelete.lWas;
    free(aUnStr[nUnBottom].un.unopdelete.pszWas);
    break;
  case UNOP_INSOVER:
    ulUnFreeStorage += aUnStr[nUnBottom].un.unopinsover.lWas;
    free(aUnStr[nUnBottom].un.unopinsover.pszWas);
    break;
  }
  return FALSE;
}


BOOL BUFSTR::ReDrop()
{
  if (!pRedoTop) return TRUE;
  switch (pRedoTop->reOperation)
  {
  case REOP_TYPESTR:
    free (pRedoTop->un.reoptypestr.pszTxt);
    break;
  case REOP_INSERT:
    free(pRedoTop->un.reopinsert.pszTxt);
    break;
  case REOP_INSOVER:
    free(pRedoTop->un.reopinsover.pszTxt);
    break;
  }
  PREDOSTR pre = pRedoTop->previous;
  free(pRedoTop);
  pRedoTop     = pre;
  return FALSE;
}




/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*>>> Module initialisation: must be called before use
>
> HAB     hAB          (in) - Ancor block
> PSZ     szHomePath   (in) - Path to read configuration files
> HMODULE hmResource   (in) - Resource module handle (or 0 to use application's .exe file)
> ULONG   ulMovePtrId  (in) - Mouse pointer for mouse move operation resource id.
> ULONG   ulCopyPtrId  (in) - Mouse pointer for mouse copy operation resource id.
>
> Returns: errorlevel (0 - ok)
*/
PSZ TedInit(HAB hAB, PSZ szHomePath, HMODULE hmResource, ULONG ulMovePtrId, ULONG ulCopyPtrId)
{
  int         i;
  static BOOL fFirstCall = TRUE;
  if (!fFirstCall)         return 0;
  fFirstCall             = FALSE;

  TED::hAB      = hAB;
  TED::hptrMove = WinLoadPointer (HWND_DESKTOP, hmResource, ulMovePtrId);
  TED::hptrCopy = WinLoadPointer (HWND_DESKTOP, hmResource, ulCopyPtrId);

  if (!WinRegisterClass(hAB,TEDCLASS,(PFNWP)fwpTed,CS_SIZEREDRAW|CS_CLIPCHILDREN,8L))
    return "Can't registrate editor window class";

  char  szFull[CCHMAXPATH];
  int   l = strlen(szHomePath);
  strcpy(szFull,szHomePath);
  if (l && szFull[l-1] != '\\' && szFull[l-1] != '/') strcat(szFull,"\\");
  strcat(szFull, "\\" TED_CFGFILENAME);
  RegisterTedCommands();
  for (i=0; i<CLP_PALETTELEN; i++)
  {
    aaRGBInit[i][CLP_FOREGROUND] = 0x00000080;
    aaRGBInit[i][CLP_BACKGROUND] = 0x00f0f0e0;
  }
  aaRGBInit[CLP_DEFAULT]  [CLP_FOREGROUND] = 0x00000000;
  aaRGBInit[CLP_SELECTION][CLP_FOREGROUND] = 0x00ffffff;
  aaRGBInit[CLP_SELECTION][CLP_BACKGROUND] = 0x00808080;
  aaRGBInit[CLP_CURLINE]  [CLP_BACKGROUND] = 0x00e0e0e0;
  return 0;
}


/********************************************************************************************/
/*                                                                                          */
/*   T E D window procedure                                                                 */
/*                                                                                          */
/********************************************************************************************/

MRESULT EXPENTRY fwpTed (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  PTED pTed = 0;
  if (ulMsg == WM_CREATE)
  {
    WinDefWindowProc(hWnd,ulMsg,m1,m2);
    PTEDCTLDATA ptcdata = (PTEDCTLDATA)m1;
    try
    {
      pTed = new TED(hWnd,ptcdata->pfCH,ptcdata->hNotifyWindow,ptcdata->hSharedEd);
      if (!pTed) throw "Out of memory error (Location: new TED)";
    }
    catch (char *sz)
    {
      if (pTed) delete (pTed);
      WinMessageBox(HWND_DESKTOP,hWnd,sz,"Editor",0,MB_ERROR|MB_OK|MB_MOVEABLE);
      return MRESULT(1);
    }
    WinSetWindowULong(hWnd,QWL_TEDTHIS,ULONG(pTed));
    return 0;
  }
  if (!(pTed = (PTED)WinQueryWindowULong(hWnd,QWL_TEDTHIS)))
    return WinDefWindowProc(hWnd,ulMsg,m1,m2);

  switch (ulMsg)
  {
  case TM_CLEAR:        return pTed->TMClear        (m1,m2);
  case TM_CLEARALL:     return pTed->TMClearAll     (m1,m2);
  case TM_EXPORT:       return pTed->TMExport       (m1,m2);
  case TM_FIND:         return pTed->TMFind         (m1,m2);
  case TM_GETCURWORD:   return pTed->TMGetCurWord   (m1,m2);
  case TM_GETCURWORD1:  return pTed->TMGetCurWord1  (m1,m2);
  case TM_GETSELTEXT:   return pTed->TMGetSelText   (m1,m2);
  case TM_GOBRACKET:    return pTed->TMGoBracket    (m1,m2);
  case TM_LOCKUPDATE:   return pTed->TMLockUpdate   (m1,m2);
  case TM_QUERYCHANGED: return pTed->TMQueryChanged (m1,m2);
  case TM_QUERYCOLORS:  return pTed->TMQueryColors  (m1,m2);
  case TM_QUERYEDCONF:  return pTed->TMQueryEdConf  (m1,m2);
  case TM_QUERYPAINTER: return pTed->TMQueryPainter (m1,m2);
  case TM_QUERYREADONLY:return pTed->TMQueryReadOnly(m1,m2);
  case TM_QUERYSEL:     return pTed->TMQuerySel     (m1,m2);
  case TM_QUERYUNDO:    return pTed->TMQueryUndo    (m1,m2);
  case TM_SELCURWORD:   return pTed->TMSelCurWord   (m1,m2);
  case TM_SETCASE:      return pTed->TMSetCase      (m1,m2);
  case TM_SETCHANGED:   return pTed->TMSetChanged   (m1,m2);
  case TM_SETCOLORS:    return pTed->TMSetColors    (m1,m2);
  case TM_SETEDCONF:    return pTed->TMSetEdConf    (m1,m2);
  case TM_SETEDFRAME:   return pTed->TMSetEdFrame   (m1,m2);
  case TM_SETFONT:      return pTed->TMSetFont      (m1,m2);
  case TM_SETHILITE:    return pTed->TMSetHilite    (m1,m2);
  case TM_SETPAINTER:   return pTed->TMSetPainter   (m1,m2);
  case TM_SETREADONLY:  return pTed->TMSetReadOnly  (m1,m2);
  case TM_SETSEL:       return pTed->TMSetSel       (m1,m2);
  case TM_SETSELSHOWUP: return pTed->TMSetSelShowUp (m1,m2);
  case TM_SETSHARE:     return pTed->TMSetShare     (m1,m2);
  case TM_SETTEXT:      return pTed->TMSetText      (m1,m2);
  case TM_QUERYBUFSTR:  return MRESULT(pTed->pBuf);
  case TM_QUERYSHARED:  return pTed->TMQueryShared  (m1,m2);
  case WM_BUTTON1DOWN:  return pTed->WMButton1Down  (m1,m2);
  case WM_BUTTON1UP:    return pTed->WMButton1Up    (m1,m2);
  case WM_CHAR:         return pTed->WMChar         (m1,m2);
  case WM_DESTROY:      return pTed->WMDestroy      (m1,m2);
  case WM_FOCUSCHANGE:  return pTed->WMFocusChange  (m1,m2);
  case WM_MOUSEMOVE:    return pTed->WMMouseMove    (m1,m2);
  case WM_PAINT:        return pTed->WMPaint        (m1,m2);
  case WM_SIZE:         return pTed->WMSize         (m1,m2);
  case WM_SYSCOMMAND:   return pTed->WMSysCommand   (m1,m2);
  case WM_TIMER:        return pTed->WMTimer        (m1,m2);
  case WM_VSCROLL:      return pTed->WMVScroll      (m1,m2);
  }
  return WinDefWindowProc(hWnd,ulMsg,m1,m2);
}




/********************************************************************************************/
/*                                                                                          */
/*   T E D   c l a s s                                                                      */
/*                                                                                          */
/********************************************************************************************/

TED::TED (HWND hEd, PFNCHARHANDLER pfCH, HWND hNotifyWindow, HWND hSharedEd)
{

  SWP             swp;

  memset(this,0,sizeof(*this));

  if (hSharedEd)
  {
    pBuf = (PBUFSTR)WinSendMsg(hSharedEd,TM_QUERYBUFSTR,0,0);
    if (!pBuf)                throw "Internal error: can't share window";
    if (!pBuf->Connect(this)) throw "Too many shared windows";
  }
  else // Create our own buffer
  {
    pBuf     = new BUFSTR(this);
    if (!pBuf) throw "Out of memory error (Location: TED constructor)";
  }

  TED::hEd               = hEd;
  TED::hNotifyWindow     = hNotifyWindow;
  pfnCH                  = pfCH;
  hEdFrame               = 0;
  hVScroll               = 0;
  hpsEd                  = WinGetPS(hEd);
  GpiSetBackMix(hpsEd,BM_OVERPAINT);
  GpiCreateLogColorTable(hpsEd,LCOL_PURECOLOR,LCOLF_RGB,0,0,0);
  memcpy(aaRGB,aaRGBInit,sizeof(aaRGB));
  WinPostMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_PALETTECHANGED),MPARAM(hEd));
  ulBlockMode            = BLOCK_STREAM;

  WinQueryWindowPos(hEd,&swp);
  xWindowHeight          = swp.cy;
  xWindowWidth           = swp.cx;
  xLeftScrShift          = 0;
  xHorScrollZone         = min(xWindowWidth/5,30);
  xHorScrollStep         = max(xHorScrollZone,1);
  lVertScrollZone        = (xWindowHeight>=xLineHeight*4) ? 1:0;

  {
    FATTRS fAt;
    memset(&fAt,0,sizeof(fAt));
    strcpy(fAt.szFacename,"Courier");
    fAt.usRecordLength    = sizeof(fAt);
    fAt.lMatch            = 10;
    fAt.lMaxBaselineExt   = 16;
    fAt.lAveCharWidth     = 9;
    fAt.fsFontUse         = 2;
    TMSetFont(MPARAM(&fAt),MPARAM(TRUE));
  }

  xCurPosPix             = 0;
  xCurPosPixDraw         = 0;
  lFirstScrLine          = 0;
  fHilite                = fHiliteInit;
  fHiliteCurLine         = fHiliteCurLineInit;
  aclMap                 = aMyMap;

  tprAnchor              .Set(0,0);
  tprCursor              .Set(0,0);

  fMouseMoverNow         = FALSE;

  ulUnMode               = 0;
  ulReMode               = 0;
  fScrollLock            = 0x0001 & WinGetKeyState(HWND_DESKTOP,VK_SCRLLOCK);

  WinInvalidateRect(hEd,0,FALSE);
  SetVScroll();
} // end TED::TED;


TED::~TED()
{
  if (hpsEd)
    WinReleasePS(hpsEd);
  if (pBuf) pBuf->Disconnect(this);
  Purgen();
}



void TED::GotoCoord(LONG lLine, LONG lPos, BOOL fUndo, BOOL fShowOnly)
{
  LONG          i,j,k,fline,lshift,llen,xPos;
  char         *pch;

  if (fUndo && !fShowOnly) UnMovement(&tprCursor,&tprAnchor);

  lLine = min(lLine, pBuf->lMaxLineNum);
  lLine = max(lLine, 0);
  if (!fShowOnly && lLine!=tprCursor.lLine)
  {
    if (fHiliteCurLine)
    {
      inv_lines(tprCursor.lLine, tprCursor.lLine);
      inv_lines(lLine,           lLine);
    }
    tprCursor.lLine = lLine;
  }
  pch   = pBuf->aPLines[lLine]->sz;
  llen  = strlen(pch);

  /* === If (lPos >= 0)
   *        change xCurPosPix
   *      else
   *        approach xCurPosPix with tprCursor.lPos
   * === Set xCurPosPixDraw on the real symbol.
  */
  if (lPos >= 0)
  {
    k = min(lPos,llen);
    for (i=0, j=0; i<k;    i++) j += alCharWidths[pch[i]];
    for (        ; i<lPos; i++) j += alCharWidths[' '];
    xPos = j;
    if (!fShowOnly)
    {
      xCurPosPix     = xCurPosPixDraw = j;
      tprCursor.lPos = lPos;
    }
  }
  else
  {
    if (!fShowOnly)
    {
      for (i=0, j=0; i<llen; i++)
      {
        if (j>=xCurPosPix) break;
        j += alCharWidths[pch[i]];
      }
      if (j<xCurPosPix)
      {
        k = (xCurPosPix-j)/alCharWidths[' '] + 1; // Blanks after EOL
        i += k;
        j += k*alCharWidths[' '];
      }
      xCurPosPixDraw = j;
      tprCursor.lPos = i;
    }
    else xPos = xCurPosPixDraw;
  }

  /* === Now: all the tuning are complete, it's time to *
  *      examine cursor screen position and move window *
  *      contents if it is need.                        *
  *                                                     *
  * Referensed coordinates are xPos in the lLine       */

  /* === fline := new lFirstScrLine */
  fline = lFirstScrLine;
  {
    UINT scrlines = xWindowHeight/xLineHeight;
    if (!scrlines) scrlines++;
    if (fScrollLock)
      fline = (lLine>scrlines/2) ? lLine-scrlines/2 : 0;
    else
    {
      if (lLine < lFirstScrLine+lVertScrollZone)
        fline = (lLine>lVertScrollZone)?lLine-lVertScrollZone:0;
      else
      {
        if (lLine+1 > lFirstScrLine+scrlines-lVertScrollZone)
          fline = lLine+1-scrlines+lVertScrollZone;
      }
    }
  }
  /* === lshift := new xLeftScrShift */
  lshift = xLeftScrShift;
  if (xPos < xLeftScrShift+xHorScrollZone)
    if(xPos < xHorScrollZone)
      lshift = 0;
    else
      lshift = ((xPos - xHorScrollZone)/xHorScrollStep)*xHorScrollStep;
  else if (xPos > xLeftScrShift+xWindowWidth-xHorScrollZone)
    lshift = ((xPos-xWindowWidth+xHorScrollZone+xHorScrollStep-1)/
              xHorScrollStep)*xHorScrollStep;

  if ((lshift!=xLeftScrShift || fline!=lFirstScrLine) && xWindowWidth>1)
  {
    lFirstScrLine  = fline;
    xLeftScrShift  = lshift;
    WinInvalidateRect(hEd,0,TRUE);
  }
  RestartCursor();
  WinShowCursor(hEd,TRUE);
  SetVScroll();
  if (!fShowOnly && !pBuf->fLockWinUpdate)
    WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_POSCHANGED),MPARAM(&tprCursor));
}

void TED::HideSel(BOOL fNoNotify)
{
  if (tprAnchor!=tprCursor)
  {
    InvalidateLines(tprCursor.lLine,tprAnchor.lLine,TRUE);
    tprAnchor = tprCursor;
  }
  if (!fNoNotify && WinQueryFocus(HWND_DESKTOP)==hEd && !pBuf->fLockWinUpdate)
    WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(FALSE));
}
void TED::InvSel(BOOL fCurOnly)
{
  if (tprAnchor!=tprCursor)
    InvalidateLines(tprCursor.lLine,tprAnchor.lLine, fCurOnly);
}


void TED::RestartCursor()
{
  if (fMouseMoverNow || WinQueryFocus(HWND_DESKTOP)!=hEd)
  {
    WinDestroyCursor(hEd);
    return;
  }
  LONG nWdth = 2;
  if (!fInsertMode)
  {
    if (tprCursor.lLine>pBuf->lMaxLineNum) return;
    char *psz = CURLINESZ;
    nWdth     = alCharWidths[ (tprCursor.lPos<strlen(psz)) ? psz[tprCursor.lPos] : ' ' ];
  }
  WinCreateCursor(hEd,xCurPosPixDraw-xLeftScrShift,
                    xWindowHeight-xLineHeight*(tprCursor.lLine-lFirstScrLine+1),
                    nWdth,xLineHeight,CURSOR_FLASH,NULL);
  WinShowCursor(hEd,TRUE);
}

void TED::SetVScroll()
{
  if (!hVScroll) return;
  LONG lMaxLin     = pBuf->lMaxLineNum;
  ULONG cft        = lMaxLin/0x7fff + 1;
  ULONG visible    = xWindowHeight/xLineHeight;
  WinSendMsg(hVScroll,SBM_SETTHUMBSIZE,
             MPFROM2SHORT(SHORT(max(visible/cft,1)),SHORT((lMaxLin)/cft)),0);
  WinSendMsg(hVScroll,SBM_SETSCROLLBAR,MPARAM(lFirstScrLine/cft),
             MPFROM2SHORT(0,SHORT((max(lMaxLin,0))/cft)) );
  WinInvalidateRect(hVScroll,0,0);
}


// A little useful subr:
LONG sum_comlevels(LONG lStartLev, LONG lStartLine, PLINESTR aPLines[], LONG lLineNeed)
{
  LONG i;
  if (!aPLines || lLineNeed<0) return 0;
  if ((i=lStartLine)<0 || i>lLineNeed) i=0,lStartLev=0;
  for(;i<lLineNeed;i++) if((lStartLev+=aPLines[i]->cl)<0) lStartLev=0;
  return lStartLev;
}


void TED::DrawNumLine(LONG lLine)
{
  static char    szBlank[1]  = "";
  LONG           i,j;
  char          *pch;
  LONG           lLen;
  LONG           lCL;

  if (lLine > pBuf->lMaxLineNum)
  {
    lCL = 0;
    pch = szBlank;
  }
  else
  {
    lCL = sum_comlevels(pBuf->lKnownCLevLevel, pBuf->lKnownCLevLine, pBuf->aPLines, lLine);
    if (pCmdTed==this && (pBuf->lKnownCLevLine < lLine-200 || pBuf->lKnownCLevLine>lLine))
    {
      pBuf->lKnownCLevLine  = lLine;
      pBuf->lKnownCLevLevel = lCL;
    }
    pch = pBuf->aPLines[lLine]->sz;
  }
  lLen = strlen(pch);

  //--- Set color map:
  if (fHilite && pPainter) (*pPainter->PClClrLine)(pch, lCL);
  else                     cl_FillMap(aclMap,0,CLM_MAPLEN,CLP_DEFAULT);

  //--- Highlight selection
  if (tprAnchor != tprCursor)
  {
    LONG lBegLine,lBegPos,lEndLine,lEndPos;
    if (tprAnchor<tprCursor)
    {
      lBegLine = tprAnchor.lLine;    lBegPos  = tprAnchor.lPos;
      lEndLine = tprCursor.lLine;    lEndPos  = tprCursor.lPos;
    }
    else
    {
      lBegLine = tprCursor.lLine;    lBegPos  = tprCursor.lPos;
      lEndLine = tprAnchor.lLine;    lEndPos  = tprAnchor.lPos;
    }
    if (lLine>=lBegLine && lLine<=lEndLine)
    {
      if (((lLine>lBegLine && lLine<lEndLine) || ulBlockMode==BLOCK_LINES) && ulBlockMode != BLOCK_COLUMN)
        cl_FillMap(aclMap,0,CLM_MAPLEN,CLP_SELECTION);
      else
      {
        if (lBegLine!=lEndLine && ulBlockMode!=BLOCK_COLUMN)
        {
          if (lLine==lBegLine) lEndPos = CLM_MAPLEN;
          else                 lBegPos = 0;
          cl_FillMap(aclMap,lBegPos,lEndPos-lBegPos,CLP_SELECTION);
        }
        else
          cl_FillMap(aclMap,min(lBegPos,lEndPos),abs(lEndPos-lBegPos),CLP_SELECTION);
      }
    }
  }//-- Highlight selection


  //--- Draw line pch[0..lLen-1] with aclMap[] colors
  const short   RCL_MAXLEN = 100;
  LONG          alRgnLens[CLP_PALETTELEN];
  RECTL         arclRgns [CLP_PALETTELEN][RCL_MAXLEN];
  RECTL         rcl;
  POINTL        ptl;
  CLM_ELEMTYPE  cl;
  LONG          lLeft,lRight,lTop,lBottom;
  HRGN          hrgn, hrgnOld;

  memset(alRgnLens,0,sizeof(alRgnLens));

  lTop        = xWindowHeight - (lLine-lFirstScrLine)*xLineHeight;
  rcl.yTop    = lTop-1;
  lBottom     =
  rcl.yBottom = lTop-xLineHeight;
  ptl.y       = lBottom + xMaxDescender + 1;
  rcl.xLeft   = 0;
  rcl.xRight  = xWindowWidth;

  for ( i=0       ,  j=-xLeftScrShift ;
        i<lLen && j<-xMaxCharWidth ;
        j += alCharWidths[pch[i++]]
      );
  ptl.x = j; // Start draw x-position

  // -- Draw line pch[i..lLen] with aclMap[i...] colors
  // -- from ptl position ('i' may be out of map)
  //
  lLeft = lRight = ptl.x;
  if ((j=i) < CLM_MAPLEN)
  {
    cl = aclMap[j];
    do
    {
      if (cl==aclMap[j])
        lRight += alCharWidths[(j<lLen) ? pch[j] : ' '];
      else
      {
        arclRgns[cl][alRgnLens[cl]].xLeft   = lLeft;
        arclRgns[cl][alRgnLens[cl]].xRight  = lRight;
        arclRgns[cl][alRgnLens[cl]].yTop    = lTop;
        arclRgns[cl][alRgnLens[cl]].yBottom = lBottom;
        if (alRgnLens[cl]<RCL_MAXLEN-1) alRgnLens[cl]++;
        lLeft   = lRight;
        cl      = aclMap[j];
        lRight += alCharWidths[(j<lLen) ? pch[j] : ' '];
      }
      j++;
    } while (j < CLM_MAPLEN);
    if (lRight!=lLeft)
    {
        arclRgns[cl][alRgnLens[cl]].xLeft   = lLeft;
        arclRgns[cl][alRgnLens[cl]].xRight  = lRight;
        arclRgns[cl][alRgnLens[cl]].yTop    = lTop;
        arclRgns[cl][alRgnLens[cl]].yBottom = lBottom;
        if (alRgnLens[cl]<RCL_MAXLEN-1) alRgnLens[cl]++;
        lLeft = lRight;
    }
  }
  if (lLeft<=xWindowWidth)
  {
    cl = aclMap[CLM_MAPLEN-1];
    arclRgns[cl][alRgnLens[cl]].xLeft   = lLeft;
    arclRgns[cl][alRgnLens[cl]].xRight  = xWindowWidth;
    arclRgns[cl][alRgnLens[cl]].yTop    = lTop;
    arclRgns[cl][alRgnLens[cl]].yBottom = lBottom;
    if (alRgnLens[cl]<RCL_MAXLEN-1) alRgnLens[cl]++;
  }

  // --- Draw it!
  ULONG flOptions;
  ULONG ulFore;
  for (cl=0; cl<CLP_PALETTELEN; cl++)
  {
    if (j=alRgnLens[cl])
    {
      hrgn = GpiCreateRegion(hpsEd,j,arclRgns[cl]);
      GpiSetClipRegion(hpsEd,hrgn,&hrgnOld);

      ulFore = aaRGB[cl][TCN_FOREGROUND];
      GpiSetColor    (hpsEd, ulFore & CLP_COLORMASK );
      if (fHiliteCurLine && lLine==tprCursor.lLine && cl!=CLP_SELECTION)
        GpiSetBackColor(hpsEd, aaRGB[CLP_CURLINE][TCN_BACKGROUND] );
      else
        GpiSetBackColor(hpsEd, aaRGB[cl][TCN_BACKGROUND] );

      flOptions                                 = CHS_OPAQUE;
      if (ulFore & CLP_UNDERLINETAG) flOptions |= CHS_UNDERSCORE;

      GpiCharStringPosAt(hpsEd,&ptl,&rcl,flOptions,
                         min( max(lLen-i,0), 512),
                         pch+i, 0);
      GpiSetClipRegion(hpsEd,hrgnOld,&hrgn);
      GpiDestroyRegion(hpsEd,hrgn);
    }
  }
}

void TED::InvalidateLines (LONG l1, LONG l2, BOOL fCurOnly)
{
  if (fCurOnly) inv_lines(l1,l2);
  else          for (LONG l=pBuf->nUsers-1; l>=0; l--) pBuf->aptUsers[l]->inv_lines(l1,l2);
}
void TED::inv_lines(LONG l1, LONG l2)
{
  LONG lMostScrLine = lFirstScrLine+xWindowHeight/xLineHeight+1;
  LONG lMin = max(lFirstScrLine, min(l1,l2));
  LONG lMax = min(lMostScrLine,  max(l1,l2));
  if (lMin>lMostScrLine || lMax<lFirstScrLine) return;
  RECTL rcl = {0,
               xWindowHeight-(lMax-lFirstScrLine+1)*xLineHeight,
               xWindowWidth,
               xWindowHeight-(lMin-lFirstScrLine)*xLineHeight-1
              };
  WinInvalidateRect(hEd,&rcl,0);
}

BOOL TED::UpdateComLevels(LONG l1, LONG l2)
// Returns TRUE if some levels are changed
{
  l1                         = min(l1,pBuf->lMaxLineNum);
  l2                         = min(l2,pBuf->lMaxLineNum);
  BOOL              fRet     = FALSE;
  PLINESTR          pl, *ppl = pBuf->aPLines+l1;
  CLC_COMLEVELTYPE  cl;
  LONG              l;
  if (fHilite && pPainter)
  {
    CLC_PLINELEVEL pfLineLevel = pPainter->PClLineLevel;
    for (l = l1; l<=l2; l++)
    {
      pl = *(ppl++);
      cl = (*pfLineLevel)(pl->sz);
      if (cl!=pl->cl)
      {
        pl->cl = cl;
        if (!fRet)
        {
          fRet = TRUE;
          if (pBuf->lKnownCLevLine>l) pBuf->lKnownCLevLine = -1;
        }
      }
    }
    return fRet;
  }
  else
  {
    for (l = l1; l<=l2; l++)
    {
      pl     = *(ppl++);
      pl->cl = 0;
    }
    return FALSE;
  }
}

void TED::ExportChState(BOOL fAnyway)
{
  static int  nLastSent = -1;
  int         state     = pBuf->fBufferChanged ? 1 : (pBuf->fReadOnly ? 2 : 0);
  if (nLastSent!=state || fAnyway)
    WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_CHANGESTATE),MPARAM(state));
  nLastSent = state;
}

void TED::TextChanged()
{
  if (tcdStruct.tca==TCD::TCA_UNINITED) return;
  for (int i=pBuf->nUsers-1; i>=0; i--)
    pBuf->aptUsers[i]->UpdateTPRs(&tcdStruct,pBuf->aptUsers[i]==this);
  tcdStruct.hEd = hEd;
  WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_TEXTCHANGED),MPARAM(&tcdStruct));
  pBuf->fBufferChanged = TRUE;
}

void TED::UpdateTPRs(PTCD ptcd, BOOL fActive)
{
  if (!fActive)
  {
    if (tprCursor!=tprAnchor) InvSel();
    tprCursor.UpdatePos(ptcd);
    tprAnchor.UpdatePos(ptcd);
    if (tprCursor!=tprAnchor) InvSel();
  }
}

PLINESTR TED::cut_eolspc(LONG lLine)
{
  LONG     i;
  if (lLine<0) lLine = min(tprCursor.lLine,pBuf->lMaxLineNum);
  PLINESTR pl  = pBuf->aPLines[lLine];
  LONG     len = strlen(pl->sz);
  for(i = len; i>0 && (pl->sz[i-1]==' ' || pl->sz[i-1]=='\t');i--);
  if (i!=len && (pl = ReallocLine(pl,i)))
    pBuf->aPLines[lLine] = pl;
  return pl;
}

char *TED::GetWord(char *pchLine, LONG lPos, PLONG plLen, BOOL fCheckSpaces)
{
  // Finds word (if fCheckSpaces=TRUE - space area assumed as a word) around pch position.
  // Returns pointer to the word found or 0, plLen sets to the word length

  LONG  lRet;
  if (lPos>=strlen(pchLine)) return FALSE;
  char *pch = pchLine+lPos;

  if (!sf_iswordbreak(*pch))
  {
    while (pch!=pchLine && !sf_iswordbreak(pch[-1])) pch--;
    for(lRet=0; !sf_iswordbreak(pch[lRet]); lRet++);
  }
  else if (fCheckSpaces && (*pch==' ' || *pch=='\t'))
  {
    while (pch!=pchLine && (pch[-1]==' ' || pch[-1]=='\t')) pch--;
    for(lRet=0; pch[lRet]==' ' || pch[lRet]=='\t'; lRet++);
  }
  else
    return 0;
  *plLen = lRet;
  return pch;
}

char *TED::AllocBlock(BOOL fSharedMem, ULONG ulBlockMode,PTPR ptpr1, PTPR ptpr2)
// Allocates a memory using DosAllocSharedMem() of malloc() depending on fSharedMem state
// and store the selected block;
// Separates lines with CR+LF sequences;
// Terminates it with '\0'.
{
  if (!ptpr1) ptpr1 = &tprCursor;
  if (!ptpr2) ptpr2 = &tprAnchor;
  if (*ptpr1==*ptpr2) return 0;
  if (!ulBlockMode) ulBlockMode = TED::ulBlockMode;
  LONG tmp;
  LONG lBegLine,lBegPos,lEndLine,lEndPos,lLin;
  LONG lAlloc = 0;
  char *pchMem, *pchTarg;
  if (*ptpr1 < *ptpr2)
  {
    lBegLine = ptpr1->lLine;  lBegPos  = ptpr1->lPos;
    lEndLine = ptpr2->lLine;  lEndPos  = ptpr2->lPos;
  }
  else
  {
    lBegLine = ptpr2->lLine;  lBegPos  = ptpr2->lPos;
    lEndLine = ptpr1->lLine;  lEndPos  = ptpr1->lPos;
  }
  if (lEndLine>pBuf->lMaxLineNum) return 0;
  if (ulBlockMode==BLOCK_COLUMN)
  {
    if (lBegPos>lEndPos){tmp=lBegPos; lBegPos=lEndPos; lEndPos=tmp;}
    lAlloc = (lEndPos-lBegPos+2)*(lEndLine-lBegLine+1)+1;
  }
  else
  {
    for (lLin=lBegLine, lAlloc=1; lLin<=lEndLine; lLin++)
      lAlloc += strlen(pBuf->aPLines[lLin]->sz)+2;
  }
  if (ulBlockMode == BLOCK_LINES)
  {
    lBegPos = 0;
    lEndPos = 0x7fffffff;
  }
  if (fSharedMem) DosAllocSharedMem(PPVOID(&pchMem),NULL,lAlloc,PAG_COMMIT|OBJ_GIVEABLE|PAG_READ|PAG_WRITE);
  else            pchMem = (char*)malloc(lAlloc);
  if (!pchMem)  return 0;

  // Copy block
  pchTarg = pchMem;
  if (ulBlockMode==BLOCK_COLUMN)
  {
    LONG lWidth = lEndPos-lBegPos;
    LONG w;
    for (lLin=lBegLine; lLin<=lEndLine; lLin++)
    {
      tmp = strlen(pBuf->aPLines[lLin]->sz);
      w   = max(0,tmp-lBegPos);
      w   = min(w,lWidth);
      if (w)        memcpy(pchTarg,pBuf->aPLines[lLin]->sz+lBegPos,w);
      if (w<lWidth) memset(pchTarg+w,' ',lWidth-w);
      pchTarg[lWidth]   = CR;
      pchTarg[lWidth+1] = LF;
      pchTarg+=lWidth+2;
    }
  }
  else
  {
    char *pchSrc;
    LONG w;
    BOOL fCR;
    for (lLin=lBegLine; 1; lLin++)
    {
      pchSrc = pBuf->aPLines[lLin]->sz;
      w      = strlen(pchSrc);
      fCR    = lEndPos > w;
      if (lLin==lEndLine) w = min(w,lEndPos);
      if (lLin==lBegLine)
      {
        tmp = min(w,lBegPos);
        pchSrc += tmp;
        w      -= tmp;
      }
      memcpy(pchTarg,pchSrc,w);
      pchTarg   += w;
      pchTarg[0] = CR;
      pchTarg[1] = LF;
      if (lLin==lEndLine)
      {
        if (fCR) pchTarg += 2;
        break;
      }
      pchTarg += 2;
    }
  }
  *pchTarg = '\0';
  return pchMem;
}

PSZ ins_getsrclin(PBOOL pfLF, char **ppch,char *pchE)
// pfLF (out)    - set on when LF found (may be NULL);
// ppch (in/out) - In:  pointer to the string to process;
//                 out: pointer to the char after the processed
// pchE (in)     - pointer to the end of the string ('\0' terminates scanning too)
// Returns: malloc()ed line (w/o CR,LF)
{
  char *pch1, *pch2, *pch;
  PSZ  psz = 0;
  BOOL fLF = FALSE;
  pch      = *ppch;
  while(pch<pchE && *pch==CR) pch++;
  if ((pch1=pch)!=pchE)
  {
    if (!(pch1 = (char*)memchr(pch, LF, pchE-pch))) pch1 = pchE;
    else                                            fLF  = TRUE;
    int len = pch1-pch;
    pch2    = psz = (PSZ)malloc(len+1);
    if (!psz) throw "Out of memory error (Location: ins_getsrclin)";
    while(pch<pch1) if((*pch2=*(pch++))!=CR) pch2++;
    *pch2 = '\0';
  }
  if (fLF)  pch1++;
  if (pfLF) *pfLF = fLF;
  *ppch = pch1;
  return psz;
}

PSZ ins_tabs(PSZ pszLin, LONG lBegPos, LONG lTabSize)
{
  char *pch;
  PSZ   pszRet = pszLin;
  LONG  l      = strlen(pszRet);

  while (pch = strchr(pszRet,'\t'))
  {
    int pos = pch-pszRet;
    int len = lTabSize - pos%lTabSize - 1;
    if (len)
    {
      if (!(pszRet  = (PSZ)realloc(pszRet,l+len+1)))
        throw "Out of memory error (Location: ins_tabs)";
      memmove(pszRet+pos+len,pszRet+pos,l-pos);
    }
    memset (pszRet+pos,' ',len+1);
    l+=len;
  }
  return pszRet;
}


BOOL TED::IsSelectedPos(PTPR p)
{
  TPR  tprMin = min(tprCursor,tprAnchor);
  TPR  tprMax = max(tprCursor,tprAnchor);

  if (p->lLine<tprMin.lLine || p->lLine>tprMax.lLine)
    return FALSE;

  switch (ulBlockMode)
  {
    case BLOCK_LINES:
      return TRUE;
    case BLOCK_STREAM:
      return (*p>=tprMin && *p<tprMax);
    case BLOCK_COLUMN:
      return (p->lPos >= min(tprMin.lPos,tprMax.lPos) && p->lPos < max(tprMin.lPos,tprMax.lPos));
  }
  return FALSE;
}

TPR TED::Graf2Pos(ULONG xy)
{
  SHORT y0      = SHORT(xy>>16);
  SHORT x       = min( max(SHORT(xy), 0), xWindowWidth );
  SHORT y       = min( max(y0, 0),        xWindowHeight);
  LONG  xPos    = x+xLeftScrShift;
  LONG  i,j,k;
  char *pch;
  LONG  lLine,lPos;

  lLine = min((xWindowHeight-y)/xLineHeight+lFirstScrLine, pBuf->lMaxLineNum);
  if (!lVertScrollZone)
  {
    if      (y0>xWindowHeight) lLine>0  ? (lLine)-- : 00;
    else if (y0<=1)            lLine    = min(lLine+1,pBuf->lMaxLineNum);
  }

  pch = pBuf->aPLines[lLine]->sz;
  k   = strlen(pch);
  for (i=0,j=0; i<k; i++)
  {
    j+=alCharWidths[i];
    if (j>xPos) break;
  }
  if (j<xPos) i += (xPos-j)/alCharWidths[' '];
  lPos = i;
  TPR   tpr(lLine,lPos);
  return tpr;
}


void TED::TerminateMouseMover()
{
  if (fMouseMoverNow){
    fMouseMoverNow = 0;
    WinSetPointer(HWND_DESKTOP,hptrOld);
    Beep();
  }
}


MRESULT TED::InsertText (char* pchBl, LONG lLen, BOOL fUndo, ULONG ulBlockMode)
{
// Returns 0 - Success, else - Error
  if (pBuf->fReadOnly) {Beep(); return 0;}
  try
  {
    if (!ulBlockMode) ulBlockMode = TED::ulBlockMode;

    TPR       tprCur0      = tprCursor;
    TPR       tprAnc0      = tprAnchor;

    LONG      lPos1, lPos0, lLineMax, lLine0;
    LONG      lHeight      =  0;
    LONG      lMaxLineNum0 = pBuf->lMaxLineNum;
    PLINESTR *apl          = 0;
    char     *pchE         = pchBl+lLen;
    char     *pch0         = pchBl;
    PSZ       pszLin;
    LONG      l;
    BOOL      fCOver       = ulBlockMode==BLOCK_COLUMNOVER;
    PSZ       pszCOver     = 0;

    if (tprCursor.lLine>lMaxLineNum0)
      throw "Internal (not fatal) error (Location: Insert text: 0)";

    lPos1    = lPos0  = tprCursor.lPos;
    lLineMax = lLine0 = tprCursor.lLine;

    if (ulBlockMode==BLOCK_STREAM || ulBlockMode == BLOCK_LINES)
    {
      LONG lInsLines=0;
      char ch;
        for (l=0; ch=pchBl[l]; l++) if (ch==LF) lInsLines++;
      apl = (PLINESTR*)malloc(sizeof(PLINESTR)*(lInsLines+2));
      if (!apl) throw "Out of memory error (Location: InsertText:1)";
      ///--- Insert stream/lines block
      lInsLines    = 0;
      LONG lCurLen = strlen(pBuf->aPLines[lLine0]->sz);

      static char szEnd[] = "";
      LONG        lTabArg = lPos0;
      BOOL        fLF;
      do
      {
        if (pszLin = ins_getsrclin(&fLF,&pch0,pchE)) pszLin = ins_tabs(pszLin,lTabArg,lTabSize);
        else                                         pszLin = szEnd;
        lTabArg  = 0;
        l        = strlen(pszLin);

        if (!lInsLines && ulBlockMode==BLOCK_STREAM)
        {
          apl[0] = AllocLine(lPos0+l);
          memcpy(apl[0]->sz,pBuf->aPLines[lLine0]->sz,min(lPos0,lCurLen));
          if (lPos0>lCurLen) memset(apl[0]->sz+lCurLen, ' ', lPos0-lCurLen);
          memcpy(apl[0]->sz+lPos0,pszLin,l);
          apl[0]->sz[lPos0+l] = '\0';
          apl[0]->cl = pBuf->aPLines[lLine0]->cl;
        }
        // Last LF on BLOCK_LINES must die...
        else if (l || fLF || !lInsLines || ulBlockMode!=BLOCK_LINES)
          apl[lInsLines] = MkLine(0,0,pszLin,l);
        else
          lInsLines--;
        if (pszLin!=szEnd) free(pszLin);
        lInsLines++;
      }while(fLF);

      if (ulBlockMode==BLOCK_STREAM)
      {
        lPos1 = strlen(apl[lInsLines-1]->sz); // Position next to the inserted block
        if (lCurLen>lPos0) // Append the current line's tail
        {
          apl[lInsLines-1] = ReallocLine(apl[lInsLines-1],lPos1+lCurLen-lPos0);
          memcpy(apl[lInsLines-1]->sz+lPos1, pBuf->aPLines[lLine0]->sz+lPos0,lCurLen-lPos0);
          apl[lInsLines-1]->sz[lPos1+lCurLen-lPos0] = '\0';
        }
      }
      // Insert apl[0..lInsLines-1] lines instead/before the current line
      int instead = (ulBlockMode==BLOCK_STREAM);
      pBuf->LimitLines(pBuf->lMaxLineNum+lInsLines-instead);
      pBuf->lMaxLineNum += lInsLines-instead;
      if (instead) FreeLine(pBuf->aPLines[lLine0]);
      memmove(pBuf->aPLines+lLine0+lInsLines-instead,
              pBuf->aPLines+lLine0,
              (pBuf->lMaxLineNum+2-(lLine0+lInsLines))*sizeof(PLINESTR));
      memcpy (pBuf->aPLines+lLine0, apl, lInsLines*sizeof(PLINESTR));
      lLineMax = lLine0+lInsLines-instead;
      GotoCoord(lLineMax,lPos1,FALSE);
      //Now: Ok. lLineMax:lPos1 == new cursor pos; new pBuf->lMaxLineNum is set
    }
    else // BLOCK_COLUMN[OVER]
    {
      if (memchr(pchBl,'\t',lLen))
        throw "You can't insert text with the 'TAB' characters in the 'Column' selection mode.";

      LONG lWidth  = -1;
      while(pszLin = ins_getsrclin(0,&pch0,pchE))
      {
        l = strlen(pszLin);
        free(pszLin);
        if      (lWidth<0) lWidth = l;
        else if (lWidth!=l) throw "You can't insert the text in the 'Column' selection mode"
                                                " because its lines lengths are defferent.";
        lHeight++;
      }
      if (!lWidth || !lHeight) return 0;
      ///--- if (fCOver) clear space
      if (fCOver)
      {
        TPR tprTwo(min(pBuf->lMaxLineNum, tprCursor.lLine+lHeight-1), tprCursor.lPos+lWidth);
        pszCOver = AllocBlock(FALSE,BLOCK_COLUMN,&tprCursor,&tprTwo);
        if (pszCOver)
        {
          LONG len = strlen(pszCOver)+1;
          if (!ISUNDONABLE(len))
          {
            free(pszCOver);
            pszCOver = 0;
          }
        }
        if (!pszCOver)
        {
          if (MBID_YES!=MessageBox(MB_ICONQUESTION|MB_YESNO|MB_MOVEABLE,"Paste over","The deletion is "
              "too large to be undone. Delete anyway (all the UNDO information will be purged)?"))
          return MRESULT(1);
        }
        if (DeleteBlock(tprCursor,tprTwo,BLOCK_COLUMN,FALSE))
        {
            free(pszCOver);
            return MRESULT(1);
        }
      }

      ///--- Insert column block
      int   lLin;
      LONG  l0,l1;
      char *psz;
      for (lLin=lLine0,pch0=pchBl; lLin<lLine0+lHeight && (pszLin = ins_getsrclin(0,&pch0,pchE)); lLin++)
      {
        lLineMax = lLin;
        if (lLin>pBuf->lMaxLineNum)
        {
          pBuf->LimitLines(pBuf->lMaxLineNum+1);
          pBuf->aPLines[pBuf->lMaxLineNum+1] = MkLine(0,0,"",0);
          pBuf->lMaxLineNum++;
        }
        l0 = strlen(pBuf->aPLines[lLin]->sz);
        l1 = max(l0,lPos0)+lWidth;
        pBuf->aPLines[lLin] = ReallocLine(pBuf->aPLines[lLin],l1);
        psz = pBuf->aPLines[lLin]->sz;
        if (lPos0>l0) memset (psz+l0, ' ', lPos0-l0);
        else          memmove(psz+lPos0+lWidth,psz+lPos0,l0-lPos0);
        memcpy(psz+lPos0, pszLin, lWidth);
        psz[l1] = 0;
      }
      GotoCoord(lLine0, lPos1 = lPos0+lWidth, FALSE);
      //Now: lLineMax == max. changed line; lPos1 = right block's margin; pBuf->lMaxLineNum is set
    }
    for (l = lLine0; l<=lLineMax; l++) cut_eolspc(l);
    LONG lInvMax =
       (UpdateComLevels(lLine0,lLineMax) ||
        ulBlockMode==BLOCK_LINES ||
        ulBlockMode==BLOCK_STREAM && lLineMax!=lLine0) ? pBuf->lMaxLineNum+1000 : lLineMax;
    if (fUndo)
    {
      if (!fCOver)
       UnInsert (&tprCur0, &tprAnc0, lLine0, lPos0, lLineMax, lPos1, ulBlockMode, lMaxLineNum0);
      else if (pszCOver)
       UnInsOver(&tprCur0, &tprAnc0, lLine0, lPos0, lLineMax, lPos1, pszCOver,    lMaxLineNum0);
      else Purgen();
    }
    InvalidateLines(lLine0,lInvMax);

    //--- Account canges
    if (!fCOver)
    {
      if      (ulBlockMode==BLOCK_STREAM) tcdStruct.InsStream(lLine0, lLineMax, lPos0, lPos1, pBuf->lMaxLineNum);
      else if (ulBlockMode==BLOCK_LINES)  tcdStruct.InsStream(lLine0, lLineMax, 0,     0,     pBuf->lMaxLineNum);
      else                                tcdStruct.InsCol   (lLine0, lLineMax, lPos0, lPos1, pBuf->lMaxLineNum);
      TextChanged();
    }
  }
  catch (char *sz)
  {
    MessageBox(MB_ERROR|MB_OK|MB_MOVEABLE,"Insert text",sz);
    return MRESULT(1);
  }
  pBuf->fBufferChanged = TRUE;
  ExportChState();
  return 0;
}


MRESULT TED::TypeString(PSZ psz, BOOL fInsertMode)
{ //   Operation will be terminated on 0x0a (0x0d) character
  //   Hides selection
  // returns 0 - ok, else - error (buffer overflow)
  //
  if (pBuf->fReadOnly) {Beep(); return 0;}
  try
  {
    TPR     tprCur0 = tprCursor;
    TPR     tprAnc0 = tprAnchor;
    LONG    lLine0  = tprCursor.lLine;
    LONG    lPos0   = tprCursor.lPos;
    PSZ     pszCur  = CURLINESZ;
    int     lLen0   = strlen (pszCur);
    int     lIns    = strcspn(psz,"\r\n");
    BOOL    fApp    = lPos0>=lLen0;
    PSZ     pszWas  = 0;

    if (!lIns)                    return 0;
    if (lLine0>pBuf->lMaxLineNum) throw "Internal (not fatal) error (Location: TypeString: 0)";
    if (fInsertMode==-1)          fInsertMode = TED::fInsertMode;
    if(!fInsertMode && !fApp) // Over the line: store pszWas
    {
      LONG lWas = min(lLen0-lPos0, lIns);
      if (lWas>0)
      {
        if (!(pszWas = (PSZ)malloc(lWas+1)))
          throw "Out of memory error (Location: TypeString:1)";
        memcpy(pszWas,pszCur+lPos0,lWas);
        pszWas[lWas] = '\0';
      }
    }

    LONG lLen1 = fApp ? lPos0+lIns : (fInsertMode ? lLen0+lIns : max(lPos0+lIns, lLen0));
    pBuf->aPLines[lLine0] = ReallocLine(pBuf->aPLines[lLine0], lLen1);
    pszCur = pBuf->aPLines[lLine0]->sz;
    if (fInsertMode && !fApp) // Insert into the line
    {
      memmove(pszCur+lPos0+lIns, pszCur+lPos0, lLen0-lPos0+1);
      memcpy (pszCur+lPos0, psz, lIns);
      //--- Account canges
      tcdStruct.InsStream(lLine0, lLine0, lPos0, lPos0+lIns, pBuf->lMaxLineNum);
      TextChanged();
    }
    else  // Overwrite (or append)
    {
      memset(pszCur+lLen0, ' ', lLen1-lLen0);
      memcpy(pszCur+lPos0, psz, lIns);
      pszCur[lLen1] = '\0';
    }
    cut_eolspc();
    InvSel();
    GotoCoord(lLine0,lPos0+lIns,FALSE);
    tprAnchor = tprCursor;
    if (!pBuf->fLockWinUpdate)
      WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(FALSE));
    if(lIns==1) { UnTypeStr(&tprCur0, &tprAnc0, lIns, pszWas ? pszWas[0] : '\0', NULL  ); free(pszWas); }
    else          UnTypeStr(&tprCur0, &tprAnc0, lIns, (char)0,                   pszWas);
    LONG lInvMax = UpdateComLevels(lLine0,lLine0) ? pBuf->lMaxLineNum : lLine0;
    InvalidateLines(tprCursor.lLine,lInvMax);
  }
  catch (char *sz)
  {
    MessageBox(MB_ERROR|MB_OK|MB_MOVEABLE,"Type string",sz);
    return MRESULT(1);
  }
  pBuf->fBufferChanged = TRUE;
  ExportChState();
  return MRESULT(0);
}

MRESULT TED::DeleteAction (ULONG ulDelMode)
// returns 0 or error
{
  TPR    tprOne  = tprCursor;        //
  LONG   lPos    = tprCursor.lPos;   // We'll cut text in [tprOne .. (lLine:lPos)]
  LONG   lLine   = tprCursor.lLine;  //
  LONG   lCurLen = strlen(CURLINESZ);

  if (ulDelMode & (DEL_CHAR | DEL_CHARLEFT | DEL_LINELEFT | DEL_LINERIGHT | DEL_WORD | DEL_WORDLEFT | DEL_WORDRIGHT)
      && tprCursor!=tprAnchor)
    return DeleteAction(DEL_SELECTION);


  switch(ulDelMode)
  {
  case DEL_CHAR:
  del_char:
    lPos++;
    break;
  case DEL_CHARLEFT: // == Backspace
  del_charleft:
    if (lPos>lCurLen)
    {
      InvSel();
      GotoCoord(tprCursor.lLine,lPos-1,TRUE); // Left move only
      tprAnchor = tprCursor;
      WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(FALSE));
      return 0;
    }
    else if (lPos == 0)
    {
      if (lLine) --lLine, lPos = strlen(pBuf->aPLines[lLine]->sz);
      else       return 0;
    }
    else
      lPos--;
    break;
  case DEL_LINE:
    tprOne.lPos = 0;
    if (lLine==pBuf->lMaxLineNum) lPos = lCurLen;
    else                          lPos = 0, lLine++;
    break;
  case DEL_LINELEFT:
    tprOne.lPos = 0;
    lPos = min(lPos,lCurLen);
    break;
  case DEL_LINERIGHT:
    if (lPos>=lCurLen)
    {
      if (lLine==pBuf->lMaxLineNum) return 0;
      else                          lPos = 0; lLine++;
    }
    else
      lPos = lCurLen;
    break;
  case DEL_SELECTION:
    tprOne = tprAnchor;  // NOTE: Selection deletion depends on the current selection mode !
    break;
  case DEL_WORD:
  case DEL_WORDLEFT:
  case DEL_WORDRIGHT:
  {
    BOOL fLeft = (ulDelMode==DEL_WORDLEFT);

    // Right join:
    if (lPos >= lCurLen)
    {
      if (!fLeft)
        goto del_char;
      else if (lPos>lCurLen)
      {  // Jump left to EOL
        InvSel();
        GotoCoord(lLine,lCurLen,TRUE);
        tprAnchor = tprCursor;
        WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(FALSE));
        return 0;
      }
    }
    // Left join:
    if (!lPos && fLeft)
    {
      if (!lLine) return 0;
      --lLine;
      lPos = strlen(pBuf->aPLines[lLine]->sz);
      break;
    }

    // No join, within the line:
    LONG lLen;
    PSZ  pszCur   = CURLINESZ;
    char *pchWord = GetWord(pszCur, lPos-(fLeft ? 1 : 0), &lLen, TRUE);
    if (!pchWord)
    {
      if (fLeft) goto del_charleft;
      else       goto del_char;
    }
    if  (fLeft || ulDelMode==DEL_WORD) lPos        = pchWord-pszCur;
    if  (!fLeft)                       tprOne.lPos = pchWord-pszCur + lLen;
    break;
  }
  default: return 0; // Bug? Me?!
  }

  {
    TPR tprTwo(lLine,lPos);
    return (MRESULT)DeleteBlock(tprOne,tprTwo, (ulDelMode==DEL_SELECTION) ? 0 : BLOCK_STREAM);
  }
}



BOOL TED::DeleteBlock (TPR tprOne, TPR tprTwo, ULONG ulBlockMode, BOOL fUndo)
// returns error state
{
  LONG          lBegPos,lEndPos,lBegLine,lEndLine;
  TPR           tprCursor0   = tprCursor;
  TPR           tprAnchor0   = tprAnchor;
  PSZ           pszDeletion = 0;

  if (pBuf->fReadOnly) {Beep(); return 0;}
  try
  {
    if (tprOne.lLine>pBuf->lMaxLineNum || tprTwo.lLine>pBuf->lMaxLineNum)
      throw "Internal (not fatal) error (Location: DeleteBlock: 0)";
    if (!ulBlockMode) ulBlockMode = TED::ulBlockMode;

    if (tprOne==tprTwo) return 0;
    if (tprTwo<tprOne) // Swap them
    {
      TPR tpr = tprOne;
      tprOne  = tprTwo;
      tprTwo  = tpr;
    }

    if (ulBlockMode==BLOCK_LINES)
    {
      tprOne.lPos = 0;
      tprTwo.lPos = strlen(pBuf->aPLines[tprTwo.lLine]->sz)+1;
    }

    if (ulBlockMode!=BLOCK_COLUMN && tprTwo.lPos>strlen(pBuf->aPLines[tprTwo.lLine]->sz))
    {
      if (tprTwo.lLine<pBuf->lMaxLineNum) {tprTwo.lPos = 0; tprTwo.lLine++;}
      else                                 tprTwo.lPos  = strlen(pBuf->aPLines[tprTwo.lLine]->sz);
    }

    if (tprOne==tprTwo) return 0;

    lBegLine = tprOne.lLine;    lEndLine = tprTwo.lLine;
    lBegPos  = tprOne.lPos;     lEndPos  = tprTwo.lPos;

    if (fUndo)
    {// Can we undone it?
      pszDeletion = AllocBlock(FALSE, (ulBlockMode==BLOCK_LINES) ? BLOCK_STREAM : ulBlockMode ,&tprOne,&tprTwo);
      if (pszDeletion)
      {
        LONG len    = strlen(pszDeletion)+1;
        if (!ISUNDONABLE(len))
        {
          free(pszDeletion);
          pszDeletion = 0;
        }
      }
      if (!pszDeletion)
      {
        if (MBID_YES!=MessageBox(MB_ICONQUESTION|MB_YESNO|MB_MOVEABLE,"Delete","The deletion is "
            "too large to be undone. Delete anyway (all the UNDO information will be purged)?"))
        return 1;
      }
    }

    if (ulBlockMode==BLOCK_COLUMN)
    {
      if (lBegPos>lEndPos)
        {LONG l=lBegPos; lBegPos=lEndPos; lEndPos=l;}
      for (LONG lLine = lBegLine; lLine<=lEndLine; lLine++)
      {
        PSZ  psz = pBuf->aPLines[lLine]->sz;
        LONG l0  = strlen(psz);
        LONG w   = min(l0,lEndPos)-lBegPos;
        if (w>0)
        {
          PLINESTR pl = MkLine(pBuf->aPLines[lLine]->cl,0,psz,l0-w);
          if(l0>lEndPos) strcpy(pl->sz+lBegPos,psz+lEndPos);
          pl->sz[l0-w] = '\0';
          FreeLine(pBuf->aPLines[lLine]);
          pBuf->aPLines[lLine] = pl;
          cut_eolspc(lLine);
        }
      }
      InvSel();
      GotoCoord(min(tprCursor0.lLine,tprAnchor0.lLine),lBegPos,FALSE);
      tprAnchor = tprCursor;
      WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(FALSE));
      LONG lInvMax = UpdateComLevels(lBegLine,lEndLine) ? pBuf->lMaxLineNum+1000 : lEndLine;
      InvalidateLines(lBegLine,lInvMax);
      if (fUndo)
      {
        if (pszDeletion) UnDelete(&tprCursor0, &tprAnchor0, lBegLine, lBegPos,
                                  lEndLine, lEndPos, ulBlockMode, pszDeletion);
        else             Purgen();
      }
      //--- Account canges
      tcdStruct.DelCol(lBegLine, lEndLine, lBegPos, lEndPos, pBuf->lMaxLineNum);
      TextChanged();
      ExportChState();
      return FALSE;
    }

    // The block mode is BLOCK_LINE or BLOCK_STREAM //

    InvSel();
    GotoCoord(lBegLine,lBegPos,FALSE);
    tprAnchor = tprCursor;
    if (!pBuf->fLockWinUpdate)
      WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(FALSE));

    LONG lLine = lBegLine;;
    LONG l0    = strlen(CURLINESZ);
    LONG l1    = max(l0,lBegPos);
    LONG l11   = strlen(pBuf->aPLines[lEndLine]->sz)-lEndPos;
    if (l11>0) l1 += l11; // The resulting line length

    PLINESTR pl = AllocLine(l1);
    memset(pl->sz,' ',l1);
    memcpy(pl->sz,CURLINESZ,min(l0,lBegPos));
    if (l11>0) memcpy(pl->sz+lBegPos, pBuf->aPLines[lEndLine]->sz+lEndPos, l11);
    pl->sz[l1] = '\0';
    pl->cl     = pBuf->aPLines[lBegLine]->cl;

    for (LONG l=lBegLine; l<=lEndLine; l++) FreeLine(pBuf->aPLines[l]);
    pBuf->aPLines[lBegLine] = pl;
    cut_eolspc(lBegLine);
    memmove(&(pBuf->aPLines[lBegLine+1]), &(pBuf->aPLines[lEndLine+1]),
            sizeof(PLINESTR)*(pBuf->lMaxLineNum-lEndLine));
    pBuf->lMaxLineNum -= lEndLine-lBegLine;
    pBuf->LimitLines(pBuf->lMaxLineNum);

    LONG lInvMax = (UpdateComLevels(lBegLine,lBegLine) || (lBegLine!=lEndLine)) ? pBuf->lMaxLineNum+1000 : lLine;
    InvalidateLines(lLine,lInvMax);

    if (fUndo)
    {
      if (pszDeletion) UnDelete(&tprCursor0, &tprAnchor0, lBegLine, lBegPos,
                                lEndLine, lEndPos, ulBlockMode, pszDeletion);
      else             Purgen();
    }

    //--- Account canges
    tcdStruct.DelStream(lEndLine, lBegLine, lEndPos, lBegPos, pBuf->lMaxLineNum);
    TextChanged();
  }
  catch (char *sz)
  {
    MessageBox(MB_ERROR|MB_OK|MB_MOVEABLE,"Delete",sz);
    return TRUE;
  }
  pBuf->fBufferChanged = TRUE;
  ExportChState();
  return 0;
}

LONG TED::MoveSel(ULONG ulMSMode, LONG dy) // : dy
{
  LONG dyRet;
  LONG lLineMin  = tprCursor.lLine;
  LONG lLineMax  = tprAnchor.lLine;
  LONG lVJump    = dy;
  LONG lHJump    = 0;
  BOOL fNoLine   = (tprAnchor!=tprCursor) && !tprAnchor.lPos;
  LONG page      = (xWindowHeight+xLineHeight-1)/xLineHeight - 2;
  page           = max(page,1);

  if (tprAnchor<tprCursor)
  {
    lLineMin  =  tprAnchor.lLine;
    lLineMax  =  tprCursor.lLine;
    fNoLine   = !tprCursor.lPos;
  }

  fNoLine &= (ulBlockMode==BLOCK_STREAM);
  if (fNoLine) lLineMax--;
  if (!lVJump)
    switch(ulMSMode){
    case MOVSEL_UP:         lVJump = -1;     break;
    case MOVSEL_DOWN:       lVJump = +1;     break;
    case MOVSEL_LEFT:       lHJump = -1;     break;
    case MOVSEL_RIGHT:      lHJump = +1;     break;
    case MOVSEL_PAGEUP:     lVJump = -page;  break;
    case MOVSEL_PAGEDOWN:   lVJump = +page;  break;
    }
  if (lVJump>0 && fNoLine && tprCursor.lLine==pBuf->lMaxLineNum) return 0;
  LONG l1stCol   = min(tprCursor.lPos,tprAnchor.lPos);
  LONG lAfterCol = (lHJump && ulBlockMode==BLOCK_COLUMN) ? max(tprCursor.lPos,tprAnchor.lPos) : -1;
  move_sel(lLineMin,lLineMax,lVJump,lHJump,fNoLine,l1stCol,lAfterCol,&dyRet);
  return dyRet;
}

BOOL  TED::move_sel(LONG lLineMin, LONG lLineMax, LONG lVJump, LONG lHJump, BOOL fKeepHPos,
                    LONG l1stCol, LONG lAfterCol, PLONG pdy)
// returns TRUE when no movement is possible
// l1stCol, lAfterCol - for a column blocks
{
  if (pdy) *pdy = 0;
  if (pBuf->fReadOnly) {Beep(); return TRUE;}
  TPR  tprCursor0 = tprCursor;
  TPR  tprAnchor0 = tprAnchor;
  BOOL fMoveOver  = lAfterCol >= 0;
  lLineMin        = max(0,min(pBuf->lMaxLineNum,lLineMin)); // For diff.
  lLineMax        = max(0,min(pBuf->lMaxLineNum,lLineMax)); //       case
  try
  {
    if (lHJump)
    {
      LONG lLine;
      if (fMoveOver && (l1stCol==lAfterCol)) return TRUE;
      if (lHJump>0)
      {
        for(lLine=lLineMin; lLine<=lLineMax; lLine++)
        {
          PSZ  pszLine = pBuf->aPLines[lLine]->sz;
          LONG l0      = strlen(pszLine);
          char chIns   = (lAfterCol<l0) ? pszLine[lAfterCol] : ' ';
          pBuf->aPLines[lLine] = ReallocLine(pBuf->aPLines[lLine],l0+1);
          pszLine      = pBuf->aPLines[lLine]->sz;
          if (fMoveOver)
          {
            if (l1stCol<l0)
            {
              memmove(pszLine+l1stCol+1,pszLine+l1stCol,min(lAfterCol,l0)-l1stCol);
              pszLine[l1stCol] = chIns;
              pszLine[l0+1]    = '\0';
            }
          }
          else
          {
            memmove(pszLine+1,pszLine,l0);
            pszLine[0]    = ' ';
            pszLine[l0+1] = '\0';
          }
          if (!fKeepHPos && tprAnchor.lLine==lLine) tprAnchor.lPos++;
          if (!fKeepHPos && tprCursor.lLine==lLine) tprCursor.lPos++;
          cut_eolspc(lLine);
        }
        //--- Account canges
        tcdStruct.InsCol(lLineMin, lLineMax, 0, 1, pBuf->lMaxLineNum);
        TextChanged();
      }
      else // lHJump<0
      {
        if (fMoveOver)
        {
          if (!l1stCol) return TRUE;
        }
        else
          for(lLine=lLineMin; lLine<=lLineMax; lLine++)
            if (pBuf->aPLines[lLine]->sz[0] && pBuf->aPLines[lLine]->sz[0] != ' ') return TRUE;
        for(lLine=lLineMin; lLine<=lLineMax; lLine++)
        {
          PSZ  pszLine  = pBuf->aPLines[lLine]->sz;
          LONG l0       = strlen(pszLine);
          if (fMoveOver)
          {
            if (l1stCol <= l0)
            {
              if (l0<lAfterCol)
              {
                pBuf->aPLines[lLine] = ReallocLine(pBuf->aPLines[lLine],lAfterCol);
                pszLine              = pBuf->aPLines[lLine]->sz;
                memset(pszLine+l0,' ',lAfterCol-l0);
                pszLine[lAfterCol] = '\0';
                l0                 = lAfterCol;
              }
              char chIns = pszLine[l1stCol-1];
              memmove(pszLine+l1stCol-1,pszLine+l1stCol,lAfterCol-l1stCol);
              pszLine[lAfterCol-1] = chIns;
              cut_eolspc(lLine);
            }
          }
          else if(l0)
          {
            memmove(pszLine,pszLine+1,l0);
            pBuf->aPLines[lLine] = ReallocLine(pBuf->aPLines[lLine],l0-1);
          }
          if ((!fKeepHPos && tprAnchor.lLine==lLine) && tprAnchor.lPos) tprAnchor.lPos--;
          if ((!fKeepHPos && tprCursor.lLine==lLine) && tprCursor.lPos) tprCursor.lPos--;
        }
        //--- Account canges
        tcdStruct.DelCol(lLineMin, lLineMax, 0, 1, pBuf->lMaxLineNum);
        TextChanged();
      }
      GotoCoord(tprCursor.lLine,tprCursor.lPos,FALSE);
      UnMovSel(&tprCursor0, &tprAnchor0, lLineMin,lLineMax,lVJump,lHJump,l1stCol,lAfterCol);
      InvalidateLines(lLineMin,        lLineMax);
      InvalidateLines(tprAnchor.lLine, tprCursor.lLine);
      pBuf->fBufferChanged = TRUE;
      ExportChState();
      return FALSE;
    }
    // --- Vertical movement ---
    {
      if (lVJump>0) lVJump = min(lVJump,pBuf->lMaxLineNum-lLineMax);
      else          lVJump = max(lVJump,-lLineMin);
      if (pdy)      *pdy   = lVJump;
      if (!lVJump) return TRUE;

      LONG      lSwapMin = (lVJump<0) ? lLineMin+lVJump : lLineMax+1;
      LONG      lSwapMax = (lVJump<0) ? lLineMin-1      : lLineMax+lVJump;
      LONG      lSwapHgh = lSwapMax-lSwapMin+1;
      LONG      lSelHgh  = lLineMax-lLineMin+1;
      LONG      lCLin0   = min(pBuf->lMaxLineNum, max(lSwapMax,lLineMax)+1);
      LONG      lCLev0   = sum_comlevels(pBuf->lKnownCLevLevel, pBuf->lKnownCLevLine, pBuf->aPLines, lCLin0);
      PLINESTR *apl      = (PLINESTR*)malloc(sizeof(PLINESTR)*lSwapHgh);

      if (!apl) throw "Out of memory error (Location: MoveSel)";
      memcpy (apl,pBuf->aPLines+lSwapMin,sizeof(PLINESTR)*lSwapHgh);
      memmove(pBuf->aPLines+lLineMin+lVJump, pBuf->aPLines+lLineMin, sizeof(PLINESTR)*lSelHgh);
      memcpy (pBuf->aPLines + (lVJump<0 ? lSwapMin+lSelHgh : lLineMin), apl, sizeof(PLINESTR)*lSwapHgh);
      free(apl);

      if (pBuf->lKnownCLevLine > min(lLineMin,lSwapMin)) pBuf->lKnownCLevLine = -1;
      LONG lInvMax = (lCLev0==sum_comlevels(pBuf->lKnownCLevLevel, pBuf->lKnownCLevLine, pBuf->aPLines, lCLin0)) ?
                     max(lSwapMax,lLineMax) : pBuf->lMaxLineNum;
      tprAnchor.lLine += lVJump;
      GotoCoord(tprCursor0.lLine+lVJump, tprCursor.lPos, FALSE);
      InvalidateLines(min(lLineMin,lSwapMin), lInvMax);
      UnMovSel(&tprCursor0, &tprAnchor0, lLineMin,lLineMax,lVJump,lHJump,l1stCol,lAfterCol);
      //--- Account canges
      tcdStruct.VMoveSel(lLineMin, lLineMax, lVJump);
      TextChanged();
    }
  }
  catch (char *sz)
  {
    MessageBox(MB_ERROR|MB_OK|MB_MOVEABLE,"Move selection",sz);
    return TRUE;
  }
  pBuf->fBufferChanged = TRUE;
  ExportChState();
  return FALSE;
}

BOOL TED::DupSel(BOOL fUp)
{
  TPR  tprAnc0 = tprAnchor;
  TPR  tprCur0 = tprCursor;
  TPR  tprMin  = min(tprAnchor,tprCursor);
  TPR  tprMax  = max(tprAnchor,tprCursor);
  BOOL fEr     = FALSE;
  BOOL fNoLine = !tprMax.lPos && ulBlockMode==BLOCK_STREAM && tprAnchor!=tprCursor;

  tprMax.Set(tprMax.lLine - !!fNoLine,  tprMin.lPos+1);

  // Now: tprMin..tprMax represents (not empty) LINES block to duplicate.

  char *psz = AllocBlock(FALSE,BLOCK_LINES,&tprMin,&tprMax);
  if (!psz) return TRUE;

  GotoCoord(tprMin.lLine,0,FALSE);
  fEr = (BOOL)InsertText(psz,strlen(psz),FALSE,BLOCK_LINES);
  free(psz);
  if (!fEr)
  {
    LONG lSh = fUp ? 0 : (tprMax.lLine-tprMin.lLine+1);
    GotoCoord    (tprCur0.lLine + lSh, tprCur0.lPos, FALSE);
    tprAnchor.Set(tprAnc0.lLine + lSh, tprAnc0.lPos);
    // UnDupSel needs _first_ copy of lines (see redo)
    UnDupSel(&tprCur0, &tprAnc0, tprMin.lLine, tprMax.lLine);
  }
  InvalidateLines(tprMin.lLine,pBuf->lMaxLineNum);
  return fEr;
}


ULONG TED::MessageBox (ULONG ulFlags, char *szCaption, char *sz1, char *sz2, char *sz3)
{
  char sz[CCHMAXPATH+100];
  strcpy(sz,sz1);
  if (sz2) strcat (sz,sz2);
  if (sz3) strcat (sz,sz3);
  return WinMessageBox(HWND_DESKTOP,hEd,sz,szCaption,0,ulFlags);
}



/********************************************************************************************/
/*                                                                                          */
/*   Window message processing                                                              */
/*                                                                                          */
/********************************************************************************************/


MRESULT TED::TMClear (MPARAM m1, MPARAM m2)
{
  return DeleteAction(DEL_SELECTION);
}

MRESULT TED::TMClearAll (MPARAM m1,MPARAM m2)
{
  pBuf->Disconnect(this);
  pBuf             = new BUFSTR(this);
  xCurPosPix       = xCurPosPixDraw = 0;
  lFirstScrLine    = 0;
  fMouseMoverNow   = FALSE;
  tprAnchor.Set  ( 0, 0);
  tprCursor.Set  ( 0, 0);
  Purgen();
  ExportChState();
  WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(FALSE));
  return 0;
}


//====== TM_FIND area:

char  szF_Pattern[FINDLINELEN+1];
char  chF_Pattern;
int    nF_PatternLen;
BOOL   fF_Back;
BOOL   fF_WholeWord;
BOOL   fF_CaseSensitive;

inline BOOL find_whole_test(PSZ pszLine, char *pch)
{
  if (!fF_WholeWord)                           return TRUE;
  if (pch>pszLine && !sf_iswordbreak(pch[-1])) return FALSE;
  return sf_iswordbreak(pch[nF_PatternLen]);
}

LONG find_in_line(PSZ pszLine, LONG lLineLen, LONG lBegPos) // Ret: Pos | <negative>
{
  char *pch  = pszLine+lBegPos;
  char *pchE = pszLine+lLineLen-nF_PatternLen;
  BOOL  f;
  int   i;
  if (fF_Back)
  {
    if (pch>pchE) pch=pchE;
    for(;pch >= pszLine;pch--)
    {
      if (sf_toupper(*pch)!=chF_Pattern) continue;
      if (fF_CaseSensitive)
        f =!strncmp(szF_Pattern,pch,nF_PatternLen);
      else
      {
        f = TRUE;
        for (i=nF_PatternLen; --i >= 0; )
        {
          if (sf_toupper(pch[i])!=szF_Pattern[i])
          {
            f = FALSE;
            break;
          }
        }
      }
      if (f && find_whole_test(pszLine,pch)) return pch-pszLine;
    }
    return -1;
  }
  else
  {
    for(;pch<=pchE;pch++)
    {
      if (sf_toupper(*pch)!=chF_Pattern) continue;
      if (fF_CaseSensitive)
        f =!strncmp(szF_Pattern,pch,nF_PatternLen);
      else
      {
        f = TRUE;
        for (i=nF_PatternLen; --i >= 0; )
        {
          if (sf_toupper(pch[i])!=szF_Pattern[i])
          {
            f = FALSE;
            break;
          }
        }
      }
      if (f && find_whole_test(pszLine,pch)) return pch-pszLine;
    }
    return -1;
  }
}


MRESULT TED::TMFind(MPARAM m1, MPARAM m2)
{
  int         i;
  PFINDSTRUCT pfs     = (PFINDSTRUCT)m1;
  if (pfs->tprPos.lLine<0 || pfs->tprPos.lPos<0) pfs->tprPos = tprCursor;
  LONG        lLine   = pfs->tprPos.lLine;
  LONG        lPos    = pfs->tprPos.lPos;

  fF_Back             = pfs->fAction==FA_FINDBACK;
  fF_WholeWord        = pfs->fWholeWord;
  fF_CaseSensitive    = pfs->fCaseSensitive;
  nF_PatternLen       = strlen(pfs->szFind);
  chF_Pattern         = sf_toupper(pfs->szFind[0]);
  strcpy(szF_Pattern,   pfs->szFind);
  if (!fF_CaseSensitive) for (i=0; szF_Pattern[i]; i++) szF_Pattern[i] = sf_toupper(szF_Pattern[i]);

  if (pfs->fAction == FA_MATCHSEL)
  {
    lLine = tprCursor.lLine;
    if (lLine!=tprAnchor.lLine || lLine>pBuf->lMaxLineNum)   return MRESULT(FALSE);
    if (abs(tprAnchor.lPos-tprCursor.lPos) != nF_PatternLen) return MRESULT(FALSE);
    PSZ pszLine = pBuf->aPLines[lLine]->sz;
    lPos = min(tprAnchor.lPos,tprCursor.lPos);
    return MRESULT(lPos == find_in_line(pszLine, strlen(pszLine), lPos));
  }
  else if (pfs->fAction == FA_FINDGLOBAL)
    lLine = lPos = 0;
  else if (pfs->fAction != FA_FINDNEXT && pfs->fAction != FA_FINDBACK)
    return MRESULT(FALSE);

  for( ;  lLine>=0 && lLine<=pBuf->lMaxLineNum;  (pfs->fAction==FA_FINDBACK) ? lLine-- : lLine++)
  {
    PSZ pszLine = pBuf->aPLines[lLine]->sz;
    if ((lPos = find_in_line(pszLine, strlen(pszLine), lPos)) >= 0)
    {
      pfs->tprPos.Set(lLine,lPos);
      pfs->lLenFound = nF_PatternLen;
      if (pfs->fGotoFound) // Select the text found
      {
        if (pfs->fShowDownScreen && lFirstScrLine)
        {
          WinInvalidateRect(hEd,0,0);
          lFirstScrLine = 0;
        }
        TPR tprAnc;
        tprAnc.Set(lLine,lPos+nF_PatternLen);
        if (ulBlockMode==BLOCK_LINES) RxExecute("EdMode('SELECTION','STREAM','NOTIFY');"); /*++++ reenterable ? */
        WinSendMsg(hEd,TM_SETSEL,MPARAM(&pfs->tprPos),MPARAM(&tprAnc));
      }
      return MRESULT(TRUE);
    }
    lPos = (pfs->fAction==FA_FINDBACK) ? 0x1fffffff : 0;
  }
  return MRESULT(FALSE);
}

//====== End of TM_FIND area.


MRESULT TED::TMExport(MPARAM m1, MPARAM m2)
{
  TPR tpr1(0,0);
  TPR tpr2(pBuf->lMaxLineNum,0x7fffffff);
  PSZ pszExp = AllocBlock(FALSE, BLOCK_STREAM, &tpr1, &tpr2);
  return (MRESULT) pszExp;
}

MRESULT TED::TMGetCurWord(MPARAM m1, MPARAM m2)
{
  LONG lLen;
  PSZ  pszLine = pBuf->aPLines[tprCursor.lLine]->sz;
  PSZ  pszW    = GetWord(pszLine, tprCursor.lPos, &lLen, FALSE);
  if (pszW && lLen)
  {
    PSZ pszRet;
    if (pszRet=(PSZ)malloc(lLen+1))
    {
      strncpy(pszRet,pszW,lLen);
      pszRet[lLen] = 0;
      if (m1) *PLONG(m1) = tprCursor.lLine;
      if (m2) *PLONG(m2) = pszW - pszLine;
      return MRESULT(pszRet);
    }
  }
  return MRESULT(NULL);
}

MRESULT TED::TMGetCurWord1(MPARAM m1, MPARAM m2)
{
  LONG lLen;
  LONG lPos    = tprCursor.lPos;
  PSZ  pszLine = pBuf->aPLines[tprCursor.lLine]->sz;
  PSZ  pszW    = GetWord(pszLine, lPos, &lLen, FALSE);
  if ((!pszW || !lLen) && lPos && m1)
  {
    lPos--;
    pszW = GetWord(pszLine, lPos, &lLen, FALSE);
  }
  if (pszW && lLen)
  {
    PSZ pszRet;
    if (pszRet=(PSZ)malloc(lLen+1))
    {
      strncpy(pszRet,pszW,lLen);
      pszRet[lLen] = 0;
      return MRESULT(pszRet);
    }
  }
  return MRESULT(NULL);
}

MRESULT TED::TMGetSelText(MPARAM m1, MPARAM m2)
{
  PSZ psz = AllocBlock(FALSE);
  if (!psz) return MRESULT(0);
  ULONG cb  = strlen(psz);
  if (cb && m1 && m2)
  {
    cb = min(cb,ULONG(m2)-1);
    strncpy(PSZ(m1),psz,cb);
    PSZ(m1)[cb] = '\0';
  }
  free(psz);
  return MRESULT(cb);
}


MRESULT TED::TMGoBracket(MPARAM m1, MPARAM m2)
{
  const  int  BRACKETS         = 8;
  static char szBr[BRACKETS+1] = "([{<>}])";
  LONG        lLine            = tprCursor.lLine;
  LONG        lPos             = tprCursor.lPos;
  PSZ         pszLin           = pBuf->aPLines[lLine]->sz;
  char        chBr, chPair, ch;
  BOOL        fBack;
  LONG        lNestCount = 0;
  if (strlen(pszLin)<=lPos)
    return 0;                                    // <-- OOPS
  {
    chBr       = pszLin[tprCursor.lPos];
    ULONG idx  = strchr(szBr,chBr)-szBr;
    if (idx>=BRACKETS) return 0;                 // <-- OOPS
    chPair     = szBr[BRACKETS-idx-1];
    fBack      = (idx >= BRACKETS/2);
  }
  if(fBack)
    while(1)
    {
      while(lPos>=0)
      {
        if ((ch = pszLin[lPos])==chBr)          lNestCount++;
        else if (ch == chPair && !--lNestCount) goto found;
        lPos--;
      }
      if (!(lLine--)) return 0;                  // <-- OOPS
      pszLin = pBuf->aPLines[lLine]->sz;
      lPos   = strlen(pszLin)-1;
    }
  else
    while(1)
    {
      while(ch=pszLin[lPos])
      {
        if      (ch==chBr)                      lNestCount++;
        else if (ch == chPair && !--lNestCount) goto found;
        lPos++;
      }
      if (++lLine > pBuf->lMaxLineNum) return 0; // <-- OOPS
      pszLin = pBuf->aPLines[lLine]->sz;
      lPos   = 0;
    }
found:
  HideSel();
  GotoCoord(lLine, lPos, TRUE);
  tprAnchor = tprCursor;
  return MRESULT(TRUE);
}

MRESULT TED::TMLockUpdate(MPARAM m1, MPARAM m2)
{
  if (!pBuf->fLockWinUpdate != !m1)
  {
    pBuf->fLockWinUpdate = !!m1;
    if (!m1)
    {
      if (WinQueryFocus(HWND_DESKTOP)==hEd)
      {
        ExportChState(TRUE);
        WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_POSCHANGED),MPARAM(&tprCursor));
        WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(tprAnchor!=tprCursor));
      }
      InvalidateLines(0,pBuf->lMaxLineNum+1000);
    }
  }
  return 0;
}

MRESULT TED::TMQueryChanged(MPARAM m1, MPARAM m2)
{
  return (MRESULT)!!(m1 ? pBuf->fBufferWasChanged : pBuf->fBufferChanged);
}

MRESULT TED::TMQueryColors(MPARAM m1, MPARAM m2)
{
  if (m1)
    memcpy((CLP_PALETTETYPE*)m1,aaRGB,sizeof(aaRGB));
  return (MRESULT)0;
}

MRESULT TED::TMQueryEdConf(MPARAM m1, MPARAM m2)
{
  PEDCONF p  = (PEDCONF)m1;
  ULONG   ws = WinQueryWindowULong(hEdFrame,QWL_STYLE);
  memset(p,0,sizeof(*p));
  p->fMaximized      = !!(ws&WS_MAXIMIZED);
  p->fMinimized      = !!(ws&WS_MINIMIZED);
  if (ws & (WS_MAXIMIZED|WS_MINIMIZED))
  {
    p->x0            = WinQueryWindowUShort(hEdFrame,QWS_XRESTORE);
    p->y0            = WinQueryWindowUShort(hEdFrame,QWS_YRESTORE);
    p->cx            = WinQueryWindowUShort(hEdFrame,QWS_CXRESTORE);
    p->cy            = WinQueryWindowUShort(hEdFrame,QWS_CYRESTORE);
  }
  else
  {
    SWP swp;
    WinQueryWindowPos(hEdFrame,&swp);
    p->x0            = swp.x;
    p->y0            = swp.y;
    p->cx            = swp.cx;
    p->cy            = swp.cy;

  }
  p->xLeftScrShift   = xLeftScrShift;
  p->lFirstScrLine   = lFirstScrLine;
  p->lLine           = tprCursor.lLine;
  p->lPos            = tprCursor.lPos;
  p->fHilite         = fHilite;
  p->fHiliteCurLine  = fHiliteCurLine;
  p->lTabSize        = lTabSize;
  p->fSmartTab       = fSmartTab;
  p->fTabCharsMode   = fTabCharsMode;
  p->fAutoidentMode  = fAutoidentMode;
  p->ulBlockMode     = ulBlockMode;
  p->fInsertMode     = fInsertMode;
  memcpy(&p->fAttrs,    &fAttrs,     sizeof(FATTRS));
  memcpy(&p->edPalette,  aaRGB,      sizeof(p->edPalette));
  memcpy(&p->szPainterId,szPainterId,sizeof(szPainterId));
  return 0;
}

MRESULT TED::TMQueryPainter(MPARAM m1, MPARAM m2)
{
  return (MRESULT)pPainter;
}

MRESULT TED::TMQuerySel(MPARAM m1, MPARAM m2)
{
  if (m1) *PTPR(m1) = tprCursor;
  if (m2) *PTPR(m2) = tprAnchor;
  return (MRESULT)0;
}


MRESULT TED::TMQueryUndo(MPARAM m1, MPARAM m2)
{
  return MRESULT(pBuf->nUnTop != pBuf->nUnBottom);
}

MRESULT TED::TMSelCurWord(MPARAM m1, MPARAM m2)
{
  LONG lLen;
  LONG lLine = tprCursor.lLine;
  if (lLine <= pBuf->lMaxLineNum)
  {
    PSZ  pszLine = pBuf->aPLines[lLine]->sz;
    PSZ  pszW    = GetWord(pszLine, tprCursor.lPos, &lLen, FALSE);
    if (pszW && lLen)
    {
      if(ulBlockMode==BLOCK_LINES && !RxExecute("EdMode('SELECTION','STREAM','NOTIFY');"))
        return MRESULT(FALSE);
      InvSel();
      tprAnchor.Set(lLine,pszW-pszLine+lLen);
      GotoCoord(lLine, pszW-pszLine, TRUE);
      InvalidateLines(lLine,lLine,TRUE);
      return MRESULT(TRUE);
    }
  }
  return MRESULT(FALSE);
}


MRESULT TED::TMSetCase(MPARAM m1, MPARAM m2)
{
  LONG  lLen;
  TPR   tprW;
  PSZ   pszLine = pBuf->aPLines[tprCursor.lLine]->sz;
  PSZ   pszW    = GetWord(pszLine, tprCursor.lPos, &lLen, FALSE);
  if (!pszW && tprCursor.lPos)
    pszW = GetWord(pszLine, tprCursor.lPos-1, &lLen, FALSE);
  if  (!pszW || lLen>=FINDLINELEN) return 0;
  char szBuf   [FINDLINELEN+1];
  char szBufWas[FINDLINELEN+1];
  memcpy(szBuf,pszW,lLen);
  szBuf[lLen] = '\0';
  strcpy(szBufWas,szBuf);
  tprW.Set(tprCursor.lLine,pszW-pszLine);
  if (m1) // Upper case
    for (int i=0; szBuf[i]; i++) szBuf[i] = sf_toupper(szBuf[i]);
  else    // Same case
  {
    FINDSTRUCT fs;
    strcpy(fs.szFind,szBuf);
    fs.fAction         = FA_FINDBACK;
    fs.fWholeWord      = TRUE;
    fs.fCaseSensitive  = FALSE;
    fs.fGotoFound      = FALSE;
    fs.fShowDownScreen = FALSE;
    fs.tprPos          = tprW;
    if      (fs.tprPos.lPos)  fs.tprPos.lPos--;
    else if (fs.tprPos.lLine) fs.tprPos.lLine--, fs.tprPos.lPos=0;
    else return 0;
    if (!TMFind(MPARAM(&fs),0)) return 0;
    PSZ pszFound = pBuf->aPLines[fs.tprPos.lLine]->sz + fs.tprPos.lPos;
    strncpy(szBuf,pszFound,lLen);
  }
  // Теперь надо заменить слово на szBuf
  {// Undo:
    PSZ psz = (PSZ)malloc(strlen(szBufWas)+1);
    if (!psz) return 0;
    strcpy(psz,szBufWas);
    UnInsOver(&tprCursor, &tprAnchor, tprW.lLine, tprW.lPos, tprW.lLine,
            tprW.lPos+lLen, psz, pBuf->lMaxLineNum);
  }
  memcpy(pszW,szBuf,lLen);
  pBuf->fBufferChanged = TRUE;
  ExportChState();
  InvalidateLines(tprW.lLine,tprW.lLine);
  return MRESULT(TRUE);
}

MRESULT TED::TMSetChanged(MPARAM m1, MPARAM m2)
{
  pBuf->fBufferWasChanged   |= pBuf->fBufferChanged;
  BOOL fCh = (!pBuf->fBufferChanged != !m1);
  pBuf->fBufferChanged = !!m1;
  if (fCh) ExportChState();
  return 0;
}

MRESULT TED::TMSetColors(MPARAM m1, MPARAM m2)
{
  if (m1) memcpy   (aaRGB, PVOID(m1), sizeof(aaRGB));
  WinInvalidateRect(hEd,0,0);
  WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_PALETTECHANGED),MPARAM(hEd));
  return 0;
}

MRESULT TED::TMSetEdConf(MPARAM m1, MPARAM m2)
{
  PEDCONF p  = (PEDCONF)m1;
  fHilite         = p->fHilite;
  fHiliteCurLine  = p->fHiliteCurLine;
  lTabSize        = p->lTabSize;
  fSmartTab       = p->fSmartTab;
  fTabCharsMode   = p->fTabCharsMode;
  fAutoidentMode  = p->fAutoidentMode;
  ulBlockMode     = p->ulBlockMode;
  fInsertMode     = p->fInsertMode;
  memcpy      (aaRGB,   &p->edPalette, sizeof(aaRGB));
  TMSetFont   (MPARAM(&p->fAttrs),MPARAM(TRUE));
  TMSetPainter(MPARAM(p->szPainterId[0] ? p->szPainterId : 0), MPARAM(FALSE));
  WinSetWindowPos(hEdFrame,0, p->x0,p->y0,p->cx,p->cy, SWP_SIZE|SWP_RESTORE);
  WinSetWindowPos(hEdFrame,0, p->x0,p->y0,p->cx,p->cy, SWP_MOVE);
  if (p->fMaximized || p->fMinimized)
    WinSetWindowPos(hEdFrame,0, 0,0,0,0, (p->fMaximized ? SWP_MAXIMIZE : 0)
                    | (p->fMinimized ? SWP_MINIMIZE : 0));
  xLeftScrShift   = p->xLeftScrShift;
  lFirstScrLine   = min(p->lFirstScrLine,pBuf->lMaxLineNum);
  GotoCoord(p->lLine,p->lPos);
  tprAnchor=tprCursor;
  WinInvalidateRect(hEd,0,0);
  if (WinQueryFocus(HWND_DESKTOP)==hEd)
    WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(tprAnchor!=tprCursor));
  WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_PALETTECHANGED),MPARAM(hEd));
  return 0;
}

MRESULT TED::TMSetEdFrame(MPARAM m1, MPARAM m2)
{
  hEdFrame = (HWND)m1;
  hVScroll = WinWindowFromID(hEdFrame,FID_VERTSCROLL);
  SetVScroll();
  return 0;
}


MRESULT TED::TMSetFont(MPARAM m1, MPARAM m2)
{
  FONTMETRICS fm;
  int         i;

  {
    ULONG cpage,cpsize;
    DosGetCp(sizeof(cpage), &cpage, &cpsize);
    PFATTRS(ULONG(m1))->usCodePage = USHORT(cpage);
  }

  memcpy(&fAttrs,PFATTRS(ULONG(m1)),sizeof(FATTRS));
  GpiCreateLogFont(hpsEd,NULL,001L,PFATTRS(ULONG(m1)));
  GpiSetCharSet   (hpsEd,001L);
  GpiQueryFontMetrics   (hpsEd,sizeof(FONTMETRICS),&fm);
  GpiQueryWidthTable    (hpsEd,0L,256,alCharWidths);
  xLineHeight    = fm.lMaxBaselineExt+fm.lExternalLeading+1;
  xMaxDescender  = fm.lMaxDescender;
  xMaxCharWidth  = 1;
  for (i=0; i<256; i++)
    if (alCharWidths[i]>xMaxCharWidth)
      xMaxCharWidth = alCharWidths[i];
  if (!m2)
  {
    GotoCoord(tprCursor.lLine,tprCursor.lPos,FALSE);
    WinInvalidateRect(hEd,0,0);
  }
  return 0;
}

MRESULT TED::TMSetHilite(MPARAM m1,MPARAM m2)
{
  if (!m1 != !fHilite  ||  !m2 != !fHiliteCurLine)
  {
    fHilite        = !!m1;
    fHiliteCurLine = !!m2;
    WinInvalidateRect(hEd,0,0);
  }
  return 0;
}

MRESULT TED::TMSetPainter(MPARAM m1,MPARAM m2)
{
  extern PAINTEROFFICE ptOffice; /// :-(
  int                  i;
  PPAINTER             ppt = 0;
  if (m1)
  {
    ppt = ptOffice.LoadPainter((PSZ)m1, m2 ? hEd : 0);
    if (!ppt) return MRESULT(FALSE);
  }
  for (i=0; i<pBuf->nUsers; i++)
  {
    PTED pted = pBuf->aptUsers[i];
    pted->pPainter = ppt;
    pted->aclMap   = ppt ? (*ppt->PClQueryMap)() : pted->aMyMap;
    if (ppt) strcpy(pted->szPainterId,PSZ(m1));
    else     pted->szPainterId[0] = 0;
    pted->UpdateComLevels(0,0x7fffffff);
    WinUpdateWindow(pted->hEd);
  }
  return MRESULT(TRUE);
}

MRESULT TED::TMSetReadOnly(MPARAM m1,MPARAM m2)
{
  pBuf->fReadOnly = !!m1;
  ExportChState(FALSE);
  return 0;
}

MRESULT TED::TMSetSel(MPARAM m1,MPARAM m2)
{
  HideSel(TRUE);
  GotoCoord(PTPR(m1)->lLine,PTPR(m1)->lPos,TRUE);
  tprAnchor.Set(min(PTPR(m2)->lLine,pBuf->lMaxLineNum),PTPR(m2)->lPos);
  InvSel(TRUE);
  if (WinQueryFocus(HWND_DESKTOP)==hEd && !pBuf->fLockWinUpdate)
    WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(tprAnchor!=tprCursor));
  return 0;
}

MRESULT TED::TMSetSelShowUp(MPARAM m1,MPARAM m2)
{
  lFirstScrLine = max(PTPR(m1)->lLine - 1, 0);
  WinInvalidateRect(hEd,0,0);
  return TMSetSel(m1,m2);
}

MRESULT TED::TMSetShare(MPARAM m1,MPARAM m2)
{
  PBUFSTR pBuf1 = (PBUFSTR)WinSendMsg(HWND(m1),TM_QUERYBUFSTR,0,0);
  if (!pBuf1 || !pBuf1->Connect(this)) return 0;
  TMClearAll(0,0);
  pBuf->Disconnect(this);
  pBuf = pBuf1;
  GotoCoord(0,0,FALSE);
  WinInvalidateRect(hEd,0,0);
  ExportChState(TRUE);
  return MRESULT(TRUE);
}

MRESULT TED::TMSetText (MPARAM m1,MPARAM m2)
// m1:       pointer to the text
// m2:       text length
// returns:  error state
// Notes: InsertText is too slow so we don't use it here
//
{
  char    *pchB,*pchE,*pchMax;
  char     ch;
  PLINESTR pl;
  LONG     lMax = 0;


  try
  {
    pBuf->ClearBuf();
    FreeLine(pBuf->aPLines[0]);
    pchB   = (char*)m1;
    pchMax = pchB+LONG(m2);

    for (lMax=0; TRUE; lMax++)
    {
      for(pchE=pchB; pchE<pchMax && (ch=*pchE) && ch!='\r' && ch!='\n'; pchE++);
      pl = MkLine(0,0,pchB,pchE-pchB);
      if (!fTabCharsMode && strchr(pl->sz,'\t'))
      { // TAB characters processing
        char *pchTab;
        LONG lLen = strlen(pl->sz);
        while(pchTab=strchr(pl->sz,'\t'))
        {
          LONG pos = pchTab-pl->sz;
          LONG ins = lTabSize - pos%lTabSize - 1;
          pl->sz[pos] = ' ';
          pl = ReallocLine(pl,lLen+ins);
          memmove(pl->sz+pos+ins,pl->sz+pos,lLen-pos);
          memset (pl->sz+pos,' ',ins);
          lLen+=ins;
        }
      }
      pBuf->LimitLines(lMax);
      pBuf->aPLines[lMax] = pl;
      if (pchE<pchMax &&    ch == CR) pchE++;
      if (pchE<pchMax && *pchE == LF) pchE++;
      if (pchE>=pchMax || !*pchE) break;
      pchB = pchE;
    }
    pBuf->lMaxLineNum = lMax;
  }
  catch (char *sz)
  {
    pBuf->lMaxLineNum = lMax;
    MessageBox(MB_ERROR|MB_OK|MB_MOVEABLE,"Set editor text",sz);
    pBuf->ClearBuf();
  }
  Purgen();
  UpdateComLevels(0,pBuf->lMaxLineNum);
  for (LONG l=pBuf->nUsers-1; l>=0; l--)
  {
    pBuf->aptUsers[l]->GotoCoord(0,0);
  }
  InvalidateLines(0,pBuf->lMaxLineNum+1000);
  ExportChState(TRUE);
  return MRESULT(FALSE);
}

MRESULT TED::TMQueryShared(MPARAM m1,MPARAM m2)
{
  if (ULONG(m1)>=pBuf->nUsers) return 0;
  return (MRESULT)pBuf->aptUsers[ULONG(m1)]->hEd;
}

MRESULT TED::WMButton1Down(MPARAM m1,MPARAM m2)
{
  TPR           tpr;
  fMouseMoverNow = FALSE;
  if (WinQueryFocus(HWND_DESKTOP)!=hEd)
  {
    WinSetWindowPos        (hEd,HWND_TOP,0,0,0,0,SWP_ZORDER);
    WinSetActiveWindow     (HWND_DESKTOP,hEd);
    return WinDefWindowProc(hEd,WM_BUTTON1DOWN,m1,m2);
  }
  xyMouse = (ULONG)m1; // For WMTimer only
  tpr     = Graf2Pos(ULONG(m1));
  WinSetCapture(HWND_DESKTOP,hEd);
  WinStartTimer(hAB,hEd,TID_MOUSESCROLL,60);
  if (IsSelectedPos(&tpr))
  {
    hptrOld = WinQueryPointer(HWND_DESKTOP);
    if (SHORT2FROMMP(m2)&KC_CTRL)
    {
      fMouseMoverNow = MOUSE_COPY_NOW;
      WinSetPointer(HWND_DESKTOP,hptrCopy);
    }
    else
    {
      fMouseMoverNow = MOUSE_MOVE_NOW;
      WinSetPointer(HWND_DESKTOP,hptrMove);
    }
  }

  else //--- outside selection - set position
  {
    tpr -= tprCursor;
    char sz[40];
    sprintf(sz,"EdMove(%d, %d);",tpr.lPos,tpr.lLine);
    RxExecute(sz);
  }
  return 0;
}


MRESULT TED::WMButton1Up(MPARAM m1,MPARAM m2)
{
  TPR   tpr;
  if (WinQueryCapture(HWND_DESKTOP)!=hEd) return 0;
  WinSetCapture(HWND_DESKTOP,0);
  WinStopTimer(hAB,hEd,TID_MOUSESCROLL);
  tpr = Graf2Pos(ULONG(m1));
  if (!fMouseMoverNow)
  {
    tpr -= tprCursor;
    char sz[40];
    sprintf(sz,"EdMove(%d, %d, \'SELECT\');",tpr.lPos,tpr.lLine);
    RxExecute(sz);
  }
  else
  {
    BOOL fMove     = (fMouseMoverNow==MOUSE_MOVE_NOW);
    fMouseMoverNow = 0;
    WinSetPointer(HWND_DESKTOP,hptrOld);
    if (IsSelectedPos(&tpr)    ||
        SHORT1FROMMP(m1) < 0  ||  SHORT1FROMMP(m1) >= xWindowWidth         ||
        SHORT2FROMMP(m1) < 0  ||  SHORT2FROMMP(m1) >= xWindowHeight         )
    {
      Beep();
      return 0; // Inside selection or outside window
    }

    char sz[40];
    tpr -= tprCursor;
    sprintf(sz,"EdMouseBlock(%d, %d, %s);",tpr.lPos,tpr.lLine, fMove ? "\'MOVE\'" : "\'COPY\'");
    RxExecute(sz);
  }
  return 0;
}


MRESULT TED::WMChar(MPARAM m1, MPARAM m2)
{
  {
    BOOL f      = fScrollLock;
    fScrollLock = 0x0001 & WinGetKeyState(HWND_DESKTOP,VK_SCRLLOCK);
    if (f!=fScrollLock && fScrollLock)
      GotoCoord(tprCursor.lLine,tprCursor.lPos,FALSE);
  }
  if (fMouseMoverNow && !((SHORT1FROMMP(m1)&KC_PREVDOWN) && !(SHORT1FROMMP(m1)&KC_KEYUP))){
      TerminateMouseMover();
      return 0;
  }
  return pfnCH(hEd,m1,m2);
}


MRESULT TED::WMDestroy(MPARAM m1, MPARAM m2)
{
  HWND hNotify = TED::hNotifyWindow;
  HWND hEd     = TED::hEd;
  WinSetWindowULong(hEd,QWL_TEDTHIS,ULONG(NULL));
  delete this; // Self-destruction (am I a ghost now?  8-P~~ )
  WinSendMsg(hNotify,WM_CONTROL,MPFROM2SHORT(0,TN_DESTROYED),MPARAM(hEd));
  return 0;
}


MRESULT TED::WMFocusChange(MPARAM m1, MPARAM m2)
{
  if (SHORT1FROMMP(m2))
  {
    pCmdTed     = this;
    fScrollLock = 0x0001 & WinGetKeyState(HWND_DESKTOP,VK_SCRLLOCK);
    if (tprCursor.lLine>pBuf->lMaxLineNum)
      GotoCoord(pBuf->lMaxLineNum,tprCursor.lPos,FALSE);
    if (tprAnchor.lLine > pBuf->lMaxLineNum)
    {
      InvSel();
      tprAnchor.lLine = pBuf->lMaxLineNum;
    }
    RestartCursor();
    WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SETFOCUS),MPARAM(hEd));
    ExportChState(TRUE);
    WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_POSCHANGED),MPARAM(&tprCursor));
    WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(tprAnchor!=tprCursor));
    UnNotify();
  }
  else
  {
    WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_KILLFOCUS),MPARAM(hEd));
    pCmdTed = 0;
    WinDestroyCursor(hEd);
  }
  return WinDefWindowProc(hEd,WM_FOCUSCHANGE,m1,m2);
}


MRESULT TED::WMMouseMove(MPARAM m1, MPARAM m2)
{
  TPR tpr;
  if (WinQueryCapture(HWND_DESKTOP)!=hEd)
    return WinDefWindowProc(hEd,WM_MOUSEMOVE,m1,m2);
  xyMouse = (ULONG)m1; // For WMTimer only
  if
  ( LONG(m2)!=0xffffffff                &&  // Not from timer?
    ( SHORT(xyMouse)     < 0            ||
      SHORT(xyMouse)     > xWindowWidth ||
      SHORT(xyMouse>>16) < 0            ||
      SHORT(xyMouse>>16) > xWindowHeight
    )
  ) return 0;
  tpr = Graf2Pos(ULONG(m1));
  if (!fMouseMoverNow)
  {
    if(tpr != tprCursor)
    {
      if (ulBlockMode==BLOCK_COLUMN)
        InvalidateLines(tprAnchor.lLine, tpr.lLine);
      InvalidateLines(tprCursor.lLine, tpr.lLine);
      GotoCoord(tpr.lLine,tpr.lPos,TRUE);
    }
  }
  else GotoCoord(tpr.lLine,tpr.lPos,FALSE,TRUE); // Show it only
  WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(tprAnchor!=tprCursor));
  return 0;
}


MRESULT TED::WMPaint(MPARAM m1, MPARAM m2)
{
  RECTL rcl;
  LONG  i;

  WinEndPaint(WinBeginPaint(hEd,NULLHANDLE,&rcl));
  if (pBuf->fLockWinUpdate) return 0;

  LONG  lLinBeg    = lFirstScrLine + (xWindowHeight-1-rcl.yTop)     /xLineHeight;
  LONG  lLinEnd    = lFirstScrLine + (xWindowHeight-1-rcl.yBottom)  /xLineHeight;
  LONG  lHiScrLine = lFirstScrLine + (xWindowHeight+xLineHeight-1)  /xLineHeight;

  lLinBeg = min(lHiScrLine,max(lLinBeg,lFirstScrLine));
  lLinEnd = min(lHiScrLine,max(lLinEnd,lFirstScrLine));

  if (lLinBeg<=lLinEnd)
  {
    WinShowCursor(hEd,FALSE);
    for (i=lLinBeg; i<=lLinEnd; i++)
      DrawNumLine(i);
    RestartCursor();
    WinShowCursor(hEd,TRUE);
  }
  return 0;
}


MRESULT TED::WMSize(MPARAM m1, MPARAM m2)
{
  SWP swp;
  WinQueryWindowPos(hEd,&swp);
  xWindowHeight    = swp.cy;
  xWindowWidth     = swp.cx;
  xHorScrollZone   = min(xWindowWidth/5,30);
  xHorScrollStep   = max(xHorScrollZone,1);
  lVertScrollZone  = (xWindowHeight>=xLineHeight*4)? 1:0;
  if (!(swp.fl & SWP_MINIMIZE) && swp.cx)
    GotoCoord(tprCursor.lLine, tprCursor.lPos, FALSE);
  return 0;
}

MRESULT TED::WMSysCommand(MPARAM m1, MPARAM m2)
{
  if (m1 != MPARAM(SC_CLOSE)) return WinDefWindowProc(hEd,WM_SYSCOMMAND,m1,m2);
  if (WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_LETMEDIE),MPARAM(hEd)))
    WinDestroyWindow(hEdFrame);
  return 0;
}

MRESULT TED::WMTimer(MPARAM m1, MPARAM m2)
{
  if (SHORT1FROMMP(m1) != TID_MOUSESCROLL ||
      WinQueryCapture(HWND_DESKTOP)!=hEd)
    return WinDefWindowProc(hEd,WM_TIMER,m1,m2);
  this->WMMouseMove(MPARAM(xyMouse),MPARAM(0xffffffff));
  return 0;
}

MRESULT TED::WMVScroll(MPARAM m1, MPARAM m2)
{
  LONG lMax  = pBuf->lMaxLineNum;
  LONG cft   = lMax/0x7fff + 1;
  LONG lPage = (xWindowHeight+xLineHeight-1)/xLineHeight - 2;
  lPage      = max(lPage,1);
  switch (SHORT2FROMMP(m2)){
  case SB_LINEUP:
    if (lFirstScrLine) lFirstScrLine--;
    break;
  case SB_LINEDOWN:
    if (lFirstScrLine<lMax) lFirstScrLine++;
    break;
  case SB_PAGEUP:
    lFirstScrLine = max(lFirstScrLine-lPage, 0);
    break;
  case SB_PAGEDOWN:
    lFirstScrLine = min(lFirstScrLine+lPage, lMax);
    break;
  case SB_SLIDERPOSITION:
  case SB_SLIDERTRACK:
    lFirstScrLine = min(ULONG(SHORT1FROMMP(m2))*cft, lMax);
    break;
  }
  SetVScroll();
  RestartCursor();
  WinInvalidateRect(hEd,0,0);
  return 0;
}

/*########################################################################################################*/
/*#                                                                                                      #*/
/*#  U N D O / R E D O   h a n d l i n g                                                                 #*/
/*#                                                                                                      #*/
/*########################################################################################################*/

BOOL TED::UnTypeStr (PTPR ptprCur0, PTPR ptprAnc0, LONG lTypLen, char chWas, PSZ pszWas)
//
// lTypLen      - length of the typed string
// chWas        = overwritten character (or 0) | Both = 0 =>
// pszWas       - overwritten string    (or 0) |    => it was insertion
{
  if ( ulUnMode)  return 0;
  if (!ulReMode) PurgeRedo();
  LONG len = 0;
  if (pszWas)
  {
    len = strlen(pszWas)+1;
    if (!ISUNDONABLE(len))
    {
      free(pszWas);
      Purgen();
      return TRUE;                         // Text is too large
    }
    while (len > pBuf->ulUnFreeStorage && !UnDrop());
    if (pBuf->ulUnFreeStorage < len)
      pBuf->ulUnFreeStorage = UNMAXSTORE; // Fat louse..
    pBuf->ulUnFreeStorage -= len;
  }
  UNDOSTR us;
  us.unOperation                 = UNOP_TYPESTR;
  us.un.unoptypestr.lLine        = ptprCur0->lLine;
  us.un.unoptypestr.lPos         = ptprCur0->lPos;
  us.un.unoptypestr.lTypLen      = lTypLen;
  us.un.unoptypestr.chWas        = chWas;
  us.un.unoptypestr.pszWas       = pszWas;
  us.un.unoptypestr.lWas         = len;
  UnPush(&us,ptprCur0,ptprAnc0);
  return FALSE;
}
BOOL TED::UnInsert  (PTPR ptprCur0, PTPR ptprAnc0, LONG lLine, LONG lPos, LONG lEndLine,
                     LONG lEndPos, ULONG ulBlockMode, LONG lMaxLineNum0)
{
  if ( ulUnMode) return 0;
  if (!ulReMode) PurgeRedo();
  UNDOSTR us;
  us.unOperation                = UNOP_INSERT;
  us.un.unopinsert.lLine        = lLine;
  us.un.unopinsert.lPos         = lPos;
  us.un.unopinsert.lEndLine     = lEndLine;
  us.un.unopinsert.lEndPos      = lEndPos;
  us.un.unopinsert.ulBlockMode  = ulBlockMode;
  us.un.unopinsert.lMaxLineNum0 = lMaxLineNum0;
  UnPush(&us,ptprCur0,ptprAnc0);
  return FALSE;
}
BOOL TED::UnInsOver (PTPR ptprCur0, PTPR ptprAnc0, LONG lLine, LONG lPos, LONG lEndLine,
                     LONG lEndPos, PSZ pszWas, LONG lMaxLineNum0)
{
  if ( ulUnMode) return 0;
  if (!ulReMode) PurgeRedo();
  LONG len = strlen(pszWas)+1;
  if (!ISUNDONABLE(len))
  {
    free(pszWas);
    return TRUE;                         // Text is too large
  }
  while (len > pBuf->ulUnFreeStorage && !UnDrop());
  if (pBuf->ulUnFreeStorage < len)
    pBuf->ulUnFreeStorage = UNMAXSTORE; // Fat louse..
  pBuf->ulUnFreeStorage -= len;

  UNDOSTR us;
  us.unOperation                 = UNOP_INSOVER;
  us.un.unopinsover.lLine        = lLine;
  us.un.unopinsover.lPos         = lPos;
  us.un.unopinsover.lEndLine     = lEndLine;
  us.un.unopinsover.lEndPos      = lEndPos;
  us.un.unopinsover.lMaxLineNum0 = lMaxLineNum0;
  us.un.unopinsover.pszWas       = pszWas;
  us.un.unopinsover.lWas         = len;
  UnPush(&us,ptprCur0,ptprAnc0);
  return FALSE;
}
BOOL TED::UnDelete  (PTPR ptprCur0, PTPR ptprAnc0, LONG lLine, LONG lPos,
                     LONG lLine1, LONG lPos1, ULONG ulBlockMode, PSZ pszWas)
{
  if ( ulUnMode) return 0;
  if (!ulReMode) PurgeRedo();
  LONG len = strlen(pszWas)+1;
  if (!ISUNDONABLE(len))
  {
    free(pszWas);
    return TRUE;                  // Text is too large
  }
  while (len > pBuf->ulUnFreeStorage && !UnDrop());
  if (pBuf->ulUnFreeStorage < len)
    pBuf->ulUnFreeStorage = UNMAXSTORE; // Fat louse..
  pBuf->ulUnFreeStorage -= len;

  UNDOSTR us;
  us.unOperation                = UNOP_DELETE;
  us.un.unopdelete.lLine        = lLine;
  us.un.unopdelete.lPos         = lPos;
  us.un.unopdelete.lLine1       = lLine1;
  us.un.unopdelete.lPos1        = lPos1;
  us.un.unopdelete.ulBlockMode  = ulBlockMode;
  us.un.unopdelete.pszWas       = pszWas;
  us.un.unopdelete.lWas         = len;
  UnPush(&us,ptprCur0,ptprAnc0);
  return FALSE;
}

BOOL TED::UnMovSel(PTPR ptprCur0, PTPR ptprAnc0, LONG lLineMin, LONG lLineMax, LONG lVJump, LONG lHJump,
                   LONG l1stCol,LONG lAfterCol)
{
  if ( ulUnMode) return 0;
  if (!ulReMode) PurgeRedo();
  UNDOSTR us;
  us.unOperation                = UNOP_MOVSEL;
  us.un.unopmovsel.lLineMin     = lLineMin;
  us.un.unopmovsel.lLineMax     = lLineMax;
  us.un.unopmovsel.lVJump       = lVJump;
  us.un.unopmovsel.lHJump       = lHJump;
  us.un.unopmovsel.l1stCol      = l1stCol;
  us.un.unopmovsel.lAfterCol    = lAfterCol;
  UnPush(&us,ptprCur0,ptprAnc0);
  return FALSE;
}

BOOL TED::UnDupSel  (PTPR ptprCur0, PTPR ptprAnc0, LONG lLineMin, LONG lLineMax)
{
  if (ulUnMode)  return 0;
  if (!ulReMode) PurgeRedo();
  UNDOSTR us;
  us.unOperation                = UNOP_DUPSEL;
  us.un.unopdupsel.lLineMin     = lLineMin;
  us.un.unopdupsel.lLineMax     = lLineMax;
  UnPush(&us,ptprCur0,ptprAnc0);
  return FALSE;
}

BOOL TED::UnMovement(PTPR ptprCur0, PTPR ptprAnc0)
{
  if ( ulUnMode) return 0;
  if (!ulReMode) PurgeRedo();
  if (pBuf->nUnTop!=pBuf->nUnBottom && pBuf->aUnStr[pBuf->nUnTop].unOperation == UNOP_MOVEMENT) return FALSE;
  UNDOSTR us;
  us.unOperation                  = UNOP_MOVEMENT;
  UnPush(&us,ptprCur0,ptprAnc0);
  return FALSE;
}


BOOL TED::DoUnTypeStr  (UNOPTYPESTR *pun)
{
  if (pun->lLine > pBuf->lMaxLineNum) return TRUE;
  LONG         lLine   = pun->lLine;
  PSZ          pszLin  = pBuf->aPLines[lLine]->sz;                    // It's line
  LONG         lPos    = pun->lPos;                                   // pszLin[lPos..
  LONG         lTypLen = pun->lTypLen;                                //    ..lPos+lTypLen] - typed str.
  char        *pchWas  = pun->pszWas ? pun->pszWas    :  &pun->chWas; // Text to insert
  LONG         lWas    = pun->pszWas ? strlen(pchWas) : !!pun->chWas; // Insertion length
  LONG         lLin    = strlen(pszLin);
  LONG         lTail   = (lLin>lPos+lTypLen) ? lLin-lPos-lTypLen : 0;
  PLINESTR     pl      = AllocLine(lPos + lWas + lTail);

  {//--- Store Redo info
    REDOSTR  re;
    LONG     lTyp = max(0,    lLin-lPos);
             lTyp = min(lTyp, lTypLen  );
    re.reOperation                    = REOP_TYPESTR;
    re.un.reoptypestr.fIns            = !lWas;
    if (! (re.un.reoptypestr.pszTxt   = (PSZ)malloc(lTypLen+1)))
      throw "Out of memory error (Location: DoUnTypeStr:1)";
    memset(re.un.reoptypestr.pszTxt,  ' ',lTypLen);
    memcpy(re.un.reoptypestr.pszTxt,  pszLin+lPos, lTyp);
    re.un.reoptypestr.pszTxt[lTypLen] = '\0';
    re.un.reoptypestr.lLine = lLine;
    re.un.reoptypestr.lPos  = lPos;
    RePush(&re,&tprCursor,&tprAnchor);
  }

  pl->cl = pBuf->aPLines[lLine]->cl;
  memcpy(pl->sz,pszLin,lPos);
  memcpy(pl->sz+lPos,pchWas,lWas);
  memcpy(pl->sz+lPos+lWas,pszLin+lPos+lTypLen,lTail);
  pl->sz[lPos+lWas+lTail] = '\0';
  FreeLine(pBuf->aPLines[lLine]);
  pBuf->aPLines[lLine] = pl;
  cut_eolspc(lLine);
  //--- Account canges
  if (!lWas) // It was un-insertion
  {
    tcdStruct.DelStream(lLine, lLine, lPos+lTypLen, lPos, pBuf->lMaxLineNum);
    TextChanged();
  }
  LONG lInvMax = UpdateComLevels(lLine,lLine) ? pBuf->lMaxLineNum : lLine;
  InvalidateLines(lLine,lInvMax);
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Text type undone"));
  return FALSE;
}
BOOL TED::DoReTypeStr(REOPTYPESTR *pre)
{
  HideSel();
  GotoCoord(pre->lLine,pre->lPos,FALSE);
  if (TypeString(pre->pszTxt,pre->fIns)) return TRUE;
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Text type re-done"));
  return FALSE;
}



BOOL TED::DoUnInsert   (UNOPINSERT *pun)
{
  ULONG     ulBlockMode = pun->ulBlockMode;
  TPR       tprBeg (pun->lLine,    pun->lPos);
  TPR       tprEnd (pun->lEndLine, pun->lEndPos);
  if (ulBlockMode==BLOCK_STREAM)
  {
    LONG lLen = strlen(pBuf->aPLines[tprEnd.lLine]->sz);
    tprEnd.lPos = min(tprEnd.lPos,lLen);
  }
  else if (ulBlockMode==BLOCK_LINES)
  {
    tprEnd.lLine--;
    tprEnd.lPos++;
  }
  {//--- Store Redo info
    REDOSTR  re;
    PSZ      psz = 0;
    if (tprBeg<tprEnd && !(psz=AllocBlock(FALSE,ulBlockMode,&tprBeg,&tprEnd))) return TRUE;
    re.reOperation               = REOP_INSERT;
    re.un.reopinsert.lLine       = tprBeg.lLine;
    re.un.reopinsert.lPos        = tprBeg.lPos;
    re.un.reopinsert.pszTxt      = psz;
    re.un.reopinsert.ulBlockMode = ulBlockMode;
    RePush(&re,&tprCursor,&tprAnchor);
  }
  if (tprBeg<tprEnd && DeleteBlock(tprBeg, tprEnd, ulBlockMode, FALSE)) return TRUE;

  if (pun->lMaxLineNum0 < pBuf->lMaxLineNum)
  {
    LONG lCut;
    for (lCut = pun->lMaxLineNum0+1; lCut<=pBuf->lMaxLineNum; lCut++)
      if (pBuf->aPLines[lCut]->sz[0]) return TRUE; // It must be free lines only
    for (lCut = pun->lMaxLineNum0+1; lCut<=pBuf->lMaxLineNum; lCut++)
      FreeLine(pBuf->aPLines[lCut]);
    pBuf->lMaxLineNum = pun->lMaxLineNum0;
    //--- Account canges
    tcdStruct.MaxLin(pBuf->lMaxLineNum);
    TextChanged();
  }
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Insertion undone"));
  return FALSE;
}
BOOL TED::DoReInsert(REOPINSERT *pre)
{
  HideSel();
  GotoCoord(pre->lLine, pre->lPos, !pre->pszTxt);
  if (pre->pszTxt && InsertText(pre->pszTxt, strlen(pre->pszTxt), TRUE, pre->ulBlockMode))
    return TRUE;
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Insertion re-done"));
  return FALSE;
}



BOOL TED::DoUnInsOver   (UNOPINSOVER *pun)
{
  {//--- Store Redo info
    REDOSTR  re;
    TPR      tprBeg (pun->lLine,   pun->lPos);
    TPR      tprEnd (pun->lEndLine,pun->lEndPos);
    PSZ      psz    = AllocBlock(FALSE,BLOCK_COLUMN,&tprBeg,&tprEnd);
    if (!psz) return TRUE;
    re.reOperation           = REOP_INSOVER;
    re.un.reopinsover.pszTxt = psz;
    re.un.reopinsover.lLine  = tprBeg.lLine;
    re.un.reopinsover.lPos   = tprBeg.lPos;
    RePush(&re, &tprCursor, &tprAnchor);
  }
  HideSel();
  GotoCoord(pun->lLine, pun->lPos, FALSE);
  if (InsertText (pun->pszWas, pun->lWas-1, FALSE, BLOCK_COLUMNOVER))
    return TRUE;
  if (pun->lMaxLineNum0 < pBuf->lMaxLineNum)
  {
    while(pBuf->lMaxLineNum > pun->lMaxLineNum0)
    {
      FreeLine(pBuf->aPLines[pBuf->lMaxLineNum]);
      pBuf->lMaxLineNum--;
    }
    InvalidateLines(pBuf->lMaxLineNum,pBuf->lMaxLineNum+1000);
    //--- Account canges
    tcdStruct.MaxLin(pBuf->lMaxLineNum);
    TextChanged();
  }
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Insertion undone"));
  return FALSE;
}
BOOL TED::DoReInsOver(REOPINSOVER *pre)
{
  HideSel();
  GotoCoord(pre->lLine, pre->lPos, FALSE);
  if (InsertText (pre->pszTxt, strlen(pre->pszTxt), TRUE, BLOCK_COLUMNOVER)) return TRUE;
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Insertion undone"));
  return FALSE;
}


BOOL TED::DoUnDelete   (UNOPDELETE *pun)
{
  {//--- Store Redo info
    REDOSTR  re;
    re.reOperation               = REOP_DELETE;
    re.un.reopdelete.lLineOne    = pun->lLine;
    re.un.reopdelete.lPosOne     = pun->lPos;
    re.un.reopdelete.lLineTwo    = pun->lLine1;
    re.un.reopdelete.lPosTwo     = pun->lPos1;
    re.un.reopdelete.ulBlockMode = pun->ulBlockMode;
    RePush(&re, &tprCursor, &tprAnchor);
  }
  InvSel();
  GotoCoord(pun->lLine, pun->lPos, FALSE);
  tprAnchor = tprCursor;
  if (InsertText (pun->pszWas, pun->lWas-1, FALSE, pun->ulBlockMode)) return TRUE;
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Deletion undone"));
  return FALSE;
}
BOOL TED::DoReDelete(REOPDELETE *pre)
{
  HideSel();
  TPR  tprOne (pre->lLineOne,pre->lPosOne);
  TPR  tprTwo (pre->lLineTwo,pre->lPosTwo);
  if (DeleteBlock (tprOne, tprTwo, pre->ulBlockMode, TRUE)) return TRUE;
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Deletion re-done"));
  return FALSE;
}


BOOL TED::DoUnMovSel   (UNOPMOVSEL *pun)
{
  LONG l1stCol   = pun->l1stCol;
  LONG lAfterCol = pun->lAfterCol;
  {//--- Store Redo info
    REDOSTR  re;
    re.reOperation             = REOP_MOVSEL;
    re.un.reopmovsel.lMin      = pun->lLineMin;
    re.un.reopmovsel.lMax      = pun->lLineMax;
    re.un.reopmovsel.lVJump    = pun->lVJump;
    re.un.reopmovsel.lHJump    = pun->lHJump;
    re.un.reopmovsel.l1stCol   = l1stCol;
    re.un.reopmovsel.lAfterCol = lAfterCol;
    RePush(&re, &tprCursor, &tprAnchor);
  }
  HideSel();
  if      (lAfterCol>=0 && pun->lHJump<0) l1stCol--,lAfterCol--;
  else if (lAfterCol>=0 && pun->lHJump>0) l1stCol++,lAfterCol++;
  if (move_sel(pun->lLineMin+pun->lVJump,
               pun->lLineMax+pun->lVJump,
               -pun->lVJump,
               -pun->lHJump,
               0,
               l1stCol,
               lAfterCol))
    return TRUE;
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Selection movement undone"));
  return FALSE;
}
BOOL TED::DoReMovSel(REOPMOVSEL *pre)
{
  HideSel();
  if (move_sel(pre->lMin, pre->lMax, pre->lVJump, pre->lHJump, 0, pre->l1stCol, pre->lAfterCol)) return TRUE;
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Selection movement re-done"));
  return FALSE;
}


BOOL TED::DoUnDupSel   (UNOPDUPSEL *pun)
{
  {//--- Store Redo info
    REDOSTR  re;
    re.reOperation            = REOP_DUPSEL;
    re.un.reopdupsel.lLineMin = pun->lLineMin;
    re.un.reopdupsel.lLineMax = pun->lLineMax;
    RePush(&re, &tprCursor, &tprAnchor);
  }
  TPR       tpr1 (pun->lLineMin, 0);
  TPR       tpr2 (pun->lLineMax, 1);
  HideSel();
  DeleteBlock(tpr1,tpr2,BLOCK_LINES,FALSE);
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Selection duplication undone"));
  return FALSE;
}
BOOL TED::DoReDupSel(REOPDUPSEL *pre)
{
  HideSel();
  GotoCoord    (pre->lLineMin, 0, FALSE);
  tprAnchor.Set(pre->lLineMax,1);
  if (DupSel(FALSE)) return TRUE;
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Selection duplication re-done"));
  return FALSE;
}



BOOL TED::DoUnMovement (UNOPMOVEMENT *pun)
{
  {//--- Store Redo info
    REDOSTR  re;
    re.reOperation = REOP_MOVEMENT;
    RePush(&re, &tprCursor, &tprAnchor);
  }
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Cursor movement(s) undone"));
  return FALSE;
}
BOOL TED::DoReMovement(REOPMOVEMENT *pre)
{
  InvSel();
  WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("Cursor movement(s) re-done"));
  return FALSE;
}


BOOL TED::DoUndo()
// returns err
//
{
  if (pBuf->fReadOnly) {Beep(); return 0;}
  BOOL fRC = TRUE;
  if (pBuf->nUnTop==pBuf->nUnBottom)
  {
    WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("UNDO buffer is empty"));
    return fRC;
  }
  ulUnMode = 1;
  try
  {
    switch (pBuf->aUnStr[pBuf->nUnTop].unOperation)
    {
      case UNOP_TYPESTR:  fRC = DoUnTypeStr  (&pBuf->aUnStr[pBuf->nUnTop].un.unoptypestr);   break;
      case UNOP_INSERT:   fRC = DoUnInsert   (&pBuf->aUnStr[pBuf->nUnTop].un.unopinsert);    break;
      case UNOP_INSOVER:  fRC = DoUnInsOver  (&pBuf->aUnStr[pBuf->nUnTop].un.unopinsover);   break;
      case UNOP_DELETE:   fRC = DoUnDelete   (&pBuf->aUnStr[pBuf->nUnTop].un.unopdelete);    break;
      case UNOP_MOVSEL:   fRC = DoUnMovSel   (&pBuf->aUnStr[pBuf->nUnTop].un.unopmovsel);    break;
      case UNOP_DUPSEL:   fRC = DoUnDupSel   (&pBuf->aUnStr[pBuf->nUnTop].un.unopdupsel);    break;
      case UNOP_MOVEMENT: fRC = DoUnMovement (&pBuf->aUnStr[pBuf->nUnTop].un.unopmovement);  break;
    }
  }
  catch (char *sz)
  {
    ulUnMode = 0;
    MessageBox(MB_ERROR|MB_OK|MB_MOVEABLE,"Undo",sz);
    Purgen();
    return TRUE;
  }
  if (pBuf->aUnStr[pBuf->nUnTop].unOperation != UNOP_MOVEMENT)
  {
    pBuf->fBufferChanged = TRUE;
    ExportChState();
  }
  HideSel(TRUE);
  GotoCoord    (pBuf->aUnStr[pBuf->nUnTop].lCurLine,pBuf->aUnStr[pBuf->nUnTop].lCurPos,FALSE);
  tprAnchor.Set(pBuf->aUnStr[pBuf->nUnTop].lAncLine,pBuf->aUnStr[pBuf->nUnTop].lAncPos);
  if (tprAnchor.lLine > pBuf->lMaxLineNum) tprAnchor = tprCursor;
  InvSel();
  ulUnMode = 0;
  WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(tprAnchor!=tprCursor));
  switch (pBuf->aUnStr[pBuf->nUnTop].unOperation)
  {
  case UNOP_TYPESTR:
  {
    LONG len = pBuf->aUnStr[pBuf->nUnTop].un.unoptypestr.lWas;
    if (len && pBuf->aUnStr[pBuf->nUnTop].un.unoptypestr.pszWas)
    {
      pBuf->ulUnFreeStorage += len;
      free(pBuf->aUnStr[pBuf->nUnTop].un.unoptypestr.pszWas);
    }
    break;
  }
  case  UNOP_DELETE:
    pBuf->ulUnFreeStorage += pBuf->aUnStr[pBuf->nUnTop].un.unopdelete.lWas;
    free(pBuf->aUnStr[pBuf->nUnTop].un.unopdelete.pszWas);
    break;
  case UNOP_INSOVER:
    pBuf->ulUnFreeStorage += pBuf->aUnStr[pBuf->nUnTop].un.unopinsover.lWas;
    free(pBuf->aUnStr[pBuf->nUnTop].un.unopinsover.pszWas);
    break;
  }
  if (fRC)
  {
    MessageBox(MB_ERROR|MB_OK|MB_MOVEABLE,"Undo","Unrecognized error occured while UNDO operation. UNDO information was purged.");
    Purgen();
  }
  else
  {
    pBuf->nUnTop = (pBuf->nUnTop+UNMAXDEEP-1)%UNMAXDEEP;
    if (pBuf->nUnTop == pBuf->nUnBottom)
      UnNotify(FALSE);
  }
  return fRC;
}

BOOL TED::DoRedo()
// returns err
//
{
  if (pBuf->fReadOnly) {Beep(); return 0;}
  BOOL fRC = TRUE;
  if (!pBuf->pRedoTop)
  {
    WinSendMsg(hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM("REDO buffer is empty"));
    return TRUE;
  }
  ulReMode = 1;
  try
  {
    switch (pBuf->pRedoTop->reOperation)
    { // 'fBufferChanged' actuality is provided by the functions used, we don't set it here
      case REOP_TYPESTR:  fRC = DoReTypeStr  (&pBuf->pRedoTop->un.reoptypestr);  break;
      case REOP_INSERT:   fRC = DoReInsert   (&pBuf->pRedoTop->un.reopinsert);   break;
      case REOP_INSOVER:  fRC = DoReInsOver  (&pBuf->pRedoTop->un.reopinsover);  break;
      case REOP_DELETE:   fRC = DoReDelete   (&pBuf->pRedoTop->un.reopdelete);   break;
      case REOP_MOVSEL:   fRC = DoReMovSel   (&pBuf->pRedoTop->un.reopmovsel);   break;
      case REOP_DUPSEL:   fRC = DoReDupSel   (&pBuf->pRedoTop->un.reopdupsel);   break;
      case REOP_MOVEMENT: fRC = DoReMovement (&pBuf->pRedoTop->un.reopmovement); break;
    }
  }
  catch (char *sz)
  {
    MessageBox(MB_ERROR|MB_OK|MB_MOVEABLE,"Redo",sz);
    return fRC = TRUE;
  }
  GotoCoord    (pBuf->pRedoTop->lCurLine, pBuf->pRedoTop->lCurPos, FALSE);
  tprAnchor.Set(pBuf->pRedoTop->lAncLine, pBuf->pRedoTop->lAncPos);
  InvSel();
  ulReMode = 0;
  ReDrop();
  WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(tprAnchor!=tprCursor));
  if (fRC)
  {
    MessageBox(MB_ERROR|MB_OK|MB_MOVEABLE,"Redo","Unrecognized error occured while REDO operation. REDO information was purged.");
    PurgeRedo();
    return TRUE;
  }
  return FALSE;
}

void TED::UnNotify(int nVal)
{
  if (nVal==-1) nVal = (pBuf->nUnTop != pBuf->nUnBottom);
  WinSendMsg(hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_UNAVAILABLE),MPARAM(nVal));
}


/*########################################################################################################*/
/*#                                                                                                      #*/
/*#  C o m m a n d   h a n d l i n g  (functions called via Rc_RunCmd)                                   #*/
/*#                                                                                                      #*/
/*########################################################################################################*/
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


/*>>> EdCopy([UNSELECT]): "ok" | ""
*/
LONG   EXPENTRY EdCopy(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc>1) return RxERROR;
  BOOL fUnSel  = (argc==1 && !strcmp(argv[0].strptr,"UNSELECT"));
  char *pchMem = pCmdTed->AllocBlock(TRUE); // S h a r e d  memory!
  if (pchMem)
  {
    if (WinOpenClipbrd(pCmdTed->hAB))
    {
      WinSetClipbrdData(pCmdTed->hAB,(ULONG)pchMem,CF_TEXT,CFI_POINTER);
      WinCloseClipbrd(pCmdTed->hAB);
    }
    else
    {
      DosFreeMem((PVOID)pchMem);
      pchMem = 0;
    }
  }
  if (fUnSel) pCmdTed->HideSel();
  if (pchMem) rx_set_retstr(retstr,"ok");
  return 0;
}

/*>>> EdDelete( CHAR | CHARLEFT | LINE | LINELEFT |LINERIGHT | SELECTION | WORD | WORDLEFT |WORDRIGHT ): "ok" | ""
*/
LONG   EXPENTRY EdDelete(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  static BOOL     fFirstCall = TRUE;
  static SymTable stab;

  if (fFirstCall)
  {
    stab.AddName("CHAR",      DEL_CHAR);
    stab.AddName("CHARLEFT",  DEL_CHARLEFT);
    stab.AddName("WORD",      DEL_WORD);
    stab.AddName("WORDRIGHT", DEL_WORDRIGHT);
    stab.AddName("WORDLEFT",  DEL_WORDLEFT);
    stab.AddName("LINE",      DEL_LINE);
    stab.AddName("LINERIGHT", DEL_LINERIGHT);
    stab.AddName("SELECTION", DEL_SELECTION);
    stab.AddName("LINELEFT",  DEL_LINELEFT);
    fFirstCall = FALSE;
  }

  if (!pCmdTed || argc!=1) return RxERROR;

  ULONG ulDelMode = stab.SearchName(argv[0].strptr);
  if (ulDelMode == -1)                  return RxERROR;
  if (pCmdTed->DeleteAction(ulDelMode)) rx_set_retstr(retstr,"");
  else                                  rx_set_retstr(retstr,"ok");
  return 0;
}

/*>>> EdDupSel( UP | DOWN ): "ok" | ""
*/
LONG   EXPENTRY EdDupSel(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  if (!pCmdTed || argc!=1) return RxERROR;
  BOOL fUp = (!strcmp(argv[0].strptr,"UP"));
  if (pCmdTed->DupSel(fUp)) rx_set_retstr(retstr,"");
  else                      rx_set_retstr(retstr,"ok");
  return 0;
}

/*>>> EdEnd([SELECT]): "dx" | ""
*/
LONG   EXPENTRY EdEnd(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  if (!pCmdTed || argc>1) return RxERROR;
  BOOL fSel  = (argc==1 && !strcmp(argv[0].strptr,"SELECT"));
  LONG lLine = pCmdTed->tprCursor.lLine;
  int  x0    = pCmdTed->tprCursor.lPos;
  if (!fSel) pCmdTed->HideSel();
  pCmdTed->GotoCoord(lLine, strlen(pCmdTed->pBuf->aPLines[pCmdTed->tprCursor.lLine]->sz), TRUE);
  if (fSel) pCmdTed->InvalidateLines(lLine,lLine);
  else      pCmdTed->tprAnchor = pCmdTed->tprCursor;
  if (fSel && pCmdTed->ulBlockMode==BLOCK_COLUMN)
    pCmdTed->InvalidateLines(pCmdTed->tprAnchor.lLine,lLine);
  WinSendMsg(pCmdTed->hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),
             MPARAM(pCmdTed->tprAnchor != pCmdTed->tprCursor));
  rx_set_retval(retstr,pCmdTed->tprCursor.lPos-x0);
  return 0;
}

/*>>> EdEnter()
*/
LONG   EXPENTRY EdEnter(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc) return RxERROR;
  int nLeft  = 0;
  PSZ pszCur = pCmdTed->pBuf->aPLines[pCmdTed->tprCursor.lLine]->sz;
  if (pCmdTed->fAutoidentMode)
    while(pszCur[nLeft]==' ' || pszCur[nLeft]=='\t') nLeft++;
  if (nLeft >= pCmdTed->tprCursor.lPos) nLeft = 0;
  char *pch = (char*)malloc(nLeft+1);
  if (pch)
  {
    pch[0] = LF;
    memcpy(pch+1,pszCur,nLeft);
    if (!pCmdTed->InsertText(pch,nLeft+1,TRUE,BLOCK_STREAM))
    {
      pCmdTed->HideSel();
      rx_set_retstr(retstr,"ok");
    }
  }
  return 0;
}

/*>>> EdFind(<pattern>, <Options>)
>
> Options: char set of:
>   I - use the current word instead the pattern, 'W+ C-'   // Only one of {I,L} can be specifyed
>   L - use the last pattern ant 'CW' instead the specifyed // Only one of {I,L} can be specifyed
>   D - don't move it to the window bottom
>   W - whole word
>   C - case sensitive
>   S - skip cur. position    // Conflicts with {G,M,I}
>   B - back direction        // Only one of {G,B,M} can be specifyed
>   G - find global           // Only one of {G,B,M} can be specifyed
>   M - only match selection  // Only one of {G,B,M} can be specifyed
>
> Returns "<line>,<pos>" string or RxERROR when an error occured or the
> pattern required is not found.
>
*/

LONG   EXPENTRY EdFind(
  PSZ        name,                     /* function name              */
  LONG       argc,                     /* count of arguments         */
  PRXSTRING  argv,                     /* argument RXSTRINGs         */
  PSZ        queue,                    /* current Rexx queue         */
  PRXSTRING  retstr )                  /* returned string value      */
{
  static BOOL fLastC;
  static BOOL fLastW;
  if (!pCmdTed || argc!=2) return RxERROR;
  retstr->strlength = 0;
  retstr->strptr[0] = 0;

  BOOL       fI = (BOOL)strchr(argv[1].strptr,'I');
  FINDSTRUCT fs;
  fs.tprPos = pCmdTed->tprCursor;
  strncpy(fs.szFind,argv[0].strptr,sizeof(fs.szFind));
  if (fI)
  {
    LONG lPos;
    PSZ psz = (PSZ)WinSendMsg(pCmdTed->hEd,TM_GETCURWORD,0,MPARAM(&lPos));
    if (!psz) return RxERROR;
    strncpy(fs.szFind,psz,sizeof(fs.szFind));
    fs.tprPos.lPos = lPos; // Start from the beginning of this word
    free(psz);
    fs.fWholeWord      = TRUE;
    fs.fCaseSensitive  = FALSE;
  }
  else
  {
    if (strchr(argv[1].strptr,'L'))
    {
      strcpy(fs.szFind,ed_szLastPattern);
      fs.fWholeWord      = fLastW;
      fs.fCaseSensitive  = fLastC;
    }
    else
    {
      fLastW = fs.fWholeWord     = !!strchr(argv[1].strptr,'W');
      fLastC = fs.fCaseSensitive = !!strchr(argv[1].strptr,'C');
    }
  }
  fs.szFind[sizeof(fs.szFind)-1] = '\0';
  if (!fs.szFind[0]) return RxERROR;
  if (!fI) strcpy(ed_szLastPattern,fs.szFind);

  fs.fAction         = FA_FINDNEXT;
  if (strchr(argv[1].strptr,'G')) fs.fAction     = FA_FINDGLOBAL;
  if (strchr(argv[1].strptr,'B')) fs.fAction     = FA_FINDBACK;
  if (strchr(argv[1].strptr,'M')) fs.fAction     = FA_MATCHSEL;
  fs.fGotoFound      = TRUE;
  fs.fShowDownScreen = !strchr(argv[1].strptr,'D');

  if (strchr(argv[1].strptr,'S') || fI)
  {
    if (fs.fAction == FA_FINDNEXT)  fs.tprPos.lPos ++;
    else if     (fs.tprPos.lPos)    fs.tprPos.lPos --;
         else if(fs.tprPos.lLine)  {fs.tprPos.lLine--; fs.tprPos.lPos = 0x1fffffff;}
  }
  if (WinSendMsg(pCmdTed->hEd,TM_FIND,MPARAM(&fs),0))
  {
    sprintf(retstr->strptr,"%u,%u",fs.tprPos.lLine,fs.tprPos.lPos);
    retstr->strlength = strlen(retstr->strptr);
    return 0;
  }
  else
    return RxERROR;
}


/*>>> EdFindBracket() : "line,pos" : "";
/  Find the pair bracket
*/
LONG   EXPENTRY EdFindBracket(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc) return RxERROR;
  if (WinSendMsg(pCmdTed->hEd,TM_GOBRACKET,0,0))
    rx_set_retpos(retstr,pCmdTed->tprCursor.lLine,pCmdTed->tprCursor.lPos);
  return 0;
}

/*>>> EdGoto([<line>], [<pos>] [,IFEXISTS] [,SELECT] ): "line,pos" | "";
*/
LONG   EXPENTRY EdGoto(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc>4) return RxERROR;
  TPR  tpr,tpr0;
  BOOL fIfExists = FALSE;
  BOOL fSelect   = FALSE;
  for (int n=2; n<=3; n++)
  {
    if (argc>n)
    {
      if (!sf_stricmp(argv[n].strptr,"IFEXISTS") && !fIfExists) fIfExists = TRUE;
      if (!sf_stricmp(argv[n].strptr,"SELECT")   && !fSelect  ) fSelect   = TRUE;
      else return RxERROR;
    }
  }
  tpr0.lLine = pCmdTed->tprCursor.lLine;
  tpr0.lPos  = pCmdTed->tprCursor.lPos;
  tpr.lLine  = (argc>=1 && argv[0].strptr) ? strtol(argv[0].strptr,0,0) : tpr0.lLine;
  tpr.lPos   = (argc>=2 && argv[1].strptr) ? strtol(argv[1].strptr,0,0) : tpr0.lPos;
  if (tpr.lLine > pCmdTed->pBuf->lMaxLineNum)
    if (fIfExists) return 0;
    else           tpr.lLine = pCmdTed->pBuf->lMaxLineNum;
  WinSendMsg(pCmdTed->hEd,TM_SETSEL,MPARAM(&tpr),MPARAM(fSelect ? &tpr0 : &tpr));
  rx_set_retpos(retstr,pCmdTed->tprCursor.lLine,pCmdTed->tprCursor.lPos);
  return 0;
}

/*>>> EdFindWord(NEXT | PREVIOUS [,SELECT]): "line,pos" | "";
*/
LONG   EXPENTRY EdFindWord(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc<1 || argc>2) return RxERROR;
  BOOL fNext = (           !strcmp(argv[0].strptr,"NEXT"));
  BOOL fSel  = (argc==2 && !strcmp(argv[1].strptr,"SELECT"));
  LONG lLine = pCmdTed->tprCursor.lLine;
  LONG lPos  = pCmdTed->tprCursor.lPos;
  char *pch  = pCmdTed->pBuf->aPLines[lLine]->sz;
  LONG lLen  = strlen(pch);
  if (fNext)
  {
    if (lPos>=lLen) lPos++;
    else
    {
      while(lPos<lLen &&  sf_iswordbreak(pch[lPos])) lPos++;
      while(lPos<lLen && !sf_iswordbreak(pch[lPos])) lPos++;
    }
  }
  else
  {
    if (lPos = min(lPos,lLen)) lPos--;
    while(lPos>0 &&  sf_iswordbreak(pch[lPos])) lPos--;
    while(lPos>0 && !sf_iswordbreak(pch[lPos])) lPos--;
    if (sf_iswordbreak(pch[lPos]) && !sf_iswordbreak(pch[lPos+1])) lPos++;
  }
  if (!fSel) pCmdTed->HideSel();
  pCmdTed->GotoCoord(lLine, lPos, TRUE);
  if (fSel) pCmdTed->InvalidateLines(lLine,lLine);
  else      pCmdTed->tprAnchor = pCmdTed->tprCursor;
  if (fSel && pCmdTed->ulBlockMode==BLOCK_COLUMN)
    pCmdTed->InvalidateLines(pCmdTed->tprAnchor.lLine,lLine);
  WinSendMsg(pCmdTed->hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),
             MPARAM(pCmdTed->tprAnchor != pCmdTed->tprCursor));
  rx_set_retpos(retstr,pCmdTed->tprCursor.lLine,pCmdTed->tprCursor.lPos);
  return 0;
}

/*>>> EdHome([SELECT]): "dx" | "";
*/
LONG   EXPENTRY EdHome(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc>1) return RxERROR;
  int  x0     = pCmdTed->tprCursor.lPos;
  BOOL fSel   = (argc==1 && !strcmp(argv[0].strptr,"SELECT"));
  LONG lLine  = pCmdTed->tprCursor.lLine;
  PSZ  pszCur = pCmdTed->pBuf->aPLines[lLine]->sz;
  LONG lPos;
  for (lPos=0; pszCur[lPos]==' ' || pszCur[lPos]=='\t'; lPos++);
  if (!pszCur[lPos])                   lPos = 0;
  if (pCmdTed->tprCursor.lPos == lPos) lPos = 0;
  if (!fSel) pCmdTed->HideSel();
  pCmdTed->GotoCoord(lLine, lPos, TRUE);
  if (fSel) pCmdTed->InvalidateLines(lLine,lLine);
  else      pCmdTed->tprAnchor = pCmdTed->tprCursor;
  if (fSel && pCmdTed->ulBlockMode==BLOCK_COLUMN)
    pCmdTed->InvalidateLines(pCmdTed->tprAnchor.lLine,lLine);
  WinSendMsg(pCmdTed->hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),
             MPARAM(pCmdTed->tprAnchor != pCmdTed->tprCursor));
  rx_set_retval(retstr,pCmdTed->tprCursor.lPos-x0);
  return 0;
}

/*>>> EdMode(INSERTION [,ON | OFF])                          : <mode_before>;
>     EdMode(SELECTION [,STREAM | COLUMN | LINES] [,NOTIFY]) : <mode_before>;
> INSERTION:
>   Set or toggle insertion mode
> SELECTION:
>   Set or set next selection mode
> Returns: mode before call of ""
*/
LONG   EXPENTRY EdMode(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || !argc) return RxERROR;
  if (!strcmp(argv[0].strptr,"INSERTION"))
  {
    BOOL fNewIns = !pCmdTed->fInsertMode;
    rx_set_retstr(retstr,fNewIns ? "OFF" : "ON");
    if (argc>1) fNewIns = !strcmp(argv[1].strptr,"ON");
    pCmdTed->fInsertMode = fNewIns;
    pCmdTed->RestartCursor();
  }
  else if (!strcmp(argv[0].strptr,"SELECTION"))
  {
    ULONG ulMode = (pCmdTed->ulBlockMode) % 3 + 1;  // 1..3 values
    if (argc>1)
    {
      if      (!strcmp(argv[1].strptr,"STREAM")   ) ulMode = BLOCK_STREAM;
      else if (!strcmp(argv[1].strptr,"COLUMN")   ) ulMode = BLOCK_COLUMN;
      else if (!strcmp(argv[1].strptr,"LINES" )   ) ulMode = BLOCK_LINES;
    }
    char asz[3][10] = {"STREAM","COLUMN","LINES"};
    rx_set_retstr(retstr,asz[pCmdTed->ulBlockMode-1]);
    pCmdTed->ulBlockMode = ulMode;
    pCmdTed->InvalidateLines(0,0x7fffffff);
    if (argc && !strcmp(argv[argc-1].strptr,"NOTIFY"))
    {
      char asz[3][10] = {"Stream","Column","Lines"};
      char sz[30];
      sprintf(sz,"Selection mode: %s",asz[ulMode-1]);
      WinSendMsg(pCmdTed->hNotifyWindow, WM_CONTROL, MPFROM2SHORT(0,TN_COMMENT),MPARAM(sz));
    }
  }
  else return RxERROR;
  return 0;
}
/*>>> EdSelMode([STREAM | COLUMN | LINES],[NOTIFY])
/ W/o argument sets the next
*/
LONG   EXPENTRY EdSelMode(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  if (!pCmdTed || argc>2) return RxERROR;

}





/*>>> EdMouseBlock(dx,dy, MOVE | COPY)
*/
LONG   EXPENTRY EdMouseBlock(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc!=3) return RxERROR;
  char *pch;
  BOOL fMove     = (!strcmp(argv[2].strptr,"MOVE"));
  TPR  tpr       = TPR(strtol(argv[1].strptr,&pch,0), strtol(argv[0].strptr,&pch,0));
  tpr           += pCmdTed->tprCursor;
  char *pchBlock = pCmdTed->AllocBlock(FALSE);
  if (fMove) pCmdTed->DeleteAction(DEL_SELECTION);
  tpr.UpdatePos(&pCmdTed->tcdStruct);
  pCmdTed->GotoCoord(tpr.lLine,tpr.lPos,TRUE);
  if (pchBlock)
  {
    pCmdTed->InsertText(pchBlock,strlen(pchBlock),TRUE,pCmdTed->ulBlockMode);
    free(pchBlock);
  }
  pCmdTed->HideSel();
  return 0;
}

/*>>> EdMove(dx,dy [,SELECT]) : "line,pos" | "";
*/
LONG   EXPENTRY EdMove(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc<2 || argc>3 ) return RxERROR;
  char *pch;
  BOOL fSel   = (argc==3 && !strcmp(argv[2].strptr,"SELECT"));
  if (!fSel) pCmdTed->HideSel();
  LONG lLine0 = pCmdTed->tprCursor.lLine;
  LONG dx     = strtol(argv[0].strptr,&pch,0);
  LONG lPos   = max(    0, pCmdTed->tprCursor.lPos  + dx);
  LONG lLine  = min(max(0, pCmdTed->tprCursor.lLine + strtol(argv[1].strptr,&pch,0)), pCmdTed->pBuf->lMaxLineNum);
  pCmdTed->GotoCoord(lLine, lPos, TRUE);
  if (fSel) pCmdTed->InvalidateLines(lLine0,lLine);
  else      pCmdTed->tprAnchor = pCmdTed->tprCursor;
  if (fSel && dx && pCmdTed->ulBlockMode==BLOCK_COLUMN)
    pCmdTed->InvalidateLines(pCmdTed->tprAnchor.lLine,lLine);
  WinSendMsg(pCmdTed->hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),
             MPARAM(pCmdTed->tprAnchor != pCmdTed->tprCursor));
  rx_set_retpos(retstr,pCmdTed->tprCursor.lLine,pCmdTed->tprCursor.lPos);
  return 0;
}

/*>>> EdMoveSel(UP | DOWN | LEFT | RIGHT | PAGEUP | PAGEDOWN | <dy>): "dy" | "";
*/
LONG   EXPENTRY EdMoveSel(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  static BOOL     fFirstCall = TRUE;
  static SymTable stab;
  if (fFirstCall)
  {
    stab.AddName("UP",      MOVSEL_UP);
    stab.AddName("DOWN",    MOVSEL_DOWN);
    stab.AddName("LEFT",    MOVSEL_LEFT);
    stab.AddName("RIGHT",   MOVSEL_RIGHT);
    stab.AddName("PAGEUP",  MOVSEL_PAGEUP);
    stab.AddName("PAGEDOWN",MOVSEL_PAGEDOWN);
    fFirstCall = FALSE;
  }

  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc!=1) return RxERROR;

  ULONG ulMSMode = stab.SearchName(argv[0].strptr);
  LONG  dy       = 0;
  if (ulMSMode == -1 && !(dy = strtol(argv[0].strptr,0,0))) return RxERROR;
  dy = pCmdTed->MoveSel(ulMSMode,dy);
  if (dy) rx_set_retval(retstr,dy);
  return 0;
}

/*>>> EdPage( UP | DOWN [,SELECT]): "dy" | "";
*/
LONG   EXPENTRY EdPage(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc<1 || argc>2) return RxERROR;
  int  y0          = pCmdTed->tprCursor.lLine;
  BOOL fPgUp       = !strcmp(argv[0].strptr,"UP");
  LONG lPage       = (pCmdTed->xWindowHeight+pCmdTed->xLineHeight-1)/pCmdTed->xLineHeight - 2;
  lPage            = max(lPage,1);
  BOOL fSel   = (argc==2 && !strcmp(argv[1].strptr,"SELECT"));
  if (fPgUp) lPage = -min(lPage, pCmdTed->tprCursor.lLine);
  else       lPage =  min(lPage, pCmdTed->pBuf->lMaxLineNum - pCmdTed->tprCursor.lLine);
  LONG       lLine = pCmdTed->tprCursor.lLine + lPage;
  if (!fSel) pCmdTed->HideSel();
  if (lPage)
  {
    pCmdTed->lFirstScrLine = max(0, pCmdTed->lFirstScrLine + lPage);
    pCmdTed->GotoCoord(lLine, pCmdTed->tprCursor.lPos, TRUE);
    if (!fSel) pCmdTed->tprAnchor = pCmdTed->tprCursor;
    WinInvalidateRect(pCmdTed->hEd,0,FALSE);
    WinSendMsg(pCmdTed->hNotifyWindow,WM_CONTROL,MPFROM2SHORT(0,TN_SELPRESENTS),MPARAM(FALSE));
  }
  if (y0 = pCmdTed->tprCursor.lLine-y0) rx_set_retval(retstr,y0);
  return 0;
}

/*>>> EdPaste([<string>]): "ok" | "";
/ Строка <string> может быть пустой, если она не задана -
/ берет из clipboard-а, только если он не пуст.
*/
LONG   EXPENTRY EdPaste(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc>1) return RxERROR;
  PSZ  pszPaste = 0;
  BOOL fClipb   = !argc;
  LONG lRC      = 0;
  if (fClipb)
  {
    if (WinOpenClipbrd(pCmdTed->hAB)) pszPaste = (PCHAR)WinQueryClipbrdData(pCmdTed->hAB,CF_TEXT);
    else                              lRC      = RxERROR;
  }
  else                                pszPaste = argv[0].strptr;

  if (pszPaste)
  {
    ULONG ulBlockMode = pCmdTed->ulBlockMode;
    if (ulBlockMode == BLOCK_COLUMN && !pCmdTed->fInsertMode)
      ulBlockMode = BLOCK_COLUMNOVER;
    if ((!fClipb || pszPaste[0]) && pCmdTed->tprAnchor!=pCmdTed->tprCursor)
      lRC = pCmdTed->DeleteAction(DEL_SELECTION) ? RxERROR : lRC;
    if (pszPaste[0] && pCmdTed->InsertText(pszPaste,strlen(pszPaste),TRUE,ulBlockMode))
      lRC = RxERROR;
    pCmdTed->HideSel();
    if (!lRC) rx_set_retstr(retstr,"ok");
  }
  if (fClipb) WinCloseClipbrd(pCmdTed->hAB);
  return lRC;
}

/*>>> EdQuery(<arg>)
/ <arg>:
/   INSMODE:       INSERT | OVERWRITE
/   CURWORD:       the word under the cursor
/   LEFTWORD:      the [part of the] word on the left to the cursor
/   CURLINE:       cur. line text
/   CURCHAR:       character at the cursor or ""
/   SELECTION:     "line,pos,lineAnchor,posAnchor,selmode"
/   SELECTIONSORT: "line1,pos1,line2,pos3,selmode" (1 <= 2)
/   MAXLINE:       last line number ( .GE. 0)
*/
LONG   EXPENTRY EdQuery(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc!=1) return RxERROR;
  BOOL  f;
  if      (!strcmp(argv[0].strptr,"INSMODE"))
    strcpy(retstr->strptr, pCmdTed->fInsertMode ? "INSERT" : "OVERWRITE");
  else if ((f = !strcmp(argv[0].strptr,"CURWORD")) || !strcmp(argv[0].strptr,"LEFTWORD"))
  {
    LONG  lLen;
    LONG  lPos          = pCmdTed->tprCursor.lPos;
    PSZ   pszLine       = pCmdTed->pBuf->aPLines[pCmdTed->tprCursor.lLine]->sz;
    PSZ            pszW = 0;;
    if (f || lPos) pszW = pCmdTed->GetWord(pszLine, pCmdTed->tprCursor.lPos-(f?0:1), &lLen, FALSE);
    if (pszW)
    {
      char *pchE = f ? pszW+lLen : pszLine+lPos;
      lLen       = pchE - pszW;
      if (lLen<254) rx_set_retstr(retstr,pszLine);
    }
  }
  else if (!strcmp(argv[0].strptr,"CURLINE"))
  {
    PSZ   pszLine = pCmdTed->pBuf->aPLines[pCmdTed->tprCursor.lLine]->sz;
    LONG  lLen    = strlen(pszLine);
    if (lLen>=254 &&  DosAllocMem((PPVOID)&retstr->strptr, lLen+1, PAG_COMMIT | PAG_WRITE))
      return RxERROR;
    rx_set_retstr(retstr,pszLine);
  }
  else if (!strcmp(argv[0].strptr,"CURCHAR"))
  {
    PSZ   pszLine = pCmdTed->pBuf->aPLines[pCmdTed->tprCursor.lLine]->sz;
    if (pCmdTed->tprCursor.lPos<strlen(pszLine))
    {
      retstr->strlength = 1;
      retstr->strptr[0] = pszLine[pCmdTed->tprCursor.lPos];
      retstr->strptr[1] = 0;
    }
  }
  else if ((f = !strcmp(argv[0].strptr,"SELECTION")) || !strcmp(argv[0].strptr,"SELECTIONSORT"))
  {
    TPR tpr0 = pCmdTed->tprCursor;
    TPR tpr1 = pCmdTed->tprAnchor;
    if (!f && tpr0>tpr1) { TPR t=tpr0; tpr0=tpr1; tpr1=t;}
    char asz[3][10] = {"STREAM","COLUMN","LINES"};
    sprintf(retstr->strptr,"%d,%d,%d,%d,%s", tpr0.lLine, tpr0.lPos, tpr1.lLine, tpr1.lPos, asz[pCmdTed->ulBlockMode-1]);
    retstr->strlength = strlen(retstr->strptr);
  }
  else if (!strcmp(argv[0].strptr,"MAXLINE"))
    rx_set_retval(retstr,pCmdTed->pBuf->lMaxLineNum);
  else return RxERROR;
  return 0;
}


/*>>> EdRedo(): "ok" | "";
*/
LONG   EXPENTRY EdRedo(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc) return RxERROR;
  if (pCmdTed->DoRedo()) Beep();
  else                   rx_set_retstr(retstr,"ok");
  return 0;
}


/*>>> EdSelWord(): "ok" | "";
*/
LONG   EXPENTRY EdSelWord(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc)                           return RxERROR;
  if (WinSendMsg(pCmdTed->hEd,TM_SELCURWORD,0,0)) rx_set_retstr(retstr,"ok");
  return 0;
}

/*>>> EdTab(): "<pos>";
*/
LONG   EXPENTRY EdTab(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  if (!pCmdTed || argc) return RxERROR;
  LONG  lPos  = pCmdTed->tprCursor.lPos;
  LONG  lLine = pCmdTed->tprCursor.lLine;
  LONG  len   = 0;
  if (!pCmdTed->fTabCharsMode)
  {
    if (pCmdTed->fSmartTab && lLine)
    {
      char *pchPrev = pCmdTed->pBuf->aPLines[lLine-1]->sz;
      LONG  lPrev   = strlen(pchPrev);
      if (lPrev > lPos)
      {
        while(lPos+len<lPrev && pchPrev[lPos+len]!=' ') len++;
        while(lPos+len<lPrev && pchPrev[lPos+len]==' ') len++;
        if (lPos+len==lPrev) len = 0;
      }
    }
    if (!len) len = pCmdTed->lTabSize - lPos % pCmdTed->lTabSize;
  }
  if (len)
  {
    char *psz = (char*)malloc(len);
    memset(psz,' ',len);
    pCmdTed->InsertText(psz,len,TRUE,BLOCK_STREAM);
    free(psz);
  }
  else pCmdTed->InsertText("\t",1,TRUE,BLOCK_STREAM);
  pCmdTed->HideSel();
  rx_set_retval(retstr,pCmdTed->tprCursor.lPos);
  return 0;
}

/*>>> EdType( string )
*/
LONG   EXPENTRY EdType(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  if (!pCmdTed || argc!=1) return RxERROR;
  MRESULT rc;
  char *pch  = argv[0].strptr;
  char *pchE = strpbrk(pch,"\r\n"); if(pchE) *pchE = '\0'; // Can't type it !
  if (pCmdTed->tprAnchor!=pCmdTed->tprCursor)
    if(pCmdTed->DeleteAction(DEL_SELECTION)) return RxERROR;
  rc = pCmdTed->TypeString(pch);
  if (rc) return RxERROR;
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  return 0;
}


/*>>> EdUndo(): "ok" | "";
*/
LONG   EXPENTRY EdUndo(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  rx_set_retstr(retstr,"");
  if (!pCmdTed || argc) return RxERROR;
  if (pCmdTed->DoUndo()) Beep();
  else                   rx_set_retstr(retstr,"ok");
  return 0;
}

/*>>> EdWordCase( UPPER | SAME )
*/
LONG   EXPENTRY EdWordCase(PSZ name,LONG argc,PRXSTRING argv,PSZ queue,PRXSTRING retstr )
{
  if (!pCmdTed || argc!=1) return RxERROR;

  int   nMode;
  retstr->strlength = 0;
  retstr->strptr[0] = 0;
  if      (!strcmp(argv[0].strptr,"UPPER")) nMode =  1;
  else if (!strcmp(argv[0].strptr,"SAME" )) nMode =  0;
  else return RxERROR;
  WinSendMsg(pCmdTed->hEd,TM_SETCASE,MPARAM(nMode),0);
  return 0;
}



void RegisterTedCommands()
{
  RxRegister("EdCopy",        (PFN)EdCopy,        FALSE);
  RxRegister("EdDelete",      (PFN)EdDelete,      TRUE );
  RxRegister("EdDupSel",      (PFN)EdDupSel,      FALSE);
  RxRegister("EdEnd",         (PFN)EdEnd,         FALSE);
  RxRegister("EdEnter",       (PFN)EdEnter,       TRUE );
  RxRegister("EdFind",        (PFN)EdFind,        FALSE);
  RxRegister("EdFindBracket", (PFN)EdFindBracket, FALSE);
  RxRegister("EdFindWord",    (PFN)EdFindWord,    TRUE );
  RxRegister("EdGoto",        (PFN)EdGoto,        FALSE);
  RxRegister("EdHome",        (PFN)EdHome,        FALSE);
  RxRegister("EdMode",        (PFN)EdMode,        FALSE);
  RxRegister("EdMouseBlock",  (PFN)EdMouseBlock,  FALSE);
  RxRegister("EdMove",        (PFN)EdMove,        TRUE );
  RxRegister("EdMoveSel",     (PFN)EdMoveSel,     TRUE );
  RxRegister("EdPage",        (PFN)EdPage,        TRUE );
  RxRegister("EdPaste",       (PFN)EdPaste,       TRUE);
  RxRegister("EdQuery",       (PFN)EdQuery,       FALSE);
  RxRegister("EdRedo",        (PFN)EdRedo,        TRUE );
  RxRegister("EdSelWord",     (PFN)EdSelWord,     FALSE);
  RxRegister("EdTab",         (PFN)EdTab,         TRUE );
  RxRegister("EdType",        (PFN)EdType,        TRUE );
  RxRegister("EdUndo",        (PFN)EdUndo,        TRUE );
  RxRegister("EdWordCase",    (PFN)EdWordCase,    FALSE);
}


