/*
 * BinaryComSerializer.h
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#ifndef TBOX_BINARYCOMSERIALIZER_H_
#define TBOX_BINARYCOMSERIALIZER_H_

// standard
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

// boost

// project

// local

class BinaryComSerializer final
{
public:

	BinaryComSerializer(const uint8_t* const data, const std::size_t length);
	BinaryComSerializer(uint8_t* const data, const std::size_t length);

	BinaryComSerializer& operator<<(const std::string& str);
	BinaryComSerializer& operator>>(std::string& str);

	template <typename T,
		typename std::enable_if<std::is_same<bool, T>::value, T>::type,
		typename std::enable_if<sizeof(T) != 1, T>::type>
	BinaryComSerializer& operator<<(const bool val)
	{
		return operator<<(static_cast<uint8_t>(val));
	}

	template <typename T,
		typename std::enable_if<std::is_same<bool, T>::value, T>::type,
		typename std::enable_if<sizeof(T) != 1, T>::type>
	BinaryComSerializer& operator>>(bool& val)
	{
		return operator>>(static_cast<uint8_t&>(val));
	}

	template <typename T>
	BinaryComSerializer& operator<<(const T& val)
	{
		assert(m_wrData);
		assert((m_wrData + sizeof(T)) <= m_dataEnd);

		std::memcpy(m_wrData, reinterpret_cast<const uint8_t*>(&val), sizeof(T));

		m_wrData += sizeof(T);

		return *this;
	}

	template <typename T>
	BinaryComSerializer& operator>>(T& val)
	{
		assert(m_rdData);
		assert(m_rdData + sizeof(T) <= m_dataEnd);

		std::memcpy(reinterpret_cast<uint8_t*>(&val), m_rdData, sizeof(T));

		m_rdData += sizeof(T);

		return *this;
	}

	bool read() const
	{
		return m_rdData != nullptr;
	}

	bool write() const
	{
		return !read();
	}

	std::size_t capacity() const
	{
		return m_dataEnd - m_dataBegin;
	}

	std::size_t size() const
	{
		if (read())
		{
			return m_rdData - m_dataBegin;
		}

		return m_wrData - m_dataBegin;
	}

private:

	const uint8_t* m_rdData;
	uint8_t* m_wrData;

	const uint8_t* const m_dataBegin;
	const uint8_t* m_dataEnd;
};

#endif /* TBOX_BINARYCOMSERIALIZER_H_ */
