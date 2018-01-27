/*
 * PruManagerService.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "PruManagerService.h"

// standard

// project
#include "exceptionMacros.h"
#include "IPCDeviceProxyService.h"
#include "Logger.h"
#include "CommonMessageTypes.h"
#include "PrussDriver.h"
#include "ServiceMessageBase.h"
#include "tboxdefs.h"

MODULE_LOG(PruManagerService);

using namespace commonservices;

namespace commonservices
{
	extern const char* Pru0ProxyService;
	extern const char* Pru1ProxyService;
};

PruManagerService::PruManagerService(const std::string& name, const PruImageConfig& image1,
		const PruImageConfig& image2) : AbstractService(name), m_pru0IpcProxyService(),
		m_pru1IpcProxyService(), m_pruImages({ image1, image2 })
{
}

PruManagerService::PruManagerService(const std::string& name, const PruImageConfig& image)
		: AbstractService(name), m_pru0IpcProxyService(), m_pru1IpcProxyService(),
		  m_pruImages({ image })
{
}

void PruManagerService::onStart(ServiceAllocator& allocator)
{
	m_pru0IpcProxyService = allocator.allocateService<IPCDeviceProxyService>(commonservices::Pru0ProxyService, *this);
	TB_ASSERT(m_pru0IpcProxyService);

	m_pru1IpcProxyService = allocator.allocateService<IPCDeviceProxyService>(commonservices::Pru1ProxyService, *this);
	TB_ASSERT(m_pru1IpcProxyService);

	loadImages();
}

PruManagerService::StopStatus PruManagerService::onStop(ServiceAllocator& allocator)
{
	allocator.releaseService(m_pru0IpcProxyService, *this);
	allocator.releaseService(m_pru1IpcProxyService, *this);

	return StopStatus::Done;
}

void PruManagerService::onMessage(const ServiceMessageBase& message)
{
	INFO("Got message " << CommonMessageTypes::toString(message.getType()));

	switch (message.getType())
	{
 	TB_DEFAULT(CommonMessageTypes::toString(message.getType()));
	}
}

void PruManagerService::loadImages()
{
	for (const auto& image : m_pruImages)
	{
		loadImage(image);
	}
}

void PruManagerService::loadImage(const PruImageConfig& image)
{
	PrussDriver& prussDrv = PrussDriver::instance();

	switch (image.getPru())
	{
	case PruImageConfig::PruId::Pru0:
		prussDrv.getPru0Proxy().loadApplication(image.getTextBin(), image.getDataBin());
		break;

	case PruImageConfig::PruId::Pru1:
		prussDrv.getPru1Proxy().loadApplication(image.getTextBin(), image.getDataBin());
		break;

	TB_DEFAULT(PruImageConfig::toString(image.getPru()));
	}
}

