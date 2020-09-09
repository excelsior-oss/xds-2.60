# common part of makefiles for all XDS on-line docs (HTML format)

COMMON = ..\back.tex ..\def.tex ..\title.tex ..\xdsbook.tex
EUDIR   = ..\..\enduser\xds\doc
ENDUSER = $(EUDIR)\$(TARGET).html

!ifndef PICDIRS
PICDIRS = .
!endif

html: $(ENDUSER)

$(ENDUSER): $(TARGET).html
	xcopy $(TARGET)*.html "$(EUDIR)\"  /Y
	xcopy $(TARGET)*.js   "$(EUDIR)\"  /Y
	xcopy ..\gifs\*.gif   "$(EUDIR)\"  /Y
	xcopy ..\gifs\*.js    "$(EUDIR)\"  /Y
	-for %a in ($(PICDIRS)) do mkdir $(EUDIR)\%a
	for %a in ($(PICDIRS)) do xcopy /Y %a\*.gif $(EUDIR)\%a
	for %a in ($(PICDIRS)) do xcopy /Y %a\*.jpg $(EUDIR)\%a

$(TARGET).html: $(SOURCE) $(COMMON)
	..\convertn -hf -m $(TARGET).tex > $(TARGET).hlg
