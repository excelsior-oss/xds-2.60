.*==============================================================*
.*                                                              *
.* Edit.ipf - Information Tag Language file for the Edit menu   *
.*              help panels.                                    *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd.                                *
.*                                                              *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
.*                                                              *
.* Helps for EDIT menu and its items:                           *
.*      PANEL_EDIT                                              *
.*        PANEL_EDITUNDO                                        *
.*        PANEL_EDITREDO                                        *
.*        PANEL_EDITCUT                                         *
.*        PANEL_EDITCOPY                                        *
.*        PANEL_EDITPASTE                                       *
.*        PANEL_EDITSELALL                                      *
.*        PANEL_EDITCLEAR                                       *
.*        PANEL_EDITFIND                                        *
.*        PANEL_EDITFINDNEXT                                    *
.*        PANEL_EDITFINDPREVIOUS                                *
.*        PANEL_EDITFILEFIND                                    *
.*        PANEL_EDITPREVERROR                                   *
.*        PANEL_EDITNEXTERR                                     *
.*        PANEL_EDITGOTOLINE                                    *
.*                                                              *
.* Helps for appropriate dialogs:                               *
.*                                                              *
.*      PANEL_FINDDLG                                           *
.*      PANEL_FILEFINDDLG                                       *
.*                                                              *
.*==============================================================*


.*--------------------------------------------------------------*\
.*  Main Edit menu                                              *
.*      res = PANEL_EDIT                                        *
.*--------------------------------------------------------------*/
:h1 res=2000 name=PANEL_EDIT.Edit Menu
:i1 id=Edit.Edit Menu
:p.The :hp2.Edit:ehp2. menu contains commands that you use to undo
changes you've made to file, to cut, copy, paste and delete portions
of text, to find and replace text, and to locate errors contained
in the Message list window. The following commands appear in the
:hp2.Edit:ehp2. menu:
:parml compact tsize=3 break=all.
:pt.:hp2.Undo:ehp2.
:pd.Undoes editing changes
:pt.:hp2.Redo:ehp2.
:pd.Repeats changes that were undone
:pt.:hp2.Cut:ehp2.
:pd.Cuts selected text to the clipboard
:pt.:hp2.Copy:ehp2.
:pd.Copies selected text to the clipboard
:pt.:hp2.Paste:ehp2.
:pd.Inserts contents of the clipboard at the current position
:pt.:hp2.Select all:ehp2.
:pd.Selects the whole text in the current window
:pt.:hp2.Clear:ehp2.
:pd.Deletes selected text
:pt.:hp2.Find:ehp2.
:pd.Searches for the string, optionally replacing it with another
:pt.:hp2.Find next:ehp2.
:pd.Searches for the next occurence of a find pattern
:pt.:hp2.Find previous:ehp2.
:pd.Searches for the previous occurence of a find pattern
:pt.:hp2.File find:ehp2.
:pd.Searches for a string in project source files
:pt.:hp2.Prev error:ehp2.
:pd.Locates previous error from the Message List window
:pt.:hp2.Next error:ehp2.
:pd.Locates next error from the Message List window
:pt.:hp2.Goto line:ehp2.
:pd.Moves cursor to the specified line
:eparml.


.*--------------------------------------------------------------*\
.*  Edit menu Undo command help panel                           *
.*      res = PANEL_EDITUNDO                                    *
.*--------------------------------------------------------------*/
:h1 res=2005 name=PANEL_EDITUNDO.Undo
:i2 refid=Edit.Undo
:p.Use the :hp2.Undo:ehp2. command to undo changes you've made to
the current file.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\undo.bmp'.
:p.Default keyboard shortcut: :hp2.Alt+Backspace:ehp2.
:nt.Each window has its own undo buffer of fixed size. Thus, all types
(but not any number) of changes can be undone, except cutting or deleting
a large block of text.


.*--------------------------------------------------------------*\
.*  Edit menu Undo command help panel                           *
.*      res = PANEL_EDITREDO                                    *
.*--------------------------------------------------------------*/
:h1 res=2010 name=PANEL_EDITREDO.Redo
:i2 refid=Edit.Redo
:p.Use the :hp2.Redo:ehp2. command to repeat the last action
that was abandoned by the
:link reftype=hd refid=PANEL_EDITUNDO.Undo:elink.
command.
:p.Default keyboard shortcut: :hp2.Ctrl+Enter:ehp2.

.*--------------------------------------------------------------*\
.*  Edit menu Cut command help panel                            *
.*      res = PANEL_EDITCUT                                     *
.*--------------------------------------------------------------*/
:h1 res=2015 name=PANEL_EDITCUT.Cut
:i2 refid=Edit.Cut
:p.Use the :hp2.Cut:ehp2. command to delete selected text from the
file and move it to the Clipboard. You can use the :hp2.Paste:ehp2.
command later to insert Clipboard contents at another position,
into another window, or even into a file opened in a different application.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\cut.bmp'.
:p.Deafult keyboard shortcut: :hp2.Shift+Delete:ehp2.


.*--------------------------------------------------------------*\
.*  Edit menu Copy command help panel                           *
.*      res = PANEL_EDITCOPY                                    *
.*--------------------------------------------------------------*/
:h1 res=2020 name=PANEL_EDITCOPY.Copy
:i2 refid=Edit.Copy
:p.Use the :hp2.Copy:ehp2. command to duplicate selected text. A copy of
selected text is moved to the Clipboard. You can use the :hp2.Paste:ehp2.
command later to insert Clipboard contents at another position,
into another window, or even into a file opened in a different application.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\copy.bmp'.
:p.Default Keyboard shortcut: :hp2.Ctrl+Ins:ehp2.


.*--------------------------------------------------------------*\
.*  Edit menu Paste command help panel                          *
.*      res = PANEL_EDITPASTE                                   *
.*--------------------------------------------------------------*/
:h1 res=2025 name=PANEL_EDITPASTE.Paste
:i2 refid=Edit.Paste
:p.Use the :hp2.Paste:ehp2. command to insert the text from the Clipboard
into the current file at the cursor position.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\paste.bmp'.
:p.Default keyboard shortcut: :hp2.Shift+Insert:ehp2.
.* !!! Write about Paste replaces selection


.*--------------------------------------------------------------*\
.*  Edit menu Select all command help panel                     *
.*      res = PANEL_EDITSELALL                                  *
.*--------------------------------------------------------------*/
:h1 res=2030 name=PANEL_EDITSELALL.Select all
:i2 refid=Edit.Select all
:p.Use the :hp2.Select all:ehp2. command to select the whole text
in the current window.
:nt.The Stream selection mode will be set automatically.
.* !!! Ref to sel modes


.*--------------------------------------------------------------*\
.*  Edit menu Clear command help panel                          *
.*      res = PANEL_EDITCLEAR                                   *
.*--------------------------------------------------------------*/
:h1 res=2035 name=PANEL_EDITCLEAR.Clear
:i2 refid=Edit.Clear
:p.Use the :hp2.Clear:ehp2. command to delete selected text.
:p.Deafult keyboard shortcut: :hp2.Delete:ehp2.


.*--------------------------------------------------------------*\
.*  Edit menu Find command help panel                           *
.*      res = PANEL_EDITFIND                                    *
.*--------------------------------------------------------------*/
:h1 res=2040 name=PANEL_EDITFIND.Find
:i2 refid=Edit.Find
:p.Use the :hp2.Find:ehp2. command to locate or change the text in
the current file. A dialog box will appear, allowing you to
specify find/replace patterns and various options.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\find.bmp'.
:p.Default keyboard shortcut: :hp2.Ctrl+F:ehp2.


.*--------------------------------------------------------------*\
.*  Edit menu Find Next command help panel                      *
.*      res = PANEL_EDITFINDNEXT                                *
.*--------------------------------------------------------------*/
:h1 res=2045 name=PANEL_EDITFINDNEXT.Find Next
:i2 refid=Edit.Find Next
:p.Use the :hp2.Find Next:ehp2. command to find the next occurence
of the current find pattern.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\findnext.bmp'.
:p.Default keyboard shortcut: :hp2.Ctrl+L:ehp2.
.* !!! Write about Ctrl+Up/Down


.*--------------------------------------------------------------*\
.*  Edit menu Find Previous command help panel                  *
.*      res = PANEL_EDITFINDPREVIOUS                            *
.*--------------------------------------------------------------*/
:h1 res=2050 name=PANEL_EDITFINDPREVIOUS.Find Previous
:i2 refid=Edit.Find Previous
:p.Use the :hp2.Find Previous:ehp2. command to find the previous
occurence of the current find pattern.
:p.Mouse shortcut: Toolbar :hp2.Shift+:ehp2.:artwork runin name='..\res\findnext.bmp'.
:p.Default keyboard shortcut: :hp2.Ctrl+Shift+L:ehp2.
.* !!! Write about Ctrl+Up/Down


.*--------------------------------------------------------------*\
.*  Edit menu File Find command help panel                      *
.*      res = PANEL_EDITFILEFIND                                *
.*--------------------------------------------------------------*/
:h1 res=2055 name=PANEL_EDITFILEFIND.File Find
:i2 refid=Edit.File Find
:p.Use the :hp2.File Find:ehp2. command to search for a string
in the current set of source files. A dialog box will appear,
allowing you to specify a find pattern and various options.
:p.Default keyboard shortcut: :hp2.Ctrl+Shift+F:ehp2.


.*--------------------------------------------------------------*\
.*  Edit menu Previous Error help panel                         *
.*      res = PANEL_EDITPREVERROR                               *
.*--------------------------------------------------------------*/
:h1 res=2060 name=PANEL_EDITPREVERROR.Previous Error
:i2 refid=Edit.Previous Error
:p.Use the :hp2.Previous Error:ehp2. command to move the cursor
to the position of the previous error in the current file.
.* !!! Предыдущая в тексте (в листе - Shift+F7)
:p.Default keyboard shortcut: :hp2.F7:ehp2.


.*--------------------------------------------------------------*\
.*  Edit menu Next Error command help panel                     *
.*      res = PANEL_EDITNEXTERR                                 *
.*--------------------------------------------------------------*/
:h1 res=2065 name=PANEL_EDITNEXTERR.Next Error
:i2 refid=Edit.Next Error
:p.Use the :hp2.Next Error:ehp2. command to move the cursor
to the position of the next error in the current file.
.* !!! Следующая в тексте (в листе - Shift+F8)
:p.Default keyboard shortcut: :hp2.F8:ehp2.


.*--------------------------------------------------------------*\
.*  Edit menu Goto Line command help panel                      *
.*      res = PANEL_EDITGOTOLINE                                *
.*--------------------------------------------------------------*/
:h1 res=2070 name=PANEL_EDITGOTOLINE.Goto Line
:i2 refid=Edit.Goto Line
:p.Use the :hp2.Goto Line:ehp2. command to quickly move cursor
to the line with a certain number.
:p.Default keyboard shortcut: :hp2.Ctrl+G:ehp2.



.*--------------------------------------------------------------*\
.*  Find dialog box help panel                                  *
.*      res = PANEL_FINDDLG                                     *
.*--------------------------------------------------------------*/
:h1 res=2080 name=PANEL_FINDDLG.Find dialog box
:i1.Find dialog
:p.To locate or change a specific text:
:ol compact.
:li.Type the text you want to find or change in the :hp2.Find:ehp2. field.
:li.Type the text you want to change to in the :hp2.Change to:ehp2. field.
:li.Select the required options using the :hp2.Case sensitive:ehp2. and
:hp2.Find whole word:ehp2. controls.
:li.Select one of the :hp2.Find:ehp2. pushbuttons depending on the starting
point and direction desired.
:li.If the text was found, select one of the :hp2.Find/Change:ehp2. pushbuttons
for appropriate action or the :hp2.Close:ehp2. pushbutton to stop finding text.
:eol.
:p.Detailed description of controls:
:dl break=all tsize=3.
:dt.:hp2.Find:ehp2.
:dd.Type the text you want to find - the :hp1.find_pattern:ehp1.,
or choose it from the dropdown list.
:dt.:hp2.Change to:ehp2.
:dd.Type the text you want to change the :hp1.find_pattern:ehp1. to - the
:hp1.change_pattern:ehp1., or choose it from the dropdown list.
:dt.:hp2.Case sensitive:ehp2.
:dd.Matches case in the :hp1.find_pattern:ehp1.. For example, the "QWERTY" and
"qwerty" patterns are treated different.
:dt.:hp2.Find whole word:ehp2.
:dd.Finds only text bounded with non-alphabetical characters. For example,
"hole" won't be found in "whole".
:dt.:hp2.Find:ehp2.
:dd.Select :hp2.Find:ehp2. to find the next occurence of the
:hp1.find_pattern:ehp1..
:dt.:hp2.Find global:ehp2.
:dd.Select :hp2.Find global:ehp2. to find the first occurence of the
:hp1.find_pattern:ehp1. from the beginnning of the file.
:dt.:hp2.Find backward:ehp2.
:dd.Select :hp2.Find backward:ehp2. to find the previous occurence of the
:hp1.find_pattern:ehp1..
:dt.:hp2.Change, then find:ehp2.
:dd.Select :hp2.Change, then find:ehp2. to change the found text to
:hp1.change_pattern:ehp1. and find the next occurence of the
:hp1.find_pattern:ehp1..
:dt.:hp2.Change all:ehp2.
:dd.Select :hp2.Change all:ehp2. to change all occurences of the
:hp1.find_pattern:ehp1. to :hp1.change_pattern:ehp1..
:dt.:hp2.Cancel:ehp2.
:dd.Select :hp2.Close:ehp2. to close the :hp2.Find:ehp2. dialog.
:edl.

.*--------------------------------------------------------------*\
.*  File find dialog box help panel                             *
.*      res = PANEL_FILEFINDDLG                                 *
.*--------------------------------------------------------------*/
:h1 res=2085 name=PANEL_FILEFINDDLG.File find dialog box
:i1.File find dialog
:p.To find source files containing a specific text:
:ol compact.
:li.Type the text you want to find in the :hp2.Containing:ehp2. field.
:li.Select the required options using the :hp2.Case sensitive:ehp2.,
:hp2.Find whole word:ehp2., and :hp2.Find in selected files:ehp2. controls.
:li.Select the :hp2.Find:ehp2. pushbutton.
:li.If the text was found, double-click a file name in the :hp2.Files found:ehp2.
listbox to open that file or select :hp2.Close:ehp2. pushbutton to stop finding text.
:eol.
:p.Detailed description of controls:
:dl break=all tsize=3.
:dt.:hp2.Files found:ehp2.
:dd.This listbox contains names of source files in which the last specified pattern
was found. Double-click a file name or select it and press :hp2.Enter:ehp2.
to load that file into an auto window. The cursor will be positioned at the first
occurence of the pattern that was sought.
.* !!! ref to Auto Window
:dt.:hp2.Containing:ehp2.
:dd.Type the text you want to find - the :hp1.find_pattern:ehp1.,
or choose it from the dropdown list.
:dt.:hp2.Case sensitive:ehp2.
:dd.Matches case in the :hp1.find_pattern:ehp1.. For example, the "QWERTY" and
"qwerty" patterns are treated different.
:dt.:hp2.Find whole word:ehp2.
:dd.Finds only text bounded with non-alphabetical characters. For example,
"hole" won't be found in "whole".
:dt.:hp2.Find in selected files:ehp2.
:dd.Search only files selected in the source list.
.* !!! Ref to Source list.
:edl.
