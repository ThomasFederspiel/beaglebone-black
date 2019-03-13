/*
 * TCPServer.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "TCPServer.h"

// standard

// boost
#include <boost/bind.hpp>

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "stdExtension.h"
#include "TCPConnection.h"
#include "ThreadFactory.h"
#include "ThreadWrapper.h"

MODULE_LOG(TCPServer);

TCPServer::TCPServer(const std::string& name) : AbstractRunnable(), m_name(name), m_ioService(), m_started(false),
		m_acceptor()
{
}

const std::string& TCPServer::name() const
{
	return m_name;
}

void TCPServer::start(const int port)
{
	TB_ASSERT(!m_started);

	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
	m_acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(m_ioService, endpoint);

	doAccept();

	// io service need to be prepared be fore stating else io service will exit
	ThreadFactory::thread_t thread = ThreadFactory::instance().createThread(*this);
	thread->start();

	m_started = true;
}

void TCPServer::stop()
{
	TB_ASSERT(m_started);

	m_ioService.stop();

	m_started = false;
}

void TCPServer::shutdown()
{
	boost::system::error_code error;
	m_acceptor->close(error);

	if (error)
	{
		ERROR("Failed closing acceptor, error = " << error.message());
	}

	m_connectionManager.closeAll();

	onShutdown();
}

void TCPServer::doAccept()
{
	auto connection = TCPConnection::create(m_ioService, *this);

	m_acceptor->async_accept(connection->socket(),
	        boost::bind(&TCPServer::onAccept, this, connection,
	          boost::asio::placeholders::error));
}

void TCPServer::onAccept(TCPConnection::ConnectionPtr connection, const boost::system::error_code& error)
{
    if (error)
    {
		ERROR("Accept failed, error = " << error.message());
    }
    else
    {
		onConnectionOpenImpl(connection);
    }

    doAccept();
}

void TCPServer::onConnectionOpenImpl(TCPConnection::ConnectionPtr connection)
{
	m_connectionManager.add(connection);

	onConnectionOpen(connection);

	doRead(connection);
}

void TCPServer::doClose(TCPConnection::ConnectionPtr connection)
{
	if (connection->isOpen())
	{
		m_ioService.post(boost::bind(&TCPServer::doCloseImpl, this, connection));
	}
}

void TCPServer::doCloseImpl(TCPConnection::ConnectionPtr connection)
{
	connection->socket().close();
}

void TCPServer::doSend(TCPConnection::ConnectionPtr connection, TCPConnection::ComBuffer buffer)
{
	if (connection->isOpen())
	{
		m_ioService.post(boost::bind(&TCPServer::doSendImpl, this, connection, buffer));
	}
}

void TCPServer::doSendImpl(TCPConnection::ConnectionPtr connection, TCPConnection::ComBuffer buffer)
{
	const bool startWrite = !connection->hasTxBuffers();

	connection->pushTxBuffer(std::move(buffer));

	if (startWrite)
	{
		auto& buffer = connection->frontTxBuffer();

		boost::asio::async_write(connection->socket(),
				boost::asio::buffer(buffer.data(), buffer.contentSize()),
				boost::bind(&TCPServer::onWriteCompletionImpl, this, connection, boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
	}
}

void TCPServer::doRead(TCPConnection::ConnectionPtr connection)
{
	connection->getRxBuffer().setContentSize(0);

	boost::asio::async_read(connection->socket(),
	    boost::asio::buffer(connection->getRxBuffer().data()),
	    boost::bind(&TCPServer::onReadProgressImpl, this, connection, boost::asio::placeholders::error,
	    		boost::asio::placeholders::bytes_transferred),
	    boost::bind(&TCPServer::onReadCompletionImpl, this, connection, boost::asio::placeholders::error,
	    		boost::asio::placeholders::bytes_transferred));
}

std::size_t TCPServer::onReadProgressImpl(TCPConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	std::size_t count = 0;

	if (connection->isOpen())
	{
		connection->getRxBuffer().setContentSize(bytes_transferred);

		count = onReadProgress(connection, error, bytes_transferred);

		if (error && (error != boost::asio::error::operation_aborted))
		{
			m_connectionManager.erase(connection);

			count = 0;
		}
	}
	else
	{
		onConnectionCloseImpl(connection);
	}

	return count;
}

void TCPServer::onReadCompletionImpl(TCPConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (connection->isOpen())
	{
		connection->getRxBuffer().setContentSize(bytes_transferred);

		onReadCompletion(connection, error, bytes_transferred);

		if (!error)
		{
			doRead(connection);
		}
		else if (error != boost::asio::error::operation_aborted)
		{
			doCloseImpl(connection);
			onConnectionCloseImpl(connection);
		}
	}
	else
	{
		onConnectionCloseImpl(connection);
	}
}

void TCPServer::doWrite(TCPConnection::ConnectionPtr connection)
{
	if (connection->hasTxBuffers())
	{
		auto& buffer = connection->frontTxBuffer();

		boost::asio::async_write(connection->socket(),
				boost::asio::buffer(buffer.data(), buffer.contentSize()),
				boost::bind(&TCPServer::onWriteCompletionImpl, this, connection, boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
	}
}

void TCPServer::onWriteCompletionImpl(TCPConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	TB_ASSERT(connection->hasTxBuffers());

	connection->popTxBuffer();

	onWriteCompletion(connection, error, bytes_transferred);

	if (!error)
	{
		doWrite(connection);
	}
	else if (error != boost::asio::error::operation_aborted)
	{
		m_connectionManager.erase(connection);
	}
}

void TCPServer::onConnectionCloseImpl(TCPConnection::ConnectionPtr connection)
{
	if (m_connectionManager.exists(connection))
	{
		onConnectionClose(connection);

		m_connectionManager.erase(connection);
	}
}

void TCPServer::operator()()
{
	m_ioService.run();

	shutdown();
}




