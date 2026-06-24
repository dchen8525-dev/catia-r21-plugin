# HTTP API 获取零件编号设计文档

## 目标

修改"零件取号"对话框功能，点击取号按钮时调用 HTTP API 获取零件编号，替代当前的本地生成方式。

## API 信息

- **URL**: `http://localhost:8080/api/getCode`
- **方法**: GET
- **返回格式**: 
  ```json
  {
    "code": 200,
    "message": "success",
    "data": {
      "partCode": "DLXX0016"
    }
  }
  ```
- **提取字段**: `data.partCode`

## 修改文件清单

| 文件 | 修改内容 |
|------|---------|
| `NewBomDlg.cpp` | 添加 WinHTTP 请求函数，修改 `OnGetNumber()` |
| `NewBomDlg.h` | 添加 `FetchPartCodeFromServer()` 方法声明 |
| `Imakefile.mk` | 添加 `winhttp.lib` 链接 |

## 技术方案

使用 **WinHTTP API**（Windows 原生 HTTP 客户端库）。

### 选择理由
- Windows 原生 API，无需额外依赖
- CATIA CAA 项目可直接使用
- 支持完整的 HTTP 功能
- 代码清晰易于维护

## HTTP 请求流程

```
用户点击"取号"按钮
    ↓
调用 WinHttpOpen() 初始化会话
    ↓
调用 WinHttpConnect() 连接 localhost:8080
    ↓
调用 WinHttpOpenRequest() 创建 GET /api/getCode
    ↓
调用 WinHttpSendRequest() 发送请求
    ↓
调用 WinHttpReceiveResponse() 接收响应
    ↓
读取响应数据
    ↓
解析 JSON，提取 data.partCode
    ↓
成功：显示编号
失败：显示错误信息
```

## 新增函数

### FetchPartCodeFromServer

```cpp
/**
 * 从服务器获取零件编号
 * 
 * @param oPartCode    输出缓冲区，存储获取的编号
 * @param bufferSize   输出缓冲区大小
 * @param oErrorMsg    输出缓冲区，存储错误信息
 * @param errorSize    错误信息缓冲区大小
 * @return true 成功获取编号，false 失败
 */
static bool FetchPartCodeFromServer(
    char* oPartCode, 
    size_t bufferSize,
    char* oErrorMsg, 
    size_t errorSize);
```

## JSON 解析方案

使用简单字符串查找方式解析 JSON（无需第三方库）：
- 查找 `"partCode":` 字段
- 提取引号内的值

## 错误处理

| 场景 | 显示信息 |
|------|---------|
| 网络连接失败 | "网络连接失败，请检查服务器是否运行" |
| 请求超时 | "请求超时，请稍后重试" |
| 服务器返回错误 (code != 200) | "服务器错误: {message}" |
| JSON 解析失败 | "响应数据格式错误" |
| HTTP 状态码非 200 | "HTTP 错误: {状态码}" |

## 超时设置

使用 WinHTTP 默认超时设置，无需手动配置。

## 链接配置

在 `Imakefile.mk` 中添加：
```mk
LINK_WITH=CATApplicationFrame JS0GROUP JS0FM DI0PANV2 winhttp
```

## 实现步骤

1. 更新 `Imakefile.mk` 添加 winhttp 链接
2. 在 `NewBomDlg.h` 添加函数声明
3. 在 `NewBomDlg.cpp` 实现 WinHTTP 请求函数
4. 修改 `OnGetNumber()` 调用新函数
5. 添加错误处理和状态显示
6. 编译测试

## 成功标准

- 点击取号按钮成功调用 API
- 正确显示返回的 partCode
- 网络失败时显示详细错误信息
- 编译无错误