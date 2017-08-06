/*
 * IIPCDeviceGpioProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IIPCDEVICEGPIOPROXY_H_
#define PRUIPC_IIPCDEVICEGPIOPROXY_H_

// standard
#include <stdint.h>

class IIPCDeviceGpioProxy
{
public:

	virtual void set(const uint32_t pin) = 0;
	virtual void clear(const uint32_t pin) = 0;
	virtual void toggle(const uint32_t pin) = 0;
};

#endif /* PRUIPC_IIPCDEVICEGPIOPROXY_H_ */
