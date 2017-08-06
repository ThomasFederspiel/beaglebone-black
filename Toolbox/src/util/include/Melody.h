/*
 * Melody.h
 *
 *  Created on: 26 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_MELODY_H_
#define TBOX_MELODY_H_

#include "PlayNote.h"

#include <cstdlib>

class Melody final
{
private:

	struct Tone
	{
		PlayNote::NoteEnum note;
		uint8_t beat;
	};

public:

	Melody(const Melody&) = delete;
	Melody& operator=(const Melody&) = delete;

	static void playMarioMainTheme(PlayNote& player);
	static void playMarioUnderWorldTheme(PlayNote& player);
	static void playSilentTheme(PlayNote& player);

private:

	Melody() {};

	static void playTheme(PlayNote& player, const struct Tone* const theme, const std::size_t size);

	static constexpr struct Tone MarioMainTheme[] =
	{
		{ PlayNote::NoteEnum::NoteE7, 12 }, { PlayNote::NoteEnum::NoteE7, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteE7, 12 },
		{ PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteC7, 12 }, { PlayNote::NoteEnum::NoteE7, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 },
		{ PlayNote::NoteEnum::NoteG7, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, {  PlayNote::NoteEnum::NoteSilent, 12 },
		{ PlayNote::NoteEnum::NoteG6, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 },

		{ PlayNote::NoteEnum::NoteC7, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteG6, 12 },
		{ PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteE6, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 },
		{ PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteA6, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteB6, 12 },
		{ PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteAS6, 12 }, { PlayNote::NoteEnum::NoteA6, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 },

		{ PlayNote::NoteEnum::NoteG6, 9 }, { PlayNote::NoteEnum::NoteE7, 9 }, { PlayNote::NoteEnum::NoteG7, 9 },
		{ PlayNote::NoteEnum::NoteA7, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteF7, 12 }, { PlayNote::NoteEnum::NoteG7, 12 },
		{ PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteE7, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteC7, 12 },
		{ PlayNote::NoteEnum::NoteD7, 12 }, { PlayNote::NoteEnum::NoteB6, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 },

		{ PlayNote::NoteEnum::NoteC7, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteG6, 12 },
		{ PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteE6, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 },
		{ PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteA6, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteB6, 12 },
		{ PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteAS6, 12 }, { PlayNote::NoteEnum::NoteA6, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 },

		{ PlayNote::NoteEnum::NoteG6, 9 }, { PlayNote::NoteEnum::NoteE7, 9 }, { PlayNote::NoteEnum::NoteG7, 9 },
		{ PlayNote::NoteEnum::NoteA7, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteF7, 12 }, { PlayNote::NoteEnum::NoteG7, 12 },
		{ PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteE7, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteC7, 12 },
		{ PlayNote::NoteEnum::NoteD7, 12 }, { PlayNote::NoteEnum::NoteB6, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }, { PlayNote::NoteEnum::NoteSilent, 12 }
	};

	static constexpr struct Tone MarioUnderWorldTheme[] =
	{
		{ PlayNote::NoteEnum::NoteC4, 12 }, { PlayNote::NoteEnum::NoteC5, 12 }, { PlayNote::NoteEnum::NoteA3, 12 }, { PlayNote::NoteEnum::NoteA4, 12 },
		{ PlayNote::NoteEnum::NoteAS3, 12 }, { PlayNote::NoteEnum::NoteAS4, 12 	}, { PlayNote::NoteEnum::NoteSilent, 6 },
		{ PlayNote::NoteEnum::NoteSilent, 3 },

		{ PlayNote::NoteEnum::NoteC4, 12 }, { PlayNote::NoteEnum::NoteC5, 12 }, { PlayNote::NoteEnum::NoteA3, 12 }, { PlayNote::NoteEnum::NoteA4, 12 },
		{ PlayNote::NoteEnum::NoteAS3, 12 }, { PlayNote::NoteEnum::NoteAS4, 12 }, { PlayNote::NoteEnum::NoteSilent, 6 },
		{ PlayNote::NoteEnum::NoteSilent, 3 },

		{ PlayNote::NoteEnum::NoteF3, 12 }, { PlayNote::NoteEnum::NoteF4, 12 }, { PlayNote::NoteEnum::NoteD3, 12 }, { PlayNote::NoteEnum::NoteD4, 12 },
		{ PlayNote::NoteEnum::NoteDS3, 12 }, { PlayNote::NoteEnum::NoteDS4, 12 }, { PlayNote::NoteEnum::NoteSilent, 6 },
		{ PlayNote::NoteEnum::NoteSilent, 3 },

		{ PlayNote::NoteEnum::NoteF3, 12 }, { PlayNote::NoteEnum::NoteF4, 12 }, { PlayNote::NoteEnum::NoteD3, 12 }, { PlayNote::NoteEnum::NoteD4, 12 },
		{ PlayNote::NoteEnum::NoteDS3, 12 }, { PlayNote::NoteEnum::NoteDS4, 12 }, { PlayNote::NoteEnum::NoteSilent, 6 },
		{ PlayNote::NoteEnum::NoteSilent, 6 }, { PlayNote::NoteEnum::NoteDS4, 18 }, { PlayNote::NoteEnum::NoteCS4, 18 }, { PlayNote::NoteEnum::NoteD4, 18 },

		{ PlayNote::NoteEnum::NoteCS4, 6 }, { PlayNote::NoteEnum::NoteDS4, 6 },
		{ PlayNote::NoteEnum::NoteDS4, 6 }, { PlayNote::NoteEnum::NoteGS3, 6 },
		{ PlayNote::NoteEnum::NoteG3, 6 }, { PlayNote::NoteEnum::NoteCS4, 6 },

		{ PlayNote::NoteEnum::NoteC4, 18 }, { PlayNote::NoteEnum::NoteFS4, 18 }, { PlayNote::NoteEnum::NoteF4, 18 }, { PlayNote::NoteEnum::NoteE3, 18 }, { PlayNote::NoteEnum::NoteAS4, 18 }, { PlayNote::NoteEnum::NoteA4, 18 },
		{ PlayNote::NoteEnum::NoteGS4, 10 }, { PlayNote::NoteEnum::NoteDS4, 10 }, { PlayNote::NoteEnum::NoteB3, 10 },
		{ PlayNote::NoteEnum::NoteAS3, 10 }, { PlayNote::NoteEnum::NoteA3, 10 }, { PlayNote::NoteEnum::NoteGS3, 10 },
		{ PlayNote::NoteEnum::NoteSilent, 3 }, { PlayNote::NoteEnum::NoteSilent, 3 }, { PlayNote::NoteEnum::NoteSilent, 3 }
	};

	static constexpr struct Tone SilentTheme[] =
	{
		{ PlayNote::NoteEnum::NoteSilent, 10 }, { PlayNote::NoteEnum::NoteSilent, 10 }, { PlayNote::NoteEnum::NoteSilent, 10 },
		{ PlayNote::NoteEnum::NoteSilent, 10 }
	};
};

#endif /* TBOX_MELODY_H_ */
