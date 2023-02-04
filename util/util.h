#pragma once
#include <furi.h>
#define allocate(X) local_allocate( X, __FILE__, __LINE__, __FUNCTION__)
#define release(X) local_free( X, __FILE__, __LINE__, __FUNCTION__)

#define CHECK_HEAP() FURI_LOG_I("FlipperGameEngine", "Free/total heap: %zu / %zu", memmgr_get_free_heap(), memmgr_get_total_heap())


void* local_allocate(size_t size, const char *file, int line, const char *func);
void local_free(void *p, const char *file, int line, const char *func);
