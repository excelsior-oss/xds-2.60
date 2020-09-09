/*
*
*  File: FMAN.H
*
*  XDS Shell: file management
*
*  (c) 1996,1997 by *FSA & NHA Inc.
*
*/

#ifndef  __FMAN__
#define  __FMAN__
#include "XDSBASE.H"

#define  MAXFGROUPS     10  // Total filegroups can be created
#define  MAXBOOKMARKS   10  // Duplicated definition in xdsbase.h
#define  IN_AUTO_WINDOW 0x7fff
/*-------------------------------------------------------------------------*/
/*             Useful functions                                            */
/*-------------------------------------------------------------------------*/
void CenterChildWindow (HWND hWnd);
BOOL CreateNewDir      (PSZ pszTarg, PSZ pszSrc); // if (pszTarg) it := the directory name(szSrc) w/o last '\'



enum CLIENTTYPE {
  CLIENT_ABSENT,
  CLIENT_EDITOR,
  CLIENT_MSGLIST,
  CLIENT_SRCLIST
};

struct CLIENTINFO
{
  CLIENTTYPE    clType;
  HWND          hWnd;         // Client window handle (client window)
  HWND          hFrame;       // Client window handle (frame window or 0)
  CLIENTINFO(CLIENTTYPE clTyp)
  {
    clType = clTyp;
    hWnd   = hFrame = NULL;
  }
};
typedef CLIENTINFO *PCLIENTINFO; // MDI client's handle

// Used to open new editor
// and to write profile
struct TEDWINPARAMS
{
  LONG         x0;
  LONG         y0;
  LONG         cx;
  LONG         cy;
  BOOL         fMax;
  BOOL         fMin;
  BOOL         fHidden;
  static BOOL  fMaxNew;
  static int   nSpill;
  TEDWINPARAMS (HWND hDesk);
};
typedef TEDWINPARAMS *PTEDWINPARAMS;


// --- FMAN's internal types:

    struct  FILEGROUP;
    typedef FILEGROUP *PFILEGROUP;
    struct  FILEED;
    typedef FILEED    *PFILEED;
    class   FILEINFO;
    typedef FILEINFO  *PFILEINFO;

    class   FILEINFO
    { //NOTE: use 'new' to allocate it; no delete needs
    public:
           FILEINFO();
      void SetInfo     (PSZ szPath);
      int  CheckInfo   (PSZ szPath);  // Returns: 0 - Ok ob; 1 - updated; 2 - removed
      void Capture     () {     ++nCapt;}                   // Additional user
      void Release     () {if (!--nCapt) delete (this);}    // One user disappeared
      void Free        () {fSet = FALSE;}
      void TextChanged (PTCD ptcd);
      PPAINTER         pPainter;
    private:
      FILESTATUS3    fs3;
      BOOL           fSet;  // fs3 is Ok
      int            nCapt; // initial = 1;
      TPR            atprBM[MAXBOOKMARKS]; // (-1,-1) == free
      friend struct  FILEED;
    };

    struct FILEED : public CLIENTINFO
    {
      PFILEGROUP   pFGroup;      // The group this FILEED is belonged to
      PSZ          pszFilename;  // malloc()ed filename of NULL if 'Nonamed'
      int          nNonNum;      // For 'Noname' : noname number
      PFILEED      next;         // List of us
      PFILEINFO    pFInfo;       // Shared fileinfo
      static BOOL  fShortName;   // Dot't include path in the name

                   FILEED      (PFILEGROUP pFG);
                  ~FILEED      ();
      BOOL         SaveChanges (HWND hMsgWnd);
      void         Destroy     ();
      BOOL         SaveFile    (HWND hMsgWnd, BOOL fSaveAs, PSZ pszDestDir, PSZ szNewName = 0);
      void         NameSelf    ();
      void         QueryName   (PCH pBuf, LONG lBufLen);
      BOOL         CheckUpdated(HWND hMsgWnd); // Ret: FALSE - editor contents has been updated (or losed)
      PLONG        QDisabledBMs();
      PTPR         GoBM        (ULONG usCmd);
      PTPR         SetBM       (ULONG usCmd);
      PTPR         QueryBM     (ULONG usCmd);
    };

    struct FILEGROUP
    {
      int          nGroup;       // Group number (to get it easy)
      PFILEED      pFEdList;     // List of FILEED structures
      PFILEED      pFEdActive;   // The active editor (or 0)

                   FILEGROUP  ()  { memset(this,0,sizeof(*this));}
                  ~FILEGROUP  ();
      PFILEED      GOpenEditor(PSZ pszFilename, HWND hMsgWnd, PTEDWINPARAMS pTWP, PFILEED pFEIn, int nNonNum, HWND *phEDUseShared = 0);
      void         DestroyAll ();
    };

/*-------------------------------------------------------------------------*/
/*               GROUPOFICE   class definition                             */
/*-------------------------------------------------------------------------*/

class GROUPOFFICE
{
public:
  GROUPOFFICE    ();
 ~GROUPOFFICE    ();

  /* Open new file in the group (or in the spicifyed window)
  >
  >  ULONG         nGroup (in)      - Group to open editor in (may be IN_AUTO_WINDOW - to open in/new [the]
  >                                   auto window depends on f1AutoWin state)
  >  PSZ           pszFilename (in) - File to open or 0 to create 'Noname' window
  >  HWND          nMsgWnd (in)     - Window to use as a parent to show a messages. If 0 - show nothing.
  >  PTEDWINPARAMS pTWP (in)        - TED creation data. If 0 - use default
  >  PFILEED       pFEIn   (in,def) - If not 0 - try to open in its window
  >  int           nNonNum (in,def) - When a 'Noname' file needs denotes it's number (If <0 - autogenerate)
  >  Return value (BOOL)            - Success indicator
  */
  BOOL OpenEditor(ULONG nGroup, PSZ pszFilename, HWND hMsgWnd, PTEDWINPARAMS pTWP,
                  PFILEED pFEIn = 0, int nNonNum=-1);

  /* CloseWindow
  >
  >  HWND hEd (in)       - Window to close (0 - close all)
  >  HWND nMsgWnd (in)   - Window to use as a parent to show a messages. If 0 - close immediatly
  >  Return value (BOOL) - Success indicator
  */
  BOOL CloseWindow(HWND hEd, HWND hMsgWnd);

  /* SaveOneFile
  >
  >  HWND hEd            - hEd to save or 0 to save the active
  >  BOOL fSaveAs        - 'Save as' mode (for 'Noname - anyway)
  >  HWND hMsgWnd        - Window to use as a parent to show a messages
  >  Return value        - success state
  */
  BOOL SaveOneFile (HWND hEd, BOOL fSaveAs, HWND hMsgWnd, PSZ szNewName = 0);

  /* SaveAll (save all the changed buffers)
  >
  >  BOOL fPrompt        - Prompt before save (if it is need)
  >  HWND nMsgWnd        - Window to use as a parent to show a messages
  >  PSZ  szNewName      - When fSaveAs: if not 0 - try to save using this name
  >  Return value        - hEd was not succesfully saved OR 0 - Ok.
  */
  HWND SaveAll (BOOL fPrompt, HWND hMsgWnd);

  /* Reload the file
  >
  >  PFILEED pFE     - FILEED to reload or 0 to reload the active
  >  HWND    hMsgWnd - Window to use as a parent to show a messages
  >  Return value    - TRUE - file has been reloaded
  */
  BOOL Reload(PFILEED pFE, HWND hMsgWnd);

  /* KnownFilename
  >
  >  Returns one of the editors contained the szFile file. If no one - returns 0
  */
  HWND KnownFilename (PSZ szFile);

  /* AttachPainter
  >
  >  HWND   hEd          - the editor to associate painter with
  >  PSZ    pszPainterId - painter identificator (or 0 to associate no painter)
  >  BOOL   fMsgON       - Enable displaying possible error messages
  >  Returns             - TRUE when Ok.
  */
  BOOL AssociatePainter(HWND hEd, PSZ pszPainterId, BOOL fMsgON);

  /* QueryPainter
  >
  >  HWND   hEd          - the editor wich painter we are interested in
  >  Returns             - the painter associated or 0
  >
  */
  PPAINTER QueryPainter(HWND hEd);

  /* GotoEd
  >
  >  HWND hEd (in)       - Editor to go into
  >  BOOL fShowOnly (in) - Denotes that the window doesn't recives the focus immediatly
  */
  void GotoEd (HWND hEd, BOOL fShowOnly);

  /* GotoGroup
  >
  >  ULONG nGroup (in)   - The group to activate
  >  BOOL fShowOnly (in) - Denotes that the window doesn't recives the focus immediatly
  >  Return value (BOOL) - FALSE if no windows in the group exists
  */
  BOOL GotoGroup (ULONG nGroup, BOOL fShowOnly);

  void GoNextGroup (BOOL fPrevious);
  void GoNextEditor(BOOL fPrevious);

  LONG SetAllCaptions(LONG n1stIt);

  /* EdActivated
  >
  > The editors owner must notify us when a editor window becomes active.
  > hEd=0 means that no active editor now
  */
  void EdActivated (HWND hEd);

  BOOL    Mv2Group           (HWND hFrame, int nGroup); // Move editor to group (if nGroup valid), returns IsEdFrame(hFrame)

  void    CheckFileTimes     (HWND hEd); // 0 - for all
  void    ChangeMaxNewState  (int nState=-1); // nState = 1/0/other => ON/OFF/Change
  void    ChangeAutosaveState(int nState=-1); // nState = 1/0/other => ON/OFF/Change
  void    ChangeMakeBakState (int nState=-1); // nState = 1/0/other => ON/OFF/Change
  void    Change1AutoWinState(int nState=-1); // nState = 1/0/other => ON/OFF/Change
  LONG    QueryActiveGroup   () {return nActiveGroup;}
  PFILEED QueryActiveFEd     () {return aFG[nActiveGroup].pFEdActive;}
  PSZ     QueryEdFile        (HWND hEd); // :0 for nonamed
  HWND    EnumEd             ();
  HWND    NextEnumEd         ();
  void    TextChanged        (PTCD ptcd);
  void    ReorderWBar        ();

  /* Read profile
  >
  > LONG slot    - slot number to read from
  > HWND nMsgWnd - Window to use as a parent to show a messages
  > Return value - success state
  > NOTE: all the current configuration & buffers will be LOSED!
  >
  */
  BOOL    RProfile         (LONG slot);

  /* Write to profile
  >
  > LONG slot      - slot number to write into
  > BOOL fContents - if FALSE - only the free slot contents wrote
  > HWND nMsgWnd   - Window to use as a parent to show a messages
  > Return value   - success state
  >
  */
  BOOL    WProfile         (LONG slot, BOOL fContents);

  static BOOL fAutosave; // Autosave ON state
  static BOOL fMakeBak;  // Craete '.bak' files
  static BOOL f1AutoWin; // Use 1/multiple auto window(s)

private:
  int            q_frame_ng(HWND hFrame); // ng : -1
  BOOL           mv2group(HWND hFrame, int ngTarg);
  FILEGROUP      aFG [MAXFGROUPS];
  ULONG          nActiveGroup;
  PFILEED        pfeEnum;
  static int     nNonamCount;
  static int     GenerateNonNum (){return nNonamCount++;};
  static PFILEED hEd2pFE        (HWND hEd);

  friend BOOL   curved_call(HWND, int);
  friend class  BUFSCONFIG;
  friend struct FILEGROUP;
};


/*-------------------------------------------------------------------------*/
/*               BUFSCONGIG   class definition                             */
/*-------------------------------------------------------------------------*/

enum BFITEMT {BFITEM_GROUP, BFITEM_FILE, BFITEM_NONAME, BFITEM_WRONG};

class BUFSCONFIG
// Scheme of the buffers configuration used to profiling or
//    to manage this configuration
{
public:
  BUFSCONFIG(BOOL fGroups);
 ~BUFSCONFIG() {Free();}
  LHANDLE AddItemNoname(LHANDLE hAfter, LONG lNonNum, PSZ pszName);    // lNonNum<0  => auto generate
                                                                       // pszName==0 => auto
  LHANDLE           AddItemGroup     (ULONG nGrp);                     // The next group begins
  LHANDLE           AddItemFile      (LHANDLE hAfter, PSZ szName);     // hAfter==0 => append
  BOOL              MoveItem         (LHANDLE h, LHANDLE hAfter);      // hAfter==0 => move to end, ret Ok
  LHANDLE           DeleteItem       (LHANDLE h);                      // Not for a group, h to select or 0=err
  BFITEMT           QueryItemType    (LHANDLE h);                      // BFITEM_(GROUP|FILE|NONAME)
  PSZ               QueryItemName    (LHANDLE h, BOOL fShort);         //
  ULONG             QueryItemNamePos (LHANDLE h);                      //
  int               QueryItemNum     (LHANDLE h);                      // nNonNum or nGroupNum
  LHANDLE           QVisibleParent   (LHANDLE h);                      //
  void              SetItemUL        (LHANDLE h, ULONG ul);            // User-defined info (initial = 0)
  void              SetItemName      (LHANDLE h, PSZ pszName);         // Re-assign the item's name
  void              SetItemNamePos   (LHANDLE h, ULONG ul);            //
  ULONG             QueryItemUL      (LHANDLE h);                      //
  BOOL              CloseGroup       (LHANDLE h);                      // Inverts and returns close state
  BOOL              QueryClosed      (LHANDLE h);                      // Returns close state
  BOOL              MarkFileActive   (LHANDLE h);                      // Go here after Apply(); Returns fOk
  LHANDLE           StartEnum        ();
  LHANDLE           NextEnum         (BOOL fPeekClosed=FALSE);
  BOOL              AutoBuild        ();
  BOOL              Apply            (HWND hMsgWnd);
  void              Free             ();

private:
  struct BUFINFO
  {
    BUFINFO();
   ~BUFINFO();
    BFITEMT         bfitemt;
    int             nNum;            // nNonNum OR nGroupNum
    BOOL            fGroupClosed;
    PSZ             pszName;
    PSZ             pszShortName;
    ULONG           ulUser;          //
    ULONG           ulNamePos;       // LONGFROM2SHORT(SHORT xLeft, SHORT xRight)
    BUFINFO        *next;
  };
  typedef BUFINFO *PBUFINFO;
  struct DELITEM
  {
    DELITEM(DELITEM **ppdi,PFILEED);
    PFILEED pFEDel;
    DELITEM *next;
  };
  typedef DELITEM *PDELITEM;
  BOOL        fGroups;           // TRUE/FALSE => groups/source list
  PBUFINFO    pbList;            // Elements of the group list (in the visual order)
  PDELITEM    pDelItems;         // List of the deleted editors (excluded from the pbList)
  PBUFINFO    pbiEnum;           // Last point for StartEnum & NextEnum methods
  void        bite (PFILEED pFE);
  PBUFINFO*   seek (LHANDLE h);
  LHANDLE     hActivateMe;       // Set be MarkFileActive
  static BOOL afGClosed[MAXFGROUPS];
};



//--- File dialog :
//
void    FileLoadDlg();
void    FastLoadDlg();

//--- Useful functions:
//
void    Win2WinCoord       (HWND hwSource, HWND hwTarget, PPOINTL pptl); // Converts *pptl to hwTarget's coord.
void    CenterChildWindow  (HWND hWnd);                                  // Center the window related to its parent
void    QueryCurrentPath   (char *szTarg);                                                 // Out: szTarg

//--- My drag realisation -------

#define MDM_DRAGOVER       (WM_USER+2000)
#define MDM_DRAGDROP       (WM_USER+2001)
#define MDM_DRAGEND        (WM_USER+2002)

enum MYDRAGTYPE            // Unequie values identifying the MYDRAGITEM contents
{
  MYDRAG_FILES,
  MYDRAG_GROUPITEM
};
struct MYDRAGITEM
{
  MYDRAGTYPE  mdt;
  PSZ         pszMalloced;    // drag will free it
  char        sz[CCHMAXPATH];
  ULONG       ul;
};
typedef MYDRAGITEM* PMYDRAGITEM;

//--- StartDrag
//
// It starts dragging operation and returns immediatly.
//
// HWND        hwndSource (in) - window to notify when operation terminated (or HWND_DESKTOP)
// PMYDRAGITEM pmdi (in)       - the item been dragged (StartDrag creates its own copy of it)
// HPOINTER    hptrDrop (in)   - pointer to show when dropping is possible
// HPOINTER    hptrNoDrop (in) - pointer to show when dropping is impossible
//
// Return value (BOOL)         - TRUE when error occured.
//
///// Remarks:
//
// MDM_DRAGOVER(PMYDRAGITEM pmdi, MPARAM xy) - is sent (like WM_MOUSMOVE) to the window under
//      the pointer. When the window returns TRUE the hptrDrop pointer is showed.
//      PMYDRAGITEM  pmdi
//      MPARAM       xy    - pointer coordinates (related to this window)
//
// MDM_DRAGDROP(PMYDRAGITEM, MPARAM xy) - is sent to the pointed window when the dropping occured.
//      The window returns TRUE when the dropping is success.
//      PMYDRAGITEM  pmdi
//      MPARAM       xy    - pointer coordinates (related to this window)
//
// MDM_DRAGEND (BOOL fOk, HWND hTarg) - sends to the hSource window when the drag ends.
//      BOOL         fOk   - success state (returned value of the MDM_DRAGDROP message)
//      HWND         hTarg - window the MDM_DRAGDROP message was sent to (or 0)
//
BOOL    StartDrag          (HWND hwndSource, PMYDRAGITEM pmdi, HPOINTER hptrDrop, HPOINTER hptrNoDrop);



/*=====================================================================================*/
/*                                                                                     */
/*        ------------->>> F i l e   F i n d  <<<------------                          */
/*                                                                                     */
/*=====================================================================================*/

MRESULT EXPENTRY dpFileFindDlg (HWND,ULONG,MPARAM,MPARAM);


#endif /* ifndef __FMAN__ */

