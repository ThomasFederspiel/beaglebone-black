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

void LoggerUtil::memDump(Logger::LogWrapper& logger, const uint8_t* mem, std::size_t len)
{
	std::ostringstream hexLine;
	std::ostringstream asciiLine;

	int i = 0;

	hexLine.flags(std::ios::right | std::ios::hex);
	hexLine.width(2);
	// Doesnä't work
// ;+	hexLine.fill('0');

	for (const uint8_t* p = mem; p < (mem + len); ++p)
	{
		i %= 8;

		hexLine << " " << static_cast<int>(*p);
		asciiLine << static_cast<char>(std::isprint(*p) ? *p : '.');

		if ((i % 8) == 7)
		{
			asciiLine << "'";

			Logger::SStreamLogger(logger).emit(Logger::LogLevelInfo) << "MEMORY " << logger.getModuleName() << " " << __LINE__ << " - "
					<< hexLine.str() << " " << asciiLine.str();

			hexLine.str(std::string());
			asciiLine.str(std::string("'"));
		}

		++i;
	}

	if (hexLine.tellp() != 0)
	{
		asciiLine << "'";

		Logger::SStreamLogger(logger).emit(Logger::LogLevelInfo) << "MEMORY " << logger.getModuleName() << " " << __LINE__ << " - "
				<< hexLine.str() << " " << asciiLine.str();
	}
}
