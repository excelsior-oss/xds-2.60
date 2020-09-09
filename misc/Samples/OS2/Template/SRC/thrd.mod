(**************************************************************************
 *  File name  :  thrd.mod
 *
 *  Description:  This module contains the code for a background processing
 *                thread as well as the code for any routines used in that
 *                thread.
 *
 *                This thread uses an object window to facilitate
 *                communication with the main thread.  The main thread can
 *                communicate with the background thread through messages.
 *                You define your own window messages in main.h, post or send
 *                them to the background thread through the routines provided
 *                in this file, and then process the messages in the
 *                ObjectWndProc() routine.  The object window then sends
 *                other messages back to the main window to notify the main
 *                window as to the status of the task or with the final
 *                result.
 *
 *                An object window is used as the main window of the thread
 *                for several reasons.  The window is hidden, so it is
 *                never displayed on the screen.  Since it is hidden, the
 *                window does not have to process most of the window
 *                messages that are processed by visible windows, e.g.
 *                WM_PAINT, WM_SIZE, WM_SETFOCUS, etc.  This window will
 *                never receive the focus so it need not process input
 *                messages.  You can, however, send all mouse and keyboard
 *                messages to this window and have it process all input if
 *                you wish.  Since the window does not belong to any focus
 *                or activation chains, you can block this thread on a
 *                semaphore without having the system lock up or use the
 *                window manager semaphore routines such as
 *                WinMsgSemWait().
 *
 *                The BackgroundThread() routine is designed to create only
 *                one thread.  Should you try to create another one, the
 *                call will fail.  The thread uses several variables
 *                which are defined globally within this module.  In order
 *                to enable this routine to create multiple threads, you
 *                will need to make these variables local.  You will also
 *                need to differentiate between the different object
 *                windows created so that you can specify which thread will
 *                receive a posted message.
 *
 *                In order to enable the use of the second thread, define
 *                the BACKGROUND_THREAD constant found in main.h.
 *
 *                This module contains the following procedures:
 *
 *                BackgroundThread()
 *                CreateBackgroundThread()
 *                DestroyBackgroundThread()
 *                PostBkThreadMsg(msg, mp1, mp2)
 *                SendBkThreadMsg(msg, mp1, mp2)
 *                ObjectWndProc(hwnd, msg, mp1, mp2)
 *
 *  Concepts   :  background thread
 *
 *  API's      :  WinInitialize
 *                WinPostMsg
 *                DosExit
 *                WinCreateMsgQueue
 *                WinTerminate
 *                WinLoadString
 *                WinRegisterClass
 *                WinCreateWindow
 *                DosPostEventSem
 *                WinGetMsg
 *                WinDispatchMsg
 *                WinIsWindow
 *                WinDestroyWindow
 *                WinDestroyMsgQueue
 *                DosCreateEventSem
 *                DosCreateThread
 *                WinWaitEventSem
 *                DosWaitThread
 *                WinDefWindowProc
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

IMPLEMENTATION MODULE thrd;

IMPORT OS2, OS2RTL;

IMPORT main, mainres, xtrn;

CONST THREADSTACKSIZE = 8192;
CONST SEM_TIMEOUT     = 10000;

(*
 *  Global variables
 *)
VAR
   habBkThread : OS2.HAB;
   hmqBkThread : OS2.HMQ;
   tidBkThread : OS2.TID;
   hwndObject : OS2.HWND;
   szObjectClass : ARRAY [0..OS2.MAXNAMEL-1] OF CHAR;
   fThreadCreated : OS2.BOOL;
   hevThreadInit : OS2.HEV;   (* semaphore for thread initialization *)

PROCEDURE [OS2.EXPENTRY] ObjectWndProc(hwnd : OS2.HWND
                                      ;msg  : OS2.ULONG
                                      ;mp1  : OS2.MPARAM
                                      ;mp2  : OS2.MPARAM
                                      ) : OS2.MRESULT; FORWARD;

(**************************************************************************
 *
 *  Name       : BackgroundThread(ulThreadParam)
 *
 *  Description: Routine is a background thread used for tasks to be
 *               completed in the background.
 *
 *  Concepts:    Routine is called at initialization time to create
 *               a background processing thread.
 *
 *               The routine initializes itself as a PM thread and creates
 *               a message queue.  It then creates an object window
 *               through which it will receive and send messages.
 *               It then polls through a message loop, processing any
 *               messages it receives in the object window's window
 *               procedure.  When the loop ends, it terminates.
 *
 *  API's      :  WinInitialize
 *                WinPostMsg
 *                DosExit
 *                WinCreateMsgQueue
 *                WinLoadString
 *                WinRegisterClass
 *                WinCreateWindow
 *                DosPostEventSem
 *                WinGetMsg
 *                WinDispatchMsg
 *                WinIsWindow
 *                WinDestroyWindow
 *                WinDestroyMsgQueue
 *
 *  Parameters :  ulThreadParam = thread parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE [OS2.APIENTRY] BackgroundThread(ulThreadParam : OS2.ULONG);
VAR
   qmsg : OS2.QMSG;
BEGIN
   (* create message queue for thread *)
   habBkThread := OS2.WinInitialize(0);
   IF habBkThread = OS2.NULLHANDLE THEN
      OS2.WinPostMsg(xtrn.hwndMain, xtrn.TM_THREADINITFAILED, NIL, NIL);
      OS2.DosExit(OS2.EXIT_THREAD, 0);
   END;

   hmqBkThread := OS2.WinCreateMsgQueue(habBkThread, 0);
   IF hmqBkThread = OS2.NULLHANDLE THEN
      OS2.WinPostMsg(xtrn.hwndMain, xtrn.TM_THREADINITFAILED, NIL, NIL);
      OS2.WinTerminate(habBkThread);
      OS2.DosExit(OS2.EXIT_THREAD, 0);
   END;

   (* load the string for the object window class and register the class *)
   IF (OS2.WinLoadString(habBkThread
                        ,OS2.NULLHANDLE
                        ,mainres.IDS_OBJECTCLASS
                        ,OS2.MAXNAMEL
                        ,szObjectClass) <> 0)
      AND
              (* register the main client window class *)
      OS2.WinRegisterClass(habBkThread
                          ,szObjectClass
                          ,ObjectWndProc
                          ,0
                          ,0) THEN
            (* create the object window *)
      hwndObject := OS2.WinCreateWindow(OS2.HWND_OBJECT
                                       ,szObjectClass
                                       ,NIL
                                       ,0
                                       ,0
                                       ,0
                                       ,0
                                       ,0
                                       ,OS2.NULLHANDLE
                                       ,OS2.HWND_TOP
                                       ,mainres.OBJECTID
                                       ,NIL
                                       ,NIL);
      IF hwndObject <> OS2.NULLHANDLE THEN
         (* set thread created flag so another thread of this type
          * cannot be created.
          *)
         fThreadCreated := TRUE;

         (* clear initialization semaphore *)
         OS2.DosPostEventSem(hevThreadInit);

         (* message loop *)
         WHILE OS2.WinGetMsg(habBkThread, qmsg, OS2.NULLHANDLE, 0, 0) DO
            OS2.WinDispatchMsg(habBkThread, qmsg);
         END;
      END;
   END;
   (* If thread was not created, post the corresponding message *)
   IF NOT fThreadCreated THEN
      OS2.WinPostMsg(xtrn.hwndMain, xtrn.TM_THREADINITFAILED, NIL, NIL);
   END;

   (* destroy object window, clean up message queue and terminate *)
   IF OS2.WinIsWindow(habBkThread, hwndObject) THEN
      OS2.WinDestroyWindow(hwndObject);
   END;

   OS2.WinDestroyMsgQueue(hmqBkThread);
   OS2.WinTerminate(habBkThread);

   (* If termination is due to an error initializing the thread, then
       clear the initialization semaphore so that the main thread
       can continue. *)
   IF NOT fThreadCreated THEN
      OS2.DosPostEventSem(hevThreadInit);
   END;

   OS2.DosExit(OS2.EXIT_THREAD, 0);

END BackgroundThread;


(**************************************************************************
 *
 *  Name       : CreateBackgroundThread()
 *
 *  Description: Creates the background thread
 *
 *  Concepts:    Routine is called at initialization time to create
 *               a background processing thread.
 *
 *               The routine calls DosCreateThread with the Background
 *               thread routine.
 *
 *  API's      :  DosCreateEventSem
 *                DosCreateThread
 *                WinWaitEventSem
 *
 *  Parameters :  [none]
 *
 *  Return     :  TRUE = thread is created successfully
 *                FALSE = not, or if the thread was already created
 *
 *************************************************************************)
PROCEDURE CreateBackgroundThread() : OS2.BOOL;
VAR
   sRet : OS2.ULONG;
BEGIN
   IF NOT fThreadCreated THEN
      OS2.DosCreateEventSem(NIL, hevThreadInit, 0, FALSE);
      sRet := OS2.DosCreateThread(tidBkThread
                                 ,BackgroundThread
                                 ,0
                                 ,0
                                 ,THREADSTACKSIZE);
   ELSE
      RETURN FALSE;
   END;

   (* wait until the thread has finished initialization *)
   IF OS2.WinWaitEventSem(hevThreadInit, SEM_TIMEOUT) <> 0 THEN
      RETURN FALSE;
   END;

   RETURN (sRet = 0);
END CreateBackgroundThread;

(**************************************************************************
 *
 *  Name       : DestroyBackgroundThread()
 *
 *  Description: Destroys the background thread
 *
 *  Concepts:    Routine is called at exit time to destroy the
 *               background processing thread.
 *
 *               The routine posts a WM_CLOSE message to the object window
 *               to end its message loop.  It then waits to make sure that
 *               the thread has been terminated before it returns.
 *
 *  API's      :  DosWaitThread
 *
 *  Parameters :  [none]
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE DestroyBackgroundThread;
BEGIN
   IF PostBkThreadMsg(OS2.WM_CLOSE, NIL, NIL) THEN END;
   OS2.DosWaitThread(tidBkThread, 0);
END DestroyBackgroundThread;


(**************************************************************************
 *
 *  Name       : PostBkThreadMsg(msg, mp1, mp2)
 *
 *  Description: Routine posts a message to the object window of the
 *               background thread.
 *
 *  Concepts:    Routine is called whenever a message is to be posted to
 *               the background processing thread.
 *
 *               The routine posts the message to the object window of the
 *               thread.
 *
 *  API's      :  WinPostMsg
 *
 *  Parameters :  msg = the message i.d.
 *                mp1 = first message parameter
 *                mp2 = second message parameter
 *
 *  Return     :  the return value from WinPostMsg().
 *
 *************************************************************************)
PROCEDURE PostBkThreadMsg(msg : OS2.ULONG; mp1, mp2 : OS2.MPARAM) : OS2.BOOL;
BEGIN
   RETURN OS2.WinPostMsg(hwndObject, msg, mp1, mp2);
END PostBkThreadMsg;


(**************************************************************************
 *
 *  Name       : SendBkThreadMsg(msg, mp1, mp2)
 *
 *  Description: Routine sends a message to the object window of the
 *               background thread.
 *
 *  Concepts:    Routine is called whenever a message is to be sent to
 *               the background processing thread.
 *
 *               The routine sends the message to the object window of the
 *               thread.
 *
 *  API's      :  WinSendMsg
 *
 *  Parameters :  msg = the message i.d.
 *                mp1 = first message parameter
 *                mp2 = second message parameter
 *
 *  Return     :  the return value from WinSendMsg().
 *
 *************************************************************************)
PROCEDURE SendBkThreadMsg(msg : OS2.ULONG; mp1, mp2 : OS2.MPARAM) : OS2.MRESULT;
BEGIN
   RETURN OS2.WinSendMsg(hwndObject, msg, mp1, mp2);
END SendBkThreadMsg;


(**************************************************************************
 *
 *  Name       : ObjectWndProc(hwnd, msg, mp1, mp2)
 *
 *  Description: Processes all messages sent to the Object window
 *
 *  Concepts:    Called for each message sent to the Object window.
 *
 *               The Object window processes the messages that tell
 *               the background thread what action to take.  Since
 *               the object window is not visible, it will not
 *               process any of the standard window messages.
 *
 *  API's      :  WinDefWindowProc
 *
 *  Parameters :  hwnd= window handle
 *                msg = the message i.d.
 *                mp1 = first message parameter
 *                mp2 = second message parameter
 *
 *  Return     :  dependent on message sent
 *
 *************************************************************************)
PROCEDURE [OS2.EXPENTRY] ObjectWndProc(hwnd : OS2.HWND
                                      ;msg  : OS2.ULONG
                                      ;mp1  : OS2.MPARAM
                                      ;mp2  : OS2.MPARAM
                                      ) : OS2.MRESULT;
BEGIN
   CASE msg OF
       (*
        *  Include any user-defined messages to determine which
        *  action the background thread should take.
        *)

   ELSE
      RETURN OS2.WinDefWindowProc(hwnd, msg, mp1, mp2);
   END;
   RETURN NIL;
END ObjectWndProc;

BEGIN
   fThreadCreated := FALSE;
END thrd.
