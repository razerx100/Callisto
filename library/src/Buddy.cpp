#include <Buddy.hpp>

Buddy::Buddy(size_t startingAddress, size_t totalSize, size_t minimumBlockSize/* = 256_B */)
	: AllocatorBase{ totalSize }, m_minimumBlockSize{ minimumBlockSize }, m_eightBitBlocks{},
	m_sixteenBitBlocks{}, m_thirtyTwoBitBlocks{}, m_sixtyFourBitBlocks{}
{
	// The total size might not be a 2s exponent. In that case, make a block with the largest 2s
	// exponent. Do the same on the leftover memory until all of the memory is divided into 2s
	// exponents.
	InitInitialAvailableBlocks(startingAddress, totalSize);
}

std::vector<AllocatorBase::AllocInfo>& Buddy::GetSuitableBlocks(
	size_t startingAddress, size_t totalSize
) noexcept
{
	if (BitsNeededFor(startingAddress) <= 8u && BitsNeededFor(totalSize) <= 8u)
		return m_eightBitBlocks;
	else if (BitsNeededFor(startingAddress) <= 16u && BitsNeededFor(totalSize) <= 16u)
		return m_sixteenBitBlocks;
	else if (BitsNeededFor(startingAddress) <= 32u && BitsNeededFor(totalSize) <= 32u)
		return m_thirtyTwoBitBlocks;
	else
		return m_sixtyFourBitBlocks;
}

void Buddy::MakeNewAvailableBlock(size_t startingAddress, size_t totalSize) noexcept
{
	GetSuitableBlocks(startingAddress, m_totalSize).emplace_back(
		AllocInfo{ .startingAddress = startingAddress, .size = totalSize }
	);
}

void Buddy::InitInitialAvailableBlocks(size_t startingAddress, size_t totalSize) noexcept
{
	const size_t lowerBound2sExponent = GetLowerBound2sExponent(totalSize);
	const size_t leftMemory	          = totalSize - lowerBound2sExponent;

	MakeNewAvailableBlock(startingAddress, lowerBound2sExponent);

	if (leftMemory)
	{
		const size_t newStartingAddress = startingAddress + lowerBound2sExponent;

		if (leftMemory >= m_minimumBlockSize)
			InitInitialAvailableBlocks(newStartingAddress, leftMemory);
		else
		{
			m_totalSize    -= leftMemory;
			m_availableSize = m_totalSize;
		}
	}
}

size_t Buddy::Allocate(size_t/* size */, size_t/* alignment */)
{
	return 0u;
}

std::optional<size_t> Buddy::AllocateN(size_t/* size*/, size_t/* alignment */) noexcept
{
	return {};
}
