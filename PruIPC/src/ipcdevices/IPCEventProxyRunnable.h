/*
 * IPCEventProxyRunnable.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_IPCEVENTPROXYRUNNABLE_H_
#define TBOX_IPCEVENTPROXYRUNNABLE_H_

// standard
#include <atomic>
#include <map>
#include <memory>

// project
#include "IEventMessagePublisher.h"
#include "IMemory.h"
#include "pru_ipc_devices.hp"
#include "PrussDriver.h"
#include "WorkerThreadManager.h"

class AbstractEventDefinition;
class AbstractService;

class IPCEventProxyRunnable : public WorkerThreadManager::IWorkerRunnable
{
public:

	using EventDefinitionPtr = std::unique_ptr<AbstractEventDefinition>;

	explicit IPCEventProxyRunnable(const std::string& name, IEventMessagePublisher& publisher,
			const PrussDriver::PruProxy::PruIdEnum pruId, PrussDriver::HostDirectionPruEventChannelPtr eventChannel);

	const std::string& name() const override;

	void addPruEventDefinition(EventDefinitionPtr definition);

	void registerEventMessages();

	void stop() override;

protected:

	void operator()() override;

private:

	using IoctlEventDefinitionMap = std::map<ioctl_t, EventDefinitionPtr>;
	using DeviceEventDefinitionMap = std::map<IPCDeviceEnum, IoctlEventDefinitionMap>;

	uint32_t getTransmitBufferOffset();
	void readPruEvents();
	uint32_t processPruEvent(const IPCDeviceIoctl& device, const uint32_t offset);
	void acknowledge();
	bool hasOverflown();
	void stopBuffer();
	bool isBufferActive();
	bool isBufferCorrupted(const uint32_t bufferOffset);
	void dumpBuffer();

	const std::string m_name;
	IEventMessagePublisher& m_eventPublisher;
	const PrussDriver::PruProxy::PruIdEnum m_pruId;
	std::atomic_bool m_terminated;

	DeviceEventDefinitionMap m_eventDefinitionMap;

	std::shared_ptr<IMemory> m_ipcHostDirectionalChannelContext;
	PrussDriver::HostDirectionPruEventChannelPtr m_eventChannel;

	bool m_eventRegistrationDone;
};

#endif /* TBOX_IPCEVENTPROXYRUNNABLE */
