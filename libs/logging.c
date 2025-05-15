#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "logging.h"
#include "config.h"

#define LOG_BUFFER_SIZE 4096 // On suppose que 4096 est suffisant pour la plupart des messages

// Affiche un message de debug (niveau 2)
void log_debug(const char* message, ...) {
    if (DEBUG_MODE >= 2) {
        char buffer[LOG_BUFFER_SIZE];
        va_list args;
        va_start(args, message);
        vsnprintf(buffer, LOG_BUFFER_SIZE, message, args);
        va_end(args);
        fprintf(stderr, "[DEBUG] %s\n", buffer);
    }
}

// Affiche un message d'erreur (niveau 1)
void log_error(const char* message, ...) {
    if (DEBUG_MODE >= 1) {
        char buffer[LOG_BUFFER_SIZE];
        va_list args;
        va_start(args, message);
        vsnprintf(buffer, LOG_BUFFER_SIZE, message, args);
        va_end(args);
        fprintf(stderr, "[ERROR] %s\n", buffer);
    }
}

// Affiche un message d'information (niveau 0)
void log_info(const char* message, ...) {
    if (DEBUG_MODE >= 0) {
        char buffer[LOG_BUFFER_SIZE];
        va_list args;
        va_start(args, message);
        vsnprintf(buffer, LOG_BUFFER_SIZE, message, args);
        va_end(args);
        fprintf(stderr, "[INFO] %s\n", buffer);
    }
}

// Affiche un message d'avertissement (niveau 0)
void log_warning(const char* message, ...) {
    if (DEBUG_MODE >= 0) {
        char buffer[LOG_BUFFER_SIZE];
        va_list args;
        va_start(args, message);
        vsnprintf(buffer, LOG_BUFFER_SIZE, message, args);
        va_end(args);
        fprintf(stderr, "[WARNING] %s\n", buffer);
    }
}

// Affiche un message fatal et termine le programme
void log_fatal(const char* message, ...) {
    char buffer[LOG_BUFFER_SIZE];
    va_list args;
    va_start(args, message);
    vsnprintf(buffer, LOG_BUFFER_SIZE, message, args);
    va_end(args);
    fprintf(stderr, "[FATAL] %s\n", buffer);
    exit(EXIT_FAILURE);
}