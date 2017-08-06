/*
 *
 * Logger.h
 *
 */
 
#ifndef TBOX_LOGGER_HPP
#define TBOX_LOGGER_HPP

// standard
#include <vector>
#include <string>
#include <sstream>

// system
#include <stdarg.h>
#include <syslog.h>
#include <ostream>
#include <memory>

#define CONFIGURE_LOG(appl, level, destination) {\
		s_logger.configure(#appl, static_cast<Logger::LogLevelEnum>(level), static_cast<Logger::LogDestinationEnum>(destination)); \
}

#define MODULE_LOG(module) namespace {\
		Logger::LogWrapper m_logger(s_logger, #module);\
}

#define DEBUG(content) Logger::SStreamLogger(m_logger).emit(Logger::LogLevelDebug) << "DEBUG " << m_logger.getModuleName() << " " << __LINE__ << " - " << content;
#define INFO(content) Logger::SStreamLogger(m_logger).emit(Logger::LogLevelInfo) << "INFO " << m_logger.getModuleName() << " " << __LINE__ << " - " << content;
#define WARN(content) Logger::SStreamLogger(m_logger).emit(Logger::LogLevelWarn) << "WARN " << m_logger.getModuleName() << " " << __LINE__ << " - " << content;
#define ERROR(content) Logger::SStreamLogger(m_logger).emit(Logger::LogLevelError) << "ERROR " << m_logger.getModuleName() << " " << __LINE__ << " - " << content;

#define DEBUGF(fmt, args...) m_logger.log(Logger::LogLevelDebug, "DEBUG %s %d - " fmt, m_logger.getModuleName().c_str(), __LINE__, ##args)
#define INFOF(fmt, args...) m_logger.log(Logger::LogLevelInfo, "INFO %s %d - " fmt, m_logger.getModuleName().c_str(), __LINE__, ##args)
#define WARNOF(fmt, args...) m_logger.log(Logger::LogLevelWarn, "WARN %s %d - " fmt, m_logger.getModuleName().c_str(), __LINE__, ##args)
#define ERRORF(fmt, args...) m_logger.log(Logger::LogLevelError, "ERROR %s %d - " fmt, m_logger.getModuleName().c_str(), __LINE__, ##args)

class Logger;

extern Logger s_logger;

class Logger final
{
	public :
		enum LogDestinationEnum
		{
			SyslogDest = 0x01,
			StdoutDest = 0x02,
			FileDest = 0x04,
			AllDest = (SyslogDest | StdoutDest | FileDest)
		};

		enum LogLevelEnum
		{
			LogLevelDebug = 0,
			LogLevelInfo,
			LogLevelWarn,
			LogLevelError,
			Nof_LogLevels
		};

		class LogWrapper final
		{
		public:
			LogWrapper(Logger& logger, const std::string& moduleName);

			void log(LogLevelEnum level, const char *format, ...);

			void errorDump(const std::vector<std::string>& strings);

			const std::string& getModuleName() const
			{
				return m_moduleName;
			}

		private:
			Logger& m_logger;
			const std::string m_moduleName;
		};

		class SStreamLogger final
		{
		public:
			SStreamLogger(LogWrapper& logger);
			~SStreamLogger();

			std::ostringstream& emit(const LogLevelEnum level);

		private:
			LogWrapper& m_logger;
			LogLevelEnum m_loglevel;
			std::ostringstream m_ss;
		};

		Logger();
		~Logger();

		void configure(const std::string& applicationName, const LogLevelEnum level = LogLevelInfo,
				const LogDestinationEnum destination = StdoutDest,
				const std::string& logDir = "/var/log");

		void log(LogLevelEnum level, const char *format, va_list args);

		void errorDump(const std::vector<std::string>& strings);

	private :
		void open(const std::string& applicationName);
		void close();

		void log(LogLevelEnum level, const char *format, ...);

		void setApplicationName(const std::string& applicationName);
		void setLogDir(const std::string& logDir);
		void setLogDest(int mask);
		void setLoglevel(LogLevelEnum level);
		void validateLogDir(const std::string& logDir);

		static int s_syslogLevelConvert[Nof_LogLevels];
		static bool s_configured;

		bool m_syslogOpen;
		bool m_filelogOpen;

		LogLevelEnum m_loglevel;
		int m_logDest;
		std::string m_logDir;
		std::string m_applicationName;
		bool m_validLogDir;
		int m_filelogfd;
};

extern "C" void logFile(const int fd, Logger::LogLevelEnum level, const char *format, va_list args);
extern "C" void logSyslog(int level, const char *format, va_list args);
extern "C" void logStdout(Logger::LogLevelEnum level, const char *format, va_list args);

#endif
