(* Created using H2D v1.1 from X11/Xcms.h.
 * X11/mac_Xcms.mod  Apr 12  15:24:8  1996
 *
 * Copyright (c) 1996-1998 XDS Ltd, Russia. All rights reserved.
 *
 * $RCSfile: mac_Xcms.mod,v $ $Revision: 3 $ $Date: 14.02.98 18:06 $
 *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_Xcms;

IMPORT SYSTEM;

FROM Xlib IMPORT PtrDisplay, PtrVisual, XPointer;
FROM Xcms IMPORT XcmsCCC, XcmsColor, PtrXcmsColor;

PROCEDURE ["C"] ClientWhitePointOfCCC ( ccc: XcmsCCC ): PtrXcmsColor;
BEGIN
	RETURN SYSTEM.ADR(ccc^.clientWhitePt);
END ClientWhitePointOfCCC;

PROCEDURE ["C"] DisplayOfCCC ( ccc: XcmsCCC ): PtrDisplay;
BEGIN
	RETURN ccc^.dpy;
END DisplayOfCCC;

PROCEDURE ["C"] FunctionSetOfCCC ( ccc: XcmsCCC ): XPointer;
BEGIN
	RETURN ccc^.pPerScrnInfo^.functionSet;
END FunctionSetOfCCC;

PROCEDURE ["C"] ScreenNumberOfCCC ( ccc: XcmsCCC ): SYSTEM.int;
BEGIN
	RETURN ccc^.screenNumber;
END ScreenNumberOfCCC;

PROCEDURE ["C"] ScreenWhitePointOfCCC ( ccc: XcmsCCC ): PtrXcmsColor;
BEGIN
	RETURN SYSTEM.ADR( ccc^.pPerScrnInfo^.screenWhitePt);
END ScreenWhitePointOfCCC;

PROCEDURE ["C"] VisualOfCCC ( ccc: XcmsCCC ): PtrVisual;
BEGIN
	RETURN ccc^.visual;
END VisualOfCCC;

END mac_Xcms.
