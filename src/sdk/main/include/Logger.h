// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_LOGGER_H_
#define HIERO_SDK_CPP_LOGGER_H_

#include "Defaults.h"

#include <log4cxx/logger.h>
#include <memory>
#include <string_view>

namespace Hiero
{
/**
 * Logger class used by the Hiero C++ SDK. In essence, it's a wrapper for a log4cxx logger instance and provides easier
 * usage.
 */
class Logger
{
public:
  /**
   * Enumeration of logging levels used by Loggers.
   */
  enum class LoggingLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    SILENT
  };

  Logger() = default;

  /**
   * Construct with a specific logging level.
   *
   * @param level The level at which this Logger should log.
   */
  explicit Logger(LoggingLevel level);

  /**
   * Log a trace-level message.
   *
   * @param message The trace-level message to log.
   */
  void trace(std::string_view message) const;

  /**
   * Log a debug-level message.
   *
   * @param message The debug-level message to log.
   */
  void debug(std::string_view message) const;

  /**
   * Log a info-level message.
   *
   * @param message The info-level message to log.
   */
  void info(std::string_view message) const;

  /**
   * Log a warn-level message.
   *
   * @param message The warn-level message to log.
   */
  void warn(std::string_view message) const;

  /**
   * Log a error-level message.
   *
   * @param message The error-level message to log.
   */
  void error(std::string_view message) const;

  /**
   * Set the log4cxx logger this Logger should use.
   *
   * @param logger The log4cxx logger to use.
   * @return A reference to this Logger with the newly-set log4cxx logger.
   */
  Logger& setLogger(const log4cxx::LoggerPtr& logger);

  /**
   * Set the log level for this Logger.
   *
   * @param level The level at which this Logger should log.
   * @return A reference to this Logger with the newly-set logging level.
   */
  Logger& setLevel(LoggingLevel level);

  /**
   * Silence this Logger from logging.
   *
   * @param silent \c TRUE if this Logger should be silent, otherwise \c FALSE.
   * @param A reference to this Logger with the newly-set silence policy.
   */
  Logger& setSilent(bool silent);

  /**
   * Get the log4cxx logger used by this Logger.
   *
   * @return The log4cxx logger used by this Logger.
   */
  [[nodiscard]] inline log4cxx::LoggerPtr getLogger() const { return mLogger; }

  /**
   * Get the LoggingLevel currently being used by this Logger.
   *
   * @return The LoggingLevel currently being used by this Logger.
   */
  [[nodiscard]] inline LoggingLevel getLevel() const { return mCurrentLevel; }

private:
  /**
   * The wrapped log4cxx logger.
   */
  log4cxx::LoggerPtr mLogger = log4cxx::Logger::getLogger(DEFAULT_LOGGER_NAME);

  /**
   * The current logger level.
   */
  LoggingLevel mCurrentLevel = LoggingLevel::TRACE;

  /**
   * The previous logger level.
   */
  LoggingLevel mPreviousLevel = LoggingLevel::TRACE;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_LOGGER_H_
