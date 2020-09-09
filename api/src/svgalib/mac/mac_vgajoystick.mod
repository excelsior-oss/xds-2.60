(* mac_vgajoystick.mod  Feb 18  12:3:31  1999 *)
(* Copyright (c) 1999, JSC XDS *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

(* Prototypes for macro implementation *)

IMPLEMENTATION MODULE mac_vgajoystick;

IMPORT SYSTEM, vgajoystick;

PROCEDURE ["C"] joystick_button1 ( i: INTEGER ): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getbutton(i, 0);
END joystick_button1;

PROCEDURE ["C"] joystick_button2 ( i: INTEGER ): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getbutton(i, 1);
END joystick_button2;

PROCEDURE ["C"] joystick_button3 ( i: INTEGER ): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getbutton(i, 2);
END joystick_button3;

PROCEDURE ["C"] joystick_button4 ( i: INTEGER ): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getbutton(i, 3);
END joystick_button4;

PROCEDURE ["C"] joystick_getb1 (): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getbutton(0, 0);
END joystick_getb1;

PROCEDURE ["C"] joystick_getb2 (): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getbutton(0, 1);
END joystick_getb2;

PROCEDURE ["C"] joystick_getb3 (): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getbutton(0, 2);
END joystick_getb3;

PROCEDURE ["C"] joystick_getb4 (): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getbutton(0, 3);
END joystick_getb4;

PROCEDURE ["C"] joystick_getx (): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getaxis(0, 0);
END joystick_getx;

PROCEDURE ["C"] joystick_gety (): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getaxis(0, 1);
END joystick_gety;

PROCEDURE ["C"] joystick_getz (): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getaxis(0, 2);
END joystick_getz;

PROCEDURE ["C"] joystick_x ( i: INTEGER ): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getaxis(i, 0);
END joystick_x;

PROCEDURE ["C"] joystick_y ( i: INTEGER ): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getaxis(i, 1);
END joystick_y;

PROCEDURE ["C"] joystick_z ( i: INTEGER ): CHAR;
BEGIN
  RETURN vgajoystick.joystick_getaxis(i, 2);
END joystick_z;

BEGIN
  vgajoystick.JOY_CALIB_STDOUT := SYSTEM.CAST (vgajoystick.__joystick_output,
                                               vgajoystick.joystick_init);
END mac_vgajoystick.
