// NewBomAssemblyTreeDlg.cpp
// CATIA CAA Assembly Tree Dialog Implementation
//
// This dialog displays the assembly tree structure using CATDlgTree control.

#include "NewBomAssemblyTreeDlg.h"
#include "NewBomLog.h"
#include "CATDlgGridConstraints.h"
#include "CATUnicodeString.h"
#include "CATFrmEditor.h"
#include "CATIDocument.h"
#include "CATIProduct.h"
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
    
    // Set dialog title: UCS-2 for Chinese "获取装配树"
    // 获=0x83B7, 取=0x53D6, 装=0x88F6, 配=0x914D, 树=0x6811
    SetTitle(U(L"\x83B7\x53D6\x88F6\x914D\x6811"));

    // Row 0: Tree control label
    CATDlgLabel* pTreeLabel = new CATDlgLabel(this, "TreeLabel");
    pTreeLabel->SetTitle(U(L"\x83C5\x914D\x6811\x7ED3\x6784"));  // "装配树结构"
    pTreeLabel->SetGridConstraints(0, 0, 1, 1, CATGRID_LEFT);

    // Row 1: Tree control (main content)
    // CATDlgTree displays hierarchical structure
    m_pAssemblyTree = new CATDlgTree(this, "AssemblyTree", CATDlgTreeMultiSelect);
    m_pAssemblyTree->SetGridConstraints(1, 0, 1, 2, CATGRID_LEFT | CATGRID_TOP);
    // Set tree size
    m_pAssemblyTree->SetVisibleRowCount(15);
    m_pAssemblyTree->SetVisibleColumnWidth(40);

    // Row 2: Status label
    m_pStatusLabel = new CATDlgLabel(this, "StatusLabel");
    m_pStatusLabel->SetTitle(U(L"\x51C6\x5907\x83B7\x53D6\x83C5\x914D\x6811..."));  // "准备获取装配树..."
    m_pStatusLabel->SetGridConstraints(2, 0, 1, 2, CATGRID_CENTER);

    // Set column resize properties
    SetGridColumnResizable(0, 0);
    SetGridColumnResizable(1, 0);

    NewBomLog("ASSEMBLY_DLG", "BuildDialog completed tree=%p status=%p",
              m_pAssemblyTree, m_pStatusLabel);

    // Now retrieve and populate assembly structure
    GetAssemblyStructure();
}

//=============================================================================
// GetAssemblyStructure - Retrieve assembly hierarchy
//=============================================================================
void NewBomAssemblyTreeDlg::GetAssemblyStructure()
{
    NewBomLog("ASSEMBLY_DLG", "===========================================");
    NewBomLog("ASSEMBLY_DLG", "GetAssemblyStructure START");
    NewBomLog("ASSEMBLY_DLG", "===========================================");

    // Step 1: Get current editor
    CATFrmEditor* pEditor = CATFrmEditor::GetCurrentEditor();
    if (!pEditor)
    {
        NewBomLog("ASSEMBLY_DLG", "  ERROR: Cannot get current editor");
        if (m_pStatusLabel)
        {
            // "无法获取当前编辑器"
            m_pStatusLabel->SetTitle(U(L"\x65E0\x6CD5\x83B7\x53D6\x5F53\x524D\x7F16\x8F91\x5668"));
        }
        return;
    }
    NewBomLog("ASSEMBLY_DLG", "  pEditor = %p", pEditor);

    // Step 2: Get current document
    CATIDocument_var spDoc = pEditor->GetDocument();
    if (spDoc == NULL_var)
    {
        NewBomLog("ASSEMBLY_DLG", "  ERROR: No document in editor");
        if (m_pStatusLabel)
        {
            // "当前没有打开文档"
            m_pStatusLabel->SetTitle(U(L"\x5F53\x524D\x6CA1\x6709\x6253\x5F00\x6587\x6863"));
        }
        return;
    }
    NewBomLog("ASSEMBLY_DLG", "  Document obtained");

    // Step 3: Get document name for display
    CATUnicodeString docName;
    spDoc->GetDisplayName(docName);
    NewBomLog("ASSEMBLY_DLG", "  Document name: %s", docName.ConvertToChar());

    // Step 4: Query for CATIProduct interface
    CATIProduct_var spProduct = spDoc;
    if (spProduct == NULL_var)
    {
        NewBomLog("ASSEMBLY_DLG", "  Document is not a Product/Part");
        if (m_pStatusLabel)
        {
            // "当前文档不是装配或零件"
            m_pStatusLabel->SetTitle(U(L"\x5F53\x524D\x6587\x6863\x4E0D\x662F\x83C5\x914D\x6216\x96F6\x4EF6"));
        }
        return;
    }
    NewBomLog("ASSEMBLY_DLG", "  CATIProduct interface obtained");

    // Step 5: Populate tree with product structure
    PopulateTree();

    // Update status
    if (m_pStatusLabel)
    {
        // "装配树已获取"
        m_pStatusLabel->SetTitle(U(L"\x83C5\x914D\x6811\x5DF2\x83B7\x53D6"));
    }

    NewBomLog("ASSEMBLY_DLG", "GetAssemblyStructure END");
}

//=============================================================================
// PopulateTree - Fill tree control with assembly nodes
//=============================================================================
void NewBomAssemblyTreeDlg::PopulateTree()
{
    NewBomLog("ASSEMBLY_DLG", "===========================================");
    NewBomLog("ASSEMBLY_DLG", "PopulateTree START");
    NewBomLog("ASSEMBLY_DLG", "===========================================");

    if (!m_pAssemblyTree)
    {
        NewBomLog("ASSEMBLY_DLG", "  ERROR: Tree control is NULL");
        return;
    }

    // Clear existing items
    m_pAssemblyTree->DeleteAllItems();
    NewBomLog("ASSEMBLY_DLG", "  Tree cleared");

    // Get current editor and document
    CATFrmEditor* pEditor = CATFrmEditor::GetCurrentEditor();
    if (!pEditor) return;

    CATIDocument_var spDoc = pEditor->GetDocument();
    if (spDoc == NULL_var) return;

    // Get root product name
    CATUnicodeString rootName;
    spDoc->GetDisplayName(rootName);
    
    // Simplify name: extract just the filename without path
    const char* nameStr = rootName.ConvertToChar();
    const char* lastSlash = strrchr(nameStr, '\\');
    if (lastSlash)
        rootName = CATUnicodeString(lastSlash + 1);
    
    // Remove extension if present
    int dotPos = rootName.Find(".");
    if (dotPos > 0)
    {
        CATUnicodeString temp;
        for (int i = 0; i < dotPos; i++)
        {
            temp.Append(rootName[i]);
        }
        rootName = temp;
    }
    
    NewBomLog("ASSEMBLY_DLG", "  Root name: %s", rootName.ConvertToChar());

    // Add root item to tree
    // CATDlgTree uses integer indices for items
    int rootItem = m_pAssemblyTree->AddItem(rootName, -1);  // -1 means root level
    NewBomLog("ASSEMBLY_DLG", "  Root item added: index=%d", rootItem);

    // Query for CATIProduct to get children
    CATIProduct_var spProduct = spDoc;
    if (spProduct == NULL_var)
    {
        NewBomLog("ASSEMBLY_DLG", "  No CATIProduct, showing single item");
        return;
    }

    // Get children products
    CATLISTP(CATIProduct) childrenList;
    spProduct->GetChildren(childrenList);
    
    int numChildren = childrenList.Size();
    NewBomLog("ASSEMBLY_DLG", "  Number of children: %d", numChildren);

    // Recursively add children
    for (int i = 1; i <= numChildren; i++)
    {
        CATIProduct_var spChild = childrenList[i];
        if (spChild != NULL_var)
        {
            // Get child product name
            CATUnicodeString childName;
            spChild->GetDisplayName(childName);
            
            // Simplify name
            const char* childNameStr = childName.ConvertToChar();
            const char* lastChildSlash = strrchr(childNameStr, '\\');
            if (lastChildSlash)
                childName = CATUnicodeString(lastChildSlash + 1);
            
            // Remove extension
            int childDotPos = childName.Find(".");
            if (childDotPos > 0)
            {
                CATUnicodeString temp;
                for (int j = 0; j < childDotPos; j++)
                {
                    temp.Append(childName[j]);
                }
                childName = temp;
            }
            
            NewBomLog("ASSEMBLY_DLG", "    Child[%d]: %s", i, childName.ConvertToChar());
            
            // Add child item under root
            int childItem = m_pAssemblyTree->AddItem(childName, rootItem);
            NewBomLog("ASSEMBLY_DLG", "    Child item added: index=%d parent=%d", childItem, rootItem);
            
            // Recursively get children of this child (deeper levels)
            CATLISTP(CATIProduct) grandChildrenList;
            spChild->GetChildren(grandChildrenList);
            
            int numGrandChildren = grandChildrenList.Size();
            NewBomLog("ASSEMBLY_DLG", "    Grandchildren count: %d", numGrandChildren);
            
            for (int j = 1; j <= numGrandChildren; j++)
            {
                CATIProduct_var spGrandChild = grandChildrenList[j];
                if (spGrandChild != NULL_var)
                {
                    CATUnicodeString grandChildName;
                    spGrandChild->GetDisplayName(grandChildName);
                    
                    // Simplify name
                    const char* gcNameStr = grandChildName.ConvertToChar();
                    const char* lastGCSlash = strrchr(gcNameStr, '\\');
                    if (lastGCSlash)
                        grandChildName = CATUnicodeString(lastGCSlash + 1);
                    
                    int gcDotPos = grandChildName.Find(".");
                    if (gcDotPos > 0)
                    {
                        CATUnicodeString temp;
                        for (int k = 0; k < gcDotPos; k++)
                        {
                            temp.Append(grandChildName[k]);
                        }
                        grandChildName = temp;
                    }
                    
                    NewBomLog("ASSEMBLY_DLG", "      GrandChild[%d]: %s", j, grandChildName.ConvertToChar());
                    
                    int grandChildItem = m_pAssemblyTree->AddItem(grandChildName, childItem);
                    NewBomLog("ASSEMBLY_DLG", "      GrandChild item added: index=%d parent=%d", grandChildItem, childItem);
                }
            }
        }
    }

    // Expand root item to show tree
    m_pAssemblyTree->ExpandItem(rootItem);
    NewBomLog("ASSEMBLY_DLG", "  Root item expanded");

    NewBomLog("ASSEMBLY_DLG", "PopulateTree END");
}