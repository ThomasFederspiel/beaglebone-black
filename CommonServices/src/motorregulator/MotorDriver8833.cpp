/*
 * MotorDriver8833.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "MotorDriver8833.h"

// standard
#include <algorithm>
#include <cmath>

// project
#include "exceptionMacros.h"
#include "IPCDeviceProxyService.h"
#include "Logger.h"
#include "tboxdefs.h"

MODULE_LOG(MotorDriver8833);

namespace
{
	static constexpr uint16_t PWMFreq = 25000;
	static constexpr float StallLevel = 0.62f;
}

MotorDriver8833::Motor::Motor(IPCDeviceProxyService& proxy, const PwmssDeviceEnum pwmssDevice) :
		m_epwmProxy(proxy, pwmssDevice), m_pwmsProxy(proxy, pwmssDevice), m_forward(false), m_pwmEnabled(false)
{
}

void MotorDriver8833::Motor::open()
{
	stop();

	m_pwmsProxy.open();
	m_epwmProxy.open();

	m_pwmPeriod = m_epwmProxy.armFreq(PWMFreq);
}

void MotorDriver8833::Motor::close()
{
	stop();

	m_pwmsProxy.close();
	m_epwmProxy.close();
}

void MotorDriver8833::Motor::setSpeed(const float speed)
{
	const bool forward = speed > 0.f;
	const float absSpeed = std::min(std::fabs(speed), 100.f);

	if (absSpeed == 0.f)
	{
		stop();

		return;
	}

	setDirection(forward);

	const uint16_t duty = static_cast<uint16_t>(m_pwmPeriod * ((1.f - StallLevel) * absSpeed / 100.f + StallLevel));

	if (!m_pwmEnabled)
	{
		m_epwmProxy.getChannel(PWM_CH_A).enable();
		m_pwmEnabled = true;
	}

	// ;+
	INFO("duty = " << duty);

	m_epwmProxy.getChannel(PWM_CH_A).setDuty(duty);
}

float MotorDriver8833::Motor::getSpeed() const
{
	return 0.f;
}

void MotorDriver8833::Motor::stop()
{
	if (m_pwmEnabled)
	{
		m_epwmProxy.getChannel(PWM_CH_A).disable(true);
		m_pwmEnabled = false;
	}
	else
	{
		m_epwmProxy.getChannel(PWM_CH_A).pinHigh();
	}

	m_epwmProxy.getChannel(PWM_CH_B).pinHigh();

	m_forward = false;
}

void MotorDriver8833::Motor::coast()
{
	if (m_pwmEnabled)
	{
		m_epwmProxy.getChannel(PWM_CH_A).disable(false);
		m_pwmEnabled = false;
	}
	else
	{
		m_epwmProxy.getChannel(PWM_CH_A).pinLow();
	}

	m_epwmProxy.getChannel(PWM_CH_B).pinLow();

	m_forward = true;
}

void MotorDriver8833::Motor::setDirection(const bool forward)
{
	if (forward)
	{
		if (!m_forward)
		{
			m_epwmProxy.getChannel(PWM_CH_B).pinLow();
			m_forward = true;
		}
	}
	else
	{
		if (m_forward)
		{
			m_epwmProxy.getChannel(PWM_CH_B).pinHigh();
			m_forward = false;
		}
	}
}

MotorDriver8833::MotorDriver8833(IPCDeviceProxyService& proxy,
		const PwmssDeviceEnum leftMotorPwmDevice, const PwmssDeviceEnum rightMotorPwmDevice,
		const IPCDeviceGpioProxy::GpioPins nSleep, const IPCDeviceGpioProxy::GpioPins nFault) :
				m_leftMotor(proxy, leftMotorPwmDevice), m_rightMotor(proxy, rightMotorPwmDevice),
				m_gpioProxy(proxy), m_nSleep(nSleep), m_nFault(nFault)
{
}

void MotorDriver8833::setSleepMode(const bool enable)
{
	if (enable)
	{
		m_gpioProxy.clear(m_nSleep);
	}
	else
	{
		m_gpioProxy.set(m_nSleep);
	}
}

bool MotorDriver8833::isFaulty() const
{
	// return !m_gpioProxy.isHigh();
	return false;
}


