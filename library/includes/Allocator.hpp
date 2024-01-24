#ifndef CALLISTO_ALLOCATOR_HPP_
#define CALLISTO_ALLOCATOR_HPP_
#include <Buddy.hpp>
#include <concepts>

class Allocator
{
public:
    inline Allocator(size_t memoryStart, size_t memorySize, size_t minimumBlockSize) noexcept
        : m_allocator{ memoryStart, memorySize, minimumBlockSize } {}
    inline Allocator(void* memoryStart, size_t memorySize, size_t minimumBlockSize) noexcept
        : Allocator{ ToSizeT(memoryStart), memorySize, minimumBlockSize } {}
    inline Allocator(
        size_t memoryStart, size_t memorySize, size_t minimumBlockSize, size_t defaultAlignment
    ) noexcept
        : m_allocator{ memoryStart, memorySize, minimumBlockSize, defaultAlignment } {}
    inline Allocator(
        void* memoryStart, size_t memorySize, size_t minimumBlockSize, size_t defaultAlignment
    ) noexcept
        : Allocator{ ToSizeT(memoryStart), memorySize, minimumBlockSize, defaultAlignment } {}

    template<typename T = void>
    [[nodiscard]]
    inline T* Allocate(size_t size, size_t alignment)
    {
        return reinterpret_cast<T*>(m_allocator.Allocate(size, alignment));
    }

    template<std::integral T = size_t>
    [[nodiscard]]
    inline T AllocateI(size_t size, size_t alignment)
    {
        return static_cast<T>(m_allocator.Allocate(size, alignment));
    }

    inline void Deallocate(void* ptr, size_t size, size_t alignment) noexcept
    {
        m_allocator.Deallocate(ToSizeT(ptr), size, alignment);
    }

    template<typename T = void>
    [[nodiscard]]
    inline T* Allocate(size_t size)
    {
        return reinterpret_cast<T*>(m_allocator.Allocate(size));
    }

    template<std::integral T = size_t>
    [[nodiscard]]
    inline T AllocateI(size_t size)
    {
        return static_cast<T>(m_allocator.Allocate(size));
    }

    inline void Deallocate(void* ptr, size_t size) noexcept
    {
        m_allocator.Deallocate(ToSizeT(ptr), size);
    }


    [[nodiscard]]
    inline size_t GetMemorySize() const noexcept { return m_allocator.TotalSize(); }
    [[nodiscard]]
    inline size_t GetAvailableSize() const noexcept { return m_allocator.AvailableSize(); }

private:
    [[nodiscard]]
    inline static size_t ToSizeT(void* ptr) noexcept
    {
        return reinterpret_cast<size_t>(ptr);
    }

private:
    Buddy m_allocator;

public:
    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;

    inline Allocator(Allocator&& alloc) noexcept : m_allocator{ std::move(alloc.m_allocator) } {}

    inline Allocator& operator=(Allocator&& alloc) noexcept
    {
        m_allocator = std::move(alloc.m_allocator);

        return *this;
    }
};
#endif
