.*==============================================================*\
.*                                                              *
.* Edit.ipf - Information Tag Language file for the Edit menu   *
.*              help panels.                                    *
.*  Copyright (C) 1996 xTech Ltd. All rights reserved.          *
.*  Copyright 1991 IBM Corporation                              *
.*                                                              *
.*==============================================================*/

.*--------------------------------------------------------------*\
.*  Main Edit menu                                              *
.*      res = PANEL_EDIT                                        *
.*--------------------------------------------------------------*/
:h1 res=2310 name=PANEL_EDIT.Edit Menu
:i1 id=Edit.Edit Menu
:p.Enter text for the main Edit Menu help screen here.


.*--------------------------------------------------------------*\
.*  Edit menu Undo command help panel                           *
.*      res = PANEL_EDITUNDO                                    *
.*--------------------------------------------------------------*/
:h1 res=2320 name=PANEL_EDITUNDO.Undo
:i2 refid=Edit.Undo
:p. Place information for the Edit Undo menu item here.


.*--------------------------------------------------------------*\
.*  Edit menu Cut command help panel                            *
.*      res = PANEL_EDITCUT                                     *
.*--------------------------------------------------------------*/
:h1 res=2330 name=PANEL_EDITCUT.Cut
:i2 refid=Edit.Cut
:p.Place information for the Edit Cut menu item here.


.*--------------------------------------------------------------*\
.*  Edit menu Copy command help panel                           *
.*      res = PANEL_EDITCOPY                                    *
.*--------------------------------------------------------------*/
:h1 res=2340 name=PANEL_EDITCOPY.Copy
:i2 refid=Edit.Copy
:p.Place information for the Edit Copy menu item here.


.*--------------------------------------------------------------*\
.*  Edit menu Paste command help panel                          *
.*      res = PANEL_EDITPASTE                                   *
.*--------------------------------------------------------------*/
:h1 res=2350 name=PANEL_EDITPASTE.Paste
:i2 refid=Edit.Paste
:p.Place information for the Edit Paste menu item here.


.*--------------------------------------------------------------*\
.*  Edit menu Clear command help panel                          *
.*      res = PANEL_EDITCLEAR                                   *
.*--------------------------------------------------------------*/
:h1 res=2360 name=PANEL_EDITCLEAR.Clear
:i2 refid=Edit.Clear
:p.Place information for the Edit Clear menu item here.
