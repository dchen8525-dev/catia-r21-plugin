// PartNumberMenuAboutCmd.cpp
// CATIA CAA "About" Command Implementation

#include "PartNumberMenuAboutCmd.h"
#include "PartNumberMenuLog.h"
#include "CATApplicationFrame.h"
#include "CATDlgNotify.h"
#include "CATCreateExternalObject.h"
#include "CATUnicodeString.h"

// Implement the class
CATImplementClass(PartNumberMenuAboutCmd,
                  Implementation,
                  CATCommand,
                  CATNull);

// Command headers instantiate commands by class name through this factory.
CATCreateClass(PartNumberMenuAboutCmd);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
PartNumberMenuAboutCmd::PartNumberMenuAboutCmd()
    : CATCommand(NULL, "PartNumberMenuAboutCmd")
{
    PartNumberMenuLog("ABOUT", "constructor this=%p", this);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
PartNumberMenuAboutCmd::~PartNumberMenuAboutCmd()
{
    PartNumberMenuLog("ABOUT", "destructor this=%p", this);
}

//-----------------------------------------------------------------------------
// Activate - pop up the about box then self-destruct
//-----------------------------------------------------------------------------
CATStatusChangeRC PartNumberMenuAboutCmd::Activate(CATCommand* iCmd,
                                                   CATNotification* iNotif)
{
    PartNumberMenuLog("ABOUT", "Activate entered this=%p", this);

    CATApplicationFrame* pFrame = CATApplicationFrame::GetFrame();
    if (!pFrame)
    {
        PartNumberMenuLog("ABOUT", "Activate stopped: application frame is NULL");
        RequestDelayedDestruction();
        return CATStatusChangeRCCompleted;
    }

    CATDialog* pParent = pFrame->GetMainWindow();
    if (!pParent)
    {
        PartNumberMenuLog("ABOUT", "Activate stopped: main window is NULL");
        RequestDelayedDestruction();
        return CATStatusChangeRCCompleted;
    }

    // "件取号插件 v1.0 / CATIA V5R21 CAA"
    // 件=\x4EF6 取=\x53D6 号=\x53F7 插=\x63D2 件=\x4EF6
    CATUnicodeString message;
    message.BuildFromWChar(L"\x4EF6\x53D6\x53F7\x63D2\x4EF6 v1.0 / CATIA V5R21 CAA");

    // "关于本插件" : 关=\x5173 于=\x4E8E 本=\x672C 插=\x63D2 件=\x4EF6
    CATUnicodeString title;
    title.BuildFromWChar(L"\x5173\x4E8E\x672C\x63D2\x4EF6");

    // CATDlgNotify is the V5 "information modal dialog" base class.
    // It belongs to the Dialog framework (lib DI0PANV2, already LINK_WITH).
    CATDlgNotify* pNotify = new CATDlgNotify(
        pParent,
        "PartNumberMenuAboutBox",
        CATDlgNfyInformation | CATDlgNfyOK | CATDlgWndModal);
    if (pNotify)
    {
        // DisplayBlocked shows the modal box and returns when the user
        // dismisses it (OK button here). Returns the button code (1 = OK).
        int button = pNotify->DisplayBlocked(message, title);
        PartNumberMenuLog("ABOUT", "DisplayBlocked returned button=%d this=%p",
                          button, this);
        pNotify->RequestDelayedDestruction();
    }
    else
    {
        PartNumberMenuLog("ABOUT", "CATDlgNotify allocation returned NULL");
    }

    // Self-destruct: this command is one-shot, no persistent state.
    RequestDelayedDestruction();
    return CATStatusChangeRCCompleted;
}

//-----------------------------------------------------------------------------
// Desactivate
//-----------------------------------------------------------------------------
CATStatusChangeRC PartNumberMenuAboutCmd::Desactivate(CATCommand* iCmd,
                                                      CATNotification* iNotif)
{
    PartNumberMenuLog("ABOUT", "Desactivate this=%p", this);
    return CATStatusChangeRCCompleted;
}

//-----------------------------------------------------------------------------
// Cancel
//-----------------------------------------------------------------------------
CATStatusChangeRC PartNumberMenuAboutCmd::Cancel(CATCommand* iCmd,
                                                 CATNotification* iNotif)
{
    PartNumberMenuLog("ABOUT", "Cancel this=%p", this);
    return CATStatusChangeRCCompleted;
}
