(* Created using H2D v1.00 from X11/Xlib.h.
 * X11/mac_Intrinsic.mod  May 18  16:12:4  1996
 *
 * Copyright (c) 1996-1998 XDS Ltd, Russia. All rights reserved.
 *
 * $RCSfile: mac_Intrinsic.mod,v $ $Revision: 3 $ $Date: 14.02.98 18:06 $
 *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_Intrinsic;

IMPORT SYSTEM,
       Xlib,
       Xt:=Intrinsic,
       vn:=Vendor,
       sh:=Shell,
       io:=stdio,
       string;

FROM Storage IMPORT ALLOCATE;

PROCEDURE ["C"] XtIsApplicationShell ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtIsSubclassOf(widget, sh.applicationShellWidgetClass,
			       sh.topLevelShellWidgetClass, 80H);
END XtIsApplicationShell;

PROCEDURE ["C"] XtIsComposite ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtCheckSubclassFlag(widget, 8H);
END XtIsComposite;

PROCEDURE ["C"] XtIsConstraint ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtCheckSubclassFlag(widget, 10H);
END XtIsConstraint;

PROCEDURE ["C"] XtIsOverrideShell ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtIsSubclassOf(widget, sh.overrideShellWidgetClass,
                               sh.shellWidgetClass, 20H);
END XtIsOverrideShell;

PROCEDURE ["C"] XtIsRectObj ( object: Widget ): Boolean;
BEGIN
	RETURN Xt._XtCheckSubclassFlag(object, 2H);
END XtIsRectObj;

PROCEDURE ["C"] XtIsSessionShell ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtIsSubclassOf(widget, sh.sessionShellWidgetClass,
			       sh.topLevelShellWidgetClass, 80H);
END XtIsSessionShell;

PROCEDURE ["C"] XtIsShell ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtCheckSubclassFlag(widget, 20H);
END XtIsShell;

PROCEDURE ["C"] XtIsTopLevelShell ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtCheckSubclassFlag(widget, 80H);
END XtIsTopLevelShell;

PROCEDURE ["C"] XtIsTransientShell ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtIsSubclassOf(widget, sh.transientShellWidgetClass,
			       sh.wmShellWidgetClass, 40H);
END XtIsTransientShell;

PROCEDURE ["C"] XtIsVendorShell ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtIsSubclassOf(widget, vn.vendorShellWidgetClass,
			       sh.wmShellWidgetClass, 40H);
END XtIsVendorShell;

PROCEDURE ["C"] XtIsWMShell ( widget: Widget ): Boolean;
BEGIN
	RETURN Xt._XtCheckSubclassFlag(widget, 40H);
END XtIsWMShell;

PROCEDURE ["C"] XtIsWidget ( object: Widget ): Boolean;
BEGIN
	RETURN Xt._XtCheckSubclassFlag(object, 4H);
END XtIsWidget;

PROCEDURE ["C"] XtMapWidget ( widget: Widget );
BEGIN
	Xlib.XMapWindow(Xt.XtDisplay(widget),Xt.XtWindow(widget));
END XtMapWidget;

PROCEDURE ["C"] XtNew ( type: Cardinal ): SYSTEM.ADDRESS;
BEGIN
  io.fprintf(io.stderr^, 'XtNew: Don`t use XtNew - TRY XtMalloc instead\n');
  RETURN Xt.XtMalloc(type);
END XtNew;

PROCEDURE ["C"] XtNewString ( str: ARRAY OF CHAR ): String;
VAR
  ret: String;
BEGIN
  IF str[0] # 0C THEN
     ALLOCATE(ret, SIZE(CHAR)*((string.strlen(str)+1)));
     IF ret # NIL THEN
     	  string.strcpy(ret^, str);
     END (* if ret # NIL *);
     RETURN ret;
  ELSE
     RETURN NIL;
  END (* if str # '' *);
END XtNewString;

PROCEDURE ["C"] XtNumber ( arr: XtPointer ): Cardinal;
BEGIN
  io.fprintf(io.stderr^, 'XtNumber: Don`t use XtNumber - it doesn`t work correct (returns zero)\n');
  io.fprintf(io.stderr^, 'XtNumber: In C macro it returns sizeof(arr)/sizeof(arr[0])');
  RETURN 0;
END XtNumber;

PROCEDURE ["C"] XtSetArg ( VAR arg: Xt.Arg; n: ARRAY OF CHAR; d: Xt.XtArgVal );
BEGIN
  ALLOCATE(arg.name, SIZE(CHAR)*(string.strlen(n)+1));
  string.strcpy(arg.name^, n);
  arg.value := d;
END XtSetArg;

PROCEDURE ["C"] XtUnmapWidget ( widget: Widget );
BEGIN
  Xlib.XUnmapWindow(Xt.XtDisplay(widget), Xt.XtWindow(widget));
END XtUnmapWidget;

END mac_Intrinsic.
