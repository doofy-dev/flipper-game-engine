#include "util.h"

void *local_allocate(size_t size, const char *file, int line, const char *func) {
    void *p = malloc(size);
    FURI_LOG_D("FlipperGameEngine", "[ALLOCATION] %s:%s(), line %i, %p[%i]", file, func, line, p, size);
    return p;
}

void local_free(void *p, const char *file, int line, const char *func) {
    if (p) {
        FURI_LOG_D("FlipperGameEngine", "[RELEASE] %s:%s(), line %i, %p", file, func, line, p);
        free(p);
    } else {
        FURI_LOG_W("FlipperGameEngine", "[RELEASE ERROR] %s:%s(), line %i, %p", file, func, line, p);
    }
}
