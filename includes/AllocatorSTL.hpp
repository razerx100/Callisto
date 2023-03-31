#ifndef CALLISTO_ALLOCATOR_STL_HPP_
#define CALLISTO_ALLOCATOR_STL_HPP_
#include <Allocator.hpp>
#include <memory>
#include <type_traits>
#include <new>

template<typename T>
class AllocatorSTL {
public:
    typedef T value_type;
    typedef size_t size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;

    template<typename U>
    struct rebind { typedef AllocatorSTL<U> other; };

    AllocatorSTL(std::shared_ptr<Allocator> allocator) noexcept
        : m_allocator{ std::move(allocator) } {}

    pointer allocate(size_type size) {
        return static_cast<pointer>(m_allocator->Allocate(size * sizeof(T), alignof(T)));
    }

    void deallocate(pointer ptr, size_type size) {
        m_allocator->Deallocate(ptr);
    }

    template<typename X, typename... Args>
    void construct(X* ptr, Args&&... args)
        noexcept(std::is_nothrow_constructible<X, Args...>::value) {
        ::new(ptr) X(std::forward<Args>(args)...);
    }

    template<typename X>
    void destroy(X* ptr) noexcept(std::is_nothrow_destructible<X>::value) { ptr->~X(); }

    size_type max_size() const noexcept {
        return m_allocator->GetMemorySize();
    }

private:
    std::shared_ptr<Allocator> m_allocator;
};
#endif
