/*
 * IPCDeviceProxyMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef PRUIPC_IPCDEVICEPROXYMESSAGE_H_
#define PRUIPC_IPCDEVICEPROXYMESSAGE_H_

// standard
#include <algorithm>
#include <cassert>
#include <new>
#include <memory>

// project
#include "exceptionMacros.h"
#include "IMessageReceiver.h"
#include "IPCMessageBase.h"
#include "IPCMessageTypes.h"
#include "IPCResponseBase.h"
#include "pru_ipc_devices.hp"
#include "pru_ipc_types.hp"
#include "tboxdefs.h"

using namespace pruipcservice;

class IPCDeviceProxyMessage final : public IPCMessageBase
{
public:

	class ProxyMessageCompleted final : public IPCResponseBase
	{
	public:
		ProxyMessageCompleted(const IPCDeviceEnum device,
				const uint32_t ioctl) : IPCResponseBase(IPCMessageTypes::Type::IpcDeviceProxyMessageCompleted),
				m_device(device), m_ioctl(ioctl)
		{
		}

		std::unique_ptr<ServiceMessageBase> clone() const override
		{
			return tbox::make_unique<ProxyMessageCompleted>(*this);
		}

		IPCDeviceEnum device() const
		{
			return m_device;
		}

		uint32_t ioctl() const
		{
			return m_ioctl;
		}

	private:
		IPCDeviceEnum m_device;
		uint32_t m_ioctl;
	};

	explicit IPCDeviceProxyMessage(IMessageReceiver& receiver, const uint8_t* const data, const uint8_t size) : IPCMessageBase(receiver, IPCMessageTypes::Type::IpcDeviceProxyMessage),
		m_data(nullptr), m_size(size)
	{
		TB_ASSERT(data);

		m_data = tbox::make_unique_array<uint8_t>(m_size);
		assert(m_data);

		std::copy(data, data + m_size, m_data.get());
	}

	explicit IPCDeviceProxyMessage(const IPCDeviceProxyMessage& idpm) : IPCMessageBase(idpm),
		m_data(nullptr), m_size(idpm.m_size)
	{
		m_data = std::unique_ptr<uint8_t[]>(new uint8_t[m_size]);
		TB_ASSERT(m_data);

		std::copy(idpm.m_data.get(), idpm.m_data.get() + m_size, m_data.get());
	}

	explicit IPCDeviceProxyMessage(IPCDeviceProxyMessage&& idpm) : IPCMessageBase(idpm),
		m_data(nullptr), m_size(0)
	{
		using std::swap;
		swap(m_data, idpm.m_data);
		swap(m_size, idpm.m_size);
	}

	IPCDeviceProxyMessage& operator=(IPCDeviceProxyMessage rhs)
	{
		swap(*this, rhs);

		return *this;
	}

	friend void swap(IPCDeviceProxyMessage& first, IPCDeviceProxyMessage& second)
	{
		using std::swap;
		swap(first.m_data, second.m_data);
		swap(first.m_size, second.m_size);
	}

	std::unique_ptr<ServiceMessageBase> clone() const override
	{
		return tbox::make_unique<IPCDeviceProxyMessage>(*this);
	}

	uint8_t size() const
	{
		return m_size;
	}

	const uint8_t* data() const
	{
		return m_data.get();
	}

	void completed() const
	{
		const auto devIoctl = reinterpret_cast<const struct IPCDeviceIoctl*>(data());

		postResponse(ProxyMessageCompleted(static_cast<IPCDeviceEnum>(devIoctl->device), devIoctl->ioctl));
	}

private:
	std::unique_ptr<uint8_t[]> m_data;
	uint8_t m_size;
};

#endif /* PRUIPC_IPCDEVICEPROXYMESSAGE_H_ */
