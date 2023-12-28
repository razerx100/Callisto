#ifndef CALLISTO_ALLOCATOR_HPP_
#define CALLISTO_ALLOCATOR_HPP_
#include <MemoryTree.hpp>
#include <concepts>

class Allocator
{
public:
    inline Allocator(size_t memoryStart, size_t memorySize) noexcept
        : m_memTree{ memoryStart, memorySize } {}
    inline Allocator(void* memoryStart, size_t memorySize) noexcept
        : Allocator{ ToSizeT(memoryStart), memorySize } {}

    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;

    inline Allocator(Allocator&& alloc) noexcept : m_memTree{ std::move(alloc.m_memTree) } {}

    inline Allocator& operator=(Allocator&& alloc) noexcept
    {
        m_memTree = std::move(alloc.m_memTree);

        return *this;
    }

    template<typename T = void>
    [[nodiscard]]
    inline T* Allocate(size_t size, size_t alignment)
    {
        return reinterpret_cast<T*>(m_memTree.Allocate(size, alignment));
    }

    template<std::integral T = size_t>
    [[nodiscard]]
    inline T AllocateI(size_t size, size_t alignment)
    {
        return static_cast<T>(m_memTree.Allocate(size, alignment));
    }

    inline void Deallocate(void* ptr, size_t size) noexcept
    {
        m_memTree.Deallocate(ToSizeT(ptr), size);
    }

    [[nodiscard]]
    inline size_t GetMemorySize() const noexcept
    {
        return m_memTree.TotalSize();
    }

private:
    [[nodiscard]]
    inline static size_t ToSizeT(void* ptr) noexcept
    {
        return reinterpret_cast<size_t>(ptr);
    }

private:
    MemoryTree m_memTree;
};
#endif
