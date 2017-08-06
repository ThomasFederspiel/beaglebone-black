/*
 * CUICommands.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "CUICommands.h"

// standard
#include <ostream>

// project
#include "AsyncTaskMessage.h"
#include "CUICommandContext.h"
#include "ICUICommandParser.h"
#include "Logger.h"
#include "SoundMessage.h"
#include "stringifyer.h"
#include "Utils.h"

MODULE_LOG(CUICommands);

namespace
{
	static const std::string CommandPath = "/services/sound";

	static const SoundMessage::Sound sounds[] =
	{
		SoundMessage::Sound::MarioMainTheme,
		SoundMessage::Sound::MarioUnderWorldTheme
	};
}

PlaySoundCommand::PlaySoundCommand(SoundService& service) : AbstractCUICommand("ps", CommandPath),
	m_service(service)
{
}

void PlaySoundCommand::invoke(const ICUICommandParser& commandParser, CUICommandContext& context)
{
	if ((commandParser.count() < 1) || (commandParser.count() > 2))
	{
		context.finalizeError("Wrong number of arguments");
		return;
	}

	int id = 0;
	if (!commandParser.getValueArgument(0, id))
	{
		context.finalizeError("Bad sound id specified");
		return;
	}

	if ((id < 1) || (static_cast<std::size_t>(id) > countof(sounds)))
	{
		context.finalizeError("Sound id is out of range");
		return;
	}

	AsyncTaskMessage asyncTaskMessage([](SoundService* const service, const SoundMessage::Sound sound,
			CUICommandContext context) {

		TB_ASSERT(service);

		service->post(SoundMessage(sound));

		context.output() << "Done" << context.newline();
		context.finalize();

    }, &m_service, sounds[id - 1], context);

	m_service.post(asyncTaskMessage);
}

void PlaySoundCommand::genShortDesc(std::ostream& stream) const
{
	stream << "play sound";
}
