#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>
typedef enum {
    LOG_TRACE = 0,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level_types;

void log_set_level(log_level_types level);

void log_internal(log_level_types level, const char *file, size_t line, const char* fmt, ...);

#define LOG_TRACE(fmt, ...) \
    log_internal(LOG_TRACE, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_DEBUG(fmt, ...) \
    log_internal(LOG_DEBUG, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    log_internal(LOG_INFO, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    log_internal(LOG_WARN, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_ERROR(fmt, ...) \
    log_internal(LOG_ERROR, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_FATAL(fmt, ...) \
    log_internal(LOG_FATAL, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#endif
