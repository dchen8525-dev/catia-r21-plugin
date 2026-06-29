// NewBomAssemblyTreeCmd.cpp
// CATIA CAA Assembly Tree Command Implementation
//
// This command creates and displays the assembly tree dialog.
// Follows the same pattern as NewBomCmd.

#include "NewBomAssemblyTreeCmd.h"
#include "NewBomAssemblyTreeDlg.h"
#include "NewBomLog.h"
#include "CATApplicationFrame.h"
#include "CATCreateExternalObject.h"

//=============================================================================
// CLASS REGISTRATION
//=============================================================================
CATImplementClass(NewBomAssemblyTreeCmd,
                  Implementation,
                  CATCommand,
                  CATNull);

CATCreateClass(NewBomAssemblyTreeCmd);

//=============================================================================
// CONSTRUCTOR
//=============================================================================
NewBomAssemblyTreeCmd::NewBomAssemblyTreeCmd()
    : CATCommand(NULL, "NewBomAssemblyTreeCmd")
{
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "NewBomAssemblyTreeCmd CONSTRUCTOR START");
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "  this pointer = %p", this);
    
    m_pDialog = NULL;
    
    NewBomLog("ASSEMBLY_CMD", "  Calling RequestStatusChange(CATCommandMsgRequestSharedMode)");
    RequestStatusChange(CATCommandMsgRequestSharedMode);
    
    NewBomLog("ASSEMBLY_CMD", "CONSTRUCTOR END");
}

//=============================================================================
// DESTRUCTOR
//=============================================================================
NewBomAssemblyTreeCmd::~NewBomAssemblyTreeCmd()
{
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "NewBomAssemblyTreeCmd DESTRUCTOR START");
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "  this pointer = %p", this);
    NewBomLog("ASSEMBLY_CMD", "  m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        NewBomLog("ASSEMBLY_CMD", "  Dialog exists, calling RequestDelayedDestruction()");
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }
    
    NewBomLog("ASSEMBLY_CMD", "DESTRUCTOR END");
}

//=============================================================================
// Activate - COMMAND START
//=============================================================================
CATStatusChangeRC NewBomAssemblyTreeCmd::Activate(CATCommand* iCmd,
                                                   CATNotification* iNotif)
{
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "Activate() START - User clicked menu item!");
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "  this = %p", this);

    // Step 1: Get CATIA Application Frame
    CATApplicationFrame* pFrame = CATApplicationFrame::GetFrame();
    if (!pFrame)
    {
        NewBomLog("ASSEMBLY_CMD", "  ERROR: Application frame is NULL!");
        return CATStatusChangeRCCompleted;
    }
    NewBomLog("ASSEMBLY_CMD", "  pFrame = %p", pFrame);

    // Step 2: Get Main Window
    CATDialog* pWindow = pFrame->GetMainWindow();
    if (!pWindow)
    {
        NewBomLog("ASSEMBLY_CMD", "  ERROR: Main window is NULL!");
        return CATStatusChangeRCCompleted;
    }
    NewBomLog("ASSEMBLY_CMD", "  pWindow = %p", pWindow);

    // Step 3: Clean up any existing dialog
    if (m_pDialog)
    {
        NewBomLog("ASSEMBLY_CMD", "  Destroying existing dialog");
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }

    // Step 4: Create New Dialog
    NewBomLog("ASSEMBLY_CMD", "  Creating NewBomAssemblyTreeDlg...");
    m_pDialog = new NewBomAssemblyTreeDlg(pWindow, "AssemblyTreeDialog");
    
    if (!m_pDialog)
    {
        NewBomLog("ASSEMBLY_CMD", "  ERROR: Dialog creation failed!");
        return CATStatusChangeRCCompleted;
    }
    NewBomLog("ASSEMBLY_CMD", "  m_pDialog = %p", m_pDialog);

    // Step 5: Connect Dialog Events
    m_pDialog->SetFather(this);
    
    AddAnalyseNotificationCB(m_pDialog,
        m_pDialog->GetDiaCLOSENotification(),
        (CATCommandMethod)&NewBomAssemblyTreeCmd::CloseDialog,
        NULL);
    
    AddAnalyseNotificationCB(m_pDialog,
        m_pDialog->GetWindCloseNotification(),
        (CATCommandMethod)&NewBomAssemblyTreeCmd::CloseDialog,
        NULL);

    // Step 6: Show Dialog
    NewBomLog("ASSEMBLY_CMD", "  Showing dialog...");
    m_pDialog->SetVisibility(CATDlgShow);

    NewBomLog("ASSEMBLY_CMD", "Activate() END");
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// CloseDialog - HANDLE DIALOG CLOSE
//=============================================================================
void NewBomAssemblyTreeCmd::CloseDialog(CATCommand* iCmd,
                                        CATNotification* iNotif,
                                        CATCommandClientData iClientData)
{
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "CloseDialog() START");
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "  this = %p", this);
    NewBomLog("ASSEMBLY_CMD", "  m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        NewBomLog("ASSEMBLY_CMD", "  Hiding dialog");
        m_pDialog->SetVisibility(CATDlgHide);
    }
    
    NewBomLog("ASSEMBLY_CMD", "  Calling RequestDelayedDestruction()");
    RequestDelayedDestruction();
    
    NewBomLog("ASSEMBLY_CMD", "CloseDialog() END");
}

//=============================================================================
// Desactivate - COMMAND PAUSE
//=============================================================================
CATStatusChangeRC NewBomAssemblyTreeCmd::Desactivate(CATCommand* iCmd,
                                                      CATNotification* iNotif)
{
    NewBomLog("ASSEMBLY_CMD", "Desactivate() called");
    return CATStatusChangeRCCompleted;
}

//=============================================================================
// Cancel - COMMAND ABORT
//=============================================================================
CATStatusChangeRC NewBomAssemblyTreeCmd::Cancel(CATCommand* iCmd,
                                                 CATNotification* iNotif)
{
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "Cancel() START");
    NewBomLog("ASSEMBLY_CMD", "===========================================");
    NewBomLog("ASSEMBLY_CMD", "  m_pDialog = %p", m_pDialog);
    
    if (m_pDialog)
    {
        NewBomLog("ASSEMBLY_CMD", "  Destroying dialog");
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }
    
    NewBomLog("ASSEMBLY_CMD", "Cancel() END");
    return CATStatusChangeRCCompleted;
}