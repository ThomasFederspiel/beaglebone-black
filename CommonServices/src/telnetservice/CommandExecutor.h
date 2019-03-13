/*
 * CommandExecutor.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef COMMANDEXECUTOR_H_
#define COMMANDEXECUTOR_H_

// standard
#include <map>
#include <memory>
#include <mutex>

// boost
#include "boost/asio.hpp"

// project
#include "DirectoryTree.h"
#include "ICUICommand.h"
#include "ICUIManager.h"
#include "History.h"
#include "TelnetServer.h"

class CUICommandContext;

class CommandExecutor final : public ITelnetServerSubscriber, public ICUIManager
{
public:
	friend class CUICommandContextImpl;
	friend class CUICommandContextBase;

	explicit CommandExecutor(TelnetServer& telnetServer);
	~CommandExecutor();

	/// Implementing ITelnetServerSubscriber
	void onConnectionOpen(TelnetConnection& connection) override;
	void onConnectionClose(TelnetConnection& connection) override;
	void onReceivedLine(TelnetConnection& connection, const std::string& line) override;

	/// Implementing ICUIManager
	ICUIManager::hcui_t registerCommand(std::unique_ptr<ICUICommand> command) override;
	ICUIManager::hcui_t registerCommands(std::vector<std::unique_ptr<ICUICommand>>& commands) override;
	void unregisterCommand(const ICUIManager::hcui_t handle) override;

private:

	using TreeItem = std::unique_ptr<ICUICommand>;
	using DirectoryTree = tbox::DirectoryTree<TreeItem>;

	void registerBaseCommands();

	bool unregisterCommands(const ICUIManager::hcui_t handle);

	void add(TelnetConnection& connection);
	void remove(TelnetConnection& connection);
	bool exists(TelnetConnection& connection) const;
	CUICommandContext& get(TelnetConnection& connection);

	TelnetServer& m_telnetServer;
	DirectoryTree m_directoryTree;
	std::mutex m_directoryTreeMutex;
	std::map<TelnetConnection::IDType, std::unique_ptr<CUICommandContext>> m_cuiContexts;

	ICUIManager::hcui_t m_hcuiCounter;
	std::map<ICUIManager::hcui_t, std::string> m_cuiCommandMap;
	std::map<ICUIManager::hcui_t, std::vector<ICUIManager::hcui_t>> m_cuiBatchCommandMap;
	tbox::History<std::string, 10> m_history;
};

#endif /* COMMANDEXECUTOR_H_ */
