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
#include "ServiceNames.h"
#include "tboxdefs.h"

MODULE_LOG(SoundService);

using namespace melodyservice;

SoundService::SoundService(const std::string& name) : AbstractService(name), m_ipcProxyService(),
	m_stateMachine()
{
}

SoundService::~SoundService()
{
}

void SoundService::onStart(ServiceAllocator& allocator)
{
	m_ipcProxyService = allocator.allocateService<IPCDeviceProxyService>(ServiceNames::Pru0ProxyService, *this);
	TB_ASSERT(m_ipcProxyService);

	m_stateMachine = tbox::make_unique<SoundServiceState>(*this, *m_ipcProxyService);

	constexpr bool reg = true;
	manageCUICommands(allocator, reg);
}

SoundService::StopStatus SoundService::onStop(ServiceAllocator& allocator)
{
	if (m_stateMachine->isTerminated())
	{
		allocator.releaseService(m_ipcProxyService, *this);

		return StopStatus::Done;
	}
	else
	{
		m_stateMachine->emit(SoundServiceState::Type::StopEvent);
	}

	constexpr bool reg = false;
	manageCUICommands(allocator, reg);

	return StopStatus::Pending;
}

void SoundService::onMessage(const ServiceMessageBase& message)
{
	INFO("Got message " << MessageTypes::toString(message.getType()));

	switch (message.getType())
	{
	case melodyservice::MessageTypes::Type::SoundMessage:
		m_stateMachine->emit(ServiceMessageFSMEvent(message));
	break;

	case pruipcservice::IPCMessageTypes::Type::IpcDeviceProxyMessageCompleted:
		m_stateMachine->emit(ServiceMessageFSMEvent(message));
		break;

	TB_DEFAULT("Unhandled value " << MessageTypes::toString(message.getType()));
	}
}

void SoundService::manageCUICommands(ServiceAllocator& allocator, const bool reg)
{
	if (reg)
	{
		allocator.registerCommand(tbox::make_unique<PlaySoundCommand>(*this));
	}
	else
	{
		allocator.unregisterCommand(PlaySoundCommand(*this));
	}
}
