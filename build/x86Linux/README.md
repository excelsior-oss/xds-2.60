How to build Native XDS-x86 for Linux
=====================================

To build the XDS-x86 development system on Linux, you will need:

*   Binary version od XDS 2.60 ("../../bin/xds-260-linux.tgz")
*   GNU make utility (```apt-get install make```) 
*   32-bit version of GCC (```apt-get install gcc-multilib```) 
*   32-bit version of ncurses library (```sudo apt-get install libncurses*:i386```)
*   dos2unix file format converter (```apt-get install dos2unix```)

1. Run ```chmod a+x *.sh``` to grant execution permissions to the build scripts.

2. Run ```get.sh``` to configure the current folder for the XDS build.

3. Run ```build.sh```  to build XDS development system.  

File "xds-260-linux.tgz" in the current folder is the XDS setup package.  
Unpacked version of the XDS development system is located in the "enduser/xds" directory. 


In the Step 2, as a parameter of ```get.sh <xds-build-folder>``` you can specify the path   
to an empty folder that will be configured for the XDS build.  
In this case, on the Step 3 you should to go to the specified folder and run ```build.sh``` there. 

---  
  
You can prepare the build folder on Windows system then transfer it on Linux system and build XDS there.

To transfer the XDS development system from Windows to Linux, you will need:

*   MinGW - Minimalist GNU for Windows


On a Windows system

1. Setup building environment for Windows (Step 2 in [README.md](../Win32/README.md))  

2. Run ```get.bat``` script.

3. Run ```maketgz.bat``` script.

4. Transfer the resulting archive "x86Linux.tgz" into an empty directory on a Linux system.

On a Linux system

5. Unpack the archive ```tar xvf x86Linux.tgz```.

6. Run ```chmod a+x *.sh```.

7. Unpack the binary XDS package in the "bin" subdirectory ```tar xvf xds-260-linux.tgz```.

8. Run ```build.sh``` to build XDS setup package.

File "xds-260-linux.tgz" in the current directory is the XDS setup package.

Be sure to change line separators from DOS to Unix in the following files:
  build.sh
  makexds-c.sh
  makexds-x86.sh
  pack-xds.sh

8. Run ```makexds-x86.sh clean all``` to build xds-x86.
8. Run ```makexds-c.sh clean all``` to build xds-C.
