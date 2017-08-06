/*
 * PlayNote.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_PLAYNOTE_H_
#define TBOX_PLAYNOTE_H_

#include <stdint.h>

class PlayNote final
{
private:

	static constexpr uint16_t NoteFreqTable[] =
	{
		0,
		31,
		33,
		35,
		37,
		39,
		41,
		44,
		46,
		49,
		52,
		55,
		58,
		62,
		65,
		69,
		73,
		78,
		82,
		87,
		93,
		98,
		104,
		110,
		117,
		123,
		131,
		139,
		147,
		156,
		165,
		175,
		185,
		196,
		208,
		220,
		233,
		247,
		262,
		277,
		294,
		311,
		330,
		349,
		370,
		392,
		415,
		440,
		466,
		494,
		523,
		554,
		587,
		622,
		659,
		698,
		740,
		784,
		831,
		880,
		932,
		988,
		1047,
		1109,
		1175,
		1245,
		1319,
		1397,
		1480,
		1568,
		1661,
		1760,
		1865,
		1976,
		2093,
		2217,
		2349,
		2489,
		2637,
		2794,
		2960,
		3136,
		3322,
		3520,
		3729,
		3951,
		4186,
		4435,
		4699,
		4978
	};

public:

	enum NoteEnum
	{
		NoteSilent = 0,
		NoteB0,
		NoteC1,
		NoteCS1,
		NoteD1,
		NoteDS1,
		NoteE1,
		NoteF1,
		NoteFS1,
		NoteG1,
		NoteGS1,
		NoteA1,
		NoteAS1,
		NoteB1,
		NoteC2,
		NoteCS2,
		NoteD2,
		NoteDS2,
		NoteE2,
		NoteF2,
		NoteFS2,
		NoteG2,
		NoteGS2,
		NoteA2,
		NoteAS2,
		NoteB2,
		NoteC3,
		NoteCS3,
		NoteD3,
		NoteDS3,
		NoteE3,
		NoteF3,
		NoteFS3,
		NoteG3,
		NoteGS3,
		NoteA3,
		NoteAS3,
		NoteB3,
		NoteC4,
		NoteCS4,
		NoteD4,
		NoteDS4,
		NoteE4,
		NoteF4,
		NoteFS4,
		NoteG4,
		NoteGS4,
		NoteA4,
		NoteAS4,
		NoteB4,
		NoteC5,
		NoteCS5,
		NoteD5,
		NoteDS5,
		NoteE5,
		NoteF5,
		NoteFS5,
		NoteG5,
		NoteGS5,
		NoteA5,
		NoteAS5,
		NoteB5,
		NoteC6,
		NoteCS6,
		NoteD6,
		NoteDS6,
		NoteE6,
		NoteF6,
		NoteFS6,
		NoteG6,
		NoteGS6,
		NoteA6,
		NoteAS6,
		NoteB6,
		NoteC7,
		NoteCS7,
		NoteD7,
		NoteDS7,
		NoteE7,
		NoteF7,
		NoteFS7,
		NoteG7,
		NoteGS7,
		NoteA7,
		NoteAS7,
		NoteB7,
		NoteC8,
		NoteCS8,
		NoteD8,
		NoteDS8
	};

	static constexpr uint16_t MinFreq = NoteFreqTable[NoteEnum::NoteB0];
	static constexpr uint16_t MaxFreq = NoteFreqTable[NoteEnum::NoteDS8];

	class INoteGenerator
	{
	public:
		virtual ~INoteGenerator() = default;

		virtual void open() = 0;
		virtual void close() = 0;
		virtual void playNote(const uint16_t freq, const uint16_t duration, const uint16_t pause) = 0;
	};

	PlayNote(INoteGenerator& noteGenerator);

	PlayNote(const PlayNote&) = delete;
	PlayNote& operator=(const PlayNote&) = delete;

	void play(const NoteEnum note, const uint16_t duration, const uint16_t pause);
	void open();
	void close();

private:

	INoteGenerator& m_noteGenerator;
};

#endif /* TBOX_PLAYNOTE_H_ */
