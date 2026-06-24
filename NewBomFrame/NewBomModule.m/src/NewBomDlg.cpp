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

// Implement the class
CATImplementClass(NewBomDlg,
                  Implementation,
                  CATDlgDialog,
                  CATNull);

static CATUnicodeString U(const wchar_t* iText)
{
    CATUnicodeString text;
    text.BuildFromWChar(iText);
    return text;
}

//-----------------------------------------------------------------------------
// FetchPartCodeFromServer - HTTP Request Function
//-----------------------------------------------------------------------------
bool NewBomDlg::FetchPartCodeFromServer(
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

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
NewBomDlg::NewBomDlg(CATDialog* iParent,
                              const CATString& iObjectName)
    : CATDlgDialog(iParent,
                   iObjectName,
                   CATDlgWndModal | CATDlgWndBtnClose | CATDlgGridLayout)
{
    NewBomLog("DIALOG", "constructor this=%p parent=%p", this, iParent);
    m_pPartName = NULL;
    m_pCategory = NULL;
    m_pMaterial = NULL;
    m_pNotes = NULL;
    m_pNumberResult = NULL;
    m_pNumberButton = NULL;
    m_pStatusLabel = NULL;

    BuildDialog();
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
NewBomDlg::~NewBomDlg()
{
    NewBomLog("DIALOG", "destructor this=%p", this);
}

//-----------------------------------------------------------------------------
// BuildDialog - Create and layout dialog controls
//-----------------------------------------------------------------------------
void NewBomDlg::BuildDialog()
{
    NewBomLog("DIALOG", "BuildDialog entered this=%p", this);
    // Set dialog title
    SetTitle(U(L"\x96F6\x4EF6\x53D6\x53F7"));

    CATDlgLabel* pPartNameLabel = new CATDlgLabel(this, "PartNameLabel");
    pPartNameLabel->SetTitle(U(L"\x96F6\x4EF6\x540D\x79F0"));
    pPartNameLabel->SetGridConstraints(0, 0, 1, 1, CATGRID_LEFT);
    CATDlgLabel* pColumnSpacer = new CATDlgLabel(this, "ColumnSpacer");
    pColumnSpacer->SetTitle(CATUnicodeString("    "));
    pColumnSpacer->SetGridConstraints(0, 1, 1, 1, CATGRID_CST_SIZE);
    m_pPartName = new CATDlgEditor(this, "PartNameEditor");
    m_pPartName->SetText(U(L"\x6F14\x793A\x96F6\x4EF6"));
    m_pPartName->SetVisibleTextWidth(28);
    m_pPartName->SetGridConstraints(0, 2, 1, 1, CATGRID_LEFT);

    CATDlgLabel* pCategoryLabel = new CATDlgLabel(this, "CategoryLabel");
    pCategoryLabel->SetTitle(U(L"\x96F6\x4EF6\x7C7B\x522B"));
    pCategoryLabel->SetGridConstraints(1, 0, 1, 1, CATGRID_LEFT);
    m_pCategory = new CATDlgCombo(this, "CategoryCombo", CATDlgCmbDropDown);
    CATUnicodeString categories[8];
    categories[0] = U(L"\x5EA7\x6905\x603B\x6210\x7C7B");
    categories[1] = U(L"\x9AA8\x67B6\x7C7B");
    categories[2] = U(L"\x53D1\x6CE1\x7C7B");
    categories[3] = U(L"\x5851\x6599\x7C7B");
    categories[4] = U(L"\x9762\x5957\x7C7B");
    categories[5] = U(L"\x7EBF\x675F\x3001\x5F00\x5173\x7B49\x7535\x5668\x7C7B");
    categories[6] = U(L"\x6ED1\x8F68\x3001\x8C03\x89D2\x5668\x3001\x5347\x9AD8\x6CF5\x7B49\x529F\x80FD\x4EF6");
    categories[7] = U(L"\x5176\x4ED6\x96F6\x4EF6");
    m_pCategory->SetLine(categories, 8);
    m_pCategory->SetSelect(0, 0);
    m_pCategory->SetGridConstraints(1, 2, 1, 1, CATGRID_LEFT);

    CATDlgLabel* pMaterialLabel = new CATDlgLabel(this, "MaterialLabel");
    pMaterialLabel->SetTitle(U(L"\x6750\x6599"));
    pMaterialLabel->SetGridConstraints(2, 0, 1, 1, CATGRID_LEFT);
    m_pMaterial = new CATDlgEditor(this, "MaterialEditor");
    m_pMaterial->SetText(U(L"\x94A2"));
    m_pMaterial->SetVisibleTextWidth(28);
    m_pMaterial->SetGridConstraints(2, 2, 1, 1, CATGRID_LEFT);

    CATDlgLabel* pNotesLabel = new CATDlgLabel(this, "NotesLabel");
    pNotesLabel->SetTitle(U(L"\x5907\x6CE8"));
    pNotesLabel->SetGridConstraints(3, 0, 1, 1, CATGRID_LEFT);
    m_pNotes = new CATDlgEditor(this, "NotesEditor");
    m_pNotes->SetText(U(L"\x6F14\x793A\x5C5E\x6027\x6570\x636E"));
    m_pNotes->SetVisibleTextWidth(28);
    m_pNotes->SetGridConstraints(3, 2, 1, 1, CATGRID_LEFT);

    CATDlgLabel* pNumberLabel = new CATDlgLabel(this, "PartNumberLabel");
    pNumberLabel->SetTitle(U(L"\x96F6\x4EF6\x7F16\x53F7"));
    pNumberLabel->SetGridConstraints(4, 0, 1, 1, CATGRID_LEFT);
    m_pNumberResult = new CATDlgLabel(this, "PartNumberResult");
    m_pNumberResult->SetTitle(CATUnicodeString("-"));
    m_pNumberResult->SetGridConstraints(4, 2, 1, 1, CATGRID_LEFT);

    m_pNumberButton = new CATDlgPushButton(this, "GetNumberButton");
    m_pNumberButton->SetTitle(U(L"\x70B9\x51FB\x53D6\x53F7"));
    m_pNumberButton->SetGridConstraints(5, 2, 1, 1, CATGRID_CENTER);
    AddAnalyseNotificationCB(m_pNumberButton,
        m_pNumberButton->GetPushBActivateNotification(),
        (CATCommandMethod)&NewBomDlg::OnGetNumber,
        NULL);

    m_pStatusLabel = new CATDlgLabel(this, "StatusLabel");
    m_pStatusLabel->SetTitle(U(L"\x8BF7\x586B\x5199\x6F14\x793A\x5C5E\x6027"));
    m_pStatusLabel->SetGridConstraints(6, 2, 1, 1, CATGRID_CENTER);

    SetGridColumnResizable(0, 0);
    SetGridColumnResizable(1, 0);
    SetGridColumnResizable(2, 0);

    NewBomLog("DIALOG", "BuildDialog completed name=%p category=%p number=%p",
                  m_pPartName, m_pCategory, m_pNumberButton);
}

//-----------------------------------------------------------------------------
// OnGetNumber - Handle "Get Number" button click
//-----------------------------------------------------------------------------
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