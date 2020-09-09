@echo off
rem ------------------------------------------------------------------------------
rem                          Excelsior Rats System Utility
rem                            (c) 2000, Excelsior Ltd.
rem
rem Module:       countt
rem Modified By:  
rem Created:      19-Apr-2001
rem Purpose:      Check and increase 'total' counters by 1
rem Main:         report_all
rem Required:     Componet - report_all, report, set_param, countt
rem               Pakages  - cygwin-b20, u2d.exe
rem ------------------------------------------------------------------------------

SET /A totalreps=%totalreps%+1

IF %1 NEQ 0  SET /A ts_num_total=%ts_num_total%+1

