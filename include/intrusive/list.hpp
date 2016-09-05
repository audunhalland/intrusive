
#ifndef __INTRUSIVE_HPP__
#define __INTRUSIVE_HPP__

#include <cstddef>

namespace intrusive {
    /*
     *  list node property. Must be included in all classes
     *  that are to be members of list
     */
    class listnode {
    public:
        listnode()
            : prev(this),
              next(this) {}

        ~listnode() {
            erase_partial();
        }

        /* erase this node from the list that it is in */
        void erase() {
            erase_partial();
            prev = this;
            next = this;
        }

    private:
        void erase_partial() {
            prev->next = next;
            next->prev = prev;
        }

        listnode *prev;
        listnode *next;

        template <typename T, listnode T::*N>
        friend class list;
    };

    /*
     *  intrusive::list
     *
     *  doubly linked list where being a member of the list is a
     *  property of the object being part of it.
     *
     *  Advantages to std::list:
     *   * No extra listnode allocation (good for realtime applications)
     *   * Objects can be moved/erased without iterator to list
     *   * Automatic cleanup upon destruction of members in list
     *
     *  Special features:
     *   * Objects can be part of several different intrusive lists
     *     simultaneously. It will depend on the pointer-to-member
     *     property T::*N passed to intrusive::list.
     *
     *  template parameters:
     *  T     Class to be contained in the list
     *  T::*N Member pointer to the listnode property of the class to
     *        be contained in the list
     */
    template <typename T, listnode T::*N>
    class list {
    public:
        class iterator {
        public:
            iterator(T &obj) : node(&(obj.*N)) {}

            T* operator->() {
                return to_object(node);
            }

            T& operator*() {
                return *to_object(node);
            }

            bool operator!=(const iterator &other) {
                return node != other.node;
            }

            iterator &operator=(const iterator &other) {
                node = other.node;
                return *this;
            }

            bool operator==(const iterator &other) {
                return node == other.node;
            }

            iterator &operator++() {
                node = node->next;
                return *this;
            }

            iterator &operator--() {
                node = node->prev;
                return *this;
            }

        private:
            iterator(listnode *node) : node(node) {}

            listnode *node;

            friend class list<T, N>;
        };

        bool empty() const { return head.next == &head; }
        size_t size() const {
            size_t n = 0;
            for (listnode *node = head.next; node != &head; node = node->next) {
                ++n;
            }
            return n;
        }

        void push_back(T &obj) { insert(end(), obj); }
        void pop_back() { head.prev->erase(); }
        void push_front(T &obj) { insert(begin(), obj); }
        void pop_front() { head.next->erase(); }

        static iterator insert(iterator pos, T &obj) {
            listnode *node = &(obj.*N);
            node->erase_partial();
            node->prev = pos.node->prev;
            node->next = pos.node;
            pos.node->prev->next = node;
            pos.node->prev = node;
            return iterator(node);
        }

        static iterator insert(iterator pos, iterator begin, iterator end) {
            if (begin == end) return pos;
            iterator last = end;
            --last;
            // erase from old list
            begin.node->prev->next = end.node;
            end.node->prev = begin.node->prev;
            // link to this list
            begin.node->prev = pos.node->prev;
            last.node->next = pos.node;
            pos.node->prev->next = begin.node;
            pos.node->prev = last.node;
            return begin;
        }

        static iterator erase(iterator pos) {
            iterator next = ++pos;
            pos.node->erase();
            return next;
        }

        void clear() { head.erase(); }

        T &front() const { return *to_object(head.next); }
        T &back() const { return *to_object(head.prev); }

        iterator begin() { return iterator(head.next); }
        iterator end() { return iterator(&head); }

    private:
        /*
         *  Core function (only one without type safety):
         *  Convert from listnode pointer to object pointer
         */
        static inline T *to_object(listnode *n) {
            return (T*)(((char *)n) - (long)&(((T*)0)->*N));
        }

        listnode head;
    };
}

#endif /* __INTRUSIVE_HPP__ */
