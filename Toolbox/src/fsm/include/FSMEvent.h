/*
 * FSMEvent.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_FSMEVENT_H_
#define TBOX_FSMEVENT_H_

// standard
#include <cstdint>

// project
#include "exceptionMacros.h"
#include "tboxdefs.h"

class FSMEvent
{
public:

	using EventType_t = int16_t;

	explicit FSMEvent(const EventType_t type) : m_type(type)
	{
	}

	virtual ~FSMEvent() = default;

	EventType_t getType() const
	{
		return m_type;
	}

 	template<typename T>
	const T& getCasted() const
	{
		const T* castedEv = nullptr;
		const bool isInstance = tbox::instanceof<const T>(*this, castedEv);
		TB_ASSERT(isInstance);

		return *castedEv;
	}

private:
	EventType_t m_type;
};

#endif /* TBOX_FSMEVENT_H_ */
