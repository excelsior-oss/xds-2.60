.*==============================================================*\
.*                                                              *
.* Options.ipf - Information Tag Language file for the screens  *
.*              for help on Options menu items.                 *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd                                 *
.*                                                              *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
.*                                                              *
.* Helps for OPTIONS menu and its items:                        *
.*      PANEL_OPTIONS                                           *
.*        PANEL_OPTSETTINGS                                     *
.*        PANEL_OPTASSOCIATIONS                                 *
.*        PANEL_OPTMACROS                                       *
.*                                                              *
.* Helps for appropriate dialogs:                               *
.*                                                              *
.*      PANEL_EDOPTDLG                                          *
.*      PANEL_ASSDLG                                            *
.*      PANEL_EDASSLISTDLG                                      *
.*      PANEL_EDVARDLG                                          *
.*      PANEL_MACRODLG                                          *
.*      PANEL_EDMACRODLG                                        *
.*      PANEL_MACROMONDLG                                       *
.*      PANEL_EDITCLRDLG                                        *
.*      PANEL_MAKECLRDLG                                        *
.*                                                              *
.*==============================================================*



.*>>>>>> LDV: блин, у нас есть Editor options dialog, он же - Settings в меню options. Давай его
.*       как-нибудь однозначно обзовем. Как? ИМХО Editor options - у меня так внутри имена зовутся :)

.*--------------------------------------------------------------*\
.*  Main Options menu                                           *
.*      res = PANEL_OPTIONS                                     *
.*--------------------------------------------------------------*/
:h1 res=2600 name=PANEL_OPTIONS.Options Menu
:i1 id=Options.Options Menu
:p.The Options menu contains commands that you use to set edit windows
font/colors and various IDE options. The following commands appear in the
Options menu:
:parml compact tsize=3 break=all.
:pt.:hp2.Settings:ehp2.
:pd.Changes current editor settings: font, colors...
:pt.:hp2.Associations:ehp2.
:pd.Changes files associations: default editor settings
for different file types.
:pt.:hp2.Macros:ehp2.
:pd.Changes keyboard macros.
:eparml.


.*--------------------------------------------------------------*\
.*  Options menu Settings command help panel                    *
.*      res = PANEL_OPTSETTINGS                                 *
.*--------------------------------------------------------------*/
:h1 res=2605 name=PANEL_OPTSETTINGS.Settings
:i2 refid=Options.Settings
:p.Use the :hp2.Settings:ehp2. command to change the current editor
settings: font, colors, syntax highlighting etc.
:p.Mouse shortcut: Toolbar :artwork runin name='..\res\settings.bmp'.

.*--------------------------------------------------------------*\
.*  Options menu Associations command help panel                *
.*      res = PANEL_OPTASSOCIATIONS                             *
.*--------------------------------------------------------------*/
:h1 res=2610 name=PANEL_OPTASSOCIATIONS.Associations
:i2 refid=Options.Associations
:p.Use the :hp2.Associations:ehp2. command to change file associations:
default editor settings for different file types.

.*--------------------------------------------------------------*\
.*  Options menu Macros command help panel                      *
.*      res = PANEL_OPTMACROS                                   *
.*--------------------------------------------------------------*/
:h1 res=2615 name=PANEL_OPTMACROS.Macros
:i2 refid=Options.Macros
:p.Use the :hp2.Macros:ehp2. command to change keyboard macros.

.*--------------------------------------------------------------*\
.*  Editor options dialog box help panel                        *
.*      res = PANEL_EDOPTDLG                                    *
.*--------------------------------------------------------------*/
:h1 res=2620 name=PANEL_EDOPTDLG.Editor settings dialog box
:i1.Editor settings dialog
(можно, вероятно, сказать, что этот диалог похож на диалог ассоциаций,
там даже что-то задизейбелнное висит :) )
Highlight mode - dropdownlist с известными типами файлов. (а мы вообще где-нибудь
  собираемся объяснять как прицепляются highligh-теры?).
Tab width - spin button
Smart tab - если в строке выше правее нас есть пробел(ы), то мы встанем после них,
  иначе - обычный tab
Autoident mode - CR under line
Paste replaces selection - subj
Color frame:
  Color palettes: click  - set
                  2clisk - edit & set (PANEL_EDITCLRDLG)
  Selected colors sample:
                  2clisk - edit (PANEL_EDITCLRDLG)
  Highlight syntax
  Highlight current line
  Apply to all - apply changes 2 ALL editors
Font frame:
  sample
  Apply to all - apply font changes 2 ALL editors
  Change
Ok
Cancel
Help


.*--------------------------------------------------------------*\
.*  Associations dialog box help panel                          *
.*      res = PANEL_ASSDLG                                      *
.*--------------------------------------------------------------*/
:h1 res=2625 name=PANEL_ASSDLG.Associations dialog box
:i1.Associations
:p.To change associations (the default editor settings for files
whose names match a particular template)&colon.
:ol.
:li.Select the :hp2.Global list:ehp2. or the :hp2.Local list:ehp2.
radiobutton to select the appropriate list of associations. !!! ref
:li.Select a filename template from the :hp2.Associate with files:ehp2.
list or select the :hp2.Edit list:ehp2. pushbutton to edit the list
of filename templates.
:li.Select a highlight mode.
:li.Select one of the eight predefined color schemes for the mode
or double click the current color scheme to edit it.
:
Associate with files frame
  Allows us to select the association we want to edit.
  Dropdown list - список шаблонов имен файлов. Этих списков два - глобальный
    и локальный. Для переключения между ними служат радиобаттоны:
    Global list
    Local list
  Edit list - диалог редактирования листов. (PANEL_EDASSLISTDLG)
Highlight mode - dropdownlist с известными типами файлов. (а мы вообще где-нибудь
  собираемся объяснять как прицепляются highligh-теры?).
Tab width - spin button
Smart tab - если в строке выше правее нас есть пробел(ы), то мы встанем после них,
  иначе - обычный tab
Autoident mode - CR under line
Paste replaces selection - subj
Color frame:
  Color palettes: click  - set
                  2clisk - edit & set (PANEL_EDITCLRDLG)
  Selected colors sample:
                  2clisk - edit (PANEL_EDITCLRDLG)
  Highlight syntax
  Highlight current line
Font frame:
  sample
  Change
  Change all - сразу для всех ассоциаций
Ok
Cancel
Help


.*--------------------------------------------------------------*\
.*  Edit association lists dialog box help panel                *
.*      res = PANEL_EDASSLISTDLG                                *
.*--------------------------------------------------------------*/
:h1 res=2630 name=PANEL_EDASSLISTDLG.Edit association lists dialog box
:i2 refid=Associations.Edit association lists dialog box
Редактируется, причем совершенно симметрично, два листа: глобальный и локальный.
Контролы:
Local list
Global list
Около каждого листа - кнопки:
  Со стрелками вверх и вниз - позволябт переупорядочивать ассоциации. Порядок
    поиска ассоциаций: последовательный перебор сначала в локальном, затем - в
    глобальном лмсте до совпадения имени файла. В глобальном листе _всегда_ есть
    неуничтожаемый элеметн '*', так что поиск обречен на успех.
    Неименованный файл соответствует шаблону "*".
  New - запрос темплита имени файла для вновь созданной ассоциации. Опции в такой
    ассоциации будут проинициализированы по дейолту.
  Edit - редактирование темплита
  Duplicate
  Remove
Ассоциации можно таскать между листами: >>Copy, <<Copy, >>Move, >>Move
В темплитах допускается использование юзерских (да и автоматических, вероятно :))
переменных.
Variable list - диалог редактирования юзерских переменных (PANEL_EDVARDLG)
Ok
Cancel
Help
Note: нельзя изменить/скопировать пердопределенный темплит '*' в глобальном листе.

.*--------------------------------------------------------------*\
.*  Variables list dialog box help panel                        *
.*      res = PANEL_EDVARDLG                                    *
.*--------------------------------------------------------------*/
:h1 res=2635 name=PANEL_EDVARDLG.Edit user variables list dialog box
:i1 refid=Variables list dialog box
В листе перечисленны юзерские переменные в форме name = "value"
New         - запросит имя и создаст переменную с пустым значением
Edit name   - предложит изменить имя
Edit value  - предложит изменить значение
Remove
Ok
Cancel
Help

.*--------------------------------------------------------------*\
.*  Macros dialog box help panel                                *
.*      res = PANEL_MACRODLG                                    *
.*--------------------------------------------------------------*/
:h1 res=2640 name=PANEL_MACRODLG.The macro manager
:i1.Macro manager
:p.The macro manager provides keyboard macro control facilities. It displays
the list of :hp3.global:ehp3. macros, which are available in all
:link refid=PANEL_CONCEPTSWORKPLACES.workplaces:elink.
and the list of macros which are local to the current workplace.
Both lists have very similar sets of associated pushbuttons:
:dl break=all tsize=3.
:dt.:hp2.Up/Down arrows:ehp2.
:dd.Move the currently selected macro one line up or down in the list.
:dt.:hp2.Copy to local:ehp2.
:dd.:hp3.Copies:ehp3. the currently selected global macro to the local list.
:dt.:hp2.Move to global:ehp2.
:dd.:hp3.Moves:ehp3. the currently selected local macro to the global list.
:dt.:hp2.Edit:ehp2.
:dd.Edits the currently selected macro.
:dt.:hp2.New:ehp2.
:dd.Adds a new macro to the list.
:dt.:hp2.Duplicate:ehp2.
:dd.Duplicates the currently selected macro.
:dt.:hp2.Remove:ehp2.
:dd.Removes the currently selected macro.
:dt.:hp2.Ok:ehp2.
:dd.Commits changes and exits the Macro manager.
:dt.:hp2.Cancel:ehp2.
:dd.Exits the Macro manager, abandoning all changes.
:edl.
.* Global list -
.* Local list  - лист макросов. Расположенные рядом кнопки с верт. стрелками
.* позволяют переупорядочивать макросы в листе, кнопки Copy to local & Move to global,
.* а так же Duplicate и Remove - тасовать листы всякими образами.
.* При нажатии клавиши макросы ищутся сначала в локальном, затем - в глобальном
.* листе по порядку. Если находится макрос для кнопки, но тип файла у него не совпадает -
.* поиск продолжается.
.* ВАРНИНГ: менять глобальные макросы, видимые всем проектам - нехорошо. Можно всех
.* захрюкать. Расхрюкиватель содержится в скрипте MkXdsIni.cmd - он снесет глобальный
.* лист и пропишет дефолтный. Как искать файл XD$.INI можно рассказать...
.* Содержимое акроса можно изменить, вызвав диалог редактирования макроса (PANEL_EDMACRODLG)
.* кнопкой Edit.
.* Кроме того, можно создать локальный макрос по Alt+= из редактора (PANEL_MACROMONDLG)...
.* Где-то тут следовало бы документировать рексовые функции, когда они станут
.* нам известны... :)
.*
.* Контролы:
.* Слева:
.*   Global macros list
.*   Copy to local
.*   Edit - (PANEL_EDMACRODLG)
.*   New  - тоже запускает редактирование (PANEL_EDMACRODLG)
.*   Duplicate
.*   Remove
.*
.* Справа:
.*   Local macros list
.*   Move to global
.*   Edit - (PANEL_EDMACRODLG)
.*   New  - тоже запускает редактирование (PANEL_EDMACRODLG)
.*   Duplicate
.*   Remove
.*
.* Ok, Cancel, Help

.*--------------------------------------------------------------*\
.*  Edit macro dialog box help panel                            *
.*      res = PANEL_EDMACRODLG                                  *
.*--------------------------------------------------------------*/
:h1 res=2645 name=PANEL_EDMACRODLG.Edit macro dialog box
:i1.Edit macro dialog box
:p.To enter or edit the macro comment&colon.
:ol compact.
:li.Type a new or edit the existing comment in the :hp2.Comment:ehp2.
field.
:li.Select the :hp2.Ok:ehp2. pushbutton.
:eol.
:p.To change the associated key combination.
:ol compact.
:li.Select the :hp2.Key:ehp2. field using the mouse.
:li.Press the desired key combination for the macro.
:li.Select the :hp2.Ok:ehp2. pushbutton.
:eol.
:note.Some keys and key combinations may only be entered by
pressing the :hp2....:ehp2. pushbutton next to the :hp2.Key:ehp2.
field and typing them manually.
.* !!! Move to a separate topic
:p.To set or modify the associated filename filter:
.* !!! ref
:ol compact.
:li.Type the filter in the :hp2.Required file:ehp2. field.
:li.Select the :hp2.Ok:ehp2. pushbutton.
:eol.
:p.To modify the macro itself:
.* !!! ref
:ol compact.
:li.Edit the REXX script in the MLE field.
:li.Select the :hp2.Ok:ehp2. pushbutton.
:eol.
:p.Detailed description of controls:
:dl break=all tsize=3.
:dt.:hp2.???:ehp2.
:dd.???

.*--------------------------------------------------------------*\
.*  Edit macro dialog box help panel                            *
.*      res = PANEL_MACROMONDLG                                 *
.*--------------------------------------------------------------*/
:h1 res=2650 name=PANEL_MACROMONDLG.Macro recording monitor
:i1.Macro recording monitor
Comment - строка комментария для показа в листах в (PANEL_MACRODLG),
можно не
задавать.
Key     - тут показано название кнопки. При запуске монитора он,
первым делом,
дает фокус
          этому окошку и ждет нажатия кнопки. После этого - переключается
обратно на оболочку
          и пишет макрос. При повторном вызове монитора (или при переходе в него
мышой) он готов
          к запоминанию созданого макроса. Тут опять можно нарулить значения
установок в
          мониторе, можно вернуться в оболочку и продолжить запись макрова.
Required file - шаблон имени файла. Ограничивает область применения
макроса
          сими файлами.
Кнопки:
Done    - если для данной кнопки уже определен макрос, то спросит:
изменить ли
его.
          при отказе - вернется в диалог (можно поменять кнопку).
Cancel
Details>> / No details - включает/выключает расширенное окно, в
котором можно
наблюдать (но не редактировать)
          текст создаваемого макроса.
Help.

.*--------------------------------------------------------------*\
.*  Edit colors dialog box help panel                           *
.*      res = PANEL_EDITCLRDLG                                  *
.*--------------------------------------------------------------*/
:h1 res=2655 name=PANEL_EDITCLRDLG.Edit colors dialog box
:i1.Edit colors dialog box
:p.To change color settings for a highlight mode:
:ol.
:li.Select an item from the :hp2.Text element:ehp2. listbox.
:li.Select a color from the color table. Double click a color box
    to change its color (the first 16 colors cannot be changed).
:li.Select the :hp2.Underline:ehp2. checkbox if you want the
    currently selected text element to be underlined.
:li.Repeat the above steps for other text elements if desired.
:li.Select the :hp2.OK:ehp2. pushbutton.
:eol.
:p.Detailed description of controls:
:dl break=all tsize=3.
:dt.:hp2.Sample text:ehp2.
:dd.Displays a small piece of text containing all text elements supported
by the highlight mode being edited.
.* !!! Terms!!! May be "syntax elements"? "highlight mode" - I do not liek either.
.* Refernces to how all this stuff works (DLLs etc.)
:dt.:hp2.Text element:ehp2.
:dd.Select a text element which color and/or underline attribute you want to change.
:dt.:hp2.Color table:ehp2.
.* !!! This text is not present on the dialog.
:dd.Select a color for the currently selected text element. All but the first 16
colors in the table can be edited by double-clicking them. :hp2.Note::ehp2.
This table is shared by all workplaces.
:dt.:hp2.Underline:ehp2.
:dd.Select this checkbox if you want the currently select text element
to be underlined on display.
:dt.:hp2.Ok:ehp2.
:dd.Select the :hp2.Ok:ehp2. pushbutton to confirm your changes to
the color scheme.
:dt.:hp2.Cancel:ehp2.
:dd.Select the :hp2.Cancel:ehp2. pushbutton if you decide to abandon
your changes.
:edl.

.* Sample text:
.*   Раскрашенный текст со всеми известными элементами. Зависит от
.*   текущего парсера.
.* Text element listbox:
.*   Элементы текста. абор их зависит от текущего парсера.
.* Справа - цвета. Первые 16 цветов дефолтные, остальные можно
.* редактировать:
.* даблклик на
.*          цвете - диалог PANEL_MAKECLRDLG для его редактирования.
.* Underline checkbox - подчеркивание элемента текста.
.* Следует заметить, что
.*   1) Таблица цветов глобальна, т.е. едина для все вокплэйсов.
.*   2) При выборе цвета элемента текста мы берем его значение и дальнейшее
.* редатирование
.*      исходного цвета на раскраске ранее использовавших его элементов текста не
.* отразится.
.* иже - кнопки Ok Cancel Help

.*--------------------------------------------------------------*\
.*  Color dialog box help panel                                 *
.*      res = PANEL_MAKECLRDLG                                  *
.*--------------------------------------------------------------*/
:h1 res=2660 name=PANEL_MAKECLRDLG.Color dialog box
:i1.Color dialog box
Слева расположено поле с цветами. (RGB которые :)
Тыкая в него можно выбрать желаетый цвет, он показывается
в окне справа.
Скролл Bright служит для регулирования яркости цвета,
Скроллы R, G, B - для регулировки его цветовых составляющих.
Кнопка Ok приводит нас к успеху,
Cancel - не приводит.
Help дает возможность прочесть сей текст.

