/*
 * CommandExecutor.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "CommandExecutor.h"

// standard
#include <memory>
#include <string>

// project
#include "CUICommandContext.h"
#include "CUICommandContextImpl.h"
#include "CUICommandParser.h"
#include "CUICommands.h"
#include "exceptionMacros.h"
#include "ICUICommand.h"
#include "Logger.h"
#include "Stringifyer.h"
#include "tboxdefs.h"
#include "TelnetServer.h"
#include "Utils.h"

MODULE_LOG(CommandExecutor);

CommandExecutor::CommandExecutor(TelnetServer& telnetServer) : m_telnetServer(telnetServer), m_directoryTree(),
		m_directoryTreeMutex()
{
	registerBaseCommands();

	m_telnetServer.subscribe(*this);
}

CommandExecutor::~CommandExecutor()
{
	m_telnetServer.unsubscribe(*this);
}

void CommandExecutor::registerCommand(std::unique_ptr<ICUICommand> command)
{
	TB_ASSERT(command);

	std::lock_guard<std::mutex> guard(m_directoryTreeMutex);

	DirectoryTree::ErrorCode error = DirectoryTree::ErrorCode::Success;

	const auto path = command->getPath();
 	m_directoryTree.insertItem(path, std::move(command), error);

 	if (error != DirectoryTree::ErrorCode::Success)
 	{
 		ERROR("Failed to register CUI command " << path << ", error = " << DirectoryTree::toString(error));
 	}
}

void CommandExecutor::unregisterCommand(const ICUICommand& command)
{
	std::lock_guard<std::mutex> guard(m_directoryTreeMutex);

	DirectoryTree::ErrorCode error = DirectoryTree::ErrorCode::Success;

 	m_directoryTree.deleteItem(command.getPath(), error);

 	if (error != DirectoryTree::ErrorCode::Success)
 	{
 		ERROR("Failed to unregister CUI command " << command.getPath() << ", error = " << DirectoryTree::toString(error));
 	}
}

void CommandExecutor::onConnectionOpen(TelnetConnection& connection)
{
	add(connection);

	auto& context = get(connection);

	context.finalize();
}

void CommandExecutor::onConnectionClose(TelnetConnection& connection)
{
	remove(connection);
}

void CommandExecutor::onReceivedLine(TelnetConnection& connection, const std::string& line)
{
	auto& context = get(connection);

	if (line.empty())
	{
		context.finalize();

		return;
	}

	CUICommandParser cmdLine(line);

	if (cmdLine.hasCommandPath())
	{
		DirectoryTree::ErrorCode error = DirectoryTree::ErrorCode::Success;

		std::lock_guard<std::mutex> guard(m_directoryTreeMutex);

		// try absolute
		auto item = m_directoryTree.getItem(cmdLine.getCommandPath(), error);

		if (!item)
		{
			auto& contextImpl = dynamic_cast<CUICommandContextImpl&>(context.getImpl());

			// try relative to CWD
			item = m_directoryTree.getItem(Utils::concatPath(contextImpl.getCWD(), cmdLine.getCommandPath()), error);

			if (!item)
			{
				// try relative to root
				item = m_directoryTree.getItem(Utils::concatPath(contextImpl.getRoot(), cmdLine.getCommandPath()), error);
			}
		}

		if (item)
		{
			(*item)->invoke(cmdLine, context);
		}
		else
		{
			context.finalizeError(TBOX_STRINGIFY("Command " << cmdLine.getCommandPath() << " not found."));
		}
	}
	else
	{
		context.finalizeError("Command not specified");
	}
}

void CommandExecutor::registerBaseCommands()
{
 	registerCommand(tbox::make_unique<PwdCommand>());
 	registerCommand(tbox::make_unique<CdCommand>());
	registerCommand(tbox::make_unique<LsCommand>());
 	registerCommand(tbox::make_unique<QuitCommand>());
}

void CommandExecutor::add(TelnetConnection& connection)
{
	TB_ASSERT(!exists(connection));

	m_cuiContexts[connection.getId()] = tbox::make_unique<CUICommandContext>(std::make_shared<CUICommandContextImpl>(m_directoryTree, connection));
}

void CommandExecutor::remove(TelnetConnection& connection)
{
	TB_ASSERT(exists(connection));

	m_cuiContexts.erase(connection.getId());
}

bool CommandExecutor::exists(TelnetConnection& connection) const
{
	return m_cuiContexts.find(connection.getId()) != m_cuiContexts.end();
}

CUICommandContext& CommandExecutor::get(TelnetConnection& connection)
{
	TB_ASSERT(exists(connection));

	return *m_cuiContexts[connection.getId()];
}
