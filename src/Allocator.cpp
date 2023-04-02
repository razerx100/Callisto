#include <Allocator.hpp>
#include <cassert>

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

// Memory Tree
MemoryTree::MemoryTree(MemoryTree&& memTree) noexcept : m_root{ std::move(memTree.m_root) } {}

MemoryTree::MemoryTree(const MemoryTree& memTree) noexcept : m_root{ memTree.m_root } {}

MemoryTree& MemoryTree::operator=(const MemoryTree& memTree) noexcept {
    m_root = memTree.m_root;

    return *this;
}

MemoryTree& MemoryTree::operator=(MemoryTree&& memTree) noexcept {
    m_root = std::move(memTree.m_root);

    return *this;
}

MemoryTree::MemoryTree(size_t startingAddress, size_t size) noexcept {
    assert(size % 4u == 0u && "Not divisible by 4u");

    MemoryBlock block{
        .startingAddress = startingAddress,
        .size = size,
        .available = size
    };

    BlockNode node{ .block = block };
    m_root = std::make_shared<BlockNode>(node);

    const size_t smaller2sExponent = GetUpperBound2sExponent(size) >> 1u;
    CreateChildren(m_root, startingAddress, smaller2sExponent, size);
}

size_t MemoryTree::GetUpperBound2sExponent(size_t size) noexcept {
    size_t result = 4u;
    for (; result < size; result <<= 1u);

    return result;
}

size_t MemoryTree::GetLowerBound2sExponent(size_t size) noexcept {
    size_t result = 4u;
    for (; result <= size; result <<= 1u);

    return result >> 1u;
}

void MemoryTree::CreateChildren(
    std::shared_ptr<BlockNode> parent, size_t startingAddress, size_t blockSize,
    size_t totalSize
) noexcept {
    if (blockSize >= 4u) {
        MemoryBlock block{
        .size = blockSize,
        .available = blockSize
        };

        const size_t offsettedSize = startingAddress + totalSize;
        const size_t smaller2sExponent = blockSize >> 1u;

        size_t offset = startingAddress;
        size_t withinSize = offset + blockSize;

        for (; withinSize <= offsettedSize; offset += blockSize, withinSize += blockSize) {
            block.startingAddress = offset;
            BlockNode node{ .block = block, .parent = parent };
            auto child = std::make_shared<BlockNode>(node);

            parent->children.emplace_back(child);

            CreateChildren(child, offset, smaller2sExponent, blockSize);
        }

        if (offset < offsettedSize)
            CreateChildren(parent, offset, smaller2sExponent, offsettedSize - offset);
    }
}

size_t MemoryTree::Allocate(size_t size, size_t alignment) {
    return 0u;
}
