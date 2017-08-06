// standard
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>

// system
#include "prussdrv/prussdrv.h"
#include "prussdrv/pruss_intc_mapping.h"

// Project
#include "PrussDriver.h"
#include "IMemory.h"
#include "IPCDeviceProxy.h"
#include "IPCDeviceCoreProxy.h"
#include "IPCDeviceEPwmProxy.h"
#include "IPCDevicePwmsProxy.h"
#include "Logger.h"

#include "StopWatch.h"
#include "Melody.h"
#include "CapeManager.h"
#include "exceptionMacros.h"

#include "NoteGeneratorAdapter.h"
#include "PlayNote.h"
#include "Pru0Proxy.h"
#include "Pru1Proxy.h"

#include "pru_ipc_device_test.hp"
#include "pru_ipc_devices.hp"
#include "pru_shram_layout.hp"

#include "ServiceNames.h"
#include "ServiceManager.h"
#include "ServiceLayers.h"
#include "SoundService.h"
#include "IPCDeviceProxyService.h"
#include "ServiceLayers.h"

#include "VirtualMessageReceiver.h"

#include "PruManagerService.h"
#include "PruImageConfig.h"

#include "MotorRegulatorService.h"
#include "MotionMessage.h"

#include "TelnetService.h"

#define USE_PRU0 1

#ifdef USE_PRU0

#define PRU_NUM 0
#define PRUSS_SHARED_DATARAM PRUSS0_SHARED_DATARAM
#define ARM_PRU_INTERRUPT ARM_PRU0_INTERRUPT
#define PRU_ARM_INTERRUPT PRU0_ARM_INTERRUPT

#else

#define PRU_NUM 1
#define PRUSS_SHARED_DATARAM PRUSS1_SHARED_DATARAM
#define ARM_PRU_INTERRUPT ARM_PRU1_INTERRUPT
#define PRU_ARM_INTERRUPT PRU1_ARM_INTERRUPT

#endif

#define LOOPS	3

#define DDR_SIZE	0x1000
#define SHRAM_SIZE	0x0300

APPLICATION_LOG(MelodyTest, HostMain)

void tbAssertHook()
{
	INFO("tbAssertHook()");
}

static ServiceManager m_serviceManager;
static PrussDriver::PruEventChannelPtr m_pru0EventChannel;
static PrussDriver::PruEventChannelPtr m_pru1EventChannel;

void playSoundService()
{
	auto soundService = m_serviceManager.getService<SoundService>(melodyservice::ServiceNames::SoundService);
	TB_ASSERT(soundService);

	soundService->post(SoundMessage(SoundMessage::Sound::MarioMainTheme));
}

void runMotor()
{
	auto motorService = m_serviceManager.getService<MotorRegulatorService>(melodyservice::ServiceNames::MotorRegulatorService);
	TB_ASSERT(motorService);

	INFO("Speed 100%");

	motorService->post(MotionMessage(1.0f, 1.0f));
//	motorService->post(MotionMessage(MotionMessage::Motor::RightMotor, 1.0f));

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	INFO("Speed 90%");

	motorService->post(MotionMessage(0.9f, 0.9f));
//	motorService->post(MotionMessage(MotionMessage::Motor::RightMotor, 0.9f));

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	INFO("Speed 80%");

	motorService->post(MotionMessage(0.8f, 0.8f));
//	motorService->post(MotionMessage(MotionMessage::Motor::RightMotor, 0.8f));

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	INFO("Speed 70%");

	motorService->post(MotionMessage(0.7f, 0.7f));
//	motorService->post(MotionMessage(MotionMessage::Motor::RightMotor, 0.7f));

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	INFO("Speed 60%");

	motorService->post(MotionMessage(0.6f, 0.6f));
//	motorService->post(MotionMessage(MotionMessage::Motor::RightMotor, 0.6f));

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	INFO("High");

	motorService->post(MotionMessage(MotionMessage::Motor::AllMotors, MotionMessage::MotorAction::Stop));
//	motorService->post(MotionMessage(MotionMessage::Motor::RightMotor, MotionMessage::MotorAction::Stop));

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	INFO("Low");

	motorService->post(MotionMessage(MotionMessage::Motor::AllMotors, MotionMessage::MotorAction::Coast));
//	motorService->post(MotionMessage(MotionMessage::Motor::RightMotor, MotionMessage::MotorAction::Coast));

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

void setupServices(const PruImageConfig& pru0, const PruImageConfig& pru1)
{
	INFO("Setting up services");

	PrussDriver& prussDrv = PrussDriver::instance();
	m_pru0EventChannel = prussDrv.getPru0Proxy().openEventChannel(ARM_PRU0_INTERRUPT, PRU0_ARM_INTERRUPT);

	if (!m_pru0EventChannel)
	{
		ERROR("Creation of event channel for pru 0 failed");
	}

	m_pru1EventChannel = prussDrv.getPru1Proxy().openEventChannel(ARM_PRU1_INTERRUPT, PRU1_ARM_INTERRUPT);

	if (!m_pru1EventChannel)
	{
		ERROR("Creation of event channel for pru 1 failed");
	}

	auto telnetService = std::make_shared<TelnetService>(melodyservice::ServiceNames::TelnetService);

	m_serviceManager.addService<SoundService>(melodyservice::ServiceNames::SoundService);
	m_serviceManager.addService<MotorRegulatorService>(melodyservice::ServiceNames::MotorRegulatorService);
	m_serviceManager.addService(telnetService);
	m_serviceManager.setCUIManager(telnetService);

	m_serviceManager.addService<melodyservice::ServiceLayers::PruManagerLayer, PruManagerService>(melodyservice::ServiceNames::PruManagerService,
			pru0, pru1);

	m_serviceManager.addService<melodyservice::ServiceLayers::DriverLayer, IPCDeviceProxyService>(melodyservice::ServiceNames::Pru0ProxyService, PrussDriver::PruProxy::PruId0, m_pru0EventChannel);
	m_serviceManager.addService<melodyservice::ServiceLayers::DriverLayer, IPCDeviceProxyService>(melodyservice::ServiceNames::Pru1ProxyService, PrussDriver::PruProxy::PruId1, m_pru1EventChannel);
}

void startServices()
{
	INFO("Starting services");

	m_serviceManager.startServices();
}

void stopServices()
{
	INFO("Stopping services");

	m_serviceManager.stopServices();
}

/* main */

int main(int ac, char** av)
{
  setupSignalHandlers();

  CapeManager capeManager;

  bool loaded = capeManager.loadFragment("pru_gpio_all");
  TB_ASSERT(loaded, "Failed to load fragment");

  loaded = capeManager.loadFragment("pru_enable");
  TB_ASSERT(loaded, "Failed to load fragment");

  PrussDriver& prussDrv = PrussDriver::instance();

  Pru0Proxy pru0("./pru0_main.elf.text.bin", "./pru0_main.elf.data.bin");
  Pru1Proxy pru1("./pru1_main.elf.text.bin", "./pru1_main.elf.data.bin");

  const PruImageConfig pru0ImageConfig(PruImageConfig::PruId::Pru0, "./pru0_main.elf.text.bin", "./pru0_main.elf.data.bin", SHRAM_SIZE, DDR_SIZE);
  const PruImageConfig pru1ImageConfig(PruImageConfig::PruId::Pru1, "./pru1_main.elf.text.bin", "./pru1_main.elf.data.bin", SHRAM_SIZE, DDR_SIZE);

  setupServices(pru0ImageConfig, pru1ImageConfig);

  pru0.setup(*m_serviceManager.getService<IPCDeviceProxyService>(melodyservice::ServiceNames::Pru0ProxyService));
  pru1.setup(*m_serviceManager.getService<IPCDeviceProxyService>(melodyservice::ServiceNames::Pru1ProxyService));

  INFO("ext mem size = 0x" << std::hex << prussDrv.getDDR().size());
  INFO("ext mem address = " << reinterpret_cast<void*>(prussDrv.getDDR().baseAddress()));

  pru0.start();
  pru1.start();

  startServices();

  std::this_thread::sleep_for(std::chrono::seconds(1));

//  playSoundService();
//  runMotor();

  INFO("Wait");
  std::this_thread::sleep_for(std::chrono::milliseconds(3000000));

//  uint8_t j = 0;
//
//  while (j < LOOPS)
//  {
//	++j;
//
//	pru0.exec();
//	pru1.exec();
//  }

  stopServices();

  pru0.terminate();
  pru1.terminate();

  pru0.close(m_pru0EventChannel);
  pru1.close(m_pru1EventChannel);

  prussDrv.exit();

#if 0
  bool unloaded = capeManager.unloadFragment("pru_enable");
  TB_ASSERT(unloaded, "Failed to unload fragment");

  unloaded = capeManager.unloadFragment("pru_gpio_all");
  TB_ASSERT(unloaded, "Failed to unload fragment");
#endif

  return 0;
}
