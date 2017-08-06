/*
 * IPCDeviceMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEMESSAGE_H_
#define PRUIPC_IPCDEVICEMESSAGE_H_

// local
#include "MessageTypes.h"

// project
#include "ServiceMessageBase.h"

class IPCDeviceMessage : public MessageBase
{
public:
sdasda

protected:

	explicit MessageBase(const MessageTypes::Type type) : ServiceMessageBase(), m_type(type)
	{
	}

private:

	MessageTypes::Type m_type;
};

} // namespace


#endif /* PRUIPC_IPCDEVICEMESSAGE_H_ */
