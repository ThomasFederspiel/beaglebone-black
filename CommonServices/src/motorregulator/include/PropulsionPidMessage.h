/*
 * PropulsionPidMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PROPULSIONPIDMESSAGE_H_
#define PROPULSIONPIDMESSAGE_H_

// standard
#include <cstdint>

// project
#include "CommonMessageTypes.h"
#include "ServiceMessageBase.h"
#include "stdExtension.h"

class PropulsionPidMessage final : public ServiceMessageBase
{
public:

	enum class Motor : uint8_t
	{
		LeftMotor = 0,
		RightMotor
	};

	explicit PropulsionPidMessage(const Motor motor, const float setPoint, const float kpFactor, const float kiFactor,
			const float kdFactor, const float input, const float error, const float output) : ServiceMessageBase(getMessageType(motor)),
					m_motor(motor), m_setPoint(setPoint), m_kpFactor(kpFactor), m_kiFactor(kiFactor), m_kdFactor(kdFactor),
					m_input(input), m_error(error), m_output(output)
	{
	}

	Motor getMotor() const
	{
		return m_motor;
	}

	float getSetPoint() const
	{
		return m_setPoint;
	}

	float getKpFactor() const
	{
		return m_kpFactor;
	}

	float getKiFactor() const
	{
		return m_kiFactor;
	}

	float getKdFactor() const
	{
		return m_kdFactor;
	}

	float getInput() const
	{
		return m_input;
	}

	float getError() const
	{
		return m_error;
	}

	float getOutput() const
	{
		return m_output;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return std::make_unique<PropulsionPidMessage>(*this);
	}

	static const char* toString(const Motor motor)
	{
		#define CASE(v) case v: return #v;
		switch (motor)
		{
			CASE(Motor::LeftMotor)
			CASE(Motor::RightMotor)
		}
		static std::string tmp = std::to_string(static_cast<int>(motor));
		return tmp.c_str();
		#undef CASE
	}

private:
	static commonservices::CommonMessageTypes::Type getMessageType(const Motor motor)
	{
		commonservices::CommonMessageTypes::Type type =
				commonservices::CommonMessageTypes::Type::LeftPropulsionPidMessage;

		switch (motor)
		{
		case Motor::LeftMotor:
			type = commonservices::CommonMessageTypes::Type::LeftPropulsionPidMessage;
			break;

		case Motor::RightMotor:
			type = commonservices::CommonMessageTypes::Type::RightPropulsionPidMessage;
			break;
		}

		return type;
	}

	Motor m_motor;
	float m_setPoint;
	float m_kpFactor;
	float m_kiFactor;
	float m_kdFactor;
	float m_input;
	float m_error;
	float m_output;
};

#endif /* PROPULSIONPIDMESSAGE_H_ */
