//~ String creation functions

internal inline String8 S8_FromCString(char* str) {
    return (String8){
        .cstr = str,
        .size = strlen(str),
    };
}

internal String8 S8_PushFV(MArena* arena, const char* format, va_list args) {
	va_list args2;
	va_copy(args2, args);
	u32 needed_bytes = stbsp_vsnprintf(0, 0, format, args) + 1;
	String8 result;
	{
		result.s              = MArenaPush(arena, needed_bytes);
		result.size           = stbsp_vsnprintf(result.cstr, needed_bytes, format, args2);
		result.s[result.size] = '\0';
	}
	return result;
}

internal String8 S8_PushF(MArena* arena, const char* format, ...) {
	va_list args;
	va_start(args, format);
	String8 result = S8_PushFV(arena, format, args);
	va_end(args);
	return result;
}

//~ String matching functions

internal inline b32 S8_IsEmpty(String8 str) {
	return str.s == NULL || str.size == 0 || str.s[0] == '\0';
}

internal b32 S8_Match(String8 a, String8 b) {
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

internal b32 S8_MatchCString(String8 a, const char* b) {
	const char* iter = b;
    b32 result = 1;
	
	for (u64 i = 0; i < a.size && *iter; ++i, ++iter) {
		if (a.s[i] != *iter) {
			result = 0;
			break;
		}
	}
	if ((u64) (iter - b) != a.size) {
		result = 0;
	}
	
    return result;
}

//~ Find occurrence functions

internal String8 S8_SubstrOnLastOccurenceOf(String8 str, char c) {
	for (u8* i = str.s + str.size - 1; i >= str.s; --i) {
		if (*i == c) {
			return (String8){
				.s	= i,
				.size = str.s + str.size - i,
			};
		}
	}
	
	return str;
}

internal String8 S8_SubstrAfterLastOccurrenceOf(String8 str, char c) {
	String8 on_last = S8_SubstrOnLastOccurenceOf(str, c);
	if (on_last.size - 1 == 0 || on_last.s == str.s) {
		return str;
	}
	return (String8){
		.s    = on_last.s + 1,
		.size = on_last.size - 1
	};
}

internal String8 S8_PathToBaseName(String8 path) {
    String8 result = S8_SubstrAfterLastOccurrenceOf(path, '/');
    if (result.s == path.s) {
        result = S8_SubstrAfterLastOccurrenceOf(path, '\\');
    }
    return result;
}
