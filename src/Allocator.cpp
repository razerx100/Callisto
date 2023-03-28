#include <Allocator.hpp>

Allocator::Allocator(std::uint8_t* memoryStart, size_t memorySize) noexcept
    : m_memoryStart{ memoryStart }, m_memorySize{ memorySize }, m_offset{ 0u } {}

void Allocator::SetMemory(std::uint8_t* memoryStart, size_t memorySize) noexcept {
    m_memoryStart = memoryStart;
    m_memorySize = memorySize;
}

void* Allocator::Allocate(size_t size, size_t alignment) noexcept {
    std::uint8_t* memory = m_memoryStart + m_offset;
    m_offset += size;

    return memory;
}

void Allocator::Deallocate(void* ptr) noexcept {}

size_t Allocator::GetMemorySize() const noexcept {
    return m_memorySize;
}
