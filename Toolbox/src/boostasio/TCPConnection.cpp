/*
 * TCPConnection.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "TCPConnection.h"

// standard
#include <string>

// project
#include "TCPServer.h"

TCPConnection::TCPConnection(boost::asio::io_service& ioService, TCPServer& server) : m_idGenerator(), m_socket(ioService),
	m_tcpServer(server), m_rxBuffer(RX_BUFFER_SIZE), m_id(m_idGenerator.next())
{
}

TCPConnection::ConnectionPtr TCPConnection::create(boost::asio::io_service& ioService, TCPServer& server)
{
	return std::shared_ptr<TCPConnection>(new TCPConnection(ioService, server));
}

boost::asio::ip::tcp::socket& TCPConnection::socket()
{
	return m_socket;
}

TCPConnection::IDType TCPConnection::getId() const
{
	return m_id;
}

bool TCPConnection::hasTxBuffers() const
{
	return !m_txBuffers.empty();
}

void TCPConnection::popTxBuffer()
{
	m_txBuffers.pop();
}

void TCPConnection::pushTxBuffer(ComBuffer&& buffer)
{
	m_txBuffers.push(std::move(buffer));
}

TCPConnection::ComBuffer& TCPConnection::frontTxBuffer()
{
	return m_txBuffers.front();
}

TCPConnection::ComBuffer& TCPConnection::getRxBuffer()
{
	return m_rxBuffer;
}

void TCPConnection::clearRxBuffer()
{
	m_rxBuffer.clear();
}

void TCPConnection::close()
{
	m_tcpServer.doClose(shared_from_this());
}

bool TCPConnection::isOpen() const
{
	return m_socket.is_open();
}

void TCPConnection::send(const uint8_t* const data, const std::size_t length)
{
	m_tcpServer.doSend(shared_from_this(), ComBuffer(data, length));
}

