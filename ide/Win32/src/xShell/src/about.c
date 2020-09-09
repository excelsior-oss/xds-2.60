#include "util.h"
#include "shell.h"
#include "res.h"
#include "xdsres.h"
#include "var.h"

OSVERSIONINFO Verinfo;
SYSTEM_INFO   sysinfo;

WNDPROC OldStaticProc;
HBITMAP AboutBmp;
HPALETTE AboutPalette;
HPALETTE OldPalette = 0;
int about_cx, about_cy;

LRESULT CALLBACK DrawPictureProc (HWND hwnd, UINT msg,
				  WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC dc, bmpdc;
	HPALETTE pal;
	if (msg == WM_PAINT)	{
		dc = BeginPaint (hwnd, &ps);
		bmpdc = CreateCompatibleDC (dc);
		pal = SelectPalette  (dc, AboutPalette, FALSE);
		RealizePalette (dc);
		SelectPalette  (bmpdc, AboutPalette, FALSE);
		RealizePalette (bmpdc);
		if (!OldPalette) OldPalette = pal;
		SelectObject (bmpdc, AboutBmp);
		BitBlt (dc, 0, 0, about_cx, about_cy,
			bmpdc, 0, 0, SRCCOPY);
		RealizePalette (dc);
		DeleteDC (bmpdc);
		EndPaint (hwnd, &ps);
		return 0;
	} else
		return CallWindowProc (OldStaticProc, hwnd, msg, wparam, lparam);
}

void	expand_wnd (HWND hwnd, int dx, int dy)
{
	RECT rc;

	Get_child_pos (hwnd, &rc);
	SetWindowPos (hwnd, 0, 0, 0, rc.right - rc.left + dx,
		      rc.bottom - rc.top + dy,
		       SWP_NOZORDER | SWP_NOMOVE);
}

void	shift_wnd (HWND hwnd, int x, int y)
{
	RECT rc;

	Get_child_pos (hwnd, &rc);
	SetWindowPos (hwnd, 0, rc.left+x, rc.top+y, 0, 0,
		      SWP_NOZORDER | SWP_NOSIZE);
}

void	draw_os_info (HWND hwnd)
{
	MEMORYSTATUS mstatus;
	char t [100];
	char s [1024];
	DWORD sz;
	WORD proc;

	sz = sizeof (t);
	GetComputerName (t, &sz);
	t [sz] = 0;
	SetDlgItemText (hwnd, ABOUT_COMPUTER, t);

	sz = sizeof (t);
	GetUserName (t, &sz);
	t [sz] = 0;
	SetDlgItemText (hwnd, ABOUT_USER, t);

	sprintf (s, "%s %ld.%ld, build %ld",
		Verinfo.dwPlatformId == VER_PLATFORM_WIN32s ? "Win32s" :
		Verinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ? "Windows 95" :
		Verinfo.dwPlatformId == VER_PLATFORM_WIN32_NT ? "Windows NT" : "Unknown system",
		Verinfo.dwMajorVersion,
		Verinfo.dwMinorVersion,
		Verinfo.dwBuildNumber & 0xFFFF);

	if (Verinfo.szCSDVersion [0])
		sprintf (s+strlen (s), " [%s]", Verinfo.szCSDVersion);

	SetDlgItemText (hwnd, ABOUT_OS, s);

	SetDlgItemText (hwnd, ABOUT_PROCESSOR,
			sysinfo.dwNumberOfProcessors == 1 ? "Processor:" : "Processors:");

	wsprintf (t, "%ld", (long) sysinfo.dwProcessorType);

#ifdef __BORLANDC__
	proc = sysinfo.u.s.wProcessorArchitecture;
#else
	proc = sysinfo.wProcessorArchitecture;
#endif

	wsprintf (s, "%d %s %s",
		  (int) sysinfo.dwNumberOfProcessors,
		  proc == PROCESSOR_ARCHITECTURE_INTEL ? "Intel":
		  proc == PROCESSOR_ARCHITECTURE_MIPS  ? "MIPS":
		  proc == PROCESSOR_ARCHITECTURE_ALPHA ? "Alpha":
		  proc == PROCESSOR_ARCHITECTURE_PPC ? "Power PC" : "Unknown",
		  sysinfo.dwProcessorType == PROCESSOR_INTEL_386 ? "386":
		  sysinfo.dwProcessorType == PROCESSOR_INTEL_486 ? "486":
		  sysinfo.dwProcessorType == PROCESSOR_INTEL_PENTIUM ? "Pentium":
		  sysinfo.dwProcessorType == PROCESSOR_MIPS_R4000 ? "R4000": t);

	SetDlgItemText (hwnd, ABOUT_PROCESSOR, s);

	memset (&mstatus, 0, sizeof (mstatus));
	mstatus.dwLength = sizeof (mstatus);
	GlobalMemoryStatus (&mstatus);

	sprintf (s, "total: %ldK; available: %ldK",
		 mstatus.dwTotalPhys/1024, mstatus.dwAvailPhys/1024);
	SetDlgItemText (hwnd, ABOUT_PHYSICAL, s);

	sprintf (s, "total: %ldK; available: %ldK",
		 (mstatus.dwTotalPhys + mstatus.dwTotalPageFile)/1024,
		 (mstatus.dwAvailPhys + mstatus.dwAvailPageFile)/1024);

	SetDlgItemText (hwnd, ABOUT_VIRTUAL, s);
}

void	draw_expire_info (HWND hwnd)
{
	char s [300], t[300];
	SYSTEMTIME st;

#ifdef _EDUCVERSION
	SetDlgItemText (hwnd, ABOUT_EXPIRE,
		"This program may only be used for the sole purposes\nof education or personal entertainment");
#else
	if (ExpireDate)	{
		memset (&st, 0, sizeof (st));
		st.wDay   = ExpireDate % 100;
		st.wMonth = ExpireDate / 100 % 100;
		st.wYear  = ExpireDate / 10000;
		GetDateFormat (LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, t, sizeof (t));
		if (HasExpired)
			strcpy (s, "This was a trial version of the software.\nIt has expired on ");
		else
			strcpy (s, "This is a trial version of the software.\nIt expires on ");
		strcat (s, t);
		SetDlgItemText (hwnd, ABOUT_EXPIRE, s);
	} else	{
		ShowWindow (GetDlgItem (hwnd, ABOUT_EXPIRE), SW_HIDE);
		ShowWindow (GetDlgItem (hwnd, ABOUT_EXPIRE_FRAME), SW_HIDE);
	}
#endif
}

void	align_about_dialog (HWND hwnd)
{
	RECT rc1, rc2;
	int dx, dy;

#ifdef _EDUCVERSION
#else
	if (ExpireDate)	{
		SetDlgItemText (hwnd, ABOUT_EDUCATIONAL, "Trial");
		SetDlgItemText (hwnd, ABOUT_EDITION, "Version");
	} else	{
		ShowWindow (GetDlgItem (hwnd, ABOUT_EDUCATIONAL), SW_HIDE);
		ShowWindow (GetDlgItem (hwnd, ABOUT_EDITION), SW_HIDE);
	}
#endif

	shift_wnd  (GetDlgItem (hwnd, IDCANCEL), 10000, 10000);
	expand_wnd (hwnd, about_cx, 0);
	expand_wnd (GetDlgItem (hwnd, ABOUT_PICTURE), about_cx, about_cy);
	expand_wnd (GetDlgItem (hwnd, ABOUT_PICTURE_FRAME), about_cx, about_cy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_HEADER_FRAME), about_cx, 0);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_HEADER_TEXT), about_cx, 0);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_EDUCATIONAL), about_cx, 0);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_EDITION), about_cx, 0);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_VERSION_TEXT), about_cx, 0);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_SHELL_VERSION_TEXT), about_cx, 0);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_COPYRIGHT_TEXT), about_cx, 0);

	Get_child_pos (GetDlgItem (hwnd, ABOUT_PICTURE_FRAME), &rc1);
	Get_child_pos (GetDlgItem (hwnd, ABOUT_HEADER_FRAME), &rc2);
	dy = rc1.bottom - rc2.bottom;
	if (dy < 0) dy = 0;

#ifdef _EDUCVERSION
		shift_wnd  (GetDlgItem (hwnd, ABOUT_EXPIRE_FRAME), 0, dy);
		shift_wnd  (GetDlgItem (hwnd, ABOUT_EXPIRE), 0, dy);
		Get_child_pos (GetDlgItem (hwnd, ABOUT_HEADER_FRAME), &rc1);
		Get_child_pos (GetDlgItem (hwnd, ABOUT_EXPIRE_FRAME), &rc2);
		dx = rc1.right - rc2.right;
		expand_wnd (GetDlgItem (hwnd, ABOUT_EXPIRE), dx, 0);
		expand_wnd (GetDlgItem (hwnd, ABOUT_EXPIRE_FRAME), dx, 0);
#else
	if (ExpireDate)	{
		shift_wnd  (GetDlgItem (hwnd, ABOUT_EXPIRE_FRAME), 0, dy);
		shift_wnd  (GetDlgItem (hwnd, ABOUT_EXPIRE), 0, dy);
		Get_child_pos (GetDlgItem (hwnd, ABOUT_HEADER_FRAME), &rc1);
		Get_child_pos (GetDlgItem (hwnd, ABOUT_EXPIRE_FRAME), &rc2);
		dx = rc1.right - rc2.right;
		expand_wnd (GetDlgItem (hwnd, ABOUT_EXPIRE), dx, 0);
		expand_wnd (GetDlgItem (hwnd, ABOUT_EXPIRE_FRAME), dx, 0);
	} else	{
		Get_child_pos (GetDlgItem (hwnd, ABOUT_OS_FRAME), &rc1);
		Get_child_pos (GetDlgItem (hwnd, ABOUT_EXPIRE_FRAME), &rc2);
		dy -= rc1.top - rc2.top;
	}
#endif
	expand_wnd (hwnd, 0, dy);
	expand_wnd (GetDlgItem (hwnd, ABOUT_PICTURE), 0, about_cy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_OS_FRAME), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_OS_LABEL), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_COMPUTER_LABEL), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_USER_LABEL), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_PROCESSOR_LABEL), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_VIRTUAL_LABEL), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_PHYSICAL_LABEL), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_COMPUTER), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_USER), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_OS), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_PROCESSOR), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_VIRTUAL), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, ABOUT_PHYSICAL), 0, dy);
	shift_wnd  (GetDlgItem (hwnd, IDOK), 0, dy);

	Get_child_pos (GetDlgItem (hwnd, ABOUT_HEADER_FRAME), &rc1);
	Get_child_pos (GetDlgItem (hwnd, ABOUT_OS_FRAME), &rc2);
	dx = rc1.right - rc2.right;
	if (dx > 0)	{
		expand_wnd (GetDlgItem (hwnd, ABOUT_OS_FRAME), dx, 0);
		expand_wnd (GetDlgItem (hwnd, ABOUT_COMPUTER), dx, 0);
		expand_wnd (GetDlgItem (hwnd, ABOUT_OS), dx, 0);
		expand_wnd (GetDlgItem (hwnd, ABOUT_PROCESSOR), dx, 0);
		expand_wnd (GetDlgItem (hwnd, ABOUT_VIRTUAL), dx, 0);
		expand_wnd (GetDlgItem (hwnd, ABOUT_PHYSICAL), dx, 0);
	}
	Get_child_pos (GetDlgItem (hwnd, IDOK), &rc1);
	SetWindowPos (GetDlgItem (hwnd, IDOK), 0,
		      rc1.left+about_cx/2, rc1.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

HFONT	about_big_font;

char * file_version (char * fname, BOOL build)
{
	char * vinfo;
	int s;
	VS_FIXEDFILEINFO *fv;
	static char ver [20];
	UINT verlen;
	DWORD h;

	vinfo = 0;
	s = GetFileVersionInfoSize (fname, &h);
	if (s)
		vinfo = malloc (s);
	if (vinfo && ! GetFileVersionInfo (fname, 0, s, vinfo))	{
		free (vinfo);
		vinfo = 0;
	}
	if (vinfo)	{
		VerQueryValue (vinfo, "\\", (LPVOID*)&fv, &verlen);
		sprintf (ver, "%d.%02d",
				 HIWORD (fv->dwFileVersionMS),
				 LOWORD (fv->dwFileVersionMS));
		if (fv->dwFileFlags & VS_FF_PRERELEASE)
			sprintf (ver+strlen (ver), " (pre-release %d.%d)",
				 HIWORD (fv->dwFileVersionLS),
				 LOWORD (fv->dwFileVersionLS));
		else if (build && LOWORD (fv->dwFileVersionLS) && HIWORD (fv->dwFileVersionLS))
			sprintf (ver+strlen (ver), " build %d.%d",
				 HIWORD (fv->dwFileVersionLS),
				 LOWORD (fv->dwFileVersionLS));
		else if (build && LOWORD (fv->dwFileVersionLS))
			sprintf (ver+strlen (ver), " build %d",
				 LOWORD (fv->dwFileVersionLS));
	} else
		strcpy (ver, "");
	free (vinfo);
	return ver;
}

BOOL CALLBACK AboutProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	char s [300];
	char fname [_MAX_PATH+1];

	switch (msg)	{
	case WM_INITDIALOG:
		if (DemoMode)	{
			GetDlgItemText (hwnd, ABOUT_HEADER_TEXT, s, sizeof (s));
			strcat (s, " DEMO");
			SetDlgItemText (hwnd, ABOUT_HEADER_TEXT, s);
		}
		Make_line (XDSMAIN, fname, sizeof (fname));
		sprintf (s, "Version: %s", file_version (fname, TRUE));
		SetDlgItemText (hwnd, ABOUT_VERSION_TEXT, s);
		GetModuleFileName (MyInstance, fname, _MAX_PATH+1);
		sprintf (s, "Environment: %s", file_version (fname, TRUE));
		SetDlgItemText (hwnd, ABOUT_SHELL_VERSION_TEXT, s);
		draw_os_info (hwnd);
		if (about_big_font) SendMessage (GetDlgItem (hwnd, ABOUT_HEADER_TEXT), WM_SETFONT, (WPARAM) about_big_font, 0);
		align_about_dialog (hwnd);
		draw_expire_info (hwnd);
		OldStaticProc = Subclass_control (GetDlgItem (hwnd, ABOUT_PICTURE),
						  DrawPictureProc);
		Center_window (hwnd);
		return 1;
	case WM_COMMAND:
		if (CONTROL_ID == IDOK || CONTROL_ID == IDCANCEL)
			EndDialog (hwnd, 1);
		return 1;
	case WM_HELP:
		return Dialog_help (ABOUT_DIALOG);
	}
	return 0;
}

void	Help_about (void)
{
	BITMAP b;
	int aboutcnt = 1;

	Verinfo.dwOSVersionInfoSize = sizeof (Verinfo);
	GetVersionEx (&Verinfo);
	GetSystemInfo (&sysinfo);
	
	AboutBmp = LoadResourceBitmap (ResInstance, MAKEINTRESOURCE (BMP_ABOUT), &AboutPalette);
	if (!AboutBmp)
		about_cx = about_cy = 0;
	else	{
		GetObject (AboutBmp, sizeof (b), &b);
		about_cx = b.bmWidth;
		about_cy = b.bmHeight;
	}

	about_big_font = CreateFont (SystemFontHeight * 2,
			 0,0,0,FW_BOLD,0,0,0,0,0,0,PROOF_QUALITY,0,
			 "MS Sans Serif");


	OldPalette = NULL;
	Dialog (ABOUT_DIALOG, AboutProc);
	if (about_big_font) DeleteObject (about_big_font);
{
	HDC dc = GetDC (0);

	SelectPalette (dc, OldPalette, FALSE);
	RealizePalette (dc);
	ReleaseDC (NULL, dc);
}
	DeleteObject (AboutBmp);
	DeleteObject (AboutPalette);
}

void	Get_version_information (void)
{
	Verinfo.dwOSVersionInfoSize = sizeof (Verinfo);
	GetVersionEx (&Verinfo);
	GetSystemInfo (&sysinfo);
}
