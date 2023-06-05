#pragma once
#include "furi.h"

#ifdef DEBUG
    #define check_pointer(X) check_ptr( X, __FILE__, __LINE__, __FUNCTION__)
#else
#define check_pointer(X) while(0)
#endif

#define CHECK_HEAP() FURI_LOG_I("FlipperGameEngine", "Free/total heap: %zu / %zu", memmgr_get_free_heap(), memmgr_get_total_heap())

char *basename(const char *path);
void check_ptr(void *p, const char *file, int line, const char *func);

float lerp(float a, float b, float t) {
    if (t > 1) return b;
    return (1 - t) * a + t * b;
}