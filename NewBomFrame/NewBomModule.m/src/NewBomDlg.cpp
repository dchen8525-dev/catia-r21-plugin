// NewBomDlg.cpp
// CATIA CAA Dialog Implementation

#include "NewBomDlg.h"
#include "NewBomLog.h"
#include "CATDlgGridConstraints.h"
#include "CATUnicodeString.h"
#include <stdio.h>
#include <stdlib.h>

// Windows headers for WinHTTP
#include <windows.h>
#include <winhttp.h>

// Link WinHTTP library using pragma
#pragma comment(lib, "winhttp.lib")

// Implement the class
CATImplementClass(NewBomDlg,
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

// Helper function to convert ASCII/UTF-8 error message to CATUnicodeString
static CATUnicodeString FromAscii(const char* iText)
{
    CATUnicodeString text;
    // BuildFromMBChar may not exist, use alternative approach
    // Create UCS-2 buffer from ASCII characters
    int len = 0;
    while (iText[len] != '\0') len++;
    
    CATUC2Bytes* ucs2 = new CATUC2Bytes[len + 1];
    for (int i = 0; i < len; i++)
    {
        ucs2[i] = (CATUC2Bytes)(unsigned char)iText[i];
    }
    ucs2[len] = 0;
    text.BuildFromUCChar(ucs2, len);
    delete[] ucs2;
    return text;
}

//-----------------------------------------------------------------------------
// FetchPartCodeFromServer - HTTP Request Function
//-----------------------------------------------------------------------------
bool NewBomDlg::FetchPartCodeFromServer(
    char* oPartCode, 
    size_t iBufferSize)
{
    NewBomLog("HTTP", "FetchPartCodeFromServer START");
    
    // Initialize output
    if (oPartCode && iBufferSize > 0) oPartCode[0] = '\0';

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
        m_pStatusLabel->SetTitle(U(L"\x7F51\x7EDC\x521D\x59CB\x5316\x5931\x8D25"));  // "网络初始化失败"
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
        m_pStatusLabel->SetTitle(U(L"\x7F51\x7EDC\x8FDE\x63A5\x5931\x8D25\xFF0C\x8BF7\x68C0\x67E5\x670D\x52A1\x5668"));  // "网络连接失败，请检查服务器"
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
        m_pStatusLabel->SetTitle(U(L"\x521B\x5EFA\x8BF7\x6C42\x5931\x8D25"));  // "创建请求失败"
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    NewBomLog("HTTP", "WinHttpOpenRequest success: hRequest=%p", hRequest);

    // Set timeout values (5 seconds for connect, 10 seconds for receive)
    DWORD dwTimeout = 5000;  // 5 seconds
    WinHttpSetOption(hRequest, WINHTTP_OPTION_CONNECT_TIMEOUT, &dwTimeout, sizeof(dwTimeout));
    dwTimeout = 10000;  // 10 seconds
    WinHttpSetOption(hRequest, WINHTTP_OPTION_SEND_TIMEOUT, &dwTimeout, sizeof(dwTimeout));
    WinHttpSetOption(hRequest, WINHTTP_OPTION_RECEIVE_TIMEOUT, &dwTimeout, sizeof(dwTimeout));
    NewBomLog("HTTP", "Timeouts set: connect=5s, send=10s, receive=10s");

    // Step 4: Send request
    NewBomLog("HTTP", "Calling WinHttpSendRequest...");
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
            m_pStatusLabel->SetTitle(U(L"\x8BF7\x6C42\x8D85\x65F6\xFF0C\x8BF7\x7A0D\x540E\x91CD\x8BD5"));  // "请求超时，请稍后重试"
        else if (err == ERROR_WINHTTP_CANNOT_CONNECT)
            m_pStatusLabel->SetTitle(U(L"\x65E0\x6CD5\x8FDE\x63A5\x670D\x52A1\x5668\xFF0C\x8BF7\x68C0\x67E5\x670D\x52A1\x5668\x662F\x5426\x8FD0\x884C"));  // "无法连接服务器，请检查服务器是否运行"
        else
            m_pStatusLabel->SetTitle(U(L"\x7F51\x7EDC\x8FDE\x63A5\x5931\x8D25\xFF0C\x8BF7\x68C0\x67E5\x7F51\x7EDC"));  // "网络连接失败，请检查网络"
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
            m_pStatusLabel->SetTitle(U(L"\x54CD\x5E94\x8D85\x65F6\xFF0C\x8BF7\x7A0D\x540E\x91CD\x8BD5"));  // "响应超时，请稍后重试"
        else
            m_pStatusLabel->SetTitle(U(L"\x670D\x52A1\x5668\x54CD\x5E94\x5931\x8D25"));  // "服务器响应失败"
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
        m_pStatusLabel->SetTitle(U(L"\x670D\x52A1\x5668\x8FD4\x56DE\x9519\x8BEF"));  // "服务器返回错误"
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
        m_pStatusLabel->SetTitle(U(L"\x670D\x52A1\x5668\x8FD4\x56DE\x7A7A\x6570\x636E"));  // "服务器返回空数据"
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    // Allocate buffer and read data
    pBuffer = new char[dwSize + 1];
    if (!pBuffer)
    {
        m_pStatusLabel->SetTitle(U(L"\x5185\x5B58\x5206\x914D\x5931\x8D25"));  // "内存分配失败"
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    BOOL bReadResult = WinHttpReadData(hRequest, pBuffer, dwSize, &dwDownloaded);
    if (!bReadResult || dwDownloaded == 0)
    {
        NewBomLog("HTTP", "WinHttpReadData failed: %lu", GetLastError());
        m_pStatusLabel->SetTitle(U(L"\x8BFB\x53D6\x6570\x636E\x5931\x8D25"));  // "读取数据失败"
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
            m_pStatusLabel->SetTitle(U(L"\x670D\x52A1\x5668\x8FD4\x56DE\x9519\x8BEF"));  // "服务器返回错误"
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
        m_pStatusLabel->SetTitle(U(L"\x6570\x636E\x683C\x5F0F\x9519\x8BEF"));  // "数据格式错误"
        delete[] pBuffer;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    
    const char* partCodePos = strstr(dataPos, "\"partCode\":");
    if (!partCodePos)
    {
        m_pStatusLabel->SetTitle(U(L"\x6570\x636E\x683C\x5F0F\x9519\x8BEF"));  // "数据格式错误"
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
    
    // Set dialog title: UCS-2 for Chinese "零件取号"
    SetTitle(U(L"\x96F6\x4EF6\x53D6\x53F7"));

    // Row 0: Part Name
    CATDlgLabel* pPartNameLabel = new CATDlgLabel(this, "PartNameLabel");
    pPartNameLabel->SetTitle(U(L"\x96F6\x4EF6\x540D\x79F0"));  // "零件名称"
    pPartNameLabel->SetGridConstraints(0, 0, 1, 1, CATGRID_LEFT);
    
    CATDlgLabel* pColumnSpacer = new CATDlgLabel(this, "ColumnSpacer");
    pColumnSpacer->SetTitle(CATUnicodeString("    "));
    pColumnSpacer->SetGridConstraints(0, 1, 1, 1, CATGRID_CST_SIZE);
    
    m_pPartName = new CATDlgEditor(this, "PartNameEditor");
    m_pPartName->SetText(U(L"\x6F14\x793A\x96F6\x4EF6"));  // "演示零件"
    m_pPartName->SetVisibleTextWidth(28);
    m_pPartName->SetGridConstraints(0, 2, 1, 1, CATGRID_LEFT);

    // Row 1: Category
    CATDlgLabel* pCategoryLabel = new CATDlgLabel(this, "CategoryLabel");
    pCategoryLabel->SetTitle(U(L"\x96F6\x4EF6\x7C7B\x522B"));  // "零件类别"
    pCategoryLabel->SetGridConstraints(1, 0, 1, 1, CATGRID_LEFT);
    
    m_pCategory = new CATDlgCombo(this, "CategoryCombo", CATDlgCmbDropDown);
    CATUnicodeString categories[8];
    categories[0] = U(L"\x5EA7\x6905\x603B\x6210\x7C7B");  // "座椅总成类"
    categories[1] = U(L"\x9AA8\x67B6\x7C7B");  // "骨架类"
    categories[2] = U(L"\x53D1\x6CE1\x7C7B");  // "发泡类"
    categories[3] = U(L"\x5851\x6599\x7C7B");  // "塑料类"
    categories[4] = U(L"\x9762\x5957\x7C7B");  // "面套类"
    categories[5] = U(L"\x7EBF\x675F\x3001\x5F00\x5173\x7B49\x7535\x5668\x7C7B");  // "线束、开关等电器类"
    categories[6] = U(L"\x6ED1\x8F68\x3001\x8C03\x89D2\x5668\x3001\x5347\x9AD8\x6CF5\x7B49\x529F\x80FD\x4EF6");  // "滑轨、调角器、升高泵等功能件"
    categories[7] = U(L"\x5176\x5B83\x96F6\x4EF6");  // "其它零件"
    m_pCategory->SetLine(categories, 8);
    m_pCategory->SetSelect(0, 0);
    m_pCategory->SetGridConstraints(1, 2, 1, 1, CATGRID_LEFT);

    // Row 2: Material
    CATDlgLabel* pMaterialLabel = new CATDlgLabel(this, "MaterialLabel");
    pMaterialLabel->SetTitle(U(L"\x6750\x6599"));  // "材料"
    pMaterialLabel->SetGridConstraints(2, 0, 1, 1, CATGRID_LEFT);
    
    m_pMaterial = new CATDlgEditor(this, "MaterialEditor");
    m_pMaterial->SetText(U(L"\x94A2"));  // "钢"
    m_pMaterial->SetVisibleTextWidth(28);
    m_pMaterial->SetGridConstraints(2, 2, 1, 1, CATGRID_LEFT);

    // Row 3: Notes
    CATDlgLabel* pNotesLabel = new CATDlgLabel(this, "NotesLabel");
    pNotesLabel->SetTitle(U(L"\x5907\x6CE8"));  // "备注"
    pNotesLabel->SetGridConstraints(3, 0, 1, 1, CATGRID_LEFT);
    
    m_pNotes = new CATDlgEditor(this, "NotesEditor");
    m_pNotes->SetText(U(L"\x6F14\x793A\x5C5E\x6027\x6570\x636E"));  // "演示属性数据"
    m_pNotes->SetVisibleTextWidth(28);
    m_pNotes->SetGridConstraints(3, 2, 1, 1, CATGRID_LEFT);

    // Row 4: Part Number
    CATDlgLabel* pNumberLabel = new CATDlgLabel(this, "PartNumberLabel");
    pNumberLabel->SetTitle(U(L"\x96F6\x4EF6\x7F16\x53F7"));  // "零件编号"
    pNumberLabel->SetGridConstraints(4, 0, 1, 1, CATGRID_LEFT);
    
    m_pNumberResult = new CATDlgLabel(this, "PartNumberResult");
    m_pNumberResult->SetTitle(CATUnicodeString("-"));
    m_pNumberResult->SetGridConstraints(4, 2, 1, 1, CATGRID_LEFT);

    // Row 5: Get Number Button
    m_pNumberButton = new CATDlgPushButton(this, "GetNumberButton");
    m_pNumberButton->SetTitle(U(L"\x70B9\x51FB\x53D6\x53F7"));  // "点击取号"
    m_pNumberButton->SetGridConstraints(5, 2, 1, 1, CATGRID_CENTER);
    
    AddAnalyseNotificationCB(m_pNumberButton,
        m_pNumberButton->GetPushBActivateNotification(),
        (CATCommandMethod)&NewBomDlg::OnGetNumber,
        NULL);

    // Row 6: Status Label
    m_pStatusLabel = new CATDlgLabel(this, "StatusLabel");
    m_pStatusLabel->SetTitle(U(L"\x8BF7\x586B\x5199\x6F14\x793A\x5C5E\x6027"));  // "请填写演示属性"
    m_pStatusLabel->SetGridConstraints(6, 2, 1, 1, CATGRID_CENTER);

    // Set column resize properties
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
    
    bool success = FetchPartCodeFromServer(partCode, sizeof(partCode));
    
    if (success && partCode[0] != '\0')
    {
        NewBomLog("DIALOG", "Part code fetched successfully: %s", partCode);
        if (m_pNumberResult)
            m_pNumberResult->SetTitle(CATUnicodeString(partCode));
        if (m_pStatusLabel)
            m_pStatusLabel->SetTitle(U(L"\x53D6\x53F7\x6210\x529F"));  // "取号成功"
    }
    else
    {
        NewBomLog("DIALOG", "Part code fetch failed");
        if (m_pNumberResult)
            m_pNumberResult->SetTitle(CATUnicodeString("-"));
    }
    
    NewBomLog("DIALOG", "OnGetNumber END");
}