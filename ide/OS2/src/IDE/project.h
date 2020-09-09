/*
*
*  File: PROJECT.H
*
*  XDS Shell - project: open & edit features
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#ifndef __PROJECT__
#define __PROJECT__

BOOL   NewProject    ();

/* OpenProject:
>  PSZ pszFile (in) - 0           - browse
>                     ""          - нет проекта
>                     <file name> - поставить его как проект
>  Return value: TRUE если проект поменялся
*/
BOOL   OpenProject   (PSZ pszFile, HWND hMsgWnd);

//// Открыть редактор с файлом проекта
//
BOOL   EdProjectFile ();

//// Собственно редактирование проекта 'в картинках'
//
// hMsgWnd   - родительское окно для диалога(ов)
// szPrjFile - полное имя файла проекта
// szPrtFile - полное имя *.prt файла = темплита диалога
// Returns   - TRUE если файл проекта изменился
//
BOOL   PrjEdit(HWND hMsgWnd, PSZ szPrjFile, PSZ szPrtFile);

#endif /* ifndef __PROJECT__ */

