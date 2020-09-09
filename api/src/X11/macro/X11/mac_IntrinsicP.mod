(* Created using H2D v1.18 from X11/IntrinsicP.h.
 * X11/mac_IntrinsicP.mod  Nov 1  19:26:18  1996
 *
 * Copyright (c) 1996-1998 XDS Ltd, Russia. All rights reserved.
 *
 * $RCSfile: mac_IntrinsicP.mod,v $ $Revision: 5 $ $Date: 14.02.98 18:06 $
 *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_IntrinsicP;

IMPORT  SYSTEM,
      	X, Xlib,
      	Xt:=IntrinsicP;

PROCEDURE ["C"] XtClass ( widget: Xt.Widget ): Xt.WidgetClass;
BEGIN
	RETURN widget^.core.widget_class;
END XtClass;

PROCEDURE ["C"] XtDisplay ( widget: Xt.Widget ): Xlib.PtrDisplay;
BEGIN
	RETURN Xlib.DisplayOfScreen(widget^.core.screen);
END XtDisplay;

PROCEDURE ["C"] XtIsRealized ( object: Xt.Widget ): Xt.Boolean;
BEGIN
	RETURN Xt.XtWindowOfObject(object) <> X.None;
END XtIsRealized;


PROCEDURE ["C"] XtCheckSubclass ( w: Xt.Widget;
				  widget_class_ptr: Xt.WidgetClass;
			          message: ARRAY OF CHAR );
VAR
	params: ARRAY [0..2] OF Xt.String;
	num_params: Xt.Cardinal;
BEGIN
	num_params := 3;
	IF (NOT Xt.XtIsSubclass(w, widget_class_ptr)) THEN
		params[0] := w^.core.widget_class^.core_class.class_name;
		params[1] := widget_class_ptr^.core_class.class_name;
		params[2] := SYSTEM.ADR(message);
		Xt.XtAppErrorMsg(Xt.XtWidgetToApplicationContext(w),
				 "subclassMismatch", "xtCheckSubclass",
				 "XtToolkitError",
				 "Widget class %s found when subclass of %s expected: %s",
				 SYSTEM.ADR(params), num_params);
	END;
END XtCheckSubclass;


PROCEDURE ["C"] XtParent ( widget: Xt.Widget ): Xt.Widget;
BEGIN
	RETURN widget^.core.parent;
END XtParent;

PROCEDURE ["C"] XtScreen ( widget: Xt.Widget ): Xlib.PtrScreen;
BEGIN
	RETURN widget^.core.screen;
END XtScreen;

PROCEDURE ["C"] XtSuperclass ( widget: Xt.Widget ): Xt.WidgetClass;
VAR
	class: Xt.WidgetClass;
BEGIN
	class := XtClass(widget);
	RETURN class^.core_class.superclass;
END XtSuperclass;

PROCEDURE ["C"] XtWindow ( widget: Xt.Widget ): X.Window;
BEGIN
	RETURN widget^.core.window;
END XtWindow;

BEGIN
	Xt.coreClassRec := Xt.widgetClassRec;
	_XtShellPositionValid  := 1;
	_XtShellNotReparented  := 2;
	_XtShellPPositionOK    := 4;
	_XtShellGeometryParsed := 8;
END mac_IntrinsicP.
