.*==============================================================*\
.*                                                              *
.* File.ipf - Information Tag Language file for the File menu   *
.*              help panels.                                    *
.*  Copyright (C) 1996 xTech Ltd. All rights reserved.          *
.*  Copyright 1991 IBM Corporation                              *
.*                                                              *
.*==============================================================*/

.*--------------------------------------------------------------*\
.*  Main file menu                                              *
.*      res = PANEL_FILE                                        *
.*--------------------------------------------------------------*/
:h1 res=2210 name=PANEL_FILE.File Menu
:i1 id=File.File Menu
:p.The File menu contains commands that you use to create, open,
and save data files. In addition, it also contains the
command that you use to exit the Sample application. The following
commands appear in the File menu:
:parml tsize=15 break=none.
:pt.New
:pd.Creates a new untitled file
:pt.Open
:pd.Opens an existing file
:pt.Save
:pd.Saves any changes to current file
:pt.Save As
:pd.Saves the current file using a new name
:pt.Exit
:pd.Quits the Sample application
:eparml.

.*--------------------------------------------------------------*\
.*  File menu New command help panel                            *
.*      res = PANEL_FILENEW                                     *
.*--------------------------------------------------------------*/
:h1 res=2220 name=PANEL_FILENEW.New
:i2 refid=File.New
:p. You can create a new file in the Sample application window by
using the New command. To create a new file, do the following:
:ul.
:li.Select the File menu and choose the New command.
:eul.
:p. The word "Untitled" appears in the title bar of the new file.


.*--------------------------------------------------------------*\
.*  File menu Open command help panel                           *
.*      res = PANEL_FILEOPEN                                    *
.*--------------------------------------------------------------*/
:h1 res=2230 name=PANEL_FILEOPEN.Open
:i2 refid=File.Open
:p.You can open a file that exists on any drive or in any directory
by using the Open command. To open a file, do the following:
:ul.
:li.Select the File menu and choose the Open command.
:eul.
:p.A dialog box appears, showing you a list of files in the current
directory.


.*--------------------------------------------------------------*\
.*  File menu Save command help panel                           *
.*      res = PANEL_FILESAVE                                    *
.*--------------------------------------------------------------*/
:h1 res=2240 name=PANEL_FILESAVE.Save
:i2 refid=File.Save
:p.You use the Save command to save changes you've made to your file.
To save changes to the current file, do the following:
:ul.
:li.Select the File menu and choose the Save command.
:eul.


.*--------------------------------------------------------------*\
.*  File menu Save As command help panel                        *
.*      res = PANEL_FILESAVEAS                                  *
.*--------------------------------------------------------------*/
:h1 res=2250 name=PANEL_FILESAVEAS.Save As
:i2 refid=File.Save As
:p.You use the Save As command to name and save a new file.
:p.Follow these steps to save a new file:
:ol.
:li.Select the File menu and choose the Save As command.
:eol.
:p.A dialog box appears, prompting you for a new filename.

.*--------------------------------------------------------------*\
.*  File menu Exit command help panel                           *
.*      res = PANEL_FILEEXIT                                    *
.*--------------------------------------------------------------*/
:h1 res=2290 name=PANEL_FILEEXIT.Exit
:i2 refid=File.Exit
:p.You quit the Sample application by using the Exit command. To
quit the Sample application, do the following:
:ol.
:li.Select the File menu and choose the Exit command.
:eol.

.***********************************************************************
.*Help for Open Dialog box   (D)
:h1 hide res=4163 scroll=none titlebar=yes group=1 id=A9003.Help for Open
.***********************************************************************
:link reftype=hd res=20500 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=20600 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=20500 nosearch noprint.Help for Open
:ol compact.
:li.Select the down arrow to the right of the :hp2.Drive:ehp2. list to display
all the drives on your system.
:li.Select a drive from the :hp2.Drive:ehp2. list.
:li.Select a directory from the :hp2.Directory:ehp2. list.
:li.Select a file name from the :hp2.File:ehp2. list or type in a file name
and select the :hp2.Open:ehp2. pushbutton to display
the file you want to edit.
:eol.
:p.For specific help, select a topic below.

:h1 hide res=20600 nosearch noprint.List of Fields
:dl compact tsize=20.
:dt.:link reftype=hd res=20501 group=20.File name:elink.
:dd.:link reftype=hd res=20502 group=21.Type of file:elink.
:dt.:link reftype=hd res=20503 group=22.Drive:elink.
:dd.:link reftype=hd res=20504 group=23.File:elink.
:dt.:link reftype=hd res=20505 group=24.Directory:elink.
:dd.:link reftype=hd res=20506 group=25.Open pushbutton:elink.
:edl.
.***********************************************************************

.*Help for File name  (F)

:h1 hide res=20501 scroll=none titlebar=yes group=26 nosearch noprint.Help for File name
:link reftype=hd res=20550 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=20650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=20550 nosearch.Help for File name
:p.Type the name of the file you want to open in the
:hp2.File name:ehp2. field
and select the :hp2.Open:ehp2. pushbutton.
:p.For more help, select a topic below.

:h1 hide res=20650 nosearch noprint.List of Fields
:dl compact tsize=20.
:dt.:link reftype=hd res=20501 group=27.File name:elink.
:dd.:link reftype=hd res=20502 group=28.Type of file:elink.
:dt.:link reftype=hd res=20503 group=29.Drive:elink.
:dd.:link reftype=hd res=20504 group=30.File:elink.
:dt.:link reftype=hd res=20505 group=31.Directory:elink.
:dd.:link reftype=hd res=20506 group=32.Open pushbutton:elink.
:dt.
:dd.:link reftype=hd res=4163  group=33.General help:elink.
:edl.
.************************************************************************

.*Help for Type of file  (F)

:h1 hide res=20502 scroll=none titlebar=yes group=38 nosearch noprint.Help for Type of file
:link reftype=hd res=20552 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=20650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=20552 group=39 nosearch.Help for Type of file
:p.Select the down arrow to the right of the :hp2.Type of file:ehp2. field to
display the available file types.  The sample has set this field for
all file types.
:p.For more help, select a topic below.
.***********************************************************************

.*Help for Drive  (F)

:h1 hide res=20503 scroll=none titlebar=yes group=11 nosearch noprint.Help for Drive
:link reftype=hd res=20553 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=20650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=20553 group=40 nosearch.Help for Drive
:p.The :hp2.Drive:ehp2. list displays the drives on your system.  Select the
drive that contains the files you want to edit.
:p.For more help, select a topic below.
.***************************************************************************

.*Help for File  (F)

:h1 hide res=20504 scroll=none titlebar=yes group=11 nosearch noprint.Help for File
:link reftype=hd res=20554 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=20650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=20554 group=42 nosearch.Help for File
:p.The :hp2.File:ehp2. list displays all the files in the directory
you selected from the :hp2.Directory:ehp2. list.  Select the file you want
to open.
:p.For more help, select a topic below.
.***************************************************************************

.*Help for Directory  (F)

:h1 hide res=20505 scroll=none titlebar=yes group=11 nosearch noprint.Help for Directory
:link reftype=hd res=20555 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=20650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=20555 group=43 nosearch.Help for Directory
:p.The :hp2.Directory:ehp2. list displays the directories on the selected
drive.  Select a directory to display the list of files from that directory
in the :hp2.File:ehp2. list box.
:p.For more help, select a topic below.
.*************************************************************************

.*Help for Open pushbutton  (PU)

:h1 hide res=20506 scroll=none titlebar=yes group=17 nosearch noprint.Help for Open
:link reftype=hd res=20556 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=20650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=20556 group=44 nosearch.Help for Open
:p.Select the :hp2.Open:ehp2. pushbutton to display
the file you want to edit.
:p.For more help, select a topic below.
.*************************************************************************

.*Help for Save (PD)
:h1 hide res=4192 id=A4192 nosearch.Help for Save
:p.Use :hp2.Save:ehp2. to store the file you are editing.  After the file
is saved, the text remains in the window so that you can continue editing it.
:nt.If you are editing a new file, select the :hp2.Save:ehp2. or :hp2.Save
as:ehp2. choices to display the :hp2.Save as:ehp2. pop-up so that you can name
the file you are editing.  A file must have a title to be saved.
.**************************************************************************


.*Help for Save As (PD)
:h1 hide res=4208 id=A4208 nosearch.Help for Save as
:p.Use :hp2.Save as:ehp2. to name and save a new file or to save an existing
file under a different name, in a different directory, or on a different disk.
.**************************************************************************


.*Help for Save As Dialog box (D)

:h1 hide res=4210 scroll=none titlebar=yes group=100 id=A9004.Help for Save as
:link reftype=hd res=26500 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=26600 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=26500 nosearch noprint.Help for Save as
:ol compact.
:li.Select the down arrow to the right of the :hp2.Drive:ehp2. list to display
all the drives on your system.
:li.Select a drive from the :hp2.Drive:ehp2. list.
:li.Select a directory from the :hp2.Directory:ehp2. list.
:li.Type the name of the file you want to save in the :hp2.File name:ehp2. field
and select the :hp2.Save:ehp2. pushbutton.
:eol.
:p.For specific help, select a topic below.

:h1 hide res=26600 nosearch noprint.List of Fields
:dl compact tsize=20.
:dt.:link reftype=hd res=26501 group=200.File name:elink.
:dd.:link reftype=hd res=26502 group=210.Type of file:elink.
:dt.:link reftype=hd res=26503 group=220.Drive:elink.
:dd.:link reftype=hd res=26504 group=230.File:elink.
:dt.:link reftype=hd res=26505 group=240.Directory:elink.
:dd.:link reftype=hd res=26506 group=250.Save pushbutton:elink.
:edl.
.***********************************************************************

.*Help for File name  (F)

:h1 hide res=26501 scroll=none titlebar=yes group=260 nosearch noprint.Help for File name
:link reftype=hd res=26550 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=26650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=26550 nosearch.Help for File name
:p.Type the name of the file you want to save in the
:hp2.File name:ehp2. field
and select the :hp2.Save:ehp2. pushbutton.
:p.For more help, select a topic below.

:h1 hide res=26650 nosearch noprint.List of Fields
:dl compact tsize=20.
:dt.:link reftype=hd res=26501 group=270.File name:elink.
:dd.:link reftype=hd res=26502 group=280.Type of file:elink.
:dt.:link reftype=hd res=26503 group=290.Drive:elink.
:dd.:link reftype=hd res=26504 group=300.File:elink.
:dt.:link reftype=hd res=26505 group=310.Directory:elink.
:dd.:link reftype=hd res=26506 group=320.Save pushbutton:elink.
:dt.:link reftype=hd res=4210  group=330.General help:elink.
:dd.
:edl.
.************************************************************************

.*Help for Type of file  (F)

:h1 hide res=26502 scroll=none titlebar=yes group=380 nosearch noprint.Help for Type of file
:link reftype=hd res=26552 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=26650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=26552 group=390 nosearch.Help for Type of file
:p.Select the down arrow to the right of the :hp2.Type of file:ehp2. field to
display the available file types.  The sample has set this field for
all file types.
:p.For more help, select a topic below.
.***********************************************************************

.*Help for Drive     (F)

:h1 hide res=26503 scroll=none titlebar=yes group=110 nosearch noprint.Help for Drive
:link reftype=hd res=26553 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=26650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=26553 group=400 nosearch.Help for Drive
:p.The :hp2.Drive:ehp2. list displays the drives on your system.  Select the
drive that contains the files you want to save.
:p.For more help, select a topic below.
.***************************************************************************

.*Help for File    (F)

:h1 hide res=26504 scroll=none titlebar=yes group=120 nosearch noprint.Help for File
:link reftype=hd res=26554 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=26650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=26554 group=420 nosearch.Help for File
:p.The :hp2.File:ehp2. list displays all the files in the directory
you selected from the :hp2.Directory:ehp2. list.  Select the file you want
to rename and save.
:p.For more help, select a topic below.
.***************************************************************************

.*Help for Directory    (F)


:h1 hide res=26505 scroll=none titlebar=yes group=11 nosearch noprint.Help for Directory
:link reftype=hd res=26555 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=26650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=26555 group=430 nosearch.Help for Directory
:p.The :hp2.Directory:ehp2. list displays the directories on the selected
drive.  Select a directory to display the list of files from that directory
in the :hp2.File:ehp2. list box.
:p.For more help, select a topic below.
.*************************************************************************

.*Help for Save pushbutton  (PU)

:h1 hide res=26506 scroll=none titlebar=yes group=170 nosearch noprint.Help for Save
:link reftype=hd res=26556 vpx=left vpy=top vpcx=100% vpcy=70% scroll=vertical rules=border titlebar=none child auto.
:link reftype=hd res=26650 vpx=left vpy=bottom vpcx=100% vpcy=30% scroll=vertical rules=none titlebar=none child auto.

:h1 hide res=26556 group=440 nosearch.Help for Save
:p.Select the :hp2.Save:ehp2. pushbutton to save the
file to the drive and directory you selected and with the file name
you specified.
:p.For more help, select a topic below.
.*************************************************************************
