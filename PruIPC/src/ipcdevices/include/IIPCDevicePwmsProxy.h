/*
 * IIPCDevicePwmsProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IIPCDEVICEPWMSPROXY_H_
#define PRUIPC_IIPCDEVICEPWMSPROXY_H_

class IIPCDevicePwmsProxy
{
public:

	virtual void open() = 0;
	virtual void close() = 0;
};

#endif /* PRUIPC_IIPCDEVICECEPWPROXY_H_ */
