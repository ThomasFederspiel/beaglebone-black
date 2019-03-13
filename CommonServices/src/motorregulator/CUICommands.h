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
#include "ICUIManager.h"
#include "MotorRegulatorService.h"

class ICUICommandParser;
class CUICommandContext;
class ServiceAllocator;

namespace motorregulator
{

class CUICommands final
{
public:
	static void registerCUICommands(ServiceAllocator& allocator, MotorRegulatorService& service);
	static void unregisterCUICommands(ServiceAllocator& allocator, MotorRegulatorService& service);

private:
	static ICUIManager::hcui_t m_handle;
};

class SetMotorSpeedCommand final : public AbstractCUICommand
{
public:
	SetMotorSpeedCommand(MotorRegulatorService& service);

	void invoke(const ICUICommandParser& commandLine, CUICommandContext& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;

private:
	void parse(const ICUICommandParser& commandParser, CUICommandContext& context);

	MotorRegulatorService& m_service;
};

class SetMotorDistanceCommand final : public AbstractCUICommand
{
public:
	SetMotorDistanceCommand(MotorRegulatorService& service);

	void invoke(const ICUICommandParser& commandLine, CUICommandContext& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;

private:
	void parse(const ICUICommandParser& commandParser, CUICommandContext& context);

	MotorRegulatorService& m_service;
};

class SetRegulatorModeCommand final : public AbstractCUICommand
{
public:
	SetRegulatorModeCommand(MotorRegulatorService& service);

	void invoke(const ICUICommandParser& commandLine, CUICommandContext& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;

private:
	void parse(const ICUICommandParser& commandParser, CUICommandContext& context);

	MotorRegulatorService& m_service;
};

class SetMotorPidTuningCommand final : public AbstractCUICommand
{
public:
	SetMotorPidTuningCommand(MotorRegulatorService& service);

	void invoke(const ICUICommandParser& commandLine, CUICommandContext& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;

private:
	void parse(const ICUICommandParser& commandParser, CUICommandContext& context);

	MotorRegulatorService& m_service;
};

class GetMotorStatusCommand final : public AbstractCUICommand
{
public:
	GetMotorStatusCommand(MotorRegulatorService& service);

	void invoke(const ICUICommandParser& commandLine, CUICommandContext& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;

private:
	void parse(const ICUICommandParser& commandParser, CUICommandContext& context);

	MotorRegulatorService& m_service;
};

} // namespace

#endif /* CUICOMMANDS_H_ */
