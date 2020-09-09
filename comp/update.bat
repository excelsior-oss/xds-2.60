@set target=.\enduser\windows
@if not "%~1" == "" @set target=%~1\bin

@if exist xc.exe  copy xc.exe "%target%\bin\"
@if exist xm.exe  copy xm.exe "%target%\bin\"