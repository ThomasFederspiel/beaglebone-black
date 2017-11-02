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

/**
 * DRV8833 channels, xIN1 is controlled by channel A and xIN2 by channel B
 *
 * 	xIN1 (A)		xIN2 (B)		Function
 * 	-------			-------			---------------
 *	0				0				Coast/Fast decay
 *	0				1				Reverse
 *	1				0				Forward
 *	1 				1				Brake/Slow decay
 *	PWM				0				Forward PWM, Fast decay
 *	1				PWM				Forward PWM, Slow decay
 * 	0				PWM				Reverse PWM, Fast decay
 * 	PWM				1				Reverse PWM, Slow decay
 *
 */
namespace
{
	static constexpr uint16_t PWMFreq = 25000;
	static constexpr float StallLevel = 0.62f;
}

MotorDriver8833::Motor::Motor(IPCDeviceProxyService& proxy, const PwmssDeviceEnum pwmssDevice) :
		m_epwmProxy(proxy, pwmssDevice), m_pwmsProxy(proxy, pwmssDevice),
		m_state(Motor::State::Stopped), m_pwmPeriod(0), m_pwmDuty(0), m_pwmEnabled(false),
		m_decay(Motor::Decay::Fast), m_directionChannel(PWM_CH_B), m_pwmChannel(PWM_CH_A)
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

	if (absSpeed < 1e-5f)
	{
		stop();

		return;
	}

	const uint16_t pwmDuty = static_cast<uint16_t>(m_pwmPeriod * ((1.f - StallLevel) * absSpeed / 100.f + StallLevel));

	apply(forward, pwmDuty);
}

void MotorDriver8833::Motor::setDecay(const Motor::Decay decay)
{
	TB_ASSERT((m_state == State::Stopped) || (m_state == State::Coasting));

	m_decay = decay;
}

void MotorDriver8833::Motor::stop()
{
	// ;+
	INFO("Stop");

	if (m_pwmEnabled)
	{
		const bool pinHigh = true;
		m_epwmProxy.getChannel(m_pwmChannel).disable(pinHigh);
		m_pwmEnabled = false;
	}
	else
	{
		m_epwmProxy.getChannel(m_pwmChannel).pinHigh();
	}

	m_epwmProxy.getChannel(m_directionChannel).pinHigh();

	m_state = State::Stopped;
}

void MotorDriver8833::Motor::coast()
{
	// ;+
	INFO("Coast");

	if (m_pwmEnabled)
	{
		const bool pinHigh = false;
		m_epwmProxy.getChannel(m_pwmChannel).disable(pinHigh);
		m_pwmEnabled = false;
	}
	else
	{
		m_epwmProxy.getChannel(m_pwmChannel).pinLow();
	}

	m_epwmProxy.getChannel(m_directionChannel).pinLow();

	m_state = State::Coasting;
}

void MotorDriver8833::Motor::apply(const bool forward, const uint16_t duty)
{
	switch (m_state)
	{
	case State::Forward:
		if (!forward)
		{
			coast();

			selectChannels(forward);

			m_state = State::Reverse;
		}
		break;

	case State::Reverse:
		if (forward)
		{
			coast();

			selectChannels(forward);

			m_state = State::Forward;
		}
		break;

	case State::Stopped:
	case State::Coasting:
		selectChannels(forward);

		if (forward)
		{
			m_state = State::Forward;
		}
		else
		{
			m_state = State::Reverse;
		}

		break;

	TB_DEFAULT(toString(m_state));
	}

	m_pwmDuty = duty;

	// ;+
	INFO("duty = " << m_pwmDuty);

	if (m_decay == Decay::Slow)
	{
		m_epwmProxy.getChannel(m_directionChannel).pinHigh();
	}
	else
	{
		m_epwmProxy.getChannel(m_directionChannel).pinLow();
	}

	if (!m_pwmEnabled)
	{
		m_epwmProxy.getChannel(m_pwmChannel).enable();
		m_pwmEnabled = true;
	}

	m_epwmProxy.getChannel(m_pwmChannel).setDuty(m_pwmDuty);
}

void MotorDriver8833::Motor::selectChannels(const bool forward)
{
	if (forward)
	{
		// ;+
		INFO("Forward");

		if (m_decay == Decay::Slow)
		{
			m_directionChannel = PWM_CH_B;
			m_pwmChannel = PWM_CH_A;
		}
		else
		{
			m_directionChannel = PWM_CH_A;
			m_pwmChannel = PWM_CH_B;
		}
	}
	else
	{
		// ;+
		INFO("Reverse");

		if (m_decay == Decay::Slow)
		{
			m_directionChannel = PWM_CH_A;
			m_pwmChannel = PWM_CH_B;
		}
		else
		{
			m_directionChannel = PWM_CH_B;
			m_pwmChannel = PWM_CH_A;
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


