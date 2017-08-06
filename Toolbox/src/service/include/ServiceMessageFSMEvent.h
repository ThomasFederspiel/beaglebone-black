/*
 * ServiceMessageFSMEvent.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_SERVICEMESSAGEFSMEVENT_H_
#define TBOX_SERVICEMESSAGEFSMEVENT_H_

// project
#include "FSMEvent.h"
#include "ServiceMessageBase.h"

class ServiceMessageFSMEvent final : public FSMEvent
{
public:

	explicit ServiceMessageFSMEvent(const ServiceMessageBase& message) : FSMEvent(message.getType()), m_message(message)
	{
	}

 	template<typename T>
	const T& getCastedPayload() const
	{
		return m_message.getCasted<T>();
	}

private:
	const ServiceMessageBase& m_message;
};

#endif /* TBOX_SERVICEMESSAGEFSMEVENT_H_ */
