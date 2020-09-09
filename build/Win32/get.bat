@echo off
rem Usage: get.bat [Build_Directory]

set XDS_SRC_DIR=%~dp0\..\..
set XDS_BUILD_DIR=%~1

if "%XDS_BUILD_DIR%" == ""   set XDS_BUILD_DIR=.
if "%XDS_BUILD_DIR%" == "."  goto lbl_get_files

if exist "%XDS_BUILD_DIR%"  (echo Remove existing directory "%XDS_BUILD_DIR%" & rmdir /Q /S "%XDS_BUILD_DIR%" || exit /B 1)
mkdir "%XDS_BUILD_DIR%"

:lbl_get_files
rem ============================================= Create Folders 
for %%i in (debug,src,log) do call :lbl_MakeBuildDir "%%i"
call :lbl_MakeBuildDir "enduser\tmp"
call :lbl_MakeBuildDir "enduser\xds\lib\C"
call :lbl_MakeBuildDir "enduser\xds\lib\x86"
call :lbl_MakeBuildDir "enduser\xds\sym\C"
call :lbl_MakeBuildDir "enduser\xds\sym\x86"

rem ============================================= Copy Build Environment
rem if not "%XDS_BUILD_DIR%" == "."  for %%i in (build.bat,makefile,setenv.bat,clean.bat) do copy /Y "%%i" "%XDS_BUILD_DIR%\%%i"
if not "%XDS_BUILD_DIR%" == "."  for %%i in (build.bat,makefile,setenv.bat,clean.bat) do call :lbl_XCopyFiles "build\Win32\%%i"  ""
if not "%XDS_BUILD_DIR%" == "."  (call :lbl_XCopyDir  ".config"  ".config\"  || exit /B 1)

call :lbl_RemoveFiles "enduser\xds\bin\*.exe"


rem ============================================= Compiler
call :lbl_XCopyDir "comp\src"  "src\"  || exit /B 1      

call :lbl_XCopyFiles "comp\enduser\share\bin\*.*"    "enduser\xds\bin\"  || exit /B 1
call :lbl_XCopyFiles "comp\enduser\windows\bin\*.*"  "enduser\xds\bin\"  || exit /B 1
call :lbl_XCopyFiles "comp\enduser\windows\*.*"      "enduser\xds\"      || exit /B 1

rem ============================================= Debugger 
call :lbl_XCopyDir "debug\src"  "src\"  || exit /B 1      

call :lbl_XCopyFiles "debug\xd_win32\*.msg"  "enduser\xds\bin\"  || exit /B 1

rem ============================================= Readme and Licence files 
call :lbl_XCopyDir "misc\readme\Common"  "enduser\xds\readme\"  || exit /B 1
call :lbl_XCopyDir "misc\readme\Win32"   "enduser\xds\readme\"  || exit /B 1

call :lbl_XCopyDir "misc\licenses\windows"  "enduser\xds\licenses\"  || exit /B 1

rem ============================================= Library 
call :lbl_XCopyDir "lib\src"  "src\"  || exit /B 1      

call :lbl_XCopyFiles "lib\src\isodef\*.def"  "enduser\xds\def\iso\"  || exit /B 1
call :lbl_XCopyFiles "lib\src\syslib\*.def"  "enduser\xds\def\iso\"  || exit /B 1
call :lbl_XCopyFiles "lib\src\pimlib\*.def"  "enduser\xds\def\pim\"  || exit /B 1
call :lbl_XCopyFiles "lib\src\ulib\*.def"    "enduser\xds\def\xds\"   || exit /B 1

call :lbl_XCopyFiles "lib\src\os\POSIX\POSIXIOChan.def"   "enduser\xds\def\xds\"  || exit /B 1
call :lbl_XCopyFiles "lib\src\os\w95\Win32IOChan.def"     "enduser\xds\def\xds\"  || exit /B 1
call :lbl_XCopyFiles "lib\src\os\w95\Win32Processes.def"  "enduser\xds\def\xds\"  || exit /B 1

call :lbl_XCopyFiles "lib\src\lib\XEXCEPTIONS.def"  "enduser\xds\def\xds\"  || exit /B 1
call :lbl_XCopyFiles "lib\src\lib\xFilePos.def"     "enduser\xds\def\xds\"  || exit /B 1
call :lbl_XCopyFiles "lib\src\lib\XIOChan.def"      "enduser\xds\def\xds\"  || exit /B 1
call :lbl_XCopyFiles "lib\src\lib\XProcesses.def"   "enduser\xds\def\xds\"  || exit /B 1

call :lbl_XCopyFiles "lib\src\vterm\*.def"  "enduser\xds\def\Win32\"  || exit /B 1

call :lbl_XCopyFiles "lib\src\TSLibs\*.def"  "enduser\xds\def\ts\"  || exit /B 1
call :lbl_RemoveFiles "enduser\xds\def\ts\x*.def"

call :lbl_MakeBuildDir "enduser\xds\def\ob2"

rem ============================================= Win32\POSIX API 
call :lbl_XCopyFiles "api\src\Win32\enduser\*.*"       "enduser\xds\def\Win32\"        || exit /B 1
call :lbl_XCopyFiles "api\src\Win32\build\*.lst"       "enduser\xds\def\Win32\build\"  || exit /B 1
call :lbl_XCopyFiles "api\src\Win32\build\import.mod"  "enduser\xds\def\Win32\build\"  || exit /B 1
call :lbl_XCopyFiles "api\src\Win32\build\pragmas"     "enduser\xds\def\Win32\build\"  || exit /B 1

call :lbl_XCopyDir   "api\src\Win32"         "src\Win32\"              || exit /B 1
call :lbl_XCopyFiles "api\src\Win32\*.def"   "enduser\xds\def\Win32\"  || exit /B 1

call :lbl_XCopyDir   "api\src\POSIX"            "src\POSIX\"                  || exit /B 1
call :lbl_XCopyFiles "api\src\posix\*.def"      "enduser\xds\def\posix\"      || exit /B 1
call :lbl_XCopyFiles "api\src\posix\*.prj"      "enduser\xds\def\posix\"      || exit /B 1
call :lbl_XCopyFiles "api\src\posix\mac\*.def"  "enduser\xds\def\posix\mac\"  || exit /B 1
call :lbl_XCopyFiles "api\src\posix\mac\*.prj"  "enduser\xds\def\posix\mac\"  || exit /B 1

rem ============================================= Samples 
call :lbl_XCopyDir "misc\samples\Common"      "enduser\xds\samples\"  || exit /B 1
call :lbl_XCopyDir "misc\samples\Native"      "enduser\xds\samples\"  || exit /B 1
call :lbl_XCopyDir "misc\samples\Win32"       "enduser\xds\samples\"  || exit /B 1
call :lbl_XCopyDir "misc\samples\TopSpeed"    "enduser\xds\samples\"  || exit /B 1
call :lbl_XCopyDir "misc\samples\H2D"         "enduser\xds\samples\h2d\"  || exit /B 1

rem ============================================= Utils
call :lbl_XCopyDir   "utils\h2d\src"          "src\h2d\"          || exit /B 1      
call :lbl_XCopyFiles "utils\h2d\src\h2d.msg"  "enduser\xds\bin\"  || exit /B 1
call :lbl_XCopyFiles "utils\h2d\src\h2d.cfg"  "enduser\xds\bin\"  || exit /B 1

call :lbl_XCopyDir "utils\xdasm\src"  "src\xdasm\"  || exit /B 1      

call :lbl_XCopyDir "utils\xlib\src"   "src\xlib\"   || exit /B 1      

call :lbl_XCopyDir "utils\xlink\src"  "src\xlink\"  || exit /B 1      

call :lbl_XCopyDir   "utils\xrc\src"              "src\xrc\"          || exit /B 1      
call :lbl_XCopyFiles "utils\xrc\src\windows.stb"  "enduser\xds\bin\"  || exit /B 1

rem ============================================= IDE 
rem NOTE: IDE generation disabled.
rem call :lbl_XCopyDir "ide\Win32\src"  "src\"  || exit /B 1
rem 
rem call :lbl_XCopyFiles "ide\Win32\src\xShell\src\java\*.kwd"  "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\src\xShell\src\m2\*.kwd"    "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\enduser\xds\bin\*.opt"      "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\enduser\xds\bin\*.tpr"      "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\enduser\xds\bin\*.trd"      "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\enduser\xds\bin\*.mdf"      "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\enduser\xds\bin\*.bat"      "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\enduser\xds\bin\xdsc.ini"   "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\enduser\xds\bin\xdsx86.ini" "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\enduser\xds\bin\xrc.fil"    "enduser\xds\bin\"  || exit /B 1
rem call :lbl_XCopyFiles "ide\Win32\enduser\xds\bin\xds.cnt"    "enduser\xds\bin\"  || exit /B 1

rem ============================================= Help
rem NOTE: Help generation disabled 
rem Prebuilt pdf documentation is provided
call :lbl_XCopyFiles "misc\Doc\enduser\share\pdf\*.pdf"   "enduser\xds\pdf\"  || exit /B 1
call :lbl_XCopyFiles "misc\Doc\enduser\windows\pdf\*.pdf" "enduser\xds\pdf\"  || exit /B 1

rem call :lbl_XCopyDir "misc\doc\doc"      "doc\"          || exit /B 1      
rem call :lbl_XCopyDir "misc\doc\convert"  "doc\convert\"  || exit /B 1      

echo === Ok ===  Creation of process is successfully complete
echo Run build.bat in "%XDS_BUILD_DIR%" 
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
