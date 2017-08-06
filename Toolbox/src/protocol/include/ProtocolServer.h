/*
 * ProtocolServer.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_PROTOCOLSERVER_H_
#define TBOX_PROTOCOLSERVER_H_

// standard
#include <memory>
#include <string>

// boost
#include "boost/asio.hpp"

// project
#include "exceptionMacros.h"
#include "IComConnection.h"

// local
#include "IComProtocol.h"

template<typename ComServerImpl>
class ProtocolServer : public ComServerImpl
{
public:

	explicit ProtocolServer(const std::string& name, std::unique_ptr<IComProtocol> protocol) : ComServerImpl(name), m_protocol(std::move(protocol))
	{
		TB_ASSERT(m_protocol);
	}

	~ProtocolServer()
	{
		// needed for unique_ptr
	}

	IComProtocol& getprotocol()
	{
		return *m_protocol;
	}

protected:

	void onConnectionOpen(IComConnection::ConnectionPtr connection) override
	{
		m_protocol->onConnectionOpen(connection);
	}

	void onConnectionClose(IComConnection::ConnectionPtr connection) override
	{
		m_protocol->onConnectionOpen(connection);
	}

	std::size_t onReadProgress(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override
	{
		return m_protocol->onReadProgress(connection, error, bytes_transferred);
	}

	void onReadCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override
	{
		m_protocol->onReadCompletion(connection, error, bytes_transferred);
	}

	void onWriteCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override
	{
		m_protocol->onWriteCompletion(connection, error, bytes_transferred);
	}

private:
	std::unique_ptr<IComProtocol> m_protocol;
};

#endif /* TBOX_PROTOCOLSERVER_H_ */
