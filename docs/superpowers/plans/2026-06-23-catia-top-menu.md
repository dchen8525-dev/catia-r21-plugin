# CATIA 顶层自定义菜单 实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在 CATIA V5R21 顶层菜单栏新增一个名为「件取号」的自定义菜单，下挂「编号管理」「工具」两个分组子菜单，分别挂载「取新编号」(复用现有命令) 和「关于本插件」(新命令)。

**Architecture:** 沿用现有 `CATIAfrGeneralWksAddin` addin 架构。重写 `CreateToolbars()` 用 `NewAccess/SetAccessChild/SetAccessNext/SetAddinMenu` 构建嵌套菜单容器树；新增一个轻量"关于"命令 `PartNumberMenuAboutCmd`（`CATDlgNotify` 弹模态信息框后自销毁）；NLS 资源补菜单容器标题和命令标题。

**Tech Stack:** C++ (CATIA CAA V5R21 RADE), `CATCmdContainer`/`CATCmdStarter` 菜单框架, `CATDlgNotify` (Dialog 框架), NLS/CATNls 资源。

**参考 spec:** `docs/superpowers/specs/2026-06-23-catia-top-menu-design.md`

**验证方式说明:** 本项目是 CATIA CAA C++ 插件，无单元测试框架，且当前环境无 CATIA 运行时。验证 = 编译通过（`build.bat`）+ 静态自检（NLS key ↔ 容器名一致性、菜单父子/兄弟关系配对、命令工厂注册完整）。每个任务末尾有"静态自检"步骤替代测试步骤。

**环境注意:** 本项目不是 git 仓库，所以任务末尾的"提交"步骤改为"做静态自检并记录"，不执行 git 命令。

---

## 文件结构

### 新增文件（2）

| 文件 | 职责 |
|------|------|
| `PartNumberMenuFrame/PartNumberMenuModule.m/LocalInterfaces/PartNumberMenuAboutCmd.h` | "关于"命令类声明。继承 `CATCommand`，声明 `CATDeclareClass` + 构造/析构 + `Activate/Desactivate/Cancel` override。无成员变量。 |
| `PartNumberMenuFrame/PartNumberMenuModule.m/src/PartNumberMenuAboutCmd.cpp` | "关于"命令实现。`CATImplementClass` + `CATCreateClass` 工厂注册 + `Activate`(InfoMsgBox+自销毁) + 空 `Desactivate/Cancel`。 |

### 修改文件（3）

| 文件 | 改动 |
|------|------|
| `PartNumberMenuFrame/PartNumberMenuModule.m/src/PartNumberMenuAddin.cpp` | ① 加 include；② `CreateCommands()` 注册新命令头；③ 重写 `CreateToolbars()` 为新菜单树 |
| `PartNumberMenuFrame/PartNumberMenuModule.m/LocalInterfaces/PartNumberMenuAddin.h` | **不改**（`CreateToolbars` 签名不变）—— 列出以确认无需动 |
| `PartNumberMenuFrame/CNext/resources/msgcatalog/PartNumberMenuHeader.CATNls` | 改 `PartNumberMenuCmd.Title`；加 `PartNumberMenuAboutCmd` 4 行；加 3 个菜单容器 `.Title` |

---

## Task 1: 新建「关于」命令类头文件

**Files:**
- Create: `PartNumberMenuFrame/PartNumberMenuModule.m/LocalInterfaces/PartNumberMenuAboutCmd.h`

- [ ] **Step 1: 创建头文件**

写入以下完整内容（风格对齐现有 `PartNumberMenuCmd.h`）：

```cpp
// PartNumberMenuAboutCmd.h
// CATIA CAA "About" Command - displays plugin version info

#ifndef __PartNumberMenuAboutCmd_h__
#define __PartNumberMenuAboutCmd_h__

#include "CATCommand.h"

/**
 * @brief About Command
 *
 * Lightweight command that pops up an information box with the plugin
 * version info, then self-destructs. No persistent dialog, no shared mode.
 */
class PartNumberMenuAboutCmd : public CATCommand
{
    CATDeclareClass;

public:
    PartNumberMenuAboutCmd();
    virtual ~PartNumberMenuAboutCmd();

    // CATCommand overrides
    virtual CATStatusChangeRC Activate(CATCommand* iCmd,
                                       CATNotification* iNotif);
    virtual CATStatusChangeRC Desactivate(CATCommand* iCmd,
                                          CATNotification* iNotif);
    virtual CATStatusChangeRC Cancel(CATCommand* iCmd,
                                     CATNotification* iNotif);
};

#endif // __PartNumberMenuAboutCmd_h__
```

- [ ] **Step 2: 静态自检**

确认文件已创建。检查：
- 头文件守卫 `__PartNumberMenuAboutCmd_h__` 双向匹配
- 三个 override 签名与 `PartNumberMenuCmd.h` 完全一致（参数类型 `CATCommand*` / `CATNotification*`，返回 `CATStatusChangeRC`）

---

## Task 2: 实现「关于」命令

**Files:**
- Create: `PartNumberMenuFrame/PartNumberMenuModule.m/src/PartNumberMenuAboutCmd.cpp`

- [ ] **Step 1: 创建实现文件**

写入以下完整内容。中文用 `\xXXXX` 转义（沿用 `PartNumberMenuDlg.cpp` 的 `U()` 风格），避免源文件编码问题。

```cpp
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
```

- [ ] **Step 2: 静态自检**

检查：
- `CATImplementClass` 第二参数 `Implementation`、第三参数 `CATCommand`、第四参数 `CATNull` —— 与现有 `PartNumberMenuCmd.cpp` 完全一致的模式
- `CATCreateClass(PartNumberMenuAboutCmd)` 类名与 header 类名、构造函数 `CATCommand(NULL, "PartNumberMenuAboutCmd")` 的字符串三者一致
- `#include` 顺序：自身头 → Log → `CATApplicationFrame.h` → `CATDlgNotify.h` → `CATCreateExternalObject.h` → `CATUnicodeString.h`
- 三个 override 签名与 `PartNumberMenuCmd.cpp` 完全一致
- 中文转义对照：`件取号插件` = `\x4EF6\x53D6\x53F7\x63D2\x4EF6`，`关于本插件` = `\x5173\x4E8E\x672C\x63D2\x4EF6`

> **编辑提示（重要）:** `CATDlgNotify` 是 V5R21 的模态信息框基类（Dialog 框架，lib `DI0PANV2`），不是 `CATInformationBox`（后者在 V5R21 中不存在）。`CATCreateClass` 宏定义在 `CATCreateExternalObject.h`（System 框架），必须 include，否则编译报 C4430。

---

## Task 3: 更新 NLS 资源

**Files:**
- Modify: `PartNumberMenuFrame/CNext/resources/msgcatalog/PartNumberMenuHeader.CATNls`

- [ ] **Step 1: 改写整个文件**

当前文件内容（5 行）。改写为以下完整内容（改 1 行 + 新增 8 行）：

```text
PartNumberMenuHeader.PartNumberMenuCmd.Category  = "Tools";
PartNumberMenuHeader.PartNumberMenuCmd.Title     = "取新编号";
PartNumberMenuHeader.PartNumberMenuCmd.ShortHelp = "Display Hello World dialog";
PartNumberMenuHeader.PartNumberMenuCmd.Help      = "Display a Hello World dialog";
PartNumberMenuHeader.PartNumberMenuCmd.LongHelp  = "Display a Hello World dialog.";

PartNumberMenuHeader.PartNumberMenuAboutCmd.Category  = "Tools";
PartNumberMenuHeader.PartNumberMenuAboutCmd.Title     = "关于本插件";
PartNumberMenuHeader.PartNumberMenuAboutCmd.ShortHelp = "显示插件版本信息";
PartNumberMenuHeader.PartNumberMenuAboutCmd.Help      = "显示插件版本信息";
PartNumberMenuHeader.PartNumberMenuAboutCmd.LongHelp  = "显示件取号插件版本信息。";

PartNumberMenuHeader.PartNumberMenuTopMnu.Title       = "件取号";
PartNumberMenuHeader.PartNumberMenuNumberMgtMnu.Title = "编号管理";
PartNumberMenuHeader.PartNumberMenuToolsMnu.Title     = "工具";
```

- [ ] **Step 2: 静态自检**

对照 Task 4 将要写的 `CreateToolbars()` 容器名，逐个核对 NLS key：
- `PartNumberMenuTopMnu` ← 顶层菜单容器名 ✓
- `PartNumberMenuNumberMgtMnu` ← 「编号管理」子菜单容器名 ✓
- `PartNumberMenuToolsMnu` ← 「工具」子菜单容器名 ✓
- `PartNumberMenuAboutCmd` ← 命令 HeaderID ✓

确认所有容器 `.Title` 都有对应 key（菜单文字缺失会导致菜单显示为空或乱码）。

> **注意:** NLS 文件本身**用 UTF-8 / 本地编码直接写中文字符即可**（不像 C++ 源码要 `\xXXXX` 转义）—— 因为现有 `PartNumberMenuAddin.CATNls` 里已有中文 `Hello World`（`ToolbarV3.Title = "Hello World"` 实测可被 CATIA 正确读取），说明 NLS 资源链支持中文。但为了和现有 NLS 文件保持完全一致的编码写法，写完后用编辑器确认文件无乱码。

---

## Task 4: 重写 CreateToolbars 构建菜单树

**Files:**
- Modify: `PartNumberMenuFrame/PartNumberMenuModule.m/src/PartNumberMenuAddin.cpp`

- [ ] **Step 1: 替换 `CreateToolbars()` 整个函数体**

把现有的 `CreateToolbars()` 函数（从 `CATCmdContainer* PartNumberMenuAddin::CreateToolbars()` 到它的 `return pAddinRoot;` 和右大括号）**整体替换**为以下内容：

```cpp
CATCmdContainer* PartNumberMenuAddin::CreateToolbars()
{
    PartNumberMenuLog("ADDIN", "CreateToolbars/CreateMenus entered");

    // Addin root container and the addin's menu bar container.
    NewAccess(CATCmdContainer, pAddinRoot, PartNumberMenuAddinRoot);
    NewAccess(CATCmdContainer, pMenuBar, PartNumberMenuMenuBar);

    // ---- Top-level menu「件取号」 ----
    NewAccess(CATCmdContainer, pTopMenu, PartNumberMenuTopMnu);
    SetAccessChild(pMenuBar, pTopMenu);

    // ---- Submenu「编号管理」 (first child of top menu) ----
    NewAccess(CATCmdContainer, pNumberMgtMenu, PartNumberMenuNumberMgtMnu);
    SetAccessChild(pTopMenu, pNumberMgtMenu);

    //      Command item「取新编号」 -> reuses existing PartNumberMenuCmd
    NewAccess(CATCmdStarter, pGetNumberStarter, PartNumberMenuGetNumberStr);
    SetAccessCommand(pGetNumberStarter, "PartNumberMenuCmd");
    SetAccessChild(pNumberMgtMenu, pGetNumberStarter);

    // ---- Submenu「工具」 (sibling of「编号管理」, chained via SetAccessNext) ----
    NewAccess(CATCmdContainer, pToolsMenu, PartNumberMenuToolsMnu);
    SetAccessNext(pNumberMgtMenu, pToolsMenu);

    //      Command item「关于本插件」 -> PartNumberMenuAboutCmd
    NewAccess(CATCmdStarter, pAboutStarter, PartNumberMenuAboutStr);
    SetAccessCommand(pAboutStarter, "PartNumberMenuAboutCmd");
    SetAccessChild(pToolsMenu, pAboutStarter);

    // Attach the menu bar to the CATIA frame.
    SetAddinMenu(pAddinRoot, pMenuBar);

    PartNumberMenuLog("ADDIN",
                      "CreateToolbars completed root=%p menubar=%p top=%p numbermgt=%p tools=%p",
                      pAddinRoot, pMenuBar, pTopMenu, pNumberMgtMenu, pToolsMenu);
    return pAddinRoot;

    // =================================================================
    //  扩展点：如何加新命令 / 新菜单项 / 新子菜单
    // =================================================================
    //  1) 新命令:
    //     - CreateCommands() 里 new PartNumberMenuHeader("<HeaderID>",
    //           "PartNumberMenuModule", "<ClassName>", NULL);
    //     - 新建 <ClassName>.h / .cpp (CATImplementClass + CATCreateClass)
    //     - PartNumberMenuHeader.CATNls 加 PartNumberMenuHeader.<HeaderID>.Title 等
    //     - 下面用 SetAccessCommand(starter, "<HeaderID>") 挂到菜单
    //
    //  2) 新菜单项加到现有子菜单:
    //     NewAccess(CATCmdStarter, pNew, <StarterName>);
    //     SetAccessCommand(pNew, "<HeaderID>");
    //     SetAccessNext(<上一个starter或子菜单>, pNew);   // 平级串到末尾
    //
    //  3) 全新子菜单 (与现有子菜单平级):
    //     NewAccess(CATCmdContainer, pNewMnu, <MenuName>);
    //     SetAccessNext(pToolsMenu, pNewMnu);             // 串到子菜单链末尾
    //     NewAccess(CATCmdStarter, pFirst, ...);
    //     SetAccessChild(pNewMnu, pFirst);                // 往新子菜单里挂第一项
    //     PartNumberMenuHeader.CATNls 加 PartNumberMenuHeader.<MenuName>.Title
    // =================================================================
}
```

**关键变化（相比旧实现）：**
- 删除旧的 `SetAccessAnchorName(pTopMenu, "CATAfrWindowMnu")`（不再挂到 Window 菜单）
- 改用 `SetAccessChild(pMenuBar, pTopMenu)` 让顶层菜单直接成为菜单栏的独立项
- 新增两层子菜单容器和对应的 starter

- [ ] **Step 2: 静态自检**

逐行核对菜单结构（参照 spec 第 3 节菜单树）：
- `pMenuBar` ← `SetAccessChild(pMenuBar, pTopMenu)` → `pTopMenu` ✓（顶层菜单挂在菜单栏）
- `pTopMenu` ← `SetAccessChild(pTopMenu, pNumberMgtMenu)` → `pNumberMgtMenu` ✓（编号管理是顶层第一个子项）
- `pNumberMgtMenu` ← `SetAccessChild(pNumberMgtMenu, pGetNumberStarter)` → `pGetNumberStarter` ✓（取新编号在编号管理下）
- `pNumberMgtMenu` ← `SetAccessNext(pNumberMgtMenu, pToolsMenu)` → `pToolsMenu` ✓（工具是编号管理的兄弟）
- `pToolsMenu` ← `SetAccessChild(pToolsMenu, pAboutStarter)` → `pAboutStarter` ✓（关于在工具下）
- `SetAccessCommand(pGetNumberStarter, "PartNumberMenuCmd")` ✓（复用现有命令）
- `SetAccessCommand(pAboutStarter, "PartNumberMenuAboutCmd")` ✓（新命令）
- 每个 `NewAccess` 的第二参数标识符（如 `PartNumberMenuTopMnu`）与 NLS key 一一对应

确认：每个新建的容器都有且仅有一个 `SetAccessChild` 把它挂到父级（顶层菜单/子菜单都满足），兄弟关系用 `SetAccessNext` 表达。

---

## Task 5: 注册「关于」命令头

**Files:**
- Modify: `PartNumberMenuFrame/PartNumberMenuModule.m/src/PartNumberMenuAddin.cpp`

- [ ] **Step 1: 加 include**

在 `PartNumberMenuAddin.cpp` 顶部现有的 include 块（`#include "PartNumberMenuAddin.h"` 等）末尾，加一行：

```cpp
#include "PartNumberMenuAboutCmd.h"
```

最终 include 块应类似：
```cpp
#include "PartNumberMenuAddin.h"
#include "PartNumberMenuAboutCmd.h"
#include "PartNumberMenuLog.h"
#include "TIE_CATIAfrGeneralWksAddin.h"
#include "CATCommandHeader.h"
#include "CATCmdContainer.h"
#include "CATCreateWorkshop.h"
```

- [ ] **Step 2: 在 `CreateCommands()` 末尾加新命令头**

找到现有 `CreateCommands()` 函数，当前末尾是：
```cpp
    CATCommandHeader* header = new PartNumberMenuHeader("PartNumberMenuCmd",
                                                    "PartNumberMenuModule",
                                                    "PartNumberMenuCmd",
                                                    (void*) NULL);
    PartNumberMenuLog("ADDIN", "CreateCommands completed header=%p", header);
```

在 `PartNumberMenuLog("ADDIN", "CreateCommands completed...")` 这一行**之前**插入新命令头注册：

```cpp
    CATCommandHeader* header = new PartNumberMenuHeader("PartNumberMenuCmd",
                                                    "PartNumberMenuModule",
                                                    "PartNumberMenuCmd",
                                                    (void*) NULL);
    CATCommandHeader* aboutHeader = new PartNumberMenuHeader("PartNumberMenuAboutCmd",
                                                    "PartNumberMenuModule",
                                                    "PartNumberMenuAboutCmd",
                                                    (void*) NULL);
    PartNumberMenuLog("ADDIN", "CreateCommands completed header=%p aboutheader=%p",
                      header, aboutHeader);
```

- [ ] **Step 3: 静态自检**

检查：
- 两个命令头的第 3 参数（ClassName）与 `CATCreateClass` 注册的类名一致：`PartNumberMenuCmd` ↔ `CATCreateClass(PartNumberMenuCmd)`、`PartNumberMenuAboutCmd` ↔ `CATCreateClass(PartNumberMenuAboutCmd)` ✓
- 第 1 参数（HeaderID）与 NLS key、`SetAccessCommand` 字符串三者一致：`PartNumberMenuCmd`、`PartNumberMenuAboutCmd` ✓
- 第 2 参数（LoadName）都是 `"PartNumberMenuModule"`（DLL 名，对应 Imakefile 的 `BUILT_OBJECT_TYPE=SHARED LIBRARY` 产物）✓
- include 顺序合理，`PartNumberMenuAboutCmd.h` 放在 addin 自身 header 之后

---

## Task 6: 编译验证

**Files:** 无（验证步骤）

- [ ] **Step 1: 运行构建**

Run: `build.bat`（在项目根目录 `D:\work\catia_menu_plugin_r21` 下）

预期：构建脚本调用 CATSTART → RADE 环境 → `mkmk -a` → `mkCreateRuntimeView`。

- [ ] **Step 2: 判定结果**

- **成功**：脚本输出 `RADE x64 build and runtime view completed successfully.` 且退出码 0。
- **失败**：查看 `rade_build.log` / `rade_preq.log` / `rade_runtime.log`（在项目根目录），根据报错定位：
  - 找不到符号 → 检查头文件是否属于已 `LINK_WITH` 的框架（`CATDlgNotify.h` 属 `Dialog`/`DI0PANV2`，已在 Imakefile；`CATCreateExternalObject.h` 属 `System`/`JS0GROUP`，已在 Imakefile）
  - `PartNumberMenuAboutCmd` 未注册 → 检查 `CATCreateClass` 是否写对
  - 中文转义语法错 → 检查 `\xXXXX` 是否都在 `L"..."` 宽字符串内

> **注意:** 本环境（当前 agent 运行环境）**未必安装了 CATIA RADE**，`build.bat` 可能因找不到 `CATSTART.exe` 直接 `exit /b 2`。这种情况属于"环境不具备"，**不算实现错误** —— 改动本身的正确性由 Task 1-5 的静态自检保证。若运行失败，记录失败原因后仍可认为代码层面完成，等用户在有 RADE 的机器上实跑。

- [ ] **Step 3: 记录结果**

把构建结果（成功 / 因环境缺失失败的具体报错）记到本次实现的总结里，供用户在目标机器上复验。

---

## 完成定义 (Definition of Done)

全部满足即视为实现完成：
1. Task 1-5 的所有文件已创建/修改，静态自检全部通过
2. Task 6 的 `build.bat` 要么编译成功，要么失败原因明确是"环境缺 RADE"（非代码错误）
3. 菜单树结构（spec 第 3 节）与 `CreateToolbars()` 实现一一对应
4. NLS 每个容器/命令都有 `.Title`，无缺失 key
5. 扩展注释块存在于 `CreateToolbars()` 末尾，三种扩展场景都有模板
```
