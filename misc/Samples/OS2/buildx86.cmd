cd BENCH
xc =m dry.mod
if errorlevel 1 goto error
xc =m linnew.mod
if errorlevel 1 goto error
xc =m whet.mod
if errorlevel 1 goto error
cd ..

cd DLL
xc =p dll1
if errorlevel 1 goto error
xc =p dll2
if errorlevel 1 goto error
xc =p exe1
if errorlevel 1 goto error
xc =p exe2
if errorlevel 1 goto error
cd ..

cd FINDPROC
xc =m findproc
if errorlevel 1 goto error
cd ..

cd H2D
h2d =p example.h2d
if errorlevel 1 goto error
cd ..

cd HUFFCHAN
xc =m huf.mod
if errorlevel 1 goto error
xc =m unhuf.mod
if errorlevel 1 goto error
cd ..

cd MAND
rc -r mand.rc mand.res
if errorlevel 1 goto error
xc =p mand
if errorlevel 1 goto error
cd ..

cd MGDEMO
xc =p mgdemo.prj
if errorlevel 1 goto error
cd ..

cd MODULA
xc =m bf.mod
if errorlevel 1 goto error
xc =m e.mod
if errorlevel 1 goto error
xc =m except.mod
if errorlevel 1 goto error
xc =m exp.mod
if errorlevel 1 goto error
xc =m fact.mod
if errorlevel 1 goto error
xc =m halt.mod
if errorlevel 1 goto error
xc =m hello.mod
if errorlevel 1 goto error
xc =m hisdemo.mod +map +genhistory +lineno
if errorlevel 1 goto error
xc =m queens.mod
if errorlevel 1 goto error
xc =m sieve.mod
if errorlevel 1 goto error
xc =m term.mod
if errorlevel 1 goto error
cd ..

cd NODES
xc =p runme
if errorlevel 1 goto error
cd ..

cd OBERON
xc =m ackerman.ob2
if errorlevel 1 goto error
xc =m exp.ob2
if errorlevel 1 goto error
xc =m gcreport.ob2
if errorlevel 1 goto error
xc =m hello.ob2
if errorlevel 1 goto error
xc =m Random.ob2
if errorlevel 1 goto error
xc =m self.ob2
if errorlevel 1 goto error
xc =m sieve.ob2
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
xc =p template.prj
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

cd WINDEMOM
xc =p windemom.prj
if errorlevel 1 goto error
cd ..

goto quit

:error
echo *** FAULT ***
cd ..

:quit

