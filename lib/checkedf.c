#include <process.h>
#include <io.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#define  MAX_LENGTH  200
struct list {
    char  ident[ MAX_LENGTH ];
    struct list * next;
};

struct list * head, *tail, *ptr;


 void add_list( char *ident ){
     if ( head== NULL ){
       head = ( struct list *)malloc( sizeof(struct list) );
       strcpy( head->ident , ident);
       head->next  = NULL;
       tail = head;
     } else {
      tail->next = ( struct list * )malloc(sizeof(struct list) );
      tail = tail->next;
      strcpy( tail->ident , ident);
      tail->next  = NULL;
     }
 }

void ReadString(char *buf, int *pos,char *ident,int size){
  int i;

  while (( isspace(buf[(*pos)]) )&&( (*pos)<=size )) {(*pos)++;}
  i=0;
  while  (( buf[*pos] != 0x0D)&&( *pos<size  )&& ( buf[*pos] != 0x0A)) { 
    ident[i++]=buf[(*pos)++]; 
  }
  if (( buf[*pos] == 0x0D )||( buf[*pos] == 0x0A) ) { (*pos)++;}
  if ( ( ident[i] == 0x0D )||( ident[i] == 0x0A) )
      { ident[i-1]=0;}
  else
     { ident[i]=0;  }; 
}

void first( char *arg1, char *arg2){
  int i;
	for ( i=0;  !isspace(arg1[i]) ;i++ ){
	   arg2[i]=arg1[i];
	 }
	 arg2[i]=0;
}

void  work( char * ident){
 
  struct list * ptr1;
   
  if ( strcmp( head->ident , ident) == 0 ){
   	ptr1 = head;
	head= head->next;
	free(ptr1);
	return;
   }		   
   ptr = head;
   while( ptr->next != NULL){
	   if ( strcmp(ident, ptr->next->ident) ==0 ){		  
		   ptr1 = ptr->next;
		   ptr->next = ptr->next->next;
		   free(ptr1);
		   return;
	   }
	   ptr=ptr->next;
   }
   printf( "   %s\n",ident);
   return;
}

void main(int argc, char **argv){
  int  edf ,lib ;
  int  res ;
  unsigned int size_edf ,size_lib , pos;
  char    ident[ MAX_LENGTH ],ident1[ MAX_LENGTH ];
  char *  buf_edf , * buf_lib;
  char *  path;

  if (argc <3 ){
  	  printf( "util.exe file1.lib  file2.edf " );
	  exit(1);
  }
  path = "xlib.exe  /LIST  /NOLOGO  >out153.$$$ ";
  strcat( path ,argv[1] );
  res = system( path );
  if ( res != 0 ) {
  	 printf("xlib error \n" );
	 exit(1);
  }
	
  edf = open( argv[2], O_RDONLY  | O_BINARY);
  if ( edf == -1 ){
  	printf( "edf file not open" );
	exit(1);
  }
  size_edf = filelength(edf);
  buf_edf = (char * )malloc( size_edf );
  res = read(edf , buf_edf, size_edf );
  pos = 0;
  
  head = NULL;
  do{
  	ReadString(buf_edf, &pos, ident , size_edf);
  }while (( strncmp(ident,"EXPORTS",7) != 0)&&(pos <= size_edf) );
  if ( pos == size_edf ){
  	 printf("Incorrect edf file \n");
	 exit(1);
  }
  
  do{
  	ReadString(buf_edf, &pos, ident , size_edf);
	first( ident, ident1);
	add_list( ident1 );
  }while ( pos < size_edf );
  free ( buf_edf );

  lib = open("out153.$$$" , O_RDONLY  | O_BINARY );
  if ( lib == -1 ){
  	printf( "lib file not open" );
	exit(1);
  }
  pos = 0;
  size_lib = filelength( lib );
  buf_lib = (char*)malloc( size_lib );
  read(lib, buf_lib, size_lib);

  do{
   ReadString(buf_lib, &pos, ident , size_lib);
 }while (( strncmp(ident, "MODULE",6) != 0 )&&( pos <=size_lib ));
 
  do{
   ReadString(buf_lib, &pos, ident , size_lib);
   first( ident, ident1 );
   if ( strncmp(ident1 , "MODULE", 6) != 0){
   	 work( ident1);	 
   }
 } while (pos < size_lib );

 free( buf_lib);
}