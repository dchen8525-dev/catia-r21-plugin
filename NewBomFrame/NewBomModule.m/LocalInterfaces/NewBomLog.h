// =============================================================================
// NewBomLog.h
// Logging Utility Header - For Debugging and Tracking Plugin Status
// =============================================================================
//
// Why need logging?
// CATIA CAA development debugging is difficult:
//   - Cannot use printf or cout directly (CATIA not in console)
//   - Cannot easily use VS breakpoints (RADE environment special)
//   - Error messages hard to track
//
// This logging utility provides:
//   - Write to log file (can view history)
//   - Output to debugger (visible in VS debugging)
//   - Include timestamp, process ID, thread ID
//   - Stage markers (ADDIN, COMMAND, ABOUT, DIALOG, etc.)
//
// Log File Location:
// %LOCALAPPDATA%\DassaultSystemes\CATTemp\NewBomPlugin.log
// or %TEMP%\NewBomPlugin.log
//
// Usage:
// NewBomLog("StageName", "Message format", args...);
// Example:
//   NewBomLog("ADDIN", "CreateCommands() START");
//   NewBomLog("COMMAND", "Dialog created at %p", m_pDialog);
//
// =============================================================================

#ifndef __NewBomLog_h__
#define __NewBomLog_h__

/**
 * Write log message
 * 
 * Parameters:
 * @param iStage Stage identifier (e.g. "ADDIN", "COMMAND", "ABOUT", "DIALOG")
 *               Used to distinguish log sources, convenient for filtering
 * @param iFormat Format string (similar to printf)
 *               Supports %p (pointer), %d (integer), %s (string), etc.
 * @param ... Variable arguments (based on format string)
 * 
 * Log Format:
 * 2024-06-24 10:30:27.123 pid=1234 tid=5678 [ADDIN] CreateCommands() START
 * |--Timestamp (ms precision)--|--ProcessID--|--ThreadID--|--Stage--|--Message--
 * 
 * Output Destinations:
 *   1. Log file: NewBomPlugin.log
 *   2. Debugger: OutputDebugString (visible in VS debugging)
 */
void NewBomLog(const char* iStage, const char* iFormat, ...);

#endif // __NewBomLog_h__