#include <stdio.h>

#include "logging.h"
#include "config.h"

void log_debug(const char* message) {
    if (DEBUG_MODE >= 2) {
        printf("[DEBUG] %s\n", message);
    }
}

void log_error(const char* message) {
    if (DEBUG_MODE >= 1) {
        printf("[ERROR] %s\n", message);
    }
}

void log_info(const char* message) {
    if (DEBUG_MODE >= 0) {
        printf("[INFO] %s\n", message);
    }
}

void log_warning(const char* message) {
    if (DEBUG_MODE >= 0) {
        printf("[WARNING] %s\n", message);
    }
}

void log_fatal(const char* message) {
    printf("[FATAL] %s\n", message);
    exit(EXIT_FAILURE);
}

