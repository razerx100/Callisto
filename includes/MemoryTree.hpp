#ifndef MEMORY_TREE_HPP_
#define MEMORY_TREE_HPP_
#include <cstdint>
#include <vector>
#include <memory>

class MemoryTree
{
    struct MemoryBlock
    {
        size_t startingAddress;
        size_t size;
        bool available;
    };

    struct BlockNode
    {
        MemoryBlock block;
        size_t parentIndex;
        std::vector<size_t> childrenIndices;
    };

public:
    MemoryTree(size_t startingAddress, size_t size) noexcept;

    inline MemoryTree(const MemoryTree& memTree) noexcept
        : m_memTree{ memTree.m_memTree }, m_availableBlocks{ memTree.m_availableBlocks },
        m_rootIndex{ memTree.m_rootIndex }, m_totalSize{ memTree.m_totalSize } {}
    inline MemoryTree(MemoryTree&& memTree) noexcept
        : m_memTree{ std::move(memTree.m_memTree) },
        m_availableBlocks{ std::move(memTree.m_availableBlocks) },
        m_rootIndex{ memTree.m_rootIndex }, m_totalSize{ memTree.m_totalSize } {}

    inline MemoryTree& operator=(const MemoryTree& memTree) noexcept
    {
        m_memTree = memTree.m_memTree;
        m_availableBlocks = memTree.m_availableBlocks;
        m_rootIndex = memTree.m_rootIndex;
        m_totalSize = memTree.m_totalSize;

        return *this;
    }
    inline MemoryTree& operator=(MemoryTree&& memTree) noexcept
    {
        m_memTree = std::move(memTree.m_memTree);
        m_availableBlocks = std::move(memTree.m_availableBlocks);
        m_rootIndex = memTree.m_rootIndex;
        m_totalSize = memTree.m_totalSize;

        return *this;
    }

    [[nodiscard]]
    size_t Allocate(size_t size, size_t alignment);

    [[nodiscard]]
    inline size_t TotalSize() const noexcept { return m_totalSize; }

    void Deallocate(size_t address, size_t size) noexcept;

private:
    [[nodiscard]]
    static bool IsAddressInBlock(const MemoryBlock& block, size_t ptrAddress) noexcept;

    [[nodiscard]]
    size_t FindAvailableBlockRecursive(
        size_t size, size_t alignment, size_t nodeIndex
    ) const noexcept;
    [[nodiscard]]
    size_t FindUnavailableBlockRecursive(
        size_t address, size_t size, size_t nodeIndex
    ) const noexcept;

    void ManageAvailableBlocksRecursive(size_t nodeIndex) noexcept;
    void ManageUnavailableBlocksRecursive(size_t nodeIndex) noexcept;
    void RemoveIndexFromAvailable(size_t index) noexcept;

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
    std::vector<size_t> m_availableBlocks;
    size_t m_rootIndex;
    size_t m_totalSize;
};
#endif
