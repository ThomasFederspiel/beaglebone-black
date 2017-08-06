/*
 * IPCDeviceGpioProxy.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "IPCDeviceGpioProxy.h"

// local
#include "IMessageReceiver.h"
#include "IPCDeviceProxyService.h"
#include "pru_ipc_devices.hp"
#include "pru_ipc_device_gpio_types.hp"
#include "SinkMessageReceiver.h"

// project
#include "exceptionMacros.h"
#include "Logger.h"

MODULE_LOG(IPCDeviceGpioProxy)

namespace
{
	SinkMessageReceiver m_sink;
};

IPCDeviceGpioProxy::IPCDeviceGpioProxy(IPCDeviceProxyService& proxy) : IPCDeviceGpioProxy(m_sink, proxy)
{
}

IPCDeviceGpioProxy::IPCDeviceGpioProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy) : AbstractIPCDeviceProxy(receiver, proxy)
{
}

void IPCDeviceGpioProxy::set(const uint32_t pin)
{
	const struct IPCDeviceGpio_Set set =
	{
		{
			IPCDeviceGpio,
			IPCDeviceGpio_Set
		},
		pin
	};

	TB_ASSERT(pin & OUT_BIT);

	sendSyncMessage(reinterpret_cast<const uint8_t*>(&set), sizeof(set));
}

void IPCDeviceGpioProxy::clear(const uint32_t pin)
{
	const struct IPCDeviceGpio_Clear clear =
	{
		{
			IPCDeviceGpio,
			IPCDeviceGpio_Clear
		},
		pin
	};

	TB_ASSERT(pin & OUT_BIT);

	sendSyncMessage(reinterpret_cast<const uint8_t*>(&clear), sizeof(clear));
}

void IPCDeviceGpioProxy::toggle(const uint32_t pin)
{
	const struct IPCDeviceGpio_Toggle toggle =
	{
		{
			IPCDeviceGpio,
			IPCDeviceGpio_Toggle
		},
		pin
	};

	TB_ASSERT(pin & OUT_BIT);

	sendSyncMessage(reinterpret_cast<const uint8_t*>(&toggle), sizeof(toggle));
}


