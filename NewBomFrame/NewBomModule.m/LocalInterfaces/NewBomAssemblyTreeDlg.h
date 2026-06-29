// =============================================================================
// NewBomAssemblyTreeDlg.h
// CATIA CAA Assembly Tree Dialog Header File
// =============================================================================
//
// This dialog displays the assembly tree structure using CATDlgEditor control.
// Text format with indentation shows hierarchical structure.
//
// =============================================================================

#ifndef __NewBomAssemblyTreeDlg_h__
#define __NewBomAssemblyTreeDlg_h__

#include "CATDlgDialog.h"
#include "CATDlgEditor.h"
#include "CATDlgLabel.h"
#include "CATUnicodeString.h"

/**
 * @class NewBomAssemblyTreeDlg
 * @brief Dialog for displaying assembly tree structure
 *
 * Uses CATDlgEditor to show hierarchical product/part structure as text.
 */
class NewBomAssemblyTreeDlg : public CATDlgDialog
{
    CATDeclareClass;

private:
    CATDlgEditor* m_pAssemblyTree;    // Text editor for hierarchy display
    CATDlgLabel* m_pStatusLabel;      // Status information label

public:
    NewBomAssemblyTreeDlg(CATDialog* iParent, const CATString& iObjectName);
    virtual ~NewBomAssemblyTreeDlg();

    /**
     * BuildDialog - Create and layout dialog controls
     */
    void BuildDialog();

private:
    /**
     * GetAssemblyStructure - Retrieve assembly hierarchy from current document
     */
    void GetAssemblyStructure();
};

#endif // __NewBomAssemblyTreeDlg_h__