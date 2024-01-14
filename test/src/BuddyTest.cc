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

	template<std::integral T>
	using AllocInfo = Buddy::AllocInfo<T>;

	[[nodiscard]]
	inline const std::vector<AllocInfo<std::uint8_t>>& GetEightBitBlocks() const noexcept
	{ return m_buddy.m_eightBitBlocks; }
	[[nodiscard]]
	inline const std::vector<AllocInfo<std::uint16_t>>& GetSixteenBitBlocks() const noexcept
	{ return m_buddy.m_sixteenBitBlocks; }
	[[nodiscard]]
	inline const std::vector<AllocInfo<std::uint32_t>>& GetThirtyTwoBitBlocks() const noexcept
	{ return m_buddy.m_thirtyTwoBitBlocks; }
	[[nodiscard]]
	inline const std::vector<AllocInfo<std::uint64_t>>& GetSixtyFourBitBlocks() const noexcept
	{ return m_buddy.m_sixtyFourBitBlocks; }

private:
	Buddy m_buddy;
};

TEST(BuddyTest, BuddyInitTest)
{
	size_t startingAddress  = 0u;
	size_t totalSize        = 1_GB;
	size_t minimumBlockSize = 16_KB;

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
			const TestBuddy::AllocInfo<std::uint32_t>& allocInfo = thirtyTwoBitBlocks.front();
			EXPECT_EQ(allocInfo.startingAddress, 0u) << "Starting Address isn't 0.";
			EXPECT_EQ(allocInfo.size, 1_GB) << "Size isn't 1GB.";
		}
	}

	startingAddress  = 0u;
	totalSize        = 2_GB + 512_MB + 10_KB;
	minimumBlockSize = 8_KB;

	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		size_t newTestSize = totalSize - 2_KB;

		EXPECT_EQ(buddy.TotalSize(), newTestSize) << "TotalSize isn't 2GB 512MB and 8KB.";
		EXPECT_EQ(buddy.AvailableSize(), newTestSize) << "AvailableSize isn't 2GB 512MB and 8KB.";
		EXPECT_EQ(buddy.MinimumBlockSize(), 8_KB) << "MinimumBlockSize isn't 8_KB.";

		EXPECT_EQ(std::size(buddy.GetEightBitBlocks()), 0u) << "There shouldn't be any EightBitBlocks.";
		EXPECT_EQ(std::size(buddy.GetSixteenBitBlocks()), 1u) << "There should be 1 SixteenBitBlocks.";
		EXPECT_EQ(std::size(buddy.GetThirtyTwoBitBlocks()), 2u) << "There should be two ThirtyTwoBitBlock.";
		EXPECT_EQ(std::size(buddy.GetSixtyFourBitBlocks()), 0u) << "There shouldn't be any SixtyFourBitBlocks.";

		if (const auto& sixteenBitBlocks = buddy.GetSixteenBitBlocks(); !std::empty(sixteenBitBlocks))
		{
			size_t testStartingAddress = 0u;

			const TestBuddy::AllocInfo<std::uint16_t>& allocInfo = sixteenBitBlocks.front();
			EXPECT_EQ(allocInfo.startingAddress, testStartingAddress)
				<< "Starting Address isn't 0.";
			EXPECT_EQ(allocInfo.size, 8_KB) << "Size isn't 8KB.";
		}

		if (const auto& thirtyTwoBitBlocks = buddy.GetThirtyTwoBitBlocks(); !std::empty(thirtyTwoBitBlocks))
		{
			size_t testStartingAddress = 8_KB;

			const TestBuddy::AllocInfo<std::uint32_t>& allocInfo = thirtyTwoBitBlocks.front();
			EXPECT_EQ(allocInfo.startingAddress, testStartingAddress)
				<< "Starting Address isn't 8KB.";
			EXPECT_EQ(allocInfo.size, 512_MB) << "Size isn't 512MB.";

			testStartingAddress += 512_MB;

			const TestBuddy::AllocInfo<std::uint32_t>& allocInfo1 = thirtyTwoBitBlocks[1];
			EXPECT_EQ(allocInfo1.startingAddress, testStartingAddress)
				<< "Starting Address doesn't match.";
			EXPECT_EQ(allocInfo1.size, 2_GB) << "Size isn't 2GB.";
		}
	}
}
