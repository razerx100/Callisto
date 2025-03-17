#ifndef REUSABLE_VECTOR_HPP_
#define REUSABLE_VECTOR_HPP_
#include <vector>
#include <deque>
#include <type_traits>
#include <utility>
#include <IndicesManager.hpp>

template<typename T, typename Container_t>
class ReusableContainer
{
public:
	ReusableContainer() : m_elements{}, m_indicesManager{} {}
	ReusableContainer(size_t initialSize) : m_elements{ initialSize }, m_indicesManager{ initialSize }
	{}

	void ReserveNewElements(size_t newCount) noexcept
	{
		m_elements.resize(newCount);
		m_indicesManager.Resize(newCount);
	}

	[[nodiscard]]
	// If these many indices are free then return that or allocate that many.
	std::vector<std::uint32_t> GetFreeIndicesU32(size_t requiredFreeIndexCount) noexcept
	{
		const size_t freeIndexCount = m_indicesManager.GetFreeIndexCount();

		if (freeIndexCount < requiredFreeIndexCount)
			ReserveNewElements(std::size(m_elements) + requiredFreeIndexCount - freeIndexCount);

		return m_indicesManager.GetAllAvailableIndicesU32();
	}

	[[nodiscard]]
	// To use this, T must have a copy ctor for the reserving.
	std::vector<std::uint32_t> AddElementsU32(std::vector<T>&& elements)
	{
		std::vector<std::uint32_t> freeIndices = GetFreeIndicesU32(std::size(elements));

		const size_t newElementCount           = std::size(freeIndices);

		for (size_t index = 0u; index < newElementCount; ++index)
		{
			const size_t freeIndex = freeIndices[index];

			m_elements[freeIndex]  = std::move(elements[index]);
			m_indicesManager.ToggleAvailability(freeIndex, false);
		}

		return freeIndices;
	}

	[[nodiscard]]
	// To use this, T must have a copy ctor for the reserving.
	std::vector<std::uint32_t> AddElementsU32(const std::vector<T>& elements)
	{
		std::vector<std::uint32_t> freeIndices = GetFreeIndicesU32(std::size(elements));

		const size_t newElementCount           = std::size(freeIndices);

		for (size_t index = 0u; index < newElementCount; ++index)
		{
			const size_t freeIndex = freeIndices[index];

			m_elements[freeIndex]  = elements[index];
			m_indicesManager.ToggleAvailability(freeIndex, false);
		}

		return freeIndices;
	}

	[[nodiscard]]
	size_t GetNextFreeIndex(size_t extraAllocCount = 0) noexcept
	{
		size_t elementIndex                 = std::numeric_limits<size_t>::max();
		std::optional<size_t> oElementIndex = m_indicesManager.GetFirstAvailableIndex();

		if (oElementIndex)
			elementIndex = oElementIndex.value();
		else
		{
			// This part should only be executed when both the availableIndices and elements
			// containers have the same size. So, getting the size should be fine.
			elementIndex = GetCount();

			// ElementIndex is the previous size, we have the new item, and then the extraAllocations.
			const size_t newElementCount = elementIndex + 1u + extraAllocCount;

			ReserveNewElements(newElementCount);
		}

		return elementIndex;
	}

	template<typename U>
	// To use this, T must have a copy ctor for the reserving.
	size_t Add(U&& element, size_t extraAllocCount = 0)
	{
		size_t elementIndex      = GetNextFreeIndex(extraAllocCount);

		m_elements[elementIndex] = std::forward<U>(element);
		m_indicesManager.ToggleAvailability(elementIndex, false);

		return elementIndex;
	}

	void RemoveElement(size_t index) noexcept
	{
		m_elements[index] = T{};
		MakeUnavailable(index);
	}

	void MakeUnavailable(size_t index) noexcept
	{
		m_indicesManager.ToggleAvailability(index, true);
	}

	[[nodiscard]]
	bool IsInUse(size_t index) const noexcept { return m_indicesManager.IsInUse(index); }

	T& at(size_t index) noexcept { return m_elements[index]; }
	const T& at(size_t index) const noexcept { return m_elements[index]; }

	T& operator[](size_t index) noexcept { return m_elements[index]; }
	const T& operator[](size_t index) const noexcept { return m_elements[index]; }

	// Don't sort, as that will mess up the indices. If I need to do sorting in the future
	// will have to add custom iterators
	Container_t::iterator begin() { return std::begin(m_elements); }
	Container_t::const_iterator begin() const { return std::begin(m_elements); }

	Container_t::iterator end() { return std::end(m_elements); }
	Container_t::const_iterator end() const { return std::end(m_elements); }

	Container_t::size_type size() const noexcept { return std::size(m_elements); }

	T* data() { return std::data(m_elements); }
	T const* data() const { return std::data(m_elements); }

	bool empty() const { return std::empty(m_elements); }

	void erase(size_t index)
	{
		m_elements.erase(std::next(std::begin(m_elements), index));
		m_indicesManager.erase(index);
	}

	void EraseInactiveElements() noexcept
	{
		size_t inactiveElementCount = m_indicesManager.GetFreeIndexCount();

		// Removing in reverse should be more efficient.
		size_t currentIndex         = std::size(m_elements) - 1u;

		while (inactiveElementCount)
		{
			if (!m_indicesManager.IsInUse(currentIndex))
			{
				--inactiveElementCount;

				erase(currentIndex);
			}
			else
				--currentIndex;
		}
	}

	[[nodiscard]]
	const Container_t& Get() const noexcept { return m_elements; }
	[[nodiscard]]
	Container_t& Get() noexcept { return m_elements; }
	[[nodiscard]]
	T const* GetPtr() const noexcept { return std::data(m_elements); }
	[[nodiscard]]
	T* GetPtr() noexcept { return std::data(m_elements); }
	[[nodiscard]]
	size_t GetCount() const noexcept { return std::size(m_elements); }

	[[nodiscard]]
	const IndicesManager& GetIndicesManager() const noexcept { return m_indicesManager; }

private:
	Container_t    m_elements;
	IndicesManager m_indicesManager;

public:
	ReusableContainer(const ReusableContainer& other) noexcept
		: m_elements{ other.m_elements }, m_indicesManager{ other.m_indicesManager }
	{}
	ReusableContainer& operator=(const ReusableContainer& other) noexcept
	{
		m_elements       = other.m_elements;
		m_indicesManager = other.m_indicesManager;

		return *this;
	}
	ReusableContainer(ReusableContainer&& other) noexcept
		: m_elements{ std::move(other.m_elements) },
		m_indicesManager{ std::move(other.m_indicesManager) }
	{}
	ReusableContainer& operator=(ReusableContainer&& other) noexcept
	{
		m_elements       = std::move(other.m_elements);
		m_indicesManager = std::move(other.m_indicesManager);

		return *this;
	}
};

template<typename T>
using ReusableVector = ReusableContainer<T, std::vector<T>>;

template<typename T>
using ReusableDeque = ReusableContainer<T, std::deque<T>>;
#endif
