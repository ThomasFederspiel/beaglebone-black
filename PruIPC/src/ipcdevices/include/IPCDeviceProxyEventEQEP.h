/*
 * IPCDeviceProxyEventEQEP.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEPROXYEVENTEQEP_H_
#define PRUIPC_IPCDEVICEPROXYEVENTEQEP_H_

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

class IPCDeviceProxyEventEQEP final : public IPCDeviceProxyEventBase
{
public:

	using pruEvenType_t = struct IPCDeviceEQep_Status;
	static constexpr MessageType_t EventMessageType = IPCMessageTypes::Type::IPCDeviceProxyEventEQEP;
	static constexpr IPCDeviceEnum PruDevice = IPCDeviceEQep;
	static constexpr ioctl_t PruIoctl = IPCDeviceEQep_Status;

	IPCDeviceProxyEventEQEP() : IPCDeviceProxyEventBase(EventMessageType, IPCDeviceProxyEventBase::PeriodicMessage),
	    m_pruTime_ns(0), m_pwmssDevice(PwmssDeviceEnum::LAST_PWM_DEV), m_capCounter(0), m_capTime(0),
	    m_capPeriod(0), m_capStatus(0), m_intrStatus(0), m_counter(0), m_unitTime_ms(0), m_ueventPulses(0),
		m_captureTimeTick_ns(0)
	{
	}

	explicit IPCDeviceProxyEventEQEP(const PrussDriver::PruProxy::PruIdEnum pruId,
			const pruEvenType_t& eQepStatus) : IPCDeviceProxyEventBase(EventMessageType, IPCDeviceProxyEventBase::PeriodicMessage, pruId),
		m_pruTime_ns(evalPruClock(eQepStatus.pruClock)),
		m_pwmssDevice(static_cast<decltype(m_pwmssDevice)>(eQepStatus.pwmssDevice)),
		m_capCounter(eQepStatus.capCounter), m_capTime(eQepStatus.capTime), m_capPeriod(eQepStatus.capPeriod),
		m_capStatus(eQepStatus.capStatus), m_intrStatus(eQepStatus.intrStatus), m_counter(eQepStatus.counter),
		m_unitTime_ms(eQepStatus.unitTime_ms), m_ueventPulses(eQepStatus.ueventPulses),
		m_captureTimeTick_ns(eQepStatus.captureTimeTick_ns)
	{
	}

	IPCDeviceProxyEventEQEP& operator=(IPCDeviceProxyEventEQEP idpe)
	{
		swap(*this, idpe);

		return *this;
	}

	friend void swap(IPCDeviceProxyEventEQEP& first, IPCDeviceProxyEventEQEP& second)
	{
		using std::swap;
		swap(static_cast<IPCDeviceProxyEventBase&>(first), static_cast<IPCDeviceProxyEventBase&>(second));

		swap(first.m_pruTime_ns, second.m_pruTime_ns);
		swap(first.m_pwmssDevice, second.m_pwmssDevice);
		swap(first.m_capCounter, second.m_capCounter);
		swap(first.m_capTime, second.m_capTime);
		swap(first.m_capPeriod, second.m_capPeriod);
		swap(first.m_capStatus, second.m_capStatus);
		swap(first.m_intrStatus, second.m_intrStatus);
		swap(first.m_counter, second.m_counter);
		swap(first.m_unitTime_ms, second.m_unitTime_ms);
		swap(first.m_ueventPulses, second.m_ueventPulses);
		swap(first.m_captureTimeTick_ns, second.m_captureTimeTick_ns);
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return std::make_unique<IPCDeviceProxyEventEQEP>(*this);
	}

	PwmssDeviceEnum getPwmssDevice() const
	{
		return m_pwmssDevice;
	}

	uint64_t getPruTime() const
	{
		return m_pruTime_ns;
	}

	uint32_t getCapCounter() const
	{
		return m_capCounter;
	}

	//
	uint32_t getCapTime() const
	{
		return m_capTime;
	}

	// Tick time in [ns], see EQepCapClkDivisor in pru_eqep_types.hp
	uint32_t getCapPeriod() const
	{
		return m_capPeriod;
	}

	uint32_t getCapStatus() const
	{
		return m_capStatus;
	}

	uint32_t getIntrStatus() const
	{
		return m_intrStatus;
	}

	uint32_t getCounter() const
	{
		return m_counter;
	}

	uint16_t getUnitTime() const
	{
		return m_unitTime_ms;
	}

	uint16_t getUEventPulses() const
	{
		return m_ueventPulses;
	}

	uint16_t getCaptureTimeTick() const
	{
		return m_captureTimeTick_ns;
	}

	bool isLowSpeedValid() const
	{
		return (m_capStatus & (EQEP_STS_CAPTURE_DIR_ERROR | EQEP_STS_CAPTURE_OVERFLOW_ERROR
				| EQEP_STS_UNITEVENT_POSITION_FLAG)) == EQEP_STS_UNITEVENT_POSITION_FLAG;
	}

private:
	uint64_t m_pruTime_ns;
	PwmssDeviceEnum m_pwmssDevice;
	uint32_t m_capCounter;
	uint32_t m_capTime;
	uint32_t m_capPeriod;
	uint32_t m_capStatus;
	uint32_t m_intrStatus;
	uint32_t m_counter;
	uint16_t m_unitTime_ms;
	uint16_t m_ueventPulses;
	uint16_t m_captureTimeTick_ns;
};

#endif /* PRUIPC_IPCDEVICEPROXYEVENTEQEP_H_ */
