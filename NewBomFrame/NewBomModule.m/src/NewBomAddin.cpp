// NewBomAddin.cpp
// CATIA CAA Part Number Menu Plugin - Workbench Addin Implementation
//
// This file implements the CATIAfrGeneralWksAddin interface to add custom menus
// to CATIA's main menu bar. The plugin creates:
//   - Top-level menu "NewBOM"
//   - Submenu "NumberMgt" (Number Management) with "GetNewNumber" command
//   - Submenu "Tools" (Tools) with "AboutPlugin" (About) command
//
// Key CATIA CAA concepts used:
//   - NewAccess macro: Creates command containers (menus) and starters (menu items)
//   - SetAccessChild: Sets a child element inside a container (e.g., submenu in menu)
//   - SetAccessNext: Chains sibling elements (e.g., multiple submenus)
//   - SetAccessCommand: Links a starter to a command header
//   - SetAddinMenu: Merges addin's menu bar with CATIA's main menu bar
//   - SetTitle: Sets the display title of a menu container

#include "NewBomAddin.h"
#include "NewBomAboutCmd.h"
#include "NewBomLog.h"
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
// This means the class extends a "late type" (NewBomGeneralWksAddin)
// which is defined in the dictionary file (.dico).
//
// The dictionary entry looks like:
//   NewBomGeneralWksAddin  CATIAfrGeneralWksAddin  libNewBomModule
//
// This tells CATIA: "When someone asks for CATIAfrGeneralWksAddin on 
// NewBomGeneralWksAddin, load NewBomModule DLL and 
// create NewBomAddin instance."

CATImplementClass(NewBomAddin,
                  DataExtension,
                  CATBaseUnknown,
                  NewBomGeneralWksAddin);

// TIE_CATIAfrGeneralWksAddin creates the TIE object that implements the interface.
// This is necessary for CATIA's component model to find and use this addin.
TIE_CATIAfrGeneralWksAddin(NewBomAddin);

// MacDeclareHeader creates a command header class named NewBomHeader.
// Command headers are the link between menu items and actual command classes.
// Each menu command needs a header instance created in CreateCommands().
MacDeclareHeader(NewBomHeader);

//=============================================================================
// CONSTRUCTOR
//=============================================================================
NewBomAddin::NewBomAddin()
{
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "NewBomAddin CONSTRUCTOR START");
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "  this pointer = %p", this);
    NewBomLog("ADDIN", "  Purpose: Initialize the addin object");
    NewBomLog("ADDIN", "  Note: CATIA calls this when loading the plugin");
    NewBomLog("ADDIN", "CONSTRUCTOR END - Object created successfully");
}

//=============================================================================
// DESTRUCTOR
//=============================================================================
NewBomAddin::~NewBomAddin()
{
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "NewBomAddin DESTRUCTOR START");
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "  this pointer = %p", this);
    NewBomLog("ADDIN", "  Purpose: Clean up when CATIA closes or unloads plugin");
    NewBomLog("ADDIN", "DESTRUCTOR END");
}

//=============================================================================
// CreateCommands - REGISTER COMMAND HEADERS
//=============================================================================
// This method is called by CATIA to register all command headers.
// Each command header links a menu item ID to a command class.
//
// Flow:
//   1. CATIA calls CreateCommands()
//   2. We create header instances with: new NewBomHeader(...)
//   3. Each header has:
//      - HeaderID: Unique identifier (used in SetAccessCommand)
//      - LoadName: DLL name to load (NewBomModule)
//      - ClassName: The actual command class to instantiate
//      - Argument: Optional data passed to command (NULL here)
//
void NewBomAddin::CreateCommands()
{
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "CreateCommands() START");
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "Purpose: Register command headers that link menu items to commands");
    NewBomLog("ADDIN", "");

    // -----------------------------------------------------------------------
    // Command Header 1: NewBomCmd (GetNewNumber - Get New Number)
    // -----------------------------------------------------------------------
    NewBomLog("ADDIN", "[Command 1] Creating NewBomCmd header...");
    NewBomLog("ADDIN", "  - HeaderID: 'NewBomCmd'");
    NewBomLog("ADDIN", "  - LoadName:  'NewBomModule' (DLL name)");
    NewBomLog("ADDIN", "  - ClassName: 'NewBomCmd' (command class to run)");
    NewBomLog("ADDIN", "  - Argument:  NULL (no data passed)");
    
    CATCommandHeader* header = new NewBomHeader("NewBomCmd",
                                                    "NewBomModule",
                                                    "NewBomCmd",
                                                    (void*) NULL);
    
    if (header != NULL)
    {
        NewBomLog("ADDIN", "  SUCCESS: Header created at address %p", header);
    }
    else
    {
        NewBomLog("ADDIN", "  ERROR: Failed to create header!");
    }
    NewBomLog("ADDIN", "");

    // -----------------------------------------------------------------------
    // Command Header 2: NewBomAboutCmd (AboutPlugin - About Plugin)
    // -----------------------------------------------------------------------
    NewBomLog("ADDIN", "[Command 2] Creating NewBomAboutCmd header...");
    NewBomLog("ADDIN", "  - HeaderID: 'NewBomAboutCmd'");
    NewBomLog("ADDIN", "  - LoadName:  'NewBomModule' (DLL name)");
    NewBomLog("ADDIN", "  - ClassName: 'NewBomAboutCmd' (command class to run)");
    NewBomLog("ADDIN", "  - Argument:  NULL (no data passed)");
    
    CATCommandHeader* aboutHeader = new NewBomHeader("NewBomAboutCmd",
                                                    "NewBomModule",
                                                    "NewBomAboutCmd",
                                                    (void*) NULL);
    
    if (aboutHeader != NULL)
    {
        NewBomLog("ADDIN", "  SUCCESS: About header created at address %p", aboutHeader);
    }
    else
    {
        NewBomLog("ADDIN", "  ERROR: Failed to create about header!");
    }
    NewBomLog("ADDIN", "");

    // -----------------------------------------------------------------------
    // Command Header 3: NewBomAssemblyTreeCmd (GetAssemblyTree)
    // -----------------------------------------------------------------------
    NewBomLog("ADDIN", "[Command 3] Creating NewBomAssemblyTreeCmd header...");
    NewBomLog("ADDIN", "  - HeaderID: 'NewBomAssemblyTreeCmd'");
    NewBomLog("ADDIN", "  - LoadName:  'NewBomModule' (DLL name)");
    NewBomLog("ADDIN", "  - ClassName: 'NewBomAssemblyTreeCmd' (command class to run)");
    NewBomLog("ADDIN", "  - Argument:  NULL (no data passed)");
    
    CATCommandHeader* assemblyTreeHeader = new NewBomHeader("NewBomAssemblyTreeCmd",
                                                            "NewBomModule",
                                                            "NewBomAssemblyTreeCmd",
                                                            (void*) NULL);
    
    if (assemblyTreeHeader != NULL)
    {
        NewBomLog("ADDIN", "  SUCCESS: AssemblyTree header created at address %p", assemblyTreeHeader);
    }
    else
    {
        NewBomLog("ADDIN", "  ERROR: Failed to create AssemblyTree header!");
    }
    NewBomLog("ADDIN", "");

    NewBomLog("ADDIN", "CreateCommands() END - Headers registered: %p, %p, %p", header, aboutHeader, assemblyTreeHeader);
}

//=============================================================================
// CreateToolbars - BUILD MENU STRUCTURE
//=============================================================================
// This method builds the menu hierarchy.
//
// Menu structure:
//   NewBOM (Top Menu)
//     |- NumberMgt (Submenu)
//     |     |- GetNewNumber (Command Item)
//     |- Tools (Submenu)
//           |- AboutPlugin (Command Item)
//
// Key concepts:
//   - CATCmdContainer: A container that holds other elements (menu, submenu)
//   - CATCmdStarter: A clickable menu item that triggers a command
//   - SetAccessChild(parent, child): Put child inside parent
//   - SetAccessNext(prev, next): Chain siblings at same level
//
CATCmdContainer* NewBomAddin::CreateToolbars()
{
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "CreateToolbars() START - Building Menu Structure");
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "");
    NewBomLog("ADDIN", "Target menu structure:");
    NewBomLog("ADDIN", "  NewBOM (Top Menu)");
    NewBomLog("ADDIN", "    |- NumberMgt (Submenu)");
    NewBomLog("ADDIN", "    |     |- GetNewNumber (Command)");
    NewBomLog("ADDIN", "    |- Tools (Submenu)");
    NewBomLog("ADDIN", "          |- AboutPlugin (Command)");
    NewBomLog("ADDIN", "");

    // =========================================================================
    // STEP 1: Create Root Containers
    // =========================================================================
    NewBomLog("ADDIN", "STEP 1: Creating root containers...");
    NewBomLog("ADDIN", "  [1a] Creating NewBomAddinRoot (root container)");
    NewBomLog("ADDIN", "       This is the top-level container returned to CATIA");
    
    NewAccess(CATCmdContainer, pAddinRoot, NewBomAddinRoot);
    NewBomLog("ADDIN", "       Result: pAddinRoot = %p", pAddinRoot);
    
    NewBomLog("ADDIN", "  [1b] Creating NewBomMenuBar (menu bar container)");
    NewBomLog("ADDIN", "       This holds all top-level menus we create");
    
    NewAccess(CATCmdContainer, pMenuBar, NewBomMenuBar);
    NewBomLog("ADDIN", "       Result: pMenuBar = %p", pMenuBar);
    NewBomLog("ADDIN", "STEP 1 COMPLETE");
    NewBomLog("ADDIN", "");

    // =========================================================================
    // STEP 2: Create Top-Level Menu "NewBOM"
    // =========================================================================
    NewBomLog("ADDIN", "STEP 2: Creating top-level menu 'NewBOM'...");
    NewBomLog("ADDIN", "  [2a] NewAccess creates container with ID 'NewBomTopMnu'");
    
    NewAccess(CATCmdContainer, pTopMenu, NewBomTopMnu);
    NewBomLog("ADDIN", "       Result: pTopMenu = %p", pTopMenu);
    
    NewBomLog("ADDIN", "  [2b] SetAccessChild attaches top menu to menu bar");
    NewBomLog("ADDIN", "       Call: SetAccessChild(pMenuBar, pTopMenu)");
    
    SetAccessChild(pMenuBar, pTopMenu);
    NewBomLog("ADDIN", "       Done: Top menu is now child of menu bar");
    
    NewBomLog("ADDIN", "  [2c] SetTitle sets the display text to 'NewBOM'");
    {
        CATUnicodeString title;
        title = "NewBOM";
        NewBomLog("ADDIN", "       Creating CATUnicodeString with value 'NewBOM'");
        NewBomLog("ADDIN", "       String length = %d characters", title.GetLengthInChar());
        pTopMenu->SetTitle(title);
        NewBomLog("ADDIN", "       SetTitle() called successfully");
    }
    NewBomLog("ADDIN", "STEP 2 COMPLETE - Top menu created and titled 'NewBOM'");
    NewBomLog("ADDIN", "");

    // =========================================================================
    // STEP 3: Create Submenu "NumberMgt" (Number Management)
    // =========================================================================
    NewBomLog("ADDIN", "STEP 3: Creating submenu 'NumberMgt' (Number Management)...");
    NewBomLog("ADDIN", "  Chinese characters (UCS-2): 编=0x7F16, 号=0x53F7, 管=0x7BA1, 理=0x7406");
    
    NewBomLog("ADDIN", "  [3a] NewAccess creates container with ID 'NewBomNumberMgtMnu'");
    NewAccess(CATCmdContainer, pNumberMgtMenu, NewBomNumberMgtMnu);
    NewBomLog("ADDIN", "       Result: pNumberMgtMenu = %p", pNumberMgtMenu);
    
    NewBomLog("ADDIN", "  [3b] SetAccessChild attaches this submenu to top menu");
    NewBomLog("ADDIN", "       Call: SetAccessChild(pTopMenu, pNumberMgtMenu)");
    SetAccessChild(pTopMenu, pNumberMgtMenu);
    NewBomLog("ADDIN", "       Done: 'NumberMgt' is now a submenu of 'NewBOM'");
    
    NewBomLog("ADDIN", "  [3c] SetTitle sets Chinese display text using UCS-2 code points");
    {
        // BuildFromUCChar is the recommended way to create Unicode strings
        // It takes an array of 16-bit UCS-2 character codes
        static const CATUC2Bytes titleChars[] = {0x7F16, 0x53F7, 0x7BA1, 0x7406, 0};  // "NumberMgt" + null
        CATUnicodeString title;
        NewBomLog("ADDIN", "       Calling BuildFromUCChar with UCS-2 array");
        title.BuildFromUCChar(titleChars, 4);
        NewBomLog("ADDIN", "       String length = %d characters (should be 4)", title.GetLengthInChar());
        pNumberMgtMenu->SetTitle(title);
        NewBomLog("ADDIN", "       SetTitle() called - menu should display 'NumberMgt'");
    }
    NewBomLog("ADDIN", "STEP 3 COMPLETE - Submenu 'NumberMgt' created");
    NewBomLog("ADDIN", "");

    // =========================================================================
    // STEP 4: Create Command Item "GetNewNumber" (Get New Number)
    // =========================================================================
    NewBomLog("ADDIN", "STEP 4: Creating command item 'GetNewNumber' (Get New Number)...");
    
    NewBomLog("ADDIN", "  [4a] NewAccess creates a starter with ID 'NewBomGetNumberStr'");
    NewAccess(CATCmdStarter, pGetNumberStarter, NewBomGetNumberStr);
    NewBomLog("ADDIN", "       Result: pGetNumberStarter = %p", pGetNumberStarter);
    
    NewBomLog("ADDIN", "  [4b] SetAccessCommand links starter to command header");
    NewBomLog("ADDIN", "       Call: SetAccessCommand(pGetNumberStarter, 'NewBomCmd')");
    NewBomLog("ADDIN", "       This links the menu item to the header we created in CreateCommands()");
    SetAccessCommand(pGetNumberStarter, "NewBomCmd");
    NewBomLog("ADDIN", "       Done: Clicking this item will run NewBomCmd");
    
    NewBomLog("ADDIN", "  [4c] SetAccessChild puts this item inside 'NumberMgt' submenu");
    NewBomLog("ADDIN", "       Call: SetAccessChild(pNumberMgtMenu, pGetNumberStarter)");
    SetAccessChild(pNumberMgtMenu, pGetNumberStarter);
    NewBomLog("ADDIN", "       Done: 'GetNewNumber' is now inside 'NumberMgt'");
    
    NewBomLog("ADDIN", "STEP 4 COMPLETE - Command item 'GetNewNumber' created");
    NewBomLog("ADDIN", "");

    // =========================================================================
    // STEP 4.5: Create Submenu "AssemblyTree" (获取装配树) - Sibling of "NumberMgt"
    // =========================================================================
    NewBomLog("ADDIN", "STEP 4.5: Creating submenu 'AssemblyTree' (获取装配树)...");
    NewBomLog("ADDIN", "  Chinese characters (UCS-2): 获=0x83B7, 取=0x53D6, 装=0x88C5, 配=0x914D, 树=0x6811");
    
    NewBomLog("ADDIN", "  [4.5a] NewAccess creates container with ID 'NewBomAssemblyTreeMnu'");
    NewAccess(CATCmdContainer, pAssemblyTreeMenu, NewBomAssemblyTreeMnu);
    NewBomLog("ADDIN", "       Result: pAssemblyTreeMenu = %p", pAssemblyTreeMenu);
    
    // Note: We don't call SetAccessChild here - SetAccessNext in STEP 5 will chain
    // AssemblyTree as sibling of NumberMgt, which automatically puts it in TopMenu
    
    NewBomLog("ADDIN", "  [4.5b] SetTitle sets Chinese display text");
    {
        static const CATUC2Bytes titleChars[] = {0x83B7, 0x53D6, 0x88C5, 0x914D, 0x6811, 0};  // "获取装配树"
        CATUnicodeString title;
        title.BuildFromUCChar(titleChars, 5);
        NewBomLog("ADDIN", "       String length = %d characters (should be 5)", title.GetLengthInChar());
        pAssemblyTreeMenu->SetTitle(title);
        NewBomLog("ADDIN", "       SetTitle() called");
    }
    
    NewBomLog("ADDIN", "  [4.5c] Create command item starter");
    NewAccess(CATCmdStarter, pAssemblyTreeStarter, NewBomAssemblyTreeStr);
    NewBomLog("ADDIN", "       Result: pAssemblyTreeStarter = %p", pAssemblyTreeStarter);
    
    NewBomLog("ADDIN", "  [4.5d] SetAccessCommand links starter to command header");
    SetAccessCommand(pAssemblyTreeStarter, "NewBomAssemblyTreeCmd");
    NewBomLog("ADDIN", "       Done: Clicking this item will run NewBomAssemblyTreeCmd");
    
    NewBomLog("ADDIN", "  [4.5e] SetAccessChild puts item inside 'AssemblyTree' submenu");
    SetAccessChild(pAssemblyTreeMenu, pAssemblyTreeStarter);
    NewBomLog("ADDIN", "       Done: Command item is now inside 'AssemblyTree'");
    
    NewBomLog("ADDIN", "STEP 4.5 COMPLETE - Submenu 'AssemblyTree' created");
    NewBomLog("ADDIN", "");

    // =========================================================================
    // STEP 5: Create Submenu "Tools" (Tools) - Sibling of "NumberMgt"
    // =========================================================================
    NewBomLog("ADDIN", "STEP 5: Creating submenu 'Tools' (Tools)...");
    NewBomLog("ADDIN", "  Chinese characters (UCS-2): 工=0x5DE5, 具=0x5177");
    NewBomLog("ADDIN", "  Note: This submenu is a sibling (same level) as 'NumberMgt'");
    
    NewBomLog("ADDIN", "  [5a] NewAccess creates container with ID 'NewBomToolsMnu'");
    NewAccess(CATCmdContainer, pToolsMenu, NewBomToolsMnu);
    NewBomLog("ADDIN", "       Result: pToolsMenu = %p", pToolsMenu);
    
    NewBomLog("ADDIN", "  [5b] SetAccessNext chains menus in order: NumberMgt -> AssemblyTree -> Tools");
    NewBomLog("ADDIN", "       Call: SetAccessNext(pNumberMgtMenu, pAssemblyTreeMenu)");
    NewBomLog("ADDIN", "       Call: SetAccessNext(pAssemblyTreeMenu, pToolsMenu)");
    NewBomLog("ADDIN", "       IMPORTANT: SetAccessNext creates sibling relationship");
    SetAccessNext(pNumberMgtMenu, pAssemblyTreeMenu);
    SetAccessNext(pAssemblyTreeMenu, pToolsMenu);
    NewBomLog("ADDIN", "       Done: Menu order is now '编号管理' -> '获取装配树' -> '工具'");
    
    NewBomLog("ADDIN", "  [5c] SetTitle sets Chinese display text");
    {
        static const CATUC2Bytes titleChars[] = {0x5DE5, 0x5177, 0};  // "Tools" + null
        CATUnicodeString title;
        NewBomLog("ADDIN", "       Calling BuildFromUCChar with UCS-2 array");
        title.BuildFromUCChar(titleChars, 2);
        NewBomLog("ADDIN", "       String length = %d characters (should be 2)", title.GetLengthInChar());
        pToolsMenu->SetTitle(title);
        NewBomLog("ADDIN", "       SetTitle() called - menu should display 'Tools'");
    }
    NewBomLog("ADDIN", "STEP 5 COMPLETE - Submenu 'Tools' created");
    NewBomLog("ADDIN", "");

    // =========================================================================
    // STEP 6: Create Command Item "AboutPlugin" (About Plugin)
    // =========================================================================
    NewBomLog("ADDIN", "STEP 6: Creating command item 'AboutPlugin' (About Plugin)...");
    
    NewBomLog("ADDIN", "  [6a] NewAccess creates a starter with ID 'NewBomAboutStr'");
    NewAccess(CATCmdStarter, pAboutStarter, NewBomAboutStr);
    NewBomLog("ADDIN", "       Result: pAboutStarter = %p", pAboutStarter);
    
    NewBomLog("ADDIN", "  [6b] SetAccessCommand links to 'NewBomAboutCmd' header");
    NewBomLog("ADDIN", "       Call: SetAccessCommand(pAboutStarter, 'NewBomAboutCmd')");
    SetAccessCommand(pAboutStarter, "NewBomAboutCmd");
    NewBomLog("ADDIN", "       Done: Clicking will run NewBomAboutCmd (shows dialog)");
    
    NewBomLog("ADDIN", "  [6c] SetAccessChild puts this item inside 'Tools' submenu");
    NewBomLog("ADDIN", "       Call: SetAccessChild(pToolsMenu, pAboutStarter)");
    SetAccessChild(pToolsMenu, pAboutStarter);
    NewBomLog("ADDIN", "       Done: 'AboutPlugin' is now inside 'Tools'");
    
    NewBomLog("ADDIN", "STEP 6 COMPLETE - Command item 'AboutPlugin' created");
    NewBomLog("ADDIN", "");

    // =========================================================================
    // STEP 7: Merge Menu with CATIA's Main Menu Bar
    // =========================================================================
    NewBomLog("ADDIN", "STEP 7: Merging our menu with CATIA's main menu bar...");
    NewBomLog("ADDIN", "  Call: SetAddinMenu(pAddinRoot, pMenuBar)");
    NewBomLog("ADDIN", "  This is the final step that makes our menu visible in CATIA");
    NewBomLog("ADDIN", "  - pAddinRoot: The root container returned to CATIA");
    NewBomLog("ADDIN", "  - pMenuBar: Our menu bar to be merged");
    
    SetAddinMenu(pAddinRoot, pMenuBar);
    NewBomLog("ADDIN", "  Done: Menu structure is now attached to CATIA");
    
    NewBomLog("ADDIN", "STEP 7 COMPLETE");
    NewBomLog("ADDIN", "");

    // =========================================================================
    // FINAL SUMMARY
    // =========================================================================
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "CreateToolbars() COMPLETE - Menu Structure Built Successfully");
    NewBomLog("ADDIN", "===========================================");
    NewBomLog("ADDIN", "Summary of created objects:");
    NewBomLog("ADDIN", "  pAddinRoot        = %p (returned to CATIA)", pAddinRoot);
    NewBomLog("ADDIN", "  pMenuBar          = %p (menu bar container)", pMenuBar);
    NewBomLog("ADDIN", "  pTopMenu          = %p (NewBOM top menu)", pTopMenu);
    NewBomLog("ADDIN", "  pNumberMgtMenu    = %p (NumberMgt submenu)", pNumberMgtMenu);
    NewBomLog("ADDIN", "  pAssemblyTreeMenu = %p (AssemblyTree submenu)", pAssemblyTreeMenu);
    NewBomLog("ADDIN", "  pToolsMenu        = %p (Tools submenu)", pToolsMenu);
    NewBomLog("ADDIN", "");
    NewBomLog("ADDIN", "Menu tree ready:");
    NewBomLog("ADDIN", "  NewBOM -> NumberMgt -> GetNewNumber");
    NewBomLog("ADDIN", "         -> AssemblyTree -> GetAssemblyTree");
    NewBomLog("ADDIN", "         -> Tools -> AboutPlugin");
    NewBomLog("ADDIN", "===========================================");
    
    return pAddinRoot;
}