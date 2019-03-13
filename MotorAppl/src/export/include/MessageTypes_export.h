/*
 * MessageTypes_export.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef MESSAGETYPES_EXPORT_H_
#define MESSAGETYPES_EXPORT_H_

// project
#include "exceptionMacros.h"
#include "ServiceTypes.h"

namespace exports
{
class MessageTypes final
{
public:

	MessageTypes() = delete;

	enum Type
	{
		// Message types for CommonServices
		SoundMessage = 0,
		MotorServiceCUIMessage,
		NetManageMessage,
		LeftPropulsionOdometerMessage,
		RightPropulsionOdometerMessage,
		LeftPropulsionPidMessage,
		RightPropulsionPidMessage,
		DeadReckoningMessage,

		// Message types for PruIPC
		IPCDeviceProxyMessage,
		IPCDeviceProxyMessageCompleted,
		IPCDeviceProxyEventEQEP,
		IPCDeviceProxyEventTrace,
		IPCDeviceProxyEventOdo,

		// End Of Enum
		EOEMessage
	};

	static const char* toString(const MessageType_t type)
	{
		#define CASE(v) case v: return #v;
		switch (static_cast<Type>(type))
		{
			CASE(Type::SoundMessage)
		    CASE(Type::MotorServiceCUIMessage)
		    CASE(Type::NetManageMessage)
		    CASE(Type::LeftPropulsionOdometerMessage)
		    CASE(Type::RightPropulsionOdometerMessage)
		    CASE(Type::LeftPropulsionPidMessage)
		    CASE(Type::RightPropulsionPidMessage)
		    CASE(Type::DeadReckoningMessage)
			CASE(Type::IPCDeviceProxyMessage)
			CASE(Type::IPCDeviceProxyMessageCompleted)
			CASE(Type::IPCDeviceProxyEventEQEP)
			CASE(Type::IPCDeviceProxyEventTrace)
			CASE(Type::IPCDeviceProxyEventOdo)
			case EOEMessage:
				TB_ERROR("Don't use EOEMessage");
		}
		static std::string tmp = std::to_string(static_cast<int>(type));
		return tmp.c_str();
		#undef CASE
	}
};

} // namespace

#endif /* MESSAGETYPES_EXPORT_H_ */
