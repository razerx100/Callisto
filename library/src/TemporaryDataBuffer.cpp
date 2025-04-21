#include <TemporaryDataBuffer.hpp>

namespace Callisto
{
void TemporaryDataBufferGPU::SetUsed(size_t frameIndex) noexcept
{
	// The tempBuffer should be like a queue and the front of it should be already used.
	// So, it should be a bit faster if I start marking the new resources used from the
	// back.
	for (auto rIt = std::rbegin(m_tempBuffers); rIt != std::rend(m_tempBuffers); ++rIt)
	{
		TempBuffer& tempBuffer = *rIt;

		// Once we have hit the first tempBuffer which has an owner, every next buffer
		// should also have an owner. So, we shouldn't need to go any farther.
		if (tempBuffer.ownerIndex != std::numeric_limits<size_t>::max())
			break;

		tempBuffer.ownerIndex = frameIndex;
	}
}

void TemporaryDataBufferGPU::Clear(size_t frameIndex) noexcept
{
	std::erase_if(
		m_tempBuffers,
		[frameIndex](const TempBuffer& tempBuffer)
		{ return tempBuffer.ownerIndex == frameIndex; }
	);
}
}
