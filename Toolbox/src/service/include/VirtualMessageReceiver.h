/*
 * VirtualMessageReceiver.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_VIRTUALMESSAGERECEIVER_H_
#define TBOX_VIRTUALMESSAGERECEIVER_H_

// standard
#include <memory>
#include <mutex>
#include <condition_variable>

// local
#include "IMessageReceiver.h"
#include "ServiceMessageBase.h"

// project
#include "tboxdefs.h"

class VirtualMessageReceiver final : public IMessageReceiver
{
public:

	VirtualMessageReceiver() : m_lock(), m_signal(), m_hasResponse(false), m_response()
	{
	}

	void post(const ServiceMessageBase& message) override
	{
		std::unique_lock<std::mutex> lock(m_lock);

		TB_ASSERT(!m_response);

		m_response = message.clone();
		m_hasResponse = true;

		m_signal.notify_one();
	}

	std::unique_ptr<ServiceMessageBase> wait()
	{
		std::unique_lock<std::mutex> lock(m_lock);

		m_signal.wait(lock, [&](){ return m_hasResponse; });
		TB_ASSERT(m_response);

		auto&& tmp = m_response;

		m_response.reset();
		m_hasResponse = false;

		return std::move(tmp);
	}

private:

	mutable std::mutex m_lock;
	std::condition_variable m_signal;
	bool m_hasResponse;
	std::unique_ptr<ServiceMessageBase> m_response;
};

#endif /* TBOX_VIRTUALMESSAGERECEIVER_H_ */
