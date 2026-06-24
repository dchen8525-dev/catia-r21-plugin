// NewBomCmd.h
// CATIA CAA Hello World Command

#ifndef __NewBomCmd_h__
#define __NewBomCmd_h__

#include "CATCommand.h"

class NewBomDlg;

/**
 * @brief Hello World Command
 * 
 * This command is activated when the user clicks the toolbar button.
 * It creates and displays a Hello World dialog.
 */
class NewBomCmd : public CATCommand
{
    CATDeclareClass;

public:
    NewBomCmd();
    virtual ~NewBomCmd();

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
    NewBomDlg* m_pDialog;
};

#endif // __NewBomCmd_h__
