/*
 * IPCDeviceEQepProxy.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "IPCDeviceEQepProxy.h"

// local
#include "IMessageReceiver.h"
#include "IPCDeviceProxyService.h"
#include "pru_ipc_devices.hp"
#include "pru_ipc_device_eqep_types.hp"
#include "SinkMessageReceiver.h"

// project
#include "Logger.h"

MODULE_LOG(IPCDeviceEQepProxy)

namespace
{
	SinkMessageReceiver m_sink;

	static uint16_t evaluateUTimerPeriod(const EQepUnitTimerPeriod utimerPeriod_ms)
	{
		// time in [ms] when 100 MHz system clock
		return utimerPeriod_ms / 100000;
	}

	static uint16_t evaluateUEventPulses(const EQepUpEventDivisor upEventDivisor)
	{
		return 0x01 << upEventDivisor;
	}

	static uint16_t evaluateCaptureTimeTick(const EQepCapClkDivisor clkDiv)
	{
		return 10 * (0x01 << (clkDiv >> 4));
	}
};

IPCDeviceEQepProxy::IPCDeviceEQepProxy(IPCDeviceProxyService& proxy,
		const PwmssDeviceEnum pwmssDevice) : IPCDeviceEQepProxy(m_sink, proxy, pwmssDevice)
{
}

IPCDeviceEQepProxy::IPCDeviceEQepProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy,
		const PwmssDeviceEnum pwmssDevice) : AbstractIPCDeviceProxy(receiver, proxy), m_pwmssDevice(pwmssDevice)
{
}

void IPCDeviceEQepProxy::enableEQepQuadrature(const EQepUnitTimerPeriod utimerPeriod,
		const EQepCapClkDivisor capClkDivisor, const EQepUpEventDivisor upEventDivisor)
{
	struct IPCDeviceEQep_EnableQuadrature enable =
	{
		{
			IPCDeviceEQep,
			IPCDeviceEQep_EnableQuadrature
		},
		m_pwmssDevice,
		utimerPeriod,
		capClkDivisor,
		upEventDivisor,
		evaluateUTimerPeriod(utimerPeriod),
		evaluateUEventPulses(upEventDivisor),
		evaluateCaptureTimeTick(capClkDivisor)
	};

	sendSyncMessage(reinterpret_cast<uint8_t*>(&enable), sizeof(enable));
}

void IPCDeviceEQepProxy::disableEQep()
{
	struct IPCDeviceEQep_disable disable =
	{
		{
			IPCDeviceEQep,
			IPCDeviceEQep_disable
		},
		m_pwmssDevice
	};

	sendSyncMessage(reinterpret_cast<uint8_t*>(&disable), sizeof(disable));
}
