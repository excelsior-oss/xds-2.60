# common part of makefiles for all Postscript XDS manuals

!ifdef WINDIR
# Win32
LATEX	= latex
DVIPS	= dvips
MKIDX	= makeindex
!else
# OS/2
LATEX	= latex2e
DVIPS	= dvips32
MKIDX	= makeindx
!endif

COMMON = ..\def.tex ..\title.tex ..\xdsbook.tex

!ifndef EUDIR
EUDIR   = ..\..\enduser\xds\ps
!endif

ENDUSER = $(EUDIR)\$(TARGET)_a4.ps $(EUDIR)\$(TARGET)_a5.ps $(EUDIR)\$(TARGET)_letter.ps

ps: $(ENDUSER)

$(EUDIR)\$(TARGET)_a4.ps: $(TARGET)_a4.ps 
	xcopy $(TARGET)_a4.ps "$(EUDIR)\" /Y

$(EUDIR)\$(TARGET)_a5.ps: $(TARGET)_a5.ps 
	xcopy $(TARGET)_a5.ps "$(EUDIR)\" /Y

$(EUDIR)\$(TARGET)_letter.ps: $(TARGET)_letter.ps 
	xcopy $(TARGET)_letter.ps "$(EUDIR)\" /Y

$(TARGET)_a4.ps: $(TARGET).ind
	$(DVIPS) $(TARGET).dvi -o $(TARGET)_a4.ps

$(TARGET)_a5.ps: $(TARGET).ind
        $(DVIPS) -t a5 -y 707 -O -0.3in,-0.3in -o $(TARGET)_a5.ps $(TARGET).dvi 

$(TARGET)_letter.ps: $(TARGET).ind
        $(DVIPS) -t letter -O 0.1in,-0.2in -o $(TARGET)_letter.ps $(TARGET).dvi 

$(TARGET).ind: $(TARGET).idx
        copy $(TARGET).idx $(TARGET).ids
	$(MKIDX) $(TARGET).idx
	$(LATEX) $(TARGET).tex
        copy $(TARGET).ids $(TARGET).idx
        del $(TARGET).ids

$(TARGET).idx: $(SOURCE) $(COMMON)
	$(LATEX) $(TARGET).tex
