/*
 * IPCDeviceEPwmProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEEPWMPROXY_H_
#define PRUIPC_IPCDEVICEEPWMPROXY_H_

// standard
#include <stdint.h>

// local
#include "AbstractIPCDeviceProxy.h"
#include "IIPCDeviceEPwmProxy.h"
#include "pru_ipc_device_epwm_types.hp"
#include "pru_epwm_types.hp"
#include "pru_pwms_types.hp"

class IMessageReceiver;
class IPCDeviceProxyService;

class IPCDeviceEPwmProxy final : public IIPCDeviceEPwmProxy, protected AbstractIPCDeviceProxy
{
public:
	friend class EPwmChannel;

	class EPwmChannel final : public IIPCDeviceEPwmProxy::IEPwmChannel
	{
	public:

		explicit EPwmChannel(const EPwmChannelEnum channel,
				IPCDeviceEPwmProxy& epwmProxy);

		void enable() override;
		void disable(const bool pinHigh) override;
		void setDuty(const uint16_t onTime) override;
		void pinLow() override;
		void pinHigh() override;

	private:
		EPwmChannelEnum m_channel;
		IPCDeviceEPwmProxy& m_epwmProxy;
	};

	explicit IPCDeviceEPwmProxy(IPCDeviceProxyService& proxy, const PwmDeviceEnum pwmDevice);
	explicit IPCDeviceEPwmProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy,
			const PwmDeviceEnum pwmDevice, const EPwmChannelEnum channel);
	explicit IPCDeviceEPwmProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy,
			const PwmDeviceEnum pwmDevice);

	void open() override;
	void close() override;
	uint16_t armFreq(const uint32_t freq) override;
	void armFreq(const uint16_t periodCntr, const uint8_t clkDiv, const uint8_t hspClkDiv) override;
	uint16_t setFreq(const uint32_t freq) override;

	void setDefaultChannel(const EPwmChannelEnum channel) override;
	IIPCDeviceEPwmProxy::IEPwmChannel& getDefaultChannel() override;
	IIPCDeviceEPwmProxy::IEPwmChannel& getChannel(const EPwmChannelEnum channel) override;

private:
	bool calcPrescalers(const uint32_t rqstPrescaler,
	                uint16_t& prescaleDiv, uint8_t& clkDivRes, uint8_t& hspClkDivRes);

	PwmDeviceEnum m_pwmDevice;
	EPwmChannel m_channelA;
	EPwmChannel m_channelB;
	IEPwmChannel* m_defaultChannel;
	uint16_t m_currentPrescaleDiv;
};

#endif /* PRUIPC_IPCDEVICECEPWPROXY_H_ */
