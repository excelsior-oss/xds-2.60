/*
*
*  File: hlt_mod.cpp
*
*  XDS IDE: syntax hilighter for MOD & DEF files
*
*  (c) 1996 by *FSA & NHA Inc.
*
*/

#define  INCL_DOS
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "..\hltdll.h"
#include "..\sf_lib.h"

#pragma export(ClInit,            "ClInit",           1)
#pragma export(ClQueryMap,        "ClQueryMap",       2)
#pragma export(ClQueryExText,     "ClQueryExText",    3)
#pragma export(ClQueryEntryInfo,  "ClQueryEntryInfo", 4)
#pragma export(ClLineLevel,       "ClLineLevel",      5)
#pragma export(ClClrLine,         "ClClrLine",        6)
#pragma export(ClDescription,     "ClDescription",   10)

#define INIFILENAME "xdsmod.wrd"

#define TAG_DIGIT    0x0001
#define TAG_LETTER   0x0002
#define TAG_HEX      0x0004
#define TAG_HEXDIGIT 0x0008
#define TAG_BLANK    0x0010
#define TAG_COMBEG   0x0020
#define TAG_COMEND   0x0040
#define TAG_COMEOL   0x0080
#define TAG_RESWBEG  0x0100
#define TAG_PRGBEG   0x0200
#define TAG_PRGEND   0x0400

//// Globals:
//
CLM_MAPTYPE clm_aclMap;
ULONG               aChTags[256];   // TAG_... tags for each character
LINELIST           *pllComEol;      // -
LINELIST           *pllComBegs;     // - Lists of
LINELIST           *pllComEnds;     // -  lexems (from xds.wrd)
LINELIST           *pllResWords;    // -
LINELIST           *pllPrgBegs;     //
LINELIST           *pllPrgEnds;     //


//// Our color definitions:
//
#define CLP_NUMBER    3   //   numerical constants
#define CLP_RESWORD   4   //   reserwed words
#define CLP_TEXTLINE  5   //   quoted constants
#define CLP_COMMENT   6   //   comments
#define CLP_PRAGMA    7   //   pragma

/*=== CLM_ELEMTYPE *ClmQueryMap() =====================================================/
/                                                                                      /
/  Return value                     - address of the map                               /
/                                                                                      /
/-------------------------------------------------------------------------------------*/
CLM_ELEMTYPE* APIENTRY ClQueryMap()
{
  return clm_aclMap;
}


/*=== PSZ ClQueryExText() =============================================================/
/                                                                                      /
/  Return value - text for the color set dialog. '%' chars shows the selection         /
/                 begin and end (=cursor) positions, must be changed to ' '            /
/                                                                                      /
/-------------------------------------------------------------------------------------*/
PSZ APIENTRY ClQueryExText()
{
  return
  "(* Modula2 text *) \n"
  "<*+M2EXTENSIONS*>\n"
  "-- %Selection% \n"
  "FOR I:=1 TO 100 DO\n"
  "   WrStr(\"String \");\n"
  "END;\n";
}

/*=== BOOL ClQueryEntryInfo(ULONG ulEnt, CLP_PRENTRYINFO prEntInf); ===================/
/                                                                                      /
/  Query the palette entryes info to represent them for palette editor.                /
/                                                                                      /
/  ULONG           ulEnt   (in)     - order of the item to show in the palette editor  /
/  CLP_PRENTRYINFO prEntInf(in/out) - pointer to the structure                         /
/  Return value                     - FALSE if ulEnt is too large                      /
/-------------------------------------------------------------------------------------*/
BOOL APIENTRY ClQueryEntryInfo(ULONG ulEnt, CLP_PRENTRYINFO prEntInf)
{
  CLP_RENTRYINFO ar[] =
  {
    {"Comments",                  CLP_COMMENT,   FALSE },
    {"Pragmas",                   CLP_PRAGMA,    FALSE },
    {"Reserved words",            CLP_RESWORD,   FALSE },
    {"Numerical constants",       CLP_NUMBER,    FALSE },
    {"Quoted texts",              CLP_TEXTLINE,  FALSE }
  };
  if (ulEnt >= (sizeof(ar)/sizeof(ar[0]))) return FALSE;
  memcpy(prEntInf, &ar[ulEnt], sizeof(ar[0]));
  return TRUE;
}



/*-------- Token recognizing ------------------------*/

inline char *cl_findlist(LINELIST *pll, char *pch)
{
  while(pll)
  {
    if (!strncmp(pch,pll->text,pll->textlen))
      return pch + pll->textlen;
    pll = pll->next;
  }
  return 0;
}
inline char *cl_is_eolcomment(char *pch) { return cl_findlist(pllComEol,  pch);}
inline char *cl_is_resword(char *pch)    { return cl_findlist(pllResWords,pch);}
inline char *cl_is_begcomment(char *pch) { return cl_findlist(pllComBegs, pch);}
inline char *cl_is_endcomment(char *pch) { return cl_findlist(pllComEnds, pch);}
inline char *cl_is_begpragma(char *pch)  { return cl_findlist(pllPrgBegs, pch);}
inline char *cl_is_endpragma(char *pch)  { return cl_findlist(pllPrgEnds, pch);}

/*=== CLC_COMLEVELTYPE ClLineLevel(PSZ pszLine); =============================/
/                                                                             /
/  Calculate comlevel change for the line                                     /
/                                                                             /
/  PSZ pszLine (in) - address of the 0-terminated string                      /
/  Return value     - comlevel change value (f.ex. "(* comment *) *)" -> -1)  /
/                                                                             /
/----------------------------------------------------------------------------*/
CLC_COMLEVELTYPE APIENTRY ClLineLevel(PSZ pch)
// It must be as fast as it is possible
{
  CLC_COMLEVELTYPE  cl = 0;
  ULONG             ulTags;
  char             *pch1;
  char              ch;
  pch--;
  while(ch=*++pch)
  {
    // Note: EOL comments processes NOT fully correctly.
    //
    // EOL comment must hide comment open bracket, but in this case it will
    // hide comment close bracket and this is the worst bug.
    // (So " -- (* comment" opens a new comment... :-( )
    //
    if ((ulTags=aChTags[ch]) & TAG_COMBEG && (pch1 = cl_is_begcomment(pch)))
    {
      pch = pch1-1;
      cl++;
      continue;
    }
    if (ulTags & TAG_COMEND && (pch1 = cl_is_endcomment(pch)))
    {
      pch = pch1-1;
      cl--;
      continue;
    }
  }
  return cl;
}


/*=== CLC_COMLEVELTYPE ClClrLine(PSZ *pszLine, CLC_COMLEVELTYPE cmComLevel); ==============/
/                                                                                         /
/  Filling the global map (clm_aclMap) for the line                                       /
/                                                                                         /
/  PSZ              pszLine    (in) - address of the 0-terminated string                  /
/  CLC_COMLEVELTYPE cmComLevel (in) - comment level before this line                      /
/  Return value                     - comment level after this line                       /
/                                                                                         /
/----------------------------------------------------------------------------------------*/
CLC_COMLEVELTYPE APIENTRY ClClrLine(PSZ pch, CLC_COMLEVELTYPE cmComLevel)
{
  LONG          i,lMapPos=0;
  char          ch,*pch1;
  ULONG         tags;

  while(1)
  {
    // ----- Parse comment ----- //
    if (cmComLevel)
    {
      i=0; // Total commented length counter

      while(2)
      {
        tags = aChTags[ch = *pch];
        if (!ch)
        {
          cl_FillMap(clm_aclMap,lMapPos,CLM_MAPLEN,CLP_COMMENT);
          return cmComLevel;
        }

        if (tags & TAG_COMEOL && cl_is_eolcomment(pch))
        {
          cl_FillMap(clm_aclMap,lMapPos,CLM_MAPLEN,CLP_COMMENT);
          return cmComLevel;
        }
        if (tags & TAG_COMBEG && (pch1 = cl_is_begcomment(pch)))
        {
          cmComLevel++;   // Nested comment begin
          i  += pch1-pch;
          pch = pch1;
          continue; //while(2)
        }
        if (tags & TAG_COMEND)
        {
          if (pch1 = cl_is_endcomment(pch))
          {
            i += pch1-pch;
            pch = pch1;
            if (--cmComLevel) continue;         // while(2);
            cl_FillMap(clm_aclMap,lMapPos,i,CLP_COMMENT);  // Comment(s) closed
            lMapPos += i;
            break;                              // break while(2);
          }
        }

        pch++;
        i++;
      } // while(2)
      continue; // while(1)
    } // if (cmComLevel)

    //-- There is no comment now (*pComLevel==0)

    tags = aChTags[ch = *pch];

    if (!ch)  // EOL: return
    {
      cl_FillMap(clm_aclMap,lMapPos,CLM_MAPLEN,CLP_DEFAULT);
      return cmComLevel;
    }

    if (ch=='\'' || ch == '\"')
    {
      i = 1;
      while(pch[i]!=ch && pch[i]) i++;
      i++;
      cl_FillMap(clm_aclMap,lMapPos,i,CLP_TEXTLINE);
      lMapPos += i;
      pch     += i;
      continue;
    }

    if (tags & TAG_PRGBEG && (pch1=cl_is_begpragma(pch)))
    {
      char ch1, *pch2=0;
      pch1--;
      while(ch1 = *++pch1)
        if (aChTags[ch1]&TAG_PRGEND && (pch2=cl_is_endpragma(pch1)))
          break;
      int i = pch2 ? pch2-pch : pch1-pch;
      cl_FillMap(clm_aclMap,lMapPos,i,CLP_PRAGMA);
      lMapPos += i;
      pch     += i;
      continue;
    }


    if (tags & TAG_COMEOL && cl_is_eolcomment(pch))
    {
      cl_FillMap(clm_aclMap,lMapPos,CLM_MAPLEN,CLP_COMMENT);
      return cmComLevel;
    }
    if (tags & TAG_COMBEG && (pch1 = cl_is_begcomment(pch)))
    {
      cmComLevel++;
      cl_FillMap(clm_aclMap,lMapPos,pch1-pch,CLP_COMMENT);
      lMapPos += pch1-pch;
      pch      = pch1;
      continue; // To enter while(2);
    }
    if (tags & TAG_DIGIT)
    {
      i         = 0;
      ULONG hex = 0;
      // note: ###C values (CHAR type) looks like HEX values
      while(hex|=(tags = aChTags[ch = pch[i]]), tags&TAG_HEXDIGIT) i++;
      if      ( ch=='H' || ch=='h')          i++;  // Hex
      else if ( ch=='.' && !(hex & TAG_HEX))       // Real...
      {
        i++;
        while(tags = aChTags[ch = pch[i]], tags&TAG_DIGIT) i++;
        if (ch=='e' || ch=='E' || ch=='d' || ch=='D')
        {
          if ((ch = pch[++i])=='-' || ch=='+') i++;
          while(tags = aChTags[ch = pch[i]], tags&TAG_DIGIT) i++;
        }
      }
      cl_FillMap(clm_aclMap,lMapPos,i,CLP_NUMBER);
      lMapPos += i;
      pch     += i;
      continue;
    }

    if (tags&TAG_LETTER)
    {
      if (tags & TAG_RESWBEG && (pch1 = cl_is_resword(pch)) &&
          !(aChTags[*pch1]&(TAG_DIGIT|TAG_LETTER))
         )
      {
        cl_FillMap(clm_aclMap,lMapPos,pch1-pch,CLP_RESWORD);
        lMapPos += pch1-pch;
        pch      = pch1;
      }
      else
      {
        i = 1;
        while(aChTags[pch[i]] & (TAG_LETTER|TAG_DIGIT)) i++;
        cl_FillMap(clm_aclMap,lMapPos,i,CLP_DEFAULT);
        lMapPos += i;
        pch     += i;
      }
      continue;
    }
    else
    {
      if (lMapPos<CLM_MAPLEN) clm_aclMap[lMapPos] = CLP_DEFAULT;
      lMapPos++;
      pch++;
      continue;
    }
  } // while(1)
}

/*=== PSZ ClDescription (int nItem) ======================================================/
/                                                                                         /
/  Descripts the highlighter                                                              /
/                                                                                         /
/  int              nItem      (in) - what to describe                                    /
/                                     0 - internal name (f.ex. "CPP")                     /
/                                     1 - description   (f.ex. "C++ files")               /
/  Return value                     - malloc()ed string (or 0 when error)                 /
/                                                                                         /
/----------------------------------------------------------------------------------------*/
PSZ APIENTRY ClDescription(int nItem)
{
  switch (nItem)
  {
    case 0: return "MOD2";
    case 1: return "Modula2 files";
  }
  return 0;
}

/* --- ClInit: ---------------------------------------*/

void cl_initags(ULONG ctg, LINELIST *pll)
{
  while(pll)
  {
    aChTags[pll->text[0]] |= ctg;
    pll = pll->next;
  }
}

/*=== PSZ ClInit(PSZ pszPath); ========================================================/
/                                                                                      /
/  Initialise the parser using pszPath to find it's configuration file.                /
/  Ignores all the calls except first.                                                 /
/                                                                                      /
/  PSZ  pszPath (in) - path used to search the configuration file                      /
/  Return value      - 0 when Ok or a pointer to malloc()ed error description line.    /
/-------------------------------------------------------------------------------------*/

PSZ APIENTRY ClInit(PSZ pszPath)
{
  HFILE         hf;
  ULONG         ulAction;
  BOOL          fErr;
  char          ch;
  char          szFile[CCHMAXPATH];

  sprintf(szFile,"%s\\%s",pszPath,INIFILENAME);

  pllResWords = 0;
  pllComBegs  = 0;
  pllComEnds  = 0;
  pllComEol   = 0;
  pllPrgBegs  = 0;
  pllPrgEnds  = 0;

  /* --- Set char tags --- */
  memset(aChTags,0,sizeof(aChTags));
  for (ch='0'; ch<='9'; ch++) aChTags[ch] |= (TAG_DIGIT | TAG_HEXDIGIT);
  for (ch='A'; ch<='F'; ch++)
  {
    aChTags[ch]         |= (TAG_HEXDIGIT | TAG_HEX);
    aChTags[ch-'A'+'a'] |= (TAG_HEXDIGIT | TAG_HEX);
  }
  for (ch='A'; ch<='Z'; ch++)
  {
    aChTags[ch]         |= TAG_LETTER;
    aChTags[ch-'A'+'a'] |= TAG_LETTER;
  }
  aChTags['_']  |= TAG_LETTER;
  aChTags[' ']  |= TAG_BLANK;
  aChTags['\t'] |= TAG_BLANK;

  /* --- Read elements to color --- */
  fErr = (BOOL)DosOpen(szFile,&hf,&ulAction,0,FILE_NORMAL,FILE_OPEN,
         OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE,(PEAOP2)0);
  if (fErr)
  {
    PSZ psz = (PSZ)malloc(CCHMAXPATH+100);
    sprintf(psz,"Can't open file: '%s'.",szFile);
    return psz;
  };
  pllResWords = sf_ini_read(hf,"ResWords");
  pllComBegs  = sf_ini_read(hf,"CommBeg");
  pllComEnds  = sf_ini_read(hf,"CommEnd");
  pllComEol   = sf_ini_read(hf,"CommEOL");
  pllPrgBegs  = sf_ini_read(hf,"PragmaBeg");
  pllPrgEnds  = sf_ini_read(hf,"PragmaEnd");
  DosClose(hf);
  cl_initags(TAG_RESWBEG,pllResWords);
  cl_initags(TAG_COMBEG, pllComBegs);
  cl_initags(TAG_COMEOL, pllComEol);
  cl_initags(TAG_COMEND, pllComEnds);
  cl_initags(TAG_PRGBEG, pllPrgBegs);
  cl_initags(TAG_PRGEND, pllPrgEnds);
  return 0;
}








