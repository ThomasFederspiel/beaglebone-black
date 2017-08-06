/*
 * PruController.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */
#include "PruController.h"

// project
#include "Logger.h"

MODULE_LOG(PruController);

PruController::PruController(const PruImageConfig& pruConfig) : m_pruConfig(pruConfig)
{
}

void PruController::setup()
{
	INFO("Setting up " << PruImageConfig::toString(m_pruConfig.getPru()));

	PrussDriver& prussDrv = PrussDriver::instance();
	m_ddrRam = prussDrv.getDDR().allocateMemoryChunk(m_pruConfig.getDDRSize());
	m_shram = prussDrv.getSHRAM().allocateMemoryChunk(m_pruConfig.getShramSize());

	INFO("pru 0, ext mem size = 0x" << std::hex << m_ddrRam->size());
	INFO("pru 0, ext mem address = " << reinterpret_cast<void*>(m_ddrRam->baseAddress()));

	INFO("pru 0, shram mem size = 0x" << std::hex << m_shram->size());
	INFO("pru 0, shram mem address = " << reinterpret_cast<void*>(m_shram->baseAddress()));

	prussDrv.getPru0Proxy().getConfig().setDDRMemoryMap(*m_ddrRam);
	prussDrv.getPru0Proxy().getConfig().setSHRAMMemoryMap(*m_shram);
}

void PruController::start()
{
	INFO("Starting " << PruImageConfig::toString(m_pruConfig.getPru()));

	PrussDriver& prussDrv = PrussDriver::instance();

	prussDrv.getPru0Proxy().loadApplication(m_pruConfig.getTextBin(), m_pruConfig.getDataBin());
}

void PruController::terminate()
{
	INFO("Terminating " << PruImageConfig::toString(m_pruConfig.getPru()));

	PrussDriver& prussDrv = PrussDriver::instance();

	prussDrv.getPru0Proxy().getConfig().terminate();
}

void PruController::close(PrussDriver::BiDirectionPruEventChannelPtr eventChannel)
{
	INFO("Closing " << PruImageConfig::toString(m_pruConfig.getPru()));

	PrussDriver& prussDrv = PrussDriver::instance();

	/* disable pru and close memory mapping */
	prussDrv.getPru0Proxy().disable();
	eventChannel->close();
}
