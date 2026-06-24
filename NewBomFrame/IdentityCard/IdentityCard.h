// =============================================================================
// IdentityCard.h
// CATIA CAA Framework Identity Card - Define Framework Dependencies
// =============================================================================
//
// What is a Framework?
// In CATIA CAA, framework is the basic unit for organizing code. Each framework:
//   - Contains one or more modules
//   - Defines dependencies on other frameworks
//   - Has unique name (e.g. NewBomFrame)
//
// What is a Module?
// Module is the actual code unit within a framework:
//   - Contains source files (.cpp, .h)
//   - Compiles to DLL or library
//   - Implements specific functionality
//
// This file's purpose:
// IdentityCard.h defines what CATIA frameworks NewBomFrame depends on.
// These dependencies tell RADE build system:
//   - Which libraries to link
//   - Which headers to include
//   - Which interfaces to import
//
// AddPrereqComponent Command:
// AddPrereqComponent("FrameworkName", Public/Private);
//
// Parameters:
//   - "FrameworkName": CATIA system framework name
//   - Public: Public dependency (headers visible to other frameworks)
//   - Private: Private dependency (internal use only)
//
// Common CATIA Frameworks:
//   - ApplicationFrame: CATIA application frame (main window, commands, etc.)
//   - Dialog: Dialog system (various controls)
//   - ObjectModelerBase: Object model base (CATBaseUnknown, etc.)
//   - System: System base (strings, collections, etc.)
//   - SpecialAPI: Special API (logging, error handling, etc.)
//
// =============================================================================

// ApplicationFrame - CATIA Application Frame
// Contains:
//   - CATApplicationFrame: Main application frame class
//   - CATCommand: Command base class
//   - CATCommandHeader: Command header
//   - CATIAfrGeneralWksAddin: General workshop addin interface
//   - CATCmdContainer, CATCmdStarter: Menu container and starter
//
// We need it for:
//   - Create menus (NewAccess, SetAccessChild, etc.)
//   - Create commands (CATCommand subclass)
//   - Implement workshop addin (CATIAfrGeneralWksAddin)
AddPrereqComponent("ApplicationFrame", Public);

// Dialog - CATIA Dialog System
// Contains:
//   - CATDlgDialog: Dialog base class
//   - CATDlgLabel: Label control
//   - CATDlgEditor: Editor control
//   - CATDlgCombo: Combo box control
//   - CATDlgPushButton: Push button control
//   - CATDlgNotify: Information dialog
//   - CATDlgGridConstraints: Grid layout constraints
//
// We need it for:
//   - Create get number dialog (NewBomDlg)
//   - Create about dialog (CATDlgNotify)
AddPrereqComponent("Dialog", Public);

// ObjectModelerBase - CATIA Object Model Base
// Contains:
//   - CATBaseUnknown: Base class for all CATIA classes
//   - CATImplementClass, CATDeclareClass: Class registration macros
//   - CATCreateClass, CATCreateExternalObject: Class factory macros
//   - Various interfaces (e.g. CATIWorkbenchAddin)
//
// We need it for:
//   - All CATIA class base (CATBaseUnknown)
//   - Class registration system (CATImplementClass, etc.)
AddPrereqComponent("ObjectModelerBase", Public);

// SpecialAPI - CATIA Special API
// Contains:
//   - CATError: Error handling
//   - CATMsg: Message system
//   - Other special functionality
//
// We need it for:
//   - Basic API support
AddPrereqComponent("SpecialAPI", Public);

// System - CATIA System Base
// Contains:
//   - CATUnicodeString: Unicode string class
//   - CATString: String class
//   - CATListOfCATString: String list
//   - Other system utilities
//
// We need it for:
//   - Unicode string handling (BuildFromUCChar)
//   - Menu title setting
AddPrereqComponent("System", Public);

// =============================================================================
// Dependency Graph
// =============================================================================
//
// NewBomFrame (Our Framework)
//     |- ApplicationFrame (App frame - menus, commands)
//     |- Dialog (Dialog system - controls)
//     |- ObjectModelerBase (Object model - base classes)
//     |- SpecialAPI (Special API)
//     |- System (System base - strings)
//
// =============================================================================