@echo off
call ctr_env.bat

rmdir /Q /S %REP_DIR% 1>nul 2>nul
mkdir %REP_DIR%
