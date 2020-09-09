@echo off
rem Usage: get.bat [Build_Directory]

set XDS_SRC_DIR=%~dp0\..\..
set XDS_BUILD_DIR=%~1

if "%XDS_BUILD_DIR%" == ""   set XDS_BUILD_DIR=.
if "%XDS_BUILD_DIR%" == "."  goto lbl_get_files

if exist "%XDS_BUILD_DIR%"  (echo Remove existing directory "%XDS_BUILD_DIR%" & rmdir /Q /S "%XDS_BUILD_DIR%"  || exit /B 1)
mkdir "%XDS_BUILD_DIR%"

call :lbl_MakeBuildDir "bin"
call :lbl_XCopyFiles "bin\xds-260-linux.tgz"  "bin\"  || exit /B 1

:lbl_get_files
rem ============================================= Create Folders 
call :lbl_MakeBuildDir "log"
call :lbl_MakeBuildDir "src"

call :lbl_MakeBuildDir "enduser/xds/bin"

call :lbl_MakeBuildDir "enduser/xds/C/C"
call :lbl_MakeBuildDir "enduser/xds/include/C"
call :lbl_MakeBuildDir "enduser/xds/lib/C"
call :lbl_MakeBuildDir "enduser/xds/sym/C"

call :lbl_MakeBuildDir "enduser/xds/def/iso"
call :lbl_MakeBuildDir "enduser/xds/def/ob2"
call :lbl_MakeBuildDir "enduser/xds/def/pim"
call :lbl_MakeBuildDir "enduser/xds/def/POSIX"
call :lbl_MakeBuildDir "enduser/xds/def/ts"
call :lbl_MakeBuildDir "enduser/xds/def/X11"
call :lbl_MakeBuildDir "enduser/xds/def/xds"
call :lbl_MakeBuildDir "enduser/xds/def/Xm"

call :lbl_MakeBuildDir "enduser/xds/doc"
call :lbl_MakeBuildDir "enduser/xds/pdf"
call :lbl_MakeBuildDir "enduser/xds/readme"

call :lbl_MakeBuildDir "enduser/xds/licenses"

if not "%XDS_BUILD_DIR%" == "."  for %%i in (maketgz.bat,setenv.bat,readme) do call :lbl_XCopyFiles "build\cUnix\%%i"  ""
if not "%XDS_BUILD_DIR%" == "."  for %%i in (build.sh,makexds-c.sh,pack-xds.sh,setenv.sh)   do call :lbl_XCopyFiles "build\cUnix\%%i"  ""

if not "%XDS_BUILD_DIR%" == "."  (call :lbl_XCopyDir  ".config"  ".config\"  || exit /B 1)

rem ============================================= Compiler
call :lbl_XCopyDir "comp\src"  "src\"  || exit /B 1     

call :lbl_XCopyFiles "comp\enduser\share\bin\*.*"  "enduser\xds\bin\"  || exit /B 1
call :lbl_XCopyFiles "comp\enduser\linux\bin\*.*"  "enduser\xds\bin\"  || exit /B 1
call :lbl_RemoveFiles "\enduser\xds\bin\xc*"

rem ============================================= API 
call :lbl_XCopyFiles "api\enduser\linux\*-c"    "enduser\xds\"  || exit /B 1
call :lbl_XCopyFiles "api\enduser\linux\setenv" "enduser\xds\"  || exit /B 1

call :lbl_XCopyDir "api\src\X11"     "src\X11\"    || exit /B 1      
call :lbl_XCopyDir "api\src\POSIX"   "src\POSIX\"  || exit /B 1      
call :lbl_XCopyDir "api\src\Linux"   "src\Linux\"  || exit /B 1      

rem ============================================= Library 
call :lbl_XCopyDir "\lib\src"  "src\"  || exit /B 1      

call :lbl_XCopyFiles "lib\src\TSLibs\*.def"  "enduser\xds\def\ts\"  || exit /B 1
for %%i in (MsMouse.def,Graph.def) do call :lbl_RemoveFiles "enduser\xds\def\ts\%%i"
call :lbl_RemoveFiles "enduser\xds\def\ts\x*.def"

rem ============================================= Utils
call :lbl_XCopyDir   "utils\h2d\src"          "src\h2d\"          || exit /B 1    
call :lbl_XCopyFiles "utils\h2d\src\h2d.msg"  "enduser\xds\bin\"  || exit /B 1
call :lbl_XCopyFiles "utils\h2d\src\h2d.cfg"  "enduser\xds\bin\"  || exit /B 1

call :lbl_XCopyDir "utils\xlib\src"   "src\xlib\"   || exit /B 1      
call :lbl_XCopyDir "utils\xlink\src"  "src\xlink\"  || exit /B 1      


rem ============================================= Licence files  
call :lbl_XCopyFiles "misc\licenses\linux\licenses.txt" "enduser\xds\licenses\"  || exit /B 1
call :lbl_XCopyFiles "misc\licenses\linux\xdsc.txt"     "enduser\xds\licenses\"  || exit /B 1

rem ============================================= Readme files 
call :lbl_XCopyDir "misc\readme\common"    "enduser\xds\readme\"  || exit /B 1      
call :lbl_XCopyDir "misc\readme\x86Linux"  "enduser\xds\readme\"  || exit /B 1      

rem ============================================= Samples 
call :lbl_XCopyDir "misc\samples\common"   "enduser\xds\samples\"      || exit /B 1      
call :lbl_XCopyDir "misc\samples\h2d"      "enduser\xds\samples\h2d\"  || exit /B 1      
call :lbl_XCopyDir "misc\samples\Native"   "enduser\xds\samples\"      || exit /B 1      

call :lbl_XCopyFiles "misc\samples\xdssamp"  "enduser\xds\bin\"      || exit /B 1      
call :lbl_XCopyFiles "misc\samples\buildc"   "enduser\xds\samples\"  || exit /B 1      


rem ============================================= Help 
rem NOTE: Help generation disabled
rem if not "%XDS_BUILD_DIR%" == "."  call :lbl_XCopyFiles "build\cUnix\makedoc.bat"  ""

rem call :lbl_XCopyFiles "misc\Doc\enduser\share\doc\index.html" "enduser\xds\doc\index.html"  || exit /B 1
rem call :lbl_XCopyDir "misc\doc\doc"      "doc\"          || exit /B 1     
rem call :lbl_XCopyDir "misc\doc\convert"  "doc\convert\"  || exit /B 1      

rem Prebuilt pdf documentation is provided
call :lbl_XCopyFiles "misc\Doc\enduser\share\pdf\*.pdf"  "enduser\xds\pdf\"  || exit /B 1
call :lbl_XCopyFiles "misc\Doc\enduser\linux\pdf\*.pdf"  "enduser\xds\pdf\"  || exit /B 1
call :lbl_RemoveFiles "\enduser\xds\pdf\xc*"


echo === Ok ===  Creation of process is successfully complete
echo See readme in "%XDS_BUILD_DIR%" 
goto :EOF


rem ================================= Command aliases

:lbl_MakeBuildDir
if not exist "%XDS_BUILD_DIR%/%~1"  mkdir "%XDS_BUILD_DIR%/%~1"
goto :EOF

:lbl_XCopyFiles
echo xcopy "%XDS_SRC_DIR%\%~1" "%XDS_BUILD_DIR%\%~2"
xcopy "%XDS_SRC_DIR%\%~1" "%XDS_BUILD_DIR%\%~2"  /Y /Q  || exit /B 1
goto :EOF

:lbl_XCopyDir
echo xcopy "%XDS_SRC_DIR%\%~1" "%XDS_BUILD_DIR%\%~2"
xcopy "%XDS_SRC_DIR%\%~1" "%XDS_BUILD_DIR%\%~2"  /Y /Q /E  || exit /B 1
goto :EOF

:lbl_RemoveFiles
if exist "%XDS_BUILD_DIR%\%~1"  del /Q "%XDS_BUILD_DIR%\%~1" 
goto :EOF
