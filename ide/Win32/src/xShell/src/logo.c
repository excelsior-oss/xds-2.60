#include "util.h"
#include "shell.h"
#include "logo.h"
#include "res.h"
#include "xdsres.h"
#include "classes.h"


HBITMAP bmp_logo = 0;
HPALETTE palette_logo = 0;

int	dx_logo, dy_logo;

UINT	logo_timer_id;
HWND	hwnd_logo;
BOOL	may_hide_logo = FALSE;
#if 0
void	draw_demo (void)
{
	int h;
	HDC dc, bmpdc;
	HBITMAP b;
	HFONT f;
	SIZE size;
	RECT rc;

	h = dy_logo / 3;
	f = CreateFont (h, 0, 300, 300, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, VARIABLE_PITCH|FF_SWISS, "Arial");
	if (!f) return;
	dc = GetDC (0);
	bmpdc = CreateCompatibleDC (dc);
	ReleaseDC (0, dc);
	b = SelectObject (bmpdc, bmp_logo);
	f = SelectObject (bmpdc, f);
	GetTextExtentPoint32 (bmpdc, "DEMO", 4, &size);
	rc.top = rc.left = 20;
	rc.right = dx_logo - 20;
	rc.bottom = dy_logo - 20;
	SetTextAlign (bmpdc, TA_CENTER);
	TextOut (bmpdc, dx_logo/2, dy_logo/2, "DEMO", 4);
	SelectObject (bmpdc, f);
	SelectObject (bmpdc, b);
	DeleteDC (bmpdc);
	DeleteObject (f);
}
#endif
BOOL CALLBACK LogoDialogProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC dc, bmpdc;

	switch (msg)	{
	case WM_INITDIALOG:
		MoveWindow (hwnd, 0, 0, dx_logo, dy_logo, 0);
		Center_window (hwnd);
		logo_timer_id = SetTimer (hwnd, 1, 2500, 0);
		SetFocus (hwnd);
//		SetCapture (hwnd);
		return 0;
	case WM_PAINT:
		dc = BeginPaint (hwnd, &ps);
		bmpdc = CreateCompatibleDC (dc);
		SelectPalette  (dc, palette_logo, FALSE);
		RealizePalette (dc);
		SelectPalette  (bmpdc, palette_logo, FALSE);
		RealizePalette (bmpdc);
		SelectObject (bmpdc, bmp_logo);
		BitBlt (dc, 0, 0, dx_logo, dy_logo,
			bmpdc, 0, 0, SRCCOPY);
		RealizePalette (dc);
		DeleteDC (bmpdc);
		EndPaint (hwnd, &ps);
		return 0;
	case WM_TIMER:
		if (logo_timer_id) KillTimer (hwnd, logo_timer_id);
		logo_timer_id = 0;
		if (may_hide_logo)
			DestroyWindow (hwnd);
		return 1;
	case WM_KEYDOWN:
	case WM_LBUTTONDOWN:
		if (logo_timer_id) KillTimer (hwnd, logo_timer_id);
		logo_timer_id = 0;
		DestroyWindow (hwnd);
		return 1;
	case WM_DESTROY:
		if (logo_timer_id) KillTimer (hwnd, logo_timer_id);
		logo_timer_id = 0;
		DeleteObject (bmp_logo);
		DeleteObject (palette_logo);
		hwnd_logo = 0;
		return 1;
	case WM_ACTIVATE:
		if (WND_TO_DEACTIVATE == hwnd)
			DestroyWindow (hwnd);
		return 1;
	}
	return 0;
}

void	Display_logo (void)
{
        BITMAP b;
	HDC dc;
	int n, m;

	dc = GetDC (0);
	n = GetDeviceCaps (dc, PLANES);
	m = GetDeviceCaps (dc, BITSPIXEL);
	if (n * m <= 4) return;

	bmp_logo = LoadResourceBitmap (ResInstance, MAKEINTRESOURCE (BMP_LOGO), &palette_logo);

        if (! GetObject (bmp_logo, sizeof (b), &b))  {
                DeleteObject (bmp_logo);
                return;
        }
        dx_logo = b.bmWidth;
        dy_logo = b.bmHeight;

	hwnd_logo = CreateDialog (MyInstance, MAKEINTRESOURCE (LOGO_DIALOG), NULL, LogoDialogProc);
	if (! hwnd_logo)	{
		DeleteObject (bmp_logo);
		DeleteObject (palette_logo);
	}
}

void	Hide_logo (void)
{
	may_hide_logo = TRUE;
	if (hwnd_logo && ! logo_timer_id)
		DestroyWindow (hwnd_logo);
	EnableWindow (FrameWindow, TRUE);
}

void	Hide_logo_now (void)
{
	if (hwnd_logo)
		DestroyWindow (hwnd_logo);
}

