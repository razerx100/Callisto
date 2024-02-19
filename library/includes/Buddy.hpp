#ifndef BUDDY_HPP_
#define BUDDY_HPP_
#include <AllocatorBase.hpp>
#include <ranges>
#include <algorithm>

class Buddy : public AllocatorBase
{
	friend class TestBuddy;
public:

	Buddy(size_t startingAddress, size_t totalSize, size_t minimumBlockSize);
	Buddy(
		size_t startingAddress, size_t totalSize, size_t defaultAlignment, size_t minimumBlockSize
	);

    [[nodiscard]]
	// Returns an aligned offset where the requested amount of size can be allocated or throws an
	// exception.
    size_t Allocate(size_t size, size_t alignment) override;
    [[nodiscard]]
	// Returns either an aligned offset where the requested amount of size can be allocated or an empty
	// optional.
    std::optional<size_t> AllocateN(size_t size, size_t alignment) noexcept override;

	void Deallocate(size_t startingAddress, size_t size, size_t alignment) noexcept override;

	// Apparently the functions without the alignment argument get hidden for some reason when you
	// override. So need to define these again here.
	using AllocatorBase::Deallocate;
	using AllocatorBase::Allocate;
	using AllocatorBase::AllocateN;

private:
	void InitInitialAvailableBlocks(size_t startingAddress, size_t totalSize) noexcept;

	void MakeInitialBlocks(size_t blockSize) noexcept;
	void AssignInitialBlockAddresses(size_t startingAddress) noexcept;

	void MakeNewAvailableBlock(size_t startingAddress, size_t size) noexcept;
	void MakeNewAvailableBlock(size_t size) noexcept;

	void MergeBuddies(const AllocInfo64& buddy);

	[[nodiscard]]
	std::optional<AllocInfo64> GetAllocInfo(size_t size, size_t alignment) noexcept;
	[[nodiscard]]
	AllocInfo64 AllocateOnBlock(
		size_t blockStartingAddress, size_t blockSize, size_t allocationSize, size_t allocationAlignment,
		size_t alignedSize
	) noexcept;
	[[nodiscard]]
	AllocInfo64 GetOriginalBlockInfo(
		size_t allocationStartingAddress, size_t allocationSize, size_t allocationAlignment
	) const noexcept;

	[[nodiscard]]
	static size_t GetBuddyAddress(size_t buddyAddress, size_t blockSize) noexcept;

private:
	size_t                   m_startingAddress;
	size_t                   m_minimumBlockSize;
	std::vector<AllocInfo64> m_sixtyFourBitBlocks;
	std::vector<AllocInfo32> m_thirtyTwoBitBlocks;
	std::vector<AllocInfo16> m_sixteenBitBlocks;
	std::vector<AllocInfo8>  m_eightBitBlocks;

public:
	Buddy(const Buddy&) = delete;
	Buddy& operator=(const Buddy&) = delete;

	inline Buddy(Buddy&& other) noexcept
		: AllocatorBase{ std::move(other) },
		m_startingAddress{ other.m_startingAddress },
		m_minimumBlockSize{other.m_minimumBlockSize},
		m_sixtyFourBitBlocks{ std::move(other.m_sixtyFourBitBlocks) },
		m_thirtyTwoBitBlocks{ std::move(other.m_thirtyTwoBitBlocks) },
		m_sixteenBitBlocks{ std::move(other.m_sixteenBitBlocks) },
		m_eightBitBlocks{ std::move(other.m_eightBitBlocks) } {}

	inline Buddy& operator=(Buddy&& other) noexcept
	{
		AllocatorBase::operator=(std::move(other));
		m_startingAddress    = other.m_startingAddress;
		m_minimumBlockSize   = other.m_minimumBlockSize;
		m_sixtyFourBitBlocks = std::move(other.m_sixtyFourBitBlocks);
		m_thirtyTwoBitBlocks = std::move(other.m_thirtyTwoBitBlocks);
		m_sixteenBitBlocks   = std::move(other.m_sixteenBitBlocks);
		m_eightBitBlocks     = std::move(other.m_eightBitBlocks);

		return *this;
	}

private:
	template<std::integral T>
	void RemoveIterator(std::vector<Buddy::AllocInfo<T>>::iterator infoIt) noexcept
	{
		if constexpr (std::is_same_v<T, std::uint8_t>)
			m_eightBitBlocks.erase(infoIt);
		else if constexpr (std::is_same_v<T, std::uint16_t>)
			m_sixteenBitBlocks.erase(infoIt);
		else if constexpr (std::is_same_v<T, std::uint32_t>)
			m_thirtyTwoBitBlocks.erase(infoIt);
		else if constexpr (std::is_same_v<T, std::uint64_t>)
			m_sixtyFourBitBlocks.erase(infoIt);
	}

	template<std::integral T>
	static void AddAllocBlock(
		std::vector<AllocatorBase::AllocInfo<T>>& blocks, size_t startingAddress, size_t size
	) noexcept {
		// The blocks container should be sorted by size. So, let's use std::lower_bound to find
		// where to add this new block. So, the container stays sorted.
		auto result = std::ranges::lower_bound(blocks, size, {},
			[](const AllocatorBase::AllocInfo<T>& element)
			{
				return element.size;
			}
		);

		// If no larger element exists, add the new block at the end. Otherwise,
		// the result would be either equal to larger than the size.
		// So, we should insert the new element before it.
		if (result == std::end(blocks))
			blocks.emplace_back(MakeAllocInfo<T>(startingAddress, size));
		else
			blocks.insert(result, MakeAllocInfo<T>(startingAddress, size));
	}

	template<std::integral T>
	std::optional<Buddy::AllocInfo64> FindAllocationBlock(
		std::vector<Buddy::AllocInfo<T>>& blocks, size_t allocationSize, size_t allocationAlignment
	) noexcept {
		auto result = std::lower_bound(
			std::begin(blocks), std::end(blocks), allocationSize,
			[allocationAlignment, startingAddress = m_startingAddress]
			(const AllocatorBase::AllocInfo<T>& info, size_t allocationSize)
			{
				const size_t blockSize             = info.size;
				const size_t startingAddressOffset = info.startingAddress;
				const size_t actualStartingAddress = startingAddressOffset + startingAddress;
				const size_t alignedSize           = GetAlignedSize(
					actualStartingAddress, allocationAlignment, allocationSize
				);

				return blockSize < alignedSize;
			}
		);

		// If a result is found, it should the required size or more. So, we can allocate in it.
		if (result != std::end(blocks))
		{
			const AllocatorBase::AllocInfo<T>& info = *result;
			const size_t blockSize                  = info.size;
			const size_t startingAddressOffset      = info.startingAddress;
			const size_t actualStartingAddress      = startingAddressOffset + m_startingAddress;
			const size_t alignedSize                = GetAlignedSize(
				actualStartingAddress, allocationAlignment, allocationSize
			);

			RemoveIterator<T>(result);
			return AllocateOnBlock(
				startingAddressOffset, blockSize, allocationSize, allocationAlignment, alignedSize
			);
		}
		else
			return {};
	}
};
#endif
