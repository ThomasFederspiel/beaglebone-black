/*
 * IPCDeviceGpioProxy.h
 *
 *  Created on: 25 dec 2015
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEGPIOPROXY_H_
#define PRUIPC_IPCDEVICEGPIOPROXY_H_

// system
#include <cstdint>

// project
#include "pru_gpio.hp"

// local
#include "AbstractIPCDeviceProxy.h"
#include "IIPCDeviceGpioProxy.h"

class IMessageReceiver;
class IPCDeviceProxyService;

class IPCDeviceGpioProxy final : public IIPCDeviceGpioProxy, protected AbstractIPCDeviceProxy
{
private:
	static constexpr uint32_t PRU0_BIT = BIT(31);
	static constexpr uint32_t PRU1_BIT = BIT(30);
	static constexpr uint32_t IN_BIT = BIT(29);
	static constexpr uint32_t OUT_BIT = BIT(28);

public:

	enum GpioPins
	{
		PRU1_GPIO_63_P8_20_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_63_P8_20_OUT,
		PRU1_GPIO_63_P8_20_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_63_P8_20_IN,

		// out:30.t12, in:31.t12 - P8_21, GPIO xx
		PRU1_GPIO_xx_P8_21_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_21_OUT,
		PRU1_GPIO_xx_P8_21_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_21_IN,

		// out:30.t11, in:31.t11 - P8_30, GPIO xx
		PRU1_GPIO_xx_P8_30_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_30_OUT,
		PRU1_GPIO_xx_P8_30_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_30_IN,

		// out:30.t10, in:31.t10 - P8_28, GPIO xx
		PRU1_GPIO_xx_P8_28_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_28_OUT,
		PRU1_GPIO_xx_P8_28_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_28_IN,

		// out:30.t9, in:31.t9 - P8_29, GPIO xx
		PRU1_GPIO_xx_P8_29_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_29_OUT,
		PRU1_GPIO_xx_P8_29_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_29_IN,

		// out:30.t8, in:31.t8 - P8_27, GPIO xx
		PRU1_GPIO_xx_P8_27_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_27_OUT,
		PRU1_GPIO_xx_P8_27_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_27_IN,

		// out:30.t7, in:31.t7 - P8_40, GPIO xx
		PRU1_GPIO_xx_P8_40_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_40_OUT,
		PRU1_GPIO_xx_P8_40_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_40_IN,

		// out:30.t6, in:31.t6 - P8_39, GPIO xx
		PRU1_GPIO_xx_P8_39_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_39_OUT,
		PRU1_GPIO_xx_P8_39_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_39_IN,

		// out:30.t5, in:31.t5 - P8_42, GPIO xx
		PRU1_GPIO_xx_P8_42_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_42_OUT,
		PRU1_GPIO_xx_P8_42_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_42_IN,

		// out:30.t4, in:31.t4 - P8_41, GPIO xx
		PRU1_GPIO_xx_P8_41_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_41_OUT,
		PRU1_GPIO_xx_P8_41_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_41_IN,

		// out:30.t3, in:31.t3 - P8_44, GPIO xx
		PRU1_GPIO_xx_P8_44_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_44_OUT,
		PRU1_GPIO_xx_P8_44_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_44_IN,

		// out:30.t2, in:31.t2 - P8_43, GPIO xx
		PRU1_GPIO_xx_P8_43_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_43_OUT,
		PRU1_GPIO_xx_P8_43_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_43_IN,

		// out:30.t1, in:31.t1 - P8_46, GPIO xx
		PRU1_GPIO_xx_P8_46_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_46_OUT,
		PRU1_GPIO_xx_P8_46_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_46_IN,

		// out:30.t0, in:31.t0 - P8_45, GPIO xx
		PRU1_GPIO_xx_P8_45_OUT = PRU1_BIT | OUT_BIT | PRU1_FW_GPIO_xx_P8_45_OUT,
		PRU1_GPIO_xx_P8_45_IN = PRU1_BIT | IN_BIT | PRU1_FW_GPIO_xx_P8_45_IN,

		// PRU 0
		// out:- in:31.t15 - P8_15, GPIO xx
		PRU0_GPIO_xx_P8_15_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_xx_P8_15_IN,

		// out:- in:31.t14 - P8_16, GPIO xx
		PRU0_GPIO_xx_P8_16_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_xx_P8_16_IN,

		// out:30.t15 in:- - P8_11, GPIO xx
		PRU0_GPIO_xx_P8_11_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_xx_P8_11_IN,

		// out:30.t14 in:- - P8_12, GPIO xx
		PRU0_GPIO_xx_P8_12_OUT = PRU0_BIT | OUT_BIT | PRU0_FW_GPIO_xx_P8_12_OUT,

		// out:- in:31.t16 - P9_24, GPIO xx
		PRU0_GPIO_xx_P9_24_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_xx_P9_24_IN,

		// out:30.t7, in:31.t7 - P9_25, GPIO 117
		PRU0_GPIO_117_P9_25_OUT = PRU0_BIT | OUT_BIT | PRU0_FW_GPIO_117_P9_25_OUT,
		PRU0_GPIO_117_P9_25_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_117_P9_25_IN,

		// out:30.t6, in:31.t6 - P9_41, GPIO xx
		PRU0_GPIO_xx_P9_41_OUT = PRU0_BIT | OUT_BIT | PRU0_FW_GPIO_xx_P9_41_OUT,
		PRU0_GPIO_xx_P9_41_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_xx_P9_41_IN,

		// out:30.t5, in:31.t5 - P9_27, GPIO xx
		PRU0_GPIO_xx_P9_27_OUT = PRU0_BIT | OUT_BIT | PRU0_FW_GPIO_xx_P9_27_OUT,
		PRU0_GPIO_xx_P9_27_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_xx_P9_27_IN,

		// out:30.t4, in:31.t4 - P9_42, GPIO xx
		PRU0_GPIO_xx_P9_42_OUT = PRU0_BIT | OUT_BIT | PRU0_FW_GPIO_xx_P9_42_OUT,
		PRU0_GPIO_xx_P9_42_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_xx_P9_42_IN,

		// out:30.t2, in:31.t2 - P9_30, GPIO xx
		PRU0_GPIO_xx_P9_30_OUT = PRU0_BIT | OUT_BIT | PRU0_FW_GPIO_xx_P9_30_OUT,
		PRU0_GPIO_xx_P9_30_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_xx_P9_30_IN,

		// out:30.t1, in:31.t1 - P9_29, GPIO xx
		PRU0_GPIO_xx_P9_29_OUT = PRU0_BIT | OUT_BIT | PRU0_FW_GPIO_xx_P9_29_OUT,
		PRU0_GPIO_xx_P9_29_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_xx_P9_29_IN,

		// out:30.t3, in:31.t3 - P9_28, GPIO 113
		PRU0_GPIO_113_P9_28_OUT = PRU0_BIT | OUT_BIT | PRU0_FW_GPIO_113_P9_28_OUT,
		PRU0_GPIO_113_P9_28_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_113_P9_28_IN,

		// out:30.t0, in:31.t0 - P9_31, GPIO 110
		PRU0_GPIO_110_P9_31_OUT = PRU0_BIT | OUT_BIT | PRU0_FW_GPIO_110_P9_31_OUT,
		PRU0_GPIO_110_P9_31_IN = PRU0_BIT | IN_BIT | PRU0_FW_GPIO_110_P9_31_IN,
	};

	explicit IPCDeviceGpioProxy(IPCDeviceProxyService& proxy);
	explicit IPCDeviceGpioProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy);

	void set(const uint32_t pin) override;
	void clear(const uint32_t pin) override;
	void toggle(const uint32_t pin) override;
};

#endif /* PRUIPC_IPCDEVICEGPIOPROXY_H_ */
