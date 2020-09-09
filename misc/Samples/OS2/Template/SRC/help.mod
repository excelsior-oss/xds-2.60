(**************************************************************************
 *  File name  :  help.mod
 *
 *  Description:  This module contains all the routines for interfacing
 *                the IPF Help Manager.
 *
 *                This source file contains the following functions:
 *
 *                InitHelp()
 *                HelpIndex()
 *                HelpGeneral()
 *                HelpUsingHelp()
 *                HelpKeys()
 *                HelpTutorial()
 *                HelpProductInfo()
 *                DisplayHelpPanel(idPanel)
 *                DestroyHelpInstance()
 *
 *  Concepts   :  help manager
 *
 *  API's      :  WinLoadString
 *                WinCreateHelpInstance
 *                WinAssociateHelpInstance
 *                WinSendMsg
 *                WinDlgBox
 *                WinDestroyHelpInstance
 *                WinMessageBox
 *                WinAlarm
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

(*
 *  Include files, macros, defined constants, and externs
 *)

IMPLEMENTATION MODULE help;

IMPORT SYSTEM;

IMPORT OS2, OS2RTL;

IMPORT dlg, dlgres, helpres, main, mainres, xtrn;

(* If DEBUG_HELP is declared, then the help panels will display their
 *  id values on their title bar.  This is useful for determining
 *  which help panels are being shown for each dialog item.  When
 *  the DEBUG_HELP option is not declared, then the panel ids are not
 *  displayed.
 *)

CONST HELPLIBRARYNAMELEN = 20;

(*
 *  Global variables
 *)

VAR
   hwndHelpInstance : OS2.HWND;
   szLibName : ARRAY [0..HELPLIBRARYNAMELEN-1] OF CHAR;
   szWindowTitle : ARRAY [0..HELPLIBRARYNAMELEN-1] OF CHAR;

(*
 *  Entry point declarations
 *)
--MRESULT EXPENTRY ProductInfoDlgProc(HWND hwnd,    !!!
--                                    ULONG msg,
--                                    MPARAM mp1,
--                                    MPARAM mp2);


(**************************************************************************
 *
 *  Name       : InitHelp()
 *
 *  Description:  Initializes the IPF help facility
 *
 *  Concepts:     Called once during initialization of the program
 *
 *                Initializes the HELPINIT structure and creates the
 *                help instance.  If successful, the help instance
 *                is associated with the main window.
 *
 *  API's      :  WinLoadString
 *                WinCreateHelpInstance
 *                WinAssociateHelpInstance
 *
 *  Parameters :  [none]
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE InitHelp;
VAR
   hini : OS2.HELPINIT;
BEGIN
   (* if we return because of an error, Help will be disabled *)
   xtrn.fHelpEnabled := FALSE;

   (* initialize help init structure *)
   hini.cb := SIZE(OS2.HELPINIT);
   hini.ulReturnCode := 0;

   hini.pszTutorialName := NIL;   (* if tutorial added, add name here *)

   hini.phtHelpTable := SYSTEM.CAST(OS2.PHELPTABLE,helpres.TEMPLATE_HELP_TABLE+0FFFF0000H);
   hini.hmodHelpTableModule := 0;
   hini.hmodAccelActionBarModule := 0;
   hini.idAccelTable := 0;
   hini.idActionBar := 0;

   IF OS2.WinLoadString(xtrn.hab
                       ,0
                       ,mainres.IDS_HELPWINDOWTITLE
                       ,HELPLIBRARYNAMELEN
                       ,szWindowTitle) = 0 THEN
      xtrn.MessageBox(xtrn.hwndMain
                     ,mainres.IDMSG_CANNOTLOADSTRING
                     ,OS2.MB_OK+OS2.MB_ERROR
                     ,FALSE);
      RETURN;
   END;

   hini.pszHelpWindowTitle := SYSTEM.ADR(szWindowTitle);

   (* if debugging, show panel ids, else don't *)
<* IF DEFINED(DEBUG_HELP) & DEBUG_HELP THEN *>
   hini.fShowPanelId := OS2.CMIC_SHOW_PANEL_ID;
<* ELSE *>
   hini.fShowPanelId := OS2.CMIC_HIDE_PANEL_ID;
<* END *>

   IF OS2.WinLoadString(xtrn.hab
                       ,0
                       ,mainres.IDS_HELPLIBRARYNAME
                       ,HELPLIBRARYNAMELEN
                       ,szLibName) = 0 THEN
      xtrn.MessageBox(xtrn.hwndMain
                     ,mainres.IDMSG_CANNOTLOADSTRING
                     ,OS2.MB_OK+OS2.MB_ERROR
                     ,FALSE);
      RETURN;
   END;

   hini.pszHelpLibraryName := SYSTEM.ADR(szLibName);

   (* creating help instance *)
   hwndHelpInstance := OS2.WinCreateHelpInstance(xtrn.hab, hini);

   IF (hwndHelpInstance = OS2.NULLHANDLE) OR (hini.ulReturnCode <> 0) THEN
      xtrn.MessageBox(xtrn.hwndMainFrame
                     ,mainres.IDMSG_HELPLOADERROR
                     ,OS2.MB_OK+OS2.MB_ERROR
                     ,TRUE);
      RETURN;
   END;

   (* associate help instance with main frame *)
   IF NOT OS2.WinAssociateHelpInstance(hwndHelpInstance, xtrn.hwndMainFrame) THEN
      xtrn.MessageBox(xtrn.hwndMainFrame
                     ,mainres.IDMSG_HELPLOADERROR
                     ,OS2.MB_OK+OS2.MB_ERROR
                     ,TRUE);
      RETURN;
   END;

   (* help manager is successfully initialized so set flag to TRUE *)
   xtrn.fHelpEnabled := TRUE;

END InitHelp;


(**************************************************************************
 *
 *  Name       : HelpGeneral()
 *
 *  Description: Processes the WM_COMMAND message posted by the
 *               General Help item of the Help menu.
 *
 *  Concepts:    Called from MainCommand when the General Help
 *               menu item is selected.
 *
 *               Sends an HM_EXT_HELP message to the help
 *               instance so that the default Extended Help is
 *               displayed.
 *
 *  API's      : WinSendMsg
 *
 *  Parameters :  [none]
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE HelpGeneral;
BEGIN
    (* this just displays the system extended help panel *)
   IF xtrn.fHelpEnabled THEN
      IF OS2.WinSendMsg(hwndHelpInstance
                       ,OS2.HM_EXT_HELP
                       ,NIL
                       ,NIL) <> NIL THEN
         xtrn.MessageBox(xtrn.hwndMain
                        ,mainres.IDMSG_HELPDISPLAYERROR
                        ,OS2.MB_OK+OS2.MB_ERROR
                        ,FALSE);
      END;
   END;
END HelpGeneral;


(**************************************************************************
 *
 *  Name       : HelpUsingHelp()
 *
 *  Description: Processes the WM_COMMAND message posted by the
 *               Using Help item of the Help menu.
 *
 *  Concepts:    Called from MainCommand when the Using Help
 *               menu item is selected.
 *
 *               Sends an HM_DISPLAY_HELP message to the help
 *               instance so that the default Using Help is
 *               displayed.
 *
 *  API's      : WinSendMsg
 *
 *  Parameters :  [none]
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE HelpUsingHelp;
BEGIN
   (* this just displays the system help for help panel *)
   IF xtrn.fHelpEnabled THEN
      IF OS2.WinSendMsg(hwndHelpInstance
                       ,OS2.HM_DISPLAY_HELP
                       ,NIL
                       ,NIL) <> NIL THEN
         xtrn.MessageBox(xtrn.hwndMain
                        ,mainres.IDMSG_HELPDISPLAYERROR
                        ,OS2.MB_OK+OS2.MB_ERROR
                        ,FALSE);
      END;
   END;
END HelpUsingHelp;


(**************************************************************************
 *
 *  Name       : HelpKeys()
 *
 *  Description: Processes the WM_COMMAND message posted by the
 *               Keys Help item of the Help menu.
 *
 *  Concepts:    Called from MainCommand when the Keys Help
 *               menu item is selected.
 *
 *               Sends an HM_KEYS_HELP message to the help
 *               instance so that the default Keys Help is
 *               displayed.
 *
 *  API's      : WinSendMsg
 *
 *  Parameters :  [none]
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE HelpKeys;
BEGIN
   (* this just displays the system keys help panel *)
   IF xtrn.fHelpEnabled THEN
      IF OS2.WinSendMsg(hwndHelpInstance
                       ,OS2.HM_KEYS_HELP
                       ,NIL
                       ,NIL) <> NIL THEN
         xtrn.MessageBox(xtrn.hwndMain
                        ,mainres.IDMSG_HELPDISPLAYERROR
                        ,OS2.MB_OK+OS2.MB_ERROR
                        ,FALSE);
      END;
   END;
END HelpKeys;


(**************************************************************************
 *
 *  Name       : HelpIndex()
 *
 *  Description: Processes the WM_COMMAND message posted by the
 *               Help Index item of the Help menu.
 *
 *  Concepts:    Called from MainCommand when the Help Index
 *               menu item is selected.
 *
 *               Sends an HM_INDEX_HELP message to the help
 *               instance so that the default Help Index is
 *               displayed.
 *
 *  API's      : WinSendMsg
 *
 *  Parameters :  [none]
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE HelpIndex;
BEGIN
   (* this just displays the system help index panel *)
   IF xtrn.fHelpEnabled THEN
      IF OS2.WinSendMsg(hwndHelpInstance
                       ,OS2.HM_HELP_INDEX
                       ,NIL
                       ,NIL) <> NIL THEN
         xtrn.MessageBox(xtrn.hwndMain
                        ,mainres.IDMSG_HELPDISPLAYERROR
                        ,OS2.MB_OK+OS2.MB_ERROR
                        ,FALSE);
      END;
   END;
END HelpIndex;


(**************************************************************************
 *
 *  Name       : HelpTutorial()
 *
 *  Description: Processes the WM_COMMAND message posted by the
 *                Tutorial Help item of the Help menu.  While the
 *                standard template application does not include a
 *                Tutorial menu item, you can add one if your
 *                application has a tutorial.
 *
 *  Concepts:    Called from MainCommand when the Tutorial Help
 *               menu item is selected.
 *
 *  API's      :  WinLoadMessage
 *                WinMessageBox
 *                WinAlarm
 *
 *  Parameters :  [none]
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE HelpTutorial;
VAR
   szText : ARRAY [0..xtrn.MESSAGELEN-1] OF CHAR;
BEGIN

   (*
    *  Insert code for any tutorial below in place of the message box.
    *)
   IF OS2.WinLoadMessage(xtrn.hab
                        ,OS2.NULLHANDLE
                        ,mainres.IDMSG_YOURTUTORIAL
                        ,xtrn.MESSAGELEN
                        ,szText) = 0 THEN
      OS2.WinAlarm(OS2.HWND_DESKTOP, OS2.WA_ERROR);
      RETURN;
   END;
   OS2.WinMessageBox(OS2.HWND_DESKTOP
                    ,xtrn.hwndMain
                    ,szText
                    ,"Tutorial"
                    ,OS2.MB_OK+OS2.MB_INFORMATION
                    ,0);

END HelpTutorial;


(**************************************************************************
 *
 *  Name       : HelpProductInfo()
 *
 *  Description: Processes the WM_COMMAND message posted by the
 *               Product information item of the Help Menu.
 *
 *  Concepts:    Called from MainCommand when the Product information
 *               menu item is selected
 *
 *               Calls WinDlgBox to display the Product information dialog.
 *
 *  API's      : WinDlgBox
 *
 *  Parameters :  [none]
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE HelpProductInfo;
BEGIN
   (* display the Product Information dialog. *)
   OS2.WinDlgBox(OS2.HWND_DESKTOP
                ,xtrn.hwndMain
                ,dlg.ProductInfoDlgProc
                ,0
                ,dlgres.IDD_PRODUCTINFO
                ,NIL);
END HelpProductInfo;


(**************************************************************************
 *
 *  Name       : DisplayHelpPanel(idPanel)
 *
 *  Description: Displays the help panel whose id is given
 *
 *  Concepts:    Called whenever a help panel is desired to be
 *               displayed, usually from the WM_HELP processing
 *               of the dialog boxes.
 *
 *               Sends HM_DISPLAY_HELP message to the help instance.
 *
 *  API's      : WinSendMsg
 *
 *  Parameters :  idPanel = panel i.d.
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE DisplayHelpPanel(idPanel : OS2.ULONG);
BEGIN
   IF xtrn.fHelpEnabled THEN
      IF OS2.WinSendMsg(hwndHelpInstance
                       ,OS2.HM_DISPLAY_HELP
                       ,OS2.MPFROMLONG(idPanel)
                       ,OS2.MPFROMSHORT(OS2.HM_RESOURCEID)) <> NIL THEN
         xtrn.MessageBox(xtrn.hwndMainFrame
                        ,mainres.IDMSG_HELPDISPLAYERROR
                        ,OS2.MB_OK+OS2.MB_ERROR
                        ,TRUE);
      END;
   END;
END DisplayHelpPanel;


(**************************************************************************
 *
 *  Name       : DestroyHelpInstance()
 *
 *  Description: Destroys the help instance for the application
 *
 *  Concepts:    Called after exit from message loop.
 *
 *               Calls WinDestroyHelpInstance() to destroy the
 *               help instance.
 *
 *  API's      : WinDestroyHelpInstance
 *
 *  Parameters :  [none]
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE DestroyHelpInstance;
BEGIN
   IF hwndHelpInstance <> OS2.NULLHANDLE THEN
      OS2.WinDestroyHelpInstance(hwndHelpInstance);
   END
END DestroyHelpInstance;

BEGIN
   hwndHelpInstance := OS2.NULLHANDLE;
END help.

