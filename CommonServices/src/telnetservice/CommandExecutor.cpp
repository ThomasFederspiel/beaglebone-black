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
#include "TelnetServer.h"
#include "Utils.h"

MODULE_LOG(CommandExecutor);

CommandExecutor::CommandExecutor(TelnetServer& telnetServer) : m_telnetServer(telnetServer), m_directoryTree(),
		m_directoryTreeMutex(), m_cuiContexts(), m_hcuiCounter(0), m_cuiCommandMap(),
		m_cuiBatchCommandMap(), m_history()
{
	registerBaseCommands();

	m_telnetServer.subscribe(*this);
}

CommandExecutor::~CommandExecutor()
{
	m_telnetServer.unsubscribe(*this);
}

ICUIManager::hcui_t CommandExecutor::registerCommand(std::unique_ptr<ICUICommand> command)
{
	TB_ASSERT(command);

	std::lock_guard<std::mutex> guard(m_directoryTreeMutex);

	DirectoryTree::ErrorCode error = DirectoryTree::ErrorCode::Success;

	const auto path = command->getPath();
 	m_directoryTree.insertItem(path, std::move(command), error);

 	if (error == DirectoryTree::ErrorCode::Success)
 	{
 	 	++m_hcuiCounter;

 		TB_ASSERT(m_cuiCommandMap.find(m_hcuiCounter) == m_cuiCommandMap.end());

 	 	m_cuiCommandMap[m_hcuiCounter] = path;
 	}
 	else
 	{
		ERROR("Failed to register CUI command " << path << ", error = " << DirectoryTree::toString(error));

		return 0;
 	}

 	return m_hcuiCounter;
}

ICUIManager::hcui_t CommandExecutor::registerCommands(std::vector<std::unique_ptr<ICUICommand>>& commands)
{
	TB_ASSERT(commands.size());

	ICUIManager::hcui_t groupHcui = 0;

	for (auto& command : commands)
	{
		TB_ASSERT(command);

		const auto hcui = registerCommand(std::move(command));

		if (hcui != 0)
		{
			if (groupHcui == 0)
			{
				groupHcui = hcui;

		 		TB_ASSERT(m_cuiBatchCommandMap.find(groupHcui) == m_cuiBatchCommandMap.end());
			}

	 	 	auto batch = m_cuiBatchCommandMap[groupHcui];
			batch.push_back(hcui);
		}
		else
		{
			unregisterCommands(groupHcui);

			return 0;
		}
	}

 	return groupHcui;
}

bool CommandExecutor::unregisterCommands(const ICUIManager::hcui_t handle)
{
	auto batch = m_cuiBatchCommandMap.find(handle);

	if (batch != m_cuiBatchCommandMap.end())
	{
		for (auto& handle : batch->second)
		{
			unregisterCommand(handle);
		}

		m_cuiBatchCommandMap.erase(batch);

		return true;
	}

	return false;
}

void CommandExecutor::unregisterCommand(const ICUIManager::hcui_t handle)
{
	std::lock_guard<std::mutex> guard(m_directoryTreeMutex);

	if (!unregisterCommands(handle))
	{
		DirectoryTree::ErrorCode error = DirectoryTree::ErrorCode::Success;

		auto iter = m_cuiCommandMap.find(handle);

		if (iter == m_cuiCommandMap.end())
		{
			ERROR("Failed to unregister CUI command with handle " << handle);

			return;
		}

		m_directoryTree.deleteItem(iter->second, error);

		if (error != DirectoryTree::ErrorCode::Success)
		{
			ERROR("Failed to unregister CUI command " << iter->second << ", error = " << DirectoryTree::toString(error));
		}
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

	// TODO: Handle arrow keys, echo history, if line empty and history active run
	// history command

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

		// TODO: Add too history

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
 	registerCommand(std::make_unique<PwdCommand>());
 	registerCommand(std::make_unique<CdCommand>());
	registerCommand(std::make_unique<LsCommand>());
 	registerCommand(std::make_unique<QuitCommand>());
}

void CommandExecutor::add(TelnetConnection& connection)
{
	TB_ASSERT(!exists(connection));

	m_cuiContexts[connection.getId()] = std::make_unique<CUICommandContext>(std::make_shared<CUICommandContextImpl>(m_directoryTree, connection));
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
