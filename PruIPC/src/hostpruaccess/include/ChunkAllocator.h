/*
 * ChunkAllocator.h
 *
 *  Created on: 4 jan 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_CHUNKALLOCATOR_H_
#define PRUIPC_CHUNKALLOCATOR_H_

// standard
#include <cstdlib>
#include <iostream>
#include <memory>
#include <limits>

// project
#include "exceptionMacros.h"

// local
#include "IMemory.h"

class ChunkAllocator final
{
public:

	class MemoryChunk final : public IMemory
	{
	public:

		explicit MemoryChunk(IMemory& memory, const std::size_t address, const std::size_t offset,
				const std::size_t size) : m_memory(memory), m_offset(offset), m_baseAddress(address), m_size(size)
		{
		}

		std::size_t size() const override
		{
			return m_size;
		}

		std::size_t baseAddress() const override
		{
			return m_baseAddress;
		}

		std::shared_ptr<IMemory> getReference(const std::size_t offset, const std::size_t size) override
		{
			TB_ASSERT(m_offset + offset + size <= m_size);

			return std::make_shared<MemoryChunk>(m_memory, m_baseAddress + offset, m_offset + offset, size);
		}

		void read(void* const dest, const std::size_t address, const std::size_t size) const override
		{
			TB_ASSERT(address + size <= m_size);

			m_memory.read(dest, m_offset + address, size);
		}

		void write(const void* const source, const std::size_t address, const std::size_t size) override
		{
			TB_ASSERT(address + size <= m_size);

			m_memory.write(source, m_offset + address, size);
		}

		void fill(const uint8_t value, const std::size_t address, const std::size_t size)
		{
			std::size_t realSize = size;

			if (realSize == std::numeric_limits<std::size_t>::max())
			{
				realSize = m_size;
			}

			TB_ASSERT(address + realSize <= m_size);

			m_memory.fill(value, m_offset + address, realSize);
		}

	private:
		IMemory& m_memory;
		std::size_t m_offset;
		std::size_t m_baseAddress;
		std::size_t m_size;
	};

	explicit ChunkAllocator(IMemory& memory, const std::size_t address, const std::size_t size) : m_memory(memory), m_allocOffset(0),
			m_baseAddress(address), m_size(size)
	{
	}

	std::shared_ptr<IMemory> allocateChunk(const std::size_t size)
	{
		std::shared_ptr<MemoryChunk> chunk;

		if (m_allocOffset + size <= m_size)
		{
			chunk = std::make_shared<MemoryChunk>(m_memory, m_baseAddress + m_allocOffset, m_allocOffset, size);

			// Keep offset byte aligned
			m_allocOffset += size + size % 2;
		}

		return std::move(chunk);
	}

private:

	IMemory& m_memory;
	std::size_t m_allocOffset;
	std::size_t m_baseAddress;
	std::size_t m_size;
};

#endif /* PRUIPC_CHUNKALLOCATOR_H_ */
