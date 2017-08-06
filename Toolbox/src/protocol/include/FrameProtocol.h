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

// local
#include "IComConnection.h"
#include "IComProtocol.h"

class FrameProtocolConnection final : public IComConnection
{
public:

	explicit FrameProtocolConnection(IComConnection::ConnectionPtr connection);

	IDType getId() const override;
	ComBuffer& getRxBuffer() override;
	bool isOpen() const override;
	void close() override;
	void send(const std::string& data) override;
	void send(const std::vector<uint8_t>& data) override;

private:

	IComConnection::ConnectionPtr m_connection;
};

class FrameProtocol final : public IComProtocol
{
public:

	explicit FrameProtocol();

	void onConnectionOpen(IComConnection::ConnectionPtr connection) override;
	void onConnectionClose(IComConnection::ConnectionPtr connection) override;
	std::size_t onReadProgress(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override;
	void onReadCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override;
	void onWriteCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred) override;

private:

};

#endif /* TBOX_PROTOCOLSERVER_H_ */
