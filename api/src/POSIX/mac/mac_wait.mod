(* Copyright (c) 1997 xTech Ltd, Russia. All rights reserved. *)

(* $RCSfile: mac_wait.mod,v $ $Revision: 1.1.1.1 $ $Date: 1997/10/20 19:32:35 $ *)

<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_wait;

(*
    Title: 	Implementation of C-macros from POSIX 1003.1 wait.h header.
    Created:	Sat Aug 30 19:25:30 1997
    Author: 	Igor L. Ziryanov
		<ego@pluto.xTech.RU>
*)

FROM SYSTEM IMPORT SHIFT, CAST;

-------------------------------- WEXITSTATUS ----------------------------------
-- If WIFEXITED(STATUS), the low-order 8 bits of the status.
-------------------------------------------------------------------------------

PROCEDURE ["C"] WEXITSTATUS (status: BITSET) : BITSET;
BEGIN
  RETURN SHIFT (status * CAST (BITSET, 0FF00H), -8);
END WEXITSTATUS;

--------------------------------- WTERMSIG ------------------------------------
-- If WIFSIGNALED(STATUS), the terminating signal.
-------------------------------------------------------------------------------

PROCEDURE ["C"] WTERMSIG (status: BITSET) : BITSET;
BEGIN
  RETURN status * CAST (BITSET, 7FH);
END WTERMSIG;

-------------------------------- WIFEXITED ------------------------------------
-- Nonzero if STATUS indicates normal termination.
-------------------------------------------------------------------------------

PROCEDURE ["C"] WIFEXITED (status: BITSET) : BOOLEAN;
BEGIN
  RETURN (status * CAST (BITSET, 0FFH)) = CAST (BITSET, 0);
END WIFEXITED;

-------------------------------- WIFSTOPPED -----------------------------------
-- Nonzero if STATUS indicates the child is stopped.
-------------------------------------------------------------------------------

PROCEDURE ["C"] WIFSTOPPED (status: BITSET) : BOOLEAN;
BEGIN
  RETURN (status * CAST (BITSET, 0FFH)) = CAST (BITSET, 7FH);
END WIFSTOPPED;

------------------------------- WIFSIGNALED -----------------------------------
-- Nonzero if STATUS indicates termination by a signal.
-------------------------------------------------------------------------------

PROCEDURE ["C"] WIFSIGNALED (status: CARDINAL) : BOOLEAN;
BEGIN
  RETURN CAST(CARDINAL, CAST(BITSET, status-1) * CAST(BITSET, 0FFFFH)) < 0FFH;
END WIFSIGNALED;

END mac_wait.
