#include <gtest/gtest.h>

#include <Buddy.hpp>
#include <format>
#include <string>
#include <cstdint>

class TestBuddy
{
public:
	inline TestBuddy(size_t startingAddress, size_t totalSize, size_t minimumBlockSize)
		: m_buddy{ startingAddress, totalSize, minimumBlockSize } {}

	// Wrappers for the functions from Buddy.
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

	[[nodiscard]]
	std::optional<Buddy::AllocInfo64> GetAllocInfo(size_t size, size_t alignment) noexcept
	{ return m_buddy.GetAllocInfo(size, alignment); }

public:
	enum class BlocksType
	{
		EightBits,
		SixteenBits,
		ThirtyTwoBits,
		SixtyFourBits
	};

public:
	// Test functions.
	void SizeTest(
		size_t availableSize, size_t totalSize, size_t minimumBlockSize, std::uint_least32_t lineNumber
	) const;
	void BlocksCountTest(
		size_t eightBitsCount, size_t sixteenBitsCount, size_t thirtyTwoBitsCount,
		size_t sixtyFourBitsCount, std::uint_least32_t lineNumber
	) const;

	template<std::integral T>
	static void AllocInfoTest(
		const Buddy::AllocInfo<T>& allocInfo, size_t startingAddress, size_t blockSize,
		std::uint_least32_t lineNumber
	) {
		EXPECT_EQ(allocInfo.startingAddress, startingAddress)
			<< std::format("Starting Address isn't {} on the line {}.", startingAddress, lineNumber);
		EXPECT_EQ(allocInfo.size, blockSize)
			<< std::format("Size isn't {} on the line {}", blockSize, lineNumber);
	}

	template<std::integral T>
	static void SpecificBlockTest(
		const std::vector<Buddy::AllocInfo<T>>& blocks, size_t index, size_t startingAddress,
		size_t blockSize, std::uint_least32_t lineNumber
	) {
		EXPECT_LT(index, std::size(blocks))
			<< std::format("Index of the Blocks array doesn't exist on the line {}.", lineNumber);

		const Buddy::AllocInfo<T>& allocInfo = blocks.at(index);

		AllocInfoTest(allocInfo, startingAddress, blockSize, lineNumber);
	}
	void SpecificBlockTest(
		BlocksType type, size_t index, size_t startingAddress, size_t blockSize,
		std::uint_least32_t lineNumber
	) const;

private:
	Buddy m_buddy;
};

void TestBuddy::SizeTest(
	size_t availableSize, size_t totalSize, size_t minimumBlockSize,
	std::uint_least32_t lineNumber
) const {
	EXPECT_EQ(AvailableSize(), availableSize)
		<< std::format("AvailableSize isn't {} on the line {}.", availableSize, lineNumber);
	EXPECT_EQ(TotalSize(), totalSize)
		<< std::format("TotalSize isn't {} on the line {}.", totalSize, lineNumber);
	EXPECT_EQ(MinimumBlockSize(), minimumBlockSize)
		<< std::format("MinimumBlockSize isn't {} on the line {}.", minimumBlockSize, lineNumber);
}

void TestBuddy::BlocksCountTest(
	size_t eightBitsCount, size_t sixteenBitsCount, size_t thirtyTwoBitsCount,
	size_t sixtyFourBitsCount, std::uint_least32_t lineNumber
) const {
	EXPECT_EQ(std::size(GetEightBitBlocks()), eightBitsCount)
		<< std::format(
			"There should be {} EightBitsBlocks on the line {}.", eightBitsCount, lineNumber
		);
	EXPECT_EQ(std::size(GetSixteenBitBlocks()), sixteenBitsCount)
		<< std::format(
			"There should be {} SixteenBitsBlocks on the line {}.", sixteenBitsCount, lineNumber
		);
	EXPECT_EQ(std::size(GetThirtyTwoBitBlocks()), thirtyTwoBitsCount)
		<< std::format(
			"There should be {} ThirtyTwoBitsBlocks on the line {}.", thirtyTwoBitsCount, lineNumber
		);
	EXPECT_EQ(std::size(GetSixtyFourBitBlocks()), sixtyFourBitsCount)
		<< std::format(
			"There should be {} SixtyFourBitsBlocks on the line {}.", sixtyFourBitsCount, lineNumber
		);
}

void TestBuddy::SpecificBlockTest(
	BlocksType type, size_t index, size_t startingAddress, size_t blockSize,
	std::uint_least32_t lineNumber
) const {
	if (type == BlocksType::EightBits)
		SpecificBlockTest(GetEightBitBlocks(), index, startingAddress, blockSize, lineNumber);
	else if (type == BlocksType::SixteenBits)
		SpecificBlockTest(GetSixteenBitBlocks(), index, startingAddress, blockSize, lineNumber);
	else if (type == BlocksType::ThirtyTwoBits)
		SpecificBlockTest(GetThirtyTwoBitBlocks(), index, startingAddress, blockSize, lineNumber);
	else if (type == BlocksType::SixtyFourBits)
		SpecificBlockTest(GetSixtyFourBitBlocks(), index, startingAddress, blockSize, lineNumber);
}

TEST(BuddyTest, BuddyInitTest)
{
	size_t startingAddress  = 0u;
	size_t totalSize        = 1_GB;
	size_t minimumBlockSize = 16_KB;

	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		using enum TestBuddy::BlocksType;

		buddy.SizeTest(1_GB, 1_GB, minimumBlockSize, __LINE__);
		buddy.BlocksCountTest(0u, 0u, 1u, 0u, __LINE__);
		buddy.SpecificBlockTest(ThirtyTwoBits, 0u, 0u, 1_GB, __LINE__);
	}

	startingAddress  = 0u;
	totalSize        = 2_GB + 512_MB + 10_KB;
	minimumBlockSize = 8_KB;

	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		size_t newTestSize = totalSize - 2_KB;
		using enum TestBuddy::BlocksType;

		buddy.SizeTest(newTestSize, newTestSize, minimumBlockSize, __LINE__);
		buddy.BlocksCountTest(0u, 1u, 2u, 0u, __LINE__);


		size_t testStartingAddress = 0u;
		size_t testBlockSize       = 8_KB;

		buddy.SpecificBlockTest(SixteenBits, 0u, testStartingAddress, testBlockSize, __LINE__);

		testStartingAddress += testBlockSize;
		testBlockSize        = 512_MB;

		buddy.SpecificBlockTest(ThirtyTwoBits, 0u, testStartingAddress, testBlockSize, __LINE__);

		testStartingAddress += testBlockSize;
		testBlockSize        = 2_GB;

		buddy.SpecificBlockTest(ThirtyTwoBits, 1u, testStartingAddress, testBlockSize, __LINE__);
	}
}

TEST(BuddyTest, BuddyAllocationTest)
{
	constexpr size_t startingAddress  = 0u;
	constexpr size_t totalSize        = 1_GB;
	constexpr size_t minimumBlockSize = 16_KB;

	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		buddy.SizeTest(1_GB, 1_GB, minimumBlockSize, __LINE__);
		buddy.BlocksCountTest(0u, 0u, 1u, 0u, __LINE__);

		size_t testAllocationSize = 16_KB;

		{
			auto allocInfo = buddy.GetAllocInfo(testAllocationSize, 256_B);

			const bool allocationSuccess = allocInfo != std::nullopt;
			EXPECT_EQ(allocationSuccess, true) << "Failed to Allocate.";

			if (allocInfo)
			{
				buddy.AllocInfoTest(*allocInfo, 0u, testAllocationSize, __LINE__);

				buddy.BlocksCountTest(0u, 2u, 14u, 0u, __LINE__);
			}
		}
		{
			testAllocationSize = 256_KB;
			auto allocInfo = buddy.GetAllocInfo(testAllocationSize, 256_B);

			const bool allocationSuccess = allocInfo != std::nullopt;
			EXPECT_EQ(allocationSuccess, true) << "Failed to Allocate.";

			if (allocInfo)
			{
				buddy.AllocInfoTest(*allocInfo, 256_KB, testAllocationSize, __LINE__);

				buddy.BlocksCountTest(0u, 2u, 13u, 0u, __LINE__);
			}
		}
	}
}
