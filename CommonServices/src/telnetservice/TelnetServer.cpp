/*
 * TelnetServer.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "TelnetServer.h"

// standard
#include <cctype>
#include <functional>

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "LoggerUtil.h"
#include "tboxdefs.h"
#include "Utils.h"

MODULE_LOG(TelnetServer);

namespace
{
	static constexpr char CR = '\r';
	static constexpr char LF = '\n';
	static constexpr char UNDEFINED_TERMINATION_CHAR = ' ';
}

TelnetServer::TelnetServer(const std::string& name) : TCPServer(name), m_terminationChar(UNDEFINED_TERMINATION_CHAR)
{
}

std::size_t TelnetServer::onReadProgress(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	std::size_t count = 0;

	if (!error)
	{
		TB_ASSERT(connection->getRxBuffer().contentSize() == bytes_transferred);

		if (bytes_transferred > 0)
		{
			TB_ASSERT(connection->getRxBuffer().contentSize() >= bytes_transferred);

			const char lastChar = connection->getRxBuffer()[bytes_transferred - 1];

			if ((m_terminationChar == UNDEFINED_TERMINATION_CHAR) && ((lastChar == CR) || (lastChar == LF)))
			{
				m_terminationChar = lastChar;
			}

			if (lastChar == m_terminationChar)
			{
				count = 0;
			}
			else
			{
				count = 1;
			}
		}
		else
		{
			count = 1;
		}
	}

	return count;
}

void TelnetServer::onReadCompletion(IComConnection::ConnectionPtr connection, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	TB_ASSERT((error != boost::system::errc::success) || (bytes_transferred != 0));

	if (!error)
	{
		TB_ASSERT(connection->getRxBuffer().contentSize() == bytes_transferred);

		std::vector<char> line;
		const auto& buffer = connection->getRxBuffer();

		std::copy_if(buffer.begin(), buffer.begin() + bytes_transferred, std::back_inserter(line), [](const char& ch)
				{
					return std::isprint(ch) != 0;
				});

		std::string lineStr = std::string(line.data(), line.size());

		Utils::trim(lineStr);

		notify(TelnetConnection(connection), lineStr);
	}
}

void TelnetServer::onConnectionOpen(IComConnection::ConnectionPtr connection)
{
	notifySubscribers(std::function<void(ITelnetServerSubscriber& subscriber, TelnetConnection connection)>([](ITelnetServerSubscriber& subscriber,
				TelnetConnection connection)
			{
				subscriber.onConnectionOpen(connection);
			}), TelnetConnection(connection));
}

void TelnetServer::onConnectionClose(IComConnection::ConnectionPtr connection)
{
	notifySubscribers(std::function<void(ITelnetServerSubscriber& subscriber, TelnetConnection connection)>([](ITelnetServerSubscriber& subscriber,
				TelnetConnection connection)
			{
				subscriber.onConnectionClose(connection);
			}), TelnetConnection(connection));
}

void TelnetServer::notify(TelnetConnection connection, const std::string& line)
{
	notifySubscribers(std::function<void(ITelnetServerSubscriber& subscriber, TelnetConnection connection, const std::string)>([](ITelnetServerSubscriber& subscriber,
				TelnetConnection connection, const std::string line)
			{
				subscriber.onReceivedLine(connection, line);
			}), connection, line);
}

