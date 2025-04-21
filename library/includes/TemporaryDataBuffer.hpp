#ifndef CALLISTO_TEMPORARY_DATA_BUFFER_HPP_
#define CALLISTO_TEMPORARY_DATA_BUFFER_HPP_
#include <vector>
#include <memory>

namespace Callisto
{
class TemporaryDataBufferCPU
{
public:
	TemporaryDataBufferCPU() : m_tempBuffer{} {};

	void Add(std::shared_ptr<void> tempData) noexcept
	{
		m_tempBuffer.emplace_back(std::move(tempData));
	}

	void Clear() noexcept { m_tempBuffer.clear(); }

private:
	std::vector<std::shared_ptr<void>> m_tempBuffer;

public:
	TemporaryDataBufferCPU(const TemporaryDataBufferCPU&) = delete;
	TemporaryDataBufferCPU& operator=(const TemporaryDataBufferCPU&) = delete;

	TemporaryDataBufferCPU(TemporaryDataBufferCPU&& other) noexcept
		: m_tempBuffer{ std::move(other.m_tempBuffer) }
	{}
	TemporaryDataBufferCPU& operator=(TemporaryDataBufferCPU&& other) noexcept
	{
		m_tempBuffer = std::move(other.m_tempBuffer);

		return *this;
	}
};

class TemporaryDataBufferGPU
{
private:
	struct TempBuffer
	{
		std::shared_ptr<void> buffer;
		size_t                ownerIndex;
	};

public:
	TemporaryDataBufferGPU() : m_tempBuffers{} {}

	void Add(std::shared_ptr<void> tempData) noexcept
	{
		m_tempBuffers.emplace_back(
			TempBuffer{
				.buffer     = std::move(tempData),
				.ownerIndex = std::numeric_limits<size_t>::max()
			}
		);
	}

	void SetUsed(size_t frameIndex) noexcept;

	void Clear(size_t frameIndex) noexcept;

private:
	std::vector<TempBuffer> m_tempBuffers;

public:
	TemporaryDataBufferGPU(const TemporaryDataBufferGPU&) = delete;
	TemporaryDataBufferGPU& operator=(const TemporaryDataBufferGPU&) = delete;

	TemporaryDataBufferGPU(TemporaryDataBufferGPU&& other) noexcept
		: m_tempBuffers{ std::move(other.m_tempBuffers) }
	{}
	TemporaryDataBufferGPU& operator=(TemporaryDataBufferGPU&& other) noexcept
	{
		m_tempBuffers = std::move(other.m_tempBuffers);

		return *this;
	}
};
}
#endif
