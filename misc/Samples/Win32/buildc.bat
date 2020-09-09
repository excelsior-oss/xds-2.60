cd GENERIC
xrc /stb generic.rc
if errorlevel 1 goto error
xm =p =a generic
if errorlevel 1 goto error
cd ..

cd H2D
h2d =p example.h2d
if errorlevel 1 goto error
cd ..

cd HUFFCHAN
xm =m =a huf.mod
if errorlevel 1 goto error
xm =m =a unhuf.mod
if errorlevel 1 goto error
cd ..

cd MAND
xm =p =a mandset
if errorlevel 1 goto error
cd ..

cd MIXED
xm =p =a cmain
if errorlevel 1 goto error
xm =p =a m2main
if errorlevel 1 goto error
cd ..

cd MODULA
xm =m =a bf.mod
if errorlevel 1 goto error
xm =m =a e.mod
if errorlevel 1 goto error
xm =m =a except.mod
if errorlevel 1 goto error
xm =m =a exp.mod
if errorlevel 1 goto error
xm =m =a fact.mod
if errorlevel 1 goto error
xm =m =a halt.mod
if errorlevel 1 goto error
xm =m =a hello.mod
if errorlevel 1 goto error
xm =m =a hisdemo.mod +gendebug +lineno
if errorlevel 1 goto error
xm =m =a queens.mod
if errorlevel 1 goto error
xm =m =a sieve.mod
if errorlevel 1 goto error
xm =m =a term.mod
if errorlevel 1 goto error
cd ..

cd NODES
xm =p =a runme
if errorlevel 1 goto error
cd ..

rem Option +changesym is used to avoid conflict between XDS-x86 and XDS-C
cd OBERON
xm =m =a ackermann.ob2 +changesym
if errorlevel 1 goto error
xm =m =a exp.ob2 +changesym
if errorlevel 1 goto error
xm =m =a gcreport.ob2 +changesym
if errorlevel 1 goto error
xm =m =a hello.ob2 +changesym
if errorlevel 1 goto error
xm =m =a Random.ob2 +changesym
if errorlevel 1 goto error
xm =m =a self.ob2 +changesym
if errorlevel 1 goto error
xm =m =a sieve.ob2 +changesym
if errorlevel 1 goto error
cd ..

cd PENTA
xrc /stb penta.rc
if errorlevel 1 goto error
xm =p =a penta
if errorlevel 1 goto error
cd ..

goto quit

:error
echo *** FAULT ***
cd ..

:quit

