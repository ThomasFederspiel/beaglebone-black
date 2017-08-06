/*
 * IPCResponseBase.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_RESPONSEBASE_H_
#define PRUIPC_RESPONSEBASE_H_

// local
#include "IPCMessageTypes.h"

// project
#include "ServiceMessageBase.h"

namespace pruipcservice
{

class IPCResponseBase : public ServiceMessageBase
{
protected:

	explicit IPCResponseBase(const IPCMessageTypes::Type type) : ServiceMessageBase(type)
	{
	}
};

} // namespace

#endif /* PRUIPC_RESPONSEBASE_H_ */
