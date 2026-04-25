#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;
        
        node() : data(nullptr), prev(nullptr), next(nullptr) {}
        node(const T &value) : data(new T(value)), prev(nullptr), next(nullptr) {}
        ~node() {
            if (data) {
                delete data;
                data = nullptr;
            }
        }
    };

protected:
    node *head;
    node *tail;
    size_t list_size;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        if (pos->prev) {
            pos->prev->next = cur;
        }
        pos->prev = cur;
        if (pos == head) {
            head = cur;
        }
        list_size++;
        return cur;
    }
    
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        if (pos->prev) {
            pos->prev->next = pos->next;
        } else {
            head = pos->next;
        }
        if (pos->next) {
            pos->next->prev = pos->prev;
        } else {
            tail = pos->prev;
        }
        list_size--;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    private:
        list *container;
        node *current;

    public:
        iterator() : container(nullptr), current(nullptr) {}
        iterator(list *c, node *n) : container(c), current(n) {}
        
        /**
         * iter++
         */
        iterator operator++(int) {
            if (!container || !current || current == container->tail) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            current = current->next;
            return tmp;
        }
        
        /**
         * ++iter
         */
        iterator & operator++() {
            if (!container || !current || current == container->tail) {
                throw invalid_iterator();
            }
            current = current->next;
            return *this;
        }
        
        /**
         * iter--
         */
        iterator operator--(int) {
            if (!container || !current || current == container->head) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            current = current->prev;
            return tmp;
        }
        
        /**
         * --iter
         */
        iterator & operator--() {
            if (!container || !current || current == container->head) {
                throw invalid_iterator();
            }
            current = current->prev;
            return *this;
        }
        
        /**
         * *it
         */
        T & operator *() const {
            if (!container || !current || !current->data || current == container->tail) {
                throw invalid_iterator();
            }
            return *(current->data);
        }
        
        /**
         * it->field
         */
        T * operator ->() const {
            if (!container || !current || !current->data || current == container->tail) {
                throw invalid_iterator();
            }
            return current->data;
        }
        
        bool operator==(const iterator &rhs) const {
            return container == rhs.container && current == rhs.current;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return container == rhs.container && current == rhs.current;
        }
        
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
        
        friend class const_iterator;
        friend class list;
    };
    
    class const_iterator {
    private:
        const list *container;
        node *current;

    public:
        const_iterator() : container(nullptr), current(nullptr) {}
        const_iterator(const list *c, node *n) : container(c), current(n) {}
        const_iterator(const iterator &other) : container(other.container), current(other.current) {}
        
        const_iterator operator++(int) {
            if (!container || !current || current == container->tail) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            current = current->next;
            return tmp;
        }
        
        const_iterator & operator++() {
            if (!container || !current || current == container->tail) {
                throw invalid_iterator();
            }
            current = current->next;
            return *this;
        }
        
        const_iterator operator--(int) {
            if (!container || !current || current == container->head) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            current = current->prev;
            return tmp;
        }
        
        const_iterator & operator--() {
            if (!container || !current || current == container->head) {
                throw invalid_iterator();
            }
            current = current->prev;
            return *this;
        }
        
        const T & operator *() const {
            if (!container || !current || !current->data || current == container->tail) {
                throw invalid_iterator();
            }
            return *(current->data);
        }
        
        const T * operator ->() const {
            if (!container || !current || !current->data || current == container->tail) {
                throw invalid_iterator();
            }
            return current->data;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return container == rhs.container && current == rhs.current;
        }
        
        bool operator==(const iterator &rhs) const {
            return container == rhs.container && current == rhs.current;
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }
        
        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
        
        friend class iterator;
    };
    
    list() : head(nullptr), tail(nullptr), list_size(0) {
        tail = new node();
        head = tail;
    }
    
    list(const list &other) : head(nullptr), tail(nullptr), list_size(0) {
        tail = new node();
        head = tail;
        
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }
    
    virtual ~list() {
        clear();
        if (tail) {
            delete tail;
            tail = nullptr;
        }
        head = nullptr;
    }
    
    list &operator=(const list &other) {
        if (this != &other) {
            clear();
            for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
                push_back(*it);
            }
        }
        return *this;
    }
    
    const T & front() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(head->data);
    }
    
    const T & back() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(tail->prev->data);
    }
    
    iterator begin() {
        return iterator(this, head);
    }
    
    const_iterator cbegin() const {
        return const_iterator(this, head);
    }
    
    iterator end() {
        return iterator(this, tail);
    }
    
    const_iterator cend() const {
        return const_iterator(this, tail);
    }
    
    virtual bool empty() const {
        return list_size == 0;
    }
    
    virtual size_t size() const {
        return list_size;
    }
    
    virtual void clear() {
        while (!empty()) {
            pop_front();
        }
    }
    
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this) {
            throw invalid_iterator();
        }
        node *new_node = new node(value);
        insert(pos.current, new_node);
        return iterator(this, new_node);
    }
    
    virtual iterator erase(iterator pos) {
        if (pos.container != this || !pos.current || pos.current == tail) {
            throw invalid_iterator();
        }
        node *next_node = pos.current->next;
        erase(pos.current);
        delete pos.current;
        return iterator(this, next_node);
    }
    
    void push_back(const T &value) {
        insert(end(), value);
    }
    
    void pop_back() {
        if (empty()) {
            throw container_is_empty();
        }
        iterator it = end();
        --it;
        erase(it);
    }
    
    void push_front(const T &value) {
        insert(begin(), value);
    }
    
    void pop_front() {
        if (empty()) {
            throw container_is_empty();
        }
        erase(begin());
    }
    
    void sort() {
        if (list_size <= 1) return;
        
        T **arr = new T*[list_size];
        node *current = head;
        for (size_t i = 0; i < list_size; ++i) {
            arr[i] = current->data;
            current = current->next;
        }
        
        sjtu::sort<T*>(arr, arr + list_size, [](const T *a, const T *b) { return *a < *b; });
        
        current = head;
        for (size_t i = 0; i < list_size; ++i) {
            current->data = arr[i];
            current = current->next;
        }
        
        delete[] arr;
    }
    
    void merge(list &other) {
        if (this == &other) return;
        
        node *p1 = head;
        node *p2 = other.head;
        
        while (p1 != tail && p2 != other.tail) {
            if (*(p1->data) <= *(p2->data)) {
                p1 = p1->next;
            } else {
                node *next_p2 = p2->next;
                other.erase(p2);
                insert(p1, p2);
                p2 = next_p2;
            }
        }
        
        while (p2 != other.tail) {
            node *next_p2 = p2->next;
            other.erase(p2);
            insert(tail, p2);
            p2 = next_p2;
        }
    }
    
    void reverse() {
        if (list_size <= 1) return;
        
        node *current = head;
        node *new_head = nullptr;
        node *new_tail = nullptr;
        
        // Reverse all nodes except the tail sentinel
        while (current != tail) {
            node *next = current->next;
            current->next = new_head;
            current->prev = next;
            if (new_head == nullptr) {
                new_tail = current;
            }
            new_head = current;
            current = next;
        }
        
        // Update head and tail pointers
        head = new_head;
        tail->prev = new_tail;
        new_tail->next = tail;
        head->prev = nullptr;
    }
    
    void unique() {
        if (list_size <= 1) return;
        
        node *current = head;
        while (current != tail && current->next != tail) {
            if (*(current->data) == *(current->next->data)) {
                node *to_remove = current->next;
                erase(to_remove);
                delete to_remove;
            } else {
                current = current->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
