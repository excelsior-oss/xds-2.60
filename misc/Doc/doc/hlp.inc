# common part of makefiles for all XDS on-line docs (Windows Help 4 format)

COMMON = ..\back.tex ..\def.tex ..\title.tex ..\xdsbook.tex
EUDIR   = ..\..\enduser\xds\bin
ENDUSER = $(EUDIR)\$(TARGET).hlp $(EUDIR)\$(TARGET).cnt

!ifndef CNTLEVEL
CNTLEVEL = 2
!endif

hlp: $(ENDUSER)

$(EUDIR)\$(TARGET).hlp: $(TARGET).hlp
	xcopy $(TARGET).hlp "$(EUDIR)\" /Y

$(EUDIR)\$(TARGET).cnt: $(TARGET).cnt
	xcopy $(TARGET).cnt "$(EUDIR)\" /Y

$(TARGET).hlp: $(TARGET).rtf $(TARGET).hpj
	hcrtf -xn $(TARGET).hpj

$(TARGET).cnt $(TARGET).rtf: $(SOURCE) $(COMMON)
	..\convertn -rc$(CNTLEVEL) $(TARGET).tex > $(TARGET).rlg
