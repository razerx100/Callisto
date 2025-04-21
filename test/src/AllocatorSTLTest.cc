#include <gtest/gtest.h>

#include <AllocatorSTL.hpp>

TEST(AllocatorSTLTest, AllocationTest)
{
    constexpr size_t memorySize = 256u;
    std::uint8_t memory[memorySize];
    Callisto::Allocator allocator{ memory, std::size(memory), 256_B };

    EXPECT_EQ(allocator.GetAvailableSize(), memorySize) << "Available Size isn't 256bytes";
    EXPECT_EQ(allocator.GetMemorySize(), memorySize) << "Total size isn't 256bytes";

    {
        Callisto::AllocatorSTL<int> alloc{ allocator };

        std::vector<int, Callisto::AllocatorSTL<int>> vec{ alloc };
        vec.reserve(32u);

        EXPECT_EQ(allocator.GetAvailableSize(), 112u) << "Available Size isn't 112bytes";
    }

    EXPECT_EQ(allocator.GetAvailableSize(), memorySize) << "Available Size isn't 256bytes";
}
