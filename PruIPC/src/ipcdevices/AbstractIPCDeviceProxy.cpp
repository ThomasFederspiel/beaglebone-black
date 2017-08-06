/*
 * AbstractIPCDeviceProxy.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "AbstractIPCDeviceProxy.h"

// local
#include "IMessageReceiver.h"
#include "IPCDeviceProxyMessage.h"
#include "IPCDeviceProxyService.h"

AbstractIPCDeviceProxy::AbstractIPCDeviceProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy) : m_proxy(proxy),
	m_receiver(receiver)
{
}

void AbstractIPCDeviceProxy::sendSyncMessage(const uint8_t* message, const uint8_t size)
{
	const IPCDeviceProxyMessage proxyMessage(m_receiver, message, size);

	m_proxy.post(proxyMessage);
}

