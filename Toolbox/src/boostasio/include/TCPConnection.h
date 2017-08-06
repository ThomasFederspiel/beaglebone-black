/*
 * TCPConnection.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_TCPCONNECTION_H_
#define TBOX_TCPCONNECTION_H_

// standard
#include <cstdint>
#include <memory>
#include <vector>
#include <queue>

// boost
#include "boost/asio.hpp"

// project
#include "IdGenerator.h"

// local
#include "IComConnection.h"

class TCPServer;

class TCPConnection final : public IComConnection, public std::enable_shared_from_this<TCPConnection>
{
public:
	using ConnectionPtr = std::shared_ptr<TCPConnection>;

	static ConnectionPtr create(boost::asio::io_service& ioService, TCPServer& server);

	boost::asio::ip::tcp::socket& socket();

	bool hasTxBuffers() const;
	void popTxBuffer();
	void pushTxBuffer(ComBuffer&& buffer);
	ComBuffer& frontTxBuffer();
	void clearRxBuffer();

	// Inherited from IComConnection

	IDType getId() const override;
	bool isOpen() const override;
	ComBuffer& getRxBuffer() override;

	// Thread safe
	void close() override;

	// Thread safe
	virtual void send(const std::string& data) override;
	virtual void send(const std::vector<uint8_t>& data) override;

private:

	static constexpr std::size_t RX_BUFFER_SIZE = 512;

	explicit TCPConnection(boost::asio::io_service& ioService, TCPServer& server);

	IdGenerator<IDType> m_idGenerator;
	boost::asio::ip::tcp::socket m_socket;

	TCPServer& m_tcpServer;

	ComBuffer m_rxBuffer;
	std::queue<ComBuffer> m_txBuffers;

	IDType m_id;
};

#endif /* TBOX_TCPCONNECTION_H_ */
