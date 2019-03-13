/*
 * ServiceAllocator.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "ServiceAllocator.h"

// standard
#include <memory>

// project
#include "AbstractService.h"
#include "exceptionMacros.h"
#include "ICUICommand.h"
#include "IService.h"
#include "Logger.h"

MODULE_LOG(ServiceAllocator);

ServiceAllocator::ServiceAllocator(ServiceManager& manager) : m_manager(manager)
{
}

ICUIManager::hcui_t ServiceAllocator::registerCommand(std::unique_ptr<ICUICommand> command)
{
	return m_manager.registerCommand(std::move(command));
}

ICUIManager::hcui_t ServiceAllocator::registerCommands(std::vector<std::unique_ptr<ICUICommand>>& commands)
{
	return m_manager.registerCommands(commands);
}

void ServiceAllocator::unregisterCommand(const ICUIManager::hcui_t handle)
{
	m_manager.unregisterCommand(handle);
}

const std::string& ServiceAllocator::getEventServiceName(const MessageType_t messageType) const
{
	return m_manager.getEventServiceName(messageType);
}

std::shared_ptr<AbstractService> ServiceAllocator::allocateEventService(const MessageType_t messageType, const IService& service)
{
	return m_manager.allocateEventService(messageType, service);
}

