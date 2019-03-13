/*
 * TCPServer.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_TCPSERVER_H_
#define TBOX_TCPSERVER_H_

// standard
#include <memory>
#include <string>

// boost
#include "boost/asio.hpp"

// project
#include "AbstractRunnable.h"
#include "TCPConnectionManager.h"

// local
#include "IComConnection.h"
#include "IComServer.h"

class TCPConnection;

class TCPServer : public AbstractRunnable, public IComServer
{
public:
	friend class TCPConnection;

	explicit TCPServer(const std::string& name);

	const std::string& name() const override;
	void start(const int port);
	void stop();
	void shutdown();

protected:

	virtual void onConnectionOpen(IComConnection::ConnectionPtr connection) override
	{
	}

	virtual void onConnectionClose(IComConnection::ConnectionPtr connection) override
	{
	}

	virtual void onWriteCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
	{
	}

	virtual void onShutdown() override
	{
	}

private:

	void doSend(TCPConnection::ConnectionPtr connection, TCPConnection::ComBuffer buffer);
	void doSendImpl(TCPConnection::ConnectionPtr connection, TCPConnection::ComBuffer buffer);

	void doClose(TCPConnection::ConnectionPtr connection);
	void doCloseImpl(TCPConnection::ConnectionPtr connection);

	void closeDown();
	void doAccept();
	void onAccept(TCPConnection::ConnectionPtr connection, const boost::system::error_code& error);
	void onConnectionOpenImpl(TCPConnection::ConnectionPtr connection);
	std::size_t onReadProgressImpl(TCPConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred);
	void onReadCompletionImpl(TCPConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred);
	void doRead(TCPConnection::ConnectionPtr connection);
	void operator()() override;
	void doWrite(TCPConnection::ConnectionPtr connection);
	void onWriteCompletionImpl(TCPConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred);
	void onConnectionCloseImpl(TCPConnection::ConnectionPtr connection);

	std::string m_name;
	boost::asio::io_service m_ioService;
	bool m_started;

	std::unique_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;

	TCPConnectionManager m_connectionManager;
};

#endif /* TBOX_TCPSERVER_H_ */
