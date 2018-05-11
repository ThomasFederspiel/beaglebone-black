/*
 * IMemory.h
 *
 *  Created on: 4 jan 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_IMEMORY_H_
#define PRUIPC_IMEMORY_H_

#include <cstdlib>
#include <memory>
#include <limits>


class IMemory
{
public:
	virtual ~IMemory() = default;

	virtual std::size_t size() const = 0;
	virtual std::size_t baseAddress() const = 0;
	virtual std::shared_ptr<IMemory> getReference(const std::size_t offset, const std::size_t size) = 0;
	virtual void read(void* const dest, const std::size_t address, const std::size_t size) const = 0;
	virtual void write(const void* const source, const std::size_t address, const std::size_t size) = 0;
	virtual void fill(const uint8_t value, const std::size_t offset = 0, const std::size_t size = std::numeric_limits<std::size_t>::max()) = 0;
};

#endif /* PRUIPC_IMEMORY_H_ */
