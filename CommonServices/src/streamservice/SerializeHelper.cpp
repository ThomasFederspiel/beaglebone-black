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
#include "DeadReckoningMessage.h"
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

static void serialize(const DeadReckoningMessage& message, BinaryComSerializer& serializer)
{
	const auto& pose = message.getPose();

	serializer << pose.getX();
	serializer << pose.getY();
	serializer << pose.getTheta();
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

	case CommonMessageTypes::DeadReckoningMessage:
		serialize(message.getCasted<DeadReckoningMessage>(), serializer);
		break;

	TB_DEFAULT(message.getType());
	}
}
