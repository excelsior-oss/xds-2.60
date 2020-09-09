MODULE WinDlg;

IMPORT
  W  := Windows
 ,S  := SYSTEM
 ,RC := h2d_resource
 ,Strings
 ;

CONST
  DEFAULTINIFILE = ".\tc2shellCfg.ini";
  MAX_PATH       = 256;
  TAB            = 11C;
  BST_UNCHECKED  = 0;
  BST_CHECKED    = 1;


TYPE
  SORTMODE* = (SORT_NONE, SORT_COUNTER, SORT_CONDTYPE, SORT_POSITION, SORT_NOT_SET_MODE);

  DLG_DATA* = RECORD
    szLogFileName*   : ARRAY MAX_PATH OF CHAR;
    szModuleName*    : ARRAY MAX_PATH OF CHAR;
    checkModuleName* : BOOLEAN;
    szGrepLine*      : ARRAY MAX_PATH OF CHAR;
    checkGrepLine*   : BOOLEAN;
    szPathPrefix*    : ARRAY MAX_PATH OF CHAR;
    hideZer*         : BOOLEAN;
    sortMode*        : SORTMODE;
  END;
  PDLG_DATA* = POINTER TO DLG_DATA;

VAR
  iniFileName*: ARRAY MAX_PATH OF CHAR;
  pDlgData    : PDLG_DATA;

--- Profile utils --------------------------------------------------------------

PROCEDURE SaveDlgDataToProfile*(pdd : PDLG_DATA);
VAR sz : ARRAY 16 OF CHAR;
BEGIN
  W.WritePrivateProfileString("DLG_DATA", "szLogFileName", pdd.szLogFileName, iniFileName);
  W.WritePrivateProfileString("DLG_DATA", "szModuleName",  pdd.szModuleName,  iniFileName);
  W.WritePrivateProfileString("DLG_DATA", "szGrepLine",    pdd.szGrepLine,    iniFileName);
  W.WritePrivateProfileString("DLG_DATA", "szPathPrefix",  pdd.szPathPrefix,  iniFileName);
  IF (pdd.checkModuleName) THEN
    W.WritePrivateProfileString("DLG_DATA", "checkModuleName",  "1",  iniFileName);
  ELSE
    W.WritePrivateProfileString("DLG_DATA", "checkModuleName",  "0",  iniFileName);
  END;
  IF (pdd.checkGrepLine) THEN
    W.WritePrivateProfileString("DLG_DATA", "checkGrepLine",  "1",  iniFileName);
  ELSE
    W.WritePrivateProfileString("DLG_DATA", "checkGrepLine",  "0",  iniFileName);
  END;
  IF (pdd.hideZer) THEN
    W.WritePrivateProfileString("DLG_DATA", "hideZer",  "1",  iniFileName);
  ELSE
    W.WritePrivateProfileString("DLG_DATA", "hideZer",  "0",  iniFileName);
  END;
  CASE pdd.sortMode OF
  |SORT_COUNTER:  sz := "COUNTER";
  |SORT_CONDTYPE: sz := "CONDTYPE";
  |SORT_POSITION: sz := "POSITION";
  ELSE            sz := "NONE";
  END;
  W.WritePrivateProfileString("DLG_DATA", "sortMode",  sz,  iniFileName);
END SaveDlgDataToProfile;


PROCEDURE ReadDlgDataFromProfile*(pdd : PDLG_DATA);
VAR buf : ARRAY 16 OF CHAR;
BEGIN
  W.GetPrivateProfileString("DLG_DATA", "szLogFileName", "", pdd.szLogFileName,LEN(pdd.szLogFileName),iniFileName);
  W.GetPrivateProfileString("DLG_DATA", "szModuleName",  "", pdd.szModuleName, LEN(pdd.szModuleName), iniFileName);
  W.GetPrivateProfileString("DLG_DATA", "szGrepLine",    "", pdd.szGrepLine,   LEN(pdd.szGrepLine),   iniFileName);
  W.GetPrivateProfileString("DLG_DATA", "szPathPrefix",  "", pdd.szPathPrefix, LEN(pdd.szPathPrefix), iniFileName);
  W.GetPrivateProfileString("DLG_DATA", "checkModuleName", "0", buf, LEN(buf), iniFileName);
  pdd.checkModuleName := buf[0] = '1';
  W.GetPrivateProfileString("DLG_DATA", "checkGrepLine",   "0", buf, LEN(buf), iniFileName);
  pdd.checkGrepLine   := buf[0] = '1';
  W.GetPrivateProfileString("DLG_DATA", "hideZer",   "0", buf, LEN(buf), iniFileName);
  pdd.hideZer         := buf[0] = '1';
  W.GetPrivateProfileString("DLG_DATA", "sortMode",  "-", buf, LEN(buf), iniFileName);
  IF    (Strings.Equal(buf, "COUNTER"))  THEN pdd.sortMode := SORT_COUNTER;
  ELSIF (Strings.Equal(buf, "CONDTYPE")) THEN pdd.sortMode := SORT_CONDTYPE;
  ELSIF (Strings.Equal(buf, "POSITION")) THEN pdd.sortMode := SORT_POSITION;
  ELSE                                       pdd.sortMode := SORT_NONE; END;
END ReadDlgDataFromProfile;

--- Dialog ---------------------------------------------------------------------

PROCEDURE GetTrimmedText(hWnd: W.HWND; rcid : LONGINT; VAR buf : ARRAY OF CHAR);
VAR
  l  : LONGINT;
BEGIN
  W.GetDlgItemText(hWnd, rcid, buf, LEN(buf));
  l := 0;
  WHILE (buf[l] = ' ') OR (buf[l] = TAB) DO
    INC(l);
  END;
  Strings.Delete(buf, 0, l);
  l := LENGTH(buf);
  WHILE (l>0) AND ((buf[l-1] = ' ') OR (buf[l-1] = TAB)) DO
    DEC(l);
  END;
  buf[l] := 0C;
END GetTrimmedText;



VAR
  acceptNotify : BOOLEAN;

PROCEDURE [W.CALLBACK] dlgProc (hWnd     : W.HWND;
                                message  : W.UINT;
                                wParam   : W.WPARAM;
                                lParam   : W.LPARAM)
                                         : W.BOOL;
  ----
  PROCEDURE BrowseDlg(VAR fname : ARRAY OF CHAR; filter, title : ARRAY OF CHAR; hParent: W.HWND): LONGINT;
  -- returns nFileOffset or -1
  VAR
    of : W.OPENFILENAME;
    ss : ARRAY 8 OF CHAR;
  BEGIN
    ss := ".";
    S.FILL(S.ADR(of), 0, SIZE(of));
    of.lStructSize      := SIZE(of);
    of.hwndOwner        := hParent;
    of.lpstrFilter      := S.VAL(W.PSTR, S.ADR(filter[0]));
    of.lpstrFile        := S.VAL(W.PSTR, S.ADR(fname[0]));
    of.lpstrInitialDir  := S.VAL(W.PSTR, S.ADR(ss[0]));
    of.nMaxFile         := LEN(fname);
    of.lpstrTitle       := S.VAL(W.PSTR, S.ADR(title[0]));
    of.Flags            := W.OFN_FILEMUSTEXIST + W.OFN_HIDEREADONLY + W.OFN_LONGNAMES +
                           W.OFN_NODEREFERENCELINKS + W.OFN_NONETWORKBUTTON +
                           W.OFN_NOTESTFILECREATE + W.OFN_PATHMUSTEXIST;
    IF W.GetOpenFileName(of) THEN
      RETURN of.nFileOffset;
    ELSE
      RETURN -1;
    END;
  END BrowseDlg;
  ----
  PROCEDURE BrowseLog();
  VAR
    fname : ARRAY MAX_PATH OF CHAR;
  BEGIN
    fname := "";
    IF BrowseDlg(fname, "All files (*.*)" + 0C + "*.*" + 0C + 0C, "Open info file", hWnd) >= 0 THEN
      W.SetDlgItemText(hWnd, RC.IDC_EFFILE, fname);
    END;
  END BrowseLog;
  ----
  PROCEDURE BrowseModule();
  VAR
    fname : ARRAY MAX_PATH OF CHAR;
    fofs  : LONGINT;
  BEGIN
    fname := "";
    fofs := BrowseDlg(fname, "All files (*.*)" + 0C + "*.*" + 0C + 0C, "Open info file", hWnd);
    IF (fofs>=0) THEN
      W.SetDlgItemText(hWnd, RC.IDC_EFMODULE, S.VAL(W.PSTR, S.ADR(fname[fofs])));
    END;
  END BrowseModule;
  ----
VAR
  n : LONGINT;
BEGIN
  CASE message OF
  | W.WM_INITDIALOG:
      acceptNotify := FALSE;
      W.ShowWindow(hWnd, W.SW_SHOW);
      W.SetDlgItemText(hWnd, RC.IDC_EFFILE,     pDlgData.szLogFileName);
      W.SetDlgItemText(hWnd, RC.IDC_EFMODULE,   pDlgData.szModuleName);
      W.SetDlgItemText(hWnd, RC.IDC_EFGREPLINE, pDlgData.szGrepLine);
      W.SetDlgItemText(hWnd, RC.IDC_EFPPREFIX,  pDlgData.szPathPrefix);
      IF (pDlgData.checkModuleName) THEN
        W.CheckDlgButton(hWnd, RC.IDC_CBMODULE, BST_CHECKED);
      END;
      IF (pDlgData.checkGrepLine) THEN
        W.CheckDlgButton(hWnd, RC.IDC_CBGREP, BST_CHECKED);
      END;
      IF (pDlgData.hideZer) THEN
        W.CheckDlgButton(hWnd, RC.IDC_CBHIDEZ, BST_CHECKED);
      END;
      CASE (pDlgData.sortMode) OF
      |SORT_COUNTER:  n := RC.IDC_RBSORTCOUNTER;
      |SORT_CONDTYPE: n := RC.IDC_RBSORTCONDTYPE;
      |SORT_POSITION: n := RC.IDC_RBSORTPOSITION;
      ELSE            n := RC.IDC_RBSORTNONE;
      END;
      W.CheckDlgButton(hWnd, n, BST_CHECKED);
      W.SetFocus(W.GetDlgItem(hWnd, RC.IDC_EFFILE));
      acceptNotify := TRUE;
      RETURN FALSE;
  | W.WM_COMMAND:
      CASE S.VAL(S.CARD16, wParam) OF
      | RC.IDC_PBBROWSELOG:
          BrowseLog();
      | RC.IDC_PBBROWSEMODULE:
          BrowseModule();
      | W.IDCANCEL:
          W.EndDialog(hWnd,0);
      | W.IDOK:
          GetTrimmedText(hWnd, RC.IDC_EFFILE, pDlgData.szLogFileName);
          GetTrimmedText(hWnd, RC.IDC_EFMODULE,   pDlgData.szModuleName);
          GetTrimmedText(hWnd, RC.IDC_EFGREPLINE, pDlgData.szGrepLine);
          GetTrimmedText(hWnd, RC.IDC_EFPPREFIX,  pDlgData.szPathPrefix);
          pDlgData.checkModuleName := (W.IsDlgButtonChecked(hWnd, RC.IDC_CBMODULE) = BST_CHECKED);
          pDlgData.checkGrepLine   := (W.IsDlgButtonChecked(hWnd, RC.IDC_CBGREP)   = BST_CHECKED);
          pDlgData.hideZer         := (W.IsDlgButtonChecked(hWnd, RC.IDC_CBHIDEZ)  = BST_CHECKED);
          IF    (W.IsDlgButtonChecked(hWnd, RC.IDC_RBSORTCOUNTER) = BST_CHECKED) THEN
            pDlgData.sortMode := SORT_COUNTER;
          ELSIF (W.IsDlgButtonChecked(hWnd, RC.IDC_RBSORTCONDTYPE) = BST_CHECKED) THEN
            pDlgData.sortMode := SORT_CONDTYPE;
          ELSIF (W.IsDlgButtonChecked(hWnd, RC.IDC_RBSORTPOSITION) = BST_CHECKED) THEN
            pDlgData.sortMode := SORT_POSITION;
          ELSE
            pDlgData.sortMode := SORT_NONE;
          END;
          IF (pDlgData.szLogFileName[0] = 0C) THEN
            W.MessageBox(hWnd, "Please specify info file name.", "Error", W.MB_OK+W.MB_ICONERROR);
            W.SetFocus(W.GetDlgItem(hWnd, RC.IDC_EFFILE));
          ELSE
            W.EndDialog(hWnd,1);
          END;
      | RC.IDC_EFMODULE:
          IF acceptNotify AND (wParam >> 16 = W.EN_CHANGE) THEN
            W.GetDlgItemText(hWnd, RC.IDC_EFMODULE, pDlgData.szModuleName, LEN(pDlgData.szModuleName));
            IF (pDlgData.szModuleName[0] = 0C) THEN
              W.CheckDlgButton(hWnd, RC.IDC_CBMODULE, BST_UNCHECKED);
            ELSE
              W.CheckDlgButton(hWnd, RC.IDC_CBMODULE, BST_CHECKED);
            END;
          END;
      | RC.IDC_EFGREPLINE:
          IF acceptNotify AND (wParam >> 16 = W.EN_CHANGE) THEN
            W.GetDlgItemText(hWnd, RC.IDC_EFGREPLINE, pDlgData.szGrepLine, LEN(pDlgData.szGrepLine));
            IF (pDlgData.szGrepLine[0] = 0C) THEN
              W.CheckDlgButton(hWnd, RC.IDC_CBGREP, BST_UNCHECKED);
            ELSE
              W.CheckDlgButton(hWnd, RC.IDC_CBGREP, BST_CHECKED);
            END;
          END;
      ELSE
      END;
      RETURN TRUE;
  ELSE
  END;
  RETURN FALSE;
END dlgProc;


-- returns FALSE when cancelled
PROCEDURE Dialog*(data : PDLG_DATA (*<=>*)) : BOOLEAN;
VAR
  ok   : BOOLEAN;
BEGIN
  pDlgData := data;
  ok := (W.DialogBox(W.GetModuleHandle(NIL),S.VAL(W.PSTR, VAL(S.CARD32,RC.IDD_TCOV)),
                     NIL, dlgProc) = 1);
  pDlgData := NIL;
  RETURN ok;
END Dialog;

BEGIN
  iniFileName := DEFAULTINIFILE;
END WinDlg.
