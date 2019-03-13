/*
 * ProtocolManager.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "ProtocolManager.h"

// project
#include "exceptionMacros.h"
#include "BinaryComSerializer.h"
#include "Logger.h"
#include "ProtocolServer.h"
#include "ServiceAllocator.h"
#include "ServiceMessageBase.h"
#include "TCPServer.h"

// local
#include "StreamSelectMessage.h"

MODULE_LOG(ProtocolManager);

namespace
{
	static constexpr int StreamPort = 5555;
	static std::string StreamServerName = "StreamServer";
}

ProtocolManager::ProtocolManager(AbstractService& service) :
		m_protocolServer(new ProtocolServer<TCPServer>(StreamServerName, std::make_unique<FrameProtocol>())),
		m_switchboard(), m_streamManager(service)
{
}

ProtocolManager::~ProtocolManager()
{
	// needed for unique_ptr
}

void ProtocolManager::start(ServiceAllocator& allocator)
{
	registerProtocolMessages();

	m_streamManager.start(allocator);

	m_protocolServer->getProtocol<FrameProtocol>().subscribe(*this);
	m_protocolServer->start(StreamPort);
}

void ProtocolManager::stop(ServiceAllocator& allocator)
{
	m_streamManager.stop(allocator);

	m_protocolServer->stop();
	m_protocolServer->getProtocol<FrameProtocol>().unsubscribe(*this);
}

void ProtocolManager::registerProtocolMessages()
{
	m_switchboard.registerAction<StreamSelectMessage>(StreamSelectMessage::ID,
			std::bind(&ProtocolManager::handleSelectMessage, this, std::placeholders::_1,
			std::placeholders::_2));
}

void ProtocolManager::onReceivedFrame(FrameProtocolConnection& connection, const uint8_t* const data, const std::size_t length)
{
	BinaryComSerializer serializer(data, length);

	INFO("onReceivedFrame");

	m_switchboard.invoke(StreamSelectMessage::ID, connection, serializer);
}

void ProtocolManager::handleSelectMessage(FrameProtocolConnection& connection, const StreamSelectMessage& message)
{
	INFO("Received StreamSelectMessage");

	m_streamManager.addStream(connection, message);
}

void ProtocolManager::process(const ServiceMessageBase& message)
{
	m_streamManager.process(message);
}

