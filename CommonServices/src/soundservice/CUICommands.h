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
#include "SoundService.h"

class ICUICommandParser;
class CUICommandContext;

namespace soundservice
{

class CUICommands final
{
public:
	static void registerCUICommands(ServiceAllocator& allocator, SoundService& service);
	static void unregisterCUICommands(ServiceAllocator& allocator, SoundService& service);

private:
	static ICUIManager::hcui_t m_handle;
};

class PlaySoundCommand final : public AbstractCUICommand
{
public:
	PlaySoundCommand(SoundService& service);

	void invoke(const ICUICommandParser& commandLine, CUICommandContext& context) override;

protected:
	void genShortDesc(std::ostream& stream) const override;

private:
	SoundService& m_service;
};

} // namespace

#endif /* CUICOMMANDS_H_ */
