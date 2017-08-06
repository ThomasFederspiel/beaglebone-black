/*
 * IPCDeviceCoreProxy.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "IPCDeviceCoreProxy.h"

// boost
#include <boost/numeric/conversion/cast.hpp>

// local
#include "DDRMemory.h"
#include "IMessageReceiver.h"
#include "IPCDeviceProxyService.h"
#include "PrussDriver.h"
#include "pru_ipc_device_core_types.hp"
#include "pru_ipc_devices.hp"

IPCDeviceCoreProxy::IPCDeviceCoreProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy) : AbstractIPCDeviceProxy(receiver, proxy)
{
}

void IPCDeviceCoreProxy::sendTerminate()
{
	const struct IPCDeviceCore_Terminate terminate =
	{
		{
			IPCDeviceCore,
			IPCDeviceCore_Terminate
		}
	};

	sendSyncMessage(reinterpret_cast<const uint8_t*>(&terminate), sizeof(terminate));
}
