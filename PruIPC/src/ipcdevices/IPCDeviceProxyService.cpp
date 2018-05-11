/*
 * IPCDeviceProxyService.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "IPCDeviceProxyService.h"

// system
#include "prussdrv/prussdrv.h"
#include "prussdrv/pruss_intc_mapping.h"

// project
#include "exceptionMacros.h"
#include "IPCDeviceProxyMessage.h"
#include "Logger.h"
#include "pru_ipc_devices.hp"
#include "pru_ipc_types.hp"
#include "tboxdefs.h"
#include "Stringifyer.h"

// local
#include "IPCEventProxyRunnable.h"


namespace
{
	static const std::string IPCEventRunnableName("IPCEventRunnable");
};

MODULE_LOG_OFF(IPCDeviceProxyService);

IPCDeviceProxyService::IPCDeviceProxyService(const std::string& name, const PrussDriver::PruProxy::PruIdEnum pruId, PrussDriver::BiDirectionPruEventChannelPtr biDirectionEventChannel) :
		IPCDeviceProxyService(name, pruId, biDirectionEventChannel, PrussDriver::HostDirectionPruEventChannelPtr())
{
}

IPCDeviceProxyService::IPCDeviceProxyService(const std::string& name, const PrussDriver::PruProxy::PruIdEnum pruId,
			PrussDriver::BiDirectionPruEventChannelPtr biDirectionEventChannel,
			PrussDriver::HostDirectionPruEventChannelPtr hostDirectionEventChannel) :
		IPCServiceBase(name), m_ipcBiDirectionalChannelContext(), m_eventChannel(biDirectionEventChannel),
		m_ipcEventProxyRunnable()
{
	TB_ASSERT(m_eventChannel);

	PrussDriver& prussDrv = PrussDriver::instance();

	m_ipcBiDirectionalChannelContext = prussDrv.getSHRAM().allocateMemoryChunk(sizeof(struct IPCBiDirectionalChannelContext));
	TB_ASSERT(m_ipcBiDirectionalChannelContext);

	prussDrv.getPruProxy(pruId).getConfig().setIPCBiDirectionalChannelContext(*m_ipcBiDirectionalChannelContext);

	if (hostDirectionEventChannel)
	{
		m_ipcEventProxyRunnable = tbox::make_unique<IPCEventProxyRunnable>(TBOX_STRINGIFY(name << " - "<< IPCEventRunnableName), *this, pruId,
				hostDirectionEventChannel);
	}
}

IPCDeviceProxyService::~IPCDeviceProxyService()
{
	// needed because of unique_ptr members
}

void IPCDeviceProxyService::onStart(ServiceAllocator& allocator)
{
	if (m_ipcEventProxyRunnable)
	{
		m_ipcEventProxyRunnable->registerEventMessages();

		createWorker(*m_ipcEventProxyRunnable);
	}
}

IPCDeviceProxyService::StopStatus IPCDeviceProxyService::onStop(ServiceAllocator& allocator)
{
	if (m_ipcEventProxyRunnable)
	{
		stopWorker(*m_ipcEventProxyRunnable);

		if (!isWorkerActive(*m_ipcEventProxyRunnable))
		{
			return StopStatus::Done;
		}

		return StopStatus::Pending;
	}

	return StopStatus::Done;
}

void IPCDeviceProxyService::onMessage(const IPCMessageBase& message)
{
	INFO("Got message " << IPCMessageTypes::toString(message.getType()));

	switch (message.getType())
	{
	case IPCMessageTypes::Type::IPCDeviceProxyMessage:
	{
		const auto& proxyMessage = message.getCasted<IPCDeviceProxyMessage>();

		sendSyncIPCMessage(proxyMessage.data(), proxyMessage.size());

		proxyMessage.completed();

		break;
	}

	TB_DEFAULT(IPCMessageTypes::toString(message.getType()));
	}
}

void IPCDeviceProxyService::sendSyncIPCMessage(const uint8_t* const message, const uint8_t size)
{
	INFO("Sending IPC message to device " << toDeviceString(static_cast<enum IPCDeviceEnum>(reinterpret_cast<const struct IPCDeviceIoctl*>(message)->device)));

	m_ipcBiDirectionalChannelContext->write(message, offsetof(struct IPCBiDirectionalChannelContext, rxBuffer), size);

	writeEOB(size);

	m_eventChannel->sendWaitEvent();

	INFO("Send completed");
}

void IPCDeviceProxyService::writeEOB(const std::size_t address)
{
	const uint32_t eob = IPC_EOB;
	m_ipcBiDirectionalChannelContext->write(&eob, offsetof(struct IPCBiDirectionalChannelContext, rxBuffer) + address, sizeof(eob));
}

void IPCDeviceProxyService::addPruEventDefinition(std::unique_ptr<AbstractEventDefinition> definition)
{
	TB_ASSERT(m_ipcEventProxyRunnable);

	m_ipcEventProxyRunnable->addPruEventDefinition(std::move(definition));
}
