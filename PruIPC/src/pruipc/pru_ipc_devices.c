/*
 * pru_ipc_devices.c
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#include "pru_ipc_devices.hp"

const char* toDeviceString(const enum IPCDeviceEnum device)
{
	#define CASE(v) case v: return #v;
	switch (device)
	{
		CASE(IPCDeviceCore)
		CASE(IPCDeviceTest)
		CASE(IPCDeviceEPwm)
		CASE(IPCDevicePwms)
		CASE(IPCDeviceGpio)
		CASE(IPCDeviceECap)
		CASE(IPCDeviceEQep)
		CASE(IPCDeviceTrace)
	}
	return "Unknown IPC device";
	#undef CASE
}

