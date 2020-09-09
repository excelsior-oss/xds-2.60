(**************************************************************************
 *  File name  :  file.mod
 *
 *  Description:  This module contains the code for the WM_COMMAND messages
 *                posted by the standard File menu.
 *
 *                This module contains the following procedures:
 *
 *                FileNew(mp2);
 *                FileOpen(mp2);
 *                FileSave(mp2);
 *                FileSaveAs(mp2);
 *                WriteFileToDisk(hf);
 *                GetFileName();
 *                UpdateTitleText(hwnd);
 *                TemplateOpenFilterProc(hwnd, msg, mp1, mp2)
 *                TemplateSaveFilterProc(hwnd, msg, mp1, mp2)
 *
 *  Concepts   :  file management
 *
 *  API's      :  WinLoadString
 *                WinFileDlg
 *                DosOpen
 *                DosClose
 *                WinSetWindowText
 *                WinWindowFromID
 *                WinDefFileDlgProc
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

IMPLEMENTATION MODULE file;

IMPORT SYSTEM;

IMPORT OS2, OS2RTL;

IMPORT Strings;

IMPORT help, helpres, main, mainres, xtrn;

PROCEDURE WriteFileToDisk(hf : OS2.HFILE); FORWARD;

PROCEDURE GetFileName() : BOOLEAN; FORWARD;

PROCEDURE [OS2.EXPENTRY] TemplateOpenFilterProc(hwnd : OS2.HWND
                                               ;message: OS2.ULONG
                                               ;mp1, mp2 : OS2.MPARAM
                                               ) : OS2.MRESULT; FORWARD;

PROCEDURE [OS2.EXPENTRY] TemplateSaveFilterProc(hwnd : OS2.HWND
                                               ;message: OS2.ULONG
                                               ;mp1, mp2 : OS2.MPARAM
                                               ) : OS2.MRESULT; FORWARD;
(*
 *  Global variables
 *)

VAR
  szFullPath : ARRAY [0..OS2.CCHMAXPATH-1] OF CHAR;

(**************************************************************************
 *
 *  Name       : FileNew(mp2)
 *
 *  Description: Processes the File menu's New item
 *
 *  Concepts:  called whenever NEW from the FILE menu is selected
 *
 *  API's      : [none]
 *
 *  Parameters :  mp2      = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE FileNew(mp2 : OS2.MPARAM);
BEGIN
    (*
     *  Enter routines for creating a new file and window
     *)

    (* clear file name and reset the titlebar text *)
   szFullPath[0] := '';
   UpdateTitleText(xtrn.hwndMainFrame);

    (*  This routine currently doesn't use the mp2 parameter but
     *  it is referenced here to prevent an 'Unreferenced Parameter'
     *  warning at compile time.
     *)
END FileNew;


(**************************************************************************
 *
 *  Name       : FileOpen(mp2)
 *
 *  Description: Processes the File menu's Open item
 *
 *  Concepts:  Called whenever OPEN from the FILE menu is selected
 *
 *             Calls the standard file open dialog to get the
 *             file name.  The file name is passed onto DosOpen
 *             which returns the handle to the file.  The file
 *             input procedure is called and then the file handle
 *             is closed.
 *
 *  API's      : WinLoadString
 *               WinFileDlg
 *               DosOpen
 *               DosClose
 *
 *  Parameters :  mp2      = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE FileOpen(mp2 : OS2.MPARAM);
VAR
   fdg : OS2.FILEDLG;
   hfIn : OS2.HFILE;
   ulAction : OS2.ULONG;
   szTitle, szButton : ARRAY [0..xtrn.MESSAGELEN-1] OF CHAR;
BEGIN

   fdg.cbSize := SIZE(OS2.FILEDLG);

   IF OS2.WinLoadString(xtrn.hab, 0, mainres.IDS_OPEN, xtrn.MESSAGELEN, szTitle) = 0 THEN
      xtrn.MessageBox(xtrn.hwndMain
                     ,mainres.IDMSG_CANNOTLOADSTRING
                     ,OS2.MB_OK+OS2.MB_ERROR
                     ,TRUE);
      RETURN;
   END;

   IF OS2.WinLoadString(xtrn.hab, 0, mainres.IDS_OPEN, xtrn.MESSAGELEN, szButton) = 0 THEN
      xtrn.MessageBox(xtrn.hwndMain
                     ,mainres.IDMSG_CANNOTLOADSTRING
                     ,OS2.MB_OK+OS2.MB_ERROR
                     ,TRUE);
      RETURN;
   END;

   fdg.pszTitle := SYSTEM.ADR(szTitle);
   fdg.pszOKButton := SYSTEM.ADR(szButton);
   fdg.ulUser := 0;
   fdg.fl := OS2.FDS_HELPBUTTON + OS2.FDS_CENTER + OS2.FDS_OPEN_DIALOG;
   fdg.pfnDlgProc := TemplateOpenFilterProc;
   fdg.lReturn := 0;
   fdg.lSRC := 0;
   fdg.hMod := 0;
   fdg.usDlgId := mainres.FILEOPEN;
   fdg.x := 0;
   fdg.y := 0;

   IF OS2.WinLoadString(xtrn.hab
                       ,0
                       ,mainres.IDS_FILEOPENEXT
                       ,OS2.CCHMAXPATH
                       ,fdg.szFullFile) = 0 THEN
      xtrn.MessageBox(xtrn.hwndMain
                     ,mainres.IDMSG_CANNOTLOADSTRING
                     ,OS2.MB_OK+OS2.MB_ERROR
                     ,TRUE);
      RETURN;
   END;

   fdg.pszIType       := NIL;
   fdg.papszITypeList := NIL;
   fdg.pszIDrive      := NIL;
   fdg.papszIDriveList:= NIL;
   fdg.sEAType        := 0;
   fdg.papszFQFilename:= NIL;
   fdg.ulFQFCount     := 0;

   (* get the file *)
   IF OS2.WinFileDlg(OS2.HWND_DESKTOP, xtrn.hwndMain, fdg) = 0 THEN RETURN END;

   (*
    *  Upon sucessful return of a file, open it for reading
    *)

   IF fdg.lReturn = OS2.DID_OK THEN
      IF OS2.DosOpen(fdg.szFullFile,     (* file name from Open dialog *)
                     hfIn,               (* file handle returned *)
                     ulAction,
                     0,
                     OS2.FILE_NORMAL,
                     OS2.FILE_OPEN,
                     OS2.OPEN_ACCESS_READONLY + OS2.OPEN_SHARE_DENYNONE,
                     NIL) <> 0 THEN
         xtrn.MessageBox(xtrn.hwndMain,
                         mainres.IDMSG_CANNOTOPENINPUTFILE,
                         OS2.MB_OK+OS2.MB_ERROR,
                         FALSE);
           (*
            *  NOTE:  You now have several options on how to proceed
            *      from this point:
            *          - You can abort the File Open by returning from
            *            this procedure.
            *          - You can bring up the File Open dialog again
            *            and have the user pick another file.
            *          - You can check the error code from the DosOpen,
            *            determine why the open failed, and take an
            *            action appropriate to the specific failure.
            *)
         RETURN;
      END;

       (* copy file name into file name buffer *)
      Strings.Assign(fdg.szFullFile, szFullPath);

       (*
        *  Place routine for reading the file here.
        *)

      OS2.DosClose(hfIn);

      UpdateTitleText(xtrn.hwndMainFrame);
   END;
END FileOpen;


(**************************************************************************
 *
 *  Name       : FileSave(mp2)
 *
 *  Description: Processes the File menu's Save item
 *
 *  Concepts:  Called whenever SAVE from the FILE menu is selected
 *
 *             Routine opens the file for output, calls the
 *             application's save routine, and closes the file.
 *
 *  API's      : DosOpen
 *               DosClose
 *
 *  Parameters :  mp2      = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE FileSave(mp2 : OS2.MPARAM);
VAR
   hf       : OS2.HFILE;
   ulAction : OS2.ULONG;
BEGIN
   (*
    * If the file currently is untitled, we will need to get a file
    * name from the user before we can open the file.  Getting a
    * file name is normally done during the FileSaveAs operation
    * so we will treat this save as a SaveAs and call FileSaveAs().
    * If the file is titled, then we save the file.
    *
    * NOTE:  This routine will be called by FileSaveAs(), but only
    *  after a valid file name has been obtained.  So, FileSaveAs()
    *  will not be called again from this routine.
    *)
   IF szFullPath[0] = '' THEN
      FileSaveAs(mp2);
      RETURN;
   END;

   (* open the file *)
   IF OS2.DosOpen(szFullPath          (* file name of current document *)
                 ,hf                  (* file handle of output file *)
                 ,ulAction
                 ,0
                 ,OS2.FILE_NORMAL
                 ,OS2.FILE_OPEN+OS2.FILE_CREATE
                 ,OS2.OPEN_ACCESS_WRITEONLY+OS2.OPEN_SHARE_DENYNONE
                 ,NIL) <> 0 THEN
       xtrn.MessageBox(xtrn.hwndMain
                      ,mainres.IDMSG_CANNOTOPENOUTPUTFILE
                      ,OS2.MB_OK+OS2.MB_ERROR
                      ,FALSE);
       RETURN;
   END;

   WriteFileToDisk(hf);

   OS2.DosClose(hf);
END FileSave;

(**************************************************************************
 *
 *  Name       : FileSaveAs(mp2)
 *
 *  Description: Processes the File menu's Save as item
 *
 *  Concepts:  Called whenever SAVE AS from the FILE menu is selected
 *
 *             Routine prompts the user for the name of the file,
 *             then saves the file.
 *
 *  API's      : DosOpen
 *               DosClose
 *
 *  Parameters :  mp2      = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE FileSaveAs(mp2 : OS2.MPARAM);
VAR
   hf : OS2.HFILE;
   ulAction : OS2.ULONG;
   sT : OS2.ULONG;
BEGIN
   LOOP     (* infinite loop until we break out of it *)
      (* Get a name for the file *)
      IF NOT GetFileName() THEN RETURN END;

      (* See if the file exists.  If it does, then confirm that the
       * user wants to overwrite it.  If he doesn't, then get a new
       * file name
       *)
      IF OS2.DosOpen(szFullPath      (* file name from, GetFileName() *)
                    ,hf              (* handle of opened file *)
                    ,ulAction
                    ,0
                    ,OS2.FILE_NORMAL
                    ,OS2.FILE_CREATE+OS2.FILE_OPEN
                    ,OS2.OPEN_ACCESS_WRITEONLY+OS2.OPEN_SHARE_DENYNONE
                    ,NIL) <> 0 THEN
         xtrn.MessageBox(xtrn.hwndMain
                        ,mainres.IDMSG_CANNOTOPENOUTPUTFILE
                        ,OS2.MB_OK+OS2.MB_ERROR
                        ,FALSE);
         RETURN;
      ELSE
         OS2.DosClose(hf);
      END;

      (* if file exists, ask if we want to overwrite it *)
      IF ulAction = OS2.FILE_EXISTED THEN
         sT := xtrn.MessageBox(xtrn.hwndMain,
                              mainres.IDMSG_OVERWRITEFILE,
                              OS2.MB_QUERY+OS2.MB_YESNOCANCEL,
                              FALSE);
         IF sT = OS2.MBID_CANCEL THEN RETURN
         ELSIF sT = OS2.MBID_YES THEN EXIT
         END;
          (* if user selected no, repeat the sequence *)
      ELSE
         EXIT
      END;
   END;   (* LOOP *)

   UpdateTitleText(xtrn.hwndMainFrame);

   (*
    * Now that we have a valid file name, save the file.  This is
    * normally done under the File Save function so we can just
    * call the FileSave() function here.  Note that FileSave() will
    * not call FileSaveAs() back since there is a valid file name
    *)
    FileSave(mp2);
END FileSaveAs;


(**************************************************************************
 *
 *  Name       : WriteFileToDisk(hfile)
 *
 *  Description: writes the current file to the file in hf
 *
 *  Concepts:  called from FileSave and FileSaveAs when a fileelected
 *             is to be saved to disk
 *
 *             NOTE:  This routine must not close the file.
 *
 *  API's      : [none]
 *
 *  Parameters :  hfile    = file handle
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE WriteFileToDisk(hf : OS2.HFILE);
BEGIN
    (*
     *  Place routine to write a disk file here.
     *)
END WriteFileToDisk;


(**************************************************************************
 *
 *  Name       : GetFileName()
 *
 *  Description: gets the name of the save file
 *
 *  Concepts:    called when the user needs to supply a name for
 *               the file to be saved
 *
 *                calls the standard file open dialog to get the
 *               file name.
 *
 *  API's      : WinLoadString
 *               WinFileDlg
 *
 *  Parameters : [none]
 *
 *  Return     :  TRUE if successful in getting a file name
 *                FALSE if not successful in getting a file name
 *
 *************************************************************************)
PROCEDURE GetFileName() : BOOLEAN;
VAR
   fdg : OS2.FILEDLG;
   szTitle, szButton : ARRAY [0..xtrn.MESSAGELEN-1] OF CHAR;
BEGIN
   fdg.cbSize := SIZE(OS2.FILEDLG);

   IF OS2.WinLoadString(xtrn.hab
                       ,0
                       ,mainres.IDS_SAVE
                       ,xtrn.MESSAGELEN
                       ,szTitle) = 0 THEN
      xtrn.MessageBox(xtrn.hwndMain
                     ,mainres.IDMSG_CANNOTLOADSTRING
                     ,OS2.MB_OK+OS2.MB_ERROR
                     ,TRUE);
      RETURN FALSE;
   END;

   IF OS2.WinLoadString(xtrn.hab
                       ,0
                       ,mainres.IDS_SAVE
                       ,xtrn.MESSAGELEN
                       ,szButton) = 0 THEN
      xtrn.MessageBox(xtrn.hwndMain
                     ,mainres.IDMSG_CANNOTLOADSTRING
                     ,OS2.MB_OK+OS2.MB_ERROR
                     ,TRUE);
      RETURN FALSE;
   END;

   fdg.pszTitle := SYSTEM.ADR(szTitle);
   fdg.pszOKButton := SYSTEM.ADR(szButton);

   fdg.ulUser := 0;
   fdg.fl := OS2.FDS_HELPBUTTON+OS2.FDS_CENTER+OS2.FDS_SAVEAS_DIALOG;
   fdg.pfnDlgProc := TemplateSaveFilterProc;
   fdg.lReturn := 0;
   fdg.lSRC := 0;
   fdg.hMod := 0;
   fdg.usDlgId := mainres.FILESAVE;
   fdg.x := 0;
   fdg.y := 0;
   fdg.pszIType := NIL;
   fdg.papszITypeList := NIL;
   fdg.pszIDrive := NIL;
   fdg.papszIDriveList := NIL;
   fdg.sEAType := 0;
   fdg.papszFQFilename := NIL;
   fdg.ulFQFCount := 0;

   Strings.Assign(szFullPath, fdg.szFullFile);

   (* get the file *)
   IF OS2.WinFileDlg(OS2.HWND_DESKTOP, xtrn.hwndMain, fdg) = 0 THEN
       RETURN FALSE;
   END;

   IF fdg.lReturn <> OS2.DID_OK THEN
       RETURN FALSE;
   END;

   (* copy file name and path returned into buffers *)
   Strings.Assign(fdg.szFullFile,szFullPath);

   RETURN TRUE;
END GetFileName;

(**************************************************************************
 *
 *  Name       : UpdateTitleText
 *
 *  Description: Updates the text in the main window's title bar to
 *               display the app name, followed by the separator,
 *               followed by the file name.
 *
 *  Concepts :   Called at init time and when the text file is changed.
 *               Gets the program name, appends the separator, and
 *               appends the file name.
 *
 *  API's      : WinLoadString
 *               WinSetWindowText
 *               WinWindowFromID
 *
 *  Parameters : hwnd = window handle
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE UpdateTitleText(hwnd : OS2.HWND);
VAR
   szBuf : ARRAY [0..OS2.MAXNAMEL+xtrn.TITLESEPARATORLEN+OS2.CCHMAXPATH-1] OF CHAR;
   szSeparator : ARRAY [0..xtrn.TITLESEPARATORLEN] OF CHAR;
BEGIN
   OS2.WinLoadString(xtrn.hab, 0, mainres.IDS_APPNAME, OS2.MAXNAMEL, szBuf);
   OS2.WinLoadString(xtrn.hab,
                     0,
                     mainres.IDS_TITLEBARSEPARATOR,
                     xtrn.TITLESEPARATORLEN,
                     szSeparator);

   Strings.Concat(szBuf, szSeparator, szBuf);

   IF szFullPath[0] = '' THEN
      Strings.Concat(szBuf, xtrn.szUntitled, szBuf);
   ELSE
      Strings.Concat(szBuf, szFullPath, szBuf);
   END;

   OS2.WinSetWindowText(OS2.WinWindowFromID(hwnd, OS2.FID_TITLEBAR), szBuf);
END UpdateTitleText;


(**************************************************************************
 *
 *  Name       : TemplateOpenFilterProc(hwnd, msg, mp1, mp2)
 *
 *  Description: Filters the help messages to the open dialog
 *
 *  Concepts :   If help is requested during the standard OPEN
 *               dialog, this routine will display an appropriate
 *               panel.
 *
 *  API's      : WinDefFileDlgProc
 *
 *  Parameters : hwnd = window handle
 *               message = message i.d.
 *               mp1     = first message parameter
 *               mp2     = second message parameter
 *
 *  Return     :  the result of the message
 *
 *************************************************************************)
PROCEDURE [OS2.EXPENTRY] TemplateOpenFilterProc(hwnd : OS2.HWND
                                               ;message: OS2.ULONG
                                               ;mp1, mp2 : OS2.MPARAM
                                               ) : OS2.MRESULT;
BEGIN
   IF message = OS2.WM_HELP THEN
      help.DisplayHelpPanel(helpres.HID_FS_OPEN_DLG_HELP_PANEL);
      RETURN OS2.MRFROMULONG(0);     (* i.e. FALSE *)
   END;

   RETURN OS2.WinDefFileDlgProc(hwnd, message, mp1, mp2);
END TemplateOpenFilterProc;


(**************************************************************************
 *
 *  Name       : TemplateSaveFilterProc(hwnd, msg, mp1, mp2)
 *
 *  Description: Filters the help messages to the save dialog
 *
 *  Concepts :   If help is requested during the standard SAVEAS
 *               dialog, this routine will display an appropriate
 *               panel.
 *
 *  API's      : WinDefFileDlgProc
 *
 *  Parameters : hwnd = window handle
 *               message = message i.d.
 *               mp1     = first message parameter
 *               mp2     = second message parameter
 *
 *  Return     :  the result of the message
 *
 *************************************************************************)
PROCEDURE [OS2.EXPENTRY] TemplateSaveFilterProc(hwnd : OS2.HWND
                                               ;message: OS2.ULONG
                                               ;mp1, mp2 : OS2.MPARAM
                                               ) : OS2.MRESULT;
BEGIN
   IF message = OS2.WM_HELP THEN
      help.DisplayHelpPanel(helpres.HID_FS_SAVE_DLG_HELP_PANEL);
      RETURN OS2.MRFROMULONG(0);  (* i.e. FALSE *)
   END;
   RETURN OS2.WinDefFileDlgProc(hwnd, message, mp1, mp2);
END TemplateSaveFilterProc;


BEGIN
   szFullPath := "";
END file.
