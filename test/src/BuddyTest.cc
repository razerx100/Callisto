#include <gtest/gtest.h>

#include <Buddy.hpp>
#include <format>
#include <string>
#include <cstdint>

class TestBuddy
{
public:
	TestBuddy(size_t startingAddress, size_t totalSize, size_t minimumBlockSize)
		: m_buddy{ startingAddress, totalSize, minimumBlockSize }
	{}

	// Wrappers for the functions from Buddy.
	[[nodiscard]]
	size_t TotalSize() const noexcept { return m_buddy.TotalSize(); }
	[[nodiscard]]
	size_t AvailableSize() const noexcept { return m_buddy.AvailableSize(); }
	[[nodiscard]]
	size_t MinimumBlockSize() const noexcept { return m_buddy.m_minimumBlockSize; }

	template<std::integral T>
	using AllocInfo = Buddy::AllocInfo<T>;

	[[nodiscard]]
	const std::vector<AllocInfo<std::uint8_t>>& GetEightBitBlocks() const noexcept
	{
		return m_buddy.m_eightBitBlocks;
	}
	[[nodiscard]]
	const std::vector<AllocInfo<std::uint16_t>>& GetSixteenBitBlocks() const noexcept
	{
		return m_buddy.m_sixteenBitBlocks;
	}
	[[nodiscard]]
	const std::vector<AllocInfo<std::uint32_t>>& GetThirtyTwoBitBlocks() const noexcept
	{
		return m_buddy.m_thirtyTwoBitBlocks;
	}
	[[nodiscard]]
	const std::vector<AllocInfo<std::uint64_t>>& GetSixtyFourBitBlocks() const noexcept
	{
		return m_buddy.m_sixtyFourBitBlocks;
	}
	[[nodiscard]]
	Buddy::AllocInfo64 GetOriginalBlockInfo(
		size_t allocationStartingAddress, size_t allocationSize, size_t allocationAlignment
	) const noexcept {
		return m_buddy.GetOriginalBlockInfo(
			allocationStartingAddress, allocationSize, allocationAlignment
		);
	}

	[[nodiscard]]
	std::optional<Buddy::AllocInfo64> GetAllocInfo(size_t size, size_t alignment) noexcept
	{
		return m_buddy.GetAllocInfo(size, alignment);
	}

    [[nodiscard]]
    std::optional<size_t> AllocateN(size_t size, size_t alignment) noexcept
	{
		return m_buddy.AllocateN(size, alignment);
	}

	void Deallocate(size_t startingAddress, size_t size, size_t alignment) noexcept
	{
		m_buddy.Deallocate(startingAddress, size, alignment);
	}

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
	void AllocationTest(
		size_t allocationSize, size_t allocationAlignment, size_t expectedStartingAddress,
		size_t eightBitsCount, size_t sixteenBitsCount, size_t thirtyTwoBitsCount,
		size_t sixtyFourBitsCount, std::uint_least32_t lineNumber
	);

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

void TestBuddy::AllocationTest(
	size_t allocationSize, size_t allocationAlignment, size_t expectedStartingAddress,
	size_t eightBitsCount, size_t sixteenBitsCount, size_t thirtyTwoBitsCount,
	size_t sixtyFourBitsCount, std::uint_least32_t lineNumber
) {
	auto allocInfo = GetAllocInfo(allocationSize, allocationAlignment);

	const bool allocationSuccess = allocInfo != std::nullopt;
	EXPECT_EQ(allocationSuccess, true)
		<< std::format("Failed to Allocate on the line {}.", lineNumber);

	if (allocInfo)
	{
		AllocInfoTest(*allocInfo, expectedStartingAddress, allocationSize, lineNumber);

		BlocksCountTest(
			eightBitsCount, sixteenBitsCount, thirtyTwoBitsCount, sixtyFourBitsCount, lineNumber
		);
	}
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
	size_t startingAddress            = 0u;
	constexpr size_t totalSize        = 1_GB;
	constexpr size_t minimumBlockSize = 16_KB;

	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		buddy.SizeTest(1_GB, 1_GB, minimumBlockSize, __LINE__);
		buddy.BlocksCountTest(0u, 0u, 1u, 0u, __LINE__);

		size_t testAllocationSize = 16_KB;
		buddy.AllocationTest(testAllocationSize, 256_B, 0u, 0u, 2u, 14u, 0u, __LINE__);

		testAllocationSize = 256_KB;
		buddy.AllocationTest(testAllocationSize, 256_B, 256_KB, 0u, 2u, 13u, 0u, __LINE__);
	}

	startingAddress = 5_GB + 20u;
	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		buddy.SizeTest(1_GB, 1_GB, minimumBlockSize, __LINE__);
		buddy.BlocksCountTest(0u, 0u, 1u, 0u, __LINE__);

		size_t testAllocationSize      = 16_KB;
		constexpr size_t testAlignment = 256_B;
		size_t testStartingAddress     = 5_GB + testAlignment;
		buddy.AllocationTest(
			testAllocationSize, testAlignment, testStartingAddress, 0u, 1u, 14u, 0u, __LINE__
		);

		testAllocationSize   = 256_KB;
		testStartingAddress += 512_KB;
		buddy.AllocationTest(
			testAllocationSize, testAlignment, testStartingAddress, 0u, 1u, 13u, 0u, __LINE__
		);
	}

	{
		size_t requiredSize = Buddy::GetMinimumRequiredNewAllocationSizeFor(100_MB);

		TestBuddy buddy{ 0u, requiredSize, 16_KB };

		EXPECT_EQ(std::size(buddy.GetThirtyTwoBitBlocks()), 1u) << "More than one block upon creation.";
	}
}

TEST(BuddyTest, GetOriginalBlockTest)
{
	size_t startingAddress            = 5_GB + 24u;
	constexpr size_t totalSize        = 1_GB;
	constexpr size_t minimumBlockSize = 16_KB;

	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		constexpr size_t allocationSize      = 16_KB;
		constexpr size_t allocationAlignment = 256_B;

		auto allocInfo = buddy.GetAllocInfo(allocationSize, allocationAlignment);

		const bool allocationSuccess = allocInfo != std::nullopt;
		EXPECT_EQ(allocationSuccess, true) << "Failed to Allocate.";

		if (allocInfo)
		{
			const auto& blockInfo = *allocInfo;
			buddy.AllocInfoTest(blockInfo, 5_GB + 256_B, allocationSize, __LINE__);

			const auto originalBlock = buddy.GetOriginalBlockInfo(
				blockInfo.startingAddress, blockInfo.size, allocationAlignment
			);
			buddy.AllocInfoTest(originalBlock, 0u, 32_KB, __LINE__);
		}
	}

	startingAddress  = 256u;

	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		constexpr size_t allocationSize      = 16_KB;
		constexpr size_t allocationAlignment = 256_B;

		auto allocInfo = buddy.GetAllocInfo(allocationSize, allocationAlignment);

		const bool allocationSuccess = allocInfo != std::nullopt;
		EXPECT_EQ(allocationSuccess, true) << "Failed to Allocate.";

		if (allocInfo)
		{
			const auto& blockInfo = *allocInfo;
			buddy.AllocInfoTest(blockInfo, 256_B, allocationSize, __LINE__);

			const auto originalBlock = buddy.GetOriginalBlockInfo(
				blockInfo.startingAddress, blockInfo.size, allocationAlignment
			);
			buddy.AllocInfoTest(originalBlock, 0u, 16_KB, __LINE__);
		}
	}
}

TEST(BuddyTest, DeallocationTest)
{
	constexpr size_t startingAddress  = 5_GB + 24u;
	constexpr size_t totalSize        = 2_GB + 256_MB;
	constexpr size_t minimumBlockSize = 16_KB;

	{
		TestBuddy buddy{ startingAddress, totalSize, minimumBlockSize };

		size_t allocationSize                = 16_KB;
		size_t availableSize                 = totalSize;
		constexpr size_t allocationAlignment = 256_B;

		buddy.SizeTest(availableSize, totalSize, minimumBlockSize, __LINE__);
		buddy.BlocksCountTest(0u, 0u, 2u, 0u, __LINE__);

		auto startinAddressResult = buddy.AllocateN(allocationSize, allocationAlignment);

		availableSize -= 32_KB;

		buddy.SizeTest(availableSize, totalSize, minimumBlockSize, __LINE__);
		buddy.BlocksCountTest(0u, 1u, 13u, 0u, __LINE__);

		allocationSize = 256_MB;
		auto startinAddressResult1 = buddy.AllocateN(allocationSize, allocationAlignment);

		availableSize -= 512_MB;

		buddy.SizeTest(availableSize, totalSize, minimumBlockSize, __LINE__);
		buddy.BlocksCountTest(0u, 1u, 14u, 0u, __LINE__);

		if (startinAddressResult1)
		{
			buddy.Deallocate(*startinAddressResult1, 256_MB, allocationAlignment);

			availableSize += 512_MB;

			buddy.SizeTest(availableSize, totalSize, minimumBlockSize, __LINE__);
			buddy.BlocksCountTest(0u, 1u, 13u, 0u, __LINE__);
		}

		if (startinAddressResult)
		{
			buddy.Deallocate(*startinAddressResult, 16_KB, allocationAlignment);

			availableSize += 32_KB;

			buddy.SizeTest(availableSize, totalSize, minimumBlockSize, __LINE__);
			buddy.BlocksCountTest(0u, 0u, 2u, 0u, __LINE__);
		}
	}
}
