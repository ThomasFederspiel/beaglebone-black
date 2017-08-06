/*
 * FrameProtocol.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "FrameProtocol.h"

// standard
#include <cstdint>

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "LoggerUtil.h"

MODULE_LOG(FrameProtocol)

namespace
{

struct FrameHeader
{
	uint16_t signature;
	uint16_t length;
};

static constexpr uint16_t FrameSignature = 0xECCE;

}

FrameProtocolConnection::FrameProtocolConnection(IComConnection::ConnectionPtr connection) : m_connection(connection)
{
}

FrameProtocolConnection::IDType FrameProtocolConnection::getId() const
{
	return m_connection->getId();
}

FrameProtocolConnection::ComBuffer& FrameProtocolConnection::getRxBuffer()
{
	return m_connection->getRxBuffer();
}

bool FrameProtocolConnection::isOpen() const
{
	return m_connection->isOpen();
}

void FrameProtocolConnection::close()
{
	m_connection->close();
}

void FrameProtocolConnection::send(const std::string& data)
{
	TB_ASSERT(data.size() <= std::numeric_limits<decltype(FrameHeader::length)>::max());

	const FrameHeader header =
	{
		FrameSignature,
		static_cast<decltype(FrameHeader::length)>(data.size())
	};

	std::string frame;
	frame.resize(sizeof(header) + data.size());
	frame.assign(reinterpret_cast<const std::string::value_type*>(&header), reinterpret_cast<const std::string::value_type*>(&header) + sizeof(header));
	frame.insert(frame.end(), data.begin(), data.end());

	m_connection->send(frame);
}

void FrameProtocolConnection::send(const std::vector<uint8_t>& data)
{
	TB_ASSERT(data.size() <= std::numeric_limits<decltype(FrameHeader::length)>::max());

	const FrameHeader header =
	{
		FrameSignature,
		static_cast<decltype(FrameHeader::length)>(data.size())
	};

	std::vector<uint8_t> frame;
	frame.resize(sizeof(header) + data.size());
	frame.assign(reinterpret_cast<const uint8_t*>(&header), reinterpret_cast<const uint8_t*>(&header) + sizeof(header));
	frame.insert(frame.end(), data.begin(), data.end());

	m_connection->send(frame);
}

FrameProtocol::FrameProtocol()
{
}

void FrameProtocol::onConnectionOpen(IComConnection::ConnectionPtr connection)
{
	// Nothing to do
}

void FrameProtocol::onConnectionClose(IComConnection::ConnectionPtr connection)
{
}

std::size_t FrameProtocol::onReadProgress(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	// ;+
	INFO("onReadProgress");

	std::size_t count = 0;

	if (!error)
	{
		TB_ASSERT(connection->getRxBuffer().contentSize() == bytes_transferred);

		if (bytes_transferred < sizeof(struct FrameHeader))
		{
			count = sizeof(struct FrameHeader) - bytes_transferred;
		}
		else
		{
			auto header = reinterpret_cast<struct FrameHeader*>(connection->getRxBuffer().data().data());

			if (header->signature != FrameSignature)
			{
				TB_ERROR("Resync not implemented");
			}

			TB_ASSERT(header->signature == FrameSignature);

			if (bytes_transferred < (sizeof(struct FrameHeader) + header->length))
			{
				count = sizeof(struct FrameHeader) + header->length - bytes_transferred;
			}
			else
			{
				count = 0;
			}
		}
	}

	// ;+
	INFO("Count " << count);

	return count;
}

void FrameProtocol::onReadCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	TB_ASSERT((error != boost::system::errc::success) || (bytes_transferred != 0));

	if (!error)
	{
		TB_ASSERT(connection->getRxBuffer().contentSize() == bytes_transferred);

		const auto header = reinterpret_cast<struct FrameHeader*>(connection->getRxBuffer().data().data());

		TB_ASSERT(header->signature == FrameSignature);
		TB_ASSERT(bytes_transferred == (sizeof(struct FrameHeader) + header->length));

		INFO("MEMDUMP size = "<< bytes_transferred);
		LOG_MEMDUMP(connection->getRxBuffer().data().data(), bytes_transferred);


//		std::vector<char> line;
//		const auto& buffer = connection->getRxBuffer();
//
//		std::copy_if(buffer.begin(), buffer.begin() + bytes_transferred, std::back_inserter(line), [](const char& ch)
//				{
//					return std::isprint(ch) != 0;
//				});
//
//		std::string lineStr = std::string(line.data(), line.size());
//
//		Utils::trim(lineStr);
//
//		notify(TelnetConnection(connection), lineStr);
	}

}

void FrameProtocol::onWriteCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	// Nothing to do
}


