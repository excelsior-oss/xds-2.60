(* Created using H2D v1.00 from X11/Xresource.h.
 * X11/mac_Xresource.mod  Apr 7  20:5:12  1996
 *
 * Copyright (c) 1996-1998 XDS Ltd, Russia. All rights reserved.
 *
 * $RCSfile: mac_Xresource.mod,v $ $Revision: 3 $ $Date: 14.02.98 18:06 $
 *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_Xresource;

IMPORT 	string,
	xrm:=Xresource;

FROM Xresource IMPORT XrmClass, XrmClassList, XrmString, XrmRepresentation,
     	       	      XrmName, XrmNameList;

PROCEDURE ["C"] XrmClassToString ( c_class: XrmClass ): XrmString;
BEGIN
	RETURN xrm.XrmQuarkToString( c_class );
END XrmClassToString;

PROCEDURE ["C"] XrmNameToString ( name: XrmName ): XrmString;
BEGIN
	RETURN xrm.XrmQuarkToString( name );
END XrmNameToString;

PROCEDURE ["C"] XrmRepresentationToString ( type: XrmRepresentation ): XrmString;
BEGIN
	RETURN xrm.XrmQuarkToString( type );
END XrmRepresentationToString;

PROCEDURE ["C"] XrmStringToClass ( c_class: ARRAY OF CHAR ): XrmClass;
BEGIN
	RETURN xrm.XrmStringToQuark( c_class );
END XrmStringToClass;

PROCEDURE ["C"] XrmStringToClassList ( str: ARRAY OF CHAR; c_class: XrmClassList );
BEGIN
	xrm.XrmStringToQuarkList( str, c_class );
END XrmStringToClassList;

PROCEDURE ["C"] XrmStringToName ( string: ARRAY OF CHAR ): XrmName;
BEGIN
	RETURN xrm.XrmStringToQuark( string );
END XrmStringToName;

PROCEDURE ["C"] XrmStringToNameList ( str: ARRAY OF CHAR; name: XrmNameList );
BEGIN
	xrm.XrmStringToQuarkList( str, name );
END XrmStringToNameList;

PROCEDURE ["C"] XrmStringToRepresentation ( string: ARRAY OF CHAR ): XrmRepresentation;
BEGIN
	RETURN xrm.XrmStringToQuark( string );
END XrmStringToRepresentation;

PROCEDURE ["C"] XrmStringsEqual ( a1, a2: ARRAY OF CHAR ): BOOLEAN;
BEGIN
	RETURN (string.strcmp(a1,a2) = 0);
END XrmStringsEqual;

END mac_Xresource.
