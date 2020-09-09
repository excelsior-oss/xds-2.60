#include<stdio.h>
#include<stdlib.h>
#include<string.h>
char * skipspaces (char * p)
 {
   while (*p == ' ' || *p == '\t') ++p;
   return p;
 }
int findit(char * str)
 {
   str = skipspaces(str);
   return !strncmp(str, "PROCEDURE", 9) || !strncmp(str, "VAR", 3) ||
	  !strncmp(str, "TYPE", 4)      || !strncmp(str, "CONST", 5);
 }
int endofmodule(char * str, char * nmod)
 {
   str = skipspaces(str);
   if (!strncmp(str, "END", 3))
     {
       str += 3;
       str = skipspaces(str);
       if (!strncmp(str, nmod, strlen(nmod)))
	 {
	   str += strlen(nmod);
	   str = skipspaces(str);
	   return *str == '.';
	 }
       return 0;
     }
   else return 0;
 }
void main()
{
  FILE * fi, * fo;
  char listname[] = "build\\list";
  char outname[]  = "os9k.def";
  char modulename[]="os9k";
  char pragmas[]  = "build\\pragmas";
  char list[40][40];
  int j, i = 0;
  char buffer[256];
  /* reading the listfile */
  fi = fopen(listname, "r");
  if (!fi) { printf("Error: file %s not found", listname); exit(1); }
  while (fscanf(fi,"%s", list[i]) != EOF) i++;
  fclose(fi);
  fo = fopen(outname, "w");
  if (!fo) { printf("Error: file %s could not be opened for writing", outname); exit(1);}
  fi = fopen(pragmas, "r");
  if (!fi) { printf("Error: file %s not found", pragmas); exit(1); }
  while (fgets(buffer, 256, fi))
    { int error;
      error = fputs(buffer, fo);
      if (error==EOF)
	{ printf("Error: cannot write to %s file", outname);
	  exit(1);
	}
    }
  fclose(fi);
  fprintf(fo, "DEFINITION MODULE [\"C\"] %s;\n", modulename);
  fprintf(fo,"\nIMPORT SYSTEM;\n");
  for (j=0; j<i; j++)
    { /* fopen the regular file and add its body to our file */
      char fname[40+4];
      strcpy(fname, list[j]);
      fi = fopen(strcat(fname,".def"), "r");
      if (!fi)
	{  printf("Error: file %s not found", fname);
	   exit(1);
	}
      /* Search the string begining with PROCEDURE or VAR or TYPE */
      while (!findit(fgets(buffer, 256, fi)));
      while (!endofmodule(buffer, list[j]))
	{  /* process the buffer and output it */
	   char * bufpoint = buffer;
	   char outputstr[256];
	   char * outpoint = outputstr;
	   int k;
	   while (*bufpoint)
	    {
	      for (k = 0; k<i; k++)
		{
		  int length;
		  length = strlen(list[k]);
		  if (!strncmp(bufpoint, list[k], length) && bufpoint[length] =='.')
		      bufpoint += length+1;
		}
	      *(outpoint++) = *(bufpoint++);
	    }
	   *outpoint = '\0';
	   fputs(outputstr, fo);
	   fgets(buffer, 256, fi);
	}
      fclose(fi);
    }

  fprintf(fo,"END %s.\n", modulename);
  fclose(fo);
 }