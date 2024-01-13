#ifndef ALLOCATOR_BASE_HPP_
#define ALLOCATOR_BASE_HPP_
#include <cstdint>
#include <array>
#include <vector>
#include <optional>

union CUint64
{
	std::uint64_t value64;
	struct
	{
		std::uint32_t first;
		std::uint32_t second;
	} value32;
	struct
	{
		std::uint16_t first;
		std::uint16_t second;
		std::uint16_t third;
		std::uint16_t fourth;
	} value16;
	struct
	{
		std::uint8_t first;
		std::uint8_t second;
		std::uint8_t third;
		std::uint8_t fourth;
		std::uint8_t sixth;
		std::uint8_t seventh;
		std::uint8_t eighth;
		std::uint8_t nineth;
	} value8;
};

constexpr size_t operator"" _B(unsigned long long number) noexcept {
	return static_cast<size_t>(number);
}

constexpr size_t operator"" _KB(unsigned long long number) noexcept {
	return static_cast<size_t>(number * 1024u);
}

constexpr size_t operator"" _MB(unsigned long long number) noexcept {
	return static_cast<size_t>(number * 1024u * 1024u);
}

constexpr size_t operator"" _GB(unsigned long long number) noexcept {
	return static_cast<size_t>(number * 1024u * 1024u * 1024u);
}

class AllocatorBase
{
	friend class TestAllocatorBase;
public:
	inline AllocatorBase(size_t totalSize) : m_totalSize{ totalSize }, m_availableSize{ totalSize } {}
	virtual ~AllocatorBase() = default;

	[[nodiscard]]
	inline size_t TotalSize() const noexcept { return m_totalSize; }
	[[nodiscard]]
	inline size_t AvailableSize() const noexcept { return m_availableSize; }

    [[nodiscard]]
    virtual size_t Allocate(size_t size, size_t alignment) = 0;
    [[nodiscard]]
    virtual std::optional<size_t> AllocateN(size_t size, size_t alignment) noexcept = 0;

protected:
	struct AllocInfo
	{
		CUint64 startingAddress;
		CUint64 size;
	};

protected:
	[[nodiscard]]
	static size_t BitsNeededFor(size_t value) noexcept;

	// Aligns the address to the alignment.
	// Ex: fn(address 18, alignment 16) = 32.
	[[nodiscard]]
	inline static size_t Align(size_t address, size_t alignment) noexcept
	{
		return (address + (alignment - 1u)) & ~(alignment - 1u);
	}

	// Gets the upperBound 2s Exponent.
	// Ex: fn(48) = 64.
	[[nodiscard]]
	inline static size_t GetUpperBound2sExponent(size_t size) noexcept
	{
		size_t result = 1u;
		for (; result < size; result <<= 1u);

		return result;
	}

	// Gets the lowerBound 2s Exponent.
	// Ex: fn(48) = 32.
	[[nodiscard]]
	inline static size_t GetLowerBound2sExponent(size_t size) noexcept
	{
		size_t result = 0x8000000000000000lu;
		for (; result > size; result >>= 1u);

		return result;
	}

	// Gets the total size after the starting address has been aligned.
	// Ex: fn(18, 16, 64) = 78.
	[[nodiscard]]
	inline static size_t GetAlignedSize(size_t startingAddress, size_t alignment, size_t size) noexcept
	{
		return size + (Align(startingAddress, alignment) - startingAddress);
	}

protected:
	size_t m_totalSize;
	size_t m_availableSize;
};
#endif
