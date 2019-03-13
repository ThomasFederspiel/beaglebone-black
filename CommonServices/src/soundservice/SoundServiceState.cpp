/*
 * SoundServiceState.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "SoundServiceState.h"

// standard

// project
#include "exceptionMacros.h"
#include "FSMEventTypes.h"
#include "IPCMessageTypes.h"
#include "IPCDeviceProxyMessage.h"
#include "Logger.h"
#include "PlayerQueue.h"
#include "PlayerQueueProxies.h"
#include "ServiceMessageFSMEvent.h"
#include "SoundMessage.h"
#include "ThreadFactory.h"
#include "ThreadWrapper.h"

MODULE_LOG(SoundServiceState);

namespace
{
static constexpr PwmssDeviceEnum SpeakerEPwmDevice = PWMSS_DEV_3;
static constexpr EPwmChannelEnum SpeakerEPwmChannel = PWM_CH_A;
}

FSM_TABLE_BEGIN(SoundServiceState)
	FSM_STATE(startState),
	FSM_STATE(activeState),
	FSM_STATE(waitOnFinishState),
	FSM_STATE(stopState)
FSM_TABLE_END()

SoundServiceState::SoundServiceState(IMessageReceiver& receiver, IPCDeviceProxyService& m_proxyService) : FSM_CONSTRUCTOR(startState),
	m_messageReceiver(receiver), m_ipcProxyService(m_proxyService), m_epwmProxy(), m_playerQueue(), m_completionCount(0)
{
	init();
}

SoundServiceState::~SoundServiceState()
{
}

void SoundServiceState::startState(const FSMEvent& event)
{
	switch (event.getType())
	{
	case FSMEventTypes::Type::EnterEvent:
		startPlayerQueue();

		FSM_TRANSITION(activeState);
		break;

	case FSMEventTypes::Type::ExitEvent:
		break;
	}
}

void SoundServiceState::activeState(const FSMEvent& event)
{
	switch (event.getType())
	{
	case FSMEventTypes::Type::EnterEvent:
		break;

	case FSMEventTypes::Type::ExitEvent:
		break;

	case commonservices::CommonEventTypes::Type::SoundMessageEvent:
		m_playerQueue->add(SoundMessage(event.getCasted<ServiceMessageFSMEvent>().getCastedPayload<SoundMessage>()));
		break;

	case commonservices::CommonEventTypes::Type::IpcDeviceProxyMessageCompletedEvent:
	{
		// One of them will react the other will ignore
		m_epwmProxy->completion();
	}
	break;

	case SoundServiceState::Type::StopEvent:

		m_playerQueue->shutdown();
		m_epwmProxy->terminate();

		FSM_TRANSITION(waitOnFinishState);
		break;
	}
}

void SoundServiceState::waitOnFinishState(const FSMEvent& event)
{
	switch (event.getType())
	{
	case FSMEventTypes::Type::EnterEvent:
		break;

	case FSMEventTypes::Type::ExitEvent:
		break;

	case SoundServiceState::Type::StopEvent:

		if (m_playerQueue->isShutdown())
		{
			m_epwmProxy->getEPwmProxy().close();

			FSM_TRANSITION(stopState);
		}

		break;
	}
}

void SoundServiceState::stopState(const FSMEvent& event)
{
	switch (event.getType())
	{
	case FSMEventTypes::Type::EnterEvent:
		m_completionCount = 0;
		break;

	case FSMEventTypes::Type::ExitEvent:
		break;

	case SoundServiceState::Type::StopEvent:
		break;

	case commonservices::CommonEventTypes::Type::IpcDeviceProxyMessageCompletedEvent:
		++m_completionCount;

		if (m_completionCount == 1)
		{
			terminate();
		}

		break;
	}
}

void SoundServiceState::startPlayerQueue()
{
	m_epwmProxy = std::make_unique<PlayerQueueEPwmProxy>(m_messageReceiver, m_ipcProxyService, SpeakerEPwmDevice, SpeakerEPwmChannel);

	m_playerQueue = std::make_unique<PlayerQueue>(*m_epwmProxy);

	ThreadFactory::thread_t thread = ThreadFactory::instance().createThread(*m_playerQueue);
	thread->start();
}
