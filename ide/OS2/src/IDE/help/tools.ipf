.*==============================================================*\
.*                                                              *
.* Tools.ipf - Information Tag Language file for the Tools      *
.*             menu help panels.                                *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd.                                *
.*                                                              *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
.*                                                              *
.* Helps for TOOLS menu and its items:                          *
.*       PANEL_TOOLS                                            *
.*        PANEL_TOOLCFG                                         *
.*                                                              *
.* Helps for appropriate dialogs:                               *
.*       PANEL_TOOLCFGDLG                                       *
.*       PANEL_TOOLCHBTNDLG                                     *
.*       PANEL_ADDTOOLDLG                                       *
.*       PANEL_TOOLWIZDLG                                       *
.*       PANEL_TOOLEDFILTERDLG                                  *
.*       PANEL_POPUPWINDLG                                      *
.*                                                              *
.*==============================================================*/


.*--------------------------------------------------------------*\
.*  Main Tools menu                                             *
.*      res = PANEL_TOOLS                                       *
.*--------------------------------------------------------------*/
:h1 res=3000 name=PANEL_TOOLS.Tools Menu
:i1 id=Tools.Tools Menu
:p.The :hp2.Tools:ehp2. menu contains commands that you use to
modify the current set of
:link refid=PANEL_CONCEPTSTOOLS.tools:elink.
and to access them.
The following commands appear in the :hp2.Tools:ehp2. menu:
:parml compact tsize=3 break=all.
:pt.:hp2.Configure tools:ehp2.
:pd.Invoke the tools configuration dialog
:pt.:hp2.Recall popup window:ehp2.
.* !!! Move to another menu?
:pd.Display the tool pop-up window
:eparml.
:p.Menu items below these commands, if any, are the currently enabled
tools.

.*--------------------------------------------------------------*\
.*  Tools menu Edit tools command help panel                    *
.*      res = PANEL_TOOLCFG                                     *
.*--------------------------------------------------------------*/
:h1 res=3005 name=PANEL_TOOLCFG.Configure tools
:p.Use the :hp2.Configure tools:ehp2. command to add or remove tools and
to change their properties. A dialog box will appear, which contain
a list of all registered tools.

.*--------------------------------------------------------------*\
.*  Tools menu Edit tools command help panel                    *
.*      res = PANEL_TOOLCFGDLG                                  *
.*--------------------------------------------------------------*/
:h1 res=3020 name=PANEL_TOOLCFGDLG.Configure tools dialog
:i2 refid=Tools.Congigure tools dialog
:p.Use the :hp2.Configure tools dialog:ehp2. to add or remove tools and
to change their properties. A dialog box will appear, which contain
a list of tools.
Controls:
Tools list: рядом с ним кнопки с вертикальными стрелками для переупорядочивания
            тулов. Тулы ставятся в меню и в тулбар в этом порядке, кроме того, перегруженные
            (имеющие одно имя) тулы проверяются в этом же порядке до первого выполнябельного.
            В тулбаре показываются все заданные кнопки (дла перегруженых тулов - только для первого),
            они засериваются; в оконное меню набиваются только выполнябельные тулы (опять же, для
            перегруженных - только первый).
 Menu item: текст раздела мнею (можно не задавать), можно использовать $() переменные
 Hot-key:   там жмется кнопка.
 Comment:   текст выдаваемый в статусном окне при наезжании мышой на кнопку, можно использовать
            $() переменные
 Button frame: содержит поле с показываемой в нем кнопкой и кнопку
   Change      для изменения установки. (PANEL_TOOLCHBTNDLG)
 Enable rulezzz frame:
   Project required:  CB
   Filename required: EF - темплит имени файла, можно использовать $() переменные,
            <пусто> эквивалентно '*'
Кнопки
   Add       - Вылезает диалог PANEL_ADDTOOLDLG и добавляет созданный в нем тул в список
   Rename    - Запрашивает новое имя для тула
   Duplicate - сабж
   Remove    - сабж
   Default   - если тул ведет свое происхождение из темплита вокплэйса, то его установки на момент
               чтения оттуда хранятся и могут быть восстановлены этой кнопкой. Дуплицированные тулы
               тоже наследуют эту информацию (не на момент дублирования, а еще ту - старую).
   Wizard    - Диалог PANEL_TOOLWIZDLG для конфигуряния скрипта тула. Если скрипт отредактирован вручную, то
               визард может его не разобрать и предложит вставить команду завуска перед скриптом. Если же
               он и разберет его - то, кто его знает, может и поломать чего...
Tool script: MLE - скрипт тула. Можно использовать $() переменные. Лучше руками не трогать.
Ok
Cancel
Help


.*--------------------------------------------------------------*\
.*  Tools menu Edit tools command help panel                    *
.*      res = PANEL_TOOLCHBTNDLG                                *
.*--------------------------------------------------------------*/
:h1 res=3025 name=PANEL_TOOLCHBTNDLG.Set tool button dialog
:p.Use the :hp2.Set tool button dialog:ehp2.
 Текст на табличке гласит:  "Click the button you want to place to the toolbar for this tool.";
 Ниже расположена клавиатура с кнопками, на них нарисованы какие-то картинки.
 На большой кнопке внизу кто-то вырезал слова: "Place NO button".
 Так же имеются кнопки Cancel и Help.
 Поблизости никого нет.


.*--------------------------------------------------------------*\
.*  Tools menu Edit tools command help panel                    *
.*      res = PANEL_ADDTOOLDLG                                  *
.*--------------------------------------------------------------*/
:h1 res=3030 name=PANEL_ADDTOOLDLG.Add a new tool
:p.Use the :hp2.Add a new tool dialog:ehp2.
Tool name: EF
Comment:   EF
Ok Cancel Help


.*--------------------------------------------------------------*\
.*  Tools menu Edit tools command help panel                    *
.*      res = PANEL_TOOLWIZDLG                                  *
.*--------------------------------------------------------------*/
:h1 res=3035 name=PANEL_TOOLWIZDLG.Tool wizard dialog
:p.Use the :hp2.Tool wizard dialog:ehp2. to...

Tool     - inducates toolname
Comment  - comment
Execute  - файл с его командной строкой , в dropdown листе под ним висит "<Projecj>" -
           оно подставит $(ProjName).exe в качестве экзюшника.
Browse   - поиск файла
Run in separate session:  RB - запускать ли сие в отдельной сессии через start?
  Keep session after run: CB - ON: "start /F ...", OFF: "start /N /F ...";
Run in popup window:      RB - запускать в popup window
  Entry field :  popup window caption
  Use output filter:      CB - пускать ли stdout & stderr через фильтр
  Edit filter:            PB - диалог редактирования фильтра (PANEL_TOOLEDFILTERDLG)
Ok
Cancel
Help

.*--------------------------------------------------------------*\
.*  Tools menu Edit tools command help panel                    *
.*      res = PANEL_TOOLEDFILTERDLG                             *
.*--------------------------------------------------------------*/
:h1 res=3040 name=PANEL_TOOLEDFILTERDLG.Filter dialog
:p.Use the :hp2.Filter dialog:ehp2. to create, edit and delete
:hp3.filters:ehp3. - rules that may be used to extract error and
warning messages from data that an external program tool writes
to standard output.
:p.A filter is a regular expression and a set of associated rules. Each
line of the program output is compared with that regular expression and
if a match occurs, the rules are used to build the message and determine
its type (error, warning, or notice) and location (file, line, and column).


    Фильтр сообщений:
    ----------------
    Регулярное выражение (с переменными) и правила интерпретации этих переменных.
    Фильтров может быть описано несколько.

    Заданные фильтры последовательно сравниваются с началом очередной строки и,
    при нахождении удачного сравнения срабатывают. Если в фильтре нет правил для
    построения сообщения, то сообщения он не порождает, совпавшая строка даьлше
    не фильтруется.


    Описание фильтра:
    ==== Cut ====

    Filter = "<regular expression>"
    <rule 1>
    .
    .
    <rule n>

    ==== Cut ====
    Рулезов может и не быть.
    '"' внутри строки должны удваиваться.
    Строки начинающиеся на ';', '%' или '#' - комментарии
    Хвост строки правила после успешного его разбора - тоже комментарий
    Правила определяют значения для: File, Line, Position, Message, MessageKind.
    Виды их таковы:
    File         = ".\$1"     ; понятно
    Line         = "$1+1"     ; то, что пришло в $1, прибавить 1 (разрешается указывать +/- константу, можно не писать)
    Position     = "$1+1"     ; то, что пришло в $1, прибавить 1 (разрешается указывать +/- константу, можно не писать)
    Message      = "Oops: $(1)" ;
    MessageKind  = "$1" "expr 1" ... "expr n"
    ;; В Line и Position не строится текст, а вычисляются +/- смещения.
    ;; В MessageKind значение 0 выражения подставляется последовательно в регулярные выражения 1..4 и, пр первом
    ;;   успешном сравнении, сообщению придается, соответственно, тип: ERROR, WARNING, NOTICE, MSG_SEVERE.
    ;;     Тип по умолчанию - ERROR.
   Пример работающего фильтра (для СSet)
   ------------------
     ;; Error message example:
     ;; e:\fsa\mymdi\filters.cpp(234:4) : warning EDC3104: Unrecognized source character "#", code point 0x23.

     Filter       = "*$1\({0-9}$2:{0-9}$3\) : {a-z}$4 {A-Z}{0-9}: *$5"
     File         = "$1"
     Line         = "$2"
     Position     = "$3"
     Message      = "$5"
     MessageKind  = "$4" "error" "warning" "---" "---" ; Expressions order: ERROR, WARNING, NOTICE, MSG_SEVERE.
   ------------------

Итак, в диалоге дано:
MLE с фильтром(ами);
Ok
Cancel
Insert sample - вставить сэмпловый фильтр (надо довести до ума);
Help


.*--------------------------------------------------------------*\
.*  Edit tools dialog box help panel                            *
.*      res = PANEL_POPUPWINDLG                                 *
.*--------------------------------------------------------------*/
:h1 res=3045 name=PANEL_POPUPWINDLG.Popup window
:i1 id=tooldlg.Popup window
В первом фрейме - мигалка, которая мигает желтым при выполнении и становится
  красной/зеленой при завершении с не/нулевым кодом возврата.
Особо мудрые тулы могут выдавать текст в поле после нее, при ненулевом коде возврата там будет
  индицировано его значение.

Второй фрейм - тоже работает только со своими:
   Line: 0   Градусник
   Comment:

Третий: счетчики ошибок и ворнингов добавленных в процессе работы в Messagelist

Далее - окно выдачи тула. Правая кнопка мыши всплывает менюшку с
   Font...
   Copy - в клипбоад

Close - терминировать тул и закрыть окно
Stop  - терминировать тул
Message list - терминировать тул и перейти в messagelist (disabled if it is empty)
Help


