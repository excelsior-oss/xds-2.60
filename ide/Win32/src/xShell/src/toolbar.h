HWND	Make_empty_toolbar (HWND parent);
void	Add_toolbar_button (HWND bar, HBITMAP bmp, HBITMAP dbmp, char * string, int cmd, int pause, int after);
void	Add_toolbar (HWND bar, int Resource, int cmd, int pause, int after);
void	Delete_toolbar_buttons (HWND bar, int from, int to);
void	Enable_toolbar_button (HWND bar, int cmd, BOOL enable);
void	show_toolbar_button (HWND bar, int cmd, BOOL show);
void	Bind_status_bar (HWND toolbar, HWND statusbar);
void	Redither_toolbar (HWND bar);

void	Set_flat_toolbar (HWND toolbar, BOOL flat);
