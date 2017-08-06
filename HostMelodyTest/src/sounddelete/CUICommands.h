/*
 * CUICommands.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef CUICOMMANDS_H_
#define CUICOMMANDS_H_

// standard

// project
#include "AbstractCUICommand.h"
#include "SoundService.h"

class ICUICommandParser;
class ICUICommandContext;

class PlaySoundCommand final : public AbstractCUICommand
{
public:
	PlaySoundCommand(SoundService& service);

	void invoke(const ICUICommandParser& commandLine, ICUICommandContext& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;

private:
	SoundService& m_service;
};

#endif /* CUICOMMANDS_H_ */
