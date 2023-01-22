#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#if defined(ESP32) || defined(ESP8266)
#include <functional>
#endif

#include "Arduino.h"

template <typename T>
class ArrayList {
   public:
    class Iterator {
       public:
        Iterator(T* ptr)
            : m_Ptr(ptr) {}

        Iterator operator++() {
            ++m_Ptr;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++m_Ptr;
            return tmp;
        }

        Iterator operator--() {
            --m_Ptr;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --m_Ptr;
            return tmp;
        }

        Iterator operator+(const int& n) {
            return Iterator(m_Ptr + n);
        }

        Iterator operator-(const int& n) {
            return Iterator(m_Ptr - n);
        }

        Iterator& operator+=(const int& n) {
            m_Ptr += n;
            return *this;
        }

        Iterator& operator-=(const int& n) {
            m_Ptr -= n;
            return *this;
        }

        bool operator==(const Iterator& other) {
            return m_Ptr == other.m_Ptr;
        }

        bool operator!=(const Iterator& other) const {
            return m_Ptr != other.m_Ptr;
        }

        T& operator[](const int& n) {
            return *(m_Ptr + n);
        }

        T& operator*() const {
            return *m_Ptr;
        }

        T* operator->() const {
            return m_Ptr;
        }

       private:
        T* m_Ptr;
    };

    ArrayList()
        : m_Data(NULL),
          m_Size(0),
          m_Capacity(0) {
        _reallocate(2);
    }

    ArrayList(const ArrayList& other)
        : ArrayList() {
        if (_reallocate(other.m_Capacity)) {
            m_Size            = other.m_Size;
            const size_t size = m_Size * sizeof(T);
            memcpy(m_Data, other.m_Data, size);
        }
    }

    ArrayList(ArrayList&& other) noexcept
        : ArrayList() {
        _swap(other);
    }

    ArrayList(T arr[], size_t len)
        : m_Data(NULL),
          m_Size(0),
          m_Capacity(0) {
        if (_reallocate(len + len / 2)) {
            for (size_t i = 0; i < len; i++) {
                add(arr[i]);
            }
        }
    }

    template <typename... Args>
    ArrayList(Args... args)
        : ArrayList() {
        if (_reallocate(sizeof...(args) + sizeof...(args) / 2)) {
            int dummy[] = {(add(args), 0)...};
            (void)dummy;
        }
    }

    ~ArrayList() {
        _deallocate();
    }

    bool add(const T& element) {
        if (m_Size >= m_Capacity) {
            if (!_reallocate(m_Capacity + m_Capacity / 2)) return false;
        }
        m_Data[m_Size] = element;
        m_Size++;
        return true;
    }

    bool add(T&& element) {
        if (m_Size >= m_Capacity) {
            if (!_reallocate(m_Capacity + m_Capacity / 2)) return false;
        }
        m_Data[m_Size] = element;
        m_Size++;
        return true;
    }

    bool addAll(const ArrayList& other) {
        if (m_Size + other.m_Size >= m_Capacity) {
            if (!_reallocate(m_Capacity + other.m_Capacity)) return false;
        }
        for (size_t i = 0; i < other.m_Size; i++) {
            m_Data[m_Size + i] = other.m_Data[i];
        }
        m_Size += other.m_Size;
        return true;
    }

    void resize(const size_t& newSize) {
        _reallocate(newSize);
    }

    void remove(const T& element) {
        removeAt(indexOf(element));
    }

    void removeAt(size_t index) {
        if (index < 0) return;
        m_Size--;
        for (size_t i = index; i < m_Size; i++) {
            m_Data[i] = m_Data[i + 1];
        }
    }

#if defined(ESP32) || defined(ESP8266)
    void removeIf(std::function<bool(T)> predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) {
                removeAt(i);
                i--;
            }
        }
    }
#else
    template <typename Callable>
    void removeIf(Callable predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) {
                removeAt(i);
                i--;
            }
        }
    }
#endif

    bool contains(const T& element) {
        for (size_t i = 0; i < m_Size; i++) {
            if (m_Data[i] == element) return true;
        }
        return false;
    }

#if defined(ESP32) || defined(ESP8266)
    bool contains(std::function<bool(T)> predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) return true;
        }
        return false;
    }
#else
    template <typename Callable>
    bool contains(Callable predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) return true;
        }
        return false;
    }
#endif

int indexOf(const T& element) {
    for (size_t i = 0; i < m_Size; i++) {
        if (m_Data[i] == element) return i;
    }
    return -1;
}

#if defined(ESP32) || defined(ESP8266)
    int indexOf(std::function<bool(T)> predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) return i;
        }
        return -1;
    }

    void clear() {
        _deallocate();
        _reallocate(2);
    }
#else
    template <typename Callable>
    int indexOf(Callable predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) return i;
        }
        return -1;
    }

    void clear() {
        _deallocate();
        _reallocate(2);
    }
#endif

    const T get(const size_t& index, const T& defaultValue) const {
        if (index >= m_Size) {
            return defaultValue;
        }
        return m_Data[index];
    }

    T get(const size_t& index, const T& defaultValue) {
        if (index >= m_Size) return const_cast<T&>(defaultValue);
        return m_Data[index];
    }

    const T& operator[](const size_t& index) const {
        return m_Data[index];
    }

    T& operator[](const size_t& index) {
        return m_Data[index];
    }

    ArrayList& operator=(const ArrayList& other) {
        m_Size = 0;
        if (_reallocate(other.m_Capacity)) {
            m_Size            = other.m_Size;
            const size_t size = m_Size * sizeof(T);
            memcpy(m_Data, other.m_Data, size);
        }
        return *this;
    }

    ArrayList& operator=(ArrayList&& other) noexcept {
        _swap(other);
        return *this;
    }

#if defined(ESP32) || defined(ESP8266)
    void forEach(std::function<bool(T&, size_t)> predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (!predicate(m_Data[i], i)) break;
        }
    }
#else
    template <typename Callable>
    void forEach(Callable predicate) {
        for (size_t i = 0; i < m_Size; i++) {
            if (!predicate(m_Data[i], i)) break;
        }
    }
#endif

#if defined(ESP32) || defined(ESP8266)
    template <typename E = T>
    ArrayList<E> map(std::function<E(T&, size_t)> predicate) {
        ArrayList<E> buffer;
        for (size_t i = 0; i < m_Size; i++) {
            buffer.add(predicate(m_Data[i], i));
        }
        return buffer;
    }
#else
    template <typename E = T, typename Callable>
    ArrayList<E> map(Callable predicate) {
        ArrayList<E> buffer;
        for (size_t i = 0; i < m_Size; i++) {
            buffer.add(predicate(m_Data[i], i));
        }
        return buffer;
    }
#endif

#if defined(ESP32) || defined(ESP8266)
    ArrayList filter(std::function<bool(T&)> predicate) {
        ArrayList buffer;
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) buffer.add(m_Data[i]);
        }
        return buffer;
    }
#else
    template <typename Callable>
    ArrayList filter(Callable predicate) {
        ArrayList buffer;
        for (size_t i = 0; i < m_Size; i++) {
            if (predicate(m_Data[i])) buffer.add(m_Data[i]);
        }
        return buffer;
    }
#endif

#if defined(ESP32) || defined(ESP8266)
    void sort(std::function<bool(T&, T&)> predicate) {
        for (size_t i = 1; i < m_Size; i++) {
            for (size_t j = i; j > 0 && predicate(m_Data[j - 1], m_Data[j]); j--) {
                T tmp         = m_Data[j - 1];
                m_Data[j - 1] = m_Data[j];
                m_Data[j]     = tmp;
            }
        }
    }
#else
    template <typename Callable>
    void sort(Callable predicate) {
        for (size_t i = 1; i < m_Size; i++) {
            for (size_t j = i; j > 0 && predicate(m_Data[j - 1], m_Data[j]); j--) {
                T tmp         = m_Data[j - 1];
                m_Data[j - 1] = m_Data[j];
                m_Data[j]     = tmp;
            }
        }
    }
#endif

    void sort() {
        sort([](T a, T b) -> bool { return a > b; });
    }

    void reverse() {
        for (size_t i = 0; i < m_Size / 2; i++) {
            T buffer               = m_Data[i];
            m_Data[i]              = m_Data[m_Size - 1 - i];
            m_Data[m_Size - 1 - i] = buffer;
        }
    }

    Iterator begin() {
        return Iterator(m_Data);
    }

    Iterator end() {
        return Iterator(m_Data + m_Size);
    }

    size_t size() const {
        return m_Size;
    }

    bool isEmpty() const {
        return m_Size == 0;
    }

   private:
    T* m_Data;
    size_t m_Size;
    size_t m_Capacity;

    bool _reallocate(size_t newCapacity) {
        T* newBlock = new T[newCapacity];
        if (newBlock) {
            if (newCapacity < m_Size) {
                m_Size = newCapacity;
            }
            for (size_t i = 0; i < m_Size; i++) {
                newBlock[i] = m_Data[i];
            }
            delete[] m_Data;
            m_Data     = newBlock;
            m_Capacity = newCapacity;
            return true;
        }
        return false;
    }

    void _deallocate() {
        delete[] m_Data;
        m_Data     = NULL;
        m_Size     = 0;
        m_Capacity = 0;
    }

    void _swap(ArrayList& other) {
        T* _data         = m_Data;
        size_t _size     = m_Size;
        size_t _capacity = m_Capacity;

        m_Data     = other.m_Data;
        m_Size     = other.m_Size;
        m_Capacity = other.m_Capacity;

        other.m_Data     = _data;
        other.m_Size     = _size;
        other.m_Capacity = _capacity;
    }
};

#endif
