// PartNumberMenuAboutCmd.cpp
// CATIA CAA "About" Command Implementation
//
// This file implements the "关于本插件" (About Plugin) command.
// When user clicks this menu item, CATIA shows a simple information dialog.
//
// This command is simpler than PartNumberMenuCmd:
//   - No persistent dialog (shows modal box, then exits)
//   - No event callbacks (dialog blocks until user clicks OK)
//   - Self-destructs after showing the dialog
//
// Key CATIA CAA concepts:
//   - CATDlgNotify: Modal information dialog (like MessageBox)
//   - DisplayBlocked(): Shows dialog and waits for user response
//   - Modal dialogs block until user dismisses them
//   - One-shot commands: Self-destruct immediately after action

#include "PartNumberMenuAboutCmd.h"
#include "PartNumberMenuLog.h"
#include "CATApplicationFrame.h"
#include "CATDlgNotify.h"
#include "CATCreateExternalObject.h"
#include "CATUnicodeString.h"
#include "CATI18NTypes.h"

//=============================================================================
// CLASS REGISTRATION
//=============================================================================
CATImplementClass(PartNumberMenuAboutCmd,
                  Implementation,
                  CATCommand,
                  CATNull);

CATCreateClass(PartNumberMenuAboutCmd);

//=============================================================================
// CONSTRUCTOR
//=============================================================================
PartNumberMenuAboutCmd::PartNumberMenuAboutCmd()
    : CATCommand(NULL, "PartNumberMenuAboutCmd")
{
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "PartNumberMenuAboutCmd CONSTRUCTOR START");
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "  this pointer = %p", this);
    PartNumberMenuLog("ABOUT", "  Purpose: About command created (will show info dialog)");
    PartNumberMenuLog("ABOUT", "CONSTRUCTOR END");
}

//=============================================================================
// DESTRUCTOR
//=============================================================================
PartNumberMenuAboutCmd::~PartNumberMenuAboutCmd()
{
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "PartNumberMenuAboutCmd DESTRUCTOR START");
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "  this pointer = %p", this);
    PartNumberMenuLog("ABOUT", "DESTRUCTOR END");
}

//=============================================================================
// Activate - SHOW ABOUT DIALOG
//=============================================================================
// This method is called when user clicks "关于本插件" menu item.
// It shows a modal information dialog and then self-destructs.
//
// Flow:
//   1. Get CATIA application frame
//   2. Get main window as dialog parent
//   3. Create CATDlgNotify (modal information dialog)
//   4. Set dialog title and message
//   5. Call DisplayBlocked() - this waits for user to click OK
//   6. Destroy the dialog
//   7. Self-destruct (RequestDelayedDestruction)
//
CATStatusChangeRC PartNumberMenuAboutCmd::Activate(CATCommand* iCmd,
                                                   CATNotification* iNotif)
{
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "Activate() START - User clicked '关于本插件'!");
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "  this = %p", this);
    PartNumberMenuLog("ABOUT", "  iCmd = %p (command that triggered activation)", iCmd);
    PartNumberMenuLog("ABOUT", "  iNotif = %p (notification data)", iNotif);
    PartNumberMenuLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 1: Get CATIA Application Frame
    // -----------------------------------------------------------------------
    PartNumberMenuLog("ABOUT", "STEP 1: Getting CATIA Application Frame...");
    PartNumberMenuLog("ABOUT", "  Call: CATApplicationFrame::GetFrame()");
    
    CATApplicationFrame* pFrame = CATApplicationFrame::GetFrame();
    
    if (!pFrame)
    {
        PartNumberMenuLog("ABOUT", "  ERROR: Application frame is NULL!");
        PartNumberMenuLog("ABOUT", "  CATIA may not be running properly");
        PartNumberMenuLog("ABOUT", "  Calling RequestDelayedDestruction (abort)");
        RequestDelayedDestruction();
        return CATStatusChangeRCCompleted;
    }
    
    PartNumberMenuLog("ABOUT", "  SUCCESS: pFrame = %p", pFrame);
    PartNumberMenuLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 2: Get Main Window
    // -----------------------------------------------------------------------
    PartNumberMenuLog("ABOUT", "STEP 2: Getting CATIA Main Window...");
    PartNumberMenuLog("ABOUT", "  Call: pFrame->GetMainWindow()");
    
    CATDialog* pParent = pFrame->GetMainWindow();
    
    if (!pParent)
    {
        PartNumberMenuLog("ABOUT", "  ERROR: Main window is NULL!");
        PartNumberMenuLog("ABOUT", "  Cannot create dialog without parent");
        PartNumberMenuLog("ABOUT", "  Calling RequestDelayedDestruction (abort)");
        RequestDelayedDestruction();
        return CATStatusChangeRCCompleted;
    }
    
    PartNumberMenuLog("ABOUT", "  SUCCESS: pParent = %p", pParent);
    PartNumberMenuLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 3: Prepare Dialog Content (Chinese Text)
    // -----------------------------------------------------------------------
    PartNumberMenuLog("ABOUT", "STEP 3: Preparing dialog content...");
    PartNumberMenuLog("ABOUT", "  Creating message string: '件取号插件 v1.0 / CATIA V5R21 CAA'");
    PartNumberMenuLog("ABOUT", "  Chinese characters (UCS-2):");
    PartNumberMenuLog("ABOUT", "    件=0x4EF6, 取=0x53D6, 号=0x53F7, 插=0x63D2, 件=0x4EF6");
    
    // Build the message string using UCS-2 code points
    // "件取号插件 v1.0 / CATIA V5R21 CAA"
    static const CATUC2Bytes msgChars[] = {
        0x4EF6, 0x53D6, 0x53F7, 0x63D2, 0x4EF6,  // "件取号插件"
        ' ', 'v', '1', '.', '0', ' ', '/', ' ',   // " v1.0 / "
        'C', 'A', 'T', 'I', 'A', ' ', 'V', '5', 'R', '2', '1', ' ', 'C', 'A', 'A',
        0  // null terminator
    };
    
    CATUnicodeString message;
    message.BuildFromUCChar(msgChars, 29);
    PartNumberMenuLog("ABOUT", "  Message string length = %d characters", message.GetLengthInChar());
    
    PartNumberMenuLog("ABOUT", "  Creating title string: '关于本插件'");
    PartNumberMenuLog("ABOUT", "  Chinese characters (UCS-2):");
    PartNumberMenuLog("ABOUT", "    关=0x5173, 于=0x4E8E, 本=0x672C, 插=0x63D2, 件=0x4EF6");
    
    // Build the title string
    // "关于本插件"
    static const CATUC2Bytes titleChars[] = {
        0x5173, 0x4E8E, 0x672C, 0x63D2, 0x4EF6,  // "关于本插件"
        0  // null terminator
    };
    
    CATUnicodeString title;
    title.BuildFromUCChar(titleChars, 5);
    PartNumberMenuLog("ABOUT", "  Title string length = %d characters", title.GetLengthInChar());
    PartNumberMenuLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 4: Create Modal Dialog
    // -----------------------------------------------------------------------
    PartNumberMenuLog("ABOUT", "STEP 4: Creating modal information dialog...");
    PartNumberMenuLog("ABOUT", "  Call: new CATDlgNotify(pParent, 'PartNumberMenuAboutBox', ...)");
    PartNumberMenuLog("ABOUT", "  Parameters:");
    PartNumberMenuLog("ABOUT", "    - pParent: Parent window (CATIA main window)");
    PartNumberMenuLog("ABOUT", "    - 'PartNumberMenuAboutBox': Dialog identifier");
    PartNumberMenuLog("ABOUT", "    - CATDlgNfyInformation: Dialog type (information icon)");
    PartNumberMenuLog("ABOUT", "    - CATDlgNfyOK: Show OK button");
    PartNumberMenuLog("ABOUT", "    - CATDlgWndModal: Modal dialog (blocks until closed)");
    
    CATDlgNotify* pNotify = new CATDlgNotify(
        pParent,
        "PartNumberMenuAboutBox",
        CATDlgNfyInformation | CATDlgNfyOK | CATDlgWndModal);
    
    if (!pNotify)
    {
        PartNumberMenuLog("ABOUT", "  ERROR: CATDlgNotify creation failed!");
        PartNumberMenuLog("ABOUT", "  Calling RequestDelayedDestruction (abort)");
        RequestDelayedDestruction();
        return CATStatusChangeRCCompleted;
    }
    
    PartNumberMenuLog("ABOUT", "  SUCCESS: pNotify = %p (dialog created)", pNotify);
    PartNumberMenuLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 5: Display Dialog (BLOCKING)
    // -----------------------------------------------------------------------
    PartNumberMenuLog("ABOUT", "STEP 5: Displaying modal dialog...");
    PartNumberMenuLog("ABOUT", "  Call: pNotify->DisplayBlocked(message, title)");
    PartNumberMenuLog("ABOUT", "  IMPORTANT: This method BLOCKS until user clicks OK!");
    PartNumberMenuLog("ABOUT", "  The dialog will appear and CATIA will wait");
    PartNumberMenuLog("ABOUT", "  User must click OK button to continue");
    
    int button = pNotify->DisplayBlocked(message, title);
    
    PartNumberMenuLog("ABOUT", "  DisplayBlocked returned: button = %d", button);
    PartNumberMenuLog("ABOUT", "  Button codes: 1=OK, 2=Cancel (if Cancel button present)");
    PartNumberMenuLog("ABOUT", "  Dialog has been dismissed by user");
    PartNumberMenuLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 6: Destroy Dialog
    // -----------------------------------------------------------------------
    PartNumberMenuLog("ABOUT", "STEP 6: Destroying dialog...");
    PartNumberMenuLog("ABOUT", "  Call: pNotify->RequestDelayedDestruction()");
    
    pNotify->RequestDelayedDestruction();
    
    PartNumberMenuLog("ABOUT", "  Dialog will be destroyed safely");
    PartNumberMenuLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 7: Self-Destruct Command
    // -----------------------------------------------------------------------
    PartNumberMenuLog("ABOUT", "STEP 7: Self-destructing command...");
    PartNumberMenuLog("ABOUT", "  Call: RequestDelayedDestruction()");
    PartNumberMenuLog("ABOUT", "  This is a one-shot command - no persistent state needed");
    PartNumberMenuLog("ABOUT", "  Command will be destroyed after this method returns");
    
    RequestDelayedDestruction();
    
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "Activate() END - About dialog shown and dismissed");
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "  User clicked OK button");
    PartNumberMenuLog("ABOUT", "  Returning: CATStatusChangeRCCompleted");
    
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// Desactivate - COMMAND PAUSE
//=============================================================================
// Called when another command activates while this one is running.
// For a modal dialog command, this typically doesn't happen because
// the dialog blocks until dismissed.
//
CATStatusChangeRC PartNumberMenuAboutCmd::Desactivate(CATCommand* iCmd,
                                                      CATNotification* iNotif)
{
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "Desactivate() - Command paused");
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "  this = %p", this);
    PartNumberMenuLog("ABOUT", "  Note: This should not happen for modal dialog commands");
    
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// Cancel - COMMAND ABORT
//=============================================================================
// Called if command is cancelled before dialog is shown.
//
CATStatusChangeRC PartNumberMenuAboutCmd::Cancel(CATCommand* iCmd,
                                                 CATNotification* iNotif)
{
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "Cancel() - Command cancelled");
    PartNumberMenuLog("ABOUT", "===========================================");
    PartNumberMenuLog("ABOUT", "  this = %p", this);
    PartNumberMenuLog("ABOUT", "  Note: Command was cancelled before completion");
    
    return CATStatusChangeRCCompleted;
}