(**************************************************************************
 *  File name  :  edit.mod
 *
 *  Description:  This module reserves paths for code for the WM_COMMAND
 *                messages posted by the standard edit menu.
 *
 *                This source file contains the following procedures:
 *
 *                EditUndo(mp2)
 *                EditCut(mp2)
 *                EditCopy(mp2)
 *                EditPaste(mp2)
 *                EditClear(mp2)
 *
 *  Concepts   :  clipboard
 *
 *  API's      :  [none]
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

IMPLEMENTATION MODULE edit;

IMPORT OS2, OS2RTL;

(**************************************************************************
 *
 *  Name       : EditUndo(mp2)
 *
 *  Description: Processes selection of the Undo choice of the Edit
 *               pulldown
 *
 *  Concepts:  called whenever Undo from the Edit menu is selected
 *
 *  API's      :  [none]
 *
 *  Parameters :  mp2      = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE EditUndo(mp2 : OS2.MPARAM);
BEGIN
END EditUndo;


(**************************************************************************
 *
 *  Name       : EditCut(mp2)
 *
 *  Description: Processes selection of the Cut choice of the Edit
 *               pulldown
 *
 *  Concepts:  called whenever Cut from the Edit menu is selected
 *
 *  API's      :  [none]
 *
 *  Parameters :  mp2      = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE EditCut(mp2 : OS2.MPARAM);
BEGIN
END EditCut;


(**************************************************************************
 *
 *  Name       : EditCopy(mp2)
 *
 *  Description: Processes selection of the Copy choice of the Edit
 *               pulldown
 *
 *  Concepts:  called whenever Copy from the Edit menu is selected
 *
 *  API's      :  [none]
 *
 *  Parameters :  mp2      = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE EditCopy(mp2 : OS2.MPARAM);
BEGIN
END EditCopy;


(**************************************************************************
 *
 *  Name       : EditPaste(mp2)
 *
 *  Description: Processes selection of the Paste choice of the Edit
 *               pulldown
 *
 *  Concepts:  called whenever Paste from the Edit menu is selected
 *
 *  API's      :  [none]
 *
 *  Parameters :  mp2      = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE EditPaste(mp2 : OS2.MPARAM);
BEGIN
END EditPaste;


(**************************************************************************
 *
 *  Name       : EditClear(mp2)
 *
 *  Description: Processes selection of the Clear choice of the Edit
 *               pulldown
 *
 *  Concepts:  called whenever Clear from the Edit menu is selected
 *
 *  API's      :  [none]
 *
 *  Parameters :  mp2      = second message parameter
 *
 *  Return     :  [none]
 *
 *************************************************************************)
PROCEDURE EditClear(mp2 : OS2.MPARAM);
BEGIN
END EditClear;

END edit.
