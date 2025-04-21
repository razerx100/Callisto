#include <SharedBufferAllocator.hpp>
#include <ranges>
#include <algorithm>

namespace Callisto
{
void SharedBufferAllocator::AddAllocInfo(size_t offset, size_t size) noexcept
{
	// To find the possible previous alloc info, that allocation's offset + size should be
	// the same as the argument offset.
	const size_t possibleNextOffset   = offset + size;

	size_t previousAdjacentBlockIndex = std::numeric_limits<size_t>::max();
	size_t nextAdjacentBlockIndex     = std::numeric_limits<size_t>::max();

	const size_t availableMemoryCount = std::size(m_availableMemory);

	// Check if we can find adjacent blocks. There should be only two at max,
	// one before and one after, if there is any.
	for (size_t index = 0u; index < availableMemoryCount; ++index)
	{
		const AllocInfo& allocInfo = m_availableMemory[index];

		if (allocInfo.offset + allocInfo.size == offset)
			previousAdjacentBlockIndex = index;
		else if (possibleNextOffset == allocInfo.offset)
			nextAdjacentBlockIndex = index;

		const bool bothFound = nextAdjacentBlockIndex != std::numeric_limits<size_t>::max()
			&& previousAdjacentBlockIndex != std::numeric_limits<size_t>::max();

		if (bothFound)
			break;
	}

	if (previousAdjacentBlockIndex != std::numeric_limits<size_t>::max())
	{
		const AllocInfo& previousBlock = GetAndRemoveAllocInfo(previousAdjacentBlockIndex);

		offset = previousBlock.offset;

		size  += previousBlock.size;
	}

	if (nextAdjacentBlockIndex != std::numeric_limits<size_t>::max())
	{
		// Since we have searched for both of the indices on the same iteration,
		// if we have found both, removing the previousBlock can invalidate
		// the index of the nextBlock. So, need to fix that.
		if (previousAdjacentBlockIndex < nextAdjacentBlockIndex)
			--nextAdjacentBlockIndex;

		const AllocInfo& nextBlock = GetAndRemoveAllocInfo(nextAdjacentBlockIndex);

		size += nextBlock.size;
	}

	auto result = std::ranges::upper_bound(
		m_availableMemory, size, {},
		[](const AllocInfo& info) { return info.size; }
	);

	m_availableMemory.insert(result, AllocInfo{ offset, size });
}

std::optional<size_t> SharedBufferAllocator::GetAvailableAllocInfo(size_t size) const noexcept
{
	auto result = std::ranges::lower_bound(
		m_availableMemory, size, {},
		[](const AllocInfo& info) { return info.size; }
	);

	if (result != std::end(m_availableMemory))
		return std::distance(std::begin(m_availableMemory), result);
	else
		return {};
}

SharedBufferAllocator::AllocInfo SharedBufferAllocator::GetAndRemoveAllocInfo(size_t index) noexcept
{
	AllocInfo allocInfo = m_availableMemory[index];

	m_availableMemory.erase(std::next(std::begin(m_availableMemory), index));

	return allocInfo;
}

size_t SharedBufferAllocator::AllocateMemory(const AllocInfo& allocInfo, size_t size) noexcept
{
	const size_t offset     = allocInfo.offset;
	const size_t freeMemory = allocInfo.size - size;

	if (freeMemory)
		AddAllocInfo(offset + size, freeMemory);

	return offset;
}
}
