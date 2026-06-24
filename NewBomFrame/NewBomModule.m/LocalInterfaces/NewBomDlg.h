// NewBomDlg.h
// CATIA CAA Hello World Dialog

#ifndef __NewBomDlg_h__
#define __NewBomDlg_h__

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
class NewBomDlg : public CATDlgDialog
{
    CATDeclareClass;

public:
    NewBomDlg(CATDialog* iParent, 
                  const CATString& iObjectName = "NewBomDlg");
    virtual ~NewBomDlg();

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

#endif // __NewBomDlg_h__
