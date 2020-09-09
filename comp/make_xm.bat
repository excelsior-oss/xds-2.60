@call "..\.config\setenv.bat" || exit /B 1
rem XDSDIR is used to avoid using local version of  xc
%XDSDIR%\bin\xc =p src\m\xm.prj =a %*