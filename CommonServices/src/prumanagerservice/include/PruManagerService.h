/*
 * PruManagerService.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef PRUMANAGERSERVICE_H_
#define PRUMANAGERSERVICE_H_

// standard
#include <memory>
#include <string>
#include <vector>

// local
#include "PruImageConfig.h"

// project
#include "AbstractService.h"
#include "ServiceAllocator.h"

class IPCDeviceProxyService;
class ServiceMessageBase;

class PruManagerService final : public AbstractService
{
public:

	explicit PruManagerService(const std::string& name, const PruImageConfig& image1,
			const PruImageConfig& image2);
	explicit PruManagerService(const std::string& name, const PruImageConfig& image);

protected:

	void onMessage(const ServiceMessageBase& message) override;
	void onStart(ServiceAllocator& allocator) override;
	StopStatus onStop(ServiceAllocator& allocator) override;

private:
	void loadImages();
	void loadImage(const PruImageConfig& image);

	std::shared_ptr<IPCDeviceProxyService> m_pru0IpcProxyService;
	std::shared_ptr<IPCDeviceProxyService> m_pru1IpcProxyService;

	std::vector<PruImageConfig> m_pruImages;
};

#endif /* PRUMANAGERSERVICE_H_ */
