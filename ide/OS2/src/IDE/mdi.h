/*
*
*  File: MDI.h
*
*  Multy Document Interface definition file
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#ifndef __MDI__
#define __MDI__

// Our resources:
#define IDD_MDILISTDLG 9600
#define IDD_MDILB      9601
#define IDD_MDIPBHELP  9602

// NOTE(s):
//
//  Command values IDM_MDIFIRST(=32000)..IDM_MDILAST must be reserved for MDI
//
//
//          In the following text the next window names are used:
//
//  MDIFrame    - the main MDI frame, its parent HWND_DESKTOP, no owner
//  MDI0Client  - all MDIFrame's client area, covered by 'Bars' and 'MDIDesk', its parent is MDIFrame
//  MDIDesk     - MDI0Client's part not covered by the 'Bars', contains 'Client' windows
//  Bar         - rectangular window sticked to the one side of the 'MDI0Client'
//  Client (Frame) - client window. Must be frame window.
//
//
//  Windows creating/destroying:
//     Windows must be created by the class methods using,
//     to destroy them use the usual API methods.
//     To destroy the MDI frame (and all the clients) the class implementation must
//     be destroyed (usual API methods are allowed while the application termination only)
//
//  Ownership (descendant -> its owner -> ...):
//     Client Frame              -> MDI0Client  -> MDIFrame  -> 0;
//     Client frame's FID_CLIENT -> [this client's frame or user defined window];
//     Bar window                -> [user defined window, usualy MDI0Client used];
//
//  Message dispatching:
//     All the most useful messages walks througth the MDI0Client's window procedure,
//  so we have a good point to process them. SetMsgHook method used to set the procedure
//  instead WinDefWindowProc call at the MDI0Client's window function. It is a good
//  idea to set MDI0Client as a owner or all the controls in the toolbars and so on
//  and to catch there its notification messages.
//     WM_SYSCOMMAND with SC_CLOSE action client's frame sents to its FID_CLIENT window
//  to prevent frame's default reaction - posting WM_QUIT message to the application queue.
//
//
//  Hidden clients:
//     The window menu shows thus items in disabled state.
//
//  Known bugs:
//     If the client processes the WM_OWNERPOSCHANGE message when its frame is
//     maximized, minimized or restored and tryes to change the frame state, it
//     may cause errors (in the final frame sizes). The better way for the client
//     is to use a functions like WinSetWindowPos.
//


/*>>> Advanced WM_CONTROL:
>
> If the MDIFrame widow is minimized or restored (maximized) from the minimized state
> it generates this message and sends it to all the its descendants, informing them
> that they haven't to process any user input or drawing operations untill the MDI frame
> will be restored back.
>
> param1
>      USHORT  Reserved, 0
>      USHORT  usNotifyCode = MDIN_MINIMIZE  --  Notification code
>
> param2
>      BOOL   fMinimize                      --  TRUE  - MDIFrame is minimized
>                                                FALSE - MDIFrame is restored
> returns
>      ULONG   ulReserved                    --  Reserved value, should be 0.
>
*/
#define MDIN_MINIMIZE   1100


/*>>>   M e n u   c o m m a n d s   f o r   M D I
>
>  Resource definitions may contains window menu with the following IDM_WIN* items
*/


#define MDI_MAXFRAMES     90        // Up to MDI_MAXFRAMES clients can be created
#define IDM_MDIFIRST      32000

#define IDM_WINDOW        (IDM_MDIFIRST+0)
#define IDM_WINCASCADE    (IDM_MDIFIRST+1)
#define IDM_WINMAXALL     (IDM_MDIFIRST+3)
#define IDM_WINCLOSEALL   (IDM_MDIFIRST+4)
#define IDM_WINFIRST      (IDM_MDIFIRST+9)
//...                     // Up to MDI_MAXFRAMES items
#define IDM_WINLAST       (IDM_WINFIRST + MDI_MAXFRAMES - 1)
#define IDM_WINMORE       (IDM_WINLAST+1)


//-- MDI client system menu (this order!)
#define MDICL_CLOSE       (IDM_WINLAST+10) //
#define MDICL_RESTORE     (IDM_WINLAST+11) //
#define MDICL_MOVE        (IDM_WINLAST+12) // This
#define MDICL_SIZE        (IDM_WINLAST+13) //   order!
#define MDICL_MAXIMIZE    (IDM_WINLAST+14) //
#define MDICL_HIDE        (IDM_WINLAST+15) //
#define MDICL_SYSMENU     (IDM_WINLAST+16) //

// 2400..2499 - our reserve:
#define IDM_POPUP_WINBAR    2400
#define IDM_POP_WBBCLOSE    2401
#define IDM_POP_WBBRESTORE  2402
#define IDM_POP_WBBMAXIMIZE 2403
#define IDM_POP_WBBHIDE     2404
#define IDM_POP_WBBVTILE    2405
#define IDM_POP_WBBHTILE    2406
#define IDM_POP_WBFONT      2407
#define IDM_POP_WBCOLORS    2408
#define IDM_POP_WBWARP      2409
// Примочки к MDI... :(
#define IDM_POP_MVGROUPS    2489
#define IDM_POP_MVAGROUP    2490
#define IDM_POP_MV1GROUP    2491
#define IDM_POP_MV2GROUP    2492
#define IDM_POP_MV3GROUP    2493
#define IDM_POP_MV4GROUP    2494
#define IDM_POP_MV5GROUP    2495
#define IDM_POP_MV6GROUP    2496
#define IDM_POP_MV7GROUP    2497
#define IDM_POP_MV8GROUP    2498
#define IDM_POP_MV9GROUP    2499


enum MDIBIND
{
 MDIBIND_LEFTTOP,       //
 MDIBIND_RIGHTTOP,      //   Client (frame) window is sticked to the
 MDIBIND_LEFTBOTTOM,    //     corresponded corner of the MDIDesk
 MDIBIND_RIGHTBOTTOM,   //

 MDIBIND_LEFTBAR,       // (always MDIBIND_LEFTTOP    in the MDI0Client)
 MDIBIND_RIGHTBAR,      // (always MDIBIND_RIGHTTOP   in the MDI0Client)   Bars are excluded from the
 MDIBIND_TOPBAR,        // (always MDIBIND_LEFTTOP    in the MDI0Client)     MDIDesk (only the width coordinates used)
 MDIBIND_BOTTOMBAR      // (always MDIBIND_LEFTBOTTOM in the MDI0Client)
};

enum MDIQW
{
  MDIQW_UPPERFRAME,       // The upperest frame
//  MDIQW_LASTFRAME,        // Active frame or (if 0) frame was active last
  MDIQW_NEXTFRAME,        // Frame under the active (Z-order)
  MDIQW_BOTTOMFRAME,      // Frame at the bottom (Z-order)
  MDIQW_NEXTVISFRAME,     // Visible frame under the active (Z-order)
  MDIQW_BOTTOMVISFRAME,   // Visible frame at the bottom (Z-order)
  MDIQW_DESK,             // Desk window
  MDIQW_MAINFRAME         // The main frame window
};


class   MDICLPOS;         // MDI client position
typedef MDICLPOS *PMDICLPOS;
class   WINBAR;
typedef WINBAR   *PWINBAR;

#define WBV_MULTILINE    0x0001
#define WBV_HIDDEN       0x0002
#define WBV_SHOWCOLOR    0x0004

struct WBCONFIGDATA
{
  ULONG   ulWBV;
  char    szPPFont[FACESIZE+10];
};
typedef WBCONFIGDATA *PWBCONFIGDATA;


/*====== C L A S S   M D I F R A M E  ===============================================*/

class MDIFRAME
{
  USHORT     cb;
  public:


  /*>>> Create MDIFRAME instance
  >
  > PHWND    phMDI0Client   (in/out)  - created MDI0Client window handle or 0 when error;
  > HAB      hAB            (in)      - anchor block;
  > ULONG    ulClientIconId (in)      - default client icon identifyer
  > --- WinCreateStdWindow-like parameters:
  > ULONG    flStyle        (in)      - frame style
  > PULONG   pflCreateFlags (in)      - frame create flags
  > PSZ      pszTitle       (in)      - frame title
  > HMODULE  hmResource     (in)      - resource identifoer or 0 (resources for the main menu and icons means)
  > ULONG    ulId           (in)      - the identifier within the resource definition of the required resources
  > LONG     x0,x0,cx,cy    (in)      - the desktop-relative coordinates for the MDIFrame
  >
  */
  MDIFRAME (PHWND phMDI0Client, HAB hAB, ULONG ulClientIconId, ULONG flStyle, PULONG pflCreateFlags,
            PSZ pszTitle, HMODULE hmResource, ULONG ulId, LONG x0, LONG y0, LONG cx, LONG cy);

 ~MDIFRAME ();



  /*>>> Create child window (frame or bar)
  >
  > MDIBIND  usBind         (in) - window position binding type (defines the bar or client types)
  > PSZ      pszClass       (in) - bar: class,       frame: its FID_CLIENT's class
  > PSZ      szName         (in) -                   frame: caption
  > ULONG    flStyle        (in) - bar: style,       frame: its FID_CLIENT's style
  > ULONG    flCreateFlags  (in) -                   frame: FCF_* flags
  > LONG     x0,y0,cx,cy    (in) - bar: only the appropriate width used,
  >                                                  frame: frame initial sizes (relative to MDIDesk window)
  > HWND     hwndOwner      (in) - bar: owner,       frame: its FID_CLIENT's owner (0 - set it's frame as the owner)
  > PVOID    pCtlData       (in) - Bar: pCtlData,    frame: its FID_CLIENT's pCtlData
  > PVOID    pPresParam     (in) - Bar: pPresParams, frame: its FID_CLIENT's pPresParams
  > returns                      - handle of the window created (for frame - its frame window mean)
  >
  > Note: frame windows are created in unvisible state.
  >
  */
  HWND CreateChild (MDIBIND usBind, PSZ pszClass, PSZ szName, ULONG flStyle, ULONG flCreateFlags, LONG x0, LONG y0,
                    LONG cx, LONG cy, HWND hwndOwner, PVOID pCtlData, PVOID pPresParams );


  /*>>> Set messahe hook: this hook will be called instead WinDefWindowProc from the h0Client's window procedure
  */
  void SetMsgHook (PFNWP pfnwpHook);


  BOOL SetBarWidth       (HWND hBar, LONG lNewWidth);   // Returns success

  HWND QueryWindow       (MDIQW mdiqw);                 // MDIQW_* values

  BOOL SetChildHandle    (HWND hChild, ULONG ulHandle); // Set the child (frame of bar) handle. Returns success.

  ULONG QueryChildHandle (HWND hChild);                 // Returns the handle was set or 0



  /*>>> Enumerate client frames in Z-order (from top to bottom):
  >
  >    PHWND apWnd (in/out) - pointer to array where n handles will be stored
  >                           0 - return total number of the hFrames
  >    int n       (in)     - size of the array pointeb by the apWnd (in elements)
  >    returns              - number of the elements placed of total number of hFrames when ahWns = 0;
  >
  */
  int EnumFrames (PHWND ahWnd, int n);



  /*>>> 'Window' menu control
  >
  >   HWND hFrame  (in)     - farme to assign menu item
  >   PSZ  pszItem (in)     - the item text           (0 - don't place item)
  >   int  nItem   (in)     - item number in the menu
  >   int  nAccel  (it)     - accelerated command     (0 - not used)
  >   returns               - success indicator       (f.ex. FALSE if SetMenuMode was fAuto of hFrame is wrong)
  >
  */
  /*not tested*/ BOOL SetMenuItem (HWND hFrame, PSZ pszItem, int nItem, int nAccel);


  /*>>> Set 'Window' menu control mode
  >
  >   BOOL  fAuto (in)      - TRUE - set auto mode (defined);  FALSE - use SetMenuItem function
  */
  /*not tested*/ void SetMenuMode (BOOL fAuto) {fAutoMenu = fAuto;}

  /*not tested*/ BOOL QueryFramePos (HWND hFrame, PMDICLPOS pcp);  // Set MDICLPOS, returns success
  /*not tested*/ BOOL SetFramePos   (HWND hFrame, PMDICLPOS pcp);  // Set frame position, returns success

  /*--- WinBar control */
  void   WBarSetText     (HWND hFrame, PSZ szText);
  void   WBarSetColors   (HWND hFrame, ULONG rgbFore, ULONG rgbBack);
  void   WBarQueryConfig (PWBCONFIGDATA pWBCfg);
  void   WBarSetConfig   (PWBCONFIGDATA pWBCfg);
  void   WBarSetOrder    (PHWND aFrames, int nFrames);

  static HAB     hAB;

  /*----P-R-I-V-A-T-E----A-R-E-A----*/


  private:
  struct CLIENTLIST
  {
    CLIENTLIST    *next;         // next CLIENTLIST
    HWND           hWnd;         // window handle
    MDIBIND        usBind;       // MDIBIND_*
    LONG           x0,y0,cx,cy;  // window sizes (frame: restored state, bar: width only)
    ULONG          ulHandle;     // user's defined handle
    BOOL           fMaxim;       // frame: maximized
    BOOL           fMinim;       // frame: minimized
    char          *pszItem;      // frame: malloc()ed menuitem text (or 0)
    int            nItem;        // frame: number of the item in the menu
    int            nAccel;       // frame: accelerated command (or 0)
    PFNWP          pwpOriginal;  // bar: the original window procedure
     CLIENTLIST();
  };
  typedef CLIENTLIST *PCLIENTLIST;

  //--- Window management:

  HWND             hMDIFrame;
  HWND             h0Client;
  HWND             hDesk;
  HWND             hWBOwner;
  PWINBAR          pWinBar;
  PFNWP            pfnwpMsgHook;
  BOOL             fMainMinimized;
  BOOL             fAutoMenu;
  HMODULE          hmResource;
  HPOINTER         hptrClientIcon;
  ULONG            ulClientIconId;
  PCLIENTLIST      pclClients;
  PCLIENTLIST      pclBars;
  PCLIENTLIST      hFr2pcl            (HWND);
  PCLIENTLIST      hBar2pcl           (HWND);
  void             fill_menu          ();

  ULONG            rgbDesk;

  static BOOL      fCalledBefore;

  MRESULT          wpMDIFrame         (HWND,ULONG,MPARAM,MPARAM);
  MRESULT          wp0Client          (HWND,ULONG,MPARAM,MPARAM);
  MRESULT          wpDesk             (HWND,ULONG,MPARAM,MPARAM);
  void             ArrangeChildren    ();
  friend struct    CLIENTINFO;
  friend MRESULT EXPENTRY fwpMDIFrame (HWND,ULONG,MPARAM,MPARAM);
  friend MRESULT EXPENTRY fwp0Client  (HWND,ULONG,MPARAM,MPARAM);
  friend MRESULT EXPENTRY fwpDesk     (HWND,ULONG,MPARAM,MPARAM);
  friend MRESULT EXPENTRY fwpWBOwner  (HWND,ULONG,MPARAM,MPARAM);
  friend MRESULT EXPENTRY fwpBars     (HWND,ULONG,MPARAM,MPARAM);
  friend MRESULT EXPENTRY fwpFrames   (HWND,ULONG,MPARAM,MPARAM);
  friend MRESULT EXPENTRY dpMDIListDlg(HWND,ULONG,MPARAM,MPARAM);
};
typedef MDIFRAME *PMDIFRAME;


class MDICLPOS            // MDI client position
{
  USHORT  Reserved0;      // reserved, must be 0
  ULONG   x0,y0,cx,cy;    // restored sizes
  BOOL    fMaxim;         // maximized state
  BOOL    fMinim;         // minimized state
  BOOL    fVisible;       // visible state
  MDIBIND usBind;         // binding method (for frame only)
  ULONG   cxDesk,cyDesk;  // hDesk size
  friend  class MDIFRAME;
};


#endif /* ifndef __MDI__ */

