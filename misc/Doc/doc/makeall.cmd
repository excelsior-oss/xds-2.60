/**/

version = '245'

listfile = 'manuals.lst'
if stream(listfile,'c','open read') <> 'READY:' then do
  say 'Failed to open' listfile
  exit 1
end

i = 1
do while lines(listfile) <> 0 
  line = linein(listfile)
  if substr(line,1,1) <> '*' then do
    line = translate(line,' ',D2C(9))
    line = space(line,1)
    parse var line name.i dir.i os.i products.i formats.i
    i = i+1
  end
end
name.0 = i-1

if stream(listfile,'c','close') <> 'READY:' then do
  say 'Failed to close' listfile
  exit 1
end

/*
do i=1 to name.0
  say '|'name.i'|'dir.i'|'os.i'|'products.i'|'formats.i'|'
end
*/

/*'@echo off'*/

arg args

os      = FindArg('OS2 WIN LINUX UNIX')
format  = FindArg('INF HLP HTML PS PDF')
product = FindArg('C X86 ALL')

if product = 'ALL' then
  archive = 'xds-'||version||'-'
else
  archive = 'xds-'||product||'-'||version||'-'

select
  when os = 'OS2' then do
    osflag     = '\osiitrue'
    archive_os = 'os2'
  end
  when os = 'WIN' then do
    osflag     = '\winnttrue'
    archive_os = 'w95'
  end
  when os = 'LINUX' then do
    osflag     = '\linuxtrue'
    archive_os = 'linux'
  end
  when os = 'MACOSX' then do
    osflag     = '\macosxtrue'
    archive_os = 'macosx'
  end
  when os = 'UNIX' then do
    osflag     = '\unixtrue'
    archive_os = 'unix'
  end
  otherwise
    say 'Unknown OS:' os
    exit 1
end

select
  when format = 'INF' then do
  end
  when format = 'HLP' then do
  end
  when format = 'PDF' then do
  end
  when format = 'HTML' then do
    archive_format = 'html'
  end
  when format = 'PS' then do
    archive_format = 'ps'
  end
  otherwise
    say 'Unknown format:' format
    exit 1
end

platform = 'platform.tex'
backend  = 'backend.tex'

'if exist' platform 'del' platform
call lineout platform, osflag
call lineout platform

do i = 1 to name.0
  say 'Making' name.i||'...'
  if (os.i <> 'ALL') & (pos(os,os.i) = 0) then iterate
  if (formats.i <> 'ALL') & (pos(format,formats.i) = 0) then iterate
  if products.i = 'ALL' then do
    'if exist' backend 'del' backend
    call lineout backend, ''
    call lineout backend
    call Make
  end
  else do
    if (pos('X86',products.i) <> 0) & (pos(product,'ALL X86') <> 0) then do
      'if exist' backend 'del' backend
      call lineout backend, '\gencodetrue'
      call lineout backend, '\geninteltrue'
      call lineout backend
      call Make 'X86'
    end
    if (pos('C',products.i) <> 0) & (pos(product,'ALL C') <> 0) then do
      'if exist' backend 'del' backend
      call lineout backend, '\genctrue'
      call lineout backend
      call Make 'C'
    end
  end
end

if format = 'PS' then do
  call directory '..\enduser\xds\ps'
  archive = archive||archive_os||'-doc-'||archive_format||'-'
  if (product = 'C') | (product = 'ALL') then do
    'zip -r -j' '..\..\..\'||archive||'a4 *_a4.ps'
    'zip -r -j' '..\..\..\'||archive||'a5 *_a5.ps'
    'zip -r -j' '..\..\..\'||archive||'letter *_letter.ps'
  end
  if (product = 'X86') | (product = 'ALL') then do
    'zip -r -j' '..\..\..\'||archive||'a4 *_a4.ps'
    'zip -r -j' '..\..\..\'||archive||'a5 *_a5.ps'
    'zip -r -j' '..\..\..\'||archive||'letter *_letter.ps'
  end
end

exit 0

Make: procedure expose dir. name. format i
  arg product
  call directory dir.i
  'nmake -f' name.i'.mkf FORMAT='format 'PRODUCT='product
  if rc <> 0 then do
    say '*** Make failed ***'
    exit 1
  end
  call directory '..'
  return

FindArg:
  arg set
  res = ''
  do i = 1 to words(set)
    if wordpos(word(set,i), args) > 0 then
      if res = '' then
        res = word(set,i)
      else do
        say res 'and' word(set,i) 'may not be specified simultaneously'
        exit 1
      end
  end
  if res = '' then do
    say 'Please specify one of:' translate(set,',',' ')
    exit 1
  end
  return res
