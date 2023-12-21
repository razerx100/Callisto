#include <MemoryTree.hpp>
#include <cassert>
#include <ranges>
#include <limits>
#include <algorithm>
#include <Exception.hpp>


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
    : m_rootIndex{ 0u }, m_totalSize{ size }
{
    assert(size % 4u == 0u && "Not divisible by 4u");

    size_t blockSize = 4u;
    size_t previousBlockCount = 0u;
    std::vector<size_t> rootSChildren{};

    // Create the first level of level children without adding any childrenIndices, since
    // those wouldn't have any children.
    CreateChildren<false>(startingAddress, size, blockSize, previousBlockCount, rootSChildren);
    blockSize <<= 1u;

    // Increase the blockSize by 2's exponent and create new levels.
    for (; blockSize < size; blockSize <<= 1u)
        CreateChildren<true>(startingAddress, size, blockSize, previousBlockCount, rootSChildren);

    const MemoryBlock memoryBlock{
        .startingAddress = startingAddress, .size = size, .available = true
    };
    BlockNode root
    {
        .block = memoryBlock, .parentIndex = {}
    };

    // Haven't added the root node yet, so the size of memTree is its index
    m_rootIndex = std::size(m_memTree);

    {
        std::vector<size_t>& children = root.childrenIndices;
        // If rootsChildren is empty, that means root only has immediate children.
        // But if the total memory size is only 4bytes, that means root is the only
        // node and thus has not children.
        if (std::empty(rootSChildren) && size != 4u)
        {
            const size_t immediateChild1 = std::size(m_memTree) - 2u;

            children.emplace_back(immediateChild1);
            children.emplace_back(immediateChild1 + 1u);
        }
        else
            for (auto child : rootSChildren)
            {
                children.emplace_back(child);

                m_memTree[child].parentIndex = m_rootIndex;
            }
    }

    m_memTree.emplace_back(root);
    m_availableBlocks.emplace_back(m_rootIndex);
}

size_t MemoryTree::Allocate(size_t size, size_t alignment)
{
    std::optional<size_t> blockIndex{};

    // Look through the available blocks for a block
    for (size_t nodeIndex : m_availableBlocks)
    {
        blockIndex = FindAvailableBlockRecursive(size, alignment, nodeIndex);
        if (blockIndex)
        {
            BlockNode& node = m_memTree[blockIndex.value()];

            MemoryBlock& memBlock = node.block;
            memBlock.available = false;

            RemoveIndexFromAvailable(nodeIndex);

            const std::optional<size_t> parentIndex = node.parentIndex;

            if (parentIndex)
                ManageAvailableBlocksRecursive(parentIndex.value());

            break;
        }
    }

    // The root-node's size might not be an exponent of 2 but the children are all 2's exponent.
    // So, if the root is not an exponent of 2, FindAvailableBlockRecursive will return null
    // even if there is enough space to allocate on the root. So, we have to check if it's
    // still possible to allocate on the root node.
    if (!blockIndex)
    {
        MemoryBlock& memBlock = m_memTree[m_rootIndex].block;
        const size_t alignedSize = GetAlignedSize(memBlock.startingAddress, alignment, size);

        if (memBlock.available && alignedSize <= memBlock.size)
        {
            memBlock.available = false;
            blockIndex = m_rootIndex;

            RemoveIndexFromAvailable(m_rootIndex);
        }
    }

    if (!blockIndex)
        throw Exception("AllocationError", "Not enough memory available for allocation.");

    const MemoryBlock& memBlock = m_memTree[blockIndex.value()].block;

    return Align(memBlock.startingAddress, alignment);
}

// Makes all the parent nodes unavailable and add their available children to the
// availableBlocks.
void MemoryTree::ManageAvailableBlocksRecursive(size_t parentNodeIndex) noexcept
{
    // Make the parent unavailable.
    BlockNode& parentNode = m_memTree[parentNodeIndex];
    MemoryBlock& memBlock = parentNode.block;
    memBlock.available = false;

    // Add the available children to the availableBlocks.
    for (size_t childIndex : parentNode.childrenIndices)
    {
        BlockNode& childNode = m_memTree[childIndex];

        if (childNode.block.available)
        {
            auto findIt = std::ranges::find(m_availableBlocks, childIndex);

            if (findIt == std::end(m_availableBlocks))
                m_availableBlocks.emplace_back(childIndex);
        }
    }

    // Recursively check the parent nodes.
    if (parentNode.parentIndex)
        ManageAvailableBlocksRecursive(parentNode.parentIndex.value());
}

void MemoryTree::RemoveIndexFromAvailable(size_t index) noexcept
{
    auto findIt = std::ranges::find(m_availableBlocks, index);
    if (findIt != std::end(m_availableBlocks))
        m_availableBlocks.erase(findIt);
}

std::optional<size_t> MemoryTree::FindAvailableBlockRecursive(
    size_t size, size_t alignment, size_t nodeIndex
) const noexcept
{
    const BlockNode& node = m_memTree[nodeIndex];

    // If the children have enough space to allocate, prioritise them.
    for (size_t child : node.childrenIndices)
    {
        const BlockNode& childNode = m_memTree[child];
        const MemoryBlock& childBlock = childNode.block;

        std::optional<size_t> blockIndex{};
        // No need to check for availability here, since the branch's root is selected from
        // available-blocks.
        if (childBlock.size >= size)
            blockIndex = FindAvailableBlockRecursive(size, alignment, child);

        if (blockIndex)
            return blockIndex;
    }

    // Try to allocate on the current node otherwise.
    const MemoryBlock& memBlock = node.block;
    const size_t alignedSize = GetAlignedSize(memBlock.startingAddress, alignment, size);

    // Every block except the root block will be a 2's exponent. So, change the aligned
    // required size to the next higher 2s exponent to see if it can fit inside. This also
    // means this function will return null for root since root might not an exponent of 2.
    const size_t blockSize = GetUpperBound2sExponent(alignedSize);
    if (blockSize == memBlock.size)
        return nodeIndex;

    return {};
}

void MemoryTree::Deallocate(size_t address, size_t size) noexcept
{
    static constexpr size_t nullValue = std::numeric_limits<size_t>::max();

    const size_t blockIndex = FindUnavailableBlockRecursive(address, size, m_rootIndex);

    BlockNode& node = m_memTree[blockIndex];
    MemoryBlock& memBlock = node.block;
    memBlock.available = true;
    m_availableBlocks.emplace_back(blockIndex);

    const std::optional<size_t> parentIndex = node.parentIndex;
    if (parentIndex)
        ManageUnavailableBlocksRecursive(parentIndex.value());
}

size_t MemoryTree::FindUnavailableBlockRecursive(
    size_t address, size_t size, size_t nodeIndex
) const noexcept
{
    const BlockNode& node = m_memTree[nodeIndex];

    for (size_t child : node.childrenIndices)
    {
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

// Check the children of a node to see all the children are available, if all of the children
// are available, then make the parent available. And do this recursively.
void MemoryTree::ManageUnavailableBlocksRecursive(size_t parentNodeIndex) noexcept
{
    BlockNode& parentNode = m_memTree[parentNodeIndex];
    MemoryBlock& memBlock = parentNode.block;

    // Check if all of the children are available.
    bool childrenAvailable = true;
    for (size_t childIndex : parentNode.childrenIndices)
    {
        BlockNode& childNode = m_memTree[childIndex];

        if (!childNode.block.available)
        {
            childrenAvailable = false;
            break;
        }
    }

    // If all of the children are available, then remove them from the availableBlocks and add
    // their parent instead.
    if (childrenAvailable)
    {
        for (size_t childIndex : parentNode.childrenIndices)
            RemoveIndexFromAvailable(childIndex);

        memBlock.available = true;
        m_availableBlocks.emplace_back(parentNodeIndex);
    }

    // Recursively check the parent nodes.
    if (parentNode.parentIndex)
        ManageUnavailableBlocksRecursive(parentNode.parentIndex.value());
}
