// =============================================================================
// NewBomCmd.h
// CATIA CAA Command Header File - "Get New Number" Command
// =============================================================================
//
// What is CATCommand?
// CATCommand is the base class for all CATIA commands. Commands handle
// user interaction:
//   - Clicking menu item triggers a command
//   - Command performs actions (show dialog, modify data, etc.)
//   - Command has lifecycle (create->activate->deactivate->destroy)
//
// Command Lifecycle:
//   1. Construct: User clicks menu -> CATIA creates command instance
//   2. Activate: CATIA calls Activate() -> command starts executing
//   3. Running: User interacts with command (fill dialog)
//   4. Deactivate: Called when another command activates
//   5. Cancel: Called when user cancels command
//   6. Destroy: Command is destroyed after completion
//
// This Command's Function:
//   - Display dialog for user to fill part information
//   - Generate part number when button clicked
//   - Number format: DLXX0001 (DLXX + 4 digits)
//
// =============================================================================

#ifndef __NewBomCmd_h__
#define __NewBomCmd_h__

#include "CATCommand.h"

class NewBomDlg;

/**
 * @class NewBomCmd
 * @brief "Get New Number" Command Implementation
 * 
 * When user clicks "Get New Number" menu:
 *   1. CATIA creates NewBomCmd instance
 *   2. Calls Activate() to show dialog
 *   3. User fills information and clicks button
 *   4. Generate part number
 *   5. User closes dialog -> command ends
 * 
 * Shared Mode vs Exclusive Mode:
 *   - Exclusive: CATIA switches to exclusive workbench for command
 *   - Shared: Command can run alongside other commands
 * 
 * This command uses shared mode (dialog doesn't need to monopolize CATIA).
 * Set via RequestStatusChange(CATCommandMsgRequestSharedMode).
 */
class NewBomCmd : public CATCommand
{
    CATDeclareClass;

public:
    NewBomCmd();
    virtual ~NewBomCmd();

    // CATCommand virtual methods - lifecycle management
    
    /**
     * Command Activation
     * 
     * When: Called after user clicks menu item
     * 
     * What it does:
     *   1. Get CATIA main window (as dialog parent)
     *   2. Create dialog instance
     *   3. Connect dialog close event
     *   4. Show dialog
     */
    virtual CATStatusChangeRC Activate(CATCommand* iCmd,
                                       CATNotification* iNotif);
    
    /**
     * Command Deactivation
     * 
     * When: Another command activates while this one is running
     * In shared mode, dialog can remain visible but inactive.
     */
    virtual CATStatusChangeRC Desactivate(CATCommand* iCmd,
                                          CATNotification* iNotif);
    
    /**
     * Command Cancel
     * 
     * When: User cancels command (press ESC) or system cancels
     * 
     * What it does:
     *   1. Destroy dialog
     *   2. Clear pointer
     */
    virtual CATStatusChangeRC Cancel(CATCommand* iCmd,
                                     CATNotification* iNotif);

private:
    /**
     * Handle dialog close event
     * 
     * Callback function, called when user closes dialog.
     * Registered via AddAnalyseNotificationCB.
     * 
     * What it does:
     *   1. Hide dialog (don't destroy immediately)
     *   2. Request command destruction (command is done)
     */
    void CloseDialog(CATCommand* iCmd,
                     CATNotification* iNotif,
                     CATCommandClientData iClientData);

    // Dialog instance pointer
    // Command owns the dialog, responsible for creation and destruction
    NewBomDlg* m_pDialog;
};

#endif // __NewBomCmd_h__