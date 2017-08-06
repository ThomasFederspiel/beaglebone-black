/*
 * IPCDeviceTestProxy.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "IPCDeviceTestProxy.h"

// system
#include "prussdrv/prussdrv.h"
#include "prussdrv/pruss_intc_mapping.h"

// local
#include "IMessageReceiver.h"
#include "IPCDeviceProxyService.h"
#include "pru_ipc_device_test.hp"
#include "pru_ipc_devices.hp"

IPCDeviceTestProxy::IPCDeviceTestProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy) : AbstractIPCDeviceProxy(receiver, proxy)
{
}

void IPCDeviceTestProxy::sendTrig()
{
	const struct IPCDeviceTest_Trig trig =
	{
		{
			IPCDeviceTest,
			IPCDeviceTest_Trig
		}
	};

	sendSyncMessage(reinterpret_cast<const uint8_t*>(&trig), sizeof(trig));
}
