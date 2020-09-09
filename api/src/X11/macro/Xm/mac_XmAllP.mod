(* Created using H2D v1.18.6a from Xm/XmAllP.h.
 * Xm/mac_XmAllP.mod  Nov 27  9:46:19  1996
 *
 * Copyright (c) 1996-1998 XDS Ltd, Russia. All rights reserved.
 *
 * $RCSfile: mac_XmAllP.mod,v $ $Revision: 4 $ $Date: 14.02.98 18:06 $
 *)

<*+ M2ADDTYPES *>
<*+ M2EXTENSIONS *>

IMPLEMENTATION MODULE mac_XmAllP;

IMPORT SYSTEM, X, Xlib, Xrm:=Xresource, Xt:=IntrinsicP, Xm:=XmAllP;

FROM SYSTEM IMPORT CAST, ADR, SHIFT;
FROM X IMPORT SET8;

PROCEDURE ["C"] BB_ButtonFontList ( w: Xm.XmBulletinBoardWidget ): Xm.XmFontList;
BEGIN
  RETURN w^.bulletin_board.button_font_list;
END BB_ButtonFontList;

PROCEDURE ["C"] BB_CancelButton ( w: Xm.XmBulletinBoardWidget ): Xt.Widget;
BEGIN
  RETURN w^.bulletin_board.cancel_button;
END BB_CancelButton;

PROCEDURE ["C"] BB_DefaultButton ( w: Xm.XmBulletinBoardWidget ): Xt.Widget;
BEGIN
  RETURN w^.bulletin_board.default_button;
END BB_DefaultButton;

PROCEDURE ["C"] BB_DynamicCancelButton ( w: Xm.XmBulletinBoardWidget ): Xt.Widget;
BEGIN
  RETURN w^.bulletin_board.dynamic_cancel_button;
END BB_DynamicCancelButton;

PROCEDURE ["C"] BB_DynamicDefaultButton ( w: Xm.XmBulletinBoardWidget ): Xt.Widget;
BEGIN
  RETURN w^.bulletin_board.dynamic_default_button;
END BB_DynamicDefaultButton;

PROCEDURE ["C"] BB_InSetValues ( w: Xm.XmBulletinBoardWidget ): Xt.Boolean;
BEGIN
  RETURN w^.bulletin_board.in_set_values;
END BB_InSetValues;

PROCEDURE ["C"] BB_InitialFocus ( w: Xm.XmBulletinBoardWidget ): Xt.Boolean;
BEGIN
  RETURN w^.bulletin_board.initial_focus;
END BB_InitialFocus;

PROCEDURE ["C"] BB_LabelFontList ( w: Xm.XmBulletinBoardWidget ): Xm.XmFontList;
BEGIN
  RETURN w^.bulletin_board.label_font_list;
END BB_LabelFontList;

PROCEDURE ["C"] BB_MarginHeight ( w: Xm.XmBulletinBoardWidget ): Xt.Dimension;
BEGIN
  RETURN w^.bulletin_board.margin_height;
END BB_MarginHeight;

PROCEDURE ["C"] BB_MarginWidth ( w: Xm.XmBulletinBoardWidget ): Xt.Dimension;
BEGIN
  RETURN w^.bulletin_board.margin_width;
END BB_MarginWidth;

PROCEDURE ["C"] BB_ResizePolicy ( w: Xm.XmBulletinBoardWidget ): SHORTCARD;
BEGIN
  RETURN w^.bulletin_board.resize_policy;
END BB_ResizePolicy;

PROCEDURE ["C"] BB_StringDirection ( w: Xm.XmBulletinBoardWidget ): Xm.XmStringDirection;
BEGIN
  RETURN w^.manager.string_direction;
END BB_StringDirection;

PROCEDURE ["C"] BB_TextFontList ( w: Xm.XmBulletinBoardWidget ): Xm.XmFontList;
BEGIN
  RETURN w^.bulletin_board.text_font_list;
END BB_TextFontList;

PROCEDURE ["C"] CBG_ActivateCall ( cb: Xm.XmCascadeButtonGadget ): Xt.XtCallbackList;
BEGIN
  RETURN cb^.cascade_button.activate_callback;
END CBG_ActivateCall;

PROCEDURE ["C"] CBG_Armed ( cb: Xm.XmCascadeButtonGadget ): Xt.Boolean;
BEGIN
  RETURN cb^.cascade_button.armed;
END CBG_Armed;

PROCEDURE ["C"] CBG_ArmedPixmap ( cb: Xm.XmCascadeButtonGadget ): X.Pixmap;
BEGIN
  RETURN cb^.cascade_button.cache^.armed_pixmap;
END CBG_ArmedPixmap;

PROCEDURE ["C"] CBG_Cache ( cb: Xm.XmCascadeButtonGadget ): Xm.XmCascadeButtonGCache;
BEGIN
  RETURN cb^.cascade_button.cache;
END CBG_Cache;

PROCEDURE ["C"] CBG_CascadeCall ( cb: Xm.XmCascadeButtonGadget ): Xt.XtCallbackList;
BEGIN
  RETURN cb^.cascade_button.cascade_callback;
END CBG_CascadeCall;

PROCEDURE ["C"] CBG_CascadePixmap ( cb: Xm.XmCascadeButtonGadget ): X.Pixmap;
BEGIN
  RETURN cb^.cascade_button.cache^.cascade_pixmap;
END CBG_CascadePixmap;

PROCEDURE ["C"] CBG_CascadeRect ( cb: Xm.XmCascadeButtonGadget ): Xlib.XRectangle;
BEGIN
  RETURN cb^.cascade_button.cascade_rect;
END CBG_CascadeRect;

PROCEDURE ["C"] CBG_Cascade_height ( cb: Xm.XmCascadeButtonGadget ): Xt.Dimension;
BEGIN
  RETURN cb^.cascade_button.cascade_rect.height;
END CBG_Cascade_height;

PROCEDURE ["C"] CBG_Cascade_width ( cb: Xm.XmCascadeButtonGadget ): Xt.Dimension;
BEGIN
  RETURN cb^.cascade_button.cascade_rect.width;
END CBG_Cascade_width;

PROCEDURE ["C"] CBG_Cascade_x ( cb: Xm.XmCascadeButtonGadget ): Xt.Position;
BEGIN
  RETURN cb^.cascade_button.cascade_rect.x;
END CBG_Cascade_x;

PROCEDURE ["C"] CBG_Cascade_y ( cb: Xm.XmCascadeButtonGadget ): Xt.Position;
BEGIN
  RETURN cb^.cascade_button.cascade_rect.y;
END CBG_Cascade_y;

PROCEDURE ["C"] CBG_ClassCachePart ( w: Xm.XmCascadeButtonGadgetClass ): Xm.XmCacheClassPartPtr;
BEGIN
  RETURN w^.gadget_class.cache_part;
END CBG_ClassCachePart;

PROCEDURE ["C"] CBG_HasCascade ( cb: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN ((Xm.LabG_MenuType(cb) = Xm.XmMENU_PULLDOWN) OR
          (Xm.LabG_MenuType(cb) = Xm.XmMENU_POPUP) OR
          (Xm.LabG_MenuType(cb) = Xm.XmMENU_OPTION)) AND
         (Xm.CBG_Submenu(cb) <> NIL);
END CBG_HasCascade;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] CBG_IsArmed ( cb: Xm.XmCascadeButtonGadget ): Xt.Boolean;
BEGIN
  RETURN cb^.cascade_button.armed OR
         ((CAST(BITSET,cb^.cascade_button.armed) *
           CAST(BITSET,Xm.XmCBG_ARMED_BIT)) # CAST(BITSET,0));
  (* in C is return cb->cascade_button.armed & XmCBG_TRAVERSE_BIT; *)
END CBG_IsArmed;

PROCEDURE ["C"] CBG_MapDelay ( cb: Xm.XmCascadeButtonGadget ): INTEGER;
BEGIN
  RETURN cb^.cascade_button.cache^.map_delay;
END CBG_MapDelay;

PROCEDURE ["C"] CBG_SetArmed ( cb: Xm.XmCascadeButtonGadget;
                               v: BOOLEAN );
BEGIN
  Xm.CBG_SetBit( cb^.cascade_button.armed, Xm.XmCBG_ARMED_BIT, v);
END CBG_SetArmed;

(* Can`t make same procedure becouse stupid bitset operations in Modula *)
PROCEDURE ["C"] CBG_SetBit ( VAR byte: Xt.Boolean;
                             bit: SET8;
                             v: BOOLEAN );
VAR
  my_bitset: SET8;
BEGIN
  (* #define CBG_SetBit(byte,bit,v)  byte = (byte & (~bit)) | (v ? bit : 0) *)
  my_bitset := CAST(SET8,byte);
  IF v THEN
    my_bitset := my_bitset + bit;
  ELSE
    my_bitset := my_bitset - bit;
  END;
  byte:=CAST(Xt.Boolean,my_bitset);
END CBG_SetBit;

PROCEDURE ["C"] CBG_SetTraverse ( cb: Xm.XmCascadeButtonGadget; v: BOOLEAN );
BEGIN
  Xm.CBG_SetBit( cb^.cascade_button.armed, Xm.XmCBG_TRAVERSE_BIT, v);
END CBG_SetTraverse;

PROCEDURE ["C"] CBG_Submenu ( cb: Xm.XmCascadeButtonGadget ): Xt.Widget;
BEGIN
  RETURN cb^.cascade_button.submenu;
END CBG_Submenu;

PROCEDURE ["C"] CBG_Timer ( cb: Xm.XmCascadeButtonGadget ): Xt.XtIntervalId;
BEGIN
  RETURN cb^.cascade_button.timer;
END CBG_Timer;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] CBG_Traversing ( cb: Xm.XmCascadeButtonGadget ): Xt.Boolean;
BEGIN
  RETURN cb^.cascade_button.armed OR
         ((CAST(BITSET,cb^.cascade_button.armed) *
           CAST(BITSET,Xm.XmCBG_TRAVERSE_BIT)) # CAST(BITSET,0));
  (* in C is return cb->cascade_button.armed & XmCBG_TRAVERSE_BIT; *)
END CBG_Traversing;

PROCEDURE ["C"] CB_ActivateCall ( cb: Xm.XmCascadeButtonWidget ): Xt.XtCallbackList;
BEGIN
  RETURN cb^.cascade_button.activate_callback;
END CB_ActivateCall;

PROCEDURE ["C"] CB_ArmedPixmap ( cb: Xm.XmCascadeButtonWidget ): X.Pixmap;
BEGIN
  RETURN cb^.cascade_button.armed_pixmap;
END CB_ArmedPixmap;

PROCEDURE ["C"] CB_CascadeCall ( cb: Xm.XmCascadeButtonWidget ): Xt.XtCallbackList;
BEGIN
  RETURN cb^.cascade_button.cascade_callback;
END CB_CascadeCall;

PROCEDURE ["C"] CB_CascadePixmap ( cb: Xm.XmCascadeButtonWidget ): X.Pixmap;
BEGIN
  RETURN cb^.cascade_button.cascade_pixmap;
END CB_CascadePixmap;

PROCEDURE ["C"] CB_Cascade_height ( cb: Xm.XmCascadeButtonWidget ): Xt.Dimension;
BEGIN
  RETURN cb^.cascade_button.cascade_rect.height;
END CB_Cascade_height;

PROCEDURE ["C"] CB_Cascade_width ( cb: Xm.XmCascadeButtonWidget ): Xt.Dimension;
BEGIN
  RETURN cb^.cascade_button.cascade_rect.width;
END CB_Cascade_width;

PROCEDURE ["C"] CB_Cascade_x ( cb: Xm.XmCascadeButtonWidget ): Xt.Position;
BEGIN
  RETURN cb^.cascade_button.cascade_rect.x;
END CB_Cascade_x;

PROCEDURE ["C"] CB_Cascade_y ( cb: Xm.XmCascadeButtonWidget ): Xt.Position;
BEGIN
  RETURN cb^.cascade_button.cascade_rect.y;
END CB_Cascade_y;

PROCEDURE ["C"] CB_HasCascade ( cb: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN ((Xm.Lab_MenuType(cb) = Xm.XmMENU_PULLDOWN) OR
          (Xm.Lab_MenuType(cb) = Xm.XmMENU_POPUP)) AND
         (Xm.CB_Submenu(cb) <> NIL);
END CB_HasCascade;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] CB_IsArmed ( cb: Xm.XmCascadeButtonWidget ): Xt.Boolean;
BEGIN
  RETURN cb^.cascade_button.armed OR
         ((CAST(BITSET,cb^.cascade_button.armed) *
           CAST(BITSET,Xm.XmCB_ARMED_BIT)) # CAST(BITSET,0));
  (* In C is return cb->cascade_button.armed & XmCB_ARMED_BIT; *)
END CB_IsArmed;

PROCEDURE ["C"] CB_SetArmed ( cb: Xm.XmCascadeButtonWidget; v: BOOLEAN );
BEGIN
  Xm.CB_SetBit( cb^.cascade_button.armed, Xm.XmCB_ARMED_BIT, v);
END CB_SetArmed;

(* Can`t make same procedure becouse stupid bitset operations in Modula *)
PROCEDURE ["C"] CB_SetBit ( VAR byte: Xt.Boolean;
                            bit: SET8;
                            v: BOOLEAN );
VAR
  my_bitset: SET8;
BEGIN
  (* #define CB_SetBit(byte,bit,v)  byte = (byte & (~bit)) | (v ? bit : 0) *)
  my_bitset := CAST(SET8,byte);
  IF v THEN
    my_bitset := my_bitset + bit;
  ELSE
    my_bitset := my_bitset - bit;
  END;
  byte:=CAST(Xt.Boolean,my_bitset);
END CB_SetBit;

PROCEDURE ["C"] CB_SetTraverse ( cb: Xm.XmCascadeButtonWidget;
                                 v: BOOLEAN );
BEGIN
  Xm.CB_SetBit( cb^.cascade_button.armed, Xm.XmCB_TRAVERSE_BIT, v);
END CB_SetTraverse;

PROCEDURE ["C"] CB_Submenu ( cb: Xm.XmCascadeButtonWidget ): Xt.Widget;
BEGIN
  RETURN cb^.cascade_button.submenu;
END CB_Submenu;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] CB_Traversing ( cb: Xm.XmCascadeButtonWidget ): Xt.Boolean;
BEGIN
  RETURN cb^.cascade_button.armed OR
         ((CAST(BITSET,cb^.cascade_button.armed) *
           CAST(BITSET,Xm.XmCB_TRAVERSE_BIT)) # CAST(BITSET,0));
  (* In C is return cb->cascade_button.armed & XmCB_TRAVERSE_BIT; *)
END CB_Traversing;

PROCEDURE ["C"] CacheDataPtr ( p: Xm.XmGadgetCacheRefPtr ): Xt.XtPointer;
BEGIN
  RETURN ADR(p^.data);
END CacheDataPtr;

PROCEDURE ["C"] ClassCacheCompare ( cp: Xm.XmCacheClassPartPtr ): Xm.XmCacheCompareProc;
BEGIN
  RETURN cp^.cache_compare;
END ClassCacheCompare;

PROCEDURE ["C"] ClassCacheCopy ( cp: Xm.XmCacheClassPartPtr ): Xm.XmCacheCopyProc;
BEGIN
  RETURN cp^.cache_copy;
END ClassCacheCopy;

PROCEDURE ["C"] ClassCacheHead ( cp: Xm.XmCacheClassPartPtr ): Xm.XmGadgetCache;
BEGIN
  RETURN cp^.cache_head;
END ClassCacheHead;

PROCEDURE ["C"] DSMChangeRoot ( dsm: Xm.XmDropSiteManagerObject;
                                clientData, callData: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.changeRoot(dsm, clientData, callData);
END DSMChangeRoot;

PROCEDURE ["C"] DSMCreateInfo ( dsm: Xm.XmDropSiteManagerObject;
                                widget: Xt.Widget;
                                args: Xt.ArgList;
                                numArgs: Xt.Cardinal );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.createInfo(dsm, widget, args, numArgs);
END DSMCreateInfo;

PROCEDURE ["C"] DSMCreateTable ( dsm: Xm.XmDropSiteManagerObject );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.createTable(dsm);
END DSMCreateTable;

PROCEDURE ["C"] DSMDestroyInfo ( dsm: Xm.XmDropSiteManagerObject;
                                 widget: Xt.Widget );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.destroyInfo(dsm, widget);
END DSMDestroyInfo;

PROCEDURE ["C"] DSMDestroyTable ( dsm: Xm.XmDropSiteManagerObject );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.destroyTable(dsm);
END DSMDestroyTable;

PROCEDURE ["C"] DSMEndUpdate ( dsm: Xm.XmDropSiteManagerObject;
                               widget: Xt.Widget );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.endUpdate(dsm, widget);
END DSMEndUpdate;

PROCEDURE ["C"] DSMGetTreeFromDSM ( dsm: Xm.XmDropSiteManagerObject;
                                    shell, dataPtr: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.getTreeFromDSM(dsm, shell, dataPtr);
END DSMGetTreeFromDSM;

PROCEDURE ["C"] DSMInsertInfo ( dsm: Xm.XmDropSiteManagerObject;
                                info, call_data: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.insertInfo(dsm, info, call_data);
END DSMInsertInfo;

PROCEDURE ["C"] DSMOperationChanged ( dsm: Xm.XmDropSiteManagerObject;
                                      clientData, callData: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.operationChanged(dsm, clientData, callData);
END DSMOperationChanged;

PROCEDURE ["C"] DSMProcessDrop ( dsm: Xm.XmDropSiteManagerObject;
                                 clientData, callData: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.processDrop(dsm, clientData, callData);
END DSMProcessDrop;

PROCEDURE ["C"] DSMProcessMotion ( dsm: Xm.XmDropSiteManagerObject;
                                   clientData, callData: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.processMotion(dsm, clientData, callData);
END DSMProcessMotion;

PROCEDURE ["C"] DSMRegisterInfo ( dsm: Xm.XmDropSiteManagerObject;
                                  widget: Xt.Widget;
                                  info: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.registerInfo(dsm, widget, info);
END DSMRegisterInfo;

PROCEDURE ["C"] DSMRemoveInfo ( dsm: Xm.XmDropSiteManagerObject;
                                info: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.removeInfo(dsm, info);
END DSMRemoveInfo;

PROCEDURE ["C"] DSMRetrieveInfo ( dsm: Xm.XmDropSiteManagerObject;
                                  widget: Xt.Widget;
                                  args: Xt.ArgList;
                                  numArgs: Xt.Cardinal );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.retrieveInfo(dsm, widget, args, numArgs);
END DSMRetrieveInfo;

PROCEDURE ["C"] DSMStartUpdate ( dsm: Xm.XmDropSiteManagerObject;
                                 widget: Xt.Widget );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.startUpdate(dsm, widget);
END DSMStartUpdate;

PROCEDURE ["C"] DSMSyncTree ( dsm: Xm.XmDropSiteManagerObject;
                              shell: Xt.Widget );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.syncTree(dsm, shell);
END DSMSyncTree;

PROCEDURE ["C"] DSMUnregisterInfo ( dsm: Xm.XmDropSiteManagerObject;
                                    info: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.unregisterInfo(dsm, info);
END DSMUnregisterInfo;

PROCEDURE ["C"] DSMUpdate ( dsm: Xm.XmDropSiteManagerObject;
                            clientData, callData: Xt.XtPointer );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.updateDSM(dsm, clientData, callData);
END DSMUpdate;

PROCEDURE ["C"] DSMUpdateInfo ( dsm: Xm.XmDropSiteManagerObject;
                                widget: Xt.Widget;
                                args: Xt.ArgList;
                                numArgs: Xt.Cardinal );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.updateInfo(dsm, widget, args, numArgs);
END DSMUpdateInfo;

PROCEDURE ["C"] DSMWidgetToInfo ( dsm: Xm.XmDropSiteManagerObject;
                                  widget: Xt.Widget );
VAR
  dsmoc: Xm.XmDropSiteManagerObjectClass;
BEGIN
  dsmoc:=CAST(Xm.XmDropSiteManagerObjectClass,
              Xt.XtClass(CAST(Xt.Widget, dsm)));
  dsmoc^.dropManager_class.widgetToInfo(dsm, widget);
END DSMWidgetToInfo;

PROCEDURE ["C"] FS_DirList ( w: Xm.XmFileSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.file_selection_box.dir_list;
END FS_DirList;

PROCEDURE ["C"] FS_DirListItemCount ( w: Xm.XmFileSelectionBoxWidget ): INTEGER;
BEGIN
  RETURN w^.file_selection_box.dir_list_item_count;
END FS_DirListItemCount;

PROCEDURE ["C"] FS_DirListItems ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmStringTable;
BEGIN
  RETURN w^.file_selection_box.dir_list_items;
END FS_DirListItems;

PROCEDURE ["C"] FS_DirListLabel ( w: Xm.XmFileSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.file_selection_box.dir_list_label;
END FS_DirListLabel;

PROCEDURE ["C"] FS_DirListLabelString ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmString;
BEGIN
  RETURN w^.file_selection_box.dir_list_label_string;
END FS_DirListLabelString;

PROCEDURE ["C"] FS_DirListSelectedItemPosition ( w: Xm.XmFileSelectionBoxWidget ): INTEGER;
BEGIN
  RETURN w^.file_selection_box.dir_list_selected_item_position;
END FS_DirListSelectedItemPosition;

PROCEDURE ["C"] FS_DirMask ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmString;
BEGIN
  RETURN w^.file_selection_box.dir_mask;
END FS_DirMask;

PROCEDURE ["C"] FS_DirSearchProc ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmSearchProc;
BEGIN
  RETURN w^.file_selection_box.dir_search_proc;
END FS_DirSearchProc;

PROCEDURE ["C"] FS_Directory ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmString;
BEGIN
  RETURN w^.file_selection_box.directory;
END FS_Directory;

PROCEDURE ["C"] FS_DirectoryValid ( w: Xm.XmFileSelectionBoxWidget ): Xt.Boolean;
BEGIN
  RETURN w^.file_selection_box.directory_valid;
END FS_DirectoryValid;

PROCEDURE ["C"] FS_FileSearchProc ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmSearchProc;
BEGIN
  RETURN w^.file_selection_box.file_search_proc;
END FS_FileSearchProc;

PROCEDURE ["C"] FS_FileTypeMask ( w: Xm.XmFileSelectionBoxWidget ): SHORTCARD;
BEGIN
  RETURN w^.file_selection_box.file_type_mask;
END FS_FileTypeMask;

PROCEDURE ["C"] FS_FilterLabel ( w: Xm.XmFileSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.file_selection_box.filter_label;
END FS_FilterLabel;

PROCEDURE ["C"] FS_FilterLabelString ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmString;
BEGIN
  RETURN w^.file_selection_box.filter_label_string;
END FS_FilterLabelString;

PROCEDURE ["C"] FS_FilterText ( w: Xm.XmFileSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.file_selection_box.filter_text;
END FS_FilterText;

PROCEDURE ["C"] FS_ListUpdated ( w: Xm.XmFileSelectionBoxWidget ): Xt.Boolean;
BEGIN
  RETURN w^.file_selection_box.list_updated;
END FS_ListUpdated;

PROCEDURE ["C"] FS_NoMatchString ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmString;
BEGIN
  RETURN w^.file_selection_box.no_match_string;
END FS_NoMatchString;

PROCEDURE ["C"] FS_Pattern ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmString;
BEGIN
  RETURN w^.file_selection_box.pattern;
END FS_Pattern;

PROCEDURE ["C"] FS_QualifySearchDataProc ( w: Xm.XmFileSelectionBoxWidget ): Xm.XmQualifyProc;
BEGIN
  RETURN w^.file_selection_box.qualify_search_data_proc;
END FS_QualifySearchDataProc;

(* This field (such as real_default_button) doesn`t exists in structure
   Probably Motif Programmers Team BUG :)
PROCEDURE ["C"] FS_RealDefaultButton ( w: Xm.XmFileSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.file_selection_box.real_default_button;
END FS_RealDefaultButton;
*)

PROCEDURE ["C"] FS_StateFlags ( w: Xm.XmFileSelectionBoxWidget ): SHORTCARD;
BEGIN
  RETURN w^.file_selection_box.state_flags;
END FS_StateFlags;

PROCEDURE ["C"] GCEPTR ( wc: Xm.XmGadgetClass ): Xt.XtPointer;
BEGIN
  RETURN ADR(wc^.gadget_class.extension);
END GCEPTR;

PROCEDURE ["C"] G_HighlightThickness ( g: Xm.XmGadget ): Xt.Dimension;
BEGIN
  RETURN g^.gadget.highlight_thickness;
END G_HighlightThickness;

PROCEDURE ["C"] G_ShadowThickness ( g: Xm.XmGadget ): Xt.Dimension;
BEGIN
  RETURN g^.gadget.shadow_thickness;
END G_ShadowThickness;

PROCEDURE ["C"] LabG_AccTextRect ( w: Xm.XmLabelGadget ): Xlib.XRectangle;
BEGIN
  RETURN w^.label.acc_TextRect;
END LabG_AccTextRect;

PROCEDURE ["C"] LabG_Accelerator ( w: Xm.XmLabelGadget ): Xt.String;
BEGIN
  RETURN w^.label.accelerator;
END LabG_Accelerator;

PROCEDURE ["C"] LabG_Alignment ( w: Xm.XmLabelGadget ): SHORTCARD;
BEGIN
  RETURN w^.label.cache^.alignment;
END LabG_Alignment;

PROCEDURE ["C"] LabG_Baseline ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN Xm._XmStringBaseline (w^.label.font, w^.label._label);
END LabG_Baseline;

PROCEDURE ["C"] LabG_Cache ( w: Xm.XmLabelGadget ): Xm.XmLabelGCache;
BEGIN
  RETURN w^.label.cache;
END LabG_Cache;

PROCEDURE ["C"] LabG_ClassCachePart ( w: Xm.XmLabelGadgetClass ): Xm.XmCacheClassPartPtr;
BEGIN
  w:=CAST(Xm.XmLabelGadgetClass,Xm.xmLabelGadgetClass);
  RETURN w^.gadget_class.cache_part;
END LabG_ClassCachePart;

PROCEDURE ["C"] LabG_Font ( w: Xm.XmLabelGadget ): Xm.XmFontList;
BEGIN
  RETURN w^.label.font;
END LabG_Font;

PROCEDURE ["C"] LabG_Highlight ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.gadget.highlight_thickness;
END LabG_Highlight;

PROCEDURE ["C"] LabG_InsensitiveGC ( w: Xm.XmLabelGadget ): Xlib.GC;
BEGIN
  RETURN w^.label.insensitive_GC;
END LabG_InsensitiveGC;

PROCEDURE ["C"] LabG_IsPixmap ( w: Xm.XmLabelGadget ): Xt.Boolean;
BEGIN
  RETURN LabG_LabelType(w) = Xm.XmPIXMAP;
END LabG_IsPixmap;

PROCEDURE ["C"] LabG_IsText ( w: Xm.XmLabelGadget ): Xt.Boolean;
BEGIN
  RETURN LabG_LabelType(w) = Xm.XmSTRING;
END LabG_IsText;

PROCEDURE ["C"] LabG_LabelType ( w: Xm.XmLabelGadget ): SHORTCARD;
BEGIN
  RETURN w^.label.cache^.label_type;
END LabG_LabelType;

PROCEDURE ["C"] LabG_MarginBottom ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.label.cache^.margin_bottom;
END LabG_MarginBottom;

PROCEDURE ["C"] LabG_MarginHeight ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.label.cache^.margin_height;
END LabG_MarginHeight;

PROCEDURE ["C"] LabG_MarginLeft ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.label.cache^.margin_left;
END LabG_MarginLeft;

PROCEDURE ["C"] LabG_MarginRight ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.label.cache^.margin_right;
END LabG_MarginRight;

PROCEDURE ["C"] LabG_MarginTop ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.label.cache^.margin_top;
END LabG_MarginTop;

PROCEDURE ["C"] LabG_MarginWidth ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.label.cache^.margin_width;
END LabG_MarginWidth;

PROCEDURE ["C"] LabG_MenuType ( w: Xm.XmLabelGadget ): SHORTCARD;
BEGIN
  RETURN w^.label.cache^.menu_type;
END LabG_MenuType;

PROCEDURE ["C"] LabG_Mnemonic ( w: Xm.XmLabelGadget ): X.KeySym;
BEGIN
  RETURN w^.label.mnemonic;
END LabG_Mnemonic;

PROCEDURE ["C"] LabG_MnemonicCharset ( w: Xm.XmLabelGadget ): Xm.XmStringCharSet;
BEGIN
  RETURN w^.label.mnemonicCharset;
END LabG_MnemonicCharset;

PROCEDURE ["C"] LabG_NormalGC ( w: Xm.XmLabelGadget ): Xlib.GC;
BEGIN
  RETURN w^.label.normal_GC;
END LabG_NormalGC;

PROCEDURE ["C"] LabG_Pixmap ( w: Xm.XmLabelGadget ): X.Pixmap;
BEGIN
  RETURN w^.label.pixmap;
END LabG_Pixmap;

PROCEDURE ["C"] LabG_PixmapInsensitive ( w: Xm.XmLabelGadget ): X.Pixmap;
BEGIN
  RETURN w^.label.pixmap_insen;
END LabG_PixmapInsensitive;

PROCEDURE ["C"] LabG_RecomputeSize ( w: Xm.XmLabelGadget ): Xt.Boolean;
BEGIN
  RETURN w^.label.cache^.recompute_size;
END LabG_RecomputeSize;

PROCEDURE ["C"] LabG_Shadow ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.gadget.shadow_thickness;
END LabG_Shadow;

PROCEDURE ["C"] LabG_SkipCallback ( w: Xm.XmLabelGadget ): Xt.Boolean;
BEGIN
  RETURN w^.label.cache^.skipCallback;
END LabG_SkipCallback;

PROCEDURE ["C"] LabG_StringDirection ( w: Xm.XmLabelGadget ): Xm.XmStringDirection;
BEGIN
  RETURN w^.label.cache^.string_direction;
END LabG_StringDirection;

PROCEDURE ["C"] LabG_TextRect ( w: Xm.XmLabelGadget ): Xlib.XRectangle;
BEGIN
  RETURN w^.label.TextRect;
END LabG_TextRect;

PROCEDURE ["C"] LabG_TextRect_height ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.label.TextRect.height;
END LabG_TextRect_height;

PROCEDURE ["C"] LabG_TextRect_width ( w: Xm.XmLabelGadget ): Xt.Dimension;
BEGIN
  RETURN w^.label.TextRect.width;
END LabG_TextRect_width;

PROCEDURE ["C"] LabG_TextRect_x ( w: Xm.XmLabelGadget ): Xt.Position;
BEGIN
  RETURN w^.label.TextRect.x;
END LabG_TextRect_x;

PROCEDURE ["C"] LabG_TextRect_y ( w: Xm.XmLabelGadget ): Xt.Position;
BEGIN
  RETURN w^.label.TextRect.y;
END LabG_TextRect_y;

PROCEDURE ["C"] LabG__acceleratorText ( w: Xm.XmLabelGadget ): Xm._XmString;
BEGIN
  RETURN w^.label._acc_text;
END LabG__acceleratorText;

PROCEDURE ["C"] LabG__label ( w: Xm.XmLabelGadget ): Xm._XmString;
BEGIN
  RETURN w^.label._label;
END LabG__label;

PROCEDURE ["C"] Lab_Accelerator ( w: Xm.XmLabelWidget ): Xt.String;
BEGIN
  RETURN w^.label.accelerator;
END Lab_Accelerator;

(* This field (such as acc_text) doesn`t exists in structure
   Probably Motif Programmers Team BUG :)
PROCEDURE ["C"] Lab_AcceleratorText ( w: Xm.XmLabelWidget ): Xt.String;
BEGIN
  RETURN w^.label.acc_text;
END Lab_AcceleratorText;
*)

PROCEDURE ["C"] Lab_Baseline ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN Xm._XmStringBaseline ( w^.label.font, w^.label._label);
END Lab_Baseline;

PROCEDURE ["C"] Lab_Font ( w: Xm.XmLabelWidget ): Xm.XmFontList;
BEGIN
  RETURN w^.label.font;
END Lab_Font;

PROCEDURE ["C"] Lab_Highlight ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.primitive.highlight_thickness;
END Lab_Highlight;

PROCEDURE ["C"] Lab_IsPixmap ( w: Xm.XmLabelWidget ): Xt.Boolean;
BEGIN
  RETURN w^.label.label_type = Xm.XmPIXMAP;
END Lab_IsPixmap;

PROCEDURE ["C"] Lab_IsText ( w: Xm.XmLabelWidget ): Xt.Boolean;
BEGIN
  RETURN w^.label.label_type = Xm.XmSTRING;
END Lab_IsText;

PROCEDURE ["C"] Lab_MarginBottom ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.label.margin_bottom;
END Lab_MarginBottom;

PROCEDURE ["C"] Lab_MarginHeight ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.label.margin_height;
END Lab_MarginHeight;

PROCEDURE ["C"] Lab_MarginLeft ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.label.margin_left;
END Lab_MarginLeft;

PROCEDURE ["C"] Lab_MarginRight ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.label.margin_right;
END Lab_MarginRight;

PROCEDURE ["C"] Lab_MarginTop ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.label.margin_top;
END Lab_MarginTop;

PROCEDURE ["C"] Lab_MarginWidth ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.label.margin_width;
END Lab_MarginWidth;

PROCEDURE ["C"] Lab_MenuType ( w: Xm.XmLabelWidget ): SHORTCARD;
BEGIN
  RETURN w^.label.menu_type;
END Lab_MenuType;

PROCEDURE ["C"] Lab_Mnemonic ( w: Xm.XmLabelWidget ): X.KeySym;
BEGIN
  RETURN w^.label.mnemonic;
END Lab_Mnemonic;

PROCEDURE ["C"] Lab_Shadow ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.primitive.shadow_thickness;
END Lab_Shadow;

PROCEDURE ["C"] Lab_TextRect_height ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.label.TextRect.height;
END Lab_TextRect_height;

PROCEDURE ["C"] Lab_TextRect_width ( w: Xm.XmLabelWidget ): Xt.Dimension;
BEGIN
  RETURN w^.label.TextRect.width;
END Lab_TextRect_width;

PROCEDURE ["C"] Lab_TextRect_x ( w: Xm.XmLabelWidget ): Xt.Position;
BEGIN
  RETURN w^.label.TextRect.x;
END Lab_TextRect_x;

PROCEDURE ["C"] Lab_TextRect_y ( w: Xm.XmLabelWidget ): Xt.Position;
BEGIN
  RETURN w^.label.TextRect.y;
END Lab_TextRect_y;

PROCEDURE ["C"] MGR_KeyboardList ( m: Xm.XmManagerWidget ): Xm.XmKeyboardDataList;
BEGIN
  RETURN m^.manager.keyboard_list;
END MGR_KeyboardList;

PROCEDURE ["C"] MGR_NumKeyboardEntries ( m: Xm.XmManagerWidget ): SYSTEM.INT16;
BEGIN
  RETURN m^.manager.num_keyboard_entries;
END MGR_NumKeyboardEntries;

PROCEDURE ["C"] MGR_ShadowThickness ( m: Xm.XmManagerWidget ): Xt.Dimension;
BEGIN
  RETURN m^.manager.shadow_thickness;
END MGR_ShadowThickness;

PROCEDURE ["C"] MGR_SizeKeyboardList ( m: Xm.XmManagerWidget ): SYSTEM.INT16;
BEGIN
  RETURN m^.manager.size_keyboard_list;
END MGR_SizeKeyboardList;

PROCEDURE ["C"] XmParentBackgroundGC ( w: Xt.Widget ): Xlib.GC;
VAR
  wp: Xm.XmManagerWidget;
BEGIN
  wp := CAST(Xm.XmManagerWidget,w^.core.parent);
  RETURN wp^.manager.background_GC;
END XmParentBackgroundGC;

PROCEDURE ["C"] XmParentBottomShadowGC ( w: Xt.Widget ): Xlib.GC;
VAR
  wp: Xm.XmManagerWidget;
BEGIN
  wp := CAST(Xm.XmManagerWidget,w^.core.parent);
  RETURN wp^.manager.bottom_shadow_GC;
END XmParentBottomShadowGC;

PROCEDURE ["C"] XmParentHighlightGC ( w: Xt.Widget ): Xlib.GC;
VAR
  wp: Xm.XmManagerWidget;
BEGIN
  wp := CAST(Xm.XmManagerWidget,w^.core.parent);
  RETURN wp^.manager.highlight_GC;
END XmParentHighlightGC;

PROCEDURE ["C"] XmParentTopShadowGC ( w: Xt.Widget ): Xlib.GC;
VAR
  wp: Xm.XmManagerWidget;
BEGIN
  wp := CAST(Xm.XmManagerWidget,w^.core.parent);
  RETURN wp^.manager.top_shadow_GC;
END XmParentTopShadowGC;

PROCEDURE ["C"] MS_FocusPolicy ( m: Xm.XmMenuShellWidget ): SHORTCARD;
BEGIN
  RETURN m^.menu_shell.focus_policy;
END MS_FocusPolicy;

PROCEDURE ["C"] PBG_ActivateCallback ( w: Xm.XmPushButtonGadget ): Xt.XtCallbackList;
BEGIN
  RETURN w^.pushbutton.activate_callback;
END PBG_ActivateCallback;

PROCEDURE ["C"] PBG_ArmCallback ( w: Xm.XmPushButtonGadget ): Xt.XtCallbackList;
BEGIN
  RETURN w^.pushbutton.arm_callback;
END PBG_ArmCallback;

PROCEDURE ["C"] PBG_ArmColor ( w: Xm.XmPushButtonGadget ): Xt.Pixel;
BEGIN
  RETURN w^.pushbutton.cache^.arm_color;
END PBG_ArmColor;

PROCEDURE ["C"] PBG_ArmPixmap ( w: Xm.XmPushButtonGadget ): X.Pixmap;
BEGIN
  RETURN w^.pushbutton.cache^.arm_pixmap;
END PBG_ArmPixmap;

PROCEDURE ["C"] PBG_Armed ( w: Xm.XmPushButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.pushbutton.armed;
END PBG_Armed;

PROCEDURE ["C"] PBG_BackgroundGc ( w: Xm.XmPushButtonGadget ): Xlib.GC;
BEGIN
  RETURN w^.pushbutton.cache^.background_gc;
END PBG_BackgroundGc;

PROCEDURE ["C"] PBG_Cache ( w: Xm.XmPushButtonGadget ): Xm.XmPushButtonGCache;
BEGIN
  RETURN w^.pushbutton.cache;
END PBG_Cache;

PROCEDURE ["C"] PBG_ClassCachePart ( w: Xm.XmPushButtonGadgetClass ): Xm.XmCacheClassPartPtr;
BEGIN
  w := CAST(Xm.XmPushButtonGadgetClass,Xm.xmPushButtonGadgetClass);
  RETURN w^.gadget_class.cache_part;
END PBG_ClassCachePart;

PROCEDURE ["C"] PBG_ClickCount ( w: Xm.XmPushButtonGadget ): INTEGER;
BEGIN
  RETURN w^.pushbutton.click_count;
END PBG_ClickCount;

PROCEDURE ["C"] PBG_Compatible ( w: Xm.XmPushButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.pushbutton.compatible;
END PBG_Compatible;

PROCEDURE ["C"] PBG_DefaultButtonShadowThickness ( w: Xm.XmPushButtonGadget ): Xt.Dimension;
BEGIN
  RETURN w^.pushbutton.cache^.default_button_shadow_thickness;
END PBG_DefaultButtonShadowThickness;

PROCEDURE ["C"] PBG_DisarmCallback ( w: Xm.XmPushButtonGadget ): Xt.XtCallbackList;
BEGIN
  RETURN w^.pushbutton.disarm_callback;
END PBG_DisarmCallback;

PROCEDURE ["C"] PBG_FillGc ( w: Xm.XmPushButtonGadget ): Xlib.GC;
BEGIN
  RETURN w^.pushbutton.cache^.fill_gc;
END PBG_FillGc;

PROCEDURE ["C"] PBG_FillOnArm ( w: Xm.XmPushButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.pushbutton.cache^.fill_on_arm;
END PBG_FillOnArm;

PROCEDURE ["C"] PBG_MultiClick ( w: Xm.XmPushButtonGadget ): SHORTCARD;
BEGIN
  RETURN w^.pushbutton.cache^.multiClick;
END PBG_MultiClick;

PROCEDURE ["C"] PBG_ShowAsDefault ( w: Xm.XmPushButtonGadget ): Xt.Dimension;
BEGIN
  RETURN w^.pushbutton.show_as_default;
END PBG_ShowAsDefault;

PROCEDURE ["C"] PBG_Timer ( w: Xm.XmPushButtonGadget ): Xt.XtIntervalId;
BEGIN
  RETURN w^.pushbutton.cache^.timer;
END PBG_Timer;

PROCEDURE ["C"] PBG_UnarmPixmap ( w: Xm.XmPushButtonGadget ): X.Pixmap;
BEGIN
  RETURN w^.pushbutton.cache^.unarm_pixmap;
END PBG_UnarmPixmap;

PROCEDURE ["C"] Prim_HaveTraversal ( w: Xm.XmPrimitiveWidget ): Xt.Boolean;
BEGIN
  RETURN w^.primitive.have_traversal;
END Prim_HaveTraversal;

PROCEDURE ["C"] Prim_ShadowThickness ( w: Xm.XmPrimitiveWidget ): Xt.Dimension;
BEGIN
  RETURN w^.primitive.shadow_thickness;
END Prim_ShadowThickness;

PROCEDURE ["C"] PCEPTR ( wc: Xm.XmPrimitiveWidgetClass ): Xm.XmPrimitiveClassExtPtr;
BEGIN
  RETURN ADR(wc^.primitive_class.extension);
END PCEPTR;

PROCEDURE ["C"] _XmGetPrimitiveClassExtPtr ( wc: Xm.XmPrimitiveWidgetClass;
                                             owner: Xrm.XrmQuark ): Xm.XmPrimitiveClassExtPtr;
VAR
  wce: Xm.XmPrimitiveClassExt;
  gce: Xm.XmGenericClassExt;
BEGIN
  wce := CAST(Xm.XmPrimitiveClassExt,wc^.primitive_class.extension);
  IF (wc^.primitive_class.extension # NIL) AND
     (wce^.record_type = owner) THEN
    RETURN ADR(wc^.primitive_class.extension);
  ELSE
    gce := CAST(Xm.XmGenericClassExt,wc^.primitive_class.extension);
    RETURN CAST(Xm.XmPrimitiveClassExtPtr,
                Xm._XmGetClassExtensionPtr(gce,owner));
  END
END _XmGetPrimitiveClassExtPtr;

PROCEDURE ["C"] RC_AdjLast ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.adjust_last;
END RC_AdjLast;

PROCEDURE ["C"] RC_AdjMargin ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.adjust_margin;
END RC_AdjMargin;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_BeingArmed ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.armed OR
         ((CAST(BITSET,m^.row_column.armed) *
           CAST(BITSET,Xm.XmRC_BEING_ARMED_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.armed & XmRC_BEING_ARMED_BIT; *)
END RC_BeingArmed;

PROCEDURE ["C"] RC_Boxes ( m: Xm.XmRowColumnWidget ): Xm.XmRCKidGeometry;
BEGIN
  RETURN m^.row_column.boxes;
END RC_Boxes;

PROCEDURE ["C"] RC_CascadeBtn ( m: Xm.XmRowColumnWidget ): Xt.Widget;
BEGIN
  RETURN m^.row_column.cascadeBtn;
END RC_CascadeBtn;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_DoExpose ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.armed OR
         ((CAST(BITSET,m^.row_column.armed) *
           CAST(BITSET,Xm.XmRC_EXPOSE_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.armed & XmRC_EXPOSE_BIT; *)
END RC_DoExpose;

PROCEDURE ["C"] RC_DoMarginAdjust ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.adjust_margin;
END RC_DoMarginAdjust;

PROCEDURE ["C"] RC_EntryAlignment ( m: Xm.XmRowColumnWidget ): SHORTCARD;
BEGIN
  RETURN m^.row_column.entry_alignment;
END RC_EntryAlignment;

PROCEDURE ["C"] RC_EntryBorder ( m: Xm.XmRowColumnWidget ): Xt.Dimension;
BEGIN
  RETURN m^.row_column.entry_border;
END RC_EntryBorder;

PROCEDURE ["C"] RC_EntryClass ( m: Xm.XmRowColumnWidget ): Xt.WidgetClass;
BEGIN
  RETURN m^.row_column.entry_class;
END RC_EntryClass;

PROCEDURE ["C"] RC_EntryVerticalAlignment ( m: Xm.XmRowColumnWidget ): SHORTCARD;
BEGIN
  RETURN m^.row_column.entry_vertical_alignment;
END RC_EntryVerticalAlignment;

PROCEDURE ["C"] RC_Entry_cb ( m: Xm.XmRowColumnWidget ): Xt.XtCallbackList;
BEGIN
  RETURN m^.row_column.entry_callback;
END RC_Entry_cb;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_FromInit ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.to_state OR
         ((CAST(BITSET,m^.row_column.to_state) *
           CAST(BITSET,Xm.XmTO_FROM_INIT_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.to_state & XmTO_FROM_INIT_BIT *)
END RC_FromInit;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_FromResize ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.armed OR
         ((CAST(BITSET,m^.row_column.armed) *
           CAST(BITSET,Xm.XmRC_FROM_RESIZE_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.armed & XmRC_FROM_RESIZE_BIT *)
END RC_FromResize;

PROCEDURE ["C"] RC_HelpPb ( m: Xm.XmRowColumnWidget ): Xt.Widget;
BEGIN
  RETURN m^.row_column.help_pushbutton;
END RC_HelpPb;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_IsArmed ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.armed OR
         ((CAST(BITSET,m^.row_column.armed) *
           CAST(BITSET,Xm.XmRC_ARMED_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.armed & XmRC_ARMED_BIT *)
END RC_IsArmed;

PROCEDURE ["C"] RC_IsHomogeneous ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.homogeneous;
END RC_IsHomogeneous;

PROCEDURE ["C"] RC_Map_cb ( m: Xm.XmRowColumnWidget ): Xt.XtCallbackList;
BEGIN
  RETURN m^.row_column.map_callback;
END RC_Map_cb;

PROCEDURE ["C"] RC_MarginH ( m: Xm.XmRowColumnWidget ): Xt.Dimension;
BEGIN
  RETURN m^.row_column.margin_height;
END RC_MarginH;

PROCEDURE ["C"] RC_MarginW ( m: Xm.XmRowColumnWidget ): Xt.Dimension;
BEGIN
  RETURN m^.row_column.margin_width;
END RC_MarginW;

PROCEDURE ["C"] RC_MemWidget ( m: Xm.XmRowColumnWidget ): Xt.Widget;
BEGIN
  RETURN m^.row_column.memory_subwidget;
END RC_MemWidget;

PROCEDURE ["C"] RC_MenuAccelerator ( m: Xm.XmRowColumnWidget ): Xt.String;
BEGIN
  RETURN m^.row_column.menu_accelerator;
END RC_MenuAccelerator;

PROCEDURE ["C"] RC_MenuPost ( m: Xm.XmRowColumnWidget ): Xt.String;
BEGIN
  RETURN m^.row_column.menuPost;
END RC_MenuPost;

PROCEDURE ["C"] RC_Mnemonic ( m: Xm.XmRowColumnWidget ): X.KeySym;
BEGIN
  RETURN m^.row_column.mnemonic;
END RC_Mnemonic;

PROCEDURE ["C"] RC_MnemonicCharSet ( m: Xm.XmRowColumnWidget ): Xm.XmStringCharSet;
BEGIN
  RETURN m^.row_column.mnemonicCharSet;
END RC_MnemonicCharSet;

PROCEDURE ["C"] RC_NCol ( m: Xm.XmRowColumnWidget ): SYSTEM.INT16;
BEGIN
  RETURN m^.row_column.num_columns;
END RC_NCol;

PROCEDURE ["C"] RC_OldFocusPolicy ( m: Xm.XmRowColumnWidget ): SHORTCARD;
BEGIN
  RETURN m^.row_column.oldFocusPolicy;
END RC_OldFocusPolicy;

PROCEDURE ["C"] RC_OptionLabel ( m: Xm.XmRowColumnWidget ): Xm.XmString;
BEGIN
  RETURN m^.row_column.option_label;
END RC_OptionLabel;

PROCEDURE ["C"] RC_OptionSubMenu ( m: Xm.XmRowColumnWidget ): Xt.Widget;
BEGIN
  RETURN m^.row_column.option_submenu;
END RC_OptionSubMenu;

PROCEDURE ["C"] RC_Orientation ( m: Xm.XmRowColumnWidget ): SHORTCARD;
BEGIN
  RETURN m^.row_column.orientation;
END RC_Orientation;

PROCEDURE ["C"] RC_Packing ( m: Xm.XmRowColumnWidget ): SHORTCARD;
BEGIN
  RETURN m^.row_column.packing;
END RC_Packing;

PROCEDURE ["C"] RC_ParentShell ( m: Xm.XmRowColumnWidget ): Xt.Widget;
BEGIN
  RETURN m^.row_column.ParentShell;
END RC_ParentShell;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_PoppingDown ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.armed OR
         ((CAST(BITSET,m^.row_column.armed) *
           CAST(BITSET,Xm.XmRC_POPPING_DOWN_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.armed & XmRC_POPPING_DOWN_BIT *)
END RC_PoppingDown;

PROCEDURE ["C"] RC_PopupEnabled ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.popup_enabled;
END RC_PopupEnabled;

PROCEDURE ["C"] RC_PopupPosted ( m: Xm.XmRowColumnWidget ): Xt.Widget;
BEGIN
  RETURN m^.row_column.popupPosted;
END RC_PopupPosted;

PROCEDURE ["C"] RC_PostButton ( m: Xm.XmRowColumnWidget ): CARDINAL;
BEGIN
  RETURN m^.row_column.postButton;
END RC_PostButton;

PROCEDURE ["C"] RC_PostEventType ( m: Xm.XmRowColumnWidget ): INTEGER;
BEGIN
  RETURN m^.row_column.postEventType;
END RC_PostEventType;

PROCEDURE ["C"] RC_PostModifiers ( m: Xm.XmRowColumnWidget ): CARDINAL;
BEGIN
  RETURN m^.row_column.postModifiers;
END RC_PostModifiers;

PROCEDURE ["C"] RC_RadioAlwaysOne ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.radio_one;
END RC_RadioAlwaysOne;

PROCEDURE ["C"] RC_RadioBehavior ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.radio;
END RC_RadioBehavior;

PROCEDURE ["C"] RC_ResizeHeight ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.resize_height;
END RC_ResizeHeight;

PROCEDURE ["C"] RC_ResizeWidth ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.resize_width;
END RC_ResizeWidth;

PROCEDURE ["C"] RC_SetArmed ( m: Xm.XmRowColumnWidget;
                              v: BOOLEAN );
BEGIN
  RC_SetBit (m^.row_column.armed, Xm.XmRC_ARMED_BIT, v);
END RC_SetArmed;

PROCEDURE ["C"] RC_SetBeingArmed ( m: Xm.XmRowColumnWidget;
                                   v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.armed, Xm.XmRC_BEING_ARMED_BIT, v);
END RC_SetBeingArmed;

PROCEDURE ["C"] RC_SetBit ( VAR byte: Xt.Boolean;
                            bit: SET8;
                            v: BOOLEAN );
VAR
  my_bitset: SET8;
BEGIN
(* #define RC_SetBit(byte,bit,v)  byte = (byte & (~bit)) | (v ? bit : 0) *)
  my_bitset := CAST(SET8,byte);
  IF v THEN
    my_bitset := my_bitset + bit;
  ELSE
    my_bitset := my_bitset - bit;
  END;
  byte:=CAST(Xt.Boolean,my_bitset);
END RC_SetBit;

PROCEDURE ["C"] RC_SetExpose ( m: Xm.XmRowColumnWidget;
                               v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.armed, Xm.XmRC_EXPOSE_BIT, v);
END RC_SetExpose;

PROCEDURE ["C"] RC_SetFromInit ( m: Xm.XmRowColumnWidget;
                                 v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.to_state, Xm.XmTO_FROM_INIT_BIT,v);
END RC_SetFromInit;

PROCEDURE ["C"] RC_SetFromResize ( m: Xm.XmRowColumnWidget;
                                   v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.armed, Xm.XmRC_FROM_RESIZE_BIT,v);
END RC_SetFromResize;

PROCEDURE ["C"] RC_SetPoppingDown ( m: Xm.XmRowColumnWidget;
                                    v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.armed, Xm.XmRC_POPPING_DOWN_BIT,v);
END RC_SetPoppingDown;

PROCEDURE ["C"] RC_SetTearOffActive ( m: Xm.XmRowColumnWidget;
                                      v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.to_state, Xm.XmTO_ACTIVE_BIT,v);
END RC_SetTearOffActive;

PROCEDURE ["C"] RC_SetTearOffDirty ( m: Xm.XmRowColumnWidget;
                                     v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.to_state, Xm.XmTO_VISUAL_DIRTY_BIT,v);
END RC_SetTearOffDirty;

PROCEDURE ["C"] RC_SetTornOff ( m: Xm.XmRowColumnWidget;
                                v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.to_state, Xm.XmTO_TORN_OFF_BIT,v);
END RC_SetTornOff;

PROCEDURE ["C"] RC_SetWidgetMoved ( m: Xm.XmRowColumnWidget;
                                    v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.armed, Xm.XmRC_WIDGET_MOVED_BIT,v);
END RC_SetWidgetMoved;

PROCEDURE ["C"] RC_SetWindowMoved ( m: Xm.XmRowColumnWidget;
                                    v: BOOLEAN );
BEGIN
  RC_SetBit(m^.row_column.armed, Xm.XmRC_WINDOW_MOVED_BIT,v);
END RC_SetWindowMoved;

PROCEDURE ["C"] RC_Spacing ( m: Xm.XmRowColumnWidget ): Xt.Dimension;
BEGIN
  RETURN m^.row_column.spacing;
END RC_Spacing;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_TearOffActive ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.to_state OR
         ((CAST(BITSET,m^.row_column.to_state) *
           CAST(BITSET,Xm.XmTO_ACTIVE_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.to_state & XmTO_ACTIVE_BIT *)
END RC_TearOffActive;

PROCEDURE ["C"] RC_TearOffControl ( m: Xm.XmRowColumnWidget ): Xt.Widget;
BEGIN
  RETURN m^.row_column.tear_off_control;
END RC_TearOffControl;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_TearOffDirty ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.to_state OR
         ((CAST(BITSET,m^.row_column.to_state) *
           CAST(BITSET,Xm.XmTO_VISUAL_DIRTY_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.to_state & XmTO_VISUAL_DIRTY_BIT *)
END RC_TearOffDirty;

PROCEDURE ["C"] RC_TearOffModel ( m: Xm.XmRowColumnWidget ): SHORTCARD;
BEGIN
  RETURN m^.row_column.TearOffModel;
END RC_TearOffModel;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_TornOff ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.to_state OR
         ((CAST(BITSET,m^.row_column.to_state) *
           CAST(BITSET,Xm.XmTO_TORN_OFF_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.to_state & XmTO_TORN_OFF_BIT *)
END RC_TornOff;

PROCEDURE ["C"] RC_Type ( m: Xm.XmRowColumnWidget ): SHORTCARD;
BEGIN
  RETURN m^.row_column.type;
END RC_Type;

PROCEDURE ["C"] RC_Unmap_cb ( m: Xm.XmRowColumnWidget ): Xt.XtCallbackList;
BEGIN
  RETURN m^.row_column.unmap_callback;
END RC_Unmap_cb;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_WidgetHasMoved ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.armed OR
         ((CAST(BITSET,m^.row_column.armed) *
           CAST(BITSET,Xm.XmRC_WIDGET_MOVED_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.armed & XmRC_WIDGET_MOVED_BIT *)
END RC_WidgetHasMoved;

(* Not adecvate - see comment after RETURN *)
PROCEDURE ["C"] RC_WindowHasMoved ( m: Xm.XmRowColumnWidget ): Xt.Boolean;
BEGIN
  RETURN m^.row_column.armed OR
         ((CAST(BITSET,m^.row_column.armed) *
           CAST(BITSET,Xm.XmRC_WINDOW_MOVED_BIT)) # CAST(BITSET,0));
  (* in C is return m->row_column.armed & XmRC_WINDOW_MOVED_BIT *)
END RC_WindowHasMoved;

PROCEDURE ["C"] RC_popupMenuClick ( m: Xm.XmRowColumnWidget ): SHORTCARD;
BEGIN
  RETURN m^.row_column.popup_menu_click;
END RC_popupMenuClick;

PROCEDURE ["C"] SB_AddingSelWidgets ( w: Xm.XmSelectionBoxWidget ): Xt.Boolean;
BEGIN
  RETURN w^.selection_box.adding_sel_widgets;
END SB_AddingSelWidgets;

PROCEDURE ["C"] SB_ApplyButton ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.selection_box.apply_button;
END SB_ApplyButton;

PROCEDURE ["C"] SB_ButtonFontList ( w: Xm.XmSelectionBoxWidget ): Xm.XmFontList;
BEGIN
  RETURN w^.bulletin_board.button_font_list;
END SB_ButtonFontList;

PROCEDURE ["C"] SB_CancelButton ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.bulletin_board.cancel_button;
END SB_CancelButton;

PROCEDURE ["C"] SB_DefaultButton ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.bulletin_board.default_button;
END SB_DefaultButton;

PROCEDURE ["C"] SB_HelpButton ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.selection_box.help_button;
END SB_HelpButton;

PROCEDURE ["C"] SB_LabelFontList ( w: Xm.XmSelectionBoxWidget ): Xm.XmFontList;
BEGIN
  RETURN w^.bulletin_board.label_font_list;
END SB_LabelFontList;

PROCEDURE ["C"] SB_List ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.selection_box.list;
END SB_List;

PROCEDURE ["C"] SB_ListItemCount ( w: Xm.XmSelectionBoxWidget ): INTEGER;
BEGIN
  RETURN w^.selection_box.list_item_count;
END SB_ListItemCount;

PROCEDURE ["C"] SB_ListLabel ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.selection_box.list_label;
END SB_ListLabel;

PROCEDURE ["C"] SB_ListSelectedItemPosition ( w: Xm.XmSelectionBoxWidget ): INTEGER;
BEGIN
  RETURN w^.selection_box.list_selected_item_position;
END SB_ListSelectedItemPosition;

PROCEDURE ["C"] SB_ListVisibleItemCount ( w: Xm.XmSelectionBoxWidget ): INTEGER;
BEGIN
  RETURN w^.selection_box.list_visible_item_count;
END SB_ListVisibleItemCount;

PROCEDURE ["C"] SB_MarginHeight ( w: Xm.XmSelectionBoxWidget ): Xt.Dimension;
BEGIN
  RETURN w^.bulletin_board.margin_height;
END SB_MarginHeight;

PROCEDURE ["C"] SB_MarginWidth ( w: Xm.XmSelectionBoxWidget ): Xt.Dimension;
BEGIN
  RETURN w^.bulletin_board.margin_width;
END SB_MarginWidth;

PROCEDURE ["C"] SB_MinimizeButtons ( w: Xm.XmSelectionBoxWidget ): Xt.Boolean;
BEGIN
  RETURN w^.selection_box.minimize_buttons;
END SB_MinimizeButtons;

PROCEDURE ["C"] SB_MustMatch ( w: Xm.XmSelectionBoxWidget ): Xt.Boolean;
BEGIN
  RETURN w^.selection_box.must_match;
END SB_MustMatch;

PROCEDURE ["C"] SB_OkButton ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.selection_box.ok_button;
END SB_OkButton;

PROCEDURE ["C"] SB_SelectionLabel ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.selection_box.selection_label;
END SB_SelectionLabel;

PROCEDURE ["C"] SB_Separator ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.selection_box.separator;
END SB_Separator;

PROCEDURE ["C"] SB_StringDirection ( w: Xm.XmSelectionBoxWidget ): Xm.XmStringDirection;
BEGIN
  RETURN w^.manager.string_direction;
END SB_StringDirection;

PROCEDURE ["C"] SB_Text ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.selection_box.text;
END SB_Text;

PROCEDURE ["C"] SB_TextAccelerators ( w: Xm.XmSelectionBoxWidget ): Xt.XtAccelerators;
BEGIN
  RETURN w^.selection_box.text_accelerators;
END SB_TextAccelerators;

PROCEDURE ["C"] SB_TextColumns ( w: Xm.XmSelectionBoxWidget ): SYSTEM.INT16;
BEGIN
  RETURN w^.selection_box.text_columns;
END SB_TextColumns;

PROCEDURE ["C"] SB_TextFontList ( w: Xm.XmSelectionBoxWidget ): Xm.XmFontList;
BEGIN
  RETURN w^.bulletin_board.text_font_list;
END SB_TextFontList;

PROCEDURE ["C"] SB_WorkArea ( w: Xm.XmSelectionBoxWidget ): Xt.Widget;
BEGIN
  RETURN w^.selection_box.work_area;
END SB_WorkArea;

PROCEDURE ["C"] SEPG_Cache ( w: Xm.XmSeparatorGadget ): Xm.XmSeparatorGCache;
BEGIN
  RETURN w^.separator.cache;
END SEPG_Cache;

PROCEDURE ["C"] SEPG_ClassCachePart ( w: Xm.XmSeparatorGadgetClass ): Xm.XmCacheClassPartPtr;
BEGIN
  w := CAST(Xm.XmSeparatorGadgetClass,Xm.xmSeparatorGadgetClass);
  RETURN w^.gadget_class.cache_part;
END SEPG_ClassCachePart;

PROCEDURE ["C"] SEPG_Margin ( w: Xm.XmSeparatorGadget ): Xt.Dimension;
BEGIN
  RETURN w^.separator.cache^.margin;
END SEPG_Margin;

PROCEDURE ["C"] SEPG_Orientation ( w: Xm.XmSeparatorGadget ): SHORTCARD;
BEGIN
  RETURN w^.separator.cache^.orientation;
END SEPG_Orientation;

PROCEDURE ["C"] SEPG_SeparatorGC ( w: Xm.XmSeparatorGadget ): Xlib.GC;
BEGIN
  RETURN w^.separator.cache^.separator_GC;
END SEPG_SeparatorGC;

PROCEDURE ["C"] SEPG_SeparatorType ( w: Xm.XmSeparatorGadget ): SHORTCARD;
BEGIN
  RETURN w^.separator.cache^.separator_type;
END SEPG_SeparatorType;

PROCEDURE ["C"] ShouldWordWrap ( data: Xm.OutputData;
                                 widget: Xm.XmTextWidget ): Xt.Boolean;
BEGIN
  RETURN data^.wordwrap AND
         ( NOT
           ( data^.scrollhorizontal AND
             ( Xt.XtClass(widget^.core.parent) =
               Xm.xmScrolledWindowWidgetClass)
           )
         ) AND
         (widget^.text.edit_mode <> Xm.XmSINGLE_LINE_EDIT) AND
         (NOT data^.resizewidth);
END ShouldWordWrap;

PROCEDURE ["C"] GetSrc ( widget: Xm.XmTextWidget ): Xm.XmTextSource;
BEGIN
  RETURN widget^.text.source;
END GetSrc;

PROCEDURE ["C"] TBG_ArmCB ( w: Xm.XmToggleButtonGadget ): Xt.XtCallbackList;
BEGIN
  RETURN w^.toggle.arm_CB;
END TBG_ArmCB;

PROCEDURE ["C"] TBG_Armed ( w: Xm.XmToggleButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.toggle.Armed;
END TBG_Armed;

PROCEDURE ["C"] TBG_BackgroundGC ( w: Xm.XmToggleButtonGadget ): Xlib.GC;
BEGIN
  RETURN w^.toggle.cache^.background_gc;
END TBG_BackgroundGC;

PROCEDURE ["C"] TBG_Cache ( w: Xm.XmToggleButtonGadget ): Xm.XmToggleButtonGCache;
BEGIN
  RETURN w^.toggle.cache;
END TBG_Cache;

PROCEDURE ["C"] TBG_ClassCachePart ( w: Xm.XmToggleButtonGadgetClass ): Xm.XmCacheClassPartPtr;
BEGIN
  w := CAST(Xm.XmToggleButtonGadgetClass,Xm.xmToggleButtonGadgetClass);
  RETURN w^.gadget_class.cache_part;
END TBG_ClassCachePart;

PROCEDURE ["C"] TBG_DisarmCB ( w: Xm.XmToggleButtonGadget ): Xt.XtCallbackList;
BEGIN
  RETURN w^.toggle.disarm_CB;
END TBG_DisarmCB;

PROCEDURE ["C"] TBG_FillOnSelect ( w: Xm.XmToggleButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.toggle.cache^.fill_on_select;
END TBG_FillOnSelect;

PROCEDURE ["C"] TBG_IndOn ( w: Xm.XmToggleButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.toggle.cache^.ind_on;
END TBG_IndOn;

PROCEDURE ["C"] TBG_IndType ( w: Xm.XmToggleButtonGadget ): SHORTCARD;
BEGIN
  RETURN w^.toggle.cache^.ind_type;
END TBG_IndType;

PROCEDURE ["C"] TBG_IndicatorDim ( w: Xm.XmToggleButtonGadget ): Xt.Dimension;
BEGIN
  RETURN w^.toggle.cache^.indicator_dim;
END TBG_IndicatorDim;

PROCEDURE ["C"] TBG_IndicatorSet ( w: Xm.XmToggleButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.toggle.indicator_set;
END TBG_IndicatorSet;

PROCEDURE ["C"] TBG_InsenPixmap ( w: Xm.XmToggleButtonGadget ): X.Pixmap;
BEGIN
  RETURN w^.toggle.cache^.insen_pixmap;
END TBG_InsenPixmap;

PROCEDURE ["C"] TBG_OnPixmap ( w: Xm.XmToggleButtonGadget ): X.Pixmap;
BEGIN
  RETURN w^.toggle.cache^.on_pixmap;
END TBG_OnPixmap;

PROCEDURE ["C"] TBG_SelectColor ( w: Xm.XmToggleButtonGadget ): Xt.Pixel;
BEGIN
  RETURN w^.toggle.cache^.select_color;
END TBG_SelectColor;

PROCEDURE ["C"] TBG_SelectGC ( w: Xm.XmToggleButtonGadget ): Xlib.GC;
BEGIN
  RETURN w^.toggle.cache^.select_GC;
END TBG_SelectGC;

PROCEDURE ["C"] TBG_Set ( w: Xm.XmToggleButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.toggle.set;
END TBG_Set;

PROCEDURE ["C"] TBG_Spacing ( w: Xm.XmToggleButtonGadget ): Xt.Dimension;
BEGIN
  RETURN w^.toggle.cache^.spacing;
END TBG_Spacing;

PROCEDURE ["C"] TBG_ValueChangedCB ( w: Xm.XmToggleButtonGadget ): Xt.XtCallbackList;
BEGIN
  RETURN w^.toggle.value_changed_CB;
END TBG_ValueChangedCB;

PROCEDURE ["C"] TBG_Visible ( w: Xm.XmToggleButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.toggle.cache^.visible;
END TBG_Visible;

PROCEDURE ["C"] TBG_VisualSet ( w: Xm.XmToggleButtonGadget ): Xt.Boolean;
BEGIN
  RETURN w^.toggle.visual_set;
END TBG_VisualSet;

PROCEDURE ["C"] TextF_ActivateCallback ( tfg: Xm.XmTextFieldWidget ): Xt.XtCallbackList;
BEGIN
  RETURN tfg^.text.activate_callback;
END TextF_ActivateCallback;

PROCEDURE ["C"] TextF_BlinkRate ( tfg: Xm.XmTextFieldWidget ): INTEGER;
BEGIN
  RETURN tfg^.text.blink_rate;
END TextF_BlinkRate;

PROCEDURE ["C"] TextF_Columns ( tfg: Xm.XmTextFieldWidget ): SYSTEM.INT16;
BEGIN
  RETURN tfg^.text.columns;
END TextF_Columns;

PROCEDURE ["C"] TextF_CursorPosition ( tfg: Xm.XmTextFieldWidget ): Xm.XmTextPosition;
BEGIN
  RETURN tfg^.text.cursor_position;
END TextF_CursorPosition;

PROCEDURE ["C"] TextF_CursorPositionVisible ( tfg: Xm.XmTextFieldWidget ): Xt.Boolean;
BEGIN
  RETURN tfg^.text.cursor_position_visible;
END TextF_CursorPositionVisible;

PROCEDURE ["C"] TextF_Editable ( tfg: Xm.XmTextFieldWidget ): Xt.Boolean;
BEGIN
  RETURN tfg^.text.editable;
END TextF_Editable;

PROCEDURE ["C"] TextF_FocusCallback ( tfg: Xm.XmTextFieldWidget ): Xt.XtCallbackList;
BEGIN
  RETURN tfg^.text.focus_callback;
END TextF_FocusCallback;

PROCEDURE ["C"] TextF_Font ( tfg: Xm.XmTextFieldWidget ): Xlib.PtrXFontStruct;
BEGIN
  RETURN tfg^.text.font;
END TextF_Font;

PROCEDURE ["C"] TextF_FontAscent ( tfg: Xm.XmTextFieldWidget ): Xt.Dimension;
BEGIN
  RETURN tfg^.text.font_ascent;
END TextF_FontAscent;

PROCEDURE ["C"] TextF_FontDescent ( tfg: Xm.XmTextFieldWidget ): Xt.Dimension;
BEGIN
  RETURN tfg^.text.font_descent;
END TextF_FontDescent;

PROCEDURE ["C"] TextF_FontList ( tfg: Xm.XmTextFieldWidget ): Xm.XmFontList;
BEGIN
  RETURN tfg^.text.font_list;
END TextF_FontList;

PROCEDURE ["C"] TextF_LosingFocusCallback ( tfg: Xm.XmTextFieldWidget ): Xt.XtCallbackList;
BEGIN
  RETURN tfg^.text.losing_focus_callback;
END TextF_LosingFocusCallback;

PROCEDURE ["C"] TextF_MarginHeight ( tfg: Xm.XmTextFieldWidget ): Xt.Dimension;
BEGIN
  RETURN tfg^.text.margin_height;
END TextF_MarginHeight;

PROCEDURE ["C"] TextF_MarginWidth ( tfg: Xm.XmTextFieldWidget ): Xt.Dimension;
BEGIN
  RETURN tfg^.text.margin_width;
END TextF_MarginWidth;

PROCEDURE ["C"] TextF_MaxLength ( tfg: Xm.XmTextFieldWidget ): INTEGER;
BEGIN
  RETURN tfg^.text.max_length;
END TextF_MaxLength;

PROCEDURE ["C"] TextF_ModifyVerifyCallback ( tfg: Xm.XmTextFieldWidget ): Xt.XtCallbackList;
BEGIN
  RETURN tfg^.text.modify_verify_callback;
END TextF_ModifyVerifyCallback;

PROCEDURE ["C"] TextF_ModifyVerifyCallbackWcs ( tfg: Xm.XmTextFieldWidget ): Xt.XtCallbackList;
BEGIN
  RETURN tfg^.text.wcs_modify_verify_callback;
END TextF_ModifyVerifyCallbackWcs;

PROCEDURE ["C"] TextF_MotionVerifyCallback ( tfg: Xm.XmTextFieldWidget ): Xt.XtCallbackList;
BEGIN
  RETURN tfg^.text.motion_verify_callback;
END TextF_MotionVerifyCallback;

PROCEDURE ["C"] TextF_PendingDelete ( tfg: Xm.XmTextFieldWidget ): Xt.Boolean;
BEGIN
  RETURN tfg^.text.pending_delete;
END TextF_PendingDelete;

PROCEDURE ["C"] TextF_ResizeWidth ( tfg: Xm.XmTextFieldWidget ): Xt.Boolean;
BEGIN
  RETURN tfg^.text.resize_width;
END TextF_ResizeWidth;

PROCEDURE ["C"] TextF_SelectionArray ( tfg: Xm.XmTextFieldWidget ): Xm.XmTextScanTypeList;
BEGIN
  RETURN tfg^.text.selection_array;
END TextF_SelectionArray;

PROCEDURE ["C"] TextF_SelectionArrayCount ( tfg: Xm.XmTextFieldWidget ): INTEGER;
BEGIN
  RETURN tfg^.text.selection_array_count;
END TextF_SelectionArrayCount;

PROCEDURE ["C"] TextF_Threshold ( tfg: Xm.XmTextFieldWidget ): INTEGER;
BEGIN
  RETURN tfg^.text.threshold;
END TextF_Threshold;

PROCEDURE ["C"] TextF_UseFontSet ( tfg: Xm.XmTextFieldWidget ): Xt.Boolean;
BEGIN
  RETURN tfg^.text.have_fontset;
END TextF_UseFontSet;

PROCEDURE ["C"] TextF_Value ( tfg: Xm.XmTextFieldWidget ): Xt.String;
BEGIN
  RETURN tfg^.text.value;
END TextF_Value;

PROCEDURE ["C"] TextF_ValueChangedCallback ( tfg: Xm.XmTextFieldWidget ): Xt.XtCallbackList;
BEGIN
  RETURN tfg^.text.value_changed_callback;
END TextF_ValueChangedCallback;

PROCEDURE ["C"] TextF_WcValue ( tfg: Xm.XmTextFieldWidget ): Xlib.Ptrwchar_t;
BEGIN
  RETURN tfg^.text.wc_value;
END TextF_WcValue;

PROCEDURE ["C"] XM_WM_PROTOCOL_ATOM ( shell: Xt.Widget ): X.Atom;
BEGIN
  RETURN Xm.XmInternAtom(shell^.core.screen^.display,
                         "WM_PROTOCOLS", FALSE);
END XM_WM_PROTOCOL_ATOM;

PROCEDURE ["C"] XmActivateWMProtocol ( shell: Xt.Widget;
                                       protocol: X.Atom );
BEGIN
  Xm.XmActivateProtocol(shell, Xm.XmInternAtom(shell^.core.screen^.display,
                                               "WM_PROTOCOLS", FALSE),
                        protocol);
END XmActivateWMProtocol;

PROCEDURE ["C"] XmAddWMProtocolCallback ( shell: Xt.Widget;
                                          protocol: X.Atom;
                                          callback: Xt.XtCallbackProc;
                                          closure: Xt.XtPointer );
BEGIN
  Xm.XmAddProtocolCallback(shell,
                           Xm.XmInternAtom(shell^.core.screen^.display,
                                           "WM_PROTOCOLS", FALSE),
                           protocol,
                           callback,
                           closure);
END XmAddWMProtocolCallback;

PROCEDURE ["C"] XmAddWMProtocols ( shell: Xt.Widget;
                                   protocols: X.PAAtom;
                                   num_protocols: Xt.Cardinal );
BEGIN
  Xm.XmAddProtocols(shell,
                    Xm.XmInternAtom(shell^.core.screen^.display,
                                    "WM_PROTOCOLS", FALSE),
                    protocols,
                    num_protocols);
END XmAddWMProtocols;

PROCEDURE ["C"] XmDeactivateWMProtocol ( shell: Xt.Widget;
                                         protocol: X.Atom );
BEGIN
  Xm.XmDeactivateProtocol(shell,
                          Xm.XmInternAtom(shell^.core.screen^.display,
                                          "WM_PROTOCOLS", FALSE),
                          protocol);
END XmDeactivateWMProtocol;

PROCEDURE ["C"] XmRemoveWMProtocolCallback ( shell: Xt.Widget;
                                             protocol: X.Atom;
                                             callback: Xt.XtCallbackProc;
                                             closure: Xt.XtPointer );
BEGIN
  Xm.XmRemoveProtocolCallback(shell,
                              Xm.XmInternAtom(shell^.core.screen^.display,
                                              "WM_PROTOCOLS", FALSE),
                              protocol,
                              callback,
                              closure);
END XmRemoveWMProtocolCallback;

PROCEDURE ["C"] XmRemoveWMProtocols ( shell: Xt.Widget;
                                      protocols: X.PAAtom;
                                      num_protocols: Xt.Cardinal );
BEGIN
  Xm.XmRemoveProtocols(shell,
                       Xm.XmInternAtom(shell^.core.screen^.display,
                                       "WM_PROTOCOLS", FALSE),
                       protocols,
                       num_protocols);
END XmRemoveWMProtocols;

PROCEDURE ["C"] XmSetWMProtocolHooks ( shell: Xt.Widget;
                                       protocol: X.Atom;
                                       pre_h: Xt.XtCallbackProc;
                                       pre_c: Xt.XtPointer;
                                       post_h: Xt.XtCallbackProc;
                                       post_c: Xt.XtPointer );
BEGIN
  Xm.XmSetProtocolHooks(shell,
                        Xm.XmInternAtom(shell^.core.screen^.display,
                                        "WM_PROTOCOLS", FALSE),
                        protocol,
                        pre_h, pre_c,
                        post_h, post_c);
END XmSetWMProtocolHooks;

PROCEDURE ["C"] XmGetDisplay ( w: Xt.Widget ): Xt.Widget;
BEGIN
  RETURN Xm.XmGetXmDisplay(w^.core.screen^.display);
END XmGetDisplay;

PROCEDURE ["C"] XmIsArrowButton ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmARROW_BUTTON_BIT);
END XmIsArrowButton;

PROCEDURE ["C"] XmIsArrowButtonGadget ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmARROW_BUTTON_GADGET_BIT);
END XmIsArrowButtonGadget;

PROCEDURE ["C"] XmIsBulletinBoard ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmBULLETIN_BOARD_BIT);
END XmIsBulletinBoard;

PROCEDURE ["C"] XmIsCascadeButton ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmCASCADE_BUTTON_BIT);
END XmIsCascadeButton;

PROCEDURE ["C"] XmIsCascadeButtonGadget ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmCASCADE_BUTTON_GADGET_BIT);
END XmIsCascadeButtonGadget;

PROCEDURE ["C"] XmIsCommandBox ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmCOMMAND_BOX_BIT);
END XmIsCommandBox;

PROCEDURE ["C"] XmIsDialogShell ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmDIALOG_SHELL_BIT);
END XmIsDialogShell;

PROCEDURE ["C"] XmIsDisplay ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmDISPLAY_BIT);
END XmIsDisplay;

PROCEDURE ["C"] XmIsDragContext ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmDRAG_CONTEXT_BIT);
END XmIsDragContext;

PROCEDURE ["C"] XmIsDragIcon ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmDRAG_ICON_BIT);
END XmIsDragIcon;

PROCEDURE ["C"] XmIsDragOverShell ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmDRAG_OVER_SHELL_BIT);
END XmIsDragOverShell;

PROCEDURE ["C"] XmIsDrawingArea ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmDRAWING_AREA_BIT);
END XmIsDrawingArea;

PROCEDURE ["C"] XmIsDrawnButton ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmDRAWN_BUTTON_BIT);
END XmIsDrawnButton;

PROCEDURE ["C"] XmIsDropSiteManager ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmDROP_SITE_MANAGER_BIT);
END XmIsDropSiteManager;

PROCEDURE ["C"] XmIsDropTransfer ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmDROP_TRANSFER_BIT);
END XmIsDropTransfer;

PROCEDURE ["C"] XmIsFileSelectionBox ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmFILE_SELECTION_BOX_BIT);
END XmIsFileSelectionBox;

PROCEDURE ["C"] XmIsForm ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmFORM_BIT);
END XmIsForm;

PROCEDURE ["C"] XmIsFrame ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmFRAME_BIT);
END XmIsFrame;

PROCEDURE ["C"] XmIsGadget ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmGADGET_BIT);
END XmIsGadget;

PROCEDURE ["C"] XmIsLabel ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmLABEL_BIT);
END XmIsLabel;

PROCEDURE ["C"] XmIsLabelGadget ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmLABEL_GADGET_BIT);
END XmIsLabelGadget;

PROCEDURE ["C"] XmIsList ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmLIST_BIT);
END XmIsList;

PROCEDURE ["C"] XmIsMainWindow ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmMAIN_WINDOW_BIT);
END XmIsMainWindow;

PROCEDURE ["C"] XmIsManager ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmMANAGER_BIT);
END XmIsManager;

PROCEDURE ["C"] XmIsMenuShell ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmMENU_SHELL_BIT);
END XmIsMenuShell;

PROCEDURE ["C"] XmIsMessageBox ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmMESSAGE_BOX_BIT);
END XmIsMessageBox;

PROCEDURE ["C"] XmIsPanedWindow ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmPANED_WINDOW_BIT);
END XmIsPanedWindow;

PROCEDURE ["C"] XmIsPrimitive ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmPRIMITIVE_BIT);
END XmIsPrimitive;

PROCEDURE ["C"] XmIsPushButton ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmPUSH_BUTTON_BIT);
END XmIsPushButton;

PROCEDURE ["C"] XmIsPushButtonGadget ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmPUSH_BUTTON_GADGET_BIT);
END XmIsPushButtonGadget;

PROCEDURE ["C"] XmIsRowColumn ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmROW_COLUMN_BIT);
END XmIsRowColumn;

PROCEDURE ["C"] XmIsSash ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmSASH_BIT);
END XmIsSash;

PROCEDURE ["C"] XmIsScale ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmSCALE_BIT);
END XmIsScale;

PROCEDURE ["C"] XmIsScreen ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmSCREEN_BIT);
END XmIsScreen;

PROCEDURE ["C"] XmIsScrollBar ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmSCROLL_BAR_BIT);
END XmIsScrollBar;

PROCEDURE ["C"] XmIsScrolledWindow ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmSCROLLED_WINDOW_BIT);
END XmIsScrolledWindow;

PROCEDURE ["C"] XmIsSelectionBox ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmSELECTION_BOX_BIT);
END XmIsSelectionBox;

PROCEDURE ["C"] XmIsSeparator ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmSEPARATOR_BIT);
END XmIsSeparator;

PROCEDURE ["C"] XmIsSeparatorGadget ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmSEPARATOR_GADGET_BIT);
END XmIsSeparatorGadget;

PROCEDURE ["C"] XmIsTearOffButton ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmTEAROFF_BUTTON_BIT);
END XmIsTearOffButton;

PROCEDURE ["C"] XmIsText ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmTEXT_BIT);
END XmIsText;

PROCEDURE ["C"] XmIsTextField ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmTEXT_FIELD_BIT);
END XmIsTextField;

PROCEDURE ["C"] XmIsToggleButton ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmTOGGLE_BUTTON_BIT);
END XmIsToggleButton;

PROCEDURE ["C"] XmIsToggleButtonGadget ( w: Xt.Object ): Xt.Boolean;
BEGIN
  RETURN Xm._XmIsFastSubclass(w^.object.widget_class,
                              Xm.XmTOGGLE_BUTTON_GADGET_BIT);
END XmIsToggleButtonGadget;

PROCEDURE ["C"] XmIsExtObject ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass(w, Xm.xmExtObjectClass);
END XmIsExtObject;

PROCEDURE ["C"] XmIsCommand ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass (w, Xm.xmCommandWidgetClass);
END XmIsCommand;

PROCEDURE ["C"] XmIsDesktopObject ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass(w, Xm.xmDesktopClass);
END XmIsDesktopObject;

PROCEDURE ["C"] XmIsDialogShellExt ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass(w, Xm.xmDialogShellExtObjectClass);
END XmIsDialogShellExt;

PROCEDURE ["C"] XmIsDragIconObjectClass ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass(w, Xm.xmDragIconObjectClass);
END XmIsDragIconObjectClass;

PROCEDURE ["C"] XmIsProtocol ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass(w, Xm.xmProtocolObjectClass);
END XmIsProtocol;

PROCEDURE ["C"] XmIsShellExt ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass(w, Xm.xmShellExtObjectClass);
END XmIsShellExt;

PROCEDURE ["C"] XmIsVendorShell ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass(w, Xm.vendorShellWidgetClass);
END XmIsVendorShell;

PROCEDURE ["C"] XmIsVendorShellExt ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass(w, Xm.xmVendorShellExtObjectClass);
END XmIsVendorShellExt;

PROCEDURE ["C"] XmIsWorldObject ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN Xt.XtIsSubclass(w, Xm.xmWorldClass);
END XmIsWorldObject;

PROCEDURE ["C"] XmIsTearOffShellDescendant ( mask: CARDINAL ): Xt.Boolean;
BEGIN
  RETURN ( CAST(BITSET, mask) *
           CAST(BITSET, Xm.XmMENU_TEAR_OFF_SHELL_DESCENDANT_BIT)) #
         CAST(BITSET, 0);
      	(* in C return (mask & XmMENU_TEAR_OFF_SHELL_DESCENDANT_BIT); *)
END XmIsTearOffShellDescendant;

PROCEDURE ["C"] XmIsTorn ( mask: CARDINAL ): Xt.Boolean;
BEGIN
  RETURN ( CAST(BITSET, mask) *
           CAST(BITSET, Xm.XmMENU_TORN_BIT)) #
         CAST(BITSET, 0);
      	(* (mask & XmMENU_TORN_BIT) *)
END XmIsTorn;

PROCEDURE ["C"] XmPopupPosted ( mask: CARDINAL ): Xt.Boolean;
BEGIN
  RETURN ( CAST(BITSET, mask) *
           CAST(BITSET, Xm.XmMENU_POPUP_POSTED_BIT)) #
         CAST(BITSET, 0);
      	(* (mask & XmMENU_POPUP_POSTED_BIT) *)
END XmPopupPosted;

PROCEDURE ["C"] XtX ( w: Xt.Widget ): Xt.Position;
BEGIN
  RETURN w^.core.x;
END XtX;

PROCEDURE ["C"] XtY ( w: Xt.Widget ): Xt.Position;
BEGIN
  RETURN w^.core.y;
END XtY;

PROCEDURE ["C"] XtHeight ( w: Xt.Widget ): Xt.Dimension;
BEGIN
  RETURN w^.core.height;
END XtHeight;

PROCEDURE ["C"] XtWidth ( w: Xt.Widget ): Xt.Dimension;
BEGIN
  RETURN w^.core.width;
END XtWidth;

PROCEDURE ["C"] XtBorderWidth ( w: Xt.Widget ): Xt.Dimension;
BEGIN
  RETURN w^.core.border_width;
END XtBorderWidth;

PROCEDURE ["C"] XtBackground ( w: Xt.Widget ): Xt.Pixel;
BEGIN
  RETURN w^.core.background_pixel;
END XtBackground;

PROCEDURE ["C"] XtSensitive ( w: Xt.Widget ): Xt.Boolean;
BEGIN
  RETURN w^.core.sensitive AND w^.core.ancestor_sensitive;
END XtSensitive;

PROCEDURE ["C"] _XmBCE ( wc: Xt.WidgetClass ): Xm.XmBaseClassExt;
BEGIN
  RETURN wc^.core_class.extension;
END _XmBCE;

PROCEDURE ["C"] _XmBCEPTR ( wc: Xt.WidgetClass ): Xm.XmBaseClassExtPtr;
BEGIN
  RETURN ADR(wc^.core_class.extension);
END _XmBCEPTR;

PROCEDURE ["C"] _XmCreateImage ( VAR IMAGE: Xlib.PtrXImage;
                                 DISPLAY: Xlib.PtrDisplay;
                                 DATA: Xlib.XPointer;
                                 WIDTH, HEIGHT: CARDINAL;
                                 BYTE_ORDER: INTEGER );
BEGIN
  IMAGE := Xlib.XCreateImage(DISPLAY,
                             DISPLAY^.screens^[DISPLAY^.default_screen].root_visual
                             ,
                             1,
                             X.XYBitmap,
                             0,
                             DATA,
                             WIDTH, HEIGHT,
                             8,
                             CAST(CARDINAL,SHIFT(CAST(BITSET,WIDTH+7),-3)));
  IMAGE^.byte_order := BYTE_ORDER;
  IMAGE^.bitmap_unit := 8;
  IMAGE^.bitmap_bit_order := X.LSBFirst;
END _XmCreateImage;

PROCEDURE ["C"] _XmDCtoDD ( dc: Xt.Widget ): Xt.Widget;
BEGIN
  RETURN dc^.core.parent;
END _XmDCtoDD;

PROCEDURE ["C"] _XmDragCancel ( dc: Xt.Widget );
VAR
  dcc: Xm.XmDragContextClass;
BEGIN
  dcc := CAST(Xm.XmDragContextClass, dc^.core.widget_class);
  dcc^.drag_class.cancel(CAST(Xm.XmDragContext,dc));
END _XmDragCancel;

PROCEDURE ["C"] _XmDragStart ( dc, srcW: Xt.Widget;
                               event: Xlib.PtrXEvent );
VAR
  dcc: Xm.XmDragContextClass;
BEGIN
  dcc := CAST(Xm.XmDragContextClass, dc^.core.widget_class);
  dcc^.drag_class.start(CAST(Xm.XmDragContext,dc), srcW, event);
END _XmDragStart;

PROCEDURE ["C"] _XmFastSubclassInit ( wc: Xt.WidgetClass;
                                      bit_field: SET8 );
VAR
  bce: Xm.XmBaseClassExt;
BEGIN
  Xm._Xm_fastPtr := Xm._XmGetBaseClassExtPtr( wc, Xm.XmQmotif);
  IF (Xm._Xm_fastPtr # NIL) THEN
    bce := Xm._Xm_fastPtr^;
    IF (bce # NIL) THEN
      Xm._XmSetFlagsBit(bce^.flags, bit_field);
    END
  END
END _XmFastSubclassInit;

PROCEDURE ["C"] _XmGetBaseClassExtPtr ( wc: Xt.WidgetClass;
                                        owner: Xrm.XrmQuark ): Xm.XmGenericClassExtPtr;
VAR
  bce: Xm.XmBaseClassExt;
  gce: Xm.XmGenericClassExt;
BEGIN
  bce := Xm._XmBCE(wc);
  IF (bce # NIL) AND
     (bce^.record_type = owner) THEN
    RETURN ADR(wc^.core_class.extension);
  ELSE
    gce := CAST(Xm.XmGenericClassExt, bce);
    RETURN Xm._XmGetClassExtensionPtr(gce, owner);
  END
END _XmGetBaseClassExtPtr;

PROCEDURE ["C"] _XmGetFlagsBit ( field: Xm._XmBCEFlags;
                                 bit: SET8 ): SET8;
BEGIN
  RETURN field[CAST(INTEGER, SHIFT(bit, -3))] *
         SHIFT(SET8{0}, CAST(INTEGER, bit * SET8{0,1,2}));
	(* (field[ (bit >> 3) ]) & (1 << (bit & 0x07)) *)
END _XmGetFlagsBit;

PROCEDURE ["C"] _XmGetGadgetClassExtPtr ( wc: Xm.XmGadgetClass;
                                          owner: Xrm.XrmQuark ): Xm.XmGenericClassExtPtr;
VAR
  bce: Xm.XmGadgetClassExt;
  gce: Xm.XmGenericClassExt;
BEGIN
  bce := CAST(Xm.XmGadgetClassExt,wc^.gadget_class.extension);
  IF (bce # NIL) AND
     (bce^.record_type = owner) THEN
    RETURN ADR(wc^.gadget_class.extension);
  ELSE
    gce := CAST(Xm.XmGenericClassExt, wc^.gadget_class.extension);
    RETURN Xm._XmGetClassExtensionPtr(gce,owner);
  END
END _XmGetGadgetClassExtPtr;

PROCEDURE ["C"] _XmIsFastSubclass ( wc: Xt.WidgetClass;
                                    bit: SET8 ): Xt.Boolean;
VAR
  bce: Xm.XmBaseClassExt;
BEGIN
  Xm._Xm_fastPtr := Xm._XmGetBaseClassExtPtr(wc, Xm.XmQmotif);
  IF (Xm._Xm_fastPtr # NIL) THEN
    bce := Xm._Xm_fastPtr^;
    IF ( bce # NIL ) THEN
      IF Xm._XmGetFlagsBit( bce^.flags, bit) # SET8{} THEN
        RETURN TRUE;
      ELSE
        RETURN FALSE;
      END
    ELSE
      RETURN FALSE;
    END
  END
END _XmIsFastSubclass;

PROCEDURE ["C"] _XmSetFlagsBit ( VAR field: Xm._XmBCEFlags;
                                 bit: SET8 );
VAR
  i: INTEGER;
BEGIN
  i := CAST(INTEGER, SHIFT(bit, -3));
  field[i] := field[i] / SHIFT(SET8{0}, CAST(INTEGER, bit * SET8{0,1,2}));
  (* (field[ (bit >> 3) ] |= (1 << (bit & 0x07))) *)
END _XmSetFlagsBit;

PROCEDURE ["C"] DOExpose ( do: Xt.Widget );
BEGIN
  do^.core.widget_class^.core_class.expose ( do, NIL, NIL );
END DOExpose;

END mac_XmAllP.
