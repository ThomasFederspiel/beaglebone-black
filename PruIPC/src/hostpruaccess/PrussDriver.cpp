/*
 * PrussDriver.cpp
 *
 *  Created on: 18 aug 2016
 *      Author: Thomas
 */

#include "PrussDriver.h"

// standard
#include <cstring>
#include <iostream>

// boots
#include <boost/numeric/conversion/cast.hpp>

// project
#include "exceptionMacros.h"
#include "Logger.h"
#include "prussdrv/prussdrv.h"
#include "prussdrv/pruss_intc_mapping.h"
#include "stdExtension.h"

MODULE_LOG(PrussDriver);

namespace
{
	static constexpr std::size_t PruConfigRamOffset = 0x0000;
	static constexpr std::size_t PruConfigRamSize = 0x0020;

	static constexpr std::size_t Pru0DataRam = 0x00000;
	static constexpr std::size_t Pru1DataRam = 0x00000;
	static constexpr std::size_t Pru0DataRamPru1 = 0x02000;
	static constexpr std::size_t Pru1DataRamPru0 = 0x02000;
	static constexpr std::size_t PruSharedRam = 0x10000;

	static const tpruss_intc_initdata Default_initdata = PRUSS_INTC_INITDATA;
}

std::unique_ptr<PrussDriver> PrussDriver::s_instance;

PrussDriver::HostDirectionPruEventChannel::HostDirectionPruEventChannel(const unsigned int hostInterrupt,
	const unsigned int sysEvent) :
	m_hostInterrupt(hostInterrupt), m_sysEvent(sysEvent)
{
}

PrussDriver::RCEnum PrussDriver::HostDirectionPruEventChannel::close()
{
	const PrussDriver::RCEnum rc = static_cast<PrussDriver::RCEnum>(prussdrv_close(m_hostInterrupt));
	return rc;
}

PrussDriver::RCEnum PrussDriver::HostDirectionPruEventChannel::waitEvent()
{
	prussdrv_pru_wait_event(m_hostInterrupt);
	prussdrv_pru_clear_event(m_hostInterrupt, m_sysEvent);

	return RCEnum::RcOk;
}

PrussDriver::RCEnum PrussDriver::HostDirectionPruEventChannel::clearEvent()
{
	return static_cast<PrussDriver::RCEnum>(prussdrv_pru_clear_event(m_hostInterrupt, m_sysEvent));
}

PrussDriver::BiDirectionPruEventChannel::BiDirectionPruEventChannel(const unsigned int hostInterrupt,
	const unsigned int eventNum,
	const unsigned int sysEvent) : HostDirectionPruEventChannel(hostInterrupt, sysEvent), m_eventNum(eventNum)
{
}

PrussDriver::RCEnum PrussDriver::BiDirectionPruEventChannel::sendEvent()
{
	return static_cast<PrussDriver::RCEnum>(prussdrv_pru_send_event(m_eventNum));
}

PrussDriver::RCEnum PrussDriver::BiDirectionPruEventChannel::sendWaitEvent()
{
	return static_cast<PrussDriver::RCEnum>(prussdrv_pru_send_wait_clear_event(m_eventNum, m_hostInterrupt, m_sysEvent));
}

PrussDriver::PruProxy::PruConfig::PruConfig(IMemory& memory) : m_memory(memory), m_config(), m_deployed(false)
{
	static_assert((sizeof(m_config) / 4) * 4 == sizeof(m_config), "Must be a multiple of four");

	std::memset(&m_config, 0, sizeof(m_config));
}

void PrussDriver::PruProxy::PruConfig::setDDRMemoryMap(const IMemory& memMap)
{
	TB_ASSERT(!m_deployed);

	m_config.ddrMap.startAddr = reinterpret_cast<uint8_t*>(memMap.baseAddress());
	m_config.ddrMap.size = boost::numeric_cast<decltype(m_config.ddrMap.size)>(memMap.size());
}

void PrussDriver::PruProxy::PruConfig::setSHRAMMemoryMap(const IMemory& memMap)
{
	TB_ASSERT(!m_deployed);

	m_config.shramMap.startAddr = reinterpret_cast<uint8_t*>(memMap.baseAddress());
	m_config.shramMap.size = boost::numeric_cast<decltype(m_config.shramMap.size)>(memMap.size());
}

void PrussDriver::PruProxy::PruConfig::setIPCBiDirectionalChannelContext(const IMemory& memMap)
{
	TB_ASSERT(!m_deployed);
	TB_ASSERT(memMap.size() == sizeof(IPCBiDirectionalChannelContext));

	m_config.ipcBiDirectionalChannelContextPtr = reinterpret_cast<IPCBiDirectionalChannelContext*>(memMap.baseAddress());
}

void PrussDriver::PruProxy::PruConfig::setIPCHostDirectionalChannelContext(const IMemory& memMap)
{
	TB_ASSERT(!m_deployed);
	TB_ASSERT(memMap.size() == sizeof(IPCHostDirectionalChannelContext));

	m_config.ipcHostDirectionalChannelContextPtr = reinterpret_cast<IPCHostDirectionalChannelContext*>(memMap.baseAddress());
}

void PrussDriver::PruProxy::PruConfig::terminate()
{
	static_assert(sizeof(m_config) <= PruConfigRamSize, "Too little RAM assigned to pru config");

	m_config.coreRegister.ctrl |= TERMINATE_CTRL_REG_MASK;

	m_memory.write(&m_config.coreRegister.ctrl, PruConfigRamOffset + offsetof(struct PruConfigLayout, coreRegister.ctrl),
			sizeof(m_config.coreRegister.ctrl));

	while (isActive());
}

bool PrussDriver::PruProxy::PruConfig::isActive()
{
	static_assert(sizeof(m_config) <= PruConfigRamSize, "Too little RAM assigned to pru config");

	m_memory.read(&m_config.coreRegister.status, PruConfigRamOffset + offsetof(struct PruConfigLayout, coreRegister.status),
			sizeof(m_config.coreRegister.status));

	return (m_config.coreRegister.status & PRU_ACTIVE_STATUS_REG_MASK) != 0;
}

PrussDriver::RCEnum PrussDriver::PruProxy::PruConfig::deploy()
{
	static_assert(sizeof(m_config) <= PruConfigRamSize, "Too little RAM assigned to pru config");

	m_memory.write(&m_config, PruConfigRamOffset, sizeof(m_config));

	m_deployed = true;

	return PrussDriver::RCEnum::RcOk;
}

void PrussDriver::PruProxy::PruConfig::init(const IMemory& ddrMemory, const IMemory& shramMemory)
{
	setDDRMemoryMap(ddrMemory);
	setSHRAMMemoryMap(shramMemory);
}

PrussDriver::PruProxy::PruProxy(const PruIdEnum pruId, PrussDriver& prussDriver) : m_pruId(pruId),
	m_prussDriver(prussDriver), m_ramMemory(PrussDriver::PruProxy::toRAMId(pruId)), m_pruConfig(m_ramMemory)
{
}

PrussDriver::PruProxy::PruConfig& PrussDriver::PruProxy::getConfig()
{
	return m_pruConfig;
}

PrussDriver::RCEnum PrussDriver::PruProxy::enable()
{
	return static_cast<PrussDriver::RCEnum>(prussdrv_pru_enable(m_pruId));
}

PrussDriver::RCEnum PrussDriver::PruProxy::enable(const size_t address)
{
	return static_cast<PrussDriver::RCEnum>(prussdrv_pru_enable_at(m_pruId, address));
}

PrussDriver::RCEnum PrussDriver::PruProxy::disable()
{
	return static_cast<PrussDriver::RCEnum>(prussdrv_pru_disable(m_pruId));
}

PrussDriver::RCEnum PrussDriver::PruProxy::reset()
{
	return static_cast<PrussDriver::RCEnum>(prussdrv_pru_reset(m_pruId));
}


PrussDriver::RCEnum PrussDriver::PruProxy::loadApplication(const std::string& codeFilepath,
		const std::string& dataFilepath)
{
	PrussDriver::RCEnum result = loadDataRam(dataFilepath);

	if (result == PrussDriver::RCEnum::RcOk)
	{
		result = exec(codeFilepath);

		if (result == PrussDriver::RCEnum::RcOk)
		{
			while (!m_pruConfig.isActive());
		}
	}

	return result;
}

PrussDriver::RCEnum PrussDriver::PruProxy::loadDataRam(const std::string& filepath)
{
	PrussDriver::RCEnum result = static_cast<PrussDriver::RCEnum>(prussdrv_load_datafile(m_pruId, filepath.c_str()));

	if (result == PrussDriver::RCEnum::RcOk)
	{
		result = m_pruConfig.deploy();
	}

	return result;
}

PrussDriver::RCEnum PrussDriver::PruProxy::exec(const std::string& filepath)
{
	return static_cast<PrussDriver::RCEnum>(prussdrv_exec_program(m_pruId, filepath.c_str()));
}

PrussDriver::RCEnum PrussDriver::PruProxy::exec(const std::string& filepath, const size_t address)
{
	return static_cast<PrussDriver::RCEnum>(prussdrv_exec_program_at(m_pruId, filepath.c_str(), address));
}

RAMMemory::RamIdEnum PrussDriver::PruProxy::toRAMId(const PrussDriver::PruProxy::PruIdEnum pruId)
{
	RAMMemory::RamIdEnum id = RAMMemory::RamIdEnum::RamIdEOE;

	switch (pruId)
	{
	case PruIdEnum::PruId0:
		id = RAMMemory::RamIdEnum::RamPru0Id;
		break;
	case PruIdEnum::PruId1:
		id = RAMMemory::RamIdEnum::RamPru1Id;
		break;

	TB_DEFAULT(pruId);
	}

	return id;
}

PrussDriver::BiDirectionPruEventChannelPtr PrussDriver::PruProxy::openBiDirectionEventChannel(const unsigned int eventNum, const unsigned int sysEvent)
{
	unsigned int hostInterrupt = -1;

	const RCEnum rc = m_prussDriver.getHostInterrupt(sysEvent, hostInterrupt);

	TB_ASSERT(rc == RCEnum::RcOk);

	if (rc == RCEnum::RcOk)
	{
		return openBiDirectionEventChannel(hostInterrupt, eventNum, sysEvent);
	}

	return PrussDriver::BiDirectionPruEventChannelPtr();
}

PrussDriver::BiDirectionPruEventChannelPtr PrussDriver::PruProxy::openBiDirectionEventChannel(const unsigned int hostInterrupt, const unsigned int eventNum, const unsigned int sysEvent)
{
	TB_ASSERT(eventNum == ARM_PRU0_INTERRUPT || eventNum == ARM_PRU1_INTERRUPT);

	PrussDriver::BiDirectionPruEventChannelPtr channel;

	if (m_prussDriver.open(hostInterrupt) == RCEnum::RcOk)
	{
		channel = std::make_shared<PrussDriver::BiDirectionPruEventChannel>(hostInterrupt, eventNum, sysEvent);
	}

	return channel;
}

PrussDriver::HostDirectionPruEventChannelPtr PrussDriver::PruProxy::openHostDirectionEventChannel(const unsigned int sysEvent)
{
	unsigned int hostInterrupt = -1;

	const RCEnum rc = m_prussDriver.getHostInterrupt(sysEvent, hostInterrupt);

	TB_ASSERT(rc == RCEnum::RcOk);

	if (rc == RCEnum::RcOk)
	{
		return openHostDirectionEventChannel(hostInterrupt, sysEvent);
	}

	return PrussDriver::HostDirectionPruEventChannelPtr();
}

PrussDriver::HostDirectionPruEventChannelPtr PrussDriver::PruProxy::openHostDirectionEventChannel(const unsigned int hostInterrupt, const unsigned int sysEvent)
{
	PrussDriver::HostDirectionPruEventChannelPtr channel;

	if (m_prussDriver.open(hostInterrupt) == RCEnum::RcOk)
	{
		channel = std::make_shared<PrussDriver::HostDirectionPruEventChannel>(hostInterrupt, sysEvent);
	}

	return channel;
}

RAMMemory& PrussDriver::PruProxy::getRAM()
{
	return m_ramMemory;
}

PrussDriver::PrussDriver() : PrussDriver(Default_initdata)
{
}

PrussDriver::PrussDriver(const tpruss_intc_initdata& initdata) : m_state(PrussDriver::StateEnum::StateClosed),
		m_pru0(PrussDriver::PruProxy::PruIdEnum::PruId0, *this),
		m_pru1(PrussDriver::PruProxy::PruIdEnum::PruId1, *this), m_ddrMemory(), m_shramMemory()
{
	std::memcpy(&m_pruss_intc_initdata, &initdata, sizeof(tpruss_intc_initdata));

	(void)prussdrv_init();
}

PrussDriver::~PrussDriver()
{
	(void)exit();
}

PrussDriver& PrussDriver::createInstance()
{
	return createInstance(Default_initdata);
}

PrussDriver& PrussDriver::createInstance(const tpruss_intc_initdata& initdata)
{
	TB_ASSERT(!s_instance);

	if (!s_instance)
	{
		s_instance.reset(new PrussDriver(initdata));
	}

	return *s_instance;
}

PrussDriver& PrussDriver::instance()
{
	TB_ASSERT(s_instance);

	return *s_instance;
}

int PrussDriver::getVersionId() const
{
	return prussdrv_version();
}

std::string PrussDriver::getVersion() const
{
	return std::string(prussdrv_strversion(getVersionId()));
}

PrussDriver::RCEnum PrussDriver::getHostInterrupt(const unsigned int sysEvent, unsigned int& hostInterrupt)
{
    std::size_t i = 0;

	while (m_pruss_intc_initdata.sysevt_to_channel_map[i].sysevt != -1)
	{
		if (static_cast<unsigned int>(m_pruss_intc_initdata.sysevt_to_channel_map[i].sysevt) == sysEvent)
		{
			const short channel = m_pruss_intc_initdata.sysevt_to_channel_map[i].channel;

			i = 0;

			while (m_pruss_intc_initdata.channel_to_host_map[i].channel != -1)
			{
				if (m_pruss_intc_initdata.channel_to_host_map[i].channel == channel)
				{
					hostInterrupt = m_pruss_intc_initdata.channel_to_host_map[i].host - EvtoutPruOffset;

					return RCEnum::RcOk;
				}

				++i;
			}
		}

		++i;
	}

	return RCEnum::RcError;
}

PrussDriver::RCEnum PrussDriver::open(const unsigned int hostInterrupt)
{
	const PrussDriver::RCEnum rc = static_cast<PrussDriver::RCEnum>(prussdrv_open(hostInterrupt));

	if ((rc == RCEnum::RcOk) && (m_state == StateEnum::StateClosed))
	{
		prussdrv_pruintc_init(&m_pruss_intc_initdata);

		initShramAndDDR();

		m_pru0.m_pruConfig.init(*m_ddrMemory, *m_shramMemory);
		m_pru1.m_pruConfig.init(*m_ddrMemory, *m_shramMemory);

		m_state = StateEnum::StateOpened;

		return rc;
	}

	return rc;
}

PrussDriver::RCEnum PrussDriver::exit()
{
	if (m_state == StateEnum::StateOpened)
	{
		m_state = StateEnum::StateClosed;

		(void)m_pru0.disable();
		(void)m_pru1.disable();

		return static_cast<PrussDriver::RCEnum>(prussdrv_exit());
	}

	return RCEnum::RcError;
}

PrussDriver::PruProxy& PrussDriver::getPruProxy(const PrussDriver::PruProxy::PruIdEnum pruId)
{
	PruProxy* proxy = nullptr;

	switch (pruId)
	{
	case PrussDriver::PruProxy::PruId0:
		proxy = &m_pru0;
		break;

	case PrussDriver::PruProxy::PruId1:
		proxy = &m_pru1;
		break;

	TB_DEFAULT(pruId);
	}

	return *proxy;
}

PrussDriver::PruProxy& PrussDriver::getPru0Proxy()
{
	return m_pru0;
}

PrussDriver::PruProxy& PrussDriver::getPru1Proxy()
{
	return m_pru1;
}

SHRAMMemory& PrussDriver::getSHRAM()
{
	TB_ASSERT(m_shramMemory);

	return *m_shramMemory;
}

DDRMemory& PrussDriver::getDDR()
{
	TB_ASSERT(m_ddrMemory);

	return *m_ddrMemory;
}

void PrussDriver::initShramAndDDR()
{
	m_shramMemory = std::make_unique<SHRAMMemory>();
	m_ddrMemory = std::make_unique<DDRMemory>();
}
