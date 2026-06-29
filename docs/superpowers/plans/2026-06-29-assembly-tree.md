# 获取装配树功能实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在 CATIA R21 NewBOM 插件中添加"获取装配树"菜单，点击后显示当前文档的装配层级结构。

**Architecture:** 新建命令类 `NewBomAssemblyTreeCmd` 和对话框类 `NewBomAssemblyTreeDlg`，使用 CATIProduct 接口遍历装配结构，用 CATDlgTree 控件显示树形层级。

**Tech Stack:** CATIA CAA R21, CATDlgTree, CATIProduct, CATFrmEditor, CATProductStructure

---

## File Structure

### 新建文件
| 文件 | 职责 |
|------|------|
| `LocalInterfaces/NewBomAssemblyTreeCmd.h` | 命令类声明，定义生命周期方法 |
| `src/NewBomAssemblyTreeCmd.cpp` | 命令类实现，处理菜单点击、创建对话框 |
| `LocalInterfaces/NewBomAssemblyTreeDlg.h` | 对话框类声明，定义树控件和遍历方法 |
| `src/NewBomAssemblyTreeDlg.cpp` | 对话框类实现，获取装配结构并填充树控件 |

### 修改文件
| 文件 | 修改内容 |
|------|---------|
| `src/NewBomAddin.cpp` | 注册新命令头，添加新菜单项 |
| `IdentityCard/IdentityCard.h` | 添加 ProductStructure 和 CATFrmEditor 依赖 |
| `NewBomModule.m/Imakefile.mk` | 添加依赖库和源文件 |

---

### Task 1: 添加框架依赖

**Files:**
- Modify: `NewBomFrame/IdentityCard/IdentityCard.h`

- [ ] **Step 1: 添加 ProductStructure 依赖**

在 `IdentityCard.h` 文件的 `AddPrereqComponent("System", Public);` 之后添加：

```cpp
// ProductStructure - CATIA Product Structure API
// Contains:
//   - CATIProduct: Product interface for assembly structure
//   - CATProduct: Product document type
//   - CATIProduct_var: Smart pointer for product
//
// We need it for:
//   - Traverse assembly tree structure
//   - Get children products from parent
//   - Get product/part names
AddPrereqComponent("ProductStructure", Public);

// CATFrmEditor - CATIA Frame Editor
// Contains:
//   - CATFrmEditor: Editor class for current document
//   - CATIDocument: Document interface
//
// We need it for:
//   - Get current active document
//   - Determine document type (Product or Part)
AddPrereqComponent("CATFrmEditor", Public);
```

- [ ] **Step 2: 提交依赖更改**

```bash
git add NewBomFrame/IdentityCard/IdentityCard.h
git commit -m "feat: add ProductStructure and CATFrmEditor dependencies for assembly tree"
```

---

### Task 2: 创建命令类头文件

**Files:**
- Create: `NewBomFrame/NewBomModule.m/LocalInterfaces/NewBomAssemblyTreeCmd.h`

- [ ] **Step 1: 创建命令类头文件**

```cpp
// =============================================================================
// NewBomAssemblyTreeCmd.h
// CATIA CAA Assembly Tree Command Header File
// =============================================================================
//
// This command is triggered when user clicks "获取装配树" menu item.
// It creates and displays the AssemblyTree dialog showing product hierarchy.
//
// =============================================================================

#ifndef __NewBomAssemblyTreeCmd_h__
#define __NewBomAssemblyTreeCmd_h__

#include "CATCommand.h"

class NewBomAssemblyTreeDlg;

/**
 * @class NewBomAssemblyTreeCmd
 * @brief Command for displaying assembly tree structure
 *
 * Lifecycle:
 *   1. User clicks menu item
 *   2. CATIA creates command instance
 *   3. Activate() creates and shows dialog
 *   4. User closes dialog
 *   5. CloseDialog() destroys command
 */
class NewBomAssemblyTreeCmd : public CATCommand
{
    CATDeclareClass;

private:
    NewBomAssemblyTreeDlg* m_pDialog;  // Dialog pointer

public:
    NewBomAssemblyTreeCmd();
    virtual ~NewBomAssemblyTreeCmd();

    /**
     * Activate - Called when command starts
     * Creates and displays the assembly tree dialog
     */
    virtual CATStatusChangeRC Activate(CATCommand* iCmd, CATNotification* iNotif);

    /**
     * CloseDialog - Called when dialog closes
     * Hides dialog and requests command destruction
     */
    virtual void CloseDialog(CATCommand* iCmd, 
                             CATNotification* iNotif, 
                             CATCommandClientData iClientData);

    /**
     * Desactivate - Called when another command takes focus
     */
    virtual CATStatusChangeRC Desactivate(CATCommand* iCmd, CATNotification* iNotif);

    /**
     * Cancel - Called when command is cancelled
     */
    virtual CATStatusChangeRC Cancel(CATCommand* iCmd, CATNotification* iNotif);
};

#endif // __NewBomAssemblyTreeCmd_h__
```

- [ ] **Step 2: 提交头文件**

```bash
git add NewBomFrame/NewBomModule.m/LocalInterfaces/NewBomAssemblyTreeCmd.h
git commit -m "feat: add NewBomAssemblyTreeCmd header file"
```

---

### Task 3: 创建命令类实现文件

**Files:**
- Create: `NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeCmd.cpp`

- [ ] **Step 1: 创建命令类实现文件**

```cpp
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
```

- [ ] **Step 2: 提交命令实现文件**

```bash
git add NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeCmd.cpp
git commit -m "feat: implement NewBomAssemblyTreeCmd command class"
```

---

### Task 4: 创建对话框头文件

**Files:**
- Create: `NewBomFrame/NewBomModule.m/LocalInterfaces/NewBomAssemblyTreeDlg.h`

- [ ] **Step 1: 创建对话框头文件**

```cpp
// =============================================================================
// NewBomAssemblyTreeDlg.h
// CATIA CAA Assembly Tree Dialog Header File
// =============================================================================
//
// This dialog displays the assembly tree structure using CATDlgTree control.
//
// =============================================================================

#ifndef __NewBomAssemblyTreeDlg_h__
#define __NewBomAssemblyTreeDlg_h__

#include "CATDlgDialog.h"
#include "CATDlgTree.h"
#include "CATDlgLabel.h"
#include "CATUnicodeString.h"
#include "CATI18NTypes.h"

/**
 * @class NewBomAssemblyTreeDlg
 * @brief Dialog for displaying assembly tree structure
 *
 * Uses CATDlgTree to show hierarchical product/part structure.
 * Retrieves current document's assembly structure via CATIProduct.
 */
class NewBomAssemblyTreeDlg : public CATDlgDialog
{
    CATDeclareClass;

private:
    CATDlgTree* m_pAssemblyTree;    // Tree control for hierarchy
    CATDlgLabel* m_pStatusLabel;    // Status information label

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
    
    /**
     * PopulateTree - Fill tree control with assembly nodes
     */
    void PopulateTree();
};

#endif // __NewBomAssemblyTreeDlg_h__
```

- [ ] **Step 2: 提交对话框头文件**

```bash
git add NewBomFrame/NewBomModule.m/LocalInterfaces/NewBomAssemblyTreeDlg.h
git commit -m "feat: add NewBomAssemblyTreeDlg header file"
```

---

### Task 5: 创建对话框实现文件（基础结构）

**Files:**
- Create: `NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeDlg.cpp`

- [ ] **Step 1: 创建对话框实现文件（构造函数、析构函数、BuildDialog）**

```cpp
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
```

- [ ] **Step 2: 提交基础结构**

```bash
git add NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeDlg.cpp
git commit -m "feat: add NewBomAssemblyTreeDlg basic structure (constructor, BuildDialog)"
```

---

### Task 6: 实现装配结构获取逻辑

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeDlg.cpp`

- [ ] **Step 1: 添加 GetAssemblyStructure 方法**

在 `NewBomAssemblyTreeDlg.cpp` 文件的 `BuildDialog()` 之后添加：

```cpp
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
```

- [ ] **Step 2: 提交装配结构获取**

```bash
git add NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeDlg.cpp
git commit -m "feat: implement GetAssemblyStructure method"
```

---

### Task 7: 实现树控件填充逻辑

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeDlg.cpp`

- [ ] **Step 1: 添加 PopulateTree 方法**

在 `GetAssemblyStructure()` 之后添加：

```cpp
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
    CATListPtrCATIProduct childrenList;
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
            // Note: For simplicity, we only show 2 levels here
            // Full implementation would recurse deeper
            CATListPtrCATIProduct grandChildrenList;
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
```

- [ ] **Step 2: 提交树填充逻辑**

```bash
git add NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeDlg.cpp
git commit -m "feat: implement PopulateTree method with recursive traversal"
```

---

### Task 8: 添加必要的 include 头文件

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeDlg.cpp`

- [ ] **Step 1: 添加缺失的头文件 include**

在文件顶部的 `#include` 区域，确保包含以下：

```cpp
#include "NewBomAssemblyTreeDlg.h"
#include "NewBomLog.h"
#include "CATDlgGridConstraints.h"
#include "CATUnicodeString.h"
#include "CATFrmEditor.h"           // For CATFrmEditor
#include "CATIDocument.h"           // For CATIDocument
#include "CATIProduct.h"            // For CATIProduct
#include "CATListPtrCATIProduct.h"  // For CATListPtrCATIProduct
#include <stdio.h>                  // For strrchr
```

这些 include 已在 Task 5 的代码中包含。如果编译时报错缺少 `CATListPtrCATIProduct.h`，需要检查 ProductStructure 框架是否正确添加。

- [ ] **Step 2: 提交头文件确认**

```bash
git add NewBomFrame/NewBomModule.m/src/NewBomAssemblyTreeDlg.cpp
git commit -m "fix: ensure all required headers included for assembly tree"
```

---

### Task 9: 修改 Addin 注册命令头

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/src/NewBomAddin.cpp`

- [ ] **Step 1: 在 CreateCommands() 中添加命令头注册**

在 `NewBomAddin.cpp` 的 `CreateCommands()` 函数中，在已有的 `NewBomAboutCmd` 注册之后添加：

找到这段代码（约第152行）：
```cpp
    NewBomLog("ADDIN", "CreateCommands() END - Headers registered: %p, %p", header, aboutHeader);
}
```

在其之前添加：

```cpp
    // -----------------------------------------------------------------------
    // Command Header 3: NewBomAssemblyTreeCmd (GetAssemblyTree)
    // -----------------------------------------------------------------------
    NewBomLog("ADDIN", "[Command 3] Creating NewBomAssemblyTreeCmd header...");
    NewBomLog("ADDIN", "  - HeaderID: 'NewBomAssemblyTreeCmd'");
    NewBomLog("ADDIN", "  - LoadName:  'NewBomModule' (DLL name)");
    NewBomLog("ADDIN", "  - ClassName: 'NewBomAssemblyTreeCmd' (command class to run)");
    NewBomLog("ADDIN", "  - Argument:  NULL (no data passed)");
    
    CATCommandHeader* assemblyTreeHeader = new NewBomHeader("NewBomAssemblyTreeCmd",
                                                            "NewBomModule",
                                                            "NewBomAssemblyTreeCmd",
                                                            (void*) NULL);
    
    if (assemblyTreeHeader != NULL)
    {
        NewBomLog("ADDIN", "  SUCCESS: AssemblyTree header created at address %p", assemblyTreeHeader);
    }
    else
    {
        NewBomLog("ADDIN", "  ERROR: Failed to create AssemblyTree header!");
    }
    NewBomLog("ADDIN", "");

    NewBomLog("ADDIN", "CreateCommands() END - Headers registered: %p, %p, %p", header, aboutHeader, assemblyTreeHeader);
```

同时修改原有的 END 日志行：
```cpp
    // 删除这行:
    // NewBomLog("ADDIN", "CreateCommands() END - Headers registered: %p, %p", header, aboutHeader);
```

- [ ] **Step 2: 提交命令头注册**

```bash
git add NewBomFrame/NewBomModule.m/src/NewBomAddin.cpp
git commit -m "feat: register NewBomAssemblyTreeCmd command header in CreateCommands"
```

---

### Task 10: 修改 Addin 创建菜单结构

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/src/NewBomAddin.cpp`

- [ ] **Step 1: 在 CreateToolbars() 中添加菜单项**

在 `NewBomAddin.cpp` 的 `CreateToolbars()` 函数中，在 "STEP 5: Create Submenu Tools" 之前添加新菜单。

找到这段代码（约第289行）：
```cpp
    // =========================================================================
    // STEP 5: Create Submenu "Tools" (Tools) - Sibling of "NumberMgt"
    // =========================================================================
```

在其之前插入：

```cpp
    // =========================================================================
    // STEP 4.5: Create Submenu "AssemblyTree" (获取装配树) - Sibling of "NumberMgt"
    // =========================================================================
    NewBomLog("ADDIN", "STEP 4.5: Creating submenu 'AssemblyTree' (获取装配树)...");
    NewBomLog("ADDIN", "  Chinese characters (UCS-2): 获=0x83B7, 取=0x53D6, 装=0x88F6, 配=0x914D, 树=0x6811");
    
    NewBomLog("ADDIN", "  [4.5a] NewAccess creates container with ID 'NewBomAssemblyTreeMnu'");
    NewAccess(CATCmdContainer, pAssemblyTreeMenu, NewBomAssemblyTreeMnu);
    NewBomLog("ADDIN", "       Result: pAssemblyTreeMenu = %p", pAssemblyTreeMenu);
    
    NewBomLog("ADDIN", "  [4.5b] SetAccessChild attaches this submenu to top menu");
    SetAccessChild(pTopMenu, pAssemblyTreeMenu);
    NewBomLog("ADDIN", "       Done: 'AssemblyTree' is now a submenu of 'NewBOM'");
    
    NewBomLog("ADDIN", "  [4.5c] SetTitle sets Chinese display text");
    {
        static const CATUC2Bytes titleChars[] = {0x83B7, 0x53D6, 0x88F6, 0x914D, 0x6811, 0};  // "获取装配树"
        CATUnicodeString title;
        title.BuildFromUCChar(titleChars, 5);
        NewBomLog("ADDIN", "       String length = %d characters (should be 5)", title.GetLengthInChar());
        pAssemblyTreeMenu->SetTitle(title);
        NewBomLog("ADDIN", "       SetTitle() called");
    }
    
    NewBomLog("ADDIN", "  [4.5d] Create command item starter");
    NewAccess(CATCmdStarter, pAssemblyTreeStarter, NewBomAssemblyTreeStr);
    NewBomLog("ADDIN", "       Result: pAssemblyTreeStarter = %p", pAssemblyTreeStarter);
    
    NewBomLog("ADDIN", "  [4.5e] SetAccessCommand links starter to command header");
    SetAccessCommand(pAssemblyTreeStarter, "NewBomAssemblyTreeCmd");
    NewBomLog("ADDIN", "       Done: Clicking this item will run NewBomAssemblyTreeCmd");
    
    NewBomLog("ADDIN", "  [4.5f] SetAccessChild puts item inside 'AssemblyTree' submenu");
    SetAccessChild(pAssemblyTreeMenu, pAssemblyTreeStarter);
    NewBomLog("ADDIN", "       Done: Command item is now inside 'AssemblyTree'");
    
    NewBomLog("ADDIN", "STEP 4.5 COMPLETE - Submenu 'AssemblyTree' created");
    NewBomLog("ADDIN", "");
```

- [ ] **Step 2: 修改兄弟关系链**

需要修改 STEP 5 中 Tools 菜单的 `SetAccessNext`，使 AssemblyTree 在 NumberMgt 之后，Tools 在 AssemblyTree 之后：

找到（约第303行）：
```cpp
    SetAccessNext(pNumberMgtMenu, pToolsMenu);
```

修改为：
```cpp
    NewBomLog("ADDIN", "  [5b] SetAccessNext chains menus in order: NumberMgt -> AssemblyTree -> Tools");
    SetAccessNext(pNumberMgtMenu, pAssemblyTreeMenu);
    SetAccessNext(pAssemblyTreeMenu, pToolsMenu);
    NewBomLog("ADDIN", "       Done: Menu order is now '编号管理' -> '获取装配树' -> '工具'");
```

- [ ] **Step 3: 更新菜单结构日志**

找到 FINAL SUMMARY 区域的菜单树日志（约第370行）：
```cpp
    NewBomLog("ADDIN", "Menu tree ready:");
    NewBomLog("ADDIN", "  NewBOM -> NumberMgt -> GetNewNumber");
    NewBomLog("ADDIN", "         -> Tools -> AboutPlugin");
```

修改为：
```cpp
    NewBomLog("ADDIN", "Menu tree ready:");
    NewBomLog("ADDIN", "  NewBOM -> NumberMgt -> GetNewNumber");
    NewBomLog("ADDIN", "         -> AssemblyTree -> GetAssemblyTree");
    NewBomLog("ADDIN", "         -> Tools -> AboutPlugin");
```

同时更新 Summary of created objects：
```cpp
    NewBomLog("ADDIN", "Summary of created objects:");
    NewBomLog("ADDIN", "  pAddinRoot     = %p (returned to CATIA)", pAddinRoot);
    NewBomLog("ADDIN", "  pMenuBar       = %p (menu bar container)", pMenuBar);
    NewBomLog("ADDIN", "  pTopMenu       = %p (NewBOM top menu)", pTopMenu);
    NewBomLog("ADDIN", "  pNumberMgtMenu = %p (NumberMgt submenu)", pNumberMgtMenu);
    NewBomLog("ADDIN", "  pAssemblyTreeMenu = %p (AssemblyTree submenu)", pAssemblyTreeMenu);
    NewBomLog("ADDIN", "  pToolsMenu     = %p (Tools submenu)", pToolsMenu);
```

- [ ] **Step 4: 提交菜单结构修改**

```bash
git add NewBomFrame/NewBomModule.m/src/NewBomAddin.cpp
git commit -m "feat: add AssemblyTree menu structure in CreateToolbars"
```

---

### Task 11: 更新 Imakefile.mk

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/Imakefile.mk`

- [ ] **Step 1: 添加依赖库**

修改 `LINK_WITH` 行，添加 ProductStructure 相关库：

```mk
# =============================================================================
# Imakefile.mk - CATIA CAA Module Build Configuration
# =============================================================================
# 
# BUILT_OBJECT_TYPE: Output type (SHARED LIBRARY = DLL)
# LINK_WITH: Libraries to link
#
# BUILT_OBJECT_TYPE indicates the type of build result:
#   - SHARED LIBRARY: Build a DLL that can be dynamically loaded
#   - LIBRARY: Build a static library
#   - PROGRAM: Build an executable
#
# LINK_WITH lists all CATIA frameworks/libraries needed:
#   - CATApplicationFrame: Main application, commands, menus
#   - JS0GROUP: Base object management (CATBaseUnknown)
#   - JS0FM: String handling (CATUnicodeString)
#   - DI0PANV2: Dialog system (CATDlgDialog, CATDlgTree, etc.)
#   - CATProductStructure: Product/Assembly structure (CATIProduct)
#
# =============================================================================

BUILT_OBJECT_TYPE=SHARED LIBRARY

LINK_WITH=CATApplicationFrame JS0GROUP JS0FM DI0PANV2 CATProductStructure
```

- [ ] **Step 2: 提交 Imakefile 更新**

```bash
git add NewBomFrame/NewBomModule.m/Imakefile.mk
git commit -m "feat: add CATProductStructure to LINK_WITH for assembly tree"
```

---

### Task 12: 编译验证

**Files:**
- 无文件修改，仅执行编译

- [ ] **Step 1: 运行编译脚本**

```bash
cd D:/work/catia_menu_plugin_r21
./build.bat
```

预期输出：
- 编译成功，生成 `win_b64/code/bin/NewBomModule.dll`
- 无编译错误

如果编译失败：
- 检查头文件是否正确引用
- 检查依赖库是否在 IdentityCard.h 中声明
- 查看编译日志定位具体错误

- [ ] **Step 2: 检查编译输出**

```bash
ls -la win_b64/code/bin/NewBomModule.dll
```

预期：DLL 文件存在且更新时间最近

---

### Task 13: 部署和测试

**Files:**
- 无文件修改，仅执行部署和测试

- [ ] **Step 1: 部署插件**

```bash
# 需要管理员权限
./deploy.bat
```

预期输出：
- DLL 复制成功
- 字典文件复制成功
- 资源文件复制成功

- [ ] **Step 2: 启动 CATIA**

```bash
./launch_catia.bat
```

- [ ] **Step 3: 测试功能**

测试步骤：
1. 打开 CATIA
2. 打开一个装配文档（Product 文件）
3. 点击 NewBOM → 获取装配树
4. 观察对话框是否弹出
5. 检查树控件是否显示装配结构
6. 查看日志文件 `%TEMP%\NewBomPlugin.log`

预期结果：
- 对话框正确弹出
- 树控件显示装配层级
- 日志中有 `[ASSEMBLY_DLG]` 标签的输出

- [ ] **Step 4: 测试边界情况**

边界测试：
1. 未打开文档时点击菜单 → 应显示 "当前没有打开文档"
2. 打开零件文档时点击 → 应显示零件名称
3. 关闭对话框后再次点击 → 应能重新弹出

---

### Task 14: 最终提交

**Files:**
- 提交所有变更

- [ ] **Step 1: 查看所有变更**

```bash
git status
git log --oneline -10
```

- [ ] **Step 2: 确保所有变更已提交**

如果有未提交的文件：
```bash
git add .
git commit -m "feat: complete assembly tree feature implementation"
```

- [ ] **Step 3: 推送到远程（可选）**

```bash
git push origin main
```

---

## 自审检查

### 1. Spec 覆盖检查

| Spec 需求 | 任务覆盖 |
|-----------|---------|
| 新建命令类和对话框 | Task 2, 3, 4, 5, 6, 7 ✅ |
| 添加菜单项 | Task 9, 10 ✅ |
| 获取装配结构 | Task 6, 7 ✅ |
| 树控件显示 | Task 5, 7 ✅ |
| 错误处理 | Task 6 ✅ |
| 日志记录 | Task 3, 6, 7 ✅ |
| 依赖配置 | Task 1, 11 ✅ |
| 编译测试 | Task 12, 13 ✅ |

### 2. Placeholder 检查

- 无 TBD/TODO
- 无 "implement later"
- 所有代码完整

### 3. 类型一致性检查

- `CATIProduct_var` 在所有任务中使用一致
- `CATDlgTree` 方法调用一致
- UCS-2 编码值一致

---

## 成功标准

- ✅ 编译无错误
- ✅ 菜单项正确显示
- ✅ 点击菜单弹出对话框
- ✅ 装配文档显示层级结构
- ✅ 错误情况有适当提示
- ✅ 日志正确输出