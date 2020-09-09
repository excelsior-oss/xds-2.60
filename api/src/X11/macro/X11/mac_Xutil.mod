(* Created using H2D v1.00 from X11/Xutil.h.
 * X11/mac_Xutil.mod  Apr 7  18:39:47  1996
 *
 * Copyright (c) 1996-1998 XDS Ltd, Russia. All rights reserved.
 *
 * $RCSfile: mac_Xutil.mod,v $ $Revision: 3 $ $Date: 14.02.98 18:06 $
 *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_Xutil;

IMPORT	sys:=SYSTEM,
	xk :=keysym,
	xrm:=Xresource;

FROM X IMPORT KeySym;
FROM Xlib IMPORT Bool, XImage, PtrXImage, funcs;

PROCEDURE ["C"] IsCursorKey ( keysym: KeySym ): Bool;
BEGIN
	RETURN (keysym >= xk.XK_Home) AND (keysym < xk.XK_Select);
END IsCursorKey;

PROCEDURE ["C"] IsFunctionKey ( keysym: KeySym ): Bool;
BEGIN
	RETURN (keysym >= xk.XK_F1) AND (keysym <= xk.XK_F35);
END IsFunctionKey;

PROCEDURE ["C"] IsKeypadKey ( keysym: KeySym ): Bool;
BEGIN
	RETURN (keysym >= xk.XK_KP_Space) AND (keysym <= xk.XK_KP_Equal);
END IsKeypadKey;

PROCEDURE ["C"] IsMiscFunctionKey ( keysym: KeySym ): Bool;
BEGIN
	RETURN (keysym >= xk.XK_Select) AND (keysym <= xk.XK_Break);
END IsMiscFunctionKey;

PROCEDURE ["C"] IsModifierKey ( keysym: KeySym ): Bool;
BEGIN
	RETURN ((keysym >= xk.XK_Shift_L) AND (keysym <= xk.XK_Hyper_R))
	    OR  (keysym = xk.XK_Mode_switch) OR (keysym = xk.XK_Num_Lock);
END IsModifierKey;

PROCEDURE ["C"] IsPFKey ( keysym: KeySym ): Bool;
BEGIN
	RETURN (keysym >= xk.XK_KP_F1) AND (keysym <= xk.XK_KP_F4);
END IsPFKey;

PROCEDURE ["C"] IsPrivateKeypadKey ( keysym: KeySym ): Bool;
BEGIN
	RETURN (keysym >= 11000000H) AND (keysym <= 1100FFFFH);
END IsPrivateKeypadKey;

PROCEDURE ["C"] XAddPixel ( ximage: PtrXImage; value: LONGINT ): sys.int;
BEGIN
	RETURN ximage^.f.add_pixel(ximage, value);
END XAddPixel;

PROCEDURE ["C"] XDestroyImage ( ximage: PtrXImage ): sys.int;
BEGIN
	RETURN ximage^.f.destroy_image(ximage);
END XDestroyImage;

PROCEDURE ["C"] XGetPixel ( ximage: PtrXImage; x, y: sys.int ): LONGCARD;
BEGIN
	RETURN ximage^.f.get_pixel(ximage, x, y);
END XGetPixel;

PROCEDURE ["C"] XPutPixel ( ximage: PtrXImage; x, y: sys.int;
			    pixel: LONGCARD ): sys.int;
BEGIN
	RETURN ximage^.f.put_pixel(ximage, x, y, pixel);
END XPutPixel;

PROCEDURE ["C"] XStringToContext ( string: ARRAY OF CHAR ): sys.int;
BEGIN
	RETURN xrm.XrmStringToQuark(string);
END XStringToContext;

PROCEDURE ["C"] XSubImage ( ximage: PtrXImage; x, y: sys.int;
			    width, height: sys.unsigned ): PtrXImage;
BEGIN
	RETURN ximage^.f.sub_image(ximage, x, y, width, height);
END XSubImage;

PROCEDURE ["C"] XUniqueContext ( ): sys.int;
BEGIN
	RETURN xrm.XrmUniqueQuark();
END XUniqueContext;

END mac_Xutil.
