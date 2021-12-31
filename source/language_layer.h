//~ Helper macros

#ifndef MemoryCopy
#define MemoryCopy memcpy
#endif

#ifndef MemoryMove
#define MemoryMove memmove
#endif

#ifndef MemorySet
#define MemorySet memset
#endif

#define global        static
#define internal      static
#define local_persist static
#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define Bytes(n)      (n)
#define Kilobytes(n)  (n << 10)
#define Megabytes(n)  (n << 20)
#define Gigabytes(n)  (((u64) n) << 30)
#define Terabytes(n)  (((u64) n) << 40)
#define Unreferenced(a) ((void) (a))

//~ Base types

typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef i8        b8;
typedef i16       b16;
typedef i32       b32;
typedef i64       b64;
typedef float     f32;
typedef double    f64;

typedef wchar_t wchar;

//~ Assertions

#define _Assert(b, crash)                                            \
do {                                                              \
if (!(b)) {                                                   \
LogError("[Assertion Failure] Assertion of \"#b\" at __FILE__:__LINE__ failed"); \
OutputErrorMessage("Assertion Failure", "Assertion of \"#b\" at __FILE__:__LINE__ failed"); \
if (crash) { \
BreakDebugger(); \
abort(); \
} \
} \
} while (0)
// TODO(geni): Kind of a bad name
#define HardAssert(b) _Assert(b, 1)
#define SoftAssert(b) _Assert(b, 0)
#define StaticAssert _Static_assert
#undef  Assert
#define Assert HardAssert

internal inline void BreakDebugger(void) {
#if _MSC_VER && BUILD_WIN32
    __debugbreak();
#else
	// TODO(geni): BreakDebugger for Linux/macOS
#endif
}

