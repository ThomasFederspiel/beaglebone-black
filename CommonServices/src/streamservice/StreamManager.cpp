/*
 * StreamManager.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "StreamManager.h"

// standard
#include <array>
#include <vector>

// project
#include "AbstractService.h"
#include "exceptionMacros.h"
#include "BinaryComSerializer.h"
#include "CommonMessageTypes.h"
#include "Logger.h"
#include "SerializeHelper.h"
#include "ServiceAllocator.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

// local
#include "StreamSelectMessage.h"

MODULE_LOG(StreamManager);

namespace
{
	static constexpr std::size_t SerializeBufferSize = 100;
	static uint8_t SerializeBuffer[SerializeBufferSize];

	struct StreamVsMessage final
	{
	public:
		StreamSelectMessage::StreamId m_streamId;
		CommonMessageTypes::Type m_messageType;
	};

	static std::array<StreamVsMessage, 2> StreamTable =
	{{
		{
			StreamSelectMessage::LeftOdometerStream,
			CommonMessageTypes::Type::LeftPropulsionOdometerMessage
		},
		{
			StreamSelectMessage::RightOdometerStream,
			CommonMessageTypes::Type::RightPropulsionOdometerMessage
		}
	}};
}

StreamManager::StreamManager(AbstractService& service) : m_service(service), m_serviceCollection(service), m_activeStreams(), m_streamContexts()
{
}

void StreamManager::start(ServiceAllocator& allocator)
{
	for (const auto& item : StreamTable)
	{
		auto service = m_serviceCollection.allocateEventService(item.m_messageType, allocator);
		TB_ASSERT(service);

		m_streamContexts.emplace_back(item.m_streamId, item.m_messageType, service);
	}
}

void StreamManager::stop(ServiceAllocator& allocator)
{
	m_serviceCollection.releaseServices(allocator);

	m_streamContexts.clear();
}

void StreamManager::addStream(FrameProtocolConnection& connection, const StreamSelectMessage& message)
{
	// ;+
	const auto messageType = getSelectedMessage(message);

	// Either inserts or returns existing value
	auto element = m_activeStreams.insert(std::make_pair(messageType, Connections()));

	element.first->second.push_back(connection);

	// New value was inserted
	if (element.second)
	{
		subscribeEvent(messageType);
	}
}

void StreamManager::process(const ServiceMessageBase& message)
{
	auto iter = m_activeStreams.find(static_cast<commonservices::CommonMessageTypes::Type>(message.getType()));

	TB_ASSERT(iter != m_activeStreams.end());

	auto& connections = iter->second;

	BinaryComSerializer serializer(SerializeBuffer, SerializeBufferSize);

	serializeMessage(message, serializer);

	for (auto iter = connections.begin(); iter != connections.end();)
	{
		if (iter->isOpen())
		{
			iter->send(SerializeBuffer, serializer.size());

			++iter;
		}
		else
		{
			iter = connections.erase(iter);
		}
	}

	if (connections.empty())
	{
		unsubscribeEvent(static_cast<CommonMessageTypes::Type>(message.getType()));

		m_activeStreams.erase(iter);
	}
}

StreamManager::StreamServiceContext::StreamServiceContext(const StreamSelectMessage::StreamId streamId,
		const CommonMessageTypes::Type messageType,
		std::shared_ptr<AbstractService> service) : m_streamId(streamId), m_messageType(messageType), m_service(service)
{
}

CommonMessageTypes::Type StreamManager::getSelectedMessage(const StreamSelectMessage& message) const
{
	CommonMessageTypes::Type type = CommonMessageTypes::EOEMessage;

	for (const auto& stream : m_streamContexts)
	{
		if (stream.m_streamId == message.getStreamId())
		{
			type = stream.m_messageType;
		}
	}

	TB_ASSERT(type != CommonMessageTypes::EOEMessage,
			"Stream " << StreamSelectMessage::toString(message.getStreamId()) << " not defined");

	return type;
}

void StreamManager::subscribeEvent(const CommonMessageTypes::Type type)
{
	for (const auto& stream : m_streamContexts)
	{
		if (stream.m_messageType == type)
		{
			stream.m_service->subscribeEvent(type, m_service);
			return;
		}
	}

	TB_ERROR("Stream for message " << CommonMessageTypes::toString(type) << " not defined");
}

void StreamManager::unsubscribeEvent(const CommonMessageTypes::Type type)
{
	for (const auto& stream : m_streamContexts)
	{
		if (stream.m_messageType == type)
		{
			stream.m_service->unsubscribeEvent(type, m_service);
			return;
		}
	}

	TB_ERROR("Stream for message " << CommonMessageTypes::toString(type) << " not defined");
}

