/*
 * MotionMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef MOTIONMESSAGE_H_
#define MOTIONMESSAGE_H_

// project
#include "CommonMessageTypes.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

class MotionMessage final : public ServiceMessageBase
{
public:

	enum class Motor
	{
		LeftMotor = 0,
		RightMotor,
		AllMotors
	};

	enum class MotorAction
	{
		Stop = 0,
		Coast,
		Run
	};

	explicit MotionMessage(const float leftSpeed, const float rightSpeed) : ServiceMessageBase(commonservices::CommonMessageTypes::Type::MotionMessage),
			m_motor(Motor::AllMotors), m_action(MotorAction::Run), m_leftSpeed(leftSpeed), m_rightSpeed(rightSpeed)
	{
	}

	explicit MotionMessage(const Motor motor, const float speed) : ServiceMessageBase(commonservices::CommonMessageTypes::Type::MotionMessage),
			m_motor(motor), m_action(MotorAction::Run), m_leftSpeed(speed), m_rightSpeed(speed)
	{
	}

	explicit MotionMessage(const Motor motor, const MotorAction action) : ServiceMessageBase(commonservices::CommonMessageTypes::Type::MotionMessage),
			m_motor(motor), m_action(action)
	{
		TB_ASSERT(action != MotorAction::Run);
	}

	Motor getMotor() const
	{
		return m_motor;
	}

	MotorAction getAction() const
	{
		return m_action;
	}

	float getLeftSpeed() const
	{
		return m_leftSpeed;
	}

	float getRightSpeed() const
	{
		return m_rightSpeed;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return tbox::make_unique<MotionMessage>(*this);
	}

	static const char* toString(const Motor motor)
	{
		#define CASE(v) case v: return #v;
		switch (motor)
		{
			CASE(Motor::LeftMotor)
			CASE(Motor::RightMotor)
			CASE(Motor::AllMotors)
		}
		static std::string tmp = std::to_string(static_cast<int>(motor));
		return tmp.c_str();
		#undef CASE
	}

	static const char* toString(const MotorAction action)
	{
		#define CASE(v) case v: return #v;
		switch (action)
		{
			CASE(MotorAction::Run)
			CASE(MotorAction::Coast)
			CASE(MotorAction::Stop)
		}
		static std::string tmp = std::to_string(static_cast<int>(action));
		return tmp.c_str();
		#undef CASE
	}

private:
	Motor m_motor;
	MotorAction m_action;

	float m_leftSpeed;
	float m_rightSpeed;
};

#endif /* MOTIONMESSAGE_H_ */
