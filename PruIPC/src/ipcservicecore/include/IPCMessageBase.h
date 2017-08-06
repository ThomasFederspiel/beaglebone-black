/*
 * IPCMessageBase.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_MESSAGEBASE_H_
#define PRUIPC_MESSAGEBASE_H_

// local

#include "IPCMessageTypes.h"

// project
#include "IMessageReceiver.h"
#include "ServiceMessageBase.h"

namespace pruipcservice
{

class IPCMessageBase : public ServiceMessageBase
{
protected:

	explicit IPCMessageBase(IMessageReceiver& receiver, const IPCMessageTypes::Type type) : ServiceMessageBase(type), m_receiver(receiver)
	{
	}

protected:

	void postResponse(const ServiceMessageBase& response) const
	{
		m_receiver.post(response);
	}

private:

	 IMessageReceiver& m_receiver;
};

} // namespace


#endif /* PRUIPC_MESSAGEBASE_H_ */
