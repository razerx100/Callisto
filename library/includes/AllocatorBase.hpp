#ifndef ALLOCATOR_BASE_HPP_
#define ALLOCATOR_BASE_HPP_
#include <array>
#include <vector>
#include <optional>
#include <concepts>
#include <AllocationLiterals.hpp>

// Aligns the address to the alignment.
// Ex: fn(address 18, alignment 16) = 32.
template<std::integral T, std::integral G>
[[nodiscard]]
constexpr T Align(T address, G alignment) noexcept
{
	// The address should always have the biggest size.
	// So, casting to T should solve the size mismatching warnings.
	const auto tAlignment = static_cast<T>(alignment);

	return (address + (tAlignment - 1u)) & ~(tAlignment - 1u);
}

class AllocatorBase
{
	friend class TestAllocatorBase;
public:
	AllocatorBase(size_t totalSize)
		: m_defaultAlignment{ 0u }, m_totalSize{ totalSize }, m_availableSize{ totalSize }
	{}
	AllocatorBase(size_t totalSize, size_t defaultAlignment)
		: m_defaultAlignment{ defaultAlignment }, m_totalSize{ totalSize },
		m_availableSize{ totalSize }
	{}

	[[nodiscard]]
	size_t TotalSize() const noexcept { return m_totalSize; }
	[[nodiscard]]
	size_t AvailableSize() const noexcept { return m_availableSize; }

protected:
	size_t m_defaultAlignment;
	size_t m_totalSize;
	size_t m_availableSize;

protected:
	template<std::integral T>
	struct AllocInfo
	{
		T startingAddress;
		T size;

		AllocInfo() = default;
		AllocInfo(T startingAddress, T size) : startingAddress{ startingAddress }, size{ size }
		{}

		AllocInfo(const AllocInfo& other) noexcept
			: startingAddress{ other.startingAddress }, size{ other.size }
		{}
		AllocInfo(AllocInfo&& other) noexcept
			: startingAddress{ other.startingAddress }, size{ other.size }
		{}

		AllocInfo& operator=(const AllocInfo& other) noexcept
		{
			startingAddress = other.startingAddress;
			size            = other.size;

			return *this;
		}
		AllocInfo& operator=(AllocInfo&& other) noexcept
		{
			startingAddress = other.startingAddress;
			size            = other.size;

			return *this;
		}

		template<std::integral G>
		AllocInfo(const AllocInfo<G>& other) noexcept
			: startingAddress{ static_cast<T>(other.startingAddress) },
			size{ static_cast<T>(other.size) }
		{}
		template<std::integral G>
		AllocInfo(AllocInfo<G>&& other) noexcept
			: startingAddress{ static_cast<T>(other.startingAddress) },
			size{ static_cast<T>(other.size) }
		{}

		template<std::integral G>
		AllocInfo& operator=(const AllocInfo<G>& other) noexcept
		{
			startingAddress = static_cast<T>(other.startingAddress);
			size            = static_cast<T>(other.size);

			return *this;
		}
		template<std::integral G>
		AllocInfo& operator=(AllocInfo<G>&& other) noexcept
		{
			startingAddress = static_cast<T>(other.startingAddress);
			size            = static_cast<T>(other.size);

			return *this;
		}
	};

	typedef AllocInfo<std::uint64_t> AllocInfo64;
	typedef AllocInfo<std::uint32_t> AllocInfo32;
	typedef AllocInfo<std::uint16_t> AllocInfo16;
	typedef AllocInfo<std::uint8_t> AllocInfo8;

	template<std::integral T>
	static constexpr AllocInfo<T> MakeAllocInfo(size_t startingAddress, size_t size) noexcept
	{
		return AllocInfo<T>{ static_cast<T>(startingAddress), static_cast<T>(size) };
	}

protected:
	[[nodiscard]]
	static size_t BitsNeededFor(size_t value) noexcept;

	// Gets the upperBound 2s Exponent.
	// Ex: fn(48) = 64.
	[[nodiscard]]
	static size_t GetUpperBound2sExponent(size_t size) noexcept
	{
		size_t result = 1u;
		for (; result < size; result <<= 1u);

		return result;
	}

	// Gets the lowerBound 2s Exponent.
	// Ex: fn(48) = 32.
	[[nodiscard]]
	static size_t GetLowerBound2sExponent(size_t size) noexcept
	{
		size_t result = 0x8000000000000000lu;
		for (; result > size; result >>= 1u);

		return result;
	}

	// Gets the total size after the starting address has been aligned.
	// Ex: fn(18, 16, 64) = 78.
	[[nodiscard]]
	static size_t GetAlignedSize(size_t startingAddress, size_t alignment, size_t size) noexcept
	{
		return size + (Align(startingAddress, alignment) - startingAddress);
	}

public:
	AllocatorBase(const AllocatorBase&) = delete;
	AllocatorBase& operator=(const AllocatorBase&) = delete;

	AllocatorBase(AllocatorBase&& other) noexcept
		: m_defaultAlignment{ other.m_defaultAlignment }, m_totalSize{ other.m_totalSize },
		m_availableSize{ other.m_availableSize } {}

	AllocatorBase& operator=(AllocatorBase&& other) noexcept
	{
		m_defaultAlignment = other.m_defaultAlignment;
		m_totalSize        = other.m_totalSize;
		m_availableSize    = other.m_availableSize;

		return *this;
	}
};
#endif
