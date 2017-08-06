/*
 * Pru1Proxy.h
 *
 *  Created on: 4 jan 2016
 *      Author: Thomas
 */

#ifndef PRU1PROXY_H_
#define PRU1PROXY_H_

#include <string>

#include <PrussDriver.h>

#include "IPCDeviceGpioProxy.h"
#include "IPCDeviceTestProxy.h"
#include "VirtualMessageReceiver.h"

class IPCDeviceProxyService;
class IMemory;

class Pru1Proxy final
{
public:
	explicit Pru1Proxy(const std::string& textBin, const std::string& dataBin);

	void setup(IPCDeviceProxyService& proxy);
	void start();
	void terminate();
	void close(PrussDriver::PruEventChannelPtr eventChannel);

	void exec();

private:
	std::string m_textBin;
	std::string m_dataBin;

	PrussDriver::PruEventChannelPtr m_eventChannel;
	std::unique_ptr<IPCDeviceTestProxy> m_ipcDeviceTestProxy;
	std::unique_ptr<IPCDeviceGpioProxy> m_ipcDeviceGpioProxy;
	std::shared_ptr<IMemory> m_ddrRam;
	std::shared_ptr<IMemory> m_shram;
	VirtualMessageReceiver m_virtualReceiver;
};

#endif /* PRU1PROXY_H_ */
