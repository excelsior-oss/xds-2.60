.*=============================================================================*
.*                                                                             *
.* Others.ipf - Information Tag Language file for the popup menus              *
.*              help panels.                                                   *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd.                                               *
.*                                                                             *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
.*                                                                             *
.*                                                                             *
.* Popup menu: IDM_POPUP_MSGLIST                                               *
.*   Font...                  (IDM_POP_FONT):        PANEL_IDM_POP_FONT        *
.*   Sort                     (IDM_POP_SORT):        PANEL_IDM_POP_SORT        *
.*   Default                  (IDM_POP_DEFAULT):     PANEL_IDM_POP_DEFAULT     *
.*   Unsorted                 (IDM_POP_UNSORTED):    PANEL_IDM_POP_UNSORTED    *
.*   Copy                     (IDM_POP_COPY):        PANEL_IDM_POP_COPY        *
.*                                                                             *
.* Popup menu: IDM_POPUP_SRCLIST                                               *
.*   Full names               (IDM_POP_FULLNAMES):   PANEL_IDM_POP_FULLNAMES   *
.*   Select...\tGray +        (IDM_POP_SELFILES):    PANEL_IDM_POP_SELFILES    *
.*   Unselect...\tGray -      (IDM_POP_UNSELFILES):  PANEL_IDM_POP_UNSELFILES  *
.*   Invert selection\tGray * (IDM_POP_INVSELFILES): PANEL_IDM_POP_INVSELFILES *
.*   File find\tShift+Ctrl+F  (IDM_POP_FILEFIND):    PANEL_IDM_POP_FILEFIND    *
.*   Open file                (IDM_POP_GROUPS):      PANEL_IDM_POP_GROUPS      *
.*     In Auto window         (IDM_POP_AGROUP):      PANEL_IDM_POP_AGROUP      *
.*     In 1 group             (IDM_POP_1GROUP):      PANEL_IDM_POP_1GROUP      *
.*     In 2 group             (IDM_POP_2GROUP):      PANEL_IDM_POP_2GROUP      *
.*     In 3 group             (IDM_POP_3GROUP):      PANEL_IDM_POP_3GROUP      *
.*     In 4 group             (IDM_POP_4GROUP):      PANEL_IDM_POP_4GROUP      *
.*     In 5 group             (IDM_POP_5GROUP):      PANEL_IDM_POP_5GROUP      *
.*     In 6 group             (IDM_POP_6GROUP):      PANEL_IDM_POP_6GROUP      *
.*     In 7 group             (IDM_POP_7GROUP):      PANEL_IDM_POP_7GROUP      *
.*     In 8 group             (IDM_POP_8GROUP):      PANEL_IDM_POP_8GROUP      *
.*     In 9 group             (IDM_POP_9GROUP):      PANEL_IDM_POP_9GROUP      *
.*                                                                             *
.* Popup menu: IDM_POPUP_POPUPWIN                                              *
.*   Font...                  (IDM_POP_FONT):        (defined before)          *
.*   Copy                     (IDM_POP_COPY):        (defined before)          *
.*                                                                             *
.* Popup menu:  IDM_POPUP_GOBM                                                 *
.*   Goto marker 0...         (IDM_POP_GOBM0..9):    PANEL_IDM_POP_GOBM        *
.*                                                                             *
.* Popup menu:  IDM_POPUP_SETBM                                                *
.*   Set marker 0...          (IDM_POP_SETBM0..9):   PANEL_IDM_POP_SETBM       *
.*                                                                             *
.* Popup menu:  IDM_POPUP_EDTITLEBAR                                           *
.*   Show full filenames      (IDM_POP_FULLNAMES):   (defined before)          *
.*                                                                             *
.* Popup menu:  IDM_POPUP_WINBAR                                               *
.*   Close                    (IDM_POP_WBBCLOSE):    PANEL_IDM_POP_WBBCLOSE    *
.*   Restore                  (IDM_POP_WBBRESTORE):  PANEL_IDM_POP_WBBRESTORE  *
.*   Maximize                 (IDM_POP_WBBMAXIMIZE): PANEL_IDM_POP_WBBMAXIMIZE *
.*   Hide                     (IDM_POP_WBBHIDE):     PANEL_IDM_POP_WBBHIDE     *
.*   Tile vertical            (IDM_POP_WBBVTILE):    PANEL_IDM_POP_WBBVTILE    *
.*   Tile horizontal          (IDM_POP_WBBHTILE):    PANEL_IDM_POP_WBBHTILE    *
.*   Move to group            (IDM_POP_MVGROUPS):    PANEL_IDM_POP_MVGROUPS    *
.*     1 group...             (IDM_POP_MV1..AGROUP): PANEL_IDM_POP_MV2GROUP    *
.*   Font...                  (IDM_POP_WBFONT):      PANEL_IDM_POP_WBFONT      *
.*   Show colors              (IDM_POP_WBCOLORS):    PANEL_IDM_POP_WBCOLORS    *
.*   Wrap winbar              (IDM_POP_WBWARP):      PANEL_IDM_POP_WBWRAP      *
.*                                                                             *
.* Popup menu:  IDM_POPUP_FILELIST                                             *
.*   Unselect all             (IDM_POP_UNSELALL):    PANEL_IDM_POP_UNSELALL    *
.*   Name \tCtrl+F3           (IDM_POP_NAME):        PANEL_IDM_POP_NAME        *
.*   Type \tCtrl+F4           (IDM_POP_TYPE):        PANEL_IDM_POP_TYPE        *
.*                                                                             *
.* Popup menu:  IDM_POPUP_GRPLIST  - in groups list mode                       *
.* Popup menu:  IDM_POPUP_GRPLIST1 - in source list mode                       *
.*   New file                 (IDM_POP_NEWFILE):     PANEL_IDM_POP_NEWFILE     *
.*   Specify new file         (IDM_POP_NEWFILE):     PANEL_IDM_POP_NEWFILE     *
.*   Close window \tDel       (IDM_POP_CLOSEWIN):    PANEL_IDM_POP_CLOSEWINDOW *
.*   Remove from list \tDel   (IDM_POP_CLOSEWIN):    PANEL_IDM_POP_CLOSEWINDOW *
.*                                                                             *
.* More windows dialog box:                          PANEL_MDILISTDLG          *
.* Select directory dialog box:                      PANEL_SELDIRDLG           *
.*                                                                             *
.*                                                                             *
.*=============================================================================*



.*--------------------------------------------------------------*\
.*  Popup menu: IDM_POPUP_MSGLIST                               *
.*--------------------------------------------------------------*/

.*---   "~Font..." (IDM_POP_FONT)
:h1 res=4000 name=PANEL_IDM_POP_FONT.Font
:p.Use the :hp2.Font:ehp2. command to change font used in the window.

.*---   "~Sort" (IDM_POP_SORT)
:h1 res=4005 name=PANEL_IDM_POP_SORT.Sort
:p.Use :hp2.Sort:ehp2. menu to change order of the list items.

.*---   "~Default" (IDM_POP_DEFAULT)
:h1 res=4010 name=PANEL_IDM_POP_DEFAULT.Default sort order
:p.Use :hp2.Default:ehp2. command to sort the list items in default order.

.*---   "~Unsorted" (IDM_POP_UNSORTED)
:h1 res=4015 name=PANEL_IDM_POP_UNSORTED.Unsorted
:p.Use :hp2.Unsorted:ehp2. command to turn sorting off.

.*---   "~Copy" (IDM_POP_COPY)
:h1 res=4020 name=PANEL_IDM_POP_COPY.Copy
:p.Use :hp2.Copy:ehp2. command to copy list contents into clipboard.


.*--------------------------------------------------------------*
.*  Popup menu IDM_POPUP_SRCLIST                                *
.*--------------------------------------------------------------*

.*---  "Full ~names" (IDM_POP_FULLNAMES)
.*
:h1 res=4025 name=PANEL_IDM_POP_FULLNAMES.Full names
:p.Select :hp2.Full names:ehp2. item to display full file names.

.*---  "Select...\tGray +" (IDM_POP_SELFILES)
.*
:h1 res=4030 name=PANEL_IDM_POP_SELFILES.Select files
:p.Use :hp2.Select:ehp2. command to select files by mask.

.*---  "Unselect...\tGray -" (IDM_POP_UNSELFILES)
.*
:h1 res=4035 name=PANEL_IDM_POP_UNSELFILES.Unselect files
:p.Use :hp2.Unselect:ehp2. command to unselect files by mask.

.*---  "Invert selection\tGray *" (IDM_POP_INVSELFILES)
.*
:h1 res=4040 name=PANEL_IDM_POP_INVSELFILES.Invert selection
:p.Use :hp2.Invert selection:ehp2. command to invert files selection.

.*---  "File find\tShift+Ctrl+F" (IDM_POP_FILEFIND)
.*
:h1 res=4045 name=PANEL_IDM_POP_FILEFIND.File find
:p.Use :hp2.File find:ehp2. command to enter File find dialog (PANEL_EDITFILEFIND).

.*---  "~Open file" (Submenu: IDM_POP_GROUPS)
.*
:h1 res=4050 name=PANEL_IDM_POP_GROUPS.Open file
:p.Use :hp2.Open file:ehp2. submenu to open file in editor window.

.*--- "In ~Auto window" (IDM_POP_AGROUP)
.*
:h1 res=4055 name=PANEL_IDM_POP_AGROUP.Open file
:p.Use :hp2.In Auto window:ehp2. choice to open file in auto window.

.*--- "In ~1 group" (IDM_POP_1GROUP)
.*
:h1 res=4060 name=PANEL_IDM_POP_1GROUP.Open file in 1 group
:p.Use :hp2.In 1 group:ehp2. choice to open file in the 1 editors group.

.*--- "In ~2 group" (IDM_POP_2GROUP)
.*
:h1 res=4065 name=PANEL_IDM_POP_2GROUP.Open file in 2 group
:p.Use :hp2.In 2 group:ehp2. choice to open file in the 2 editors group.

.*--- "In ~3 group" (IDM_POP_3GROUP)
.*
:h1 res=4070 name=PANEL_IDM_POP_3GROUP.Open file in 3 group
:p.Use :hp2.In 3 group:ehp2. choice to open file in the 3 editors group.

.*--- "In ~4 group" (IDM_POP_4GROUP)
.*
:h1 res=4075 name=PANEL_IDM_POP_4GROUP.Open file in 4 group
:p.Use :hp2.In 4 group:ehp2. choice to open file in the 4 editors group.

.*--- "In ~5 group" (IDM_POP_5GROUP)
.*
:h1 res=4080 name=PANEL_IDM_POP_5GROUP.Open file in 5 group
:p.Use :hp2.In 5 group:ehp2. choice to open file in the 5 editors group.

.*--- "In ~6 group" (IDM_POP_6GROUP)
.*
:h1 res=4085 name=PANEL_IDM_POP_6GROUP.Open file in 6 group
:p.Use :hp2.In 6 group:ehp2. choice to open file in the 6 editors group.

.*--- "In ~7 group" (IDM_POP_7GROUP)
.*
:h1 res=4090 name=PANEL_IDM_POP_7GROUP.Open file in 7 group
:p.Use :hp2.In 7 group:ehp2. choice to open file in the 7 editors group.

.*--- "In ~8 group" (IDM_POP_8GROUP)
.*
:h1 res=4095 name=PANEL_IDM_POP_8GROUP.Open file in 8 group
:p.Use :hp2.In 8 group:ehp2. choice to open file in the 8 editors group.

.*--- "In ~9 group" (IDM_POP_9GROUP)
.*
:h1 res=4100 name=PANEL_IDM_POP_9GROUP.Open file in 9 group
:p.Use :hp2.In 9 group:ehp2. choice to open file in the 9 editors group.


.*--------------------------------------------------------*
.* Popup menu:  IDM_POPUP_POPUPWIN                        *
.*--------------------------------------------------------*
.*--- "~Font..." (IDM_POP_FONT)
.*  defined before
.*--- "~Copy" (IDM_POP_COPY)
.*  defined before


.*--------------------------------------------------------*
.* Popup menu:  IDM_POPUP_GOBM                            *
.*--------------------------------------------------------*

.*--- "Goto marker ~0"... (IDM_POP_GOBM0 - IDM_POP_GOBM9)
.*
:h1 res=4105 name=PANEL_IDM_POP_GOBM.Goto marker
:p.Use :hp2.Goto marker:ehp2. command to set cursor at the marked position.

.*--------------------------------------------------------*
.* Popup menu:  IDM_POPUP_SETBM                           *
.*--------------------------------------------------------*

.*--- "Set marker ~0"... (IDM_POP_SETBM0 - IDM_POP_SETBM9)
.*
:h1 res=4110 name=PANEL_IDM_POP_SETBM.Set marker
:p.Use :hp2.Set marker:ehp2. command to mark cursor position.
You can use up to 10 markers for each text.

.*--------------------------------------------------------*
.* Popup menu:  IDM_POPUP_EDTITLEBAR                      *
.*--------------------------------------------------------*
.*--- "Show full filenames" (IDM_POP_FULLNAMES)
.*  defined before


.*--------------------------------------------------------*
.* Popup menu:  IDM_POPUP_WINBAR                          *
.*--------------------------------------------------------*

.*--- "Close" (IDM_POP_WBBCLOSE)
.*
:h1 res=4115 name=PANEL_IDM_POP_WBBCLOSE.Close
:p.Use :hp2.Close:ehp2. command to close editor window.

.*--- "Restore" (IDM_POP_WBBRESTORE)
.*
:h1 res=4120 name=PANEL_IDM_POP_WBBRESTORE.Restore
:p.Use :hp2.Restore:ehp2. command to restore editor window size.

.*--- "Maximize" (IDM_POP_WBBMAXIMIZE)
.*
:h1 res=4125 name=PANEL_IDM_POP_WBBMAXIMIZE.Maximize
:p.Use :hp2.Maximize:ehp2. command to maximize editor window size.

.*--- "Hide" (IDM_POP_WBBHIDE)
.*
:h1 res=4130 name=PANEL_IDM_POP_WBBHIDE.Hide
:p.Use :hp2.Hide:ehp2. command to hide editor window.

.*--- "Tile vertical" (IDM_POP_WBBVTILE)
.*
:h1 res=4135 name=PANEL_IDM_POP_WBBVTILE.Tile vertical
:p.Use :hp2.Tile vertical:ehp2. command to place the current window
and the selected window so that they occupy left and right half of the
Environment desktop respectively.

.*--- "Tile horizontal" (IDM_POP_WBBHTILE)
.*
:h1 res=4140 name=PANEL_IDM_POP_WBBHTILE.Tile horizontal
:p.Use :hp2.Tile horizontal:ehp2. command to to place the current window
and the selected window so that they occupy bottom and top half of the
Environment desktop respectively.

.*--- "Move to group" (Submenu: IDM_POP_MVGROUPS)
.*
:h1 res=4145 name=PANEL_IDM_POP_MVGROUPS.Move to group
:p.Use :hp2.Move to group:ehp2. submenu to move editor window into anotrer group.

.*--- "1 group"... (IDM_POP_MV1GROUP - IDM_POP_MVAGROUP)
.*
:h1 res=4150 name=PANEL_IDM_POP_MV2GROUP.Move to group
:p.Use :hp2.Move to group:ehp2. command to move editor window into anotrer group.

.*--- "Font..." (IDM_POP_WBFONT)
.*
:h1 res=4155 name=PANEL_IDM_POP_WBFONT.Font
:p.Use the :hp2.Font:ehp2. command to change winbar font.

.*--- "Show colors" (IDM_POP_WBCOLORS)
.*
:h1 res=4160 name=PANEL_IDM_POP_WBCOLORS.Colors
:p.Select :hp2.Colors:ehp2. to toggle displaying of WinBar buttons
in colors of respective windows.

.*--- "Wrap winbar" (IDM_POP_WBWARP)
.*
:h1 res=4165 name=PANEL_IDM_POP_WBWRAP.Wrap
:p.Select :hp2.Wrap:ehp2. to toggle WinBar wrapping.

.*--------------------------------------------------------*
.* Popup menu:  IDM_POPUP_FILELIST                        *
.*--------------------------------------------------------*

.*--- "Unselect all" (IDM_POP_UNSELALL)
.*
:h1 res=4170 name=PANEL_IDM_POP_UNSELALL.Unselect all
:p.Use :hp2.Unselect all:ehp2. command to remove selection from
all files currently selected in the file list.


.*--- "Font..." (IDM_POP_FONT)
.*  defined before
 .*---   "~Sort" (IDM_POP_SORT)
.*  defined before

.*--- "Name \tCtrl+F3" (IDM_POP_NAME)
.*
:h1 res=4175 name=PANEL_IDM_POP_NAME.Sort by name
:p.Use :hp2.Name:ehp2. command to sort files by name.

.*--- "Type \tCtrl+F4" (IDM_POP_TYPE)
.*
:h1 res=4180 name=PANEL_IDM_POP_TYPE.Sort by type
:p.Use :hp2.Type:ehp2. command to sort files by type.


.*--------------------------------------------------------*
.* Popup menu:  IDM_POPUP_GRPLIST   - in groups list mode *
.*--------------------------------------------------------*
.* Popup menu:  IDM_POPUP_GRPLIST1  - in source list mode *
.*--------------------------------------------------------*
.*

.*--- "New file"         (IDM_POP_NEWFILE)
.*--- "Specify new file" (IDM_POP_NEWFILE)
.*
:h1 res=4185 name=PANEL_IDM_POP_NEWFILE.New file
:p.Use :hp2.New file:ehp2. command to insert new file into the list.

.*--- "Close window \tDel"     (IDM_POP_CLOSEWIN)
.*--- "Remove from list \tDel" (IDM_POP_CLOSEWIN)
.* :-((( хреново. Нужно или удачно сформулировать, или разнести менюшки.
.*
:h1 res=4190 name=PANEL_IDM_POP_CLOSEWINDOW.Remove from list
:p.Use :hp2.Remove:ehp2. command to remove file from yhe list.

.*--- "Font..." (IDM_POP_FONT)
.*  defined before
.*--- "Show full filenames" (IDM_POP_FULLNAMES)
.*  defined before

.*--------------------------------------------------------------*\
.*  More windows dialog box help panel                          *
.*      res = PANEL_MDILISTDLG                                  *
.*--------------------------------------------------------------*/
:h1 res=3900 name=PANEL_MDILISTDLG.More windows dialog box
:i1.More windows dialog box
Listbox contains windows list
Ok
Cancel
Help


.*--------------------------------------------------------------*\
.*  Select directory dialog                                     *
.*      res = PANEL_SELDIRDLG                                   *
.*--------------------------------------------------------------*/
:h1 res=3905 name=PANEL_SELDIRDLG.Selest directory dialog box
:i1.Selest directory dialog box
Directory is... entry field - the directory name. You can type
it here.
Directories - listbox with directories & drive names. Doubleclick or
press enter
here.
Buttons: Set, Cancel.


