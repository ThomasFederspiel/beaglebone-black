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
#include "Logger.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

// local
#include "ProtocolManager.h"

MODULE_LOG(StreamService);

using namespace commonservices;

StreamService::StreamService(const std::string& name) : AbstractService(name),
		m_protocolManager(new ProtocolManager(*this))
{
}

StreamService::~StreamService()
{
	// needed for unique_ptr
}

void StreamService::onStart(ServiceAllocator& allocator)
{
	m_protocolManager->start(allocator);
}

StreamService::StopStatus StreamService::onStop(ServiceAllocator& allocator)
{
	m_protocolManager->stop(allocator);

	return StopStatus::Done;
}

void StreamService::onMessage(const ServiceMessageBase& message)
{
	INFO("Got message " << CommonMessageTypes::toString(message.getType()));

	m_protocolManager->process(message);
}

