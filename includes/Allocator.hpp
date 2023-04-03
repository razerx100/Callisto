#ifndef CALLISTO_ALLOCATOR_HPP_
#define CALLISTO_ALLOCATOR_HPP_
#include <MemoryTree.hpp>

class Allocator {
public:
    Allocator(size_t memoryStart, size_t memorySize) noexcept;
    Allocator(void* memoryStart, size_t memorySize) noexcept;

    Allocator(const Allocator& alloc) noexcept;
    Allocator(Allocator&& alloc) noexcept;

    Allocator& operator=(const Allocator& alloc) noexcept;
    Allocator& operator=(Allocator&& alloc) noexcept;

    [[nodiscard]]
    void* Allocate(size_t size, size_t alignment);

    void Deallocate(void* ptr) noexcept;

    [[nodiscard]]
    size_t GetMemorySize() const noexcept;

private:
    size_t m_memoryStart;
    size_t m_memorySize;
    MemoryTreeA m_memTree;
};
#endif
