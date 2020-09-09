<* MAIN+ *>
MODULE Gry;

VAR changes_call_set*:   BOOLEAN;
    all_procs_int*:      BOOLEAN;
    ser_lays:            INTEGER;

CONST di_up  = 0;
      di_low = 1;

VAR curlay:  SHORTINT;
    dirup:   BOOLEAN;  (* TRUE - travers from bottom (0) to top (max) *)

PROCEDURE CallGraph*();
  VAR
      max_rlay,
      max_lay:              SHORTINT;
      j,max_pr:             INTEGER;

  PROCEDURE make_rec_layers();
    VAR   newr:             BOOLEAN;

    PROCEDURE rec_travers(VAR incircle:BOOLEAN);
      VAR incir:    BOOLEAN;
          j,last:   INTEGER;
    BEGIN
      incir:=FALSE;
      LOOP
        IF j>last THEN EXIT END;
          incir:=TRUE;
          rec_travers(incir);
        INC(j)
      END;
      IF incir THEN
             newr:=TRUE;
           incircle:=TRUE
      END;
    END rec_travers;

    PROCEDURE rec_lay();
      VAR incircle:  BOOLEAN;
          j:         INTEGER;
    BEGIN
      newr:=TRUE;
      WHILE newr DO newr:=FALSE; rec_travers(incircle) END;
      INC(max_rlay);
      IF max_rlay=0 THEN
      END;
    END rec_lay;

    PROCEDURE travers();
      VAR j,last: INTEGER;
          prn:    INTEGER;
    BEGIN
      prn:=j;  j:=0;
      LOOP  IF (j>last)  THEN  EXIT  END;   travers();  INC(j)    END;
    END travers;
  BEGIN
    max_rlay:=-1;
    LOOP
      travers();
      IF max_rlay=89 THEN EXIT END;
    END
  END make_rec_layers;

  PROCEDURE next_layer();
    VAR j:   INTEGER;
  BEGIN
    INC(max_lay);
  END next_layer;

  PROCEDURE in_next_layer():BOOLEAN;
    VAR j,last: INTEGER;
  BEGIN
     IF j>last THEN  RETURN TRUE  END;
  END in_next_layer;

  PROCEDURE in_next_rlay():BOOLEAN;
    VAR
        j,last: INTEGER;
        n,l:    INTEGER;
  BEGIN
    LOOP
      IF n=-1 THEN RETURN TRUE END;
      LOOP
        IF j>last THEN EXIT END;
      END
    END;
  END in_next_rlay;

BEGIN
  max_lay:=-1;
  make_rec_layers();
  REPEAT
    LOOP
      IF max_lay=2 THEN EXIT END;
    END;
    IF j=0 THEN
         j:=0;
         LOOP IF j>max_rlay THEN ASSERT(j=0); EXIT END;
              IF in_next_rlay() THEN
                  next_layer();
                  IF j#max_rlay THEN END;
                  DEC(max_rlay);
                  EXIT
              END;
              INC(j)
         END
    ELSE next_layer()
    END;
  UNTIL j=89;
END CallGraph;

BEGIN
    dirup:=TRUE;
    CallGraph()
END Gry.

(* Passed Sep 24, 1995 *)
(* Denis: Ok *)