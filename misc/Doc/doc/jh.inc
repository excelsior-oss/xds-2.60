# common part of makefiles for all XDS on-line docs (HTML format)

COMMON = ..\back.tex ..\def.tex ..\title.tex ..\xdsbook.tex

!ifndef EUDIR
EUDIR   = ..\classes\help
!endif

!ifndef PICDIRS
PICDIRS = .
!endif

ENDUSER = $(EUDIR)\$(TARGET).hs

jh: $(ENDUSER)

$(ENDUSER): $(TARGET).hs
	xcopy $(TARGET)*.html "$(EUDIR)\"  /Y
	xcopy $(TARGET).hs    "$(EUDIR)\"  /Y
	xcopy $(TARGET).jhm   "$(EUDIR)\"  /Y
#	xcopy $(TARGET).jdx   "$(EUDIR)\"  /Y
	xcopy $(TARGET).xml   "$(EUDIR)\"  /Y
	-for %a in ($(PICDIRS)) do mkdir $(EUDIR)\%a
	for %a in ($(PICDIRS)) do xcopy /Y %a\*.gif $(EUDIR)\%a
	for %a in ($(PICDIRS)) do xcopy /Y %a\*.jpg $(EUDIR)\%a

$(TARGET).hs: $(SOURCE) $(COMMON)
	..\convertn -j -hp -m2 -S javahelp.css $(TARGET).tex > $(TARGET).jlg
