/*
 * MotorSKU415.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "MotorSKU415.h"

// standard
#include <cmath>

// project
#include "IPCDeviceProxyEventEQEP.h"
#include "Logger.h"

MODULE_LOG(MotorSKU415);

namespace
{
// Calculated
// static constexpr float MotorQuadraturePulsesPerRev = 341.2f;
// static constexpr float MinRPM = 155.f;
// static constexpr float MaxRPM = 805.f;
// Measured
static constexpr float MotorQuadraturePulsesPerRev = 1500.f;
static constexpr float QuadratureLowSpeedPercentage = 5 / 100.f;
static constexpr float MinControlSignal = 2730;
static constexpr float MaxControlSignal = 4000;
static constexpr float MinRPM = 33.f;
static constexpr float MaxRPM = 171.f;
static constexpr float StallLevelerPercentage = MinControlSignal / MaxControlSignal;
}

MotorSKU415::MotorSKU415(MotorDriver8833::Motor& motor) : m_motor(motor),
	m_speedEvaluator(MotorQuadraturePulsesPerRev, QuadratureLowSpeedPercentage), m_counter(0),
	m_distanceTracker()
{
}

void MotorSKU415::coast()
{
	m_motor.coast();
}

void MotorSKU415::stop()
{
	m_motor.stop();
}

void MotorSKU415::setControlSignal(const bool forward, const uint16_t signal)
{
	m_motor.setControlSignal(forward, signal);
}

void MotorSKU415::setRPMSignal(const float speed)
{
	const bool forward = speed > 0.f;
	const float absSpeed = std::min(std::fabs(speed), MaxRPM);
	const uint16_t ctrlSignal = convertRPM(absSpeed);

	if (ctrlSignal == 0)
	{
		m_motor.stop();
	}
	else
	{
		m_motor.setControlSignal(forward, ctrlSignal);
	}
}

void MotorSKU415::setDistance(const int distance)
{
	m_distanceTracker.set(distance, m_counter);
}

uint32_t MotorSKU415::getCounter() const
{
	return m_counter;
}

float MotorSKU415::getSpeedRPM() const
{
	return m_speedEvaluator.getSpeedRPM();
}

uint16_t MotorSKU415::getControlSignalMax() const
{
	return m_motor.getControlSignalMax();
}

uint16_t MotorSKU415::getRPMSignalMin() const
{
	return MinRPM;
}

uint16_t MotorSKU415::getRPMSignalMax() const
{
	return MaxRPM;
}

void MotorSKU415::update(const IPCDeviceProxyEventEQEP& eqep)
{
	if (eqep.getPwmssDevice() == m_motor.getPwmssDevice())
	{
		m_counter = eqep.getCounter();

		if (m_distanceTracker.isEnabled())
		{
			if (m_distanceTracker.evaluate(m_counter))
			{
				m_motor.stop();
			}
		}

		m_speedEvaluator.update(eqep);
	}
}

uint16_t MotorSKU415::convertRPM(const float speedRPM) const
{
	// for 340
	// return 1.952422351f * (speedRPM - 154.7479401f) + 2730;

	// for 1500
	return 9.187865565f * (speedRPM - 32.88392875f) + 2730;
}

MotorSKU415::DistanceTracker::DistanceTracker() : m_distance(0), m_startCounter(0)
{
}

void MotorSKU415::DistanceTracker::set(const int distance, const int counter)
{
	m_distance = distance;
	m_startCounter = counter;

	// ;+
	INFO("m_distance = " << m_distance);
	INFO("m_startCounter = " << m_startCounter);
}

bool MotorSKU415::DistanceTracker::isEnabled() const
{
	return m_distance != 0;
}

bool MotorSKU415::DistanceTracker::evaluate(const int counter)
{
	// ;+
	INFO("diff = " << counter - m_startCounter);
	INFO("counter = " << counter);

	if ((counter - m_startCounter) > m_distance)
	{
		// ;+
		INFO("diff = " << counter - m_startCounter);
		INFO("counter = " << counter);

		m_distance = 0;

		return true;
	}

	return false;
}

