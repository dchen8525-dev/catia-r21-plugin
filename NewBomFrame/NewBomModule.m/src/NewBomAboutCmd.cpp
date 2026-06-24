// NewBomAboutCmd.cpp
// CATIA CAA "About" Command Implementation
//
// This file implements the "关于本插件" (About Plugin) command.
// When user clicks this menu item, CATIA shows a simple information dialog.
//
// This command is simpler than NewBomCmd:
//   - No persistent dialog (shows modal box, then exits)
//   - No event callbacks (dialog blocks until user clicks OK)
//   - Self-destructs after showing the dialog
//
// Key CATIA CAA concepts:
//   - CATDlgNotify: Modal information dialog (like MessageBox)
//   - DisplayBlocked(): Shows dialog and waits for user response
//   - Modal dialogs block until user dismisses them
//   - One-shot commands: Self-destruct immediately after action

#include "NewBomAboutCmd.h"
#include "NewBomLog.h"
#include "CATApplicationFrame.h"
#include "CATDlgNotify.h"
#include "CATCreateExternalObject.h"
#include "CATUnicodeString.h"
#include "CATI18NTypes.h"

//=============================================================================
// CLASS REGISTRATION
//=============================================================================
CATImplementClass(NewBomAboutCmd,
                  Implementation,
                  CATCommand,
                  CATNull);

CATCreateClass(NewBomAboutCmd);

//=============================================================================
// CONSTRUCTOR
//=============================================================================
NewBomAboutCmd::NewBomAboutCmd()
    : CATCommand(NULL, "NewBomAboutCmd")
{
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "NewBomAboutCmd CONSTRUCTOR START");
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "  this pointer = %p", this);
    NewBomLog("ABOUT", "  Purpose: About command created (will show info dialog)");
    NewBomLog("ABOUT", "CONSTRUCTOR END");
}

//=============================================================================
// DESTRUCTOR
//=============================================================================
NewBomAboutCmd::~NewBomAboutCmd()
{
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "NewBomAboutCmd DESTRUCTOR START");
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "  this pointer = %p", this);
    NewBomLog("ABOUT", "DESTRUCTOR END");
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
CATStatusChangeRC NewBomAboutCmd::Activate(CATCommand* iCmd,
                                                   CATNotification* iNotif)
{
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "Activate() START - User clicked '关于本插件'!");
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "  this = %p", this);
    NewBomLog("ABOUT", "  iCmd = %p (command that triggered activation)", iCmd);
    NewBomLog("ABOUT", "  iNotif = %p (notification data)", iNotif);
    NewBomLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 1: Get CATIA Application Frame
    // -----------------------------------------------------------------------
    NewBomLog("ABOUT", "STEP 1: Getting CATIA Application Frame...");
    NewBomLog("ABOUT", "  Call: CATApplicationFrame::GetFrame()");
    
    CATApplicationFrame* pFrame = CATApplicationFrame::GetFrame();
    
    if (!pFrame)
    {
        NewBomLog("ABOUT", "  ERROR: Application frame is NULL!");
        NewBomLog("ABOUT", "  CATIA may not be running properly");
        NewBomLog("ABOUT", "  Calling RequestDelayedDestruction (abort)");
        RequestDelayedDestruction();
        return CATStatusChangeRCCompleted;
    }
    
    NewBomLog("ABOUT", "  SUCCESS: pFrame = %p", pFrame);
    NewBomLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 2: Get Main Window
    // -----------------------------------------------------------------------
    NewBomLog("ABOUT", "STEP 2: Getting CATIA Main Window...");
    NewBomLog("ABOUT", "  Call: pFrame->GetMainWindow()");
    
    CATDialog* pParent = pFrame->GetMainWindow();
    
    if (!pParent)
    {
        NewBomLog("ABOUT", "  ERROR: Main window is NULL!");
        NewBomLog("ABOUT", "  Cannot create dialog without parent");
        NewBomLog("ABOUT", "  Calling RequestDelayedDestruction (abort)");
        RequestDelayedDestruction();
        return CATStatusChangeRCCompleted;
    }
    
    NewBomLog("ABOUT", "  SUCCESS: pParent = %p", pParent);
    NewBomLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 3: Prepare Dialog Content (Chinese Text)
    // -----------------------------------------------------------------------
    NewBomLog("ABOUT", "STEP 3: Preparing dialog content...");
    NewBomLog("ABOUT", "  Creating message string: '件取号插件 v1.0 / CATIA V5R21 CAA'");
    NewBomLog("ABOUT", "  Chinese characters (UCS-2):");
    NewBomLog("ABOUT", "    件=0x4EF6, 取=0x53D6, 号=0x53F7, 插=0x63D2, 件=0x4EF6");
    
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
    NewBomLog("ABOUT", "  Message string length = %d characters", message.GetLengthInChar());
    
    NewBomLog("ABOUT", "  Creating title string: '关于本插件'");
    NewBomLog("ABOUT", "  Chinese characters (UCS-2):");
    NewBomLog("ABOUT", "    关=0x5173, 于=0x4E8E, 本=0x672C, 插=0x63D2, 件=0x4EF6");
    
    // Build the title string
    // "关于本插件"
    static const CATUC2Bytes titleChars[] = {
        0x5173, 0x4E8E, 0x672C, 0x63D2, 0x4EF6,  // "关于本插件"
        0  // null terminator
    };
    
    CATUnicodeString title;
    title.BuildFromUCChar(titleChars, 5);
    NewBomLog("ABOUT", "  Title string length = %d characters", title.GetLengthInChar());
    NewBomLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 4: Create Modal Dialog
    // -----------------------------------------------------------------------
    NewBomLog("ABOUT", "STEP 4: Creating modal information dialog...");
    NewBomLog("ABOUT", "  Call: new CATDlgNotify(pParent, 'NewBomAboutBox', ...)");
    NewBomLog("ABOUT", "  Parameters:");
    NewBomLog("ABOUT", "    - pParent: Parent window (CATIA main window)");
    NewBomLog("ABOUT", "    - 'NewBomAboutBox': Dialog identifier");
    NewBomLog("ABOUT", "    - CATDlgNfyInformation: Dialog type (information icon)");
    NewBomLog("ABOUT", "    - CATDlgNfyOK: Show OK button");
    NewBomLog("ABOUT", "    - CATDlgWndModal: Modal dialog (blocks until closed)");
    
    CATDlgNotify* pNotify = new CATDlgNotify(
        pParent,
        "NewBomAboutBox",
        CATDlgNfyInformation | CATDlgNfyOK | CATDlgWndModal);
    
    if (!pNotify)
    {
        NewBomLog("ABOUT", "  ERROR: CATDlgNotify creation failed!");
        NewBomLog("ABOUT", "  Calling RequestDelayedDestruction (abort)");
        RequestDelayedDestruction();
        return CATStatusChangeRCCompleted;
    }
    
    NewBomLog("ABOUT", "  SUCCESS: pNotify = %p (dialog created)", pNotify);
    NewBomLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 5: Display Dialog (BLOCKING)
    // -----------------------------------------------------------------------
    NewBomLog("ABOUT", "STEP 5: Displaying modal dialog...");
    NewBomLog("ABOUT", "  Call: pNotify->DisplayBlocked(message, title)");
    NewBomLog("ABOUT", "  IMPORTANT: This method BLOCKS until user clicks OK!");
    NewBomLog("ABOUT", "  The dialog will appear and CATIA will wait");
    NewBomLog("ABOUT", "  User must click OK button to continue");
    
    int button = pNotify->DisplayBlocked(message, title);
    
    NewBomLog("ABOUT", "  DisplayBlocked returned: button = %d", button);
    NewBomLog("ABOUT", "  Button codes: 1=OK, 2=Cancel (if Cancel button present)");
    NewBomLog("ABOUT", "  Dialog has been dismissed by user");
    NewBomLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 6: Destroy Dialog
    // -----------------------------------------------------------------------
    NewBomLog("ABOUT", "STEP 6: Destroying dialog...");
    NewBomLog("ABOUT", "  Call: pNotify->RequestDelayedDestruction()");
    
    pNotify->RequestDelayedDestruction();
    
    NewBomLog("ABOUT", "  Dialog will be destroyed safely");
    NewBomLog("ABOUT", "");

    // -----------------------------------------------------------------------
    // Step 7: Self-Destruct Command
    // -----------------------------------------------------------------------
    NewBomLog("ABOUT", "STEP 7: Self-destructing command...");
    NewBomLog("ABOUT", "  Call: RequestDelayedDestruction()");
    NewBomLog("ABOUT", "  This is a one-shot command - no persistent state needed");
    NewBomLog("ABOUT", "  Command will be destroyed after this method returns");
    
    RequestDelayedDestruction();
    
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "Activate() END - About dialog shown and dismissed");
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "  User clicked OK button");
    NewBomLog("ABOUT", "  Returning: CATStatusChangeRCCompleted");
    
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// Desactivate - COMMAND PAUSE
//=============================================================================
// Called when another command activates while this one is running.
// For a modal dialog command, this typically doesn't happen because
// the dialog blocks until dismissed.
//
CATStatusChangeRC NewBomAboutCmd::Desactivate(CATCommand* iCmd,
                                                      CATNotification* iNotif)
{
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "Desactivate() - Command paused");
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "  this = %p", this);
    NewBomLog("ABOUT", "  Note: This should not happen for modal dialog commands");
    
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// Cancel - COMMAND ABORT
//=============================================================================
// Called if command is cancelled before dialog is shown.
//
CATStatusChangeRC NewBomAboutCmd::Cancel(CATCommand* iCmd,
                                                 CATNotification* iNotif)
{
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "Cancel() - Command cancelled");
    NewBomLog("ABOUT", "===========================================");
    NewBomLog("ABOUT", "  this = %p", this);
    NewBomLog("ABOUT", "  Note: Command was cancelled before completion");
    
    return CATStatusChangeRCCompleted;
}