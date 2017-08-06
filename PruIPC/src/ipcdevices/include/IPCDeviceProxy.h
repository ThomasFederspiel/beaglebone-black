/*
 * IPCDeviceProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEPROXY_H_
#define PRUIPC_IPCDEVICEPROXY_H_

// standard
#include <stdint.h>
#include <memory>

// project
#include "IMemory.h"
#include "PrussDriver.h"

// ;+ Remove no longer needed

class IPCDeviceProxy
{
public:
	IPCDeviceProxy(const PrussDriver::PruProxy::PruIdEnum pruId, PrussDriver::BiDirectionPruEventChannelPtr biDirectionEventChannel);

	void sendSyncMessage(const uint8_t* const message, const uint8_t size);

private:
	std::shared_ptr<IMemory> m_ipcBiDirectionalChannelContext;

	PrussDriver::BiDirectionPruEventChannelPtr m_eventChannel;
};

#endif /* PRUIPC_IPCDEVICEPROXY_H_ */
