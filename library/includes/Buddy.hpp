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
};
#endif
