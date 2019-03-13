/*
 * ICUIManager.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_ICUIMANAGER_H_
#define TBOX_ICUIMANAGER_H_

// standard
#include <memory>
#include <vector>

class ICUICommand;

class ICUIManager
{
public:

	using hcui_t = std::size_t;

	static constexpr hcui_t UndefinedCUIHandle = 0;

	virtual ~ICUIManager()
	{
	}

	virtual hcui_t registerCommand(std::unique_ptr<ICUICommand> command) = 0;
	virtual hcui_t registerCommands(std::vector<std::unique_ptr<ICUICommand>>& commands) = 0;
	virtual void unregisterCommand(const hcui_t handle) = 0;
};

#endif /* TBOX_ICUIMANAGER_H_ */
