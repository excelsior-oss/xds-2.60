@echo off
xc =p m2\RegEdt
del RegEdt.obj >nul
del RegEdt.sym >nul
del init.obj >nul
del jni.sym >nul
del tmp.lnk >nul
set classpath=.;%classpath%
javac RegistryKey.java
javac RegKey.java