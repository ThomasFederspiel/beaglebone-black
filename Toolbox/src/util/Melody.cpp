/*
 * Melody.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "Melody.h"

// standard
#include <chrono>
#include <thread>

// boost
#include <boost/numeric/conversion/cast.hpp>

// local
#include "Utils.h"

constexpr struct Melody::Tone Melody::MarioMainTheme[];
constexpr struct Melody::Tone Melody::MarioUnderWorldTheme[];
constexpr struct Melody::Tone Melody::SilentTheme[];

void Melody::playMarioMainTheme(PlayNote& player)
{
	playTheme(player, MarioMainTheme, countof(MarioMainTheme));
}

void Melody::playMarioUnderWorldTheme(PlayNote& player)
{
	playTheme(player, MarioUnderWorldTheme, countof(MarioUnderWorldTheme));
}

void Melody::playSilentTheme(PlayNote& player)
{
	playTheme(player, SilentTheme, countof(SilentTheme));
}

void Melody::playTheme(PlayNote& player, const struct Melody::Tone* const theme, const std::size_t size)
{
	player.open();

	for (std::size_t i = 0; i < size; ++i)
	{
		const struct Tone& tone = theme[i];

		const uint16_t duration = 1000 / tone.beat;
		const uint16_t pause = boost::numeric_cast<uint16_t>(duration * 1.3f);

		player.play(tone.note, duration, pause);
	}

	player.close();
}
