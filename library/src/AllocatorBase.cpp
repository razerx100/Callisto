#include <AllocatorBase.hpp>

size_t AllocatorBase::BitsNeededFor(size_t value) noexcept
{
	constexpr size_t maxBits        = sizeof(size_t) * 8u;
	constexpr size_t leftBitChecker = 0x8000000000000000lu;

	for (size_t bitCount = maxBits; bitCount > 0u; --bitCount)
	{
		if (value & leftBitChecker)
			return bitCount;

		value <<= 1u;
	}

	return 0u;
}
