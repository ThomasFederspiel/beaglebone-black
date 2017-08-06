/*
 * SystemMessageTypes.h
 *
 *  Created on: 25 mar 2016
 *      Author: Thomas
 */

#ifndef TBOX_SYSTEMMESSAGETYPES_H_
#define TBOX_SYSTEMMESSAGETYPES_H_

// standard
#include <cstring>
#include <type_traits>

// project
#include "ServiceTypes.h"

class SystemMessageTypes final
{
public:

	SystemMessageTypes() = delete;

	enum Type
	{
		StartMessage = -10,
		StopMessage,
		ShutdownMessage,
		AsyncTaskMessage
	};

	static const char* toString(const MessageType_t type)
	{
		#define CASE(v) case v: return #v;
		switch (static_cast<Type>(type))
		{
			CASE(Type::StartMessage)
			CASE(Type::StopMessage)
			CASE(Type::ShutdownMessage)
			CASE(Type::AsyncTaskMessage)
		}
		static std::string tmp = std::to_string(static_cast<int>(type));
		return tmp.c_str();
		#undef CASE
	}
};

#endif /* TBOX_SYSTEMMESSAGETYPES_H_ */
