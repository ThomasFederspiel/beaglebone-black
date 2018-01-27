/*
 * NetManagerService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "NetManagerService.h"

// standard

// project
#include "CommonMessageTypes.h"
#include "exceptionMacros.h"
#include "Logger.h"
#include "NetManageMessage.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

MODULE_LOG(NetManagerService);

using namespace commonservices;

namespace commonservices
{
	extern const char* Pru0ProxyService;
	extern const char* Pru1ProxyService;
};

NetManagerService::NetManagerService(const std::string& name) : AbstractService(name)
{
}

void NetManagerService::onStart(ServiceAllocator& allocator)
{
}

NetManagerService::StopStatus NetManagerService::onStop(ServiceAllocator& allocator)
{
	return StopStatus::Done;
}

void NetManagerService::onMessage(const ServiceMessageBase& message)
{
	INFO("Got message " << CommonMessageTypes::toString(message.getType()));

	switch (message.getType())
	{
	case CommonMessageTypes::NetManageMessage:
		executeAction(message.getCasted<NetManageMessage>());
		break;

 	TB_DEFAULT(CommonMessageTypes::toString(message.getType()));
	}
}

void NetManagerService::executeAction(const NetManageMessage& message)
{
	switch (message.getAction())
	{
	case NetManageMessage::Action::UndefinedAction:
		break;

	case NetManageMessage::Action::IfUpAction:
		break;

	case NetManageMessage::Action::IfDownAction:
		break;

	default:
		break;
	}
}
