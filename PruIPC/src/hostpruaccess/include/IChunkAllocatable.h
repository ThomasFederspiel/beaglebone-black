/*
 * IChunkAllocatable.h
 *
 *  Created on: 4 jan 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_ICHUNKALLOCATABLE_H_
#define PRUIPC_ICHUNKALLOCATABLE_H_

#include <cstdlib>
#include <memory>

#include "IMemory.h"

class IChunkAllocatable
{
public:
	virtual ~IChunkAllocatable() = default;

	virtual std::shared_ptr<IMemory> allocateMemoryChunk(const std::size_t size) = 0;
};

#endif /* PRUIPC_ICHUNKALLOCATABLE_H_ */
