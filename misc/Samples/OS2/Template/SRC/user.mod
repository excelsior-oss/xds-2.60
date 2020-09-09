(**************************************************************************
 *  File name  :  user.mod
 *
 *  Description:  This module contains the code for processing messages
 *                sent to the standard window that the standard window does
 *                not process.  The application developer need only modify
 *                this file in order to implement new menu items or process
 *                messages not handled by the standard message routine.
 *
 *                This module contains the following procedures:
 *
 *                UserWndProc(hwnd, msg, mp1, mp2)
 *                UserCommand(mp1, mp2)
 *                InitMenu(mp1, mp2);
 *                EnableMenuItem(hwndMenu, idItem, fEnable);
 *
 *  Concepts   :  message processing
 *
 *  API's      :  WinDefWindowProc
 *                WinSendMsg
 *
 *  Copyright (C) 1996 xTech Ltd. All rights reserved.
 *  Copyright (C) 1991 IBM Corporation
 *
 *      DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
 *      sample code created by IBM Corporation. This sample code is not
 *      part of any standard or IBM product and is provided to you solely
 *      for  the purpose of assisting you in the development of your
 *      applications.  The code is provided "AS IS", without
 *      warranty of any kind.  IBM shall not be liable for any damages
 *      arising out of your use of the sample code, even if they have been
 *      advised of the possibility of such damages.
 *************************************************************************)

IMPLEMENTATION MODULE user;

IMPORT SYSTEM;

IMPORT OS2, OS2RTL;

IMPORT main, mainres, xtrn;

(**************************************************************************
 *
 *  Name       : UserWndProc(hwnd, msg, mp1, mp2)
 *
 *  Description: Process any messages sent to hwndMain that
 *               are not processed by the standard window procedure.
 *
 *  Concepts:    Routine is called for each message MainWndProc
 *               does not process.
 *
 *               A switch statement branches control based upon
 *               the message passed.  Any messages not processed
 *               here must be passed onto WinDefWindowProc().
 *
 *  API's      : WinDefWindowProc
 *
 *  Parameters :  hwnd = window handle
 *                msg  = message i.d.
 *                mp1  = first message parameter
 *                mp2  = second message parameter
 *
 *  Return     :  Return value dependent upon the message processed
 *
 *************************************************************************)
PROCEDURE UserWndProc(hwnd     : OS2.HWND
                     ;msg      : OS2.ULONG
                     ;mp1, mp2 : OS2.MPARAM): OS2.MRESULT;
BEGIN
   CASE msg OF
   (*
    *  Add case statements for message ids you wish to process.
    *)

   ELSE    (* default must call WinDefWindowProc() *)
      RETURN OS2.WinDefWindowProc(hwnd, msg, mp1, mp2);
   END;
   RETURN NIL;
END UserWndProc;

(**************************************************************************
 *
 *  Name       : UserCommand(mp1, mp2)
 *
 *  Description: Process any WM_COMMAND messages sent to hwndMain
 *               that are not processed by MainCommand.
 *
 *  Concepts:    Routine is called for each WM_COMMAND that is
 *               not posted by a standard menu item.
 *
 *               A switch statement branches control based upon
 *               the id of the control which posted the message.
 *
 *  API's      :  [none]
 *
 *  Parameters :  mp1  = first message parameter
 *                mp2  = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE UserCommand(mp1, mp2: OS2.MPARAM);
BEGIN
   CASE OS2.SHORT1FROMMP(mp1) OF
   (*
    *  Add case statements for menuitem ids you wish to process.
    *)
   ELSE
   END;
END UserCommand;

(**************************************************************************
 *
 *  Name       : InitMenu()
 *
 *  Description: Processes the WM_INITMENU message for the main window,
 *               disabling any menus that are not active.
 *
 *  Concepts:    Routine is called each time a menu is dropped.
 *
 *               A switch statement branches control based upon
 *               the id of the menu that is being displayed.
 *
 *  API's      :  [none]
 *
 *  Parameters :  mp1  = first message parameter
 *                mp2  = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE InitMenu(mp1, mp2: OS2.MPARAM);
VAR
   hwndMenu : OS2.HWND;
BEGIN
   hwndMenu := OS2.HWNDFROMMP(mp2);

   CASE OS2.USHORT1FROMMP(mp1) OF
   |mainres.IDM_FILE:
   |mainres.IDM_HELP:
            (*
             * Enable or disable the Help menu depending upon whether the
             * help manager has been enabled
             *)
        EnableMenuItem(hwndMenu, mainres.IDM_HELPUSINGHELP, xtrn.fHelpEnabled);
        EnableMenuItem(hwndMenu, mainres.IDM_HELPGENERAL, xtrn.fHelpEnabled);
        EnableMenuItem(hwndMenu, mainres.IDM_HELPKEYS, xtrn.fHelpEnabled);
        EnableMenuItem(hwndMenu, mainres.IDM_HELPINDEX, xtrn.fHelpEnabled);

         (*  REMEMBER: add a case for IDM_HELPTUTORIAL if you include
          *  the Tutorial menu item.
          *)
    ELSE
    END
END InitMenu;

(**************************************************************************
 *
 *  Name       : EnableMenuItem(hwndMenu, idItem, fEnable)
 *
 *  Description: Enables or disables the menu item
 *
 *  Concepts:    Called whenever a menu item is to be enabled or
 *               disabled
 *
 *               Sends a MM_SETITEMATTR to the menu with the
 *               given item id.  Sets the MIA_DISABLED attribute
 *               flag if the item is to be disabled, clears the flag
 *               if enabling.
 *
 *  API's      : WinSendMsg
 *
 *  Parameters :  hwndmenu = menu window handle
 *                idItem   = menu item i.d.
 *                fEnable  = enable (yes) or disable (no)
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE EnableMenuItem(hwndMenu : OS2.HWND
                        ;idItem   : OS2.USHORT
                        ;fEnable  : OS2.BOOL);
VAR
   fsFlag : OS2.SHORT;

BEGIN
   IF fEnable THEN
      fsFlag := 0;
   ELSE
      fsFlag := OS2.MIA_DISABLED;
   END;

   OS2.WinSendMsg(hwndMenu,
                  OS2.MM_SETITEMATTR,
                  OS2.MPFROM2SHORT(idItem, 1),
                  OS2.MPFROM2SHORT(OS2.MIA_DISABLED, fsFlag));
END EnableMenuItem;

END user.

