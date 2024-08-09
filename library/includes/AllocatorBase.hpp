#ifndef ALLOCATOR_BASE_HPP_
#define ALLOCATOR_BASE_HPP_
#include <cstdint>
#include <array>
#include <vector>
#include <optional>
#include <concepts>

constexpr size_t operator"" _B(unsigned long long number) noexcept
{
	return static_cast<size_t>(number);
}

constexpr size_t operator"" _KB(unsigned long long number) noexcept
{
	return static_cast<size_t>(number * 1024u);
}

constexpr size_t operator"" _MB(unsigned long long number) noexcept
{
	return static_cast<size_t>(number * 1024u * 1024u);
}

constexpr size_t operator"" _GB(unsigned long long number) noexcept
{
	return static_cast<size_t>(number * 1024u * 1024u * 1024u);
}

// Aligns the address to the alignment.
// Ex: fn(address 18, alignment 16) = 32.
template<std::integral T>
[[nodiscard]]
constexpr T Align(T address, T alignment) noexcept
{
	return (address + (alignment - 1u)) & ~(alignment - 1u);
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
	virtual ~AllocatorBase() = default;

	[[nodiscard]]
	size_t TotalSize() const noexcept { return m_totalSize; }
	[[nodiscard]]
	size_t AvailableSize() const noexcept { return m_availableSize; }

    [[nodiscard]]
	// Returns an aligned offset where the requested amount of size can be allocated or throws an
	// exception.
    virtual size_t Allocate(size_t size, size_t alignment) = 0;
    [[nodiscard]]
	// Returns either an aligned offset where the requested amount of size can be allocated or an empty
	// optional.
    virtual std::optional<size_t> AllocateN(size_t size, size_t alignment) noexcept = 0;

	// Call the function with the alignment parameter if you had allocated using the allocate
	// function with the alignment parameter.
	virtual void Deallocate(size_t startingAddress, size_t size, size_t alignment) noexcept = 0;

	[[nodiscard]]
	// Returns an aligned offset where the requested amount of size can be allocated or throws an
	// exception.
	size_t Allocate(size_t size) { return Allocate(size, m_defaultAlignment); }
	[[nodiscard]]
	// Returns either an aligned offset where the requested amount of size can be allocated or an empty
	// optional.
	std::optional<size_t> AllocateN(size_t size) noexcept
	{
		return AllocateN(size, m_defaultAlignment);
	}

	// Call the function with the alignment parameter if you had allocated using the allocate
	// function with the alignment parameter.
	void Deallocate(size_t startingAddress, size_t size) noexcept
	{
		Deallocate(startingAddress, size, m_defaultAlignment);
	}

private:
	size_t m_defaultAlignment;

protected:
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
