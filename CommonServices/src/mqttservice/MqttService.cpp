/*
 * MqttService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "MqttService.h"

// standard

// project
#include "CommonMessageTypes.h"
#include "exceptionMacros.h"
#include "Logger.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

// local

MODULE_LOG(MqttService);

using namespace commonservices;

MqttService::MqttService(const std::string& name) : AbstractService(name)
//		m_protocolManager(new ProtocolManager(*this))
{
}

MqttService::~MqttService()
{
	// needed for unique_ptr
}

void MqttService::onStart(ServiceAllocator& allocator)
{
	//	m_protocolManager->start(allocator);
}

MqttService::StopStatus MqttService::onStop(ServiceAllocator& allocator)
{
	//	m_protocolManager->stop(allocator);

	return StopStatus::Done;
}

void MqttService::onMessage(const ServiceMessageBase& message)
{
//	m_protocolManager->process(message);
}

