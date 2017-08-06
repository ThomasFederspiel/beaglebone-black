/*
 * ICUIManager.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_ICUIMANAGER_H_
#define TBOX_ICUIMANAGER_H_

// standard

class ICUICommand;

class ICUIManager
{
public:

	virtual ~ICUIManager()
	{
	}

	virtual void registerCommand(std::unique_ptr<ICUICommand> command) = 0;
	virtual void unregisterCommand(const ICUICommand& command) = 0;
};

#endif /* TBOX_ICUIMANAGER_H_ */
