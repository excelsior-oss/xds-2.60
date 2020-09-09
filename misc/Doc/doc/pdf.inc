# common part of makefiles for all PDF manuals

LATEX	= pdflatex -c-style-errors -halt-on-error -interaction=batchmode  
MKIDX	= makeindex

COMMON = ..\back.tex ..\def.tex ..\title.tex ..\xdsbook.tex

!ifndef EUDIR
EUDIR   = ..\..\enduser\xds\pdf
!endif

ENDUSER = $(EUDIR)\$(TARGET).pdf 

pdf: $(ENDUSER)

$(EUDIR)\$(TARGET).pdf: $(TARGET).pdf 
	xcopy $(TARGET).pdf "$(EUDIR)\"  /Y

$(TARGET).pdf: $(TARGET).ind

$(TARGET).ind: $(TARGET).idx
        copy $(TARGET).idx $(TARGET).ids
	$(MKIDX) $(TARGET).idx
	$(LATEX) $(TARGET).tex
        copy $(TARGET).ids $(TARGET).idx
        del $(TARGET).ids

$(TARGET).idx: $(SOURCE) $(COMMON)
	$(LATEX) $(TARGET).tex
