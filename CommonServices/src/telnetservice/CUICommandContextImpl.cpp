/*
 * CUICommandContextImpl.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "CUICommandContextImpl.h"

// project
#include "ICUICommandContext.h"
#include "tboxdefs.h"
#include "Utils.h"

namespace
{
static const std::string CRLF("\r\n");
static const std::string Root("/");
}

CUICommandContextBase::CUICommandContextBase(const CommandExecutor::DirectoryTree& directoryTree) : m_directoryTree(directoryTree),
	m_cwd("/")
{
}

const std::string& CUICommandContextBase::getRoot() const
{
	return Root;
}

const CommandExecutor::DirectoryTree& CUICommandContextBase::getTree() const
{
	return m_directoryTree;
}

void CUICommandContextBase::setCWD(const std::string& cwd)
{
	m_cwd = cwd;
}

const std::string& CUICommandContextBase::getCWD() const
{
	return m_cwd;
}

CUICommandContextImpl::CUICommandContextImpl(const CommandExecutor::DirectoryTree& directoryTree, TelnetConnection& connection) : CUICommandContextBase(directoryTree),
	m_connection(connection), m_output(new std::ostringstream())
{
}

const std::string& CUICommandContextImpl::newline() const
{
	return CRLF;
}

std::ostringstream& CUICommandContextImpl::output()
{
	return *m_output;
}

void CUICommandContextImpl::finalizeError(const std::string&& message)
{
	output() << "Error: " << message;

	finalize();
}

void CUICommandContextImpl::finalize()
{
	auto str = m_output->str();

	if (!str.empty())
	{
		str = Utils::ensureEnd(str, CRLF);
	}

	m_connection.send(str + getCWD() + ">");

	// clear
	m_output = tbox::make_unique<std::ostringstream>();
}

void CUICommandContextImpl::shutdown()
{
	m_connection.close();
}
