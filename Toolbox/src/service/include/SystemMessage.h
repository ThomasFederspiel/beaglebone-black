/*
 * SystemMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_SYSTEMMESSAGE_H_
#define TBOX_SYSTEMMESSAGE_H_

// local
#include "ServiceMessageBase.h"

class SystemMessage : public ServiceMessageBase
{
public:

	SystemMessage(const MessageType_t messageType) : ServiceMessageBase(messageType, ServiceMessageBase::Classification::System)
	{
	}
};

#endif /* TBOX_SYSTEMMESSAGE_H_ */
