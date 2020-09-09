.*==============================================================*
.*                                                              *
.* File.ipf - Information Tag Language file for the File menu   *
.*              help panels.                                    *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd.                                *
.*                                                              *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
.*                                                              *
.* Helps for FILE menu and its items:                           *
.* PANEL_FILE                                                   *
.*     PANEL_FILENEW                                            *
.*     PANEL_FILELOAD                                           *
.*     PANEL_FILERELOAD                                         *
.*     PANEL_FILESAVE                                           *
.*     PANEL_FILESAVEAS                                         *
.*     PANEL_FILESAVEALL                                        *
.*     PANEL_FILEPRINT                                          *
.*     PANEL_FILEAUTOSAVE                                       *
.*     PANEL_FILEMAXNEW                                         *
.*     PANEL_FILEBAK                                            *
.*     PANEL_FILEUSE1AUTOWIN                                    *
.*                                                              *
.* Helps for appropriate dialogs:                               *
.*                                                              *
.*   PANEL_FASTLOADDLG                                          *
.*   PANEL_FILEDLG                                              *
.*   PANEL_SAVEASDLG                                            *
.*   PANEL_SAVECHDLG                                            *
.*                                                              *
.*==============================================================*


.*--------------------------------------------------------------*\
.*  Main file menu                                              *
.*      res = PANEL_FILE                                        *
.*--------------------------------------------------------------*/
:h1 res=2200 name=PANEL_FILE.File Menu
:i1 id=File.File Menu
:p.The :hp2.File:ehp2. menu contains commands that you use to create,
open, and save source files. It also contains a list of files (up to 15)
that were most recently opened or saved using the :hp2.Save As:ehp2. command.
The following commands appear in the :hp2.File:ehp2. menu:
:parml compact tsize=3 break=all.
:pt.:hp2.New:ehp2.
:pd.Opens new empty window
:pt.:hp2.Load:ehp2.
:pd.Loads a file into new or existing window
:pt.:hp2.Reload:ehp2.
:pd.Reloads current file from disk
:pt.:hp2.Save:ehp2.
:pd.Saves any changes made to the current file
:pt.:hp2.Save As:ehp2.
:pd.Saves the current file using a new name
:pt.:hp2.Save All:ehp2.
:pd.Saves all changed files
:pt.:hp2.Print:ehp2.
:pd.Prints the cureent file or selection
:pt.:hp2.Autosave:ehp2.
:pd.Toggles autosave mode on or off
:pt.:hp2.Maximize new windows:ehp2.
:pd.Toggles new windows maximizing on or off
:pt.:hp2.Create backup files:ehp2.
:pd.Toggles create .BAK files mode on or off
:pt.:hp2.Use single 'Auto window':ehp2.
:pd.Toggles creation of new autowindows
:eparml.


.*--------------------------------------------------------------*\
.*  File menu New command help panel                            *
.*      res = PANEL_FILENEW                                     *
.*--------------------------------------------------------------*/
:h1 res=2205 name=PANEL_FILENEW.New
:i2 refid=File.New
:p.Use the :hp2.New:ehp2. command to create a new empty window.
:p.The word "Noname:hp1.x:ehp1." will appear in its title bar,
where :hp1.x:ehp1. is an automatically generated number. You'll
have to specify a name for the new file when you save it.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\filenew.bmp'.

.*--------------------------------------------------------------*\
.*  File menu Load command help panel                           *
.*      res = PANEL_FILELOAD                                    *
.*--------------------------------------------------------------*/
:h1 res=2210 name=PANEL_FILELOAD.Load
:i2 refid=File.Load
:p.Use the :hp2.Load:ehp2. command to open an existing file.
:p.A dialog will appear, providing different methods of
loading files.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\fileopen.bmp'.
:p.Default keyboard shortcut: :hp2.F3:ehp2.

.*--------------------------------------------------------------*\
.*  File menu Load command help panel                           *
.*      res = PANEL_FILERELOAD                                  *
.*--------------------------------------------------------------*/
:h1 res=2215 name=PANEL_FILERELOAD.Reload
:i2 refid=File.Reload
:p.Use the :hp2.Reload:ehp2. command to reload the file in the
current window from disk, discarding all changes made to it since
last save. You will be prompted to save your changes, if any.
:p.Default keyboard shortcut: :hp2.Ctrl+F3:ehp2.


.*--------------------------------------------------------------*\
.*  File menu Save command help panel                           *
.*      res = PANEL_FILESAVE                                    *
.*--------------------------------------------------------------*/
:h1 res=2220 name=PANEL_FILESAVE.Save
:i2 refid=File.Save
:p.Use :hp2.Save:ehp2. to store the current file.  After the file
is saved, the text remains in the window so that you can continue
editing it.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\filesave.bmp'.
:p.Default keyboard shortcut: :hp2.F2:ehp2.
:nt.If you are editing a new (Noname:hp1.x:ehp1.) file, select the
:hp2.Save:ehp2. or :hp2.Save as:ehp2. choices to display the
:hp2.Save as:ehp2. dialog so that you can name the file you are editing.
A file must have a name to be saved.


.*--------------------------------------------------------------*\
.*  File menu Save As command help panel                        *
.*      res = PANEL_FILESAVEAS                                  *
.*--------------------------------------------------------------*/
:h1 res=2225 name=PANEL_FILESAVEAS.Save As
:i2 refid=File.Save As
:p.Use the :hp2.Save As:ehp2. command to name and save a new file
or to save a file under a different name, or to a different location.
:p.A dialog box will appear, prompting you for a new filename. You can
also select a different drive/directory.


.*--------------------------------------------------------------*\
.*  File menu Save All command help panel                       *
.*      res = PANEL_FILESAVEALL                                 *
.*--------------------------------------------------------------*/
:h1 res=2230 name=PANEL_FILESAVEALL.Save All
:i2 refid=File.Save All
:p.Use the :hp2.Save All:ehp2. command to save all changed files.
:p.Default keyboard shortcut: :hp2.Ctrl+F2:ehp2.


.*--------------------------------------------------------------*\
.*  File menu Print command help panel                          *
.*      res = PANEL_FILEPRINT                                   *
.*--------------------------------------------------------------*/
:h1 res=2235 name=PANEL_FILEPRINT.Print
:i2 refid=File.Print
:p.Use the :hp2.Print:ehp2. command to print the current file or
selection.


.*--------------------------------------------------------------*\
.*  File menu Autosave command help panel                       *
.*      res = PANEL_FILEAUTOSAVE                                *
.*--------------------------------------------------------------*/
:h1 res=2240 name=PANEL_FILEAUTOSAVE.Autosave
:i2 refid=File.Autosave
:p.Use :hp2.Autosave:ehp2. to toggle the autosave mode on or off.
In the autosave mode XDS automatically saves all changed files before
exiting or starting an external tool, such as a compiler. When the
mode is off, you will be prompted to save your changes.


.*--------------------------------------------------------------*\
.*  File menu Maximize new windows command help panel           *
.*      res = PANEL_FILEMAXNEW                                  *
.*--------------------------------------------------------------*/
:h1 res=2245 name=PANEL_FILEMAXNEW.Maximize new windows
:i2 refid=File.Maximize new windows
:p.Use :hp2.Maximize new windows:ehp2. to toggle automatic
maximization of newly opened windows on or off.


.*--------------------------------------------------------------*\
.*  File menu Maximize new windows command help panel           *
.*      res = PANEL_FILEBAK                                     *
.*--------------------------------------------------------------*/
:h1 res=2250 name=PANEL_FILEBAK.Create backup files
:i2 refid=File.Create backup files
:p.Use :hp2.Create backup files:ehp2. to toggle automatic
creation of backup files.


.*--------------------------------------------------------------*\
.*  File menu Use single 'Auto window' command help panel       *
.*      res = PANEL_FILEUSE1AUTOWIN                             *
.*--------------------------------------------------------------*/
:h1 res=2255 name=PANEL_FILEUSE1AUTOWIN.Use single 'Auto window'
:i2 refid=File.Use single 'Auto window'
:p.Use :hp2.Use single 'Auto window':ehp2. to toggle
multiple/single autowindow modes. See
:link reftype=hd refid=GROUPS.Window Groups:elink.
for more information.


.************************ D i a l o g   b o x e s : *************************.

.*--------------------------------------------------------------*\
.*  Load file dialog box help panel                             *
.*      res = PANEL_FASTLOADDLG                                 *
.*--------------------------------------------------------------*/
:h1 res=2265 name=PANEL_FASTLOADDLG.Load file dialog box
:i1.Load file dialog
:p.To open a file:
:ol compact.
:li.Type the file name in the :hp2.File:ehp2. field or select
it using the dropdown list. You may also select the :hp2.Browse:ehp2.
pushbutton to display the standard file dialog.
:li.Choose a window group from the :hp2.Window:ehp2. dropdown list.
:li.Select the :hp2.Ok:ehp2. pushbutton.
:eol.
:p.The :hp2.Advanced:ehp2. pushbutton displays the Advanced File Load Diaog (PANEL_FILEDLG)
.* !!! Ref
:p.Detailed description of controls:
:dl break=all tsize=3.
:dt.:hp2.Path:ehp2.
:dd.Displays the path that will be used if you specify a relative pathname.
:dt.:hp2.File:ehp2.
:dd.Type the name of the file you want to open in the :hp2.File:ehp2.
field and select the :hp2.OK:ehp2. pushbutton. You may also type absolute or
relative pathname.
:dt.:hp2.Window:ehp2.
:dd.Select a window group in which you want the file to be opened.
:dt.:hp2.Browse:ehp2.
:dd.Select the :hp2.Browse:ehp2. pushbutton to display the standard file
dialog and use it to choose a file. The file name will be placed into the
:hp2.File:ehp2. field.
:dt.:hp2.Advanced:ehp2.
:dd.Select the :hp2.Advanced:ehp2. pushbutton to display the
:link reftype=hd refid=PANEL_FILEDLG.Advanced file dialog:elink.
:dt.:hp2.Ok:ehp2.
:dd.Select the :hp2.Ok:ehp2. pushbutton to open the file.
:dt.:hp2.Cancel:ehp2.
:dd.Select the :hp2.Cancel:ehp2. pushbutton if you decide not to load a file.
:edl.


.*--------------------------------------------------------------*\
.*  Advanced file dilalog box help panel                        *
.*      res = PANEL_FILEDLG                                     *
.*--------------------------------------------------------------*/
:h1 res=2270 name=PANEL_FILEDLG.Advanced file dilalog box
:i1.Advanced file dilalog box
:p.This dialog box provides editor window and source file list
management functions.
:note.All changes you make in this dialog are committed when you
select the :hp2.Ok:ehp2. pushbutton. Select the :hp2.Cancel:ehp2.
pushbutton to abandon your actions.
:note.It is not possible to control the automatically generated file
list.
.* !!! ref.
:p.The dialog contains two panes. The left pane is used to display
the list of window groups
.* !!! ref.
or the source files list.
.* !!! ref.
The right pane contains a Norton Commander-like view
or a directory.

:sl compact.
:li.:link  reftype=hd refid=PANEL_FILEDLG_NEW.To create a new file...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_OPEN.To open files in a certain window group...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_SAVEAS.To save a file under a new name...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_CLOSE.To close a file...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_MOVE.To move an opened file from one window group to another...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_FONT.To select a different font for the group/list pane...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_FULLNAMES.To toggle full filenames mode...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_LEFTMODE.To switch between group/list panes...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_ADD.To add files to list...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_REMOVE.To remove a file from list...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_SELECT.To select files...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_PATH.To change drive and/or directory...:elink.
:li.:link  reftype=hd refid=PANEL_FILEDLG_SORT.To toggle sort mode...:elink.
:esl.

:h2 name=PANEL_FILEDLG_NEW.Creating a new file
:p.To create a new file in a certain group&colon.
:ol compact.
:li.Point to the group header or to any file already belonging to that group
in the ??? pane.
:li.Click mouse button 2.
:li.Select :hp2.New file:ehp2. from the pop-up menu.
:eol.
:p.An empty file named "Noname:hp1.x:ehp1.",
where :hp1.x:ehp1. is an automatically generated number,
will be added to the group. You will
have to specify a name for the new file when you save it.

:h2 name=PANEL_FILEDLG_OPEN.Opening files
:p.To open one or more files in a certain window group,
:link reftype=hd refid=PANEL_FILEDLG_SELECT.select:elink.
the file(s) in the file pane and do one of the following&colon.
:ul compact.
:li.Press :hp2.Alt+:ehp2.:hp3.group number:ehp3.
:li.Drag the selected file(s) to the group pane and
drop onto the desired group header or onto any file already
belonging to that group.
:eul.

:h2 name=PANEL_FILEDLG_SAVEAS.Saving a file under a new name
:p.To save an opened file under a new name, drag it from
the group pane to the file pane. The
:link reftype=hd refid=PANEL_SAVEASDLG.Save As:elink.
dialog will be displayed.

:h2 name=PANEL_FILEDLG_CLOSE.Closing a file
:p.To close an opened file&colon.
:ol compact.
:li.Point to the file in the group pane.
:li.Click mouse button 2.
:li.Select :hp2.Close window:ehp2. from the pop-up menu.
:eol.

:h2 name=PANEL_FILEDLG_MOVE.Moving files between groups
:p.To move an opened file from one window group to another,
drag it and drop inside the desired group.

:h2 name=PANEL_FILEDLG_FONT.Selecting font
:p.To select font to be used in a pane, click  mouse
button 2 inside that pane and select :hp2:Font:ehp2:
from the pop-up menu. The standard font dialog will be displayed.

:h2 name=PANEL_FILEDLG_FULLNAMES.Toggling full filenames mode
:p.To toggle display of full file names in the group pane,
click  mouse button 2 inside that pane and select
:hp2:Show full filenames:ehp2: from the pop-up menu.

:h2 name=PANEL_FILEDLG_LEFTMODE.Left pane mode
:p.To display the list of opened files in the left pane,
select the :hp2.Editors:ehp2. !!! pushbutton.
To display the list of source files for the !!! ref
current workplace in that pane,
select the :hp2.Source files list:ehp2. !!! pushbutton.

:h2 name=PANEL_FILEDLG_ADD.Adding files to list
:p.To add one or more files from the file pane
to the source file list&colon.
:ul compact.
:li.:link reftype=hd refid=PANEL_FILEDLG_SELECT.Select:elink.
them in the file pane.
:li.Drag the selected file(s) to the list pane.
:eul.
:p.To add a file by specifying its full name, click
mouse button 2 anywhere inside the list and select
:hp2:Add file:ehp2: from the pop-up menu. !!! correct menu, open std dialog

:h2 name=PANEL_FILEDLG_REMOVE.Removing a file from list
:p.To remove a file from the source file list&colon.
:ol compact.
:li.Point to the file in the list pane.
:li.Click mouse button 2.
:li.Select :hp2.Remove from list:ehp2. from the pop-up menu.
:eol.

:h2 name=PANEL_FILEDLG_SELECT.Selecting files in the file pane
:p.One line in the files pane contains a cursor. The file under the
cursor is considered selected unless there is a multiple selection (see below).
:p.To move the cursor, press the :hp2.Up/Down:ehp2: or :hp2.Ctrl-PgUp/Ctrl-PgDn:ehp2:
keys.
:p.To position the cursor on a particular file, click that file
with mouse button 1 or start typing the name of that file on the
keyboard.
:p.Multiple files can be selected using the standard mouse
technique with Ctrl-clicks and Shift-clicks, or
with the keyboard by pressing the :hp2.Ins:ehp2. key.
:p.To quickly remove selection, click mouse button 2
anywhere inside the file pane and select
:hp2:Unselect all:ehp2: from the pop-up menu.

:h2 name=PANEL_FILEDLG_PATH.Changing drive and directory
:p.To display files on another drive, select that drive
from the :hp2.Path:ehp2. combobox.
:p,To display another directory, do one of the following&colon.
:ul compact.
:li.if the name of that directory is displayed in the file pane,
double-click it or :li.:link reftype=hd refid=PANEL_FILEDLG_SELECT.Select:elink.
it and press :hp2.Enter:ehp2..
:li.type the full pathname of that directory in the :hp2.Path:ehp2.
combobox and select the :hp2.Go:ehp2 pushbutton.
:eul.
You may also press :hp2.Ctrl-PgUp:ehp2. to display the parent
directory.

:h2 name=PANEL_FILEDLG_SORT.Choosing sort mode
:p.To sort files in the file pane by name, press :hp2.Ctrl+F3:ehp2.
or click mouse button 2 anywhere inside the file pane and select
:hp2:Sort/By name:ehp2: from the pop-up menu. !!! correct menu
:p.To sort files in the file pane by type, press :hp2.Ctrl+F4:ehp2.
or click mouse button 2 anywhere inside the file pane and select
:hp2:Sort/By type:ehp2: from the pop-up menu. !!! correct menu


.*--------------------------------------------------------------*\
.*  Save as dialog box help panel                               *
.*      res = PANEL_SAVEASDLG                                   *
.*--------------------------------------------------------------*/
:h1 res=2275 name=PANEL_SAVEDLG.Save as dialog box
:i1.Save as dialog
:p.To name and save a new file of to save the current file under a new name:
:ol compact.
:li.Type the name of the file you want to save in the entry field or
select :hp2.Browse:ehp2. to choose it using the standard file dialog.
:li.Select the :hp2.OK:ehp2. pushbutton.
:eol.
:p.Detailed description of controls:
:dl break=all tsize=3.
:dt.:hp2.Save file as:ehp2.
:dd.Type a new pathname of the file you want to save and select the
:hp2.OK:ehp2. pushbutton.
:dt.:hp2.Browse:ehp2.
:dd.Select the :hp2.Browse:ehp2. pushbutton to display the standard file
dialog and use it to choose a name for the file. The name will be
placed into the :hp2.Save file as:ehp2. field.
:dt.:hp2.Reset path:ehp2.
:dd.Select the :hp2.Reset path:ehp2. pushbutton to reset the directory
part of the pathname to the workplace directory.
.* !!! link
:dt.:hp2.OK:ehp2.
:dd.Select the :hp2.OK:ehp2. pushbutton to save the file under the
pathname specified in the :hp2.Save file as:ehp2. field.
:dt.:hp2.Cancel:ehp2.
:dd.Select the :hp2.Cancel:ehp2. pushbutton if you decide not to save the file.
:edl.

.*--------------------------------------------------------------*\
.*  Save changes dialog box help panel                          *
.*      res = PANEL_SAVECHDLG                                   *
.*--------------------------------------------------------------*/
:h1 res=2280 name=PANEL_SAVECHDLG.Save changes dialog box
:i1.Save changes dialog
:p.This dialog box is displayed if there is one or more files
that were not saved after last modification and you are launching a tool
.* !!! ref
or terminating the Environment.
:p.Select:
:dl break=all tsize=3.
:dt.:hp2.Save all:ehp2.
:dd.to save all modified files and continue the requested operation.
:dt.:hp2.Prompt for each changed text:ehp2.
:dd.to have the Environment prompt you if you want to save each modified file
and then continue the operation.
:dt.:hp2.Don't save and continue:ehp2.
:dd.to continue the operation without saving any files.
:dt.:hp2.Cancel:ehp2.
:dd.to cancel the operation without saving any files.
:edl.

