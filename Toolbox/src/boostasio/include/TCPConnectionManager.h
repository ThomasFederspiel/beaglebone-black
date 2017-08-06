/*
 * TCPConnectionManager.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_TCPCONNECTIONMANAGER_H_
#define TBOX_TCPCONNECTIONMANAGER_H_

// standard
#include <map>

// project
#include "TCPConnection.h"

class TCPConnectionManager final
{
public:

	bool exists(TCPConnection::ConnectionPtr& connection);
	void add(TCPConnection::ConnectionPtr& connection);
	void erase(TCPConnection::ConnectionPtr& connection);
	void close(TCPConnection::ConnectionPtr& connection);
	void closeAll();

private:

	std::map<TCPConnection::IDType, TCPConnection::ConnectionPtr> m_activeConnections;
};

#endif /* TBOX_TCPCONNECTIONMANAGER_H_ */
