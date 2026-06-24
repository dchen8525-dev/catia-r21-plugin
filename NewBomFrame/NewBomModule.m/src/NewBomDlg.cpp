// NewBomDlg.cpp
// CATIA CAA Hello World Dialog Implementation

#include "NewBomDlg.h"
#include "NewBomLog.h"
#include "CATDlgGridConstraints.h"
#include "CATUnicodeString.h"
#include <stdio.h>
#include <stdlib.h>

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

static int GetNextPartNumber(char* oPartNumber, size_t iBufferSize)
{
    const char* appData = getenv("APPDATA");
    if (!appData || !oPartNumber || iBufferSize < 9)
        return 0;

    char sequencePath[1024];
    sprintf_s(sequencePath, sizeof(sequencePath),
              "%s\\DassaultSystemes\\CATSettings\\HelloWorldNumber.seq",
              appData);

    int lastNumber = 0;
    FILE* sequenceFile = NULL;
    if (fopen_s(&sequenceFile, sequencePath, "r") == 0 && sequenceFile)
    {
        fscanf_s(sequenceFile, "%d", &lastNumber);
        fclose(sequenceFile);
    }

    const int nextNumber = lastNumber + 1;
    if (nextNumber > 9999)
        return 0;

    sequenceFile = NULL;
    if (fopen_s(&sequenceFile, sequencePath, "w") != 0 || !sequenceFile)
        return 0;
    fprintf(sequenceFile, "%d\n", nextNumber);
    fclose(sequenceFile);

    sprintf_s(oPartNumber, iBufferSize, "DLXX%04d", nextNumber);
    return 1;
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

void NewBomDlg::OnGetNumber(CATCommand* iCmd,
                                CATNotification* iNotif,
                                CATCommandClientData iClientData)
{
    const int categoryIndex = m_pCategory ? m_pCategory->GetSelect() : -1;
    char partNumber[9] = {0};
    if (!GetNextPartNumber(partNumber, sizeof(partNumber)))
    {
        NewBomLog("FORM", "number generation failed");
        if (m_pStatusLabel)
            m_pStatusLabel->SetTitle(U(L"\x53D6\x53F7\x5931\x8D25"));
        return;
    }

    NewBomLog("FORM", "number generated=%s categoryIndex=%d",
                  partNumber, categoryIndex);
    if (m_pNumberResult)
        m_pNumberResult->SetTitle(CATUnicodeString(partNumber));
    if (m_pStatusLabel)
        m_pStatusLabel->SetTitle(U(L"\x53D6\x53F7\x6210\x529F"));
}
