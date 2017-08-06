/*
 * ICUICommandContext.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_ICUICOMMANDCONTEXT_H_
#define TBOX_ICUICOMMANDCONTEXT_H_

// standard

// project

class ICUICommandContext
{
public:

	virtual ~ICUICommandContext()
	{
	}

	virtual const std::string& newline() const = 0;
	virtual std::ostringstream& output() = 0;
	virtual void finalizeError(const std::string&& message) = 0;
	virtual void finalize() = 0;
};

#endif /* TBOX_ICUICOMMANDCONTEXT_H_ */
