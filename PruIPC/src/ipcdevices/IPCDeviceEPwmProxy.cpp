/*
 * IPCDeviceEPwmProxy.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "IPCDeviceEPwmProxy.h"

// local
#include "IMessageReceiver.h"
#include "IPCDeviceProxyService.h"
#include "pru_ipc_devices.hp"
#include "SinkMessageReceiver.h"

// project
#include "Logger.h"

MODULE_LOG(IPCDeviceEPwmProxy)

#define EPWM_PER_CNT_HZ(hz) 100000000 / hz

namespace
{
	SinkMessageReceiver m_sink;
};

IPCDeviceEPwmProxy::EPwmChannel::EPwmChannel(const EPwmChannelEnum channel,
		IPCDeviceEPwmProxy& epwmProxy) : m_channel(channel), m_epwmProxy(epwmProxy)
{
}

void IPCDeviceEPwmProxy::EPwmChannel::enable()
{
	const struct IPCDeviceEPwm_Enable enable =
	{
		{
			IPCDeviceEPwm,
			IPCDeviceEPwm_Enable
		},
		m_epwmProxy.m_pwmDevice,
		m_channel
	};

	m_epwmProxy.sendSyncMessage(reinterpret_cast<const uint8_t*>(&enable), sizeof(enable));
}

void IPCDeviceEPwmProxy::EPwmChannel::disable(const bool pinHigh)
{
	const struct IPCDeviceEPwm_Disable disable =
	{
		{
			IPCDeviceEPwm,
			IPCDeviceEPwm_Disable
		},
		m_epwmProxy.m_pwmDevice,
		m_channel,
		pinHigh
	};

	m_epwmProxy.sendSyncMessage(reinterpret_cast<const uint8_t*>(&disable), sizeof(disable));
}

void IPCDeviceEPwmProxy::EPwmChannel::setDuty(const uint16_t onTime)
{
	const struct IPCDeviceEPwm_SetDuty setDuty =
	{
		{
			IPCDeviceEPwm,
			IPCDeviceEPwm_SetDuty
		},
		m_epwmProxy.m_pwmDevice,
		m_channel,
		onTime
	};

	m_epwmProxy.sendSyncMessage(reinterpret_cast<const uint8_t*>(&setDuty), sizeof(setDuty));
}

void IPCDeviceEPwmProxy::EPwmChannel::pinLow()
{
	const bool high = false;
	const struct IPCDeviceEPwm_SetPin setPin =
	{
		{
			IPCDeviceEPwm,
			IPCDeviceEPwm_SetPin
		},
		m_epwmProxy.m_pwmDevice,
		m_channel,
		high
	};

	m_epwmProxy.sendSyncMessage(reinterpret_cast<const uint8_t*>(&setPin), sizeof(setPin));
}

void IPCDeviceEPwmProxy::EPwmChannel::pinHigh()
{
	const bool high = true;
	const struct IPCDeviceEPwm_SetPin setPin =
	{
		{
			IPCDeviceEPwm,
			IPCDeviceEPwm_SetPin
		},
		m_epwmProxy.m_pwmDevice,
		m_channel,
		high
	};

	m_epwmProxy.sendSyncMessage(reinterpret_cast<const uint8_t*>(&setPin), sizeof(setPin));
}

IPCDeviceEPwmProxy::IPCDeviceEPwmProxy(IPCDeviceProxyService& proxy,
		const PwmDeviceEnum pwmDevice) : IPCDeviceEPwmProxy(m_sink, proxy,
				pwmDevice)
{
}

IPCDeviceEPwmProxy::IPCDeviceEPwmProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy,
		const PwmDeviceEnum pwmDevice) : IPCDeviceEPwmProxy(m_sink, proxy, pwmDevice, PWM_CH_A)
{
}

IPCDeviceEPwmProxy::IPCDeviceEPwmProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy,
		const PwmDeviceEnum pwmDevice, const EPwmChannelEnum channel) : AbstractIPCDeviceProxy(receiver, proxy),
	m_pwmDevice(pwmDevice), m_channelA(PWM_CH_A, *this), m_channelB(PWM_CH_B, *this), m_defaultChannel(nullptr),
	m_currentPrescaleDiv(0)
{
	setDefaultChannel(channel);
}

void IPCDeviceEPwmProxy::setDefaultChannel(const EPwmChannelEnum channel)
{
	m_defaultChannel = &getChannel(channel);
}

IPCDeviceEPwmProxy::IEPwmChannel& IPCDeviceEPwmProxy::getDefaultChannel()
{
	return *m_defaultChannel;
}

IPCDeviceEPwmProxy::IEPwmChannel& IPCDeviceEPwmProxy::getChannel(const EPwmChannelEnum channel)
{
	if (channel == PWM_CH_A)
	{
		return m_channelA;
	}

	return m_channelB;
}

void IPCDeviceEPwmProxy::open()
{
	const struct IPCDeviceEPwm_Open open =
	{
		{
			IPCDeviceEPwm,
			IPCDeviceEPwm_Open
		},
		m_pwmDevice
	};

	sendSyncMessage(reinterpret_cast<const uint8_t*>(&open), sizeof(open));
}

void IPCDeviceEPwmProxy::close()
{
	const struct IPCDeviceEPwm_Close close =
	{
		{
			IPCDeviceEPwm,
			IPCDeviceEPwm_Close
		},
		m_pwmDevice
	};

	sendSyncMessage(reinterpret_cast<const uint8_t*>(&close), sizeof(close));

	m_currentPrescaleDiv = 0;
}

uint16_t IPCDeviceEPwmProxy::setFreq(const uint32_t freq)
{
	if (m_currentPrescaleDiv)
	{
		const uint16_t periodCntr = EPWM_PER_CNT_HZ(freq) / m_currentPrescaleDiv;

		const struct IPCDeviceEPwm_SetFreq setFreq =
		{
			{
				IPCDeviceEPwm,
				IPCDeviceEPwm_SetFreq
			},
			m_pwmDevice,
			periodCntr
		};

		INFO("periodCntr = " << periodCntr);

		sendSyncMessage(reinterpret_cast<const uint8_t*>(&setFreq), sizeof(setFreq));

		return periodCntr;
	}

	return 0;
}

uint16_t IPCDeviceEPwmProxy::armFreq(const uint32_t freq)
{
	m_currentPrescaleDiv = 0;
	uint8_t clkDiv = 0;
	uint8_t hspClkDiv = 0;

	const uint16_t rqstPrescaleDiv = EPWM_PER_CNT_HZ(freq) / 0xFFFF;

	if (calcPrescalers(rqstPrescaleDiv,
			m_currentPrescaleDiv, clkDiv, hspClkDiv))
	{
		const uint16_t periodCntr = EPWM_PER_CNT_HZ(freq) / m_currentPrescaleDiv;

		INFO("periodCntr = " << periodCntr);
		INFO("periodCclkDivntr = " << static_cast<uint16_t>(clkDiv));
		INFO("hspClkDiv = " << static_cast<uint16_t>(hspClkDiv));

		armFreq(periodCntr, clkDiv, hspClkDiv);

		return periodCntr;
	}

	return 0;
}

void IPCDeviceEPwmProxy::armFreq(const uint16_t periodCntr, const uint8_t clkDiv, const uint8_t hspClkDiv)
{
	const struct IPCDeviceEPwm_ArmRaw armRaw =
	{
		{
			IPCDeviceEPwm,
			IPCDeviceEPwm_ArmRaw
		},
		m_pwmDevice,
		periodCntr,
		clkDiv,
		hspClkDiv
	};

	sendSyncMessage(reinterpret_cast<const uint8_t*>(&armRaw), sizeof(armRaw));
}

bool IPCDeviceEPwmProxy::calcPrescalers(const uint32_t rqstPrescaler,
                uint16_t& prescaleDiv, uint8_t& clkDivRes, uint8_t& hspClkDivRes)
{
	 for (uint8_t clkdiv = 0; clkdiv <= 7; ++clkdiv)
	 {
		 for (uint8_t hspclkdiv = 0; hspclkdiv <= 7; ++hspclkdiv)
		 {
			/*
			 * calculations for prescaler value :
			 * prescaleDiv = HSPCLKDIVIDER * CLKDIVIDER.
			 * CLKDIVIDER = 2 ** clkdiv
			 * HSPCLKDIVIDER = (1), if hspclkdiv == 0 *OR*
			 *              (2 * hspclkdiv), if hspclkdiv != 0
			 *
			 * Configure prescale_div value such that period
			 * register value is less than 65535.
			 */

			if (hspclkdiv)
			{
				prescaleDiv = (1 << clkdiv) * (hspclkdiv << 1);
			}
			else
			{
				prescaleDiv = (1 << clkdiv);
			}

			if (prescaleDiv >= rqstPrescaler)
			{
				clkDivRes = clkdiv;
				hspClkDivRes = hspclkdiv;

				return true;
			}
		}
	}

	return false;
}

