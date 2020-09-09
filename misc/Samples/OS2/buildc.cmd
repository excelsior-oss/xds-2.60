cd FINDPROC
xm =m findproc
if errorlevel 1 goto error
cd ..

cd H2D
h2d =p example.h2d
if errorlevel 1 goto error
cd ..

cd HUFFCHAN
xm =m huf.mod
if errorlevel 1 goto error
xm =m unhuf.mod
if errorlevel 1 goto error
cd ..

cd MAND
rc -r mand.rc mand.res
if errorlevel 1 goto error
xm =p mand
if errorlevel 1 goto error
cd ..

cd MODULA
xm =m bf.mod
if errorlevel 1 goto error
xm =m e.mod
if errorlevel 1 goto error
xm =m except.mod
if errorlevel 1 goto error
xm =m exp.mod
if errorlevel 1 goto error
xm =m fact.mod
if errorlevel 1 goto error
xm =m halt.mod
if errorlevel 1 goto error
xm =m hello.mod
if errorlevel 1 goto error
xm =m hisdemo.mod +gendebug +lineno
if errorlevel 1 goto error
xm =m queens.mod
if errorlevel 1 goto error
xm =m sieve.mod
if errorlevel 1 goto error
xm =m term.mod
if errorlevel 1 goto error
cd ..

cd NODES
xm =p runme
if errorlevel 1 goto error
cd ..

cd OBERON
xm =m ackerman.ob2
if errorlevel 1 goto error
xm =m exp.ob2
if errorlevel 1 goto error
xm =m gcreport.ob2
if errorlevel 1 goto error
xm =m hello.ob2
if errorlevel 1 goto error
xm =m Random.ob2
if errorlevel 1 goto error
xm =m self.ob2
if errorlevel 1 goto error
xm =m sieve.ob2
if errorlevel 1 goto error
cd ..

cd TEMPLATE
if not exist ch  mkdir ch
if not exist obj mkdir obj
if not exist sym mkdir sym
h2d =p template.h2d
if errorlevel 1 goto error
rc -r -I res res\template.rc
if errorlevel 1 goto error
xm =p template.prj
if errorlevel 1 goto error
cmd /c ipfc hlp\template.ipf template.hlp
if errorlevel 1042 goto error
if errorlevel 1041 goto noipfc
if errorlevel 1 goto error
goto ipfcok
:noipfc
echo Could not run IPFC 
:ipfcok
cd ..

goto quit

:error
echo *** FAULT ***
cd ..

:quit

