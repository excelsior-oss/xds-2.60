(**************************************************************************
 *  File name  :  main.mod
 *
 *  Description:  This module contains the main application window
 *                procedure.
 *
 *                This source file contains the following procedures:
 *
 *                MainWndProc(hwnd, msg, mp1, mp2)
 *                MainCommand(mp1, mp2)
 *
 *  Concepts   :  Presentation Manager
 *
 *  API's      :  WinPostMsg
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
IMPLEMENTATION MODULE main;

IMPORT SYSTEM;

IMPORT OS2, OS2RTL;

IMPORT edit, file, help, helpres, init, mainres, pnt, user, xtrn;

PROCEDURE MainCommand(mp1, mp2: OS2.MPARAM); FORWARD;

(**************************************************************************
 *
 *  Name       : MainWndProc(hwnd, msg, mp1, mp2)
 *
 *  Description:  Processes the messages sent to the main client
 *                window.  This routine processes the basic
 *                messages all client windows should process
 *                and passes all others onto UserWndProc where
 *                the developer can process any others.
 *
 *  Concepts:     Called for each message placed in the main
 *                window's message queue
 *
 *                A switch statement branches to the routines to be
 *                performed for each message processed.  Any messages
 *                not specifically process are passed to the user's
 *                message processing procedure UserWndProc().
 *
 *  API's      :  WinPostMsg
 *
 *  Parameters :  hwnd = window handle
 *                msg  = message i.d.
 *                mp1  = first message parameter
 *                mp2  = second message parameter
 *
 *  Return     :  value is determined by each message
 *
 *************************************************************************)
PROCEDURE [OS2.EXPENTRY] MainWndProc(hwnd : OS2.HWND
                                    ;msg  : OS2.ULONG
                                    ;mp1  : OS2.MPARAM
                                    ;mp2  : OS2.MPARAM
                                    ): OS2.MRESULT;
BEGIN
   CASE msg OF
   | OS2.WM_CREATE:
       RETURN init.InitMainWindow(hwnd, mp1, mp2)
   | OS2.WM_PAINT:
       pnt.MainPaint(hwnd);
   | OS2.WM_COMMAND:
       MainCommand(mp1, mp2);
   | OS2.WM_INITMENU:
       user.InitMenu(mp1, mp2);
   | OS2.HM_QUERY_KEYS_HELP:
       RETURN OS2.MRFROMULONG(helpres.PANEL_HELPKEYS);   (* return id of key help panel *)
   | xtrn.TM_THREADINITFAILED:
       (*  The message is received if the background thread
        *  initialization fails.
        *  A message box is displayed and the application is
        *  terminated.
        *)
       xtrn.MessageBox(OS2.HWND_DESKTOP
                      ,mainres.IDMSG_INITFAILED
                      ,OS2.MB_OK+OS2.MB_ERROR
                      ,TRUE) ;
       OS2.WinPostMsg(hwnd, OS2.WM_CLOSE, NIL, NIL);
   ELSE
   (*
    *      Any messages not processed are passed on
    *      to the user's window proc.  It is
    *      responsible for passing any messages it
    *      doesn't handle onto WinDefWindowProc()
    *)
      RETURN user.UserWndProc(hwnd, msg, mp1, mp2);
   END;
   RETURN NIL;  (* all window procedures return 0 as a default *)
END MainWndProc;


(**************************************************************************
 *
 *  Name       : MainCommand(mp1, mp2)
 *
 *  Description: Calls the appropriate procedures that deal with
 *               the selected menu item.
 *
 *  Concepts:    Routine is called whenever a WM_COMMAND message
 *               is posted to the main window.
 *
 *               A switch statement branches on the id of the
 *               menu item that posted the message and the
 *               appropriate action for that item is taken.  Any
 *               menu ids that are not part of the standard menu
 *               set are passed onto the user defined WM_COMMAND
 *               processing procedure.
 *
 *  API's      : [none]
 *
 *  Parameters : mp1 = first message parameter
 *               mp2 = second message parameter
 *
 *  Return     : [none]
 *
 *************************************************************************)
PROCEDURE MainCommand(mp1, mp2: OS2.MPARAM);
BEGIN
   CASE OS2.USHORT1FROMMP(mp1) OF
   | mainres.IDM_FILENEW:
      file.FileNew(mp2);
   | mainres.IDM_FILEOPEN:
      file.FileOpen(mp2);
   | mainres.IDM_FILESAVE:
      file.FileSave(mp2);
   | mainres.IDM_FILESAVEAS:
      file.FileSaveAs(mp2);
   | mainres.IDM_EDITUNDO:
      edit.EditUndo(mp2);
   | mainres.IDM_EDITCUT:
      edit.EditCut(mp2);
   | mainres.IDM_EDITCOPY:
      edit.EditCopy(mp2);
   | mainres.IDM_EDITPASTE:
      edit.EditPaste(mp2);
   | mainres.IDM_EDITCLEAR:
      edit.EditClear(mp2);
   | mainres.IDM_HELPINDEX:
      help.HelpIndex;
   | mainres.IDM_HELPGENERAL:
      help.HelpGeneral;
   | mainres.IDM_HELPUSINGHELP:
      help.HelpUsingHelp;
   | mainres.IDM_HELPKEYS:
      help.HelpKeys;
   | mainres.IDM_HELPTUTORIAL:
      help.HelpTutorial;
   | mainres.IDM_HELPPRODUCTINFO:
      help.HelpProductInfo;
   (*
    *      User command processing routine is called
    *      here so any ids not processed here can be
    *      processed.
    *)
   ELSE
      user.UserCommand(mp1, mp2);
   END;
END MainCommand;

END main.
