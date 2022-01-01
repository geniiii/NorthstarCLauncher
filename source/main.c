enum Flags {
	Flags_Vanilla         = (1 << 0),
	Flags_NoOriginStartup = (1 << 1),
};
global u32 flags;

typedef b8  (*NorthstarInitFn)(void);
typedef i32 (*LauncherMainFn)(HINSTANCE, HINSTANCE, LPSTR, i32);

internal b32 IsOriginRunning(void) {
	// NOTE(geni): OpenProcess doesn't let us open processes created by other users, even with PROCESS_QUERY_LIMITED_INFORMATION, so we can't use EnumProcesses :(
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		OutputErrorMessage("Northstar Launcher Error", "Failed to create snapshot");
        return 0;
	}
	
    PROCESSENTRY32W process_entry = {
		.dwSize = sizeof process_entry,
	};
    if (!Process32First(snapshot, &process_entry)) {
		OutputErrorMessage("Northstar Launcher Error", "Failed to get first process");
        return 0;
	}
	
    while (Process32Next(snapshot, &process_entry)) {
		String16 process_name = S16_FromWString(process_entry.szExeFile);
        if (S16_Match(S16Lit(L"OriginClientService.exe"), process_name) || S16_Match(S16Lit(L"EADesktop.exe"), process_name)) {
            CloseHandle(snapshot);
            return 1;
        }
    }
	
    CloseHandle(snapshot);
    return 0;
}

internal b32 StartOrigin(void) {
	HKEY key;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Origin", 0, KEY_READ, &key) != ERROR_SUCCESS) {
		OutputErrorMessage("Northstar Launcher Error", "Failed to open Origin registry key");
		return 0;
	}
	
	DWORD size = 0;
	if (RegGetValueW(key, NULL, L"ClientPath", RRF_RT_REG_SZ, NULL, NULL, &size) != ERROR_SUCCESS) {
		OutputErrorMessage("Northstar Launcher Error", "Failed to get size of Origin path");
		return 0;
	}
	wchar* path = MArenaPushTypeN(&g_arena, wchar, size);
	if (RegGetValueW(key, NULL, L"ClientPath", RRF_RT_REG_SZ, NULL, path, &size) != ERROR_SUCCESS) {
		OutputErrorMessage("Northstar Launcher Error", "Failed to get Origin path");
		return 0;
	}
	
	STARTUPINFOW startup_info = {
		.cb = sizeof startup_info,
	};
	PROCESS_INFORMATION process_info;
	if (CreateProcessW(path, NULL, NULL, NULL, 0, CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &startup_info, &process_info)) {
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);
	} else {
		OutputErrorMessage("Northstar Launcher Error", "Failed to create Origin process");
		return 0;
	}
	
	MArenaPopTypeN(&g_arena, wchar, size);
	while (!IsOriginRunning()) {
		Log("Waiting for Origin to launch...");
		Sleep(100);
	}
	
	return 1;
}

i32 main(i32 args_count, char** args) {
	g_arena = MArenaNew();
	
	// NOTE(geni): Set CONOUT handle
	{
		g_conout_handle = CreateFileA("CONOUT$", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		SoftAssert(g_conout_handle != INVALID_HANDLE_VALUE);
		SoftAssert(SetStdHandle(STD_OUTPUT_HANDLE, g_conout_handle));
		SoftAssert(SetStdHandle(STD_ERROR_HANDLE,  g_conout_handle));
	}
	
	if (args_count >= 2) {
		if (S8_MatchCString(S8Lit("-h"), args[1])) {
			String8 base_name = S8_PathToBaseName(S8_FromCString(args[0]));
			printf("Usage: %.*s [-h] [[-vanilla] [-dedicated] [-noOriginStartup]]", S8VArg(base_name));
			return 0;
		}
		
		for (i32 i = 1; i < args_count; ++i) {
			String8 arg = S8_FromCString(args[i]);
			if (S8_Match(S8Lit("-vanilla"), arg)) {
				flags |= Flags_Vanilla;
			} else if (S8_Match(S8Lit("-dedicated"), arg) ||
					   S8_Match(S8Lit("-noOriginStartup"), arg)) {
				flags |= Flags_NoOriginStartup;
			}
		}
	}
	
	// NOTE(geni): Start origin if missing NoOriginStartup flag and Origin isn't already running
	if (!(flags & Flags_NoOriginStartup) && !IsOriginRunning()) {
		if (!StartOrigin()) {
			return 1;
		}
	}
	
	HMODULE tier0_module = LoadLibraryExW(L".\\bin\\x64_retail\\tier0.dll", 0, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!tier0_module) {
		OutputErrorMessage("Northstar Launcher Error", "Failed to load tier0.dll\nError code: %lu", GetLastError());
		return 1;
	}
	
	b32 load_northstar = !(flags & Flags_Vanilla);
	if (load_northstar && FileExists(S16Lit(L"run_northstar.txt"))) {
		u8* data;
		u64 data_size;
		if (!LoadEntireFile(&g_arena, S16Lit(L"run_northstar.txt"), &data, &data_size)) {
			return 1;
		}
		
		if (data_size != 0 && !CStringToI32((char*) data)) {
			load_northstar = 0;
		}
	}
	
	if (load_northstar) {
		HMODULE module = LoadLibraryW(L"Northstar.dll");
		if (!module) {
			OutputErrorMessage("Northstar Launcher Error", "Failed to load Northstar.dll\nError code: %lu", GetLastError());
			return 1;
		}
		NorthstarInitFn init = (NorthstarInitFn) GetProcAddress(module, "InitialiseNorthstar");
		if (init == NULL) {
			OutputErrorMessage("Northstar Launcher Error", "Failed to get Northstar initialisation function from Northstar.dll\nError code: %lu", GetLastError());
			return 1;
		}
		init();
	}
	
	HMODULE launcher_module = LoadLibraryExW(L".\\bin\\x64_retail\\launcher.dll", 0, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!launcher_module) {
		OutputErrorMessage("Northstar Launcher Error", "Failed to load launcher.dll\nError code: %lu", GetLastError());
		return 1;
	}
	LauncherMainFn launcher_main = (LauncherMainFn) GetProcAddress(launcher_module, "LauncherMain");
	
	return launcher_main(NULL, NULL, NULL, 0);
}
