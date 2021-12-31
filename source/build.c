#define BUILD_WIN32 1

//~ Windows headers

// NOTE(geni): 0x0601 = Windows 7
#define UNICODE
#define WINVER       0x0601
#define _WIN32_WINNT 0x0601
#define RPC_NO_WINDOWS_H
#define VC_EXTRALEAN
#define NOATOM
#define NOHELP
#define NOPROFILER
#define NOGDICAPMASKS
#define NOATOM
#define NOSERVICE
#define NOMETAFILE
#define NOMINMAX
#define NOKANJI
#define NODRAWTEXT
#define NOCOLOR
#define NOSYSMETRICS
#define NOMENUS
#define NOKEYSTATES
#define NOWH
#define NOCOMM
#define NOMCX
#define NOIME
#define NODEFERWINDOWPOS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define PSAPI_VERSION 2
#include <Windows.h>
#include <Tlhelp32.h>
#undef DeleteFile
#undef CopyFile

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "language_layer.h"

#include "memory.h"
#include "string8.h"
#include "string16.h"

#define STB_SPRINTF_STATIC
#define STB_SPRINTF_IMPLEMENTATION
#include "ext/stb_sprintf.h"

#include "win32/win32_memory.c"
#include "memory.c"
#include "cstrings.c"
#include "string8.c"
#include "string16.c"
#include "log.c"
#include "win32/win32_utilities.c"
#include "win32/win32_file_io.c"

#include "main.c"
