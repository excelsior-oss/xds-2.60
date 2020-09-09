IMPLEMENTATION MODULE result;
FROM pat IMPORT SOLUTION;
FROM Storage IMPORT ALLOCATE;

CONST MAX_RES = 100;
      RES_CLUSTER = 1000;

TYPE CLUSTER  = ARRAY [0..RES_CLUSTER-1] OF SOLUTION;
     PCLUSTER = POINTER TO CLUSTER;

VAR results : ARRAY [0..MAX_RES-1] OF PCLUSTER;

PROCEDURE add_solution (VAR sol : SOLUTION) : BOOLEAN;
BEGIN
    IF nresults = MAX_RES * RES_CLUSTER THEN
        RETURN FALSE;
    END;
    IF results [nresults DIV RES_CLUSTER] = NIL THEN
        NEW (results [nresults DIV RES_CLUSTER]);
        IF results [nresults DIV RES_CLUSTER] = NIL THEN
            RETURN FALSE;
        END;
    END;
    results [nresults DIV RES_CLUSTER]^[nresults MOD RES_CLUSTER] := sol;
    INC (nresults);
    RETURN TRUE;
END add_solution;

PROCEDURE get_solution (i : INTEGER; VAR sol : SOLUTION);
BEGIN
    sol := results [i DIV RES_CLUSTER]^[i MOD RES_CLUSTER];
END get_solution;

VAR i : INTEGER;

BEGIN
    FOR i := 0 TO MAX_RES-1 DO
        results [i] := NIL;
    END;
END result.
