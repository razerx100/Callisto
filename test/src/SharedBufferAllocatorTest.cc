#include <gtest/gtest.h>

#include <AllocationLiterals.hpp>
#include <SharedBufferAllocator.hpp>

class SharedBufferAllocatorTest
{
public:
	[[nodiscard]]
	static const std::vector<SharedBufferAllocator::AllocInfo>& GetAvailableMemory(
		const SharedBufferAllocator& allocator
	) noexcept {
		return allocator.m_availableMemory;
	}
};

SharedBufferAllocator::AllocInfo GetAllocation(
	SharedBufferAllocator& allocator, size_t& bufferSize,
	size_t requestedSize
) noexcept {
	auto availableAllocIndex = allocator.GetAvailableAllocInfo(requestedSize);
	SharedBufferAllocator::AllocInfo allocInfo{ .offset = 0u, .size = 0u };

	if (!availableAllocIndex)
	{
		allocInfo.size   = requestedSize;
		allocInfo.offset = bufferSize;

		bufferSize += requestedSize;
	}
	else
		allocInfo = allocator.GetAndRemoveAllocInfo(*availableAllocIndex);

	return SharedBufferAllocator::AllocInfo
	{
		.offset = allocator.AllocateMemory(allocInfo, requestedSize),
		.size   = requestedSize
	};
}

TEST(SharedBufferAllocatorTest, AllocationTest)
{
	SharedBufferAllocator allocator{};

	size_t bufferSize = 0u;

	const auto allocation  = GetAllocation(allocator, bufferSize, 5_KB);

	EXPECT_EQ(allocation.offset, 0u) << "Allocation 0 offset isn't 0.";
	EXPECT_EQ(allocation.size, 5_KB) << "Allocation 0 size isn't 5KB.";

	const auto allocation1 = GetAllocation(allocator, bufferSize, 15_KB);

	EXPECT_EQ(allocation1.offset, 5_KB) << "Allocation 1 offset isn't 5_KB.";
	EXPECT_EQ(allocation1.size, 15_KB) << "Allocation 1 size isn't 15KB.";

	const auto allocation2 = GetAllocation(allocator, bufferSize, 10_KB);

	EXPECT_EQ(allocation2.offset, 20_KB) << "Allocation 2 offset isn't 20KB.";
	EXPECT_EQ(allocation2.size, 10_KB) << "Allocation 2 size isn't 10KB.";
}

TEST(SharedBufferAllocatorTest, ReallocationTest)
{
	SharedBufferAllocator allocator{};

	size_t bufferSize = 0u;

	const auto allocation  = GetAllocation(allocator, bufferSize, 5_KB);

	EXPECT_EQ(allocation.offset, 0u) << "Allocation 0 offset isn't 0.";
	EXPECT_EQ(allocation.size, 5_KB) << "Allocation 0 size isn't 5KB.";

	const auto allocation1 = GetAllocation(allocator, bufferSize, 15_KB);

	EXPECT_EQ(allocation1.offset, 5_KB) << "Allocation 1 offset isn't 5_KB.";
	EXPECT_EQ(allocation1.size, 15_KB) << "Allocation 1 size isn't 15KB.";

	const auto allocation2 = GetAllocation(allocator, bufferSize, 10_KB);

	EXPECT_EQ(allocation2.offset, 20_KB) << "Allocation 2 offset isn't 20KB.";
	EXPECT_EQ(allocation2.size, 10_KB) << "Allocation 2 size isn't 10KB.";

	allocator.RelinquishMemory(allocation1.offset, allocation1.size);

	const auto allocation3 = GetAllocation(allocator, bufferSize, 10_KB);

	EXPECT_EQ(allocation3.offset, 5_KB) << "Allocation 3 offset isn't 5_KB.";
	EXPECT_EQ(allocation3.size, 10_KB) << "Allocation 3 size isn't 10KB.";

	const auto allocation4 = GetAllocation(allocator, bufferSize, 20_KB);

	EXPECT_EQ(allocation4.offset, 30_KB) << "Allocation 4 offset isn't 30_KB.";
	EXPECT_EQ(allocation4.size, 20_KB) << "Allocation 4 size isn't 20KB.";

	const auto allocation5 = GetAllocation(allocator, bufferSize, 10_KB);

	EXPECT_EQ(allocation5.offset, 50_KB) << "Allocation 5 offset isn't 50_KB.";
	EXPECT_EQ(allocation5.size, 10_KB) << "Allocation 5 size isn't 10KB.";

	const auto allocation6 = GetAllocation(allocator, bufferSize, 20_KB);

	EXPECT_EQ(allocation6.offset, 60_KB) << "Allocation 6 offset isn't 60_KB.";
	EXPECT_EQ(allocation6.size, 20_KB) << "Allocation 6 size isn't 20KB.";

	const auto& availableMemory = SharedBufferAllocatorTest::GetAvailableMemory(allocator);

	EXPECT_EQ(std::size(availableMemory), 1u) << "Available block count isn't 1.";
	EXPECT_EQ(availableMemory[0].offset, 15_KB) << "Available block 0 offset isn't 15KB.";
	EXPECT_EQ(availableMemory[0].size, 5_KB) << "Available block 0 size isn't 5KB.";

	allocator.RelinquishMemory(allocation6.offset, allocation6.size);
	allocator.RelinquishMemory(allocation4.offset, allocation4.size);
	allocator.RelinquishMemory(allocation2.offset, allocation2.size);

	EXPECT_EQ(std::size(availableMemory), 2u) << "Available block count isn't 2.";
	EXPECT_EQ(availableMemory[0].offset, 60_KB) << "Available block 0 offset isn't 60KB.";
	EXPECT_EQ(availableMemory[0].size, 20_KB) << "Available block 0 size isn't 20KB.";
	EXPECT_EQ(availableMemory[1].offset, 15_KB) << "Available block 1 offset isn't 15KB.";
	EXPECT_EQ(availableMemory[1].size, 35_KB) << "Available block 1 size isn't 35KB.";
}
