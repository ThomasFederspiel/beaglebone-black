/*
 * AsyncTaskMessage.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_ASYNCTASKMESSAGE_H_
#define TBOX_ASYNCTASKMESSAGE_H_

// standard
#include <functional>

// project
#include "SystemMessage.h"
#include "SystemMessageTypes.h"

class AsyncTaskMessage final : public SystemMessage
{
public:

	template <typename Function, typename... Args >
	explicit AsyncTaskMessage(Function&& f, Args&&... args);

	std::unique_ptr<ServiceMessageBase> clone() const override;

	void operator()();

private:

	std::function<void()> m_task;
};

template <typename Function, typename... Args >
AsyncTaskMessage::AsyncTaskMessage(Function&& f, Args&&... args) : SystemMessage(SystemMessageTypes::Type::AsyncTaskMessage),
	m_task(std::bind(std::forward<Function>(f), std::forward<Args>(args)...))
{
}

#endif /* TBOX_ASYNCTASKMESSAGE_H_ */
