/*
 * PropulsionOdometerMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PROPULSIONODOMETERMESSAGE_H_
#define PROPULSIONODOMETERMESSAGE_H_

// standard
#include <cstdint>

// project
#include "CommonMessageTypes.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

class PropulsionOdometerMessage final : public ServiceMessageBase
{
public:

	enum class Motor : uint8_t
	{
		LeftMotor = 0,
		RightMotor
	};

	explicit PropulsionOdometerMessage(const Motor motor, const uint32_t capCounter,
			const uint32_t capTime, const uint32_t capPeriod, const uint32_t counter) : ServiceMessageBase(getMessageType(motor)),
					m_motor(motor), m_capCounter(capCounter), m_capTime(capTime), m_capPeriod(capPeriod), m_counter(counter)
	{
	}

	Motor getMotor() const
	{
		return m_motor;
	}

	uint32_t getCapCounter() const
	{
		return m_capCounter;
	}

	uint32_t getCapTime() const
	{
		return m_capTime;
	}

	uint32_t getCapPeriod() const
	{
		return m_capPeriod;
	}

	uint32_t getCounter() const
	{
		return m_counter;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return tbox::make_unique<PropulsionOdometerMessage>(*this);
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
				commonservices::CommonMessageTypes::Type::LeftPropulsionOdometerMessage;

		switch (motor)
		{
		case Motor::LeftMotor:
			type = commonservices::CommonMessageTypes::Type::LeftPropulsionOdometerMessage;
			break;

		case Motor::RightMotor:
			type = commonservices::CommonMessageTypes::Type::RightPropulsionOdometerMessage;
			break;
		}

		return type;
	}

	Motor m_motor;
	uint32_t m_capCounter;
	uint32_t m_capTime;
	uint32_t m_capPeriod;
	uint32_t m_counter;
};

#endif /* PROPULSIONODOMETERMESSAGE_H_ */
