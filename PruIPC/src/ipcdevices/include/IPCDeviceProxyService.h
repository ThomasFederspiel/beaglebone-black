/*
 * IPCDeviceProxyService.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEPROXYSERVICE_H_
#define PRUIPC_IPCDEVICEPROXYSERVICE_H_

// standard
#include <memory>
#include <stdint.h>
#include <string>

// project
#include "IMemory.h"
#include "IPCServiceBase.h"
#include "pru_ipc_devices.hp"
#include "pru_ipc_types.hp"
#include "PruEventDefinition.h"
#include "PrussDriver.h"
#include "stdExtension.h"

class IPCEventProxyRunnable;

using namespace pruipcservice;

class IPCDeviceProxyService final : public IPCServiceBase
{
public:
	friend class IPCEventProxyRunnable;

	explicit IPCDeviceProxyService(const std::string& name, const PrussDriver::PruProxy::PruIdEnum pruId,
				PrussDriver::BiDirectionPruEventChannelPtr biDirectionEventChannel);
	explicit IPCDeviceProxyService(const std::string& name, const PrussDriver::PruProxy::PruIdEnum pruId,
				PrussDriver::BiDirectionPruEventChannelPtr biDirectionEventChannel,
				PrussDriver::HostDirectionPruEventChannelPtr hostDirectionEventChannel);

	~IPCDeviceProxyService();

	template <typename M>
	void addPruEventDefinition();

protected:

	void onMessage(const IPCMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;
	void onServicesReadyMessage(ServiceMessageBase& message) override;

private:
	void addPruEventDefinition(std::unique_ptr<AbstractEventDefinition> definition);
	void sendSyncIPCMessage(const uint8_t* const message, const uint8_t size);
	void writeEOB(const std::size_t address);

	std::shared_ptr<IMemory> m_ipcBiDirectionalChannelContext;
	PrussDriver::BiDirectionPruEventChannelPtr m_eventChannel;

	std::unique_ptr<IPCEventProxyRunnable> m_ipcEventProxyRunnable;
};

template <typename M>
void IPCDeviceProxyService::addPruEventDefinition()
{
	addPruEventDefinition(std::make_unique<EventDefinition<M>>());
}

#endif /* PRUIPC_IPCDEVICEPROXYERVICE_H_ */
