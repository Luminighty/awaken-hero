#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define LOG(...) printf("[" LOG_HEADER "] " __VA_ARGS__); fflush(stdout)

#endif // LOG_H
