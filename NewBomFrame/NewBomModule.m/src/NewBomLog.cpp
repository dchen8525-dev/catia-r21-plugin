// =============================================================================
// NewBomLog.cpp
// Logging Utility Implementation - For Debugging and Tracking
// =============================================================================
//
// Implementation Principles:
//   1. Determine log file path
//   2. Format log message
//   3. Write to log file
//   4. Output to debugger
//
// Windows API Usage:
//   - GetEnvironmentVariableA: Get environment variable
//   - CreateDirectoryA: Create directory
//   - CreateFileA: Open/create file
//   - WriteFile: Write to file
//   - OutputDebugStringA: Output to debugger
//   - GetLocalTime: Get current time
//   - GetCurrentProcessId/GetCurrentThreadId: Get process/thread ID
//
// =============================================================================

#include "NewBomLog.h"

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------
// BuildLogPath - Build log file full path
// -----------------------------------------------------------------------------
// Determines where log file should be placed. Prefer CATIA temp directory.
//
// Path Selection Logic:
//   1. Try %LOCALAPPDATA%\DassaultSystemes\CATTemp\ (CATIA standard temp)
//   2. If LOCALAPPDATA doesn't exist, use Windows temp %TEMP%
//   3. If temp doesn't exist, use current directory "."
//
// Why CATTemp?
// CATIA stores runtime temp files in %LOCALAPPDATA%\DassaultSystemes\CATTemp.
// Our log file here makes it easy to view with other CATIA logs.
//
static void BuildLogPath(char* oPath, size_t iSize)
{
    char base[MAX_PATH] = { 0 };
    
    // Try to get LOCALAPPDATA (Windows user local app data directory)
    DWORD length = GetEnvironmentVariableA("LOCALAPPDATA", base, MAX_PATH);
    
    if (length == 0 || length >= MAX_PATH)
    {
        // LOCALAPPDATA not found, try Windows temp directory
        length = GetTempPathA(MAX_PATH, base);
        if (length == 0 || length >= MAX_PATH)
        {
            // Temp not found, use current directory
            lstrcpynA(base, ".", MAX_PATH);
        }
    }
    else
    {
        // Got LOCALAPPDATA, create CATIA directory structure
        char directory[MAX_PATH] = { 0 };
        
        // Create DassaultSystemes directory (if not exists)
        _snprintf_s(directory, sizeof(directory), _TRUNCATE,
                    "%s\\DassaultSystemes", base);
        CreateDirectoryA(directory, NULL);
        
        // Create CATTemp directory (if not exists)
        _snprintf_s(directory, sizeof(directory), _TRUNCATE,
                    "%s\\DassaultSystemes\\CATTemp", base);
        CreateDirectoryA(directory, NULL);
    }

    // Build full log file path
    _snprintf_s(oPath, iSize, _TRUNCATE,
                "%s%sNewBomPlugin.log",
                base,
                (base[lstrlenA(base) - 1] == '\\') ? "" : "\\DassaultSystemes\\CATTemp\\");
}

// -----------------------------------------------------------------------------
// NewBomLog - Main log function
// -----------------------------------------------------------------------------
// Format log message and write to file and debugger.
//
// Log Format:
// 2024-06-24 10:30:27.123 pid=1234 tid=5678 [ADDIN] CreateCommands() START
//
// Output Destinations:
//   1. OutputDebugStringA - Windows debug output
//      Visible in VS "Output" window when debugging
//   2. Log file - Permanent storage, can view later
//
void NewBomLog(const char* iStage, const char* iFormat, ...)
{
    // Step 1: Format user message
    char message[1024] = { 0 };
    va_list arguments;
    va_start(arguments, iFormat);
    _vsnprintf_s(message, sizeof(message), _TRUNCATE, iFormat, arguments);
    va_end(arguments);

    // Step 2: Get current time
    SYSTEMTIME now;
    GetLocalTime(&now);

    // Step 3: Build complete log line
    char line[1400] = { 0 };
    _snprintf_s(line, sizeof(line), _TRUNCATE,
                "%04u-%02u-%02u %02u:%02u:%02u.%03u pid=%lu tid=%lu [%s] %s\r\n",
                now.wYear, now.wMonth, now.wDay,
                now.wHour, now.wMinute, now.wSecond, now.wMilliseconds,
                GetCurrentProcessId(), GetCurrentThreadId(), iStage, message);

    // Step 4: Output to debugger
    OutputDebugStringA(line);

    // Step 5: Write to log file
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