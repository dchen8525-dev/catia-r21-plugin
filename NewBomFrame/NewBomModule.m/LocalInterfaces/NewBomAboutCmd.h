// =============================================================================
// NewBomAboutCmd.h
// CATIA CAA Command Header File - "About Plugin" Command
// =============================================================================
//
// What is this command?
// A simple "one-shot" command that displays plugin version info.
// Unlike NewBomCmd:
//   - No persistent dialog
//   - Shows modal info box then immediately ends
//   - No shared mode needed
//
// Modal vs Non-modal Dialogs:
//   - Modal: Dialog blocks CATIA, user must close it first
//   - Non-modal: Dialog allows user to continue operating CATIA
//
// This command uses modal dialog (CATDlgNotify) because:
//   - Info box only needs user to click "OK"
//   - No need for user to do other operations in CATIA
//   - Command ends immediately after display
//
// Why simpler than NewBomCmd?
//   1. No dialog pointer management (modal dialog manages itself)
//   2. No event callbacks needed (DisplayBlocked directly waits)
//   3. No shared mode needed (command ends immediately)
//   4. Only one method (Activate) completes all work
//
// =============================================================================

#ifndef __NewBomAboutCmd_h__
#define __NewBomAboutCmd_h__

#include "CATCommand.h"

/**
 * @class NewBomAboutCmd
 * @brief "About Plugin" Command Implementation
 * 
 * Execution Flow:
 *   1. User clicks "About Plugin" menu
 *   2. CATIA creates NewBomAboutCmd instance
 *   3. Calls Activate()
 *   4. Creates CATDlgNotify (modal info box)
 *   5. Calls DisplayBlocked() - shows and waits for user click
 *   6. User clicks OK -> DisplayBlocked returns
 *   7. Destroys info box
 *   8. Calls RequestDelayedDestruction() - command self-destructs
 * 
 * One-shot command characteristics:
 *   - Performs simple action then immediately ends
 *   - Maintains no state
 *   - No continuous user interaction needed
 */
class NewBomAboutCmd : public CATCommand
{
    CATDeclareClass;

public:
    NewBomAboutCmd();
    virtual ~NewBomAboutCmd();

    // CATCommand virtual methods
    
    /**
     * Command Activation - Show About Info
     * 
     * Full Flow:
     *   1. Get CATIA application frame
     *   2. Get main window as dialog parent
     *   3. Prepare message and title (Chinese text)
     *   4. Create CATDlgNotify modal dialog
     *   5. DisplayBlocked() - show and wait
     *   6. Destroy dialog
     *   7. Self-destruct command
     */
    virtual CATStatusChangeRC Activate(CATCommand* iCmd,
                                       CATNotification* iNotif);
    
    /**
     * Command Deactivation
     * 
     * For modal dialog commands, usually not called
     * because DisplayBlocked blocks until dialog closes.
     */
    virtual CATStatusChangeRC Desactivate(CATCommand* iCmd,
                                          CATNotification* iNotif);
    
    /**
     * Command Cancel
     * 
     * Called if command cancelled before dialog shown.
     */
    virtual CATStatusChangeRC Cancel(CATCommand* iCmd,
                                     CATNotification* iNotif);
};

#endif // __NewBomAboutCmd_h__