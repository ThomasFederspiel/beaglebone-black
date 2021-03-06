/*
 * ServiceAllocator.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_SERVICEALLICATOR_H_
#define TBOX_SERVICEALLICATOR_H_

// standard
#include <memory>
#include <string>

// local
#include "ICUICommand.h"
#include "ICUIManager.h"
#include "ServiceManager.h"
#include "ServiceTypes.h"

class IService;

class ServiceAllocator
{
public:

	explicit ServiceAllocator(ServiceManager& manager);

	template <typename T>
	std::shared_ptr<T> allocateService(const std::string& name, const IService& service);

	const std::string& getEventServiceName(const MessageType_t messageType) const;
	std::shared_ptr<AbstractService> allocateEventService(const MessageType_t messageType, const IService& service);

	template <typename T>
	void releaseService(std::shared_ptr<T>& allocatedService, const IService& service);

	ICUIManager::hcui_t registerCommand(std::unique_ptr<ICUICommand> command);
	ICUIManager::hcui_t registerCommands(std::vector<std::unique_ptr<ICUICommand>>& commands);
	void unregisterCommand(const ICUIManager::hcui_t handle);

private:
	ServiceManager& m_manager;
};

template <typename T>
std::shared_ptr<T> ServiceAllocator::allocateService(const std::string& name, const IService& service)
{
	return m_manager.allocateService<T>(name, service);
}

template <typename T>
void ServiceAllocator::releaseService(std::shared_ptr<T>& allocatedService, const IService& service)
{
	TB_ASSERT(allocatedService);

	m_manager.releaseService(allocatedService->name(), service);
	allocatedService.reset();
}

#endif /* TBOX_SERVICEALLOCATOR_H_ */
