(* mac_vgajoystick.mod  Feb 18  12:3:31  1999 *)
(* Copyright (c) 1999, JSC XDS *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

(* Prototypes for macro implementation *)

IMPLEMENTATION MODULE mac_vgagl;

IMPORT SYSTEM, vgagl;

PROCEDURE ["C"] BYTESPERPIXEL (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.bytesperpixel;
END BYTESPERPIXEL;

PROCEDURE ["C"] BYTEWIDTH (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.bytewidth;
END BYTEWIDTH;

PROCEDURE ["C"] WIDTH (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.width;
END WIDTH;

PROCEDURE ["C"] HEIGHT (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.height;
END HEIGHT;

PROCEDURE ["C"] VBUF (): SYSTEM.ADDRESS;
BEGIN
  RETURN vgagl.currentcontext.vbuf;
END VBUF;

PROCEDURE ["C"] MODETYPE (): SHORTCARD;
BEGIN
  RETURN vgagl.currentcontext.modetype;
END MODETYPE;

PROCEDURE ["C"] MODEFLAGS (): SHORTCARD;
BEGIN
  RETURN vgagl.currentcontext.modeflags;
END MODEFLAGS;

PROCEDURE ["C"] BITSPERPIXEL (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.bitsperpixel;
END BITSPERPIXEL;

PROCEDURE ["C"] COLORS (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.colors;
END COLORS;

PROCEDURE ["C"] __clip (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.clip;
END __clip;

PROCEDURE ["C"] __clipx1 (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.clipx1;
END __clipx1;

PROCEDURE ["C"] __clipy1 (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.clipy1;
END __clipy1;

PROCEDURE ["C"] __clipx2 (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.clipx2;
END __clipx2;

PROCEDURE ["C"] __clipy2 (): INTEGER;
BEGIN
  RETURN vgagl.currentcontext.clipy2;
END __clipy2;

END mac_vgagl.
