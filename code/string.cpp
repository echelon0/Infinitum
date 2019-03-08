
int
StrLen(char *string) {
    int Count = 0;
    while(*string) {
        Count++;
        *string++;
    }
    return Count;
}

void
StrCopy(char *Dest, char *Source) {
    while(*Source) {
        *Dest++ = *Source++;
    }
    *Dest = '\0';
}

void
StrCat(char *Dest, char *Source) {
    while(*Dest) {
        *Dest++;
    }
    while(*Source) {
        *Dest++ = *Source++;
    }
    *Dest = '\0';
}

void
StrNCat(char *Dest, char *Source, int nBytes) {
    while(*Dest) {
        *Dest++;
    }
    int Count = 0;
    while(Count < nBytes) {
        *Dest++ = *Source++;
        Count++;
    }
    *Dest = '\0';
}
