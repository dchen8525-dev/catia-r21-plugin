# CATIA 顶层自定义菜单（带子菜单）设计

- **日期**: 2026-06-23
- **目标**: 在 CATIA V5R21 顶层菜单栏新增一个名为「件取号」的自定义菜单，该菜单包含分组子菜单，子菜单下挂命令项。
- **范围**: `PartNumberMenuFrame` 框架内的 addin / 命令 / NLS 资源改动，不涉及 dictionary、IdentityCard、Imakefile。

---

## 1. 背景与现状

现有插件 `PartNumberMenuFrame` 已实现：
- 一个命令 `PartNumberMenuCmd`，点击后弹出「件取号」对话框（`PartNumberMenuDlg`）。
- addin `PartNumberMenuAddin`（实现 `CATIAfrGeneralWksAddin`）通过 `CreateToolbars()` 把命令以 starter 形式挂到 **CATIA 已有的 Window 菜单**（`SetAccessAnchorName(pTopMenu, "CATAfrWindowMnu")`）。

**问题**：当前没有独立的顶层菜单，命令只是塞在 Window 菜单下的一项。

**本次目标**：新建一个独立的顶层菜单「件取号」，下挂分组子菜单。

---

## 2. 选定方案（方案 A）

沿用现有 `CATIAfrGeneralWksAddin` addin 架构，只改 `CreateToolbars()` 里的菜单容器层级，新建独立顶层菜单容器并嵌套出「分组子菜单 → 命令项」结构，最后用 `SetAddinMenu` 挂到菜单栏。

**选型理由**：改动最小，完全复用现有 addin / dictionary / NLS 体系，符合 CAA 标准 `SetAddinMenu` 挂载方式；方案 B（独立 Workshop）对"加个菜单"属过度设计，方案 C 不解决问题。

---

## 3. 菜单树结构

```
菜单栏 (PartNumberMenuMenuBar)
└─ 顶层菜单「件取号」 (PartNumberMenuTopMnu)
    ├─ 子菜单「编号管理」 (PartNumberMenuNumberMgtMnu)
    │   └─ 命令项「取新编号」 → 复用现有 PartNumberMenuCmd
    └─ 子菜单「工具」 (PartNumberMenuToolsMnu)
        └─ 命令项「关于本插件」 → 新命令 PartNumberMenuAboutCmd
```

**构建方式**：
- `NewAccess(CATCmdContainer, …)` 逐层建容器
- `SetAccessChild(parent, child)` 把子菜单/命令挂到父级下
- `SetAccessNext(prev, next)` 把「编号管理」「工具」两个子菜单串成平级兄弟
- 命令项用 `NewAccess(CATCmdStarter, …)` + `SetAccessCommand(starter, "<HeaderID>")`
- 最后 `SetAddinMenu(pAddinRoot, pMenuBar)` 挂到菜单栏

**扩展点约定**（代码末尾留注释块说明）：
- 加新命令：`CreateCommands()` 末尾仿照 `PartNumberMenuAboutCmd` 加 `new PartNumberMenuHeader(...)`，并在 NLS 补 Title；菜单里用 `SetAccessCommand(starter, "新HeaderID")`。
- 加新菜单项到现有子菜单：`NewAccess(CATCmdStarter,…)` + `SetAccessCommand` + `SetAccessNext(<上一项>, 新starter)`。
- 加全新子菜单：`NewAccess(CATCmdContainer,…)` + `SetAccessNext(<末尾子菜单>, 新子菜单)`，再用 `SetAccessChild` 往里挂 starter。

未实现的命令（如"回收编号"）**本次不放**，靠上述扩展点后续添加，避免出现灰色禁用项。

---

## 4. 命令头与命令类

### 4.1 命令头注册（`CreateCommands()`）

沿用 `MacDeclareHeader` 生成的通用命令头类 `PartNumberMenuHeader`，每个命令注册一个 `HeaderID`：

```cpp
// 现有（保留）
new PartNumberMenuHeader("PartNumberMenuCmd",      "PartNumberMenuModule", "PartNumberMenuCmd",      NULL);
// 新增
new PartNumberMenuHeader("PartNumberMenuAboutCmd", "PartNumberMenuModule", "PartNumberMenuAboutCmd", NULL);
```

参数顺序：`HeaderID` / `LoadName(DLL)` / `ClassName` / `Argument`。"关于"命令的 HeaderID 与 ClassName 均为 `PartNumberMenuAboutCmd`。

### 4.2 新命令类 `PartNumberMenuAboutCmd`

- 轻量命令，继承 `CATCommand`，`CATDeclareClass` + `CATImplementClass(... Implementation, CATCommand, CATNull)`。
- `CATCreateClass(PartNumberMenuAboutCmd)` 作为命令头工厂入口。
- `Activate()`：通过 `CATApplicationFrame::GetFrame()->GetMainWindow()` 获取父窗口 → 构造中文文本（`\xXXXX` 转义）→ `new CATDlgNotify(parent, name, CATDlgNfyInformation|CATDlgNfyOK|CATDlgWndModal)` → `DisplayBlocked(msg, title)` 阻塞显示 → 销毁 notify → `RequestDelayedDestruction()` 自销毁。
- 内容：「件取号插件 v1.0 / CATIA V5R21 CAA」。
- `Desactivate()` / `Cancel()`：空实现 + 日志，保持风格一致。
- 无成员变量、不开常驻对话框、不需要 shared mode。

---

## 5. NLS / RSC 资源

文件：`CNext/resources/msgcatalog/PartNumberMenuHeader.CATNls`

```text
// —— 命令 ——
PartNumberMenuHeader.PartNumberMenuCmd.Category  = "Tools";
PartNumberMenuHeader.PartNumberMenuCmd.Title     = "取新编号";        // 由 "Hello World" 改名
PartNumberMenuHeader.PartNumberMenuCmd.ShortHelp = "Display Hello World dialog";
PartNumberMenuHeader.PartNumberMenuCmd.Help      = "Display a Hello World dialog";
PartNumberMenuHeader.PartNumberMenuCmd.LongHelp  = "Display a Hello World dialog.";

PartNumberMenuHeader.PartNumberMenuAboutCmd.Category  = "Tools";
PartNumberMenuHeader.PartNumberMenuAboutCmd.Title     = "关于本插件";
PartNumberMenuHeader.PartNumberMenuAboutCmd.ShortHelp = "显示插件版本信息";
PartNumberMenuHeader.PartNumberMenuAboutCmd.Help      = "显示插件版本信息";
PartNumberMenuHeader.PartNumberMenuAboutCmd.LongHelp  = "显示件取号插件版本信息。";

// —— 菜单容器标题（key = 容器名） ——
PartNumberMenuHeader.PartNumberMenuTopMnu.Title       = "件取号";
PartNumberMenuHeader.PartNumberMenuNumberMgtMnu.Title = "编号管理";
PartNumberMenuHeader.PartNumberMenuToolsMnu.Title     = "工具";
```

**现有 `PartNumberMenuCmd.Title`** 从 `"Hello World"` 改为「取新编号」，使其与菜单项语义一致。

### 不改动的资源

- `PartNumberMenuHeader.CATRsc`：不涉及"关于"命令的图标（信息框无需图标）；现有 `PartNumberMenuCmd` 图标保持。
- `PartNumberMenuAddin.CATNls`、`PartNumberMenuModule.CATNls`：不变。

---

## 6. 文件改动清单

### 新增（2）

| 文件 | 内容 |
|------|------|
| `LocalInterfaces/PartNumberMenuAboutCmd.h` | 类声明：`CATDeclareClass` + 构造/析构 + `Activate/Desactivate/Cancel`，无成员 |
| `src/PartNumberMenuAboutCmd.cpp` | `CATImplementClass` + `CATCreateClass` + `Activate`(InfoMsgBox+自销毁) + 空 `Desactivate/Cancel` |

### 修改（3）

| 文件 | 改动 |
|------|------|
| `src/PartNumberMenuAddin.cpp` | ① `#include "PartNumberMenuAboutCmd.h"`；② `CreateCommands()` 加 `PartNumberMenuAboutCmd` 命令头；③ 重写 `CreateToolbars()` 为新菜单树（顶层菜单 + 两子菜单 + 两 starter），删旧的 `SetAccessAnchorName(..., "CATAfrWindowMnu")`，末尾留扩展注释块 |
| `CNext/resources/msgcatalog/PartNumberMenuHeader.CATNls` | 改 `PartNumberMenuCmd.Title`；加 `PartNumberMenuAboutCmd` 4 行；加 3 个菜单容器 `.Title` |

### 不改动（备查）

`PartNumberMenuCmd.cpp/h`、`PartNumberMenuDlg.cpp/h`、`PartNumberMenuModule.cpp`、`PartNumberMenuLog.cpp/h`、`Imakefile.mk`、`IdentityCard.h`、`PartNumberMenuFrame.dico`、`PartNumberMenuHeader.CATRsc`、`PartNumberMenuAddin.CATNls`、`PartNumberMenuModule.CATNls`

### 依赖说明

- `CATDlgNotify` 属 `Dialog` 框架（导出库 `DI0PANV2`），`Imakefile.mk` 已 `LINK_WITH=... DI0PANV2`，**无需改 Imakefile**。
- `CATCreateClass` 宏定义在 `System/PublicInterfaces/CATCreateExternalObject.h`，About 命令实现需 include 此头文件（`PartNumberMenuCmd.cpp` 已有先例）。
- dictionary 走 `CATCreateClass` 命令工厂，不在 dico 注册，**无需改 dico**。
- IdentityCard 框架前提条件不变，**无需改 IdentityCard**。

---

## 7. `CreateToolbars()` 目标实现预览

```cpp
CATCmdContainer* PartNumberMenuAddin::CreateToolbars()
{
    PartNumberMenuLog("ADDIN", "CreateToolbars/CreateMenus entered");

    NewAccess(CATCmdContainer, pAddinRoot, PartNumberMenuAddinRoot);
    NewAccess(CATCmdContainer, pMenuBar, PartNumberMenuMenuBar);

    // 顶层菜单「件取号」
    NewAccess(CATCmdContainer, pTopMenu, PartNumberMenuTopMnu);
    SetAccessChild(pMenuBar, pTopMenu);

    // 子菜单「编号管理」
    NewAccess(CATCmdContainer, pNumberMgtMenu, PartNumberMenuNumberMgtMnu);
    SetAccessChild(pTopMenu, pNumberMgtMenu);

    NewAccess(CATCmdStarter, pGetNumberStarter, PartNumberMenuGetNumberStr);
    SetAccessCommand(pGetNumberStarter, "PartNumberMenuCmd");
    SetAccessChild(pNumberMgtMenu, pGetNumberStarter);

    // 子菜单「工具」(与「编号管理」平级)
    NewAccess(CATCmdContainer, pToolsMenu, PartNumberMenuToolsMnu);
    SetAccessNext(pNumberMgtMenu, pToolsMenu);

    NewAccess(CATCmdStarter, pAboutStarter, PartNumberMenuAboutStr);
    SetAccessCommand(pAboutStarter, "PartNumberMenuAboutCmd");
    SetAccessChild(pToolsMenu, pAboutStarter);

    SetAddinMenu(pAddinRoot, pMenuBar);

    PartNumberMenuLog("ADDIN", "CreateToolbars completed root=%p menubar=%p top=%p",
                      pAddinRoot, pMenuBar, pTopMenu);
    return pAddinRoot;

    // ┌──── 扩展点注释块：新命令 / 新菜单项 / 新子菜单 三种模板 ────┐
    // （见上文档第 3 节"扩展点约定"）                                │
    // └─────────────────────────────────────────────────────────────┘
}
```

---

## 8. 验证方式

本环境无 CATIA 运行时，无法实跑验证。实现完成后的**静态验证清单**：
1. 编译：`build.bat` 通过（命令头工厂 `CATCreateClass` 注册正确、`CATDlgNotify`/`CATCreateExternalObject` 头文件可解析、`PartNumberMenuAboutCmd.obj` 生成、DLL 链接无 ERROR）。注意 `build.bat` 退出码有误报倾向，应以 `rade_build.log` 内容和 DLL 产物存在性为准。
2. 资源完整性：NLS 里每个容器的 key 与 `NewAccess` 第二参数标识符一一对应。
3. 菜单结构自检：顶层菜单 → 2 子菜单 → 各 1 starter 的父子/兄弟关系正确（`SetAccessChild` / `SetAccessNext` 配对）。
4. 复用性：现有 `PartNumberMenuCmd` 命令/对话框代码零改动，仅改 NLS Title。
```
