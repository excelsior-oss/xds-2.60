cd BENCH
xc =m =a dry.mod
if errorlevel 1 goto error
xc =m =a linnew.mod
if errorlevel 1 goto error
xc =m =a whet.mod
if errorlevel 1 goto error
cd ..

cd DLL
xc =p =a dll1
if errorlevel 1 goto error
xc =p =a dll2
if errorlevel 1 goto error
xc =p =a exe1
if errorlevel 1 goto error
xc =p =a exe2
if errorlevel 1 goto error
cd ..

cd GENERIC
xrc /stb generic.rc
if errorlevel 1 goto error
xc =p =a generic
if errorlevel 1 goto error
cd ..

cd H2D
h2d =p example.h2d
if errorlevel 1 goto error
cd ..

cd HUFFCHAN
xc =m =a huf.mod
if errorlevel 1 goto error
xc =m =a unhuf.mod
if errorlevel 1 goto error
cd ..

cd MAND
xc =p =a mandset
if errorlevel 1 goto error
cd ..

cd MODULA
xc =m =a bf.mod
if errorlevel 1 goto error
xc =m =a e.mod
if errorlevel 1 goto error
xc =m =a except.mod
if errorlevel 1 goto error
xc =m =a exp.mod
if errorlevel 1 goto error
xc =m =a fact.mod
if errorlevel 1 goto error
xc =m =a halt.mod
if errorlevel 1 goto error
xc =m =a hello.mod
if errorlevel 1 goto error
xc =m =a hisdemo.mod +map +genhistory +lineno
if errorlevel 1 goto error
xc =m =a queens.mod
if errorlevel 1 goto error
xc =m =a sieve.mod
if errorlevel 1 goto error
xc =m =a term.mod
if errorlevel 1 goto error
cd ..

cd NODES
xc =p =a runme
if errorlevel 1 goto error
cd ..

rem Option +changesym is used to avoid conflict between XDS-x86 and XDS-C
cd OBERON
xc =m =a ackermann.ob2 +changesym
if errorlevel 1 goto error
xc =m =a exp.ob2 +changesym
if errorlevel 1 goto error
xc =m =a gcreport.ob2 +changesym
if errorlevel 1 goto error
xc =m =a hello.ob2 +changesym
if errorlevel 1 goto error
xc =m =a Random.ob2 +changesym
if errorlevel 1 goto error
xc =m =a self.ob2 +changesym
if errorlevel 1 goto error
xc =m =a sieve.ob2 +changesym
if errorlevel 1 goto error
cd ..

cd PENTA
xrc /stb penta.rc
if errorlevel 1 goto error
xc =p =a penta
if errorlevel 1 goto error
cd ..

if not exist mgdemo\mgdemo.prj goto quit

cd MGDEMO
xc =p =a mgdemo.prj
if errorlevel 1 goto error
cd ..

cd WINDEMOM
xc =p =a windemom.prj
if errorlevel 1 goto error
cd ..

cd VTERM\Golygon
xc =p =a golygon.prj
if errorlevel 1 goto error
cd ..\..
cd VTERM\GraphDemo1	
xc =p =a GraphDemo1.prj
if errorlevel 1 goto error
cd ..\..
cd VTERM\GraphDemo2	
xc =p =a GraphDemo2.prj
if errorlevel 1 goto error
cd ..\..

goto quit

:error
echo *** FAULT ***
cd ..

:quit

