@echo off
for %%s in (CH DEF OBJ SRC SYM) do if not exist %%s mkdir %%s
echo> xm.tpr %% XDS-C project file
echo.>>xm.tpr 
echo>>xm.tpr -lookup = *.def = $!/def
echo>>xm.tpr -lookup = *.mod = $!/src
echo>>xm.tpr -lookup = *.ob2 = $!/src
echo>>xm.tpr -lookup = *.prj = $!
echo>>xm.tpr -lookup = *.c   = ./ch
echo>>xm.tpr -lookup = *.h   = ./ch
echo>>xm.tpr -lookup = *.obj = ./obj
echo>>xm.tpr -lookup = *.mkf = .
echo.>>xm.tpr 
echo>>xm.tpr !module %% Type the name of your main module here

echo    DONE

