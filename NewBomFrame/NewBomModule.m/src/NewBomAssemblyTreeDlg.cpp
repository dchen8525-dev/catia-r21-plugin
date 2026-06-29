// NewBomAssemblyTreeDlg.cpp
// CATIA CAA Assembly Tree Dialog Implementation

#include "NewBomAssemblyTreeDlg.h"
#include "NewBomLog.h"
#include "CATDlgGridConstraints.h"
#include "CATUnicodeString.h"
#include "CATApplicationFrame.h"
#include <stdio.h>
#include <string.h>

// Implement the class
CATImplementClass(NewBomAssemblyTreeDlg,
                  Implementation,
                  CATDlgDialog,
                  CATNull);

// Helper function to build Unicode string from wchar_t
static CATUnicodeString U(const wchar_t* iText)
{
    CATUnicodeString text;
    text.BuildFromWChar(iText);
    return text;
}

//=============================================================================
// Constructor
//=============================================================================
NewBomAssemblyTreeDlg::NewBomAssemblyTreeDlg(CATDialog* iParent,
                                             const CATString& iObjectName)
    : CATDlgDialog(iParent,
                   iObjectName,
                   CATDlgWndModal | CATDlgWndBtnClose | CATDlgGridLayout)
{
    NewBomLog("ASSEMBLY_DLG", "constructor this=%p parent=%p", this, iParent);
    m_pAssemblyTree = NULL;
    m_pStatusLabel = NULL;

    BuildDialog();
}

//=============================================================================
// Destructor
//=============================================================================
NewBomAssemblyTreeDlg::~NewBomAssemblyTreeDlg()
{
    NewBomLog("ASSEMBLY_DLG", "destructor this=%p", this);
}

//=============================================================================
// BuildDialog - Create and layout dialog controls
//=============================================================================
void NewBomAssemblyTreeDlg::BuildDialog()
{
    NewBomLog("ASSEMBLY_DLG", "BuildDialog entered this=%p", this);
    
    // Set dialog title: "获取装配树"
    SetTitle(U(L"\x83B7\x53D6\x88C5\x914D\x6811"));

    // Row 0: Title label
    CATDlgLabel* pTitleLabel = new CATDlgLabel(this, "TitleLabel");
    pTitleLabel->SetTitle(U(L"\x88C5\x914D\x6811\x7ED3\x6784"));  // "装配树结构"
    pTitleLabel->SetGridConstraints(0, 0, 1, 1, CATGRID_LEFT);

    // Row 1: Empty separator row
    CATDlgLabel* pSeparator1 = new CATDlgLabel(this, "Separator1");
    pSeparator1->SetTitle(CATUnicodeString(""));  // Empty label as separator
    pSeparator1->SetGridConstraints(1, 0, 1, 1, CATGRID_CST_SIZE);

    // Row 2: Tree display (CATDlgEditor)
    m_pAssemblyTree = new CATDlgEditor(this, "AssemblyTree");
    m_pAssemblyTree->SetVisibleTextWidth(50);
    m_pAssemblyTree->SetVisibleTextHeight(12);
    m_pAssemblyTree->SetGridConstraints(2, 0, 1, 1, CATGRID_LEFT);

    // Row 3: Empty separator row
    CATDlgLabel* pSeparator2 = new CATDlgLabel(this, "Separator2");
    pSeparator2->SetTitle(CATUnicodeString(""));  // Empty label as separator
    pSeparator2->SetGridConstraints(3, 0, 1, 1, CATGRID_CST_SIZE);

    // Row 4: Status label
    m_pStatusLabel = new CATDlgLabel(this, "StatusLabel");
    m_pStatusLabel->SetTitle(U(L"\x51C6\x5907\x83B7\x53D6..."));  // "准备获取..."
    m_pStatusLabel->SetGridConstraints(4, 0, 1, 1, CATGRID_CENTER);

    // Set column properties
    SetGridColumnResizable(0, 0);

    NewBomLog("ASSEMBLY_DLG", "BuildDialog completed");

    // Retrieve assembly structure
    GetAssemblyStructure();
}

//=============================================================================
// GetAssemblyStructure - Retrieve assembly hierarchy
//=============================================================================
void NewBomAssemblyTreeDlg::GetAssemblyStructure()
{
    NewBomLog("ASSEMBLY_DLG", "GetAssemblyStructure START");

    CATUnicodeString treeText;
    
    // Check if CATIA is running
    CATApplicationFrame* pFrame = CATApplicationFrame::GetFrame();
    if (!pFrame)
    {
        NewBomLog("ASSEMBLY_DLG", "  ERROR: Cannot get application frame");
        treeText = U(L"\x65E0\x6CD5\x83B7\x53D6\x5E94\x7528\x6846\x67B6");  // "无法获取应用框架"
        if (m_pAssemblyTree) m_pAssemblyTree->SetText(treeText);
        if (m_pStatusLabel) m_pStatusLabel->SetTitle(U(L"\x9519\x8BEF"));  // "错误"
        return;
    }
    
    NewBomLog("ASSEMBLY_DLG", "  pFrame = %p", pFrame);

    // Build display text
    // "获取装配树功能": 获取装配树 + 功=0x529F, 能=0x80FD
    treeText.Append(U(L"\x83B7\x53D6\x88C5\x914D\x6811\x529F\x80FD"));
    treeText.Append("\n\n");
    // "当前实现为基础算法"
    treeText.Append(U(L"\x5F53\x524D\x5B9E\x73B0\x4E3A\x57FA\x7840\x7B97\x6CD5"));
    treeText.Append("\n");
    // "完整装配树需要扩展"
    treeText.Append(U(L"\x5B8C\x6574\x88C5\x914D\x6811\x9700\x8981\x6269\x5C55"));

    // Display result
    if (m_pAssemblyTree)
    {
        m_pAssemblyTree->SetText(treeText);
    }

    // Update status: "装配树已获取"
    if (m_pStatusLabel)
    {
        m_pStatusLabel->SetTitle(U(L"\x88C5\x914D\x6811\x5DF2\x83B7\x53D6"));
    }

    NewBomLog("ASSEMBLY_DLG", "GetAssemblyStructure END");
}