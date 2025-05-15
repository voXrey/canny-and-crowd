#ifndef LOGGING_H
#define LOGGING_H


void log_debug(const char* message, ...);
void log_error(const char* message, ...);
void log_info(const char* message, ...);
void log_warning(const char* message, ...);
void log_fatal(const char* message, ...);


#endif