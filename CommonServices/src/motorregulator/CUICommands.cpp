/*
 * CUICommands.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "CUICommands.h"

// standard
#include <future>
#include <ostream>

// project
#include "AsyncTaskMessage.h"
#include "CUICommandContext.h"
#include "ICUICommandParser.h"
#include "Logger.h"
#include "MotorServiceCUIMessage.h"
#include "stringifyer.h"

MODULE_LOG(CUICommands);

namespace
{
	static const std::string CommandPath = "/services/motor";

	static const std::string SetMotorSpeedCmd = "sms";
	static const std::string GetMotorStatusCmd = "stat";
	static const std::string SetMotorDistanceCmd = "smd";

	static const std::string SetMotorPidTuningCmd = "spt";
	static const std::string LeftArg = "left";
	static const std::string RightArg = "right";

	static const std::string SetRegulatorModeCmd = "srm";
	static const std::string PidModeArg = "pid";
	static const std::string RPMModeArg = "rpm";
	static const std::string RawModeArg = "raw";
	static const std::string PercentageModeArg = "per";
	static const std::string PercentageAdjustedModeArg = "peradj";
}

void CUICommands::registerCUICommands(ServiceAllocator& allocator, MotorRegulatorService& service)
{
	allocator.registerCommand(tbox::make_unique<SetMotorSpeedCommand>(service));
	allocator.registerCommand(tbox::make_unique<SetMotorDistanceCommand>(service));
	allocator.registerCommand(tbox::make_unique<SetRegulatorModeCommand>(service));
	allocator.registerCommand(tbox::make_unique<GetMotorStatusCommand>(service));
	allocator.registerCommand(tbox::make_unique<SetMotorPidTuningCommand>(service));
}

void CUICommands::unregisterCUICommands(ServiceAllocator& allocator, MotorRegulatorService& service)
{
	allocator.unregisterCommand(SetMotorSpeedCommand(service));
	allocator.unregisterCommand(SetMotorDistanceCommand(service));
	allocator.unregisterCommand(SetRegulatorModeCommand(service));
	allocator.unregisterCommand(GetMotorStatusCommand(service));
	allocator.unregisterCommand(SetMotorPidTuningCommand(service));
}

SetMotorSpeedCommand::SetMotorSpeedCommand(MotorRegulatorService& service) : AbstractCUICommand(SetMotorSpeedCmd, CommandPath),
	m_service(service)
{
}

void SetMotorSpeedCommand::invoke(const ICUICommandParser& commandParser, CUICommandContext& context)
{
// ;+	using Type = int(int,int);
//
//	std::packaged_task<Type> task([](int a, int b) {
//	        return a + b;
//	    });
//
//	int a = 10;
//	int b = 12;
//
//	AsyncTaskMessage asyncTaskMessage(std::move(task));

	AsyncTaskMessage asyncTaskMessage([](int a, int b) {
//	AsyncTaskMessage asyncTaskMessage([](int a, int b, CUICommandContext& context) {

		int c = a + b;

		INFO("c = " << c);

//		context.output() << "result = " << c;
//		context.finalize();
    }, 10, 33);
//    }, 10, 33, context);

	m_service.post(asyncTaskMessage);

	parse(commandParser, context);
}

void SetMotorSpeedCommand::genShortDesc(std::ostream& stream) const
{
	stream << "set motor speed";
}

void SetMotorSpeedCommand::parse(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	if (commandParser.count() != 2)
	{
		context.finalizeError("Bad command");
		return;
	}

	float leftSpeed = 0.f;
	if (!commandParser.getValueArgument(0, leftSpeed))
	{
		context.finalizeError("Illegal left speed specified");
		return;
	}

	float rightSpeed = 0.f;
	if (!commandParser.getValueArgument(1, rightSpeed))
	{
		context.finalizeError("Illegal right speed specified");
		return;
	}

	m_service.post(MotorServiceCUIMessage::createSetMotorSpeed(leftSpeed, rightSpeed));

	context.output() << "Done" << context.newline();
	context.finalize();
}

SetRegulatorModeCommand::SetRegulatorModeCommand(MotorRegulatorService& service) : AbstractCUICommand(SetRegulatorModeCmd, CommandPath),
	m_service(service)
{
}

void SetRegulatorModeCommand::invoke(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	parse(commandParser, context);
}

void SetRegulatorModeCommand::genShortDesc(std::ostream& stream) const
{
	stream << "set regulator mode";
}

void SetRegulatorModeCommand::parse(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	if (commandParser.count() != 1)
	{
		context.finalizeError("Bad command");
		return;
	}

	std::string modeArg;

	if (!commandParser.getStringArgument(0, modeArg))
	{
		context.finalizeError("Bad mode argument");
		return;
	}

	MotorServiceCUIMessage::RegulatorMode mode = MotorServiceCUIMessage::RegulatorMode::Raw;

	if (modeArg == PidModeArg)
	{
		mode = MotorServiceCUIMessage::RegulatorMode::PidRegulation;
	}
	else if (modeArg == RPMModeArg)
	{
		mode = MotorServiceCUIMessage::RegulatorMode::RPM;
	}
	else if (modeArg == PercentageAdjustedModeArg)
	{
		mode = MotorServiceCUIMessage::RegulatorMode::PercentageAdjusted;
	}
	else if (modeArg == PercentageModeArg)
	{
		mode = MotorServiceCUIMessage::RegulatorMode::Percentage;
	}
	else if (modeArg == RawModeArg)
	{
		mode = MotorServiceCUIMessage::RegulatorMode::Raw;
	}
	else
	{
		context.finalizeError("Bad mode argument");
		return;
	}

	m_service.post(MotorServiceCUIMessage::createSetRegulatorMode(mode));

	context.output() << "Done" << context.newline();
	context.finalize();
}

SetMotorPidTuningCommand::SetMotorPidTuningCommand(MotorRegulatorService& service) : AbstractCUICommand(SetMotorPidTuningCmd, CommandPath),
	m_service(service)
{
}

void SetMotorPidTuningCommand::invoke(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	parse(commandParser, context);
}

void SetMotorPidTuningCommand::genShortDesc(std::ostream& stream) const
{
	stream << "set motor pid tuning";
}

void SetMotorPidTuningCommand::parse(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	if ((commandParser.count() != 4) && (commandParser.count() != 3))
	{
		context.finalizeError("Bad command");
		return;
	}

	MotorServiceCUIMessage::Motor motor = MotorServiceCUIMessage::Motor::AllMotors;

	std::size_t index = 0;

	if (commandParser.count() == 4)
	{
		std::string sideArg;

		if (!commandParser.getStringArgument(index++, sideArg))
		{
			context.finalizeError("Bad side argument");
			return;
		}

		if (sideArg == LeftArg)
		{
			motor = MotorServiceCUIMessage::Motor::LeftMotor;
		}
		else if (sideArg == RightArg)
		{
			motor = MotorServiceCUIMessage::Motor::RightMotor;
		}
		else
		{
			context.finalizeError("Bad side argument");
			return;
		}
	}

	float kp = 0;
	if (!commandParser.getValueArgument(index++, kp))
	{
		context.finalizeError("Illegal Kp tuning parameter specified");
		return;
	}

	float ki = 0;
	if (!commandParser.getValueArgument(index++, ki))
	{
		context.finalizeError("Illegal Ki tuning parameter specified");
		return;
	}

	float kd = 0;
	if (!commandParser.getValueArgument(index++, kd))
	{
		context.finalizeError("Illegal Kd tuning parameter specified");
		return;
	}

	m_service.post(MotorServiceCUIMessage::createSetMotorPidTuning(motor, kp, ki, kd));

	context.output() << "Done" << context.newline();
	context.finalize();
}

GetMotorStatusCommand::GetMotorStatusCommand(MotorRegulatorService& service) : AbstractCUICommand(GetMotorStatusCmd, CommandPath),
	m_service(service)
{
}

void GetMotorStatusCommand::invoke(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	parse(commandParser, context);
}

void GetMotorStatusCommand::genShortDesc(std::ostream& stream) const
{
	stream << "get motor status";
}

void GetMotorStatusCommand::parse(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	m_service.post(MotorServiceCUIMessage::createGetMotorStatus(context.createInstance()));
}

SetMotorDistanceCommand::SetMotorDistanceCommand(MotorRegulatorService& service) : AbstractCUICommand(SetMotorDistanceCmd, CommandPath),
	m_service(service)
{
}

void SetMotorDistanceCommand::invoke(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	parse(commandParser, context);
}

void SetMotorDistanceCommand::genShortDesc(std::ostream& stream) const
{
	stream << "set motor distance";
}

void SetMotorDistanceCommand::parse(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	if (commandParser.count() != 2)
	{
		context.finalizeError("Bad command");
		return;
	}

	int leftDistance = 0;
	if (!commandParser.getValueArgument(0, leftDistance))
	{
		context.finalizeError("Illegal left distance specified");
		return;
	}

	int rightDistance = 0;
	if (!commandParser.getValueArgument(1, rightDistance))
	{
		context.finalizeError("Illegal right distance specified");
		return;
	}

	m_service.post(MotorServiceCUIMessage::createSetMotorDistance(leftDistance, rightDistance));

	context.output() << "Done" << context.newline();
	context.finalize();
}

