/*
 * DDRMemory.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "DDRMemory.h"

// standard
#include <cstring>
#include <limits>

// system
#include "prussdrv/prussdrv.h"

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"

// local
#include "ChunkAllocator.h"

DDRMemory::DDRMemory() : m_base(0), m_size(0), m_allocator()
{
	getDDRInfo();

	m_allocator = tbox::make_unique<ChunkAllocator>(*this, baseAddress(), size());
}

std::shared_ptr<IMemory> DDRMemory::allocateMemoryChunk(const std::size_t size)
{
	TB_ASSERT(m_allocator);

	return m_allocator->allocateChunk(size);
}

std::size_t DDRMemory::size() const
{
	return m_size;
}

std::size_t DDRMemory::baseAddress() const
{
	return m_base;
}

std::shared_ptr<IMemory> DDRMemory::getReference(const std::size_t offset, const std::size_t size)
{
	TB_ASSERT(offset + size <= DDRMemory::size());

	return std::make_shared<ChunkAllocator::MemoryChunk>(*this, baseAddress() + offset, offset, size);
}

void DDRMemory::read(void* const dest, const std::size_t offset, const std::size_t size) const
{
	TB_ASSERT(dest);

	TB_ASSERT(offset + size <= DDRMemory::size());

	const uint8_t* ptr = nullptr;

	prussdrv_map_extmem((void**)&ptr);

	TB_ASSERT(ptr);

	memcpy(dest, ptr + offset, size);
}

void DDRMemory::write(const void* const source, const std::size_t offset, const std::size_t size)
{
	TB_ASSERT(source);

	TB_ASSERT(offset + size <= DDRMemory::size());

	uint8_t* ptr = nullptr;

	prussdrv_map_extmem((void**)&ptr);

	TB_ASSERT(ptr);

	memcpy(ptr + offset, source, size);
}

void DDRMemory::fill(const uint8_t value, const std::size_t offset, const std::size_t size)
{
	std::size_t realSize = size;

	if (realSize == std::numeric_limits<std::size_t>::max())
	{
		realSize = DDRMemory::size();
	}

	TB_ASSERT(offset + realSize <= DDRMemory::size());

	uint8_t* ptr = nullptr;

	prussdrv_map_extmem((void**)&ptr);

	TB_ASSERT(ptr);

	memset(ptr + offset, value, realSize);
}

void DDRMemory::getDDRInfo()
{
	volatile const uint32_t* ptr = nullptr;

	prussdrv_map_extmem((void**)&ptr);

	TB_ASSERT(ptr);

	m_base = prussdrv_get_phys_addr((void*)ptr);
	m_size = prussdrv_extmem_size();
}

