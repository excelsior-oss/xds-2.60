/**/
'@echo off'
'rxqueue /clear'
'dir /s /f *.dvi *.?lg *.ps *.ipf *.inf *.rtf *.cnt *.hlp *.html *.aux *.log *.toc *.ind *.idx *.bak *.lot *.ext *.fts *cnt.js | rxqueue'
do while lines('queue:') <> 0
  pull file
  'del 'file
end
exit
