(* Copyright (c) 1996,99 XDS Ltd. All Rights Reserved. *)
<* +M2EXTENSIONS *>
IMPLEMENTATION MODULE xosExec; (* Hady. 28.06.96 16:57 *)

IMPORT  SYSTEM, xlibOS, X2C, xPOSIX, xmRTS, xrtsOS;

TYPE
  X2C_pCHAR = xmRTS.X2C_pCHAR;
  int = SYSTEM.int;


PROCEDURE ["C"] put_in_quotes(to: xmRTS.X2C_pCHAR; a: xmRTS.X2C_pCHAR);
BEGIN
  to^:='"'; to:=SYSTEM.ADDADR(to,1);
  WHILE a^#0C DO
    to^:=a^; to:=SYSTEM.ADDADR(to,1);
    a:=SYSTEM.ADDADR(a,1);
  END;
  to^:='"'; to:=SYSTEM.ADDADR(to,1);
  to^:=0C;
END put_in_quotes;

PROCEDURE ["C"] X2C_Execute(cmd,args: xmRTS.X2C_pCHAR; overlay: SYSTEM.int; VAR exitcode: SYSTEM.CARD32): SYSTEM.INT32;
  VAR
    argv: ARRAY [0..255] OF xPOSIX.xPOSIX_PCHAR;
    i   : INTEGER;
    rc  : SYSTEM.INT32;
    bsz : CARDINAL;
BEGIN
  i:=1;
  LOOP
    WHILE args^<=' ' DO
      IF args^=0C THEN EXIT END;
      args:=SYSTEM.ADDADR(args,1);
    END;
    argv[i]:=args; INC(i);
    WHILE args^>' ' DO args:=SYSTEM.ADDADR(args,1) END;
    IF args^=0C THEN EXIT END;
    args^:=0C; args:=SYSTEM.ADDADR(args,1);
  END;
  argv[i]:=NIL;

  bsz     := xPOSIX.strlen(cmd) + 2 + 1;
  argv[0] := xrtsOS.X2C_malloc(bsz);
  put_in_quotes(argv[0], cmd);

  SYSTEM.CODE("#if defined(__WATCOMC__)");
  cmd := argv[0];
  SYSTEM.CODE("#endif");

  IF (overlay#0) THEN
    SYSTEM.CODE("#if defined(_msdos)");
    rc := xPOSIX.spawnv(xPOSIX.P_WAIT,cmd,argv);
    SYSTEM.CODE("#else");
    rc := xPOSIX.execv(cmd,argv);
    SYSTEM.CODE("#endif");
  ELSE
    rc := xPOSIX.spawnv(xPOSIX.P_WAIT,cmd,argv);
  END;

  xrtsOS.X2C_free(argv[0], bsz);
  
  IF rc < 0 THEN
    RETURN xPOSIX.errno
  ELSE
    exitcode := rc;
    RETURN 0;
  END
END X2C_Execute;

PROCEDURE ["C"] X2C_ExecuteNoWindow(cmd,args: xmRTS.X2C_pCHAR; overlay: SYSTEM.int; VAR exitcode: SYSTEM.CARD32): SYSTEM.INT32;
BEGIN
  RETURN X2C_Execute (cmd, args, overlay, exitcode);
END X2C_ExecuteNoWindow;


(*----------------------------------------------------------------------------*)
PROCEDURE ["C"] X2C_Command (cmd: xmRTS.X2C_pCHAR; VAR exitcode: SYSTEM.CARD32): SYSTEM.INT32;
VAR  rc: SYSTEM.INT32;
BEGIN
  rc := xPOSIX.system(cmd);
  IF rc < 0 THEN
    RETURN xPOSIX.errno
  ELSE
    exitcode := rc;
    RETURN 0;
  END
END X2C_Command;

END xosExec.
