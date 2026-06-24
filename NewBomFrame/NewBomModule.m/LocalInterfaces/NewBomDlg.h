// =============================================================================
// NewBomDlg.h
// CATIA CAA Dialog Header File - "Get New Number" Dialog
// =============================================================================
//
// What is CATDlgDialog?
// CATDlgDialog is the base class for CATIA dialogs. All custom dialogs
// inherit from it. It provides:
//   - Window management (show, hide, close)
//   - Event handling (button click, window close, etc.)
//   - Control management (labels, editors, buttons, etc.)
//
// Dialog Control Types:
//   - CATDlgLabel:      Label (display text)
//   - CATDlgEditor:     Editor (user input text)
//   - CATDlgCombo:      Combo box (select option)
//   - CATDlgPushButton: Push button (click triggers action)
//   - CATDlgCheckBox:   Check box
//   - CATDlgRadioButton: Radio button
//
// Dialog Layout:
// CATIA dialogs support multiple layout methods:
//   - CATDlgGridLayout: Grid layout (row/column arrangement)
//   - CATDlgBoxLayout:  Box layout (horizontal or vertical)
// This dialog uses grid layout for easier control positioning.
//
// Dialog Function:
//   - Input part name
//   - Select part category (combo box)
//   - Input material
//   - Input notes
//   - Display generated part number
//   - Click button to generate number
//
// =============================================================================

#ifndef __NewBomDlg_h__
#define __NewBomDlg_h__

#include "CATDlgDialog.h"
#include "CATDlgLabel.h"
#include "CATDlgEditor.h"
#include "CATDlgCombo.h"
#include "CATDlgPushButton.h"

/**
 * @class NewBomDlg
 * @brief "Get New Number" Dialog Implementation
 * 
 * Dialog Layout (Grid 3 columns x 7 rows):
 *   Row0: [Label:PartName] [Spacer] [Editor:PartName]
 *   Row1: [Label:Category] [Spacer] [Combo:CategorySelect]
 *   Row2: [Label:Material] [Spacer] [Editor:Material]
 *   Row3: [Label:Notes]    [Spacer] [Editor:Notes]
 *   Row4: [Label:PartNum]  [Spacer] [Label:NumResult]
 *   Row5: [Blank]          [Spacer] [Button:GetNumber]
 *   Row6: [Blank]          [Spacer] [Label:Status]
 * 
 * Workflow:
 *   1. User fills part information
 *   2. Clicks "Get Number" button
 *   3. OnGetNumber() callback triggered
 *   4. Generate number (DLXX0001 format)
 *   5. Display number result
 */
class NewBomDlg : public CATDlgDialog
{
    CATDeclareClass;

public:
    /**
     * Constructor
     * 
     * Parameters:
     * @param iParent Parent window (CATIA main window)
     *               Dialog needs parent for proper display and management
     * @param iObjectName Dialog identifier
     *               Used for resource file matching and debugging
     * 
     * Dialog Style:
     *   - CATDlgWndModal: Modal window (optional)
     *   - CATDlgWndBtnClose: Show close button
     *   - CATDlgGridLayout: Grid layout
     */
    NewBomDlg(CATDialog* iParent, 
              const CATString& iObjectName = "NewBomDlg");
    
    virtual ~NewBomDlg();

    /**
     * Build Dialog
     * 
     * When: Called automatically in constructor
     * 
     * What it does:
     *   1. Set dialog title
     *   2. Create controls (labels, editors, combos, buttons)
     *   3. Set control grid positions (SetGridConstraints)
     *   4. Connect button click event
     */
    void BuildDialog();

private:
    /**
     * Handle "Get Number" button click
     * 
     * Callback registered via AddAnalyseNotificationCB:
     *   - Listen for GetPushBActivateNotification()
     *   - Call this method when clicked
     * 
     * What it does:
     *   1. Get currently selected category
     *   2. Call GetNextPartNumber() to generate number
     *   3. Display number result
     *   4. Show status message
     */
    void OnGetNumber(CATCommand* iCmd,
                     CATNotification* iNotif,
                     CATCommandClientData iClientData);

    /**
     * Fetch part code from HTTP server
     * 
     * Uses WinHTTP to call http://localhost:8080/api/getCode
     * Parses JSON response and extracts data.partCode
     * Error messages are displayed directly in m_pStatusLabel
     * 
     * @param oPartCode    Output buffer for part code (e.g., "DLXX0016")
     * @param iBufferSize  Size of output buffer
     * @return true if successful, false on error
     */
    static bool FetchPartCodeFromServer(
        char* oPartCode, 
        size_t iBufferSize);

    // Control member variables
    CATDlgEditor*     m_pPartName;      // Part name editor
    CATDlgCombo*      m_pCategory;      // Category combo box
    CATDlgEditor*     m_pMaterial;      // Material editor
    CATDlgEditor*     m_pNotes;         // Notes editor
    CATDlgLabel*      m_pNumberResult;  // Number result label
    CATDlgPushButton* m_pNumberButton;  // Get number button
    CATDlgLabel*      m_pStatusLabel;   // Status label
};

#endif // __NewBomDlg_h__