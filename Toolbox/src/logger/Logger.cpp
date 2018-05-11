/*
 *
 * Logger.cpp
 *
 */

#include "Logger.h"

// System
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>

// standard
#include <iostream>
#include <cassert>
#include <ctime>
#include <chrono>

// project
#include "exceptionMacros.h"
#include "Utils.h"


int Logger::s_syslogLevelConvert[] = { LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERR };
bool Logger::s_configured = false;

Logger s_logger;

Logger::SStreamLogger::SStreamLogger(Logger::LogWrapper& logger)
	: m_logger(logger), m_loglevel(Logger::LogLevelDebug)
{
}

Logger::SStreamLogger::~SStreamLogger()
{
	m_logger.log(m_loglevel, "%s", m_ss.str().c_str());
}

std::ostringstream& Logger::SStreamLogger::emit(const LogLevelEnum level)
{
	m_loglevel = level;

	return m_ss;
}

Logger::LogWrapper::LogWrapper(Logger& logger, const std::string& moduleName, bool enable)
	: m_logger(logger),  m_moduleName(moduleName), m_enabled(enable)
{
}

void Logger::LogWrapper::log(LogLevelEnum level, const char *format, ...)
{
	if (m_enabled)
	{
		va_list args;
		va_start(args, format);
		m_logger.log(level, format, args);
		va_end(args);
	}
}

void Logger::LogWrapper::errorDump(const std::vector<std::string>& strings)
{
	m_logger.errorDump(strings);
}

Logger::Logger() : m_syslogOpen(false), m_filelogOpen(false), m_loglevel(LogLevelDebug), m_logDest(StdoutDest),
		m_logDir(), m_applicationName(), m_validLogDir(false), m_filelogfd(-1)
{
}

Logger::~Logger()
{
	close();
}

void Logger::configure(const std::string& applicationName, const LogLevelEnum level,
		const LogDestinationEnum destination, const std::string& logDir)
{
	TB_ASSERT(!s_configured);

	setApplicationName(applicationName);
	setLogDest(destination);
	setLoglevel(level);
	setLogDir(logDir);

	open(applicationName);

	s_configured = true;
}

void Logger::validateLogDir(const std::string& logDir)
{
	m_validLogDir = false;

	if (!logDir.empty())
	{
		const bool recursive = true;
		if (Utils::mkdir(logDir, recursive))
		{
			m_validLogDir = true;
		}
	}
}

void Logger::open(const std::string& applicationName)
{
	if (!applicationName.empty())
	{
		if (!m_syslogOpen && (m_logDest & SyslogDest))
		{
			openlog(applicationName.c_str(), LOG_CONS | LOG_PID, LOG_LOCAL0);
			m_syslogOpen = true;
		}

		if (m_validLogDir && !m_filelogOpen && (m_logDest & FileDest))
		{
			m_filelogfd = ::open(Utils::concatPath(m_logDir, std::string(applicationName) + ".log", '/').c_str(), O_RDWR | O_CREAT | O_APPEND );

			m_filelogOpen = true;
		}
	}
}

void Logger::setApplicationName(const std::string& applicationName)
{
	m_applicationName = applicationName;
}

void Logger::setLoglevel(LogLevelEnum level)
{
	m_loglevel = level;
}

void Logger::setLogDest(int mask)
{
	TB_ASSERT((mask & ~AllDest) == 0);

	m_logDest = mask;
}

void Logger::setLogDir(const std::string& logDir)
{
	m_logDir = logDir;

	validateLogDir(logDir);
}

void Logger::log(LogLevelEnum level, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log(level, format, args);
	va_end(args);
}

void Logger::log(LogLevelEnum level, const char *format, va_list args)
{
	TB_ASSERT(s_configured);
	TB_ASSERT(format);

	if (level >= m_loglevel)
	{
		if (m_syslogOpen && (m_logDest & SyslogDest))
		{
			logSyslog(s_syslogLevelConvert[level], format, args);
		}

		if (m_filelogOpen && (m_logDest & FileDest))
		{
			logFile(m_filelogfd, level, format, args);
		}

		if (m_logDest & StdoutDest)
		{
			logStdout(level, format, args);
		}
	}
}

void Logger::close()
{
	if (m_syslogOpen)
	{
		m_syslogOpen = false;
		closelog();
	}

	if (m_filelogOpen)
	{
		(void)fsync(m_filelogfd);
		(void)::close(m_filelogfd);

		m_filelogfd = -1;
		m_filelogOpen = false;
	}
}

void Logger::errorDump(const std::vector<std::string>& strings)
{
	bool dumped = false;

	if (m_validLogDir)
	{
		const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		const std::time_t nowtime = std::chrono::system_clock::to_time_t(now);

		struct std::tm* nowtm = std::localtime(&nowtime);

		char datebuf[64];
		std::strftime(datebuf, sizeof(datebuf), "%b %e %T Error dump : \n", nowtm);

		std::string timeStamp(datebuf);

		// We want tp make sure that everything is flushed to disk
		// after dumping to error log. There for we need
		// to execute fsync on both the directory and the actual
		// file.

		const int dd = ::open(m_logDir.c_str(), O_RDONLY );

		if (dd != -1)
		{
			const int fd = ::open(Utils::concatPath(m_logDir, "errors", '/').c_str(), O_RDWR | O_CREAT | O_APPEND );

			if (fd != -1)
			{
				(void)write(fd, timeStamp.c_str(), timeStamp.length());

				for (std::size_t i = 0; i < strings.size(); i++)
				{
					(void)write(fd, strings[i].c_str(), strings[i].length());
					(void)write(fd, "\n", 1);
				}

				(void)fsync(fd);
				(void)fsync(dd);

				(void)::close(fd);

				dumped = true;
			}

			(void)::close(dd);
		}
	}

	if (!dumped)
	{
		for (size_t i = 0; i < strings.size(); i++)
		{
			log(LogLevelError, "%s", strings[i].c_str());
		}
	}
}

void logFile(const int fd, Logger::LogLevelEnum level, const char* format, va_list args)
{
	TB_ASSERT(format);

	va_list args_copy;
	va_copy(args_copy, args);

	char buffer[200];
	int bufferSize = ARRAY_SIZE(buffer);

	const int rc = vsnprintf(buffer, bufferSize, format, args);

	if (rc > bufferSize)
	{
		bufferSize = rc;
		char* buffer = new char[bufferSize];

		(void)vsnprintf(buffer, bufferSize, format, args_copy);

		(void)write(fd, buffer, bufferSize);
		(void)write(fd, "\n", 1);

		(void)fsync(fd);

		delete[] buffer;
	}
	else
	{
		(void)write(fd, buffer, rc);
		(void)write(fd, "\n", 1);

		(void)fsync(fd);
	}

	va_end(args_copy);
}

void logSyslog(int level, const char *format, va_list args)
{
	TB_ASSERT(format);

	vsyslog(level, format, args);
}

void logStdout(Logger::LogLevelEnum level, const char* format, va_list args)
{
	TB_ASSERT(format);

	va_list args_copy;
	va_copy(args_copy, args);

	char buffer[200];
	int bufferSize = ARRAY_SIZE(buffer);

	const int rc = vsnprintf(buffer, bufferSize, format, args);

	if (rc > bufferSize)
	{
		bufferSize = rc;
		char* buffer = new char[bufferSize];

		(void)vsnprintf(buffer, bufferSize, format, args_copy);

		std::cout << buffer << std::endl;

		delete[] buffer;
	}
	else
	{
		std::cout << buffer << std::endl;
	}

	va_end(args_copy);
}
