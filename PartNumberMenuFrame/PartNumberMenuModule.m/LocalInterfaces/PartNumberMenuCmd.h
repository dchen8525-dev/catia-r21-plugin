// PartNumberMenuCmd.h
// CATIA CAA Hello World Command

#ifndef __PartNumberMenuCmd_h__
#define __PartNumberMenuCmd_h__

#include "CATCommand.h"

class PartNumberMenuDlg;

/**
 * @brief Hello World Command
 * 
 * This command is activated when the user clicks the toolbar button.
 * It creates and displays a Hello World dialog.
 */
class PartNumberMenuCmd : public CATCommand
{
    CATDeclareClass;

public:
    PartNumberMenuCmd();
    virtual ~PartNumberMenuCmd();

    // CATCommand overrides
    virtual CATStatusChangeRC Activate(CATCommand* iCmd,
                                       CATNotification* iNotif);
    virtual CATStatusChangeRC Desactivate(CATCommand* iCmd,
                                          CATNotification* iNotif);
    virtual CATStatusChangeRC Cancel(CATCommand* iCmd,
                                     CATNotification* iNotif);

private:
    void CloseDialog(CATCommand* iCmd,
                     CATNotification* iNotif,
                     CATCommandClientData iClientData);

    // Dialog instance
    PartNumberMenuDlg* m_pDialog;
};

#endif // __PartNumberMenuCmd_h__
