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

class CUICommandContextImpl;
class ICUICommandParser;
class CUICommandContext;

class AbstractSystemCUICommand : public AbstractCUICommand
{
public:
	explicit AbstractSystemCUICommand(const std::string& name, const std::string& path);

	void invoke(const ICUICommandParser& commandLine, CUICommandContext& context) override;

	virtual void invoke(const ICUICommandParser& commandLine, CUICommandContextImpl& context) = 0;
};

class PwdCommand final : public AbstractSystemCUICommand
{
public:
	PwdCommand();

	void invoke(const ICUICommandParser& commandLine, CUICommandContextImpl& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;
};

class CdCommand final : public AbstractSystemCUICommand
{
public:
	CdCommand();

	void invoke(const ICUICommandParser& commandLine, CUICommandContextImpl& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;
};

class LsCommand final : public AbstractSystemCUICommand
{
public:
	LsCommand();

	void invoke(const ICUICommandParser& commandLine, CUICommandContextImpl& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;
};

class QuitCommand final : public AbstractSystemCUICommand
{
public:
	QuitCommand();

	void invoke(const ICUICommandParser& commandLine, CUICommandContextImpl& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;
};

class TstCommand final : public AbstractSystemCUICommand
{
public:
	TstCommand();

	void invoke(const ICUICommandParser& commandLine, CUICommandContextImpl& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;
};

#endif /* CUICOMMANDS_H_ */
