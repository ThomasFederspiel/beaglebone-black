/*
 * AbstractIPCDeviceProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_ABSTRACTIPCDEVICEPROXY_H_
#define PRUIPC_ABSTRACTIPCDEVICEPROXY_H_

// standard
#include <stdint.h>

class IMessageReceiver;
class IPCDeviceProxyService;

class AbstractIPCDeviceProxy
{
public:
	explicit AbstractIPCDeviceProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy);

protected:

	void sendSyncMessage(const uint8_t* const message, const uint8_t size);

private:

	IPCDeviceProxyService& m_proxy;
	IMessageReceiver& m_receiver;
};

#endif /* PRUIPC_ABSTRACTIPCDEVICEPROXY_H_ */
