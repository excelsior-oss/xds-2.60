# common part of makefiles for all Excelsior on-line docs (HtmlHelp format)

COMMON = ..\back.tex ..\def.tex ..\title.tex ..\xdsbook.tex

!ifndef EUDIR
EUDIR   = ..\..\enduser\xds\doc
!endif

!ifndef PICDIRS
PICDIRS = .
!endif

ENDUSER = $(EUDIR)\$(TARGET).chm

hh: $(ENDUSER)

$(ENDUSER): $(TARGET).chm
	xcopy $(TARGET).chm "$(EUDIR)\" /Y

$(TARGET).chm: $(TARGET).hhc
        -1 hhc $(TARGET).hhp

$(TARGET).hhc: $(SOURCE) $(COMMON)
	convertn -H -hp -m2 -s $(TARGET).tex > $(TARGET).clg
