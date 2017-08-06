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
#include "MainService.h"

class ICUICommandParser;
class CUICommandContext;

class ShutdownCommand final : public AbstractCUICommand
{
public:
	ShutdownCommand(MainService& service);

	void invoke(const ICUICommandParser& commandLine, CUICommandContext& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;

private:
	MainService& m_service;
};

#endif /* CUICOMMANDS_H_ */
