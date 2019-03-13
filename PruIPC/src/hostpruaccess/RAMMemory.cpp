/*
 * RANMemory.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "RAMMemory.h"

// standard
#include <cstring>
#include <limits>

// project
#include "exceptionMacros.h"
#include "stdExtension.h"


namespace
{
static constexpr std::size_t RAMPru0Base = 0x0000;
static constexpr std::size_t RAMPru1Base = 0x2000;
static constexpr std::size_t RAMSize = 8 * 1024;
}

RAMMemory::RAMMemory(const RamIdEnum ramId) : m_ramId(ramId), m_allocator()
{
	TB_ASSERT(ramId < RamIdEOE);

	m_allocator = std::make_unique<ChunkAllocator>(*this, baseAddress(), size());
}

std::shared_ptr<IMemory> RAMMemory::allocateMemoryChunk(const std::size_t size)
{
	TB_ASSERT(m_allocator);

	return m_allocator->allocateChunk(size);
}

std::size_t RAMMemory::size() const
{
	return RAMSize;
}

std::size_t RAMMemory::baseAddress() const
{
	std::size_t addr = RAMPru0Base;

	if (m_ramId == RamPru1Id)
	{
		addr = RAMPru1Base;
	}

	return addr;
}

std::shared_ptr<IMemory> RAMMemory::getReference(const std::size_t offset, const std::size_t size)
{
	TB_ASSERT(offset + size <= RAMMemory::size());

	return std::make_shared<ChunkAllocator::MemoryChunk>(*this, baseAddress() + offset, offset, size);
}

void RAMMemory::read(void* const dest, const std::size_t offset, const std::size_t size) const
{
	TB_ASSERT(dest);

	TB_ASSERT(offset + size <= RAMMemory::size());

	const uint8_t* ptr = nullptr;

	prussdrv_map_prumem(m_ramId, (void**) &ptr);

	TB_ASSERT(ptr);
	TB_ASSERT(reinterpret_cast<std::uintptr_t>(ptr + offset) % 2 == 0);

	std::memcpy(dest, ptr + offset, size);
}

void RAMMemory::write(const void* const source, const std::size_t offset, const std::size_t size)
{
	TB_ASSERT(source);

	TB_ASSERT(offset + size <= RAMMemory::size());

	uint8_t* ptr = nullptr;

	prussdrv_map_prumem(m_ramId, (void**)&ptr);

	TB_ASSERT(ptr);
	TB_ASSERT(reinterpret_cast<std::uintptr_t>(ptr + offset) % 2 == 0);

	std::memcpy(ptr + offset, source, size);
}

void RAMMemory::fill(const uint8_t value, const std::size_t offset, const std::size_t size)
{
	std::size_t realSize = size;

	if (realSize == std::numeric_limits<std::size_t>::max())
	{
		realSize = RAMMemory::size();
	}

	TB_ASSERT(offset + realSize <= RAMMemory::size());

	uint8_t* ptr = nullptr;

	prussdrv_map_prumem(m_ramId, (void**)&ptr);

	TB_ASSERT(ptr);
	TB_ASSERT(reinterpret_cast<std::uintptr_t>(ptr + offset) % 2 == 0);

	std::memset(ptr + offset, value, realSize);
}
