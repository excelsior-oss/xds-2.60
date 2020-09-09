/*
*
*  File: hltdll.h (cl_)
*
*  Color syntax highlighter(s) interface
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/
#ifndef __TED_CL__
  #define __TED_CL__
  #include <os2.h>
  #include <string.h>
  #include <stdlib.h>

  /*=== #pragma export(ClInit, "ClInit",1) ==============================================/
  /                                                                                      /
  /  Initialise the parser using pszPath to find it's configuration file.                /
  /  Ignores all the calls except first.                                                 /
  /                                                                                      /
  /  PSZ  pszPath (in) - path used to search the configuration file                      /
  /  Return value      - 0 when Ok or a pointer to malloc()ed error description line.    /
  /-------------------------------------------------------------------------------------*/
  typedef PSZ (APIENTRY CL_INIT)(PSZ pszPath);
  typedef               CL_INIT *CL_PINIT;

  //--------------------------  Palette definitions (CLP_*)  --------------------------------//
  //
  // Palette is a set of the color pairs used to show a text elements. Each pair consists of
  //   tho ULONG values for fore- and background of the text. The high byte if the value used
  //     to contain an additional tags determines a font characteristics, three low bytes
  //       contains a usual RGB value.
  // Numeral colors means the same anyway. It represents 'default' colors (used when the parser
  //   is turned off of for the text elements are not recognized by the parser), 'selection' color
  //     (used to show the selection in the text) and the color used for hilighting the current
  //       line. By the way it is only the common agreements are importrnt for drawing program
  //         but the parser exports only the information interesting for the palette editor and
  //           all the interpretation of the palette contents is not parser-depended. It returns
  //             only the indexes in the palette.
  //

  #define CLP_FOREGROUND        0
  #define CLP_BACKGROUND        1
  #define CLP_PALETTELEN        32

  #define CLP_CURLINE           0   // This palette entryes means the predefault
  #define CLP_SELECTION         1   //   items and can have different backgrounds. All
  #define CLP_DEFAULT           2   //     the other entryes has a syntax-depanded means.

  #define CLP_UNDERLINETAG      0x80000000 // All the foreground color values in the palette can
  #define CLP_BOLDTAG           0x40000000 //   have this additional tags in the high byte to
  #define CLP_ITALICTAG         0x20000000 //     manage the font characteristics.
  #define CLP_COLORMASK         0x00ffffff // Mask of the RGB part of the values.

  typedef ULONG                 CLP_PALETTETYPE [CLP_PALETTELEN][2]; // <----- Palette type

  struct CLP_RENTRYINFO
  {
    // The parser 'knows' its own color names (depands on a language been parsed)
    //   and returns its names and indexes in the palette. This indexes used while
    //     parsing (expext the indexes used to represent the selection and the
    //       current line. But it enumerates them via ClQueryEntryInfo to.)
    //
    char  szName[100];     // Name of the item     (f.ex. 'Reserved words')
    ULONG idx;             // Index in the palette (.LT. CLP_PALETTELEN)
    ULONG fBackground;     // Background/foreground. The 'default' background must be breed.
    CLP_RENTRYINFO *next;  // DLL does not uses this field
  };
  typedef CLP_RENTRYINFO *CLP_PRENTRYINFO;


  /*=== #pragma export(ClQueryEntryInfo, "ClQueryEntryInfo",4) ==========================/
  /                                                                                      /
  /  Query the palette entryes info to represent them for palette editor.                /
  /    It doesn't returnt the 'selection' and 'current line' foreground because of       /
  /      the painting program doesn't needs it.                                          /
  /                                                                                      /
  /  ULONG           ulEnt   (in)     - order of the item to show in the palette editor  /
  /  CLP_PRENTRYINFO prEntInf(in/out) - pointer to the structure                         /
  /  Return value                     - FALSE if ulEnt is too large                      /
  /-------------------------------------------------------------------------------------*/
  typedef BOOL (APIENTRY CLP_QUERYENTRYINFO)(ULONG ulEnt, CLP_PRENTRYINFO prEntInf);
  typedef                CLP_QUERYENTRYINFO *CLP_PQUERYENTRYINFO;


  //--------------------------  Color map definitions (CLM_*)  --------------------------------//
  //
  // The color map is an array of CLM_ELEMTYPE values. Each value correspondes to the symbol in the
  //   text line and contains an index in the palette. The map's length is limited (up to CLM_MAPLEN)
  //     so only first CLM_MAPLEN symbols of line can has a different colors.
  //

  typedef char          CLM_ELEMTYPE;
  #define CLM_MAPLEN    500
  typedef CLM_ELEMTYPE  CLM_MAPTYPE [CLM_MAPLEN];

  /*=== #pragma export(ClQueryMap, "ClQueryMap",2) ======================================/
  /                                                                                      /
  /  Return value                     - address of the map                               /
  /                                                                                      /
  /-------------------------------------------------------------------------------------*/
  typedef CLM_ELEMTYPE *(APIENTRY CLM_QUERYMAP)();
  typedef                         CLM_QUERYMAP *CLM_PQUERYMAP;


  /*=== #pragma export(ClQueryExText, "ClQueryExText",3) ================================/
  /                                                                                      /
  /  Return value - text for the color set dialog. '%' chars shows the selection         /
  /                 begin and end (=cursor) positions, must be changed to ' '            /
  /                                                                                      /
  /-------------------------------------------------------------------------------------*/
  typedef PSZ (APIENTRY CLM_QUERYEXTEXT)();
  typedef               CLM_QUERYEXTEXT *CLM_PQUERYEXTEXT;


  //--------------------------  Comment levels (CLC_*)  ---------------------------------------//
  //
  // The parser can understand a nested comments. The 'comment level' means a signed value represents
  //   the changing of this nesting during the tested line. It increased by 1 for each opened coment
  //     and decreased for closed. The next section defines something to this.
  //

  typedef signed char   CLC_COMLEVELTYPE;  // Value to represent it

  /*=== #pragma export(ClLineLevel, "ClLineLevel",5) ===========================/
  /                                                                             /
  /  Calculate comlevel change for the line                                     /
  /                                                                             /
  /  PSZ pszLine (in) - address of the 0-terminated string                      /
  /  Return value     - comlevel change value (f.ex. "(* comment *) *)" -> -1)  /
  /                                                                             /
  /----------------------------------------------------------------------------*/
  typedef CLC_COMLEVELTYPE (APIENTRY CLC_LINELEVEL)(PSZ pszLine);
  typedef                            CLC_LINELEVEL *CLC_PLINELEVEL;


  //--------------------------  Color determine (CLC_*)  ---------------------------------------//
  //
  // The next part describes the methods used to buidl a color map for line during painting
  //   the text.
  //

  /*=== #pragma export(ClClrLine, "ClClrLine",6) ===========================================/
  /                                                                                         /
  /  Filling the global map (clm_aclMap) for the line                                       /
  /                                                                                         /
  /  PSZ              pszLine    (in) - address of the 0-terminated string                  /
  /  CLC_COMLEVELTYPE cmComLevel (in) - comment level before this line                      /
  /  Return value                     - comment level after this line                       /
  /                                                                                         /
  /----------------------------------------------------------------------------------------*/
  typedef CLC_COMLEVELTYPE (APIENTRY CLC_CLRLINE)(PSZ pszLine, CLC_COMLEVELTYPE cmComLevel);
  typedef                            CLC_CLRLINE *CLC_PCLRLINE;


  /*=== #pragma export(ClDescription, "ClDescription",10) ==================================/
  /                                                                                         /
  /  Descripts the highlighter                                                              /
  /                                                                                         /
  /  int              nItem      (in) - what to describe                                    /
  /                                     0 - internal name (f.ex. "CPP")                     /
  /                                     1 - description   (f.ex. "C++ files")               /
  /  Return value                     - pointer to the string (or 0 when error)             /
  /                                                                                         /
  /----------------------------------------------------------------------------------------*/
  typedef PSZ (APIENTRY CLC_DESCRIPTION)(int nItem);
  typedef               CLC_DESCRIPTION *CLC_PDESCRIPTION;

  /* --- cl_FillMap ----------------------------------------------/
  /
  /  Filling the map checking the parameters boundaries
  /
  /  CLM_MAPTYPE   clm (in/out) - the map to fill
  /  ULONG         x   (in)     - 1-st filling position
  /  ULONG         l   (in)     - number of positions to fill
  /  CLM_ELEMTYPE  cl  (in)     - index in the palette to fill
  /
  /--------------------------------------------------------------*/
  inline void cl_FillMap(CLM_MAPTYPE clm, ULONG x, ULONG l, CLM_ELEMTYPE cl)
  {
    if (x<CLM_MAPLEN)
      memset(clm + x, cl, min(l,CLM_MAPLEN-x));
  }

  #ifdef _DLL_USER_
    #include "sf_lib.h"
    #include <string.h>
    #include <stdlib.h>
    #include "redir.h"

    /*===================== The next methods are used to access the parser(s) ======================*/

      #define INI_APP      "XDS"      // OS2.INI: 'XDS' <--
      #define INI_PAINTKEY "XDS_HLT_" // OS2.INI: XDS/XDS_HLT_* - the painters
      #define PAINTIDLEN   10

      class PAINTER
      {
      public:
      /*
        PSZ                (*PClInit)            (PSZ pszPath);
        BOOL               (*PClQueryEntryInfo)  (ULONG ulEnt, CLP_PRENTRYINFO prEntInf);
        CLC_COMLEVELTYPE   (*PClLineLevel)       (PSZ pszLine);
        CLC_COMLEVELTYPE   (*PClClrLine)         (PSZ *pszLine, CLC_COMLEVELTYPE cmComLevel);
        CLM_QUERYMAP       (*PClQueryMap)        ();
        PSZ                (*PClQueryExText)     ();
        PSZ                (*PClDescription)     (int nItem);
      */
        CL_PINIT            PClInit;
        CLP_PQUERYENTRYINFO PClQueryEntryInfo;
        CLC_PLINELEVEL      PClLineLevel;
        CLC_PCLRLINE        PClClrLine;
        CLM_PQUERYMAP       PClQueryMap;
        CLM_PQUERYEXTEXT    PClQueryExText;
        CLC_PDESCRIPTION    PClDescription;
        char                szNameId[PAINTIDLEN];
        PSZ                 pszDescription;
        HMODULE             hmDll;
        PAINTER            *next;
         PAINTER();
        ~PAINTER();
      };
      typedef PAINTER *PPAINTER;

      struct ASSTR
      {
        CLP_PALETTETYPE   aaRGB;
        char              szPainterId[PAINTIDLEN];
        FATTRS            fAttrs;
        BOOL              fHilite;
        BOOL              fHiliteCurLine;
        ULONG             ulTabWidth;
        BOOL              fSmartTabs;
        BOOL              fAutoident;
        BOOL              fPasteRepSel;
        BOOL              fUseTabChars;
        BOOL              fDefault; // Совпадает всегда (должен быть! в глобальном листе)
        ASSTR            *next;
        void              Assign      (ASSTR *pAss);
        void              SetFiles    (PSZ sz);
        PSZ               QueryFiles  () {return pszFiles;}
        ASSTR             (BOOL fDef=FALSE);
       ~ASSTR             ();
      private:
        PSZ               pszFiles;
        friend ASSTR     *r_prf_asslist(LONG,BOOL);
      };
      typedef ASSTR *PASSTR;

      class PAINTEROFFICE
      {
      public:
        void             Init           (HWND hMsgWnd);
        PPAINTER         GetPainterList () {return pPainterList;}
        PPAINTER         LoadPainter    (PSZ szNameId, HWND hMsgWnd);
        PASSTR           QueryAss       (PSZ szFilename);
        PASSTR           QueryAssList   (BOOL fGlobal)  {return fGlobal ? pGlobalAssList : pLocalAssList;}
        void             ApplyAss       (HWND hEd, PASSTR pAss);
        void             SetNewAssList  (BOOL fGlobal, PASSTR pList, BOOL fApply); // The list willn't be duplicated, don't deallocate it!
        void             RProfile       (LONG slot, BOOL fLocalOnly=FALSE);
        void             WProfile       (LONG slot);
        PAINTEROFFICE();
       ~PAINTEROFFICE();
      private:
        PPAINTER  pPainterList;
        PASSTR    pGlobalAssList;
        PASSTR    pLocalAssList;
        PASSTR   *find_ass(PSZ szFilename);
      };

    /*===================================================================================*/

  #endif /* ifdef _DLL_USER_ */

#endif /* ifndef __TED_CL__ */


