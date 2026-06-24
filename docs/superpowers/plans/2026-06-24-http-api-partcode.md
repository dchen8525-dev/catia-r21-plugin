# HTTP API 零件编号获取功能实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 修改零件取号对话框，点击取号按钮时调用 HTTP API `http://localhost:8080/api/getCode` 获取零件编号。

**Architecture:** 使用 WinHTTP API 发送 GET 请求，解析 JSON 响应提取 `data.partCode`，替换本地编号生成逻辑。

**Tech Stack:** C++, WinHTTP API, CATIA CAA RADE V5R21

---

## 文件结构

| 文件 | 操作 | 说明 |
|------|------|------|
| `NewBomFrame/NewBomModule.m/Imakefile.mk` | 修改 | 添加 winhttp 链接库 |
| `NewBomFrame/NewBomModule.m/LocalInterfaces/NewBomDlg.h` | 修改 | 添加 HTTP 函数声明 |
| `NewBomFrame/NewBomModule.m/src/NewBomDlg.cpp` | 修改 | 实现 WinHTTP 请求，修改 OnGetNumber |

---

## Task 1: 更新 Imakefile.mk 添加 winhttp 链接

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/Imakefile.mk`

- [ ] **Step 1: 修改 Imakefile.mk 添加 winhttp**

修改 LINK_WITH 行，添加 winhttp：

```mk
BUILT_OBJECT_TYPE=SHARED LIBRARY
LINK_WITH=CATApplicationFrame JS0GROUP JS0FM DI0PANV2 winhttp
```

- [ ] **Step 2: 验证文件内容**

查看修改后的文件确认正确。

---

## Task 2: 更新 NewBomDlg.h 添加函数声明

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/LocalInterfaces/NewBomDlg.h`

- [ ] **Step 1: 在 private 区域添加 HTTP 函数声明**

在 `OnGetNumber` 方法声明后添加：

```cpp
    /**
     * Handle "Get Number" button click
     */
    void OnGetNumber(CATCommand* iCmd,
                     CATNotification* iNotif,
                     CATCommandClientData iClientData);

    /**
     * Fetch part code from HTTP server
     * 
     * Uses WinHTTP to call http://localhost:8080/api/getCode
     * Parses JSON response and extracts data.partCode
     * 
     * @param oPartCode    Output buffer for part code (e.g., "DLXX0016")
     * @param iBufferSize  Size of output buffer
     * @param oErrorMsg    Output buffer for error message
     * @param iErrorSize   Size of error buffer
     * @return true if successful, false on error
     */
    static bool FetchPartCodeFromServer(
        char* oPartCode, 
        size_t iBufferSize,
        char* oErrorMsg, 
        size_t iErrorSize);
```

- [ ] **Step 2: 验证头文件语法正确**

确认声明位置和格式正确。

---

## Task 3: 实现 WinHTTP 请求函数

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/src/NewBomDlg.cpp`

- [ ] **Step 1: 添加 WinHTTP 头文件和函数实现**

在文件开头添加 Windows 和 WinHTTP 头文件：

```cpp
// NewBomDlg.cpp
// CATIA CAA Hello World Dialog Implementation

#include "NewBomDlg.h"
#include "NewBomLog.h"
#include "CATDlgGridConstraints.h"
#include "CATUnicodeString.h"
#include <stdio.h>
#include <stdlib.h>

// Windows headers for WinHTTP
#include <windows.h>
#include <winhttp.h>
```

- [ ] **Step 2: 在 GetNextPartNumber 函数后添加 FetchPartCodeFromServer 实现**

添加完整的 WinHTTP 请求函数：

```cpp
//-----------------------------------------------------------------------------
// FetchPartCodeFromServer - HTTP Request Function
//-----------------------------------------------------------------------------
static bool FetchPartCodeFromServer(
    char* oPartCode, 
    size_t iBufferSize,
    char* oErrorMsg, 
    size_t iErrorSize)
{
    NewBomLog("HTTP", "FetchPartCodeFromServer START");
    
    // Initialize output
    if (oPartCode && iBufferSize > 0) oPartCode[0] = '\0';
    if (oErrorMsg && iErrorSize > 0) oErrorMsg[0] = '\0';

    // Step 1: Initialize WinHTTP session
    HINTERNET hSession = WinHttpOpen(
        L"CATIA NewBOM Plugin/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    
    if (!hSession)
    {
        NewBomLog("HTTP", "WinHttpOpen failed: %lu", GetLastError());
        if (oErrorMsg) sprintf_s(oErrorMsg, iErrorSize, "网络初始化失败");
        return false;
    }
    NewBomLog("HTTP", "WinHttpOpen success: hSession=%p", hSession);

    // Step 2: Connect to server
    HINTERNET hConnect = WinHttpConnect(
        hSession,
        L"localhost",
        8080,
        0);
    
    if (!hConnect)
    {
        NewBomLog("HTTP", "WinHttpConnect failed: %lu", GetLastError());
        if (oErrorMsg) sprintf_s(oErrorMsg, iErrorSize, "网络连接失败，请检查服务器是否运行");
        WinHttpCloseHandle(hSession);
        return false;
    }
    NewBomLog("HTTP", "WinHttpConnect success: hConnect=%p", hConnect);

    // Step 3: Create request
    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",
        L"/api/getCode",
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0);
    
    if (!hRequest)
    {
        NewBomLog("HTTP", "WinHttpOpenRequest failed: %lu", GetLastError());
        if (oErrorMsg) sprintf_s(oErrorMsg, iErrorSize, "创建请求失败");
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    NewBomLog("HTTP", "WinHttpOpenRequest success: hRequest=%p", hRequest);

    // Step 4: Send request
    BOOL bSendResult = WinHttpSendRequest(
        hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0);
    
    if (!bSendResult)
    {
        NewBomLog("HTTP", "WinHttpSendRequest failed: %lu", GetLastError());
        DWORD err = GetLastError();
        if (err == ERROR_WINHTTP_TIMEOUT)
            sprintf_s(oErrorMsg, iErrorSize, "请求超时，请稍后重试");
        else if (err == ERROR_WINHTTP_CANNOT_CONNECT)
            sprintf_s(oErrorMsg, iErrorSize, "无法连接服务器，请检查服务器是否运行");
        else
            sprintf_s(oErrorMsg, iErrorSize, "发送请求失败 (错误码: %lu)", err);
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    NewBomLog("HTTP", "WinHttpSendRequest success");

    // Step 5: Receive response
    BOOL bReceiveResult = WinHttpReceiveResponse(hRequest, NULL);
    if (!bReceiveResult)
    {
        NewBomLog("HTTP", "WinHttpReceiveResponse failed: %lu", GetLastError());
        DWORD err = GetLastError();
        if (err == ERROR_WINHTTP_TIMEOUT)
            sprintf_s(oErrorMsg, iErrorSize, "响应超时，请稍后重试");
        else
            sprintf_s(oErrorMsg, iErrorSize, "接收响应失败 (错误码: %lu)", err);
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    NewBomLog("HTTP", "WinHttpReceiveResponse success");

    // Step 6: Check HTTP status code
    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &statusCode,
        &statusCodeSize,
        WINHTTP_NO_HEADER_INDEX);
    
    NewBomLog("HTTP", "HTTP Status Code: %lu", statusCode);
    if (statusCode != 200)
    {
        sprintf_s(oErrorMsg, iErrorSize, "HTTP 错误: %lu", statusCode);
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    // Step 7: Read response data
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    char* pBuffer = NULL;
    
    // Query available data size
    WinHttpQueryDataAvailable(hRequest, &dwSize);
    NewBomLog("HTTP", "Data available: %lu bytes", dwSize);
    
    if (dwSize == 0)
    {
        sprintf_s(oErrorMsg, iErrorSize, "服务器返回空数据");
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    // Allocate buffer and read data
    pBuffer = new char[dwSize + 1];
    if (!pBuffer)
    {
        sprintf_s(oErrorMsg, iErrorSize, "内存分配失败");
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    BOOL bReadResult = WinHttpReadData(hRequest, pBuffer, dwSize, &dwDownloaded);
    if (!bReadResult || dwDownloaded == 0)
    {
        NewBomLog("HTTP", "WinHttpReadData failed: %lu", GetLastError());
        sprintf_s(oErrorMsg, iErrorSize, "读取数据失败");
        delete[] pBuffer;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    pBuffer[dwDownloaded] = '\0';
    NewBomLog("HTTP", "Response data: %s", pBuffer);

    // Step 8: Parse JSON response
    // Expected format: {"code":200,"message":"success","data":{"partCode":"DLXX0016"}}
    
    // Check for "code" field value
    const char* codePos = strstr(pBuffer, "\"code\":");
    if (codePos)
    {
        int apiCode = 0;
        codePos += 7; // Skip "code":
        while (*codePos == ' ' || *codePos == '\t') codePos++; // Skip whitespace
        sscanf_s(codePos, "%d", &apiCode);
        NewBomLog("HTTP", "API code field: %d", apiCode);
        
        if (apiCode != 200)
        {
            // Extract message for error
            const char* msgPos = strstr(pBuffer, "\"message\":");
            if (msgPos)
            {
                msgPos += 10; // Skip "message":
                while (*msgPos == ' ' || *msgPos == '\t' || *msgPos == '"') msgPos++;
                char msg[128] = {0};
                int i = 0;
                while (*msgPos != '"' && *msgPos != ',' && *msgPos != '}' && i < 127)
                {
                    msg[i++] = *msgPos++;
                }
                sprintf_s(oErrorMsg, iErrorSize, "服务器错误: %s", msg);
            }
            else
            {
                sprintf_s(oErrorMsg, iErrorSize, "服务器返回错误 (code: %d)", apiCode);
            }
            delete[] pBuffer;
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }
    }
    
    // Extract partCode from data
    const char* dataPos = strstr(pBuffer, "\"data\":");
    if (!dataPos)
    {
        sprintf_s(oErrorMsg, iErrorSize, "响应数据格式错误: 缺少 data 字段");
        delete[] pBuffer;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    const char* partCodePos = strstr(dataPos, "\"partCode\":");
    if (!partCodePos)
    {
        sprintf_s(oErrorMsg, iErrorSize, "响应数据格式错误: 缺少 partCode 字段");
        delete[] pBuffer;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    partCodePos += 11; // Skip "partCode":
    while (*partCodePos == ' ' || *partCodePos == '\t' || *partCodePos == '"') partCodePos++;
    
    // Extract the part code value
    int i = 0;
    while (*partCodePos != '"' && *partCodePos != ',' && *partCodePos != '}' && i < (int)iBufferSize - 1)
    {
        oPartCode[i++] = *partCodePos++;
    }
    oPartCode[i] = '\0';
    
    NewBomLog("HTTP", "Extracted partCode: %s", oPartCode);

    // Cleanup
    delete[] pBuffer;
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    NewBomLog("HTTP", "FetchPartCodeFromServer END - success");
    return true;
}
```

---

## Task 4: 修改 OnGetNumber 函数调用 HTTP API

**Files:**
- Modify: `NewBomFrame/NewBomModule.m/src/NewBomDlg.cpp`

- [ ] **Step 1: 修改 OnGetNumber 函数实现**

替换现有的 OnGetNumber 函数：

```cpp
void NewBomDlg::OnGetNumber(CATCommand* iCmd,
                                CATNotification* iNotif,
                                CATCommandClientData iClientData)
{
    NewBomLog("DIALOG", "OnGetNumber START");
    
    const int categoryIndex = m_pCategory ? m_pCategory->GetSelect() : -1;
    NewBomLog("DIALOG", "categoryIndex=%d", categoryIndex);
    
    // Call HTTP API to fetch part code
    char partCode[64] = {0};
    char errorMsg[256] = {0};
    
    bool success = FetchPartCodeFromServer(partCode, sizeof(partCode), errorMsg, sizeof(errorMsg));
    
    if (success && partCode[0] != '\0')
    {
        NewBomLog("DIALOG", "Part code fetched successfully: %s", partCode);
        if (m_pNumberResult)
            m_pNumberResult->SetTitle(CATUnicodeString(partCode));
        if (m_pStatusLabel)
            m_pStatusLabel->SetTitle(U(L"\x53D6\x53F7\x6210\x529F")); // "取号成功"
    }
    else
    {
        NewBomLog("DIALOG", "Part code fetch failed: %s", errorMsg);
        if (m_pNumberResult)
            m_pNumberResult->SetTitle(CATUnicodeString("-"));
        if (m_pStatusLabel)
        {
            // Display error message
            CATUnicodeString errorStr;
            errorStr.BuildFromMBChar(errorMsg);
            m_pStatusLabel->SetTitle(errorStr);
        }
    }
    
    NewBomLog("DIALOG", "OnGetNumber END");
}
```

---

## Task 5: 编译测试

- [ ] **Step 1: 运行 build.bat 编译项目**

运行编译脚本，检查是否有编译错误。

- [ ] **Step 2: 检查编译输出**

确认 DLL 生成成功：
- `win_b64/code/bin/NewBomModule.dll`

- [ ] **Step 3: 运行 deploy.bat 部署**

部署到 CATIA 环境。

---

## Task 6: 提交代码

- [ ] **Step 1: Git 提交**

```bash
git add NewBomFrame/NewBomModule.m/Imakefile.mk
git add NewBomFrame/NewBomModule.m/LocalInterfaces/NewBomDlg.h
git add NewBomFrame/NewBomModule.m/src/NewBomDlg.cpp
git commit -m "feat: Add HTTP API call for part code fetching

- Add FetchPartCodeFromServer using WinHTTP
- Call http://localhost:8080/api/getCode
- Parse JSON response and display partCode
- Show detailed error messages on failure
- Update Imakefile.mk to link winhttp.lib"
```

- [ ] **Step 2: 推送到 GitHub**

```bash
git push
```

---

## 自检清单

- [x] Spec 覆盖：所有设计文档要求已实现
- [x] 无占位符：所有代码完整无 TBD/TODO
- [x] 类型一致性：函数签名匹配声明和实现
- [x] 错误处理：网络失败、服务器错误、JSON 解析失败都有处理
- [x] 文件路径：所有路径精确指定

---

## 测试验证

启动 CATIA 后：
1. 点击 "NewBOM" → "编号管理" → "零件取号"
2. 确保后端服务器运行在 localhost:8080
3. 点击 "点击取号" 按钮
4. 验证编号显示正确（如 DLXX0016）
5. 测试服务器关闭时的错误提示