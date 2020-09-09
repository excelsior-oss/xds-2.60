<*+ M2EXTENSIONS *>
IMPLEMENTATION MODULE RATSLib;

IMPORT SYSTEM, Printf, FormOut, platform;

PROCEDURE ok;
BEGIN
  Printf.printf("--- OK ---\n");
END ok;

PROCEDURE Success;
BEGIN
  ok;
  HALT(0);
END Success;

PROCEDURE fail;
BEGIN
  Printf.printf("*** FAIL ***\n");
END fail;

PROCEDURE Failure;
BEGIN
  fail;
  HALT(1);
END Failure;

<* IF __GEN_C__ THEN *>
PROCEDURE [3] SUCCESS*; (* for sl1 *)
BEGIN
  Success;
END SUCCESS;

PROCEDURE [3] FAILURE*; (* for sl1 *)
BEGIN
  Failure;
END FAILURE;
<* END *>

PROCEDURE Assert(v: BOOLEAN);
BEGIN
  IF NOT v THEN Failure END;
END Assert;

PROCEDURE OkIf(v: BOOLEAN);
BEGIN
  IF v THEN Success
  ELSE Failure
  END;
END OkIf;

PROCEDURE print(fmt-: ARRAY OF CHAR; SEQ x: SYSTEM.BYTE);
BEGIN
  Printf.printf(fmt, x);
END print;

BEGIN
  FormOut.LineSeparator(platform.lineSep);
  FormOut.TextSeparator(platform.textSep);
END RATSLib.
