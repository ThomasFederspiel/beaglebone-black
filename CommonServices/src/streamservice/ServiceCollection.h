/*
 * ServiceCollection.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef SERVICECOLLECTION_H_
#define SERVICECOLLECTION_H_

// standard
#include <map>
#include <memory>

// project
#include "AbstractService.h"
#include "IService.h"
#include "ServiceAllocator.h"

class ServiceCollection final
{
public:

	explicit ServiceCollection(const IService& allocatorService)
		: m_allocatorService(allocatorService), m_services()
	{
	}

	~ServiceCollection()
	{
		TB_ASSERT(m_services.empty());
	}

	std::shared_ptr<AbstractService> allocateEventService(const MessageType_t messageType, ServiceAllocator& allocator)
	{
		auto& name = allocator.getEventServiceName(messageType);
		TB_ASSERT(!name.empty(), "Failed to allocate event " << messageType);

		auto iter = m_services.find(name);

		if (iter == m_services.end())
		{
			auto service = allocator.allocateService<AbstractService>(name, m_allocatorService);

			m_services[name] = service;

			return service;
		}

		return iter->second;
	}

	void releaseServices(ServiceAllocator& allocator)
	{
		for (auto& item : m_services)
		{
			allocator.releaseService(item.second, m_allocatorService);
		}

		m_services.clear();
	}

private:

	const IService& m_allocatorService;
	std::map<std::string, std::shared_ptr<AbstractService>> m_services;
};

#endif /* SERVICECOLLECTION_H_ */
