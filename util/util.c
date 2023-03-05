#include "util.h"

void *local_allocate(size_t size, const char *file, int line, const char *func) {
    void *p = malloc(size);
    FURI_LOG_D("FlipperGameEngine", "[ALLOCATION][%p] %s:%s():%i, [%i]", p, file, func, line,  size);
    return p;
}

void local_free(void *p, const char *file, int line, const char *func) {
    if (p) {
        FURI_LOG_D("FlipperGameEngine", "[RELEASE][%p] %s:%s():%i", p, file, func, line);
        free(p);
    } else {
        FURI_LOG_W("FlipperGameEngine", "[RELEASE ERROR] %s:%s():%i, %p", file, func, line, p);
    }
}

void check_ptr(void *p, const char *file, int line, const char *func){
    if(p==NULL)
        FURI_LOG_W("FlipperGameEngine", "[NULLPTR] %s:%s():%i", file, func, line);
}