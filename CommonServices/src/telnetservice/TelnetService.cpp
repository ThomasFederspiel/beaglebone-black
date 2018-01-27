/*
 * TelnetService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "TelnetService.h"

// standard

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "CommonMessageTypes.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"
#include "TelnetServer.h"

// local
#include "CommandExecutor.h"
#include "ICUICommand.h"

MODULE_LOG(TelnetService);

using namespace commonservices;

namespace
{
	static constexpr int TelnetPort = 1234;
	static std::string TelnetServerName = "TelnetServer";
}

TelnetService::TelnetService(const std::string& name) : AbstractService(name), m_telnetServer(new TelnetServer(TelnetServerName)),
		m_commandExecutor(new CommandExecutor(*m_telnetServer))
{
}

TelnetService::~TelnetService()
{
	// needed for unique_ptr
}

void TelnetService::registerCommand(std::unique_ptr<ICUICommand> command)
{
	m_commandExecutor->registerCommand(std::move(command));
}

void TelnetService::unregisterCommand(const ICUICommand& command)
{
	m_commandExecutor->unregisterCommand(command);
}

void TelnetService::onStart(ServiceAllocator& allocator)
{
	m_telnetServer->start(TelnetPort);
}

TelnetService::StopStatus TelnetService::onStop(ServiceAllocator& allocator)
{
	m_telnetServer->stop();

	return StopStatus::Done;
}

void TelnetService::onMessage(const ServiceMessageBase& message)
{
	INFO("Got message " << CommonMessageTypes::toString(message.getType()));

	switch (message.getType())
	{
	TB_DEFAULT(CommonMessageTypes::toString(message.getType()));
	}
}

