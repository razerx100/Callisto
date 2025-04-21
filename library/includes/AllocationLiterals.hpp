#ifndef CALLISTO_ALLOCATION_LITERALS_HPP_
#define CALLISTO_ALLOCATION_LITERALS_HPP_
#include <cstdint>

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
#endif
