#include <Allocator.hpp>

Allocator::Allocator(size_t memoryStart, size_t memorySize) noexcept
    : m_memoryStart{ memoryStart }, m_memorySize{ memorySize },
    m_memTree{ memoryStart, memorySize } {}

Allocator::Allocator(void* memoryStart, size_t memorySize) noexcept
    : m_memoryStart{ reinterpret_cast<size_t>(memoryStart) }, m_memorySize{ memorySize },
    m_memTree{ m_memoryStart, memorySize } {}

Allocator::Allocator(const Allocator& alloc) noexcept
    : m_memoryStart{ alloc.m_memoryStart }, m_memorySize{ alloc.m_memorySize },
    m_memTree{ alloc.m_memTree } {}

Allocator::Allocator(Allocator&& alloc) noexcept
    : m_memoryStart{ alloc.m_memoryStart }, m_memorySize{ alloc.m_memorySize },
    m_memTree{ std::move(alloc.m_memTree) } {}

Allocator& Allocator::operator=(const Allocator& alloc) noexcept {
    m_memoryStart = alloc.m_memoryStart;
    m_memorySize = alloc.m_memorySize;
    m_memTree = alloc.m_memTree;

    return *this;
}

Allocator& Allocator::operator=(Allocator&& alloc) noexcept {
    m_memoryStart = alloc.m_memoryStart;
    m_memorySize = alloc.m_memorySize;
    m_memTree = std::move(alloc.m_memTree);

    return *this;
}

void* Allocator::Allocate(size_t size, size_t alignment) {
    return reinterpret_cast<void*>(m_memTree.Allocate(size, alignment));
}

void Allocator::Deallocate(void* ptr) noexcept {}

size_t Allocator::GetMemorySize() const noexcept {
    return m_memorySize;
}
