/*
 * FSMEventTypes_export.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef FSMEVENTTYPES_EXPORT_H_
#define FSMEVENTTYPES_EXPORT_H_

// project
#include "exceptionMacros.h"
#include "FSMEvent.h"
#include "MessageTypes_export.h"

namespace exports
{
class FSMEventTypes final
{
public:

	FSMEventTypes() = delete;

	enum Type
	{
		// Event types for FSM core
		EnterEvent = -2,
		ExitEvent = -1,

		EOEEvent = exports::MessageTypes::EOEMessage
	};

	static const char* toString(const FSMEvent::EventType_t type)
	{
		#define CASE(v) case v: return #v;
		switch (static_cast<Type>(type))
		{
			CASE(Type::EnterEvent)
			CASE(Type::ExitEvent)
			case EOEEvent:
				TB_ERROR("Don't use EOEEvent");
		}
		static std::string tmp = std::to_string(static_cast<int>(type));
		return tmp.c_str();
		#undef CASE
	}
};

} // namespace

#endif /* FSMEVENTTYPES_EXPORT_H_ */
