/*
 * IPCDeviceTestProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICETESTPROXY_H_
#define PRUIPC_IPCDEVICETESTPROXY_H_

// local
#include "AbstractIPCDeviceProxy.h"

class IMessageReceiver;
class IPCDeviceProxyService;

class IPCDeviceTestProxy final : protected AbstractIPCDeviceProxy
{
public:
	explicit IPCDeviceTestProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy);

	void sendTrig();
};

#endif /* PRUIPC_IPCDEVICETESTPROXY_H_ */
