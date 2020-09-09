.*==============================================================*\
.*                                                              *
.* Windows.ipf - Information Tag Language file for the screens  *
.*              for help on Window menu items.                  *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd.                                *
.*                                                              *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
.*                                                              *
.* Helps for WINDOW menu and its items:                         *
.*  PANEL_WINDOW                                                *
.*    PANEL_WINMSGLIST                                          *
.*    PANEL_WINSRCLIST                                          *
.*    PANEL_WINRECALLPOPUP                                      *
.*    PANEL_WINREXXMON                                          *
.*    PANEL_WINCASCADE                                          *
.*    PANEL_WINMAXALL                                           *
.*    PANEL_WINCLOSEALL                                         *
.*                                                              *
.*==============================================================*


.*--------------------------------------------------------------*\
.*  Main Windows menu                                           *
.*      res = PANEL_WINDOW                                      *
.*--------------------------------------------------------------*/
:h1 res=3200 name=PANEL_WINDOW.Window Menu
:i1 id=Windows.Window Menu
:p.The :hp2.Window Menu:ehp2. contains commands that you use to select
a window and manage the windows layout. A list of all open windows
is provided.
:parml compact tsize=3 break=all.
:pt.:hp2.Message list:ehp2.
:pd.Displays the Message list window
:pt.:hp2.Source files:ehp2.
:pd.Displays the Source files window
:pt.:hp2.Recall popup window:ehp2.
:pd.Displays the Popup window
:pt.:hp2.REXX execution monitor:ehp2.
:pd.Displays REXX execution monitor window
:pt.:hp2.Cascade:ehp2.
:pd.Arranges edit windows in cascade style
:pt.:hp2.Close all:ehp2.       // А может убрать ее нафиг?
:pd.Closes all editor windows  // Да и не работает оно - или сделать?
:eparml.


.*--------------------------------------------------------------*\
.*  Windows menu Messages list command help panel               *
.*      res = PANEL_WINMSGLIST                                  *
.*--------------------------------------------------------------*/
:h1 res=3205 name=PANEL_WINMSGLIST.Messages list
:i2 refid=Windows.Messages list
:p.Use the :hp2.Messages list:ehp2. command to display the
Messages list window.


.*--------------------------------------------------------------*\
.*  Windows menu Source files command help panel                *
.*      res = PANEL_WINSRCLIST                                  *
.*--------------------------------------------------------------*/
:h1 res=3210 name=PANEL_WINSRCLIST.Source files
:i2 refid=Windows.Source files
:p.Use the :hp2.Source files:ehp2. command to display the
Source files window.


.*--------------------------------------------------------------*\
.*  Windows menu Source files command help panel                *
.*      res = PANEL_WINRECALLPOPUP                              *
.*--------------------------------------------------------------*/
:h1 res=3215 name=PANEL_WINRECALLPOPUP.Recall popup window
:i2 refid=Windows.recall popup window
Displays the Popup window


.*--------------------------------------------------------------*\
.*  Windows menu Source files command help panel                *
.*      res = PANEL_WINREXXMON                                  *
.*--------------------------------------------------------------*/
:h1 res=3220 name=PANEL_WINREXXMON.REXX execution monitor
:i2 refid=Windows.REXX execution monitor
Displays REXX execution monitor window

.*--------------------------------------------------------------*\
.*  Windows menu Source files command help panel                *
.*      res = PANEL_WINCASCADE                                  *
.*--------------------------------------------------------------*/
:h1 res=3225 name=PANEL_WINCASCADE.Cascade windows
Arranges edit windows in cascade style


.*--------------------------------------------------------------*\
.*  Windows menu Source files command help panel                *
.*      res = PANEL_WINNAXALL                                   *
.*--------------------------------------------------------------*/
:h1 res=3230 name=PANEL_WINMAXALL.Maximize all editor windows
maximizes all editor windows


.*--------------------------------------------------------------*\
.*  Windows menu Source files command help panel                *
.*      res = PANEL_WINCLOSEALL                                 *
.*--------------------------------------------------------------*/
:h1 res=3235 name=PANEL_WINCLOSEALL.Close all editor windows
Closes all editor windows



