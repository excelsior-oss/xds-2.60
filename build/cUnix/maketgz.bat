@echo off

if not exist enduser  call get.bat

call setenv.bat

if exist x86Linux.tgz  del /Q x86Linux.tgz

for %%i in (build*.sh,make*.sh,pack*.sh,setenv.sh) do  attrib -R %%i

for %%i in (dos2unix.exe) do  set CONVER_TOOL=%%~$PATH:i
if "%CONVER_TOOL%"=="" for %%i in (d2u.exe) do  set CONVER_TOOL=%%~$PATH:i
if not "%CONVER_TOOL%"=="" "%CONVER_TOOL%" build*.sh make*.sh pack*.sh setenv.sh enduser/xds/bin/*.msg

tar cvfz "cUnix.tgz" enduser src bin build.sh makexds-c.sh pack-xds.sh setenv.sh 
