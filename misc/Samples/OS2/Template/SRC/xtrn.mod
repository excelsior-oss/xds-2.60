(**************************************************************************
 *  File name  :  xtrn.def
 *
 *  Description:  This module contains the application-wide constants,
 *                variables, and general purpose procedures.
 *
 *                This module contains the following procedures:
 *
 *                MessageBox(hwndOwner, idMsg, fsStyle, fBeep)
 *
 *  Concepts   :  global definition
 *                message boxes
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

IMPLEMENTATION MODULE xtrn;

IMPORT OS2;

IMPORT mainres;

(**************************************************************************
 *
 *  Name       : MessageBox(hwndOwner, nIdMsg, fsStyle, fBeep)
 *
 *  Description:  Displays the message box with the message
 *                given in idMsg retrieved from the message table
 *                and using the style flags in fsStyle.
 *
 *  Concepts:     Called whenever a message box is to be displayed
 *
 *                - Message string is loaded from the process'
 *                    message table
 *                - Alarm beep is sounded if desired
 *                - Message box with the message is displayed
 *                - WinMessageBox return value is returned
 *
 *  API's      :  WinLoadMessage
 *                WinAlarm
 *                WinMessageBox
 *
 *  Parameters :  hwndOwner = window handle of the owner
 *                nIdMsg    = message i.d.
 *                fsStyle   = style flags for the message box
 *                fBeep     = should an alarm be sounded?
 *
 *  Return     :  return value from WinMessageBox
 *
 *************************************************************************)
PROCEDURE [OS2.APIENTRY] MessageBox(hwndOwner : OS2.HWND
                                   ;idMsg     : OS2.ULONG
                                   ;fsStyle   : OS2.ULONG
                                   ;fBeep     : OS2.BOOL
                                   ): OS2.ULONG;
VAR
   szText : ARRAY [0..MESSAGELEN-1] OF CHAR;

BEGIN
   IF OS2.WinLoadMessage(hab
                        ,0
                        ,idMsg
                        ,MESSAGELEN
                        ,szText) = 0 THEN
      OS2.WinAlarm(OS2.HWND_DESKTOP, OS2.WA_ERROR);
      RETURN OS2.MBID_ERROR;
   END;

   IF fBeep THEN
      OS2.WinAlarm(OS2.HWND_DESKTOP, OS2.WA_ERROR);
   END;

   RETURN OS2.WinMessageBox(OS2.HWND_DESKTOP,
                            hwndOwner,
                            szText,
                            NIL,
                            mainres.MSGBOXID,
                            fsStyle);
END MessageBox;

BEGIN
  hwndMainFrame := OS2.NULLHANDLE;
END xtrn.
