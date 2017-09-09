/*
 * FrameProtocol.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_FRAMEPROTOCOL_H_
#define TBOX_FRAMEPROTOCOL_H_

// standard

// boost
#include "boost/asio.hpp"

// project
#include "Publisher.h"
#include "Subscriber.h"

// local
#include "IComConnection.h"
#include "IComProtocol.h"

class FrameProtocolConnection final
{
public:

	explicit FrameProtocolConnection(IComConnection::ConnectionPtr connection);

	/// Thread safe
	void send(const uint8_t* const data, const std::size_t length)
	{
		m_connection->send(data, length);
	}

	/// Thread safe
	bool isOpen() const
	{
		return m_connection->isOpen();
	}

private:

	IComConnection::ConnectionPtr m_connection;
};

class FrameProtocol;

class IFrameProtocolSubscriber : public tbox::Subscriber<FrameProtocol, IFrameProtocolSubscriber>
{
public:

	virtual ~IFrameProtocolSubscriber() = default;

	virtual void onReceivedFrame(FrameProtocolConnection& connection, const uint8_t* const data, const std::size_t length) = 0;
};

class FrameProtocol final : public IComProtocol, public tbox::Publisher<IFrameProtocolSubscriber>
{
public:

	FrameProtocol();

private:

	void onConnectionOpen(IComConnection::ConnectionPtr connection) override;
	void onConnectionClose(IComConnection::ConnectionPtr connection) override;
	std::size_t onReadProgress(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override;
	void onReadCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override;
	void onWriteCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override;

	void notify(FrameProtocolConnection connection, const uint8_t* const data, const std::size_t length);

};

#endif /* TBOX_FRAMEPROTOCOL_H_ */
