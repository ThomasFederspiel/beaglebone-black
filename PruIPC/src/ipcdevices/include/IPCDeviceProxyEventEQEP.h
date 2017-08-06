/*
 * IPCDeviceProxyEventECAP.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEPROXYEVENTECAP_H_
#define PRUIPC_IPCDEVICEPROXYEVENTECAP_H_

// system
#include "PrussDriver.h"

// standard
#include <memory>

// project
#include "ServiceMessageBase.h"
#include "IPCMessageTypes.h"
#include "tboxdefs.h"

#include "pru_ipc_device_eqep_types.hp"
#include "pru_ipc_devices.hp"
#include "pru_ipc_types.hp"

using namespace pruipcservice;

class IPCDeviceProxyEventEQEP final : public ServiceMessageBase
{
public:

	using pruEvenType_t = struct IPCDeviceEQep_Status;
	static constexpr MessageType_t EventMessageType = IPCMessageTypes::Type::IpcDeviceProxyEventEQEP;
	static constexpr IPCDeviceEnum PruDevice = IPCDeviceEQep;
	static constexpr ioctl_t PruIoctl = IPCDeviceEQep_Status;

	explicit IPCDeviceProxyEventEQEP(const PrussDriver::PruProxy::PruIdEnum pruId,
			const pruEvenType_t& eCapStatus) : ServiceMessageBase(IPCMessageTypes::Type::IpcDeviceProxyEventEQEP),
		m_pruId(pruId), m_capCounter(eCapStatus.capCounter), m_capTime(eCapStatus.capTime), m_capPeriod(eCapStatus.capPeriod),
		m_capStatus(eCapStatus.capStatus), m_counter(eCapStatus.counter)
	{
	}

	explicit IPCDeviceProxyEventEQEP(const IPCDeviceProxyEventEQEP& idpe) : ServiceMessageBase(idpe), m_pruId(idpe.m_pruId),
			m_capCounter(idpe.m_capCounter), m_capTime(idpe.m_capTime), m_capPeriod(idpe.m_capPeriod),
			m_capStatus(idpe.m_capStatus), m_counter(idpe.m_counter)
	{
	}

	explicit IPCDeviceProxyEventEQEP(IPCDeviceProxyEventEQEP&& idpe) : ServiceMessageBase(idpe), m_pruId(idpe.m_pruId),
			m_capCounter(idpe.m_capCounter), m_capTime(idpe.m_capTime), m_capPeriod(idpe.m_capPeriod),
			m_capStatus(idpe.m_capStatus), m_counter(idpe.m_counter)
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
		swap(first.m_pruId, second.m_pruId);
		swap(first.m_capCounter, second.m_capCounter);
		swap(first.m_capTime, second.m_capTime);
		swap(first.m_capPeriod, second.m_capPeriod);
		swap(first.m_capStatus, second.m_capStatus);
		swap(first.m_counter, second.m_counter);
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return tbox::make_unique<IPCDeviceProxyEventEQEP>(*this);
	}

	PrussDriver::PruProxy::PruIdEnum getPruId() const
	{
		return m_pruId;
	}

	uint32_t getCapCounter() const
	{
		return m_capCounter;
	}

	uint32_t getCapTime() const
	{
		return m_capTime;
	}

	uint32_t getCapPeriod() const
	{
		return m_capPeriod;
	}

	uint32_t getCapStatus() const
	{
		return m_capStatus;
	}

	uint32_t getCounter() const
	{
		return m_counter;
	}

private:
	PrussDriver::PruProxy::PruIdEnum m_pruId;
	uint32_t m_capCounter;
	uint32_t m_capTime;
	uint32_t m_capPeriod;
	uint32_t m_capStatus;
	uint32_t m_counter;
};

#endif /* PRUIPC_IPCDEVICEPROXYEVENTEQEP_H_ */
