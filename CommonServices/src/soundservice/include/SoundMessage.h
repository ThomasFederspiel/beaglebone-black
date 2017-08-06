/*
 * SoundMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef SOUNDMESSAGE_H_
#define SOUNDMESSAGE_H_

// project
#include "CommonMessageTypes.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

class SoundMessage final : public ServiceMessageBase
{
public:

	enum class Sound
	{
		MarioMainTheme = 0,
		MarioUnderWorldTheme
	};

	explicit SoundMessage(Sound sound) : ServiceMessageBase(commonservices::CommonMessageTypes::Type::SoundMessage), m_sound(sound)
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
