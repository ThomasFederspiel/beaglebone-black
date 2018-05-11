/*
 * MainService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */
#include "MainService.h"

// standard
#include <memory>

// system
#include "prussdrv/pruss_intc_mapping.h"

// project
#include "CUICommands.h"
#include "exceptionMacros.h"
#include "IPCDeviceProxyEventEQEP.h"
#include "IPCDeviceProxyEventTrace.h"
#include "IPCDeviceProxyService.h"
#include "Logger.h"
#include "MainServiceBase.h"
#include "MotorRegulatorService.h"
#include "PruImageConfig.h"
#include "PruManagerService.h"
#include "PrussDriver.h"
#include "ServiceMessageBase.h"
#include "ServiceLayers.h"
#include "ServiceNames.h"
#include "SoundService.h"
#include "StreamService.h"
#include "tboxdefs.h"
#include "TelnetService.h"


MODULE_LOG(MainService);

namespace
{
static constexpr std::size_t SHRAMSize = 0x0300;
static constexpr std::size_t DDRSize = 0x1000;

static constexpr char PRU0_ARM_INTERRUPT_IPC = DRV_PRU0_ARM_INTERRUPT_1;
static constexpr char PRU0_ARM_INTERRUPT_EVENT = DRV_PRU0_ARM_INTERRUPT_2;
static constexpr char PRU1_ARM_INTERRUPT_IPC = DRV_PRU1_ARM_INTERRUPT_1;
static constexpr char ARM_PRU0_INTERRUPT_IPC = DRV_ARM_PRU0_INTERRUPT;
static constexpr char ARM_PRU1_INTERRUPT_IPC = DRV_ARM_PRU1_INTERRUPT;

static const tpruss_intc_initdata PrussINTCInitdata =
{
		{
				PRU0_ARM_INTERRUPT_IPC,
				PRU0_ARM_INTERRUPT_EVENT,
				PRU1_ARM_INTERRUPT_IPC,
				ARM_PRU0_INTERRUPT_IPC,
				ARM_PRU1_INTERRUPT_IPC,
				(char)-1
		},
		{
				{
						PRU0_ARM_INTERRUPT_IPC, CHANNEL2
				},
				{
						PRU0_ARM_INTERRUPT_EVENT, CHANNEL4
				},
				{
						PRU1_ARM_INTERRUPT_IPC, CHANNEL3
				},
				{
						ARM_PRU0_INTERRUPT_IPC, CHANNEL0
				},
				{
						ARM_PRU1_INTERRUPT_IPC, CHANNEL1
				},
				{
						-1, -1
				}
		},
		{
				{
						CHANNEL0, PRU0
				},
				{
						CHANNEL1, PRU1
				},
				{
						CHANNEL2, PrussDriver::InterruptToHostEvtout<PRU0_ARM_INTERRUPT_IPC>() // PRU_EVTOUT0
				},
				{
						CHANNEL3, PrussDriver::InterruptToHostEvtout<PRU1_ARM_INTERRUPT_IPC>() // PRU_EVTOUT1
				},
				{
						CHANNEL4, PrussDriver::InterruptToHostEvtout<PRU0_ARM_INTERRUPT_EVENT>() // PRU_EVTOUT4
				},
				{
						-1, -1
				}
		},
		/* Enable PRU0, PRU1, PRU_EVTOUT0, PRU_EVTOUT1, PRU_EVTOUT4 */
		(PRU0_HOSTEN_MASK | PRU1_HOSTEN_MASK
				| PrussDriver::InterruptToHostEnableMask<PRU0_ARM_INTERRUPT_IPC>() /* PRU_EVTOUT0_HOSTEN_MASK */
				| PrussDriver::InterruptToHostEnableMask<PRU1_ARM_INTERRUPT_IPC>() /* PRU_EVTOUT1_HOSTEN_MASK */
				| PrussDriver::InterruptToHostEnableMask<PRU0_ARM_INTERRUPT_EVENT>()) /* PRU_EVTOUT4_HOSTEN_MASK */
};
}

MainService* MainService::s_mainService = nullptr;

MainService::MainService() : m_capeManager(), m_serviceManager(), m_pru0Controller(), m_pru1Controller(),
		m_biDirectionPru0EventChannel(), m_hostDirectionPru0EventChannel(), m_biDirectionPru1EventChannel()
{
	TB_ASSERT(!s_mainService);

	s_mainService = this;
}

void MainService::onStart()
{
	registerSignals();

	loadFragments();

	setupServices();
	registerCUICommands();
	registerPruEvents();

	m_pru0Controller->setup();
	m_pru0Controller->start();

	startServices();
}

void MainService::onStop()
{
	unregisterCUICommands();
	stopServices();

	m_pru0Controller->terminate();
	m_pru0Controller->close(m_biDirectionPru0EventChannel);

	PrussDriver& prussDrv = PrussDriver::instance();
	prussDrv.exit();

	// ;+ unloadFragments();

	unregisterSignals();
}

void MainService::loadFragments()
{
	bool loaded = m_capeManager.loadFragment("pru_gpio_all");
	TB_ASSERT(loaded, "Failed to load fragment");

	loaded = m_capeManager.loadFragment("pru_enable");
	TB_ASSERT(loaded, "Failed to load fragment");
}

void MainService::unloadFragments()
{
	bool unloaded = m_capeManager.unloadFragment("pru_enable");
	TB_ASSERT(unloaded, "Failed to unload fragment");

	unloaded = m_capeManager.unloadFragment("pru_gpio_all");
	TB_ASSERT(unloaded, "Failed to unload fragment");
}

void MainService::setupServices()
{
	INFO("Setting up services");

	const PruImageConfig pru0ImageConfig(PruImageConfig::PruId::Pru0, "./pru0_main.elf.text.bin", "./pru0_main.elf.data.bin", SHRAMSize, DDRSize);
//	const PruImageConfig pru1ImageConfig(PruImageConfig::PruId::Pru1, "./pru1_main.elf.text.bin", "./pru1_main.elf.data.bin", SHRAMSize, DDRSize);

	PrussDriver& prussDrv = PrussDriver::createInstance(PrussINTCInitdata);

	m_hostDirectionPru0EventChannel = prussDrv.getPru0Proxy().openHostDirectionEventChannel(PRU0_ARM_INTERRUPT_EVENT);

	if (!m_hostDirectionPru0EventChannel)
	{
		ERROR("Creation of host directional event channel for pru 0 failed");
	}

	m_biDirectionPru0EventChannel = prussDrv.getPru0Proxy().openBiDirectionEventChannel(ARM_PRU0_INTERRUPT_IPC, PRU0_ARM_INTERRUPT_IPC);

	if (!m_biDirectionPru0EventChannel)
	{
		ERROR("Creation of bi-directional event channel for pru 0 failed");
	}

//	m_biDirectionPru1EventChannel = prussDrv.getPru1Proxy().openEventChannel(ARM_PRU1_INTERRUPT_IPC, PRU1_ARM_INTERRUPT_IPC);
//
//	if (!m_biDirectionPru1EventChannel)
//	{
//		ERROR("Creation of bi-directional event channel for pru 1 failed");
//	}

	m_pru0Controller = tbox::make_unique<PruController>(pru0ImageConfig);
//	m_pru1Controller = tbox::make_unique<PruController>(pru1ImageConfig)

	auto telnetService = std::make_shared<TelnetService>(ServiceNames::TelnetService);

	m_serviceManager.addService<SoundService>(ServiceNames::SoundService);
	m_serviceManager.addService<MotorRegulatorService>(ServiceNames::MotorRegulatorService);
	m_serviceManager.addService<StreamService>(ServiceNames::StreamService);
	m_serviceManager.addService(telnetService);

	INFO("Setting up CUI manager");
	m_serviceManager.setCUIManager(telnetService);


//	m_serviceManager.addService<ServiceLayers::PruManagerLayer, PruManagerService>(ServiceNames::PruManagerService,
//			pru0ImageConfig, pru1ImageConfig);

//	m_serviceManager.addService<ServiceLayers::PruManagerLayer, PruManagerService>(ServiceNames::PruManagerService,
//			pru0ImageConfig);

	m_serviceManager.addService<ServiceLayers::DriverLayer, IPCDeviceProxyService>(ServiceNames::Pru0ProxyService, PrussDriver::PruProxy::PruId0, m_biDirectionPru0EventChannel,
			m_hostDirectionPru0EventChannel);
//	m_serviceManager.addService<ServiceLayers::DriverLayer, IPCDeviceProxyService>(ServiceNames::Pru1ProxyService, PrussDriver::PruProxy::PruId1, m_biDirectionPru1EventChannel);

}

void MainService::registerPruEvents()
{
	auto pru0ProxyService = m_serviceManager.getService<IPCDeviceProxyService>(ServiceNames::Pru0ProxyService);
	TB_ASSERT(pru0ProxyService);

	pru0ProxyService->addPruEventDefinition<IPCDeviceProxyEventEQEP>();
	pru0ProxyService->addPruEventDefinition<IPCDeviceProxyEventTrace>();
}

void MainService::startServices()
{
	INFO("Starting services");

	m_serviceManager.startServices();
}

void MainService::stopServices()
{
	INFO("Stopping services");

	m_serviceManager.stopServices();
}

void MainService::registerCUICommands()
{
	m_serviceManager.registerCommand(tbox::make_unique<ShutdownCommand>(*this));
}

void MainService::unregisterCUICommands()
{
	m_serviceManager.unregisterCommand(ShutdownCommand(*this));
}

void MainService::registerSignals()
{
	setupSignalHandler(SIGINT, MainService::signalHandler);
	setupSignalHandler(SIGHUP, MainService::signalHandler);
	setupSignalHandler(SIGTERM, MainService::signalHandler);
}

void MainService::unregisterSignals()
{
	restoreSignalHandler(SIGINT);
	restoreSignalHandler(SIGHUP);
	restoreSignalHandler(SIGTERM);
}

void MainService::signalHandler(int sigNum, siginfo_t* pSigInfo, void* pContext)
{
	TB_ASSERT(s_mainService);

	s_mainService->shutdown();
}
