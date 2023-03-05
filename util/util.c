#include "util.h"
char *basename(const char *path) {
    const char *base = path;
    while (*path) {
        if (*path++ == '/') {
            base = path;
        }
    }
    return (char *) base;
}

void *local_allocate(size_t size, const char *file, int line, const char *func) {
    void *p = malloc(size);
    FURI_LOG_D("FlipperGameEngine", "[ALLOCATION][%p] %s:%s():%i, [%i]", p, basename((char*)file), func, line,  size);
    return p;
}

void local_free(void *p, const char *file, int line, const char *func) {
    if (p) {
        FURI_LOG_D("FlipperGameEngine", "[RELEASE][%p] %s:%s():%i", p, basename((char*)file), func, line);
        free(p);
    } else {
        FURI_LOG_W("FlipperGameEngine", "[RELEASE ERROR] %s:%s():%i, %p", basename((char*)file), func, line, p);
    }
}

void check_ptr(void *p, const char *file, int line, const char *func){
    if(p==NULL) {
        FURI_LOG_W("FlipperGameEngine", "[NULLPTR] %s:%s():%i", basename((char*)file), func, line);
    }
}