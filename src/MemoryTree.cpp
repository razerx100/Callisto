#include <MemoryTree.hpp>
#include <cassert>
#include <ranges>
#include <limits>
#include <algorithm>


// Static functions
[[nodiscard]]
static size_t Align(size_t address, size_t alignment) noexcept
{
    return (address + (alignment - 1u)) & ~(alignment - 1u);
}

[[nodiscard]]
static size_t GetUpperBound2sExponent(size_t size) noexcept
{
    size_t result = 4u;
    for (; result < size; result <<= 1u);

    return result;
}

[[nodiscard]]
static size_t GetAlignedSize(size_t startingAddress, size_t alignment, size_t size) noexcept
{
    return size + (Align(startingAddress, alignment) - startingAddress);
}


// Memory Tree
MemoryTree::MemoryTree(size_t startingAddress, size_t size) noexcept
    : m_rootIndex{ 0u }, m_totalSize{ size } {
    assert(size % 4u == 0u && "Not divisible by 4u");

    size_t blockSize = 4u;
    size_t previousBlockCount = 0u;
    std::vector<size_t> rootSChildren;

    CreateChildren<true>(startingAddress, size, blockSize, previousBlockCount, rootSChildren);
    blockSize <<= 1u;

    for (; blockSize < size; blockSize <<= 1u)
        CreateChildren<false>(startingAddress, size, blockSize, previousBlockCount, rootSChildren);

    MemoryBlock memoryBlock{
        .startingAddress = startingAddress,
        .size = size,
        .available = true
    };
    BlockNode root{
        .block = memoryBlock,
        .parentIndex = std::numeric_limits<size_t>::max()
    };

    m_rootIndex = std::size(m_memTree);

    {
        auto& children = root.childrenIndices;
        // Root shall either have only a single immediate children or two.
        // If the rootSChildren array is empty, that means root has two immediate children
        if (std::empty(rootSChildren) && size != 4u) {
            const size_t child1 = std::size(m_memTree) - 2u;
            const size_t child2 = child1 + 1u;

            children.emplace_back(child1);
            children.emplace_back(child2);
        }
        else
            for (auto child : rootSChildren) {
                children.emplace_back(child);

                m_memTree[child].parentIndex = m_rootIndex;
            }
    }

    m_memTree.emplace_back(root);
    m_availableBlocks.emplace_back(m_rootIndex);
}

size_t MemoryTree::Allocate(size_t size, size_t alignment) {
    static constexpr size_t nullValue = std::numeric_limits<size_t>::max();

    size_t blockIndex = nullValue;
    for (size_t nodeIndex : m_availableBlocks) {
        blockIndex = FindAvailableBlockRecursive(size, alignment, nodeIndex);
        if (blockIndex != nullValue) {
            BlockNode& node = m_memTree[blockIndex];
            MemoryBlock& memBlock = node.block;

            memBlock.available = false;

            RemoveIndexFromAvailable(nodeIndex);

            const size_t parentIndex = node.parentIndex;
            if (parentIndex != nullValue) // Value should never be null but still checking
                ManageAvailableBlocksRecursive(parentIndex);

            break;
        }
    }

    // The root-node's size can be a non-exponent of 2 but the children are all 2's exponent.
    // So, if the root is a non-exponent of 2, FindBlockRecursive will return null even if
    // there is enough space to allocate
    if (blockIndex == nullValue) {
        BlockNode& root = m_memTree[m_rootIndex];
        MemoryBlock& memBlock = root.block;
        const size_t alignedSize = GetAlignedSize(memBlock.startingAddress, alignment, size);

        if (memBlock.available && alignedSize <= memBlock.size) {
            memBlock.available = false;
            blockIndex = m_rootIndex;

            RemoveIndexFromAvailable(m_rootIndex);
        }
    }

    assert(blockIndex != nullValue && "Not enough memory available for allocation");

    const MemoryBlock& memBlock = m_memTree[blockIndex].block;

    return Align(memBlock.startingAddress, alignment);
}

void MemoryTree::ManageAvailableBlocksRecursive(size_t nodeIndex) noexcept {
    BlockNode& node = m_memTree[nodeIndex];
    MemoryBlock& memBlock = node.block;
    memBlock.available = false;

    for (size_t childIndex : node.childrenIndices) {
        BlockNode& childNode = m_memTree[childIndex];

        if (childNode.block.available) {
            auto findIt = std::ranges::find(m_availableBlocks, childIndex);

            if (findIt == std::end(m_availableBlocks))
                m_availableBlocks.emplace_back(childIndex);
        }
    }

    if (node.parentIndex != std::numeric_limits<size_t>::max())
        ManageAvailableBlocksRecursive(node.parentIndex);
}

void MemoryTree::RemoveIndexFromAvailable(size_t index) noexcept {
    auto findIt = std::ranges::find(m_availableBlocks, index);
    if (findIt != std::end(m_availableBlocks))
        m_availableBlocks.erase(findIt);
}

size_t MemoryTree::FindAvailableBlockRecursive(
    size_t size, size_t alignment, size_t nodeIndex
) const noexcept {
    const BlockNode& node = m_memTree[nodeIndex];

    static constexpr size_t nullValue = std::numeric_limits<size_t>::max();

    // If the children have enough space to allocate, prioritise them
    for (size_t child : node.childrenIndices) {
        const BlockNode& childNode = m_memTree[child];
        const MemoryBlock& childBlock = childNode.block;

        size_t blockIndex = nullValue;
        // No need to check for availability, since the branch's root is selected from
        // available-blocks
        if (childBlock.size >= size)
            blockIndex = FindAvailableBlockRecursive(size, alignment, child);

        if (blockIndex != nullValue)
            return blockIndex;
    }

    const MemoryBlock& memBlock = node.block;
    const size_t alignedSize = GetAlignedSize(memBlock.startingAddress, alignment, size);

    const size_t blockSize = GetUpperBound2sExponent(alignedSize);
    if (blockSize == memBlock.size)
        return nodeIndex;

    return nullValue;
}

void MemoryTree::Deallocate(size_t address, size_t size) noexcept {
    static constexpr size_t nullValue = std::numeric_limits<size_t>::max();

    const size_t blockIndex = FindUnavailableBlockRecursive(address, size, m_rootIndex);

    assert(blockIndex != nullValue && "Can't find the block for deallocation.");

    BlockNode& node = m_memTree[blockIndex];
    MemoryBlock& memBlock = node.block;
    memBlock.available = true;
    m_availableBlocks.emplace_back(blockIndex);

    const size_t parentIndex = node.parentIndex;
    if (parentIndex != nullValue)
        ManageUnavailableBlocksRecursive(parentIndex);
}

size_t MemoryTree::FindUnavailableBlockRecursive(
    size_t address, size_t size, size_t nodeIndex
) const noexcept {
    const BlockNode& node = m_memTree[nodeIndex];

    for (size_t child : node.childrenIndices) {
        const BlockNode& childNode = m_memTree[child];
        const MemoryBlock& childBlock = childNode.block;

        // The last unavailable node of a branch with the address in it will be the desired
        // block
        if (IsAddressInBlock(childBlock, address) && !childBlock.available)
            return FindUnavailableBlockRecursive(address, size, child);
    }

    return nodeIndex;
}

bool MemoryTree::IsAddressInBlock(const MemoryBlock& block, size_t ptrAddress) noexcept {
    const size_t nextBlockStart = block.startingAddress + block.size;

    return block.startingAddress <= ptrAddress && ptrAddress < nextBlockStart;
}

void MemoryTree::ManageUnavailableBlocksRecursive(size_t nodeIndex) noexcept {
    BlockNode& node = m_memTree[nodeIndex];
    MemoryBlock& memBlock = node.block;

    bool childrenAvailable = true;
    for (size_t childIndex : node.childrenIndices) {
        BlockNode& childNode = m_memTree[childIndex];

        if (!childNode.block.available) {
            childrenAvailable = false;
            break;
        }
    }

    // If all of the children are available, then remove them from the availblocks and add
    // their parent instead
    if (childrenAvailable) {
        for (size_t childIndex : node.childrenIndices)
            RemoveIndexFromAvailable(childIndex);

        memBlock.available = true;
        m_availableBlocks.emplace_back(nodeIndex);
    }

    if (node.parentIndex != std::numeric_limits<size_t>::max())
        ManageUnavailableBlocksRecursive(node.parentIndex);
}
