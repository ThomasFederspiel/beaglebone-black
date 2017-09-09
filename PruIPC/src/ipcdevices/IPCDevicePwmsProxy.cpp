/*
 * IPCDevicePwmsProxy.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "IPCDevicePwmsProxy.h"

// local
#include "IMessageReceiver.h"
#include "IPCDeviceProxyService.h"
#include "pru_ipc_devices.hp"
#include "pru_ipc_device_pwms_types.hp"
#include "SinkMessageReceiver.h"

namespace
{
	SinkMessageReceiver m_sink;
};

IPCDevicePwmsProxy::IPCDevicePwmsProxy(IPCDeviceProxyService& proxy,
		const PwmssDeviceEnum pwmssDevice) : IPCDevicePwmsProxy(m_sink, proxy, pwmssDevice)
{
}

IPCDevicePwmsProxy::IPCDevicePwmsProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy,
		const PwmssDeviceEnum pwmssDevice) : AbstractIPCDeviceProxy(receiver, proxy), m_pwmssDevice(pwmssDevice)
{
}

void IPCDevicePwmsProxy::open()
{
	struct IPCDevicePwms_Open open =
	{
		{
			IPCDevicePwms,
			IPCDevicePwms_Open
		},
		m_pwmssDevice
	};

	sendSyncMessage(reinterpret_cast<uint8_t*>(&open), sizeof(open));
}

void IPCDevicePwmsProxy::close()
{
	struct IPCDevicePwms_Close close =
	{
		{
			IPCDevicePwms,
			IPCDevicePwms_Close
		},
		m_pwmssDevice
	};

	sendSyncMessage(reinterpret_cast<uint8_t*>(&close), sizeof(close));
}
