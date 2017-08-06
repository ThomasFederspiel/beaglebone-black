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

#define LOG_MEMDUMP(mem, len) LoggerUtil::memDump(m_logger, mem, len)

class LoggerUtil final
{
public :
	static void memDump(Logger::LogWrapper& logger, const uint8_t* mem, std::size_t len);
};

#endif
