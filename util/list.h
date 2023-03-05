#pragma once

#include <furi.h>
#include "../config.h"

#ifdef DEBUG
    #define make_list(X) make_list_check( X, __FILE__, __LINE__, __FUNCTION__)
    #define list_clear(X) list_clear_check( X, __FILE__, __LINE__, __FUNCTION__)
#else
#define make_list(X) make_list_local( X )
#define list_clear(X) list_clear_local( X )
#endif


typedef struct ListItem t_ListItem;

struct ListItem{
    t_ListItem *next;
    void *data;
};

typedef struct {
    uint32_t count;
    t_ListItem *start;
    size_t item_size;
} List;

void* make_list_check(size_t size, const char *file, int line, const char *func);
void list_clear_check(List *list, const char *file, int line, const char *func);
List *make_list_local(size_t item_size);
void *list_get(List *list, uint32_t index);
void list_add(List *list, void *data);
bool list_splice(List *list, uint32_t index, uint32_t count);
void list_clear_local(List *list);
void list_free(List *list);
void list_remove_item(List *list, void *data);
void * list_find(List *list, void *data);