// PartNumberMenuDlg.h
// CATIA CAA Hello World Dialog

#ifndef __PartNumberMenuDlg_h__
#define __PartNumberMenuDlg_h__

#include "CATDlgDialog.h"
#include "CATDlgLabel.h"
#include "CATDlgEditor.h"
#include "CATDlgCombo.h"
#include "CATDlgPushButton.h"

/**
 * @brief Hello World Dialog
 * 
 * A simple dialog that displays "Hello World!" message
 */
class PartNumberMenuDlg : public CATDlgDialog
{
    CATDeclareClass;

public:
    PartNumberMenuDlg(CATDialog* iParent, 
                  const CATString& iObjectName = "PartNumberMenuDlg");
    virtual ~PartNumberMenuDlg();

    void BuildDialog();

private:
    void OnGetNumber(CATCommand* iCmd,
                     CATNotification* iNotif,
                     CATCommandClientData iClientData);

    CATDlgEditor*     m_pPartName;
    CATDlgCombo*      m_pCategory;
    CATDlgEditor*     m_pMaterial;
    CATDlgEditor*     m_pNotes;
    CATDlgLabel*      m_pNumberResult;
    CATDlgPushButton* m_pNumberButton;
    CATDlgLabel*      m_pStatusLabel;
};

#endif // __PartNumberMenuDlg_h__
