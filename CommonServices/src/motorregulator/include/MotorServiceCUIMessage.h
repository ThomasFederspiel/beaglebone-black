/*
 * MotorServiceCUIMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef MOTIONMESSAGE_H_
#define MOTIONMESSAGE_H_

// standard
#include <memory>

// project
#include "CommonMessageTypes.h"
#include "CUICommandContext.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

class MotorServiceCUIMessage final : public ServiceMessageBase
{
public:

	enum class CUIMessageType
	{
		Undefined = 0,
		SetRegulatorMode,
		setMotorSpeed,
		setMotorDistance,
		GetMotorStatus,
		SetMotorPidTuning
	};

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

	enum class RegulatorMode
	{
		PidRegulation = 0,
		PercentageAdjusted,
		Percentage,
		Raw,
		RPM
	};

	static MotorServiceCUIMessage createSetMotorSpeed(const float leftSpeed, const float rightSpeed)
	{
		MotorServiceCUIMessage msg;

		msg.m_cuiMessageType = CUIMessageType::setMotorSpeed;
		msg.m_motor = Motor::AllMotors;
		msg.m_action = MotorAction::Run;
		msg.m_leftSpeed = leftSpeed;
		msg.m_rightSpeed = rightSpeed;

		return msg;
	}

	static MotorServiceCUIMessage createSetMotorDistance(const int leftDistance, const int rightDistance)
	{
		MotorServiceCUIMessage msg;

		msg.m_cuiMessageType = CUIMessageType::setMotorDistance;
		msg.m_leftDistance = leftDistance;
		msg.m_rightDistance = rightDistance;

		return msg;
	}

	static MotorServiceCUIMessage createSetRegulatorMode(const RegulatorMode mode)
	{
		MotorServiceCUIMessage msg;

		msg.m_cuiMessageType = CUIMessageType::SetRegulatorMode;
		msg.m_regulatorMode = mode;

		return msg;
	}

	static MotorServiceCUIMessage createSetMotorPidTuning(const Motor motor, const float kp,
			const float ki, const float kd)
	{
		MotorServiceCUIMessage msg;

		msg.m_cuiMessageType = CUIMessageType::SetMotorPidTuning;
		msg.m_motor = motor;
		msg.m_kp = kp;
		msg.m_ki = ki;
		msg.m_kd = kd;

		return msg;
	}

	static MotorServiceCUIMessage createGetMotorStatus(std::shared_ptr<CUICommandContext> context)
	{
		MotorServiceCUIMessage msg;

		msg.m_cuiMessageType = CUIMessageType::GetMotorStatus;
		msg.m_context = context;

		return msg;
	}

	CUICommandContext& getCUIContext() const
	{
		return *m_context;
	}

	CUIMessageType getMessageType() const
	{
		return m_cuiMessageType;
	}

	RegulatorMode getRegulatorMode() const
	{
		return m_regulatorMode;
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

	int getLeftDistance() const
	{
		return m_leftDistance;
	}

	int getRightDistance() const
	{
		return m_rightDistance;
	}

	float getKpFactor() const
	{
		return m_kp;
	}

	float getKiFactor() const
	{
		return m_ki;
	}

	float getKdFactor() const
	{
		return m_kd;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return std::make_unique<MotorServiceCUIMessage>(*this);
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

	static const char* toString(const RegulatorMode mode)
	{
		#define CASE(v) case v: return #v;
		switch (mode)
		{
			CASE(RegulatorMode::PidRegulation)
			CASE(RegulatorMode::PercentageAdjusted)
			CASE(RegulatorMode::Percentage)
			CASE(RegulatorMode::Raw)
			CASE(RegulatorMode::RPM)
		}
		static std::string tmp = std::to_string(static_cast<int>(mode));
		return tmp.c_str();
		#undef CASE
	}

	static const char* toString(const CUIMessageType type)
	{
		#define CASE(v) case v: return #v;
		switch (type)
		{
			CASE(CUIMessageType::Undefined)
			CASE(CUIMessageType::setMotorSpeed)
			CASE(CUIMessageType::setMotorDistance)
			CASE(CUIMessageType::SetRegulatorMode)
			CASE(CUIMessageType::GetMotorStatus)
			CASE(CUIMessageType::SetMotorPidTuning)
		}
		static std::string tmp = std::to_string(static_cast<int>(type));
		return tmp.c_str();
		#undef CASE
	}

private:
	explicit MotorServiceCUIMessage() : ServiceMessageBase(commonservices::CommonMessageTypes::Type::MotorServiceCUIMessage),
		m_cuiMessageType(CUIMessageType::Undefined), m_regulatorMode(RegulatorMode::PidRegulation), m_motor(Motor::AllMotors), m_action(MotorAction::Run),
		m_leftSpeed(0.f), m_rightSpeed(0.f), m_context(), m_leftDistance(0), m_rightDistance(0), m_kp(0.f),
		m_ki(0.f), m_kd(0.f)
	{
	}

	CUIMessageType m_cuiMessageType;

	// Used for SetRegulatorMode
	RegulatorMode m_regulatorMode;

	// Used for SetMotorSpeed
	Motor m_motor;
	MotorAction m_action;
	float m_leftSpeed;
	float m_rightSpeed;

	// Used for GetMotorStatus
	std::shared_ptr<CUICommandContext> m_context;

	// Used for SetMotorDistance
	int m_leftDistance;
	int m_rightDistance;

	// Used for SetMotorPidTuning
	// m_motor
	float m_kp;
	float m_ki;
	float m_kd;
};

#endif /* MOTIONMESSAGE_H_ */
