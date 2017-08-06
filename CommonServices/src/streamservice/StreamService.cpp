/*
 * StreamService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "StreamService.h"

// standard

// project
#include "CommonMessageTypes.h"
#include "exceptionMacros.h"
#include "FrameProtocol.h"
#include "Logger.h"
#include "ProtocolServer.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"
#include "TCPServer.h"

// local

MODULE_LOG(StreamService);

using namespace commonservices;

namespace
{
	static constexpr int StreamPort = 5555;
	static std::string StreamServerName = "StreamServer";
}

StreamService::StreamService(const std::string& name) : AbstractService(name),
		m_protocolServer(new ProtocolServer<TCPServer>(StreamServerName, tbox::make_unique<FrameProtocol>()))
{
}

StreamService::~StreamService()
{
	// needed for unique_ptr
}

void StreamService::onStart(ServiceAllocator& allocator)
{
	m_protocolServer->start(StreamPort);
}

StreamService::StopStatus StreamService::onStop(ServiceAllocator& allocator)
{
	m_protocolServer->stop();

	return StopStatus::Done;
}

void StreamService::onMessage(const ServiceMessageBase& message)
{
	INFO("Got message " << CommonMessageTypes::toString(message.getType()));

	switch (message.getType())
	{
	TB_DEFAULT("Unhandled value " << CommonMessageTypes::toString(message.getType()));
	}
}

