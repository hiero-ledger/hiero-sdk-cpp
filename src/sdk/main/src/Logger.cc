// SPDX-License-Identifier: Apache-2.0
#include "Logger.h"

namespace Hiero
{
//-----
Logger::Logger(Hiero::Logger::LoggingLevel level)
  : mCurrentLevel(level)
  , mPreviousLevel(level)
{
}

//-----
void Logger::trace(std::string_view message) const
{
  if (mCurrentLevel != LoggingLevel::SILENT)
  {
    LOG4CXX_TRACE(mLogger, message);
  }
}

//-----
void Logger::debug(std::string_view message) const
{
  if (mCurrentLevel != LoggingLevel::SILENT)
  {
    LOG4CXX_DEBUG(mLogger, message);
  }
}

//-----
void Logger::info(std::string_view message) const
{
  if (mCurrentLevel != LoggingLevel::SILENT)
  {
    LOG4CXX_INFO(mLogger, message);
  }
}

//-----
void Logger::warn(std::string_view message) const
{
  if (mCurrentLevel != LoggingLevel::SILENT)
  {
    LOG4CXX_WARN(mLogger, message);
  }
}

//-----
void Logger::error(std::string_view message) const
{
  if (mCurrentLevel != LoggingLevel::SILENT)
  {
    LOG4CXX_ERROR(mLogger, message);
  }
}

//-----
Logger& Logger::setLogger(const log4cxx::LoggerPtr& logger)
{
  mLogger = logger;
  return *this;
}

//-----
Logger& Logger::setLevel(Logger::LoggingLevel level)
{
  mPreviousLevel = mCurrentLevel;
  mCurrentLevel = level;
  switch (mCurrentLevel)
  {
    case LoggingLevel::TRACE:
      mLogger->setLevel(log4cxx::Level::getTrace());
      break;
    case LoggingLevel::DEBUG:
      mLogger->setLevel(log4cxx::Level::getDebug());
      break;
    case LoggingLevel::INFO:
      mLogger->setLevel(log4cxx::Level::getInfo());
      break;
    case LoggingLevel::WARN:
      mLogger->setLevel(log4cxx::Level::getWarn());
      break;
    case LoggingLevel::ERROR:
      mLogger->setLevel(log4cxx::Level::getError());
      break;
    case LoggingLevel::SILENT:
      mLogger->setLevel(log4cxx::Level::getOff());
      break;
  }
  return *this;
}

//-----
Logger& Logger::setSilent(bool silent)
{
  mCurrentLevel = silent ? LoggingLevel::SILENT : mPreviousLevel;
  return *this;
}

} // namespace Hiero
