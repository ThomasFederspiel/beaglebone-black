/*
 * ServiceManager.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "ServiceManager.h"

// standard
#include <iomanip>
#include <sstream>
#include <thread>

// project
#include "AbstractService.h"
#include "ICUICommand.h"
#include "Logger.h"
#include "ThreadFactory.h"
#include "ThreadWrapper.h"
#include "ServiceAllocator.h"
#include "Stringifyer.h"
#include "tboxutility.h"

// local
#include "ServiceStatistics.h"

MODULE_LOG(ServiceManager);

namespace
{
static constexpr std::chrono::milliseconds SERVICE_STATE_WAIT_MARGIN_TIME_MS = std::chrono::milliseconds(100);
static constexpr std::chrono::milliseconds SERVICE_STATE_RUNNING_TIMEOUT_MS = std::chrono::milliseconds(100);
} // end namespace

ServiceManager::ServiceManager() : m_services(), m_serviceLayerIds(), m_state(State::Idle), m_allocator(new ServiceAllocator(*this))
{
}

ServiceManager::~ServiceManager()
{
	if (!m_allocatedServices.empty())
	{
		logActiveServices();
	}

	TB_ASSERT(m_allocatedServices.empty());
}

ICUIManager::hcui_t ServiceManager::registerCommand(std::unique_ptr<ICUICommand> command)
{
	TB_ASSERT(m_cuiManager);
	return m_cuiManager->registerCommand(std::move(command));
}

ICUIManager::hcui_t ServiceManager::registerCommands(std::vector<std::unique_ptr<ICUICommand>>& commands)
{
	TB_ASSERT(m_cuiManager);
	return m_cuiManager->registerCommands(commands);
}

void ServiceManager::unregisterCommand(const ICUIManager::hcui_t handle)
{
	TB_ASSERT(m_cuiManager);
	m_cuiManager->unregisterCommand(handle);
}

void ServiceManager::setCUIManager(std::shared_ptr<ICUIManager> manager)
{
	TB_ASSERT(manager);
	TB_ASSERT(!m_cuiManager);

	m_cuiManager = manager;
}

void ServiceManager::startServices()
{
	TB_ASSERT(getState() == State::Idle);

	initServices();
	runServices();

	signalServicesReady();

	setState(State::Active);
}

void ServiceManager::stopServices()
{
	TB_ASSERT(getState() == State::Active);

	for (const auto& id : tbox::make_reverse_iterator(m_serviceLayerIds))
	{
		for (auto& service : m_services)
		{
			if (service.second.m_layerId == id)
			{
				INFO("Stopping service " << service.first)

				TB_ASSERT(service.second.m_service->state() == AbstractService::ServiceState::Running);

				service.second.m_service->stop(*m_allocator);

				// Needed to solve constexpr compilation error
				const auto delay = AbstractService::StopWaitDelay;
				if (!service.second.m_service->waitFor(AbstractService::ServiceState::Stopped, delay + SERVICE_STATE_WAIT_MARGIN_TIME_MS))
				{
					ERROR("Failed to stop service " << service.second.m_service->name() << " in time");
				}
			}
		}
	}

	ThreadFactory::instance().waitOnThreadTerminations();

	setState(State::Stopped);
}

void ServiceManager::initServices()
{
	for (auto& service : m_services)
	{
		INFO("Initiating service " << service.first)

		TB_ASSERT(service.second.m_service->state() == AbstractService::ServiceState::Created);

		service.second.m_service->start(*m_allocator);
	}
}

void ServiceManager::runServices()
{
	for (const auto& id : m_serviceLayerIds)
	{
		for (auto& service : m_services)
		{
			if (service.second.m_layerId == id)
			{
				INFO("Starting service " << service.first)

				ThreadFactory::thread_t thread = ThreadFactory::instance().createThread(*service.second.m_service);
				thread->start();

				if (!service.second.m_service->waitFor(AbstractService::ServiceState::Running, SERVICE_STATE_RUNNING_TIMEOUT_MS))
				{
					ERROR("Failed to start service " << service.second.m_service->name() << " in time");
				}
			}
		}
	}
}

void ServiceManager::signalServicesReady()
{
	for (auto& service : m_services)
	{
		service.second.m_service->signalServicesReady();
	}
}

std::shared_ptr<AbstractService> ServiceManager::getService(const std::string& name) const
{
	std::shared_ptr<AbstractService> service;

	auto iter = m_services.find(name);

	if (iter != m_services.end())
	{
		service = iter->second.m_service;
	}

	return service;
}

std::shared_ptr<AbstractService> ServiceManager::allocateEventService(const MessageType_t messageType, const IService& allocatorService)
{
	std::shared_ptr<AbstractService> foundService;

	for (auto& eventService : m_services)
	{
		if (eventService.second.m_service->hasEventSubscriber(messageType))
		{
			foundService = eventService.second.m_service;

			if (!isServiceAllocated(allocatorService.name(), foundService->name()))
			{
				allocateService(foundService->name(), allocatorService);
			}
		}
	}

	return foundService;
}

const std::string& ServiceManager::getEventServiceName(const MessageType_t messageType) const
{
	for (auto& service : m_services)
	{
		if (service.second.m_service->hasEventSubscriber(messageType))
		{
			return service.second.m_service->name();
		}
	}

	static std::string emptyName;
	return emptyName;
}

void ServiceManager::allocateService(const std::string& name, const IService& allocatorService)
{
	auto mapIter = m_allocatedServices.find(name);

	if (mapIter == m_allocatedServices.end())
	{
		m_allocatedServices[name] = {};
	}

	auto setIter = m_allocatedServices[name].find(allocatorService.name());

	if (setIter == m_allocatedServices[name].end())
	{
		m_allocatedServices[name].insert(allocatorService.name());
	}
	else
	{
		TB_ERROR("Service " << allocatorService.name() << " has already allocated service " << name);
	}
}

void ServiceManager::releaseService(const std::string& name, const IService& allocatorService)
{
	auto mapIter = m_allocatedServices.find(name);

	if (mapIter != m_allocatedServices.end())
	{
		auto setIter = mapIter->second.find(allocatorService.name());

		if (setIter != mapIter->second.end())
		{
			mapIter->second.erase(setIter);

			if (mapIter->second.empty())
			{
				m_allocatedServices.erase(mapIter);
			}
		}
		else
		{
			TB_ERROR("Service " << allocatorService.name() << " hasn't allocated service " << name);
		}
	}
	else
	{
		TB_ERROR("Service " << allocatorService.name() << " hasn't allocated service " << name);
	}
}

bool ServiceManager::isServiceAllocated(const std::string& allocatorService, const std::string& allocatedService)
{
	bool allocated = false;

	auto mapIter = m_allocatedServices.find(allocatedService);

	if (mapIter != m_allocatedServices.end())
	{
		auto setIter = mapIter->second.find(allocatorService);

		allocated = setIter != mapIter->second.end();
	}

	return allocated;
}

void ServiceManager::addServiceLayerId(const ServiceLayerExtendedId id)
{
	auto iter = std::lower_bound(m_serviceLayerIds.begin(), m_serviceLayerIds.end(), id);

	if (iter == m_serviceLayerIds.end())
	{
		m_serviceLayerIds.push_back(id);
	}
	else if (*iter == id)
	{
		// Already added
	}
	else if (iter != m_serviceLayerIds.end())
	{
		m_serviceLayerIds.insert(iter, id);
	}
}

void ServiceManager::streamServiceInformation(std::ostream& stream) const
{
	for (const auto& item : m_services)
	{
		const auto& aggregate = item.second;
		const auto serviceStat = aggregate.m_service->getStatistics();
		const auto& messageStat = serviceStat.getMessageStatistics();

		stream << "---------------------------------------------------------------" << std::endl;
		stream << "Service: " << serviceStat.name() << std::endl;

		for (const auto& msg : messageStat)
		{
			stream << std::left <<
					"Message: " << std::setw(4) << msg.second.id() <<
					std::setw(12) << TBOX_STRINGIFY(" (#" << serviceStat.getMessageCount() << ")") <<
					std::setw(11) << "Last [us]" <<
					std::setw(10) << "Min [us]" <<
					std::setw(14) << "Average [us]" <<
					std::setw(10) << "Max [us]" <<
					std::setw(14) << "Std dev [us]" << std::endl;

			stream << std::left <<
					std::setw(25) << "Latency" <<
					std::setw(11) << msg.second.lastLatency().count() <<
					std::setw(10) << msg.second.minLatency().count() <<
					std::setw(14) << msg.second.averageLatency().count() <<
					std::setw(10) << msg.second.maxLatency().count() <<
					std::setw(14) << msg.second.stdDeviationLatency().count() << std::endl;

			stream << std::left <<
					std::setw(25) << "Duration" <<
					std::setw(11) << msg.second.lastDuration().count() <<
					std::setw(10) << msg.second.minDuration().count() <<
					std::setw(14) << msg.second.averageDuration().count() <<
					std::setw(10) << msg.second.maxDuration().count() <<
					std::setw(14) << msg.second.stdDeviationDuration().count() << std::endl;

			if (msg.second.isPeriodic())
			{
				stream << std::left <<
						std::setw(25) << "Period" <<
						std::setw(11) << msg.second.lastPeriod().count() <<
						std::setw(10) << msg.second.minPeriod().count() <<
						std::setw(14) << msg.second.averagePeriod().count() <<
						std::setw(10) << msg.second.maxPeriod().count() <<
						std::setw(14) << msg.second.stdDeviationPeriod().count() << std::endl;
			}

			stream << std::left <<
					std::setw(25) << "Queue Depth" <<
					std::setw(11) << msg.second.lastQueueDepth() <<
					std::setw(10) << msg.second.minQueueDepth() <<
					std::setw(14) << msg.second.averageQueueDepth() <<
					std::setw(10) << msg.second.maxQueueDepth() <<
					std::setw(14) << msg.second.stdDeviationQueueDepth() << std::endl;
		}
	}
}

void ServiceManager::logActiveServices() const
{
	INFO("Active services");

	for (const auto& item : m_services)
	{
		const auto& aggregate = item.second;

		std::ostringstream oss;

		auto iter= m_allocatedServices.find(aggregate.m_service->name());

		if (iter!= m_allocatedServices.end())
		{
			for (const auto& name : iter->second)
			{
				oss << name << " ";
			}
		}
		else
		{
			oss << "none";
		}

		INFO(" " << aggregate.m_service->name() << "(" << AbstractService::toString(aggregate.m_service->state()) << ")" << " used by " << oss.str());
	}
}

