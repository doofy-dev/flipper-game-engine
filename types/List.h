#pragma once

#include <furi.h>
#include "Helpers.h"

template<typename T>
struct ListItem {
    ListItem *next;
    T *data;
};

template<typename T>
struct List {
    uint32_t count;
    ListItem<T> *start;

    List() : count(0) {}

    ~List() {
        empty();
    }

    void clear() {
        auto item = start;
        ListItem<T> *t;
        while (item) {
            t = item;
            item = item->next;
            if (t->data)
                release(t->data);
            release(t);
        }
        count = 0;
    }

    void empty() {
        clear();
        release(start);
    }

    void add(T *data) {
        count++;
        if (count > 1) {
            ListItem<T> *c = start;
            while (c->next) {
                c = c->next;
            }
            c->next = static_cast<ListItem<T>*>(allocate(sizeof(ListItem<T>)));
            c->next->data = data;
        } else {
            start = static_cast<ListItem<T>*>(allocate(sizeof(ListItem<T>)));
            start->data = data;
        }
    }

    void remove(T *data) {
        if (!start || !data) return;

        ListItem<T> *s = start;
        if (s->data == data) {
            release(s->data);
            start = start->next;
            count--;
        } else {
            while (s) {
                if (s->next && s->next->data == data) {
                    auto n = s->next->next;
                    release(s->next->data);
                    release(s->next);
                    s->next = n;
                    count--;
                    return;
                }

                s = s->next;
            }
        }
    }

    void splice(uint32_t index, uint32_t amount) {
        if (index < count) {
            uint32_t m = (index + amount) > count ? count - index : amount;
            uint32_t curr_id = 0;
            auto s = start;
            while (curr_id < index) {
                s = s->next;
                if (!s) return;
                curr_id++;
            }
            ListItem<T> *t;
            for (uint32_t i = 0; i < m; i++) {
                t = s->next;
                if (s->data)
                    release(s->data);
                release(s);
                s = t->next;
            }
            if (index == 0) {
                start = s;
            }
        }
    }

};