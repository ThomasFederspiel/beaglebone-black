/*
 * IPCDeviceProxy.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "IPCDeviceProxy.h"

// system
#include "prussdrv/prussdrv.h"
#include "prussdrv/pruss_intc_mapping.h"

// local
#include "exceptionMacros.h"
#include "PrussDriver.h"
#include "pru_ipc_devices.hp"
#include "pru_ipc_types.hp"
#include "pru_shram_layout.hp"

// ;+ Remove no longer needed

IPCDeviceProxy::IPCDeviceProxy(const PrussDriver::PruProxy::PruIdEnum pruId, PrussDriver::BiDirectionPruEventChannelPtr biDirectionEventChannel) :
	m_ipcBiDirectionalChannelContext(), m_eventChannel(std::move(biDirectionEventChannel))
{
	TB_ASSERT(m_eventChannel);

	PrussDriver& prussDrv = PrussDriver::instance();
	m_ipcBiDirectionalChannelContext = prussDrv.getSHRAM().allocateMemoryChunk(sizeof(struct IPCBiDirectionalChannelContext));

	prussDrv.getPruProxy(pruId).getConfig().setIPCBiDirectionalChannelContext(*m_ipcBiDirectionalChannelContext);

	TB_ASSERT(m_ipcBiDirectionalChannelContext);
}

void IPCDeviceProxy::sendSyncMessage(const uint8_t* const message, const uint8_t size)
{
	m_ipcBiDirectionalChannelContext->write(message, offsetof(struct IPCBiDirectionalChannelContext, txBuffer), size);

	m_eventChannel->sendWaitEvent();
}

