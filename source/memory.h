typedef struct {
    void* base;
    u64   max;
    u64   alloc_position;
    u64   commit_position;
} MArena;

global MArena g_arena;
