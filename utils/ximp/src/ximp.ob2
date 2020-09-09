<* MAIN + *>
MODULE ximp;
IMPORT
  SYSTEM,
  adt,
  file:= H2DFile,
  lstr:= LongStrs,
  io:= Printf,
  env:= ProgEnv,
  opts:= Options,
  cmdline,
  parse;

CONST banner = "XDS Import, version 0.01 \n(c) 2000 Excelsior LLC.\n\n";
      footer = "File %s created.\n";

TYPE
  INT = SYSTEM.INT32;

  Module = POINTER TO ModuleDesc;
  ModuleDesc = RECORD (adt.NamedElementDesc)
    in, out: adt.List; (* Module *)
    mark, mark2: BOOLEAN;
  END;

  Component = POINTER TO ComponentDesc;
  ComponentDesc = RECORD (adt.ElementDesc)
    l: adt.List; (* Component *)
  END;

VAR
  Modules   : adt.Tree;  (* At the begin: all modules of the project;
                            at the end  : modules in loops only *)
  Nodes     : adt.List;  (* Non-looped modules only *)
  Components: adt.List;  (* Loop components *)

  files: adt.List;       (* Project files *)
  HeadModules: adt.List; (* Head modules for DLL *)


PROCEDURE Error(fmt-: ARRAY OF CHAR; SEQ x: SYSTEM.BYTE);
BEGIN
  io.printf(fmt, x);
  HALT;
END Error;

PROCEDURE fprintf(f: file.FILE; fmt-: ARRAY OF CHAR; SEQ x: SYSTEM.BYTE);
VAR s: ARRAY 512 OF CHAR;
BEGIN
  io.sprintf(s, fmt, x);
  file.WrStr(f, s);
END fprintf;

PROCEDURE ReadImport(filename-: ARRAY OF CHAR; modules: adt.Tree);
VAR f: file.FILE;
    s: lstr.String;
    m, tmp_m: Module;
    e: adt.Element;
    ne: adt.NamedElement;

  PROCEDURE get_mod(wholename-: ARRAY OF CHAR): Module;
  VAR e: adt.Element;
      ne: adt.NamedElement;
      tmp_m: Module;
      path, name, ext: lstr.String;
  BEGIN
    file.SplitName(wholename, path, name, ext);
    IF name^ = '' THEN Error("Corrupted file %s\n", filename) END;
    adt.NewNamedElement(ne, name^);
    modules.Find(ne, e);
    IF e # NIL THEN
      tmp_m:= e(Module);
    ELSE
      NEW(tmp_m); tmp_m.SetName(name^);
      adt.NewList(tmp_m.in);
      adt.NewList(tmp_m.out);
      tmp_m.mark:= FALSE;
      tmp_m.mark2:= FALSE;
      modules.Insert(tmp_m);
    END;
    adt.Deallocate(ne);
    lstr.Deallocate(path);
    lstr.Deallocate(name);
    lstr.Deallocate(ext);
    RETURN tmp_m;
  END get_mod;
BEGIN
  IF ~file.Open(filename, file.rdmode, f) THEN
    Error("Can't open file %s\n", filename);
  END;

  file.RdStr(f, s); m:= NIL;
  WHILE s # NIL DO
    file.CutSpace(s^);
    IF s^ # '' THEN
      IF s[0] = '-' THEN
	IF m # NIL THEN
	  lstr.Delete(s^, 0, 9);
	  tmp_m:= get_mod(s^);
	  IF tmp_m # m THEN
	    m.in.Find(tmp_m, e);
	    IF e = NIL THEN
	      m.in.Insert(tmp_m);
	      tmp_m.out.Insert(m);
	    END;
	  END;
	ELSE
	  Error("Corrupted file %s\n", filename);
	END;
      ELSE
        adt.NewNamedElement(ne, s^);
        files.Insert(ne);
	m:= get_mod(s^);
      END;
    END;
    file.RdStr(f, s);
  END;
END ReadImport;

PROCEDURE CutImport(modules: adt.Tree; nodes: adt.List);
VAR e, e1, e2: adt.Element;
BEGIN
  WHILE ~modules.IsEmpty() & ~nodes.IsEmpty() DO
    nodes.FindFirst(e); nodes.DeleteCurrent();
    WITH e: Module DO
      IF e.in.IsEmpty() THEN
	e.out.FindFirst(e1);
	WHILE e1 # NIL DO
	  WITH e1: Module DO
	    e1.in.Find(e, e2); e1.in.DeleteCurrent();
	    IF e1.in.IsEmpty() THEN
	      nodes.Insert(e1);
	      modules.Delete(e1);
	    END;
	  END;
	  e.out.FindNext(e1);
	END;
      ELSIF e.out.IsEmpty() THEN
	e.in.FindFirst(e1);
	WHILE e1 # NIL DO
	  WITH e1: Module DO
	    e1.out.Find(e, e2); e1.out.DeleteCurrent();
	    IF e1.out.IsEmpty() THEN
	      nodes.Insert(e1);
	      modules.Delete(e1);
	    END;
	  END;
	  e.in.FindNext(e1);
	END;
      ELSE
	ASSERT(FALSE);
      END;
    END;
  END;
END CutImport;

PROCEDURE GetComponents(components: adt.List; modules: adt.Tree);
VAR e, e1, e2: adt.Element;
    c: Component;

  PROCEDURE get(m: Module);
  VAR e, e1: adt.Element;
  BEGIN
    modules.Delete(m); c.l.Insert(m);
    ASSERT(~m.in.IsEmpty() & ~m.out.IsEmpty());
    m.in.FindFirst(e);
    WHILE e # NIL DO
      modules.Find(e, e1);
      IF e1 # NIL THEN
	get(e1(Module));
      END;
      m.in.FindNext(e);
    END;
    m.out.FindFirst(e);
    WHILE e # NIL DO
      modules.Find(e, e1);
      IF e1 # NIL THEN
	get(e1(Module));
      END;
      m.out.FindNext(e);
    END;
  END get;

BEGIN
  modules.FindFirst(e);
  WHILE e # NIL DO
    NEW(c); adt.NewList(c.l);
    get(e(Module));
    components.Insert(c);
    modules.FindFirst(e);
  END;
END GetComponents;

PROCEDURE PrintComponents(f: file.FILE; components: adt.List);
VAR e, e1, e2: adt.Element;
    comp_num: INTEGER;
    m: Module;
    path: adt.List;
    offset: INTEGER;

    PROCEDURE print(name-: ARRAY OF CHAR);
    VAR j: INTEGER;
    BEGIN
      FOR j:= 1 TO offset DO fprintf(f, ' ') END;
      fprintf(f, "%s\n", name);
    END print;

    PROCEDURE find_path(lm: Module): BOOLEAN;
    VAR e: adt.Element;
    BEGIN
      lm.mark:= TRUE;
      INC(offset, 2); --print(lm.name^);
      lm.out.FindFirst(e);
      WHILE e # NIL DO
	IF e(Module) = m THEN
	  RETURN TRUE;
	ELSIF ~e(Module).mark THEN
	  path.Insert(e(Module));
	  IF find_path(e(Module)) THEN
	    RETURN TRUE;
	  ELSE
	    DEC(offset, 2);
	    path.FindLast(e); path.DeleteCurrent();
	  END;
	END;
	lm.out.FindNext(e);
      END;
      RETURN FALSE;
    END find_path;

    PROCEDURE reset(l: adt.List);
    VAR e: adt.Element;
	id: INT;
    BEGIN
      l.Backup(id);
      l.FindFirst(e);
      WHILE e # NIL DO
	e(Module).mark:= FALSE;
	l.FindNext(e);
      END;
      l.Restore(id);
    END reset;

BEGIN
  components.FindFirst(e); comp_num:= 0; adt.NewList(path);
  WHILE e # NIL DO
--    fprintf(out_file, "Component %d:\n", comp_num);
    e(Component).l.FindFirst(e1);
    WHILE e1 # NIL DO
      m:= e1(Module);
      path.Clean();
      offset:= 0;
      IF ~m.mark2 & find_path(m) THEN
	fprintf(f, "   %s -> ", m.name^);
	m.mark2:= TRUE;
	path.FindFirst(e2);
	WHILE e2 # NIL DO
	  fprintf(f, "%s -> ", e2(Module).name^);
	  e2(Module).mark2:= TRUE;
	  path.FindNext(e2);
	END;
	fprintf(f, "%s\n", m.name^);
      END;
      reset(e(Component).l);
      e(Component).l.FindNext(e1);
    END;
    INC(comp_num);
    components.FindNext(e);
  END;
END PrintComponents;

PROCEDURE GetHeadModules(l: adt.List);
VAR i, j: INT;
    s: lstr.String;
    mod_name: ARRAY 100 OF CHAR;
    ne: adt.NamedElement;
BEGIN
  s:= NIL; cmdline.StrEquation(opts.genimp, s);
  IF s[0] = 0X THEN RETURN END;
  i:= 0;
  LOOP
    j:= 0;
    WHILE (s[i] >= 'A') & (s[i] <= 'Z') OR (s[i] >= 'a') & (s[i] <= 'z') OR
          (s[i] >= '0') & (s[i] <= '9') OR (s[i] = '_')
    DO
      mod_name[j]:= s[i];
      INC(i); INC(j);
    END;
    IF j = 0 THEN Error("Bad directive value: %s\n", opts.genimp) END;
    mod_name[j]:= 0X;
    adt.NewNamedElement(ne, mod_name);
    l.Insert(ne);
    IF s[i] = 0X THEN
      EXIT;
    ELSIF s[i] = ',' THEN
      INC(i);
    ELSE
      Error("Bad directive value: %s", opts.genimp);
    END;
  END;
END GetHeadModules;

PROCEDURE MarkImport(m: Module);
VAR e: adt.Element;
BEGIN
  IF m.mark THEN RETURN END;
  m.mark:= TRUE;
  m.in.FindFirst(e);
  WHILE e # NIL DO
    MarkImport(e(Module));
    m.in.FindNext(e);
  END;
END MarkImport;


VAR e, e1: adt.Element;
    wholename: lstr.String;
    name: lstr.String;
    out_file: file.FILE;
    import: adt.List;



BEGIN
  io.printf(banner);

  adt.NewList(HeadModules);
  GetHeadModules(HeadModules);
  IF cmdline.filenames.IsEmpty() OR
     HeadModules.IsEmpty() &
     ~cmdline.BoolOption(opts.genunused) &
     ~cmdline.BoolOption(opts.genloops)
  THEN
    io.printf(
      "Usage: ximp [options] [files]\n" +
      "Options:\n" +
      "            -%-10s find unused imported modules\n" +
      "            -%-10s find import loops\n", 
--      "            -%-10s find all imported modules\n",
        opts.genunused,
        opts.genloops,
        opts.genimp
    );
    HALT;
  END;


  adt.NewTree(Modules);
  adt.NewList(Nodes);
  adt.NewList(Components);
  wholename:= NIL; name:= NIL; adt.NewList(files); adt.NewList(import);
  cmdline.GetFileName(wholename);
  WHILE wholename # NIL DO
    adt.Deallocate(Modules); adt.NewTree(Modules);
    Nodes.Clean(); files.Clean(); import.Clean();

    ReadImport(wholename^, Modules);

    file.GetName(wholename^, name);
    file.CreateName('', name^, 'imp', wholename);
    IF ~file.Open(wholename^, file.crmode, out_file) THEN
      Error("Can't open file %s\n", wholename^);
    END;
    fprintf(out_file, banner);


    IF ~HeadModules.IsEmpty() THEN
      HeadModules.FindFirst(e);
      WHILE e # NIL DO
        Modules.Find(e, e1);
        IF e1 # NIL THEN
          MarkImport(e1(Module));
        ELSE
          Error("Module %s is not found in input file\n", e(adt.NamedElement).name^);
        END;
        HeadModules.FindNext(e);
      END;

      fprintf(out_file, "Import:\n");
      Modules.FindFirst(e);
      WHILE e # NIL DO
        IF e(Module).mark THEN
          fprintf(out_file, "   %s\n", e(Module).name^);
          e(Module).mark:= FALSE;
        END;
        Modules.FindNext(e);
      END;
      fprintf(out_file, "\n");
    END;

    IF cmdline.BoolOption(opts.genloops) THEN
      Modules.FindFirst(e);
      WHILE e # NIL DO
        IF e(Module).in.IsEmpty() OR e(Module).out.IsEmpty() THEN
          Nodes.Insert(e);
          Modules.Delete(e);
        END;
        Modules.FindNext(e);
      END;

      CutImport(Modules, Nodes);

      IF Modules.IsEmpty() THEN
        fprintf(out_file, "No loops\n\n");
      ELSE
        fprintf(out_file, "Loops:\n");
        Components.Clean();
        GetComponents(Components, Modules);
        PrintComponents(out_file, Components);
        fprintf(out_file, "\n");
      END;
    END;

    IF cmdline.BoolOption(opts.genunused) THEN
      fprintf(out_file, "Unused import:\n");
      files.FindFirst(e);
      WHILE e # NIL DO
        parse.CheckModule(e(adt.NamedElement).name^, import);
        IF ~import.IsEmpty() THEN
          fprintf(out_file, "   File %s:\n", e(adt.NamedElement).name^);
          import.FindFirst(e1);
          WHILE e1 # NIL DO
            IF e1(parse.Import).real_name^ # e1(parse.Import).name^ THEN
              fprintf(out_file, "      %s:= %s\n", e1(parse.Import).name^, e1(parse.Import).real_name^);
            ELSE
              fprintf(out_file, "      %s\n", e1(parse.Import).real_name^);
            END;
            import.FindNext(e1);
          END;
        END;
        files.FindNext(e);
      END;
    END;

    file.Close(out_file);
    io.printf(footer, wholename^);

    cmdline.GetFileName(wholename);
  END;
END ximp.
