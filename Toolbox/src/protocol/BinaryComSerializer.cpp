/*
 * BinaryComSerializer.cpp
 *
 *  Created on: 3 apr 2016
 *      Author: Thomas
 */

#include "BinaryComSerializer.h"

// standard

// project
#include "exceptionMacros.h"
#include "Logger.h"


#include "LoggerUtil.h"

MODULE_LOG(BinaryComSerializer)

BinaryComSerializer::BinaryComSerializer(const uint8_t* const data, const std::size_t length) : m_rdData(data),
	m_wrData(nullptr), m_dataBegin(data), m_dataEnd(data + length)
{
	TB_ASSERT(m_rdData);
	TB_ASSERT(!m_wrData);
}

BinaryComSerializer::BinaryComSerializer(uint8_t* const data, const std::size_t capacity) : m_rdData(nullptr),
	m_wrData(data), m_dataBegin(data),  m_dataEnd(data + capacity)
{
	TB_ASSERT(!m_rdData);
	TB_ASSERT(m_wrData);
}

BinaryComSerializer& BinaryComSerializer::operator<<(const std::string& str)
{
	TB_ASSERT(m_wrData);
	TB_ASSERT((m_wrData + str.size() + 1) <= m_dataEnd);

	std::memcpy(m_wrData, str.c_str(), str.size());

	m_wrData += str.size();

	// null terminated string
	m_wrData = 0x00;

	++m_wrData;

	return *this;
}

BinaryComSerializer& BinaryComSerializer::operator>>(std::string& str)
{
	TB_ASSERT(m_rdData);

	const std::size_t len = std::strlen(reinterpret_cast<const char*>(m_rdData));

	TB_ASSERT(len < static_cast<std::size_t>(m_dataEnd - m_rdData));

	str.assign(reinterpret_cast<const char*>(m_rdData), len);

	m_rdData += str.size();

	return *this;
}

