.*==============================================================*\
.*                                                              *
.* Wkplace.ipf - Information Tag Language file for the          *
.*               Workplace menu help panels.                    *
.*  Copyright 1999 Excelsior. All Rights Reserved.              *
.*  Copyright 1996,97 xTech Ltd.                                *
.*                                                              *
.*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
.*                                                              *
.* Helps for WORKPLACE menu and its items:                      *
.*      PANEL_WORKPLACE                                         *
.*       PANEL_WPNEW                                            *
.*       PANEL_WPLIST                                           *
.*       PANEL_WPPROPERTIES                                     *
.*       PANEL_WPSAVEAS                                         *
.*       PANEL_WPTEMPLATES                                      *
.*                                                              *
.* Helps for appropriate dialogs:                               *
.*      PANEL_NEWWPDLG                                          *
.*      PANEL_WPLISTDLG                                         *
.*      PANEL_WPPROPDLG                                         *
.*      PANEL_WPTPLDLG                                          *
.*      PANEL_APPWPDLG                                          *
.*      PANEL_WPTPLADVDLG                                       *
.*                                                              *
.*==============================================================*


.*--------------------------------------------------------------*\
.*  Main Workplace menu                                         *
.*      res = PANEL_WORKPLACE                                   *
.*--------------------------------------------------------------*/
:h1 res=3400 name=PANEL_WORKPLACE.Workplace Menu
:i1 id=Workplace.Workplace Menu
:p.The :hp2.Workplace:ehp2. menu contains commands that you use to
maintain your workplaces.
.*
The following commands appear in the :hp2.Workplace:ehp2. menu:
:parml compact tsize=3 break=all.
:pt.:hp2.New:ehp2.
:pd.Creates a new worplace
:pt.:hp2.List:ehp2.
:pd.Displays workplace list and allows to open or forget an another workplace. +++
:pt.:hp2.Properties:ehp2.
:pd.Changes workplace properties
:pt.:hp2.Save as:ehp2.
:pd.Saves current workplace with new name. +++
:pt.:hp2.Templates:ehp2.
:pd.Creates and applyes workplace templites.
:eparml.



.*--------------------------------------------------------------*\
.*  Workplace menu New workplace command help panel             *
.*      res = PANEL_WPNEW                                       *
.*--------------------------------------------------------------*/
:h1 res=3405 name=PANEL_WPNEW.New
:p.Use the :hp2.New:ehp2. command to create a new workplace
using a workplace template or by extracting it from a mirror file.
A dialog will display, prompting you for the required parameters.

.*--------------------------------------------------------------*\
.*  Workplace menu List workplace command help panel            *
.*      res = PANEL_WPLIST                                      *
.*--------------------------------------------------------------*/
:h1 res=3410 name=PANEL_WPLIST.List
:p.Use the :hp2.List workplace:ehp2. command to open or forget
an another workplace. +++

.*--------------------------------------------------------------*\
.*  Workplace menu List workplace command help panel            *
.*      res = PANEL_WPPROPERTIES                                *
.*--------------------------------------------------------------*/
:h1 res=3415 name=PANEL_WPPROPERTIES.Workplace properties
:p.Use the :hp2.Workplace properties:ehp2. command to change workplace
properties: name, comment, working directory, and mirror file settings.

.*--------------------------------------------------------------*\
.*  Workplace menu List workplace command help panel            *
.*      res = PANEL_WPSAVEAS                                    *
.*--------------------------------------------------------------*/
:h1 res=3420 name=PANEL_WPSAVEAS.Workplace save as...
:p.Use the :hp2.Workplace save as...:ehp2. command to save current
workplace with a new name.

.*--------------------------------------------------------------*\
.*  Workplace menu List workplace command help panel            *
.*      res = PANEL_WPTEMPLATES                                 *
.*--------------------------------------------------------------*/
:h1 res=3425 name=PANEL_WPTEMPLATES.Workplace templates
:p.Use the :hp2.Workplace templtes:ehp2. command to create or
apply workplace templates.

.*--------------------------------------------------------------*\
.*  Workplace menu New workplace dialog help panel              *
.*      res = PANEL_NEWWPDLG                                    *
.*--------------------------------------------------------------*/
:h1 res=3430 name=PANEL_NEWWPDLG.Create new workplace dialog
:i2 refid=workplace.New workplace
:p.To create a new workplace using a
:link refid=PANEL_CONCEPTSWORKPLACES.workplace template:elink.&colon.
:ol compact.
:li.Select the :hp2.Use workplace template:ehp2. radiobutton.
:li.Select a workplace template from the list of available templates.
:li.Type the name for the new workplace in the
:hp2.New workplace name:ehp2. field.
:li.Select the :hp2.Change:ehp2. pushbutton next to the :hp2.Directory:ehp2.
box to change the default workplace directory.
:li.Select the :hp2.Ok:ehp2. pushbutton.
:eol.
:p.To extract a new workplace from a
:link.mirror file:elink.:
!!!
:ol compact.
:li.Select the :hp2.Extract from a mirror file:ehp2. radiobutton.
:li.Type the name of the mirror file in the :hp2.File:ehp2. field or
select the :hp2.Browse:ehp2. pushbutton next to it to select
it using a standard file dialog.
:li.Select the :hp2.Ok:ehp2. pushbutton.
:eol.
:p.Detailed description of controls:
:dl break=all tsize=3.
:dt.:hp2.Use workplace template:ehp2.
:dd.Select this radiobutton if you want to create a new workplace.
:dt.:hp2.Template:ehp2.
:dd.Contains the list of available workplace templates. Select "<None>"
to create a workplace using default settings.
:dt.:hp2.Comment:ehp2.
:dd.Displays comment for the currently selected template.
:dt.:hp2.New workplace name:ehp2.
:dd.Type a desired name for the new workplace.
:dt.:hp2.Directory:ehp2.
:dd.Displays the default directory for the new workplace. Initially
it is the same as the current workplace default directory.
:dt.:hp2.Change:ehp2.
:dd.Select this pushbutton to change the default directory for
the new workplace.
:dt.:hp2.Extract from a mirror file:ehp2.
:dd.Select this pushbutton if you want to extract a workplace
created on another system from a
:link.mirror file:elink..                      !!!
:dt.:hp2.File:ehp2.
:dd.Type the name of the mirror file from which you wish the
new workplace to be extracted.
:dt.:hp2.Browse:ehp2.
:dd.Select this pushbutton to select the mirror file to load
using the standard file dialog.
:dt.:hp2.Ok:ehp2.
:dd.Select the :hp2.Ok:ehp2. pushbutton to create a new workplace.
:dt.:hp2.Cancel:ehp2.
:dd.Select the :hp2.Cancel:ehp2. pushbutton if you decide not
to create a new workplace.
:edl.


.*--------------------------------------------------------------*\
.*  Workplace menu List workplace dialog help panel             *
.*      res = PANEL_WPLISTDLG                                   *
.*--------------------------------------------------------------*/
:h1 res=3435 name=PANEL_WPLISTDLG.Workplace list dialog
:i2 refid=workplace.Workplace list

Controls:
Workplace: listbox with workplaces
Comment:   MLE - comment for the selected workplace. May be changed.
Open
Remove
Close
Help

.*--------------------------------------------------------------*\
.*  Workplace menu Properties workplace dialog help panel       *
.*      res = PANEL_WPPROPDLG                                   *
.*--------------------------------------------------------------*/
:h1 res=3440 name=PANEL_WPPROPDLG.Workplace properties dialog
:i2 refid=workplace.Workplace properties
Controls:
Workplace name:    EF, may be changed
Workplace comment: MLE, may be changed
Directory:         STATIC, shows the current directory
Browse:            PB, use to change it
Use mirror file:   CB, ON/OFF mirror usage
                   EF, filename (with extension, but w/o path - it must
                                 be in the current directory)
Browse:            PB.
Ok cancel Help

.*--------------------------------------------------------------*\
.*  Workplace menu Templates dialog help panel                  *
.*      res = PANEL_WPTPLDLG                                    *
.*--------------------------------------------------------------*/
:h1 res=3445 name=PANEL_WPTPLDLG.Workplace templates dialog
:i2 refid=workplace.Workplace templates
Controls:
Apply template section:
  Available templates:    LB, available templates names
  Comment:                MLE, selected template comment. May be changed.
  Apply:                  PB, Workplace template Apply dialog (PANEL_APPWPDLG)
  Remove:                 PB, subj
Create new template section:
  New template name:      EF, subj
  Save:                   PB
  Advanced:               PB, Workplace template advanced settings dialog (PANEL_WPTPLADWDLG)
Ok cancel Help

.*--------------------------------------------------------------*\
.*  Workplace menu Templates command -> Apply dialog            *
.*      res = PANEL_APPWPDLG                                    *
.*--------------------------------------------------------------*/
:h1 res=3450 name=PANEL_APPWPDLG.Workplace: Apply workplace template dialog
:i2 refid=workplace.Apply workplace template
Controls:
Template name:          shows the name
Comment:                shows thw comment
Apply associations:     CB, set that local associations instead current
Apply toolset:          CB, when ON:
  Extend current:       RB
  Replace current:      RB
Ok Cancel Help

.*--------------------------------------------------------------*\
.*  Workplace menu Templates command -> Advanced settings dialog*
.*      res = PANEL_WPTPLADVDLG                                 *
.*--------------------------------------------------------------*/
:h1 res=3455 name=PANEL_WPTPLADVDLG.Workplace: Advanced workplace template dialog
:i2 refid=workplace.Advanced workplace template
Controls:
REXX script:            MLE, скрипт хранящийся с вокплэйсом. Выполняется при апплаинии темплита, созданного из
                        данного вокплэйса (или при создании нового вокплэйса на основе сего темплита). При этом,
                        данный скрипт прописывается в вокплэйс, если там уже был какой-то скрипт, то он потеряется.)
                        Основное применение - навесить хелпов в менюшку при создании вокплэйса.
Ok Cancel Help


  Apply template section:
  Available templates:    LB, available templates names
  Comment:                MLE, selected template comment. May be changed.
  Apply:                  PB, Workplace template Apply dialog (PANEL_APPWPDLG)
  Remove:                 PB, subj
Create new template section:
  New template name:      EF, subj
  Save:                   PB
  Advanced:               PB, Workplace template advanced settings dialog (PANEL_WPTPLADWDLG)
Ok cancel Help


