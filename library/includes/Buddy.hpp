#ifndef BUDDY_HPP_
#define BUDDY_HPP_
#include <AllocatorBase.hpp>

class Buddy : public AllocatorBase
{
	friend class TestBuddy;
public:
	Buddy(size_t startingAddress, size_t totalSize, size_t minimumBlockSize = 256_B);

    [[nodiscard]]
    size_t Allocate(size_t size, size_t alignment) override;
    [[nodiscard]]
    std::optional<size_t> AllocateN(size_t size, size_t alignment) noexcept override;

private:
	void InitInitialAvailableBlocks(size_t startingAddress, size_t totalSize) noexcept;

	void MakeInitialBlocks(size_t blockSize) noexcept;
	void AssignInitialBlockAddresses(size_t startingAddress) noexcept;

	void MakeNewAvailableBlock(size_t startingAddress, size_t size) noexcept;
	void MakeNewAvailableBlock(size_t size) noexcept;

	[[nodiscard]]
	std::optional<AllocInfo64> GetAllocInfo(size_t size, size_t alignment) noexcept;
	[[nodiscard]]
	AllocInfo64 AllocateOnBlock(
		size_t blockStartingAddress, size_t blockSize, size_t allocationSize, size_t allocationAlignment,
		size_t alignedSize
	) noexcept;

private:
	size_t                   m_minimumBlockSize;
	std::vector<AllocInfo64> m_sixtyFourBitBlocks;
	std::vector<AllocInfo32> m_thirtyTwoBitBlocks;
	std::vector<AllocInfo16> m_sixteenBitBlocks;
	std::vector<AllocInfo8>  m_eightBitBlocks;

public:
	Buddy(const Buddy&) = delete;
	Buddy& operator=(const Buddy&) = delete;

	inline Buddy(Buddy&& other) noexcept
		: AllocatorBase{ std::move(other) },
		m_minimumBlockSize{other.m_minimumBlockSize},
		m_sixtyFourBitBlocks{ std::move(other.m_sixtyFourBitBlocks) },
		m_thirtyTwoBitBlocks{ std::move(other.m_thirtyTwoBitBlocks) },
		m_sixteenBitBlocks{ std::move(other.m_sixteenBitBlocks) },
		m_eightBitBlocks{ std::move(other.m_eightBitBlocks) } {}

	inline Buddy& operator=(Buddy&& other) noexcept
	{
		m_totalSize          = other.m_totalSize;
		m_availableSize      = other.m_availableSize;
		m_minimumBlockSize   = other.m_minimumBlockSize;
		m_sixtyFourBitBlocks = std::move(other.m_sixtyFourBitBlocks);
		m_thirtyTwoBitBlocks = std::move(other.m_thirtyTwoBitBlocks);
		m_sixteenBitBlocks   = std::move(other.m_sixteenBitBlocks);
		m_eightBitBlocks     = std::move(other.m_eightBitBlocks);

		return *this;
	}

private:
	template<std::integral T>
	[[nodiscard]]
	std::optional<AllocInfo64> AllocateOnBlock(
		const AllocInfo<T>& info, size_t size, size_t alignment
	) noexcept {
		const size_t blockSize       = info.size;
		const size_t startingAddress = info.startingAddress;

		const size_t alignedSize = GetAlignedSize(startingAddress, alignment, size);

		if (alignedSize <= blockSize)
			return AllocateOnBlock(startingAddress, blockSize, size, alignment, alignedSize);
		else
			return {};
	}
};
#endif
