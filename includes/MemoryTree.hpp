#ifndef MEMORY_TREE_HPP_
#define MEMORY_TREE_HPP_
#include <cstdint>
#include <vector>
#include <memory>

class MemoryTree {
    struct MemoryBlock {
        size_t startingAddress;
        size_t size;
        bool available;
    };

    struct BlockNode {
        MemoryBlock block;
        size_t parentIndex;
        std::vector<size_t> childrenIndices;
    };

public:
    MemoryTree(size_t startingAddress, size_t size) noexcept;

    MemoryTree(const MemoryTree& memTree) noexcept;
    MemoryTree(MemoryTree&& memTree) noexcept;

    MemoryTree& operator=(const MemoryTree& memTree) noexcept;
    MemoryTree& operator=(MemoryTree&& memTree) noexcept;

    [[nodiscard]]
    size_t Allocate(size_t size, size_t alignment);

private:
    [[nodiscard]]
    static size_t Align(size_t address, size_t alignment) noexcept;

    [[nodiscard]]
    static size_t GetUpperBound2sExponent(size_t size) noexcept;
    [[nodiscard]]
    static size_t GetAlignedSize(
        size_t startingAddress, size_t alignment, size_t size
    ) noexcept;

    [[nodiscard]]
    size_t FindBlockRecursive(size_t size, size_t alignment, size_t nodeIndex) noexcept;

    void DeactivateParentsRecursive(size_t nodeIndex) noexcept;

    template<bool blockSize4>
    void CreateChildren(
        size_t startingAddress, size_t totalSize, size_t blockSize, size_t& previousBlockCount,
        std::vector<size_t>& rootSChildren
    ) noexcept {
        const size_t blockCount = totalSize / blockSize;
        const size_t immediateParentCount =
            blockCount % 2 == 0u ? blockCount : blockCount - 1u;

        MemoryBlock memBlock{
            .size = blockSize,
            .available = true
        };

        // Since 0 % 2 == 0 would be true, moving the index back by 1
        size_t parentIndex = std::size(m_memTree) + blockCount - 1u;
        size_t childrenIndicesStart = std::size(m_memTree) - previousBlockCount;

        for (
            size_t index = 0u, offset = startingAddress;
            index < blockCount; ++index, offset += blockSize
            ) {
            memBlock.startingAddress = offset;
            BlockNode node{
                .block = memBlock,
                .parentIndex = std::numeric_limits<size_t>::max()
            };

            // Parents
            if (index < immediateParentCount) {
                if (index % 2u == 0u)
                    ++parentIndex;
                node.parentIndex = parentIndex;
            }
            else
                rootSChildren.emplace_back(std::size(m_memTree));

            // Children
            if constexpr (!blockSize4) {
                auto& children = node.childrenIndices;
                children.emplace_back(childrenIndicesStart++);
                children.emplace_back(childrenIndicesStart++);
            }

            m_memTree.emplace_back(node);
        }

        previousBlockCount = blockCount;
    }

private:
    std::vector<BlockNode> m_memTree;
    size_t m_rootIndex;
};
#endif
