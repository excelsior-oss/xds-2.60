How to build Native XDS-x86 for Windows
=======================================

To build the XDS development system, you will need:

*   Microsoft Visual Studio Community 2015 or higher
*   NASM - the Netwide Assembler
*   MinGW - Minimalist GNU for Windows

1. Go to the "bin" repository directory and unpack the binary version of the XDS system:
```
  cd ..\..\bin 
  unzip xds-260-win32.zip
```
2. Setup your building environment in the "..\\..\\.config" folder:

- Rename the ".env-COMPUTERNAME.bsc" file according to the name of your workstation,

- Set correct paths to external tools in this file.

3. Run ```get.bat``` script to configure the current folder for the XDS build.

4. Run ```build bat``` script in this directory to build XDS development system.

File "xds-260-win32.zip" in the current directory is the XDS setup package.  
Unpacked version of the XDS development system is located in the "enduser\\xds" directory. 

In the Step 2, as a parameter of ```get.bat <xds-build-folder>``` you can specify the path   
to an empty folder that will be configured for the XDS build.  
In this case, on the Step 3 you should to go to the specified folder and run ```build.bat``` there. 
