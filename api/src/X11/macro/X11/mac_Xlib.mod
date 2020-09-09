(* Created using H2D v1.00 from X11/Xlib.h.
 * X11/mac_Xlib.mod  Apr 6  12:5:3  1996
 *
 * Copyright (c) 1996-1998 XDS Ltd, Russia. All rights reserved.
 *
 * $RCSfile: mac_Xlib.mod,v $ $Revision: 4 $ $Date: 14.02.98 18:06 $
 *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_Xlib;

IMPORT SYSTEM,Xlib,X;

FROM Xlib IMPORT PtrDisplay, Screen, PtrScreen, PAScreen;

PROCEDURE ["C"] BitmapBitOrder ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.bitmap_bit_order;
END BitmapBitOrder;

PROCEDURE ["C"] BitmapPad ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.bitmap_pad;
END BitmapPad;

PROCEDURE ["C"] BitmapUnit ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.bitmap_unit;
END BitmapUnit;

PROCEDURE ["C"] BlackPixel ( dpy: PtrDisplay; scr: SYSTEM.int ): LONGCARD;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.black_pixel;
END BlackPixel;

PROCEDURE ["C"] BlackPixelOfScreen ( s: PtrScreen  ): LONGCARD;
BEGIN
	RETURN s^.black_pixel
END BlackPixelOfScreen;

PROCEDURE ["C"] CellsOfScreen ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.root_visual^.map_entries;
END CellsOfScreen;

PROCEDURE ["C"] ConnectionNumber ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.fd;
END ConnectionNumber;

PROCEDURE ["C"] DefaultColormap ( dpy: PtrDisplay; scr: SYSTEM.int ): X.Colormap;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.cmap;
END DefaultColormap;

PROCEDURE ["C"] DefaultColormapOfScreen ( s: PtrScreen  ): X.Colormap;
BEGIN
	RETURN s^.cmap;
END DefaultColormapOfScreen;

PROCEDURE ["C"] DefaultDepth ( dpy: PtrDisplay; scr: SYSTEM.int ): SYSTEM.int;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.root_depth;
END DefaultDepth;

PROCEDURE ["C"] DefaultDepthOfScreen ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.root_depth;
END DefaultDepthOfScreen;

PROCEDURE ["C"] DefaultGC ( dpy: PtrDisplay; scr: SYSTEM.int ): Xlib.GC;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.default_gc;
END DefaultGC;

PROCEDURE ["C"] DefaultGCOfScreen ( s: PtrScreen  ): Xlib.GC;
BEGIN
	RETURN s^.default_gc;
END DefaultGCOfScreen;

PROCEDURE ["C"] DefaultRootWindow ( dpy: PtrDisplay ): X.Window;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[dpy^.default_screen];
	RETURN s.root;
END DefaultRootWindow;

PROCEDURE ["C"] DefaultScreen ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.default_screen;
END DefaultScreen;

PROCEDURE ["C"] DefaultScreenOfDisplay ( dpy: PtrDisplay ): PtrScreen;
VAR
	spa: PAScreen;
BEGIN
	spa:=SYSTEM.CAST(PAScreen,dpy^.screens);
	RETURN SYSTEM.ADR( spa^[dpy^.default_screen]);
END DefaultScreenOfDisplay;

PROCEDURE ["C"] DefaultVisual ( dpy: PtrDisplay; scr: SYSTEM.int ): Xlib.PtrVisual;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.root_visual;
END DefaultVisual;

PROCEDURE ["C"] DefaultVisualOfScreen ( s: PtrScreen  ): Xlib.PtrVisual;
BEGIN
	RETURN s^.root_visual;
END DefaultVisualOfScreen;

PROCEDURE ["C"] DisplayCells ( dpy: PtrDisplay; scr: SYSTEM.int ): SYSTEM.int;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.root_visual^.map_entries;
END DisplayCells;

PROCEDURE ["C"] DisplayHeight ( dpy: PtrDisplay; scr: SYSTEM.int ): SYSTEM.int;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.height;
END DisplayHeight;

PROCEDURE ["C"] DisplayHeightMM ( dpy: PtrDisplay; scr: SYSTEM.int ): SYSTEM.int;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.mheight;
END DisplayHeightMM;

PROCEDURE ["C"] DisplayOfScreen ( s: PtrScreen  ): PtrDisplay;
BEGIN
	RETURN s^.display;
END DisplayOfScreen;

PROCEDURE ["C"] DisplayPlanes ( dpy: PtrDisplay; scr: SYSTEM.int ): SYSTEM.int;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.root_depth;
END DisplayPlanes;

PROCEDURE ["C"] DisplayString ( dpy: PtrDisplay ): Xlib.PChar;
BEGIN
	RETURN dpy^.display_name;
END DisplayString;

PROCEDURE ["C"] DisplayWidth ( dpy: PtrDisplay; scr: SYSTEM.int ): SYSTEM.int;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.width;
END DisplayWidth;

PROCEDURE ["C"] DisplayWidthMM ( dpy: PtrDisplay; scr: SYSTEM.int ): SYSTEM.int;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.mwidth;
END DisplayWidthMM;

PROCEDURE ["C"] DoesBackingStore ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.backing_store;
END DoesBackingStore;

PROCEDURE ["C"] DoesSaveUnders ( s: PtrScreen  ): BOOLEAN;
BEGIN
	RETURN s^.save_unders;
END DoesSaveUnders;

PROCEDURE ["C"] EventMaskOfScreen ( s: PtrScreen  ): BITSET;
BEGIN
	RETURN s^.root_input_mask;
END EventMaskOfScreen;

PROCEDURE ["C"] HeightMMOfScreen ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.mheight;
END HeightMMOfScreen;

PROCEDURE ["C"] HeightOfScreen ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.height;
END HeightOfScreen;

PROCEDURE ["C"] ImageByteOrder ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.byte_order;
END ImageByteOrder;

PROCEDURE ["C"] LastKnownRequestProcessed ( dpy: PtrDisplay ): LONGCARD;
BEGIN
	RETURN dpy^.last_request_read;
END LastKnownRequestProcessed;

PROCEDURE ["C"] MaxCmapsOfScreen ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.max_maps;
END MaxCmapsOfScreen;

PROCEDURE ["C"] MinCmapsOfScreen ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.min_maps;
END MinCmapsOfScreen;

PROCEDURE ["C"] NextRequest ( dpy: PtrDisplay ): LONGCARD;
BEGIN
	RETURN dpy^.request + 1;
END NextRequest;

PROCEDURE ["C"] PlanesOfScreen ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.root_depth;
END PlanesOfScreen;

PROCEDURE ["C"] ProtocolRevision ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.proto_minor_version;
END ProtocolRevision;

PROCEDURE ["C"] ProtocolVersion ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.proto_major_version;
END ProtocolVersion;

PROCEDURE ["C"] QLength ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.qlen;
END QLength;

PROCEDURE ["C"] RootWindow ( dpy: PtrDisplay; scr: SYSTEM.int ): X.Window;
VAR
	spa: PAScreen;
	s: Screen;
BEGIN
 	spa:=SYSTEM.CAST(PAScreen, dpy^.screens);
	s:=spa^[scr];
	RETURN s.root;
END RootWindow;

PROCEDURE ["C"] RootWindowOfScreen ( s: PtrScreen  ): X.Window;
BEGIN
	RETURN s^.root;
END RootWindowOfScreen;

PROCEDURE ["C"] ScreenCount ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.nscreens;
END ScreenCount;

PROCEDURE ["C"] ScreenOfDisplay ( dpy: PtrDisplay; scr: SYSTEM.int ): PtrScreen;
BEGIN
	RETURN SYSTEM.ADR( dpy^.screens^[scr]);
END ScreenOfDisplay;

PROCEDURE ["C"] ServerVendor ( dpy: PtrDisplay ): Xlib.PChar;
BEGIN
	RETURN dpy^.vendor;
END ServerVendor;

PROCEDURE ["C"] VendorRelease ( dpy: PtrDisplay ): SYSTEM.int;
BEGIN
	RETURN dpy^.release;
END VendorRelease;

PROCEDURE ["C"] WhitePixel ( dpy: PtrDisplay; scr: SYSTEM.int ): LONGCARD;
BEGIN
	RETURN dpy^.screens^[scr].white_pixel;
END WhitePixel;

PROCEDURE ["C"] WhitePixelOfScreen ( s: PtrScreen  ): LONGCARD;
BEGIN
	RETURN s^.white_pixel;
END WhitePixelOfScreen;

PROCEDURE ["C"] WidthMMOfScreen ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.mwidth;
END WidthMMOfScreen;

PROCEDURE ["C"] WidthOfScreen ( s: PtrScreen  ): SYSTEM.int;
BEGIN
	RETURN s^.width;
END WidthOfScreen;

PROCEDURE ["C"] XAllocID ( dpy: PtrDisplay ): X.XID;
BEGIN
	RETURN dpy^.resource_alloc(dpy);
END XAllocID;

END mac_Xlib.

