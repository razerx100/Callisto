#ifndef MEMORY_TREE_HPP_
#define MEMORY_TREE_HPP_
#include <cstdint>
#include <vector>
#include <memory>

class MemoryTree
{
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

    void ManageAvailableBlocksRecursive(size_t parentNodeIndex) noexcept;
    void ManageUnavailableBlocksRecursive(size_t parentNodeIndex) noexcept;
    void RemoveIndexFromAvailable(size_t index) noexcept;

    template<bool AddChildrenIndices>
    void CreateChildren(
        size_t startingAddress, size_t totalSize, size_t blockSize, size_t& previousBlockCount,
        std::vector<size_t>& rootSChildren
    ) noexcept {
        // We actually want an integer division here.
        const size_t blockCount = totalSize / blockSize;
        // If the blockCount is odd, then then a parent needs to have one child. Managing that
        // would be a mess. So, here will assign that extra child to root. So, the immediate
        // parent's count would be 1 less.
        const size_t immediateParentCount =
            blockCount % 2 == 0u ? blockCount : blockCount - 1u;

        MemoryBlock memBlock{ .size = blockSize, .available = true };

        // Assigning the last index of the current children as the parent, since it'll
        // be moved to the next index before actually setting it as the parent index.
        size_t parentIndex = std::size(m_memTree) + blockCount - 1u;
        size_t childrenIndicesStart = std::size(m_memTree) - previousBlockCount;
        size_t offset = startingAddress;

        for (size_t index = 0u; index < blockCount; ++index)
        {
            memBlock.startingAddress = offset;
            BlockNode node{
                .block = memBlock, .parentIndex = std::numeric_limits<size_t>::max()
            };

            // If the index is less than the immediateParentCount, add an immediate parent.
            if (index < immediateParentCount)
            {
                // A non-root parent can have 2 children. Since, the indices start at 0,
                // advance the parent's index on every even child's index.
                if (index % 2u == 0u)
                    ++parentIndex;
                node.parentIndex = parentIndex;
            }
            // Else add the child's index, aka memTree size (since the child hasn't been added
            // yet) to the root's children.
            else
                rootSChildren.emplace_back(std::size(m_memTree));

            // BlockSize 4 would be the leaf nodes and won't have any children. Since,
            // we know that at the compile time, instead of writing a different function
            // using constexpr if here.
            if constexpr (AddChildrenIndices)
            {
                std::vector<size_t>& children = node.childrenIndices;
                // Add the two children's index and also advance childrenIndicesStart for
                // the next iteration.
                children.emplace_back(childrenIndicesStart++);
                children.emplace_back(childrenIndicesStart++);
            }

            m_memTree.emplace_back(node);
            offset += blockSize;
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
