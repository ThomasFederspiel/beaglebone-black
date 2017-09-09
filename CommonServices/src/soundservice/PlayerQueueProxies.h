/*
 * PlayerQueueProxies.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef PLAYERQUEUEPROXIES_H_
#define PLAYERQUEUEPROXIES_H_

// standard
#include <exception>
#include <condition_variable>
#include <functional>
#include <mutex>

// local

// project
#include "IIPCDeviceEPwmProxy.h"
#include "IIPCDevicePwmsProxy.h"
#include "IPCDeviceEPwmProxy.h"
#include "IPCDevicePwmsProxy.h"
#include "pru_epwm_types.hp"
#include "pru_pwms_types.hp"

class IMessageReceiver;


class PlayerQueueTermException final : std::exception
{
public:

	const char* what() const noexcept override
	{
		return "Terminated player queue";
	}
};

class PlayerQueueEPwmProxy final : public IIPCDeviceEPwmProxy
{
public:
	friend class PlayerQueueEPwmChannel;

	class PlayerQueueEPwmChannel final : public IIPCDeviceEPwmProxy::IEPwmChannel
	{
	public:

		explicit PlayerQueueEPwmChannel(IIPCDeviceEPwmProxy::IEPwmChannel& channel, PlayerQueueEPwmProxy& proxy) : m_channel(channel), m_proxy(proxy)
		{
		}

		void enable() override
		{
			std::unique_lock<std::mutex> lock(m_proxy.m_lock);

			m_proxy.wait(lock, [this] { m_channel.enable(); });
		}

		void disable(const bool pinHigh) override
		{
			std::unique_lock<std::mutex> lock(m_proxy.m_lock);

			m_proxy.wait(lock, [this, pinHigh] { m_channel.disable(pinHigh); });
		}

		void setDuty(const uint16_t onTime) override
		{
			std::unique_lock<std::mutex> lock(m_proxy.m_lock);

			m_proxy.wait(lock, [this, onTime] { m_channel.setDuty(onTime); });
		}

		void pinLow() override
		{
			std::unique_lock<std::mutex> lock(m_proxy.m_lock);

			m_proxy.wait(lock, [this] { m_channel.pinLow(); });
		}

		void pinHigh() override
		{
			std::unique_lock<std::mutex> lock(m_proxy.m_lock);

			m_proxy.wait(lock, [this] { m_channel.pinHigh(); });
		}

	private:
		IIPCDeviceEPwmProxy::IEPwmChannel& m_channel;
		PlayerQueueEPwmProxy& m_proxy;
	};

	explicit PlayerQueueEPwmProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy,
			const PwmssDeviceEnum pwmDevice, const EPwmChannelEnum channel) : m_proxy(receiver, proxy, pwmDevice, channel),
			m_channelA(m_proxy.getChannel(PWM_CH_A), *this), m_channelB(m_proxy.getChannel(PWM_CH_B), *this), m_defaultChannel(nullptr),
			m_completed(false), m_terminated(false), m_lock(), m_signal()
	{
		setDefaultChannel(channel);
	}

	void setDefaultChannel(const EPwmChannelEnum channel) override
	{
		m_defaultChannel = &getChannel(channel);
	}

	IIPCDeviceEPwmProxy::IEPwmChannel& getDefaultChannel() override
	{
		return *m_defaultChannel;
	}

	IIPCDeviceEPwmProxy::IEPwmChannel& getChannel(const EPwmChannelEnum channel) override
	{
		if (channel == PWM_CH_A)
		{
			return m_channelA;
		}

		return m_channelB;
	}

	void open() override
	{
		std::unique_lock<std::mutex> lock(m_lock);

		wait(lock, [this] { m_proxy.open(); });
	}

	void close() override
	{
		std::unique_lock<std::mutex> lock(m_lock);

		wait(lock, [this] { m_proxy.close(); });
	}

	uint16_t armFreq(const uint32_t freq)
	{
		std::unique_lock<std::mutex> lock(m_lock);

		return wait<uint16_t>(lock, [this, freq] { return m_proxy.armFreq(freq); });
	}

	void armFreq(const uint16_t periodCntr, const uint8_t clkDiv, const uint8_t hspClkDiv) override
	{
		std::unique_lock<std::mutex> lock(m_lock);

		wait(lock, [this, periodCntr, clkDiv, hspClkDiv] { m_proxy.armFreq(periodCntr, clkDiv, hspClkDiv); });
	}

	uint16_t setFreq(const uint32_t freq) override
	{
		std::unique_lock<std::mutex> lock(m_lock);

		return wait<uint16_t>(lock, [this, freq] { return m_proxy.setFreq(freq); });
	}

	void completion()
	{
		std::unique_lock<std::mutex> lock(m_lock);

		m_completed = true;

		m_signal.notify_one();
	}

	void terminate()
	{
		std::unique_lock<std::mutex> lock(m_lock);

		m_terminated = true;

		m_signal.notify_one();
	}

	IPCDeviceEPwmProxy& getEPwmProxy()
	{
		return m_proxy;
	}

private:

	template<typename T = void>
	T wait(std::unique_lock<std::mutex>& lock, std::function<T()> action)
	{
		m_completed = false;

		T res = T();

		if (!m_terminated)
		{
			res = action();

			m_signal.wait(lock, [&](){ return m_completed || m_terminated; });
		}

		const bool terminate = m_terminated;

		m_completed = false;
		m_terminated = false;

		if (terminate)
		{
			throw PlayerQueueTermException();
		}

		return res;
	}

	void wait(std::unique_lock<std::mutex>& lock, std::function<void()> action)
	{
		m_completed = false;

		if (!m_terminated)
		{
			action();

			m_signal.wait(lock, [&](){ return m_completed || m_terminated; });
		}

		const bool terminate = m_terminated;

		m_completed = false;
		m_terminated = false;

		if (terminate)
		{
			throw PlayerQueueTermException();
		}
	}

	IPCDeviceEPwmProxy m_proxy;

	PlayerQueueEPwmChannel m_channelA;
	PlayerQueueEPwmChannel m_channelB;
	IEPwmChannel* m_defaultChannel;

	bool m_completed;
	bool m_terminated;
	mutable std::mutex m_lock;
	mutable std::condition_variable m_signal;
};

class PlayerQueuePwmsProxy final : public IIPCDevicePwmsProxy
{
public:

	explicit PlayerQueuePwmsProxy(IMessageReceiver& receiver, IPCDeviceProxyService& proxy,
			const PwmssDeviceEnum pwmDevice) : m_proxy(receiver, proxy, pwmDevice),
			m_completed(false), m_terminated(false), m_lock(), m_signal()
	{
	}

	void open() override
	{
		std::unique_lock<std::mutex> lock(m_lock);

		wait(lock, [this] { m_proxy.open(); });
	}

	void close() override
	{
		std::unique_lock<std::mutex> lock(m_lock);

		wait(lock, [this] { m_proxy.close(); });
	}

	void completion()
	{
		std::unique_lock<std::mutex> lock(m_lock);

		m_completed = true;

		m_signal.notify_one();
	}

	void terminate()
	{
		std::unique_lock<std::mutex> lock(m_lock);

		m_terminated = true;

		m_signal.notify_one();
	}

	IPCDevicePwmsProxy& getPwmsProxy()
	{
		return m_proxy;
	}

private:

	void wait(std::unique_lock<std::mutex>& lock, std::function<void()> action)
	{
		m_completed = false;

		if (!m_terminated)
		{
			action();

			m_signal.wait(lock, [&](){ return m_completed || m_terminated; });
		}

		const bool terminate = m_terminated;

		m_completed = false;
		m_terminated = false;

		if (terminate)
		{
			throw PlayerQueueTermException();
		}
	}

	IPCDevicePwmsProxy m_proxy;
	bool m_completed;
	bool m_terminated;
	mutable std::mutex m_lock;
	mutable std::condition_variable m_signal;
};

#endif /* PLAYERQUEUEPROXIES_H_ */
