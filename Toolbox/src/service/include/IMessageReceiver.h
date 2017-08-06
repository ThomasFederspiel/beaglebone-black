/*
 * IMessageReceiver.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_IMESSAGERECEIVER_H_
#define TBOX_IMESSAGERECEIVER_H_

// local
#include "ServiceMessageBase.h"

class IMessageReceiver
{
public:

	IMessageReceiver() = default;
	virtual ~IMessageReceiver() = default;

	virtual void post(const ServiceMessageBase& message) = 0;
};

#endif /* TBOX_IMESSAGERECEIVER_H_ */
