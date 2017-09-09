/*
 * StreamSelectMessage.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef STREAMSELECTMESSAGE_H_
#define STREAMSELECTMESSAGE_H_

// standard
#include <cstdint>
#include <memory>

// local
#include "BinaryComSerializer.h"
#include "IComStreamMessage.h"
#include "tboxdefs.h"

class StreamSelectMessage final : public IComStreamMessage
{
public:

	static constexpr MessageIds ID = StreamSelectId;

	enum StreamId : uint8_t
	{
		LeftOdometerStream = 1,
		RightOdometerStream
	};

	explicit StreamSelectMessage(BinaryComSerializer& serializer)
	{
		serialize(serializer);
	}

	StreamId getStreamId() const
	{
		return m_streamId;
	}

	static const char* toString(const StreamId id)
	{
		#define CASE(v) case v: return #v;
		switch (id)
		{
			CASE(LeftOdometerStream)
			CASE(RightOdometerStream)
		}
		static std::string tmp = std::to_string(static_cast<int>(id));
		return tmp.c_str();
		#undef CASE
	}

private:

	void serialize(BinaryComSerializer& serializer)
	{
		if (serializer.read())
		{
			uint8_t id = 0;
			serializer >> id;
			m_streamId = static_cast<StreamId>(id);
		}

		if (serializer.write())
		{
			serializer << m_streamId;
		}
	}

	StreamId m_streamId;
};

#endif /* STREAMSELECTMESSAGE_H_ */
