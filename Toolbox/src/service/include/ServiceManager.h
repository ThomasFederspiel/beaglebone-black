/*
 * ServiceManager.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_SERVICEMANAGER_H_
#define TBOX_SERVICEMANAGER_H_

// standard
#include <chrono>
#include <limits>
#include <memory>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

// project
#include "exceptionMacros.h"
#include "ICUIManager.h"

// local
#include "AbstractService.h"

class ICUICommand;
class IService;
class ServiceAllocator;

class ServiceManager final
{
public:
	using ServiceLayerId = uint8_t;

private:

	using ServiceLayerExtendedId = uint16_t;

	enum class State
	{
		Idle = 0,
		Active,
		Stopped
	};

	static constexpr ServiceLayerExtendedId CustomTopLayer = std::numeric_limits<ServiceLayerId>::max() + 1;

public:
	friend class ServiceAllocator;

	ServiceManager();
	~ServiceManager();

	template<typename T>
	void addService(std::shared_ptr<T>& service)
	{
		addService(CustomTopLayer, service);
	}

	template<typename T>
	void addService(const ServiceLayerExtendedId layerId, std::shared_ptr<T>& service)
	{
		TB_ASSERT(getState() == State::Idle);

		TB_ASSERT(service);
		TB_ASSERT(m_services.find(service->name()) == m_services.end());

		m_services.emplace(std::make_pair(service->name(), ServiceAggregate(service, layerId)));

		addServiceLayerId(layerId);
	}

	template<ServiceLayerId ID, typename T, typename... Ts>
	void addService(Ts&&... params)
	{
		TB_ASSERT(getState() == State::Idle);

		std::shared_ptr<T> service = std::shared_ptr<T>(new T(std::forward<Ts>(params)...));

		TB_ASSERT(service);
		TB_ASSERT(m_services.find(service->name()) == m_services.end());

		m_services.emplace(std::make_pair(service->name(), ServiceAggregate(service, ID)));

		addServiceLayerId(ID);
	}

	template<typename T, typename... Ts>
	void addService(Ts&&... params)
	{
		TB_ASSERT(getState() == State::Idle);

		std::shared_ptr<T> service = std::shared_ptr<T>(new T(std::forward<Ts>(params)...));

		TB_ASSERT(service);
		TB_ASSERT(m_services.find(service->name()) == m_services.end());

		m_services.emplace(std::make_pair(service->name(), ServiceAggregate(service, CustomTopLayer)));

		addServiceLayerId(CustomTopLayer);
	}

	template <typename T>
	std::shared_ptr<T> getService(const std::string& name) const
	{
		return std::dynamic_pointer_cast<T>(getService(name));
	}

	ICUIManager::hcui_t registerCommand(std::unique_ptr<ICUICommand> command);
	ICUIManager::hcui_t registerCommands(std::vector<std::unique_ptr<ICUICommand>>& commands);
	void unregisterCommand(const ICUIManager::hcui_t handle);

	void setCUIManager(std::shared_ptr<ICUIManager> manager);

	void startServices();
	void stopServices();

	void streamServiceInformation(std::ostream& stream) const;

private:

	static constexpr std::chrono::milliseconds WaitServiceStatePollTime = std::chrono::milliseconds(10);

	struct ServiceAggregate
	{
	public:

		explicit ServiceAggregate(std::shared_ptr<AbstractService> service,
				const ServiceLayerExtendedId id) : m_service(service), m_layerId(id)
		{
		}

		std::shared_ptr<AbstractService> m_service;
		ServiceLayerExtendedId m_layerId;
	};

	template <typename T>
	std::shared_ptr<T> allocateService(const std::string& name, const IService& allocatorService);

	const std::string& getEventServiceName(const MessageType_t messageType) const;
	std::shared_ptr<AbstractService> allocateEventService(const MessageType_t messageType, const IService& allocatorService);

	template <typename Rep, typename Period>
	bool waitForState(AbstractService& service, const AbstractService::ServiceState state,
			const std::chrono::duration<Rep, Period>& duration);

	std::shared_ptr<AbstractService> getService(const std::string& name) const;
	void allocateService(const std::string& name, const IService& allocatorService);
	void releaseService(const std::string& name, const IService& allocatorService);
	bool isServiceAllocated(const std::string& allocatorService, const std::string& serviceToAllocate);
	void addServiceLayerId(const ServiceLayerExtendedId id);
	void logActiveServices() const;

	void initServices();
	void runServices();
	void signalServicesReady();

	void setState(const State state) const
	{
		m_state = state;
	}

	State getState() const
	{
		return m_state;
	}

	std::map<std::string, ServiceAggregate> m_services;
	std::shared_ptr<ICUIManager> m_cuiManager;
	std::map<std::string, std::unordered_set<std::string>> m_allocatedServices;
	std::vector<ServiceLayerExtendedId> m_serviceLayerIds;
	mutable State m_state;
	std::unique_ptr<ServiceAllocator> m_allocator;
};

template <typename T>
std::shared_ptr<T> ServiceManager::allocateService(const std::string& name, const IService& allocatorService)
{
	allocateService(name, allocatorService);

	return getService<T>(name);
}

#endif /* TBOX_SERVICEMANAGER_H_ */
