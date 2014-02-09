
#pragma once

#ifndef __LOGGER_H

enum LoggerLevel
{
    NORMAL, VERBOSE, ERR
};

void logger_set_verbose(bool);
int avsc_log(LoggerLevel, const char*, ...);
int avsc_fatal(const char*, ...);
bool avsc_set_file(const char*);
void avsc_close_file();

#endif

