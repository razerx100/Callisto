#include <gtest/gtest.h>

#include <AllocatorSTL.hpp>

TEST(CallistoTest, AllocatorSTLTest)
{
    constexpr size_t memorySize = 256u;
    std::uint8_t memory[memorySize];
    Allocator allocator{ memory, std::size(memory) };

    EXPECT_EQ(allocator.GetAvailableSize(), memorySize) << "Available Size isn't 256bytes";
    EXPECT_EQ(allocator.GetMemorySize(), memorySize) << "Total size isn't 256bytes";

    {
        AllocatorSTL<int> alloc{ allocator };
        std::uint8_t memory[256u];

        std::vector<int, AllocatorSTL<int>> vec{ alloc };
        vec.reserve(32u);

        EXPECT_EQ(allocator.GetAvailableSize(), 112u) << "Available Size isn't 112bytes";
    }

    EXPECT_EQ(allocator.GetAvailableSize(), memorySize) << "Available Size isn't 256bytes";
}
