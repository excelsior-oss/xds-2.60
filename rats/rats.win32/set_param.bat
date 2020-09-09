@echo off
rem ------------------------------------------------------------------------------
rem                          Excelsior Rats System Utility
rem                            (c) 2000, Excelsior Ltd.
rem
rem Module:       set_param
rem Modified By:  
rem Created:      19-Apr-2001
rem Purpose:      increase global variable by 1
rem Main:         report_all
rem Required:     Componet - report_all, report, set_param, countt
rem               Pakages  - cygwin-b20, u2d.exe
rem ------------------------------------------------------------------------------

SET /A A=%A%+1
SET kol%A%=%1
