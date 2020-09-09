/*
*
*  File: bars.h (Br)
*
*  XDS Shell toolbar and status line management
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#ifndef __BARS__
#define __BARS__


#define STM_DISPLAYTEXT    (WM_USER+1001)
#define STM_SETCOLORS      (WM_USER+1002)


#define  STMODE_CHANGED    1
#define  STMODE_READONLY   2

struct   STATUSINFO        // Status bar: current position info
{
  LONG    lLine;           // If lLine < 0 or lCol < 0
  LONG    lCol;            //   the values will be hidden
  ULONG   stmode;          // STMODE_* constant
  STATUSINFO () { lLine = -1;}
};
typedef  STATUSINFO       *PSTATUSINFO;

extern  PTOOLBAR           pToolbar; // TOOLBAR calss instance
#define X_TBBUTTON_SIZE    24
#define Y_TBBUTTON_SIZE    24
#define Y_STATUSBAR_SIZE   18

void    BrForceUpdate      ();
PSZ     BrCreate           ();                   // returns error message or 0
void    BrDisplayComment   (PSZ pszComment, LONG lSec);
void    BrDrawNiceBorder   (HPS hps, LONG x, LONG y, LONG cx, LONG cy, ULONG clrLight, ULONG clrDark);

#endif /* ifndef __BARS__ */

