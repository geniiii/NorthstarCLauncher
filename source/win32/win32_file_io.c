//~ File operations

internal b32 LoadEntireFile(MArena* arena, String16 path, void** data, u64* data_size) {
    *data      = 0;
    *data_size = 0;
	
	SetLastError(0);
	
    HANDLE file = INVALID_HANDLE_VALUE;
	if ((file = CreateFileW(path.s,
							GENERIC_READ,  // Desired access
							0,             // Share mode
							NULL,          // Security attributes
							OPEN_EXISTING, // Creation disposition
							0,             // Flags and attributes
							0              // Template file
							)) != INVALID_HANDLE_VALUE) {
		u64 file_size;
		if (!GetFileSizeEx(file, (LARGE_INTEGER*) &file_size)) {
			// NOTE(geni): Failed to get file size
			goto fail;
		}
		
		void* read_data  = MArenaPush(arena, file_size + 1);
		DWORD bytes_read;
		
		if (!ReadFile(file, read_data, (DWORD) file_size, &bytes_read, NULL)) {
			// NOTE(geni): ERROR_IO_PENDING is technically not an error
			if (GetLastError() != ERROR_IO_PENDING) {
				// NOTE(geni): Failed to read file
				goto fail;
			}
		}
		
		((u8*) read_data)[file_size] = '\0';
		
		*data      = read_data;
		*data_size = bytes_read;
		
		CloseHandle(file);
		
		return 0;
	}
	
	fail:
	OutputErrorMessageS16(L"Northstar Launcher Error", L"Could not read \"%.*s\"\nError code: %lu", S16VArg(path), GetLastError());
	return 1;
}

internal b32 FileExists(String16 path) {
	return GetFileAttributesW(path.s) != INVALID_FILE_ATTRIBUTES;
}
