/*
 *
 * LoggerUtil.h
 *
 */
 
#ifndef TBOX_LOGGERUTIL_HPP
#define TBOX_LOGGERUTIL_HPP

// standard
#include <cstdint>

// local
#include "Logger.h"

#define LOG_MEMDUMP(mem, len) LoggerUtil::memDump(m_logger, mem, len, 8, true)
#define LOG_MEMDUMP_CUSTOM(mem, len, width, absolute) LoggerUtil::memDump(m_logger, mem, len, width, absolute)

class LoggerUtil final
{
public :
	static void memDump(Logger::LogWrapper& logger, const uint8_t* mem, std::size_t len, const uint8_t width, const bool absolute);
};

#endif
