#ifndef CALLISTO_SHARED_PTR_VECTOR_HPP_
#define CALLISTO_SHARED_PTR_VECTOR_HPP_
#include <vector>
#include <memory>
#include <type_traits>

namespace Callisto
{
template<typename T>
std::shared_ptr<std::uint8_t[]> CopyVectorToSharedPtr(const std::vector<T>& container) noexcept
{
	const auto bufferSize = std::size(container) * sizeof(T);

	auto dataBuffer = std::make_shared<std::uint8_t[]>(bufferSize);
	memcpy(dataBuffer.get(), std::data(container), bufferSize);

	return dataBuffer;
}
}
#endif
