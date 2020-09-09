README.TXT                    Copyright (c) 1999-2011 Excelsior.
----------------------------------------------------------------

                  Excelsior XDS Win32 Edition
                  ---------------------------
                            v2.60

                        Read Me First!


1. Overview
-----------

Your XDS setup package includes three products:
  - Native XDS-x86,
  - XDS-C and
  - TopSpeed Compatibility Pack.

Native XDS-x86 for Windows is a complete software development 
toolset featuring Modula-2 and Oberon-2 languages.

XDS-C is a "via C" Modula-2/Oberon-2 cross-compiler. Its output is 
K&R C, ANSI C, or C++ source code, which may be further compiled by 
a third-party C/C++ compiler for any target platform. 

TopSpeed Compatibility Pack (TSCP) is an add-on for XDS compilers 
making them more compatible with the TopSpeed Modula-2 compiler, 
thus simplifying porting of legacy TopSpeed Modula-2 sources.


2. Directory Structure
----------------------

In  the  following text we assume that you have chosen the C:\XDS
directory for installation. If another directory was used, please
use the name of that directory instead.

The following directory structure is created during installation:

C:\XDS\
    BIN         executable and system files
    DEF\        definition modules:
        ISO         ISO Modula-2 library 
        OB2         Oberon-2 library pseudo-definition modules
        PIM         PIM library
        TS          TopSpeed-like library
        WIN32       Win32 API 
        XDS         XDS library 
    C\          run-time library C sources
        C           XDS-C
    INCLUDE\    run-time library C headers
        C           XDS-C
    LIB\        library files:
        C           XDS-C
        X86         Native XDS-x86
    SYM\        symbol files:
        C           XDS-C
        X86         Native XDS-x86
    README      useful texts
    SAMPLES     Modula-2/Oberon-2 code samples
    UnInst      Uninstaller files


2. How to start
----------------

To invoke XDS Environment, select the "XDS Environment" item from
the XDS folder of your Start menu or type

    xds

at the command prompt and press Enter. 

To start the Native XDS-x86 compiler form the command prompt, type

    xc {options | modes} module_or_project_file {options | modes}

and press Enter. Specifying no arguments would cause a brief help 
to be printed.

To set up a working directory for a new project, create a new directory,
go to it, and type

    xcwork

This would create subdirectories for source and output files and a 
project file template, XC.TPR.


3. See also
-----------

Excelsior XDS package is configured for Native XDS-x86 out-of-the-box.
To use XDS-C you should tune it for your C compiler. See "xdsc.txt" 
in the README subdirectory for details.

To view the on-line documentation, type

    start xds.hlp 

at the command prompt or select the "XDS Help" item in the Excelsior XDS
folder of your Start menu.

The following text files in the README subdirectory contain additional
information:

    report.txt       bug reporting guidelines
    samples.txt      sample programs description
    whatsnew.txt     what is new in this release
    xdsc.txt         XDS-C readme
    xdsx86.txt       Native XDS-x86 readme

                         [end of document]
