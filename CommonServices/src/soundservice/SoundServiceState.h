/*
 * SoundServiceState.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef SOUNDSERVICESTATE_H_
#define SOUNDSERVICESTATE_H_

// standard
#include <memory>

// local

// project
#include "CommonEventTypes.h"
#include "IMessageReceiver.h"
#include "IPCDeviceProxyService.h"
#include "StateMachine.h"

class FSMEvent;
class PlayerQueue;
class PlayerQueueEPwmProxy;

class SoundServiceState final : public StateMachine<SoundServiceState>
{
public:

	enum Type
	{
		StopEvent = commonservices::CommonEventTypes::Type::EOEEvent
	};

	explicit SoundServiceState(IMessageReceiver& receiver, IPCDeviceProxyService& m_proxyService);

	// Declared as to avoid unique_ptr incomplete type
	~SoundServiceState();

private:

	FSM_DECL_TABLE();

	void startState(const FSMEvent& event);
	void activeState(const FSMEvent& event);
	void waitOnFinishState(const FSMEvent& event);
	void stopState(const FSMEvent& event);

	void startPlayerQueue();

	IMessageReceiver& m_messageReceiver;
	IPCDeviceProxyService& m_ipcProxyService;

	std::unique_ptr<PlayerQueueEPwmProxy> m_epwmProxy;
	std::unique_ptr<PlayerQueue> m_playerQueue;

	std::size_t m_completionCount;
};

#endif /* SOUNDSERVICESTATE_H_ */
