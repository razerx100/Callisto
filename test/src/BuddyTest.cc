#include <gtest/gtest.h>

#include <Buddy.hpp>

class TestBuddy
{
public:
	inline TestBuddy(size_t startingAddress, size_t totalSize, size_t minimumBlockSize)
		: m_buddy{ startingAddress, totalSize, minimumBlockSize } {}

	[[nodiscard]]
	inline size_t TotalSize() const noexcept { return m_buddy.TotalSize(); }
	[[nodiscard]]
	inline size_t AvailableSize() const noexcept { return m_buddy.AvailableSize(); }
	[[nodiscard]]
	inline size_t MinimumBlockSize() const noexcept { return m_buddy.m_minimumBlockSize; }

	using AllocInfo = Buddy::AllocInfo;

	[[nodiscard]]
	inline const std::vector<AllocInfo>& GetEightBitBlocks() const noexcept
	{ return m_buddy.m_eightBitBlocks; }
	[[nodiscard]]
	inline const std::vector<AllocInfo>& GetSixteenBitBlocks() const noexcept
	{ return m_buddy.m_sixteenBitBlocks; }
	[[nodiscard]]
	inline const std::vector<AllocInfo>& GetThirtyTwoBitBlocks() const noexcept
	{ return m_buddy.m_thirtyTwoBitBlocks; }
	[[nodiscard]]
	inline const std::vector<AllocInfo>& GetSixtyFourBitBlocks() const noexcept
	{ return m_buddy.m_sixtyFourBitBlocks; }

private:
	Buddy m_buddy;
};

TEST(BuddyTest, BuddyInitTest)
{
	constexpr size_t startingAddress  = 0u;
	constexpr size_t totalSize        = 1_GB;
	constexpr size_t minimumBlockSize = 16_KB;

	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		EXPECT_EQ(buddy.AvailableSize(), 1_GB) << "AvailableSize isn't 1GB.";
		EXPECT_EQ(buddy.TotalSize(), 1_GB) << "TotalSize isn't 1GB.";
		EXPECT_EQ(buddy.MinimumBlockSize(), 16_KB) << "MinimumBlockSize isn't 16_KB.";

		EXPECT_EQ(std::size(buddy.GetEightBitBlocks()), 0u) << "There shouldn't be any EightBitBlocks.";
		EXPECT_EQ(std::size(buddy.GetSixteenBitBlocks()), 0u) << "There shouldn't be any SixteenBitBlocks.";
		EXPECT_EQ(std::size(buddy.GetThirtyTwoBitBlocks()), 1u) << "There should be one ThirtyTwoBitBlock.";
		EXPECT_EQ(std::size(buddy.GetSixtyFourBitBlocks()), 0u) << "There shouldn't be any SixtyFourBitBlocks.";

		if (const auto& thirtyTwoBitBlocks = buddy.GetThirtyTwoBitBlocks(); !std::empty(thirtyTwoBitBlocks))
		{
			const TestBuddy::AllocInfo& allocInfo = thirtyTwoBitBlocks.front();
			EXPECT_EQ(allocInfo.startingAddress.value64, 0u) << "Starting Address isn't 0.";
			EXPECT_EQ(allocInfo.size.value64, 1_GB) << "Size isn't 1GB.";
		}
	}
}
