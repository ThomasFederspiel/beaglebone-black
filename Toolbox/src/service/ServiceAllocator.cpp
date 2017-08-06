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

void ServiceAllocator::registerCommand(std::unique_ptr<ICUICommand> command)
{
	m_manager.registerCommand(std::move(command));
}

void ServiceAllocator::unregisterCommand(const ICUICommand& command)
{
	m_manager.unregisterCommand(command);
}

