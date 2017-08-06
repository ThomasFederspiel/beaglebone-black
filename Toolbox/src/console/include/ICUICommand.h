/*
 * ICUICommand.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_ICUICOMMAND_H_
#define TBOX_ICUICOMMAND_H_

// standard
#include <string>

class CUICommandContext;
class ICUICommandParser;

class ICUICommand
{
public:

	virtual ~ICUICommand()
	{
	}

	virtual void invoke(const ICUICommandParser& commandParser, CUICommandContext& context) = 0;
	virtual std::string shortDescription() const = 0;
	virtual std::string manPage() const = 0;
	virtual std::string getPath() const = 0;
	virtual const std::string& getName() const = 0;
};

#endif /* TBOX_ICUICOMMAND_H_ */
