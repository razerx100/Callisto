#ifndef CALLISTO_ALLOCATOR_HPP_
#define CALLISTO_ALLOCATOR_HPP_
#include <Buddy.hpp>
#include <concepts>

namespace Callisto
{
class Allocator
{
public:
    Allocator(size_t memoryStart, size_t memorySize, size_t minimumBlockSize) noexcept
        : m_allocator{ memoryStart, memorySize, minimumBlockSize }
    {}
    Allocator(void* memoryStart, size_t memorySize, size_t minimumBlockSize) noexcept
        : Allocator{ ToSizeT(memoryStart), memorySize, minimumBlockSize }
    {}
    Allocator(
        size_t memoryStart, size_t memorySize, size_t minimumBlockSize, size_t defaultAlignment
    ) noexcept
        : m_allocator{ memoryStart, memorySize, minimumBlockSize, defaultAlignment }
    {}
    Allocator(
        void* memoryStart, size_t memorySize, size_t minimumBlockSize, size_t defaultAlignment
    ) noexcept
        : Allocator{ ToSizeT(memoryStart), memorySize, minimumBlockSize, defaultAlignment }
    {}

    template<typename T = void>
    [[nodiscard]]
    T* Allocate(size_t size, size_t alignment)
    {
        return reinterpret_cast<T*>(m_allocator.Allocate(size, alignment));
    }

    template<std::integral T = size_t>
    [[nodiscard]]
    T AllocateI(size_t size, size_t alignment)
    {
        return static_cast<T>(m_allocator.Allocate(size, alignment));
    }

    void Deallocate(void* ptr, size_t size, size_t alignment) noexcept
    {
        m_allocator.Deallocate(ToSizeT(ptr), size, alignment);
    }

    template<typename T = void>
    [[nodiscard]]
    T* Allocate(size_t size)
    {
        return reinterpret_cast<T*>(m_allocator.Allocate(size));
    }

    template<std::integral T = size_t>
    [[nodiscard]]
    T AllocateI(size_t size)
    {
        return static_cast<T>(m_allocator.Allocate(size));
    }

    void Deallocate(void* ptr, size_t size) noexcept
    {
        m_allocator.Deallocate(ToSizeT(ptr), size);
    }

	[[nodiscard]]
	// If the given size isn't an exponent of 2, the biggest memory block will be the largest 2's
	// exponent which is smaller than the size. So, this function should be used to query first
	// the size of the largest block, so the allocation doesn't fail.
    static size_t GetMinimumRequiredNewAllocationSizeFor(size_t size) noexcept
    {
        return Buddy::GetMinimumRequiredNewAllocationSizeFor(size);
    }

    [[nodiscard]]
    size_t GetMemorySize() const noexcept { return m_allocator.TotalSize(); }
    [[nodiscard]]
    size_t GetAvailableSize() const noexcept { return m_allocator.AvailableSize(); }

private:
    [[nodiscard]]
    static size_t ToSizeT(void* ptr) noexcept
    {
        return reinterpret_cast<size_t>(ptr);
    }

private:
    Buddy m_allocator;

public:
    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;

    Allocator(Allocator&& alloc) noexcept : m_allocator{ std::move(alloc.m_allocator) }
    {}

    Allocator& operator=(Allocator&& alloc) noexcept
    {
        m_allocator = std::move(alloc.m_allocator);

        return *this;
    }
};
}
#endif
