//~ Message output

internal void OutputGenericV(const char* title, const char* format, UINT type, va_list args) {
	String8 text = S8_PushFV(&g_arena, format, args);
    MessageBoxA(0, text.cstr, title, type);
}
internal void OutputGenericS16V(const u16* title, const u16* format, UINT type, va_list args) {
	String16 text = S16_PushFV(&g_arena, format, args);
    MessageBoxW(0, text.s, title, type);
}
internal void OutputErrorMessage(const char* title, const char* format, ...) {
	va_list args;
    va_start(args, format);
	OutputGenericV(title, format, MB_OK | MB_ICONERROR, args);
	va_end(args);
}
internal void OutputErrorMessageS16(const u16* title, const u16* format, ...) {
	va_list args;
    va_start(args, format);
	OutputGenericS16V(title, format, MB_OK | MB_ICONERROR, args);
	va_end(args);
}
