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
#include "PropulsionPidMessage.h"
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
	serializer << message.getSpeedRPM();
	serializer << message.getHighSpeedRPM();
	serializer << message.getLowSpeedRPM();
	serializer << message.isLowSpeedValid();
	serializer << message.isLowSpeedActive();
}

static void serialize(const PropulsionPidMessage& message, BinaryComSerializer& serializer)
{
	serializer << message.getMotor();
	serializer << message.getSetPoint();
	serializer << message.getKpFactor();
	serializer << message.getKiFactor();
	serializer << message.getKdFactor();
	serializer << message.getInput();
	serializer << message.getError();
	serializer << message.getOutput();
}

void serializeMessage(const ServiceMessageBase& message, BinaryComSerializer& serializer)
{
	switch (message.getType())
	{
	case CommonMessageTypes::LeftPropulsionOdometerMessage:
	case CommonMessageTypes::RightPropulsionOdometerMessage:
		serialize(message.getCasted<PropulsionOdometerMessage>(), serializer);
		break;

	case CommonMessageTypes::LeftPropulsionPidMessage:
	case CommonMessageTypes::RightPropulsionPidMessage:
		serialize(message.getCasted<PropulsionPidMessage>(), serializer);
		break;

	TB_DEFAULT(message.getType());
	}
}
