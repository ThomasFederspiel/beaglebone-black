/*
 * RAMMemory.h
 *
 *  Created on: 4 jan 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_RAMMEMORY_H_
#define PRUIPC_RAMMEMORY_H_

#include <memory>

#include "prussdrv/prussdrv.h"

#include "ChunkAllocator.h"
#include "IChunkAllocatable.h"
#include "IMemory.h"

class RAMMemory final : public IMemory, public IChunkAllocatable
{
public:

	enum RamIdEnum : unsigned int
	{
		RamPru0Id = PRUSS0_PRU0_DATARAM,
		RamPru1Id = PRUSS0_PRU1_DATARAM,
		RamIdEOE
	};

	explicit RAMMemory(const RamIdEnum ramId);

	std::shared_ptr<IMemory> allocateMemoryChunk(const std::size_t size) override;
	std::size_t size() const override;
	std::size_t baseAddress() const override;
	std::shared_ptr<IMemory> getReference(const std::size_t offset, const std::size_t size) override;
	void read(void* const dest, const std::size_t address, const std::size_t size) const override;
	void write(const void* const source, const std::size_t address, const std::size_t size) override;
	void fill(const uint8_t value, const std::size_t offset, const std::size_t size) override;

private:
	unsigned int m_ramId;
	std::unique_ptr<ChunkAllocator> m_allocator;
};

#endif /* PRUIPC_RAMMEMORY_H_ */
