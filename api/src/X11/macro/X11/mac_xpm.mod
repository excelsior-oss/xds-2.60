(* Created using H2D v1.1 from X11/xpm.h.
 * X11/mac_xpm.mod  Apr 8  20:49:5  1996
 *
 * Copyright (c) 1996,1997 XDS Ltd, Russia. All rights reserved.
 *
 * $RCSfile: mac_xpm.mod,v $ $Revision: 3 $ $Date: 14.02.98 18:06 $
 *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_xpm;

IMPORT SYSTEM, X, xpm, stdlib;

FROM Xlib IMPORT PChar, PAPChar;

PROCEDURE ["C"] XCreateDataFromPixmap ( dpy: xpm.PtrDisplay;
					VAR data: PAPChar;
					pix, mask: X.Pixmap;
					VAR att: xpm.XpmAttributes ): SYSTEM.int;
BEGIN
	RETURN xpm.XpmCreateDataFromPixmap(dpy, data, pix, mask, att);
END XCreateDataFromPixmap;

PROCEDURE ["C"] XCreatePixmapFromData ( dpy: xpm.PtrDisplay; d: X.Drawable;
					data: PAPChar; 
					VAR mask, pix: X.Pixmap;
					VAR att: xpm.XpmAttributes ): SYSTEM.int;
BEGIN
	RETURN xpm.XpmCreatePixmapFromData(dpy, d, data, pix, mask, att);
END XCreatePixmapFromData;

PROCEDURE ["C"] XReadPixmapFile ( dpy: xpm.PtrDisplay;
				  d: X.Drawable;
				  file: ARRAY OF CHAR;
				  VAR pix, mask: X.Pixmap;
				  VAR att: xpm.XpmAttributes ): SYSTEM.int;
BEGIN
	RETURN xpm.XpmReadFileToPixmap(dpy, d, file, pix, mask, att);
END XReadPixmapFile;

PROCEDURE ["C"] XWritePixmapFile ( dpy: xpm.PtrDisplay;
				   file: ARRAY OF CHAR;
				   pix, mask: X.Pixmap; 
				   VAR att: xpm.XpmAttributes): SYSTEM.int;
BEGIN
	RETURN xpm.XpmWriteFileFromPixmap(dpy, file, pix, mask, att);
END XWritePixmapFile;

PROCEDURE ["C"] XpmFree ( ptr: SYSTEM.ADDRESS );
BEGIN
	stdlib.free(ptr);
END XpmFree;

PROCEDURE ["C"] XpmReadPixmapFile ( dpy: xpm.PtrDisplay;
				    d: X.Drawable;
				    file: ARRAY OF CHAR;
				    VAR pix, mask: X.Pixmap;
				    VAR att: xpm.XpmAttributes ): SYSTEM.int;
BEGIN
	RETURN xpm.XpmReadFileToPixmap(dpy, d, file, pix, mask, att);
END XpmReadPixmapFile;

PROCEDURE ["C"] XpmWritePixmapFile ( dpy: xpm.PtrDisplay;
				     file: ARRAY OF CHAR; 
				     pix, mask: X.Pixmap; 
				     VAR att: xpm.XpmAttributes ): SYSTEM.int;
BEGIN
	RETURN xpm.XpmWriteFileFromPixmap(dpy, file, pix, mask, att);
END XpmWritePixmapFile;

END mac_xpm.
