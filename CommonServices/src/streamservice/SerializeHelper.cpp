/*
 * SerializeHelper.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "SerializeHelper.h"

// project
#include "exceptionMacros.h"
#include "BinaryComSerializer.h"
#include "CommonMessageTypes.h"
#include "Logger.h"
#include "PropulsionOdometerMessage.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

MODULE_LOG(SerializeHelper);

using namespace commonservices;

static void serialize(const PropulsionOdometerMessage& message, BinaryComSerializer& serializer)
{
	serializer << message.getMotor();
	serializer << message.getCapCounter();
	serializer << message.getCapTime();
	serializer << message.getCapPeriod();
	serializer << message.getCounter();
}

void serializeMessage(const ServiceMessageBase& message, BinaryComSerializer& serializer)
{
	switch (message.getType())
	{
	case CommonMessageTypes::LeftPropulsionOdometerMessage:
	case CommonMessageTypes::RightPropulsionOdometerMessage:
		serialize(message.getCasted<PropulsionOdometerMessage>(), serializer);
		break;

	TB_DEFAULT(message.getType());
	}
}
