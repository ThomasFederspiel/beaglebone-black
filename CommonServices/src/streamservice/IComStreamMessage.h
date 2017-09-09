/*
 * IComStreamMessage.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef ICOMSTREAMMESSAGE_H_
#define ICOMSTREAMMESSAGE_H_

// standard
#include <cstdint>

// project

class IComStreamMessage
{
public:

	enum MessageIds : uint8_t
	{
		StreamSelectId = 1
	};

	virtual ~IComStreamMessage()
	{
	}

	static const char* toString(const MessageIds id)
	{
		#define CASE(v) case v: return #v;
		switch (id)
		{
			CASE(StreamSelectId)
		}
		static std::string tmp = std::to_string(static_cast<int>(id));
		return tmp.c_str();
		#undef CASE
	}

};

#endif /* ICOMSTREAMMESSAGE_H_ */
