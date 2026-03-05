#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

static log_level_types current_level = LOG_INFO;
const char* levels[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

void log_set_level(log_level_types level){
    current_level = level;
}

static void get_timestamp(char *buf, size_t size){
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", &tm_info);
}

void log_internal(log_level_types level, const char *file, size_t line, const char *fmt, ...){
    if(level < current_level) return;
    char timestamp[32];
    get_timestamp(timestamp, sizeof(timestamp));
    
    FILE *out = (level >= LOG_ERROR) ? stderr : stdout;

    fprintf(out, "[%s] [%s] %s:(%zu): ", timestamp, levels[level], file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);

    fprintf(out, "\n");
    if(level == LOG_FATAL) fflush(out);
}



