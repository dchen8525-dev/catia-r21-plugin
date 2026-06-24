// PartNumberMenuAddin.cpp
// CATIA CAA Part Number Menu Plugin - Workbench Addin Implementation
//
// This file implements the CATIAfrGeneralWksAddin interface to add custom menus
// to CATIA's main menu bar. The plugin creates:
//   - Top-level menu "NewBOM"
//   - Submenu "编号管理" (Number Management) with "取新编号" command
//   - Submenu "工具" (Tools) with "关于本插件" (About) command
//
// Key CATIA CAA concepts used:
//   - NewAccess macro: Creates command containers (menus) and starters (menu items)
//   - SetAccessChild: Sets a child element inside a container (e.g., submenu in menu)
//   - SetAccessNext: Chains sibling elements (e.g., multiple submenus)
//   - SetAccessCommand: Links a starter to a command header
//   - SetAddinMenu: Merges addin's menu bar with CATIA's main menu bar
//   - SetTitle: Sets the display title of a menu container

#include "PartNumberMenuAddin.h"
#include "PartNumberMenuAboutCmd.h"
#include "PartNumberMenuLog.h"
#include "TIE_CATIAfrGeneralWksAddin.h"
#include "CATCommandHeader.h"
#include "CATCmdContainer.h"
#include "CATCreateWorkshop.h"
#include "CATUnicodeString.h"
#include "CATI18NTypes.h"  // For CATUC2Bytes

//=============================================================================
// CLASS REGISTRATION
//=============================================================================
// CATImplementClass registers this class as a DataExtension.
// This means the class extends a "late type" (PartNumberMenuGeneralWksAddin)
// which is defined in the dictionary file (.dico).
//
// The dictionary entry looks like:
//   PartNumberMenuGeneralWksAddin  CATIAfrGeneralWksAddin  libPartNumberMenuModule
//
// This tells CATIA: "When someone asks for CATIAfrGeneralWksAddin on 
// PartNumberMenuGeneralWksAddin, load PartNumberMenuModule DLL and 
// create PartNumberMenuAddin instance."

CATImplementClass(PartNumberMenuAddin,
                  DataExtension,
                  CATBaseUnknown,
                  PartNumberMenuGeneralWksAddin);

// TIE_CATIAfrGeneralWksAddin creates the TIE object that implements the interface.
// This is necessary for CATIA's component model to find and use this addin.
TIE_CATIAfrGeneralWksAddin(PartNumberMenuAddin);

// MacDeclareHeader creates a command header class named PartNumberMenuHeader.
// Command headers are the link between menu items and actual command classes.
// Each menu command needs a header instance created in CreateCommands().
MacDeclareHeader(PartNumberMenuHeader);

//=============================================================================
// CONSTRUCTOR
//=============================================================================
PartNumberMenuAddin::PartNumberMenuAddin()
{
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "PartNumberMenuAddin CONSTRUCTOR START");
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "  this pointer = %p", this);
    PartNumberMenuLog("ADDIN", "  Purpose: Initialize the addin object");
    PartNumberMenuLog("ADDIN", "  Note: CATIA calls this when loading the plugin");
    PartNumberMenuLog("ADDIN", "CONSTRUCTOR END - Object created successfully");
}

//=============================================================================
// DESTRUCTOR
//=============================================================================
PartNumberMenuAddin::~PartNumberMenuAddin()
{
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "PartNumberMenuAddin DESTRUCTOR START");
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "  this pointer = %p", this);
    PartNumberMenuLog("ADDIN", "  Purpose: Clean up when CATIA closes or unloads plugin");
    PartNumberMenuLog("ADDIN", "DESTRUCTOR END");
}

//=============================================================================
// CreateCommands - REGISTER COMMAND HEADERS
//=============================================================================
// This method is called by CATIA to register all command headers.
// Each command header links a menu item ID to a command class.
//
// Flow:
//   1. CATIA calls CreateCommands()
//   2. We create header instances with: new PartNumberMenuHeader(...)
//   3. Each header has:
//      - HeaderID: Unique identifier (used in SetAccessCommand)
//      - LoadName: DLL name to load (PartNumberMenuModule)
//      - ClassName: The actual command class to instantiate
//      - Argument: Optional data passed to command (NULL here)
//
void PartNumberMenuAddin::CreateCommands()
{
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "CreateCommands() START");
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "Purpose: Register command headers that link menu items to commands");
    PartNumberMenuLog("ADDIN", "");

    // -----------------------------------------------------------------------
    // Command Header 1: PartNumberMenuCmd (取新编号 - Get New Number)
    // -----------------------------------------------------------------------
    PartNumberMenuLog("ADDIN", "[Command 1] Creating PartNumberMenuCmd header...");
    PartNumberMenuLog("ADDIN", "  - HeaderID: 'PartNumberMenuCmd'");
    PartNumberMenuLog("ADDIN", "  - LoadName:  'PartNumberMenuModule' (DLL name)");
    PartNumberMenuLog("ADDIN", "  - ClassName: 'PartNumberMenuCmd' (command class to run)");
    PartNumberMenuLog("ADDIN", "  - Argument:  NULL (no data passed)");
    
    CATCommandHeader* header = new PartNumberMenuHeader("PartNumberMenuCmd",
                                                    "PartNumberMenuModule",
                                                    "PartNumberMenuCmd",
                                                    (void*) NULL);
    
    if (header != NULL)
    {
        PartNumberMenuLog("ADDIN", "  SUCCESS: Header created at address %p", header);
    }
    else
    {
        PartNumberMenuLog("ADDIN", "  ERROR: Failed to create header!");
    }
    PartNumberMenuLog("ADDIN", "");

    // -----------------------------------------------------------------------
    // Command Header 2: PartNumberMenuAboutCmd (关于本插件 - About Plugin)
    // -----------------------------------------------------------------------
    PartNumberMenuLog("ADDIN", "[Command 2] Creating PartNumberMenuAboutCmd header...");
    PartNumberMenuLog("ADDIN", "  - HeaderID: 'PartNumberMenuAboutCmd'");
    PartNumberMenuLog("ADDIN", "  - LoadName:  'PartNumberMenuModule' (DLL name)");
    PartNumberMenuLog("ADDIN", "  - ClassName: 'PartNumberMenuAboutCmd' (command class to run)");
    PartNumberMenuLog("ADDIN", "  - Argument:  NULL (no data passed)");
    
    CATCommandHeader* aboutHeader = new PartNumberMenuHeader("PartNumberMenuAboutCmd",
                                                    "PartNumberMenuModule",
                                                    "PartNumberMenuAboutCmd",
                                                    (void*) NULL);
    
    if (aboutHeader != NULL)
    {
        PartNumberMenuLog("ADDIN", "  SUCCESS: About header created at address %p", aboutHeader);
    }
    else
    {
        PartNumberMenuLog("ADDIN", "  ERROR: Failed to create about header!");
    }
    PartNumberMenuLog("ADDIN", "");

    PartNumberMenuLog("ADDIN", "CreateCommands() END - Headers registered: %p, %p", header, aboutHeader);
}

//=============================================================================
// CreateToolbars - BUILD MENU STRUCTURE
//=============================================================================
// This method builds the menu hierarchy.
//
// Menu structure:
//   NewBOM (Top Menu)
//     ├── 编号管理 (Submenu)
//     │     └── 取新编号 (Command Item)
//     └── 工具 (Submenu)
//           └── 关于本插件 (Command Item)
//
// Key concepts:
//   - CATCmdContainer: A container that holds other elements (menu, submenu)
//   - CATCmdStarter: A clickable menu item that triggers a command
//   - SetAccessChild(parent, child): Put child inside parent
//   - SetAccessNext(prev, next): Chain siblings at same level
//
CATCmdContainer* PartNumberMenuAddin::CreateToolbars()
{
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "CreateToolbars() START - Building Menu Structure");
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "");
    PartNumberMenuLog("ADDIN", "Target menu structure:");
    PartNumberMenuLog("ADDIN", "  NewBOM (Top Menu)");
    PartNumberMenuLog("ADDIN", "    ├── 编号管理 (Submenu)");
    PartNumberMenuLog("ADDIN", "    │     └── 取新编号 (Command)");
    PartNumberMenuLog("ADDIN", "    └── 工具 (Submenu)");
    PartNumberMenuLog("ADDIN", "          └── 关于本插件 (Command)");
    PartNumberMenuLog("ADDIN", "");

    // =========================================================================
    // STEP 1: Create Root Containers
    // =========================================================================
    PartNumberMenuLog("ADDIN", "STEP 1: Creating root containers...");
    PartNumberMenuLog("ADDIN", "  [1a] Creating PartNumberMenuAddinRoot (root container)");
    PartNumberMenuLog("ADDIN", "       This is the top-level container returned to CATIA");
    
    NewAccess(CATCmdContainer, pAddinRoot, PartNumberMenuAddinRoot);
    PartNumberMenuLog("ADDIN", "       Result: pAddinRoot = %p", pAddinRoot);
    
    PartNumberMenuLog("ADDIN", "  [1b] Creating PartNumberMenuMenuBar (menu bar container)");
    PartNumberMenuLog("ADDIN", "       This holds all top-level menus we create");
    
    NewAccess(CATCmdContainer, pMenuBar, PartNumberMenuMenuBar);
    PartNumberMenuLog("ADDIN", "       Result: pMenuBar = %p", pMenuBar);
    PartNumberMenuLog("ADDIN", "STEP 1 COMPLETE");
    PartNumberMenuLog("ADDIN", "");

    // =========================================================================
    // STEP 2: Create Top-Level Menu "NewBOM"
    // =========================================================================
    PartNumberMenuLog("ADDIN", "STEP 2: Creating top-level menu 'NewBOM'...");
    PartNumberMenuLog("ADDIN", "  [2a] NewAccess creates container with ID 'PartNumberMenuTopMnu'");
    
    NewAccess(CATCmdContainer, pTopMenu, PartNumberMenuTopMnu);
    PartNumberMenuLog("ADDIN", "       Result: pTopMenu = %p", pTopMenu);
    
    PartNumberMenuLog("ADDIN", "  [2b] SetAccessChild attaches top menu to menu bar");
    PartNumberMenuLog("ADDIN", "       Call: SetAccessChild(pMenuBar, pTopMenu)");
    
    SetAccessChild(pMenuBar, pTopMenu);
    PartNumberMenuLog("ADDIN", "       Done: Top menu is now child of menu bar");
    
    PartNumberMenuLog("ADDIN", "  [2c] SetTitle sets the display text to 'NewBOM'");
    {
        CATUnicodeString title;
        title = "NewBOM";
        PartNumberMenuLog("ADDIN", "       Creating CATUnicodeString with value 'NewBOM'");
        PartNumberMenuLog("ADDIN", "       String length = %d characters", title.GetLengthInChar());
        pTopMenu->SetTitle(title);
        PartNumberMenuLog("ADDIN", "       SetTitle() called successfully");
    }
    PartNumberMenuLog("ADDIN", "STEP 2 COMPLETE - Top menu created and titled 'NewBOM'");
    PartNumberMenuLog("ADDIN", "");

    // =========================================================================
    // STEP 3: Create Submenu "编号管理" (Number Management)
    // =========================================================================
    PartNumberMenuLog("ADDIN", "STEP 3: Creating submenu '编号管理' (Number Management)...");
    PartNumberMenuLog("ADDIN", "  Chinese characters (UCS-2): 编=0x7F16, 号=0x53F7, 管=0x7BA1, 理=0x7406");
    
    PartNumberMenuLog("ADDIN", "  [3a] NewAccess creates container with ID 'PartNumberMenuNumberMgtMnu'");
    NewAccess(CATCmdContainer, pNumberMgtMenu, PartNumberMenuNumberMgtMnu);
    PartNumberMenuLog("ADDIN", "       Result: pNumberMgtMenu = %p", pNumberMgtMenu);
    
    PartNumberMenuLog("ADDIN", "  [3b] SetAccessChild attaches this submenu to top menu");
    PartNumberMenuLog("ADDIN", "       Call: SetAccessChild(pTopMenu, pNumberMgtMenu)");
    SetAccessChild(pTopMenu, pNumberMgtMenu);
    PartNumberMenuLog("ADDIN", "       Done: '编号管理' is now a submenu of 'NewBOM'");
    
    PartNumberMenuLog("ADDIN", "  [3c] SetTitle sets Chinese display text using UCS-2 code points");
    {
        // BuildFromUCChar is the recommended way to create Unicode strings
        // It takes an array of 16-bit UCS-2 character codes
        static const CATUC2Bytes titleChars[] = {0x7F16, 0x53F7, 0x7BA1, 0x7406, 0};  // "编号管理" + null
        CATUnicodeString title;
        PartNumberMenuLog("ADDIN", "       Calling BuildFromUCChar with UCS-2 array");
        title.BuildFromUCChar(titleChars, 4);
        PartNumberMenuLog("ADDIN", "       String length = %d characters (should be 4)", title.GetLengthInChar());
        pNumberMgtMenu->SetTitle(title);
        PartNumberMenuLog("ADDIN", "       SetTitle() called - menu should display '编号管理'");
    }
    PartNumberMenuLog("ADDIN", "STEP 3 COMPLETE - Submenu '编号管理' created");
    PartNumberMenuLog("ADDIN", "");

    // =========================================================================
    // STEP 4: Create Command Item "取新编号" (Get New Number)
    // =========================================================================
    PartNumberMenuLog("ADDIN", "STEP 4: Creating command item '取新编号' (Get New Number)...");
    
    PartNumberMenuLog("ADDIN", "  [4a] NewAccess creates a starter with ID 'PartNumberMenuGetNumberStr'");
    NewAccess(CATCmdStarter, pGetNumberStarter, PartNumberMenuGetNumberStr);
    PartNumberMenuLog("ADDIN", "       Result: pGetNumberStarter = %p", pGetNumberStarter);
    
    PartNumberMenuLog("ADDIN", "  [4b] SetAccessCommand links starter to command header");
    PartNumberMenuLog("ADDIN", "       Call: SetAccessCommand(pGetNumberStarter, 'PartNumberMenuCmd')");
    PartNumberMenuLog("ADDIN", "       This links the menu item to the header we created in CreateCommands()");
    SetAccessCommand(pGetNumberStarter, "PartNumberMenuCmd");
    PartNumberMenuLog("ADDIN", "       Done: Clicking this item will run PartNumberMenuCmd");
    
    PartNumberMenuLog("ADDIN", "  [4c] SetAccessChild puts this item inside '编号管理' submenu");
    PartNumberMenuLog("ADDIN", "       Call: SetAccessChild(pNumberMgtMenu, pGetNumberStarter)");
    SetAccessChild(pNumberMgtMenu, pGetNumberStarter);
    PartNumberMenuLog("ADDIN", "       Done: '取新编号' is now inside '编号管理'");
    
    PartNumberMenuLog("ADDIN", "STEP 4 COMPLETE - Command item '取新编号' created");
    PartNumberMenuLog("ADDIN", "");

    // =========================================================================
    // STEP 5: Create Submenu "工具" (Tools) - Sibling of "编号管理"
    // =========================================================================
    PartNumberMenuLog("ADDIN", "STEP 5: Creating submenu '工具' (Tools)...");
    PartNumberMenuLog("ADDIN", "  Chinese characters (UCS-2): 工=0x5DE5, 具=0x5177");
    PartNumberMenuLog("ADDIN", "  Note: This submenu is a sibling (same level) as '编号管理'");
    
    PartNumberMenuLog("ADDIN", "  [5a] NewAccess creates container with ID 'PartNumberMenuToolsMnu'");
    NewAccess(CATCmdContainer, pToolsMenu, PartNumberMenuToolsMnu);
    PartNumberMenuLog("ADDIN", "       Result: pToolsMenu = %p", pToolsMenu);
    
    PartNumberMenuLog("ADDIN", "  [5b] SetAccessNext chains this submenu after '编号管理'");
    PartNumberMenuLog("ADDIN", "       Call: SetAccessNext(pNumberMgtMenu, pToolsMenu)");
    PartNumberMenuLog("ADDIN", "       IMPORTANT: SetAccessNext creates sibling relationship");
    PartNumberMenuLog("ADDIN", "       - SetAccessChild = parent-child relationship");
    PartNumberMenuLog("ADDIN", "       - SetAccessNext = sibling relationship (same level)");
    SetAccessNext(pNumberMgtMenu, pToolsMenu);
    PartNumberMenuLog("ADDIN", "       Done: '工具' now appears next to '编号管理' in menu");
    
    PartNumberMenuLog("ADDIN", "  [5c] SetTitle sets Chinese display text");
    {
        static const CATUC2Bytes titleChars[] = {0x5DE5, 0x5177, 0};  // "工具" + null
        CATUnicodeString title;
        PartNumberMenuLog("ADDIN", "       Calling BuildFromUCChar with UCS-2 array");
        title.BuildFromUCChar(titleChars, 2);
        PartNumberMenuLog("ADDIN", "       String length = %d characters (should be 2)", title.GetLengthInChar());
        pToolsMenu->SetTitle(title);
        PartNumberMenuLog("ADDIN", "       SetTitle() called - menu should display '工具'");
    }
    PartNumberMenuLog("ADDIN", "STEP 5 COMPLETE - Submenu '工具' created");
    PartNumberMenuLog("ADDIN", "");

    // =========================================================================
    // STEP 6: Create Command Item "关于本插件" (About Plugin)
    // =========================================================================
    PartNumberMenuLog("ADDIN", "STEP 6: Creating command item '关于本插件' (About Plugin)...");
    
    PartNumberMenuLog("ADDIN", "  [6a] NewAccess creates a starter with ID 'PartNumberMenuAboutStr'");
    NewAccess(CATCmdStarter, pAboutStarter, PartNumberMenuAboutStr);
    PartNumberMenuLog("ADDIN", "       Result: pAboutStarter = %p", pAboutStarter);
    
    PartNumberMenuLog("ADDIN", "  [6b] SetAccessCommand links to 'PartNumberMenuAboutCmd' header");
    PartNumberMenuLog("ADDIN", "       Call: SetAccessCommand(pAboutStarter, 'PartNumberMenuAboutCmd')");
    SetAccessCommand(pAboutStarter, "PartNumberMenuAboutCmd");
    PartNumberMenuLog("ADDIN", "       Done: Clicking will run PartNumberMenuAboutCmd (shows dialog)");
    
    PartNumberMenuLog("ADDIN", "  [6c] SetAccessChild puts this item inside '工具' submenu");
    PartNumberMenuLog("ADDIN", "       Call: SetAccessChild(pToolsMenu, pAboutStarter)");
    SetAccessChild(pToolsMenu, pAboutStarter);
    PartNumberMenuLog("ADDIN", "       Done: '关于本插件' is now inside '工具'");
    
    PartNumberMenuLog("ADDIN", "STEP 6 COMPLETE - Command item '关于本插件' created");
    PartNumberMenuLog("ADDIN", "");

    // =========================================================================
    // STEP 7: Merge Menu with CATIA's Main Menu Bar
    // =========================================================================
    PartNumberMenuLog("ADDIN", "STEP 7: Merging our menu with CATIA's main menu bar...");
    PartNumberMenuLog("ADDIN", "  Call: SetAddinMenu(pAddinRoot, pMenuBar)");
    PartNumberMenuLog("ADDIN", "  This is the final step that makes our menu visible in CATIA");
    PartNumberMenuLog("ADDIN", "  - pAddinRoot: The root container returned to CATIA");
    PartNumberMenuLog("ADDIN", "  - pMenuBar: Our menu bar to be merged");
    
    SetAddinMenu(pAddinRoot, pMenuBar);
    PartNumberMenuLog("ADDIN", "  Done: Menu structure is now attached to CATIA");
    
    PartNumberMenuLog("ADDIN", "STEP 7 COMPLETE");
    PartNumberMenuLog("ADDIN", "");

    // =========================================================================
    // FINAL SUMMARY
    // =========================================================================
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "CreateToolbars() COMPLETE - Menu Structure Built Successfully");
    PartNumberMenuLog("ADDIN", "===========================================");
    PartNumberMenuLog("ADDIN", "Summary of created objects:");
    PartNumberMenuLog("ADDIN", "  pAddinRoot     = %p (returned to CATIA)", pAddinRoot);
    PartNumberMenuLog("ADDIN", "  pMenuBar       = %p (menu bar container)", pMenuBar);
    PartNumberMenuLog("ADDIN", "  pTopMenu       = %p (NewBOM top menu)", pTopMenu);
    PartNumberMenuLog("ADDIN", "  pNumberMgtMenu = %p (编号管理 submenu)", pNumberMgtMenu);
    PartNumberMenuLog("ADDIN", "  pToolsMenu     = %p (工具 submenu)", pToolsMenu);
    PartNumberMenuLog("ADDIN", "");
    PartNumberMenuLog("ADDIN", "Menu tree ready:");
    PartNumberMenuLog("ADDIN", "  NewBOM → 编号管理 → 取新编号");
    PartNumberMenuLog("ADDIN", "         → 工具 → 关于本插件");
    PartNumberMenuLog("ADDIN", "===========================================");
    
    return pAddinRoot;
}