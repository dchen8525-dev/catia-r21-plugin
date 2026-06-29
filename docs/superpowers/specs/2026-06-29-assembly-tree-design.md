# 获取装配树功能设计文档

## 目标

在 CATIA R21 NewBOM 插件中添加"获取装配树"子菜单功能，点击后显示当前激活零件/装配的完整层级结构。

## 功能概述

### 菜单位置

```
NewBOM (顶级菜单)
  ├─ 编号管理 (现有)
  │    └─ 零件取号 (现有)
  ├─ 获取装配树 (新增)
  └─ 工具 (现有)
       └─ 关于本插件 (现有)
```

### 用户交互流程

1. 用户在 CATIA 中打开装配文档或零件文档
2. 点击 NewBOM → 获取装配树
3. 弹出对话框，显示树形层级结构
4. 仅显示零件名称，完整层级
5. 用户查看后关闭对话框

## 技术方案

### 整体架构

新增两个类：
- `NewBomAssemblyTreeCmd` - 命令类，处理菜单点击事件
- `NewBomAssemblyTreeDlg` - 对话框类，显示装配树

设计遵循现有项目的命名和结构规范：

```
NewBomModule.m/
  ├─ LocalInterfaces/
  │    ├─ NewBomAssemblyTreeCmd.h   (命令类声明)
  │    └─ NewBomAssemblyTreeDlg.h   (对话框类声明)
  └─ src/
       ├─ NewBomAssemblyTreeCmd.cpp (命令实现)
       └─ NewBomAssemblyTreeDlg.cpp (对话框实现)
```

### 命令类设计 (NewBomAssemblyTreeCmd)

```cpp
class NewBomAssemblyTreeCmd : public CATCommand
{
    CATDeclareClass;

private:
    NewBomAssemblyTreeDlg* m_pDialog;

public:
    NewBomAssemblyTreeCmd();
    virtual ~NewBomAssemblyTreeCmd();

    virtual CATStatusChangeRC Activate(CATCommand* iCmd, CATNotification* iNotif);
    virtual void CloseDialog(CATCommand* iCmd, CATNotification* iNotif, CATCommandClientData iClientData);
    virtual CATStatusChangeRC Desactivate(CATCommand* iCmd, CATNotification* iNotif);
    virtual CATStatusChangeRC Cancel(CATCommand* iCmd, CATNotification* iNotif);
};
```

关键实现点：
- 构造函数调用 `RequestStatusChange(CATCommandMsgRequestSharedMode)` 以共享模式运行
- `Activate()` 创建并显示对话框
- `CloseDialog()` 处理对话框关闭，调用 `RequestDelayedDestruction()`
- 使用 `AddAnalyseNotificationCB` 注册关闭事件回调

### 对话框类设计 (NewBomAssemblyTreeDlg)

```cpp
class NewBomAssemblyTreeDlg : public CATDlgDialog
{
    CATDeclareClass;

private:
    CATDlgTree* m_pAssemblyTree;

public:
    NewBomAssemblyTreeDlg(CATDialog* iParent, const CATString& iObjectName);
    virtual ~NewBomAssemblyTreeDlg();

    void BuildDialog();

private:
    void GetAssemblyStructure();
    void PopulateTree();
};
```

对话框布局：

```
┌─────────────────────────────────────┐
│         获取装配树                    │  ← 标题
├─────────────────────────────────────┤
│                                     │
│  ┌───────────────────────────────┐  │
│  │ □ Product1                    │  │  ← CATDlgTree 树形控件
│  │   ├─ ■ Part1                  │  │
│  │   ├─ ■ Part2                  │  │
│  │   └─ □ Product2               │  │
│  │       ├─ ■ Part3              │  │
│  │       └─ ■ Part4              │  │
│  └───────────────────────────────┘  │
│                                     │
│  状态: 当前零件 Part1              │  ← CATDlgLabel 状态标签
│                                     │
├─────────────────────────────────────┤
│                              [关闭] │
└─────────────────────────────────────┘
```

特性：
- 使用 `CATDlgGridLayout` 布局
- 使用 `CATDlgTree` 控件显示树形结构
- 仅显示零件名称，不显示路径或编号
- 当前激活零件在树中高亮显示（如果可行）

### 装配结构获取逻辑

获取流程：

```
获取 CATIA 应用框架 (CATApplicationFrame::GetFrame())
    ↓
获取当前编辑器 (CATFrmEditor::GetCurrentEditor())
    ↓
获取当前文档 (CATFrmEditor::GetDocument())
    ↓
判断文档类型
    ├─ Product 文档: 遍历子组件
    └─ Part 文档: 向上查找父级 Product
    ↓
遍历 Product 结构 (CATIProduct 接口)
    ├─ GetChildren() 获取子组件列表
    └─ 递归遍历获取完整层级
    ↓
填充树控件 (CATDlgTree → AddItem())
```

关键 CATIA 接口：
- `CATIProduct` - 获取产品/组件信息
- `CATIProduct_var` - 智能指针管理
- `GetChildren()` - 获取子组件列表
- `GetParentProduct()` - 获取父级产品（用于从 Part 向上追溯）
- `CATIDocument` - 获取文档名称

### 文件修改清单

| 文件 | 操作 | 说明 |
|------|------|------|
| `LocalInterfaces/NewBomAssemblyTreeCmd.h` | 新建 | 命令类声明 |
| `src/NewBomAssemblyTreeCmd.cpp` | 新建 | 命令类实现 |
| `LocalInterfaces/NewBomAssemblyTreeDlg.h` | 新建 | 对话框类声明 |
| `src/NewBomAssemblyTreeDlg.cpp` | 新建 | 对话框类实现（包含树遍历逻辑） |
| `src/NewBomAddin.cpp` | 修改 | 注册新命令头，添加新菜单项 |
| `Imakefile.mk` | 修改 | 添加新源文件和依赖库 |
| `CNext/resources/msgcatalog/NewBomHeader.CATNls` | 修改 | 添加菜单项中文文本 |

新增依赖库：
- `CATProductStructure` - Product 结构 API
- `CATFrmEditor` - 编辑器接口

### 菜单创建细节

在 `NewBomAddin.cpp` 中：

#### CreateCommands() 新增：

```cpp
// Command Header 3: NewBomAssemblyTreeCmd
CATCommandHeader* assemblyTreeHeader = new NewBomHeader("NewBomAssemblyTreeCmd",
                                                          "NewBomModule",
                                                          "NewBomAssemblyTreeCmd",
                                                          (void*) NULL);
```

#### CreateToolbars() 新增：

```cpp
// Create Submenu "AssemblyTree" (获取装配树)
NewAccess(CATCmdContainer, pAssemblyTreeMenu, NewBomAssemblyTreeMnu);
SetAccessChild(pTopMenu, pAssemblyTreeMenu);

// Chinese title "获取装配树" using UCS-2: 获=0x83B7, 取=0x53D6, 装=0x88F6, 配=0x914D, 树=0x6811
static const CATUC2Bytes titleChars[] = {0x83B7, 0x53D6, 0x88F6, 0x914D, 0x6811, 0};
CATUnicodeString title;
title.BuildFromUCChar(titleChars, 5);
pAssemblyTreeMenu->SetTitle(title);

// Create command item
NewAccess(CATCmdStarter, pAssemblyTreeStarter, NewBomAssemblyTreeStr);
SetAccessCommand(pAssemblyTreeStarter, "NewBomAssemblyTreeCmd");
SetAccessChild(pAssemblyTreeMenu, pAssemblyTreeStarter);

// Sibling relationship: appears after NumberMgt
SetAccessNext(pNumberMgtMenu, pAssemblyTreeMenu);
```

菜单顺序：`编号管理` → `获取装配树` → `工具`

## 错误处理

| 场景 | 处理方式 | 显示信息 |
|------|----------|---------|
| 无打开文档 | 检测后显示提示 | "当前没有打开文档" |
| 当前是图纸文档 | 检测类型后提示 | "当前文档不是装配或零件" |
| 无法获取编辑器 | 记录日志，显示错误 | "无法获取 CATIA 编辑器" |
| Product 遍历失败 | 记录日志，继续 | 日志记录，树显示部分结构 |
| 对话框创建失败 | 记录日志，命令终止 | 日志记录 |

日志记录使用现有 `NewBomLog` 函数，标签为 `[ASSEMBLY]`。

## 测试验证

### 基本功能测试

1. 打开 CATIA，加载装配文档
2. 点击 NewBOM → 获取装配树
3. 确认对话框弹出，树结构正确显示

### 边界情况测试

1. 未打开文档时点击菜单
2. 仅打开零件文档（无父级）时点击
3. 空装配文档时点击

### 日志验证

查看 `%TEMP%\NewBomPlugin.log`，确认 `[ASSEMBLY]` 标签日志正常输出。

## 成功标准

- 菜单项正确显示在 NewBOM 下
- 点击后弹出对话框，显示树形结构
- 装配文档显示完整子组件层级
- 零件文档显示父级装配结构
- 错误情况有适当的提示信息
- 编译无错误，部署后正常运行