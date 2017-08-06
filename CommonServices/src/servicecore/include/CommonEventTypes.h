/*
 * CommonEventTypes.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef COMMONEVENTTYPES_H_
#define COMMONEVENTTYPES_H_

// project
#include "exceptionMacros.h"
#include "FSMEvent.h"

// export
#include "FSMEventTypes_export.h"
#include "MessageTypes_export.h"

namespace commonservices
{

class CommonEventTypes final
{
public:

	CommonEventTypes() = delete;

	enum Type
	{
		SoundMessageEvent = exports::MessageTypes::SoundMessage,
		IpcDeviceProxyMessageCompletedEvent = exports::MessageTypes::IpcDeviceProxyMessageCompleted,
		EOEEvent = exports::FSMEventTypes::EOEEvent
	};

	static const char* toString(const FSMEvent::EventType_t type)
	{
		#define CASE(v) case v: return #v;
		switch (static_cast<Type>(type))
		{
			CASE(Type::SoundMessageEvent)
			CASE(Type::IpcDeviceProxyMessageCompletedEvent)
			case EOEEvent:
				TB_ERROR("Don't use EOEEvent");
				break;
			default:
				return exports::FSMEventTypes::toString(type);
				break;
		}
		static std::string tmp = std::to_string(static_cast<int>(type));
		return tmp.c_str();
		#undef CASE
	}
};

} // namespace

#endif /* COMMONEVENTTYPES_H_ */
