// PartNumberMenuCmd.cpp
// CATIA CAA Part Number Menu Command Implementation
//
// This file implements the main command for the "取新编号" (Get New Number) menu item.
// When user clicks this menu item, CATIA:
//   1. Loads the PartNumberMenuModule DLL
//   2. Creates an instance of PartNumberMenuCmd (this class)
//   3. Calls Activate() to run the command
//   4. Displays the dialog (PartNumberMenuDlg)
//   5. Handles dialog events (close, etc.)
//   6. Calls Desactivate/Cancel when command ends
//
// Key CATIA CAA concepts:
//   - CATCommand: Base class for all CATIA commands
//   - Activate(): Called when command starts (user clicks menu)
//   - Desactivate(): Called when command pauses (another command activates)
//   - Cancel(): Called when command is cancelled (user cancels or closes)
//   - RequestStatusChange(): Changes command state (e.g., shared mode)
//   - RequestDelayedDestruction(): Safely destroys command after it finishes

#include "PartNumberMenuCmd.h"
#include "PartNumberMenuDlg.h"
#include "PartNumberMenuLog.h"
#include "CATApplicationFrame.h"
#include "CATCreateExternalObject.h"

//=============================================================================
// CLASS REGISTRATION
//=============================================================================
// CATImplementClass registers this as a standalone Implementation class.
// Unlike PartNumberMenuAddin (DataExtension), this is a full implementation.
//
// CATCreateClass creates a factory that CATIA uses to instantiate this class.
// When command header references "PartNumberMenuCmd", CATIA calls this factory.

CATImplementClass(PartNumberMenuCmd,
                  Implementation,
                  CATCommand,
                  CATNull);

CATCreateClass(PartNumberMenuCmd);

//=============================================================================
// CONSTRUCTOR
//=============================================================================
// The constructor is called when CATIA creates a new instance of this command.
// This happens when:
//   - User clicks the menu item
//   - CATIA command header creates the command
//
// Important: RequestStatusChange(CATCommandMsgRequestSharedMode) is called.
// This allows the command to run alongside other CATIA commands (shared mode).
// Without this, CATIA would switch to a dedicated workbench for this command.
//
PartNumberMenuCmd::PartNumberMenuCmd()
    : CATCommand(NULL, "PartNumberMenuCmd")
{
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "PartNumberMenuCmd CONSTRUCTOR START");
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "  this pointer = %p", this);
    PartNumberMenuLog("COMMAND", "  m_pDialog initialized to NULL");
    
    m_pDialog = NULL;
    
    PartNumberMenuLog("COMMAND", "  Calling RequestStatusChange(CATCommandMsgRequestSharedMode)");
    PartNumberMenuLog("COMMAND", "  Purpose: Allow this command to share CATIA with other commands");
    PartNumberMenuLog("COMMAND", "  Without shared mode, CATIA would switch workbench");
    
    RequestStatusChange(CATCommandMsgRequestSharedMode);
    
    PartNumberMenuLog("COMMAND", "CONSTRUCTOR END - Command ready for activation");
}

//=============================================================================
// DESTRUCTOR
//=============================================================================
// Called when the command is destroyed.
// This happens after:
//   - User closes the dialog
//   - RequestDelayedDestruction() was called
//   - CATIA cleans up the command
//
PartNumberMenuCmd::~PartNumberMenuCmd()
{
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "PartNumberMenuCmd DESTRUCTOR START");
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "  this pointer = %p", this);
    PartNumberMenuLog("COMMAND", "  m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        PartNumberMenuLog("COMMAND", "  Dialog exists, calling RequestDelayedDestruction()");
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
        PartNumberMenuLog("COMMAND", "  Dialog pointer set to NULL");
    }
    
    PartNumberMenuLog("COMMAND", "DESTRUCTOR END");
}

//=============================================================================
// Activate - COMMAND START
//=============================================================================
// This is the MAIN entry point when user clicks the menu item.
// CATIA calls this method to "activate" the command.
//
// Typical flow:
//   1. Get CATIA's main application frame
//   2. Get the main window (parent for dialogs)
//   3. Create our dialog
//   4. Connect dialog close event to our CloseDialog method
//   5. Show the dialog
//
// Return value: CATStatusChangeRCCompleted means "command activation done"
//
CATStatusChangeRC PartNumberMenuCmd::Activate(CATCommand* iCmd,
                                          CATNotification* iNotif)
{
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "Activate() START - User clicked menu item!");
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "  this = %p", this);
    PartNumberMenuLog("COMMAND", "  iCmd = %p (command that triggered activation)", iCmd);
    PartNumberMenuLog("COMMAND", "  iNotif = %p (notification data)", iNotif);
    PartNumberMenuLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 1: Get CATIA Application Frame
    // -----------------------------------------------------------------------
    PartNumberMenuLog("COMMAND", "STEP 1: Getting CATIA Application Frame...");
    PartNumberMenuLog("COMMAND", "  Call: CATApplicationFrame::GetFrame()");
    
    CATApplicationFrame* pFrame = CATApplicationFrame::GetFrame();
    
    if (!pFrame)
    {
        PartNumberMenuLog("COMMAND", "  ERROR: Application frame is NULL!");
        PartNumberMenuLog("COMMAND", "  This means CATIA is not running properly");
        PartNumberMenuLog("COMMAND", "  Returning: CATStatusChangeRCCompleted (abort)");
        return CATStatusChangeRCCompleted;
    }
    
    PartNumberMenuLog("COMMAND", "  SUCCESS: pFrame = %p", pFrame);
    PartNumberMenuLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 2: Get Main Window
    // -----------------------------------------------------------------------
    PartNumberMenuLog("COMMAND", "STEP 2: Getting CATIA Main Window...");
    PartNumberMenuLog("COMMAND", "  Call: pFrame->GetMainWindow()");
    PartNumberMenuLog("COMMAND", "  Purpose: Need parent window for dialog creation");
    
    CATDialog* pWindow = pFrame->GetMainWindow();
    
    if (!pWindow)
    {
        PartNumberMenuLog("COMMAND", "  ERROR: Main window is NULL!");
        PartNumberMenuLog("COMMAND", "  Cannot create dialog without parent");
        PartNumberMenuLog("COMMAND", "  Returning: CATStatusChangeRCCompleted (abort)");
        return CATStatusChangeRCCompleted;
    }
    
    PartNumberMenuLog("COMMAND", "  SUCCESS: pWindow = %p (main window ready)", pWindow);
    PartNumberMenuLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 3: Clean up any existing dialog
    // -----------------------------------------------------------------------
    PartNumberMenuLog("COMMAND", "STEP 3: Cleaning up any existing dialog...");
    PartNumberMenuLog("COMMAND", "  Current m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        PartNumberMenuLog("COMMAND", "  Dialog exists, destroying it first");
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }
    else
    {
        PartNumberMenuLog("COMMAND", "  No existing dialog, proceeding");
    }
    PartNumberMenuLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 4: Create New Dialog
    // -----------------------------------------------------------------------
    PartNumberMenuLog("COMMAND", "STEP 4: Creating new dialog...");
    PartNumberMenuLog("COMMAND", "  Call: new PartNumberMenuDlg(pWindow, 'HelloWorldDialog')");
    PartNumberMenuLog("COMMAND", "  - pWindow: Parent window (CATIA main window)");
    PartNumberMenuLog("COMMAND", "  - 'HelloWorldDialog': Dialog identifier");
    
    m_pDialog = new PartNumberMenuDlg(pWindow, "HelloWorldDialog");
    
    if (!m_pDialog)
    {
        PartNumberMenuLog("COMMAND", "  ERROR: Dialog creation failed (NULL returned)");
        PartNumberMenuLog("COMMAND", "  Possible cause: Memory allocation failed");
        PartNumberMenuLog("COMMAND", "  Returning: CATStatusChangeRCCompleted (abort)");
        return CATStatusChangeRCCompleted;
    }
    
    PartNumberMenuLog("COMMAND", "  SUCCESS: m_pDialog = %p (dialog created)", m_pDialog);
    PartNumberMenuLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 5: Connect Dialog Events
    // -----------------------------------------------------------------------
    PartNumberMenuLog("COMMAND", "STEP 5: Connecting dialog events...");
    PartNumberMenuLog("COMMAND", "  Setting dialog's father to this command");
    
    m_pDialog->SetFather(this);
    
    PartNumberMenuLog("COMMAND", "  Registering callback for GetDiaCLOSENotification()");
    PartNumberMenuLog("COMMAND", "  When dialog closes, it will call CloseDialog method");
    
    AddAnalyseNotificationCB(m_pDialog,
        m_pDialog->GetDiaCLOSENotification(),
        (CATCommandMethod)&PartNumberMenuCmd::CloseDialog,
        NULL);
    
    PartNumberMenuLog("COMMAND", "  Registering callback for GetWindCloseNotification()");
    PartNumberMenuLog("COMMAND", "  When window closes (X button), it will call CloseDialog");
    
    AddAnalyseNotificationCB(m_pDialog,
        m_pDialog->GetWindCloseNotification(),
        (CATCommandMethod)&PartNumberMenuCmd::CloseDialog,
        NULL);
    
    PartNumberMenuLog("COMMAND", "  Callbacks registered successfully");
    PartNumberMenuLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 6: Show Dialog
    // -----------------------------------------------------------------------
    PartNumberMenuLog("COMMAND", "STEP 6: Showing dialog to user...");
    PartNumberMenuLog("COMMAND", "  Call: m_pDialog->SetVisibility(CATDlgShow)");
    PartNumberMenuLog("COMMAND", "  CATDlgShow makes the dialog visible");
    
    m_pDialog->SetVisibility(CATDlgShow);
    
    PartNumberMenuLog("COMMAND", "  Dialog is now visible to user");
    PartNumberMenuLog("COMMAND", "");

    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "Activate() END - Dialog displayed successfully");
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "  m_pDialog = %p", m_pDialog);
    PartNumberMenuLog("COMMAND", "  Returning: CATStatusChangeRCCompleted");
    
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// CloseDialog - HANDLE DIALOG CLOSE
//=============================================================================
// This method is called when the user closes the dialog.
// It's connected via AddAnalyseNotificationCB in Activate().
//
// Actions:
//   1. Hide the dialog (keep it for possible reuse)
//   2. Request command destruction (command is done)
//
void PartNumberMenuCmd::CloseDialog(CATCommand* iCmd,
                                CATNotification* iNotif,
                                CATCommandClientData iClientData)
{
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "CloseDialog() START - User closed dialog");
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "  this = %p", this);
    PartNumberMenuLog("COMMAND", "  iCmd = %p (dialog that sent notification)", iCmd);
    PartNumberMenuLog("COMMAND", "  m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        PartNumberMenuLog("COMMAND", "  Hiding dialog (keeping for potential reuse)");
        m_pDialog->SetVisibility(CATDlgHide);
    }
    else
    {
        PartNumberMenuLog("COMMAND", "  No dialog to hide");
    }
    
    PartNumberMenuLog("COMMAND", "  Calling RequestDelayedDestruction()");
    PartNumberMenuLog("COMMAND", "  This safely destroys command after all events processed");
    
    RequestDelayedDestruction();
    
    PartNumberMenuLog("COMMAND", "CloseDialog() END");
}

//=============================================================================
// Desactivate - COMMAND PAUSE
//=============================================================================
// Called when another command activates while this one is running.
// In shared mode, multiple commands can coexist, so this may be called.
//
CATStatusChangeRC PartNumberMenuCmd::Desactivate(CATCommand* iCmd,
                                             CATNotification* iNotif)
{
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "Desactivate() START - Command paused");
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "  this = %p", this);
    PartNumberMenuLog("COMMAND", "  Note: Another command has taken focus");
    PartNumberMenuLog("COMMAND", "  Our dialog remains visible but inactive");
    
    PartNumberMenuLog("COMMAND", "Desactivate() END");
    
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// Cancel - COMMAND ABORT
//=============================================================================
// Called when command is cancelled (e.g., user presses ESC, or system cancels).
// Must clean up any resources (like the dialog).
//
CATStatusChangeRC PartNumberMenuCmd::Cancel(CATCommand* iCmd,
                                        CATNotification* iNotif)
{
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "Cancel() START - Command cancelled");
    PartNumberMenuLog("COMMAND", "===========================================");
    PartNumberMenuLog("COMMAND", "  this = %p", this);
    PartNumberMenuLog("COMMAND", "  m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        PartNumberMenuLog("COMMAND", "  Dialog exists, destroying it");
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
        PartNumberMenuLog("COMMAND", "  Dialog destroyed and pointer cleared");
    }
    
    PartNumberMenuLog("COMMAND", "Cancel() END - Cleanup complete");
    
    return CATStatusChangeRCCompleted;
}