/*
 * AbstractService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "AbstractService.h"

// standard
#include <exception>
#include <chrono>
#include <string>
#include <thread>

// local
#include "AsyncTaskMessage.h"
#include "ServiceAllocator.h"
#include "ServiceMessageBase.h"
#include "StartServiceMessage.h"
#include "StopServiceMessage.h"
#include "ServicesReadyMessage.h"
#include "SystemMessage.h"
#include "SystemMessageTypes.h"

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "tboxdefs.h"

MODULE_LOG(AbstractService);

namespace
{
	static const std::chrono::milliseconds HolfOffDuration(1000);
	static constexpr bool HoldOff = true;
}

AbstractService::AbstractService(const std::string& name) : AbstractRunnable(), m_name(name), m_state(ServiceState::Created), m_threadId(), m_queue(),
		m_terminate(false), m_lock(), m_signal(), m_workerThreadManager(), m_statistics(name, HoldOff)
{
}

const std::string& AbstractService::name() const
{
	return m_name;
}

ServiceStatistics AbstractService::getStatistics()
{
	return m_statistics.clone();
}

bool AbstractService::isStopRequested() const
{
	return state() == ServiceState::Stopping;
}

AbstractService::ServiceState AbstractService::state() const
{
	std::unique_lock<std::mutex> lock(m_lock);

	return m_state;
}

void AbstractService::setState(const AbstractService::ServiceState state)
{
	std::unique_lock<std::mutex> lock(m_lock);

	m_state = state;

	m_signal.notify_one();
}

void AbstractService::start(ServiceAllocator& allocator)
{
	TB_ASSERT(state() == ServiceState::Created,
		"Service " << name() << ", state = " << toString(state()));

	postInternal(StartServiceMessage(allocator));
}

void AbstractService::stop(ServiceAllocator& allocator)
{
	TB_ASSERT(state() == ServiceState::Running,
		"Service " << name() << ", state = " << toString(state()));

	signalStopStarted();

	m_queue.clear();

	postInternal(StopServiceMessage(allocator));
}

void AbstractService::signalServicesReady()
{
	TB_ASSERT(state() == ServiceState::Running,
		"Service " << name() << ", state = " << toString(state()));

	postInternal(ServicesReadyMessage());
}

void AbstractService::createWorker(WorkerThreadManager::AbstractWorkerRunnable& runnable)
{
	m_workerThreadManager.createWorker(runnable);
}

void AbstractService::stopWorker(WorkerThreadManager::AbstractWorkerRunnable& runnable)
{
	m_workerThreadManager.stopWorker(runnable);
}

void AbstractService::startWorker(WorkerThreadManager::AbstractWorkerRunnable& runnable)
{
	m_workerThreadManager.startWorker(runnable);
}

bool AbstractService::isWorkerActive(WorkerThreadManager::AbstractWorkerRunnable& runnable)
{
	return m_workerThreadManager.isWorkerActive(runnable);
}

void AbstractService::post(const ServiceMessageBase& message)
{
	TB_ASSERT((state() == ServiceState::Running) || (state() == ServiceState::Stopping),
			"Service " << name() << ", state = " << toString(state()));

	postInternal(message);
}

void AbstractService::postInternal(const ServiceMessageBase& message)
{
	auto msgClone = message.clone();

	msgClone->setPostTimePoint();

	m_queue.put(std::move(msgClone));
}

void AbstractService::waitOnStart()
{
	std::unique_ptr<ServiceMessageBase> message = m_queue.get();

	TB_ASSERT(message->isSystemMessage(), "Service " << name());

	const StartServiceMessage* startMessage = nullptr;
	// TODO: Create instanceof that takes smart pointers
	const bool isInstance = tbox::instanceof<const StartServiceMessage>(*message, startMessage);
	TB_ASSERT(isInstance);

	// Need to be done before onStart() as service might get posts while
	// in onStart() from services on lower layers
	setState(ServiceState::Running);

	onStart(startMessage->getAllocator());
}

void AbstractService::onSystemMessage(ServiceMessageBase& message)
{
	SystemMessage* systemMessage = nullptr;
	const bool isInstance = tbox::instanceof<SystemMessage>(message, systemMessage);
	TB_ASSERT(isInstance);

	switch (systemMessage->getType())
	{
	case SystemMessageTypes::Type::StopMessage:
	{
		onStopMessage(message);
	}
	break;

	case SystemMessageTypes::Type::ServicesReadyMessage:
	{
		m_statistics.clearHoldoff();

		onServicesReadyMessage(message);
	}
	break;

	case SystemMessageTypes::Type::AsyncTaskMessage:
	{
		onAsyncTaskMessage(message);
	}
	break;

	TB_DEFAULT(SystemMessageTypes::toString(systemMessage->getType()));
	}
}

void AbstractService::onAsyncTaskMessage(ServiceMessageBase& message)
{
	AsyncTaskMessage* asyncTaskMessage = nullptr;
	const bool isInstance = tbox::instanceof<AsyncTaskMessage>(message, asyncTaskMessage);
	TB_ASSERT(isInstance);

	asyncTaskMessage->operator()();
}

void AbstractService::onStopMessage(ServiceMessageBase& message)
{
	const StopServiceMessage* stopMessage = nullptr;
	const bool isInstance = tbox::instanceof<const StopServiceMessage>(message, stopMessage);
	TB_ASSERT(isInstance);

	StopStatus status = StopStatus::Done;
	auto tries = StopTries;

	while (((status = onStop(stopMessage->getAllocator())) == StopStatus::Pending)
			&& (tries > 0))
	{
		// Needed as to avoid link problem as get() needs reference to argument
		const auto time = StopWaitDelay;

		auto getStatus = m_queue.get(time);

		if (getStatus.status() == decltype(m_queue)::GetStatus::Status::Data)
		{
			onMessage(*getStatus.getData());
		}

		--tries;
	}

	if (status != StopStatus::Done)
	{
		ERROR("Service " << name() << " failed to stop in time");
	}

	if (m_workerThreadManager.hasActiveWorkers())
	{
		ERROR("Service " << name() << " has still the following workers active");

		for (const auto& item : m_workerThreadManager.getWorkers())
		{
			ERROR(item.second->name());
		}

		//
		INFO("Will try to stop all workers");
		m_workerThreadManager.stopWorkers();
	}

	m_terminate = true;
}

void AbstractService::signalStopStarted()
{
	setState(ServiceState::Stopping);

	onStopStarted();
}

void AbstractService::operator()()
{
	m_threadId = std::this_thread::get_id();

	try
	{
		waitOnStart();

		INFO("Starting event loop for service " << name());

		while (!m_terminate)
		{
			const std::unique_ptr<ServiceMessageBase> message = m_queue.get();

			if (message->isSystemMessage())
			{
				onSystemMessage(*message);
			}
			else
			{
				const auto start = std::chrono::steady_clock::now();
				const auto latency = start - message->getPostTimePoint();

				onMessage(*message);

				const auto end = std::chrono::steady_clock::now();

				const auto duration = end - start;

				m_statistics.update(*message, duration, latency, m_queue.size() + 1);
			}
		}

		INFO("Event loop for service " << name() << " has terminated");

		setState(ServiceState::Stopped);
	}
	catch (std::exception& e)
	{
		ERROR("Exception killed service " << m_name << ", what: " << e.what());

		setState(ServiceState::Stopped);

		throw e;
	}
	catch (...)
	{
		ERROR("Unknown exception killed service " << m_name);

		setState(ServiceState::Stopped);

		throw std::runtime_error(std::string("Service ") + m_name + " died");
	}
}


