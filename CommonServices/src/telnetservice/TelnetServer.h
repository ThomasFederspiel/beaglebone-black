/*
 * TelnetServer.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TELNETSERVER_H_
#define TELNETSERVER_H_

// standard
#include <string>

// boost

// local

// project
#include "Publisher.h"
#include "Subscriber.h"
#include "TCPConnection.h"
#include "TCPServer.h"

class TelnetServer;

class TelnetConnection final
{
public:
	using IDType = IComConnection::IDType;

	explicit TelnetConnection(IComConnection::ConnectionPtr& connection) : m_connection(connection)
	{
	}

	/// Thread safe
	IDType getId() const
	{
		return m_connection->getId();
	}

	/// Thread safe
	void send(const std::string& message)
	{
		m_connection->send(reinterpret_cast<const uint8_t*>(message.data()), message.size());
	}

	/// Thread safe
	void close()
	{
		m_connection->close();
	}

private:
	IComConnection::ConnectionPtr m_connection;
};

class ITelnetServerSubscriber : public tbox::Subscriber<TelnetServer, ITelnetServerSubscriber>
{
public:

	virtual ~ITelnetServerSubscriber() = default;

	virtual void onConnectionOpen(TelnetConnection& connection) = 0;
	virtual void onConnectionClose(TelnetConnection& connection) = 0;
	virtual void onReceivedLine(TelnetConnection& connection, const std::string& line) = 0;
};

class TelnetServer final : public TCPServer, public tbox::Publisher<ITelnetServerSubscriber>
{
public:

	explicit TelnetServer(const std::string& name);

protected:

	void onConnectionOpen(IComConnection::ConnectionPtr connection) override;
	void onConnectionClose(IComConnection::ConnectionPtr connection) override;
	std::size_t onReadProgress(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override;
	void onReadCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override;

private:
	void notify(TelnetConnection connection, const std::string& line);

	char m_terminationChar;
};

#endif /* TELNETSERVER_H_ */
