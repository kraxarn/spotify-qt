#pragma once

#include "lib/developermode.hpp"
#include "lib/fmt.hpp"

#include <QtDebug>

namespace lib
{
	/**
	 * Application logging
	 */
	class log
	{
	public:
		/**
		 * Log information with formatting
		 * @deprecated Use qInfo instead
		 */
		template<typename Format, typename Arg, typename... Args>
		static void info(const Format &fmt, const Arg &arg, Args &&... args)
		{
			info(fmt::format(fmt, arg, args...));
		}

		/**
		 * Log information
		 * @deprecated Use qInfo instead
		 */
		template<typename Format>
		static void info(const Format &fmt)
		{
			message(QtInfoMsg, fmt);
		}

		/**
		 * Log warning with formatting
		 * @deprecated Use qWarning instead
		 */
		template<typename Format, typename Arg, typename... Args>
		static void warn(const Format &fmt, const Arg &arg, Args &&... args)
		{
			warn(fmt::format(fmt, arg, args...));
		}

		/**
		 * Log warning
		 * @deprecated Use qWarning instead
		 */
		template<typename Format>
		static void warn(const Format &fmt)
		{
			message(QtWarningMsg, fmt);
		}

		/**
		 * Log error with formatting
		 * @deprecated Use qCritical instead
		 */
		template<typename Format, typename Arg, typename... Args>
		static void error(const Format &fmt, const Arg &arg, Args &&... args)
		{
			error(fmt::format(fmt, arg, args...));
		}

		/**
		 * Log error
		 * @deprecated Use qCritical instead
		 */
		template<typename Format>
		static void error(const Format &fmt)
		{
			message(QtCriticalMsg, fmt);
		}

		/**
		 * Log verbose message with formatting
		 * @note developer_mode needs to be enabled
		 * @deprecated Use qDebug instead
		 */
		template<typename Format, typename Arg, typename... Args>
		static void debug(const Format &fmt, const Arg &arg, Args &&... args)
		{
			debug(fmt::format(fmt, arg, args...));
		}

		/**
		 * Log verbose message
		 * @note developer_mode needs to be enabled
		 * @deprecated Use qDebug instead
		 */
		template<typename Format>
		static void debug(const Format &fmt)
		{
			if (!developer_mode::enabled)
			{
				return;
			}

			message(QtDebugMsg, fmt);
		}

	private:
		/**
		 * Private constructor, this is a static class
		 */
		log() = default;

		/**
		 * Log a message with the specified type
		 * @param log_type Type of log
		 * @param message Message to log
		 */
		static void message(QtMsgType log_type, const std::string &message);
	};
}
