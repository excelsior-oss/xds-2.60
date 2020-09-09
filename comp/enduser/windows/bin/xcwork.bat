@echo off
for %%s in (DEF OBJ SRC SYM) do if not exist %%s mkdir %%s
echo> xc.tpr %% Native XDS-x86 project file
echo.>>xc.tpr 
echo>>xc.tpr -lookup = *.def = $!\def
echo>>xc.tpr -lookup = *.mod = $!\src
echo>>xc.tpr -lookup = *.ob2 = $!\src
echo>>xc.tpr -lookup = *.prj = $!
echo>>xc.tpr -lookup = *.sym = .\sym;
echo>>xc.tpr -lookup = *.obj = .\obj
echo>>xc.tpr -lookup = *.lnk = .\obj
echo.>>xc.tpr 
echo>>xc.tpr !module %% Type the name of your main module here

echo    DONE

