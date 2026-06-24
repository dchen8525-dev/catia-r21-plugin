// NewBomCmd.cpp
// CATIA CAA Part Number Menu Command Implementation
//
// This file implements the main command for the "零件取号" (Part Numbering) menu item.
// When user clicks this menu item, CATIA:
//   1. Loads the NewBomModule DLL
//   2. Creates an instance of NewBomCmd (this class)
//   3. Calls Activate() to run the command
//   4. Displays the dialog (NewBomDlg)
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

#include "NewBomCmd.h"
#include "NewBomDlg.h"
#include "NewBomLog.h"
#include "CATApplicationFrame.h"
#include "CATCreateExternalObject.h"

//=============================================================================
// CLASS REGISTRATION
//=============================================================================
// CATImplementClass registers this as a standalone Implementation class.
// Unlike NewBomAddin (DataExtension), this is a full implementation.
//
// CATCreateClass creates a factory that CATIA uses to instantiate this class.
// When command header references "NewBomCmd", CATIA calls this factory.

CATImplementClass(NewBomCmd,
                  Implementation,
                  CATCommand,
                  CATNull);

CATCreateClass(NewBomCmd);

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
NewBomCmd::NewBomCmd()
    : CATCommand(NULL, "NewBomCmd")
{
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "NewBomCmd CONSTRUCTOR START");
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "  this pointer = %p", this);
    NewBomLog("COMMAND", "  m_pDialog initialized to NULL");
    
    m_pDialog = NULL;
    
    NewBomLog("COMMAND", "  Calling RequestStatusChange(CATCommandMsgRequestSharedMode)");
    NewBomLog("COMMAND", "  Purpose: Allow this command to share CATIA with other commands");
    NewBomLog("COMMAND", "  Without shared mode, CATIA would switch workbench");
    
    RequestStatusChange(CATCommandMsgRequestSharedMode);
    
    NewBomLog("COMMAND", "CONSTRUCTOR END - Command ready for activation");
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
NewBomCmd::~NewBomCmd()
{
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "NewBomCmd DESTRUCTOR START");
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "  this pointer = %p", this);
    NewBomLog("COMMAND", "  m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        NewBomLog("COMMAND", "  Dialog exists, calling RequestDelayedDestruction()");
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
        NewBomLog("COMMAND", "  Dialog pointer set to NULL");
    }
    
    NewBomLog("COMMAND", "DESTRUCTOR END");
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
CATStatusChangeRC NewBomCmd::Activate(CATCommand* iCmd,
                                          CATNotification* iNotif)
{
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "Activate() START - User clicked menu item!");
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "  this = %p", this);
    NewBomLog("COMMAND", "  iCmd = %p (command that triggered activation)", iCmd);
    NewBomLog("COMMAND", "  iNotif = %p (notification data)", iNotif);
    NewBomLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 1: Get CATIA Application Frame
    // -----------------------------------------------------------------------
    NewBomLog("COMMAND", "STEP 1: Getting CATIA Application Frame...");
    NewBomLog("COMMAND", "  Call: CATApplicationFrame::GetFrame()");
    
    CATApplicationFrame* pFrame = CATApplicationFrame::GetFrame();
    
    if (!pFrame)
    {
        NewBomLog("COMMAND", "  ERROR: Application frame is NULL!");
        NewBomLog("COMMAND", "  This means CATIA is not running properly");
        NewBomLog("COMMAND", "  Returning: CATStatusChangeRCCompleted (abort)");
        return CATStatusChangeRCCompleted;
    }
    
    NewBomLog("COMMAND", "  SUCCESS: pFrame = %p", pFrame);
    NewBomLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 2: Get Main Window
    // -----------------------------------------------------------------------
    NewBomLog("COMMAND", "STEP 2: Getting CATIA Main Window...");
    NewBomLog("COMMAND", "  Call: pFrame->GetMainWindow()");
    NewBomLog("COMMAND", "  Purpose: Need parent window for dialog creation");
    
    CATDialog* pWindow = pFrame->GetMainWindow();
    
    if (!pWindow)
    {
        NewBomLog("COMMAND", "  ERROR: Main window is NULL!");
        NewBomLog("COMMAND", "  Cannot create dialog without parent");
        NewBomLog("COMMAND", "  Returning: CATStatusChangeRCCompleted (abort)");
        return CATStatusChangeRCCompleted;
    }
    
    NewBomLog("COMMAND", "  SUCCESS: pWindow = %p (main window ready)", pWindow);
    NewBomLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 3: Clean up any existing dialog
    // -----------------------------------------------------------------------
    NewBomLog("COMMAND", "STEP 3: Cleaning up any existing dialog...");
    NewBomLog("COMMAND", "  Current m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        NewBomLog("COMMAND", "  Dialog exists, destroying it first");
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }
    else
    {
        NewBomLog("COMMAND", "  No existing dialog, proceeding");
    }
    NewBomLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 4: Create New Dialog
    // -----------------------------------------------------------------------
    NewBomLog("COMMAND", "STEP 4: Creating new dialog...");
    NewBomLog("COMMAND", "  Call: new NewBomDlg(pWindow, 'HelloWorldDialog')");
    NewBomLog("COMMAND", "  - pWindow: Parent window (CATIA main window)");
    NewBomLog("COMMAND", "  - 'HelloWorldDialog': Dialog identifier");
    
    m_pDialog = new NewBomDlg(pWindow, "HelloWorldDialog");
    
    if (!m_pDialog)
    {
        NewBomLog("COMMAND", "  ERROR: Dialog creation failed (NULL returned)");
        NewBomLog("COMMAND", "  Possible cause: Memory allocation failed");
        NewBomLog("COMMAND", "  Returning: CATStatusChangeRCCompleted (abort)");
        return CATStatusChangeRCCompleted;
    }
    
    NewBomLog("COMMAND", "  SUCCESS: m_pDialog = %p (dialog created)", m_pDialog);
    NewBomLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 5: Connect Dialog Events
    // -----------------------------------------------------------------------
    NewBomLog("COMMAND", "STEP 5: Connecting dialog events...");
    NewBomLog("COMMAND", "  Setting dialog's father to this command");
    
    m_pDialog->SetFather(this);
    
    NewBomLog("COMMAND", "  Registering callback for GetDiaCLOSENotification()");
    NewBomLog("COMMAND", "  When dialog closes, it will call CloseDialog method");
    
    AddAnalyseNotificationCB(m_pDialog,
        m_pDialog->GetDiaCLOSENotification(),
        (CATCommandMethod)&NewBomCmd::CloseDialog,
        NULL);
    
    NewBomLog("COMMAND", "  Registering callback for GetWindCloseNotification()");
    NewBomLog("COMMAND", "  When window closes (X button), it will call CloseDialog");
    
    AddAnalyseNotificationCB(m_pDialog,
        m_pDialog->GetWindCloseNotification(),
        (CATCommandMethod)&NewBomCmd::CloseDialog,
        NULL);
    
    NewBomLog("COMMAND", "  Callbacks registered successfully");
    NewBomLog("COMMAND", "");

    // -----------------------------------------------------------------------
    // Step 6: Show Dialog
    // -----------------------------------------------------------------------
    NewBomLog("COMMAND", "STEP 6: Showing dialog to user...");
    NewBomLog("COMMAND", "  Call: m_pDialog->SetVisibility(CATDlgShow)");
    NewBomLog("COMMAND", "  CATDlgShow makes the dialog visible");
    
    m_pDialog->SetVisibility(CATDlgShow);
    
    NewBomLog("COMMAND", "  Dialog is now visible to user");
    NewBomLog("COMMAND", "");

    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "Activate() END - Dialog displayed successfully");
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "  m_pDialog = %p", m_pDialog);
    NewBomLog("COMMAND", "  Returning: CATStatusChangeRCCompleted");
    
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
void NewBomCmd::CloseDialog(CATCommand* iCmd,
                                CATNotification* iNotif,
                                CATCommandClientData iClientData)
{
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "CloseDialog() START - User closed dialog");
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "  this = %p", this);
    NewBomLog("COMMAND", "  iCmd = %p (dialog that sent notification)", iCmd);
    NewBomLog("COMMAND", "  m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        NewBomLog("COMMAND", "  Hiding dialog (keeping for potential reuse)");
        m_pDialog->SetVisibility(CATDlgHide);
    }
    else
    {
        NewBomLog("COMMAND", "  No dialog to hide");
    }
    
    NewBomLog("COMMAND", "  Calling RequestDelayedDestruction()");
    NewBomLog("COMMAND", "  This safely destroys command after all events processed");
    
    RequestDelayedDestruction();
    
    NewBomLog("COMMAND", "CloseDialog() END");
}

//=============================================================================
// Desactivate - COMMAND PAUSE
//=============================================================================
// Called when another command activates while this one is running.
// In shared mode, multiple commands can coexist, so this may be called.
//
CATStatusChangeRC NewBomCmd::Desactivate(CATCommand* iCmd,
                                             CATNotification* iNotif)
{
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "Desactivate() START - Command paused");
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "  this = %p", this);
    NewBomLog("COMMAND", "  Note: Another command has taken focus");
    NewBomLog("COMMAND", "  Our dialog remains visible but inactive");
    
    NewBomLog("COMMAND", "Desactivate() END");
    
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// Cancel - COMMAND ABORT
//=============================================================================
// Called when command is cancelled (e.g., user presses ESC, or system cancels).
// Must clean up any resources (like the dialog).
//
CATStatusChangeRC NewBomCmd::Cancel(CATCommand* iCmd,
                                        CATNotification* iNotif)
{
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "Cancel() START - Command cancelled");
    NewBomLog("COMMAND", "===========================================");
    NewBomLog("COMMAND", "  this = %p", this);
    NewBomLog("COMMAND", "  m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        NewBomLog("COMMAND", "  Dialog exists, destroying it");
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
        NewBomLog("COMMAND", "  Dialog destroyed and pointer cleared");
    }
    
    NewBomLog("COMMAND", "Cancel() END - Cleanup complete");
    
    return CATStatusChangeRCCompleted;
}