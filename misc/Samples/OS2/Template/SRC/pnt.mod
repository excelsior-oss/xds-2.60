(**************************************************************************
 *  File name  :  pnt.mod
 *
 *  Description:  This module contains the code for the main client
 *                window painting.
 *
 *                This module contains the following procedures:
 *
 *                MainPaint(hwnd)
 *
 *  Concepts   :  window painting
 *
 *  API's      :  WinBeginPaint
 *                WinFillRect
 *                WinEndPaint
 *
 *  Required
 *    Files    :  OS2.H, MAIN.H, XTRN.H
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

IMPLEMENTATION MODULE pnt;

IMPORT OS2, OS2RTL;

(**************************************************************************
 *
 *  Name       : MainPaint(hwnd)
 *
 *  Description: Paints the main client window.
 *
 *  Concepts:  Routine is called whenever the client window
 *             procedure receives a WM_PAINT message
 *
 *             - begins painting by calling WinBeginPaint
 *                 and retrieving the HPS for the window
 *             - performs any painting desired
 *             - ends painting by calling WinEndPaint
 *
 *  API's      :  WinBeginPaint
 *                WinFillRect
 *                WinEndPaint
 *
 *  Parameters :  hwnd     = window handle
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE MainPaint(hwnd : OS2.HWND);
VAR
   rclUpdate : OS2.RECTL;
   hps       : OS2.HPS;
BEGIN
   hps := OS2.WinBeginPaint(hwnd, OS2.NULLHANDLE, rclUpdate);

   (* fill update rectangle with window color *)
   OS2.WinFillRect(hps, rclUpdate, OS2.SYSCLR_WINDOW);

   (*
    *      Add painting routines here.  Hps is now
    *      the HPS for the window and rclUpdate
    *      contains the update rectangle.
    *)

   OS2.WinEndPaint(hps);
END MainPaint;

END pnt.

