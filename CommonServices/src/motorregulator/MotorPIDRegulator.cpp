/*
 * MotorPIDRegulator.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "MotorPIDRegulator.h"

// standard

// local
#include "IMotor.h"

// project
#include "exceptionMacros.h"
#include "IPCDeviceProxyEventEQEP.h"
#include "Logger.h"
#include "tboxdefs.h"

MODULE_LOG(MotorPIDRegulator);

namespace
{
	static constexpr std::chrono::milliseconds UpdateIntervallMs(10);
}

MotorPIDRegulator::MotorPIDRegulator(IMotor& motorLeftDriver, IMotor& motorRightDriver)
	: m_motorLeftDriver(motorLeftDriver), m_motorRightDriver(motorRightDriver), m_enabled(false),
	  m_leftPidRegulator(UpdateIntervallMs, 0.1f), m_rightPidRegulator(UpdateIntervallMs, 0.1f),
	  m_lastLeftRpm(0.f), m_lastRightRpm(0.f), m_timeTracker()
{
	m_leftPidRegulator.setOutputLimits(0, m_motorLeftDriver.getRPMSignalMax());
	m_rightPidRegulator.setOutputLimits(0, m_motorRightDriver.getRPMSignalMax());
}

bool MotorPIDRegulator::isEnabled() const
{
	return m_enabled;
}

void MotorPIDRegulator::enable(const bool enable)
{
	const bool modified = m_enabled != enable;

	m_enabled = enable;

	m_leftPidRegulator.setEnable(enable);
	m_rightPidRegulator.setEnable(enable);

	if (modified && !m_enabled)
	{
		m_motorLeftDriver.stop();
		m_motorRightDriver.stop();
	}
}

const PidRegulator_t::PIDState& MotorPIDRegulator::getLeftPidState() const
{
	return m_leftPidRegulator.getState();
}

void MotorPIDRegulator::setLeftTuning(const float kp, const float ki, const float kd)
{
	m_leftPidRegulator.setTuning(kp, ki, kd);
}

void MotorPIDRegulator::setLeftSpeed(const float speed)
{
	// ;+
	INFO("setLeftSpeed = " << speed);

	m_leftPidRegulator.setPoint(speed);
}

const PidRegulator_t::PIDState& MotorPIDRegulator::getRightPidState() const
{
	return m_rightPidRegulator.getState();
}

void MotorPIDRegulator::setRightTuning(const float kp, const float ki, const float kd)
{
	// ;+
	INFO("kp = " << kp);
	INFO("ki = " << ki);
	INFO("kd = " << kd);

	m_rightPidRegulator.setTuning(kp, ki, kd);
}

void MotorPIDRegulator::setRightSpeed(const float speed)
{
	// ;+
	INFO("setRightSpeed = " << speed);

	m_rightPidRegulator.setPoint(speed);
}

void MotorPIDRegulator::updateLeft()
{
	if (m_enabled)
	{
		bool overrun = false;
		const float rpm = m_leftPidRegulator.update(m_motorLeftDriver.getSpeedRPM(), overrun);

//		INFO("in rpm [left] = " << m_motorLeftDriver.getSpeedRPM());
//		INFO("out rpm [left] = " << rpm);
//		INFO("m_lastLeftRpm [left] = " << m_lastLeftRpm);

		if (overrun)
		{
			ERROR("Left pid regulator missed update time");
		}

		if (rpm != m_lastLeftRpm)
		{
			m_motorLeftDriver.setRPMSignal(rpm);
			m_lastLeftRpm = rpm;
		}
	}

	m_timeTracker.addPoint();

	if (m_timeTracker.full())
	{
//		INFO("Max = " << m_timeTracker.getMax().count() << " [ms]");
//		INFO("Min = " << m_timeTracker.getMin().count() << " [ms]");
//		INFO("Mean = " << m_timeTracker.getMean().count() << " [ms]");
//		INFO("Median = " << m_timeTracker.getMedian().count() << " [ms]");

		m_timeTracker.clear();
	}
}

void MotorPIDRegulator::updateRight()
{
	if (m_enabled)
	{
		bool overrun = false;
		const float rpm = m_rightPidRegulator.update(m_motorRightDriver.getSpeedRPM());

//    	INFO("in rpm [right] = " << m_motorRightDriver.getSpeedRPM());
//		INFO("out rpm [right] = " << rpm);
//		INFO("m_lastRightRpm [right] = " << m_lastRightRpm);

		if (overrun)
		{
			ERROR("Right pid regulator missed update time");
		}

		if (rpm != m_lastRightRpm)
		{
			m_motorRightDriver.setRPMSignal(rpm);
			m_lastRightRpm = rpm;
		}
	}
}
