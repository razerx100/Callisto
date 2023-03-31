#ifndef CALLISTO_ALLOCATOR_HPP_
#define CALLISTO_ALLOCATOR_HPP_
#include <cstdint>

class Allocator {
public:
    Allocator() = default;
    Allocator(size_t memoryStart, size_t memorySize) noexcept;
    Allocator(void* memoryStart, size_t memorySize) noexcept;

    void SetMemory(size_t memoryStart, size_t memorySize) noexcept;
    void SetMemory(void* memoryStart, size_t memorySize) noexcept;

    [[nodiscard]]
    void* Allocate(size_t size, size_t alignment) noexcept;

    void Deallocate(void* ptr) noexcept;

    [[nodiscard]]
    size_t GetMemorySize() const noexcept;

private:
    size_t m_memoryStart;
    size_t m_memorySize;
    size_t m_offset;
};
#endif
