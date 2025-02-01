#ifndef REUSABLE_VECTOR_HPP_
#define REUSABLE_VECTOR_HPP_
#include <vector>
#include <type_traits>
#include <utility>
#include <IndicesManager.hpp>

template<typename T>
class ReusableVector
{
public:
	ReusableVector() = default;
	ReusableVector(size_t initialSize) : m_elements{ initialSize }, m_indicesManager{ initialSize }
	{}

	void ReserveNewElements(size_t newCount) noexcept
	{
		m_elements.resize(newCount);
		m_indicesManager.Resize(newCount);
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
	size_t Add(U&& element, size_t extraAllocCount)
	{
		size_t elementIndex = GetNextFreeIndex(extraAllocCount);

		m_elements[elementIndex] = std::move(element);
		m_indicesManager.ToggleAvailability(elementIndex, false);

		return elementIndex;
	}

	template<typename U>
	size_t Add(U&& element)
	{
		return Add(std::move(element), 0u);
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

	std::vector<T>::iterator begin() { return std::begin(m_elements); }
	std::vector<T>::const_iterator begin() const { return std::begin(m_elements); }

	std::vector<T>::iterator end() { return std::end(m_elements); }
	std::vector<T>::const_iterator end() const { return std::end(m_elements); }

	std::vector<T>::size_type size() const noexcept { return std::size(m_elements); }

	[[nodiscard]]
	const std::vector<T>& Get() const noexcept { return m_elements; }
	[[nodiscard]]
	std::vector<T>& Get() noexcept { return m_elements; }
	[[nodiscard]]
	T const* GetPtr() const noexcept { return std::data(m_elements); }
	[[nodiscard]]
	T* GetPtr() noexcept { return std::data(m_elements); }
	[[nodiscard]]
	size_t GetCount() const noexcept { return std::size(m_elements); }

private:
	std::vector<T> m_elements;
	IndicesManager m_indicesManager;

public:
	ReusableVector(const ReusableVector& other) noexcept
		: m_elements{ other.m_elements }, m_indicesManager{ other.m_indicesManager }
	{}
	ReusableVector& operator=(const ReusableVector& other) noexcept
	{
		m_elements       = other.m_elements;
		m_indicesManager = other.m_indicesManager;

		return *this;
	}
	ReusableVector(ReusableVector&& other) noexcept
		: m_elements{ std::move(other.m_elements) },
		m_indicesManager{ std::move(other.m_indicesManager) }
	{}
	ReusableVector& operator=(ReusableVector&& other) noexcept
	{
		m_elements       = std::move(other.m_elements);
		m_indicesManager = std::move(other.m_indicesManager);

		return *this;
	}
};
#endif
