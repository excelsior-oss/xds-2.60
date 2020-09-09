(* Copyright (c) 1997 xTech Ltd, Russia. All rights reserved. *)

(* $RCSfile: mac_stat.mod,v $ $Revision: 1.1.1.1 $ $Date: 1997/10/20 19:32:35 $ *)

<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_stat;

(*
    Title: 	Implementation of C macros in POSIX 1003.1 sys/stat.h
    Created:	Thu Aug 21 19:16:48 1997
    Author: 	Igor L. Ziryanov
		<ego@pluto.xTech.RU>
*)

IMPORT SYSTEM;

----------------------- S_ISREG macros ----------------------------------------

PROCEDURE ["C"] S_ISREG ( m: BITSET ): BOOLEAN;
BEGIN
  RETURN (m * SYSTEM.CAST(BITSET, 170000B)) = SYSTEM.CAST(BITSET, 100000B);
END S_ISREG;

----------------------- S_ISBLK macros ----------------------------------------

PROCEDURE ["C"] S_ISBLK ( m: BITSET ): BOOLEAN;
BEGIN
  RETURN (m * SYSTEM.CAST(BITSET, 170000B)) = SYSTEM.CAST(BITSET, 60000B);
END S_ISBLK;

----------------------- S_ISDIR macros ----------------------------------------

PROCEDURE ["C"] S_ISDIR ( m: BITSET ): BOOLEAN;
BEGIN
  RETURN (m * SYSTEM.CAST(BITSET, 170000B)) = SYSTEM.CAST(BITSET, 40000B);
END S_ISDIR;

----------------------- S_ISCHR macros ----------------------------------------

PROCEDURE ["C"] S_ISCHR ( m: BITSET ): BOOLEAN;
BEGIN
  RETURN (m * SYSTEM.CAST(BITSET, 170000B)) = SYSTEM.CAST(BITSET, 20000B);
END S_ISCHR;

----------------------- S_ISFIFO macros ---------------------------------------

PROCEDURE ["C"] S_ISFIFO ( m: BITSET ): BOOLEAN;
BEGIN
  RETURN (m * SYSTEM.CAST(BITSET, 170000B)) = SYSTEM.CAST(BITSET, 10000B);
END S_ISFIFO;

----------------------- S_ISUID macros ----------------------------------------
------- LiNUX 2.0 feature or BUG? -------
-- PROCEDURE ["C"] S_ISUID ( m: BITSET ): BOOLEAN;
-- BEGIN
--   RETURN (m * SYSTEM.CAST(BITSET, 170000B)) = SYSTEM.CAST(BITSET, 4000B);
-- END S_ISUID;

----------------------- S_ISGID macros ----------------------------------------
------- LiNUX 2.0 feature or BUG? -------
-- PROCEDURE ["C"] S_ISGID ( m: BITSET ): BOOLEAN;
-- BEGIN
--   RETURN (m * SYSTEM.CAST(BITSET, 170000B)) = SYSTEM.CAST(BITSET, 2000B);
-- END S_ISGID;

END mac_stat.
