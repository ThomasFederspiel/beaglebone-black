/*
 * IRunnable.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_IRUNNABLE_H_
#define TBOX_IRUNNABLE_H_

// standard
#include <string>

class IRunnable
{
public:

	IRunnable() = default;

	virtual ~IRunnable() = default;

	IRunnable(const IRunnable&) = delete;
	IRunnable& operator=(const IRunnable&) = delete;

	virtual const std::string& name() const = 0;
	virtual void operator()() = 0;
};

#endif /* TBOX_IRUNNABLE_H_ */
