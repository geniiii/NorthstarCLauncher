//~ String creation functions

internal inline String16 S16_FromWString(u16* wstr) {
	return (String16){
		.s    = wstr,
		.size = wcslen(wstr),
	};
}

internal String16 S16_PushFV(MArena* arena, const u16* fmt, va_list args) {
	// TODO(geni): _vsnwprintf is MSVC-specific and requires including stdio.h
	
	va_list args2;
	va_copy(args2, args);
	u64 needed_bytes = _vsnwprintf(0, 0, fmt, args) + 1;
	String16 result;
	{
		result.s              = MArenaPushTypeN(arena, u16, needed_bytes);
		result.size           = _vsnwprintf(result.s, needed_bytes, fmt, args2);
		result.s[result.size] = L'\0';
	}
	return result;
}

internal String16 S16_PushF(MArena* arena, const u16* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	String16 result = S16_PushFV(arena, fmt, args);
	va_end(args);
	return result;
}

internal b32 S16_Match(String16 a, String16 b) {
    b32 result = 0;
	
    if (a.size == b.size) {
        result = 1;
        for (u64 i = 0; i < a.size; ++i) {
            if (a.s[i] != b.s[i]) {
                result = 0;
				break;
            }
        }
    }
	
    return result;
}
