/*
 * PruEventDefinition.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_PRUEVENTDEFINITION_H_
#define TBOX_PRUEVENTDEFINITION_H_

// standard
#include <memory>

// project
#include "PrussDriver.h"
#include "pru_ipc_devices.hp"
#include "ServiceMessageBase.h"
#include "ServiceTypes.h"
#include "stdExtension.h"

class AbstractEventDefinition
{
public:
	explicit AbstractEventDefinition()
	{
	}

	virtual IPCDeviceEnum getDevice() const = 0;
	virtual ioctl_t getIoctl() const = 0;
	virtual MessageType_t getType() const = 0;
	virtual uint32_t size() const = 0;
	virtual std::unique_ptr<ServiceMessageBase> processEvent(const PrussDriver::PruProxy::PruIdEnum pruId, const uint8_t* data) const = 0;
};

template <typename M>
class EventDefinition final : public AbstractEventDefinition
{
public:

	explicit EventDefinition() : AbstractEventDefinition()
	{
	}

	IPCDeviceEnum getDevice() const override
	{
		return M::PruDevice;
	}

	ioctl_t getIoctl() const override
	{
		return M::PruIoctl;
	}

	MessageType_t getType() const override
	{
		return M::EventMessageType;
	}

	uint32_t size() const override
	{
		static_assert(sizeof(typename M::pruEvenType_t) % 2 == 0, "Size of PRU event must be an even number of bytes");

		return sizeof(typename M::pruEvenType_t);
	}

	std::unique_ptr<ServiceMessageBase> processEvent(const PrussDriver::PruProxy::PruIdEnum pruId, const uint8_t* data) const override
	{
		return std::make_unique<M>(pruId, *reinterpret_cast<const typename M::pruEvenType_t*>(data));
	}
};

#endif /* TBOX_PRUEVENTDEFINITION_H_ */
