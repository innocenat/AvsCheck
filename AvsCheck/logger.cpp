
#define _CRT_SECURE_NO_WARNINGS

#include "logger.h"
#include <cstdio>
#include <cstdarg>
#include <cstdlib>

using namespace std;

bool   logger_verbose = false;
FILE*  logger_file = nullptr;

void logger_set_verbose(bool v) {
    logger_verbose = v;
}

int avsc_log(LoggerLevel level, const char* msg, ...) {
    if (level == VERBOSE && !logger_verbose)
        return -1;

    va_list v;
    va_start(v, msg);

    if (logger_file) {
        va_list v2;
        va_start(v2, msg);
        vfprintf(logger_file, msg, v2);
        va_end(v2);
    }

    FILE* out = level == ERR ? stderr : stdout;
    int ret = vfprintf(out, msg, v);

    va_end(v);
    return ret;

}

int avsc_fatal(const char* msg, ...) {
    va_list v;
    va_start(v, msg);

    int ret = vfprintf(stderr, msg, v);

    va_end(v);

    exit(-1);

    return ret;
}

bool avsc_set_file(const char* file) {
    if (logger_file)
        fclose(logger_file);

    logger_file = fopen(file, "w");

    return !!logger_file;
}

