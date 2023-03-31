#include <Allocator.hpp>

Allocator::Allocator(size_t memoryStart, size_t memorySize) noexcept
    : m_memoryStart{ memoryStart }, m_memorySize{ memorySize }, m_offset{ 0u } {}

Allocator::Allocator(void* memoryStart, size_t memorySize) noexcept
    : m_memoryStart{ reinterpret_cast<size_t>(memoryStart) }, m_memorySize{ memorySize },
    m_offset{ 0 } {}

void Allocator::SetMemory(size_t memoryStart, size_t memorySize) noexcept {
    m_memoryStart = memoryStart;
    m_memorySize = memorySize;
}

void Allocator::SetMemory(void* memoryStart, size_t memorySize) noexcept {
    m_memoryStart = reinterpret_cast<size_t>(memoryStart);
    m_memorySize = memorySize;
}

void* Allocator::Allocate(size_t size, size_t alignment) noexcept {
    auto memory = reinterpret_cast<void*>(m_memoryStart + m_offset);
    m_offset += size;

    return memory;
}

void Allocator::Deallocate(void* ptr) noexcept {}

size_t Allocator::GetMemorySize() const noexcept {
    return m_memorySize;
}
