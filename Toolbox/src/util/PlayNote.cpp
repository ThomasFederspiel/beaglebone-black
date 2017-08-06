/*
 * PlayNote.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "PlayNote.h"

constexpr uint16_t PlayNote::NoteFreqTable[];

PlayNote::PlayNote(PlayNote::INoteGenerator& noteGenerator) : m_noteGenerator(noteGenerator)
{
}

void PlayNote::play(const PlayNote::NoteEnum note, const uint16_t duration, const uint16_t pause)
{
	m_noteGenerator.playNote(NoteFreqTable[note], duration, pause);
}

void PlayNote::open()
{
	m_noteGenerator.open();
}

void PlayNote::close()
{
	m_noteGenerator.close();
}
