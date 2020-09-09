/////////////////////// File winmake.h

#define ERRNUMBERS 1

#define MSG_NOTICE      'N'
#define MSG_WARNING     'W'
#define MSG_ERROR       'E'
#define MSG_SEVERE      'S'

// Calls for XDS compiler
BOOL    StartMake();                 // return TRUE if shell is connected (XC must call
                                     //       it only when  __XDS_SHELL__ key presents)
void    EndMake();                   // disconnest shell
void    SendError                    // Add message to message list
             (char   err_class,      // MSG_  constant
              long x,long y,         // File position (pos/line) /* MSG_ERROR, MSG_WARNING */
              char * filename,       // Full(!) filename         /* MSG_ERROR, MSG_WARNING */
              char * body            // Error text
             );
void    SendString  (char * s);      // Add string to the make window listbox
void    SendCaption (char * s);      // Set make window caption (under the window title)
void    SendComment (char * s);      // Set comment (under progress indicator)
void    SendStartJob                 // Tune progress indicator (and reset it)
             (char * comment,        //   Progress text
              long progress_limit    //   Max. progress value
             );
void    SendProgress                 // Move progress indicator
                                     /*+++ comment_progress реально не используется: */
             (long comment_progress, //   Value to be indicated right to the comment (SendStartJob)
                                     //     we'll not display if it is less then 0
              long progress);        //   New progress value (0..progress_limit)

void    StartFileList();             // Clear project filelist & start to recive filenames
void    AppendFile   (char *szFile); // Add 'szFile' to the project filelist
void    EndFileList  ();             // Complite filelist transferring
void    SortOff      ();             // Don't sort messages in the message list

#ifdef __WINMAKER
////////////////////////////////////////////////////////////////////////////////////////////////
//
// Shell internal part
//

  #define COMMONMEMNAME    "\\SHAREMEM\\XDSSHMEM.XDS"

  #define CHG_ENDMAKE      0x00000001l
  #define CHG_SENDERROR    0x00000002l
  #define CHG_SENDCAPTION  0x00000004l
  #define CHG_SENDCOMMENT  0x00000008l
  #define CHG_SENDSTARTJOB 0x00000010l
  #define CHG_SENDPROGRESS 0x00000020l
  #define CHG_STARTFILES   0x00000040l
  #define CHG_APPENDFILE   0x00000080l
  #define CHG_ENDFILES     0x00000100l
  #define CHG_SORT_OFF     0x00000200l

  #pragma pack(1)

  struct COMMONMEM
  {
    ULONG    ulChanged;       // CHG_*  flags
    // --- SendError
    ULONG    chErrClass;      // MSG_*  constant
#if ERRNUMBERS
    ULONG    ulErrNum;
#endif
    long     lPos, lLine;     // File position (pos/line) /* MSG_ERROR, MSG_WARNING */
    char     szFilename[256]; // Full(!) filename         /* MSG_ERROR, MSG_WARNING */
    char     szBody[256];     // Error text
    // --- SendCaption
    char     szCaption[256];
    // --- SendComment
    char     szComment[256];
    // --- SendStartJob
    long     lProgressLimit;
    char     szProgressComment[100];
    // --- SendProgress
    long     lProgress;
    long     lCommentProgress;
    // --- AppendFile
    char     szFile[CCHMAXPATH];
  };
  typedef COMMONMEM *PCOMMONMEM;

  #pragma pack()
#endif /* __WINMAKER */













