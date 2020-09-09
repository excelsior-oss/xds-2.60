.*==============================================================*
.*                                                              *
.* XDS.IPF - Information Tag Language file for the main IDE     *
.*           help manager                                       *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd.                                *
.*                                                              *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
.*                                                              *
.*   General help for IDE:                                      *
.*     PANEL_MAIN (F1 key)                                      *
.*       PANEL_CONCEPTS                                         *
.*         PANEL_CONCEPTSWORKPLACES                             *
.*         PANEL_CONCEPTSTOOLS                                  *
.*         PANEL_CONCEPTSMACROS                                 *
.*         PANEL_CONCEPTSREXX                                   *
.*       PANEL_MAINMENU                                         *
.*         PANEL_FILE       (file.ipf)                          *
.*         PANEL_EDIT       (edit.ipf)                          *
.*         PANEL_OPTIONS    (options.ipf)                       *
.*         PANEL_WORKPLACE  (workplace.ipf)                     *
.*         PANEL_TOOLS      (tools.ipf)                         *
.*         PANEL_PROJECT    (project.ipf)                       *
.*         PANEL_WINDOW     (window.ipf)                        *
.*         PANEL_MIHELP     (help.ipf)                          *
.*       PANEL_HELPKEYS     (help.ipf)                          *
.*                                                              *
.*                                                              *
.*==============================================================*

:userdoc.
:title.XDS IDE Help

.*==============================================================*
.*  General help                                                *
.*  res = PANEL_MAIN                                            *
.*==============================================================*
:h1 res=3600 name=PANEL_MAIN.Help for XDS
:i1 id=aboutMain.Help for XDS
.*:artwork align=center name='..\res\portfel.bmp'.  !!!
:p.XDS Environment is a powerful programmer's text editor with
a configurable set of external tools, which may include XDS compilers
and utilities as well as third-party programs. XDS project file
editor is also provided.
:p.Topics available:
:sl compact.
:li.:link reftype=hd refid=PANEL_CONCEPTS.Concepts:elink.
:li.:link reftype=hd refid=PANEL_MAINMENU.Main Menu:elink.
:li.:link reftype=hd refid=PANEL_HELPKEYS.Key Assignments:elink.
:esl.
:p.XDS on-line documentation:
:sl compact.
:li.:link reftype=launch object='view.exe' data='xdsug'.User's Guide:elink.
:li.:link reftype=launch object='view.exe' data='isom2'.ISO Modula-2 Language Reference:elink.
:li.:link reftype=launch object='view.exe' data='isolib'.ISO Modula-2 Standard Library:elink.
:esl.

.*--------------------------------------------------------------*
.*  Concepts help panels                                        *
.*      res = PANEL_CONCEPTS*                                   *
.*--------------------------------------------------------------*
:h1 name=PANEL_CONCEPTS.XDS IDE Concepts
:i1 id=concepts.Concepts
:p.The key concepts of XDS IDE are:
:ul.
:li.:link reftype=hd refid=PANEL_CONCEPTSWORKPLACES.Workplaces:elink.
:li.:link reftype=hd refid=PANEL_CONCEPTSTOOLS.Tools:elink.
:li.:link reftype=hd refid=PANEL_CONCEPTSMACROS.Macros:elink.
:li.:link reftype=hd refid=PANEL_CONCEPTSREXX.Rexx usage:elink.
:eul.

:h2 name=PANEL_CONCEPTSWORKPLACES.Workplaces
:i2 refid=concepts.Workplaces
:p.A :hp3.Workplace:ehp3. is a combination of parameters which define
the current state of the IDE. These parameters include:
:ul compact.
:li.Workplace name.
:li.Opened editing windows (loaded files, sizes and positions, colors and fonts,
syntax highlighting mode, cursor positions, etc.).
:li.Local :link reftype=hd refid=PANEL_CONCEPTSASSOC.associations:elink..
:li.Current project, if any.
:li.:link reftype=hd refid=PANEL_CONCEPTSTOOLS.Tools:elink..
:li.Local workplace :link reftype=hd refid=PANEL_CONCEPTSMACROS.macros:elink.
:li.Working directory
:eul.
:note.The working directory is just a startup directory for all tools.
You do not have to place all your source files to it or to its subdirectories.
:p.The IDE maintains a list of workplaces, allowing fast switch
from one to another.
:p.When you create a new workplace, you may choose a :hp3.template:ehp3. that
contains predefined settings for some of the listed parameters,
such as tools. Similarly, you may create a template from the current
workplace and use it later as a base for new workplaces.
:p.Another powerful feature is :hp3.mirror files:ehp3.. You may, for instance,
store a workplace into a mirror file on your office system, bring it to the home one
along with the source files, extract the workplace, and continue your work exactly
from the point where you stopped at the office. Then the IDE will keep these two
workplaces synchronized automatically, checking if the mirror file was updated
before opening the workplace.

.* Working directory -> separate topic !!!
.* Redirection file !!!
.* More about templates !!!

:h2 name=PANEL_CONCEPTSTOOLS.Tools
:i2 refid=concepts.Tools
:p.:hp3.Tools:ehp3. is an uniform integration mechanism for
all external programs, including XDS compilers and utilities,
third-party products, and the user's own executables and scripts.
Each tool is a
:link reftype=hd refid=PANEL_CONCEPTSREXX.REXX script:elink.
which is registered under a certain name and can be invoked by using
a Tools menu item, a hotkey, and/or a toolbar button. If there is more
than one tool with a certain name, they share the menu item, etc.
:p.A tool may be set up so that it is available only if a
:link.project file:elink.
is open and/or when the current window contains a file which name matches
the given regular expression. When you invoke a tool, the Environment
invokes the first tool
!!! category.

Тул - имя + рексовый скрипт + (условия для его запуска = методы доступа к нему + его энабельность).
Обладает свойством перегружаемости: тулы с одним именем имеют общие методы доступа, однако выполнится
только первый энабельный на данный момент тул.
К методам доступа могут (произвольно) относиться: меню, тулбар и хоткей.
Эабельность тула может быть ограничена условиями, налагаемыми на вид имени текущего редактируемого
файла и необходимостью наличия открытого файла проекта.
Про рексовый скрипт можно посмотреть подробнее в PANEL_CONCEPTSREXX.

:h2 name=PANEL_CONCEPTSMACROS.Macros
:i2 refid=concepts.Macros
:p.:hp3.Macros:ehp3. are
:link reftype=hd refid=PANEL_CONCEPTSREXX.REXX scripts:elink.
that are assigned to key combinations. In fact, almost any key combination
that performs a certain function in the Environment, does it by invoking
a REXX script. Exceptions are standard accelerators, such as :hp2.Alt+F4:ehp2..
:p.A macro may have a
:link reftype=hd refid=PANEL_CONCEPTSREGEX.regular expression:elink.
associated with it. If the current file name does not match that expression,
the macro will be disabled. Several macros may be assigned to one
key combination, and if the associated regular expressions are different,
that combination invokes one of the scripts depending on the
current file name. For instance, you may have :hp2.Ctrl+I:ehp2.
to produce "INTEGER" in Modula-2 files, but "int" in C files.
:p.During an Environment session, there are two active tables of keyboard macros:
the global table that is shared by all
:link reftype=hd refid=PANEL_CONCEPTSWORKPLACES.workplaces:elink.
and a (possibly empty) table that is local to the current workplace.
When a certain key combination is pressed, the Environment seeks
a enabled macro assigned to that combination, first in the local table
and then in the global table. Tables are searched from top to bottom,
so the relative order of macros in a table is significant.
If there were no macros found, the list of
:link reftype=hd refid=PANEL_CONCEPTSTOOLS.tools:elink.
is searched. If there were no tools found either and the key
was a printable character, that character is typed into the current file,
if any.
:link reftype=hd refid=PANEL_OPTMACROS.Macros:elink.
command in the :hp2.Options:ehp2. menu invokes the
:link reftype=hd refid=PANEL_MACRODLG.Macro Manager:elink.
that provides facilities to view, edit, add, and remove
keyboard macros.

:h2 name=PANEL_CONCEPTSREXX.Rexx usage
:i2 refid=concepts.Rexx
:p.REXX is a programming language created by IBM and REXX interpreter is a standard
component of the OS/2 operating system. The language is commonly used to write
OS/2 command scripts, but applications may also use the REXX interpreter to implement
macros. In XDS Environment, most keyboard commands invoke REXX scripts that call
back Environment routines.

.*  Вначале был рекс. И не было ничего кроме рекса. Затем стала оболочка.
.* И не было никаких действий в оной совершаемо, иначе как посредством рекса.
.* И было бы это хорошо, если бы было так. Однако большинство действий пользователя
.* в скрипты преобразуются и посредством оных исполняются, а к скриптам тем для тулов
.* и для макросов используемых, есть доступ. В скриптах можно использовать переменные
.* (вида $(имя)), каковые делятся на автоматические и юзерные. Юзерные можно посмотреть/изменить
.* в диалоге Workplace->Associations->Edit List->Variable list, ибо основное их использование
.* видится разве что в ассоциациях. Кстати: они глобальны. И вообще - фигня это
.* и юзать их - имхо западло. одна только юзерная переменная может интересовать оболочку:
.* XDSCompiler - имя xds-ного компилятора. Автоматические же - полезны весьма, список их:
.*  Proj          // Full project file name (or "")
.*  ProjName      // Project name w/o path & .ext (or "")
.*  ProjPath      // Project file path (w/o last '\') (or "")
.*  File          // Full current file name (or "" if none or when 'nonamed')
.*  FileName      // Current file name w/o path & .ext (or "")
.*  FileExt       // Current file extention (or "")
.*  FilePath      // Current file path (w/0 last '\') (or "")
.*  ActiveFrame   // "" | "MessageList" | "SourceList" | "Editor"
.* При запуске скрипта оболочка первым делом сканирует его на предмет нахождения
.* последовательностей $(<имя>) и заменяет их на значения соответствующих переменных
.* (значением неизвестных переменных считается пустая строка). Потом полученный
.* скрипт передается рексу для выполнения.

.*--------------------------------------------------------------*\
.*  Main menu help panel                                        *
.*      res = PANEL_MAINMENU                                    *
.*--------------------------------------------------------------*/
:h1 name=PANEL_MAINMENU.Main Menu
:i1.Main Menu
:p.The :hp2.Main Menu:ehp2. contains the following submenus:
:sl compact.
:li.:link reftype=hd refid=PANEL_FILE.File Menu:elink.
:li.:link reftype=hd refid=PANEL_EDIT.Edit Menu:elink.
:li.:link reftype=hd refid=PANEL_OPTIONS.Options Menu:elink.
:li.:link reftype=hd refid=PANEL_WORKPLACE.Workplace Menu:elink.
:li.:link reftype=hd refid=PANEL_TOOLS.Tools Menu:elink.
:li.:link reftype=hd refid=PANEL_PROJECT.Project Menu:elink.
:li.:link reftype=hd refid=PANEL_WINDOW.Window Menu:elink.
:li.:link reftype=hd refid=PANEL_MIHELP.Help Menu:elink.
:esl.

.*-- Import the File menu help file --*
.im file.ipf

.*-- Import the Edit menu help file --*
.im edit.ipf

.*-- Import the Options menu help file --*
.im options.ipf

.*-- Import the Workplace menu help file --*
.im wkplace.ipf

.*-- Import the Tools menu help file --*
.im tools.ipf

.*-- Import the Project menu help file --*
.im project.ipf

.*-- Import the Windows menu help file --*
.im window.ipf

.*-- Import the Help menu help file --*
.im help.ipf

:euserdoc.
