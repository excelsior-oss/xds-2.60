(**************************************************************************
 *  File name  :  dlg.mod
 *
 *  Description:  This module contains the Dialog Procedures for the user
 *                defined dialogs as well as any support code they need.
 *
 *                This source file contains the following procedures:
 *
 *                ProductInfoDlgProc(hwnd, msg, mp1, mp2)
 *                SetSysMenu(hDlg)
 *
 *  Concepts   :  dialog box
 *
 *  API's      :  WinDismissDlg
 *                WinDefDlgProc
 *                WinWindowFromID
 *                WinSendMsg
 *
 *  Import:    :  OS2
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

IMPLEMENTATION MODULE dlg;

IMPORT SYSTEM;

IMPORT OS2, OS2RTL;

(**************************************************************************
 *
 *  Name       : ProductInfoDlgProc(hwnd, msg, mp1, mp2)
 *
 *  Description: Processes all messages sent to the Product information
 *               dialog
 *
 *  Concepts:  The Product information dialog has only a button control,
 *             so this routine processes only WM_COMMAND messages.  Any
 *             WM_COMMAND posted must have come from the OK
 *             button, so we dismiss the dialog upon receiving it.
 *
 *  API's      :  WinDismissDlg
 *                WinDefDlgProc
 *
 *  Parameters :  hwnd     = window handle
 *                msg      = message i.d.
 *                mp1      = first message parameter
 *                mp2      = second message parameter
 *
 *  Return     :  dependent on message sent
 *
 *************************************************************************)
PROCEDURE [OS2.EXPENTRY] ProductInfoDlgProc
                         ( hwnd : OS2.HWND       (* handle of window *)
                         ; msg  : OS2.ULONG      (* id of message *)
                         ; mp1  : OS2.MPARAM     (* first message parameter *)
                         ; mp2  : OS2.MPARAM     (* second message parameter *)
                         ) : OS2.MRESULT;
BEGIN
   CASE msg OF
   |OS2.WM_INITDLG:
      SetSysMenu(hwnd);      (* mdoify system menu for this dialog  *)
      RETURN OS2.MRFROMULONG(0);  (* i.e. FALSE *)
   |OS2.WM_COMMAND:
        (* no matter what the command, close the dialog *)
      OS2.WinDismissDlg(hwnd, 1 (*TRUE*));
   ELSE
      RETURN OS2.WinDefDlgProc(hwnd, msg, mp1, mp2);
   END;
   RETURN NIL;
END ProductInfoDlgProc;


(**************************************************************************
 *
 *  Name       : SetSysMenu(hDlg)
 *
 *  Description: Sets only the Move and Close items of the system menu
 *
 *  Concepts:  Any dialog box is free to call this routine, to edit
 *             which menu items will appear on its System Menu pulldown.
 *
 *  API's      :  WinWindowFromID
 *                WinSendMsg
 *
 *  Parameters :  hDlg     = window handle of the dialog
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE SetSysMenu(hDlg : OS2.HWND);
VAR
    hSysMenu : OS2.HWND;
    Mi       : OS2.MENUITEM;
    Pos      : OS2.ULONG;
    Id       : OS2.MRESULT;
    cItems   : OS2.SHORT;
BEGIN
    (******************************************************************)
    (*  We only want Move and Close in the system menu.               *)
    (******************************************************************)

    hSysMenu := OS2.WinWindowFromID(hDlg, OS2.FID_SYSMENU);
    OS2.WinSendMsg(hSysMenu
                  ,OS2.MM_QUERYITEM
                  ,OS2.MPFROM2USHORT(OS2.SC_SYSMENU, 0 (*FALSE*))
                  ,SYSTEM.ADR(Mi));
    Pos := 0;
    cItems := OS2.SHORT1FROMMR(OS2.WinSendMsg(Mi.hwndSubMenu
                                             ,OS2.MM_QUERYITEMCOUNT
                                             ,NIL
                                             ,NIL));
    WHILE cItems > 0 DO
        DEC(cItems);
        Id := OS2.WinSendMsg(Mi.hwndSubMenu
                           ,OS2.MM_ITEMIDFROMPOSITION
                           ,OS2.MPFROMLONG(Pos)
                           ,NIL);
        CASE OS2.USHORT1FROMMR(Id) OF
        |OS2.SC_MOVE:
        |OS2.SC_CLOSE:
            INC(Pos);  (* Don't delete that one. *)
        ELSE
            OS2.WinSendMsg(Mi.hwndSubMenu
                          ,OS2.MM_DELETEITEM
                          ,OS2.MPFROM2SHORT(OS2.SHORT1FROMMR(Id), 1 (*TRUE*))
                          ,NIL);
        END;
    END;
END SetSysMenu;

END dlg.
