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

MODULE_LOG(CUICommands);

namespace
{
	static const std::string CommandPath = "/";
}

ShutdownCommand::ShutdownCommand(MainService& service) : AbstractCUICommand("shutdown", CommandPath),
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
		ERROR("async");

		service->shutdown();
	}, &m_service);

	m_service.post(asyncTaskMessage);
}

void ShutdownCommand::genShortDesc(std::ostream& stream) const
{
	stream << "shutdown application";
}
