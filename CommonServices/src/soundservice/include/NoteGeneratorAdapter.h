/*
 * NoteGeneratorAdapter.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef NOTEGENERATORADAPTER_H_
#define NOTEGENERATORADAPTER_H_

// standard
#include <chrono>

// project
#include "PlayNote.h"
#include "IIPCDeviceEPwmProxy.h"

class IMessageReceiver;
class IPCDeviceProxyService;

class NoteGeneratorAdapter final : public PlayNote::INoteGenerator
{
public:

	class INoteGeneratorContext
	{
	public:
		virtual ~INoteGeneratorContext() = default;

		virtual void wait(const std::chrono::milliseconds& duration) = 0;
	};

	class DefaultContext final : public INoteGeneratorContext
	{
	public:
		void wait(const std::chrono::milliseconds& duration) override;
	};

	explicit NoteGeneratorAdapter(IIPCDeviceEPwmProxy& epwmProxy,
			INoteGeneratorContext& context);

	explicit NoteGeneratorAdapter(IIPCDeviceEPwmProxy& epwmProxy);

	NoteGeneratorAdapter(const NoteGeneratorAdapter&) = delete;
	NoteGeneratorAdapter& operator=(const NoteGeneratorAdapter&) = delete;

	// interface PlayNote::INoteGenerator
	void open() override;
	void close() override;
	void playNote(const uint16_t freq, const uint16_t duration, const uint16_t pause) override;

private:

	void wait(const uint16_t duration);

	IIPCDeviceEPwmProxy& m_ipcDeviceEPwmProxy;
	INoteGeneratorContext& m_context;
};

#endif /* NOTEGENERATORADAPTER_H_ */
