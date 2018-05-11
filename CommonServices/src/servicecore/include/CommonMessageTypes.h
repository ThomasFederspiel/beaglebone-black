/*
 * CommonMessageTypes.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef COMMONMESSAGETYPES_H_
#define COMMONMESSAGETYPES_H_

// standard

// project
#include "ServiceTypes.h"

// exports
#include "MessageTypes_export.h"

namespace commonservices
{

class CommonMessageTypes final
{
public:

	CommonMessageTypes() = delete;

	enum Type
	{
		MotorServiceCUIMessage = exports::MessageTypes::MotorServiceCUIMessage,
		SoundMessage = exports::MessageTypes::SoundMessage,
		NetManageMessage = exports::MessageTypes::NetManageMessage,
		LeftPropulsionOdometerMessage = exports::MessageTypes::LeftPropulsionOdometerMessage,
		RightPropulsionOdometerMessage = exports::MessageTypes::RightPropulsionOdometerMessage,
		LeftPropulsionPidMessage = exports::MessageTypes::LeftPropulsionPidMessage,
		RightPropulsionPidMessage = exports::MessageTypes::RightPropulsionPidMessage,
		EOEMessage = exports::MessageTypes::EOEMessage
	};

	static const char* toString(const MessageType_t type)
	{
		return exports::MessageTypes::toString(type);
	}
};

} // namespace

#endif /* COMMONMESSAGETYPES_H_ */
