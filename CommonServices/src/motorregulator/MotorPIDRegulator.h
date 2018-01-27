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
#include "TimeTracker.h"

class IMotor;

class MotorPIDRegulator final
{
public:
	explicit MotorPIDRegulator(IMotor& motorLeftDriver, IMotor& motorRightDriver);

	bool isEnabled() const;
	void enable(const bool enable);
	void setLeftSpeed(const float speed);
	void setRightSpeed(const float speed);
	void update();

private:
	IMotor& m_motorLeftDriver;
	IMotor& m_motorRightDriver;
	bool m_enabled;
	float m_speed;
	TimeTracker<std::chrono::milliseconds> m_timeTracker;
};

#endif /* MOTORPIDREGULATOR_H_ */
