// PartNumberMenuCmd.cpp
// CATIA CAA Hello World Command Implementation

#include "PartNumberMenuCmd.h"
#include "PartNumberMenuDlg.h"
#include "PartNumberMenuLog.h"
#include "CATApplicationFrame.h"
#include "CATCreateExternalObject.h"

// Implement the class
CATImplementClass(PartNumberMenuCmd,
                  Implementation,
                  CATCommand,
                  CATNull);

// Command headers instantiate commands by class name through this factory.
CATCreateClass(PartNumberMenuCmd);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
PartNumberMenuCmd::PartNumberMenuCmd()
    : CATCommand(NULL, "PartNumberMenuCmd")
{
    m_pDialog = NULL;
    PartNumberMenuLog("COMMAND", "constructor this=%p", this);

    // Let the command selector activate this command without replacing the
    // current CATIA workbench command. Undefined commands are ignored by it.
    RequestStatusChange(CATCommandMsgRequestSharedMode);
    PartNumberMenuLog("COMMAND", "shared mode requested this=%p", this);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
PartNumberMenuCmd::~PartNumberMenuCmd()
{
    PartNumberMenuLog("COMMAND", "destructor this=%p dialog=%p", this, m_pDialog);
    if (m_pDialog)
    {
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }
}

//-----------------------------------------------------------------------------
// Activate - Called when the command is activated (toolbar button clicked)
//-----------------------------------------------------------------------------
CATStatusChangeRC PartNumberMenuCmd::Activate(CATCommand* iCmd,
                                          CATNotification* iNotif)
{
    PartNumberMenuLog("COMMAND", "Activate entered this=%p", this);
    CATApplicationFrame* pFrame = CATApplicationFrame::GetFrame();
    if (!pFrame)
    {
        PartNumberMenuLog("COMMAND", "Activate stopped: application frame is NULL");
        return CATStatusChangeRCCompleted;
    }

    CATDialog* pWindow = pFrame->GetMainWindow();
    if (!pWindow)
    {
        PartNumberMenuLog("COMMAND", "Activate stopped: main window is NULL");
        return CATStatusChangeRCCompleted;
    }

    // Destroy previous dialog if exists
    if (m_pDialog)
    {
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }

    // Create and display the dialog
    m_pDialog = new PartNumberMenuDlg(pWindow, "HelloWorldDialog");
    if (m_pDialog)
    {
        // Shared commands must receive and handle the dialog notifications.
        m_pDialog->SetFather(this);
        AddAnalyseNotificationCB(m_pDialog,
            m_pDialog->GetDiaCLOSENotification(),
            (CATCommandMethod)&PartNumberMenuCmd::CloseDialog,
            NULL);
        AddAnalyseNotificationCB(m_pDialog,
            m_pDialog->GetWindCloseNotification(),
            (CATCommandMethod)&PartNumberMenuCmd::CloseDialog,
            NULL);
        m_pDialog->SetVisibility(CATDlgShow);
        PartNumberMenuLog("COMMAND", "dialog displayed dialog=%p", m_pDialog);
    }
    else
    {
        PartNumberMenuLog("COMMAND", "dialog allocation returned NULL");
    }

    return CATStatusChangeRCCompleted;
}

void PartNumberMenuCmd::CloseDialog(CATCommand* iCmd,
                                CATNotification* iNotif,
                                CATCommandClientData iClientData)
{
    PartNumberMenuLog("COMMAND", "CloseDialog this=%p dialog=%p", this, m_pDialog);
    if (m_pDialog)
        m_pDialog->SetVisibility(CATDlgHide);

    // The command destructor owns the final dialog destruction.
    RequestDelayedDestruction();
}

//-----------------------------------------------------------------------------
// Desactivate - Called when the command is deactivated
//-----------------------------------------------------------------------------
CATStatusChangeRC PartNumberMenuCmd::Desactivate(CATCommand* iCmd,
                                             CATNotification* iNotif)
{
    PartNumberMenuLog("COMMAND", "Desactivate this=%p", this);
    return CATStatusChangeRCCompleted;
}

//-----------------------------------------------------------------------------
// Cancel - Called when the command is canceled
//-----------------------------------------------------------------------------
CATStatusChangeRC PartNumberMenuCmd::Cancel(CATCommand* iCmd,
                                        CATNotification* iNotif)
{
    PartNumberMenuLog("COMMAND", "Cancel this=%p dialog=%p", this, m_pDialog);
    if (m_pDialog)
    {
        m_pDialog->RequestDelayedDestruction();
        m_pDialog = NULL;
    }

    return CATStatusChangeRCCompleted;
}
