(**************************************************************************
 *  File name  :  init.mod
 *
 *  Description:  This module contains the code for application
 *                initialization, as well as the code for exit list
 *                processing
 *
 *                This module contains the following functions:
 *
 *                Init()
 *                InitMainWindow(hwnd, mp1, mp2)
 *                ExitProc(usTermCode)
 *
 *  Concepts   :  initialization
 *
 *  API's      :  DosExitList
 *                DosExit
 *                WinLoadString
 *                WinRegisterClass
 *                WinIsWindow
 *                WinDestroyWindow
 *                WinDestroyMsgQueue
 *                WinTerminate
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

IMPLEMENTATION MODULE init;

IMPORT SYSTEM;

IMPORT OS2, OS2RTL;

IMPORT file, main, mainres, xtrn;

<* IF BACKGROUND_THREAD THEN *>
IMPORT thrd;
<* END *>

PROCEDURE ["SysCall"] ExitProc(ulTermCode : OS2.ULONG); FORWARD;

(**************************************************************************
 *
 *  Name       : Init()
 *
 *  Description:  Performs initialization functions required
 *                before the main window can be created.
 *
 *  Concepts:     Called once before the main window is created.
 *
 *                - Installs the routine ExitProc into the
 *                  DosExitList chain
 *                - registers all window classes
 *                - performs any command line processing
 *
 *  API's      :  DosExitList
 *                DosExit
 *                WinLoadString
 *                WinRegisterClass
 *
 *  Parameters :  [none]
 *
 *  Return     :  TRUE = initialization is successful
 *                FALSE = initialization failed
 *
 *************************************************************************)
PROCEDURE Init(): OS2.BOOL;
BEGIN
   (* Add ExitProc to the exit list to handle the exit processing.  If
    * there is an error, then terminate the process since there have
    * not been any resources allocated yet.
    *)
   IF OS2.DosExitList(OS2.EXLST_ADD, ExitProc) <> 0 THEN
      xtrn.MessageBox(OS2.HWND_DESKTOP
                     ,mainres.IDMSG_CANNOTLOADEXITLIST
                     ,OS2.MB_OK + OS2.MB_ERROR
                     ,TRUE);
      OS2.DosExit(OS2.EXIT_PROCESS, 1);
   END;

   (* load application name from resource file *)
   IF OS2.WinLoadString(xtrn.hab
                       ,0
                       ,mainres.IDS_APPNAME
                       ,OS2.MAXNAMEL
                       ,xtrn.szAppName) = 0 THEN
      RETURN FALSE;
   END;

   (* load "untitled" string *)
   IF OS2.WinLoadString(xtrn.hab
                       ,0
                       ,mainres.IDS_UNTITLED
                       ,xtrn.MESSAGELEN
                       ,xtrn.szUntitled) = 0 THEN
       RETURN FALSE;
   END;

   (* register the main client window class *)
   IF NOT OS2.WinRegisterClass(xtrn.hab,
                               xtrn.szAppName,
                               main.MainWndProc,
                               OS2.CS_SIZEREDRAW + OS2.CS_CLIPCHILDREN,
                               0) THEN
       RETURN FALSE;
   END;

   (* If you wish to create a thread for background processing, declare
    *  the BACKGROUND_THREAD option and set it ON in the project file
    *  The routines for the background thread are in the thrd module.
    *)

<* IF BACKGROUND_THREAD THEN *>
   (* see main.h for comment on using a background thread *)

   IF NOT thrd.CreateBackgroundThread() THEN
       RETURN FALSE;
   END;

<* END *>

    (*
     *      Add any command line processing here.
     *)


   RETURN TRUE;
END Init;


(**************************************************************************
 *
 *  Name       : InitMainWindow(hwnd, mp1, mp2)
 *
 *  Description: Performs initialization functions required
 *               when the main window is created.
 *
 *  Concepts:    Called once during the WM_CREATE processing when
 *               the main window is created.
 *
 *  API's      : [none]
 *
 *  Parameters :  hwnd = window handle
 *                mp1  = first message parameter
 *                mp2  = second message parameter
 *
 *  Return     :   value to be returned from the WM_CREATE message:
 *                TRUE =  window creation should stop
 *                FALSE = window creation should continue
 *
 *************************************************************************)
PROCEDURE InitMainWindow(hwnd : OS2.HWND; mp1,mp2: OS2.MPARAM) : OS2.MRESULT;
VAR
   pcrstTemp : OS2.PCREATESTRUCT;
BEGIN
   pcrstTemp := mp2;
   file.UpdateTitleText(pcrstTemp^.hwndParent);
   (* return FALSE to continue window creation, TRUE to abort it *)
   RETURN SYSTEM.CAST(OS2.MRESULT,FALSE);

END InitMainWindow;

(**************************************************************************
 *
 *  Name       : ExitProc(usTermCode)
 *
 *  Description: Cleans up certain resources when the application
 *               terminates.
 *
 *  Concepts:    Routine is called by DosExitList when the
 *               application exits.
 *
 *               Global resources, such as the main window and
 *               message queue, are destroyed and any system
 *               resources used are freed.
 *
 *  API's      : WinIsWindow
 *               WinDestroyWindow
 *               WinDestroyMsgQueue
 *               WinTerminate
 *               DosExitList
 *
 *  Parameters :  usTermCode = termination code number
 *
 *  Return    :   Returns EXLST_EXIT to the DosExitList handler
 *
 *************************************************************************)
PROCEDURE ["SysCall"] ExitProc(ulTermCode : OS2.ULONG);
BEGIN
    (* destroy the main window if it exists *)
   IF OS2.WinIsWindow(xtrn.hab, xtrn.hwndMainFrame) THEN
      OS2.WinDestroyWindow(xtrn.hwndMainFrame);
   END;

    (*
     *      Any other system resources used
     *      (e.g. memory or files) should be freed here.
     *)

   OS2.WinDestroyMsgQueue(xtrn.hmq);

   OS2.WinTerminate(xtrn.hab);

   OS2.DosExitList(OS2.EXLST_EXIT, NIL);    (* termination complete *)

END ExitProc;


END init.
(***************************  End of init.mod  ***************************)
