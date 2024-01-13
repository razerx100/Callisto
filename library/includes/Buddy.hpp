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
	[[nodiscard]]
	std::vector<AllocInfo>& GetSuitableBlocks(size_t startingAddress, size_t totalSize) noexcept;

	void InitInitialAvailableBlocks(size_t startingAddress, size_t totalSize) noexcept;
	void MakeNewAvailableBlock(size_t startingAddress, size_t totalSize) noexcept;

private:
	size_t                 m_minimumBlockSize;
	std::vector<AllocInfo> m_eightBitBlocks;
	std::vector<AllocInfo> m_sixteenBitBlocks;
	std::vector<AllocInfo> m_thirtyTwoBitBlocks;
	std::vector<AllocInfo> m_sixtyFourBitBlocks;
};
#endif
