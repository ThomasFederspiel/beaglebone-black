/*
 * IComServer.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_ICOMSERVER_H_
#define TBOX_ICOMSERVER_H_

// boost
#include "boost/asio.hpp"

// local
#include "IComConnection.h"

class IComServer
{
public:

	virtual ~IComServer()
	{
	}

	virtual void onConnectionOpen(IComConnection::ConnectionPtr connection) = 0;
	virtual void onConnectionClose(IComConnection::ConnectionPtr connection) = 0;
	virtual std::size_t onReadProgress(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) = 0;
	virtual void onReadCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) = 0;
	virtual void onWriteCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) = 0;
	virtual void onShutdown() = 0;
};

#endif /* TBOX_ICOMSERVER_H_ */
