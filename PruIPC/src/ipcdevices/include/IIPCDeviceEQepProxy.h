/*
 * IIPCDeviceEQepProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IIPCDEVICEEQEPPROXY_H_
#define PRUIPC_IIPCDEVICEEQEPPROXY_H_

// project
#include "pru_eqep_types.hp"

class IIPCDeviceEQepProxy
{
public:

	virtual void open() = 0;
	virtual void close() = 0;
	virtual void enableEQepQuadrature(const EQepUnitTimerPeriod utimerPeriod,
			const EQepCapClkDivisor capClkDivisor, const EQepUpEventDivisor upEventDivisor,
			const EQepCounterModeEnum mode) = 0;
	virtual void disableEQep() = 0;
};

#endif /* PRUIPC_IIPCDEVICEEQEPPROXY_H_ */
