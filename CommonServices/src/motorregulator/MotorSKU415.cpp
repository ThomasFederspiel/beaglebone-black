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
#include "TMath.h"

MODULE_LOG(MotorSKU415);

namespace
{
// Measured for 1500 rev per revolusion
static constexpr float MotorQuadraturePulsesPerRev = 1500.f;

// 400 * 1500 / 60 / 100 gives 100 pulses per 10 ms, 1 pulse miss per period gives 1 % fault
static constexpr float LowSpeedRPMLevel = 400;
}

MotorSKU415::MotorSKU415(MotorDriver8833::Motor& motor, const SKU415Config& config) : m_motor(motor),
	m_speedEvaluator(MotorQuadraturePulsesPerRev, LowSpeedRPMLevel), m_counter(0),
	m_config(config), m_distanceTracker()
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
	if (tbox::Math::isZero(speed))
	{
		m_motor.stop();
	}
	else
	{
		const bool forward = speed > 0.f;
		const float absSpeed = std::min(std::fabs(speed), getRPMSignalMax());
		const uint16_t ctrlSignal = convertRPM(absSpeed);

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

float MotorSKU415::getLowSpeedRPM() const
{
	return m_speedEvaluator.getLowSpeedRPM();
}

float MotorSKU415::getHighSpeedRPM() const
{
	return m_speedEvaluator.getHighSpeedRPM();
}

bool MotorSKU415::isLowSpeedValid() const
{
	return m_speedEvaluator.isLowSpeedValid();
}

bool MotorSKU415::isLowSpeedActive() const
{
	return m_speedEvaluator.isLowSpeedActive();
}

uint16_t MotorSKU415::getControlSignalMax() const
{
	return m_motor.getControlSignalMax();
}

float MotorSKU415::getRPMSignalMin() const
{
	return m_config.m_minRPM;
}

float MotorSKU415::getRPMSignalMax() const
{
	return m_config.m_maxRPM;
}

// ;+
#include "Utils.h"

void MotorSKU415::update(const IPCDeviceProxyEventEQEP& eqep)
{
	m_counter = eqep.getCounter();

	// ;+
	if (eqep.getPwmssDevice() == PWMSS_DEV_3)
	{
//	INFO("----------------- eqep.getPwmssDevice() = " << eqep.getPwmssDevice());
//	INFO("eqep.getCapCounter() = " << eqep.getCapCounter());
//	INFO("eqep.getCapTime() = " << eqep.getCapTime());
//	INFO("eqep.getCapPeriod() = " << eqep.getCapPeriod());
//	INFO("eqep.getCapStatus() = " << Utils::toBinaryStr(eqep.getCapStatus()));
//	INFO("eqep.getIntrStatus() = " << Utils::toBinaryStr(eqep.getIntrStatus()));
//	INFO("eqep.getCounter() = " << eqep.getCounter());
//	INFO("eqep.getUnitTime() = " << eqep.getUnitTime());
//	INFO("eqep.getUEventPulses() = " << eqep.getUEventPulses());
//	INFO("eqep.getCaptureTimeTick() = " << eqep.getCaptureTimeTick());
	}

	if (m_distanceTracker.isEnabled())
	{
		if (m_distanceTracker.evaluate(m_counter))
		{
			m_motor.stop();
		}
	}

	m_speedEvaluator.update(eqep);
}

uint16_t MotorSKU415::convertRPM(const float speedRPM) const
{
 // min = 2485, max = 4000
 // rpmMin = 21.6 rpmMax = 195.2
 // right [    9.00710803  2299.77917424]

 // min = 2640, max = 4000
 // rpmMin = 30.8 rpmMax = 185.6
 // left [    8.83119665  2421.77606027]

	// for 1500
	return m_config.m_rpmVsCtrlSignalSlope * speedRPM + m_config.m_rpmVsCtrlSignalYIntercept;
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

