/*
 * ChunkAllocator.h
 *
 *  Created on: 4 jan 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_CHUNKALLOCATOR_H_
#define PRUIPC_CHUNKALLOCATOR_H_

#include <cstdlib>
#include <memory>

#include <iostream>

#include "exceptionMacros.h"
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

			m_allocOffset += size;
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
