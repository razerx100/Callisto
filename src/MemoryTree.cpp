#include <MemoryTree.hpp>
#include <cassert>
#include <ranges>
#include <limits>
#include <algorithm>

// Memory Tree Pointer
MemoryTreeP::MemoryTreeP(MemoryTreeP&& memTree) noexcept
    : m_root{ std::move(memTree.m_root) } {}

MemoryTreeP::MemoryTreeP(const MemoryTreeP& memTree) noexcept : m_root{ memTree.m_root } {}

MemoryTreeP& MemoryTreeP::operator=(const MemoryTreeP& memTree) noexcept {
    m_root = memTree.m_root;

    return *this;
}

MemoryTreeP&MemoryTreeP::operator=(MemoryTreeP&& memTree) noexcept {
    m_root = std::move(memTree.m_root);

    return *this;
}

MemoryTreeP::MemoryTreeP(size_t startingAddress, size_t size) noexcept {
    assert(size % 4u == 0u && "Not divisible by 4u");

    MemoryBlock block{
        .startingAddress = startingAddress,
        .size = size,
        .available = true
    };

    BlockNode node{ .block = block };
    m_root = std::make_shared<BlockNode>(node);

    const size_t smaller2sExponent = GetUpperBound2sExponent(size) >> 1u;
    CreateChildren(m_root, startingAddress, smaller2sExponent, size);
}

size_t MemoryTreeP::GetUpperBound2sExponent(size_t size) noexcept {
    size_t result = 4u;
    for (; result < size; result <<= 1u);

    return result;
}

size_t MemoryTreeP::GetLowerBound2sExponent(size_t size) noexcept {
    size_t result = 4u;
    for (; result <= size; result <<= 1u);

    return result >> 1u;
}

void MemoryTreeP::CreateChildren(
    std::shared_ptr<BlockNode> parent, size_t startingAddress, size_t blockSize,
    size_t totalSize
) noexcept {
    if (blockSize >= 4u) {
        MemoryBlock block{
        .size = blockSize,
        .available = true
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

size_t MemoryTreeP::Allocate(size_t size, size_t alignment) {
    return 0u;
}

// Memory Tree Array
MemoryTreeA::MemoryTreeA(size_t startingAddress, size_t size) noexcept : m_rootIndex{ 0u } {
    assert(size % 4u == 0u && "Not divisible by 4u");

    size_t blockSize = 4u;
    size_t previousBlockCount = 0u;
    std::vector<size_t> rootSChildren;

    for (; blockSize < size; blockSize <<= 1u) {
        const size_t blockCount = size / blockSize;
        const size_t immediateParentCount = blockCount % 2 == 0u ? blockCount : blockCount - 1u;

        MemoryBlock memBlock{
            .size = blockSize,
            .available = true
        };

        // Since 0 % 2 == 0 would be true, moving the parent-index back by 1
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

            // Parents setup
            if (index < immediateParentCount) {
                if (index % 2u == 0u)
                    ++parentIndex;
                node.parentIndex = parentIndex;
            }
            else
                rootSChildren.emplace_back(std::size(m_memTree));

            // Children setup
            if (blockSize != 4u) {
                auto& children = node.childrenIndices;
                children.emplace_back(childrenIndicesStart++);
                children.emplace_back(childrenIndicesStart++);
            }

            m_memTree.emplace_back(node);
        }

        previousBlockCount = blockCount;
    }

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
}

MemoryTreeA::MemoryTreeA(const MemoryTreeA& memTree) noexcept
    : m_memTree{ memTree.m_memTree }, m_rootIndex{ memTree.m_rootIndex } {}

MemoryTreeA::MemoryTreeA(MemoryTreeA&& memTree) noexcept
    : m_memTree{ std::move(memTree.m_memTree) }, m_rootIndex{ memTree.m_rootIndex } {}

MemoryTreeA& MemoryTreeA::operator=(const MemoryTreeA& memTree) noexcept {
    m_memTree = memTree.m_memTree;
    m_rootIndex = memTree.m_rootIndex;

    return *this;
}

MemoryTreeA& MemoryTreeA::operator=(MemoryTreeA&& memTree) noexcept {
    m_memTree = std::move(memTree.m_memTree);
    m_rootIndex = memTree.m_rootIndex;

    return *this;
}

size_t MemoryTreeA::Allocate(size_t size, size_t alignment) {
    return 0u;
}
