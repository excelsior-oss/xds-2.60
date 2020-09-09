/*
*
*  File: help2.h
*
*  Help panel identifyers
*
*  (c) 1997 by *FSA & NHA Inc.
*
*/

#ifndef __HELP2__
#define __HELP2__

/* ===== Help table and subtables ===== */
//
#define SHELL_HELP_TABLE        1000
#define SUBTABLE_MAIN           1001
#define SUBTABLE_PRODUCTINFODLG 1002

/* ===== Edit.ipf ===== */
//
#define PANEL_EDIT              2000
#define PANEL_EDITUNDO          2005
#define PANEL_EDITREDO          2010
#define PANEL_EDITCUT           2015
#define PANEL_EDITCOPY          2020
#define PANEL_EDITPASTE         2025
#define PANEL_EDITSELALL        2030
#define PANEL_EDITCLEAR         2035
#define PANEL_EDITFIND          2040
#define PANEL_EDITFINDNEXT      2045
#define PANEL_EDITFINDPREVIOUS  2050
#define PANEL_EDITFILEFIND      2055
#define PANEL_EDITPREVERROR     2060
#define PANEL_EDITNEXTERR       2065
#define PANEL_EDITGOTOLINE      2070
//
#define PANEL_FINDDLG           2080
#define PANEL_FILEFINDDLG       2085

/* ===== File.ipf ===== */
//
#define PANEL_FILE              2200
#define PANEL_FILENEW           2205
#define PANEL_FILELOAD          2210
#define PANEL_FILERELOAD        2215
#define PANEL_FILESAVE          2220
#define PANEL_FILESAVEAS        2225
#define PANEL_FILESAVEALL       2230
#define PANEL_FILEPRINT         2235
#define PANEL_FILEAUTOSAVE      2240
#define PANEL_FILEMAXNEW        2245
#define PANEL_FILEBAK           2250
#define PANEL_FILEUSE1AUTOWIN   2255
//
#define PANEL_FASTLOADDLG       2265
#define PANEL_FILEDLG           2270
#define PANEL_SAVEASDLG         2275
#define PANEL_SAVECHDLG         2280

/* ===== Help.ipf ===== */
//
#define PANEL_MIHELP            2400
#define PANEL_MIHELPINDEX       2405
#define PANEL_MIHELPGENERAL     2410
#define PANEL_MIHELPUSINGHELP   2415
#define PANEL_MIHELPKEYS        2420
#define PANEL_MIHELPPRODUCTINFO 2425
//
#define PANEL_HELPGENERAL       2430
#define PANEL_HELPUSINGHELP     2435
#define PANEL_HELPKEYS          2440
#define PANEL_HELPKEYSMOVE      2445
#define PANEL_HELPKEYSEDIT      2450
#define PANEL_HELPKEYSSELECT    2455
#define PANEL_HELPKEYSFIND      2460
#define PANEL_HELPKEYSERRORS    2465
#define PANEL_HELPKEYSWINDOWS   2470
#define PANEL_HELPKEYSMISC      2475

/* ===== Options.ipf ===== */
//
#define PANEL_OPTIONS           2600
#define PANEL_OPTSETTINGS       2605
#define PANEL_OPTASSOCIATIONS   2610
#define PANEL_OPTMACROS         2615
#define PANEL_MACROMONDLG       2616
//
#define PANEL_EDOPTDLG          2620
#define PANEL_ASSDLG            2625
#define PANEL_EDASSLISTDLG      2630
#define PANEL_EDVARDLG          2635
#define PANEL_MACRODLG          2640
#define PANEL_EDMACRODLG        2645
#define PANEL_EDITCLRDLG        2650
#define PANEL_MAKECLRDLG        2655

/* ===== Project.ipf ===== */
//
#define PANEL_PROJECT           2800
#define PANEL_PROJNEW           2805
#define PANEL_PROJOPEN          2810
#define PANEL_PROJCLOSE         2815
#define PANEL_PROJMODIFY        2820
#define PANEL_PROJEDTEXT        2825
//
#define PANEL_PRJNEWDLG         2830
#define PANEL_PRJMODDLG         2835
#define PANEL_USROPTDLG         2840
#define PANEL_USREQDLG          2845
#define PANEL_MODULESDLG        2850
#define PANEL_USRLOOKUPDLG      2855


/* ===== Tools.ipf ===== */
//
#define PANEL_TOOLS             3000
#define PANEL_TOOLCFG           3005
//
#define PANEL_TOOLCFGDLG        3020
#define PANEL_TOOLCHBTNDLG      3025
#define PANEL_ADDTOOLDLG        3030
#define PANEL_TOOLWIZDLG        3035
#define PANEL_TOOLEDFILTERDLG   3040
#define PANEL_POPUPWINDLG       3045

/* ===== Window.ipf ===== */
//
#define PANEL_WINDOW            3200
#define PANEL_WINMSGLIST        3205
#define PANEL_WINSRCLIST        3210
#define PANEL_WINRECALLPOPUP    3215
#define PANEL_WINREXXMON        3220
#define PANEL_WINCASCADE        3225
#define PANEL_WINMAXALL         3230
#define PANEL_WINCLOSEALL       3235

/* ===== Wkplace.ipf ===== */
//
#define PANEL_WORKPLACE         3400
#define PANEL_WPNEW             3405
#define PANEL_WPLIST            3410
#define PANEL_WPPROPERTIES      3415
#define PANEL_WPSAVEAS          3420
#define PANEL_WPTEMPLATES       3425
//
#define PANEL_NEWWPDLG          3430
#define PANEL_WPLISTDLG         3435
#define PANEL_WPPROPDLG         3440
#define PANEL_WPTPLDLG          3445
#define PANEL_APPWPDLG          3450
#define PANEL_WPTPLADVDLG       3455

/* ===== XDS.IPF ===== */
//
#define PANEL_MAIN                3600 // F1 key
#define PANEL_CONCEPTS            3605
#define PANEL_CONCEPTSWORKPLACES  3610
#define PANEL_CONCEPTSTOOLS       3615
#define PANEL_CONCEPTSMACROS      3620
#define PANEL_CONCEPTSREXX        3625
#define PANEL_MAINMENU            3630

/* ===== OTHERS.IPF ===== */
//
#define PANEL_MDILISTDLG          3900
#define PANEL_SELDIRDLG           3905
//
#define PANEL_IDM_POP_FONT        4000
#define PANEL_IDM_POP_SORT        4005
#define PANEL_IDM_POP_DEFAULT     4010
#define PANEL_IDM_POP_UNSORTED    4015
#define PANEL_IDM_POP_COPY        4020
#define PANEL_IDM_POP_FULLNAMES   4025
#define PANEL_IDM_POP_SELFILES    4030
#define PANEL_IDM_POP_UNSELFILES  4035
#define PANEL_IDM_POP_INVSELFILES 4040
#define PANEL_IDM_POP_FILEFIND    4045
#define PANEL_IDM_POP_FILEINVSEL  4046
#define PANEL_IDM_POP_GROUPS      4050
#define PANEL_IDM_POP_AGROUP      4055
#define PANEL_IDM_POP_1GROUP      4060
#define PANEL_IDM_POP_2GROUP      4065
#define PANEL_IDM_POP_3GROUP      4070
#define PANEL_IDM_POP_4GROUP      4075
#define PANEL_IDM_POP_5GROUP      4080
#define PANEL_IDM_POP_6GROUP      4085
#define PANEL_IDM_POP_7GROUP      4090
#define PANEL_IDM_POP_8GROUP      4095
#define PANEL_IDM_POP_9GROUP      4100
#define PANEL_IDM_POP_GOBM        4105
#define PANEL_IDM_POP_SETBM       4110
#define PANEL_IDM_POP_WBBCLOSE    4115
#define PANEL_IDM_POP_WBBRESTORE  4120
#define PANEL_IDM_POP_WBBMAXIMIZE 4125
#define PANEL_IDM_POP_WBBHIDE     4130
#define PANEL_IDM_POP_WBBVTILE    4135
#define PANEL_IDM_POP_WBBHTILE    4140
#define PANEL_IDM_POP_MVGROUPS    4145
#define PANEL_IDM_POP_MV2GROUP    4150
#define PANEL_IDM_POP_WBFONT      4155
#define PANEL_IDM_POP_WBCOLORS    4160
#define PANEL_IDM_POP_WBWRAP      4165
#define PANEL_IDM_POP_UNSELALL    4170
#define PANEL_IDM_POP_NAME        4175
#define PANEL_IDM_POP_TYPE        4180
#define PANEL_IDM_POP_NEWFILE     4185
#define PANEL_IDM_POP_CLOSEWINDOW 4190

/*============================================================================================*/

#endif  /* ifndef __HELP2__ */

