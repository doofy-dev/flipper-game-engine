#pragma once

#include <furi.h>
#include <iterator>
#include "../Helpers.h"

template<typename T>
struct ListItem {
    ListItem *next;
    T *data;
};

template<typename T>
struct ListIterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    ListItem<T>* current;

    ListIterator(ListItem<T>* node) : current(node) {}

    ListIterator& operator++() {
        current = current->next;
        return *this;
    }

    ListIterator operator++(int) {
        ListIterator iterator = *this;
        ++(*this);
        return iterator;
    }

    bool operator==(const ListIterator& other) const {
        return current == other.current;
    }

    bool operator!=(const ListIterator& other) const {
        return !(*this == other);
    }

    T* operator*() const {
        return (current->data);
    }

    T* operator->() const {
        return current->data;
    }
};


template<typename T>
struct List {
    uint32_t count;
    ListItem<T> *start = nullptr;

    List() : count(0) {}

    ~List() {
        LOG_D("List emptied");

        empty();
    }

    void soft_clear(){
        auto *item = start;
        ListItem<T> *t;
        while (item) {
            t = item;
            item = item->next;
            delete t;
        }
        count = 0;
    }

    void clear() {
        auto *item = start;
        ListItem<T> *t;
        while (item) {
            t = item;
            item = item->next;
            if (t->data) {
                check_pointer(t->data);
                delete t->data;
            }
            check_pointer(t);
            delete t;
        }
        count = 0;
    }

    void empty() {
        clear();
        if (start) {
            check_pointer(start);
            delete start;
        }
    }

    void add(T *data) {
        count++;
        if (count > 1) {
            ListItem<T> *c = start;
            while (c->next) {
                c = c->next;
            }
            c->next = new ListItem<T>();
            c->next->data = data;
        } else {
            start = new ListItem<T>();
            start->data = data;
        }
    }

    void remove(T *data) {
        if (!start || !data) return;

        ListItem<T> *s = start;
        if (s->data == data) {
            check_pointer(s->data);
            delete s->data;
            start = start->next;
            count--;
        } else {
            while (s) {
                if (s->next && s->next->data == data) {
                    auto n = s->next->next;
                    check_pointer(s->next->data);
                    check_pointer(s->next);
                    delete s->next->data;
                    delete s->next;
                    s->next = n;
                    count--;
                    return;
                }

                s = s->next;
            }
        }
    }

    void soft_remove(T *data) {
        if (!start || !data) return;

        ListItem<T> *s = start;
        if (s->data == data) {
            auto tmp = start;
            start = start->next;
            delete tmp;
            count--;
        } else {
            while (s) {
                if (s->next && s->next->data == data) {
                    auto n = s->next->next;
                    check_pointer(s->next);
                    delete s->next;
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
                if (s->data) {
                    check_pointer(s->data);
                    delete s->data;
                }
                check_pointer(s);
                delete s;
                s = t->next;
            }
            if (index == 0) {
                start = s;
            }
        }
    }

    ListIterator<T> begin() {
        return ListIterator<T>(start);
    }

    ListIterator<T> end() {
        return ListIterator<T>(nullptr);
    }
};