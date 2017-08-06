/*
 * IPCDeviceCoreProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICECOREPROXY_H_
#define PRUIPC_IPCDEVICECOREPROXY_H_

// local
#include "AbstractIPCDeviceProxy.h"

class IMessageReceiver;
class IPCDeviceProxyService;

class IPCDeviceCoreProxy final : protected AbstractIPCDeviceProxy
{
public:

	explicit IPCDeviceCoreProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy);

	void sendTerminate();
};

#endif /* PRUIPC_IPCDEVICECOREPROXY_H_ */
