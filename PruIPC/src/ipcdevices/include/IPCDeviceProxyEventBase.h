/*
 * IPCDeviceProxyEventBase.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEPROXYEVENTBASE_H_
#define PRUIPC_IPCDEVICEPROXYEVENTBASE_H_

// system
#include "PrussDriver.h"

// project
#include "ServiceMessageBase.h"
#include "IPCMessageTypes.h"
#include "tboxdefs.h"

using namespace pruipcservice;

class IPCDeviceProxyEventBase : public ServiceMessageBase
{
public:
	IPCDeviceProxyEventBase(const MessageType_t eventMessageType) : ServiceMessageBase(eventMessageType),
		m_pruId(PrussDriver::PruProxy::PruIdEnum::PruIdEOE)
	{
	}

	explicit IPCDeviceProxyEventBase(const MessageType_t eventMessageType, const PrussDriver::PruProxy::PruIdEnum pruId) :
			ServiceMessageBase(eventMessageType), m_pruId(pruId)
	{
	}

	virtual ~IPCDeviceProxyEventBase() = default;

	IPCDeviceProxyEventBase& operator=(IPCDeviceProxyEventBase idpe)
	{
		swap(*this, idpe);

		return *this;
	}

	friend void swap(IPCDeviceProxyEventBase& first, IPCDeviceProxyEventBase& second)
	{
		using std::swap;
		swap(first.m_pruId, second.m_pruId);
	}

	PrussDriver::PruProxy::PruIdEnum getPruId() const
	{
		return m_pruId;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return tbox::make_unique<IPCDeviceProxyEventBase>(*this);
	}

private:
	PrussDriver::PruProxy::PruIdEnum m_pruId;
};

#endif /* PRUIPC_IPCDEVICEPROXYEVENTBASE_H_ */
