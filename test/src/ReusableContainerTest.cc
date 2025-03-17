#include <gtest/gtest.h>

#include <ReusableVector.hpp>

TEST(ReusableContainerTest, VectorTest)
{
	ReusableVector<int> rVec{};

	const size_t testIndex = rVec.Add(55);

	int num1 = 66;

	const size_t testIndex1 = rVec.Add(num1);

	EXPECT_EQ(testIndex, 0) << "TestIndex isn't 0.";
	EXPECT_EQ(testIndex1, 1) << "TestIndex1 isn't 1.";

	std::vector<int> itemsToAdd{ 1, 2, 3, 4 };
	std::vector<int> itemsToAdd1{ 5, 6, 7, 8 };
	std::vector<int> itemsToAdd2{ 9, 10, 11, 12, 13 };

	std::vector<std::uint32_t> itemIndices0 = rVec.AddElementsU32(itemsToAdd);

	std::uint32_t indicesExpectedStart = 2u;

	for (size_t index = 0u; index < std::size(itemIndices0); ++index)
	{
		std::uint32_t returnedIndex = itemIndices0[index];

		EXPECT_EQ(returnedIndex, indicesExpectedStart) << "Returned Index 0 number"
			<< index << " isn't " << indicesExpectedStart;

		++indicesExpectedStart;
	}

	std::vector<std::uint32_t> itemIndices1 = rVec.AddElementsU32(std::move(itemsToAdd1));

	for (size_t index = 0u; index < std::size(itemIndices1); ++index)
	{
		std::uint32_t returnedIndex = itemIndices1[index];

		EXPECT_EQ(returnedIndex, indicesExpectedStart) << "Returned Index 1 number"
			<< index << " isn't " << indicesExpectedStart;

		++indicesExpectedStart;
	}

	for (std::uint32_t index : itemIndices0)
		rVec.RemoveElement(index);

	std::vector<std::uint32_t> itemIndices2 = rVec.AddElementsU32(itemsToAdd2);

	EXPECT_EQ(std::size(itemIndices2), 5u) << "RVec itemIndices 2's size isn't 5.";
	EXPECT_EQ(itemIndices2[0], 2u) << "RVec itemIndices 2's Number 0 isn't 2.";
	EXPECT_EQ(itemIndices2[1], 3u) << "RVec itemIndices 2's Number 1 isn't 3.";
	EXPECT_EQ(itemIndices2[2], 4u) << "RVec itemIndices 2's Number 2 isn't 4.";
	EXPECT_EQ(itemIndices2[3], 5u) << "RVec itemIndices 2's Number 3 isn't 5.";
	EXPECT_EQ(itemIndices2[4], 10u) << "RVec itemIndices 2's Number 4 isn't 10.";
}

TEST(ReusableContainerTest, DequeTest)
{
	ReusableDeque<int> rDeque{};

	const size_t testIndex = rDeque.Add(55);

	int num1 = 66;

	const size_t testIndex1 = rDeque.Add(num1);

	EXPECT_EQ(testIndex, 0) << "TestIndex isn't 0.";
	EXPECT_EQ(testIndex1, 1) << "TestIndex1 isn't 1.";

	std::vector<int> itemsToAdd{ 1, 2, 3, 4 };
	std::vector<int> itemsToAdd1{ 5, 6, 7, 8 };
	std::vector<int> itemsToAdd2{ 9, 10, 11, 12, 13 };

	std::vector<std::uint32_t> itemIndices0 = rDeque.AddElementsU32(itemsToAdd);

	std::uint32_t indicesExpectedStart = 2u;

	for (size_t index = 0u; index < std::size(itemIndices0); ++index)
	{
		std::uint32_t returnedIndex = itemIndices0[index];

		EXPECT_EQ(returnedIndex, indicesExpectedStart) << "Returned Index 0 number"
			<< index << " isn't " << indicesExpectedStart;

		++indicesExpectedStart;
	}

	std::vector<std::uint32_t> itemIndices1 = rDeque.AddElementsU32(std::move(itemsToAdd1));

	for (size_t index = 0u; index < std::size(itemIndices1); ++index)
	{
		std::uint32_t returnedIndex = itemIndices1[index];

		EXPECT_EQ(returnedIndex, indicesExpectedStart) << "Returned Index 1 number"
			<< index << " isn't " << indicesExpectedStart;

		++indicesExpectedStart;
	}

	for (std::uint32_t index : itemIndices0)
		rDeque.RemoveElement(index);

	std::vector<std::uint32_t> itemIndices2 = rDeque.AddElementsU32(itemsToAdd2);

	EXPECT_EQ(std::size(itemIndices2), 5u) << "RDeque itemIndices 2's size isn't 5.";
	EXPECT_EQ(itemIndices2[0], 2u) << "RDeque itemIndices 2's Number 0 isn't 2.";
	EXPECT_EQ(itemIndices2[1], 3u) << "RDeque itemIndices 2's Number 1 isn't 3.";
	EXPECT_EQ(itemIndices2[2], 4u) << "RDeque itemIndices 2's Number 2 isn't 4.";
	EXPECT_EQ(itemIndices2[3], 5u) << "RDeque itemIndices 2's Number 3 isn't 5.";
	EXPECT_EQ(itemIndices2[4], 10u) << "RDeque itemIndices 2's Number 4 isn't 10.";
}

TEST(ReusableContainerTest, VectorEraseInactiveElementsTest0)
{
	ReusableVector<int> rVec{};

	const size_t testIndex = rVec.Add(55);

	int num1 = 66;

	const size_t testIndex1 = rVec.Add(num1);

	EXPECT_EQ(testIndex, 0) << "TestIndex isn't 0.";
	EXPECT_EQ(testIndex1, 1) << "TestIndex1 isn't 1.";

	std::vector<int> itemsToAdd{ 1, 2, 3, 4 };
	std::vector<int> itemsToAdd1{ 5, 6, 7, 8 };

	std::vector<std::uint32_t> itemIndices0 = rVec.AddElementsU32(itemsToAdd);

	[[maybe_unused]] std::vector<std::uint32_t> itemIndices1 = rVec.AddElementsU32(std::move(itemsToAdd1));

	rVec.RemoveElement(testIndex1);

	for (std::uint32_t index : itemIndices0)
		rVec.RemoveElement(index);

	rVec.EraseInactiveElements();

	EXPECT_EQ(std::size(rVec), 5u) << "RVec size is not 5.";
	EXPECT_EQ(rVec[0], 55) << "RVec index 0 isn't 55.";
	EXPECT_EQ(rVec[1], 5) << "RVec index 1 isn't 5.";
	EXPECT_EQ(rVec[2], 6) << "RVec index 2 isn't 6.";
	EXPECT_EQ(rVec[3], 7) << "RVec index 3 isn't 7.";
	EXPECT_EQ(rVec[4], 8) << "RVec index 4 isn't 8.";
}

TEST(ReusableContainerTest, DequeEraseInactiveElementsTest0)
{
	ReusableDeque<int> rDeque{};

	const size_t testIndex = rDeque.Add(55);

	int num1 = 66;

	const size_t testIndex1 = rDeque.Add(num1);

	EXPECT_EQ(testIndex, 0) << "TestIndex isn't 0.";
	EXPECT_EQ(testIndex1, 1) << "TestIndex1 isn't 1.";

	std::vector<int> itemsToAdd{ 1, 2, 3, 4 };
	std::vector<int> itemsToAdd1{ 5, 6, 7, 8 };

	std::vector<std::uint32_t> itemIndices0 = rDeque.AddElementsU32(itemsToAdd);

	[[maybe_unused]] std::vector<std::uint32_t> itemIndices1 = rDeque.AddElementsU32(
		std::move(itemsToAdd1)
	);

	rDeque.RemoveElement(testIndex1);

	for (std::uint32_t index : itemIndices0)
		rDeque.RemoveElement(index);

	rDeque.EraseInactiveElements();

	EXPECT_EQ(std::size(rDeque), 5u) << "RDeque size is not 5.";
	EXPECT_EQ(rDeque[0], 55) << "RDeque index 0 isn't 55.";
	EXPECT_EQ(rDeque[1], 5) << "RDeque index 1 isn't 5.";
	EXPECT_EQ(rDeque[2], 6) << "RDeque index 2 isn't 6.";
	EXPECT_EQ(rDeque[3], 7) << "RDeque index 3 isn't 7.";
	EXPECT_EQ(rDeque[4], 8) << "RDeque index 4 isn't 8.";
}

TEST(ReusableContainerTest, VectorEraseInactiveElementsTest1)
{
	ReusableVector<int> rVec{};

	rVec.EraseInactiveElements();

	EXPECT_EQ(std::size(rVec), 0u) << "RVec size is not 0.";
}

TEST(ReusableContainerTest, DequeEraseInactiveElementsTest1)
{
	ReusableDeque<int> rDeque{};

	rDeque.EraseInactiveElements();

	EXPECT_EQ(std::size(rDeque), 0u) << "RDeque size is not 0.";
}

TEST(ReusableContainerTest, VectorEraseInactiveElementsTest2)
{
	ReusableVector<int> rVec{};

	const size_t testIndex = rVec.Add(55);

	rVec.RemoveElement(testIndex);

	rVec.EraseInactiveElements();

	EXPECT_EQ(std::size(rVec), 0u) << "RVec size is not 0.";
}

TEST(ReusableContainerTest, DequeEraseInactiveElementsTest2)
{
	ReusableDeque<int> rDeque{};

	const size_t testIndex = rDeque.Add(55);

	rDeque.RemoveElement(testIndex);

	rDeque.EraseInactiveElements();

	EXPECT_EQ(std::size(rDeque), 0u) << "RDeque size is not 0.";
}
