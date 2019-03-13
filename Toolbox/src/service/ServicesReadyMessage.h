/*
 * ReadyServiceMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_SERVICESREADYMESSAGE_H_
#define TBOX_SERVICESREADYMESSAGE_H_

// project
#include "SystemMessage.h"
#include "SystemMessageTypes.h"
#include "stdExtension.h"

class ServicesReadyMessage final : public SystemMessage
{
public:

	explicit ServicesReadyMessage() : SystemMessage(SystemMessageTypes::Type::ServicesReadyMessage)
	{
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return std::make_unique<ServicesReadyMessage>(*this);
	}
};

#endif /* TBOX_SERVICESREADYMESSAGE_H_ */
