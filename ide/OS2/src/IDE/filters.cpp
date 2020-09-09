/*
*
*  File: Filters.cpp
*
*  XDS Shell - includes from TOOLS.CPP
*
*  (c) 1997 by *FSA & NHA Inc.
*
*/

class FILTER
{
public:
           FILTER   ();
          ~FILTER   ();
  PSZ      Set      (PSZ pszFilters); // returns 0 or malloc()ed error description
  char     Test     (PSZ pszText);    // returns MSG_* char or 0
private:
  void     free_all ();
  struct ONEFILTER
  {
    // Filter:
    PSZ        pszREFilter;
    PSZ        pszREFile;
    PSZ        pszRELine;
    PSZ        pszREPosition;
    PSZ        pszREMessage;
    PSZ        pszREMessageKind;
    PSZ        apszREMsgKinds[4];
    HEXPR      hExpr;
    // Last test results:
    PSZ        pszFile;
    LONG       lLine, lPos;
    PSZ        pszMessage;
    CHAR       chMessageKind;
    ONEFILTER *next;
    ONEFILTER  ();
   ~ONEFILTER  ();
    void       free_all();
    PSZ        Set  (PSZ *ppszFilters);
    BOOL       Test (PSZ pszText);
  };
  typedef ONEFILTER *PONEFILTER;

  PONEFILTER pFilters;
};

FILTER::FILTER()  { memset(this,0,sizeof(*this)); }
FILTER::~FILTER() { free_all();                   }
PSZ FILTER::Set (PSZ pszFilters)
{
  PSZ pszErr;
  int nFlt   = 0;
  free_all();
  if (!pszFilters) return 0;
  PONEFILTER* ppf = &pFilters;
  while(1)
  {
    nFlt++;
    *ppf = new ONEFILTER;
    if ((pszErr=(*ppf)->Set(&pszFilters)))
    {
      delete *ppf;
      *ppf    = 0;
      if (pszErr==(PSZ)1L) return 0; // EOF
      PSZ psz = (PSZ)malloc(strlen(pszErr)+200);
      if (psz) sprintf(psz,"Error in the %u filter: %s",nFlt,pszErr);
      free(pszErr);
      return psz;
    }
    ppf = &(*ppf)->next;
  }
  return 0;
}
char FILTER::Test (PSZ pszText)
{
  for (PONEFILTER pf=pFilters; pf; pf=pf->next)
    if (pf->Test(pszText))
    {
      errOffice.AddError(pf->pszMessage,pf->lPos,pf->lLine,pf->pszFile,pf->chMessageKind,0);
      return pf->chMessageKind;
    }
  return 0;
}
void FILTER::free_all()
{
  for (PONEFILTER pf,pf1=pFilters; pf=pf1;)
  {
    pf1=pf->next;
    delete pf;
  }
  pFilters = 0;
}

FILTER::ONEFILTER::ONEFILTER()  {memset(this,0,sizeof(*this)); }
FILTER::ONEFILTER::~ONEFILTER() {free_all();                   }
void FILTER::ONEFILTER::free_all()
{
  free(pszREFilter);
  free(pszREFile);
  free(pszRELine);
  free(pszREPosition);
  free(pszREMessage);
  free(pszREMessageKind);
  for (int i=0; i<sizeof(apszREMsgKinds)/sizeof(apszREMsgKinds[0]); i++)
    free(apszREMsgKinds[i]);
  RCThunk_FreeExpr(hExpr);
  free(pszFile);
  free(pszMessage);
  ONEFILTER *p = next;
  memset(this,0,sizeof(*this));
  next = p;
}

static PSZ skipbl(PSZ psz)
{
  while(*psz==' ' || *psz=='\t') psz++;
  return psz;
}

PSZ get_quoted(PSZ *ppsz)
// После пробелов ожидает строку в кавычках, (можно с "" внутри).
// *ppsz установит на первый непробел после этой строки
// Незакрытая строка читается до перевода строки.
// При ошибке - вернет 0 и не изменит *ppsz, иначе - вернет malloc()ed строку.
{
  PSZ   psz    = skipbl(*ppsz);
  if (*psz != '"') return 0;
  psz++;
  int   nAlloc = 0;
  int   nUsed  = 0;
  PSZ   pszBuf = 0;
  while(1)
  {
    if (nAlloc<=nUsed && !(pszBuf=(PSZ)realloc(pszBuf, nAlloc = (nAlloc ? nAlloc*2 : 16))))
      return 0;
    char ch = pszBuf[nUsed] = *psz;
    if      (ch=='"' && psz[1]=='"') psz++;
    else if (!ch || ch=='"' || ch==CR || ch==LF) // End
    {
      pszBuf[nUsed] = '\0';
      if (ch=='"') psz = skipbl(psz+1);
      *ppsz = psz;
      return pszBuf;
    }
    psz++;
    nUsed++;
  }
}

static PSZ nxt_line(PSZ *ppsz)
{
  static PSZ pszRet = 0;
  PSZ        psz,psz1;
  int        l;
  free(pszRet);
  pszRet = 0;
  if (*ppsz)
  {
    psz    = sf_skipspaces(*ppsz);
    psz1   = strpbrk(psz,"\r\n");
    if (!psz1) psz1 = psz+strlen(psz);
    if ((l = psz1-psz) && (pszRet=(PSZ)malloc(l+1)))
    {
      strncpy(pszRet,psz,l);
      pszRet[l] = 0;
      *ppsz = psz1;
    }
  }
  if (pszRet) return(pszRet);
  else        throw ""; // EOF
}

PSZ FILTER::ONEFILTER::Set (PSZ *ppszFilters)
{
  // returns 0 or malloc()ed error description or 1L when EOF
  LONG   l;
  BOOL   fSetOk = FALSE;
  PSZ    pszLine,psz;
  free_all();
  if (!*ppszFilters) return PSZ(1L);
  try
  {
    // FILTER begin:
    while(sf_strnicmp(pszLine = nxt_line(ppszFilters), "Filter", 6));
    psz = sf_skipspaces(pszLine+6);
    if (*psz != '=') throw "'=' expected";
    //--- Filter body
    if (!(pszREFilter = get_quoted(&(psz+=1)))) throw "filter body expected";
    //--- Compile the filter body
    RCThunk_Compile(pszREFilter, &hExpr, &l);
    if (!hExpr) throw "wrong filter expression";
    //--- Rulezzz:
    while(1)
    {
      fSetOk = TRUE;
      PSZ *ppszTarg;
      int  nSkip;
      pszLine = nxt_line(ppszFilters);
      if      (!sf_strnicmp(pszLine,"File",        4)) ppszTarg = &pszREFile,        nSkip=4;
      else if (!sf_strnicmp(pszLine,"Line",        4)) ppszTarg = &pszRELine,        nSkip=4;
      else if (!sf_strnicmp(pszLine,"Position",    8)) ppszTarg = &pszREPosition,    nSkip=8;
      else if (!sf_strnicmp(pszLine,"MessageKind",11)) ppszTarg = &pszREMessageKind, nSkip=11;
      else if (!sf_strnicmp(pszLine,"Message",     7)) ppszTarg = &pszREMessage,     nSkip=7;
      else                                         return 0;
      psz = sf_skipspaces(pszLine+nSkip);
      if (*psz != '=') throw "'=' expected";

      *ppszTarg = get_quoted(&(psz+=1));
      if (ppszTarg == &pszREMessageKind && *ppszTarg)
        for (l=0; l<sizeof(apszREMsgKinds)/sizeof(apszREMsgKinds[0]); l++)
          apszREMsgKinds[l] = get_quoted(&(psz));
    }
  }
  catch(char *szEr)
  {
    if (szEr)
    {
      if (!szEr[0]) return PSZ(fSetOk ? 0 : 1L); //ok / EOF
      free_all();
      PSZ pszRC = 0;
      if (pszRC = (PSZ)malloc(strlen(pszLine)+100))
      {
        sprintf(pszRC,"%s (source line: %s)",szEr,pszLine);
        #if DBGLOG
          fprintf(stderr,"Filter compile error: %s\n",pszRC);
        #endif
      }
      return pszRC;
    }
    return sf_mallocstr("Not fatal C++ compiler error");
  }
}

static int calc_pos(PSZ psz)
{
  PSZ  psz1;
  int  res  = 0;
  int  n;
  while (n=strtol(psz,&psz1,0), psz1!=psz)
  {
    res += n;
    psz = psz1;
  }
  return res;
}

BOOL FILTER::ONEFILTER::Test (PSZ pszText)
{
 #define BUFLEN 2000
  char    pszBuf[BUFLEN+1];
  int     i;
  if (!hExpr || !pszText || !RCThunk_Match (hExpr, pszText)) return FALSE;

  // File ($-variables alloved):
  RCThunk_Substitute(hExpr, pszText, pszREFile, pszBuf, BUFLEN);
  free(pszFile);
  pszFile = OpenExprVar(pszBuf);
  if (!pszFile[0]) free(pszFile), pszFile=0;

  // Message;
  RCThunk_Substitute(hExpr, pszText, pszREMessage, pszBuf, BUFLEN);
  free(pszMessage);
  pszMessage = sf_mallocstr(pszBuf);
  if (!pszMessage[0]) free(pszMessage), pszMessage=0;

  // Message kind
  chMessageKind = MSG_ERROR; /* 'E' */
  RCThunk_Substitute(hExpr, pszText, pszREMessageKind, pszBuf, BUFLEN);
  for (i=0; i<sizeof(apszREMsgKinds)/sizeof(apszREMsgKinds[0]); i++)
    if (apszREMsgKinds[i] && RCThunk_FastMatch(apszREMsgKinds[i],pszBuf))
    {
      char ach[] = {MSG_ERROR, MSG_WARNING, MSG_NOTICE, MSG_SEVERE};
      chMessageKind  = ach[i];
      break;
    }

  // Line,position:
  RCThunk_Substitute(hExpr, pszText, pszRELine,     pszBuf, BUFLEN);
  lLine = calc_pos(pszBuf);
  RCThunk_Substitute(hExpr, pszText, pszREPosition, pszBuf, BUFLEN);
  lPos  = calc_pos(pszBuf);

  return TRUE;
}

