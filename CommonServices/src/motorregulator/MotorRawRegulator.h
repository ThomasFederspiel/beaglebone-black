/*
 * MotorRawRegulator.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef MOTORRAWREGULATOR_H_
#define MOTORRAWREGULATOR_H_

// standard
#include <memory>

// local
#include "SpeedEvaluator.h"

// project
#include "IPCDeviceProxyEventEQEP.h"

class IMotor;

class MotorRawRegulator final
{
public:

	enum class Mode
	{
		Disabled = 0,
		RPM,
		Raw,
		Percentage,
		PercentageAdjusted,
		Distance
	};

	explicit MotorRawRegulator(IMotor& motorLeftDriver, IMotor& motorRightDriver);

	bool isEnabled() const;
	void setMode(const Mode mode);
	void setLeftSpeed(const float speed);
	void setRightSpeed(const float speed);
	void setLeftDistance(const int distance);
	void setRightDistance(const int distance);

private:

	void setSpeed(const float setSpeed, IMotor& motorDriver, float& speedStorage);

	static const char* toString(const Mode mode)
	{
		#define CASE(v) case v: return #v;
		switch (mode)
		{
			CASE(Mode::Disabled)
			CASE(Mode::RPM)
			CASE(Mode::Raw)
			CASE(Mode::Percentage)
			CASE(Mode::PercentageAdjusted)
			CASE(Mode::Distance)
		}
		static std::string tmp = std::to_string(static_cast<int>(mode));
		return tmp.c_str();
		#undef CASE
	}

	IMotor& m_motorLeftDriver;
	IMotor& m_motorRightDriver;
	Mode m_mode;
	float m_leftSpeed;
	float m_rightSpeed;
};

#endif /* MOTORRAWREGULATOR_H_ */
