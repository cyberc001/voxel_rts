#ifndef LOG_H
#define LOG_H

#define LOG_ERROR(msg, ...) fprintf(stderr, "Error: " msg "\n", ##__VA_ARGS__)
#define LOG_WARN(msg, ...) fprintf(stderr, "Warning: " msg "\n", ##__VA_ARGS__)
#define LOG_ERROR_LINE(msg, ...) fprintf(stderr, "Error on line %u:\n" msg "\n", *(line), ##__VA_ARGS__)
#define LOG_WARN_LINE(msg, ...) fprintf(stderr, "Warning on line %u:\n" msg "\n", *(line), ##__VA_ARGS__)

#endif
