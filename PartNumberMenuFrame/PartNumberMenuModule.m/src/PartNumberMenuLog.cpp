#include "PartNumberMenuLog.h"

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

static void BuildLogPath(char* oPath, size_t iSize)
{
    char base[MAX_PATH] = { 0 };
    DWORD length = GetEnvironmentVariableA("LOCALAPPDATA", base, MAX_PATH);
    if (length == 0 || length >= MAX_PATH)
    {
        length = GetTempPathA(MAX_PATH, base);
        if (length == 0 || length >= MAX_PATH)
            lstrcpynA(base, ".", MAX_PATH);
    }
    else
    {
        char directory[MAX_PATH] = { 0 };
        _snprintf_s(directory, sizeof(directory), _TRUNCATE,
                    "%s\\DassaultSystemes", base);
        CreateDirectoryA(directory, NULL);
        _snprintf_s(directory, sizeof(directory), _TRUNCATE,
                    "%s\\DassaultSystemes\\CATTemp", base);
        CreateDirectoryA(directory, NULL);
    }

    _snprintf_s(oPath, iSize, _TRUNCATE,
                "%s%sPartNumberMenuPlugin.log",
                base,
                (base[lstrlenA(base) - 1] == '\\') ? "" : "\\DassaultSystemes\\CATTemp\\");
}

void PartNumberMenuLog(const char* iStage, const char* iFormat, ...)
{
    char message[1024] = { 0 };
    va_list arguments;
    va_start(arguments, iFormat);
    _vsnprintf_s(message, sizeof(message), _TRUNCATE, iFormat, arguments);
    va_end(arguments);

    SYSTEMTIME now;
    GetLocalTime(&now);

    char line[1400] = { 0 };
    _snprintf_s(line, sizeof(line), _TRUNCATE,
                "%04u-%02u-%02u %02u:%02u:%02u.%03u pid=%lu tid=%lu [%s] %s\r\n",
                now.wYear, now.wMonth, now.wDay,
                now.wHour, now.wMinute, now.wSecond, now.wMilliseconds,
                GetCurrentProcessId(), GetCurrentThreadId(), iStage, message);

    OutputDebugStringA(line);

    char path[MAX_PATH] = { 0 };
    BuildLogPath(path, sizeof(path));
    HANDLE file = CreateFileA(path, FILE_APPEND_DATA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE)
    {
        DWORD written = 0;
        WriteFile(file, line, (DWORD)lstrlenA(line), &written, NULL);
        CloseHandle(file);
    }
}
