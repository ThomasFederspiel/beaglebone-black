/*
 * SoundMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef SOUNDMESSAGE_H_
#define SOUNDMESSAGE_H_

// project
#include "MessageTypes.h"
#include "MessageBase.h"
#include "tboxdefs.h"

using namespace melodyservice;

class SoundMessage final : public MessageBase
{
public:

	enum class Sound
	{
		MarioMainTheme = 0,
		MarioUnderWorldTheme
	};

	explicit SoundMessage(Sound sound) : MessageBase(MessageTypes::Type::SoundMessage), m_sound(sound)
	{
	}

	Sound getSound() const
	{
		return m_sound;
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return tbox::make_unique<SoundMessage>(*this);
	}

	static const char* toString(const Sound sound)
	{
		#define CASE(v) case v: return #v;
		switch (sound)
		{
			CASE(Sound::MarioMainTheme)
			CASE(Sound::MarioUnderWorldTheme)
		}
		static std::string tmp = std::to_string(static_cast<int>(sound));
		return tmp.c_str();
		#undef CASE
	}

private:

	Sound m_sound;
};

#endif /* SOUNDMESSAGE_H_ */
