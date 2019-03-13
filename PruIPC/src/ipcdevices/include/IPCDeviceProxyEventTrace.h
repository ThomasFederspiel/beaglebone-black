/*
 * IPCDeviceProxyEventTrace.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEPROXYEVENTTRACE_H_
#define PRUIPC_IPCDEVICEPROXYEVENTTRACE_H_

// system
#include "PrussDriver.h"

// standard
#include <memory>
#include <cstring>
#include <type_traits>

// project
#include "ServiceMessageBase.h"
#include "IPCMessageTypes.h"
#include "stdExtension.h"

// local
#include "IPCDeviceProxyEventBase.h"

#include "pru_ipc_device_trace_types.hp"
#include "pru_ipc_devices.hp"
#include "pru_ipc_types.hp"
#include "pru_ipc_types.hp"
#include "pru_pwms_types.hp"
#include "pru_eqep_types.hp"

using namespace pruipcservice;

class IPCDeviceProxyEventTrace final : public IPCDeviceProxyEventBase
{
public:

	using pruEvenType_t = struct IPCDeviceTrace_Status;
	static constexpr MessageType_t EventMessageType = IPCMessageTypes::Type::IPCDeviceProxyEventTrace;
	static constexpr IPCDeviceEnum PruDevice = IPCDeviceTrace;
	static constexpr ioctl_t PruIoctl = IPCDeviceTrace_Status;

	using TraceDataType = std::remove_reference<decltype(*IPCDeviceTrace_Status::traceData)>::type;
	static constexpr size_t TraceDataSize = sizeof(IPCDeviceTrace_Status::traceData) / sizeof(TraceDataType);

	IPCDeviceProxyEventTrace() : IPCDeviceProxyEventBase(EventMessageType), m_traceData()
	{
	}

	explicit IPCDeviceProxyEventTrace(const PrussDriver::PruProxy::PruIdEnum pruId,
		const pruEvenType_t& traceStatus) : IPCDeviceProxyEventBase(EventMessageType, pruId), m_traceData()
	{
		static_assert(sizeof(m_traceData) == sizeof(IPCDeviceTrace_Status::traceData), "Different sizes");

		std::memcpy(m_traceData, traceStatus.traceData, sizeof(m_traceData));
	}

	IPCDeviceProxyEventTrace& operator=(IPCDeviceProxyEventTrace idpe)
	{
		swap(*this, idpe);

		return *this;
	}

	friend void swap(IPCDeviceProxyEventTrace& first, IPCDeviceProxyEventTrace& second)
	{
		swap(static_cast<IPCDeviceProxyEventBase&>(first), static_cast<IPCDeviceProxyEventBase&>(second));

		using std::swap;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return std::make_unique<IPCDeviceProxyEventTrace>(*this);
	}

	const TraceDataType* getTrace() const
	{
		return m_traceData;
	}

	size_t size() const
	{
		return TraceDataSize;
	}

private:
	TraceDataType m_traceData[TraceDataSize];
};

#endif /* PRUIPC_IPCDEVICEPROXYEVENTTRACE_H_ */
