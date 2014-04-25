
#include <cstddef>
#include <assert.h>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
#include <type_traits>

enum NotNullTag { NotNull };
inline void* operator new(size_t, NotNullTag, void* location)
{
    assert(location);
    return location;
}

inline void operator delete(void*, NotNullTag, void*) { }

namespace my {

template<bool neesInitialization, typename T>
struct VectorInitializer;

template<typename T>
struct VectorInitializer<false, T>
{
    static void initialize(T*, T*) { }
};

template<typename T>
struct VectorInitializer<true, T>
{
    static void initialize(T* begin, T* end)
    {
        for (T* cur = begin; cur != end; ++cur) {
            new (NotNull, cur) T;
        }
    }
};

template<bool needsDestruction, typename T>
struct VectorDestructor;

template<typename T>
struct VectorDestructor<false, T>
{
    static void destruct(T*, T*) { }
};

template<typename T>
struct VectorDestructor<true, T>
{
    static void destruct(T* begin, T* end)
    {
        for (T* cur = begin; cur != end ; ++cur)
            cur->~T();
    }
};

template<bool canCopyWithMemcpy, typename T>
struct VectorCopier { };

template<typename T>
struct VectorCopier<false, T>
{
    static void copy(T* begin, T* end, T* dest)
    {
        while (begin != end) {
            *dest = *begin;
            ++begin;
            ++dest;
        }
    }
};

template<typename T>
struct VectorCopier<true, T>
{
    static void copy(T* begin, T* end, T* dest)
    {
        std::memcpy(dest, begin, reinterpret_cast<const char*>(end) - reinterpret_cast<const char*>(begin));
    }
};

template<typename T>
struct VectorOperations
{
    static void initialize(T* begin, T* end)
    {
        VectorInitializer<!std::is_pod<T>::value, T>::initialize(begin, end);
    }

    static void destruct(T* begin, T* end)
    {
        VectorDestructor<!std::is_trivially_destructible<T>::value, T>::destruct(begin, end);
    }

    static void copy(T* begin, T* end, T* dest)
    {
        VectorCopier<std::is_pod<T>::value, T>::copy(begin, end, dest);
    }
};

template<typename T>
class VectorBase
{
public:
    VectorBase()
    : m_buffer(nullptr)
    , m_capacity(0)
    { }

    VectorBase(unsigned capacity)
    : m_buffer(nullptr)
    , m_capacity(capacity)
    {
        if (m_capacity)
            allocateBuffer(capacity);
    }

    ~VectorBase()
    {
        deallocateBuffer();
    }

    void allocateBuffer(size_t newCapacity)
    {
        assert(newCapacity);
        assert(newCapacity <= std::numeric_limits<unsigned>::max() / sizeof(T));

        m_capacity = newCapacity;
        size_t sizeToAllocate = m_capacity * sizeof(T);
        m_buffer = static_cast<T*>(malloc(sizeToAllocate));
    }

    void deallocateBuffer()
    {
        free(m_buffer);
        m_buffer = nullptr;
        m_capacity = 0;
    }

    void deallocateBuffer(T* bufferToDeallocate)
    {
        if (!bufferToDeallocate)
            return;

        free(bufferToDeallocate);
    }

    T* buffer() { return m_buffer; }
    const T* buffer() const { return m_buffer; }
    size_t capacity() const { return m_capacity; }

private:
    // noncopyable
    VectorBase(const VectorBase<T>&) = delete;
    VectorBase<T>& operator=(const VectorBase<T>&) = delete;

    T* m_buffer;
    unsigned m_capacity;
};

template<typename T>
class Vector : public VectorBase<T>
{
private:
    typedef VectorBase<T> Base;
public:
    typedef T* iterator;
    typedef const T* const_iterator;

    Vector()
    : m_size(0)
    { }

    Vector(const Vector&);
    ~Vector();

    void resize(size_t size);
    void reserve(size_t size);
    void shrink(size_t size);

    void push_back(const T& data);

    size_t capacity() const { return Base::capacity(); }
    std::size_t size() const { return m_size; }
    bool isEmpty() const { return !size(); }

    T& at(std::size_t idx)
    {
        assert(idx < size());
        return data()[idx];
    }

    const T& at(std::size_t idx) const
    {
        assert(idx < size());
        return data()[idx];
    }

    T& operator[](std::size_t idx) { return at(idx); }
    const T& operator[](std::size_t idx) const { return at(idx); }

    Vector<T>& operator=(const Vector<T>& arg);

    T* data() { return Base::buffer(); }
    const T* data() const { return Base::buffer(); }

    iterator begin() { return data(); }
    iterator end() { return begin() + m_size; }
    const_iterator begin() const { return data(); }
    const_iterator end() const { return begin() + m_size; }

private:
    void expandCapacity(size_t newMinCapacity);
    void reserveCapacity(size_t newCapacity);
    void push_back_slowcase(const T&);

    using Base::buffer;
    using Base::capacity;

    unsigned m_size;
};

template<typename T>
Vector<T>::Vector(const Vector& other)
    : Base(other.capacity())
    , m_size(other.size())
{
    if (begin())
        VectorOperations<T>::copy(other.begin(), other.end(), begin());
}

template<typename T>
Vector<T>::~Vector()
{
    if (m_size)
        shrink(0);
}

template<typename T>
void Vector<T>::resize(size_t size)
{
    if (m_size >= size)
        VectorOperations<T>::destruct(begin() + size, end());
    else {
        if (size > capacity())
            expandCapacity(size);
        if (begin())
            VectorOperations<T>::initialize(end(), begin() + size);
    }
    m_size = size;
}

template<typename T>
void Vector<T>::reserve(size_t newCapacity)
{
    reserveCapacity(newCapacity);
}

template<typename T>
void Vector<T>::shrink(size_t size)
{
    assert(size <= m_size);
    VectorOperations<T>::destruct(begin() + size, end());
    m_size = size;
}

template<typename T>
void Vector<T>::expandCapacity(size_t newMinCapacity)
{
    size_t newCapacity = std::max(newMinCapacity, std::max(static_cast<size_t>(16), capacity() + capacity() / 4 + 1));
    reserveCapacity(newCapacity);
}

template<typename T>
void Vector<T>::reserveCapacity(size_t newCapacity)
{
    if (capacity() >= newCapacity)
        return;

    T* oldBuffer = begin();
    T* oldBufferEnd = end();
    Base::allocateBuffer(newCapacity);
    assert(begin());
    VectorOperations<T>::copy(oldBuffer, oldBufferEnd, begin());
    Base::deallocateBuffer(oldBuffer);
}

template<typename T>
void Vector<T>::push_back(const T& data)
{
    if (size() < capacity()) {
        *end() = data;
        ++m_size;
        return;
    }

    push_back_slowcase(data);
}

template<typename T>
void Vector<T>::push_back_slowcase(const T& data)
{
    assert(size() == capacity());
    expandCapacity(size() + 1);
    assert(begin());

    *end() = data;
    ++m_size;
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other)
{
    // FIXME: If other's capacity is much smaller than this's, we need to shrink capacity.
    if (capacity() >= other.size())
        VectorOperations<T>::destruct(begin(), end());
    else {
        Base::deallocateBuffer();
        expandCapacity(other.capacity());
    }
    VectorOperations<T>::copy(other.begin(), other.end(), begin());
}

} // namespace my
