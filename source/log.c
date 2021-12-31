global HANDLE g_conout_handle;

internal void WriteToConsole(String8 data) {
	// NOTE(geni): 
	local_persist DWORD bytes_written;
	WriteFile(g_conout_handle, data.cstr, (DWORD) data.size, &bytes_written, NULL);
	OutputDebugStringA(data.cstr);
}

typedef u32 LogFlags;
enum {
	Log_Info    = 0,
	Log_Warning = (1 << 0),
	Log_Error   = (1 << 1),
};
#ifdef _DEBUG
#define Log(...)        _DebugLog(Log_Info,    S8Lit(__FILE__), __LINE__, __VA_ARGS__)
#define LogWarning(...) _DebugLog(Log_Warning, S8Lit(__FILE__), __LINE__, __VA_ARGS__)
#define LogError(...)   _DebugLog(Log_Error,   S8Lit(__FILE__), __LINE__, __VA_ARGS__)
#define LogEvalB(e)     Log("\"" #e "\" == %S", (e) ? S8Lit("true") : S8Lit("false"))
#define LogEvalU32(e)   Log("\"" #e "\" == %u",   (e))
#define LogEvalU64(e)   Log("\"" #e "\" == %llu", (e))
#define LogEvalI32(e)   Log("\"" #e "\" == %d",   (e))
#define LogEvalI64(e)   Log("\"" #e "\" == %lld", (e))
#else 
#define Log(...)
#define LogWarning(...)
#define LogError(...)
#define LogEvalB(e)
#define LogEvalU32(e)
#define LogEvalU64(e)
#define LogEvalI32(e)
#define LogEvalI64(e)
#endif

void _DebugLog(LogFlags flags, String8 file, u32 line, const char* format, ...) {
	local_persist char info_string[4096];
	local_persist char log_string[4096];
	
	// NOTE(geni): Create info string
	{
		String8 name = flags & Log_Error ? S8Lit("Error") : (flags & Log_Warning ? S8Lit("Warning") : S8Lit("Info"));
		u32 info_string_len = stbsp_snprintf(info_string, sizeof info_string, "[%S] (%S:%u): ", name, file, line);
		WriteToConsole((String8){
						   .cstr = info_string,
						   .size = info_string_len
					   });
	}
	
	// NOTE(geni): Create log string
	{
		va_list args;
		va_start(args, format);
		// TODO(geni): Add ... to end if forced to truncate
		u32 log_string_len = stbsp_vsnprintf(log_string, sizeof log_string, format, args);
		va_end(args);
		WriteToConsole((String8){
						   .cstr = log_string,
						   .size = log_string_len
					   });
		WriteToConsole(S8Lit("\n"));
	}
}
