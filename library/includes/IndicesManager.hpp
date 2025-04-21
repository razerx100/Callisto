#ifndef CALLISTO_INDICES_MANAGER_HPP_
#define CALLISTO_INDICES_MANAGER_HPP_
#include <ranges>
#include <algorithm>
#include <vector>
#include <optional>

namespace Callisto
{
class IndicesManager
{
public:
	IndicesManager() : m_availableIndices{} {}
	IndicesManager(size_t initialSize) : m_availableIndices(initialSize, true) {}

	void ToggleAvailability(size_t index, bool on) noexcept
	{
		m_availableIndices[index] = on;
	}

	void Resize(size_t newCount)
	{
		m_availableIndices.resize(newCount, true);
	}

	[[nodiscard]]
	bool IsInUse(size_t index) const noexcept { return !m_availableIndices[index]; }

	[[nodiscard]]
	std::optional<size_t> GetFirstAvailableIndex() const noexcept
	{
		auto result = std::ranges::find(m_availableIndices, true);

		if (result != std::end(m_availableIndices))
			return static_cast<size_t>(std::distance(std::begin(m_availableIndices), result));
		else
			return {};
	}

	[[nodiscard]]
	std::optional<size_t> GetNextAvailableIndex(size_t currentIndex) const noexcept
	{
		const size_t indexCount = std::size(m_availableIndices);

		for (size_t index = currentIndex + 1u; index < indexCount; ++index)
			if (m_availableIndices[index])
				return index;

		return {};
	}

	// Only doing it for U32 since I don't like unnecessary allocations and if we decide to
	// store the indices, it will be as U32.
	[[nodiscard]]
	std::vector<std::uint32_t> GetAllAvailableIndicesU32() const noexcept
	{
		std::vector<std::uint32_t> availableIndices{};

		const size_t indexCount = std::size(m_availableIndices);

		for (size_t index = 0u; index < indexCount; ++index)
			if (m_availableIndices[index])
				availableIndices.emplace_back(static_cast<std::uint32_t>(index));

		return availableIndices;
	}

	[[nodiscard]]
	size_t GetFreeIndexCount() const noexcept
	{
		return std::ranges::count(m_availableIndices, true);
	}

	[[nodiscard]]
	size_t GetActiveIndexCount() const noexcept
	{
		return std::ranges::count(m_availableIndices, false);
	}

	void erase(size_t index)
	{
		m_availableIndices.erase(std::next(std::begin(m_availableIndices), index));
	}

	std::vector<bool>::size_type size() const noexcept { return std::size(m_availableIndices); }

private:
	std::vector<bool> m_availableIndices;

public:
	IndicesManager(const IndicesManager& other) noexcept
		: m_availableIndices{ other.m_availableIndices }
	{}
	IndicesManager& operator=(const IndicesManager& other) noexcept
	{
		m_availableIndices = other.m_availableIndices;

		return *this;
	}
	IndicesManager(IndicesManager&& other) noexcept
		: m_availableIndices{ std::move(other.m_availableIndices) }
	{}
	IndicesManager& operator=(IndicesManager&& other) noexcept
	{
		m_availableIndices = std::move(other.m_availableIndices);

		return *this;
	}
};
}
#endif
