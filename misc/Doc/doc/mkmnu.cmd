/**/
'rxqueue /clear'
'dir /b /a:d | rxqueue'
do while queued() > 0
  pull dir
  say dir
  mnu = dir'\fc.mnu'
  if stream(mnu,'c','query exists') = '' then iterate
  'del' mnu
  call OpenWrite mnu
  call lineout mnu, 'F1: Make IPF'
  call lineout mnu, '    nmake -f' dir'.mkf' dir'.ipf FORMAT=inf'
  call lineout mnu, 'F2: Make INF'
  call lineout mnu, '    nmake -f' dir'.mkf' dir'.inf FORMAT=inf'
  call lineout mnu, 'F3: View INF'
  call lineout mnu, '    view' dir'.inf'
  call lineout mnu, 'F5: Make RTF'
  call lineout mnu, '    nmake -f' dir'.mkf' dir'.rtf FORMAT=win'
  call lineout mnu, 'F6: Make HTML'
  call lineout mnu, '    nmake -f' dir'.mkf' dir'.html FORMAT=html'
  call lineout mnu, 'F7: View HTML'
  call lineout mnu, '    netscape !s'dir'.html'
  call lineout mnu, 'F9: Make DVI'
  call lineout mnu, '    latex2e' dir'.tex'
  call lineout mnu, 'F10: Make IND'
  call lineout mnu, '    makeindx' dir'.idx'
  call lineout mnu, 'F11: View DVI'
  call lineout mnu, '    dvipm @lj /fl=0 /o3 /w210mm /h297mm' dir'.dvi'
  call Close mnu
end
exit

OpenWrite: procedure
  arg file
  rc = stream(file,'c','open write')
  if rc <> 'READY:' then do
    say 'Error opening 'file': 'rc
    exit 1
  end
  return

Close: procedure
  arg file
  rc = stream(file,'c','close')
  if rc <> 'READY:' then do
    say 'Error closing 'file': 'rc
    exit 1
  end
  return
