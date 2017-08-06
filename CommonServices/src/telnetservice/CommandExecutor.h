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
	void registerCommand(std::unique_ptr<ICUICommand> command) override;
	void unregisterCommand(const ICUICommand& command) override;

private:

	using TreeItem = std::unique_ptr<ICUICommand>;
	using DirectoryTree = tbox::DirectoryTree<TreeItem>;

	void registerBaseCommands();

	void add(TelnetConnection& connection);
	void remove(TelnetConnection& connection);
	bool exists(TelnetConnection& connection) const;
	CUICommandContext& get(TelnetConnection& connection);

	TelnetServer& m_telnetServer;
	DirectoryTree m_directoryTree;
	std::mutex m_directoryTreeMutex;
	std::map<TelnetConnection::IDType, std::unique_ptr<CUICommandContext>> m_cuiContexts;
};

#endif /* COMMANDEXECUTOR_H_ */
