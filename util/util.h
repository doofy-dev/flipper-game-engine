#pragma once
#include <furi.h>
#include "../config.h"
#ifdef DEBUG
    #define allocate(X) local_allocate( X, __FILE__, __LINE__, __FUNCTION__)
    #define release(X) local_free( X, __FILE__, __LINE__, __FUNCTION__)
    #define check_pointer(X) check_ptr( X, __FILE__, __LINE__, __FUNCTION__)
#else
    #define allocate(X) malloc( X )
    #define release(X) free( X )
    #define check_pointer(X) while(0)
#endif

#define CHECK_HEAP() FURI_LOG_I("FlipperGameEngine", "Free/total heap: %zu / %zu", memmgr_get_free_heap(), memmgr_get_total_heap())


void* local_allocate(size_t size, const char *file, int line, const char *func);
void local_free(void *p, const char *file, int line, const char *func);
void check_ptr(void *p, const char *file, int line, const char *func);
