.*==============================================================*\
.*                                                              *
.* Help.ipf - Information Tag Language file for the Help menu   *
.*              help panels.                                    *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd.                                *
.*                                                              *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
.*                                                              *
.* Helps for HELP menu and its items:                           *
.*    PANEL_MIHELP                                              *
.*      PANEL_MIHELPINDEX                                       *
.*      PANEL_MIHELPGENERAL                                     *
.*      PANEL_MIHELPUSINGHELP                                   *
.*      PANEL_MIHELPKEYS                                        *
.*      PANEL_MIHELPPRODUCTINFO                                 *
.*                                                              *
.* Help panels:                                                 *
.*    PANEL_HELPGENERAL                                         *
.*    PANEL_HELPUSINGHELP                                       *
.*    PANEL_HELPKEYS                                            *
.*       PANEL_HELPKEYSMOVE                                     *
.*       PANEL_HELPKEYSEDIT                                     *
.*       PANEL_HELPKEYSSELECT                                   *
.*       PANEL_HELPKEYSFIND                                     *
.*       PANEL_HELPKEYSERRORS                                   *
.*       PANEL_HELPKEYSWINDOWS                                  *
.*       PANEL_HELPKEYSMISC                                     *
.*                                                              *
.*==============================================================*



.*--------------------------------------------------------------*\
.*  Main Help menu                                              *
.*      res = PANEL_MIHELP                                      *
.*--------------------------------------------------------------*/
:h1 res=2400 name=PANEL_MIHELP.Help Menu
:i1 id=Help.Help Menu
:p.The :hp2.Help:ehp2. menu contains commands that you use to display
the most general help topics, the alphabetical index of all help topics,
and the product information.
The following commands appear in the :hp2.File:ehp2. menu:
:parml compact tsize=3 break=all.
:pt.:hp2.Help index:ehp2.
:pd.Displays an alphabetical index of references to all help topics
:pt.:hp2.General help:ehp2.
:pd.Displays general information about XDS
:pt.:hp2.Using help:ehp2.
:pd.Displays general inforamtion about the help facitlity
:pt.:hp2.Keys help:ehp2.
:pd.Displays a list and description of keys and mouse shortcuts you can use
:pt.:hp2.Product information:ehp2.
:pd.Displays the Product information dialog
+++ Дополнительные хелпы прицепленные из вокплэйса.
:eparml.


.*--------------------------------------------------------------*\
.*  Help menu Help index command help panel                     *
.*      res = PANEL_MIHELPINDEX                                   *
.*--------------------------------------------------------------*/
:h1 res=2405 name=PANEL_MIHELPINDEX.Help index
:i2 refid=Help.Help index
:p.Use the :hp2.Help index:ehp2. command to display an alphabetic list of
references to the help topics for XDS.
:p.You can then select an index entry to display the help topic
that is related to that index entry.


.*--------------------------------------------------------------*\
.*  Help menu General help help panel                           *
.*      res = PANEL_MIHELPGENERAL                               *
.*--------------------------------------------------------------*/
:h1 res=2410 name=PANEL_MIHELPGENERAL.General help
:i2 refid=Help.General
:p.Use the :hp2.Help general:ehp2. command to ++++++++++

.*--------------------------------------------------------------*\
.*  Help menu Using help help panel                             *
.*      res = PANEL_MIHELPUSINGHELP                             *
.*--------------------------------------------------------------*/
:h1 res=2415 name=PANEL_MIHELPUSINGHELP.General help
:i2 refid=Help.Using help
:p.Use the :hp2.Using help:ehp2. to find out how to use the help facility.

.*--------------------------------------------------------------*\
.*  Help menu Key assignments help panel                        *
.*      res = PANEL_MIHELPKEYS                                  *
.*--------------------------------------------------------------*/
:h1 res=2420 name=PANEL_MIHELPKEYS.General help
:i2 refid=Help.Key assignments
:p.Use the :hp2.Keys help:ehp2. to display a description about each key
you can use in the program.

.*--------------------------------------------------------------*\
.*  Help menu Product information help panel                    *
.*      res = PANEL_MIHELPPRODUCTINFO                           *
.*--------------------------------------------------------------*/
:h1 res=2425 name=PANEL_MIHELPPRODUCTINFO.Product information
:i2 refid=Help.Product information
:p.Use the :hp2.Product information:ehp2. command to display the
:hp2.Product information:ehp2. dialog which contains XDS version
information and copyright notice.


.*--------------------------------------------------------------*\
.*  Help menu General help help panel                           *
.*      res = PANEL_HELPGENERAL                                 *
.*--------------------------------------------------------------*/
:h1 res=2430 name=PANEL_HELPGENERAL.General help
:i2 refid=Help.General
:link dependent reftype=hd refid=PANEL_MAIN auto.


.*--------------------------------------------------------------*\
.*  Help menu Using help help panel                             *
.*      res = PANEL_HELPUSINGHELP                               *
.*--------------------------------------------------------------*/
.*:h1 res=2435 name=PANEL_HELPUSINGHELP.Using Help
.*:i2 refid=Help.Using
.*:p. Place information for the Help Using Help menu here.+++


.*--------------------------------------------------------------*\
.*  Help menu Keys help help panel                              *
.*      res = PANEL_HELPKEYS                                    *
.*--------------------------------------------------------------*/
:h1 res=2440 name=PANEL_HELPKEYS.Key assignments
:i1 id=keys.Key assignments
:p.Select one of the following categories:
:sl compact.
:li.:link reftype=hd refid=PANEL_HELPKEYSMOVE.Cursor movement:elink.
:li.:link reftype=hd refid=PANEL_HELPKEYSEDIT.Text editing:elink.
:li.:link reftype=hd refid=PANEL_HELPKEYSSELECT.Selection:elink.
:li.:link reftype=hd refid=PANEL_HELPKEYSFIND.Finding text:elink.
:li.:link reftype=hd refid=PANEL_HELPKEYSERRORS.Locating errors:elink.
:li.:link reftype=hd refid=PANEL_HELPKEYSWINDOWS.Window management:elink.
:li.:link reftype=hd refid=PANEL_HELPKEYSMISC.Miscellaneous:elink.
:esl.


:h2 name=PANEL_HELPKEYSMOVE.Cursor movement
:i2 refid=keys.Cursor movement
:dl break=all tsize=3 compact.
:dt.:hp2.Arrows:ehp2.
:dd.Move cursor by one line or column
:dt.:hp2.Home:ehp2.
:dd.Move cursor to first non-space char/first column
:dt.:hp2.End:ehp2.
:dd.Move cursor to end of line
:dt.:hp2.PgUp:ehp2.
:dd.Move cursor one window up
:dt.:hp2.PgDn:ehp2.
:dd.Move cursor one window down
:dt.:hp2.Ctrl+PgUp:ehp2.
:dd.Move cursor to top of file
:dt.:hp2.Ctrl+PgDn:ehp2.
:dd.Move cursor to end of file
:dt.:hp2.Tab:ehp2.
:dd.Move cursor to the next tab stop
:dt.:hp2.Ctrl+G:ehp2.
:dd.Prompt for line number to go to
:dt.:hp2.Ctrl+K:ehp2. :hp3.digit:ehp3.
:dd.Set marker number :hp1.digit:ehp1.
:dt.:hp2.Ctrl+Q:ehp2. :hp3.digit:ehp3.
:dd.Go to marker number :hp1.digit:ehp1.
:edl.

:h2 name=PANEL_HELPKEYSEDIT.Text editing
:i2 refid=keys.Text editing
:dl break=all tsize=3 compact.
:dt.:hp2.Ins:ehp2.
:dd.Toggle insert/overwrite mode
:dt.:hp2.Backspace:ehp2.
:dd.Delete character before cursor or selection
:dt.:hp2.Ctrl+Backspace:ehp2.
:dd.Delete spaces or word before cursor or selection
:dt.:hp2.Del:ehp2.
:dd.Delete character after cursor or selection
:dt.:hp2.Ctrl+Del:ehp2.
:dd.Delete spaces or word under cursor or selection
:dt.:hp2.Ctrl+T:ehp2.
:dd.Delete spaces or word after cursor or selection
:dt.:hp2.Ctrl+E:ehp2.
:dd.Delete line after cursor or selection
:dt.:hp2.Ctrl+Y:ehp2.
:dd.Delete current line or selection
:dt.:hp2.Alt+Backspace:ehp2.
:dd.Undo last edit actions
:dt.:hp2.Ctrl+Enter:ehp2.
:dd.Undo last edit actions
:dt.:hp2.Alt+Up / Alt+Down:ehp2.
:dd.Move current line/selection one line up/down
:dt.:hp2.Alt+PgUp / Alt+PgDown:ehp2.
:dd.Move current line/selection one page up/down
:dt.:hp2.Alt+Right / Alt+Left:ehp2.
:dd.Indent/unindent current line/selection
:dd.(в режиме Column оно возит блок поверх текста) +++
:dt.:hp2.Ctrl+Shift+Up:ehp2.
:dd.Duplicate selected or current line(s) before itself
:dt.:hp2.Ctrl+Shift+Down:ehp2.
:dd.Duplicateselected or current line(s) after itself
:dt.:hp2.Enter:ehp2.
:dd.
:dt.:hp2.Ctrl+S:ehp2.
:dd.
:dt.:hp2.Ctrl+U:ehp2.
:dd.
:edl.

:h2 name=PANEL_HELPKEYSSELECT.Selection
:i2 refid=keys.Selection
Ctrl+'B'"         ||z|| "EdMode('SELECTION','NOTIFY');" ||z|| "Predefined macro" ||z|| "*" ||z
Ctrl+'W'"         ||z|| "EdSelWord();"                  ||z|| "Predefined macro" ||z|| "*" ||z
:dl break=all tsize=3 compact.
:dt.:hp2.Shift+:ehp2.:hp3.Cursor movement key:ehp3.
:dd.Extend selection
:dt.:hp2.Shift+Del:ehp2.
:dd.Cut the selected text to the Clipboard
:dt.:hp2.Ctrl+Ins:ehp2.
:dd.Copy the selected text to the Clipboard
:dt.:hp2.Shift+Ins:ehp2.
:dd.Insert the Clipboard contents at the cursor position
:dt.:hp2.Ctrl+B:ehp2.
:dd.   Stream->Column->Lines selection mode +++
:dt.:hp2.Ctrl+W:ehp2.
:dd.Select word under cursor
:edl.
:p.Mouse shortcuts:
:dl break=all tsize=3 compact.
:dt.:hp2.Drag selection:ehp2.
:dd.Move selection
:dt.:hp2.Ctrl+Drag selection:ehp2.
:dd.Copy selection
:edl.
:note.Contents of the Clipboard remain unchanged.

:h2 name=PANEL_HELPKEYSFIND.Finding text
:i2 refid=keys.Finding text
:dl break=all tsize=3 compact.
:dt.:hp2.Ctrl+F:ehp2.
:dd.Display the :hp2.Find/Change to:ehp2. dialog
:dt.:hp2.Ctrl+Shift+F:ehp2.
:dd.Display the :hp2.File find:ehp2. dialog
:dt.:hp2.Ctrl+L:ehp2.
:dd.Repeat last find
:dt.:hp2.Ctrl+Shift+L:ehp2.
:dd.Repeat last find backwards
:dt.:hp2.Ctrl+Up:ehp2.
:dd.Find previous occurence of the word under cursor
:dt.:hp2.Ctrl+Down:ehp2.
:dd.Find next occurence of the word under cursor
:dt.:hp2.Ctrl+P:ehp2.
:dd.Find matching parenthesis
// Или это все же cursor movements? :
:dt.:hp2.Ctrl+Left:ehp2.
:dd.
:dt.:hp2.Ctrl+Right:ehp2.
:dd.
:dt.:hp2.Shift+Ctrl+Left:ehp2.
:dd.
:dt.:hp2.ShiftCtrl+Right:ehp2.
:dd.
:edl.

:h2 name=PANEL_HELPKEYSERRORS.Locating errors
:i2 refid=keys.Locating errors
:dl break=all tsize=3 compact.
:dt.:hp2.F7:ehp2.
:dd.Go to previous error in textual order
:dt.:hp2.F8:ehp2.
:dd.Go to next error in textual order
:dt.:hp2.Shift+F7:ehp2.
:dd.Go to previous error in Message List order
:dt.:hp2.Shift+F8:ehp2.
:dd.Go to next error in Message List order
:edl.

:h2 name=PANEL_HELPKEYSWINDOWS.Window management
:i2 refid=keys.Window management
:dl break=all tsize=3 compact.
:dt.:hp2.F6:ehp2. or :hp2.Ctrl+Alt+'+':ehp2.
:dd.Switch to the next edit window
:dt.:hp2.Esc:ehp2. or :hp2.Ctrl+Alt+'-':ehp2.
:dd.Switch to the previous edit window
:dt.:hp2.Ctrl+'+':ehp2.
:dd.Switch to the next window in group
:dt.:hp2.Ctrl+'-':ehp2.
:dd.Switch to the previous window in group
:dt.:hp2.Alt+1..9:ehp2.
:dd.Switch to the window group by its number (0 denotes 'Auto' group)
:dt.:hp2.Alt+~:ehp2.
:dd.Switch to the Message list window
:dt.:hp2.Alt+\:ehp2.
:dd.Switch to the Source files window
:dt.:hp2.Ctrl+F5:ehp2.
:dd.Restore current window
:dt.:hp2.Ctrl+F7:ehp2.
:dd.Move current window
:dt.:hp2.Ctrl+F8:ehp2.
:dd.Change current window size
:dt.:hp2.Ctrl+F10:ehp2.
:dd.Maximize current window
:dt.:hp2.Ctrl+F11:ehp2.
:dd.Hide current window
:dt.:hp2.Ctrl+F4:ehp2.
:dd.Close current window
:dt.:hp2.Alt+F5:ehp2.
:dd.Restore main window
:dt.:hp2.Alt+F7:ehp2.
:dd.Move main window
:dt.:hp2.Alt+F8:ehp2.
:dd.Change main window size
:dt.:hp2.Alt+F9:ehp2.
:dd.Minimize main window
:dt.:hp2.Alt+F10:ehp2.
:dd.Maximize main window
:dt.:hp2.Alt+F4:ehp2.
:dd.Exit XDS
:edl.

:h2 name=PANEL_HELPKEYSMISC.Miscellaneous
:i2 refid=keys.Miscellaneous
:p.:hp7.WordStar compatibility keys:ehp7.
:dl break=all tsize=3 compact.
:dt.:hp2.Alt+'=':ehp2.
:dd.Начинает запись макроса, при этом вылезает диалог PANEL_MACROMONDLG
:dt.:hp2.Ctrl+F1:ehp2.
:dd.Контекстный хэлп для сова под курсором
:dt.:hp2.F3:ehp2.
:dd.Load file dialog. PANEL_FASTLOADDLG
:dt.:hp2.Ctrl+F3:ehp2.
:dd.Reload. Если файл изменен - спросит.
:dt.:hp2.F2:ehp2.
:dd.Save file
:dt.:hp2.Ctrl+F2:ehp2.
:dd.Save all changed files
In the source list window - оно нам тут надо?
:dt.:hp2.Gray +:ehp2.
:dd.Select files by mask
:dt.:hp2.Gray -:ehp2.
:dd.Unselect files by mask
:dt.:hp2.Gary *:ehp2.
:dd.Invert files selection
:edl.



