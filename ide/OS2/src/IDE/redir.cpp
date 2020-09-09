///////////////// file: redir.cpp

#define  INCL_DOS
#define  INCL_WIN
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "xdsbase.h"
#include "sf_lib.h"



/*==================================================================================================*/
/*========                                                                                   =======*/
/*======                             M O D U L E   R e g C o m p                               =====*/
/*========                                                                                   =======*/
/*==================================================================================================*/

/** Copyright (c) 1993 xTech Ltd, Russia. All Rights Reserved. */
/** Utility library: regular expressions */
//IMPLEMENTATION MODULE RegComp; (* Leo 07-Oct-89 *)
//                               (* O2:   Ned 10-Sep-92/01-Mar-93 *)
//                               (* C++: *FSA Nov-97 *)
//

class SET256
{
public:
//  SET                () {for (int i=0; i<8; i++) aul[i] = 0l;}
  void incl  (USHORT n) {if (n<=255) aul[n/32] |=  (0x01L << n%32); }
  void excl  (USHORT n) {if (n<=255) aul[n/32] &= ~(0x01L << n%32); }
  BOOL in    (USHORT n) { return (n<=255 && (aul[n/32] & (0x01L << n%32))); }
  void inverse       () {for (int i=0; i<8; i++) aul[i] ^= ~0L;}
private:
  ULONG    aul[8];
};

struct RESULT
{
  int pos[10];
  int len[10];
};
typedef RESULT *PRESULT;

class EXPR
{
public:
  EXPR(int kind);
 ~EXPR();
  int      nres;
  RESULT   res;
  int      kind;
  EXPR    *left; /* right=next */
  EXPR    *next;
  PSZ      pat;
  SET256   leg;
};
typedef EXPR   *PEXPR;
typedef PEXPR  *PPEXPR;


#define str     0 // string
#define any     1 // "?"
#define set     2 // "[]"
#define bra     3 // "{}"
#define seq     4 // "*"
#define and     5 // "&"
#define or      6 // "|"
#define not     7 // "^"
#define par     8 // "()"

#define ok      0
#define badParm 1


//--- GLOBALS
int res;
int i;
PSZ szExpr;
int len_szExpr;



EXPR::EXPR(int kind)
{
  memset(this,0,sizeof(EXPR));
  this->kind = kind;
}
EXPR::~EXPR()
{
  free(pat);
  delete left;
  delete next;
}


void app(PPEXPR reg, PEXPR r)
// appends r to reg
{
  PEXPR t;
  if (!*reg) *reg = r;
  else
  {
    t = *reg;
    while (t->next) t = t->next;
    if ((t->kind==seq) && (r->kind==seq))
    {
      res = badParm;
      delete r;
    }
    else
      t->next = r;
  }
}

PEXPR app_new(PPEXPR reg, int kind)
{
  PEXPR n    = new EXPR(kind);
  app(reg,n);
  return (res==ok) ? n : 0;
}

void dollar(PEXPR n)
{
  char ch;
  if (res==ok && i<len_szExpr-1 && szExpr[i]=='$')
  {
    ch = szExpr[i+1];
    if ('0'<=ch && ch<='9')
    {
      n->nres = ch-'0';
      i+=2;
    }
    else
      res = badParm;
  }
}

char char_code(char c, int *pj)
{
  c -= '0';
  for (int i=0; i<=1; i++)
  {
    if (*pj<len_szExpr && '0'<=szExpr[*pj] && szExpr[*pj]<='7')
    {
      c = c*8 + szExpr[*pj] - '0';
      *pj++;
    }
    else
    {
      res = badParm;
      return c;
    }
  }
  return c;
}

char esc(int *pj)
{
  char c = szExpr[++*pj];
  ++*pj;
  return ('0'<=c && c<='7') ? char_code(c,pj) : c;
}

  fill_set_incl (PEXPR n, char ch, char from, BOOL *prange)
  {
    if (!*prange)
      n->leg.incl(ch);
    else
    {
      if (from>ch) res = badParm;
      else
      {
        while(from<=ch)
          n->leg.incl(from++);
        *prange = FALSE;
      }
    }
  }
void fill_set(PEXPR n)
{
  char from  = 0;
  char q,ch  = 0;
  BOOL range = FALSE;
  BOOL inv;

  if (res!=ok) return;
  if (szExpr[i]=='[') q = ']';
  else                q = '}', n->kind = bra;
  i++;
  if (inv = (i<len_szExpr) && (szExpr[i]=='^')) i++;
  if (i<len_szExpr && (szExpr[i]==q))
    res = badParm;
  else
    while(szExpr[i] && szExpr[i]!=q && res==ok)
    {
      if (szExpr[i]=='\\' && i<len_szExpr-1)
      {
        ch = esc(&i);
        fill_set_incl (n, ch, from, &range);
      }
      else if (szExpr[i]=='-' && i<len_szExpr-1 && szExpr[i+1]!=q)
      {
        from  = ch;    /* save pred char */
        range = TRUE;  /* next char will be right bound of the range */
        i++;
      }
      else
      {
        ch = szExpr[i];
        fill_set_incl (n, ch, from, &range);
        i++;
      }
    }
    if (res!=ok) return;
    if (i>=len_szExpr || szExpr[i]!=q || range) res = badParm;
    else
    {
      i++;
      if (inv) n->leg.inverse();
    }
}

  int fill_str_scan(BOOL put, PEXPR n)
  {
    char ch;
    int  c,j;
    j = i; c = 0;
    while(j<len_szExpr)
    {
      ch = szExpr[j];
      if (!ch || strchr("[*?{)&|^$",ch))
      {
        if (put) i = j;
        return c;
      }
      else if (ch=='\\' && j<len_szExpr-1)
      {
        ch = esc(&j);
        if (put) n->pat[c] = ch;
        c++;
      }
      else
      {
        if (put) n->pat[c] = szExpr[j];
        c++;
        j++;
      }
    }
    if (put) i = j;
    return c;
  }
void fill_str(PEXPR n)
{
  if (res==ok)
  {
    if (n->pat = (PSZ)malloc(fill_str_scan(FALSE,n)+1))  n->pat[fill_str_scan(TRUE,n)] = 0;
    else                                                 res = badParm;
  }
}

int simple(PPEXPR reg)
{
  PEXPR n;
  char  ch;
  if (res!=ok) return res;
  if (i>=len_szExpr) return badParm;
  while(1)
  {
    if (res!=ok || i>=len_szExpr)  break;
    ch = szExpr[i];
    if (!ch || strchr(")(|&^",ch)) break;
    switch(ch)
    {
      case '*':  n = app_new(reg,seq); i++;          break;
      case '?':  n = app_new(reg,any); i++;          break;
      case '{':  n = app_new(reg,set); fill_set(n);  break;
      case '[':  n = app_new(reg,set); fill_set(n);  break;
      default:   n = app_new(reg,str); fill_str(n);
    }
    dollar(n);
  }
  return res;
}

int re(PPEXPR reg);

int factor(PPEXPR reg)
{
  *reg = 0;
  if (res!=ok)       return res;
  if (i>=len_szExpr) return badParm;
  if (szExpr[i]=='(')
  {
    i++;
    *reg = new EXPR(par);     if (res!=ok) return res;
    res  = re(&(*reg)->next); if (res!=ok) return res;
    if (szExpr[i]==')') {i++; dollar(*reg);}
    else                res = badParm;
  }
  else if (szExpr[i]=='^')
  {
    i++;
    *reg = new EXPR(not);         if (res!=ok) return res;
    res  = factor(&(*reg)->next); if (res!=ok) return res;
    if ((*reg)->next->nres >= 0) (*reg)->nres = (*reg)->next->nres,  (*reg)->next->nres = -1;
  }
  else
    res = simple(reg);
  return res;
}

int term(PPEXPR reg)
{
  PEXPR t;
  *reg = 0;
  if (res!=ok) return res;
  if (i>=len_szExpr) return badParm;
  res = factor(reg);
  if (i<len_szExpr-1 && szExpr[i]=='&' && res==ok)
  {
    t = new EXPR(and);  i++;
    t->left = *reg; *reg = t;
    res = term(&t->next);
  }
  return res;
}

int re(PPEXPR reg)
{
  PEXPR t;
  *reg = 0;
  if (res!=ok) return res;
  if (i>=len_szExpr || !szExpr[i]) return badParm;
  res = term(reg);
  if (i<len_szExpr-1 && szExpr[i]=='|' && res==ok)
  {
    t = new EXPR(or);       i++;
    t->left = *reg; *reg = t;
    res = re(&t->next);
  }
  return res;
}


void Compile(PSZ szExpr, PHEXPR reg, PLONG res)
/** res <= 0 -- error in position ABS(res);
    res >  0 -- done.
*/
{
  ::i          = 0;
  ::res        = ok;
  ::szExpr     = szExpr;
  ::len_szExpr = strlen(szExpr);
  *reg         = 0;
  ::res        = re((PPEXPR)reg);
  *res = (::res!=ok || i<len_szExpr) ? -i : i;
}


BOOL Const(HEXPR re)
{
/** Returns TRUE, if expression does not contain wildcards */
  PEXPR p = PEXPR(re);
  while(p->kind==par) p = p->next;
  return (p->kind==str && !p->next);
}

  BOOL match0_bra_seq_end(PPEXPR preg, int p, int *stop, PRESULT rs, int n)
  {
    while(*preg && ((*preg)->kind==bra || (*preg)->kind==seq)) preg = &(*preg)->next;
    if (*preg) return FALSE;
    if (n>=0) rs->len[n] = p-rs->pos[n];
    *stop = p;
    return TRUE;
  }
BOOL match0(PEXPR reg, PSZ s, int p, int *stop, PRESULT rs)
{
  int n; /* reg^.nres */
  int i;
  int LenS = strlen(s);
  *stop    = p;
  if (!reg)    return (p >= LenS);
  if (p>=LenS) return (reg->kind==bra || reg->kind==seq) && (!reg->next);
  n = reg->nres;
  switch(reg->kind)
  {
    case any: // "?"
      if (n>=0) rs->pos[n] = p; rs->len[n] = 1;
      return match0(reg->next,s,p+1,stop,rs);
    case seq: // "*"
      if (n>=0) rs->pos[n] = p;
      while(p<LenS)
      {
        if (match0(reg->next,s,p,stop,rs))
        {
          if (n>=0) rs->len[n] = p-rs->pos[n];
          return TRUE;
        }
        p++;
      }
      return match0_bra_seq_end(&reg, p, stop, rs, n);
    case set: // "[]"
      if (!reg->leg.in(s[p])) return FALSE;
      if (n>=0) rs->pos[n] = p; rs->len[n] = 1;
      return match0(reg->next,s,p+1,stop,rs);
    case bra: // "{}"
      if (n>=0) rs->pos[n] = p;
      while(p<LenS)
      {
        if (match0(reg->next,s,p,stop,rs))
        {
          if (n>=0) rs->len[n] = p-rs->pos[n];
          return TRUE;
        }
        if (!reg->leg.in(s[p])) return FALSE;
        p++;
      }
      return match0_bra_seq_end(&reg, p, stop, rs, n);
    case str: // string
      if (n>=0) rs->pos[n] = p;
      for(i=0; reg->pat[i]; i++,p++)
        if (s[p]!=reg->pat[i]) return FALSE;
      if (n>=0) rs->len[n] = p-rs->pos[n];
      return match0(reg->next,s,p,stop,rs);
    case and: // "&"
      return  match0(reg->left,s,p,stop,rs)
           && match0(reg->next,s,p,stop,rs);
    case or:  // "|"
      return match0(reg->left,s,p,stop,rs)
          || match0(reg->next,s,p,stop,rs);
    case not: // "^"
      if (n>=0) rs->pos[n] = p;
      if (match0(reg->next,s,p,stop,rs)) return FALSE;
      *stop = LenS;
      if (n>=0) rs->len[n] = *stop-rs->pos[n];
      return TRUE;
    case par: // "()"
      if (n>=0) rs->pos[n] = p;
      if (match0(reg->next,s,p,stop,rs))
      {
        if (n>=0) rs->len[n] = *stop-rs->pos[n];
        return TRUE;
      }
      return FALSE;
    default:
      return FALSE; // ASSERT(FALSE,4Fh)
  } // switch(reg->kind)
}

BOOL Match(HEXPR re, PSZ s, int pos)
/** Returns TRUE, iff expression matches with string "s" starting
  from position "pos".
*/
{
  int i;
  PEXPR p = PEXPR(re);
  memset(&p->res,0,sizeof(p->res));
  return match0(p,s,pos,&i,&p->res);
}

void Substitute(HEXPR re, PSZ s, PSZ m, PSZ d, ULONG dLen)
/** Substitutes  the substrings of "s" matched with "re" instead
  of "$digit" in the "m" and copies the resulting string into "d[0..dLen-1]".
*/
/* NB: 'm' cant be VAL because 'm' & 'd' may be the same variable */
{
  if (dLen)
  {
    int  i,j,k,l,n;
    int  LenM = strlen(m);
    int  LenS = strlen(s);
    for (i=j=0; i<LenM;)
    {
      if (j+1>=dLen) {d[dLen-1] = 0; return;}
      if (m[i]=='\\' && m[i+1]=='$')
      {
        d[j]='$'; i+=2; j++;
      }
      else if (m[i]=='$' && (n = m[i+1]-'0')>=0 && n<=9)
      {
        k = PEXPR(re)->res.pos[n];
        l = PEXPR(re)->res.len[n];
        if (j+l > dLen-1) l = dLen-j-1;
        if (k+l > LenS)   l = LenS-k;
        if (l>0)
        {
          strncpy(d+j, s+k, l);
          j+=l;
        }
        i+=2;
      }
      else
      {
        d[j] = m[i];
        i++; j++;
      }
    }
    d[min(dLen-1,j)] = 0;
  }
}


int Len(HEXPR re, int n)
/** Returns the length of  the  substring matched to "$n"
  at last call of match procedure with parameter "re".
*/
{
  return ULONG(n)<10 ? PEXPR(re)->res.len[n] : 0;
}

int Pos(HEXPR re, int n)
/** Returns the position of the  beginning  of  the  substring
  matched to "$n" at last call of match procedure with
  parameter "re".
*/
{
  return ULONG(n)<10 ? PEXPR(re)->res.pos[n] : 0;
}


void FreeExpr(HEXPR re)
/** Deallocates "re".
*/
{
  delete PEXPR(re);
}


/*==================================================================================================*/
/*========                                                                                   =======*/
/*======                             E N D   O F   R e g C o m p                             =====*/
/*========                                                                                   =======*/
/*==================================================================================================*/


/*==================================================================================================*/
/*===============================>   R C T h u n k   <==============================================*/

void RCThunk_Compile (PSZ szExpr, PHEXPR phExpr, PLONG pRes)
{
  *pRes   = -1;
  *phExpr = 0;
  if (szExpr)
  {
    Compile(szExpr,phExpr,pRes);
    if (*pRes<=0)
    {
      FreeExpr(*phExpr);
      *phExpr = 0;
    }
  }
}

void RCThunk_FreeExpr (HEXPR hExpr)
{
  if (hExpr) FreeExpr(hExpr);
}

BOOL RCThunk_Const (HEXPR hExpr)
{
  return !hExpr || Const(hExpr);
}

BOOL RCThunk_Match (HEXPR hExpr, PSZ sz)
{
  return !hExpr || sz && Match(hExpr,sz,0);
}

BOOL RCThunk_FastMatch (PSZ expr, PSZ s)
{
  HEXPR hExpr =  0;
  LONG  res   = -1;
  BOOL  rc    = FALSE;
  if (!expr || !*(expr=sf_skipspaces(expr)) || (*expr=='*' && !expr[1])) return TRUE;
  Compile(expr,&hExpr,&res);
  if (res>0)
    rc = s && Match(hExpr,s,0);
  FreeExpr(hExpr);
  return rc;
}

void RCThunk_Substitute (HEXPR hExpr, PSZ s, PSZ m, PSZ d, LONG dlen)
{
  if (!d || !dlen) return;
  d[0] = 0;
  if (!m) m = "";
  if (s) Substitute(hExpr, s, m, d, dlen);
}

LONG RCThunk_Len (HEXPR hExpr, LONG n)
{
  return hExpr ? Len(hExpr,n) : 0;
}

LONG RCThunk_Pos (HEXPR hExpr, LONG n)
{
  return hExpr ? Pos(hExpr,n) : 0;
}


/*========================>   E N D   O F   R C T h u n k   <=======================================*/
/*==================================================================================================*/






typedef struct _REDIRLIST{
  _REDIRLIST   *next;
  HEXPR         hExpr;
  char         *paths;
}REDIRLIST;

#pragma pack(1)
typedef struct _FILETIME{
  FDATE fdate;
  FTIME ftime;
} FILETIME;
#pragma pack()

REDIRLIST      *pRedirList = 0;

FILETIME        aTimesOld[2];
FILETIME        aTimesNew[2];
char            aszRedFiles[2][CCHMAXPATH]; // Redir files names (xc.red, *.prj)
const short     RF_RED = 0;
const short     RF_PRJ = 2;



void  refresh_redirections();
char *find_red_file();

void append_redir_from_file(HFILE hf, REDIRLIST **ppRL, char *szFullSrc, BOOL fXcRed = FALSE)
// if (!*ppRL) *ppRL := &new list
// else        append (*ppRL);
// '$!' macro changes using szFullSrc
{
  char          sz[1024];
  char          sz1[1024];
  char         *pch;
  char          ch;
  LONG          i,j;
  REDIRLIST    *pRL  = *ppRL;
  REDIRLIST    *pRL1;
  char          szDir[CCHMAXPATH]; // Directory for the '$!' macro
  int           nDir;

  strcpy(szDir,szFullSrc);
  if (pch = strrchr(szDir,'\\')) *pch = '\0';
  else                           strcpy(szDir,".");
  nDir = strlen(szDir);

  if (pRL) while (pRL->next) pRL = pRL->next;

  while(sf_fgets(sz,sizeof(sz),hf)){
    for (i=0;i<sizeof(sz); i++) sz[i] = sf_toupper(sz[i]);
    pch = sf_skipspaces(sz);
    if (!*pch) continue;
    if (!fXcRed){ // -LOOKUP
      if (*(pch++) != '-') continue;
      pch = sf_skipspaces(pch);
      if (sf_strnicmp(pch,"LOOKUP",6)) continue;
      pch = sf_skipspaces(pch+6);
      if (*(pch++) != '=') continue;
      pch = sf_skipspaces(pch);
    }
    for (i=0; pch[i] && pch[i]!=' ' && pch[i]!='='; i++);
    if (!i) continue;

    if (!(pRL1 = (REDIRLIST*)malloc(sizeof(REDIRLIST)))) return;
    pRL1->next = 0;
    {
      char sz[CCHMAXPATH];
      strncpy(sz,pch,min(i,sizeof(sz)-1));
      sz[min(i,sizeof(sz)-1)] = 0;

      RCThunk_Compile(sf_strtoupper(sz), &pRL1->hExpr, &j);
      if (j<0)
      {
        free(pRL1);
        continue;
      }
    }
    pch = sf_skipspaces(pch+i);
    if (*pch=='=') pch = sf_skipspaces(pch+1);

    // Get the paths
    j=0; ch = 1;
    while(ch)  //--- sz1 := 'path\{;path\}',0
    {
      ch = *(pch++);
      if (ch=='/')        ch = '\\';
      if (sf_isspace(ch)) continue;
      if (!strncmp(pch-1,"$!",2))
      {
        if (nDir+j+1 >= sizeof(sz1)) ch=0;
        else                         { strcpy(sz1+j,szDir);  j += nDir; pch++; continue;}
      }
      if (ch==';' && j && sz1[j-1]!='\\')
        sz1[j++] = '\\'; // Insert '\' after paths
      sz1[j++] = ch;
    }
    j--;
    if (j && sz1[j-1]==';')  sz1[j--] = '\0';
    if (j && sz1[j-1]!='\\') strcat(sz1,"\\"), j++;
    if (j=0 || !(pRL1->paths = (char*)malloc(j+1))){
      RCThunk_FreeExpr(pRL1->hExpr);
      free(pRL1);
      if (!j) continue;
      return;
    }
    strcpy(pRL1->paths,sz1);
    if (!pRL)
      pRL = *ppRL = pRL1;
    else
      pRL = pRL->next = pRL1;
  } // while(line)
}

void get_redfiles_timeinfo(FILETIME *aFT)
{
  FILESTATUS3 fs3;
  for (int i=0; i<2; i++){
    if (DosQueryPathInfo(aszRedFiles[i],FIL_STANDARD,&fs3,sizeof(fs3)) )
      memset(&aFT[i],0,sizeof(*aFT));
    else{
      aFT[i].fdate = fs3.fdateLastWrite;
      aFT[i].ftime = fs3.ftimeLastWrite;
    }
  }
}

void re_get_redirections()
// Полностью перечитать redir-информацию:
//   cначала - из .red файла, если он установлен;
//   затем - из проекта (если он есть).
{
  HFILE         hf;
  ULONG         ulAction;
  REDIRLIST    *pRL,*pRL1;
  APIRET        err;

  { // Чистим старую информацию:
    REDIRLIST *pRL0;
    while (pRedirList)
    {
      if (pRedirList->hExpr) RCThunk_FreeExpr(pRedirList->hExpr);
      pRL0        = pRedirList;
      pRedirList  = pRedirList->next;
      free(pRL0);
    }
    aszRedFiles[RF_RED][0] = 0;
    aszRedFiles[RF_PRJ][0] = 0;
  }

  // Чтем .RED файл:
  PSZ pszRed = 0;
  PSZ psz;
  if (pszRed = find_red_file())
  {
    if(err=DosOpen(pszRed,&hf,&ulAction,0,FILE_NORMAL,FILE_OPEN,OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE,(PEAOP2)0))
    {
      IOErMsg(err, hMDI0Client, "Get redirections", "Can't open file", pszRed, MB_OK);
      free(pszRed);
      return;
    }
    else{
      strcpy(aszRedFiles[RF_RED],pszRed);
      append_redir_from_file(hf,&pRedirList,pszRed,TRUE);
      DosClose(hf);
    }
    free(pszRed);
  }
  else if (psz = pUserVarTable->QueryVar("UseRedFile"))
  {
    MessageBox(hMDI0Client,"Get redirection info",MB_ERROR|MB_OK,"Can't find redirection file: '", psz,
               "' (see user variable 'UseRedFile').");
    return;
  }
  else if (pUserVarTable->QueryVar("XDSCompiler"))
  {
    MessageBox(hMDI0Client,"Get redirection info",MB_ERROR|MB_OK,"Can't find redirection file. "
               "(Check user variable 'XDSCompiler'.)");
    return;
  }

  // Read szPrjFile file
  if (szPrjFile[0])
  {
    if (err=DosOpen(szPrjFile,&hf,&ulAction,0,FILE_NORMAL,FILE_OPEN,OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE,(PEAOP2)0))
      IOErMsg(err, hMDI0Client, "Get redirections", "Can't open file", szPrjFile, MB_OK);
    else{
      strcpy(aszRedFiles[RF_PRJ],szPrjFile);
      pRL = 0;
      append_redir_from_file(hf,&pRL,szPrjFile);
      if (pRL){
        pRL1 = pRL;
        while(pRL1->next) pRL1 = pRL1->next;
        pRL1->next = pRedirList;
        pRedirList = pRL;
      }
      DosClose(hf);
    }
  }
  get_redfiles_timeinfo(aTimesOld);
}


char *find_red_file()
// returns malloc()ed full filename OR 0
{
  PSZ  pszBuf,pszVar,psz;
  char szRedName[CCHMAXPATH];
  if (!(pszBuf = (PSZ)malloc(CCHMAXPATH)))        return 0;
  // Red файл задан ручками?
  pszVar = pUserVarTable->QueryVar("UseRedFile");
  if (pszVar && strlen(pszVar)<=CCHMAXPATH)
  {
    if (FName2Full(pszBuf,pszVar,TRUE))           return pszBuf;
    else {free(pszBuf);                           return 0;}
  }
  // Ищем как положено (по имени компилятора из XDSCompiler)
  pszVar = pUserVarTable->QueryVar("XDSCompiler");
  if (!pszVar || strlen(pszVar)>CCHMAXPATH)       return 0;  // No *.red file used
  strcpy(szRedName, sf_fname2short(pszVar));
  if   (psz=strrchr(szRedName,'.')) strcpy(psz,".red");
  else {free(pszBuf);                             return 0;} // Там чего-то не то
  // В текущей директории е?
  if (FName2Full(pszBuf,szRedName,TRUE))          return pszBuf;
  //--- Try: XdsCompiler user variable
  if (FName2Path(pszBuf,pszVar) &&
      FName2Full(pszBuf,szRedName,TRUE,pszBuf))   return pszBuf;
  free(pszBuf);
  return 0;
}

void refresh_redirections()
// обновляет .red списки если изменились имена файлов или сами файлы
// (*red & szPrjFile)
{
  char *psz_red = find_red_file();
  char *pszRed  = psz_red ? psz_red : "";

  if (strcmp(pszRed, aszRedFiles[RF_RED]) ||
      strcmp(szPrjFile, aszRedFiles[RF_PRJ]))
    // Поменялись имена:
    re_get_redirections();
  else
  {
    get_redfiles_timeinfo(aTimesNew);
    if (memcmp(aTimesOld,aTimesNew,sizeof(aTimesOld)))
      // Поменялись сами файлы:
      re_get_redirections();
  }
  free(psz_red);
}


BOOL try_to_find(char *szFull, char *szName, char *szPaths, BOOL fFind)
// Пытается найти имя на путях из списка szPaths
// (!fFind => прилепить найти путь к имени)
{
  int   i = -1;
  char  sz[CCHMAXPATH];

  while (i<0 || szPaths[i]){
    int j=0;
    i++;
    for (;szPaths[i] && szPaths[i]!=';'; i++) sz[j++] = szPaths[i];
    sz[j]='\0';
    if (!j) continue;
    if (sz[j-1] != '\\') strcat(sz,"\\");
    strcat(sz,szName);
    if (FName2Full(szFull,sz,fFind)) return TRUE;
  }
  return FALSE;
}

/*----------------------------------------------------
/
/ Поиск файла по redirection:
/
/  szTarg (out) - буфер для записи полного имени найденного файла
/  szName (in)  - файл для поиска
/  Если не найден - вернет FALSE, szTarg не изменится.
/---------------------------------------------------*/
BOOL FindRedirFile(char *szTarg, char *szName)
{
  char          szFull[CCHMAXPATH];
  REDIRLIST    *pRL;
  int           slen     = strlen(szName);
  char          szNam[CCHMAXPATH];
  BOOL          fMatched = FALSE;
  BOOL          fm;

  refresh_redirections();
  pRL = pRedirList;
  for(int i=0; i<=slen; i++) szNam[i] = sf_toupper(szName[i]);
  while(pRL)
  {
    if ((fm=RCThunk_Match(pRL->hExpr, szNam)) &&
        try_to_find(szFull,szName,pRL->paths,TRUE) )
    {
      strcpy(szTarg,szFull);
      return TRUE;
    }
    pRL = pRL->next;
    fMatched |= fm;
  }
  return fMatched ? FALSE : FName2Full(szTarg,szName,TRUE);
}

/*====================================
/
/  Converts filename to full filename (using current drive/path).
/
/  pchTarg(char *) - output
/
/     Address of the target buffer (must be CCHMAXPATH bytes or more)
/     where function returns full filename
/     don't changed when error, can be the same with szName
/
/  szName(char*) - input
/
/     Filename to convert.
/
/  fFindIt(BOOL) - input
/
/    TRUE  - try to find this file after convertion
/    FALSE - convert only
/
/  szBasePath(char*)
/    0    - use current path
/    else - pointer to the (full!) path strung (w/o last '\')
/
/  Return
/
/    TRUE  - name converted and/or found successfully
/    FALSE - any other case
/=====================================*/
BOOL FName2Full(char *pchTarg, char *szName, BOOL fFindIt,char *szBasePath)
{
  class STCK
  {
  public:
           STCK() {p=0;}
    void   Push(char *x) {st[p++ % 32] = x;}
    char  *Pop()         {return p ? st[--p % 32] : 0;}
  private:
    char  *st[32];
    USHORT p;
  };

  STCK     stck;
  char     sz[CCHMAXPATH];
  ULONG    ul,ul1;
  char    *pch;
  char    *pch1;

  for(pch=szName; *pch==' ' || *pch=='\t'; pch++);
  for (ul=0; pch[ul];ul++) if (pch[ul]=='/') pch[ul]='\\';
  if (pch1=strchr(pch,':'))
  {
    if (pch1!=pch+1) return FALSE; // invalid drive specification
    if(pch1[1]=='\\')
      strcpy(sz,pch);
    else
    { // Path like e:zzz => transform to e:\<e-curdir>\zzz
      ULONG olddrv;
      DosQueryCurrentDisk(&olddrv,&ul1);
      ul = sf_toupper(pch[0])-'A'+1;    // spec. drivenum
      if (DosSetDefaultDisk(ul)) return FALSE;

      strncpy(sz,pch,2);                // "e:"
      sz[2] = '\\';                     // "e:\"
      ul = sizeof(sz)-3;
      DosQueryCurrentDir(0,sz+3,&ul);   // "e:\<e-curdir>
      strcat(sz,"\\");
      strcat(sz,pch1+1);                // "e:\<e-curdir>\zzz
      DosSetDefaultDisk(olddrv);
    }
  }
  else if (!strncmp(pch,"\\\\",2))
  {
    if (!pch[2] || pch[2]=='\\') return FALSE;
    else                         strcpy(sz,pch);
  }
  else
  {
    if (szBasePath)
      strcpy(sz,szBasePath);
    else
    {
      DosQueryCurrentDisk(&ul,&ul1);
      sprintf(sz,"%c:\\",'A'-1+ul);
      ul = sizeof(sz)-3;
      DosQueryCurrentDir(0,sz+3,&ul);
    }
    if (*pch=='\\')
      strcpy(sz+2,pch);
    else
    {
      strcat(sz,"\\");
      strcat(sz,pch);
    }
  }
  /*--- Exclude '\\', '..' and '.' from this path ---*/
  if (!strncmp(sz,"\\\\",2))
  {
    if (!(pch = strchr(sz+2,'\\'))) return FALSE;
    pch++;
  }
  else pch = sz;
  while(*pch)
  {
    if (!strncmp(pch,"\\..\\",4) || !strcmp(pch,"\\.."))
    {
      if (!(pch1=stck.Pop())) return FALSE;
      memmove(pch1,pch+3,strlen(pch+3)+1);
      pch = pch1;
    }
    else if(!strncmp(pch,"\\\\", 2)) memmove(pch,pch+1,strlen(pch+1)+1);
    else if(!strncmp(pch,"\\.\\",3)) memmove(pch,pch+2,strlen(pch+2)+1);
    else if(!strcmp (pch,"\\."))     *pch = 0;
    else if(*(pch++)=='\\')          stck.Push(pch-1);
  }
  if (!fFindIt)
  {
    strcpy(pchTarg,sz);
    return TRUE;
  }

  HFILE hf;
  if (!DosOpen(sz,&hf,&ul,0,FILE_NORMAL,FILE_OPEN,
               OPEN_ACCESS_READONLY|OPEN_SHARE_DENYNONE,(PEAOP2)0))
  {
    DosClose(hf);
    strcpy(pchTarg,sz);
    return TRUE;
  }
  return FALSE;
}

BOOL FName2Path(char *pchTarg, char *szName)
{
  char sz[CCHMAXPATH];
  if (!FName2Full(sz,szName,FALSE)) return FALSE;
  PSZ pch = strrchr(sz,'\\');
  if (sz[0]=='\\' && pch<sz+3) return FALSE;
  *pch = 0;
  strcpy(pchTarg,sz);
  return TRUE;
}
BOOL FName2Ext(char *pchTarg, char *szName)
{
  PSZ pch = strrchr(szName,'.');
  if (!pch || strchr(pch,'\\')) return FALSE;
  strcpy(pchTarg,pch+1);
  return TRUE;
}

BOOL Browse (char *szOut, char *szIn, HWND hOwner, char *szCaption, BOOL fSaveAs)
// returns: TRUE, szOut = target file  or  FALSE and szOut is not changed
//
{
  FILEDLG fild;
  ULONG     rc;
  memset(&fild, 0, sizeof(FILEDLG));
  fild.cbSize     = sizeof(FILEDLG);
  fild.pszTitle   = szCaption;
  fild.fl         = (fSaveAs ? FDS_SAVEAS_DIALOG : FDS_OPEN_DIALOG) | FDS_CENTER;
  strcpy(fild.szFullFile, szIn);
  rc = WinFileDlg(HWND_DESKTOP, hOwner, &fild);
  if (!rc || fild.lReturn!=DID_OK) return FALSE;
  strcpy(szOut,fild.szFullFile);
  return TRUE;
}



/*======================= $( V A R I A B L E S ) =======================*/

VARTABLE::VARTABLE(VARTABLE *pInit)
{
  lVarsAlloced = 4;
  lVarsTotal   = 0;
  apVarStr     = (PVARSTR*)malloc(lVarsAlloced*sizeof(PVARSTR));
  if (pInit)
  {
    PSZ pszName,pszValue;
    for (int i=0; pInit->QueryIdxVar(i,&pszName,&pszValue); i++)
      AssignVar(pszName,pszValue);
  }
}

VARTABLE::~VARTABLE()
{
  ForgetAll();
  free(apVarStr);
}

BOOL VARTABLE::find_name(PSZ name, PLONG pIdx)
{
  LONG i      = 0;
  LONG l      = 0;
  LONG u      = lVarsTotal-1;
  int  cmp    = 0;
  BOOL fFound = FALSE;
  while(u>=l)
  {
    i   = (u+l)/2;
    if (!(cmp = sf_stricmp(name,apVarStr[i]->szVarName)))
    {
      fFound = TRUE;
      break;
    }
    if (cmp<0) u = i-1;
    else       l = i+1;
  }
  if (!fFound && cmp>0) i++;
  *pIdx = i;
  return fFound;
}

PSZ VARTABLE::QueryVar(PSZ szName)
{
  LONG lIdx;
  return find_name(szName,&lIdx) ? apVarStr[lIdx]->szVarValue : NULL;
}

BOOL VARTABLE::QueryIdxVar(ULONG idx, PSZ *ppszName, PSZ *ppszValue)
{
  if (idx>=lVarsTotal) return FALSE;
  *ppszName  = apVarStr[idx]->szVarName;
  *ppszValue = apVarStr[idx]->szVarValue;
  return TRUE;
}

LONG VARTABLE::QueryVarIdx(PSZ szName)
{
  LONG lIdx;
  return find_name(szName,&lIdx) ? lIdx : -1;
}

void VARTABLE::AssignVar(PSZ szName, PSZ szValue)
{
  LONG    lIdx;
  PVARSTR pVS = (PVARSTR)malloc(sizeof(VARSTR)+strlen(szValue));
  if (!pVS) return;
  strncpy(pVS->szVarName,szName,VARNAMELEN);
  pVS->szVarName[VARNAMELEN-1] = '\0';
  strcpy(pVS->szVarValue,szValue);
  if (find_name(szName,&lIdx)) free(apVarStr[lIdx]);
  else
  {
    if (lVarsTotal+1 >= lVarsAlloced)
    {
      PVARSTR* ap1  = (PVARSTR*)realloc(apVarStr,lVarsAlloced*2*sizeof(PVARSTR));
      if (!ap1)
      {
        free(pVS);
        return; // Gr-r-r
      }
      apVarStr      = ap1;
      lVarsAlloced *= 2;
    }
    memmove(apVarStr+lIdx+1,apVarStr+lIdx,(lVarsTotal-lIdx)*sizeof(PVARSTR));
    lVarsTotal++;
  }
  apVarStr[lIdx] = pVS;
}

BOOL VARTABLE::ForgetVar(PSZ szName)
{
  LONG lIdx;
  if (!find_name(szName,&lIdx)) return FALSE;
  memmove(apVarStr+lIdx,apVarStr+lIdx+1,(lVarsTotal-lIdx-1)*sizeof(PVARSTR));
  lVarsTotal--;
  return TRUE;
}

void VARTABLE::ForgetAll()
{
  for (ULONG i=0; i<lVarsTotal; i++)
    free(apVarStr[i]);
  lVarsTotal = 0;
}

void VARTABLE::WProfile (LONG slot, PSZ szKey)
{
  // Пишем последовательность строк (имя,значение,имя...) разделенных
  // нулями, в конце - двойной нуль.
  LONG lLen = 2;
  LONG i,l;
  PfDeleteItem(slot, szKey);
  for (i=0; i<lVarsTotal; i++)
    lLen += strlen(apVarStr[i]->szVarName) + strlen(apVarStr[i]->szVarValue) + 2;
  char *pBuf = (char*)malloc(lLen);
  if (!pBuf) return;
  memset(pBuf,0,lLen);
  for (i=0,l=0; i<lVarsTotal; i++)
  {
    strcpy(pBuf+l,apVarStr[i]->szVarName);
    l += strlen(pBuf+l)+1;
    strcpy(pBuf+l,apVarStr[i]->szVarValue);
    l += strlen(pBuf+l)+1;
  }
  PfWriteItem(slot, szKey, pBuf, l+1);
}

char *get_substr(char *pch, char *pchE)
{
  // В интервале [pch..pchE[ найдет первый ноль, если за ним есть
  // помещающаяся в интервале строка - вернет ее. Иначе - вернет 0.
  char *r = (char*)memchr(pch,'\0', pchE-pch);
  if (r && ++r<pchE && *r && memchr(r,'\0', pchE-r)) return r;
  return 0;
}

void VARTABLE::RProfile (LONG slot, PSZ szKey, BOOL fAddNew)
{
  if (!fAddNew)
  {
    for (ULONG i=0; i<lVarsTotal; i++)
      free(apVarStr[i]);
    lVarsTotal = 0;
  }

  LONG  lCB  = PfQueryItemSize(slot,szKey);
  char *pBuf = (char*)malloc(lCB);
  if (lCB && pBuf && lCB== PfReadItem(slot, szKey, pBuf, lCB, 0, 0))
  {
    char *pszName=pBuf, *pszValue, *pchE = pBuf+lCB;
    while(pszName && *pszName && (pszValue=get_substr(pszName,pchE)))
    {
      if (!fAddNew || !QueryVar(pszName)) AssignVar(pszName,pszValue);
      pszName=get_substr(pszValue,pchE);
    }
  }
  free(pBuf);
}


MRESULT EXPENTRY dpEdVarDlg (HWND hWnd, ULONG ulMsg, MPARAM m1, MPARAM m2)
{
  static PVARTABLE pEdVars;  // Приватная копия таблицы переменных
  static HWND      hLBVars;
  // На строках листа висят malloc()ed имена соответствующих переменных

  switch(ulMsg){
  case WM_INITDLG:
  {
    int i;
    PSZ pszName, pszValue;
    hLBVars     = WinWindowFromID(hWnd,IDD_LBVARS);
    pEdVars     = new VARTABLE(pUserVarTable);
    for (i=0; pEdVars->QueryIdxVar(i,&pszName,&pszValue); i++)
      WinSendMsg(hWnd,WM_USER+1,MPARAM(pszName),MPARAM(FALSE));
    WinSendMsg(hLBVars,LM_SELECTITEM,MPARAM(0),MPARAM(TRUE));
    WinSendMsg(hWnd,WM_USER,0,0);
    break;
  }
  case WM_CONTROL:
    if (SHORT1FROMMP(m1)==IDD_LBVARS && SHORT2FROMMP(m1)==LN_ENTER)
      WinPostMsg(WinWindowFromID(hWnd,IDD_PBVAREDVALUE),BM_CLICK,0,0);
  case WM_USER: // Re-enable all
  {
    BOOL fSel = (LIT_NONE != (int)WinSendMsg(hLBVars,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0));
    WinEnableControl(hWnd, IDD_PBVAREDNAME,  fSel);
    WinEnableControl(hWnd, IDD_PBVAREDVALUE, fSel);
    WinEnableControl(hWnd, IDD_PBVARREM,     fSel);
    break;
  }
  case WM_USER+1: // Insert var. m1 nito the list; select it if m2
  {
    char   sz[300];
    PSZ    pszV,psz;
    PSZ    pszN = PSZ(m1);
    if (!(pszV=pEdVars->QueryVar(pszN))) break;
    sprintf(sz,"%s = \"",pszN);
    strncat(sz,pszV,sizeof(sz)-VARNAMELEN-10);
    strcat(sz,"\"");
    if (!(psz = (PSZ)malloc(strlen(pszN)+1))) break;
    strcpy(psz,pszN);
    int nIt = (int)WinSendMsg(hLBVars,LM_INSERTITEM,MPARAM(LIT_SORTASCENDING),MPARAM(sz));
    WinSendMsg(hLBVars,LM_SETITEMHANDLE,MPARAM(nIt),MPARAM(psz));
    if (m2)
      WinSendMsg(hLBVars,LM_SELECTITEM,MPARAM(nIt),MPARAM(TRUE));
    break;
  }
  case WM_CLOSE:
    m1 = MPARAM(DID_CANCEL);
  case WM_COMMAND:
  {
    SHORT s1m1 = SHORT1FROMMP(m1);
    int   nIt  = (int)WinSendMsg(hLBVars,LM_QUERYSELECTION,MPARAM(LIT_FIRST),0);
    switch(s1m1)
    {
      case IDD_PBVARNEW:
      {
        CPARAMSTR   cps;
        char        szName [VARNAMELEN+1] = "";
        cps.psz1  = "New variable";
        cps.psz2  = "Name:";
        cps.psz3  = szName;
        cps.l1    = VARNAMELEN;
        cps.l3    = 0;
        if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
        {
          PSZ psz = sf_skipspaces(szName);
          if (!psz[0])
          {
            MessageBox(hWnd,"New variable",MB_ERROR|MB_OK,"Wrong variable name.");
            break;
          }
          if (pEdVars->QueryVar(psz))
          {
            MessageBox(hWnd,"New variable",MB_ERROR|MB_OK,"This name is belonged to another variable.");
            break;
          }
          pEdVars->AssignVar(psz,"");
          WinSendMsg(hWnd,WM_USER+1,MPARAM(psz),MPARAM(TRUE));
        }
        break;
      }
      case IDD_PBVAREDNAME:
      case IDD_PBVAREDVALUE:
      {
        CPARAMSTR cps;
        char      szBuf     [1000] = "";
        char      szOldName [VARNAMELEN+1];
        BOOL      fVal = (s1m1==IDD_PBVAREDVALUE);
        {//--- Query name
          PSZ       pszN;
          if (nIt==LIT_NONE || !(pszN = (PSZ)WinSendMsg(hLBVars,LM_QUERYITEMHANDLE,MPARAM(nIt),0))) break;
          strcpy(szOldName,pszN);
        }
        if (fVal)
        {
          PSZ pszV = pEdVars->QueryVar(szOldName);
          if (pszV) strncpy(szBuf,pszV,sizeof(szBuf));
          else      szBuf[0] = 0; // Bug!
          szBuf[sizeof(szBuf)-1] = 0;
          cps.psz1  = "Edit variable value";
          cps.psz2  = "Value:";
          cps.l1    = sizeof(szBuf);
        }
        else
        {
          strcpy(szBuf, szOldName);
          cps.psz1  = "Edit variable name";
          cps.psz2  = "Name:";
          cps.l1    = VARNAMELEN;
        }
        cps.l3    = 0;
        cps.psz3  = szBuf;
        if (WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEntryFDlg,NULLHANDLE,IDD_ENTRYFDLG,&cps))
        {
          if (fVal)
          //--- Change value ---
            pEdVars->AssignVar(szOldName,szBuf);
          else
          //--- Change name ---
          {
            PSZ szNewName = sf_skipspaces(szBuf);
            if (!szNewName[0])
            {
              MessageBox(hWnd,"Edit variable name",MB_ERROR|MB_OK,"Wrong variable name.");
              break;
            }
            if (!sf_stricmp(szNewName,szOldName)) break;
            if (pEdVars->QueryVar(szNewName))
            {
              MessageBox(hWnd,"Edit variable name",MB_ERROR|MB_OK,"This name is belonged to another variable.");
              break;
            }
            PSZ pszV;
            { //-- pszV := malloc()ed old value
              PSZ psz = pEdVars->QueryVar(szOldName);
              if (!psz)   break;
              pszV    = (PSZ)malloc(strlen(psz)+1);
              if (!pszV)  break;
              strcpy(pszV,psz);
            }
            // Change var table
            pEdVars->ForgetVar(szOldName);
            pEdVars->AssignVar(szNewName,pszV);
            free(pszV);
            strcpy(szOldName,szNewName);
          }
          { //-- Change list contents
            free((PVOID)WinSendMsg(hLBVars,LM_QUERYITEMHANDLE,MPARAM(nIt),0));
            WinSendMsg(hLBVars,LM_DELETEITEM,MPARAM(nIt),0);
            WinSendMsg(hWnd,WM_USER+1,MPARAM(szOldName),MPARAM(TRUE));
          }
        }
        break;
      }
      case IDD_PBVARREM:
      {
        PSZ pszN;
        if (nIt==LIT_NONE || !(pszN = (PSZ)WinSendMsg(hLBVars,LM_QUERYITEMHANDLE,MPARAM(nIt),0)))
          break;
        pEdVars->ForgetVar(pszN);
        WinSendMsg(hLBVars,LM_DELETEITEM,MPARAM(nIt),0);
        free(pszN);
        WinSendMsg(hLBVars,LM_SELECTITEM,MPARAM(nIt ? nIt-1 : 0), MPARAM(TRUE));
        WinSendMsg(hWnd,WM_USER,0,0);
        break;
      }
      case IDD_PBHELP:
        DisplayHelpPanel(PANEL_EDVARDLG);
        break;
      case DID_CANCEL:
        delete pEdVars;
        WinDismissDlg(hWnd,FALSE);
        break;
      case DID_OK:
        delete pUserVarTable;
        pUserVarTable = pEdVars;
        pEdVars   = 0;
        WinDismissDlg(hWnd,TRUE);
        break;
    } // switch (s1m1)
    break;
  } //case WM_COMMAND
  case WM_CHAR:
    if(SHORT2FROMMP(m2)==VK_F1)
    {
      DisplayHelpPanel(PANEL_EDVARDLG);
      break;
    }
  default: return WinDefDlgProc(hWnd,ulMsg,m1,m2);
  }
  return 0;
}


BOOL EditVarTable(HWND hWnd)
// hWnd - parent window, вернет TRUE  если что-то было изменено
{
  return !!WinDlgBox(HWND_DESKTOP,hWnd,(PFNWP)dpEdVarDlg,NULLHANDLE,IDD_EDVARDLG,0);
}

PSZ  OpenExprVar(PSZ pszExpr)
// Открывает переменные в pszExpr выражении и возвращает
// malloc()ed результат (0 при ошибке)
{
  int   lAlloc  = 32;
  int   lOutPos = 0;
  PSZ   pszRes;
  char *pchIn   = pszExpr;
  char *pchFrom,*pch;
  LONG  lFrom;
  char  szVar[VARNAMELEN+2];
  if (!pszExpr || !(pszRes = (PSZ)malloc(lAlloc))) return 0;
  while(*pchIn)
  {
    pchFrom = pchIn;
    lFrom   = 1;
    if (!strncmp(pchIn,"$(",2))
    {
      strncpy(szVar,pchIn+2,sizeof(szVar));
      szVar[sizeof(szVar)-1] = '\0';
      if (pch=strchr(szVar,')'))
      {
        *pch    = '\0';
        if (!(pchFrom = pUserVarTable->QueryVar(szVar)) &&
            !(pchFrom = pAutoVarTable->QueryVar(szVar)))   pchFrom = "";
        lFrom         = strlen(pchFrom);
        if (!(pchIn   = strchr(pchIn,')'))) return 0; // Bug?
      }
    }
    {
      int lAlloc1=lAlloc;
      while (lOutPos+lFrom+2 >= lAlloc1) lAlloc1*=2;
      if (lAlloc!=lAlloc1)
      {
        if (!(pszRes = (PSZ)realloc(pszRes,lAlloc1))) return 0;
        lAlloc = lAlloc1;
      }
    }
    memcpy(pszRes+lOutPos,pchFrom,lFrom);
    lOutPos += lFrom;
    pchIn++;
  }
  pszRes[lOutPos] = '\0';
  return pszRes;
}


