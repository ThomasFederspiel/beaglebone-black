/*
 * IIPCDeviceEPwmProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IIPCDEVICEEPWMPROXY_H_
#define PRUIPC_IIPCDEVICEEPWMPROXY_H_

// standard
#include <stdint.h>

// project
#include "pru_epwm_types.hp"

class IIPCDeviceEPwmProxy
{
public:

	class IEPwmChannel
	{
	public:
		virtual void enable() = 0;
		virtual void disable(const bool pinHigh) = 0;
		virtual void setDuty(const uint16_t onTime) = 0;
		virtual void pinLow() = 0;
		virtual void pinHigh() = 0;
	};

	virtual void open() = 0;
	virtual void close() = 0;
	virtual uint16_t armFreq(const uint32_t freq) = 0;
	virtual void armFreq(const uint16_t periodCntr, const uint8_t clkDiv, const uint8_t hspClkDiv) = 0;
	virtual uint16_t setFreq(const uint32_t freq) = 0;
	virtual void setDefaultChannel(const EPwmChannelEnum channel) = 0;
	virtual IEPwmChannel& getDefaultChannel() = 0;
	virtual IEPwmChannel& getChannel(const EPwmChannelEnum channel) = 0;
};

#endif /* PRUIPC_IIPCDEVICECEPWPROXY_H_ */
