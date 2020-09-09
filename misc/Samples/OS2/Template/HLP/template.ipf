.*==============================================================*\
.*                                                              *
.* Main.ipf - Information Tag Language file for the main        *
.*              sample application's help manager               *
.*  Copyright (C) 1996 xTech Ltd. All rights reserved.          *
.*  Copyright 1991 IBM Corporation                              *
.*                                                              *
.*==============================================================*/
:userdoc.

.*--------------------------------------------------------------*\
.*  Main window extended help panel                             *
.*      res = PANEL_MAIN                                        *
.*--------------------------------------------------------------*/
:h1 res=2100 name=PANEL_MAIN.About Template
:i1 id=aboutMain.About Template
:p.The Template Sample application is a sample Presentation Manager application
which an application developer can use as a base for his own applications.
The sample program uses many of the standard menus and dialogs that
most applications would use.  The source is designed to serve as a
base for any Presentation Manager application and was written in such
a way that it can be easily modified to handle any application specific
commands.

.*-- Import the File menu help file --*
.im hlp\file.ipf

.*-- Import the Edit menu help file --*
.im hlp\edit.ipf

.*-- Import the Help menu help file --*
.im hlp\help.ipf

.*-- Import the Application Menu help file --*
.im hlp\menu.ipf

.*-- Import the Dialog boxes help file --*
.im hlp\dlg.ipf

:euserdoc.
