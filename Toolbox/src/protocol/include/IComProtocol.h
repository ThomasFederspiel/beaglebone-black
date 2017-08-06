/*
 * IComProtocol.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_ICOMPROTOCOL_H_
#define TBOX_ICOMPROTOCOL_H_

// standard

// boost
#include "boost/asio.hpp"

// project
#include "IComConnection.h"

class IComProtocol
{
public:

	virtual ~IComProtocol()
	{
	}

	virtual void onConnectionOpen(IComConnection::ConnectionPtr connection) = 0;
	virtual void onConnectionClose(IComConnection::ConnectionPtr connection) = 0;
	virtual std::size_t onReadProgress(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) = 0;
	virtual void onReadCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) = 0;
	virtual void onWriteCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) = 0;
};

#endif /* TBOX_ICOMPROTOCOL_H_ */
