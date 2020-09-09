MODULE parse;
IMPORT SYSTEM, adt, scan, io:= Printf, lstr:= LongStrs;

TYPE INT = scan.INT;
  Import * = POINTER TO ImportDesc;
  ImportDesc * = RECORD (adt.NamedElementDesc)
    used: BOOLEAN;
    real_name * : lstr.String;
    from: adt.List;
  END;

  Ident = POINTER TO IdentDesc;
  IdentDesc = RECORD (adt.NamedElementDesc)
    mod: Import;
  END;



PROCEDURE Error (fmt-: ARRAY OF CHAR; SEQ x: SYSTEM.BYTE);
BEGIN
  io.printf(fmt, x);
  HALT;
END Error;


PROCEDURE CheckModule * (name: ARRAY OF CHAR; l: adt.List);
VAR import: adt.List;
    imp_name, text, mod_name: lstr.String;
    token: INT;
    imp: Import;
    ne: adt.NamedElement;
    e : adt.Element;
    ident: Ident;
    idents: adt.Tree;

BEGIN
  text:= NIL; mod_name:= NIL; adt.NewList(import); imp_name:= NIL;
  adt.NewNamedElement(ne, ''); l.Clean(); adt.NewTree(idents);
  scan.Open(name);

  REPEAT
    token:= scan.GetToken(text);
  UNTIL (token = scan.eof) OR (token = scan.ident) & (text^ = 'MODULE');

  IF token = scan.eof THEN
    Error("Unexpected end of file %s\n", name);
  END;

  IF scan.GetToken(mod_name) # scan.ident THEN
    Error("Can't find module name in file %s\n", name);
  END;


  token:= scan.GetToken(text);
  LOOP
    WHILE (token # scan.eof) & ((token # scan.ident) OR
                                (text^ # 'FROM')      &
                                (text^ # 'IMPORT')    &
                                (text^ # 'TYPE')      &
                                (text^ # 'CONST')     &
                                (text^ # 'VAR')       &
                                (text^ # 'PROCEDURE') &
                                (text^ # 'END')
                               )
    DO token:= scan.GetToken(text) END;

    IF token = scan.eof THEN
      Error("Unexpected end of file %s\n", name);
    ELSIF text^ = 'IMPORT' THEN
      NEW(imp); imp.used:= FALSE; adt.NewList(imp.from);
      IF scan.GetToken(imp.name) # scan.ident THEN
        Error("Expected identifier in import list of file %s\n", name);
      ELSE
        import.Find(imp, e);
        IF e = NIL THEN import.Insert(imp) END;
      END;
      token:= scan.GetToken(text);
      IF token = scan.becomes THEN
        IF scan.GetToken(imp.real_name) # scan.ident THEN
          Error("Expected identifier in import list of file %s\n", name);
        END;
        token:= scan.GetToken(text);
      ELSE
        lstr.Assign(imp.name^, imp.real_name);
      END;
      WHILE token = scan.comma DO
        NEW(imp); imp.used:= FALSE;
        IF scan.GetToken(imp.name) # scan.ident THEN
          Error("Expected identifier in import list of file %s\n", name);
        ELSE
          import.Insert(imp);
        END;
        token:= scan.GetToken(text);
        IF token = scan.becomes THEN
          IF scan.GetToken(imp.real_name) # scan.ident THEN
            Error("Expected identifier in import list of file %s\n", name);
          END;
          token:= scan.GetToken(text);
        ELSE
          lstr.Assign(imp.name^, imp.real_name);
        END;
      END;

      IF token # scan.semicolon THEN
        Error("Expected semicolon at the end of import list of file %s\n", name);
      END;

      token:= scan.GetToken(text);
    ELSIF text^ = 'FROM' THEN
      NEW(imp); imp.used:= FALSE; adt.NewList(imp.from);
      IF scan.GetToken(imp.name) # scan.ident THEN
        Error("Expected identifier in import list of file %s\n", name);
      ELSE
        import.Find(imp, e);
        IF e = NIL THEN import.Insert(imp) END;
      END;
      lstr.Assign(imp.name^, imp.real_name);
      IF (scan.GetToken(text) # scan.ident) OR (text^ # 'IMPORT') THEN
        Error("Expected identifier in import list of file %s\n", name);
      END;
      NEW(ident);
      token:= scan.GetToken(ident.name);
      IF token # scan.ident THEN
        Error("Expected identifier in import list of file %s\n", name);
      END;
      ident.mod:= imp;
      idents.Insert(ident);
      token:= scan.GetToken(text);
      WHILE token = scan.comma DO
        NEW(ident);
        token:= scan.GetToken(ident.name);
        IF token # scan.ident THEN
          Error("Expected identifier in import list of file %s\n", name);
        END;
        ident.mod:= imp;
        idents.Insert(ident);
        token:= scan.GetToken(text);
      END;

      IF token # scan.semicolon THEN
        Error("Expected semicolon at the end of import list of file %s\n", name);
      END;

      token:= scan.GetToken(text);
    ELSE
      EXIT;
    END;
  END;


  LOOP
--    io.printf("token %d, text %s\n", token, text^);
    IF token = scan.ident THEN
      ne.SetName(text^);
      idents.Find(ne, e);
      IF e # NIL THEN
        e(Ident).mod.used:= TRUE;
        token:= scan.GetToken(text);
      ELSIF text^ = 'END' THEN
        token:= scan.GetToken(text);
        IF (token = scan.ident) & (text^ = mod_name^) THEN
          token:= scan.GetToken(text);
          IF token = scan.period THEN EXIT END;
        END;
      ELSE
        lstr.Assign(text^, imp_name);
        token:= scan.GetToken(text);
        IF token = scan.period THEN
          ne.SetName(imp_name^);
          import.Find(ne, e);
          IF e # NIL THEN
            e(Import).used:= TRUE;
          END;
          token:= scan.GetToken(text);
        END;
      END;
    ELSE
      token:= scan.GetToken(text);
    END;
  END;

  import.FindFirst(e);
  WHILE e # NIL DO
    IF ~e(Import).used THEN l.Insert(e) END;
    import.FindNext(e);
  END;
END CheckModule;

END parse.
