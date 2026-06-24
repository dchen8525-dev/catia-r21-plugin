# CATIA V5R21 NewBOM 插件

一个简洁的 CATIA CAA RADE 工作空间，用于 CATIA V5R21 64位插件开发。

## 目录

- [项目简介](#项目简介)
- [功能说明](#功能说明)
- [项目结构](#项目结构)
- [CATIA CAA 开发基础概念](#catia-caa-开发基础概念)
- [环境要求](#环境要求)
- [编译和部署](#编译和部署)
- [常见问题](#常见问题)
- [日志查看](#日志查看)

---

## 项目简介

本项目是一个 CATIA V5R21 CAA（Component Application Architecture）插件，用于在 CATIA 主菜单栏添加自定义菜单 "NewBOM"。

**适合人群：**
- CATIA CAA 开发新手
- 想学习 CATIA 插件开发流程的开发者
- 需要添加自定义菜单功能的 CATIA 用户

**学习目标：**
- 理解 CATIA CAA 插件的基本结构
- 学习如何添加自定义菜单
- 学习如何创建命令和对话框
- 学习 CATIA 的组件注册机制

---

## 功能说明

本插件在 CATIA 主菜单栏添加以下菜单结构：

```
NewBOM (顶级菜单)
  └─ 编号管理 (子菜单)
      └─ 零件取号 (命令 - 显示取号对话框)
  └─ 工具 (子菜单)
      └─ 关于本插件 (命令 - 显示版本信息)
```

**零件取号对话框功能：**
- 输入零件名称
- 选择零件类别（下拉列表）
- 输入材料
- 输入备注
- 点击按钮生成零件编号（格式：DLXX0001）

---

## 项目结构

```
catia_menu_plugin_r21/
│
├── build.bat                 # 编译脚本（生成 DLL）
├── deploy.bat                # 部署脚本（注册到 CATIA）
├── launch_catia.bat          # 启动 CATIA 脚本
│
├── NewBomFrame/              # 框架目录（Framework）
│   │
│   ├── IdentityCard/
│   │   └── IdentityCard.h    # 框架依赖声明（需要哪些 CATIA 库）
│   │
│   ├── CNext/
│   │   ├── code/
│   │   │   └── dictionary/
│   │   │       └── NewBomFrame.dico  # 字典文件（注册接口实现）
│   │   │
│   │   └── resources/
│   │       └── msgcatalog/
│   │           ├── NewBomModule.CATNls  # 菜单标题资源
│   │           └── NewBomHeader.CATNls  # 命令头资源
│   │
│   └── NewBomModule.m/       # 模块目录（Module）
│       │
│       ├── Imakefile.mk      # 编译配置（输出类型、链接库）
│       │
│       ├── LocalInterfaces/  # 头文件目录
│       │   ├── NewBomAddin.h      # Addin 类声明
│       │   ├── NewBomCmd.h        # 零件取号命令声明
│       │   ├── NewBomAboutCmd.h   # 关于命令声明
│       │   ├── NewBomDlg.h        # 对话框类声明
│       │   └── NewBomLog.h        # 日志工具声明
│       │
│       └── src/              # 源文件目录
│           ├── NewBomAddin.cpp     # Addin 实现（创建菜单）
│           ├── NewBomCmd.cpp       # 零件取号命令实现
│           ├── NewBomAboutCmd.cpp  # 关于命令实现
│           ├── NewBomDlg.cpp       # 对话框实现
│           ├── NewBomLog.cpp       # 日志工具实现
│           └── NewBomModule.cpp    # 模块入口点
│
└── win_b64/                  # 编译输出目录
    └── code/
        ├── bin/
        │   └── NewBomModule.dll  # 编译生成的插件 DLL
        └── dictionary/
            └── NewBomFrame.dico   # 复制的字典文件
```

---

## CATIA CAA 开发基础概念

### 1. 框架（Framework）和模块（Module）

**框架（Framework）：**
- 组织代码的基本单位
- 包含一个或多个模块
- 定义与其他框架的依赖关系
- 本项目的框架名：`NewBomFrame`

**模块（Module）：**
- 框架内的实际代码单元
- 包含源代码文件（.cpp, .h）
- 编译后生成 DLL
- 本项目的模块名：`NewBomModule.m`

### 2. Addin（扩展）

Addin 是 CATIA 的扩展机制，用于：
- 在现有工作台添加功能
- 不需要创建新工作台
- 可以在任何工作台使用

**关键接口：`CATIAfrGeneralWksAddin`**
- 通用工作台扩展接口
- 在 CATIA 启动时自动加载
- 添加菜单到主菜单栏

**关键方法：**
```cpp
void CreateCommands();           // 创建命令头
CATCmdContainer* CreateToolbars(); // 创建菜单结构
```

### 3. 命令头（Command Header）

命令头是菜单项和命令类之间的桥梁：

```cpp
// 创建命令头示例
CATCommandHeader* header = new NewBomHeader(
    "NewBomCmd",         // 头标识符（与菜单关联）
    "NewBomModule",      // DLL 名称
    "NewBomCmd",         // 命令类名
    NULL                 // 参数（可选）
);
```

### 4. 字典文件（Dictionary）

字典文件告诉 CATIA：
- 哪个接口由哪个类实现
- 实现类在哪个 DLL 中

**文件格式：**
```
实现类名    接口名    DLL名
NewBomGeneralWksAddin  CATIAfrGeneralWksAddin  libNewBomModule
```

### 5. 菜单创建宏

```cpp
// 创建菜单容器
NewAccess(CATCmdContainer, pTopMenu, NewBomTopMnu);

// 设置父子关系（子菜单放入父菜单）
SetAccessChild(pMenuBar, pTopMenu);

// 设置兄弟关系（同级菜单）
SetAccessNext(pNumberMgtMenu, pToolsMenu);

// 链接菜单项到命令头
SetAccessCommand(pStarter, "NewBomCmd");

// 合并菜单到 CATIA 主菜单栏
SetAddinMenu(pAddinRoot, pMenuBar);
```

### 6. 命令生命周期

```
用户点击菜单
    ↓
CATIA 创建命令实例
    ↓
调用 Activate() → 命令开始执行
    ↓
用户交互（填写对话框）
    ↓
用户关闭对话框
    ↓
调用 RequestDelayedDestruction() → 命令销毁
```

### 7. 依赖库说明

| 库名 | 作用 |
|-----|------|
| CATApplicationFrame | 应用框架、命令系统、菜单系统 |
| JS0GROUP | CATBaseUnknown 基类、对象管理 |
| JS0FM | 字符串处理（CATUnicodeString） |
| DI0PANV2 | 对话框系统、各种控件 |

---

## 环境要求

### 必需软件

1. **CATIA V5R21** - 已安装并正常运行
2. **CAA RADE V5R21** - CATIA 开发环境
3. **Visual Studio** - 用于编译（RADE 集成）

### 环境变量

RADE 安装后会自动配置以下环境：

```
CATIA 安装路径：C:\Program Files\Dassault Systemes\B21
RADE 安装路径：C:\Program Files (x86)\Dassault Systemes\B21
环境配置目录：C:\ProgramData\DassaultSystemes\CATEnv
```

### 检查环境

运行以下命令检查 RADE 环境：

```cmd
# 检查 CATSTART 是否存在
dir "C:\Program Files (x86)\Dassault Systemes\B21\intel_a\code\bin\CATSTART.exe"
```

---

## 编译和部署

### 步骤 1：编译插件

```cmd
build.bat
```

**编译过程：**
1. RADE 扫描项目结构
2. 编译所有 .cpp 文件
3. 链接生成 DLL
4. 创建运行时视图

**输出：**
- `win_b64/code/bin/NewBomModule.dll` - 插件 DLL

### 步骤 2：部署插件

```cmd
# 需要以管理员权限运行
deploy.bat
```

**部署过程：**
1. 复制 DLL 到 CATIA 目录
2. 复制字典文件
3. 复制资源文件
4. 注册运行时视图

### 步骤 3：启动 CATIA

```cmd
launch_catia.bat
```

启动后，CATIA 主菜单栏会出现 "NewBOM" 菜单。

### 步骤 4：验证功能

1. 点击 "NewBOM" → "编号管理" → "零件取号"
2. 填写零件信息
3. 点击 "点击取号" 按钮
4. 查看生成的编号

---

## 常见问题

### Q1: 编译后 DLL 不生成？

**原因：** Imakefile.mk 格式问题

**解决：** 确保 `LINK_WITH` 行格式正确：
```mk
LINK_WITH=CATApplicationFrame JS0GROUP JS0FM DI0PANV2
```

### Q2: 菜单不显示？

**检查步骤：**
1. 确认 deploy.bat 成功运行
2. 检查字典文件是否正确复制
3. 查看 CATIA 启动日志

### Q3: 中文显示为乱码？

**原因：** NLS 文件编码问题

**解决：** NLS 文件必须使用 GBK 编码（中文 Windows）

### Q4: 编译警告 C4819？

**原因：** 源文件包含中文字符

**解决：** 使用纯英文注释，或保存为 UTF-8 with BOM

### Q5: 链接错误 LNK2019？

**原因：** 缺少链接库

**解决：** 检查 IdentityCard.h 和 Imakefile.mk 的依赖配置

---

## 日志查看

插件运行时会记录详细日志，便于调试。

### 日志位置

```
%LOCALAPPDATA%\DassaultSystemes\CATTemp\NewBomPlugin.log
```

或

```
%TEMP%\NewBomPlugin.log
```

### 日志格式

```
2024-06-24 10:30:27.123 pid=1234 tid=5678 [ADDIN] CreateCommands() START
```

各字段含义：
- 时间戳（精确到毫秒）
- 进程ID（pid）
- 线程ID（tid）
- 阶段标识（ADDIN、COMMAND、ABOUT、DIALOG）
- 日志消息

### 日志阶段

| 阶段 | 含义 |
|-----|------|
| MODULE | DLL 加载/卸载 |
| ADDIN | Addin 创建、菜单构建 |
| COMMAND | 零件取号命令执行 |
| ABOUT | 关于对话框 |
| DIALOG | 取号对话框操作 |

### 使用 Visual Studio 查看日志

调试时，日志会输出到 VS 的"输出"窗口（OutputDebugString）。

---

## 文件编码注意事项

### 重要提醒

CATIA CAA 开发中，文件编码非常关键：

| 文件类型 | 建议编码 |
|---------|---------|
| .cpp/.h 源文件 | UTF-8 或 ANSI（纯英文注释） |
| .CATNls 资源文件 | GBK（中文 Windows） |
| .dico 字典文件 | ANSI |
| .mk 构建文件 | ANSI |

### 为什么使用英文注释？

在 Imakefile.mk 和源文件中使用英文注释的原因：
1. 避免 C4819 编译警告
2. 防止编码导致的编译失败
3. 确保跨平台兼容性

---

## 学习建议

### 推荐阅读顺序

1. `IdentityCard.h` - 了解框架依赖
2. `NewBomFrame.dico` - 了解接口注册
3. `NewBomAddin.h/cpp` - 学习菜单创建
4. `NewBomCmd.h/cpp` - 学习命令实现
5. `NewBomDlg.h/cpp` - 学习对话框
6. `Imakefile.mk` - 学习编译配置

### 关键代码位置

| 功能 | 文件 | 关键代码 |
|-----|------|---------|
| 菜单创建 | NewBomAddin.cpp | CreateToolbars() |
| 命令注册 | NewBomAddin.cpp | CreateCommands() |
| 命令执行 | NewBomCmd.cpp | Activate() |
| 对话框创建 | NewBomDlg.cpp | BuildDialog() |

---

## 参考资料

- [CATIA CAA 官方文档](https://catiadev.dsdeveloper.com/)
- [CAA RADE 开发指南](https://catiadev.dsdeveloper.com/documentation/)

---

## 作者信息

本项目作为 CATIA CAA 开发学习示例，适合新手入门学习。

**GitHub：** https://github.com/dchen8525-dev/catia-r21-plugin