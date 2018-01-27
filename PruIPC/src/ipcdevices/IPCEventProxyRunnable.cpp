/*
 * IPCEventProxyRunnable.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "IPCEventProxyRunnable.h"

// standard
#include <chrono>

// project
#include "AbstractService.h"
#include "exceptionMacros.h"
#include "IPCMessageTypes.h"
#include "Logger.h"
#include "pru_ipc_types.hp"
#include "tboxdefs.h"

// local
#include "PruEventDefinition.h"

MODULE_LOG(IPCEventProxyRunnable);

IPCEventProxyRunnable::IPCEventProxyRunnable(const std::string& name, IEventMessagePublisher& publisher,
		const PrussDriver::PruProxy::PruIdEnum pruId, PrussDriver::HostDirectionPruEventChannelPtr eventChannel) : m_name(name),
		m_eventPublisher(publisher), m_pruId(pruId), m_terminated(false), m_eventDefinitionMap(), m_eventChannel(eventChannel),
		m_eventRegistrationDone(false)
{
	TB_ASSERT(m_eventChannel != nullptr);

	PrussDriver& prussDrv = PrussDriver::instance();
	m_ipcHostDirectionalChannelContext = prussDrv.getSHRAM().allocateMemoryChunk(sizeof(struct IPCHostDirectionalChannelContext));
	TB_ASSERT(m_ipcHostDirectionalChannelContext);

	prussDrv.getPruProxy(m_pruId).getConfig().setIPCHostDirectionalChannelContext(*m_ipcHostDirectionalChannelContext);
}

const std::string& IPCEventProxyRunnable::name() const
{
	return m_name;
}

void IPCEventProxyRunnable::addPruEventDefinition(EventDefinitionPtr definition)
{
	TB_ASSERT(definition);

	TB_ASSERT(!m_eventRegistrationDone);

	const auto device = definition->getDevice();

	auto deviceIter = m_eventDefinitionMap.find(device);

	if (deviceIter == m_eventDefinitionMap.end())
	{
		m_eventDefinitionMap[device] = IoctlEventDefinitionMap();
	}

	auto& ioctlMap = m_eventDefinitionMap[device];

	const auto ioctl = definition->getIoctl();

	TB_ASSERT(ioctlMap.find(ioctl) == ioctlMap.end());

	ioctlMap[ioctl] = std::move(definition);
}

void IPCEventProxyRunnable::registerEventMessages()
{
	std::size_t count = 0;

	for (const auto& deviceItem : m_eventDefinitionMap)
	{
		for (const auto& ioctlItem : deviceItem.second)
		{
			m_eventPublisher.registerEvent(ioctlItem.second->getType());

			++count;
		}
	}

	if (count == 0)
	{
		ERROR("No pru events registered. Is IPC event proxy needed");
	}

	m_eventRegistrationDone = true;
}

void IPCEventProxyRunnable::stop()
{
	m_terminated = true;

	const auto rc = m_eventChannel->close();

	TB_ASSERT(rc == PrussDriver::RCEnum::RcOk)
}

void IPCEventProxyRunnable::operator()()
{
	while (!m_terminated)
	{
		m_eventChannel->waitEvent();

		if (!m_terminated)
		{
			readPruEvents();
		}
	}
}

uint32_t IPCEventProxyRunnable::getTransmitBufferOffset()
{
	uint32_t transmitBufferOffset = 0;

	const uint32_t transmitBufferIndexOffset = offsetof(struct IPCHostDirectionalChannelContext, transmitBufferIndex);

	uint32_t bufferIndex = UNDEFINED_INDEX;
	m_ipcHostDirectionalChannelContext->read(&bufferIndex, transmitBufferIndexOffset, sizeof(bufferIndex));

	TB_ASSERT(bufferIndex != UNDEFINED_INDEX);

	// ;+
	// INFO("bufferIndex = " << bufferIndex);

	switch (bufferIndex)
	{
	case 0:
		transmitBufferOffset = offsetof(struct IPCHostDirectionalChannelContext, buffer_0);
		break;

	case 1:
		transmitBufferOffset = offsetof(struct IPCHostDirectionalChannelContext, buffer_1);
		break;

	TB_DEFAULT(bufferIndex);
	}

	return transmitBufferOffset;
}

void IPCEventProxyRunnable::readPruEvents()
{
	TB_ASSERT(!hasOverflown(), "Pru event buffer overflow");

	uint32_t bufferOffset = getTransmitBufferOffset();

	IPCDeviceIoctl device;

	do
	{
		// ;+
		// INFO("bufferOffset = " << bufferOffset);
		TransmitRegister transmitRegister;
		m_ipcHostDirectionalChannelContext->read(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));

		m_ipcHostDirectionalChannelContext->read(&device, bufferOffset, sizeof(device));

		if (device.device != IPC_EOB)
		{
			// ;+
			//INFO("readPruEvents device = " << device.device);
			//INFO("readPruEvents device = " << toDeviceString(static_cast<IPCDeviceEnum>(device.device)));

			bufferOffset += processPruEvent(device, bufferOffset);
		}
	} while (device.device != IPC_EOB);

	// ;+
	// INFO("readPruEvents acknowledge");
	acknowledge();
}

uint32_t IPCEventProxyRunnable::processPruEvent(const IPCDeviceIoctl& device, const uint32_t offset)
{
	auto deviceIter = m_eventDefinitionMap.find(static_cast<IPCDeviceEnum>(device.device));

	TB_ASSERT(deviceIter != m_eventDefinitionMap.end(), "Device = " << device.device)

	auto ioctlIter = deviceIter->second.find(device.ioctl);

	TB_ASSERT(ioctlIter != deviceIter->second.end())

	auto& action = ioctlIter->second;

	const auto size = action->size();
	TB_ASSERT(size != 0);

	// ;+
	//INFO("size = " << size);

	uint8_t data[size];
	m_ipcHostDirectionalChannelContext->read(data, offset, size);

	const auto& event = action->processEvent(m_pruId, data);
	TB_ASSERT(event);

	m_eventPublisher.publishEvent(*event);

	return size;
}

void IPCEventProxyRunnable::acknowledge()
{
	TransmitRegister transmitRegister;
	m_ipcHostDirectionalChannelContext->read(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));

	transmitRegister |= TRANSMIT_BUFFER_ACKNOWLEDGE_REG_MASK;

	m_ipcHostDirectionalChannelContext->write(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));
}

bool IPCEventProxyRunnable::hasOverflown()
{
	TransmitRegister transmitRegister;
	m_ipcHostDirectionalChannelContext->read(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));

	return (transmitRegister & TRANSMIT_BUFFER_OVERFLOW_MASK) != 0;
}




