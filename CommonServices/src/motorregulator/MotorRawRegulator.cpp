/*
 * MotorRawRegulator.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "MotorRawRegulator.h"

// standard
#include <cmath>

// local
#include "IMotor.h"

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "TMath.h"
#include "tboxdefs.h"

MODULE_LOG(MotorRawRegulator);

namespace
{
	static constexpr float StallLevel = 0.62f;
	static constexpr float MaxPercentage = 100.f;
}

MotorRawRegulator::MotorRawRegulator(IMotor& motorLeftDriver, IMotor& motorRightDriver)
	: m_motorLeftDriver(motorLeftDriver), m_motorRightDriver(motorRightDriver),
	  m_mode(Mode::Disabled), m_leftSpeed(0.f), m_rightSpeed(0.f)
{
}

bool MotorRawRegulator::isEnabled() const
{
	return m_mode != Mode::Disabled;
}

void MotorRawRegulator::setMode(const Mode mode)
{
	m_mode = mode;
}

void MotorRawRegulator::setLeftDistance(const int distance)
{
	m_motorLeftDriver.setDistance(distance);
}

void MotorRawRegulator::setRightDistance(const int distance)
{
	m_motorRightDriver.setDistance(distance);
}

void MotorRawRegulator::setLeftSpeed(const float speed)
{
	setSpeed(speed, m_motorLeftDriver, m_leftSpeed);
}

void MotorRawRegulator::setRightSpeed(const float speed)
{
	setSpeed(speed, m_motorRightDriver, m_rightSpeed);
}

void MotorRawRegulator::setSpeed(const float setSpeed, IMotor& motorDriver, float& speedStorage)
{
	if (m_mode == Mode::Disabled)
	{
		return;
	}

	speedStorage = setSpeed;

	const bool forward = setSpeed > 0.f;
	uint16_t ctrlSignal = 0;

	// ;+
	INFO("mode = " << toString(m_mode));
	INFO("speed = " << setSpeed);

	switch (m_mode)
	{
	case Mode::Raw:
	{
		ctrlSignal = std::min(static_cast<uint16_t>(std::abs(setSpeed)), motorDriver.getControlSignalMax());
		break;
	}

	case Mode::RPM:
	{
		motorDriver.setRPMSignal(setSpeed);
		return;
	}

	case Mode::Percentage:
	case Mode::PercentageAdjusted:
	{
		const float absSpeed = std::min(std::fabs(setSpeed), MaxPercentage);

		if (tbox::Math::isZero(absSpeed))
		{
			ctrlSignal = 0;
		}
		else
		{
			float duty = absSpeed / 100.f;

			if (m_mode == Mode::PercentageAdjusted)
			{
				duty = (1.f - StallLevel) * duty + StallLevel;
			}

			ctrlSignal = static_cast<uint16_t>(motorDriver.getControlSignalMax() * duty);
		}
		break;
	}

	TB_DEFAULT(toString(m_mode));
	}

	// ;+
	INFO("ctrlSignal = " << ctrlSignal);

	if (ctrlSignal == 0)
	{
		motorDriver.stop();
	}
	else
	{
		motorDriver.setControlSignal(forward, ctrlSignal);
	}
}

