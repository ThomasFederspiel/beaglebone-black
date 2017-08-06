/*
 * EventTypes.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

// standard

#ifndef TBOX_FSMEVENTTYPES_H_
#define TBOX_FSMEVENTTYPES_H_

#include "FSMEventTypes_export.h"

class FSMEventTypes final
{
public:

	FSMEventTypes() = delete;

	enum Type
	{
		EnterEvent = exports::FSMEventTypes::Type::EnterEvent,
		ExitEvent = exports::FSMEventTypes::Type::ExitEvent
	};
};

#endif /* TBOX_FSMEVENTTYPES_H_ */
