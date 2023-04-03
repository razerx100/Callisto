#ifndef MEMORY_TREE_HPP_
#define MEMORY_TREE_HPP_
#include <cstdint>
#include <vector>
#include <memory>

struct MemoryBlock {
    size_t startingAddress;
    size_t size;
    bool available;
};

class MemoryTreeP {
    struct BlockNode {
        MemoryBlock block;
        std::shared_ptr<BlockNode> parent;
        std::vector<std::shared_ptr<BlockNode>> children;
    };

public:
    MemoryTreeP(size_t startingAddress, size_t size) noexcept;

    MemoryTreeP(const MemoryTreeP& memTree) noexcept;
    MemoryTreeP& operator=(const MemoryTreeP& memTree) noexcept;

    MemoryTreeP(MemoryTreeP&& memTree) noexcept;
    MemoryTreeP& operator=(MemoryTreeP&& memTree) noexcept;

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

class MemoryTreeA {
    struct BlockNode {
        MemoryBlock block;
        size_t parentIndex;
        std::vector<size_t> childrenIndices;
    };

public:
    MemoryTreeA(size_t startingAddress, size_t size) noexcept;

    MemoryTreeA(const MemoryTreeA& memTree) noexcept;
    MemoryTreeA(MemoryTreeA&& memTree) noexcept;

    MemoryTreeA& operator=(const MemoryTreeA& memTree) noexcept;
    MemoryTreeA& operator=(MemoryTreeA&& memTree) noexcept;

    [[nodiscard]]
    size_t Allocate(size_t size, size_t alignment);

private:
    std::vector<BlockNode> m_memTree;
    size_t m_rootIndex;
};
#endif
