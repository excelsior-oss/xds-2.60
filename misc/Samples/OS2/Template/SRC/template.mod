(**************************************************************************
 *  File name  :  Template.mod
 *
 *  Description:  This application serves as a template than can be
 *                modified by an application developer.  The source
 *                files are organized so that the overhead code, which
 *                should be in all applications, is located in the same
 *                files, so that these files do not need to be modified.
 *                The routines that deal with application-specific code
 *                are also locatedin their own modules.  An application
 *                developer need only change these files in order to
 *                modify this template for his application.
 *
 *                This source file contains the top-level module
 *                of the application, which body serves as the main
 *                routine.
 *
 *  Concepts   :  Presentation Manager
 *
 *  API's      :  WinInitialize
 *                DosBeep
 *                WinCreateMsgQueue
 *                WinTerminate
 *                WinCreateStdWindow
 *                WinOpenWindowDC
 *                WinGetMsg
 *                WinDispatchMsg
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

MODULE Template;

IMPORT OS2, OS2RTL;

IMPORT help, init, main, mainres, xtrn;

(*
 *  Included in the sample template is code that creates a
 *  second thread that can be used for background processing.
 *  To use this thread, toggle the BACKGROUND_THREAD option ON
 *  in the project file. This will enable the routines that
 *  create and destroy the background processing thread.
 *  The routines for the thread itself are located in the
 *  "thrd" module.
 *
 *  NOTE: Do not forget to switch the MULTITHREAD option ON
 *        to link with the multithread version of the library.
 *)

<* IF BACKGROUND_THREAD THEN *>
IMPORT thrd;
<* END *>

CONST
  BEEP_WARN_FREQ =  60; (* frequency of warning beep *)
  BEEP_WARN_DUR  = 100; (* duration of warning beep *)

VAR
   qmsg: OS2.QMSG;          (* message structure *)
   flCtlData: OS2.ULONG;    (* frame control data *)

(**************************************************************************
 *
 *  Name       : Template module body
 *
 *  Description: Initializes the PM environment, calls the
 *               initialization routine, creates the main
 *               window,  and polls the message queue
 *
 *  Concepts:    - obtains anchor block handle and creates message
 *                    queue
 *               - calls the initialization routine
 *               - creates the main frame window which creates the
 *                    main client window
 *               - polls the message queue via Get/Dispatch Msg loop
 *               - upon exiting the loop, exits
 *
 *  API's      :  WinInitialize
 *                DosBeep
 *                WinCreateMsgQueue
 *                WinTerminate
 *                WinCreateStdWindow
 *                WinOpenWindowDC
 *                WinGetMsg
 *                WinDispatchMsg
 *
 *  Parameters :  [none]
 *
 *  Return     :  0 - if successful execution completed
 *                1 - if error
 *
 *************************************************************************)

BEGIN
   xtrn.hab := OS2.WinInitialize(0);

   IF OS2.NULLHANDLE = xtrn.hab THEN
      OS2.DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
      HALT(1);
   END;

   xtrn.hmq := OS2.WinCreateMsgQueue(xtrn.hab, 0);

   IF OS2.NULLHANDLE = xtrn.hmq THEN
      OS2.DosBeep(BEEP_WARN_FREQ, BEEP_WARN_DUR);
      OS2.WinTerminate(xtrn.hab);
      HALT(1);
   END;

   IF NOT init.Init() THEN
      xtrn.MessageBox(OS2.HWND_DESKTOP
                     ,mainres.IDMSG_INITFAILED
                     ,OS2.MB_OK + OS2.MB_ERROR
                     ,TRUE);
      HALT(1);
   END;

   (* NOTE:  clean up from here is handled by the DosExitList processing *)
   (* Since signal exceptions are not handled by RTS yet, using module   *)
   (* finalization for clean up is incorrect. This will be changed in the*)
   (* next release.                                                      *)

   (* create the main window *)
   flCtlData := OS2.FCF_STANDARD;

   xtrn.hwndMainFrame := OS2.WinCreateStdWindow(OS2.HWND_DESKTOP,
                                                OS2.WS_VISIBLE,
                                                flCtlData,
                                                xtrn.szAppName,
                                                NIL,
                                                OS2.WS_VISIBLE,
                                                0,
                                                mainres.IDR_MAIN,
                                                xtrn.hwndMain);

   IF OS2.NULLHANDLE = xtrn.hwndMainFrame THEN
      xtrn.MessageBox(OS2.HWND_DESKTOP
                     ,mainres.IDMSG_MAINWINCREATEFAILED
                     ,OS2.MB_OK + OS2.MB_ERROR
                     ,TRUE);
      HALT(1);
   END;

   xtrn.hdcMain := OS2.WinOpenWindowDC(xtrn.hwndMain);

   help.InitHelp;

   (* Get/Dispatch Message loop *)
   WHILE OS2.WinGetMsg(xtrn.hab, qmsg, 0, 0, 0) DO
       OS2.WinDispatchMsg(xtrn.hab, qmsg);
   END;

   (* destroy the help instance *)
   help.DestroyHelpInstance();

<* IF BACKGROUND_THREAD THEN *>
   (* see top of file for comment on using a background thread *)
   thrd.DestroyBackgroundThread();
<* END *>

END Template.
