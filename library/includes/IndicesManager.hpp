#ifndef INDICES_MANAGER_HPP_
#define INDICES_MANAGER_HPP_
#include <ranges>
#include <algorithm>
#include <vector>
#include <optional>

class IndicesManager
{
public:
	IndicesManager() : m_availableIndices{} {}
	IndicesManager(size_t initialSize) : m_availableIndices(initialSize) {}

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
	std::optional<size_t> GetFirstAvailableIndex() noexcept
	{
		auto result = std::ranges::find(m_availableIndices, true);

		if (result != std::end(m_availableIndices))
			return static_cast<size_t>(std::distance(std::begin(m_availableIndices), result));
		else
			return {};
	}

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
#endif
