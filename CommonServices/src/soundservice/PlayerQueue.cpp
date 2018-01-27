/*
 * PlayerQueue.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "PlayerQueue.h"

// standard

// local
#include "SoundMessage.h"

// project
#include "exceptionMacros.h"
#include "IIPCDeviceEPwmProxy.h"
#include "IIPCDevicePwmsProxy.h"
#include "Logger.h"
#include "Melody.h"
#include "NoteGeneratorAdapter.h"
#include "PlayNote.h"
#include "PlayerQueueProxies.h"
#include "pru_pwms_types.hp"
#include "tboxdefs.h"

MODULE_LOG(PlayerQueue);

PlayerQueue::PlayerQueue(IIPCDeviceEPwmProxy& epwmProxy,
		IIPCDevicePwmsProxy& pwmsProxy) : m_noteGenerator(epwmProxy, pwmsProxy), m_playNote(m_noteGenerator),
		m_queue(), m_state(State::Created)
{
}

const std::string& PlayerQueue::name() const
{
	static std::string name("PlayerQueue");

	return name;
}

void PlayerQueue::shutdown()
{
	m_state = State::Stopping;

	// Fake message as to get out of queue wait
	m_queue.put(SoundMessage(SoundMessage::Sound::MarioMainTheme));
}

bool PlayerQueue::isShutdown() const
{
	return m_state == State::Stopped;
}

bool PlayerQueue::isRunning() const
{
	return m_state == State::Running;
}

void PlayerQueue::add(SoundMessage&& message)
{
	m_queue.put(std::move(message));
}

void PlayerQueue::operator()()
{
	m_state = State::Running;

	try
	{
		while (m_state == State::Running)
		{
			const SoundMessage message = m_queue.get();

			if (m_state == State::Running)
			{
				playSound(message.getSound());
			}
		}
	}
	catch (PlayerQueueTermException& e)
	{
		// Do nothing
	}
	catch (...)
	{
		ERROR("Unknown exception killed player queue");

		throw;
	}

	m_state = State::Stopped;
}

void PlayerQueue::playSound(const SoundMessage::Sound sound)
{
	switch (sound)
	{
	case SoundMessage::Sound::MarioMainTheme:
		Melody::playMarioMainTheme(m_playNote);
		break;
	case SoundMessage::Sound::MarioUnderWorldTheme:
		Melody::playMarioUnderWorldTheme(m_playNote);
		break;

	TB_DEFAULT(SoundMessage::toString(sound));
	}
}


