/*
 * IPCServiceBase.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "IPCServiceBase.h"

// standard

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "tboxdefs.h"

MODULE_LOG(IPCServiceBase);

namespace pruipcservice
{

IPCServiceBase::IPCServiceBase(const std::string& name) : AbstractService(name)
{
}

void IPCServiceBase::onMessage(const ServiceMessageBase& message)
{
	const IPCMessageBase* customMessage = nullptr;
	const bool isInstance = tbox::instanceof<const IPCMessageBase>(message, customMessage);
	TB_ASSERT(isInstance);

	onMessage(*customMessage);
}

} // namespace
