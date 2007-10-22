#include "litesql.h"
#include <string.h>

static int readSize(lsqlString* s) {
    size_t size = 0;

    unsigned char* p = s->data;
    unsigned int b = 0;
    while (1) {
        printf("readSize %x\n", *p);
        size |= ((*p) & 0x7f) << b;
        printf("size: %d\n", size);
        if (((*p) & 0x80) == 0)
            break;
        p++; 
        b += 7;
    }
   

    return size;
}

static void writeSize(lsqlString* s, size_t size) {
    
    unsigned char c = 0;
    unsigned char* p = s->data;

    for (; size; p++) {
        printf("Size now %d\n", size); 
        c = size & 0x7f;

        size >>= 7;
        if (size)
            c |= 0x80;

        printf("Writing %x\n", c);
        printf("Size left to store %d\n", size);
        *p = c;                
    }
}

static int stringSize(size_t dataSize) {

    if (dataSize < 1 << 7)
        return 1 + dataSize;
    else if (dataSize < 1 << 14)
        return 2 + dataSize;
    else if (dataSize < 1 << 21)
        return 3 + dataSize;
    else if (dataSize < 1 << 28)
        return 4 + dataSize;
    else 
        return 5 + dataSize;
}

static unsigned char* dataStart(lsqlString* s) {
    unsigned char* p = s->data;

    while (*p & 0x80) {
        printf("datastart %x\n", *p);
        p++;
    }

    return ++p;
}

int lsqlStringNew(lsqlString* s) {
    memset(s, 0, sizeof(lsqlString));
    return 0;
}

int lsqlStringCopy(lsqlString* dst, const char* src) {

    size_t size = strlen(src);


    dst->data = lsqlRealloc(dst->data, stringSize(size));

    if (dst->data == NULL)
        return LSQL_MEMORY;

    writeSize(dst, size);
    memcpy(dataStart(dst), src, size);

    return 0;
}

int lsqlStringCat(lsqlString* dst, lsqlString* src) {
    /* TODO: strcat */
    return 0;
}

size_t lsqlStringSize(lsqlString* s) {

    if (!s)
        return 0;

    return readSize(s);
}

void lsqlStringDelete(lsqlString* s) {

    lsqlFree(s->data);
    memset(s, 0, sizeof(lsqlString));
}


