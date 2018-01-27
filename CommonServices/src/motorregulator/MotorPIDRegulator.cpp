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
#include "Logger.h"
#include "tboxdefs.h"

MODULE_LOG(MotorPIDRegulator);

MotorPIDRegulator::MotorPIDRegulator(IMotor& motorLeftDriver, IMotor& motorRightDriver)
	: m_motorLeftDriver(motorLeftDriver), m_motorRightDriver(motorRightDriver), m_enabled(false),
	  m_speed(0.f), m_timeTracker()
{
}

bool MotorPIDRegulator::isEnabled() const
{
	return m_enabled;
}

void MotorPIDRegulator::enable(const bool enable)
{
	const bool modified = m_enabled != enable;

	m_enabled = enable;

	if (modified && !m_enabled)
	{
		m_motorLeftDriver.stop();
		m_motorRightDriver.stop();
	}
}

void MotorPIDRegulator::setLeftSpeed(const float speed)
{
	m_speed = speed;
}

void MotorPIDRegulator::setRightSpeed(const float speed)
{
	m_speed = speed;
}

void MotorPIDRegulator::update()
{
}

