#ifndef SHARED_BUFFER_ALLOCATOR_HPP_
#define SHARED_BUFFER_ALLOCATOR_HPP_
#include <cstdint>
#include <vector>
#include <optional>

class SharedBufferAllocator
{
	friend class SharedBufferAllocatorTest;

public:
	struct AllocInfo
	{
		size_t offset;
		size_t size;
	};

public:
	SharedBufferAllocator() : m_availableMemory{} {}

	[[nodiscard]]
	// The offset from the start of the buffer will be returned. Should make sure
	// there is enough memory before calling this.
	size_t AllocateMemory(const AllocInfo& allocInfo, size_t size) noexcept;

	void AddAllocInfo(size_t offset, size_t size) noexcept;
	void RelinquishMemory(size_t offset, size_t size) noexcept
	{
		AddAllocInfo(offset, size);
	}

	[[nodiscard]]
	std::optional<size_t> GetAvailableAllocInfo(size_t size) const noexcept;
	[[nodiscard]]
	AllocInfo GetAndRemoveAllocInfo(size_t index) noexcept;

private:
	std::vector<AllocInfo> m_availableMemory;

public:
	SharedBufferAllocator(const SharedBufferAllocator& other) noexcept
		: m_availableMemory{ other.m_availableMemory }
	{}
	SharedBufferAllocator& operator=(const SharedBufferAllocator& other) noexcept
	{
		m_availableMemory = other.m_availableMemory;

		return *this;
	}
	SharedBufferAllocator(SharedBufferAllocator&& other) noexcept
		: m_availableMemory{ std::move(other.m_availableMemory) }
	{}
	SharedBufferAllocator& operator=(SharedBufferAllocator&& other) noexcept
	{
		m_availableMemory = std::move(other.m_availableMemory);

		return *this;
	}
};
#endif
