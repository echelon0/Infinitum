
u32
MemCopy(void *Dest, void *Src, u32 nBytes) {
    u32 BytesWritten = 0;
    u8 *u8Dest = (u8 *)Dest;
    u8 *u8Src = (u8 *)Src;    
    for(u32 ByteIndex = 0; ByteIndex < nBytes; ByteIndex++) {
        if(!u8Dest || !u8Src) {
            BytesWritten;
        }
        *(u8Dest) = *(u8Src);
        BytesWritten++;
        (*(u8Dest))++;
        (*(u8Src))++;
    }
    return BytesWritten;
}
