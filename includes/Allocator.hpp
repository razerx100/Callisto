#ifndef CALLISTO_ALLOCATOR_HPP_
#define CALLISTO_ALLOCATOR_HPP_
#include <cstdint>
#include <vector>
#include <memory>

class MemoryTree {
    struct MemoryBlock {
        size_t startingAddress;
        size_t size;
        size_t available;
    };

    struct BlockNode {
        MemoryBlock block;
        std::shared_ptr<BlockNode> parent;
        std::vector<std::shared_ptr<BlockNode>> children;
    };

public:
    MemoryTree(size_t startingAddress, size_t size) noexcept;

    MemoryTree(const MemoryTree& memTree) noexcept;
    MemoryTree& operator=(const MemoryTree& memTree) noexcept;

    MemoryTree(MemoryTree&& memTree) noexcept;
    MemoryTree& operator=(MemoryTree&& memTree) noexcept;

    [[nodiscard]]
    size_t Allocate(size_t size, size_t alignment);

private:
    [[nodiscard]]
    static size_t GetUpperBound2sExponent(size_t size) noexcept;
    [[nodiscard]]
    static size_t GetLowerBound2sExponent(size_t size) noexcept;

    static void CreateChildren(
        std::shared_ptr<BlockNode> parent, size_t startingAddress, size_t blockSize,
        size_t totalSize
    ) noexcept;

private:
    std::shared_ptr<BlockNode> m_root;
};

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
    MemoryTree m_memTree;
};
#endif
