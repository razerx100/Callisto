#ifndef BUDDY_HPP_
#define BUDDY_HPP_
#include <AllocatorBase.hpp>

class Buddy : public AllocatorBase
{
	friend class TestBuddy;
public:
	Buddy(size_t startingAddress, size_t totalSize, size_t minimumBlockSize = 256_B);

    [[nodiscard]]
	// Returns an aligned offset where the requested amount of size can be allocated or throws an
	// exception.
    size_t Allocate(size_t size, size_t alignment) override;
    [[nodiscard]]
	// Returns either an aligned offset where the requested amount of size can be allocated or an empty
	// optional.
    std::optional<size_t> AllocateN(size_t size, size_t alignment) noexcept override;

private:
	void InitInitialAvailableBlocks(size_t startingAddress, size_t totalSize) noexcept;

	void MakeInitialBlocks(size_t blockSize) noexcept;
	void AssignInitialBlockAddresses(size_t startingAddress) noexcept;

	void MakeNewAvailableBlock(size_t startingAddress, size_t size) noexcept;
	void MakeNewAvailableBlock(size_t size) noexcept;

	void SortBlocksBySize() noexcept;

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
	void RemoveIterator(std::vector<Buddy::AllocInfo<T>>::iterator infoIt) noexcept
	{
		if constexpr (std::is_same_v<T, std::uint8_t>)
			m_eightBitBlocks.erase(infoIt);
		else if constexpr (std::is_same_v<T, std::uint16_t>)
			m_sixteenBitBlocks.erase(infoIt);
		else if constexpr (std::is_same_v<T, std::uint32_t>)
			m_thirtyTwoBitBlocks.erase(infoIt);
		else if constexpr (std::is_same_v<T, std::uint64_t>)
			m_sixtyFourBitBlocks.erase(infoIt);
	}

	template<std::integral T>
	[[nodiscard]]
	std::optional<Buddy::AllocInfo64> AllocateOnBlock(
		std::vector<Buddy::AllocInfo<T>>::iterator infoIt, size_t size, size_t alignment
	) noexcept {
		const Buddy::AllocInfo<T>& info = *infoIt;

		const size_t blockSize       = info.size;
		const size_t startingAddress = info.startingAddress;

		const size_t alignedSize     = GetAlignedSize(startingAddress, alignment, size);

		if (alignedSize <= blockSize)
		{
			RemoveIterator<T>(infoIt);
			return AllocateOnBlock(startingAddress, blockSize, size, alignment, alignedSize);
		}
		else
			return {};
	}
};
#endif
