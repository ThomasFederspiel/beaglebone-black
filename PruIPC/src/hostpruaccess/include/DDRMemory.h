/*
 * DDRMemory.h
 *
 *  Created on: 4 jan 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_DDRMEMORY_H_
#define PRUIPC_DDRMEMORY_H_

#include <memory>

#include "ChunkAllocator.h"
#include "IChunkAllocatable.h"
#include "IMemory.h"

class DDRMemory final : public IMemory, public IChunkAllocatable
{
public:
	explicit DDRMemory();

	std::shared_ptr<IMemory> allocateMemoryChunk(const std::size_t size) override;
	std::size_t size() const override;
	std::size_t baseAddress() const override;
	std::shared_ptr<IMemory> getReference(const std::size_t offset, const std::size_t size) override;
	void read(void* const dest, const std::size_t address, const std::size_t size) const override;
	void write(const void* const source, const std::size_t address, const std::size_t size) override;

private:
	void getDDRInfo();

	std::size_t m_base;
	std::size_t m_size;
	std::unique_ptr<ChunkAllocator> m_allocator;
};

#endif /* PRUIPC_DDRMEMORY_H_ */
