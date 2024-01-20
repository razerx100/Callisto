#include <Buddy.hpp>
#include <ranges>
#include <algorithm>

Buddy::Buddy(size_t startingAddress, size_t totalSize, size_t minimumBlockSize/* = 256_B */)
	: AllocatorBase{ totalSize }, m_minimumBlockSize{ minimumBlockSize }, m_sixtyFourBitBlocks{},
	m_thirtyTwoBitBlocks{}, m_sixteenBitBlocks{}, m_eightBitBlocks{}
{
	// The total size might not be a 2s exponent. In that case, make a block with the largest 2s
	// exponent. Do the same on the leftover memory until all of the memory is divided into 2s
	// exponents.
	InitInitialAvailableBlocks(startingAddress, totalSize);
}

void Buddy::MakeNewAvailableBlock(size_t startingAddress, size_t size) noexcept
{
	const size_t bitsRequirementSize    = BitsNeededFor(size);
	const size_t bitsRequirementAddress = BitsNeededFor(startingAddress);

	if (bitsRequirementAddress <= 8u && bitsRequirementSize <= 8u)
		m_eightBitBlocks.emplace_back(MakeAllocInfo<std::uint8_t>(startingAddress, size));
	else if (bitsRequirementAddress <= 16u && bitsRequirementSize <= 16u)
		m_sixteenBitBlocks.emplace_back(MakeAllocInfo<std::uint16_t>(startingAddress, size));
	else if (bitsRequirementAddress <= 32u && bitsRequirementSize <= 32u)
		m_thirtyTwoBitBlocks.emplace_back(MakeAllocInfo<std::uint32_t>(startingAddress, size));
	else
		m_sixtyFourBitBlocks.emplace_back(MakeAllocInfo<std::uint64_t>(startingAddress, size));
}

void Buddy::MakeNewAvailableBlock(size_t size) noexcept
{
	const size_t bitsRequirementSize = BitsNeededFor(size);
	const size_t startingAddress     = 0u;

	if (bitsRequirementSize <= 8u)
		m_eightBitBlocks.emplace_back(MakeAllocInfo<std::uint8_t>(startingAddress, size));
	else if (bitsRequirementSize <= 16u)
		m_sixteenBitBlocks.emplace_back(MakeAllocInfo<std::uint16_t>(startingAddress, size));
	else if (bitsRequirementSize <= 32u)
		m_thirtyTwoBitBlocks.emplace_back(MakeAllocInfo<std::uint32_t>(startingAddress, size));
	else
		m_sixtyFourBitBlocks.emplace_back(MakeAllocInfo<std::uint64_t>(startingAddress, size));
}

void Buddy::MakeInitialBlocks(size_t blockSize) noexcept
{
	const size_t lowerBound2sExponent = GetLowerBound2sExponent(blockSize);
	const size_t leftMemory	          = blockSize - lowerBound2sExponent;

	MakeNewAvailableBlock(lowerBound2sExponent);

	if (leftMemory)
	{
		if (leftMemory >= m_minimumBlockSize)
			MakeInitialBlocks(leftMemory);
		else
		{
			m_totalSize    -= leftMemory;
			m_availableSize = m_totalSize;
		}
	}
}

void Buddy::AssignInitialBlockAddresses(size_t startingAddress) noexcept
{
	// Try to assign the address to the smallest blocks first. If they can't fit the address
	// try to fit it in a bigger block.

	const size_t bitsRequirementAddress = BitsNeededFor(startingAddress);
	auto SortBySize = []<std::integral T>(const AllocInfo<T>& info1, const AllocInfo<T>& info2)
		{
			return info1.size < info2.size;
		};

	// First check the lowest bit requirements to fit the address. And move all the smaller
	// blocks.
	if (bitsRequirementAddress <= 8u)
	{
		std::ranges::sort(m_eightBitBlocks, SortBySize);
	}

	else if (bitsRequirementAddress <= 16u)
	{
		std::ranges::move(m_eightBitBlocks, std::back_inserter(m_sixteenBitBlocks));

		std::ranges::sort(m_sixteenBitBlocks, SortBySize);
	}
	else if (bitsRequirementAddress <= 32u)
	{
		std::ranges::move(m_eightBitBlocks, std::back_inserter(m_thirtyTwoBitBlocks));
		std::ranges::move(m_sixteenBitBlocks, std::back_inserter(m_thirtyTwoBitBlocks));

		std::ranges::sort(m_thirtyTwoBitBlocks, SortBySize);
	}
	else
	{
		std::ranges::move(m_eightBitBlocks, std::back_inserter(m_thirtyTwoBitBlocks));
		std::ranges::move(m_sixteenBitBlocks, std::back_inserter(m_thirtyTwoBitBlocks));
		std::ranges::move(m_sixteenBitBlocks, std::back_inserter(m_thirtyTwoBitBlocks));

		std::ranges::sort(m_sixtyFourBitBlocks, SortBySize);
	}

	// Try to assign the addresses now. If a blockSize can't fit it anymore. Move the rest of
	// the blocks to a bigger blockList.
	// It's fine to start from the smallest one, since we have already moved the blocks from
	// the smaller blockLists. So, the address will be assigned on the correct BlockLists.
	{
		auto it = std::begin(m_eightBitBlocks);
		for (; it != std::end(m_eightBitBlocks); ++it)
		{
			AllocInfo8& info = *it;
			info.startingAddress = static_cast<std::uint8_t>(startingAddress);

			startingAddress += info.size;

			if (BitsNeededFor(startingAddress) > 8u)
				break;
		}

		std::move(it, std::end(m_eightBitBlocks), std::back_inserter(m_sixteenBitBlocks));
		std::ranges::sort(m_sixteenBitBlocks, SortBySize);
	}

	{
		auto it = std::begin(m_sixteenBitBlocks);
		for (; it != std::end(m_sixteenBitBlocks); ++it)
		{
			AllocInfo16& info = *it;
			info.startingAddress = static_cast<std::uint16_t>(startingAddress);

			startingAddress += info.size;

			if (BitsNeededFor(startingAddress) > 16u)
				break;
		}

		std::move(it, std::end(m_sixteenBitBlocks), std::back_inserter(m_thirtyTwoBitBlocks));
		std::ranges::sort(m_thirtyTwoBitBlocks, SortBySize);
	}

	{
		auto it = std::begin(m_thirtyTwoBitBlocks);
		for (; it != std::end(m_thirtyTwoBitBlocks); ++it)
		{
			AllocInfo32& info = *it;
			info.startingAddress = static_cast<std::uint32_t>(startingAddress);

			startingAddress += info.size;

			if (BitsNeededFor(startingAddress) > 32u)
				break;
		}

		std::move(it, std::end(m_thirtyTwoBitBlocks), std::back_inserter(m_sixtyFourBitBlocks));
		std::ranges::sort(m_sixtyFourBitBlocks, SortBySize);
	}

	{
		auto it = std::begin(m_sixtyFourBitBlocks);
		for (; it != std::end(m_sixtyFourBitBlocks); ++it)
		{
			AllocInfo64& info = *it;
			info.startingAddress = startingAddress;

			startingAddress += info.size;
		}
	}
}

void Buddy::InitInitialAvailableBlocks(size_t startingAddress, size_t totalSize) noexcept
{
	MakeInitialBlocks(totalSize);
	AssignInitialBlockAddresses(startingAddress);
}

size_t Buddy::Allocate(size_t/* size */, size_t/* alignment */)
{
	return 0u;
}

std::optional<size_t> Buddy::AllocateN(size_t/* size*/, size_t/* alignment */) noexcept
{
	return {};
}

std::optional<Buddy::AllocInfo64> Buddy::GetAllocInfo(size_t size, size_t alignment) noexcept
{
	auto FindAllocationBlock = [this]<std::integral T>(
		std::vector<Buddy::AllocInfo<T>>&blocks, size_t allocationSize, size_t allocationAlignment
		) noexcept -> std::optional<Buddy::AllocInfo64>
	{
		for (auto it = std::begin(blocks); it != std::end(blocks); ++it)
		{
			if (auto alloctedBlock = AllocateOnBlock<T>(it, allocationSize, allocationAlignment);
				alloctedBlock)
			{
				return *alloctedBlock;
			}
		}

		return {};
	};

	std::optional<Buddy::AllocInfo64> allocatedBlock =
		FindAllocationBlock(m_eightBitBlocks, size, alignment);

	if (!allocatedBlock)
		allocatedBlock = FindAllocationBlock(m_sixteenBitBlocks, size, alignment);

	if (!allocatedBlock)
		allocatedBlock = FindAllocationBlock(m_thirtyTwoBitBlocks, size, alignment);

	if (!allocatedBlock)
		allocatedBlock = FindAllocationBlock(m_sixtyFourBitBlocks, size, alignment);

	if (allocatedBlock)
		SortBlocksBySize();

	return allocatedBlock;
}

Buddy::AllocInfo64 Buddy::AllocateOnBlock(
	size_t blockStartingAddress, size_t blockSize, size_t allocationSize, size_t allocationAlignment,
	size_t alignedSize
) noexcept {
	const auto halfBlockSize = static_cast<size_t>(blockSize / 2u);

	const size_t firstBlockStartingAddress  = blockStartingAddress;
	const size_t secondBlockStartingAddress = blockStartingAddress + halfBlockSize;

	if (alignedSize <= halfBlockSize)
	{
		MakeNewAvailableBlock(secondBlockStartingAddress, halfBlockSize);

		return AllocateOnBlock(
			firstBlockStartingAddress, halfBlockSize, allocationSize, allocationAlignment, alignedSize
		);
	}
	else
	{
		const size_t alignedAddress = Align(blockStartingAddress, allocationAlignment);
		return AllocInfo64{ alignedAddress, allocationSize };
	}
}

void Buddy::SortBlocksBySize() noexcept
{
	auto SortBySize = []<std::integral T>(const AllocInfo<T>& info1, const AllocInfo<T>& info2)
		{
			return info1.size < info2.size;
		};

	std::ranges::sort(m_eightBitBlocks, SortBySize);
	std::ranges::sort(m_sixteenBitBlocks, SortBySize);
	std::ranges::sort(m_thirtyTwoBitBlocks, SortBySize);
	std::ranges::sort(m_sixtyFourBitBlocks, SortBySize);
}
