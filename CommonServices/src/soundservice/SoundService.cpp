/*
 * SoundService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "SoundService.h"

// standard

// local
#include "SoundServiceState.h"

// project
#include "CUICommands.h"
#include "exceptionMacros.h"
#include "IPCDeviceProxyMessage.h"
#include "IPCDeviceProxyService.h"
#include "IPCMessageTypes.h"
#include "IPCResponseBase.h"
#include "Logger.h"
#include "ServiceMessageBase.h"
#include "ServiceMessageFSMEvent.h"
#include "tboxdefs.h"

MODULE_LOG(SoundService);

namespace commonservices
{
extern const char* Pru0ProxyService;

};

SoundService::SoundService(const std::string& name) : AbstractService(name), m_ipcProxyService(),
	m_stateMachine()
{
}

SoundService::~SoundService()
{
}

void SoundService::onStart(ServiceAllocator& allocator)
{
	m_ipcProxyService = allocator.allocateService<IPCDeviceProxyService>(commonservices::Pru0ProxyService, *this);
	TB_ASSERT(m_ipcProxyService);

	m_stateMachine = tbox::make_unique<SoundServiceState>(*this, *m_ipcProxyService);

	CUICommands::registerCUICommands(allocator, *this);
}

SoundService::StopStatus SoundService::onStop(ServiceAllocator& allocator)
{
	if (m_stateMachine->isTerminated())
	{
		allocator.releaseService(m_ipcProxyService, *this);

		CUICommands::unregisterCUICommands(allocator, *this);

		return StopStatus::Done;
	}
	else
	{
		m_stateMachine->emit(SoundServiceState::Type::StopEvent);
	}

	return StopStatus::Pending;
}

void SoundService::onMessage(const ServiceMessageBase& message)
{
	INFO("Got message " << commonservices::CommonMessageTypes::toString(message.getType()));

	switch (message.getType())
	{
	case commonservices::CommonMessageTypes::Type::SoundMessage:
		m_stateMachine->emit(ServiceMessageFSMEvent(message));
	break;

	case pruipcservice::IPCMessageTypes::Type::IPCDeviceProxyMessageCompleted:
		m_stateMachine->emit(ServiceMessageFSMEvent(message));
		break;

	TB_DEFAULT(commonservices::CommonMessageTypes::toString(message.getType()));
	}
}

