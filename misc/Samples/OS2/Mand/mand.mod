(**********************************************************************)
(*                                                                    *)
(*                     xTech Development System                       *)
(*                    Native XDS-x86 OS/2 Edition                     *)
(*                                                                    *)
(*      Mandelbrodt Set Explorer for OS/2 Presentation Manager        *)
(*                                                                    *)
(*  Mand.mod - main source file                                       *)
(*                                                                    *)
(*  Control:                                                          *)
(*      Left mouse button - 2x magnification                          *)
(*      Right mouse button - 4x magnification                         *)
(*                                                                    *)
(*  Copyright (C) 1996-97 xTech Ltd.                                  *)
(*                                                                    *)
(**********************************************************************)

MODULE Mand;

IMPORT FormStr, Storage, SYSTEM;
IMPORT O := OS2, OS2RTL;
FROM SYSTEM IMPORT ADR, ADDRESS, CAST, CARD8, REF, FILL;

CONST
  MAINCLASSNAME = "MandelbrodtSet";

  RES_MAIN      = 17;

  REFRESH_ITEM  = 17;
  COPY_ITEM     = 2;
  BACK_ITEM     = 3;
  FORWARD_ITEM  = 4;

  IDP_MAGN      = 1;

  MSG_UPDATE    = O.WM_USER+1000;
  MSG_DONE      = O.WM_USER+1001;
  MSG_MENUSTATE = O.WM_USER+1002;
  --MSG_TITLE     = O.WM_USER+1003;
  MSG_BUILD     = O.WM_USER+1004;

VAR
  hAB                     : O.HAB;
  hMainFrame, hMainClient : O.HWND;
  hArrowPointer           : O.HPOINTER;
  hWaitPointer            : O.HPOINTER;
  hMagnPointer            : O.HPOINTER;

  tidCntThread            : O.TID;
  nRecalc                 : INTEGER;
  perc, prevperc          : CARDINAL;
  ulTimerId               : CARDINAL;

PROCEDURE MyEnableMenuItem(hwndMenu :O.HWND; usId :CARDINAL; fEnable :BOOLEAN);
VAR c : O.USHORT;
BEGIN
  IF fEnable THEN c:= 0; ELSE c:= O.MIA_DISABLED; END;
  O.WinSendMsg(hwndMenu
              ,O.MM_SETITEMATTR
              ,O.MPFROM2USHORT(usId, 1)
              ,O.MPFROM2USHORT(O.MIA_DISABLED, c));
END MyEnableMenuItem;

(*-------------------------- Abstract data types -----------------------------*)

(** Bit-map info structure **)

MODULE BI;

FROM O IMPORT BITMAPINFO2, PBITMAPINFO2, RGB2;
IMPORT ADR, ADDRESS, CARD8, REF, FILL;

EXPORT QUALIFIED Setsz, bi;

TYPE  BIS = RECORD
        h   : BITMAPINFO2;
        clr : ARRAY [1..255] OF RGB2;
      END;

VAR   bi      : BIS;
      i       : CARDINAL;

PROCEDURE Setsz(cx, cy :CARDINAL);
BEGIN
  bi.h.cx := cx;
  bi.h.cy := cy;
END Setsz;

-- Type constructor

BEGIN

  FILL(ADR(bi), 0, SIZE(BIS) );
  bi.h.cbFix     := SIZE(BITMAPINFO2)-SIZE(RGB2);
  bi.h.cPlanes   := 1;
  bi.h.cBitCount := 8;

  FOR i := 0 TO 254 DO
    bi.clr[i].bRed     := VAL (CARD8, i MOD 16 * 16);
    bi.clr[i].bGreen   := VAL (CARD8, (i DIV 8 MOD 8) * 32);
    bi.clr[i].bBlue    := VAL (CARD8, (i DIV 32) * 32);
  END;

END BI;

(** Mandel info data type **)

MODULE MI;

FROM Storage IMPORT ALLOCATE, DEALLOCATE;
FROM O IMPORT DosFreeMem;
IMPORT CARD8, ADDRESS;

EXPORT MANDINFO, PMANDINFO, MINew, MIDel;

CONST START_X0 = -3.0;
      START_Y0 = -2.3;
      START_X1 =  2.0;
      START_Y1 =  2.2;

TYPE
      MANDINFO = RECORD
                  MX0,MY0,MX1,MY1 :LONGREAL; -- Mandelbrodt parms
                  Bcx,Bcy         :CARDINAL; -- current bit-map sizes (=curr client rgn sizes)
                  pMBitmap        :ADDRESS;  -- bit-map image
                 END;

      PMANDINFO = POINTER TO MANDINFO;


PROCEDURE MINew(VAR pminfo : PMANDINFO; pmsrc : PMANDINFO);
BEGIN
  ALLOCATE( pminfo, SIZE(MANDINFO) );
  IF pmsrc = NIL THEN
    pminfo^.MX0 := START_X0;
    pminfo^.MY0 := START_Y0;
    pminfo^.MX1 := START_X1;
    pminfo^.MY1 := START_Y1;
  ELSE
    pminfo^ := pmsrc^;
  END;
  pminfo^.pMBitmap := NIL;
END MINew;

PROCEDURE MIDel(pminfo : PMANDINFO);
BEGIN
  IF pminfo^.pMBitmap # NIL THEN DosFreeMem(pminfo^.pMBitmap); END;
  DEALLOCATE(pminfo, SIZE(MANDINFO));
END MIDel;

END MI;

(**    Picture List   **)

MODULE PList;

IMPORT MANDINFO, PMANDINFO, MINew, MIDel;
FROM Storage IMPORT DEALLOCATE;

EXPORT QUALIFIED nCurPic, nPic,       (* READ-ONLY *)
                 Back, Curr, Forw,To,
                 Cut, App, Replace,
                 hzMB;

CONST PnMAX = 100;

VAR
  nCurPic, nPic : INTEGER;
  aPicList      : ARRAY [0..PnMAX-1] OF PMANDINFO;

PROCEDURE Back(); BEGIN IF nCurPic # 0 THEN  DEC(nCurPic); END; END Back;

PROCEDURE Curr(): PMANDINFO; BEGIN RETURN aPicList[nCurPic]; END Curr;

PROCEDURE Forw(); BEGIN IF nCurPic+1 # nPic THEN INC(nCurPic); END; END Forw;

PROCEDURE To(n: INTEGER); BEGIN nCurPic:=n; END To;

PROCEDURE Cut(cpos: INTEGER);
VAR
  i: INTEGER;
BEGIN
  (* requires 0 < cpos < nPic *)
  FOR i := cpos TO nPic-1 DO
    MIDel(aPicList[i]);
    aPicList[i] := NIL;
  END;
  nCurPic := cpos; nPic := cpos;
  Back();
END Cut;

PROCEDURE App(pm : PMANDINFO);
BEGIN
  IF nPic = PnMAX THEN RETURN; END;
  aPicList[nPic] := pm;
  INC(nPic);
END App;

PROCEDURE Replace(n :INTEGER; p :PMANDINFO);
BEGIN
  MIDel(aPicList[n]);
  aPicList[n] := p;
END Replace;

PROCEDURE hzMB() : BOOLEAN; BEGIN RETURN aPicList[0]^.pMBitmap # NIL; END hzMB;

PROCEDURE ReleaseMem;
VAR
  i : INTEGER;
BEGIN
  FOR i := 0 TO nPic-1 DO
    DEALLOCATE( aPicList[i], SIZE(MANDINFO) );
  END;
END ReleaseMem;

BEGIN
  MINew(aPicList[0], NIL);
  nCurPic := 0;
  nPic    := 1;
FINALLY
  ReleaseMem;
END PList;

(*----------------------------------------------------------------------------*)

VAR ID :ARRAY [0..2000] OF LONGREAL;

(** Mandelbrodt set calculation procedure to execute in the background thread **)

PROCEDURE [O.EXPENTRY] CalcSet (c : CARDINAL);

(* these types are used only to cast rather than to instantiate *)
CONST UB  = 0ffffffH;
TYPE  PTR  = POINTER TO ARRAY [0..UB] OF CARD8;

VAR x0, y0, x, y, x2, y2 : LONGREAL;
    wb, i, j             : CARDINAL;
    l                    : LONGREAL;
    p                    : CARDINAL;
    n                    : CARDINAL;
    pHuge                : PTR;
    pminfo               : PMANDINFO;

BEGIN
  pminfo := PMANDINFO(c); (* thread parms passed by DosCreateThread *)
  WITH pminfo^ DO
    IF pMBitmap=NIL THEN RETURN; END;
 
    IF Bcx>Bcy THEN j := Bcx; l := MX1-MX0;
    ELSE            j := Bcy; l := MY1-MY0;
    END;
 
    FOR i := 0 TO j DO
      ID[i] := l * LFLOAT(i) / LFLOAT(j);
    END;
 
    pHuge := PTR(pMBitmap);
 
    p    := 0;
    wb   := (Bcx + 3) / 4 * 4;
    perc := 0; prevperc := 0;
    O.WinStartTimer(hAB, hMainClient, 0, 200);
 
    FOR j := 1 TO Bcy DO
      y0 := MY0 + ID[j];
      FOR i := 1 TO wb DO
        y := y0;
        x0 := MX0 + ID[i]; x := x0;
 
        n := 0;
        LOOP
          x2 := x*x; y2 := y*y;
          IF x2 + y2 >= 4.0 THEN EXIT; END;
          INC (n); IF n = 255 THEN EXIT; END;
          y := 2.0*x*y + y0; x := x2 - y2 + x0;
        END;
        pHuge^[p] := CARD8(n);
        INC (p);
      END;
      perc := (j+1)*100 / Bcy;
    END;
  END; (* WITH *)

  O.WinStopTimer(hAB, hMainClient, ulTimerId);
  O.WinPostMsg(hMainClient, MSG_DONE, NIL, pminfo);
  O.DosExit(O.EXIT_THREAD, 0);
END CalcSet;

PROCEDURE BuildImage (pminfo : PMANDINFO; w, h : CARDINAL) :BOOLEAN;
BEGIN
  WITH pminfo^ DO
    O.DosAllocMem(pMBitmap, (w+3)/4*4*h, O.PAG_READ + O.PAG_WRITE + O.PAG_COMMIT);
    IF (pMBitmap # NIL) THEN
      Bcx := w; Bcy := h;
      IF O.DosCreateThread(tidCntThread,        (* Thread ID (returned by function)  *)
                           CalcSet,             (* Address of thread program         *)
                           CARDINAL(pminfo),    (* Parameter passed to ThreadProc    *)
                           O.CREATE_READY,      (* Start thread immediately          *)
                           8192)                (* Stack size, rounded to page bdy   *)
                         = O.NO_ERROR THEN
         RETURN TRUE;
      ELSE
         O.WinMessageBox(O.HWND_DESKTOP, hMainClient,
                        "Failed to create thread",
                        "Image creation", 0,
                        O.MB_ERROR + O.MB_OK + O.MB_MOVEABLE);
      END;
    ELSE
      O.WinMessageBox(O.HWND_DESKTOP, hMainClient,
                      "Sorry, no memory for a bitmap",
                      "Image creation", 0,
                      O.MB_ERROR + O.MB_OK + O.MB_MOVEABLE);
    END;
  END;
  RETURN FALSE;
END  BuildImage;

PROCEDURE isThreadR() :BOOLEAN;
BEGIN
  RETURN O.DosWaitThread(tidCntThread, O.DCWW_NOWAIT) = O.ERROR_THREAD_NOT_TERMINATED;
END isThreadR;

PROCEDURE DrawBitmap(hps :O.HPS; cx, cy :CARDINAL; pminfo :PMANDINFO) :BOOLEAN;
VAR aPoints: ARRAY [0..3] OF O.POINTL;
BEGIN
  WITH pminfo^ DO
    IF pMBitmap = NIL THEN RETURN FALSE; END;
 
    aPoints[0].x := 0;   aPoints[0].y := 0;     -- client region size
    aPoints[1].x := cx;  aPoints[1].y := cy;
 
    aPoints[2].x := 0;   aPoints[2].y := 0;
    aPoints[3].x := Bcx; aPoints[3].y := Bcy;
 
    BI.Setsz(Bcx, Bcy);
 
    O.GpiDrawBits(hps, CAST(ADDRESS, pMBitmap), BI.bi.h, 4, aPoints,
                  O.ROP_SRCCOPY, O.BBO_IGNORE);
    RETURN TRUE;
  END;
END DrawBitmap;

(*  Window procedure  *)

PROCEDURE [O.EXPENTRY] wpMainClient(hWnd   :O.HWND;
                                    ulMsg  :CARDINAL;
                                    m1, m2 :O.MPARAM) :O.MRESULT;

VAR
  swp         : O.SWP;
  pminfo      : PMANDINFO;

  k, dmx, dmy : LONGREAL;
  Wcx,Wcy     : CARDINAL;

  clRect      : O.RECTL;
  hps         : O.HPS;

  pmcpy       : PMANDINFO;
  x,y         : LONGREAL;
  tm          : CARDINAL;

  s           : ARRAY[0..80] OF CHAR;

  hMenu       : O.HWND;

(* create a bit-map to copy to clipboard *)
PROCEDURE CopyBitmap(pminfo :PMANDINFO );

VAR  hbm  :O.HBITMAP;
     bmih :O.BITMAPINFOHEADER2;
     hps  :O.HPS;

BEGIN
  hps := O.WinGetPS(hWnd);

  SYSTEM.FILL(ADR(bmih), 0, SIZE(O.BITMAPINFOHEADER2));
  bmih.cbFix     := SIZE ( O.BITMAPINFOHEADER2 );
  bmih.cx        := pminfo^.Bcx;
  bmih.cy        := pminfo^.Bcy;
  bmih.cPlanes   := 1;
  bmih.cBitCount := 8;
  hbm := O.GpiCreateBitmap(hps, bmih, O.CBM_INIT,
                           pminfo^.pMBitmap, BI.bi.h);
  O.WinReleasePS(hps);

  IF hbm = O.NULLHANDLE THEN RETURN; END;

  IF O.WinOpenClipbrd(hAB) THEN
    O.WinSetClipbrdData(hAB, CAST(O.ULONG,hbm),O.CF_BITMAP,O.CFI_HANDLE);
    O.WinCloseClipbrd(hAB);
  END;
  O.GpiDeleteBitmap(hbm);
END CopyBitmap;

BEGIN
  pminfo := PList.Curr();
  hMenu  := O.WinWindowFromID(hMainFrame, O.FID_MENU);
  O.WinQueryWindowPos(hWnd, swp);

  WITH pminfo^ DO
    CASE ulMsg OF
    |O.WM_CREATE:
      O.WinDefWindowProc(hWnd,ulMsg,m1,m2);
      O.WinQueryWindowPos(O.HWND_DESKTOP, swp );
      O.WinSetWindowPos(hMainFrame,O.HWND_TOP,
                        swp.cx /4, swp.cy / 4, swp.cx / 2, swp.cy / 2,
                        O.SWP_ZORDER + O.SWP_SIZE +O.SWP_MOVE +O.SWP_SHOW );
      O.WinPostMsg(hWnd, MSG_MENUSTATE, NIL, NIL);
    |O.WM_SIZE:
      Wcx := swp.cx;
      Wcy := swp.cy;

      IF (Wcx # 0) AND (Wcy # 0) THEN            (* exept minimized case *)
        dmx := MX1-MX0; dmy := MY1-MY0;
        k   := dmx/LFLOAT(Wcx);
        IF k > dmy/LFLOAT(Wcy) THEN k := dmy/LFLOAT(Wcy); END;

        MX0 := MX0 + (dmx - LFLOAT(Wcx) * k) / 2;
        MX1 := MX0 + LFLOAT(Wcx) * k;
        MY0 := MY0 + (dmy - LFLOAT(Wcy) * k) / 2;
        MY1 := MY0 + LFLOAT(Wcy) * k;
      END;
    |O.WM_PAINT:
       hps := O.WinBeginPaint(hWnd, 0, clRect );

       clRect.xLeft := 0; clRect.yBottom := 0;
       clRect.xRight := swp.cx; clRect.yTop := swp.cy;

       IF NOT DrawBitmap(hps, swp.cx, swp.cy, pminfo) THEN
         O.WinFillRect(hps, clRect, O.CLR_BLACK);
       END;

       O.WinEndPaint(hps);
    |O.WM_BUTTON1DOWN, O.WM_BUTTON2DOWN:

      IF O.WinQueryFocus(O.HWND_DESKTOP) # hWnd THEN
        RETURN O.WinDefWindowProc(hWnd,ulMsg,m1,m2);
      END;

      IF isThreadR() THEN
        O.WinAlarm(O.HWND_DESKTOP,O.WA_WARNING);
        RETURN O.MRESULT(FALSE);
      END;

      IF NOT PList.hzMB() THEN
        O.WinPostMsg(hWnd, O.WM_COMMAND, O.MPARAM(REFRESH_ITEM), NIL);
        RETURN O.MRESULT(FALSE);
      END;

      IF ulMsg = O.WM_BUTTON2DOWN THEN tm := 8; ELSE tm := 4; END;

      (* create to pass in the thread *)
      MINew(pmcpy, pminfo);

      dmx := pmcpy^.MX1-pmcpy^.MX0; dmy := pmcpy^.MY1-pmcpy^.MY0;
      k := dmx/LFLOAT(pmcpy^.Bcx);
      x := LFLOAT(O.USHORT1FROMMP(m1))*k+pmcpy^.MX0;
      y := LFLOAT(O.USHORT2FROMMP(m1))*k+pmcpy^.MY0;

      pmcpy^.MX0 := x - dmx/LFLOAT(tm); pmcpy^.MX1 := x + dmx/LFLOAT(tm);
      pmcpy^.MY0 := y - dmy/LFLOAT(tm); pmcpy^.MY1 := y + dmy/LFLOAT(tm);

      (* create to append an empty picture *)
      MINew(pminfo, pmcpy);
      PList.Cut(PList.nCurPic+1); PList.App(pminfo);

      (* build image in the second thread *)
      O.WinPostMsg(hWnd, MSG_BUILD, O.MPFROMULONG(PList.nCurPic+1), pmcpy);
    |O.WM_COMMAND:
      CASE O.ULONGFROMMP(m1) OF
      |REFRESH_ITEM:
         (* create to pass in the thread *)
         MINew(pmcpy, pminfo);
         O.WinPostMsg(hWnd, MSG_BUILD, O.MPFROMULONG(PList.nCurPic), pmcpy);
      |COPY_ITEM:
         CopyBitmap(pminfo);
      |BACK_ITEM:
         PList.Back();
         O.WinSendMsg(hWnd, O.WM_SIZE, NIL, NIL);
         O.WinSendMsg(hWnd, MSG_UPDATE, NIL, NIL);
      |FORWARD_ITEM:
         PList.Forw();
         O.WinSendMsg(hWnd, O.WM_SIZE, NIL, NIL);
         O.WinSendMsg(hWnd, MSG_UPDATE, NIL, NIL);
      ELSE
         RETURN O.WinDefWindowProc(hWnd,ulMsg,m1,m2);
      END;
    |O.WM_TIMER:
      IF perc # prevperc THEN
        prevperc := perc;
        FormStr.print(s, "M-Set: calculated %d%%", perc );
        O.WinSetWindowText (hMainFrame, s);
      END;
    |O.WM_MOUSEMOVE:
      (* If do not have focus, indicate ability to get it *)
      IF O.WinQueryFocus(O.HWND_DESKTOP) # hWnd THEN
        O.WinSetPointer(O.HWND_DESKTOP,hArrowPointer);
      (* If calculating, set mouse pointer to Wait *)
      ELSIF isThreadR() THEN
        O.WinSetPointer(O.HWND_DESKTOP,hWaitPointer);
      ELSE
        O.WinSetPointer(O.HWND_DESKTOP,hMagnPointer);
      END;
      RETURN O.MPFROMULONG(1 (*TRUE*));
    |MSG_BUILD:
      IF isThreadR() THEN RETURN O.MRESULT(FALSE); END;
      MyEnableMenuItem( hMenu, REFRESH_ITEM, FALSE );
      nRecalc := INTEGER(m1);
      IF NOT BuildImage(PMANDINFO(m2), swp.cx, swp.cy) THEN
        MIDel(PMANDINFO(m2));          (* no enough memory *)
        MyEnableMenuItem( hMenu, REFRESH_ITEM, TRUE );
      ELSE
        O.WinSetPointer(O.HWND_DESKTOP,hWaitPointer);
      END;
    |MSG_UPDATE:
      FormStr.print (s, "M-Set: Picture %d (%d)", PList.nCurPic+1, PList.nPic);
      O.WinSetWindowText (hMainFrame, s);
      O.WinPostMsg(hWnd, MSG_MENUSTATE, NIL, NIL);
      O.WinInvalidateRect(hWnd, NIL, FALSE);
    |MSG_DONE:
      MyEnableMenuItem( hMenu, REFRESH_ITEM, TRUE );
      PList.To(nRecalc);
      PList.Replace(nRecalc, PMANDINFO(m2) );
      O.WinPostMsg(hWnd, MSG_UPDATE, NIL, NIL);
    |MSG_MENUSTATE:
      MyEnableMenuItem( hMenu, BACK_ITEM, (PList.nCurPic # 0) );
      MyEnableMenuItem( hMenu, FORWARD_ITEM, (PList.nCurPic+1 # PList.nPic) );
    ELSE
      RETURN O.WinDefWindowProc(hWnd,ulMsg,m1,m2);
    END; (* CASE*)
  END; (* WITH *)
  RETURN O.MRESULT(FALSE);
END wpMainClient;

VAR SwData  :O.SWCNTRL;
    hSwitch :O.HSWITCH;
    hMsgQ   :O.HMQ;
    qMsg    :O.QMSG;
    fcdata  :O.FRAMECDATA;

BEGIN
  hAB        := O.NULLHANDLE;
  hMsgQ      := O.NULLHANDLE;
  hMainFrame := O.NULLHANDLE;
  hSwitch    := O.NULLHANDLE;

  hAB := O.WinInitialize(0);
  IF hAB = O.NULLHANDLE THEN HALT; END;

  hMsgQ := O.WinCreateMsgQueue(hAB,32);
  IF hMsgQ = O.NULLHANDLE THEN HALT; END;

  IF NOT O.WinRegisterClass(hAB
                           ,MAINCLASSNAME
                           ,wpMainClient
                           ,O.CS_SIZEREDRAW + O.CS_CLIPCHILDREN
                           ,0) THEN
    HALT;
  END;

  fcdata.cb := SIZE(O.FRAMECDATA);
  fcdata.flCreateFlags := O.FCF_TITLEBAR + O.FCF_SYSMENU + O.FCF_MINMAX + O.FCF_ICON +
                          O.FCF_SIZEBORDER + O.FCF_SHELLPOSITION + O.FCF_MENU;
  fcdata.hmodResources := 0;
  fcdata.idResources   := RES_MAIN;

  hMainFrame := O.WinCreateWindow(O.HWND_DESKTOP,      (* parent window          *)
                                  O.WC_FRAME,          (* class name             *)
                                  "M-Set",             (* window text            *)
                                  0,                   (* window style           *)
                                  0,0,                 (* position (x,y)         *)
                                  0,0,                 (* size (width,height)    *)
                                  0,                   (* owner window           *)
                                  O.HWND_TOP,          (* sibling window         *)
                                  0,                   (* window id              *)
                                  ADR(fcdata),         (* control data           *)
                                  NIL);                (* presentation parms     *)

  IF hMainFrame = 0 THEN
    O.WinMessageBox(O.HWND_DESKTOP, hMainClient,
                    "Failed to create main window",
                    "M-Set", 0,
                    O.MB_ERROR + O.MB_OK + O.MB_MOVEABLE);
    HALT;
  END;

  hMainClient := O.WinCreateWindow(
                   hMainFrame,          (* parent window          *)
                   MAINCLASSNAME,       (* class name             *)
                   NIL,                 (* window text            *)
                   0,                   (* window style           *)
                   0, 0,                (* position (x,y)         *)
                   0, 0,                (* size (width,height)    *)
                   hMainFrame,          (* owner window           *)
                   O.HWND_TOP,          (* sibling window         *)
                   O.FID_CLIENT,        (* window id              *)
                   NIL,                 (* control data           *)
                   NIL);                (* presentation parms     *)

  IF hMainClient = 0 THEN
     O.WinMessageBox(O.HWND_DESKTOP, hMainClient,
                      "Failed to create client window",
                      "M-Set", 0,
                      O.MB_ERROR + O.MB_OK + O.MB_MOVEABLE);
     HALT;
  END;

  (* Add switch entry to the system Window List *)
  WITH SwData DO
    hwnd            := hMainFrame;
    hwndIcon        := 0;
    hprog           := 0;
    idProcess       := 0;
    idSession       := 0;
    uchVisibility   := O.SWL_VISIBLE;
    fbJump          := O.SWL_JUMPABLE;
    szSwtitle       := "Mandelbrodt Set Explorer";
  END;
  hSwitch := O.WinAddSwitchEntry(SwData);

  hArrowPointer := O.WinQuerySysPointer(O.HWND_DESKTOP, O.SPTR_ARROW, FALSE);
  hWaitPointer  := O.WinQuerySysPointer(O.HWND_DESKTOP, O.SPTR_WAIT,  FALSE);
  hMagnPointer  := O.WinLoadPointer(O.HWND_DESKTOP, 0, IDP_MAGN);

  O.WinPostMsg(hMainClient, O.WM_COMMAND, O.MPARAM(REFRESH_ITEM), NIL);

  (* Main message loop *)
  WHILE O.WinGetMsg(hAB, qMsg, 0, 0, 0) DO
    O.WinDispatchMsg(hAB, qMsg);
  END;

FINALLY
  (* Release all resources *)
  IF hSwitch    <> O.NULLHANDLE THEN O.WinRemoveSwitchEntry(hSwitch); END;
  IF hMainFrame <> O.NULLHANDLE THEN O.WinDestroyWindow(hMainFrame); END;
  IF hMsgQ      <> O.NULLHANDLE THEN O.WinDestroyMsgQueue(hMsgQ); END;
  IF hAB        <> O.NULLHANDLE THEN O.WinTerminate(hAB); END;
END Mand.


