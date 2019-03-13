/*
 * IPCEventProxyRunnable.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "IPCEventProxyRunnable.h"

// standard
#include <chrono>

#ifdef INSTRUMENTATION_LTTNG
// lttng
#include <lttng/tracelog.h>
#endif

// project
#include "AbstractService.h"
#include "exceptionMacros.h"
#include "IPCMessageTypes.h"
#include "Logger.h"
#include "LoggerUtil.h"
#include "pru_ipc_types.hp"
#include "pru_ipc_devices.hp"
#include "tboxdefs.h"

// local
#include "PruEventDefinition.h"

MODULE_LOG(IPCEventProxyRunnable);

namespace
{
static constexpr uint8_t CorruptCtrlValue = 0xAA;
}

IPCEventProxyRunnable::IPCEventProxyRunnable(const std::string& name, IEventMessagePublisher& publisher,
		const PrussDriver::PruProxy::PruIdEnum pruId, PrussDriver::HostDirectionPruEventChannelPtr eventChannel) : m_name(name),
		m_eventPublisher(publisher), m_pruId(pruId), m_terminated(false), m_eventDefinitionMap(), m_eventChannel(eventChannel),
		m_eventRegistrationDone(false)
{
	TB_ASSERT(m_eventChannel != nullptr);

	// ;+
	//	setPolicy(IScheduler::POLICY_RR, 10);

	PrussDriver& prussDrv = PrussDriver::instance();
	m_ipcHostDirectionalChannelContext = prussDrv.getSHRAM().allocateMemoryChunk(sizeof(struct IPCHostDirectionalChannelContext));
	TB_ASSERT(m_ipcHostDirectionalChannelContext);

	// used for detecting overflow
	m_ipcHostDirectionalChannelContext->fill(CorruptCtrlValue);

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

	stopBuffer();
}


#ifdef INSTRUMENTATION_LTTNG
#include "StopWatch.h"
static StopWatch<std::chrono::microseconds> w2;
#endif

void IPCEventProxyRunnable::operator()()
{
	enableBuffer();

	while (!m_terminated)
	{
		m_eventChannel->waitEvent();

#ifdef INSTRUMENTATION_LTTNG
		w2.start();
		tracelog(TRACE_INFO, "Start process pru event");
#endif

		if (!m_terminated)
		{
			// Avoid processing event if spurious PRU event
			if (isBufferActive())
			{
				readPruEvents();
			}
		}

#ifdef INSTRUMENTATION_LTTNG
		const auto dur = w2.duration(true);
		tracelog(TRACE_INFO, "End process pru event, time = %d ns", dur);
#endif
	}

	const auto rc = m_eventChannel->close();

	TB_ASSERT(rc == PrussDriver::RCEnum::RcOk, "rc = " << PrussDriver::toString(rc));
}

uint32_t IPCEventProxyRunnable::getTransmitBufferOffset()
{
	uint32_t transmitBufferOffset = 0;

	const uint32_t transmitBufferIndexOffset = offsetof(struct IPCHostDirectionalChannelContext, transmitBufferIndex);

	uint32_t bufferIndex = UNDEFINED_INDEX;
	m_ipcHostDirectionalChannelContext->read(&bufferIndex, transmitBufferIndexOffset, sizeof(bufferIndex));

	TB_ASSERT(bufferIndex != UNDEFINED_INDEX);

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

	TB_ASSERT(transmitBufferOffset % 2 == 0);

	return transmitBufferOffset;
}

void IPCEventProxyRunnable::readPruEvents()
{
	TB_ASSERT(!hasOverflown(), "Pru event buffer overflow");

	uint32_t bufferOffset = getTransmitBufferOffset();

	TB_ASSERT(!isBufferCorrupted(bufferOffset), "Pru event buffer corrupted");

	IPCDeviceIoctl device;

	do
	{
		m_ipcHostDirectionalChannelContext->read(&device, bufferOffset, sizeof(device));

		if (device.device != IPC_EOB)
		{
			bufferOffset += processPruEvent(device, bufferOffset);
		}
	} while (device.device != IPC_EOB);

	acknowledge();
}

uint32_t IPCEventProxyRunnable::processPruEvent(const IPCDeviceIoctl& device, const uint32_t offset)
{
	TB_ASSERT(offset % 2 == 0);

	auto deviceIter = m_eventDefinitionMap.find(static_cast<IPCDeviceEnum>(device.device));

	TB_ASSERT(deviceIter != m_eventDefinitionMap.end(), "Device = " << device.device)

	auto ioctlIter = deviceIter->second.find(device.ioctl);

	TB_ASSERT(ioctlIter != deviceIter->second.end())

	auto& action = ioctlIter->second;

	const auto size = action->size();
	TB_ASSERT(size != 0);
	TB_ASSERT(size % 2 == 0);

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

void IPCEventProxyRunnable::stopBuffer()
{
	TransmitRegister transmitRegister;
	m_ipcHostDirectionalChannelContext->read(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));

	transmitRegister |= TRANSMIT_BUFFER_TERMINATE_REG_MASK;

	m_ipcHostDirectionalChannelContext->write(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));
}

void IPCEventProxyRunnable::enableBuffer()
{
	TransmitRegister transmitRegister;
	m_ipcHostDirectionalChannelContext->read(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));

	transmitRegister &= ~TRANSMIT_BUFFER_DISABLED_REG_MASK;

	m_ipcHostDirectionalChannelContext->write(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));
}

bool IPCEventProxyRunnable::hasOverflown()
{
	TransmitRegister transmitRegister;
	m_ipcHostDirectionalChannelContext->read(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));

	return (transmitRegister & TRANSMIT_BUFFER_OVERFLOW_REG_MASK) != 0;
}

bool IPCEventProxyRunnable::isBufferActive()
{
	TransmitRegister transmitRegister;
	m_ipcHostDirectionalChannelContext->read(&transmitRegister, offsetof(struct IPCHostDirectionalChannelContext, transmitRegister), sizeof(TransmitRegister));

	return (transmitRegister & TRANSMIT_BUFFER_ACTIVE_REG_MASK) != 0;
}

bool IPCEventProxyRunnable::isBufferCorrupted(const uint32_t bufferOffset)
{
	uint8_t checkPattern = 0;
	m_ipcHostDirectionalChannelContext->read(&checkPattern, bufferOffset + PRU_IPC_TX_BUFFER_EXTENDED_SIZE - OVERFLOW_CTRL_BYTES, sizeof(checkPattern));

	return checkPattern != CorruptCtrlValue;
}

void IPCEventProxyRunnable::dumpBuffer()
{
	const uint32_t bufferOffset = getTransmitBufferOffset();
	const std::size_t size = PRU_IPC_TX_BUFFER_EXTENDED_SIZE;

	uint8_t data[size];
	m_ipcHostDirectionalChannelContext->read(data, bufferOffset, size);

	LOG_MEMDUMP_CUSTOM(data, size, 16, false);
}


