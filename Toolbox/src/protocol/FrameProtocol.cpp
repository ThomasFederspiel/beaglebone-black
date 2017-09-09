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

		notify(FrameProtocolConnection(connection), connection->getRxBuffer().data().data() + sizeof(struct FrameHeader),
				connection->getRxBuffer().contentSize() - sizeof(struct FrameHeader));
	}
}

void FrameProtocol::onWriteCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	// Nothing to do
}

void FrameProtocol::notify(FrameProtocolConnection connection, const uint8_t* const data, const std::size_t length)
{
	notifySubscribers(std::function<void(IFrameProtocolSubscriber& subscriber, FrameProtocolConnection connection, const uint8_t * const data, const std::size_t length)>([](IFrameProtocolSubscriber& subscriber,
			FrameProtocolConnection connection, const uint8_t * const data, const std::size_t length)
	{
		subscriber.onReceivedFrame(connection, data, length);
	}), connection, data, length);
}


