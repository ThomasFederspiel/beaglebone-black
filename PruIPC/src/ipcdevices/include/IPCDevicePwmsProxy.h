/*
 * IPCDevicePwmsProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEPWMSPROXY_H_
#define PRUIPC_IPCDEVICEPWMSPROXY_H_

// local
#include "AbstractIPCDeviceProxy.h"
#include "IIPCDevicePwmsProxy.h"
#include "pru_pwms_types.hp"

class IMessageReceiver;
class IPCDeviceProxyService;

class IPCDevicePwmsProxy final : public IIPCDevicePwmsProxy, protected AbstractIPCDeviceProxy
{
public:

	explicit IPCDevicePwmsProxy(IPCDeviceProxyService& proxy, const PwmDeviceEnum pwmDevice);
	explicit IPCDevicePwmsProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy, const PwmDeviceEnum pwmDevice);

	void open() override;
	void close() override;

private:
	const PwmDeviceEnum m_pwmDevice;
};

#endif /* PRUIPC_IPCDEVICECEPWPROXY_H_ */
