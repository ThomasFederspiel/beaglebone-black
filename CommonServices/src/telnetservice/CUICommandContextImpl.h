/*
 * CUICommandContextImpl.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef CUICOMMANDCONTEXTIMPL_H_
#define CUICOMMANDCONTEXTIMPL_H_

// standard
#include <memory>
#include <string>
#include <sstream>

// project
#include "CommandExecutor.h"
#include "ICUICommandContext.h"
#include "TelnetServer.h"

class CUICommandContextBase
{
public:

	using DirectoryTree = CommandExecutor::DirectoryTree;

	explicit CUICommandContextBase(const DirectoryTree& directoryTree);

	const std::string& getRoot() const;
	void setCWD(const std::string& cwd);
	const std::string& getCWD() const;
	const DirectoryTree& getTree() const;

private:
	const DirectoryTree& m_directoryTree;
	std::string m_cwd;
};

class CUICommandContextImpl final : public CUICommandContextBase, public ICUICommandContext
{
public:

	using DirectoryTree = CommandExecutor::DirectoryTree;

	explicit CUICommandContextImpl(const DirectoryTree& directoryTree, TelnetConnection& connection);

	const std::string& newline() const override;
	std::ostringstream& output() override;
	void finalizeError(const std::string&& message) override;
	void finalize() override;
	void shutdown();

private:
	std::string convertNewline(const std::string& str) const;

	TelnetConnection m_connection;
	std::unique_ptr<std::ostringstream> m_output;
};

#endif /* CUICOMMANDCONTEXTIMPL_H_ */
