/*
 * NetManagerService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef NETMANAGERSERVICE_H_
#define NETMANAGERSERVICE_H_

// standard
#include <string>

// project
#include "AbstractService.h"
#include "ServiceAllocator.h"

class IPCDeviceProxyService;
class NetManageMessage;
class ServiceMessageBase;

class NetManagerService final : public AbstractService
{
public:

	explicit NetManagerService(const std::string& name);

protected:

	void onMessage(const ServiceMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;

private:
	void executeAction(const NetManageMessage& message);

};

#endif /* NETMANAGERSERVICE_H_ */
