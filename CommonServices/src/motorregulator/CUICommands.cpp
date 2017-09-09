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
#include "MotionMessage.h"
#include "stringifyer.h"

MODULE_LOG(CUICommands);

namespace
{
	static const std::string CommandPath = "/services/motor";
	static const std::string SetMotorSpeedCmd = "sms";
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


	if (commandParser.count() != 2)
	{
		context.finalizeError("Expects two arguments");
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

	m_service.post(MotionMessage(leftSpeed, rightSpeed));

	context.output() << "Done" << context.newline();
	context.finalize();
}

void SetMotorSpeedCommand::genShortDesc(std::ostream& stream) const
{
	stream << "set motor speed";
}
