#include "util.h"
#include "fileutil.h"
#include "shell.h"
#include "tbuf.h"
#include "edit.h"
#include "fcache.h"
#include "config.h"

/* ----------------------- Various file operations ------------------ */

/* loading file into continuous memory buffer*/

BOOL	Load_file (char * name, FILEMAP * map, BOOL copy)
{
	char * ptr, * p;
	DWORD len, lenh, lenr;

	if (!DoReadFileMap) copy = TRUE;
	SetErrorMode (SEM_FAILCRITICALERRORS);
	memset (map, 0, sizeof (FILEMAP));
	map->fhandle = CreateFile (name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	ErrCode = GetLastError ();
	if (map->fhandle == INVALID_HANDLE_VALUE)
		return FALSE;
	len = GetFileSize (map->fhandle, &lenh);
	ErrCode = GetLastError ();
	if (len == 0xFFFFFFFF && ErrCode != NO_ERROR || lenh)	{
		if (lenh) ErrCode = ERROR_NOT_ENOUGH_MEMORY;
		CloseHandle (map->fhandle);
		map -> fhandle = 0;
		return FALSE;
	}
	if (!len)	{
		CloseHandle (map->fhandle);
		map -> fhandle = 0;
		map -> ptr = Malloc (1);
		map -> len = 0;
		if (!map->ptr)	{
			ErrCode = ERROR_NOT_ENOUGH_MEMORY;
			return FALSE;
		}
		return TRUE;
	}
	if (! DoReadFileMap)	{
		p = malloc (len);
		if (!p)	{
			CloseHandle (map->fhandle);
			map -> fmap = map -> fhandle = 0;
			ErrCode = ERROR_NOT_ENOUGH_MEMORY;
			return FALSE;
		}
		if (!ReadFile (map->fhandle, p, len, &lenr, 0) || lenr < len)	{
			ErrCode = GetLastError ();
			if (!ErrCode) ErrCode = ERROR_READ_FAULT;
			CloseHandle (map -> fhandle);
			CloseHandle (map->fhandle);
			map -> fhandle = 0;
			free (p);
			return FALSE;
		}
		CloseHandle (map->fhandle);
		map -> fhandle = 0;
		map -> ptr = p;
		map -> len = len;
		return TRUE;
	}

	map->fmap = CreateFileMapping (map->fhandle, NULL, PAGE_READONLY, 0, 0, NULL);
	ErrCode = GetLastError ();

	if (! map->fmap)	{
		CloseHandle (map->fhandle);
		map -> fhandle = 0;
		return FALSE;
	}
	ptr = MapViewOfFile (map->fmap, FILE_MAP_READ, 0, 0, 0);
	ErrCode = GetLastError ();
	if (! ptr)	{
		CloseHandle (map->fmap);
		CloseHandle (map->fhandle);
		map -> fmap = map -> fhandle = 0;
		return FALSE;
	}
	if (copy)	{
		p = malloc (len);
		if (!p)	{
			UnmapViewOfFile (ptr);
			CloseHandle (map->fmap);
			CloseHandle (map->fhandle);
			map -> fmap = map -> fhandle = 0;
			ErrCode = ERROR_NOT_ENOUGH_MEMORY;
			return FALSE;
		}
		__try {
			memmove (p, ptr, len);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)	{
			UnmapViewOfFile (ptr);
			CloseHandle (map->fmap);
			CloseHandle (map->fhandle);
			map -> fmap = map -> fhandle = 0;
			ErrCode = ERROR_READ_FAULT;
			return FALSE;
		}
		UnmapViewOfFile (ptr);
		CloseHandle (map->fmap);
		CloseHandle (map->fhandle);
		map -> fmap = map -> fhandle = 0;
		map -> ptr = p;
	} else
		map -> ptr = ptr;
	map -> len = len;

	return TRUE;
}

void	Unload_file (FILEMAP * map)
{
	if (map -> fhandle)	{
		UnmapViewOfFile (map->ptr);
		CloseHandle (map->fmap);
		CloseHandle (map->fhandle);
	} else
		free (map -> ptr);
}

BOOL	Test_file_exists (char * name)
{
	DWORD attr = GetFileAttributes (name);
	return  attr != 0xFFFFFFFF &&
		! (attr & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL	Test_file_readable (char * name)
{
	FILEMAP map;
	if (!Load_file (name, &map, FALSE))
		return FALSE;
	Unload_file (&map);
	set_real_file_name (name);
	return TRUE;
}

BOOL	Prepare_save_file (char * name, FILEMAP * map, unsigned len)
{
	memset (map, 0, sizeof (FILEMAP));
	map->fhandle = CreateFile (name, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
	if (map->fhandle == INVALID_HANDLE_VALUE)	{
		ErrCode = GetLastError ();
		return FALSE;
	}
	if (! len)	{
		map->fmap = 0;
		map -> ptr = 0;
		return TRUE;
	}
	if (!DoWriteFileMap)	{
		map->ptr = Malloc (len);
		if (!map->ptr)	{
			ErrCode = ERROR_NOT_ENOUGH_MEMORY;
			CloseHandle (map->fhandle);
			map -> fhandle = 0;
			return FALSE;
		}
		map->len = len;
		return TRUE;
	}

	map->fmap = CreateFileMapping (map->fhandle, NULL, PAGE_READWRITE, 0, len, NULL);
	if (! map->fmap)	{
		ErrCode = GetLastError ();
		CloseHandle (map->fhandle);
		map -> fhandle = 0;
		return FALSE;
	}
	map->ptr = MapViewOfFile (map->fmap, FILE_MAP_WRITE, 0, 0, 0);
	if (! map->ptr)	{
		ErrCode = GetLastError ();
		CloseHandle (map->fmap);
		CloseHandle (map->fhandle);
		map -> fmap = map -> fhandle = 0;
		return FALSE;
	}
	map->len = len;
	return TRUE;
}

BOOL	Finish_save_file (FILEMAP * map)
{
	DWORD lenr;
	if (!DoWriteFileMap)	{
		if (!WriteFile (map->fhandle, map->ptr, map->len, &lenr, 0) || lenr<map->len)	{
			ErrCode = GetLastError ();
			CloseHandle (map->fhandle); map->fhandle = 0;
			free (map->ptr); map->ptr = 0;
			return FALSE;
		}
		free (map->ptr); map->ptr = 0;
	} else	{
		if (map->ptr) UnmapViewOfFile (map->ptr); map->ptr = 0;
		if (map->fmap) CloseHandle (map->fmap); map->fmap = 0;
	}
	if (map->fhandle && !CloseHandle (map->fhandle))	{
		ErrCode = GetLastError ();
		return FALSE;
	}
	map->fhandle = 0;
	return TRUE;
}

BOOL	MakeBakFile (char * f)
{
	char bak [STRLEN];
	int r;
	DWORD err;

	if (GetFileAttributes (f) == 0xFFFFFFFF) return TRUE;
	if (long_filenames (f))	{
		strcpy (bak, f);
		strcat (bak, ".bak");
	} else
		change_ext (f, bak, "bak");
	if (!_stricmp (f, bak)) return TRUE;
again_move:
	if (!DeleteFile (bak))	{
		err = GetLastError ();
		if (err != ERROR_FILE_NOT_FOUND)	{
			r = MsgBox ("Write file", MB_RETRYCANCEL | MB_ICONEXCLAMATION,
				    "Can't delete old %s\n%s", bak, EText (err));
			if (r == IDRETRY) goto again_move;
			ErrCode = ERROR_CANCELLED;
			return FALSE;
		}
	}

	if (!MoveFile (f, bak))	{
		r = MsgBox ("Write file", MB_RETRYCANCEL | MB_ICONEXCLAMATION,
				    "Can't rename %s into %s\n%s", f, bak, LastErrText ());
		if (r == IDRETRY) goto again_move;
		ErrCode = ERROR_CANCELLED;
		return FALSE;
	}
	return TRUE;
}

/* ------------------------------------------------------------------ */

BOOL	Create_dirs (int ndirs, char ** dirlist)
{
	int i, r;
	DWORD attr;
	for (i = 0; i < ndirs; i++)	{
		retry:
		attr = GetFileAttributes (dirlist [i]);
		if (attr != 0xFFFFFFFF)	{
			if (attr & FILE_ATTRIBUTE_DIRECTORY) continue;
			r = MsgBox ("Error", MB_ICONSTOP | MB_ABORTRETRYIGNORE,
				    "Can't create directory %s\nbecause there is file with the same name", dirlist [i]);
			if (r == IDABORT)	{
				ErrCode = ERROR_CANCELLED;
				return FALSE;
			}
			if (r == IDRETRY) goto retry;
			continue;
		}
		if (!CreateDirectory (dirlist [i], 0))	{
			ErrCode = GetLastError ();
			r = MsgBox ("Error", MB_ICONSTOP | MB_ABORTRETRYIGNORE,
				    "Can't create directory %s:\n%s", dirlist [i], ErrText ());
			if (r == IDABORT)	{
				ErrCode = ERROR_CANCELLED;
				return FALSE;
			}
			if (r == IDRETRY) goto retry;
		}
	}
	return TRUE;
}

BOOL    Test_overwrite (char * name, BOOL prompt)
{
	DWORD attr;
	char * s;
	int r;
	char f [2000];
	LPSTR name_portion;

	GetFullPathName (name, sizeof (f), f, &name_portion);

again:
	attr = GetFileAttributes (f);
	ErrCode = GetLastError ();
	if (ErrCode == ERROR_FILE_NOT_FOUND || ErrCode == ERROR_PATH_NOT_FOUND)	{
		ErrCode = 0;
		return TRUE;
	}

	if (attr == 0xFFFFFFFF)	return FALSE;
	s = 0;
	if (attr & FILE_ATTRIBUTE_SYSTEM)    s = "system file";
	if (attr & FILE_ATTRIBUTE_READONLY)  s = "read only file";
	if (attr & FILE_ATTRIBUTE_DIRECTORY) s = "directory";
        if (s)	{
		r = MsgBox ("Write file", MB_RETRYCANCEL | MB_ICONEXCLAMATION,
			    "File %s is a %s\nCan't write over it", f, s);
		if (r == IDRETRY) goto again;
		return FALSE;
	}
	if (prompt)	{
		r = MsgBox ("Write file", MB_YESNO | MB_ICONQUESTION,
			     "File %s already exists.\nDo you want to overwrite it?",
			     name);
		if (r == IDNO) return FALSE;
	}
	return TRUE;
}

BOOL	create_file_path (char * name)
{
	char f [STRLEN];
	char * p;
	LPSTR name_portion;
	DWORD attr;
	int r;

	GetFullPathName (name, sizeof (f), f, &name_portion);
	p = f;
	if (p [0] == '\\' && p [1] == '\\')	{
		for (p = f+2; *p && *p != '\\'; p++);
		if (!*p) return FALSE;
		p++;
		while (*p && *p != '\\') ++p;
	}

	for (; p < name_portion; p++)
		if (*p == '\\' && p>f && p[-1]!=':' && p[-1] != '\\')	{
			*p = 0;
			retry:
			attr = GetFileAttributes (f);
			if (attr == 0xFFFFFFFF)	{
				ErrCode = GetLastError ();
				if (ErrCode != ERROR_FILE_NOT_FOUND)	{
					r = MsgBox ("Write file", MB_RETRYCANCEL | MB_ICONEXCLAMATION,
						"Error verifying directory %s\n%s", f, ErrText ());
					if (r == IDRETRY) goto retry;
					ErrCode = ERROR_CANCELLED;
					return FALSE;
				}
				if (! CreateDirectory (f, 0))	{
					ErrCode = GetLastError ();
					r = MsgBox ("Write file", MB_RETRYCANCEL | MB_ICONEXCLAMATION,
						"Can't create directory %s\n%s", f, ErrText ());
					if (r == IDRETRY) goto retry;
					ErrCode = ERROR_CANCELLED;
					return FALSE;
				}
			} else if (! (attr & FILE_ATTRIBUTE_DIRECTORY))	{
				r = MsgBox ("Write file", MB_RETRYCANCEL | MB_ICONEXCLAMATION,
						"Can't create directory %s\n"
						"There is a file with the same name", f);
				if (r == IDRETRY) goto retry;
				ErrCode = ERROR_CANCELLED;
				return FALSE;
			}
			*p = '\\';
		}
	return TRUE;
}

/* -------------------------------------------------------------------------- */

void	free_map_fun (void * data)
{
	FILEMAP * map = (FILEMAP*) data;
	Unload_file (map);
	free (map);
}

static BOOL isTextOem(const unsigned char *text, int len)
{
  int oemRate = 0, ansiRate = 0;
  int i = 0;
  for(; i < len; ++i) {
    if(text[i] >= 0x80 /*'A'*/ && text[i] <= 0xAF /*'ï'*/ ||
      text[i] >= 0xE0 /*'ð'*/ && text[i] <= 0xEF /*'ÿ'*/ ||
      text[i] == 0xB3 /*'|'*/ || text[i] == 0xBA /*||*/)
      ++oemRate;
    if(text[i] >= 0xC0 /*'A'*/)
      ++ansiRate;
  }
  return oemRate > ansiRate;
}

TBUF *	r_file_to_buf (int file, BOOL copy, unsigned long *len)
{
	TBUF * buf;
	FILEMAP * map;

	map = Malloc (sizeof (FILEMAP));
	if (!map) return NULL;
	if (!Load_file (filedir [file], map, copy))	{
		free (map);
		return NULL;
	}
	if (len) *len = map->len;
	set_real_file_name (filedir [file]);
	buf = create_tbuf (TabSize);
	if (!buf)	{
		Unload_file (map);
		free (map);
		ErrCode = ERROR_NOT_ENOUGH_MEMORY;
		return NULL;
	}
	if (!tbuf_set_text (buf, map->ptr, map->len, FALSE, ReadEOF))	{
		free_tbuf (buf);
		Unload_file (map);
		free (map);
		ErrCode = ERROR_NOT_ENOUGH_MEMORY;
		return NULL;
	}
  if (isTextOem (map->ptr, map->len)) {
    tbuf_set_oem (buf, TRUE);
    tbuf_oem_to_ansi (buf);
  }
	tbuf_add_destructor (buf, map, free_map_fun);
	return buf;
}

TBUF *	Read_file_to_buf (int file, BOOL copy, unsigned long * len)
{
	TBUF * buf;
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));
	buf = r_file_to_buf (file, copy, len);
	SetCursor (cur);
	return buf;
}

BOOL	write_fun (char * s, int len, DWORD p)
{
	memcpy (*(char**)p, s, len);
	*(char**)p += len;
	return TRUE;
}

BOOL	w_file_from_buf (TBUF * buf, int file, BOOL prompt_overwrite)
{
	FILEMAP map;
	int len, r;
	char * ptr;
	char f [STRLEN], d [STRLEN], tf [STRLEN];
	LPSTR name_portion;
	DWORD err;
	EOL_TYPE eol_type;
	char * eol;
	int eollen;

	if (!create_file_path (filedir [file]))
		return FALSE;
	
	if (!Test_overwrite (filedir [file], prompt_overwrite))
		return FALSE;

	GetFullPathName (filedir [file], sizeof (f), f, &name_portion);
	strcpy (d, f);
	dir_only (d);
	if (d[strlen(d)-1] == ':') strcat (d, "\\");
	if (!GetTempFileName (d, "xds", 0, tf))	{
		ErrCode = GetLastError ();
		return FALSE;
	}
	eol_type = tbuf_eol_type (buf);
	if (eol_type == EOL_DEFAULT || !Autodetect_eol) eol_type = Default_eol_type;
	eollen = 1;
	switch (eol_type)	{
	case EOL_ZERO: eol = ""; break;
	case EOL_CR: eol = "\r"; break;
	case EOL_LF: eol = "\n"; break;
	case EOL_CRLF:
	default:
		eol = "\r\n";
		eollen = 2;
	}

	len = tbuf_stream_out (buf, eol, eollen, 0, 0);
	if (WriteEOF) ++len;
	if (!Prepare_save_file (tf, &map, len))	{
		DeleteFile (tf);
		return FALSE;
	}
	ptr = map.ptr;
	tbuf_stream_out (buf, eol, eollen, write_fun, (DWORD)&ptr);
	if (WriteEOF) *ptr = 0x1A;
  if (tbuf_is_oem(buf))
    ansiToOem(map.ptr, len - 1);
	if (!Finish_save_file (&map))
		return FALSE;
	if (BakFiles)	{
		if (! MakeBakFile (f))	{
			DeleteFile (tf);
			return FALSE;
		}
	} else	{
		again_delete:
		if (!DeleteFile (f))	{
			err = GetLastError ();
			if (err != ERROR_FILE_NOT_FOUND)	{
				r = MsgBox ("Write file", MB_RETRYCANCEL | MB_ICONEXCLAMATION,
					    "Can't write over old %s\n%s", f, LastErrText ());
				if (r == IDRETRY) goto again_delete;
				ErrCode = ERROR_CANCELLED;
				return FALSE;
			}
		}
	}
again_move:
	if (!MoveFile (tf, f))	{
		r = MsgBox ("Write file", MB_RETRYCANCEL | MB_ICONEXCLAMATION,
			    "Can't rename %s into %s\n%s", tf, f, LastErrText ());
		if (r == IDRETRY) goto again_move;
		ErrCode = ERROR_CANCELLED;
		return FALSE;
	}
	tbuf_set_eol_type (buf, eol_type);
	return TRUE;
}

BOOL	Write_file_from_buf (TBUF * buf, int file, BOOL prompt_overwrite)
{
	BOOL r;
	HCURSOR cur = GetCursor ();
	SetCursor (LoadCursor (0, IDC_WAIT));
	r = w_file_from_buf (buf, file, prompt_overwrite);
	SetCursor (cur);
	return r;
}

/* ---------------- Working with files in editor buffers ---------- */

TBUF *	Find_or_read_file (int file)
{
	TBUF * buf;
	HWND	hwnd;

	hwnd = Edit_find_loaded_file (file);
	if (hwnd)
		buf = Edit_get_buf (hwnd);
	else
		buf = Read_file_to_buf (file, TRUE, NULL);
	if (buf)
		tbuf_addref (buf);
	return buf;
}

static	TBUF *	save_buf;

BOOL	Put_file_start (void)
{
	save_buf = create_tbuf (TabSize);
	if (!save_buf)	{
		ErrCode = ERROR_NOT_ENOUGH_MEMORY;
		return FALSE;
	}
	ErrCode = 0;
	return TRUE;
}

BOOL	Put_file (char * buf, int len)
{
	if (!tbuf_add_line (save_buf, buf, len, TRUE))	{
		free_tbuf (save_buf);
		ErrCode = ERROR_NOT_ENOUGH_MEMORY;
		return FALSE;
	}
	return TRUE;
}

BOOL	Put_file_fin (int file, BOOL test_overwrite)
{
	HWND hwnd;
	BOOL b;

	hwnd = Edit_find_loaded_file (file);
	if (hwnd)	{
		Edit_set_buf (hwnd, save_buf);
		tbuf_set_dirty (save_buf, TRUE);
		return TRUE;
	} else	{
		b = Write_file_from_buf (save_buf, file, test_overwrite);
		free_tbuf (save_buf);
		return b;
	}
}
