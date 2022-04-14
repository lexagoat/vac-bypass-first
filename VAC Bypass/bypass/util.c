#include "util.h"

bool running_as_service = false;

static bool compare_bytes(uint8_t* current, uint8_t* compare, const char* mask)
{
    for (; *mask; ++current, ++compare, ++mask) {
        if (*mask == 'x' && *current != *compare)
            return 0;
    }

    return (*mask == 0);
}

void* sigscan(const char* module, const char* bytes, const char* mask)
{
    int start_address = (int)GetModuleHandleA(module);
    IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*)start_address;
    IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)((uint8_t*)dos_header + dos_header->e_lfanew);
    int end_address = start_address + nt_headers->OptionalHeader.SizeOfCode;

    for (int current_address = start_address; current_address < end_address; current_address++) {
        uint8_t* current_bytes = (uint8_t*)current_address;
        if (compare_bytes(current_bytes, (uint8_t*)bytes, mask) == true)
            return (void*)current_address;
    }

    return 0;
}

void log_debug(const char* fmt, ...)
{
    char buffer[0xFF];
    va_list args;
    va_start(args, fmt);
    vsprintf_s(buffer, 0xFF, fmt, args);
    va_end(args);

    char logmessage[0xFF];
    char logtime[0xFF];
    time_t rawtime;
    struct tm timeinfo;
    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);

    strftime(logtime, sizeof(logtime), "[%H:%M:%S]", &timeinfo);
    sprintf_s(logmessage, 0xFF, "%s %s\n", logtime, buffer);
    if (!running_as_service) {
        printf(logmessage);
    }
    else {
        FILE* f = fopen("c:\\vac_log.txt", "a+");
        fwrite(logmessage, strlen(logmessage), 1, f);
        fclose(f);
    }
}