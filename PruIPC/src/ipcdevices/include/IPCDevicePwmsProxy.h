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

	explicit IPCDevicePwmsProxy(IPCDeviceProxyService& proxy, const PwmssDeviceEnum pwmssDevice);
	explicit IPCDevicePwmsProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy, const PwmssDeviceEnum pwmDevice);

	void open() override;
	void close() override;

private:
	const PwmssDeviceEnum m_pwmssDevice;
};

#endif /* PRUIPC_IPCDEVICECEPWPROXY_H_ */
