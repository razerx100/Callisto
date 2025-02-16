#include <gtest/gtest.h>

#include <ReusableVector.hpp>

TEST(ReusableVectorTest, VectorTest)
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

	EXPECT_EQ(std::size(itemIndices2), 5u) << "ItemIndices 2's size isn't 5.";
	EXPECT_EQ(itemIndices2[0], 2u) << "ItemIndices 2's Number 0 isn't 2.";
	EXPECT_EQ(itemIndices2[1], 3u) << "ItemIndices 2's Number 1 isn't 3.";
	EXPECT_EQ(itemIndices2[2], 4u) << "ItemIndices 2's Number 2 isn't 4.";
	EXPECT_EQ(itemIndices2[3], 5u) << "ItemIndices 2's Number 3 isn't 5.";
	EXPECT_EQ(itemIndices2[4], 10u) << "ItemIndices 2's Number 4 isn't 10.";
}
