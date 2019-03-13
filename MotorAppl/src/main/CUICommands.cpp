/*
 * CUICommands.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "CUICommands.h"

// standard
#include <ostream>

// project
#include "AsyncTaskMessage.h"
#include "CUICommandContext.h"
#include "ICUICommandParser.h"
#include "Logger.h"
#include "stringifyer.h"
#include "ThreadFactory.h"
#include "ServiceManager.h"

MODULE_LOG(CUICommands);

namespace
{
	static const std::string ShutdownCmdPath = "/";
	static const std::string ShutdownCmd = "shutdown";

	static const std::string SystemCmdPath = "/system";
	static const std::string ThreadInfoCmd = "thread-info";
	static const std::string ServiceInfoCmd = "service-info";
}

ShutdownCommand::ShutdownCommand(MainService& service) : AbstractCUICommand(ShutdownCmd, ShutdownCmdPath),
	m_service(service)
{
}

void ShutdownCommand::invoke(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	if (commandParser.count() != 0)
	{
		context.finalizeError("Expects no arguments");
		return;
	}

	AsyncTaskMessage asyncTaskMessage([](MainService* const service)
	{
		service->shutdown();
	}, &m_service);

	m_service.post(asyncTaskMessage);
}

void ShutdownCommand::genShortDesc(std::ostream& stream) const
{
	stream << "shutdown application";
}

ThreadInfoCommand::ThreadInfoCommand() : AbstractCUICommand(ThreadInfoCmd, SystemCmdPath)
{
}

void ThreadInfoCommand::invoke(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	if (commandParser.count() != 0)
	{
		context.finalizeError("Expects no arguments");
		return;
	}

	ThreadFactory::instance().streamThreadInformation(context.output());

	context.finalize();
}

void ThreadInfoCommand::genShortDesc(std::ostream& stream) const
{
	stream << "show thread information";
}

ServiceInfoCommand::ServiceInfoCommand(MainService& service) : AbstractCUICommand(ServiceInfoCmd, SystemCmdPath),
	m_service(service)
{
}

void ServiceInfoCommand::invoke(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	if (commandParser.count() != 0)
	{
		context.finalizeError("Expects no arguments");
		return;
	}

	m_service.getServiceManager().streamServiceInformation(context.output());

	context.finalize();
}

void ServiceInfoCommand::genShortDesc(std::ostream& stream) const
{
	stream << "show service information";
}

