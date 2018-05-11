/*
 * IPCDeviceEQepProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEEQEPPROXY_H_
#define PRUIPC_IPCDEVICEEQEPPROXY_H_

// local
#include "AbstractIPCDeviceProxy.h"
#include "IIPCDeviceEQepProxy.h"
#include "pru_eqep_types.hp"
#include "pru_pwms_types.hp"

class IMessageReceiver;
class IPCDeviceProxyService;

class IPCDeviceEQepProxy final : public IIPCDeviceEQepProxy, protected AbstractIPCDeviceProxy
{
public:

	explicit IPCDeviceEQepProxy(IPCDeviceProxyService& proxy, const PwmssDeviceEnum pwmssDevice);
	explicit IPCDeviceEQepProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy, const PwmssDeviceEnum pwmDevice);

	void open() override;
	void close() override;
	void enableEQepQuadrature(const EQepUnitTimerPeriod utimerPeriod,
			const EQepCapClkDivisor capClkDivisor, const EQepUpEventDivisor upEventDivisor,
			const EQepCounterModeEnum mode) override;
	void disableEQep() override;

private:
	const PwmssDeviceEnum m_pwmssDevice;
};

#endif /* PRUIPC_IPCDEVICEEQEPPROXY_H_ */
