#include "common.h"

extern bool running_as_service;
void* sigscan(const char* module, const char* bytes, const char* mask);
void log_debug(const char* fmt, ...);