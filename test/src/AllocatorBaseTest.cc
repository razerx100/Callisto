#include <gtest/gtest.h>

#include <limits>
#include <AllocatorBase.hpp>

class TestAllocatorBase
{
public:
	[[nodiscard]]
    static size_t BitsNeededFor(size_t value) noexcept
    {
        return AllocatorBase::BitsNeededFor(value);
    }

    static size_t Align(size_t address, size_t alignment) noexcept
    {
        return AllocatorBase::Align(address, alignment);
    }

    static size_t GetUpperBound2sExponent(size_t size) noexcept
    {
        return AllocatorBase::GetUpperBound2sExponent(size);
    }

    static size_t GetLowerBound2sExponent(size_t size) noexcept
    {
        return AllocatorBase::GetLowerBound2sExponent(size);
    }

    static size_t GetAlignedSize(size_t startingAddress, size_t alignment, size_t size) noexcept
    {
        return AllocatorBase::GetAlignedSize(startingAddress, alignment, size);
    }
};

TEST(AllocatorBaseTest, BitsNeededForTest)
{
    EXPECT_EQ(TestAllocatorBase::BitsNeededFor(0x8000000000000000lu), 64) << "Nice.";
    EXPECT_EQ(TestAllocatorBase::BitsNeededFor(7u), 3) << "Not 3.";
    EXPECT_EQ(TestAllocatorBase::BitsNeededFor(std::numeric_limits<std::uint64_t>::max()), 64u) << "Not 64.";
    EXPECT_EQ(TestAllocatorBase::BitsNeededFor(std::numeric_limits<std::uint32_t>::max()), 32u) << "Not 32.";
    EXPECT_EQ(TestAllocatorBase::BitsNeededFor(std::numeric_limits<std::uint16_t>::max()), 16u) << "Not 16.";
    EXPECT_EQ(TestAllocatorBase::BitsNeededFor(std::numeric_limits<std::uint8_t>::max()), 8u) << "Not 8.";
}

TEST(AllocatorBaseTest, AlignTest)
{
    EXPECT_EQ(TestAllocatorBase::Align(18u, 16u), 32u) << "Aligned Address isn't 32.";
    EXPECT_EQ(TestAllocatorBase::Align(0u, 16u), 0u) << "Aligned Address isn't 0.";
    EXPECT_EQ(TestAllocatorBase::Align(64u, 16u), 64u) << "Aligned Address isn't 64.";
}

TEST(AllocatorBaseTest, UpperBound2sComponentTest)
{
    EXPECT_EQ(TestAllocatorBase::GetUpperBound2sExponent(48u), 64u) << "Upper2sExponent isn't 64.";
    EXPECT_EQ(TestAllocatorBase::GetUpperBound2sExponent(32u), 32u) << "Upper2sExponent isn't 32.";
    EXPECT_EQ(TestAllocatorBase::GetUpperBound2sExponent(2u), 2u) << "Upper2sExponent isn't 2.";
}

TEST(AllocatorBaseTest, LowerBound2sComponentTest)
{
    EXPECT_EQ(TestAllocatorBase::GetLowerBound2sExponent(48u), 32u) << "Upper2sExponent isn't 32.";
    EXPECT_EQ(TestAllocatorBase::GetLowerBound2sExponent(64u), 64u) << "Upper2sExponent isn't 64.";
    EXPECT_EQ(TestAllocatorBase::GetLowerBound2sExponent(2u), 2u) << "Upper2sExponent isn't 2.";
}

TEST(AllocatorBaseTest, AlignedSizeTest)
{
    EXPECT_EQ(TestAllocatorBase::GetAlignedSize(18u, 16u, 64u), 78u) << "AlignedSize isn't 78.";
    EXPECT_EQ(TestAllocatorBase::GetAlignedSize(16u, 16u, 64u), 64u) << "AlignedSize isn't 64.";
}
