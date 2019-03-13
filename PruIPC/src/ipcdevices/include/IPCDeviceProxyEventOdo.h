/*
 * IPCDeviceProxyEventOdo.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEPROXYEVENTODO_H_
#define PRUIPC_IPCDEVICEPROXYEVENTODO_H_

// system
#include "PrussDriver.h"

// standard
#include <limits>
#include <memory>

// project
#include "ServiceMessageBase.h"
#include "IPCMessageTypes.h"
#include "stdExtension.h"

// local
#include "IPCDeviceProxyEventBase.h"

#include "pru_ipc_device_eqep_types.hp"
#include "pru_ipc_devices.hp"
#include "pru_ipc_types.hp"
#include "pru_pwms_types.hp"
#include "pru_eqep_types.hp"

using namespace pruipcservice;

class IPCDeviceProxyEventOdo final : public IPCDeviceProxyEventBase
{
public:

	using pruEvenType_t = struct IPCDeviceEQep_Odo;
	static constexpr MessageType_t EventMessageType = IPCMessageTypes::Type::IPCDeviceProxyEventOdo;
	static constexpr IPCDeviceEnum PruDevice = IPCDeviceEQep;
	static constexpr ioctl_t PruIoctl = IPCDeviceEQep_Odo;

	IPCDeviceProxyEventOdo() : IPCDeviceProxyEventBase(EventMessageType, IPCDeviceProxyEventBase::PeriodicMessage),
		m_pruTime_ns(0),
		m_pwmssDevice_1(PwmssDeviceEnum::LAST_PWM_DEV), m_counter_1(0),
		m_pwmssDevice_2(PwmssDeviceEnum::LAST_PWM_DEV), m_counter_2(0)
	{
	}

	explicit IPCDeviceProxyEventOdo(const PrussDriver::PruProxy::PruIdEnum pruId,
			const pruEvenType_t& eQepOdo) : IPCDeviceProxyEventBase(EventMessageType, IPCDeviceProxyEventBase::PeriodicMessage, pruId),
		m_pruTime_ns(evalPruClock(eQepOdo.pruClock)),
		m_pwmssDevice_1(static_cast<decltype(m_pwmssDevice_1)>(eQepOdo.pwmssDevice_1)),
		m_counter_1(eQepOdo.counter_1),
		m_pwmssDevice_2(static_cast<decltype(m_pwmssDevice_2)>(eQepOdo.pwmssDevice_2)),
		m_counter_2(eQepOdo.counter_2)
	{
	}

	IPCDeviceProxyEventOdo& operator=(IPCDeviceProxyEventOdo idpe)
	{
		swap(*this, idpe);

		return *this;
	}

	friend void swap(IPCDeviceProxyEventOdo& first, IPCDeviceProxyEventOdo& second)
	{
		using std::swap;
		swap(static_cast<IPCDeviceProxyEventBase&>(first), static_cast<IPCDeviceProxyEventBase&>(second));

		swap(first.m_pruTime_ns, second.m_pruTime_ns);
		swap(first.m_pwmssDevice_1, second.m_pwmssDevice_1);
		swap(first.m_counter_1, second.m_counter_1);
		swap(first.m_pwmssDevice_2, second.m_pwmssDevice_2);
		swap(first.m_counter_2, second.m_counter_2);
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return std::make_unique<IPCDeviceProxyEventOdo>(*this);
	}

	uint64_t getPruTime() const
	{
		return m_pruTime_ns;
	}

	PwmssDeviceEnum getPwmssDevice_1() const
	{
		return m_pwmssDevice_1;
	}

	uint32_t getCounter_1() const
	{
		return m_counter_1;
	}

	PwmssDeviceEnum getPwmssDevice_2() const
	{
		return m_pwmssDevice_2;
	}

	uint32_t getCounter_2() const
	{
		return m_counter_2;
	}

private:
	uint64_t m_pruTime_ns;
	PwmssDeviceEnum m_pwmssDevice_1;
	uint32_t m_counter_1;
	PwmssDeviceEnum m_pwmssDevice_2;
	uint32_t m_counter_2;
};

#endif /* PRUIPC_IPCDEVICEPROXYEVENTODO_H_ */
