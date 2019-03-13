/*
 * ShutdownMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_SHUTDOWNMESSAGE_H_
#define TBOX_SHUTDOWNMESSAGE_H_

// project
#include "SystemMessage.h"
#include "SystemMessageTypes.h"
#include "stdExtension.h"

class ShutdownMessage final : public SystemMessage
{
public:

	explicit ShutdownMessage() : SystemMessage(SystemMessageTypes::Type::ShutdownMessage)
	{
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return std::make_unique<ShutdownMessage>(*this);
	}
};

#endif /* TBOX_SHUTDOWNMESSAGE_H_ */
