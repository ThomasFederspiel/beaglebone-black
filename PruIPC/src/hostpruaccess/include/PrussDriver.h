/*
 * PrussDriver.h
 *
 *  Created on: 4 jan 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_PRUSSDRIVER_H_
#define PRUIPC_PRUSSDRIVER_H_

// system
#include "prussdrv/pruss_intc_mapping.h"

// standard
#include <string>
#include <cstdint>
#include <cstdlib>
#include <memory>

// local
#include "RAMMemory.h"
#include "SHRAMMemory.h"
#include "DDRMemory.h"

// project
#include "pru_ipc_types.hp"
#include "pru_config_types.hp"

#define DRV_PRU0_PRU1_INTERRUPT     17
#define DRV_PRU1_PRU0_INTERRUPT     18
#define DRV_PRU0_ARM_INTERRUPT_1    19
#define DRV_PRU1_ARM_INTERRUPT_1    20
#define DRV_ARM_PRU0_INTERRUPT      21
#define DRV_ARM_PRU1_INTERRUPT      22
#define DRV_PRU0_ARM_INTERRUPT_2    23

class PrussDriver final
{
private:
	static constexpr short EvtoutMinInterrupt = 19;
	static constexpr short EvtoutMaxInterrupt = 26;
	static constexpr short EvtoutPruOffset = 2;
	static constexpr short EvtoutBaseHostEnableMask = 0x0004;

public:
	enum class RCEnum : std::int8_t
	{
		RcError = -1,
		RcOk = 0
	};

	class HostDirectionPruEventChannel
	{
	public:
		explicit HostDirectionPruEventChannel(const unsigned int hostInterrupt,
				const unsigned int m_sysEvent);

		RCEnum close();

		RCEnum waitEvent();
		RCEnum clearEvent();

	protected:
		const unsigned int m_hostInterrupt;
		const unsigned int m_sysEvent;
	};

	class BiDirectionPruEventChannel final : public HostDirectionPruEventChannel
	{
	public:
		explicit BiDirectionPruEventChannel(const unsigned int hostInterrupt,
				const unsigned int m_eventNum,
				const unsigned int m_sysEvent);

		RCEnum sendEvent();
		RCEnum sendWaitEvent();

	private:
		const unsigned int m_eventNum;
	};

	using BiDirectionPruEventChannelPtr = std::shared_ptr<PrussDriver::BiDirectionPruEventChannel>;
	using HostDirectionPruEventChannelPtr = std::shared_ptr<PrussDriver::HostDirectionPruEventChannel>;

	class PruProxy final
	{
	public:
		friend class PrussDriver;

		enum PruIdEnum : unsigned int
		{
			PruId0 = 0,
			PruId1 = 1,
			PruIdEOE
		};

		class PruConfig final
		{
		public:
			friend class PrussDriver;
			friend class PruProxy;

			PruConfig(IMemory& memory);

			void setDDRMemoryMap(const IMemory& memMap);
			void setSHRAMMemoryMap(const IMemory& memMap);
			void setIPCBiDirectionalChannelContext(const IMemory& memMap);
			void setIPCHostDirectionalChannelContext(const IMemory& memMap);

			void terminate();
			bool isActive();

		private:
			RCEnum deploy();
			void init(const IMemory& ddrMemory, const IMemory& shramMemory);

			IMemory& m_memory;
			struct PruConfigLayout m_config;
			bool m_deployed;
		};

		PruProxy(const PruIdEnum pruId, PrussDriver& prussDriver);

		RAMMemory& getRAM();

		RCEnum enable();
		RCEnum enable(const size_t address);
		RCEnum disable();
		RCEnum reset();
		RCEnum loadApplication(const std::string& codeFilepath, const std::string& dataFilepath);

		PruConfig& getConfig();

		BiDirectionPruEventChannelPtr openBiDirectionEventChannel(const unsigned int eventNum, const unsigned int sysEvent);
		HostDirectionPruEventChannelPtr openHostDirectionEventChannel(const unsigned int sysEvent);

	private:
		static RAMMemory::RamIdEnum toRAMId(const PruIdEnum pruId);

		RCEnum loadDataRam(const std::string& filepath);
		RCEnum exec(const std::string& filepath);
		RCEnum exec(const std::string& filepath, const size_t address);

		BiDirectionPruEventChannelPtr openBiDirectionEventChannel(const unsigned int hostInterrupt,
				const unsigned int eventNum, const unsigned int sysEvent);
		HostDirectionPruEventChannelPtr openHostDirectionEventChannel(const unsigned int hostInterrupt,
				const unsigned int sysEvent);

		PruIdEnum m_pruId;
		PrussDriver& m_prussDriver;
		RAMMemory m_ramMemory;

		PruConfig m_pruConfig;
	};

	friend class PruProxy;

	~PrussDriver();

	static PrussDriver& instance();
	static PrussDriver& createInstance();
	static PrussDriver& createInstance(const tpruss_intc_initdata& initdata);

	int getVersionId() const;
	std::string getVersion() const;

	RCEnum exit();

	SHRAMMemory& getSHRAM();
	DDRMemory& getDDR();

	PruProxy& getPruProxy(const PrussDriver::PruProxy::PruIdEnum pruId);
	PruProxy& getPru0Proxy();
	PruProxy& getPru1Proxy();

	template <short interrupt>
	static constexpr short InterruptToHostEvtout()
	{
		static_assert((interrupt >= EvtoutMinInterrupt) && (interrupt <= EvtoutMaxInterrupt),
				"Bad interrupt number");

		return interrupt - EvtoutMinInterrupt + EvtoutPruOffset;
	}

	template <short interrupt>
	static constexpr short InterruptToHostEnableMask()
	{
		static_assert((interrupt >= EvtoutMinInterrupt) && (interrupt <= EvtoutMaxInterrupt),
				"Bad interrupt number");

		return EvtoutBaseHostEnableMask << (interrupt - EvtoutMinInterrupt);
	}

private:
	enum class StateEnum : uint8_t
	{
		StateClosed = 0,
		StateOpened
	};

	PrussDriver();
	explicit PrussDriver(const tpruss_intc_initdata& initdata);

	RCEnum getHostInterrupt(const unsigned int sysEvent, unsigned int& hostInterrupt);
	RCEnum open(const unsigned int hostInterrupt);
	void initShramAndDDR();

	static std::unique_ptr<PrussDriver> s_instance;

	StateEnum m_state;

	PruProxy m_pru0;
	PruProxy m_pru1;

	std::unique_ptr<DDRMemory> m_ddrMemory;
	std::unique_ptr<SHRAMMemory> m_shramMemory;

	tpruss_intc_initdata m_pruss_intc_initdata;
};

#endif /* PRUIPC_PRUSSDRIVER_H_ */
