/*
 * TCPConnectionManager.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "TCPConnectionManager.h"

// boost
#include "boost/system/error_code.hpp"

// project
#include "exceptionMacros.h"
#include "Logger.h"

MODULE_LOG(TCPConnectionManager);

void TCPConnectionManager::add(TCPConnection::ConnectionPtr& connection)
{
	TB_ASSERT(connection);
	TB_ASSERT(m_activeConnections.find(connection->getId()) == m_activeConnections.end());

	m_activeConnections[connection->getId()] = connection;
}

bool TCPConnectionManager::exists(TCPConnection::ConnectionPtr& connection)
{
	TB_ASSERT(connection);

	return m_activeConnections.find(connection->getId()) != m_activeConnections.end();
}

void TCPConnectionManager::erase(TCPConnection::ConnectionPtr& connection)
{
	TB_ASSERT(connection);
	auto it = m_activeConnections.find(connection->getId());

	if (it != m_activeConnections.end())
	{
		m_activeConnections.erase(it);
	}
}

void TCPConnectionManager::close(TCPConnection::ConnectionPtr& connection)
{
	TB_ASSERT(connection);

	boost::system::error_code error;

	connection->socket().close(error);

	if (error)
	{
		ERROR("Failed closing socket, error = " << error.message());
	}

}

void TCPConnectionManager::closeAll()
{
	for (auto& item : m_activeConnections)
	{
		close(item.second);
	}

	m_activeConnections.clear();
}
