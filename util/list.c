#include "list.h"
#include <furi.h>
#include "util.h"
void list_clear_check(List *list, const char *file, int line, const char *func){
    FURI_LOG_D("FlipperGameEngine", "[CLEAR LIST] %s:%s():%i", basename((char*)file), func, line);
    list_clear_local(list);
}
void* make_list_check(size_t size, const char *file, int line, const char *func){
    FURI_LOG_D("FlipperGameEngine", "[MAKE LIST] %s:%s():%i", basename((char*)file), func, line);

    return make_list_local(size);
}

List *make_list_local(size_t item_size) {
    List *l = allocate(sizeof(List));
    l->count = 0;
    l->item_size = item_size;
    return l;
}

void list_add(List *list, void *data) {
    list->count++;
    if (list->count > 1) {
        t_ListItem *c = list->start;
        while (c->next) {
            c = c->next;
        }
        c->next = allocate(sizeof(t_ListItem));
        c->next->data = data;
    } else {
        list->start = allocate(sizeof(t_ListItem));
        list->start->data = data;
    }
}

void list_remove_item(List *list, void *data) {
    t_ListItem *s = list->start;
    if (s == NULL) return;
    if (s->data == data) {
        list->start = s->next;
        list->count--;
    } else {
        while (s != NULL) {
            if (s->next->data == data) {
                s->next = s->next->next;
                free(s->data);
                free(s);
                list->count--;
                return;
            }
            s = s->next;
        }
    }
}

void *list_get(List *list, uint32_t index) {
    if (index < list->count) {
        uint32_t curr_id = 0;
        t_ListItem *s = list->start;
        while (curr_id < index) {
            s = s->next;
            curr_id++;
        }
        return s->data;
    } else {
        FURI_LOG_E("list_get", "Index out of range!");
        return NULL;
    }
}

bool list_splice(List *list, uint32_t index, uint32_t count) {
    if (index < list->count && (index + count) < list->count) {
        uint32_t curr_id = 0;
        t_ListItem *s = list->start;
        while (curr_id < index) {
            s = s->next;
            curr_id++;
        }
        t_ListItem *e = s;
        while (curr_id < (index + count)) {
            t_ListItem *t = e;
            e = e->next;
            if(t->data)
                release(t->data);
            release(t);
            curr_id++;
        }
        if (index > 0)
            s->next = e;
        else
            list->start = e;

        list->count -= count;

        return true;
    } else {
        FURI_LOG_E("list_splice", "Index out of range!");
        return false;
    }
}

void list_clear_local(List *list) {
    if(list == NULL) return;

    t_ListItem *item = list->start;
    while (item) {
        t_ListItem *t = item;
        item = item->next;

        if (t->data)
            release(t->data);
        release(t);
    }

    list->count = 0;
}

void list_free(List *list) {
    list_clear(list);
    release(list);
}

void *list_find(List *list, void *data) {
    t_ListItem *item = list->start;
    while (item) {
        t_ListItem *t = item;
        item = item->next;
        if (t->data == data)
            return t;
    }
    return NULL;
}