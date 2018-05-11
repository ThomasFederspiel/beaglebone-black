/*
 * SHRANMemory.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "SHRAMMemory.h"

// standard
#include <cstring>
#include <limits>

// system
#include "prussdrv/prussdrv.h"

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"

namespace
{
static constexpr std::size_t SHRAMBaseAddress = 0x10000;
static constexpr std::size_t SHRAMSize = 12 * 1024;
}

SHRAMMemory::SHRAMMemory() : m_allocator()
{
	m_allocator = tbox::make_unique<ChunkAllocator>(*this, baseAddress(), size());
}

std::shared_ptr<IMemory> SHRAMMemory::allocateMemoryChunk(const std::size_t size)
{
	TB_ASSERT(m_allocator);

	return m_allocator->allocateChunk(size);
}

std::size_t SHRAMMemory::size() const
{
	return SHRAMSize;
}

std::size_t SHRAMMemory::baseAddress() const
{
	return SHRAMBaseAddress;
}

std::shared_ptr<IMemory> SHRAMMemory::getReference(const std::size_t offset, const std::size_t size)
{
	TB_ASSERT(offset + size <= SHRAMMemory::size());

	return std::make_shared<ChunkAllocator::MemoryChunk>(*this, baseAddress() + offset, offset, size);
}

void SHRAMMemory::read(void* const dest, const std::size_t offset, const std::size_t size) const
{
	TB_ASSERT(dest);

	TB_ASSERT(offset + size <= SHRAMMemory::size());

	const uint8_t* ptr = nullptr;

	prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, (void**) &ptr);

	TB_ASSERT(ptr);

	memcpy(dest, ptr + offset, size);
}

void SHRAMMemory::write(const void* const source, const std::size_t offset, const std::size_t size)
{
	TB_ASSERT(source);

	TB_ASSERT(offset + size <= SHRAMMemory::size());

	uint8_t* ptr = nullptr;

	prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, (void**)&ptr);

	TB_ASSERT(ptr);

	memcpy(ptr + offset, source, size);
}

void SHRAMMemory::fill(const uint8_t value, const std::size_t offset, const std::size_t size)
{
	std::size_t realSize = size;

	if (realSize == std::numeric_limits<std::size_t>::max())
	{
		realSize = SHRAMMemory::size();
	}

	TB_ASSERT(offset + realSize <= SHRAMMemory::size());

	uint8_t* ptr = nullptr;

	prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, (void**)&ptr);

	TB_ASSERT(ptr);

	memset(ptr + offset, value, realSize);
}
