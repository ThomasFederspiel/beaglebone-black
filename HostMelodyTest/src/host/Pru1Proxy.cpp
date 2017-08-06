/*
 * Pru1Proxy.cpp
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "Pru1Proxy.h"

#include <chrono>
#include <thread>

#include "prussdrv/prussdrv.h"
#include "prussdrv/pruss_intc_mapping.h"

#include "IPCDeviceProxyService.h"
#include "IPCDeviceTestProxy.h"
#include "IMemory.h"
#include "Logger.h"
#include "tboxdefs.h"

MODULE_LOG(Pru1Proxy);

Pru1Proxy::Pru1Proxy(const std::string& textBin, const std::string& dataBin) : m_textBin(textBin), m_dataBin(dataBin),
		m_virtualReceiver()
{
}

void Pru1Proxy::setup(IPCDeviceProxyService& proxy)
{
	m_ipcDeviceTestProxy = tbox::make_unique<IPCDeviceTestProxy>(m_virtualReceiver, proxy);
	m_ipcDeviceGpioProxy = tbox::make_unique<IPCDeviceGpioProxy>(m_virtualReceiver, proxy);

	PrussDriver& prussDrv = PrussDriver::instance();
	m_ddrRam = prussDrv.getDDR().allocateMemoryChunk(0x1000);
	m_shram = prussDrv.getSHRAM().allocateMemoryChunk(0x0400);

	INFO("pru 1, ext mem size = 0x" << std::hex << m_ddrRam->size());
	INFO("pru 1, ext mem address = " << reinterpret_cast<void*>(m_ddrRam->baseAddress()));

	INFO("pru 1, shram mem size = 0x" << std::hex << m_shram->size());
	INFO("pru 1, shram mem address = " << reinterpret_cast<void*>(m_shram->baseAddress()));

	prussDrv.getPru1Proxy().getConfig().setDDRMemoryMap(*m_ddrRam);
	prussDrv.getPru1Proxy().getConfig().setSHRAMMemoryMap(*m_shram);
}

void Pru1Proxy::start()
{
	PrussDriver& prussDrv = PrussDriver::instance();

	prussDrv.getPru0Proxy().loadApplication(m_textBin, m_dataBin);
}

void Pru1Proxy::exec()
{
	uint32_t x[3];
	const std::size_t xSize = sizeof(x) / sizeof(x[0]);

	std::this_thread::sleep_for(std::chrono::seconds(1));
	INFO("Before trig pru1 ---");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	m_ipcDeviceTestProxy->sendTrig();
	m_virtualReceiver.wait();
	INFO("After trig pru1 ---");

	m_shram->read(x, 0x300, sizeof(x));

	INFO("Shared RAM --------------");
	for (std::size_t i = 0; i < xSize; ++i)
	{
		INFOF("0x%08x (%f)", x[i], *((float*)(x + i)));
	}
	INFO("");

	m_ddrRam->read(x, 0, sizeof(x));

	INFO("DDR RAM --------------");
	for (std::size_t i = 0; i < xSize; ++i)
	{
		INFOF("0x%08x (%f)", x[i], *((float*)(x + i)));
	}
	INFO("");

	m_ipcDeviceGpioProxy->toggle(IPCDeviceGpioProxy::PRU1_GPIO_xx_P8_29_OUT);
	m_virtualReceiver.wait();
	m_ipcDeviceGpioProxy->toggle(IPCDeviceGpioProxy::PRU1_GPIO_xx_P8_27_OUT);
	m_virtualReceiver.wait();
}

void Pru1Proxy::terminate()
{
	PrussDriver& prussDrv = PrussDriver::instance();

	INFO("Terminate pru 1");
	prussDrv.getPru1Proxy().getConfig().terminate();
}

void Pru1Proxy::close(PrussDriver::PruEventChannelPtr eventChannel)
{
	PrussDriver& prussDrv = PrussDriver::instance();

	/* disable pru and close memory mapping */
	prussDrv.getPru1Proxy().disable();
	eventChannel->close();
}
