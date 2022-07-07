#ifndef SRC_LOGGER_H
#define SRC_LOGGER_H

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

// clang specific macro
#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define LOGINFO(fmt, ...) \
    { spdlog::info(fmt, __VA_ARGS__); }

#define LOGDEBUG(fmt, ...) \
    { spdlog::debug(fmt " ({}:{} L#{})", __VA_ARGS__, __FILE_NAME__, __FUNCTION__, __LINE__); }

#define LOGWARN(fmt, ...) \
    { spdlog::warn(fmt, __VA_ARGS__); }

#define LOGERROR(fmt, ...) \
    { spdlog::error(fmt " ({}:{} L#{})", __VA_ARGS__, __FILE_NAME__, __FUNCTION__, __LINE__); }

#define LOGCRITICAL(fmt, ...) \
    { spdlog::critical(fmt " ({}:{} L#{})", __VA_ARGS__, __FILE_NAME__, __FUNCTION__, __LINE__); }

#define LOGTRACE(fmt, ...) \
    { spdlog::trace(fmt " ({}:{} L#{})", __VA_ARGS__, __FILE_NAME__, __FUNCTION__, __LINE__); }

#endif  // SRC_LOGGER_H
