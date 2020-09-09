/*
*
*  File: TOOLBAR.h
*
*  Multy Document Interface definition file
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

// !!!!!!!!!!! Capture: suxxx!

#ifndef __TOOLBAR__
#define __TOOLBAR__

/*>>> Advanced WM_CONTROL:
>
> If the button control has the text associated (see NEWBTN, pszComment) it
> generates this message and sends it to it's owner, informing the owner
> that the button is pointed (or losing the pointing) by the mouse.
> Note: It works only when the button is its application main winows's
>       descendant.  :(
>
> param1
>      USHORT  id                            --  Button control identity.
>      USHORT  usNotifyCode = TLBN_COMMENT   --  Notification code
>
> param2
>      PSZ     pszComment                    --  Pointer to the text associated with the button
>                                                or 0 when the button is losing the pointing
> returns
>      ULONG   ulReserved                    --  Reserved value, should be 0.
>
*/
#define TLBN_COMMENT   1000



/*====== C L A S S   T O O L B A R ===============================================*/

class TOOLBAR
{
  public:
  struct NEWBTN
  {
    HWND   hWnd;       // Handle of the button
    HWND   hOwner;     // button's owner
    ULONG  ulCmd;      // Command value associated with the button
    ULONG  ulBmp;      // Button bitmap resource id or 0 for separator
    ULONG  rgbBack;    // Button's background color (0x00000001 - use default)
    BOOL   fEnabled;   // Button enabled state
    PSZ    pszComment; // Comment line (See WM_CONTROL notification)
  };
  typedef NEWBTN *PNEWBTN;


  enum TBALIGNMENT
  {
    TBA_HORIZONTAL,  // Single line alignment: separators occupied 1/2 of the buttons x-size
    TBA_VERTICAL,    // Single column alignment: separators occupied 1/2 of the buttons y-size
    TBA_FILL,        // Fill all the window area (line by line), separators may work not correctly
    TBA_CENTER       // Single buttonplace in the center
  };


  /*>>> Create TOOLBAR instance
  >   PHWND   pRet(in/out)    - success indicator will be stored
  >   HAB     hAB (in)        - anchor block
  >   HWND    hParent (in)    - window to become the toolbar's parent (all the hParent size will be occupied)
  >   HWND    hOwner (in)     - window to become the toolbar's owner
  >   ULONG   dx (in)         - x-size of the buttons
  >   ULONG   dy (in)         - y-size of the buttons
  >   HMODULE hmResource (in) - resource identity containg the bitmaps (or 0 to use application .exe file)
  */
  TOOLBAR (PBOOL pRet, HAB hAB, HWND hParent, HWND hOwner, ULONG dx, ULONG dy, HMODULE hmResource);

  ~TOOLBAR ();



  /*>>> Insert buttons
  >
  >  PNEWBTN pNewBtn (in/out)    - Array of NEWBTN structures
  >                                hWnd fields will be set for the successfully created buttons,
  >                                other fields must be set
  >  USHORT  usCount (in)        - Number of elements in the array (0 or more)
  >  ULONG   ulInsertBefore (in) - Command value associated with the button to insert new buttons before
  >                                or 0 to append them at the end
  >
  >  Returns: TRUE if all the buttons was successfully created
  */
  BOOL InsertMultButton (PNEWBTN aNewBtn, USHORT usCount, ULONG ulInsertBefore);



  /*>>> Remove array of buttons
  >
  >  PULONG aulCmd (in) - array of the command values associated with the buttons or the separators to be removed
  >  USHORT usCount(in) - number of elements in the array
  >
  >  NOTE: button CAN'T be removed by destroying it's window
  >        (using the handle returned by AddButton call)
  */
  void RemoveMultButton (PULONG aulCmd, USHORT usCount);



  /*>>> Set buttons alignment
  >
  >  TBALIGNMENT tba (in) - TBA_* value
  >
  */
  void SetAlignment (TBALIGNMENT tba)   {tbAlignment = tba;AlignAll();}



  /*>>> Enable toolbar's button
  >
  >  ULONG ulCmd   (in) - button identifyer
  >  BOOL  fEnable (in) - new enable state
  >
  */
  void EnableButton(ULONG ulCmd, BOOL fEnable);


  /*>>> Set new comment to button
  >
  >  ULONG ulCmd      (in) - button identifyer
  >  PSZ   pszComment (in) - the new comment to set
  >
  */
  void SetButtonComment(ULONG ulCmd, PSZ pszComment);



  /*>>> Adjust toolbar window size
  >
  >  It have to be called when the toolbar parent window size is changed
  >
  */
  void AdjustTBSize();



  /*>>> Query the optimal toolbar window sizes to place all the controls
  >
  >  PRECTL prcl (in/out) - rectangle with the required sizes (xLeft and yBottom will be 0)
  >  (Does not working with TBA_CENTER alignment)
  >
  */
  void QueryRequiredRect (PRECTL prcl);



  /*----P-R-I-V-A-T-E----A-R-E-A----*/

  private:

  struct BTN
  {
    BTN   *next;
    HWND   hWnd;
    ULONG  ulCmd;
    ULONG  ulBmp;
    PSZ    pszComment;
    LONG   x, y;
    LONG   cx,cy;
    BTN();
   ~BTN();
  };
  typedef BTN *PBTN;

  HAB          hAB;
  PBTN         pBtn;
  HWND         hTB;
  HWND         hParent;
  HWND         hOwner;
  ULONG        dx,dy;
  LONG         xBorder,   yBorder;   // hTB window border areas widths
  LONG         xRequired, yRequired; // Optimal hTB sizes (for TBA_HORIZONTAL or TBA_VERTICAL)
  TBALIGNMENT  tbAlignment;
  HMODULE      hmResource;

  void     AlignAll       ();
  PBTN     Cmd2pbt        (ULONG ulCmd);

  static BOOL fInitialized;
  friend MRESULT EXPENTRY fwpTBWindow(HWND,ULONG,MPARAM,MPARAM);
  friend MRESULT EXPENTRY fwpButton  (HWND,ULONG,MPARAM,MPARAM);
};
typedef TOOLBAR *PTOOLBAR;

#endif /* ifndef __TOOLBAR__ */

