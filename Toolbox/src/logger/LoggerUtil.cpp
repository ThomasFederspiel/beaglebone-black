/*
 *
 * LoggerUtil.cpp
 *
 */

#include "LoggerUtil.h"

// standard
#include <cctype>
#include <iomanip>
#include <sstream>

void LoggerUtil::memDump(Logger::LogWrapper& logger, const uint8_t* mem, std::size_t len, const uint8_t width,
		const bool absolute)
{
	std::ostringstream hexLine;
	std::ostringstream asciiLine;

	int i = 0;
	const uint8_t* p = nullptr;

	for (p = mem; p < (mem + len); ++p)
	{
		i %= width;

		hexLine.flags(std::ios::left | std::ios::hex);
		hexLine.width(2);
		hexLine.fill('0');

		hexLine << " " << static_cast<int>(*p);
		asciiLine << static_cast<char>(std::isprint(*p) ? *p : '.');

		if ((i % width) == (width - 1))
		{
			asciiLine << "'";

			const void* startAddr = reinterpret_cast<const void*>(absolute ? p - i : p - reinterpret_cast<intptr_t>(mem) - i);
			const void* endAddr = reinterpret_cast<const void*>(absolute ? p : p - reinterpret_cast<intptr_t>(mem));

			Logger::SStreamLogger(logger).emit(Logger::LogLevelInfo) << "MEMORY " << logger.getModuleName() << " " << __LINE__ << " - "
					<< startAddr << ":" << endAddr
					<< " - " << hexLine.str() << " " << asciiLine.str();

			hexLine.str(std::string());
			asciiLine.str(std::string("'"));
		}

		++i;
	}

	if (hexLine.tellp() != 0)
	{
		asciiLine << "'";

		const void* startAddr = reinterpret_cast<const void*>(absolute ? p - i : p - reinterpret_cast<intptr_t>(mem) - i);
		const void* endAddr = reinterpret_cast<const void*>(absolute ? p : p - reinterpret_cast<intptr_t>(mem));

		Logger::SStreamLogger(logger).emit(Logger::LogLevelInfo) << "MEMORY " << logger.getModuleName() << " " << __LINE__ << " - "
				<< startAddr << ":" << endAddr
				<< " - " << hexLine.str() << " " << asciiLine.str();
	}
}
