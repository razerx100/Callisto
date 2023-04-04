#include <MemoryTree.hpp>
#include <cassert>
#include <ranges>
#include <limits>
#include <algorithm>

size_t MemoryTree::Align(size_t address, size_t alignment) noexcept {
    return (address + (alignment - 1u)) & ~(alignment - 1u);
}

MemoryTree::MemoryTree(size_t startingAddress, size_t size) noexcept : m_rootIndex{ 0u } {
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

MemoryTree::MemoryTree(const MemoryTree& memTree) noexcept
    : m_memTree{ memTree.m_memTree }, m_availableBlocks{ memTree.m_availableBlocks },
    m_rootIndex{ memTree.m_rootIndex } {}

MemoryTree::MemoryTree(MemoryTree&& memTree) noexcept
    : m_memTree{ std::move(memTree.m_memTree) },
    m_availableBlocks{ std::move(memTree.m_availableBlocks) },
    m_rootIndex{ memTree.m_rootIndex } {}

MemoryTree& MemoryTree::operator=(const MemoryTree& memTree) noexcept {
    m_memTree = memTree.m_memTree;
    m_availableBlocks = memTree.m_availableBlocks;
    m_rootIndex = memTree.m_rootIndex;

    return *this;
}

MemoryTree& MemoryTree::operator=(MemoryTree&& memTree) noexcept {
    m_memTree = std::move(memTree.m_memTree);
    m_availableBlocks = std::move(memTree.m_availableBlocks);
    m_rootIndex = memTree.m_rootIndex;

    return *this;
}

size_t MemoryTree::Allocate(size_t size, size_t alignment) {
    static constexpr size_t nullValue = std::numeric_limits<size_t>::max();

    size_t blockIndex = nullValue;
    for (size_t nodeIndex : m_availableBlocks) {
        blockIndex = FindBlockRecursive(size, alignment, nodeIndex);
        if (blockIndex != nullValue) {
            BlockNode& node = m_memTree[blockIndex];
            MemoryBlock& memBlock = node.block;

            memBlock.available = false;

            auto findIt = std::ranges::find(m_availableBlocks, nodeIndex);
            m_availableBlocks.erase(findIt);

            const size_t parentIndex = node.parentIndex;
            if (parentIndex != nullValue)
                ManageAvailableBlocksRecursive(parentIndex);

            break;
        }
    }

    if (blockIndex == nullValue) {
        BlockNode& root = m_memTree[m_rootIndex];
        MemoryBlock& memBlock = root.block;
        const size_t alignedSize = GetAlignedSize(memBlock.startingAddress, alignment, size);

        if (memBlock.available && alignedSize <= memBlock.size) {
            memBlock.available = false;
            blockIndex = m_rootIndex;
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

size_t MemoryTree::FindBlockRecursive(
    size_t size, size_t alignment, size_t nodeIndex
) const noexcept {
    const BlockNode& node = m_memTree[nodeIndex];
    const MemoryBlock& memBlock = node.block;

    static constexpr size_t nullValue = std::numeric_limits<size_t>::max();

    // If the current block is larger than necessary, look through the children to see if they fit
    // better
    if (memBlock.size > size) {
        for (size_t child : node.childrenIndices) {
            const size_t blockIndex = FindBlockRecursive(size, alignment, child);

            if (blockIndex != nullValue)
                return blockIndex;
        }
    }

    const size_t alignedSize = GetAlignedSize(memBlock.startingAddress, alignment, size);

    const size_t blockSize = GetUpperBound2sExponent(alignedSize);
    if (memBlock.available && blockSize == memBlock.size)
        return nodeIndex;

    return nullValue;
}

size_t MemoryTree::GetUpperBound2sExponent(size_t size) noexcept {
    size_t result = 4u;
    for (; result < size; result <<= 1u);

    return result;
}

size_t MemoryTree::GetAlignedSize(size_t startingAddress, size_t alignment, size_t size) noexcept {
    const size_t alignedAddress = Align(startingAddress, alignment);

    return size + (alignedAddress - startingAddress);
}
