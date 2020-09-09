.*==============================================================*\
.*                                                              *
.* Project.ipf - Information Tag Language file for the screens  *
.*               for help on Project menu items.                *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd.                                *
.*                                                              *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
.*                                                              *
.* Helps for PROJECT menu and its items:                        *
.*    PANEL_PROJECT                                             *
.*      PANEL_PROJNEW                                           *
.*      PANEL_PROJOPEN                                          *
.*      PANEL_PROJCLOSE                                         *
.*      PANEL_PROJMODIFY                                        *
.*      PANEL_PROJEDTEXT                                        *
.*                                                              *
.* Helps for appropriate dialogs:                               *
.*    PANEL_PRJNEWDLG                                           *
.*    PANEL_PRJMODDLG                                           *
.*    PANEL_USROPTDLG                                           *
.*    PANEL_USREQDLG                                            *
.*    PANEL_MODULESDLG                                          *
.*    PANEL_USRLOOKUPDLG                                        *
.*                                                              *
.*==============================================================*



.*--------------------------------------------------------------*\
.*  Main Project menu                                           *
.*      res = PANEL_PROJECT                                     *
.*--------------------------------------------------------------*/
:h1 res=2800 name=PANEL_PROJECT.Project Menu
:i1 id=Project.Project Menu
:p.The Project menu contains commands that you use to open, close
and edit a project file, and to compile, make and run your program.
The following commands appear in the Project menu:
:parml compact tsize=3 break=all.
:pt.:hp2.New project:ehp2.
:pd.Creates new project file and opens it as a project
:pt.:hp2.Open project:ehp2.
:pd.Opens a project file
:pt.:hp2.Close project:ehp2.
:pd.Closes a project file
:pt.:hp2.Modify project:ehp2.
:pd.Lets you modify project using a dialog box
:pt.:hp2.Edit project file:ehp2.
:pd.Lets you edit the project file manually
:eparml.
В конце меню - история проектов.


.*--------------------------------------------------------------*\
.*  Project menu Open project command help panel                *
.*      res = PANEL_PROJNEW                                     *
.*--------------------------------------------------------------*/
:h1 res=2805 name=PANEL_PROJNEW.New project
:i2 refid=Project.New project
:p.Use the :hp2.New project:ehp2. command to create new project file.
:p.A dialog will appear that allows you to enter a project file name.


.*--------------------------------------------------------------*\
.*  Project menu Open project command help panel                *
.*      res = PANEL_PROJOPEN                                    *
.*--------------------------------------------------------------*/
:h1 res=2810 name=PANEL_PROJOPEN.Open project
:i2 refid=Project.Open project
:p.Use the :hp2.Open project:ehp2. command to open an existing project file.
:p.A dialog will appear that allows you to select a project file.
Следует добавить, что тип файла тут не регламентируется, имя может быть любым -
это всего лишь значение переменных $(proj*) и условие енабеьности тулов.
Редактировать в картинках левые проекты не удастся :)


.*--------------------------------------------------------------*\
.*  Project menu Close project command help panel               *
.*      res = PANEL_PROJCLOSE                                   *
.*--------------------------------------------------------------*/
:h1 res=2815 name=PANEL_PROJCLOSE.Close project
:i2 refid=Project.Close project
:p.Use the :hp2.Close project:ehp2. command to close the currently
opened project.


.*--------------------------------------------------------------*\
.*  Project menu Modify project command help panel              *
.*      res = PANEL_PROJMODIFY                                  *
.*--------------------------------------------------------------*/
:h1 res=2820 name=PANEL_PROJMODIFY.Modify project
:i2 refid=Project.Modify project
:p.Use the :hp2.Modify project:ehp2. command to edit currently opened
project using a dialog box.
Только для известных типов проектов, иначе парсер навыдает ошибок.


.*--------------------------------------------------------------*\
.*  Project menu Edit project file command help panel           *
.*      res = PANEL_PROJEDTEXT                                  *
.*--------------------------------------------------------------*/
:h1 res=2825 name=PANEL_PROJEDTEXT.Edit project file
:i2 refid=Project.Edit project file
:p.Use the :hp2.Edit project file:ehp2. command to edit currently
opened project file manually. The auto window will be opened and the
project file will be loaded into it.
или же мы перейдем в окно, где он уже редактируется.


.*--------------------------------------------------------------*\
.*  New project dialog box help panel                           *
.*      res = PANEL_PRJNEWDLG                                   *
.*--------------------------------------------------------------*/
:h1 res=2830 name=PANEL_PRJNEWDLG.New project dialog box
:i1 id=newprjdlg.New project dialog
Project name - EF: тут можно ввести имя файла проекта (ограничений
                   нет, так что следует указывать и расширение.)
Browse...    - PB: browse dialog, полное имя того, что в нем будет
                   выбрано появится в Project name EF
Use template - dropdownlist - убрать его? Все равно его не придумали...
Create       - создвть файл (если уже есть - спросит overwrite it,
               при отказе - вернется в этот диалог.
Cancel
Help

.*--------------------------------------------------------------*\
.*  Modify project dialog box help panel                        *
.*      res = PANEL_PRJMODDLG                                   *
.*--------------------------------------------------------------*/
:h1 res=2835 name=PANEL_PRJMODDLG.Modify project dialog box
:i1 id=modifyprjdlg.Modify project dialog

А не рассказать ли откуда что берется?
В переменной $(XDSCompiler) должно быть прописано имя
компилятора (типа e:\xds\bin\xc.exe). Изначально это
может там появиться при создании вокплэйса с использованием
темплита сию установку в себя включающего (то есть там есть
адванснутый сеттинг темплита с рексовым скриптом данную
переменную устанавливающим). Если переменной нет, то надо нарулить
ручками.
Расширение имени компилятора меняется на .prt (project template)
и по нему строится дерево опций для просмотра в нижеупомянутом
диалоге. Синтаксис файла восходит ко временам забытым и посему
может быть восстановлен только при очень большом желании. Содержимое
тоже, вероятно, не соответствует современному набору опций.

Далее производится попытка разобрать текст проекта в соответствиями
со знаниями оболочки о его синтаксисе. Большая часть знаний
в последней редакции была откручена по просьбам какого-то
снежного человека, после чего она не понимает проекты из
сэмплов: модули с расширением ей неизвестны, юзерские опции
- только "-name:+" а не ":name+" и т.п. При неуспехе, ошибки
набиваются в еррор-лист и там сидят, порою по два года,
иначе - появляется диалог:

Слева - листбокс с деревом разделов для редактирования.
Даблклик на нетерминальных узлах этого дерева рас/за-крывает их.
Справа - страничка опций висящих на выбранном узле дерева.
Под ней - комментарий к опции под курсором. Комментарий
самодокументирующийся Лучше про него не писать (?)
Слева кнопки
  User options   - PANEL_USROPTDLG
  User equations - PANEL_USREQDLG
  Modules        - PANEL_MODULESDLG
  Lookups        - PANEL_USRLOOKUPDLG
Ok
Cancel
Help


.*--------------------------------------------------------------*\
.*  Modify project dialog box help panel                        *
.*      res = PANEL_USROPTDLG                                   *
.*--------------------------------------------------------------*/
:h1 res=2840 name=PANEL_USROPTDLG.User options
:i2 id=modifyprjdlg.User options dialog
Вверху - лист с опциями и их значениями.
Внизу - кнопки
New... - запросит имя дла новой опции
Remove - ясно
Toggle - изменит значение на противоположное (+ на - и - на +)
Ok
Cancel
Help

.*--------------------------------------------------------------*\
.*  Modify project dialog box help panel                        *
.*      res = PANEL_USREQDLG                                    *
.*--------------------------------------------------------------*/
:h1 res=2845 name=PANEL_USREQDLG.User equations dialog box
:i2 id=modifyprjdlg.User equations dialog
Вверху - лист со строками вида "имя=значение",
кнопки:
New        - запросит имя, затем - значение,
Remove     -
Edit value - покажет текущее значение и позволит его изменить
Ok Cancel Help


.*--------------------------------------------------------------*\
.*  Modify project dialog box help panel                        *
.*      res = PANEL_MODULESDLG                                  *
.*--------------------------------------------------------------*/
:h1 res=2850 name=PANEL_MODULESDLG.Modules dialog box
:i2 id=modifyprjdlg.Modules dialog
Вверху - лист со строками вида "!module name",
кнопки:
New        - запросит имя модуля и добавит его
Remove     -
Edit name - покажет имя и позволит его изменить
Ok Cancel Help

.*--------------------------------------------------------------*\
.*  Modify project dialog box help panel                        *
.*      res = PANEL_USRLOOKUPDLG                                *
.*--------------------------------------------------------------*/
:h1 res=2855 name=PANEL_USRLOOKUPDLG.Lookups dialog box
:i2 id=modifyprjdlg.Lookups dialog
Вверху - лист со строками вида "-lookup = template = pathlist"
кнопки:
New           - запросит имя template, затем - pathlist
Remove        -
Edit pathlist - покажет pathlist и позволит его изменить
Ok Cancel Help



