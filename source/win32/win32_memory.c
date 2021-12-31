internal void* MReserve(u64 size) {
    return VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
}
internal void MRelease(void* memory) {
    VirtualFree(memory, 0, MEM_RELEASE);
}
internal void MCommit(void* memory, u64 size) {
    VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
}
internal void MDecommit(void* memory, u64 size) {
    VirtualFree(memory, size, MEM_DECOMMIT);
}
