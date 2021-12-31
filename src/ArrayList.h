#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "Arduino.h"

template <typename T>
class ArrayList {
   public:
    class iterator {
       public:
        iterator(T* ptr) : ptr(ptr) {}

        iterator operator++() {
            ++ptr;
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }

        const T& operator*() const {
            return *ptr;
        }

       private:
        T* ptr;
    };

    ArrayList()
        : m_Data(NULL), m_Size(0), m_Capacity(0) {
        reallocate(2);
    }

    ArrayList(const ArrayList& other) : ArrayList() {
        if (reallocate(other.m_Capacity)) {
            m_Size = other.m_Size;
            const size_t size = m_Size * sizeof(T);
            memcpy(m_Data, other.m_Data, size);
        }
    }

    ArrayList(ArrayList&& other) noexcept : ArrayList() {
        swap(other);
    }

    ArrayList(T arr[], size_t len) : m_Data(NULL), m_Size(0), m_Capacity(0) {
        if (reallocate(len + len / 2)) {
            for (size_t i = 0; i < len; i++) {
                add(arr[i]);
            }
        }
    }

    template <typename... Args>
    ArrayList(Args... args) : ArrayList() {
        if (reallocate(sizeof...(args) + sizeof...(args) / 2)) {
            int dummy[] = {(add(args), 0)...};
            (void)dummy;
        }
    }

    ~ArrayList() {
        deallocate();
    }

    bool add(const T& element) {
        if (m_Size >= m_Capacity) {
            if (!reallocate(m_Capacity + m_Capacity / 2)) return false;
        }
        m_Data[m_Size] = element;
        m_Size++;
        return true;
    }

    bool add(T*& element) {
        if (m_Size >= m_Capacity) {
            if (!reallocate(m_Capacity + m_Capacity / 2)) return false;
        }
        m_Data[m_Size] = *element;
        m_Size++;
        return true;
    }

    void remove(const T& element) {
        removeByIndex(indexOf(element));
    }

    void removeByIndex(size_t index) {
        if (index < 0) return;
        m_Data[index].~T();
        m_Size--;
        for (size_t i = index; i < m_Size; i++) {
            m_Data[i] = m_Data[i + 1];
        }
    }

    template <typename Callable>
    void removeIf(Callable predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) {
                removeByIndex(i);
                i--;
            }
        }
    }

    bool contains(const T& element) {
        for (size_t i = 0; i < m_Size; i++) {
            if (m_Data[i] == element) return true;
        }
        return false;
    }

    template <typename Callable>
    bool contains(Callable predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) return true;
        }
        return false;
    }

    int indexOf(const T& element) {
        for (size_t i = 0; i < m_Size; i++) {
            if (m_Data[i] == element) return i;
        }
        return -1;
    }

    template <typename Callable>
    int indexOf(Callable predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) return i;
        }
        return -1;
    }

    void clear() {
        deallocate();
        reallocate(2);
    }

    const T& operator[](const size_t& index) const {
        return m_Data[index];
    }

    T& operator[](const size_t& index) {
        return m_Data[index];
    }

    ArrayList& operator=(const ArrayList& other) {
        m_Size = 0;
        if (reallocate(other.m_Capacity)) {
            m_Size = other.m_Size;
            const size_t size = m_Size * sizeof(T);
            memcpy(m_Data, other.m_Data, size);
        }
        return *this;
    }

    ArrayList& operator=(ArrayList&& other) noexcept {
        swap(other);
        return *this;
    }

    template <typename Callable>
    void forEach(Callable predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (!predicate(m_Data[i], i)) break;
        }
    }

    template <typename E = T, typename Callable>
    ArrayList<E> map(Callable predicate) {
        ArrayList<E> buffer;
        for (size_t i = 0; i < m_Size; i++) {
            buffer.add(predicate(m_Data[i], i));
        }
        return buffer;
    }

    template <typename Callable>
    ArrayList filter(Callable predicate) {
        ArrayList buffer;
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) buffer.add(m_Data[i]);
        }
        return buffer;
    }

    template <typename Callable>
    void sort(Callable predicate) {
        for (size_t i = 1; i < m_Size; i++) {
            for (size_t j = i; j > 0 && predicate(m_Data[j - 1], m_Data[j]); j--) {
                T tmp = m_Data[j - 1];
                m_Data[j - 1] = m_Data[j];
                m_Data[j] = tmp;
            }
        }
    }

    void sort() {
        sort([](T a, T b) -> bool { return a > b; });
    }

    void reverse() {
        for (size_t i = 0; i < m_Size / 2; i++) {
            T buffer = m_Data[i];
            m_Data[i] = m_Data[m_Size - 1 - i];
            m_Data[m_Size - 1 - i] = buffer;
        }
    }

    iterator begin() const {
        return iterator(m_Data);
    }

    iterator end() const {
        return iterator(m_Data + m_Size);
    }

    size_t size() {
        return m_Size;
    }

    bool isEmpty() {
        return m_Size == 0;
    }

   private:
    T* m_Data;
    size_t m_Size;
    size_t m_Capacity;

    bool reallocate(size_t newCapacity) {
        T* newBlock = new T[newCapacity];
        if (newBlock) {
            if (newCapacity < m_Size) {
                m_Size = newCapacity;
            }
            for (size_t i = 0; i < m_Size; i++) {
                newBlock[i] = m_Data[i];
            }
            delete[] m_Data;
            m_Data = newBlock;
            m_Capacity = newCapacity;
            return true;
        }
        return false;
    }

    void deallocate() {
        delete[] m_Data;
        m_Data = NULL;
        m_Size = 0;
        m_Capacity = 0;
    }

    void swap(ArrayList& other) {
        T* _data = m_Data;
        size_t _size = m_Size;
        size_t _capacity = m_Capacity;

        m_Data = other.m_Data;
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;

        other.m_Data = _data;
        other.m_Size = _size;
        other.m_Capacity = _capacity;
    }
};

#endif
