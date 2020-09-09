@echo off
echo cleaning up directories...
if .%runworkplace% == . goto nt
call os2clean
goto quit

:nt
del /s *.dvi *.?lg *.ps *.ipf *.inf *.rtf *.hlp *.cnt *.html *.aux *.log *.toc *.ind *.idx *.bak *.lot *.ext *.fts *cnt.js *.hs *.xml *.jhm *.out *.chm *.hhc *.hha 2>nul
del /s /a=hs *.gid 2>nul

:quit
exit