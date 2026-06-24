// =============================================================================
// NewBomModule.cpp
// CATIA CAA Plugin Module Entry Point - DLL Load/Unload Tracking
// =============================================================================
//
// What is Module Entry Point?
// When CATIA loads DLL, global objects' constructors are executed.
// This file defines global object NewBomModuleTrace for:
//   - Log when DLL loads
//   - Log when DLL unloads
//
// Why need this?
// CATIA CAA development, knowing when DLL loads/unloads is important:
//   - Confirm plugin correctly loaded
//   - Track CATIA startup process
//   - Discover unload issues
//
// How it works:
//   1. CATIA starts -> loads NewBomModule.dll
//   2. Global object g_NewBomModuleTrace constructs -> write "loaded" log
//   3. CATIA runs -> plugin works normally
//   4. CATIA closes -> unload DLL
//   5. Global object destructs -> write "unloaded" log
//
// C++ Global Object Lifecycle:
//   - Program/DLL start: All global objects construct (definition order)
//   - Program/DLL exit: All global objects destruct (reverse order)
//
// =============================================================================

#include "NewBomLog.h"

// -----------------------------------------------------------------------------
// NewBomModuleTrace Class - Track DLL Load/Unload
// -----------------------------------------------------------------------------
// Simple helper class, writes log on construct and destruct.
// No data stored, only for lifecycle tracking.
//
class NewBomModuleTrace
{
public:
    // Constructor - Called when DLL loads
    NewBomModuleTrace()
    {
        NewBomLog("MODULE", "NewBomModule.dll loaded");
        // If you see this log, it means:
        //   1. CATIA found our DLL
        //   2. DLL loaded to CATIA process
        //   3. CATIA will create Addin object next
    }

    // Destructor - Called when DLL unloads
    ~NewBomModuleTrace()
    {
        NewBomLog("MODULE", "NewBomModule.dll unloaded");
        // If you see this log, DLL unloaded normally.
        // If CATIA closes abnormally, might not see this.
    }
};

// -----------------------------------------------------------------------------
// Global Object Instance
// -----------------------------------------------------------------------------
// Define global instance. C++ runtime automatically calls constructor/destructor.
static NewBomModuleTrace g_NewBomModuleTrace;

// =============================================================================
// File End
// =============================================================================
// This file is simple, only tracks DLL lifecycle.
// Actual plugin functionality in other files:
//   - NewBomAddin.cpp: Menu creation
//   - NewBomCmd.cpp: Get new number command
//   - NewBomAboutCmd.cpp: About dialog command
//   - NewBomDlg.cpp: Get number dialog
// =============================================================================