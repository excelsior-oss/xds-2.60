XDS Modula-2/Oberon-2 Development
====================================================

The XDS development system facilitate software development in the Modula-2 and Oberon-2 languages. The unique feature of XDS is seamless integration between the two, which enables you to mix Modula-2 and Oberon-2 modules in one project freely. It includes two toolchains: Native XDS-x86, XDS-C and TopSpeed Compatibility Pack.

#### Native XDS-x86
Native XDS-x86 is an optimizing ISO Modula-2 and Oberon-2 compiler for 32-bit Intel x86 systems running Windows. The Windows version comes with additional suite of tools including debugger, disassembler, etc.

#### XDS-C
XDS-C is a Modula-2/Oberon-2 "via C" compiler: its output is ANSI C, K&R C, or C++ source code, which may then be compiled by a third-party C/C++ compiler for any target platform. This technique allows you to cross program in Modula-2 and/or Oberon-2 for virtually any target environment. XDS-C itself runs on Windows.

#### TopSpeed Compatibility Pack
TopSpeed Compatibility Pack (TSCP) is an add-on for both XDS toolchains that makes them more compatible with the TopSpeed Modula-2 compiler, thus simplifying porting of legacy TopSpeed Modula-2 sources

Building from source for Windows
================================

To build the XDS development system, you will need:

*   Git
*   Microsoft Visual Studio Community 2015 or higher
*   NASM - the Netwide Assembler
*   MinGW - Minimalist GNU for Windows

1. Download the sources:
```
  git clone https://github.com/excelsior-oss/xds-2.60.git
```
2. Go to the "bin" directory in the root of repository and unpack the binary version of the XDS system:
```
  cd xds-2.60\bin 
  unzip xds-260-win32.zip
```
3. Create your building environment:

- Go to the ".config" directory in the root of repository 
```
  cd ..\.config 
```
- Rename the ".env-COMPUTERNAME.bsc" file according to the name of your workstation,

- Set path to external tools in this file.

4. Go to the "build\\Win32" directory in the root of repository and build XDS using "build.bat" script:
```
  cd ..\build\Win32 
  build.bat
```
New file "xds-260-win32.zip" in the current directory is the XDS setup package.  
Unpacked version of the XDS development system is located in the "enduser\\xds" directory. 




Building from source for Linux
================================

To build the XDS development system on Linux, you will need:

*   Git
*   Binary version od XDS 2.60 ("../../bin/xds-260-linux.tgz")
*   GNU make utility (```apt-get install make```) 
*   32-bit version of GCC (```apt-get install gcc-multilib```) 
*   32-bit version of ncurses library (```sudo apt-get install libncurses*:i386```)
*   dos2unix file format converter (```apt-get install dos2unix```)

1. Download the sources:
```
  git clone https://github.com/excelsior-oss/xds-2.60.git
```
2. Go to the "build/x86Linux" repository directory

3. Run ```chmod a+x *.sh``` to grant execution permissions to the build scripts.

4. Run ```get.sh``` to configure the current folder for the XDS build.

5. Run ```build.sh``` to build XDS development system.  

File "xds-260-linux.tgz" in the current folder is the XDS setup package.  
Unpacked version of the XDS development system is located in the "enduser/xds" directory. 



Released under the Apache 2.0 license, see [LICENSE](LICENSE).

Copyright © 2019 [Excelsior LLC](https://excelsior.ru)
