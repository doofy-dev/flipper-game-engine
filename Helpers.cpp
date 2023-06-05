#include "Helpers.h"

char *basename(const char *path) {
    const char *base = path;
    while (*path) {
        if (*path++ == '/') {
            base = path;
        }
    }
    return (char *) base;
}

void check_ptr(void *p, const char *file, int line, const char *func){
    if(p==NULL) {
        FURI_LOG_W("FlipperGameEngine", "[NULLPTR] %s:%s():%i", basename((char*)file), func, line);
    }
}