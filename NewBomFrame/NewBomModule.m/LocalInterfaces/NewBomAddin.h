// =============================================================================
// NewBomAddin.h
// CATIA CAA Workbench Addin Header File
// =============================================================================
// 
// What is an Addin?
// An Addin (Extension) allows us to add custom functionality to CATIA's
// existing workbenches without creating a completely new workbench.
//
// This header defines NewBomAddin class that implements CATIAfrGeneralWksAddin
// interface. Through this interface, our plugin can:
//   - Add custom menus to CATIA's main menu bar
//   - Be available in any workbench (Part Design, Assembly Design, etc.)
//   - Not require switching to a specific workbench to access features
//
// Key Methods:
//   CreateCommands()  - Create command headers, link menu items to command classes
//   CreateToolbars()  - Build menu structure, define menu hierarchy
//
// =============================================================================

#ifndef __NewBomAddin_h__
#define __NewBomAddin_h__

#include "CATBaseUnknown.h"
#include "CATIAfrGeneralWksAddin.h"

class CATCmdContainer;

/**
 * @class NewBomAddin
 * @brief CATIA Workbench Addin Implementation
 * 
 * This class implements CATIAfrGeneralWksAddin interface to:
 *   1. Register command headers (CreateCommands)
 *   2. Create menu structure (CreateToolbars)
 * 
 * When CATIA starts:
 *   1. Read dictionary file (.dico), find our extension
 *   2. Load NewBomModule.dll
 *   3. Create NewBomAddin instance
 *   4. Call CreateCommands() to register commands
 *   5. Call CreateToolbars() to build menus
 * 
 * CATDeclareClass macro:
 *   - Adds metadata to the class
 *   - Makes class recognizable by CATIA component system
 *   - Supports runtime type checking
 */
class NewBomAddin : public CATBaseUnknown
{
    CATDeclareClass;

public:
    NewBomAddin();
    virtual ~NewBomAddin();

    // CATIAfrGeneralWksAddin interface methods
    
    /**
     * Create command headers
     * 
     * What is a Command Header?
     * A Command Header is a middle layer that:
     *   - Holds command's unique identifier (HeaderID)
     *   - Knows which DLL to load (LoadName)
     *   - Knows which command class to create (ClassName)
     *   - Can pass arguments to command (Argument)
     * 
     * Why use Command Headers?
     *   - Lazy loading: Command class loaded only when menu clicked
     *   - Unified management: All commands registered through headers
     *   - Parameter passing: Can pass data to commands
     */
    virtual void CreateCommands();
    
    /**
     * Create menu structure
     * 
     * Returns: CATCmdContainer pointer (root of menu tree)
     * 
     * Menu Structure:
     *   NewBOM (Top Menu)
     *     |- NumberMgt (Submenu)
     *     |    |- GetNewNumber (Command Item)
     *     |- Tools (Submenu)
     *          |- AboutPlugin (Command Item)
     * 
     * Key Concepts:
     * - CATCmdContainer: Container holding other elements (menus, toolbars)
     * - CATCmdStarter: Clickable menu item that triggers a command
     * - SetAccessChild: Parent-child relationship
     * - SetAccessNext: Sibling relationship (same level)
     */
    virtual CATCmdContainer* CreateToolbars();
};

#endif // __NewBomAddin_h__