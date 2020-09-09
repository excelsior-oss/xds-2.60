#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef char    BOOL;
#define TRUE    1
#define FALSE   0

#define HEAD_PATH ".\\hm\\"
#define HEAD_FILE "os2.hm"
#define DEST_FILE ".\\os2.def"

#define SPEC_NAME "pmerr.hm"

#define MAX_LLEN  512


FILE   *hfDst;
int     nSrcLine;
int     nDstLine;
char    szCurLine[MAX_LLEN];
BOOL    fWasSpecial = FALSE;

//--- Function prototypes ---//
//
BOOL    DoInclude(char *szFilename,BOOL fInclSpecial = FALSE);
void    SendError(char *text,char *pch);
//
//--------------------------//

/*+++++ simple text functions ++++++++++++++++++++++++++++++++++++++++++++++++++*/

char * skipbl(char *pch)
{
  char *r = pch;
  while (isspace(*r)) r++;
  return r;
}

BOOL match(char **ppch, char *sz)
{
  if (!strncmp(*ppch,sz,strlen(sz))){
    *ppch += strlen(sz);
    return TRUE;
  }
  else
    return FALSE;
}

void getname(char *ptarg, char **ppsrc)
{
  for(int i=0;;i++)
  {
    ptarg[i] = '\0';
    if (!isalnum(**ppsrc) && **ppsrc!='_') return;
    ptarg[i]=*((*ppsrc)++);
  }
}

void CheckFreeEol(char *pch)
{
  pch = skipbl(pch);
  if (!*pch || (pch[0]=='-' &&  pch[1]=='-') || (pch[0]=='(' &&  pch[1]=='*') ) return;
  SendError("Possible error in the preprocessor directive arguments",pch);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


void OutChar  (char ch)      { fprintf(hfDst, "%c",   ch);}
void OutLine  (char *szLine) { fprintf(hfDst, "%s",   szLine); }
void OutLineCR(char *szLine) { fprintf(hfDst, "%s\n", szLine); nDstLine++; }

void ConvExpr(char *pch)
{
// Only ) ( && || ! defined() operations!
  OutLine("<* IF ");
  while(1){
    pch = skipbl(pch);
    if (!strncmp("(*",pch,2) || !strncmp("--",pch,2) || !*pch){
      OutLineCR(" THEN *>");
      return; // Parsed Ok.
    }
    if (*pch=='!'){
      OutLine("NOT ");
      pch++;
      continue;
    }
    if (*pch=='(' || *pch==')'){
      OutChar(*(pch++));
      continue;
    }
    if (!strncmp(pch,"&&",2)){
      OutLine("& ");
      pch += 2;
      continue;
    }
    if (!strncmp(pch,"||",2)){
      OutLine("OR ");
      pch += 2;
      continue;
    }
    if (match(&pch,"defined")){
      OutLine("DEFINED (");
      pch = skipbl(pch);
      if (*pch != '('){
        SendError("defined(): '(' expected",pch);
        return;
      }
      pch = skipbl(pch+1);
      char sz[MAX_LLEN];
      getname(sz,&pch);
      if (!sz[0]){
        SendError("defined(): 'ident' expected",pch);
        return;
      }
      OutLine(sz);
      OutLine(") ");
      pch = skipbl(pch);
      if (*pch != ')'){
        SendError("defined(): ')' expected",pch);
        return;
      }
      pch++;
      continue;
    } // defined Ok.
    if (*pch=='0'){
      while (*pch == '0') pch++;
      CheckFreeEol(pch);
      OutLineCR("DEFINED(__NEVER_DEFINE_THIS_NAME) THEN *> (* it was '#if' 0 in the source text *)");
      return; // Parsed Ok.
    }
    else {
      SendError("#if: error in expression",pch);
      return;
    }
  } // while(1)
}




void OneLine(char *szLn)
{
char szLine[MAX_LLEN];
char *pch = szLine;
char *pch0;
char  sz[MAX_LLEN];
int   i;


  strcpy(szLine,szLn);
  while (isspace(*pch)) OutChar(*(pch++));

  if (pch0=pch, !strncmp(pch0,"END OS2.",8))
    DoInclude(SPEC_NAME,TRUE);

  if (*pch != '#'){
    OutLineCR(pch);
    return;
  }
  // ==== It is #line ====
  pch++;
  pch = skipbl(pch);
  if (match(&pch,"ifdef")){
    pch = skipbl(pch);
    getname(sz,&pch);
    OutLine("<* IF DEFINED(");
    OutLine(sz);
    OutLineCR(") THEN *>");
    CheckFreeEol(pch);
    return;
  }
  if (match(&pch,"ifndef")){
    pch = skipbl(pch);
    getname(sz,&pch);
    OutLine("<* IF NOT DEFINED(");
    OutLine(sz);
    OutLineCR(") THEN *>");
    CheckFreeEol(pch);
    return;
  }
  else if (match(&pch,"define")){
    pch = skipbl(pch);
    getname(sz,&pch);
    OutLine("<* NEW ");
    OutLine(sz);
    OutLineCR(" + *>");
    CheckFreeEol(pch);
    return;
  }
  else if (match(&pch,"else")){
    OutLineCR("<* ELSE *>");
    CheckFreeEol(pch);
    return;
  }
  else if (match(&pch,"endif")){
    OutLineCR("<* END *>");
    CheckFreeEol(pch);
    return;
  }
  else if (match(&pch,"error")){
    OutLine("$; -- In the source file was: ");
    OutLineCR(skipbl(szLine));
    return;
  }
  else if (match(&pch,"if")){
    ConvExpr(pch);
    return;
  }
  else if (match(&pch,"include")){
    pch = skipbl(pch);
    if (!(*pch=='<' || *pch=='"')){
      SendError("#include : '\"' or '<' expected",pch);
      return;
    }
    pch++;
    for (i=0;;i++)
      if ((sz[i]=pch[i])=='"' || pch[i]=='>') break;
      else if (!pch[i]){
        SendError("#include : '\"' or '>' expected",pch);
        return;
      }
      sz[i] = '\0';
    int nSrcl = nSrcLine;
    DoInclude(sz);
    nSrcLine = nSrcl;
    return;
  }
  else
    SendError("Wrong preprocessor directive",pch);
}

BOOL DoInclude(char *szFilename,BOOL fInclSpecial)
{
FILE          *hfSrc;

  char szSrc[MAX_LLEN];
  strcpy(szSrc,HEAD_PATH);
  strcat(szSrc,szFilename);
  printf("------------------- Include file: %s [Out:%6u]\n",szSrc,nDstLine);
  OutLineCR("");
  OutLine("(* ------------- Include file: ");
  OutLine(szSrc);
  OutLineCR(" ------------- *)");

  if (!strcmp(szFilename,SPEC_NAME))
    if (!fInclSpecial){
      OutLineCR("(* ------------- This file we'll include late. EOF. --------------- *)");
      OutLineCR("");
      fWasSpecial = TRUE;
      return TRUE;
    }
    else if (!fWasSpecial){
      OutLineCR("(* Sorry. We'll not include it. *)");
      return TRUE;
    }

  hfSrc = fopen(szSrc,"r");
  if (!hfSrc){
    printf("Can't open file: %s\n", szSrc);
    return FALSE;
  }

  nSrcLine = 1;

  while( fgets(szCurLine,sizeof(szCurLine),hfSrc) ){
    for (int i=0; i<MAX_LLEN; i++)
      if (szCurLine[i]=='\n' || szCurLine[i]=='\r' || !szCurLine[i]){
        szCurLine[i] = '\0';
        break;
      }
    if (i==MAX_LLEN){
      szCurLine[i-1] = 0;
      SendError("Line is too large",0);
    }
    OneLine(szCurLine);
    nSrcLine++;
  }

  OutLineCR("");
  OutLine("(* ------------- End of file:  ");
  OutLine(szSrc);
  OutLineCR(" ------------- *)");

  fclose(hfSrc);
  return TRUE;
}


void SendError(char *text, char *pch)
{
  printf("%5u [Out:%6u] %s:\n",nSrcLine,nDstLine,text);
  if (pch>=szCurLine && pch<szCurLine+strlen(szCurLine)){
    char sz[sizeof(szCurLine)];
    int  pos = pch-szCurLine;
    strncpy(sz,szCurLine,pos);
    sz[pos] = 0;
    printf(">>>> %s | %s\n",sz,szCurLine+pos);
  }
  else
    printf(">>>> %s\n",szCurLine);
}

main()
{
  FILE *hfSrc = fopen(HEAD_PATH HEAD_FILE,"r");
  if (!hfSrc){
    printf("Can't open file: %s\n\n", HEAD_PATH HEAD_FILE);
    printf("Use cnv to build " DEST_FILE " file from " HEAD_PATH HEAD_FILE " file(s)\n");
    return 2;
  }
  fclose(hfSrc);



  if (!(hfDst = fopen(DEST_FILE, "w"))){
    printf("Can't create file %s\n",DEST_FILE);
    exit(1);
  }
  nDstLine = 1;
  DoInclude(HEAD_FILE);
  if (!fWasSpecial)
    printf("Warning! File " SPEC_NAME " was newer used. We'll not append it.");
  fclose(hfDst);
  exit(0);
}









