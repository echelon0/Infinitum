
void
MemCpy(void *Dest, void *Source, int nBytes) {
    char *CharDest = (char *)Dest;
    char *CharSource = (char *)Source;
    for(int I = 0; I < nBytes; I++) {
        *CharDest++ = *CharSource++;
    }
}

int
StrLen(char *String) {
    int Count = 0;
    while(*String) {
        Count++;
        *String++;
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
