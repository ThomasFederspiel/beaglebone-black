/*
 * MotorPIDRegulator.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef MOTORPIDREGULATOR_H_
#define MOTORPIDREGULATOR_H_

// standard
#include <chrono>
#include <memory>

// local

// project
#include "PIDRegulator.h"
#include "TimeTracker.h"

class IMotor;
class IPCDeviceProxyEventEQEP;

// typedef PIDRegulator<std::chrono::milliseconds> PidRegulator_t;
//template <typename T = std::chrono::milliseconds>
//using PidRegulator_t = PIDRegulator<T>;
#define PidRegulator_t PIDRegulator<std::chrono::milliseconds>

class MotorPIDRegulator final
{
public:

	explicit MotorPIDRegulator(IMotor& motorLeftDriver, IMotor& motorRightDriver);

	bool isEnabled() const;
	void enable(const bool enable);
	const PidRegulator_t::PIDState& getLeftPidState() const;
	void setLeftSpeed(const float speed);
	void setRightSpeed(const float speed);
	void setLeftTuning(const float kp, const float ki, const float kd);
	void setRightTuning(const float kp, const float ki, const float kd);
	const PidRegulator_t::PIDState& getRightPidState() const;

	void updateLeft();
	void updateRight();

private:
	IMotor& m_motorLeftDriver;
	IMotor& m_motorRightDriver;
	bool m_enabled;
	PidRegulator_t m_leftPidRegulator;
	PidRegulator_t m_rightPidRegulator;
	float m_lastLeftRpm;
	float m_lastRightRpm;
	TimeTracker<std::chrono::milliseconds, 20> m_timeTracker;
};

#endif /* MOTORPIDREGULATOR_H_ */
