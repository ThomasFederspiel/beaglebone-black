/*
 * IPCMessageTypes.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_MESSAGETYPES_H_
#define PRUIPC_MESSAGETYPES_H_

// standard

// project
#include "ServiceTypes.h"

// exports
#include "MessageTypes_export.h"

namespace pruipcservice
{

class IPCMessageTypes final
{
public:

	IPCMessageTypes() = delete;

	enum Type
	{
		IpcDeviceProxyMessage = exports::MessageTypes::IpcDeviceProxyMessage,
		IpcDeviceProxyMessageCompleted = exports::MessageTypes::IpcDeviceProxyMessageCompleted,
		IpcDeviceProxyEventEQEP = exports::MessageTypes::IpcDeviceProxyEventEQEP
	};

	static const char* toString(const MessageType_t id)
	{
		return exports::MessageTypes::toString(id);
	}
};

} // namespace

#endif /* PRUIPC_MESSAGETYPES_H_ */
