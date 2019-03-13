/*
 * PlayerQueue.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PLAYERQUEUE_H_
#define PLAYERQUEUE_H_

// standard
#include <atomic>
#include <memory>
#include <queue>

// local
#include "NoteGeneratorAdapter.h"
#include "SoundMessage.h"

// project
#include "AtomicQueue.h"
#include "AbstractRunnable.h"
#include "NoteGeneratorAdapter.h"
#include "PlayNote.h"

class IIPCDeviceEPwmProxy;
class IIPCDevicePwmsProxy;

class PlayerQueue final : public AbstractRunnable
{
public:

	explicit PlayerQueue(IIPCDeviceEPwmProxy& epwmProxy);

	const std::string& name() const override;
	void operator()() override;

	void add(SoundMessage&& message);
	void shutdown();
	bool isShutdown() const;
	bool isRunning() const;

private:

	enum class State
	{
		Created = 0,
		Running,
		Stopping,
		Stopped
	};

	void playSound(const SoundMessage::Sound sound);

	NoteGeneratorAdapter m_noteGenerator;
	PlayNote m_playNote;
	tbox::AtomicQueue<SoundMessage> m_queue;
	std::atomic<State> m_state;
};

#endif /* PLAYERQUEUE_H_ */
