#define MARENA_MAX         Gigabytes(1)
#define MARENA_COMMIT_SIZE Kilobytes(4)

//~ Convenience macros

// NOTE(geni): Push
#define MArenaPushType(arena, t)         MArenaPush(arena, sizeof(t))
#define MArenaPushTypeN(arena, t, n)     MArenaPush(arena, sizeof(t) * (n))
#define MArenaPushTypeZero(arena, t)     MArenaPushZero(arena, sizeof(t))
#define MArenaPushTypeZeroN(arena, t, n) MArenaPushZero(arena, sizeof(t) * (n))

#define MArenaPushStruct      MArenaPushType
#define MArenaPushStructN     MArenaPushTypeN
#define MArenaPushStructZero  MArenaPushTypeZero
#define MArenaPushStructZeroN MArenaPushTypeZeroN

// NOTE(geni): Pop
#define MArenaPopType(arena, t)         MArenaPop(arena, sizeof(t))
#define MArenaPopTypeN(arena, t, n)     MArenaPop(arena, sizeof(t) * (n))

#define MArenaPopStruct      MArenaPopType
#define MArenaPopStructN     MArenaPopTypeN

//~ Functions

internal inline MArena MArenaFromSize(u64 size) {
    return (MArena){
        .max  = size,
        .base = MReserve(size),
    };
}
internal inline MArena MArenaNew(void) {
    return MArenaFromSize(MARENA_MAX);
}

internal void* MArenaPush(MArena* arena, u64 size) {
    void* memory = 0;
    if (arena->alloc_position + size > arena->commit_position) {
        u64 commit_size = size;
        commit_size += MARENA_COMMIT_SIZE - 1;
        commit_size -= commit_size % MARENA_COMMIT_SIZE;
        MCommit((u8*) arena->base + arena->commit_position, commit_size);
        arena->commit_position += commit_size;
    }
    memory = (u8*) arena->base + arena->alloc_position;
    arena->alloc_position += size;
    return memory;
}

internal void* MArenaPushZero(MArena* arena, u64 size) {
    void* memory = MArenaPush(arena, size);
    MemorySet(memory, 0, size);
    return memory;
}

internal void MArenaPop(MArena* arena, u64 size) {
    if (size > arena->alloc_position) {
        size = arena->alloc_position;
    }
    arena->alloc_position -= size;
}

internal void MArenaClear(MArena* arena) {
    MArenaPop(arena, arena->alloc_position);
}

internal void MArenaRelease(MArena* arena) {
	MRelease(arena->base);
}
