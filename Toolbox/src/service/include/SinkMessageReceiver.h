/*
 * SinkMessageReceiver.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_SINKMESSAGERECEIVER_H_
#define TBOX_SINKMESSAGERECEIVER_H_

// local
#include "IMessageReceiver.h"
#include "ServiceMessageBase.h"

// project

class SinkMessageReceiver final : public IMessageReceiver
{
public:

	void post(const ServiceMessageBase& message) override
	{
	}
};

#endif /* TBOX_SINKMESSAGERECEIVER_H_ */
